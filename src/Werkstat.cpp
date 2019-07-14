//============================================================================================
// CWerkstat.cpp : Hier werden die Flugzeuge repariert
//============================================================================================
// Link: "Werkstat.h"
//============================================================================================
#include "stdafx.h"
#include "glwerk.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Zum debuggen:
static const char FileId[] = "Werk";

static XY SlimePositions[] = { XY(93,441), XY(111,418), XY(137,430), XY(54,427), XY(14,427) };

//--------------------------------------------------------------------------------------------
//Eine Animation initialisieren:
// * GfxLib  *gfxLib       Grafikbibliothek für die Animation
// * CString &graphicIDs   Name der Startphase
// * SLONG    Anzahl       Anzahl der Bilder in der Animation
// * SBFX    *Fx           Effekt, der beim Abspielen gespielt wird
// * BOOL     StartPic     Soll beim warten auf den Animationsbeginn ein Startbild gezeigt werden?
// * SLONG    Mode         ONCE/REPEAT/RANDOM PHASE
// * SLONG    Prelude      Soviel x/20 Sekunden bis zum Animationsbeginn warten
// * SLONG    Speed        Geschwindigkeit der Animation in x/20 Sekunden pro Frame
// * SLONG    CyclycWait   (optional) Wartepausen zwischen Folgedurchgängen der Animation
// * SLONG    RepeatCore   (optional) Sooft wird die Animation am Stück sofort wiederholt
//--------------------------------------------------------------------------------------------
void CAnimation::ReSize (GfxLib *gfxLib, const CString &graphicIDs, SLONG Anzahl, SBFX *Fx, BOOL StartPic, SLONG Mode, SLONG Prelude, SLONG Speed, SLONG CyclycWait, SLONG RepeatCore)
{
   CAnimation::Frames.ReSize (gfxLib, graphicIDs, Anzahl);
   CAnimation::pSoundFx     = Fx;
   CAnimation::Mode         = Mode;                         //ANIMATION_MODE_ONCE / _REPEAT / _RANDOM
   CAnimation::StartPic     = StartPic;                     //Bild zeigen, wenn Animation noch nicht begonnen?
   CAnimation::Prelude      = Prelude;                      //Wartezeit bis zur ersten Animation
   CAnimation::CounterStart = Sim.TickerTime+Prelude;
   CAnimation::Speed        = Speed;
   CAnimation::CyclycWait   = CyclycWait;                   //Wartezeit nach jedem kompletten Animationsdurchlauf
   CAnimation::RepeatCore   = RepeatCore;

   CAnimation::JustPlayed   = FALSE;
}

//--------------------------------------------------------------------------------------------
//Setzt die Animation zurück (z.B. für den Bildschirmschoner)
//--------------------------------------------------------------------------------------------
void CAnimation::Reset (void)
{
   CAnimation::CounterStart = Sim.TickerTime+Prelude;
}

//--------------------------------------------------------------------------------------------
//Gibt den aktuell gezeigten Frame zurück:
//--------------------------------------------------------------------------------------------
SLONG CAnimation::GetFrame (void)
{
   if (Frames.AnzEntries())
   {
      if (CAnimation::Mode==ANIMATION_MODE_NEVER)
      {
         if (StartPic) return (0);
      }
      else
      {
         SLONG Phase = (SLONG(Sim.TickerTime) - CounterStart)/Speed;

         if (Phase<0) return (0);

         if (Mode==ANIMATION_MODE_RANDOM)    Phase  = SLONG(((sin (Phase)+1)/2.001)*Frames.AnzEntries());

         if (Phase>=Frames.AnzEntries()*RepeatCore)
         {
            if (Mode==ANIMATION_MODE_REPEAT && CyclycWait)
               return (0);

            if (Mode==ANIMATION_MODE_REPEAT) Phase %= Frames.AnzEntries();
            if (Mode==ANIMATION_MODE_ONCE)   Phase  = Frames.AnzEntries()-1;
         }

         return (Phase%Frames.AnzEntries());
      }
   }
   return (0);
}

//--------------------------------------------------------------------------------------------
//Entfernt die Animation vom Bildschirm:
//--------------------------------------------------------------------------------------------
void CAnimation::Remove (void)
{
   if (pSoundFx) pSoundFx->Stop();
   pSoundFx=NULL;
   Frames.Destroy();
}

//--------------------------------------------------------------------------------------------
//Startet einen Animation JETZT:
//--------------------------------------------------------------------------------------------
void CAnimation::StartNow (void)
{
   Mode         = ANIMATION_MODE_ONCE;
   CounterStart = Sim.TickerTime+Prelude;
}

//--------------------------------------------------------------------------------------------
//Blittet eine Phase an eine bestimmte Stelle:
//--------------------------------------------------------------------------------------------
void CAnimation::BlitAt (SBBM &RoomBm, SLONG x, SLONG y)
{
   if (Frames.AnzEntries())
   {
      if (CAnimation::Mode==ANIMATION_MODE_NEVER)
      {
         if (StartPic) RoomBm.BlitFrom (Frames[0], x, y);
      }
      else
      {
         SLONG Phase = (SLONG(Sim.TickerTime) - CounterStart)/Speed;

         if (Phase<0)
         {
            if (!StartPic) return;
            Phase=0;
         }

         if (Mode==ANIMATION_MODE_RANDOM)    Phase  = SLONG(((sin (Phase)+1)/2.001)*Frames.AnzEntries());

         if (Phase>=Frames.AnzEntries()*RepeatCore)
         {
            if (Mode==ANIMATION_MODE_REPEAT && CyclycWait)
            {
               CounterStart=Sim.TickerTime+CyclycWait;
               if (!StartPic) return;
               Phase=0;
            }

            if (Mode==ANIMATION_MODE_REPEAT) Phase %= Frames.AnzEntries();
            if (Mode==ANIMATION_MODE_ONCE)   Phase  = Frames.AnzEntries()-1;
         }

         if (Mode==ANIMATION_MODE_REPEAT && Prelude==0 && !JustPlayed)
         {
            if (pSoundFx && Sim.Options.OptionDigiSound)
            {
               pSoundFx->Play(DSBPLAY_NOSTOP|DSBPLAY_LOOPING, Sim.Options.OptionEffekte*100/7);
               JustPlayed=TRUE;
            }
         }
         else
         {
            if (pSoundFx && Sim.Options.OptionDigiSound && !JustPlayed && Phase%Frames.AnzEntries()==0)
            {
               pSoundFx->Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7);
               JustPlayed = TRUE;
            }
            else JustPlayed=FALSE;
         }

         RoomBm.BlitFrom (Frames[Phase%Frames.AnzEntries()], x, y);
      }
   }
}

//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
CAnimation::CAnimation()
{
   pSoundFx=NULL;
}

//--------------------------------------------------------------------------------------------
//Destruktor:
//--------------------------------------------------------------------------------------------
CAnimation::~CAnimation()
{
   if (pSoundFx) pSoundFx->Stop();
}

//--------------------------------------------------------------------------------------------
//Blittet eine Phase an eine bestimmte Stelle:
//--------------------------------------------------------------------------------------------
void CAnimation::BlitAtT (SBBM &RoomBm, SLONG x, SLONG y)
{
   if (Frames.AnzEntries())
   {
      SLONG Phase = (SLONG(Sim.TickerTime) - CounterStart)/Speed;

      if (Phase<0)
      {
         if (!StartPic) return;
         Phase=0;
      }

      if (Mode==ANIMATION_MODE_RANDOM)    Phase  = SLONG(((sin (Phase)+1)/2.001)*Frames.AnzEntries());

      if (Phase>=Frames.AnzEntries()*RepeatCore)
      {
         if (Mode==ANIMATION_MODE_REPEAT && CyclycWait)
         {
            CounterStart=Sim.TickerTime+CyclycWait;
            if (!StartPic) return;
            Phase=0;
         }

         if (Mode==ANIMATION_MODE_REPEAT) Phase %= Frames.AnzEntries();
         if (Mode==ANIMATION_MODE_ONCE)   Phase  = Frames.AnzEntries()-1;
      }

      if (Mode==ANIMATION_MODE_REPEAT && Prelude==0 && !JustPlayed)
      {
         if (pSoundFx && Sim.Options.OptionDigiSound)
         {
            pSoundFx->Play(DSBPLAY_NOSTOP|DSBPLAY_LOOPING, Sim.Options.OptionEffekte*100/7);
            JustPlayed=TRUE;
         }
      }
      else
      {
         if (pSoundFx && Sim.Options.OptionDigiSound && !JustPlayed && Phase%Frames.AnzEntries()==0)
         {
            pSoundFx->Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7);
            JustPlayed = TRUE;
         }
         else JustPlayed=FALSE;
      }

      RoomBm.BlitFromT (Frames[Phase%Frames.AnzEntries()], x, y);
   }
}

//--------------------------------------------------------------------------------------------
//Die Schalter wird eröffnet:
//--------------------------------------------------------------------------------------------
CWerkstatt::CWerkstatt(BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "werkstat.gli", GFX_WERKSTAT)
{
   SetRoomVisited (PlayerNum, ROOM_WERKSTATT);

   Sim.FocusPerson=-1;

   if (!bHandy) AmbientManager.SetGlobalVolume (40);

   KommVar=-1;

   Talkers.Talkers[TALKER_MECHANIKER].IncreaseReference ();
   DefaultDialogPartner=TALKER_MECHANIKER;

   OilCanBm.ReSize (pRoomLib, GFX_OIL);
   DoorBm.ReSize (pRoomLib, GFX_DOOR);

   SawFx.ReInit ("saw.raw");

   SP_Mann.ReSize (9);
   //--------------------------------------------------------------------------------------------
   SP_Mann.Clips[0].ReSize (0, "WK_Wait.smk", "", XY (190, 209), SPM_IDLE,       CRepeat(1,1), CPostWait(10,10), SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                            "A9A1E1", 0, 4, 1);

   SP_Mann.Clips[4].ReSize (4, "WK_Tuch.smk", "", XY (190, 209), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),  SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                            "A9", 5);
   SP_Mann.Clips[5].ReSize (5, "WK_Putz.smk", "", XY (190, 209), SPM_IDLE,       CRepeat(1,3), CPostWait(0,0),  SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                            "A9", 6);
   SP_Mann.Clips[6].ReSize (6, "WK_TuchZ.smk", "", XY (190, 209), SPM_IDLE,      CRepeat(1,1), CPostWait(0,0),  SMACKER_CLIP_DONTCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                            "A9", 0);

   SP_Mann.Clips[1].ReSize (1, "WK_Turn.smk", "", XY (190, 209), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,
                            "E1E1", 2, 3);

   SP_Mann.Clips[2].ReSize (2, "WK_Rede.smk", "", XY (190, 209), SPM_TALKING,    CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,
                            "A9E1E1", 2, 3, 7);
   SP_Mann.Clips[3].ReSize (3, "WK_RedeW.smk", "", XY (190, 209), SPM_LISTENING, CRepeat(1,1), CPostWait(10,10), SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                            "A9E1E1A3", 3, 2, 7, 8);
   SP_Mann.Clips[8].ReSize (8, "WK_Rauch.smk", "", XY (173, 209), SPM_LISTENING, CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            &KommVar, SMACKER_CLIP_SET|SMACKER_CLIP_FRAME+18*SMACKER_CLIP_MULT, 1, NULL,  //Warten
                            "A9E1E1", 3, 2, 7);

   SP_Mann.Clips[7].ReSize (7, "WK_TurnZ.smk", "", XY (190, 209), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),  SMACKER_CLIP_DONTCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                            "A9", 0);

   SP_Wer.ReSize(4);
   //--------------------------------------------------------------------------------------------
   SP_Wer.Clips[0].ReSize (0, "WerStand.smk", "", XY (535, 72), SPM_IDLE,       CRepeat(1,1), CPostWait(100,210), SMACKER_CLIP_DONTCANCEL,
                           NULL, SMACKER_CLIP_SET, 0, NULL,
                           "A9", 1);
   SP_Wer.Clips[1].ReSize (1, "WerL.smk",     "", XY (535, 72), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),     SMACKER_CLIP_DONTCANCEL,
                           NULL, SMACKER_CLIP_SET, 0, NULL,
                           "A9", 2);
   SP_Wer.Clips[2].ReSize (2, "WerStand.smk", "", XY (535, 72), SPM_IDLE,       CRepeat(1,1), CPostWait(100,210), SMACKER_CLIP_DONTCANCEL,
                           NULL, SMACKER_CLIP_SET, 0, NULL,
                           "A9", 3);
   SP_Wer.Clips[3].ReSize (3, "WerR.smk",     "", XY (535, 72), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),     SMACKER_CLIP_DONTCANCEL,
                           NULL, SMACKER_CLIP_SET, 0, NULL,
                           "A9", 0);

   SP_Schleim.ReSize(3);
   //--------------------------------------------------------------------------------------------
   if (Sim.Slimed==-1)
   {
      if (rand()%10==0 || CheatAnimNow)
         SP_Schleim.Clips[0].ReSize (0, "SLIMEW.smk", "", XY (0, 275), SPM_IDLE,       CRepeat(1,1), CPostWait(30,30),    SMACKER_CLIP_CANCANCEL,
                                     NULL, SMACKER_CLIP_SET, 0, &Sim.Slimed,
                                     "A8A1", 0, 1);
      else
         SP_Schleim.Clips[0].ReSize (0, "SLIMEW.smk", "", XY (0, 275), SPM_IDLE,       CRepeat(1,1), CPostWait(30,30),    SMACKER_CLIP_CANCANCEL,
                                     NULL, SMACKER_CLIP_SET, 0, &Sim.Slimed,
                                     "A4", 0);
   }
   else
   {
      SP_Schleim.Clips[0].ReSize (0, "SLIME02.smk", "", XY (0, 275), SPM_IDLE,       CRepeat(99,99), CPostWait(0,0),     SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A9", 2);
   }
   SP_Schleim.Clips[1].ReSize (1, "SLIME01.smk", "SLIME01.raw", XY (0, 275), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),     SMACKER_CLIP_DONTCANCEL,
                               &Sim.Slimed, SMACKER_CLIP_SET|SMACKER_CLIP_POST, 2, NULL,
                               "A9", 2);
   SP_Schleim.Clips[2].ReSize (2, "SLIME02.smk", "", XY (0, 275), SPM_IDLE,       CRepeat(99,99), CPostWait(0,0),     SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A9", 2);

   for (SLONG c=0; c<5; c++)
   {
      SP_Blase[c].ReSize(3);
      SP_Blase[c].Clips[0].ReSize (0, "BLASEW.smk", "", SlimePositions[c], SPM_IDLE,   CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_CANCANCEL,
                                   NULL, SMACKER_CLIP_SET, 0, &Sim.Slimed,
                                   "A9", 0);
      SP_Blase[c].Clips[1].ReSize (1, bprintf ("BLASE%02li.smk", c%3+1), "", SlimePositions[c], SPM_IDLE,  CRepeat(1,1), CPostWait(0,0),     SMACKER_CLIP_DONTCANCEL,
                                   NULL, SMACKER_CLIP_SET, 0, NULL,
                                   "A9", 0);
      SP_Blase[c].Clips[2].ReSize (2, "BLASEW.smk", "", SlimePositions[c],  SPM_IDLE,  CRepeat(1,1), CPostWait(30+rand()%80,130+rand()%80),   SMACKER_CLIP_DONTCANCEL,
                                   NULL, SMACKER_CLIP_SET, 0, NULL,
                                   "A9", 1);
   }

   SP_Bombe.ReSize(2);
   //--------------------------------------------------------------------------------------------
   SP_Bombe.Clips[0].ReSize (0, "BombeW.smk", "", XY (180, 386), SPM_IDLE, CRepeat(1,1), CPostWait(100,210), SMACKER_CLIP_CANCANCEL,
                             NULL, SMACKER_CLIP_SET, 0, &KommVar,
                             "A9", 0);
   SP_Bombe.Clips[1].ReSize (1, "Bombe.smk", "Bombe.raw", XY (180, 386), SPM_IDLE, CRepeat(1,1), CPostWait(0,0),     SMACKER_CLIP_DONTCANCEL,
                             &KommVar, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, NULL,
                             "A9", 0);

   SP_Hund.ReSize(1);
   //--------------------------------------------------------------------------------------------
   SP_Hund.Clips[0].ReSize (0, "Spike.smk", "", XY (529, 115), SPM_IDLE, CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,
                            "A9", 0);

   //Hintergrundsounds:
   if (Sim.Options.OptionEffekte)
   {
      SetBackgroundFx (1, "crash.raw",   100000, 10000);
      SetBackgroundFx (2, "crash2.raw",   90000, 40000);
      SetBackgroundFx (3, "crash3.raw",  120000, 75000);
   }

   //Raumanimationen
   if (rand()%6==0 || CheatAnimNow) SoudAnim.ReSize (pRoomLib, "SOUD0000", 21, &SawFx, FALSE, ANIMATION_MODE_REPEAT,  150,  2, 100);
   FlameAnim.ReSize (pRoomLib, "FLAME01",  3, NULL, TRUE, ANIMATION_MODE_REPEAT,   0,  1);
   LightAnim.ReSize (pRoomLib, "LIGHT01",  2, NULL, FALSE, ANIMATION_MODE_REPEAT, 400, 2, 300, 30);

   #ifdef DEMO
      MenuStart (MENU_REQUEST, MENU_REQUEST_NO_WERKSTATT);
      MenuSetZoomStuff (XY(320,220), 0.17, FALSE);
   #endif

   ShowWindow(SW_SHOW);
   UpdateWindow();
}

//--------------------------------------------------------------------------------------------
//Sehr destruktiv!
//--------------------------------------------------------------------------------------------
CWerkstatt::~CWerkstatt()
{
   Talkers.Talkers[TALKER_MECHANIKER].DecreaseReference ();
}

//--------------------------------------------------------------------------------------------
//Die Bank wird eröffnet:
//--------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CWerkstatt, CStdRaum)
	//{{AFX_MSG_MAP(CWerkstatt)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////////////////////
// CWerkstatt message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//void CWerkstatt::OnPaint()
//--------------------------------------------------------------------------------------------
void CWerkstatt::OnPaint()
{
   SLONG NewTip;

   { CPaintDC dc(this); }

   if (!bHandy) SetMouseLook (CURSOR_NORMAL, 0, ROOM_ARAB_AIR, 0);

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   //Der Mann im Flugzeug:
   if (Sim.Players.Players[(SLONG)PlayerNum].SecurityFlags&(1<<7))
   {
      SP_Hund.Pump ();
      SP_Hund.BlitAtT (RoomBm);
   }
   else
   {
      SP_Wer.Pump ();
      SP_Wer.BlitAtT (RoomBm);
   }

   SP_Schleim.Pump ();
   SP_Schleim.BlitAtT (RoomBm);

   if (Sim.Slimed!=-1)
      for (SLONG c=0; c<5; c++)
      {
         SP_Blase[c].Pump ();
         SP_Blase[c].BlitAtT (RoomBm);
      }

   if (!(Sim.Players.Players[(SLONG)PlayerNum].SecurityFlags&(1<<7)))
      RoomBm.BlitFromT (DoorBm, 534, 73);

   if (!Sim.Players.Players[(SLONG)PlayerNum].HasItem(ITEM_OEL))
      RoomBm.BlitFrom (OilCanBm, 72,294);

   if (!IsDialogOpen() && !MenuIsOpen())
   {
      if (gMousePosition.IfIsWithin (0, 0, 50, 439)) SetMouseLook (CURSOR_EXIT, 0, ROOM_WERKSTATT, 999);
      else if (gMousePosition.IfIsWithin (195,212,263,437)) SetMouseLook (CURSOR_HOT, 0, ROOM_WERKSTATT, 10);
      else if (!Sim.Players.Players[(SLONG)PlayerNum].HasItem(ITEM_OEL) && gMousePosition.IfIsWithin (72,294,101,315)) SetMouseLook (CURSOR_HOT, 0, ROOM_WERKSTATT, 20);
   }

   SP_Mann.Pump ();
   SP_Mann.BlitAtT (RoomBm);
   if (Sim.Slimed==-1) KommVar=-1;

   SP_Bombe.Pump ();
   SP_Bombe.BlitAtT (RoomBm);

   //Die Raum-Animationen:
   SoudAnim.BlitAt  (RoomBm, 470, 290);
   FlameAnim.BlitAt (RoomBm, 104, 314);
   LightAnim.BlitAtT (RoomBm, 244, 89);

   if (MenuIsOpen())
   {
      if (CurrentMenu==MENU_PLANECOSTS && (gMousePosition-MenuPos).IfIsWithin (216,6, 387,212))
      {
         NewTip = (gMousePosition.y-(MenuPos.y+25))/13 + MenuPage;

         if (NewTip>=0 && NewTip-MenuPage<13 && NewTip<MenuDataTable.LineIndex.AnzEntries() && Sim.Players.Players[(SLONG)PlayerNum].Planes.IsInAlbum (MenuDataTable.LineIndex[NewTip]))
         {
            if (NewTip != CurrentTip)
            {
               MenuRepaint ();
               //DrawPlaneTipContents (OnscreenBitmap, &PlaneTypes[Sim.Players.Players[(SLONG)PlayerNum].Planes[MenuDataTable.LineIndex[NewTip]].TypeId], &Sim.Players.Players[(SLONG)PlayerNum].Planes[MenuDataTable.LineIndex[NewTip]],
               DrawPlaneTipContents (OnscreenBitmap, NULL, &Sim.Players.Players[(SLONG)PlayerNum].Planes[MenuDataTable.LineIndex[NewTip]],
                                     XY(6,6), XY(6,28), &FontSmallBlack, &FontSmallBlack, TRUE);
            }

            if (MenuDataTable.ValueFlags[0+NewTip*MenuDataTable.AnzColums])
               CheckCursorHighlight (ReferenceCursorPos, CRect (MenuPos.x+216, MenuPos.y+(NewTip-MenuPage)*13+25-2, MenuPos.x+387, MenuPos.y+(NewTip-MenuPage)*13+25+12), ColorOfFontRed, CURSOR_HOT);
            else
               CheckCursorHighlight (ReferenceCursorPos, CRect (MenuPos.x+216, MenuPos.y+(NewTip-MenuPage)*13+25-2, MenuPos.x+387, MenuPos.y+(NewTip-MenuPage)*13+25+12), ColorOfFontBlack, CURSOR_HOT);

            CurrentTip = NewTip;
         }
         else NewTip = -1;
      }
      else NewTip = -1;

      if (NewTip != CurrentTip)
      {
         MenuRepaint ();
         CurrentTip = NewTip;
      }
   }

   #ifdef DEMO
      if (!IsDialogOpen() && !MenuIsOpen()) SetMouseLook (CURSOR_EXIT, 0, ROOM_WERKSTATT, 999);
   #endif

   CStdRaum::PostPaint ();
   CStdRaum::PumpToolTips ();
}

//--------------------------------------------------------------------------------------------
//void CWerkstatt::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CWerkstatt::OnLButtonDown(UINT nFlags, CPoint point)
{
   XY      RoomPos;
   PLAYER &qPlayer = Sim.Players.Players[(SLONG)PlayerNum];

   DefaultOnLButtonDown ();

   if (!ConvertMousePosition (point, &RoomPos))
   {
	   CStdRaum::OnLButtonDown(nFlags, point);
      return;
   }

   if (!PreLButtonDown (point))
   {
      if (MouseClickArea==ROOM_WERKSTATT && MouseClickId==999) Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
      else if (MouseClickArea==ROOM_WERKSTATT && MouseClickId==10) StartDialog (TALKER_MECHANIKER, MEDIUM_AIR, 1);
      else if (MouseClickArea==ROOM_WERKSTATT && MouseClickId==20)
      {
         if (qPlayer.MechTrust==0)
            StartDialog (TALKER_MECHANIKER, MEDIUM_AIR, 20);
         else
         {
            if (qPlayer.HasSpaceForItem())
            {
               StartDialog (TALKER_MECHANIKER, MEDIUM_AIR, 21);
               qPlayer.BuyItem(ITEM_OEL);
            }
         }
      }
      else CStdRaum::OnLButtonDown(nFlags, point);
   }
}

//--------------------------------------------------------------------------------------------
//void CWerkstatt::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CWerkstatt::OnRButtonDown(UINT nFlags, CPoint point)
{
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
