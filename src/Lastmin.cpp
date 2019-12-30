//============================================================================================
// LastMin.cpp : Der Last-Minute Schalter
//============================================================================================
// Link: "LastMin.h"
//============================================================================================
#include "stdafx.h"
#include "gllast.h"
#include "AtNet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char FileId[] = "Last";

static const SLONG ZettelPos [10*2]=
   {
       66,  79,      126, 212,
      145, 116,       33, 250,
      128, 289,       15, 150,
      346, 331,       40, 113,
      449, 126,      245, 307
   };

extern SLONG timeLastClose;

//////////////////////////////////////////////////////////////////////////////////////////////
// Museum Konstruktion, Initialisation, Destruction
//////////////////////////////////////////////////////////////////////////////////////////////

CString ShortenLongCities (CString City);

//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
CLastMinute::CLastMinute(BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "lastmin.gli", GFX_LASTMIN)
{
   SLONG c;

   SetRoomVisited (PlayerNum, ROOM_LAST_MINUTE);

   Sim.FocusPerson=-1;

   if (!bHandy) AmbientManager.SetGlobalVolume (60);
   Sim.NetRefill (1);
   LastMinuteAuftraege.RefillForLastMinute ();

   MoveKran=0;
   NewKranDir=-1;
   KranArrived=-1;

   SP_Girl.ReSize (7);
   SP_Girl.Clips[0].ReSize (0, "LMWait.smk", "", XY (333, 225), SPM_IDLE,        CRepeat(1,1), CPostWait(25,60), SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A9A2A4A4A8", 0, 6, 1, 2, 5);
   SP_Girl.Clips[1].ReSize (1, "LMGrins.smk", "", XY (333, 225), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A1", 0);
   SP_Girl.Clips[2].ReSize (2, "LMKnopf.smk", "Kran1.raw", XY (333, 225), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A1", 3);
   SP_Girl.Clips[3].ReSize (3, "LMKnopfW.smk", "", XY (333, 225), SPM_IDLE,      CRepeat(1,1), CPostWait(9999,9999), SMACKER_CLIP_CANCANCEL,
                               &MoveKran, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, 1, &KranArrived,
                               "A1", 3);
   SP_Girl.Clips[4].ReSize (4, "LMKnopfZ.smk", "", XY (333, 225), SPM_IDLE,      CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                               &KranArrived, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, NULL,
                               "A1", 0);
   SP_Girl.Clips[5].ReSize (5, "LMTaste.smk", "", XY (333, 225), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                               &KommMonitor, SMACKER_CLIP_SET|SMACKER_CLIP_FRAME+4*SMACKER_CLIP_MULT, 0, NULL,
                               "A1", 0);
   SP_Girl.Clips[6].ReSize (6, "LMWaitK.smk", "", XY (333, 225), SPM_IDLE,        CRepeat(1,1), CPostWait(1,1), SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A9", 0);

   SP_Monitor.ReSize (3);
   //--------------------------------------------------------------------------------------------
   SP_Monitor.Clips[0].ReSize (0, "",  "",         XY (448, 318), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),     SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A5A5", 1, 2);
   SP_Monitor.Clips[1].ReSize (1, "Moni.smk", "",  XY (448, 318), SPM_IDLE,       CRepeat(1,1), CPostWait(999,999), SMACKER_CLIP_CANCANCEL,
                               &KommMonitor, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, &KommMonitor,
                               "A1", 0);
   SP_Monitor.Clips[2].ReSize (2, "MoniA.smk", "", XY (448, 318), SPM_IDLE,       CRepeat(1,1), CPostWait(999,999), SMACKER_CLIP_CANCANCEL,
                               &KommMonitor, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, &KommMonitor,
                               "A1", 0);

   SP_Kran.ReSize (4);
   SP_Kran.Clips[0].ReSize (0, "KranRS.smk", "", XY (300, 129), SPM_IDLE,        CRepeat(1,1), CPostWait(9990,9990), SMACKER_CLIP_CANCANCEL,
                               &NewKranDir, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, &NewKranDir,
                               "A9", 0);
   SP_Kran.Clips[1].ReSize (1, "KranL.smk", "Kran2.raw", XY (300, 129), SPM_IDLE,         CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                               &KranArrived, SMACKER_CLIP_SET|SMACKER_CLIP_FRAME+21*SMACKER_CLIP_MULT, 4, NULL,
                               "A1", 2);
   SP_Kran.Clips[2].ReSize (2, "KranLS.smk", "", XY (300, 129), SPM_IDLE,        CRepeat(1,1), CPostWait(9990,9990), SMACKER_CLIP_CANCANCEL,
                               &NewKranDir, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, &NewKranDir,
                               "A1", 2);
   SP_Kran.Clips[3].ReSize (3, "KranR.smk", "Kran2.raw", XY (300, 129), SPM_IDLE,         CRepeat(1,1), CPostWait(1,0),   SMACKER_CLIP_DONTCANCEL,
                               &KranArrived, SMACKER_CLIP_SET|SMACKER_CLIP_FRAME+21*SMACKER_CLIP_MULT, 4, NULL,
                               "A1", 0);

   pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("TipAu.gli", GliPath), &pMenuLib, L_LOCMEM);
   TipBm.ReSize (pMenuLib, "BLOC1");
   MapPlaneBms[0].ReSize (pMenuLib, "PL_B00", 1+8);
   MapPlaneBms[1].ReSize (pMenuLib, "PL_V00", 1+8);
   MapPlaneBms[2].ReSize (pMenuLib, "PL_R00", 1+8);
   MapPlaneBms[3].ReSize (pMenuLib, "PL_J00", 1+8);
   MapPlaneBms[4].ReSize (pMenuLib, "PL_SW00", 1+8);
   
   SetBackgroundFx (0, "Pap3.raw",     25000);    //Papierrascheln

   for (c=Sim.Players.Players[(SLONG)PlayerNum].Planes.AnzEntries()-1; c>=0; c--)
      if (Sim.Players.Players[(SLONG)PlayerNum].Planes.IsInAlbum(c))
         Sim.Players.Players[(SLONG)PlayerNum].Planes[c].UpdateGlobePos (0);

   for (c=0; c<(SLONG)LastMinuteAuftraege.AnzEntries(); c++)
      RepaintZettel (c);

   #ifdef DEMO
      MenuStart (MENU_REQUEST, MENU_REQUEST_NO_LM);
      MenuSetZoomStuff (XY(320,220), 0.17, FALSE);
   #endif

   SDL_ShowWindow(FrameWnd->m_hWnd);
   SDL_UpdateWindowSurface(FrameWnd->m_hWnd);
}

//--------------------------------------------------------------------------------------------
//Destruktor:
//--------------------------------------------------------------------------------------------
CLastMinute::~CLastMinute()
{
   SLONG c;

   TipBm.Destroy();

   for (c=0; c<5; c++)
      MapPlaneBms[c].Destroy();

   if (pMenuLib && pGfxMain) pGfxMain->ReleaseLib (pMenuLib);

   for (c=0; c<(SLONG)LastMinuteAuftraege.AnzEntries(); c++)
      if (LastMinuteAuftraege.Auftraege[c].Praemie<0)
         LastMinuteAuftraege.Auftraege[c].Praemie=0;

   Sim.Players.Players[(SLONG)PlayerNum].Messages.AddMessage (BERATERTYP_AUFTRAG, "", MESSAGE_COMMENT);

   Sim.NetRefill (1);
   LastMinuteAuftraege.RefillForLastMinute ();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// LastMinute message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//void CLastMinute::OnPaint()
//--------------------------------------------------------------------------------------------
void CLastMinute::OnPaint()
{
   SLONG  c;
   CPoint point = Sim.Players.Players[(SLONG)PlayerNum].CursorPos;
   XY     RoomPos;
   BOOL   RemoveTip=TRUE;
   BOOL   IsOverPaper=FALSE;
   static XY     LastMouse;

   static SLONG  LastTime;
   SLONG         DeltaTime = timeGetTime()-LastTime;

   LastTime=timeGetTime();

   //Koordinaten für kleine Fenster konvertieren:
   ConvertMousePosition (point, &RoomPos);

   if (SLONG(Sim.Time)>=timeLastClose) Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();

   if (!bHandy) SetMouseLook (CURSOR_NORMAL, 0, ROOM_LAST_MINUTE, 0);

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   SP_Monitor.Pump ();
   SP_Monitor.BlitAtT (RoomBm);
   SP_Girl.Pump ();
   SP_Girl.BlitAtT (RoomBm);
   SP_Kran.Pump ();
   SP_Kran.BlitAtT (RoomBm);

   if (MoveKran)
   {
      if (SP_Kran.GetClip()==0) NewKranDir=1;
      if (SP_Kran.GetClip()==2) NewKranDir=3;
      MoveKran=FALSE;
   }

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   if (!IsDialogOpen() && !MenuIsOpen())
   {
      if (gMousePosition.IfIsWithin (560, 0, 640, 440)) SetMouseLook (CURSOR_EXIT, 0, ROOM_LAST_MINUTE, 999);

      if (RoomPos.IfIsWithin (17,73,237,343))
         SetTip (&TipBm, MapPlaneBms, FALSE, XY (300,125), TIP_BUYAUFTRAG, -1, 0, 0);
   }

   RoomBm.pBitmap->SetClipRect (CRect (0,0,640,440));
   for (c=0; c<(SLONG)LastMinuteAuftraege.AnzEntries(); c++)
   {
      if (LastMinuteAuftraege.Auftraege[c].Praemie>0)
      {
         if (!RoomBm.BlitFromT (ZettelBms[c], ZettelPos[c*2], ZettelPos[c*2+1]))
         {
            RepaintZettel (c);
            RoomBm.BlitFromT (ZettelBms[c], ZettelPos[c*2], ZettelPos[c*2+1]);
         }

         if (!IsDialogOpen() && !MenuIsOpen())
         if (RoomPos.IfIsWithin (ZettelPos[c*2], ZettelPos[c*2+1], ZettelPos[c*2]+gZettelBms[c%3].Size.x, ZettelPos[c*2+1]+gZettelBms[c%3].Size.y))
         {
            RemoveTip   = FALSE;
            IsOverPaper = TRUE;

            SetTip (&TipBm, MapPlaneBms, FALSE, XY (300,125), TIP_BUYAUFTRAG, c, 0, 0);

            SetMouseLook (CURSOR_HOT, 0, ROOM_TAFEL, 0);

            if (c!=LastTip)
            {
               LastTip=c;

               Sim.Players.Players[(SLONG)PlayerNum].CheckAuftragsBerater (LastMinuteAuftraege.Auftraege[c]);
            }
         }
      }
   }
   if (!IsOverPaper) LastTip=-1;

   for (c=0; c<(SLONG)LastMinuteAuftraege.AnzEntries(); c++)
   {
      if (LastMinuteAuftraege.Auftraege[c].Praemie<0)
      {
         LastMinuteAuftraege.Auftraege[c].Praemie+=DeltaTime*4;
         if (LastMinuteAuftraege.Auftraege[c].Praemie>0) LastMinuteAuftraege.Auftraege[c].Praemie=0;

         XY    Pos;
         SLONG p=-LastMinuteAuftraege.Auftraege[c].Praemie;

         Pos.x = (p*ZettelPos[c*2]   +  16*(1000-p))/1000;
         Pos.y = (p*ZettelPos[c*2+1] + 440*(1000-p))/1000;

         //RoomBm.BlitFromT (ZettelBms[c], Pos);
         {
            SDL_Rect SrcRect = { 0,0,ZettelBms[c].Size.x,ZettelBms[c].Size.y };
            SDL_Rect DestRect;

            DestRect.x = Pos.x;
            DestRect.y = Pos.y;
            DestRect.w = long(ZettelBms[c].Size.x*(p+400)/1400);
            DestRect.h = long(ZettelBms[c].Size.y*(p+400)/1400);

            SDL_BlitScaled(ZettelBms[c].pBitmap->GetSurface(),&SrcRect,RoomBm.pBitmap->GetSurface(),&DestRect);
         }
      }
   }
   RoomBm.pBitmap->SetClipRect (CRect (0,0,640,480));

   //if (Sim.Players.Players[(SLONG)PlayerNum].Messages.IsSilent()) LastTip=-1;

   if (RemoveTip && !gMousePosition.IfIsWithin (8,69, 242,347) && LastMouse.IfIsWithin (8,69, 242,347))
      Sim.Players.Players[(SLONG)PlayerNum].Messages.AddMessage (BERATERTYP_AUFTRAG, "", MESSAGE_COMMENT);

   LastMouse=gMousePosition;

   #ifdef DEMO
      if (!IsDialogOpen() && !MenuIsOpen()) SetMouseLook (CURSOR_EXIT, 0, ROOM_LAST_MINUTE, 999);
   #endif

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::PostPaint ();
   CStdRaum::PumpToolTips ();
}

//--------------------------------------------------------------------------------------------
//void CLastMinute::OnPaint()
//--------------------------------------------------------------------------------------------
void CLastMinute::RepaintZettel (SLONG n)
{
   if (LastMinuteAuftraege.Auftraege[n].Praemie>0)
   {
      ZettelBms[n].ReSize(gZettelBms[n%3].Size);
      ZettelBms[n].BlitFrom(gZettelBms[n%3]);

      ZettelBms[n].PrintAt (bprintf ("%s-%s", Cities [LastMinuteAuftraege.Auftraege[n].VonCity].Kuerzel, Cities [LastMinuteAuftraege.Auftraege[n].NachCity].Kuerzel),
                      FontSmallBlack, TEC_FONT_CENTERED,
                      XY(3, 10), XY(ZettelBms[n].Size.x-3, 29));

      ZettelBms[n].PrintAt (ShortenLongCities(Cities[LastMinuteAuftraege.Auftraege[n].VonCity].Name),
                      FontSmallBlack, TEC_FONT_CENTERED,
                      XY(3, 31), XY(ZettelBms[n].Size.x-3, 102));
      ZettelBms[n].PrintAt ("-",
                      FontSmallBlack, TEC_FONT_CENTERED,
                      XY(3, 41), XY(ZettelBms[n].Size.x-3, 102));
      ZettelBms[n].PrintAt (ShortenLongCities(Cities[LastMinuteAuftraege.Auftraege[n].NachCity].Name),
                      FontSmallBlack, TEC_FONT_CENTERED,
                      XY(3, 52), XY(ZettelBms[n].Size.x-3, 102));
   }
}

//--------------------------------------------------------------------------------------------
//void CLastMinute::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CLastMinute::OnLButtonDown(UINT nFlags, CPoint point)
{
   SLONG c;
   XY RoomPos;

   PLAYER &qPlayer = Sim.Players.Players[(SLONG)PlayerNum];

   DefaultOnLButtonDown ();

   if (!ConvertMousePosition (point, &RoomPos))
   {
	   CStdRaum::OnLButtonDown(nFlags, point);
      return;
   }

#ifndef DEMO
   if (!PreLButtonDown (point))
   {
      if (MouseClickArea==ROOM_LAST_MINUTE && MouseClickId==999) qPlayer.LeaveRoom();

      for (c=LastMinuteAuftraege.AnzEntries()-1; c>=0; c--)
      {
         if (LastMinuteAuftraege.Auftraege[c].Praemie>0)
         {
            if (RoomPos.IfIsWithin (ZettelPos[c*2], ZettelPos[c*2+1], ZettelPos[c*2]+gZettelBms[c%3].Size.x, ZettelPos[c*2+1]+gZettelBms[c%3].Size.y))
            {
               if (qPlayer.Auftraege.GetNumFree()<3)
                  qPlayer.Auftraege.Auftraege.ReSize (qPlayer.Auftraege.AnzEntries()+10);

               gUniversalFx.Stop();
               gUniversalFx.ReInit("paptake.raw");
               gUniversalFx.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7);

               qPlayer.Auftraege+=LastMinuteAuftraege.Auftraege[c];
               qPlayer.NetUpdateOrder (LastMinuteAuftraege.Auftraege[c]);

               //Für den Statistikscreen:
               qPlayer.Statistiken[STAT_AUFTRAEGE].AddAtPastDay (0, 1);
               qPlayer.Statistiken[STAT_LMAUFTRAEGE].AddAtPastDay (0, 1);

               Sim.SendSimpleMessage (ATNET_SYNCNUMFLUEGE, NULL, Sim.localPlayer, (long)qPlayer.Statistiken[STAT_AUFTRAEGE].GetAtPastDay (0), (long)qPlayer.Statistiken[STAT_LMAUFTRAEGE].GetAtPastDay (0));

               LastMinuteAuftraege.Auftraege[c].Praemie=-1000;   
               qPlayer.NetUpdateTook (1, c);
               break;
            }
         }
      }   
   }
#else
   if (!PreLButtonDown (point))
      if (MouseClickArea==ROOM_LAST_MINUTE && MouseClickId==999) qPlayer.LeaveRoom();
#endif

   CStdRaum::OnLButtonDown(nFlags, point);
}

//--------------------------------------------------------------------------------------------
//void CLastMinute::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CLastMinute::OnRButtonDown(UINT nFlags, CPoint point)
{
   DefaultOnRButtonDown ();

   //Außerhalb geklickt? Dann Default-Handler!
   if (point.x<WinP1.x || point.y<WinP1.y || point.x>WinP2.x || point.y>WinP2.y)
   {
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
