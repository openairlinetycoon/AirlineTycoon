//============================================================================================
// Options.cpp : Das Optionsmenü
//============================================================================================
// Link: "Options.h"
//============================================================================================
#include "stdafx.h"
#include "glstart.h"
#include "atnet.h"
#include "SbLib.h"
#include "network.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SLONG OptionsShortcut;
extern SLONG NewgameWantsToLoad;
extern SLONG NewgameToOptions;
extern SLONG gNetworkSavegameLoading;

extern char VersionString[];
extern SBNetwork gNetwork;

static SLONG nLocalOptionsOption=0;

//--------------------------------------------------------------------------------------------
//ULONG PlayerNum
//--------------------------------------------------------------------------------------------
Options::Options (BOOL bHandy, SLONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "startup.gli", GFX_HAUPTMNU)
{
   nLocalOptionsOption++;

   ChangedDisplay = false;

   //Das Optionen-Fenster ist offen! Alles anhalten!
   nOptionsOpen++;
   if (Sim.bNetwork) Sim.SendSimpleMessage (ATNET_OPTIONS, NULL, 1, Sim.localPlayer);

   if (!bHandy) AmbientManager.SetGlobalVolume (0);

   AmbientFX.ReInit("raunen.raw");
   EffektFX.ReInit("Kaffee.raw");
   PlaneFX.ReInit("prop.raw");
   TalkFX.ReInit("opt_say.raw");
   DurchsagenFX.ReInit("opt_hinw.raw");
   ClickFx.ReInit("change.raw");

   Hdu.HercPrintf (0, "stat_1.mcf");
   VersionFont.Load (lpDD, (char*)(LPCTSTR)FullFilename ("stat_1.mcf", MiscPath));

   Options::PageNum    = 1;
   Options::PlayerNum  = PlayerNum;
   Options::CursorY    = -1;
   Options::BlinkState = 0;

   if (NewgameWantsToLoad)
   {
      UpdateSavegameNames ();
      Options::PageNum = 5;
   }

   if (OptionsShortcut!=-1)
   {
      UpdateSavegameNames ();
      Options::PageNum = (UBYTE)OptionsShortcut;
   }

   if ((Sim.Gamestate & 31)!=GAMESTATE_INIT) gKlackerPlanes.Reset();

   RefreshKlackerField();

   SDL_ShowWindow(FrameWnd->m_hWnd);
   SDL_UpdateWindowSurface(FrameWnd->m_hWnd);

   //Create a timer to 'klacker'
   TimerId = SDL_AddTimer(50, TimerFunc, this);
   if (!TimerId) TimerFailure = 1;
            else TimerFailure = 0;
}

//--------------------------------------------------------------------------------------------
//Options-Fenster zerstören:
//--------------------------------------------------------------------------------------------
Options::~Options()
{
   SLONG c;
   if (TimerId) SDL_RemoveTimer(TimerId);

   nLocalOptionsOption--;

   if (Sim.bNetwork) Sim.SendSimpleMessage (ATNET_OPTIONS, NULL, -1, Sim.localPlayer);

   Sim.Options.WriteOptions ();

   if (!bLeaveGameLoop)
      for (c=0; c<Sim.Players.AnzPlayers; c++)
         if (!Sim.Players.Players[c].IsOut)
            Sim.Players.Players[c].CalcRoom ();

   gDisablePauseKey = FALSE;
   if (nOptionsOpen>0) nOptionsOpen--;
   if (Sim.bNetwork) SetNetworkBitmap ((nOptionsOpen>0)*1);


   Sim.SaveOptions ();

   if (NewgameWantsToLoad==1) NewgameWantsToLoad=FALSE;
   NewgameToOptions=FALSE;

   StatusCount=60;
   OptionsShortcut=-1;
}

//--------------------------------------------------------------------------------------------
//Welche Savegame Names haben wir auf der Pladde?
//--------------------------------------------------------------------------------------------
void Options::UpdateSavegameNames (void)
{
   SLONG   c;
   CString Filename;

   const char *pNamebaseStr;

   if (Sim.bNetwork) pNamebaseStr = "Net%li.dat";
                else pNamebaseStr = "Game%li.dat";

   for (c=0; c<12; c++)
   {
      Filename = FullFilename ((LPCTSTR)bprintf(pNamebaseStr, c), SavegamePath);

      if (DoesFileExist (Filename))
      {
         SLONG    SaveVersion;
         SLONG    SaveVersionSub;
         TEAKFILE InputFile (Filename, TEAKFILE_READ);

         InputFile >> SavegameNames[c];
         SavenamesValid[c] = TRUE;

         InputFile >> SaveVersion >> SaveVersionSub;

         if (SaveVersionSub>0)
         {
            //Info-String ignorieren
            InputFile >> SavegameInfos[c];
         }
      }
      else
      {
         SavegameNames[c]  = StandardTexte.GetS (TOKEN_MISC, 4073);
         SavenamesValid[c] = FALSE;
      }
   }
}

//--------------------------------------------------------------------------------------------
//Aktualisiert die Text-Daten im Klacker-Feld:
//--------------------------------------------------------------------------------------------
void Options::RefreshKlackerField(void)
{
   SLONG c;

   KlackerTafel.Clear ();

   KlackerTafel.PrintAt (0,  1, "========================");

   if (PageNum==1)
   {
      KlackerTafel.PrintAt (0, 0, StandardTexte.GetS (TOKEN_MISC, 4000));
      KlackerTafel.PrintAt (0, 2, StandardTexte.GetS (TOKEN_MISC, 4001));
      KlackerTafel.PrintAt (0, 3, StandardTexte.GetS (TOKEN_MISC, 4002));
      KlackerTafel.PrintAt (0, 4, StandardTexte.GetS (TOKEN_MISC, 4003));

      if ((Sim.Gamestate & 31)!=GAMESTATE_INIT)
      {
         KlackerTafel.PrintAt (0, 6, StandardTexte.GetS (TOKEN_MISC, 4004));
         KlackerTafel.PrintAt (0, 7, StandardTexte.GetS (TOKEN_MISC, 4005));
      }

      if ((Sim.Gamestate & 31)==GAMESTATE_INIT)
      {
         KlackerTafel.PrintAt (0, 6,StandardTexte.GetS (TOKEN_MISC, 4007));
      }
      else
      {
         KlackerTafel.PrintAt (0, 9, StandardTexte.GetS (TOKEN_MISC, 4006));
         KlackerTafel.PrintAt (0, 11,StandardTexte.GetS (TOKEN_MISC, bFirstClass?4007:4008));
      }

      //KlackerTafel.PrintAt (0, 15, VersionString);
   }
   else if (PageNum==2) //Grafik-Optionen
   {
      KlackerTafel.PrintAt (0, 0, StandardTexte.GetS (TOKEN_MISC, 4010));
      KlackerTafel.PrintAt (0, 2, StandardTexte.GetS (TOKEN_MISC, 4011+Sim.Options.OptionPlanes));
      KlackerTafel.PrintAt (0, 3, StandardTexte.GetS (TOKEN_MISC, 4013+Sim.Options.OptionPassengers));
      KlackerTafel.PrintAt (0, 4, StandardTexte.GetS (TOKEN_MISC, 4015+Sim.Options.OptionBlenden));
      KlackerTafel.PrintAt (0, 5, StandardTexte.GetS (TOKEN_MISC, 4020+Sim.Options.OptionThinkBubbles));
      KlackerTafel.PrintAt (0, 6, StandardTexte.GetS (TOKEN_MISC, 4022+Sim.Options.OptionFlipping));
      KlackerTafel.PrintAt (0, 7, StandardTexte.GetS (TOKEN_MISC, 4024+Sim.Options.OptionTransparenz));
      KlackerTafel.PrintAt (0, 8, StandardTexte.GetS (TOKEN_MISC, 4026+Sim.Options.OptionSchatten));
      
      KlackerTafel.PrintAt(0, 10, Sim.Options.OptionFullscreen == 0 ? "# Display : Fullscreen" : Sim.Options.OptionFullscreen == 1 ? "# Display : Windowed" : Sim.Options.OptionFullscreen == 2 ? "# Display : Borderless" : "???");
      KlackerTafel.PrintAt(0, 11, ((Sim.Options.OptionKeepAspectRatio == true) ? "# Aspect Ratio: Keep" : "# Aspect Ratio: Stretch"));

      KlackerTafel.PrintAt (0, 13, StandardTexte.GetS (TOKEN_MISC, 4099));
   }
   else if (PageNum==3) //Musik-Optionen
   {
      KlackerTafel.PrintAt (0, 0, StandardTexte.GetS (TOKEN_MISC, 4120));
      KlackerTafel.PrintAt (0, 2, StandardTexte.GetS (TOKEN_MISC, 4130+Sim.Options.OptionEnableMidi));
      //KlackerTafel.PrintAt (0, 3, StandardTexte.GetS (TOKEN_MISC, 4121));
      KlackerTafel.PrintAt (1, 4, StandardTexte.GetS (TOKEN_MISC, 4150+Sim.Options.OptionLoopMusik));

      KlackerTafel.PrintAt (0, 7, StandardTexte.GetS (TOKEN_MISC, 4132+Sim.Options.OptionEnableDigi));
      //KlackerTafel.PrintAt (0, 8, StandardTexte.GetS (TOKEN_MISC, 4127));
      KlackerTafel.PrintAt (1, 9, StandardTexte.GetS (TOKEN_MISC, 4122));
      KlackerTafel.PrintAt (1, 10, StandardTexte.GetS (TOKEN_MISC, 4123));
      KlackerTafel.PrintAt (1, 11, StandardTexte.GetS (TOKEN_MISC, 4124));
      KlackerTafel.PrintAt (1, 12, StandardTexte.GetS (TOKEN_MISC, 4125));
      KlackerTafel.PrintAt (1, 13, StandardTexte.GetS (TOKEN_MISC, 4126));
      //KlackerTafel.PrintVolumeAt (15, 3, 8, Sim.Options.OptionMusik);
      //KlackerTafel.PrintVolumeAt (15, 8, 8, Sim.Options.OptionMasterVolume);
      KlackerTafel.PrintVolumeAt (15, 9, 8, Sim.Options.OptionAmbiente);
      KlackerTafel.PrintVolumeAt (15, 10, 8, Sim.Options.OptionDurchsagen);
      KlackerTafel.PrintVolumeAt (15, 11, 8, Sim.Options.OptionTalking);
      KlackerTafel.PrintVolumeAt (15, 12, 8, Sim.Options.OptionEffekte);
      KlackerTafel.PrintVolumeAt (15, 13, 8, Sim.Options.OptionPlaneVolume);

      if (Sim.Options.OptionLoopMusik==0)
         KlackerTafel.PrintAt (1, 5, StandardTexte.GetS (TOKEN_MISC, 4140));

      KlackerTafel.PrintAt (0, 15, StandardTexte.GetS (TOKEN_MISC, 4099));
   }
   else if (PageNum==4) //Sonstiges
   {
      KlackerTafel.PrintAt (0, 0, StandardTexte.GetS (TOKEN_MISC, 4030));
      KlackerTafel.PrintAt (0, 2, StandardTexte.GetS (TOKEN_MISC, 4031+Sim.Options.OptionGirl));
      KlackerTafel.PrintAt (0, 3, StandardTexte.GetS (TOKEN_MISC, 4033+Sim.Options.OptionBerater));
      KlackerTafel.PrintAt (0, 4, StandardTexte.GetS (TOKEN_MISC, 4038+Sim.Options.OptionAutosave));
      KlackerTafel.PrintAt (0, 5, StandardTexte.GetS (TOKEN_MISC, 4040+Sim.Options.OptionFax));
      KlackerTafel.PrintAt (0, 6, StandardTexte.GetS (TOKEN_MISC, 4042+Sim.Options.OptionRealKuerzel));
      KlackerTafel.PrintAt (0, 7, StandardTexte.GetS (TOKEN_MISC, 4044+Sim.Options.OptionSpeechBubble));
      KlackerTafel.PrintAt (0, 8, StandardTexte.GetS (TOKEN_MISC, 4048+Sim.Options.OptionBriefBriefing));
      KlackerTafel.PrintAt (0, 9, StandardTexte.GetS (TOKEN_MISC, 4050+Sim.Options.OptionRandomStartday));
      KlackerTafel.PrintAt (0, 11, StandardTexte.GetS (TOKEN_MISC, 4099));
   }
   else if (PageNum==5) //Laden
   {
      KlackerTafel.PrintAt (0, 0, StandardTexte.GetS (TOKEN_MISC, 4070));

      for (c=0; c<12; c++)
         KlackerTafel.PrintAt (1, 2+c, bprintf ("%2li:%s", c+1,  (LPCTSTR)SavegameNames[c]));

      KlackerTafel.PrintAt (0, 15, StandardTexte.GetS (TOKEN_MISC, 4096));
   }
   else if (PageNum==6) //Speichern
   {
      KlackerTafel.PrintAt (0, 0, StandardTexte.GetS (TOKEN_MISC, 4071));

      for (c=0; c<11; c++)
         KlackerTafel.PrintAt (1, 2+c, bprintf ("%2li:%s", c+1,  (LPCTSTR)SavegameNames[c]));

      if (CursorY!=-1)
      {
         KlackerTafel.PrintAt (0, 15, StandardTexte.GetS (TOKEN_MISC, 4097));
         KlackerTafel.PrintAt (22, 15, StandardTexte.GetS (TOKEN_MISC, 4098));
      }
      else
         KlackerTafel.PrintAt (0, 15, StandardTexte.GetS (TOKEN_MISC, 4096));
   }
   else if (PageNum==7) //Quit
   {
      KlackerTafel.PrintAt (0, 0, StandardTexte.GetS (TOKEN_MISC, 4060));
      KlackerTafel.PrintAt (0, 2, StandardTexte.GetS (TOKEN_MISC, 4061));
      KlackerTafel.PrintAt (0, 4, StandardTexte.GetS (TOKEN_MISC, 4062));
      KlackerTafel.PrintAt (0, 5, StandardTexte.GetS (TOKEN_MISC, 4063));
   }
   else if (PageNum==8) //New Game
   {
      KlackerTafel.PrintAt (0, 0, StandardTexte.GetS (TOKEN_MISC, 4065));
      KlackerTafel.PrintAt (0, 2, StandardTexte.GetS (TOKEN_MISC, 4061));
      KlackerTafel.PrintAt (0, 4, StandardTexte.GetS (TOKEN_MISC, 4062));
      KlackerTafel.PrintAt (0, 5, StandardTexte.GetS (TOKEN_MISC, 4063));
   }
}

/////////////////////////////////////////////////////////////////////////////
// Options message handlers

//--------------------------------------------------------------------------------------------
// void Options::OnPaint():
//--------------------------------------------------------------------------------------------
void Options::OnPaint() 
{
   static SLONG x, y, py; x++;
   static SLONG LastLine=-1;

   if (nLocalOptionsOption==0) return;

   SLONG Line   = (gMousePosition.y-63)/22;
   SLONG Column = (gMousePosition.x-128)/16;

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   if (Column<0 || Column>24)
   {
      Line   = -1;
      Column = -1;
   }

   if (PageNum==3 && gMouseLButton) //Sound Slide
   {
      if (Column>=15 && Column<23 && Line>=3 && Line<=13)
      {
         if (Line==9 && Sim.Options.OptionAmbiente!=Column-15) { Sim.Options.OptionAmbiente=Column-15; AmbientManager.RecalcVolumes(); AmbientFX.SetVolume(Prozent2Dezibel(Sim.Options.OptionAmbiente*100/8)); AmbientFX.Play(DSBPLAY_NOSTOP|DSBPLAY_LOOPING); }
         if (Line==10 && Sim.Options.OptionDurchsagen!=Column-15) { Sim.Options.OptionDurchsagen=Column-15; DurchsagenFX.SetVolume(Prozent2Dezibel(Sim.Options.OptionDurchsagen*100/8)); DurchsagenFX.Play(); }

#if !defined(NO_D_VOICES) || !defined(NO_E_VOICES) || !defined(NO_N_VOICES)
         if (bVoicesNotFound==false)
         {
            if (Line==11 && Sim.Options.OptionTalking!=Column-15) { Sim.Options.OptionTalking=Column-15; TalkFX.SetVolume(Prozent2Dezibel(Sim.Options.OptionTalking*100/8)); TalkFX.Play(); }
         }
#endif

         if (Line==12 && Sim.Options.OptionEffekte!=Column-15) { Sim.Options.OptionEffekte=Column-15; EffektFX.SetVolume(Prozent2Dezibel(Sim.Options.OptionEffekte*100/8)); EffektFX.Play(); }
         if (Line==13 && Sim.Options.OptionPlaneVolume!=Column-15) { Sim.Options.OptionPlaneVolume=Column-15; PlaneFX.SetVolume(Prozent2Dezibel(Sim.Options.OptionPlaneVolume*100/8)); PlaneFX.Play(); }
         RefreshKlackerField();
         KlackerTafel.Warp ();
      }
   }

   if (TimerFailure) KlackerTafel.Klack ();  //Tafel notfalls asynchron aktualisieren

   static SLONG cnt=0; cnt++;

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   if (bActive)
   {
      //Klacker-Felder:
      if (PageNum==5 || PageNum==6)
      {
         BOOL SavenameValid;

         for (py=63,y=0; y<16; y++, py+=22)
         {
            SavenameValid=TRUE;

            if (y>=2 && y<14 && !SavenamesValid[y-2] && CursorY!=y-2)
               SavenameValid = FALSE;

            for (x=0; x<24; x++)
               if (KlackerTafel.Haben[x+y*24])
                  RoomBm.BlitFrom (KlackerTafel.KlackerBms[(long)KlackerTafel.Haben[x+y*24]+(!SavenameValid)*(73+8+3+3)], 128+x*16, py);
         }
      }
      else
      {
         for (py=63,y=0; y<16; y++, py+=22)
         {
            for (x=0; x<24; x++)
               if (KlackerTafel.Haben[x+y*24])
                  RoomBm.BlitFrom (KlackerTafel.KlackerBms[(long)KlackerTafel.Haben[x+y*24]], 128+x*16, py);
         }
      }

      if (CursorY!=-1)
         RoomBm.BlitFromT (KlackerTafel.Cursors[long(BlinkState%8)], (CursorX+4)*16+129, (CursorY+1)*22+85);

      //Cursor highlighting:
      switch (PageNum)
      {
         case 1: //Startseite:
            if (Line==2 || Line==3 || Line==4) SetMouseLook (CURSOR_HOT, 0, -100, 0);

            if ((Sim.Gamestate & 31)==GAMESTATE_INIT)
            {
               if (Line==6) SetMouseLook (CURSOR_HOT, 0, -100, 0);
            }
            else
            {
               if (Line==6 || Line==7 || Line==9 || Line==11) SetMouseLook (CURSOR_HOT, 0, -100, 0);
            }
            break;

         case 2: //Grafik:
            if ((Line>=2 && Line<=8) || Line == 10 || Line == 11 || Line == 13) SetMouseLook (CURSOR_HOT, 0, -100, 0);
            break;

         case 3: //Sound:
            if (Column>=15 && Column<23 && Line>=3 && Line<=15)
            {
               if ((Line>=9 && Line<=13)|| Line==2) SetMouseLook (CURSOR_HOT, 0, -100, 0);
            }
            if (Line==2 || Line==7 || Line==4 || Line==15) SetMouseLook (CURSOR_HOT, 0, -100, 0);
            if (Line==5 && Sim.Options.OptionLoopMusik==0) SetMouseLook (CURSOR_HOT, 0, -100, 0);
            break;

         case 4: //Sonstiges:
            if ((Line>=2 && Line<=9) || Line==11) SetMouseLook (CURSOR_HOT, 0, -100, 0);
            break;

         case 5: //Laden
            if (Line>=2 && Line<=13)
            {
               if (SavegameInfos[Line-2].GetLength()>0 && !MenuIsOpen())
               {
                  SetMouseLook (CURSOR_HOT, 5000+Line, SavegameInfos[Line-2], ROOM_OPTIONS, 0);
                  if (ToolTipState==FALSE) ToolTipTimer=timeGetTime()-601;

                  if (Line!=LastLine) ToolTipState=FALSE;
               }
            }
            if (Line==15 && Column<10) SetMouseLook (CURSOR_HOT, 0, -100, 0);
            break;

         case 6: //Speichern
            if (Line>=2 && Line<=12 && CursorY==-1)
            {
               if (SavegameInfos[Line-2].GetLength()>0)
               {
                  SetMouseLook (CURSOR_HOT, 5000+Line, SavegameInfos[Line-2], ROOM_OPTIONS, 0);
                  if (ToolTipState==FALSE) ToolTipTimer=timeGetTime()-601;
               }
            }
            if (Line==15 && Column<10) SetMouseLook (CURSOR_HOT, 0, -100, 0);
            if (Line==15 && Column>=22 && Column<24 && CursorY!=-1) SetMouseLook (CURSOR_HOT, 0, -100, 0);
            break;

         case 7: //Quit:
            if (Line==4 || Line==5) SetMouseLook (CURSOR_HOT, 0, -100, 0);
            break;

         case 8: //Leave Game:
            if (Line==4 || Line==5) SetMouseLook (CURSOR_HOT, 0, -100, 0);
            break;
      }

      RoomBm.PrintAt (VersionString, FontSmallRed, TEC_FONT_RIGHT, XY(0,429), XY(519,480));
      gKlackerPlanes.PostPaint(RoomBm);

      if (CursorY!=-1)
         gKlackerPlanes.Pump(XY((CursorX+4)*16+129, (CursorY+1)*44+85));
      else
         gKlackerPlanes.Pump(gMousePosition);
   }

   LastLine=Line;

   CStdRaum::PostPaint ();
}

//--------------------------------------------------------------------------------------------
//void Options::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void Options::OnLButtonDown(UINT nFlags, CPoint point) 
{
   if (CursorY!=-1 && PageNum!=6) return;

   if (MenuIsOpen())
   {
      MenuRightClick (point);
   }
   else if (!PreLButtonDown (point))
   {
      ClickFx.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7);
      DefaultOnLButtonDown ();

      SLONG Line=(point.y-63)/22;
      SLONG Column=(point.x-128)/16;

      switch (PageNum)
      {
         case 1: //Startseite:
            if (Line==2) PageNum=2; //Grafik-Optionen
            if (Line==3) PageNum=3; //Musik-Optionen
            if (Line==4) PageNum=4; //Sonstiges

            if ((Sim.Gamestate & 31)==GAMESTATE_INIT)
            {
               if (Line==6)
               {
                  KlackerTafel.Warp (); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();
                  Sim.Gamestate = GAMESTATE_BOOT;
               }
            }
            else
            {
               if (Line==6) { UpdateSavegameNames (); PageNum=5; } //Speichern
               if (Line==7) { UpdateSavegameNames (); PageNum=6; } //Laden

               if (Line==9) PageNum=8; //New Game
               if (Line==11)
               {
                  KlackerTafel.Warp (); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();
                  Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
               }
            }

            RefreshKlackerField();
            break;

         case 2: //Grafik:
            if (Line==2)  Sim.Options.OptionPlanes^=1;
            if (Line==3) Sim.Options.OptionPassengers^=1;
            if (Line==4)
            {
               Sim.Options.OptionBlenden^=1;
               if (Sim.Options.OptionBlenden) 
               {
                  gBlendState=-2;
                  if (FrameWnd) FrameWnd->PrepareFade();
               }
            }
            if (Line==5) Sim.Options.OptionThinkBubbles^=1;
            if (Line==6) Sim.Options.OptionFlipping^=1;
            if (Line==7) Sim.Options.OptionTransparenz^=1;
            if (Line==8) Sim.Options.OptionSchatten^=1;

            if (Line == 10){
                ChangedDisplay = true;

                Sim.Options.OptionFullscreen++;
                if(Sim.Options.OptionFullscreen > 2)
                    Sim.Options.OptionFullscreen = 0;
            } //Fullscreen Option

            if (Line == 11) {
                Sim.Options.OptionKeepAspectRatio = !Sim.Options.OptionKeepAspectRatio;
                FrameWnd->UpdateFrameSize();
            } //Aspect Ratio Option

            if (Line==13) {
                if (ChangedDisplay)
                    FrameWnd->UpdateWindow();
                PageNum=1;
            }
            RefreshKlackerField();
            break;

         case 3: //Sound:
            if (Line==4)
            {
               Sim.Options.OptionLoopMusik=(Sim.Options.OptionLoopMusik+1)%(9);
               if (Sim.Options.OptionLoopMusik) NextMidi();
               RefreshKlackerField ();
            }

            if (Line==2)
            {
               Sim.Options.OptionEnableMidi^=1;
               if (Sim.Options.OptionEnableMidi && Sim.Options.OptionMusik) NextMidi (); else StopMidi ();

               RefreshKlackerField();
               KlackerTafel.Warp ();
            }
            else if (Line==7)
            {
               Sim.Options.OptionEnableDigi^=1;
               if (Sim.Options.OptionEnableDigi) gpSSE->EnableDS(); else gpSSE->DisableDS();
               RefreshKlackerField();
               KlackerTafel.Warp ();
            }

            if (Column>=15 && Column<23 && Line>=2 && Line<=13)
            {
               if (Line==9)  { Sim.Options.OptionAmbiente=Column-15; AmbientManager.RecalcVolumes(); AmbientFX.SetVolume(Prozent2Dezibel(Sim.Options.OptionAmbiente*100/8)); AmbientFX.Play(DSBPLAY_NOSTOP|DSBPLAY_LOOPING); }
               if (Line==10) { Sim.Options.OptionDurchsagen=Column-15; DurchsagenFX.SetVolume(Prozent2Dezibel(Sim.Options.OptionDurchsagen*100/8)); DurchsagenFX.Play(); }

#if !defined(NO_D_VOICES) || !defined(NO_E_VOICES) || !defined(NO_N_VOICES)
               if (bVoicesNotFound==false)
               {
                  if (Line==11) { Sim.Options.OptionTalking=Column-15; TalkFX.SetVolume(Prozent2Dezibel(Sim.Options.OptionTalking*100/8)); TalkFX.Play(); }
               }
#endif

               if (Line==12) { Sim.Options.OptionEffekte=Column-15; EffektFX.SetVolume(Prozent2Dezibel(Sim.Options.OptionEffekte*100/8)); EffektFX.Play(); }
               if (Line==13) { Sim.Options.OptionPlaneVolume=Column-15; PlaneFX.SetVolume(Prozent2Dezibel(Sim.Options.OptionPlaneVolume*100/8)); PlaneFX.Play(); }
               RefreshKlackerField();
               KlackerTafel.Warp ();
            }

            if (Line==5)  { if (Sim.Options.OptionLoopMusik==0) NextMidi(); } //Skip
            if (Line==15) { PageNum=1; AmbientFX.Stop(); } //Back
            RefreshKlackerField();
            break;

         case 4: //Sonstiges:
            if (Line==2)  Sim.Options.OptionGirl^=1;
            if (Line==3) Sim.Options.OptionBerater^=1;
            if (Line==4) Sim.Options.OptionAutosave^=1;
            if (Line==5) Sim.Options.OptionFax^=1;
            if (Line==6) { Sim.Options.OptionRealKuerzel^=1; Cities.UseRealKuerzel (Sim.Options.OptionRealKuerzel); }
            if (Line==7) Sim.Options.OptionSpeechBubble^=1;
            if (Line==8) Sim.Options.OptionBriefBriefing^=1;
            if (Line==9) Sim.Options.OptionRandomStartday^=1;
            if (Line==11) PageNum=1;
            RefreshKlackerField();
            break;

         case 5: //Laden:
            if (Line==15)
            {
               if (NewgameWantsToLoad)
               {
                  gNetworkSavegameLoading = -1;
                  KlackerTafel.Warp (); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();
                  Sim.Gamestate = GAMESTATE_BOOT;
               }
               else if (OptionsShortcut)
               {
                  KlackerTafel.Warp (); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();
                  Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
               }
               else PageNum=1;
            }
            if (Line>=2 && Line<=13 && SavegameInfos[Line-2].GetLength()>0)
            {
               if (NewgameWantsToLoad==2)
               {
                  //Netzwerk-Laden vom Hauptmenü aus:
                  gNetworkSavegameLoading = Line-2;
                  KlackerTafel.Warp (); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();
                  Sim.Gamestate = GAMESTATE_BOOT;
               }
               else if (Sim.bNetwork)
               {
                  //Laden während des Spiels: Kommt das mit der aktuellen Anzahl der Spieler hin?
                  if (Sim.GetSavegameNumHumans(Line-2)!=Sim.Players.GetAnzHumanPlayers())
                  {
                     if (Sim.Players.Players[Sim.localPlayer].LocationWin)
                        ((CStdRaum*)Sim.Players.Players[Sim.localPlayer].LocationWin)->MenuStart (MENU_REQUEST, MENU_REQUEST_NET_NUM);
                  }
                  else
                  {
                     //Laden im Netzwerk? Erst einmal bei den anderen Spielern nachfragen, ob das geht!
                     for (SLONG c=0; c<4; c++)
                        Sim.Players.Players[c].bReadyForBriefing=false;

                     nOptionsOpen++;
                     Sim.SendSimpleMessage (ATNET_OPTIONS, NULL, 1, Sim.localPlayer);
                     Sim.SendSimpleMessage (ATNET_IO_LOADREQUEST, NULL, Sim.localPlayer, Line-2, Sim.GetSavegameUniqueGameId(Line-2, true));
                  }
               }
               else
               {
                  Sim.LoadGame (Line-2);
                  return;
               }
            }
            RefreshKlackerField();
            break;

         case 6: //Speichern:
            if (Line==15 && Column<10)
            {
               if (OptionsShortcut)
               {
                  KlackerTafel.Warp (); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();
                  Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
               }
               else
               {
                  PageNum=1; CursorY=-1;
               }
            }
            if (Line>=2 && Line<=12 && CursorY==-1) { gDisablePauseKey=TRUE; CursorX=0; CursorY=Line-2; }
            if (Line==15 && Column>=22 && Column<24 && CursorY!=-1)
            {
               CursorX=0;
               gDisablePauseKey=FALSE;

               if (Sim.bNetwork)
               {
                  Sim.UniqueGameId=((timeGetTime () ^ DWORD(rand()%30000) ^ gMousePosition.x ^ gMousePosition.y)&0x7fffffff);
                  Sim.Players.Players[(SLONG)PlayerNum].NetSave (Sim.UniqueGameId, CursorY,  (LPCTSTR)SavegameNames[CursorY]);
               }
               Sim.SaveGame (CursorY,  (LPCTSTR)SavegameNames[CursorY]);

               if ((Sim.Gamestate & 31)==GAMESTATE_INIT)
               {
                  Sim.Gamestate = GAMESTATE_BOOT;
               }
               else
               {
                  if ((Sim.Gamestate & 31)==GAMESTATE_INIT)
                  {
                     Sim.Gamestate = GAMESTATE_BOOT;
                     //PostMessage(WM_CLOSE);
                  }
                  else
                  {
                     Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
                  }
               }
            }
            RefreshKlackerField();
            break;

         case 7: //Quit:
            if (Line==4)
            {
               bLeaveGameLoop=1;
            }
            if (Line==5) { PageNum=1; RefreshKlackerField(); }
            break;

         case 8: //Leave Game:
            if (Line==4)
            {
               if (Sim.bNetwork)
               {
                  Sim.SendChatBroadcast (bprintf (StandardTexte.GetS (TOKEN_MISC, 7022), (LPCSTR)Sim.Players.Players[Sim.localPlayer].NameX));
                  gNetwork.DisConnect ();
                  Sim.bNetwork = false;
               }

               nOptionsOpen--;

               Sim.Gamestate = GAMESTATE_BOOT;
               if (NewgameToOptions) KeepRoomLib ();
            }
            if (Line==5) { PageNum=1; RefreshKlackerField(); }
            break;
      }
   }
}

//--------------------------------------------------------------------------------------------
// void Options::OnRButtonDown(UINT nFlags, CPoint point):
//--------------------------------------------------------------------------------------------
void Options::OnRButtonDown(UINT, CPoint point) 
{
   DefaultOnRButtonDown ();

   if (MenuIsOpen())
   {
      MenuRightClick (point);
   }
   else
   {
      AmbientFX.Stop();

      if (CursorY!=-1)
      {
         gDisablePauseKey=FALSE;
         UpdateSavegameNames ();
         RefreshKlackerField();
         CursorY=-1;
         return;
      }
      else if (PageNum==1)
      {
         if ((Sim.Gamestate & 31)==GAMESTATE_INIT)
         {
            KlackerTafel.Warp (); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();
            Sim.Gamestate = GAMESTATE_BOOT;
         }
         else
         {
            KlackerTafel.Warp (); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();
            Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
         }
      }
      else
      {
         if (PageNum==5 && NewgameWantsToLoad)
         {
            KlackerTafel.Warp (); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();
            Sim.Gamestate = GAMESTATE_BOOT;
         }
         else if (OptionsShortcut && (PageNum==5 || PageNum==6))
         {
            KlackerTafel.Warp (); FrameWnd->Invalidate(); MessagePump(); FrameWnd->Invalidate(); MessagePump();
            Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
         }
         else
         {
            PageNum=1;
            RefreshKlackerField();
         }
      }
   }
}

//--------------------------------------------------------------------------------------------
// void Options::OnTimer(UINT nIDEvent):
//--------------------------------------------------------------------------------------------
void Options::OnTimer(UINT nIDEvent) 
{
   //Mit 10 FPS die Anzeige rotieren lassen:
   if (nIDEvent==1) KlackerTafel.Klack();

   BlinkState++;
}

//--------------------------------------------------------------------------------------------
//void Options::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
//--------------------------------------------------------------------------------------------
void Options::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   if (CursorY!=-1)
   {
      if (nChar>='a' && nChar<='z') nChar=toupper(nChar);
      if (nChar==196 || nChar==228) nChar=(UINT)'Ä';
      if (nChar==214 || nChar==246) nChar=(UINT)'Ö';
      if (nChar==220 || nChar==252) nChar=(UINT)'Ü';
      if (nChar==' ' || nChar=='-' || nChar=='+' || nChar=='.' || (nChar>='A' && nChar<='Z') || nChar=='Ä' || nChar=='Ö' || nChar=='Ü' || (nChar>='0' && nChar<='9'))
      {
         if (!SavenamesValid[CursorY] && strncmp(SavegameNames[CursorY],StandardTexte.GetS (TOKEN_MISC, 4073),6)==0)
            SavegameNames[CursorY] = "      ";

         while (SavegameNames[CursorY].GetLength()<CursorX+1) SavegameNames[CursorY]+=" ";
         SavegameNames[CursorY].SetAt (CursorX, UBYTE(nChar));
         RefreshKlackerField();

         if (CursorX<19) CursorX++;
      }

      if (nChar==VK_RETURN)
      {
         CursorX=0;
         gDisablePauseKey=FALSE;

         if (Sim.bNetwork)
         {
            Sim.UniqueGameId=((timeGetTime () ^ DWORD(rand()%30000) ^ gMousePosition.x ^ gMousePosition.y)&0x7fffffff);
            Sim.Players.Players[(SLONG)PlayerNum].NetSave (Sim.UniqueGameId, CursorY,  (LPCTSTR)SavegameNames[CursorY]);
         }
         Sim.SaveGame (CursorY,  (LPCTSTR)SavegameNames[CursorY]);

         if ((Sim.Gamestate & 31)==GAMESTATE_INIT)
         {
            Sim.Gamestate = GAMESTATE_BOOT;
         }
         else
         {
            if ((Sim.Gamestate & 31)==GAMESTATE_INIT)
            {
               Sim.Gamestate = GAMESTATE_BOOT;
            }
            else
            {
               Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
            }
         }
      }
   }
}

//--------------------------------------------------------------------------------------------
//void Options::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//--------------------------------------------------------------------------------------------
void Options::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   if (CursorY!=-1)
   {
      if (nChar==VK_LEFT && CursorX>0)   CursorX--;
      if (nChar==VK_RIGHT && CursorX<19) CursorX++;

      if (nChar==VK_BACK)
      {
         while (SavegameNames[CursorY].GetLength()<CursorX+1) SavegameNames[CursorY]+=" ";
         SavegameNames[CursorY].SetAt (CursorX, ' ');
         RefreshKlackerField();

         if (CursorX>0) CursorX--;
      }

      if (nChar==VK_ESCAPE)
      {
         gDisablePauseKey=FALSE;
         UpdateSavegameNames ();
         RefreshKlackerField();
         CursorY=-1;
      }

   }
   else
   {
      if (nChar==VK_F3) { UpdateSavegameNames (); PageNum=5; RefreshKlackerField(); }
      if (nChar==VK_F4) { UpdateSavegameNames (); PageNum=6; RefreshKlackerField(); }

      if (nChar==VK_ESCAPE) Sim.Players.Players[Sim.localPlayer].LeaveRoom ();
   }
}