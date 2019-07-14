//============================================================================================
// Bank.cpp : Der Bankraum
//============================================================================================
#include "stdafx.h"
#include "glbank.h"
#include "Bank.h"
#include "AtNet.h"
#include <winnls.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Zum debuggen:
static const char FileId[] = "Bank";

//--------------------------------------------------------------------------------------------
//Die Bank wird eröffnet:
//--------------------------------------------------------------------------------------------
Bank::Bank (BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "bank.gli", GFX_BANK)
{
   SLONG OldZins, NewZins;

   Sim.FocusPerson=-1;

   Hdu.HercPrintf (0, "bank_bl.mcf");
   FontBankBlack.Load (lpDD, (char*)(LPCTSTR)FullFilename ("bank_bl.mcf", MiscPath));
   Hdu.HercPrintf (0, "bank_ro.mcf");
   FontBankRed.Load (lpDD, (char*)(LPCTSTR)FullFilename ("bank_ro.mcf", MiscPath));

   SP_Modem.ReSize (1);
   SP_Modem.Clips[0].ReSize (1, "Modem.smk", "", XY(307,338), SPM_IDLE,      CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_CANCANCEL,
                             NULL, SMACKER_CLIP_SET, 0, NULL,
                             "A1", 0);

   SetRoomVisited (PlayerNum, ROOM_BANK);
   Talkers.Talkers[TALKER_BANKER1].IncreaseReference ();
   Talkers.Talkers[TALKER_BANKER2].IncreaseReference ();
   DefaultDialogPartner=TALKER_BANKER1;

   MoneyBm.ReSize (pRoomLib, GFX_MONEY);

   pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("Tip.gli", GliPath), &pMenuLib, L_LOCMEM);
   ZettelBm.ReSize (pMenuLib, "BLOC1");

   //Raumanimationen
   MonitorAnim.ReSize (pRoomLib, "MONI01",  5, NULL, FALSE, ANIMATION_MODE_REPEAT, 0, 20*10);

   if (!bHandy) AmbientManager.SetGlobalVolume (40);

   CatchFly  = -1;
   FlyCaught = -1;

   SP_Pflanze.ReSize (4);
   SP_Pflanze.Clips[0].ReSize (0, "BlumW.smk", "", XY (406, 122), SPM_IDLE,     CRepeat(1,1), CPostWait(30,50), SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, &CatchFly,
                               "A9A2A2", 0, 2, 3);
   SP_Pflanze.Clips[1].ReSize (1, "Blum.smk", "SnakEat.raw", XY (406, 122), SPM_IDLE,      CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                               &FlyCaught, SMACKER_CLIP_SET|SMACKER_CLIP_FRAME+8*SMACKER_CLIP_MULT, 1, NULL,
                               "A1", 2);
   SP_Pflanze.Clips[2].ReSize (2, "BlumZug.smk", "BlumZug.raw", XY (406, 122), SPM_IDLE,   CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, &CatchFly,
                               "A1", 0);
   SP_Pflanze.Clips[3].ReSize (3, "BlumZugB.smk", "BlumZugB.raw", XY (406, 122), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, &CatchFly,
                               "A6", 0);

   SP_Mann.ReSize (9);
   SP_Mann.Clips[0].ReSize (0, "BM_Wait.smk", "", XY (430, 160), SPM_IDLE,       CRepeat(2,4), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A6A4E1", 0, 1, 2);
   SP_Mann.Clips[1].ReSize (1, "BM_Scrib.smk", "", XY (430, 160), SPM_IDLE,      CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A1", 0);
   SP_Mann.Clips[2].ReSize (2, "BM_Turn.smk", "", XY (430, 160), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A1E1E1", 4, 3, 4);
   SP_Mann.Clips[3].ReSize (3, "BM_RedeW.smk", "", XY (430, 160), SPM_LISTENING, CRepeat(2,5), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A9A2E1E1", 3, 8, 4, 7);
   SP_Mann.Clips[4].ReSize (4, "BM_Rede.smk", "", XY (430, 160), SPM_TALKING,    CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A9E1E1", 5, 3, 7);
   SP_Mann.Clips[5].ReSize (5, "BM_Taste.smk", "BM_Taste.raw", XY (430, 160), SPM_TALKING,   CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A1E1E1", 6, 3, 7);
   SP_Mann.Clips[6].ReSize (6, "BM_Rede.smk", "", XY (430, 160), SPM_TALKING,    CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A9A1E1E1", 6, 5, 3, 7);
   SP_Mann.Clips[7].ReSize (7, "BM_TurnZ.smk", "", XY (430, 160), SPM_IDLE,      CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A1", 0);
   SP_Mann.Clips[8].ReSize (8, "BM_RedeK.smk", "", XY (430, 160), SPM_LISTENING, CRepeat(1,1), CPostWait(1,1),   SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A1E1E1", 3, 4, 7);

   SP_Frau.ReSize (10);
   SP_Frau.Clips[0].ReSize (0, "BF_Wait.smk", "", XY (252, 150), SPM_IDLE,       CRepeat(2,4), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A6A4E1", 0, 1, 2);
   SP_Frau.Clips[1].ReSize (1, "BF_Work.smk", "", XY (252, 150), SPM_IDLE,       CRepeat(1,3), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A1", 0);
   SP_Frau.Clips[2].ReSize (2, "BF_Turn.smk", "", XY (252, 150), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A1E1", 3, 4);
   SP_Frau.Clips[3].ReSize (3, "BF_RedeW.smk", "", XY (252, 150), SPM_LISTENING, CRepeat(5,5), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A3A2A1E1E1", 3, 7, 5, 4, 6);
   SP_Frau.Clips[4].ReSize (4, "BF_Rede.smk", "", XY (252, 150), SPM_TALKING,    CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A9E1E1", 4, 3, 6);
   SP_Frau.Clips[5].ReSize (5, "BF_Taste.smk", "", XY (252, 150), SPM_LISTENING, CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A1E1E1E1", 3, 3, 4, 7);
   SP_Frau.Clips[7].ReSize (7, "BF_RedeK.smk", "", XY (252, 150), SPM_LISTENING, CRepeat(1,1), CPostWait(1,1),   SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A1E1E1E1", 3, 3, 4, 7);

   SP_Frau.Clips[6].ReSize (6, "BF_RedeW.smk", "", XY (252, 150), SPM_IDLE,      CRepeat(1,1), CPostWait(7,7),   SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A1A1A1", 3, 4, 8);
   SP_Frau.Clips[8].ReSize (8, "BF_RedeW.smk", "", XY (252, 150), SPM_IDLE,      CRepeat(1,1), CPostWait(7,7),   SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A1A1A1", 3, 4, 9);
   SP_Frau.Clips[9].ReSize (9, "BF_TurnZ.smk", "", XY (252, 150), SPM_IDLE,      CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A9", 0);

   SP_Fliege.ReSize (5);
   SP_Fliege.Clips[0].ReSize (0, "", "", XY (504, 99), SPM_IDLE,                 CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,
                              "A5A5", 1, 4);
   SP_Fliege.Clips[1].ReSize (1, "fliege.smk", "", XY (504, 99), SPM_IDLE,       CRepeat(3,6), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,
                              "A5", 2);
   SP_Fliege.Clips[2].ReSize (2, "fliege.smk", "", XY (504, 99), SPM_IDLE,       CRepeat(3,6), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                              &CatchFly, SMACKER_CLIP_SET|SMACKER_CLIP_FRAME+24*SMACKER_CLIP_MULT, 1, NULL,
                              "A5E1", 2, 3);
   SP_Fliege.Clips[3].ReSize (3, "", "", XY (504, 99), SPM_LISTENING,            CRepeat(1,1), CPostWait(99,99), SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,
                              "A5", 3);
   SP_Fliege.Clips[4].ReSize (4, "", "", XY (504, 99), SPM_IDLE,                 CRepeat(1,1), CPostWait(99,99), SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,
                              "A5", 4);

   CurrentAktie = -1;
   CurrentTip   = -1;
   KontoType    = -1;

   if (Sim.Options.OptionAmbiente)
   {
      SetBackgroundFx (0, "Tippen.raw",   12000);  //Schreibmaschine
      SetBackgroundFx (1, "printer.raw",  30000);    //Papierrascheln
   }

   ReloadBitmaps();

   if (Sim.Players.Players[(SLONG)PlayerNum].HasBerater (BERATERTYP_GELD)>75)
   {
      if ((Sim.Difficulty==DIFF_NORMAL && Sim.Players.Players[(SLONG)PlayerNum].SollZins==6) ||
          (Sim.Difficulty==DIFF_HARD   && Sim.Players.Players[(SLONG)PlayerNum].SollZins==10) ||
          (Sim.Difficulty==DIFF_FINAL  && Sim.Players.Players[(SLONG)PlayerNum].SollZins==15))
      {
         OldZins = Sim.Players.Players[(SLONG)PlayerNum].SollZins;
         NewZins = Sim.Players.Players[(SLONG)PlayerNum].SollZins = UBYTE((OldZins+Sim.Players.Players[(SLONG)PlayerNum].HabenZins)/2);
         Sim.Players.Players[(SLONG)PlayerNum].Messages.AddMessage (BERATERTYP_GELD, bprintf(StandardTexte.GetS (TOKEN_ADVICE, 5000), OldZins, NewZins));
      }
   }

   ShowWindow(SW_SHOW);
   UpdateWindow();
}

//--------------------------------------------------------------------------------------------
//Die Welt geht unter:
//--------------------------------------------------------------------------------------------
Bank::~Bank()
{
   ZettelBm.Destroy();
   if (pGfxMain && pMenuLib) pGfxMain->ReleaseLib (pMenuLib);

   Talkers.Talkers[TALKER_BANKER1].DecreaseReference ();
   Talkers.Talkers[TALKER_BANKER2].DecreaseReference ();
}

//--------------------------------------------------------------------------------------------
//Das **x Windows hat mal wieder die Bitmaps verschlunzt:
//--------------------------------------------------------------------------------------------
void Bank::ReloadBitmaps ()
{
}

//--------------------------------------------------------------------------------------------
//BEGIN_MESSAGE_MAP(Bank, CStdRaum)
//--------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(Bank, CStdRaum)
	//{{AFX_MSG_MAP(Bank)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////////////////////
// Bank message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

ULONG AktienKursLineColor [4] = { 0x0080ff, 0x00ff40, 0xff0000, 0xffff00 };
static XY TopLeft[4]       = { XY(388,41), XY(414,49), XY(388,99),  XY(414,99) };
static XY TopRight[4]      = { XY(408,47), XY(427,50), XY(408,99),  XY(427,99) };
static XY BottomLeft[4]    = { XY(388,94), XY(414,92), XY(388,152), XY(414,146) };
static XY BottomRight[4]   = { XY(408,94), XY(427,94), XY(408,148), XY(427,142) };

//--------------------------------------------------------------------------------------------
//void Bank::OnPaint()
//--------------------------------------------------------------------------------------------
void Bank::OnPaint() 
{
   SLONG c;

#ifdef _DEBUG___
   WORD inbuf[256];
   CFile f ("E:\\sb-projekt\\AtPlatin\\docs\\This is a test text in unicode.txt", CFile::modeRead);
   f.Read ((void*)inbuf, 70);
   f.Close();
   inbuf[70]=0;

   char buf[256];
   //strcpy ((char*)inbuf+2, "¡m²§ºØ¡Ð¯}Ãµ¦Ó¥X¡n");
   inbuf[1]=0xFEC4;
   inbuf[2]=0xFEB6;
   /*inbuf[1]+=256*1;
   inbuf[2]+=256*1;
   inbuf[3]+=256*1;
   inbuf[4]+=256*1;
   inbuf[5]+=256*1;*/
 
   //¡m²§ºØ¡Ð¯}Ãµ¦Ó¥X¡n
   {
      CPaintDC dc(this);

      CFont f;
      f.CreateFont (-20,   //Height
                    0,           //Width
                    0,           //Escapement
                    0,           //Orientation
                    FW_NORMAL,   //Weight
                    (BYTE)false, //Italic
                    0,           //Unterline
                    0,           //Strikeout
                    0,           //Charset
                    0,           //OutPrecision
                    0,           //ClipPrecision
                    0,           //Quality
                    FF_DONTCARE,
                    "Arial"); //Lucida Sans Unicode");


      CFont *pOldFont = dc.SelectObject(&f);

      int rc = WideCharToMultiByte (CP_ACP, 0, L"ÿþ", -1, buf, 256, NULL, NULL);
      TextOut(dc, 100,100, buf, 2);
      TextOutW(dc, 100,130, L"ÿþÿþ", 2);

      int rd = WideCharToMultiByte (CP_ACP, 0, inbuf, -1, buf, 256, NULL, NULL);
      TextOut(dc, 100,160, buf, 35);
      TextOutW(dc, 100,190, inbuf, 35);
      TextOut(dc, 100,220, (char*)inbuf, 35);

      dc.SelectObject(pOldFont);
   }
#else
   CPaintDC dc(this);
#endif

   if (Sim.Date>5) Sim.GiveHint (HINT_BANK);

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   if (Sim.MoneyInBankTrash)
      RoomBm.BlitFrom (MoneyBm, 281, 368);

   for (c=0; c<Sim.Players.AnzPlayers; c++)
      if (!Sim.Players.Players[c].IsOut)
      {
         FBUFFER<SLONG> Kurse(10);
         SLONG          d, Max;

         for (Max=d=0; d<10; d++)
         {
            Kurse[d]=SLONG(Sim.Players.Players[c].Kurse[9-d]);

            if (Max<Kurse[d]) Max=Kurse[d];
         }

         Max=Max+Max/2;
         DrawChart (RoomBm, AktienKursLineColor[c], Kurse, 0, Max, TopLeft[c], TopRight[c], BottomLeft[c], BottomRight[c]);
      }

   //Die Raum-Animationen:
   MonitorAnim.BlitAtT (RoomBm, 187, 200);

   SP_Fliege.Pump ();
   SP_Fliege.BlitAtT (RoomBm);
   SP_Pflanze.Pump ();
   SP_Pflanze.BlitAtT (RoomBm);
   SP_Frau.Pump ();
   SP_Frau.BlitAtT (RoomBm);
   SP_Mann.Pump ();
   SP_Mann.BlitAtT (RoomBm);

   if (FlyCaught==1)
   {
      CatchFly  = -1;
      FlyCaught = 2; //Fliege gefangen
      SP_Fliege.SetDesiredMood (SPM_LISTENING); //Fliege verschwinden lassen
   }

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   if (Sim.Players.Players[(SLONG)PlayerNum].SecurityFlags&(1<<3))
   {
      SP_Modem.Pump ();
      SP_Modem.BlitAtT (RoomBm);
   }

   if (!IsDialogOpen() && !MenuIsOpen())
   {
      //Ggf. Kontoauszug einbauen:
      if (KontoType==1 || KontoType==2)
      {
         PrimaryBm.BlitFromT (TipBm, WinP1.x+(WinP2.x-WinP1.y-TipBm.Size.x)/2, WinP1.y+(WinP2.y-WinP1.y-StatusLineSizeY-TipBm.Size.y)/2);
      }

           if (gMousePosition.IfIsWithin (462,207,601,352)) SetMouseLook (CURSOR_HOT, 0, ROOM_BANK, 10);
      else if (gMousePosition.IfIsWithin (251,155,335,262)) SetMouseLook (CURSOR_HOT, 0, ROOM_BANK, 11);
      else if (gMousePosition.IfIsWithin (62,64,173,186)) SetMouseLook (CURSOR_EXIT, 0, ROOM_BANK, 999);
      else if (Sim.MoneyInBankTrash && gMousePosition.IfIsWithin (288,388,326,410)) SetMouseLook (CURSOR_HOT, 0, ROOM_BANK, 20);
   }

   CStdRaum::PostPaint ();

   //Ggf. Tips einblenden
   if (gMousePosition.IfIsWithin(378, 33, 436, 160))
   {
      SLONG NewTip = (gMousePosition.x-378)/30 + (gMousePosition.y-33)/64*2;

      if (!Sim.Players.Players[NewTip].IsOut)
      {
         if (NewTip != CurrentTip)
         {
            CurrentTip = NewTip;
            TipBm.ReSize (ZettelBm.Size);
            TipBm.BlitFrom (ZettelBm);
            DrawKursTipContents (TipBm, PlayerNum, NewTip, &FontSmallBlack, &FontSmallBlack);
         }

         PrimaryBm.BlitFromT (TipBm, 163, 23);
      }
   }
   else CurrentTip = -1;

   CStdRaum::PumpToolTips ();
}

//--------------------------------------------------------------------------------------------
//void Bank::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void Bank::OnLButtonDown(UINT nFlags, CPoint point) 
{
   XY      RoomPos;
   PLAYER &qPlayer = Sim.Players.Players[(SLONG)PlayerNum];

   DefaultOnLButtonDown ();

   if (!ConvertMousePosition (point, &RoomPos))
   {
	   CWnd::OnLButtonDown(nFlags, point);
      return;
   }

   if (!PreLButtonDown (point))
   {
      if (MouseClickArea==ROOM_BANK && MouseClickId==999) qPlayer.LeaveRoom();
      else if (MouseClickArea==ROOM_BANK && MouseClickId==10) StartDialog (TALKER_BANKER1, MEDIUM_AIR);
      else if (MouseClickArea==ROOM_BANK && MouseClickId==11) StartDialog (TALKER_BANKER2, MEDIUM_AIR);
      else if (MouseClickArea==ROOM_BANK && MouseClickId==20)
      {
         //Geld gefunden:
         Sim.MoneyInBankTrash=0;
         qPlayer.ChangeMoney (100000, 2006, "");
         qPlayer.NetSynchronizeMoney ();
         Sim.SendSimpleMessage (ATNET_TAKETHING, NULL, ITEM_NONE);

         StartDialog (TALKER_BANKER1, MEDIUM_AIR, 20);
      }
      else CStdRaum::OnLButtonDown(nFlags, point);
   }
}

//--------------------------------------------------------------------------------------------
//void Bank::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void Bank::OnRButtonDown(UINT nFlags, CPoint point) 
{
   DefaultOnRButtonDown ();

   //Außerhalb geklickt? Dann Default-Handler!
   if (point.x<WinP1.x || point.y<WinP1.y || point.x>WinP2.x || point.y>WinP2.y)
   {
      CWnd::OnRButtonDown(nFlags, point);
      return;
   }
   else
   {
      if (MenuIsOpen())
      {
         MenuRightClick (point);
      }
      else
      {
         if (!IsDialogOpen() && point.y<440)
            Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();

         CStdRaum::OnRButtonDown(nFlags, point);
      }
   }
}

//============================================================================================
// CBilanz::
//============================================================================================
// Die Bilanzdaten:
//============================================================================================

//--------------------------------------------------------------------------------------------
//Löscht die Bilanz
//--------------------------------------------------------------------------------------------
void  CBilanz::Clear(void)
{
   HabenZinsen=Tickets=Auftraege=SollRendite=HabenRendite=0;
   SollZinsen=Kerosin=Personal=Vertragsstrafen=Wartung=Gatemiete=Citymiete=Routenmiete=0;
}

//--------------------------------------------------------------------------------------------
//Gibt den Saldo der Habens-Seite zurück:
//--------------------------------------------------------------------------------------------
SLONG CBilanz::GetHaben(void)
{
   return (HabenZinsen+Tickets+Auftraege+HabenRendite);
}

//--------------------------------------------------------------------------------------------
//Gibt den Saldo der Soll-Seite zurück:
//--------------------------------------------------------------------------------------------
SLONG CBilanz::GetSoll(void)
{
   return (SollZinsen+Kerosin+Personal+Vertragsstrafen+Wartung+Gatemiete+Citymiete+Routenmiete+SollRendite);
}

//--------------------------------------------------------------------------------------------
//Gibt den Saldo zurück:
//--------------------------------------------------------------------------------------------
SLONG CBilanz::GetSumme(void)
{
   return (GetHaben()-GetSoll());
}

//--------------------------------------------------------------------------------------------
//Speichert einen Bilanz-Datensatz:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CBilanz &Bilanz)
{
   File << Bilanz.HabenZinsen  << Bilanz.Tickets << Bilanz.Auftraege;
   File << Bilanz.HabenRendite << Bilanz.SollRendite;
   File << Bilanz.SollZinsen   << Bilanz.Kerosin << Bilanz.Personal;
   File << Bilanz.Vertragsstrafen << Bilanz.Wartung << Bilanz.Gatemiete;
   File << Bilanz.Citymiete       << Bilanz.Routenmiete;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt einen Bilanzdatensatz:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CBilanz &Bilanz)
{
   File >> Bilanz.HabenZinsen  >> Bilanz.Tickets >> Bilanz.Auftraege;
   File >> Bilanz.HabenRendite >> Bilanz.SollRendite;
   File >> Bilanz.SollZinsen   >> Bilanz.Kerosin >> Bilanz.Personal;
   File >> Bilanz.Vertragsstrafen >> Bilanz.Wartung >> Bilanz.Gatemiete;
   File >> Bilanz.Citymiete       >> Bilanz.Routenmiete;

   return (File);
}