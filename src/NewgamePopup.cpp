//============================================================================================
// NewGamePopup.cpp : implementation file
//============================================================================================
// Link to: "NewGamePopup.h"
//============================================================================================
#include "stdafx.h"
#include "glstart.h"
#include "SecurityManager.h"
#include "NewGamePopup.h"  //Fenster zum Wahl der Gegner und der Spielstärke
#include "cd_prot.h"
#include "fillfile.h"
#include "atnet.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char FileId[] = "NewG";
const char TOKEN_NEWGAME[] = "NewG";

SLONG NewgameWantsToLoad = FALSE;
SLONG NewgameToOptions = FALSE;
SLONG gNetworkSavegameLoading = -1;    //Komm-Variable, über die der Options-Screen mitteilt, welcher Spielstand für's Netzwerk geladen werden soll

extern CJumpingVar<ULONG>   gPhysicalCdRomBitlist;
extern CJumpingVar<CString> gCDPath;

#ifdef _DEBUG
char VersionString[] = "VERSION 1.5 DEBUG"; // (pre-release; build 100)";
#else
char VersionString[] = "VERSION 1.5 PRE-RELEASE"; // (pre-release; build 100)";
#endif

extern SLONG gLoadGameNumber;
extern FILE* CreditsSmackerFileHandle;
extern BOOL gSpawnOnly;

// {7EAFE365-9362-11d2-BA6A-080000278763}
//static const GUID GuidAT = { 0x7eafe365, 0x9362, 0x11d2, { 0xba, 0x6a, 0x8, 0x0, 0x0, 0x27, 0x87, 0x63 } };
SBNetwork gNetwork(false);

SLONG bNetworkUnderway = false;

static bool bNewGamePopupIsOpen = false;

const int MissionValues[] = {
	DIFF_FREEGAME,
	DIFF_FIRST,
	DIFF_EASY,
	DIFF_NORMAL,
	DIFF_HARD,
	DIFF_FINAL,
};
const CString MissionTypes[] = {
	"Free Game",
	"2500 Passengers",
	"Earn 2,500,000 DOLLARS",
	"6 routes",
	"Good image",
	"Build the rocket WIP",
};

CString gHostIP = ".";

SBStr NewGamePopup::NetworkSession;

extern SLONG nPlayerOptionsOpen[4];     //Fummelt gerade wer an den Options?
extern SLONG nPlayerAppsDisabled[4];    //Ist ein anderer Spieler gerade in einer anderen Anwendung?
extern SLONG nPlayerWaiting[4];         //Hinkt jemand hinterher?

#define READYTIME_JOIN   5000   //ms, till server may start after event
#define READYTIME_CLICK  2000   //ms, till server may start after event

void DumpAASeedSum(long CallerId);

//////////////////////////////////////////////////////////////////////////////////////////////
// NewGamePopup
//////////////////////////////////////////////////////////////////////////////////////////////
NewGamePopup::NewGamePopup(BOOL bHandy, SLONG PlayerNum) : CStdRaum(bHandy, PlayerNum, "startup.gli", GFX_HAUPTMNU)
{
	Konstruktor(bHandy, PlayerNum);

	Sim.LoadHighscores();
}

void NewGamePopup::Konstruktor(BOOL bHandy, SLONG PlayerNum)
{
	SLONG c;

	bNewGamePopupIsOpen = true;


	SessionMissionID = 0;

#ifdef CD_PROTECTION
	union
	{
		void (NewGamePopup::* p_member)(BOOL bHandy, SLONG PlayerNum);
		void* func;
	}
	MyUnion;

	MyUnion.p_member = &NewGamePopup::Konstruktor;

	// Create a new security manager
	SecurityManager* manager = new SecurityManager((char*)(LPCTSTR)FullFilename("plain_r.mcf", MiscPath), 0, 11771);

	// Decrypt the GetCode Function
	manager->DecryptFunction(MyUnion.func);

	delete manager;

	// Put a marker
	PUTSTARTMARK;
#endif

	gBroadcastBm.Destroy();

	pNetworkSessions = NULL;
	pNetworkConnections = NULL;
	pNetworkPlayers = NULL;

	Sim.bAllowCheating = FALSE;

	PlayerReadyAt = 0;

	bad = FALSE;

#ifdef BETA_LICENSE_NECESSARY
	if (!CheckComputerLicense())
	{
		bad = TRUE;
		RefreshKlackerField();
		MenuStart(MENU_REQUEST, MENU_REQUEST_BETATEST);
	}
#endif

	if (Sim.Options.OptionAirport == -1)
	{
		if (gLanguage == LANGUAGE_N) Sim.Options.OptionAirport = Cities.GetIdFromName("Berlijn");
		else if (gLanguage == LANGUAGE_F) Sim.Options.OptionAirport = Cities.GetIdFromName("Paris");
		else if (gLanguage == LANGUAGE_I) Sim.Options.OptionAirport = Cities.GetIdFromName("Roma");
		else if (gLanguage == LANGUAGE_O) Sim.Options.OptionAirport = Cities.GetIdFromName("Madrid");
		else if (gLanguage == LANGUAGE_S) Sim.Options.OptionAirport = Cities.GetIdFromName("Madrid");
		else if (gLanguage == LANGUAGE_E) Sim.Options.OptionAirport = Cities.GetIdFromName("London");
		else Sim.Options.OptionAirport = Cities.GetIdFromName("Berlin");
	}

	Sim.HomeAirportId = Sim.Options.OptionAirport;

	PageNum = PAGE_TYPE::MAIN_MENU;
	CursorX = CursorY = -1;
	BlinkState = 0;

	if (NewgameWantsToLoad == 2)
	{
		PageNum = PAGE_TYPE::MULTIPLAYER_CREATE_SESSION;
		CursorX = 0;
		Sim.bNetwork = false;

		if (gNetworkSavegameLoading != -1)
		{
			SBNetworkCreation cr;

			Sim.bNetwork = true;
			NetworkSession = Sim.GetSavegameSessionName(gNetworkSavegameLoading);
			Sim.bNetwork = false;

			cr.sessionName = NetworkSession;
			cr.maxPlayers = 4;
			cr.flags = SBCreationFlags::SBNETWORK_CREATE_TRY_NAT;

			if (gNetwork.CreateSession(SBStr("somesession"), &cr))
			{
				Sim.bIsHost = TRUE;
				Sim.SessionName = NetworkSession;
				Sim.UniqueGameId = ((timeGetTime() ^ DWORD(rand() % 30000) ^ gMousePosition.x ^ gMousePosition.y) & 0x7fffffff);
				bThisIsSessionMaster = true;
				PlayerReadyAt = 0;

				hprintf("This computer is host.");

				BOOL bOld = Sim.bNetwork;
				Sim.bNetwork = true;
				Sim.ReadSavegameOwners(gNetworkSavegameLoading);
				SLONG localPlayer = Sim.GetSavegameLocalPlayer(gNetworkSavegameLoading);
				Sim.bNetwork = bOld;

				for (SLONG d = 0; d < 4; d++)
				{
					Sim.Players.Players[d].NetworkID = 0;
					if (Sim.Players.Players[d].Owner == 2) Sim.Players.Players[d].Owner = 3;
				}

				Sim.Players.Players[localPlayer].NetworkID = gNetwork.GetLocalPlayerID();

				PageNum = PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER;
				RefreshKlackerField();
			}
		}
	}

	ClickFx.ReInit("change.raw");

	Sim.Tutorial = 0;

#ifdef DEMO
	if ((Sim.Difficulty > DIFF_FIRST && Sim.Difficulty <= DIFF_FINAL) || Sim.Difficulty == DIFF_FREEGAME)
		Sim.Difficulty = min(DIFF_FIRST, (UBYTE)Sim.MaxDifficulty);

	if (Sim.Difficulty > DIFF_ADDON02 && Sim.Difficulty <= DIFF_ADDON10)
		Sim.Difficulty = min(DIFF_ADDON02, (UBYTE)Sim.MaxDifficulty2);
#endif

#ifdef DEMO
	Sim.Options.OptionLastPlayer = 1;
#endif

	Limit(SLONG(0), Sim.Options.OptionLastPlayer, SLONG(3));

	for (c = 0; c < 4; c++)
		UnselectedNetworkIDs[c] = NULL;

	if (NewgameWantsToLoad != 2)
	{
		for (c = 0; c < 4; c++)
		{
			Sim.Players.Players[c].Logo = (UBYTE)c;
			Sim.Players.Players[c].Owner = 1;
		}

		Sim.Players.Players[Sim.Options.OptionLastPlayer].Owner = 0;
	}

	for (c = 0; c < Sim.Players.AnzPlayers; c++)
	{
		Sim.Players.Players[c].Name = Sim.Options.OptionPlayerNames[c];
		Sim.Players.Players[c].Airline = Sim.Options.OptionAirlineNames[c];
		Sim.Players.Players[c].Abk = Sim.Options.OptionAirlineAbk[c];

		if (Sim.Players.Players[c].Name.GetLength() == 0)
			Sim.Players.Players[c].Name = bprintf("%-21s", StandardTexte.GetS(TOKEN_NEWGAME, 1000 + c * 10));

		if (Sim.Players.Players[c].Airline.GetLength() == 0)
			Sim.Players.Players[c].Airline = bprintf("%-21s", StandardTexte.GetS(TOKEN_NEWGAME, 1001 + c * 10));

		if (Sim.Players.Players[c].Abk.GetLength() == 0)
			Sim.Players.Players[c].Abk = bprintf("%-2s", StandardTexte.GetS(TOKEN_NEWGAME, 1002 + c * 10));

		Sim.Players.Players[c].Name.ToUpper();
		Sim.Players.Players[c].Airline.ToUpper();
		Sim.Players.Players[c].Abk.ToUpper();

		for (int d = 0; d < Sim.Players.Players[c].Name.GetLength(); d++)
		{
			if (Sim.Players.Players[c].Name[d] == 'ä') Sim.Players.Players[c].Name.SetAt(d, 'Ä');
			if (Sim.Players.Players[c].Name[d] == 'ö') Sim.Players.Players[c].Name.SetAt(d, 'Ö');
			if (Sim.Players.Players[c].Name[d] == 'ü') Sim.Players.Players[c].Name.SetAt(d, 'Ü');
		}
	}

	//Nur auf die Schnelle?
	if (bQuick || gLoadGameNumber > -1)
	{
		for (SLONG c = 0; c < Sim.Players.Players.AnzEntries(); c++)
			Sim.Players.Players[c].IsOut = 0;

		Sim.Gamestate = UBYTE((Sim.Gamestate & (~GAMESTATE_WORKING)) | GAMESTATE_DONE);

		Routen.ReInit("Routen.csv", true);
		Sim.ChooseStartup(bQuick);

		Airport.LoadAirport(1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
		Airport.UpdateStaticDoorImage();
		Airport.CreateGateMapper();
		return;
	}

	PlayerBms.ReSize(pRoomLib, "PLAYER0", 8);
	HakenBm.ReSize(pRoomLib, "HAKEN");

	//Klacker-Tafel initialisieren:
	CheckNames();
	RefreshKlackerField();

	VersionFont.Load(lpDD, (char*)(LPCTSTR)FullFilename("stat_1.mcf", MiscPath));

	SDL_ShowWindow(FrameWnd->m_hWnd);
	SDL_GetWindowSurface(FrameWnd->m_hWnd);
	SDL_UpdateWindowSurface(FrameWnd->m_hWnd);
	
	SetMouseLook(CURSOR_NORMAL, 0, ROOM_TITLE, 0);

	//Create a timer to 'klacker'
	TimerId = SDL_AddTimer(50, TimerFunc, this);
	if (!TimerId) TimerFailure = 1;
	else TimerFailure = 0;

	DumpAASeedSum(2000);
}

//--------------------------------------------------------------------------------------------
//NewGamePopup::~NewGamePopup()
//--------------------------------------------------------------------------------------------
NewGamePopup::~NewGamePopup()
{
	SLONG c;
	if (TimerId) SDL_RemoveTimer(TimerId);

	bNewGamePopupIsOpen = false;

	gBroadcastBm.Destroy();
	gMouseLButton = 0;

	if (Sim.Difficulty >= DIFF_ATFS)
		Sim.Options.OptionLastMission3 = Sim.Difficulty;
	else if (Sim.Difficulty >= DIFF_ADDON)
		Sim.Options.OptionLastMission2 = Sim.Difficulty;
	else
		Sim.Options.OptionLastMission = Sim.Difficulty;

	if (Sim.Difficulty == DIFF_TUTORIAL && !bLeaveGameLoop)
	{
		pGfxMain->LoadLib((char*)(LPCTSTR)FullFilename("glglow.gli", GliPath), &pGlowEffectLib, L_LOCMEM);
		gGlowBms.ReSize(pGlowEffectLib, "TUTMARK1", 5);
	}

	if (!bLeaveGameLoop && Sim.Players.Players.AnzEntries() > 0)
	{
		for (c = 0; c < Sim.Players.AnzPlayers; c++)
		{
			Sim.Options.OptionPlayerNames[c] = Sim.Players.Players[c].Name;
			Sim.Options.OptionAirlineNames[c] = Sim.Players.Players[c].Airline;
			Sim.Options.OptionAirlineAbk[c] = Sim.Players.Players[c].Abk;
		}

		for (c = 0; c < Sim.Players.AnzPlayers; c++)
		{
			RecapizalizeString((UBYTE*)(LPCTSTR)Sim.Players.Players[c].Name);
			RecapizalizeString((UBYTE*)(LPCTSTR)Sim.Players.Players[c].Airline);
		}

		for (c = 0; c < Sim.Players.AnzPlayers; c++)
		{
			Sim.Players.Players[c].AirlineX = Sim.Players.Players[c].Airline;
			while (Sim.Players.Players[c].AirlineX.GetLength() > 0 && Sim.Players.Players[c].AirlineX[Sim.Players.Players[c].AirlineX.GetLength() - 1] == 32)
			{
				Sim.Players.Players[c].AirlineX = Sim.Players.Players[c].AirlineX.Left(Sim.Players.Players[c].AirlineX.GetLength() - 1);
			}

			Sim.Players.Players[c].NameX = Sim.Players.Players[c].Name;
			while (Sim.Players.Players[c].NameX.GetLength() > 0 && Sim.Players.Players[c].NameX[Sim.Players.Players[c].NameX.GetLength() - 1] == 32)
			{
				Sim.Players.Players[c].NameX = Sim.Players.Players[c].NameX.Left(Sim.Players.Players[c].NameX.GetLength() - 1);
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
//Aktualisiert die Text-Daten im Klacker-Feld:
//--------------------------------------------------------------------------------------------
void NewGamePopup::RefreshKlackerField(void)
{
	SLONG c;

	KlackerTafel.Clear();

	if (PageNum != PAGE_TYPE::MAIN_MENU && PageNum != PAGE_TYPE::MP_LOADING)
	{
		KlackerTafel.PrintAt(0, 1, "========================");
	}

	if (PageNum == PAGE_TYPE::MAIN_MENU)
	{
		Sim.bAllowCheating = TRUE;

		KlackerTafel.PrintAt(0, 0, StandardTexte.GetS(TOKEN_NEWGAME, 500));
		KlackerTafel.PrintAt(0, 1, "========================");

		if (bFirstClass)
		{
			KlackerTafel.PrintAt(1, 2, StandardTexte.GetS(TOKEN_NEWGAME, 502)); //Neues Spiel
			KlackerTafel.PrintAt(1, 3, StandardTexte.GetS(TOKEN_NEWGAME, 512)); //Nueue Missionen
#ifndef DEMO
			KlackerTafel.PrintAt(1, 4, SBStr("# ") + StandardTexte.GetS(TOKEN_NEWGAME, 501)); //Freies Spiel
			KlackerTafel.PrintAt(1, 5, StandardTexte.GetS(TOKEN_NEWGAME, 513)); //Netzwerkspiel
#endif
			KlackerTafel.PrintAt(1, 6, StandardTexte.GetS(TOKEN_NEWGAME, 506)); //Spiel laden

			KlackerTafel.PrintAt(1, 8, StandardTexte.GetS(TOKEN_NEWGAME, 504)); //Startflughafen
			KlackerTafel.PrintAt(1, 9, StandardTexte.GetS(TOKEN_NEWGAME, 507)); //Optionen

			KlackerTafel.PrintAt(1, 10, StandardTexte.GetS(TOKEN_NEWGAME, 505)); //Intro
			KlackerTafel.PrintAt(1, 11, StandardTexte.GetS(TOKEN_NEWGAME, 508)); //Credits

			if (gSpawnOnly)
			{
				KlackerTafel.LineDisabled[2] = true;
				KlackerTafel.LineDisabled[3] = true;
				KlackerTafel.LineDisabled[4] = true;
				KlackerTafel.LineDisabled[6] = true;
				KlackerTafel.LineDisabled[10] = true;
			}
		}
		else
		{
#ifndef DEMO
			KlackerTafel.PrintAt(1, 2, SBStr("# ") + StandardTexte.GetS(TOKEN_NEWGAME, 501)); //Freies Spiel
#endif
			KlackerTafel.PrintAt(1, 3, StandardTexte.GetS(TOKEN_NEWGAME, 515)); //Kampagnen
#ifndef DEMO
			KlackerTafel.PrintAt(1, 4, StandardTexte.GetS(TOKEN_NEWGAME, 514)); //Netzwerkspiel
#endif
			KlackerTafel.PrintAt(1, 5, StandardTexte.GetS(TOKEN_NEWGAME, 506)); //Spiel laden

			KlackerTafel.PrintAt(1, 7, StandardTexte.GetS(TOKEN_NEWGAME, 504)); //Startflughafen
			KlackerTafel.PrintAt(1, 8, StandardTexte.GetS(TOKEN_NEWGAME, 507)); //Optionen

			KlackerTafel.PrintAt(1, 10, StandardTexte.GetS(TOKEN_NEWGAME, 505)); //Intro
			KlackerTafel.PrintAt(1, 11, StandardTexte.GetS(TOKEN_NEWGAME, 508)); //Credits
			KlackerTafel.PrintAt(1, 12, StandardTexte.GetS(TOKEN_NEWGAME, 509)); //Highscores

			if (gSpawnOnly)
			{
				KlackerTafel.LineDisabled[2] = true;
				KlackerTafel.LineDisabled[3] = true;
				KlackerTafel.LineDisabled[5] = true;
				KlackerTafel.LineDisabled[10] = true;
			}
		}

		KlackerTafel.PrintAt(1, bFirstClass ? 13 : 14, StandardTexte.GetS(TOKEN_NEWGAME, 510)); //Beenden
	}
	else if (PageNum == PAGE_TYPE::MISSION_SELECT) //Mission wählen
	{
		CString tmp;

		KlackerTafel.PrintAt(0, 15, StandardTexte.GetS(TOKEN_NEWGAME, 4001)); //Zurück
		KlackerTafel.PrintAt(24 - strlen(StandardTexte.GetS(TOKEN_NEWGAME, 4002)), 15, StandardTexte.GetS(TOKEN_NEWGAME, 4002)); //Weiter

		KlackerTafel.PrintAt(0, 0, StandardTexte.GetS(TOKEN_NEWGAME, 1999)); //Missionen:

		for (c = 0; c <= DIFF_FINAL; c++)
		{
			if (c == Sim.Difficulty) tmp = "==>";
			else tmp = "   ";

			tmp += StandardTexte.GetS(TOKEN_NEWGAME, 2000 + c * 100);
			KlackerTafel.PrintAt(1, 2 + c, tmp);
		}

		/*if (gLanguage==LANGUAGE_D)
		{
		   if (DIFF_FREEGAME==Sim.Difficulty) tmp = "==>";
										 else tmp = "   ";

		   tmp+=StandardTexte.GetS (TOKEN_NEWGAME, 501);
		   KlackerTafel.PrintAt (1, 9, tmp);
		}*/
	}
	else if (PageNum == PAGE_TYPE::HIGHSCORES) //Highscores
	{
		CString tmp;

		KlackerTafel.PrintAt(0, 15, StandardTexte.GetS(TOKEN_NEWGAME, 4001)); //Zurück

		KlackerTafel.PrintAt(0, 0, StandardTexte.GetS(TOKEN_NEWGAME, 1100)); //Missionen:

		for (c = 1; c <= 6; c++)
		{
			tmp = bprintf("%li.", c) + CString("                         ");
			KlackerTafel.PrintAt(0, c * 2, tmp);
			KlackerTafel.PrintAt(3, c * 2, Sim.Highscores[c - 1].Name + CString("                         "));
			tmp = Insert1000erDots64(Sim.Highscores[c - 1].Score) + "                         ";
			KlackerTafel.PrintAt(3, c * 2 + 1, tmp);
		}
	}
	else if (PageNum == PAGE_TYPE::ADDON_MISSION_SELECT) //Add-On Mission wählen
	{
		CString tmp;

		KlackerTafel.PrintAt(0, 15, StandardTexte.GetS(TOKEN_NEWGAME, 4001)); //Zurück
		KlackerTafel.PrintAt(24 - strlen(StandardTexte.GetS(TOKEN_NEWGAME, 4002)), 15, StandardTexte.GetS(TOKEN_NEWGAME, 4002)); //Weiter

		KlackerTafel.PrintAt(0, 0, StandardTexte.GetS(TOKEN_NEWGAME, 1999)); //Missionen:

		for (c = 11; c <= 21; c++)
		{
			if (c == Sim.Difficulty) tmp = "==>";
			else tmp = "   ";

			tmp += StandardTexte.GetS(TOKEN_NEWGAME, 3000 + c);
			KlackerTafel.PrintAt(1, 2 + (c - 11), tmp);
		}
	}
	else if (PageNum == PAGE_TYPE::FLIGHT_SECURITY_MISSION_SELECT) //Flight Security Mission wählen
	{
		CString tmp;

		KlackerTafel.PrintAt(0, 15, StandardTexte.GetS(TOKEN_NEWGAME, 4001)); //Zurück
		KlackerTafel.PrintAt(24 - strlen(StandardTexte.GetS(TOKEN_NEWGAME, 4002)), 15, StandardTexte.GetS(TOKEN_NEWGAME, 4002)); //Weiter

		KlackerTafel.PrintAt(0, 0, StandardTexte.GetS(TOKEN_NEWGAME, 1999)); //Missionen:

		for (c = 41; c <= 50; c++)
		{
			if (c == Sim.Difficulty) tmp = "==>";
			else tmp = "   ";

			tmp += StandardTexte.GetS(TOKEN_NEWGAME, 3000 + c);
			KlackerTafel.PrintAt(1, 2 + (c - 41), tmp);
		}
	}
	else if (PageNum == PAGE_TYPE::CAMPAIGN_SELECT)
	{
		Sim.bAllowCheating = TRUE;

		if (!bad)
		{
			KlackerTafel.PrintAt(0, 0, StandardTexte.GetS(TOKEN_NEWGAME, 515) + 2);
			KlackerTafel.PrintAt(0, 1, "========================");

			KlackerTafel.PrintAt(1, 2, StandardTexte.GetS(TOKEN_NEWGAME, 502)); //Neues Spiel
			KlackerTafel.PrintAt(1, 3, StandardTexte.GetS(TOKEN_NEWGAME, 512)); //Mehr Missionen
			KlackerTafel.PrintAt(1, 4, StandardTexte.GetS(TOKEN_NEWGAME, 513)); //Noch Mehr Missionen
		}

		KlackerTafel.PrintAt(0, 6, StandardTexte.GetS(TOKEN_NEWGAME, 4001)); //Zurück
	}
	else if (PageNum == PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER || PageNum == PAGE_TYPE::SELECT_PLAYER_CAMPAIGN || PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER) //Spieler wählen
	{
		if (PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER && pNetworkPlayers) //Auf Mitspieler warten
		{
			for (SLONG d = 0; d < 4; d++)
				if (Sim.Players.Players[d].Owner != 3)
				{
					Sim.Players.Players[d].Owner = 1;
					for (c = 0; c < (SLONG)pNetworkPlayers->GetNumberOfElements(); c++)
					{
						if (c == 0) pNetworkPlayers->GetFirst();

						if (Sim.Players.Players[d].NetworkID == pNetworkPlayers->GetLastAccessed()->ID)
							if (pNetworkPlayers->GetLastAccessed()->ID == gNetwork.GetLocalPlayerID())
								Sim.Players.Players[d].Owner = 0;
							else
								Sim.Players.Players[d].Owner = 2;

						if (pNetworkPlayers->IsLast()) break;

						pNetworkPlayers->GetNext();
					}
				}
		}

		KlackerTafel.PrintAt(0, 15, StandardTexte.GetS(TOKEN_NEWGAME, 4001));  //Zurück
		if (NamesOK && (PageNum != PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER || (bThisIsSessionMaster && pNetworkPlayers && pNetworkPlayers->GetNumberOfElements() > 1)))
		{
			SLONG c = 4;

			if (gNetworkSavegameLoading != -1)
				for (c = 0; c < 4; c++)
					if (Sim.Players.Players[c].Owner == 3) break;

			if (PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER && PlayerReadyAt > timeGetTime()) c = -1;
			if (PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER && (UnselectedNetworkIDs[0] || UnselectedNetworkIDs[1] || UnselectedNetworkIDs[2] || UnselectedNetworkIDs[3]))
				c = -1;

			if (c == 4) KlackerTafel.PrintAt(24 - strlen(StandardTexte.GetS(TOKEN_NEWGAME, 4000)), 15, StandardTexte.GetS(TOKEN_NEWGAME, 4000)); //Start
		}

		KlackerTafel.PrintAt(0, 0, StandardTexte.GetS(TOKEN_NEWGAME, 999)); //Spieler wählen

		for (c = 0; c < 4; c++)
		{
			KlackerTafel.Soll[3 + (c * 2 + 2) * 24] = Sim.Players.Players[c].Logo;
			if (KlackerTafel.Haben[3 + (c * 2 + 2) * 24] < 0 || KlackerTafel.Haben[3 + (c * 2 + 2) * 24] >= LogoBms.AnzEntries())
				KlackerTafel.Haben[3 + (c * 2 + 2) * 24] = 0;

			KlackerTafel.PrintAt(6, c * 2 + 2, (LPCTSTR)Sim.Players.Players[c].Name);
			KlackerTafel.PrintAt(6, c * 2 + 3, (LPCTSTR)Sim.Players.Players[c].Airline);
			KlackerTafel.PrintAt(3, c * 2 + 3, (LPCTSTR)Sim.Players.Players[c].Abk);
		}
	}
	else if (PageNum == PAGE_TYPE::MULTIPLAYER_SELECT_NETWORK) //Netzwerk: Provider-Medium wählen
	{
		pNetworkConnections = gNetwork.GetConnectionList();

		KlackerTafel.PrintAt(0, 0, StandardTexte.GetS(TOKEN_NEWGAME, 700));
		KlackerTafel.PrintAt(0, 15, StandardTexte.GetS(TOKEN_NEWGAME, 4001)); //Zurück
		KlackerTafel.PrintAt(24 - strlen(StandardTexte.GetS(TOKEN_NEWGAME, 4002)), 15, StandardTexte.GetS(TOKEN_NEWGAME, 4002)); //Weiter

		//Die Medien ohne Modem:
		NetMediumCount = 0;
		for (c = 0; c < (SLONG)pNetworkConnections->GetNumberOfElements(); c++)
		{
			if (c == 0) pNetworkConnections->GetFirst();

			SBStr inter = pNetworkConnections->GetLastAccessed();
			//SLONG id = gNetwork.GetProviderID(inter.c_str());

			//if (id && id !=NET_MEDIUM_MODEM)
			//{
			NetMediumMapper[NetMediumCount++] = c;
			//}

			KlackerTafel.PrintAt(3, 2 + c, (LPCTSTR)inter.c_str());
			if (c == Selection) KlackerTafel.PrintAt(0, 2 + c, "==>");

			if (pNetworkConnections->IsLast()) break;
			pNetworkConnections->GetNext();
		}

		//if (Selection>=(SLONG)NetMediumCount) Selection=(SLONG)NetMediumCount-1;
		//for (c=0; c<(SLONG)NetMediumCount; c++)
		//{
		//   SLONG id = gNetwork.GetProviderID ((char*)(LPCTSTR)pNetworkConnections->Get(NetMediumMapper[c]+1));

		//   CString Buffer;
		//   if (id) Buffer=GetMediumName (id);

		//   for (SLONG d=0; d<Buffer.GetLength(); d++)
		//      Buffer.SetAt (d, GerToUpper(Buffer[(int)d]));

		//   KlackerTafel.PrintAt (3, 2+c, Buffer);
		//   if (c==Selection) KlackerTafel.PrintAt (0, 2+c, "==>");
		//}
	}
	else if (PageNum == PAGE_TYPE::MULTIPLAYER_SELECT_SESSION) //Netzwerk: Session auswählen / erzeugen
	{
		//if (gNetwork.IsEnumSessionFinished())
		//{
		//	pNetworkSessions = gNetwork.GetSessionListAsync();
		//	gNetwork.StartGetSessionListAsync();
		//}

		KlackerTafel.PrintAt(0, 0, StandardTexte.GetS(TOKEN_NEWGAME, 701));
		KlackerTafel.PrintAt(0, 15, StandardTexte.GetS(TOKEN_NEWGAME, 4001)); //Zurück

		SLONG NumberOfElements = pNetworkSessions ? pNetworkSessions->GetNumberOfElements() : 0;

		if (pNetworkSessions && NumberOfElements > 0)
			KlackerTafel.PrintAt(24 - strlen(StandardTexte.GetS(TOKEN_NEWGAME, 4002)), 15, StandardTexte.GetS(TOKEN_NEWGAME, 4002)); //Weiter

		if (!gSpawnOnly)
		{
			KlackerTafel.PrintAt(0, 10, StandardTexte.GetS(TOKEN_NEWGAME, 706)); //Session laden
			KlackerTafel.PrintAt(0, 11, StandardTexte.GetS(TOKEN_NEWGAME, 707)); //Session laden
			KlackerTafel.PrintAt(0, 13, StandardTexte.GetS(TOKEN_NEWGAME, 702)); //Neue Session
		}

		if (Selection < 0) Selection = 0;
		if (Selection >= (SLONG)NumberOfElements)
			Selection = NumberOfElements - 1;

		for (c = 0; c < (SLONG)NumberOfElements && c < 6; c++)
		{
			if (c == 0) pNetworkSessions->GetFirst();

			CString Buffer = *pNetworkSessions->GetLastAccessed();

			for (SLONG d = 0; d < Buffer.GetLength(); d++)
				Buffer.SetAt(d, GerToUpper(Buffer[(int)d]));

			KlackerTafel.PrintAt(3, 2 + c, Buffer);
			if (c == Selection) KlackerTafel.PrintAt(0, 2 + c, "==>");

			if (pNetworkSessions->IsLast()) break;

			pNetworkSessions->GetNext();
		}

		if (NumberOfElements == 0)
		{
			KlackerTafel.PrintAt(1, 2, StandardTexte.GetS(TOKEN_NEWGAME, 720));
		}
	}
	else if (PageNum == PAGE_TYPE::MULTIPLAYER_CREATE_SESSION) //Session erzeugen
	{
		KlackerTafel.PrintAt(0, 0, StandardTexte.GetS(TOKEN_NEWGAME, 702));
		KlackerTafel.PrintAt(0, 15, StandardTexte.GetS(TOKEN_NEWGAME, 4001)); //Zurück
		KlackerTafel.PrintAt(24 - strlen(StandardTexte.GetS(TOKEN_NEWGAME, 4002)), 15, StandardTexte.GetS(TOKEN_NEWGAME, 4002)); //Weiter

		KlackerTafel.PrintAt(0, 2, NetworkSession);

		KlackerTafel.PrintAt(0, 4, StandardTexte.GetS(TOKEN_NEWGAME, 710 + Sim.bAllowCheating));
		KlackerTafel.PrintAt(0, 6, "Mission:");//StandardTexte.GetS(TOKEN_NEWGAME, 710 + Sim.bAllowCheating));

		KlackerTafel.PrintAt(0, 7, MissionTypes[SessionMissionID].c_str());
	}
	else if (PageNum == PAGE_TYPE::SETTINGS_CHOOSE_AIRPORT) //Heimatflughafen wählen
	{
		KlackerTafel.PrintAt(0, 0, StandardTexte.GetS(TOKEN_NEWGAME, 600));    //Heimatflughafen wählen

		KlackerTafel.PrintAt(22, 15, StandardTexte.GetS(TOKEN_NEWGAME, 4003));  //OK

		for (SLONG c = PageSub; c < PageSub + 12; c++)
			if (Cities.IsInAlbum(c))
			{
				char Buffer[80];

				strcpy(Buffer, (LPCTSTR)Cities[c].Name);

				KlackerTafel.PrintAt(3, 2 + c - PageSub, Buffer);
				if (Cities.GetIdFromIndex(c) == (ULONG)Sim.HomeAirportId) KlackerTafel.PrintAt(0, 2 + c - PageSub, "==>");
			}

		if (PageSub > 0) KlackerTafel.PrintAt(10, 15, "<<");
		if (PageSub + 12 < SLONG(Cities.AnzEntries()) - 1) KlackerTafel.PrintAt(13, 15, ">>");
	}
	else if (PageNum == PAGE_TYPE::MULTIPLAYER_PRE_SESSION)
	{
		//Header:
		KlackerTafel.PrintAt(0, 0, StandardTexte.GetS(TOKEN_NEWGAME, 702));
		KlackerTafel.PrintAt(0, 15, StandardTexte.GetS(TOKEN_NEWGAME, 4001)); //Zurück
		
		KlackerTafel.PrintAt(0, 3, StandardTexte.GetS(TOKEN_NEWGAME, 706)); //Session laden
		KlackerTafel.PrintAt(0, 4, StandardTexte.GetS(TOKEN_NEWGAME, 707)); //Session laden
		KlackerTafel.PrintAt(0, 6, StandardTexte.GetS(TOKEN_NEWGAME, 702)); //Neue Session
	}
	else if (PageNum == PAGE_TYPE::MP_LOADING) //Loading...
	{
		KlackerTafel.PrintAt(12 - (strlen(StandardTexte.GetS(TOKEN_NEWGAME, 530)) - 3) / 2, 8, StandardTexte.GetS(TOKEN_NEWGAME, 530));  //Zurück
	}
}

//--------------------------------------------------------------------------------------------
//Überprüft ob die Namen von Spielern & Fluggesellschaften eindeutig sind:
//--------------------------------------------------------------------------------------------
void NewGamePopup::CheckNames(void)
{
	SLONG c, d;

	NamesOK = TRUE;

	//Jeden mit jedem vergleichen...
	for (c = 0; c < 4; c++)
		for (d = c + 1; d < 4; d++)
			if (Sim.Players.Players[c].Name == Sim.Players.Players[d].Name ||
				Sim.Players.Players[c].Airline == Sim.Players.Players[d].Airline ||
				Sim.Players.Players[c].Abk == Sim.Players.Players[d].Abk) NamesOK = FALSE;

	//Und es muß EXAKT einen Human-Player geben:
	for (c = d = 0; c < 4; c++)
		if (Sim.Players.Players[c].Owner == 0) d++;

	if (d != 1) NamesOK = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// NewGamePopup message handlers

//--------------------------------------------------------------------------------------------
//NewGamePopup::OnPaint
//--------------------------------------------------------------------------------------------
void NewGamePopup::OnPaint()
{
	static SLONG x, y, py; x++;

	SLONG Line = (gMousePosition.y - 63) / 22;
	SLONG Column = (gMousePosition.x - 128) / 16;
	XY    GridPos = XY(Column, Line);

	gPhysicalCdRomBitlist.Pump();

	if (TimerFailure)
	{
		KlackerTafel.Klack();  //Tafel notfalls asynchron aktualisieren
	}

	//Die Standard Paint-Sachen kann der Basisraum erledigen
	CStdRaum::OnPaint();

	//Ggf. Onscreen-Texte einbauen:
	CStdRaum::InitToolTips();

	if (bActive)
	{
		//Klacker-Felder:
		for (py = 63, y = 0; y < 16; y++, py += 22)
		{
			for (x = 0; x < 24; x++)
				if (y >= 13)
				{
					RoomBm.BlitFrom(KlackerTafel.KlackerBms[(long)KlackerTafel.Haben[x + y * 24]], x * 16 + 128, py);
				}
				else if (PageNum == PAGE_TYPE::MISSION_SELECT)
				{
					if (KlackerTafel.Haben[x + y * 24])
#ifndef DEMO
						if (y - 2 > Sim.MaxDifficulty && y != 9)
#else
						if (y - 2 > 1 || y - 2 > Sim.MaxDifficulty)
#endif
							RoomBm.BlitFrom(KlackerTafel.KlackerBms[(long)KlackerTafel.Haben[x + y * 24] + (73 + 8 + 3 + 3)], x * 16 + 128, py);
						else
							RoomBm.BlitFrom(KlackerTafel.KlackerBms[(long)KlackerTafel.Haben[x + y * 24]], x * 16 + 128, py);
				}
				else if (PageNum == PAGE_TYPE::ADDON_MISSION_SELECT)
				{
					if (KlackerTafel.Haben[x + y * 24])
#ifndef DEMO
						if (y - 2 > Sim.MaxDifficulty2 - 11 && Sim.MaxDifficulty2 != DIFF_ADDON10)
#else
						if (y - 2 > 1 || (y - 2 > Sim.MaxDifficulty2 - 11 && Sim.MaxDifficulty2 != DIFF_ADDON10))
#endif
							RoomBm.BlitFrom(KlackerTafel.KlackerBms[(long)KlackerTafel.Haben[x + y * 24] + (73 + 8 + 3 + 3)], x * 16 + 128, py);
						else
							RoomBm.BlitFrom(KlackerTafel.KlackerBms[(long)KlackerTafel.Haben[x + y * 24]], x * 16 + 128, py);
				}
				else if (PageNum == PAGE_TYPE::FLIGHT_SECURITY_MISSION_SELECT)
				{
					if (KlackerTafel.Haben[x + y * 24])
#ifndef DEMO
						if (y - 2 > Sim.MaxDifficulty3 - 41 && Sim.MaxDifficulty3 != DIFF_ATFS10)
#else
						if (y - 2 > 1 || (y - 2 > Sim.MaxDifficulty3 - 41 && Sim.MaxDifficulty3 != DIFF_ATFS10))
#endif
							RoomBm.BlitFrom(KlackerTafel.KlackerBms[(long)KlackerTafel.Haben[x + y * 24] + (73 + 8 + 3 + 3)], x * 16 + 128, py);
						else
							RoomBm.BlitFrom(KlackerTafel.KlackerBms[(long)KlackerTafel.Haben[x + y * 24]], x * 16 + 128, py);
				}
				else if (PageNum != PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER && PageNum != PAGE_TYPE::SELECT_PLAYER_CAMPAIGN && PageNum != PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER)
				{
					if (KlackerTafel.Haben[x + y * 24] > 0)
						if (KlackerTafel.LineDisabled[y])
							RoomBm.BlitFrom(KlackerTafel.KlackerBms[(long)KlackerTafel.Haben[x + y * 24] + (73 + 8 + 3 + 3)], x * 16 + 128, py);
						else
							RoomBm.BlitFrom(KlackerTafel.KlackerBms[(long)KlackerTafel.Haben[x + y * 24]], x * 16 + 128, py);
				}
				else if (x > 5 || y < 2 || ((y - 2) % 2) != 0)
				{
					if (KlackerTafel.Haben[x + y * 24])
						if (y < 2 || (y >= 2 && (y - 2) / 2 <= 3 && !Sim.Players.Players[(y - 2) / 2].Owner))
							RoomBm.BlitFrom(KlackerTafel.KlackerBms[(long)KlackerTafel.Haben[x + y * 24]], x * 16 + 128, py);
						else
							RoomBm.BlitFrom(KlackerTafel.KlackerBms[(long)KlackerTafel.Haben[x + y * 24] + (73 + 8 + 3 + 3)], x * 16 + 128, py);
				}

			if ((PageNum == PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER || PageNum == PAGE_TYPE::SELECT_PLAYER_CAMPAIGN || PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER) && y == 10) break;

			if (y == 1 || y == 3 || y == 5 || y == 7)
			{
				for (x = 0; x < 24; x++)
					RoomBm.BlitFrom(KlackerTafel.KlackerBms[0l], 128 + x * 16, py * 22);

				if (PageNum == PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER || PageNum == PAGE_TYPE::SELECT_PLAYER_CAMPAIGN || PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER) py += 22;
			}
		}

		if (PageNum == PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER || PageNum == PAGE_TYPE::SELECT_PLAYER_CAMPAIGN || PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER)
			for (x = 0; x < 24; x++)
				if (KlackerTafel.Haben[x + 15 * 24])
					RoomBm.BlitFrom(KlackerTafel.KlackerBms[(long)KlackerTafel.Haben[x + 15 * 24]], x * 16 + 128, 63 + 15 * 22);

		if (PageNum == PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER || PageNum == PAGE_TYPE::SELECT_PLAYER_CAMPAIGN || PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER)
		{
			//Logo-Felder drüber blitten:
			for (y = 0; y < 4; y++)
			{
				if (KlackerTafel.Haben[3 + (y * 2 + 2) * 24] < SmallLogoBms.AnzEntries())
					RoomBm.BlitFrom(SmallLogoBms[(long)KlackerTafel.Haben[3 + (y * 2 + 2) * 24]], 215 - 80 + 48, y * 22 * 3 + 199 - 66 - 1);

				//RoomBm.BlitFrom (KlackerTafel.KlackerBms[(long)(KlackerTafel.KlackerBms.AnzEntries()-2+Sim.Players.Players[y].Owner)], 160+48, y*22*3+235-18-66);
				RoomBm.BlitFrom(KlackerTafel.KlackerBms[0], 160 + 48, y * 22 * 3 + 235 - 18 - 66);

				SLONG o = Sim.Players.Players[y].Owner;
				if (o == 2) o = 0;
				if (o == 3) if ((timeGetTime() % 1700) > 600) o = 0; else o = 1;
				if (y + o * 4 < PlayerBms.AnzEntries())
				{
					RoomBm.BlitFromT(PlayerBms[y + o * 4], 215 - 80 - 7, y * 22 * 3 + 199 - 66 - 3 + 44 - (PlayerBms[y].Size.y));

					if (/*Sim.Players.Players[y].Owner==0 ||*/ Sim.Players.Players[y].Owner == 2)
						RoomBm.BlitFromT(HakenBm, 215 - 80 - 7 + 26 + 4, y * 22 * 3 + 199 - 66 - 3 + 44 - (PlayerBms[y].Size.y) + 26 + 5);
				}
			}

			//Text-Cursor blitten:
			if (CursorY != -1)
				RoomBm.BlitFromT(KlackerTafel.Cursors[long(BlinkState % 8)], (CursorX + 6) * 16 + 128, (CursorY + 2) * 22 + (CursorY / 2 + 1) * 22 + 63);
		}
		else if (PageNum == PAGE_TYPE::MULTIPLAYER_CREATE_SESSION)
		{
			RoomBm.BlitFromT(KlackerTafel.Cursors[long(BlinkState % 8)], (CursorX) * 16 + 128, 2 * 22 + 63);
		}

		//Cursorposition auswerten:
		//SetMouseLook (CURSOR_NORMAL, 0, ROOM_TITLE, 0);

		if (!IsDialogOpen() && !MenuIsOpen())
		{
			if (PageNum == PAGE_TYPE::MAIN_MENU)
			{
				if (bFirstClass)
				{
					if (!gSpawnOnly && GridPos.IfIsWithin(1, 2, 20, 2)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 1);     //Neues Spiel
					else if (!gSpawnOnly && GridPos.IfIsWithin(1, 3, 20, 3)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 12);    //Missionen
					else if (!gSpawnOnly && GridPos.IfIsWithin(1, 4, 20, 4)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 133);   //Freies Spiel
					else if (GridPos.IfIsWithin(1, 5, 20, 5))                SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 13);    //Netz
					else if (!gSpawnOnly && GridPos.IfIsWithin(1, 6, 20, 6)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 2);     //Laden

					else if (GridPos.IfIsWithin(1, 8, 20, 8)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 3);                    //Startflughafen
					else if (GridPos.IfIsWithin(1, 9, 20, 9)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 4);                    //Optionen
					else if (!gSpawnOnly && GridPos.IfIsWithin(1, 10, 20, 10)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 5);     //Intro
					else if (GridPos.IfIsWithin(1, 11, 20, 11)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 6);                    //Credits
				}
				else
				{
					if (!gSpawnOnly && GridPos.IfIsWithin(1, 2, 20, 2)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 133);   //Freies Spiel
					else if (!gSpawnOnly && GridPos.IfIsWithin(1, 3, 20, 3)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 150);   //Kampagnen
					//else if (!gSpawnOnly && GridPos.IfIsWithin (1,  2, 20,  2)) SetMouseLook (CURSOR_HOT, 0, ROOM_TITLE, 1);     //Neues Spiel
					//else if (!gSpawnOnly && GridPos.IfIsWithin (1,  3, 20,  3)) SetMouseLook (CURSOR_HOT, 0, ROOM_TITLE, 12);    //Missionen
					//else if (!gSpawnOnly && GridPos.IfIsWithin (1,  4, 20,  4)) SetMouseLook (CURSOR_HOT, 0, ROOM_TITLE, 122);   //Missionen2
					else if (GridPos.IfIsWithin(1, 4, 20, 4))                SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 13);    //Netz
					else if (!gSpawnOnly && GridPos.IfIsWithin(1, 5, 20, 5)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 2);     //Laden

					else if (GridPos.IfIsWithin(1, 7, 20, 7)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 3);                    //Startflughafen
					else if (GridPos.IfIsWithin(1, 8, 20, 8)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 4);                    //Optionen
					else if (!gSpawnOnly && GridPos.IfIsWithin(1, 10, 20, 10)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 5);     //Intro
					else if (GridPos.IfIsWithin(1, 11, 20, 11)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 6);                    //Credits
					else if (GridPos.IfIsWithin(1, 12, 20, 12)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 7);                    //Highscores
				}

				if (GridPos.IfIsWithin(1, bFirstClass ? 13 : 14, 20, bFirstClass ? 13 : 14)) SetMouseLook(CURSOR_EXIT, 0, ROOM_TITLE, 999);
			}
			else if (PageNum == PAGE_TYPE::MISSION_SELECT || PageNum == PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER || PageNum == PAGE_TYPE::ADDON_MISSION_SELECT || PageNum == PAGE_TYPE::FLIGHT_SECURITY_MISSION_SELECT || PageNum == PAGE_TYPE::SELECT_PLAYER_CAMPAIGN || PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER)
			{
				//"Abbrechen"
				if (Line == 15 && Column >= 0 && Column <= 10)
				{
					if ((PageNum >= PAGE_TYPE::MAIN_MENU && PageNum <= PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER) || PageNum == PAGE_TYPE::ADDON_MISSION_SELECT || PageNum == PAGE_TYPE::FLIGHT_SECURITY_MISSION_SELECT || PageNum == PAGE_TYPE::SELECT_PLAYER_CAMPAIGN) SetMouseLook(CURSOR_HOT, 0, -100, 0);
				}
				//"Los!"
				if (Line == 15 && Column >= 14 && Column < 24)
				{
					if (PageNum == PAGE_TYPE::ADDON_MISSION_SELECT || PageNum == PAGE_TYPE::FLIGHT_SECURITY_MISSION_SELECT || PageNum == PAGE_TYPE::SELECT_PLAYER_CAMPAIGN || PageNum == PAGE_TYPE::MISSION_SELECT || (PageNum == PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER && NamesOK)) SetMouseLook(CURSOR_HOT, 0, -100, 0);
				}
				else if (PageNum == PAGE_TYPE::MISSION_SELECT && Line >= 2 && Line <= 2 + Sim.MaxDifficulty)     SetMouseLook(CURSOR_HOT, 0, -100, 0);
				else if (PageNum == PAGE_TYPE::ADDON_MISSION_SELECT && Line >= 2 && Line <= 2 + Sim.MaxDifficulty2 - 11) SetMouseLook(CURSOR_HOT, 0, -100, 0);
				else if (PageNum == PAGE_TYPE::FLIGHT_SECURITY_MISSION_SELECT && Line >= 2 && Line <= 2 + Sim.MaxDifficulty3 - 41) SetMouseLook(CURSOR_HOT, 0, -100, 0);

				if (PageNum == PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER || PageNum == PAGE_TYPE::SELECT_PLAYER_CAMPAIGN || PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER)
					for (SLONG c = 0; c < 4; c++)
					{
						//Check for click at names:
						if (gMousePosition.x >= 175 + 48 && gMousePosition.y >= c * 22 * 3 + 129 && gMousePosition.y <= c * 22 * 3 + 129 + 22) SetMouseLook(CURSOR_HOT, 0, -100, 0);
						else if (gMousePosition.x >= 175 + 48 && gMousePosition.y >= c * 22 * 3 + 151 && gMousePosition.y <= c * 22 * 3 + 151 + 22) SetMouseLook(CURSOR_HOT, 0, -100, 0);
						else if (gMousePosition.x >= 127 + 48 && gMousePosition.x < 160 + 48 && gMousePosition.y >= c * 22 * 3 + 151 && gMousePosition.y <= c * 22 * 3 + 151 + 22) SetMouseLook(CURSOR_HOT, 0, -100, 0);

						//Check for Click at Persons
						if (gMousePosition.x >= 128 && gMousePosition.x <= 128 + 16 * 24 && gMousePosition.y >= c * 22 * 3 + 129 && gMousePosition.y <= c * 22 * 3 + 129 + 44) SetMouseLook(CURSOR_HOT, 0, -100, 0);
					}
			}
			else if (PageNum == PAGE_TYPE::SETTINGS_CHOOSE_AIRPORT)
			{
				if ((PageSub > 0 && Line == 15 && Column >= 10 && Column <= 11) ||
					(Line == 15 && Column >= 22 && Column < 24) ||
					(Line >= 2 && Line < 14 && Cities.IsInAlbum(Line - 2 + PageSub)) ||
					(PageSub + 12 < SLONG(Cities.AnzEntries()) - 1 && Line == 15 && Column >= 13 && Column <= 14))
					SetMouseLook(CURSOR_HOT, 0, -100, 0);
			}
			else if (PageNum == PAGE_TYPE::HIGHSCORES) //Highscores
			{
				if (Line == 15 && Column >= 0 && Column <= 10)
					SetMouseLook(CURSOR_HOT, 0, -100, 0);
			}
			else if (PageNum == PAGE_TYPE::MULTIPLAYER_SELECT_NETWORK)  //Netzwerk: Provider-Medium wählen
			{
				if (Line >= 2 && Line < 2 + NetMediumCount)
					SetMouseLook(CURSOR_HOT, 0, -100, 0);

				if (GridPos.IfIsWithin(1, 15, 7, 15)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 1);
				else if (GridPos.IfIsWithin(17, 15, 24, 15)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 1);
			}
			else if (PageNum == PAGE_TYPE::MULTIPLAYER_SELECT_SESSION)  //Netzwerk: Session wählen
			{
				if (Line >= 2 && Line < 2 + (SLONG)pNetworkConnections->GetNumberOfElements())
					SetMouseLook(CURSOR_HOT, 0, -100, 0);

				if (GridPos.IfIsWithin(1, 15, 7, 15))   SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 1);
				else if (GridPos.IfIsWithin(17, 15, 24, 15) && pNetworkSessions && pNetworkSessions->GetNumberOfElements() > 0) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 1);
				else if (!gSpawnOnly && GridPos.IfIsWithin(1, 13, 20, 13)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 1);
				else if (!gSpawnOnly && GridPos.IfIsWithin(1, 10, 20, 11)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 1);
			}
			else if (PageNum == PAGE_TYPE::MULTIPLAYER_CREATE_SESSION) //Netzwerk: Session erzeugen
			{
				if (GridPos.IfIsWithin(1, 15, 7, 15)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 1);
				else if (GridPos.IfIsWithin(17, 15, 24, 15)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 1);
				else if (GridPos.IfIsWithin(1, 4, 24, 4)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 1);
				else if (GridPos.IfIsWithin(1, 6, 24, 7)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 1);
			}
			else if (PageNum == PAGE_TYPE::MULTIPLAYER_PRE_SESSION)
			{
				//Back:
				if (GridPos.IfIsWithin(1, 15, 7, 15)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 1);
				//New from Save
				if (GridPos.IfIsWithin(0, 3, 24, 4)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 999);
				if (GridPos.IfIsWithin(0, 6, 24, 6)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 999);

			}
			else if (PageNum == PAGE_TYPE::CAMPAIGN_SELECT) //Kampagnen
			{
				if (!gSpawnOnly && GridPos.IfIsWithin(1, 2, 20, 2)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 1);     //Neues Spiel
				else if (!gSpawnOnly && GridPos.IfIsWithin(1, 3, 20, 3)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 12);    //Missionen
				else if (!gSpawnOnly && GridPos.IfIsWithin(1, 4, 20, 4)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 122);   //Missionen2

				if (GridPos.IfIsWithin(1, 6, 20, 6)) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 999);
			}
			else if (PageNum == PAGE_TYPE::MP_LOADING)
			{
				if (KlackerTafel.IsFinished())
				{
					//KlackerTafel.Warp (); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();

					if ((PageSub++) == 5)
					{
						for (SLONG c = 0; c < Sim.Players.Players.AnzEntries(); c++)
							Sim.Players.Players[c].IsOut = 0;

						//Airport.LoadAirport (1,1,1,1,1,1,1,1,1,1);
						Sim.Gamestate = UBYTE((Sim.Gamestate & (~GAMESTATE_WORKING)) | GAMESTATE_DONE);

						//We load the routes and Sim when recieving the net event to prevent race conditions
						if (Sim.bNetwork != true) {
							Routen.ReInit("Routen.csv", true);
							Sim.ChooseStartup(bQuick);
						}

						Airport.LoadAirport(1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
						Airport.UpdateStaticDoorImage();
						Airport.CreateGateMapper();
					}
				}
			}
			if (PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER)  //Netzwerk: Auf Mitspieler warten
			{
				if (GridPos.IfIsWithin(1, 15, 7, 15))   SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 1);
				else if (GridPos.IfIsWithin(17, 15, 24, 15) && bThisIsSessionMaster && pNetworkPlayers && pNetworkPlayers->GetNumberOfElements() > 1)
				{
					SLONG c = 4;

					if (gNetworkSavegameLoading != -1)
						for (c = 0; c < 4; c++)
							if (Sim.Players.Players[c].Owner == 3) break;

					if (PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER && PlayerReadyAt > timeGetTime()) c = -1;
					if (PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER && (UnselectedNetworkIDs[0] || UnselectedNetworkIDs[1] || UnselectedNetworkIDs[2] || UnselectedNetworkIDs[3]))
						c = -1;

					if (c == 4) SetMouseLook(CURSOR_HOT, 0, ROOM_TITLE, 1);
				}
			}
		}

		RoomBm.PrintAt(VersionString, FontSmallRed, TEC_FONT_RIGHT, XY(0, 429), XY(519, 480));
		gKlackerPlanes.PostPaint(RoomBm);

		if (CursorY != -1)
			gKlackerPlanes.Pump(XY((CursorX + 3 + 2) * 16 + 128, (CursorY + 2) * 22 + (CursorY / 2 + 1) * 22 + 63));
		else
			gKlackerPlanes.Pump(gMousePosition);
	}

	CStdRaum::PostPaint();

	if (CurrentMenu != MENU_NONE && TopWin)
	{
		if (MenuPos.y < 0) MenuPos.y = 0;
		PrimaryBm.BlitFrom(OnscreenBitmap, MenuPos);
	}

	CheckNetEvents();
}

//--------------------------------------------------------------------------------------------
//NewGamePopup::OnLButtonDown
//--------------------------------------------------------------------------------------------
void NewGamePopup::OnLButtonDown(UINT nFlags, CPoint point)
{
	SLONG Line = (gMousePosition.y - 63) / 22;
	SLONG Column = (gMousePosition.x - 128) / 16;
	XY    GridPos = XY(Column, Line);

	DefaultOnLButtonDown();

	if (MenuIsOpen())
	{
		if (CurrentMenu == MENU_ENTERTCPIP)
		{
			XY RoomPos;
			if (!ConvertMousePosition(point, &RoomPos))
			{
				CStdRaum::OnLButtonDown(nFlags, point);
				return;
			}

			PreLButtonDown(point);
		}
		else MenuStop();
		return;
	}

	if (!PreLButtonDown(point))
	{
		SLONG c;

		SLONG Line = (point.y - 63) / 22;
		SLONG Column = (point.x - 128) / 16;

		ClickFx.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte * 100 / 7);

		if (PageNum == PAGE_TYPE::MAIN_MENU)
		{
#ifndef DEMO
			if (!gSpawnOnly && MouseClickId == 133) //"Freies Spiel"
			{
				PageNum = PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER;
				Sim.Difficulty = DIFF_FREEGAME;
				RefreshKlackerField();
			}
#endif
			else if (!gSpawnOnly && MouseClickId == 150) //"Spiel laden"
			{
				PageNum = PAGE_TYPE::CAMPAIGN_SELECT;
				RefreshKlackerField();
			}
			else if (!gSpawnOnly && MouseClickId == 2) //"Spiel laden"
			{
				NewgameWantsToLoad = TRUE;
				NewgameToOptions = TRUE;
				KlackerTafel.Warp(); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();
				Sim.Gamestate = UBYTE((GAMESTATE_OPTIONS));
				KeepRoomLib();
			}
			else if (MouseClickId == 3) //"Heimatflughafen"
			{
				PageNum = PAGE_TYPE::SETTINGS_CHOOSE_AIRPORT;
				PageSub = 0;

			again_heimatflughafen:
				for (SLONG c = PageSub; c < PageSub + 12; c++)
					if (Cities.IsInAlbum(c) && Cities.GetIdFromIndex(c) == (ULONG)Sim.HomeAirportId) break;

				if (PageSub < 500)
				{
					if (c >= PageSub + 12)
					{
						PageSub += 12;
						goto again_heimatflughafen;
					}
				}
				else PageSub = 0; //Nur zur Sicherheit, damit's keine Endlosschleife gibt

				RefreshKlackerField();
			}
			else if (MouseClickId == 4) //"Optionen"
			{
				KlackerTafel.Warp(); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();
				Sim.Gamestate = UBYTE((GAMESTATE_OPTIONS));
				KeepRoomLib();
			}
			else if (!gSpawnOnly && MouseClickId == 5) //"Intro"
			{
#ifndef NO_INTRO
				KlackerTafel.Warp(); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();
				Sim.Gamestate = UBYTE((GAMESTATE_INTRO));
#endif
			}
			else if (MouseClickId == 6) //"Credits"
			{
				KlackerTafel.Warp(); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();
				Sim.Gamestate = UBYTE((GAMESTATE_CREDITS));
			}
			else if (MouseClickId == 13) //"Netzwerkspiel"
			{
				PageNum = PAGE_TYPE::MULTIPLAYER_SELECT_NETWORK;
				Selection = Sim.Options.OptionLastProvider;
				bNetworkUnderway = TRUE;
				RefreshKlackerField();
			}
			else if (MouseClickId == 7) //Highscores
			{
				PageNum = PAGE_TYPE::HIGHSCORES;
				RefreshKlackerField();
			}
			else if (MouseClickId == 999) //"Game Over"
			{
#ifdef DEMO
				MenuStart(MENU_QUITMESSAGE);
#else
				SDL_Quit();
				Sim.Gamestate = GAMESTATE_QUIT;
				bLeaveGameLoop = TRUE;
#endif
		}

			if (bFirstClass)
			{
				if (!gSpawnOnly && MouseClickId == 1) //"Neues Spiel"
				{
					PageNum = PAGE_TYPE::MISSION_SELECT;
					Sim.Difficulty = UBYTE(Sim.Options.OptionLastMission);

#ifdef DEMO
					if ((Sim.Difficulty > DIFF_FIRST && Sim.Difficulty <= DIFF_FINAL) || Sim.Difficulty == DIFF_FREEGAME)
						Sim.Difficulty = min(DIFF_FIRST, (UBYTE)Sim.MaxDifficulty);
#endif

					RefreshKlackerField();
				}
				else if (!gSpawnOnly && MouseClickId == 12) //"Neue Missionen"
				{
					Sim.Difficulty = UBYTE(Sim.Options.OptionLastMission2);

#ifdef DEMO
					if (Sim.Difficulty > DIFF_ADDON02 && Sim.Difficulty <= DIFF_ADDON10)
						Sim.Difficulty = min(DIFF_ADDON02, (UBYTE)Sim.MaxDifficulty2);
#endif

					PageNum = PAGE_TYPE::ADDON_MISSION_SELECT;
					RefreshKlackerField();
				}
			}
	}
		else if (PageNum == PAGE_TYPE::CAMPAIGN_SELECT)
		{
			if (!gSpawnOnly && MouseClickId == 1) //"Neues Spiel"
			{
				PageNum = PAGE_TYPE::MISSION_SELECT;
				Sim.Difficulty = UBYTE(Sim.Options.OptionLastMission);

#ifdef DEMO
				if ((Sim.Difficulty > DIFF_FIRST && Sim.Difficulty <= DIFF_FINAL) || Sim.Difficulty == DIFF_FREEGAME)
					Sim.Difficulty = min(DIFF_FIRST, (UBYTE)Sim.MaxDifficulty);
#endif

				RefreshKlackerField();
			}
			else if (!gSpawnOnly && MouseClickId == 12) //"Neue Missionen"
			{
				Sim.Difficulty = UBYTE(Sim.Options.OptionLastMission2);

#ifdef DEMO
				if (Sim.Difficulty > DIFF_ADDON02 && Sim.Difficulty <= DIFF_ADDON10)
					Sim.Difficulty = min(DIFF_ADDON02, (UBYTE)Sim.MaxDifficulty2);
#endif

				PageNum = PAGE_TYPE::ADDON_MISSION_SELECT;
				RefreshKlackerField();
			}
			else if (!gSpawnOnly && MouseClickId == 122) //"Noch mehr Missionen"
			{
				Sim.Difficulty = UBYTE(Sim.Options.OptionLastMission3);

#ifdef DEMO
				if (Sim.Difficulty > DIFF_ATFS02 && Sim.Difficulty <= DIFF_ATFS10)
					Sim.Difficulty = min(DIFF_ATFS02, (UBYTE)Sim.MaxDifficulty3);
#endif

				PageNum = PAGE_TYPE::FLIGHT_SECURITY_MISSION_SELECT;
				RefreshKlackerField();
			}
			else if (MouseClickId == 999) //"Game Over"
			{
				PageNum = PAGE_TYPE::MAIN_MENU;
				RefreshKlackerField();
			}
		}
		else if (PageNum == PAGE_TYPE::MISSION_SELECT || PageNum == PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER || PageNum == PAGE_TYPE::ADDON_MISSION_SELECT || PageNum == PAGE_TYPE::FLIGHT_SECURITY_MISSION_SELECT || PageNum == PAGE_TYPE::SELECT_PLAYER_CAMPAIGN || PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER)
		{
			//"Abbrechen"
			if (Line == 15 && Column >= 0 && Column <= 10)
			{
				if (PageNum == PAGE_TYPE::MAIN_MENU)
				{
					Sim.Gamestate = GAMESTATE_BOOT;
					return;
				}
				else if (PageNum == PAGE_TYPE::MISSION_SELECT || PageNum == PAGE_TYPE::ADDON_MISSION_SELECT || PageNum == PAGE_TYPE::FLIGHT_SECURITY_MISSION_SELECT)
				{
					PageNum = bFirstClass ? PAGE_TYPE::MAIN_MENU : PAGE_TYPE::CAMPAIGN_SELECT;
					RefreshKlackerField();
					return;
				}
				else if (PageNum == PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER)
				{
					PageNum = PAGE_TYPE::MISSION_SELECT;
					if (Sim.Difficulty == DIFF_FREEGAME)
					{
						PageNum = PAGE_TYPE::MAIN_MENU;
						Sim.Difficulty = min(DIFF_FIRST, (UBYTE)Sim.MaxDifficulty);
					}

					CursorX = CursorY = -1;
					RefreshKlackerField();
					return;
				}
				else if (PageNum == PAGE_TYPE::SELECT_PLAYER_CAMPAIGN)
				{
					if (Sim.Difficulty >= DIFF_ATFS && Sim.Difficulty <= DIFF_ATFS10)
						PageNum = PAGE_TYPE::FLIGHT_SECURITY_MISSION_SELECT;
					else
						PageNum = PAGE_TYPE::ADDON_MISSION_SELECT;

					CursorX = CursorY = -1;
					RefreshKlackerField();
					return;
				}
			}
			//"Los!"
			if (Line == 15 && Column >= 14 && Column < 24)
			{
				if (PageNum == PAGE_TYPE::MISSION_SELECT)
				{
					PageNum = PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER;
					RefreshKlackerField();
					return;
				}
				else if (PageNum == PAGE_TYPE::ADDON_MISSION_SELECT)
				{
					PageNum = PAGE_TYPE::SELECT_PLAYER_CAMPAIGN;
					RefreshKlackerField();
					return;
				}
				else if (PageNum == PAGE_TYPE::FLIGHT_SECURITY_MISSION_SELECT)
				{
					PageNum = PAGE_TYPE::SELECT_PLAYER_CAMPAIGN;
					RefreshKlackerField();
					return;
				}
				else if ((PageNum == PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER || PageNum == PAGE_TYPE::SELECT_PLAYER_CAMPAIGN || PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER) && NamesOK && (PageNum != PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER || bThisIsSessionMaster))
				{
					if (PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER)
					{
						SLONG c;

						if (!(NamesOK && bThisIsSessionMaster && pNetworkPlayers && pNetworkPlayers->GetNumberOfElements() > 1))
							return;

						if (PlayerReadyAt > timeGetTime())
							return;

						if (PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER && (UnselectedNetworkIDs[0] || UnselectedNetworkIDs[1] || UnselectedNetworkIDs[2] || UnselectedNetworkIDs[3]))
							return;

						if (gNetworkSavegameLoading != -1)
							for (c = 0; c < 4; c++)
								if (Sim.Players.Players[c].Owner == 3) return;

						if (gNetworkSavegameLoading == -1)
						{
							PageNum = PAGE_TYPE::MP_LOADING;
							PageSub = 0;
						}

						Sim.Difficulty = MissionValues[SessionMissionID];

						if (gNetworkSavegameLoading == -1)
							Sim.SendSimpleMessage(ATNET_BEGINGAME, NULL, Sim.bAllowCheating, Sim.StartTime, Sim.HomeAirportId, Sim.Difficulty);
						else
							Sim.SendSimpleMessage(ATNET_BEGINGAMELOADING, NULL, Sim.bAllowCheating, Sim.StartTime, Sim.HomeAirportId, gNetworkSavegameLoading, Sim.Difficulty);

						//Sim.Difficulty     = DIFF_ATFS07;
						Sim.bWatchForReady = TRUE;
						Sim.bNetwork = true;
						bNetworkUnderway = false;

						for (c = 0; c < 4; c++) Sim.Players.Players[c].bReadyForMorning = false;

						if (gNetworkSavegameLoading != -1)
						{
							NewgameWantsToLoad = true;
							Sim.bThisIsSessionMaster = bThisIsSessionMaster;
							nWaitingForPlayer += Sim.GetSavegameNumHumans(gNetworkSavegameLoading) - 1;
							SetNetworkBitmap(3, 1); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();
							Sim.LoadGame(gNetworkSavegameLoading);
							Sim.SendSimpleMessage(ATNET_WAITFORPLAYER, NULL, -1, Sim.localPlayer);
							gNetworkSavegameLoading = -1;
							NewgameWantsToLoad = FALSE;
							return;
						}
						else {
							Routen.ReInit("Routen.csv", true);
							Sim.ChooseStartup(bQuick);
						}
					}

					Sim.bThisIsSessionMaster = bThisIsSessionMaster;
					gNetworkSavegameLoading = -1;
					NewgameWantsToLoad = FALSE;

					PageNum = PAGE_TYPE::MP_LOADING;
					PageSub = 0;

					RefreshKlackerField();
				}
			}
			//"Choose Difficulty"
			else if (((Line >= 2 && Line <= 2 + Sim.MaxDifficulty) || Line == 9) && PageNum == PAGE_TYPE::MISSION_SELECT)
			{
				/*if (Line==9 && gLanguage==LANGUAGE_D) Sim.Difficulty=DIFF_FREEGAME;
				else*/ Sim.Difficulty = min((point.y - 129 + 22) / 22, (UBYTE)Sim.MaxDifficulty);

#ifdef DEMO
				if ((Sim.Difficulty > DIFF_FIRST || Sim.Difficulty == DIFF_FREEGAME) && Sim.Difficulty != DIFF_ADDON01 && Sim.Difficulty != DIFF_ADDON02)
					Sim.Difficulty = min(DIFF_FIRST, (UBYTE)Sim.MaxDifficulty);
#endif

				RefreshKlackerField();
				KlackerTafel.Warp();
			}
			//"Choose Add-On-Mission"
			else if (((Line >= 2 && Line <= 2 + Sim.MaxDifficulty2 - 11)) && PageNum == PAGE_TYPE::ADDON_MISSION_SELECT)
			{
				Sim.Difficulty = min((point.y - 129 + 22) / 22 + 11, (UBYTE)Sim.MaxDifficulty2);

#ifdef DEMO
				if (Sim.Difficulty > DIFF_ADDON02 && Sim.Difficulty <= DIFF_ADDON10)
					Sim.Difficulty = min(DIFF_ADDON02, (UBYTE)Sim.MaxDifficulty2);
#endif

				RefreshKlackerField();
				KlackerTafel.Warp();
			}
			//"Choose Flight-Security-Mission"
			else if (((Line >= 2 && Line <= 2 + Sim.MaxDifficulty3 - 11)) && PageNum == PAGE_TYPE::FLIGHT_SECURITY_MISSION_SELECT)
			{
				Sim.Difficulty = min((point.y - 129 + 22) / 22 + 41, (UBYTE)Sim.MaxDifficulty3);

#ifdef DEMO
				if (Sim.Difficulty > DIFF_ATFS02 && Sim.Difficulty <= DIFF_ATFS10)
					Sim.Difficulty = min(DIFF_ATFS02, (UBYTE)Sim.MaxDifficulty3);
#endif

				RefreshKlackerField();
				KlackerTafel.Warp();
			}

			if (PageNum == PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER || PageNum == PAGE_TYPE::SELECT_PLAYER_CAMPAIGN || PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER)
			{
				if (PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER && gNetworkSavegameLoading != -1 && !GridPos.IfIsWithin(1, 15, 7, 15)) return;

				for (c = 0; c < 4; c++)
				{
					if (point.x >= 128 && point.x <= 175 && (Sim.Players.Players[c].NetworkID == gNetwork.GetLocalPlayerID() && PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER))
					{
						//Deselect Player:
						if (point.y >= c * 22 * 3 + 129 && point.y <= c * 22 * 3 + 129 + 44 && PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER)
						{
							Sim.Players.Players[c].NetworkID = 0;
							Sim.Players.Players[c].Owner = 1;

							CursorX = CursorY = -1;

							if (Sim.bIsHost)
							{
								Sim.Players.Players[c].NetworkID = 0;
								Sim.Players.Players[c].Owner = 1;

								for (SLONG d = 0; d < 4; d++)
									if (UnselectedNetworkIDs[d] == 0)
									{
										UnselectedNetworkIDs[d] = gNetwork.GetLocalPlayerID();
										break;
									}
								RefreshKlackerField();
							}

							Sim.SendSimpleMessage(ATNET_UNSELECTPLAYER, NULL, c, gNetwork.GetLocalPlayerID());
						}
					}
					else if (Sim.Players.Players[c].NetworkID == 0 || Sim.Players.Players[c].NetworkID == gNetwork.GetLocalPlayerID() || PageNum != PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER)
					{
						//Check for click at names:
						if (point.x >= 175 + 48 && point.y >= c * 22 * 3 + 129 && point.y <= c * 22 * 3 + 129 + 22)
						{
							CursorX = (point.x - 175 - 48) / 16;
							CursorY = c * 2;
							if (CursorX > 17) CursorX = 17;
						}
						else if (point.x >= 175 + 48 && point.y >= c * 22 * 3 + 129 + 22 && point.y <= c * 22 * 3 + 129 + 44)
							CursorX = CursorY = -1;

						//Check for Click at Persons
						if (point.x >= 128 && point.x <= 128 + 16 * 24 && point.y >= c * 22 * 3 + 129 && point.y <= c * 22 * 3 + 129 + 44)
						{
#ifdef DEMO
							if (c != 1)
							{
								MenuStart(MENU_REQUEST, MENU_REQUEST_NO_PLAYER);
								MenuSetZoomStuff(XY(320, 220), 0.17, FALSE);
							}
#else
							bool bFound = false;

							for (SLONG d = 0; d < 4; d++)
							{
								if (Sim.Players.Players[d].Owner == 0 && PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER)
								{
									Sim.SendSimpleMessage(ATNET_SELECTPLAYER, NULL, d, c, gNetwork.GetLocalPlayerID());
									memswap(&Sim.Players.Players[d].NetworkID, &Sim.Players.Players[c].NetworkID, sizeof(ULONG));
									bFound = false;
								}

								Sim.Players.Players[d].Owner = 1;
							}

							Sim.Players.Players[c].Owner = 0;
							Sim.Players.Players[c].NetworkID = gNetwork.GetLocalPlayerID();
							if (!bFound) Sim.SendSimpleMessage(ATNET_SELECTPLAYER, NULL, -1, c, gNetwork.GetLocalPlayerID());


							for (SLONG e = 0; e < 4; e++)
								if (UnselectedNetworkIDs[e] == gNetwork.GetLocalPlayerID())
									UnselectedNetworkIDs[e] = NULL;

							Sim.Options.OptionLastPlayer = c;

							if (point.x < 175 + 48) CursorX = CursorY = -1;

							CheckNames();
							RefreshKlackerField();
#endif
					}
				}
			}
		}
}
		else if (PageNum == PAGE_TYPE::SETTINGS_CHOOSE_AIRPORT)
		{
			if (PageSub > 0 && Column >= 10 && Column <= 11)
			{
				PageSub -= 12;
				if (PageSub < 0) PageSub = 0;
				RefreshKlackerField();
			}

			if (Line == 15 && Column >= 22 && Column < 24)
			{
				PageNum = PAGE_TYPE::MAIN_MENU;
				RefreshKlackerField();
			}

			if (Line >= 2 && Line < 14 && Cities.IsInAlbum(Line - 2 + PageSub))
			{
				Sim.Options.OptionAirport = Cities.GetIdFromIndex(Line - 2 + PageSub);
				Sim.HomeAirportId = Sim.Options.OptionAirport;
				RefreshKlackerField();
				KlackerTafel.Warp();
			}

			if (PageSub + 12 < SLONG(Cities.AnzEntries() - 1) && Line == 15 && Column >= 13 && Column <= 14)
			{
				PageSub += 12;
				RefreshKlackerField();
			}
		}
		else if (PageNum == PAGE_TYPE::MULTIPLAYER_SELECT_NETWORK)  //Netzwerk: Provider-Medium wählen
		{
			long c;

			for (c = 0; c < 4; c++)
			{
				Sim.Players.Players[c].Planes.ResetNextId();
				Sim.Players.Players[c].Auftraege.ResetNextId();
				Sim.Players.Players[c].Frachten.ResetNextId();
			}

			LastMinuteAuftraege.ResetNextId();
			ReisebueroAuftraege.ResetNextId();
			gFrachten.ResetNextId();

			for (c = 0; c < MAX_CITIES; c++) AuslandsAuftraege[c].ResetNextId();
			for (c = 0; c < MAX_CITIES; c++) AuslandsFrachten[c].ResetNextId();

			if (Line >= 2 && Line < 2 + NetMediumCount * 2)
			{
				Selection = (Line - 2);
				RefreshKlackerField();
				KlackerTafel.Warp();
			}

			//Zurück:
			if (GridPos.IfIsWithin(1, 15, 7, 15))
			{
				PageNum = PAGE_TYPE::MAIN_MENU;
				Sim.Options.OptionLastProvider = Selection;
				Sim.Options.WriteOptions();

				bNetworkUnderway = FALSE;
				RefreshKlackerField();
			}
			//Weiter:
			else if (GridPos.IfIsWithin(17, 15, 24, 15))
			{
				Sim.Options.OptionLastProvider = Selection;
				Sim.Options.WriteOptions();

				if (Sim.Options.OptionRandomStartday)
				{
					srand(time(NULL));
					Sim.StartTime = (rand() % 365) * 60 * 60 * 24;
				}
				else
					Sim.StartTime = time(NULL);

				SBProviderEnum id = SBNetwork::GetProviderID((char*)(LPCTSTR)pNetworkConnections->Get(NetMediumMapper[Selection] + 1));
				gNetwork.SetProvider((SBProviderEnum)id);
				if (id == SBProviderEnum::SBNETWORK_RAKNET_DIRECT_JOIN)
				{
					bThisIsSessionMaster = false;
					gHostIP = ".";
					MenuStart(MENU_ENTERTCPIP);
					return;
				}

				if (id == SBProviderEnum::SBNETWORK_RAKNET_DIRECT_HOST || id == SBProviderEnum::SBNETWORK_RAKNET_NAT_HOST) {
					PageNum = PAGE_TYPE::MULTIPLAYER_PRE_SESSION;
					CursorX = 0;
					NetworkSession = bprintf("%-25s", StandardTexte.GetS(TOKEN_NEWGAME, 703));
					RefreshKlackerField();
					return;
				}

				if (id == SBProviderEnum::SBNETWORK_ENET || id == SBProviderEnum::SBNETWORK_RAKNET_NAT_JOIN) {
					Sim.bIsHost = FALSE;
					PageNum = PAGE_TYPE::MULTIPLAYER_SELECT_SESSION;
					RefreshKlackerField();
					if (!gNetwork.StartGetSessionListAsync())
					{
						PageNum = PAGE_TYPE::MULTIPLAYER_SELECT_NETWORK;
						Selection = Sim.Options.OptionLastProvider;
						gNetwork.DisConnect();
						RefreshKlackerField();
					}
				}

				//lpDD->FlipToGDISurface ();
				/*if (gNetwork.Connect (pNetworkConnections->Get(NetMediumMapper[Selection]+1)))
				{
				   Sim.bIsHost=FALSE;
				   PageNum=15;
				   RefreshKlackerField();
				   if (!gNetwork.StartGetSessionListAsync ())
				   {
					  PageNum=13;
					  Selection=Sim.Options.OptionLastProvider;
					  gNetwork.DisConnect ();
					  RefreshKlackerField();
				   }
				}*/
			}
		}
		else if (PageNum == PAGE_TYPE::MULTIPLAYER_SELECT_SESSION) //Netzwerk: Session auswählen / erzeugen
		{
			if (pNetworkSessions && Line >= 2 && Line <= 2 + (SLONG)pNetworkSessions->GetNumberOfElements())
			{
				Selection = Line - 2;
				RefreshKlackerField();
				KlackerTafel.Warp();
			}

			//Zurück:
			if (GridPos.IfIsWithin(1, 15, 7, 15))
			{
				gNetwork.DisConnect();
				PageNum = PAGE_TYPE::MULTIPLAYER_SELECT_NETWORK;
				NewgameWantsToLoad = 0;
				Selection = Sim.Options.OptionLastProvider;
				RefreshKlackerField();
			}
			//Weiter:
			else if (GridPos.IfIsWithin(17, 15, 24, 15) && pNetworkSessions && pNetworkSessions->GetNumberOfElements() > 0)
			{
				const std::shared_ptr<SBStr> sessionName = pNetworkSessions->Get(Selection + 1); // add ref counter so that the retrieval of a new session list doesn't throw away our string mid call
				if (gNetwork.JoinSession(*sessionName, SBStr("somename")))
				{
					hprintf("This computer is client.");

					NewgameWantsToLoad = FALSE;
					gNetworkSavegameLoading = -1;

					bThisIsSessionMaster = false;

					for (SLONG d = 0; d < 4; d++) Sim.Players.Players[d].NetworkID = 0;
					Sim.Players.Players[Sim.Options.OptionLastPlayer].NetworkID = gNetwork.GetLocalPlayerID();

					//"Please let me play with you"
					TEAKFILE Message;

					Message.Announce(30);
					Message << ATNET_WANNAJOIN << gNetwork.GetLocalPlayerID() << Sim.Options.OptionLastPlayer
						<< CString(VersionString);

					Sim.SendMemFile(Message);

					PageNum = PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER;
					RefreshKlackerField();
				}
			}
			//Eigene Session erzeugen:
			else if (!gSpawnOnly && GridPos.IfIsWithin(1, 13, 20, 13))
			{
				PageNum = PAGE_TYPE::MULTIPLAYER_CREATE_SESSION;
				CursorX = 0;
				NetworkSession = bprintf("%-25s", StandardTexte.GetS(TOKEN_NEWGAME, 703));
				RefreshKlackerField();
			}
			//Load network savegame:
			else if (!gSpawnOnly && GridPos.IfIsWithin(1, 10, 16, 11))
			{
				Sim.bNetwork = true;
				NewgameWantsToLoad = 2;
				gNetworkSavegameLoading = -1;
				NewgameToOptions = TRUE;
				KlackerTafel.Warp(); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();
				Sim.Gamestate = UBYTE((GAMESTATE_OPTIONS));
				KeepRoomLib();
			}
		}
		else if (PageNum == PAGE_TYPE::MULTIPLAYER_CREATE_SESSION) //Netzwerk: Session erzeugen
		{
			//Zurück:
			if (GridPos.IfIsWithin(1, 15, 7, 15))
			{
				PageNum = PAGE_TYPE::MULTIPLAYER_SELECT_NETWORK;
				RefreshKlackerField();
				return;
				
			}
			//Weiter:
			else if (GridPos.IfIsWithin(17, 15, 24, 15))
			{
				SBNetworkCreation cr;

				NewgameWantsToLoad = 2;
				cr.sessionName = NetworkSession;
				cr.maxPlayers = 4;
				if((gNetwork.GetSelectedProviderCapabilities() & SBNETWORK_HAS_NAT) != 0)
					cr.flags = SBCreationFlags::SBNETWORK_CREATE_TRY_NAT;
				else
					cr.flags = SBCreationFlags::SBNETWORK_CREATE_NONE;

				if (gNetwork.CreateSession(NetworkSession, &cr))
				{
					Sim.bIsHost = TRUE;
					Sim.SessionName = NetworkSession;
					Sim.UniqueGameId = ((timeGetTime() ^ DWORD(rand() % 30000) ^ gMousePosition.x ^ gMousePosition.y) & 0x7fffffff);
					bThisIsSessionMaster = true;
					PlayerReadyAt = 0;

					hprintf("This computer is host.");

					for (SLONG d = 0; d < 4; d++)
					{
						Sim.Players.Players[d].NetworkID = 0;
						Sim.Players.Players[d].Owner = 1;
					}

					Sim.Players.Players[Sim.Options.OptionLastPlayer].NetworkID = gNetwork.GetLocalPlayerID();
					Sim.Players.Players[Sim.Options.OptionLastPlayer].Owner = 0;

					PageNum = PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER;
					RefreshKlackerField();
				}else {
					
				}
			}
			//Change "Allow Cheatcodes"
			else if (GridPos.IfIsWithin(1, 4, 16, 4))
			{
				Sim.bAllowCheating ^= 1;
				RefreshKlackerField();
			}
			else if (GridPos.IfIsWithin(1, 6, 24, 7))
			{
				SessionMissionID++;
				if (SessionMissionID >= _countof(MissionValues)) {
					SessionMissionID = 0;
				}

				Sim.Difficulty = MissionValues[SessionMissionID];
				RefreshKlackerField();
			}
		}
		else if (PageNum == PAGE_TYPE::HIGHSCORES) //Highscores
		{
			PageNum = PAGE_TYPE::MAIN_MENU;
			RefreshKlackerField();
		}
		else if (PageNum == PAGE_TYPE::MULTIPLAYER_PRE_SESSION) {
			//Zurück:
			if (GridPos.IfIsWithin(0, 15, 7, 15))
			{
				PageNum = PAGE_TYPE::MULTIPLAYER_SELECT_NETWORK;
				
				RefreshKlackerField();
			}
			//Eigene Session erzeugen:
			else if (!gSpawnOnly && GridPos.IfIsWithin(0, 6, 24, 6))
			{
				PageNum = PAGE_TYPE::MULTIPLAYER_CREATE_SESSION;
				CursorX = 0;
				NetworkSession = bprintf("%-25s", StandardTexte.GetS(TOKEN_NEWGAME, 703));
				RefreshKlackerField();
			}
			//Load network savegame:
			else if (!gSpawnOnly && GridPos.IfIsWithin(1, 3, 24, 4))
			{
				Sim.bNetwork = true;
				NewgameWantsToLoad = 2;
				gNetworkSavegameLoading = -1;
				NewgameToOptions = TRUE;
				KlackerTafel.Warp(); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();
				Sim.Gamestate = UBYTE((GAMESTATE_OPTIONS));
				KeepRoomLib();
			}
		}
		if (PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER)  //Netzwerk: Auf Mitspieler warten
		{
			//Back to session creation / selection:
			if (GridPos.IfIsWithin(1, 15, 7, 15))
			{
				Sim.SendSimpleMessage(ATNET_WANNALEAVE, NULL, gNetwork.GetLocalPlayerID());
				gNetwork.CloseSession();

				SBCapabilitiesFlags caps = gNetwork.GetSelectedProviderCapabilities();
				if ((caps & SBCapabilitiesFlags::SBNETWORK_HAS_SERVER_BROWSER) == SBCapabilitiesFlags::SBNETWORK_HAS_SERVER_BROWSER) {
					gNetworkSavegameLoading = -1;
					PageNum = PAGE_TYPE::MULTIPLAYER_SELECT_NETWORK;
					RefreshKlackerField();
					return;
				}

				gNetworkSavegameLoading = -1;

				PageNum = PAGE_TYPE::MULTIPLAYER_SELECT_SESSION;
				gNetwork.StartGetSessionListAsync();

				RefreshKlackerField();
			}
			//Start Network game:
			else if (GridPos.IfIsWithin(17, 15, 24, 15) && pNetworkPlayers && pNetworkPlayers->GetNumberOfElements() > 1)
			{
				//==>+<==
			}
		}
   }
}

//--------------------------------------------------------------------------------------------
//NewGamePopup::OnRButtonDown
//--------------------------------------------------------------------------------------------
void NewGamePopup::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (MenuIsOpen())
	{
		MenuRightClick(point);
	}
	else
	{
		DefaultOnRButtonDown();

		if (PageNum == PAGE_TYPE::SETTINGS_CHOOSE_AIRPORT)
		{
			PageNum = PAGE_TYPE::MAIN_MENU;
			RefreshKlackerField();
		}
		else if (PageNum == PAGE_TYPE::MULTIPLAYER_CREATE_SESSION) {
			SLONG Line = (gMousePosition.y - 63) / 22;
			SLONG Column = (gMousePosition.x - 128) / 16;
			XY    GridPos = XY(Column, Line);

			if (GridPos.IfIsWithin(1, 6, 24, 7))
			{
				SessionMissionID--;
				if (SessionMissionID < 0) {
					SessionMissionID = _countof(MissionValues) - 1;
				}

				Sim.Difficulty = MissionValues[SessionMissionID];
				RefreshKlackerField();
			}
		}
	}
}

void NewGamePopup::CheckNetEvents() {
	if (PageNum == PAGE_TYPE::MULTIPLAYER_SELECT_NETWORK || PageNum == PAGE_TYPE::MULTIPLAYER_SELECT_SESSION || PageNum == PAGE_TYPE::MULTIPLAYER_CREATE_SESSION || PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER)
		if (gNetwork.IsInitialized() && gNetwork.GetMessageCount())
		{
			TEAKFILE Message;

			if (Sim.ReceiveMemFile(Message))
			{
				ULONG MessageType, Par1 = 0, Par2 = 0, Par3 = 0;
				Message >> MessageType;

				AT_Log_I("Net", "Received net event: %s", Translate_ATNET(MessageType));

				switch (MessageType)
				{
				case ATNET_ENTERNAME:
					Message >> Par1;
					Message >> Sim.Players.Players[(SLONG)Par1].Name;
					PlayerReadyAt = max(PlayerReadyAt, timeGetTime() + READYTIME_CLICK);
					RefreshKlackerField();
					break;

				case ATNET_PUSHNAMES:
					Message >> Sim.UniqueGameId >> gNetworkSavegameLoading;
					Message >> Sim.Players.Players[(SLONG)0].Name >> Sim.Players.Players[(SLONG)1].Name
						>> Sim.Players.Players[(SLONG)2].Name >> Sim.Players.Players[(SLONG)3].Name
						>> Sim.Players.Players[(SLONG)0].NetworkID >> Sim.Players.Players[(SLONG)1].NetworkID
						>> Sim.Players.Players[(SLONG)2].NetworkID >> Sim.Players.Players[(SLONG)3].NetworkID;

					RefreshKlackerField();
					break;

				case ATNET_WANNAJOIN2:
				case ATNET_WANNAJOIN:
					if (bThisIsSessionMaster)
					{
						SLONG c, AnzHumanPlayers;
						ULONG SenderID;
						Message >> SenderID;

						for (c = AnzHumanPlayers = 0; c < 4; c++)
							if (Sim.Players.Players[c].Owner == 0 || Sim.Players.Players[c].Owner == 2)
								AnzHumanPlayers++;

						if (AnzHumanPlayers >= 4)
						{
							TEAKFILE Message;

							Message.Announce(30);
							Message << ATNET_SORRYFULL;

							gNetwork.Send(Message.MemBuffer, Message.MemBufferUsed, SenderID, false);
						}
						else if (gNetworkSavegameLoading != -1 && MessageType == ATNET_WANNAJOIN)
						{
							TEAKFILE Message;

							Message.Announce(30);
							Message << ATNET_SAVGEGAMECHECK << gNetworkSavegameLoading << Sim.GetSavegameUniqueGameId(gNetworkSavegameLoading, true);

							gNetwork.Send(Message.MemBuffer, Message.MemBufferUsed, SenderID, false);
						}
						else
						{
							SLONG WantedIndex;
							Message >> WantedIndex;

							if (MessageType == ATNET_WANNAJOIN)
							{
								CString Version;

								Message >> Version;

								if (Version.Compare(VersionString) != 0)
								{
									TEAKFILE Message;

									Message.Announce(30);
									Message << ATNET_SORRYVERSION;

									gNetwork.Send(Message.MemBuffer, Message.MemBufferUsed, SenderID, false);
									return;
								}
							}

							if (Sim.Players.Players[WantedIndex].Owner != 3 && gNetworkSavegameLoading != -1)
							{
								TEAKFILE Message;

								Message.Announce(30);
								Message << ATNET_WANNAJOIN2NO;

								gNetwork.Send(Message.MemBuffer, Message.MemBufferUsed, SenderID, false);
								return;
							}

							if (Sim.Players.Players[WantedIndex].NetworkID == NULL)
							{
								Sim.Players.Players[WantedIndex].NetworkID = SenderID;
								Sim.Players.Players[WantedIndex].Owner = 2;
							}
							else
							{
								for (SLONG c = 0; c < Sim.Players.Players.AnzEntries(); c++)
									if (Sim.Players.Players[c].NetworkID == NULL)
									{
										Sim.Players.Players[c].NetworkID = SenderID;
										Sim.Players.Players[c].Owner = 2;
										break;
									}
							}

							PlayerReadyAt = max(PlayerReadyAt, timeGetTime() + READYTIME_JOIN);
							RefreshKlackerField();
							PushNames();
						}
					}
					break;

				case ATNET_SAVGEGAMECHECK:
				{
					SLONG SavegameIndex;
					DWORD UniqueGameId;

					Message >> SavegameIndex >> UniqueGameId;

					if (Sim.GetSavegameUniqueGameId(SavegameIndex, true) == UniqueGameId)
					{
						BOOL bOld = Sim.bNetwork;
						Sim.bNetwork = true;
						Sim.SendSimpleMessage(ATNET_WANNAJOIN2, NULL, gNetwork.GetLocalPlayerID(), Sim.GetSavegameLocalPlayer(SavegameIndex));
						Sim.bNetwork = bOld;
					}
					else
					{
						PageNum = PAGE_TYPE::MULTIPLAYER_SELECT_SESSION;
						if (pNetworkConnections == NULL) pNetworkConnections = gNetwork.GetConnectionList();
						gNetwork.StartGetSessionListAsync();
						RefreshKlackerField();
						MenuStart(MENU_REQUEST, MENU_REQUEST_NET_LOADTHIS);
					}
				}
				break;

				case ATNET_WANNAJOIN2NO:
					PageNum = PAGE_TYPE::MULTIPLAYER_SELECT_SESSION;
					if (pNetworkConnections == NULL) pNetworkConnections = gNetwork.GetConnectionList();
					gNetwork.StartGetSessionListAsync();
					RefreshKlackerField();
					MenuStart(MENU_REQUEST, MENU_REQUEST_NET_LOADTHIS);
					break;

				case ATNET_SELECTPLAYER:
				{
					SLONG OldIndex, NewIndex;
					ULONG PlayerNetworkID;

					Message >> OldIndex >> NewIndex >> PlayerNetworkID;

					for (SLONG c = 0; c < 4; c++)
						if (UnselectedNetworkIDs[c] == PlayerNetworkID)
							UnselectedNetworkIDs[c] = NULL;

					if (OldIndex != -1)
						memswap(&Sim.Players.Players[OldIndex].NetworkID, &Sim.Players.Players[NewIndex].NetworkID, sizeof(ULONG));

					PlayerReadyAt = max(PlayerReadyAt, timeGetTime() + READYTIME_CLICK);
					Sim.Players.Players[NewIndex].Owner = 2;
					Sim.Players.Players[NewIndex].NetworkID = PlayerNetworkID;
					RefreshKlackerField();
				}
				break;

				case ATNET_UNSELECTPLAYER:
				{
					SLONG PlayerIndex;
					ULONG PlayerNetworkID;

					Message >> PlayerIndex >> PlayerNetworkID;

					Sim.Players.Players[PlayerIndex].NetworkID = 0;
					Sim.Players.Players[PlayerIndex].Owner = 1;

					for (SLONG c = 0; c < 4; c++)
						if (UnselectedNetworkIDs[c] == 0)
						{
							UnselectedNetworkIDs[c] = PlayerNetworkID;
							break;
						}
					RefreshKlackerField();
				}
				break;

				case ATNET_WANNALEAVE:
					if (bThisIsSessionMaster)
					{
						ULONG SenderID;
						Message >> SenderID;

						for (SLONG c = 0; c < Sim.Players.Players.AnzEntries(); c++)
							if (Sim.Players.Players[c].NetworkID == SenderID)
							{
								Sim.Players.Players[c].NetworkID = 0;

								if (gNetworkSavegameLoading != -1)
									Sim.Players.Players[c].Owner = 3;
							}

						for (SLONG c = 0; c < 4; c++)
							if (UnselectedNetworkIDs[c] == SenderID)
								UnselectedNetworkIDs[c] = NULL;

						PushNames();
					}
					break;

				case ATNET_BEGINGAME:
					if (PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER)
					{
						SLONG Time;
						SLONG difficulty;

						PageNum = PAGE_TYPE::MP_LOADING;
						PageSub = 0;

						gNetworkSavegameLoading = -1;
						NewgameWantsToLoad = FALSE;

						Message >> Sim.bAllowCheating >> Time >> Sim.HomeAirportId >> difficulty;
						Sim.Options.OptionAirport = Sim.HomeAirportId;
						Sim.StartTime = time_t(Time);

						Sim.bNetwork = true;
						bNetworkUnderway = false;
						Sim.Difficulty = difficulty;//DIFF_ATFS07;//DIFF_FREEGAME;
						Sim.bWatchForReady = TRUE;

						for (SLONG c = 0; c < 4; c++) Sim.Players.Players[c].bReadyForMorning = false;

						Sim.bThisIsSessionMaster = bThisIsSessionMaster;
						Routen.ReInit("Routen.csv", true);
						Sim.ChooseStartup(bQuick);
						RefreshKlackerField();
					}
					break;

				case ATNET_BEGINGAMELOADING:
				{
					SLONG Time;
					SLONG Index;
					SLONG difficulty;

					Message >> Sim.bAllowCheating >> Time >> Sim.HomeAirportId >> Index >> difficulty;
					Sim.Options.OptionAirport = Sim.HomeAirportId;
					Sim.StartTime = time_t(Time);

					Sim.bNetwork = true;
					bNetworkUnderway = false;
					Sim.Difficulty = difficulty;//DIFF_ATFS07;//DIFF_FREEGAME;
					Sim.bWatchForReady = TRUE;

					for (SLONG c = 0; c < 4; c++) Sim.Players.Players[c].bReadyForMorning = false;

					Sim.bThisIsSessionMaster = bThisIsSessionMaster;

					RefreshKlackerField();
					NewgameWantsToLoad = true;
					nWaitingForPlayer += Sim.GetSavegameNumHumans(Index) - 1;
					SetNetworkBitmap(3, 1); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();
					Sim.LoadGame(Index);
					Sim.SendSimpleMessage(ATNET_WAITFORPLAYER, NULL, -1, Sim.localPlayer);
					gNetworkSavegameLoading = -1;
					NewgameWantsToLoad = FALSE;
				}
				break;

				case ATNET_SORRYVERSION:
					MenuStart(MENU_REQUEST, MENU_REQUEST_NET_VERSION);
					if (PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER)
					{
						gNetworkSavegameLoading = -1;
						gNetwork.CloseSession();
						PageNum = PAGE_TYPE::MULTIPLAYER_SELECT_SESSION;
						gNetwork.StartGetSessionListAsync();
						RefreshKlackerField();
					}
					break;

				case ATNET_WAITFORPLAYER:
					Message >> Par1 >> Par2;
					nWaitingForPlayer += Par1;
					nPlayerWaiting[(SLONG)Par2] += Par1;
					if (nPlayerWaiting[(SLONG)Par2] < 0) nPlayerWaiting[(SLONG)Par2] = 0;
					SetNetworkBitmap((nWaitingForPlayer > 0) * 3);
					break;

				case ATNET_SORRYFULL:
				case DPSYS_SESSIONLOST:
					gNetwork.CloseSession();
					if (bThisIsSessionMaster)
					{
						PageNum = PAGE_TYPE::MULTIPLAYER_CREATE_SESSION;
						RefreshKlackerField();
					}
					else
					{
						PageNum = PAGE_TYPE::MULTIPLAYER_SELECT_SESSION;
						gNetwork.StartGetSessionListAsync();
						RefreshKlackerField();
					}
					break;

				case DPSYS_HOST:
					if (PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER)
					{
						gNetwork.CloseSession();
						PageNum = PAGE_TYPE::MULTIPLAYER_SELECT_SESSION;
						gNetwork.StartGetSessionListAsync();
						RefreshKlackerField();
					}
					else bThisIsSessionMaster = true;
					break;

				case ATNET_READYFORMORNING:
					Message >> Par1;
					Sim.Players.Players[SLONG(Par1)].bReadyForMorning = true;
					break;

					//Microsoft and SBLib internal codes:
				case 0x0003: case 0x0005: case 0x0007: case 0x0021:
				case 0x0102: case 0x0103: case 0x0104: case 0x0105:
				case 0x0106: case 0x0107: case 0x0108: case 0x0109:
				case 0x010A: case 0x010D: case 0xDEADBEEF:
					break;

					//Don't care:
				case ATNET_ACTIVATEAPP:
					break;

				default:
					hprintf("NGP: Unknown Message: %lx", MessageType);
					//It's okay to receive in-game messages, since we may try to join a running
					//session and get kicked out a second later.
					break;
				}
			}
		}
}

//--------------------------------------------------------------------------------------------
//NewGamePopup::OnTimer
//--------------------------------------------------------------------------------------------
void NewGamePopup::OnTimer(UINT nIDEvent)
{
	SLONG c, l;
	static int counter = 0;

	if (!bNewGamePopupIsOpen) return;

	l = strlen(KlackerFntDef);

	if (!bNewGamePopupIsOpen) return;

	//Mit 10 FPS die Anzeige rotieren lassen:
	if (nIDEvent == 1) KlackerTafel.Klack();

	//Logo-rotation korrigieren:
	if (PageNum == PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER || PageNum == PAGE_TYPE::SELECT_PLAYER_CAMPAIGN || PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER)
		for (c = 0; c < 4; c++)
		{
			if (KlackerTafel.Haben[3 + (c * 2 + 2) * 24] == l - 1)
				KlackerTafel.Haben[3 + (c * 2 + 2) * 24] = UBYTE(LogoBms.AnzEntries() - 1);

			if (KlackerTafel.Haben[3 + (c * 2 + 2) * 24] >= LogoBms.AnzEntries())
				KlackerTafel.Haben[3 + (c * 2 + 2) * 24] = 0;
		}

	if (PageNum == PAGE_TYPE::MULTIPLAYER_SELECT_NETWORK)
	{
		if (gHostIP != ".")
		{
			//lpDD->FlipToGDISurface ();

			switch (SBProviderEnum id = SBNetwork::GetProviderID((char*)(LPCTSTR)pNetworkConnections->Get(NetMediumMapper[Selection] + 1))) {
			case(SBProviderEnum::SBNETWORK_RAKNET_DIRECT_JOIN):
				if (gNetwork.Connect(pNetworkConnections->Get(NetMediumMapper[Selection] + 1), (char*)(LPCTSTR)gHostIP)) {

					hprintf("This computer is client.");

					NewgameWantsToLoad = FALSE;
					gNetworkSavegameLoading = -1;

					bThisIsSessionMaster = false;

					for (SLONG d = 0; d < 4; d++) Sim.Players.Players[d].NetworkID = 0;
					Sim.Players.Players[Sim.Options.OptionLastPlayer].NetworkID = gNetwork.GetLocalPlayerID();

					//"Please let me play with you"
					TEAKFILE Message;

					Message.Announce(30);
					Message << ATNET_WANNAJOIN << gNetwork.GetLocalPlayerID() << Sim.Options.OptionLastPlayer
						<< CString(VersionString);

					Sim.SendMemFile(Message);

					PageNum = PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER;
					RefreshKlackerField();

				}
				break;
			case(SBProviderEnum::SBNETWORK_RAKNET_DIRECT_HOST):

				break;
			}

			//if (gNetwork.Connect (pNetworkConnections->Get(NetMediumMapper[Selection]+1), (char*)(LPCTSTR)gHostIP))
			//    {
			//    Sim.bIsHost=FALSE;
			//    PageNum=15;
			//    RefreshKlackerField();
			//    if (!gNetwork.StartGetSessionListAsync ())
			//    {
			//        PageNum=13;
			//        Selection=Sim.Options.OptionLastProvider;
			//        gNetwork.DisConnect ();
			//        RefreshKlackerField();
			//    }
			//}

			gHostIP = ".";
		}
	}

	if (((counter++) & 15) == 0)
	{
		if (PageNum == PAGE_TYPE::MULTIPLAYER_SELECT_NETWORK) {
			pNetworkConnections = gNetwork.GetConnectionList();
		}
		else if (PageNum == PAGE_TYPE::MULTIPLAYER_SELECT_SESSION) {
			if (gNetwork.IsEnumSessionFinished()) {
				pNetworkSessions = gNetwork.GetSessionListAsync();
				gNetwork.StartGetSessionListAsync();
				RefreshKlackerField();
			}
		}
		else if (PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER) {
			pNetworkPlayers = gNetwork.GetAllPlayers();
			RefreshKlackerField();
		}

		if (PageNum == PAGE_TYPE::MULTIPLAYER_SELECT_NETWORK || PageNum == PAGE_TYPE::MULTIPLAYER_SELECT_SESSION) RefreshKlackerField();
	}

	BlinkState++;
}

//--------------------------------------------------------------------------------------------
//NewGamePopup::OnChar
//--------------------------------------------------------------------------------------------
void NewGamePopup::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (MenuIsOpen())
		CStdRaum::OnChar(nChar, nRepCnt, nFlags);

	if (nChar >= 'a' && nChar <= 'z') nChar = toupper(nChar);
	if (nChar == 196 || nChar == 228) nChar = (UINT)'Ä';
	if (nChar == 214 || nChar == 246) nChar = (UINT)'Ö';
	if (nChar == 220 || nChar == 252) nChar = (UINT)'Ü';

	if (CursorY != -1 && (PageNum == PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER || PageNum == PAGE_TYPE::SELECT_PLAYER_CAMPAIGN || PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER))
	{
		if (nChar == '-' || nChar == ' ' || (nChar >= 'A' && nChar <= 'Z') || nChar == 'Ä' || nChar == 'Ö' || nChar == 'Ü' || nChar == '.')
		{
			if (CursorX < 0)
			{
				if (nChar != ' ')
					Sim.Players.Players[SLONG(CursorY / 2)].Abk.SetAt(CursorX + 3, UBYTE(nChar));
			}
			else if ((CursorY & 1) == 0)
			{
				Sim.Players.Players[SLONG(CursorY / 2)].Name.SetAt(CursorX, UBYTE(nChar));
				PushName(CursorY / 2);
			}
			else if ((CursorY & 1) == 1) Sim.Players.Players[SLONG(CursorY / 2)].Airline.SetAt(CursorX, UBYTE(nChar));
			CheckNames();
			RefreshKlackerField();

			if (CursorX < 17) CursorX++;
			if (CursorX == -1) CursorX++;
		}

		if (nChar == VK_RETURN)
		{
			CursorX = 0;
			if (CursorY < 6) CursorY += 2;
		}
	}
	else if (PageNum == PAGE_TYPE::MULTIPLAYER_CREATE_SESSION)
	{
		if (nChar == '-' || nChar == ' ' || (nChar >= 'A' && nChar <= 'Z') || nChar == 'Ä' || nChar == 'Ö' || nChar == 'Ü' || nChar == '.')
		{
			NetworkSession.SetAt(CursorX, UBYTE(nChar));
			RefreshKlackerField();

			if (CursorX < 23) CursorX++;
		}
	}
}

//--------------------------------------------------------------------------------------------
//NewGamePopup::OnKeyDown
//--------------------------------------------------------------------------------------------
void NewGamePopup::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (MenuIsOpen())
		CStdRaum::OnKeyDown(nChar, nRepCnt, nFlags);

	if (CursorY != -1 && (PageNum == PAGE_TYPE::SELECT_PLAYER_SINGLEPLAYER || PageNum == PAGE_TYPE::SELECT_PLAYER_CAMPAIGN || PageNum == PAGE_TYPE::SELECT_PLAYER_MULTIPLAYER))
	{
		if (nChar == VK_LEFT)
		{
			if (CursorX > 0) CursorX--;
			else if (CursorX == 0 && (CursorY & 1) == 1) CursorX -= 2;
			else if (CursorX<-1 && CursorX>-3) CursorX--;
		}
		else if (nChar == VK_BACK)
		{
			if (CursorX > 0)
			{
				CursorX--;

				CString& str = Sim.Players.Players[SLONG(CursorY / 2)].Name;

				str = str.Left(CursorX) + str.Mid(CursorX + 1) + " ";

				PushName(CursorY / 2);

				CheckNames();
				RefreshKlackerField();
				KlackerTafel.Warp();
			}
		}
		else if (nChar == VK_DELETE)
		{
			/*if (CursorX>=0)
			{*/
			CString& str = Sim.Players.Players[SLONG(CursorY / 2)].Name;

			str = str.Left(CursorX) + str.Mid(CursorX + 1) + " ";

			PushName(CursorY / 2);

			CheckNames();
			RefreshKlackerField();
			KlackerTafel.Warp();
			/*}*/
		}

		if (nChar == VK_RIGHT && CursorX < 17)
		{
			CursorX++;
			if (CursorX == -1) CursorX++;
		}

		if (nChar == VK_UP && (CursorY > 1 || CursorX >= 0) && !gNetwork.IsInSession())
		{
			if (CursorY > 0) CursorY -= 2;
		}

		if (nChar == VK_DOWN && CursorY < 6 && !gNetwork.IsInSession())
		{
			CursorY += 2;
		}
	}
	else if (PageNum == PAGE_TYPE::MULTIPLAYER_CREATE_SESSION)
	{
		if (nChar == VK_LEFT)
		{
			if (CursorX > 0) CursorX--;
		}
		else if (nChar == VK_BACK)
		{
			if (CursorX > 0) //CursorX--;
			{
				CursorX--;
				NetworkSession = NetworkSession.Left(CursorX) + NetworkSession.Mid(CursorX + 1) + " ";
			}
			//NetworkSession.SetAt (CursorX, ' ');

			RefreshKlackerField();
			KlackerTafel.Warp();
		}
		else if (nChar == VK_DELETE)
		{
			//NetworkSession.SetAt (CursorX, 32);
			//if (CursorX<17) CursorX++;
			NetworkSession = NetworkSession.Left(CursorX) + NetworkSession.Mid(CursorX + 1) + " ";

			RefreshKlackerField();
			KlackerTafel.Warp();
		}
		else if (nChar == VK_RIGHT && CursorX < 23)
		{
			CursorX++;
		}
	}
}

//--------------------------------------------------------------------------------------------
//BOOL CStdRaum::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) : AG:
//--------------------------------------------------------------------------------------------
BOOL NewGamePopup::OnSetCursor(void* pWnd, UINT nHitTest, UINT message)
{
	return (FrameWnd->OnSetCursor(pWnd, nHitTest, message));
}

//--------------------------------------------------------------------------------------------
//void CStdRaum::OnMouseMove(UINT nFlags, CPoint point): AG:
//--------------------------------------------------------------------------------------------
void NewGamePopup::OnMouseMove(UINT nFlags, CPoint point)
{
	FrameWnd->OnMouseMove(nFlags, point);
}

//--------------------------------------------------------------------------------------------
//Update all Names:
//--------------------------------------------------------------------------------------------
void NewGamePopup::PushNames(void)
{
	TEAKFILE Message;
	Message.Announce(30);
	Message << ATNET_PUSHNAMES;
	Message << Sim.UniqueGameId << gNetworkSavegameLoading;
	Message << Sim.Players.Players[(SLONG)0].Name << Sim.Players.Players[(SLONG)1].Name
		<< Sim.Players.Players[(SLONG)2].Name << Sim.Players.Players[(SLONG)3].Name
		<< Sim.Players.Players[(SLONG)0].NetworkID << Sim.Players.Players[(SLONG)1].NetworkID
		<< Sim.Players.Players[(SLONG)2].NetworkID << Sim.Players.Players[(SLONG)3].NetworkID;
	Sim.SendMemFile(Message);
}

//--------------------------------------------------------------------------------------------
//Update one Name:
//--------------------------------------------------------------------------------------------
void NewGamePopup::PushName(SLONG n)
{
	TEAKFILE Message;

	Message.Announce(30);
	Message << ATNET_ENTERNAME << n << Sim.Players.Players[n].Name;
	Sim.SendMemFile(Message);
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
bool SIM::SendMemFile(TEAKFILE& file, ULONG target, bool useCompression)
{
	useCompression = false;
	AT_Log_I("Net", "Send Event: %s TO: %x", Translate_ATNET((file.MemBuffer[3] << 24) | (file.MemBuffer[2] << 16) | (file.MemBuffer[1] << 8) | (file.MemBuffer[0])), target);
	if ((Sim.bNetwork || bNetworkUnderway) && gNetwork.IsInSession())
		return gNetwork.Send(file.MemBuffer, file.MemBufferUsed, target, useCompression);
	else
		return (false);
}

//--------------------------------------------------------------------------------------------
//Easy message sending
//--------------------------------------------------------------------------------------------
bool SIM::SendSimpleMessage(ULONG MessageId, ULONG target)
{
	TEAKFILE Message;

	Message.Announce(30);
	Message << MessageId;
	return Sim.SendMemFile(Message, target);
}
bool SIM::SendSimpleMessage(ULONG MessageId, ULONG target, SLONG Par1)
{
	TEAKFILE Message;

	Message.Announce(30);
	Message << MessageId << Par1;
	return Sim.SendMemFile(Message, target);
}
bool SIM::SendSimpleMessage(ULONG MessageId, ULONG target, SLONG Par1, SLONG Par2)
{
	TEAKFILE Message;

	Message.Announce(30);
	Message << MessageId << Par1 << Par2;
	return Sim.SendMemFile(Message, target);
}
bool SIM::SendSimpleMessage(ULONG MessageId, ULONG target, SLONG Par1, SLONG Par2, SLONG Par3)
{
	TEAKFILE Message;

	Message.Announce(30);
	Message << MessageId << Par1 << Par2 << Par3;
	return Sim.SendMemFile(Message, target);
}
bool SIM::SendSimpleMessage(ULONG MessageId, ULONG target, SLONG Par1, SLONG Par2, SLONG Par3, SLONG Par4)
{
	TEAKFILE Message;

	Message.Announce(30);
	Message << MessageId << Par1 << Par2 << Par3 << Par4;
	return Sim.SendMemFile(Message, target);
}
bool SIM::SendSimpleMessage(ULONG MessageId, ULONG target, SLONG Par1, SLONG Par2, SLONG Par3, SLONG Par4, SLONG Par5)
{
	TEAKFILE Message;

	Message.Announce(30);
	Message << MessageId << Par1 << Par2 << Par3 << Par4 << Par5;
	return Sim.SendMemFile(Message, target);
}
bool SIM::SendSimpleMessage(ULONG MessageId, ULONG target, SLONG Par1, SLONG Par2, SLONG Par3, SLONG Par4, SLONG Par5, SLONG Par6)
{
	TEAKFILE Message;

	Message.Announce(30);
	Message << MessageId << Par1 << Par2 << Par3 << Par4 << Par5 << Par6;
	return Sim.SendMemFile(Message, target);
}
bool SIM::SendChatBroadcast(CString Message, bool bSayFromWhom, ULONG target)
{
	TEAKFILE MessageBox;
	MessageBox.Announce(30);
	MessageBox << ATNET_CHATBROADCAST;

	if (bSayFromWhom) MessageBox << SLONG(Sim.localPlayer);
	else MessageBox << SLONG(-1);

	MessageBox << Message;
	return Sim.SendMemFile(MessageBox, target);
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
bool SIM::ReceiveMemFile(TEAKFILE& file)
{
	ULONG  Size = 0;
	UBYTE* p = NULL;

	file.Close();
	file.MemBuffer.ReSize(0);

	bool rc = gNetwork.Receive(&p, Size);

	file.MemBufferUsed = Size;
	file.MemPointer = 0;

	if (p && Size)
		file.MemBuffer.ReSize(Size, p);

	return (rc);
}

