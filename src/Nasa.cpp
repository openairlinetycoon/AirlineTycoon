//============================================================================================
// Nasa.cpp : Der Nasa-Shop
//============================================================================================
// Link: "Nasa.h"
//============================================================================================
#include "stdafx.h"
#include "Nasa.h"
#include "glnasa.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Zum debuggen:
static const char FileId[] = "Sabo";

//--------------------------------------------------------------------------------------------
//Der Sabotageraum
//--------------------------------------------------------------------------------------------
CNasa::CNasa(BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "nasa.gli", GFX_NASA)
{
#ifndef DEMO
   SetRoomVisited (PlayerNum, ROOM_NASA);
   HandyOffset = 320;

   KommVarTippNow=-1;
   KommAlien=-1;

   Sim.FocusPerson=-1;
   DefaultDialogPartner=TALKER_NASA;

   if (!bHandy) AmbientManager.SetGlobalVolume (40);

   Talkers.Talkers[TALKER_NASA].IncreaseReference ();

   //Hintergrundsounds:
   if (Sim.Options.OptionEffekte)
   {
      FunkFx.ReInit("funk.raw");
      FunkFx.Play(DSBPLAY_NOSTOP|DSBPLAY_LOOPING, Sim.Options.OptionEffekte*100/7);
   }

   SP_Mann.ReSize (16);
   SP_Mann.Clips[0].ReSize (0, "LE_RedeW.smk", "", XY (210, 140), SPM_TALKING, CRepeat(1,1), CPostWait(2,2), SMACKER_CLIP_DONTCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,
                            "A1", 7);

   SP_Mann.Clips[14].ReSize (14, "LE_Wait.smk", "", XY (210, 140), SPM_IDLE,  CRepeat(1,1), CPostWait(10,10), SMACKER_CLIP_CANCANCEL,
                             NULL, SMACKER_CLIP_SET, 0, NULL,
                             "A9A2A1A1E1", 14, 1, 2, 3, 6);

   SP_Mann.Clips[1].ReSize (1, "LE_WaitK.smk", "", XY (210, 140), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,
                            "A1", 14);
   SP_Mann.Clips[2].ReSize (2, "LE_lock.smk", "", XY (210, 140), SPM_IDLE,   CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,
                            "A1", 14);
   SP_Mann.Clips[3].ReSize (3, "LE_strck.smk", "", XY (210, 140), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,
                            "A1", 4);
   SP_Mann.Clips[4].ReSize (4, "LE_strkW.smk", "", XY (210, 140), SPM_IDLE,  CRepeat(1,1), CPostWait(20,90), SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,
                            "A1", 5);
   SP_Mann.Clips[5].ReSize (5, "LE_strkZ.smk", "", XY (210, 140), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,
                            "A1", 14);

   SP_Mann.Clips[6].ReSize (6, "LE_turn.smk", "", XY (210, 140), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,
                            "A9E1E1E1", 7, 7, 9, 13);

   SP_Mann.Clips[7].ReSize (7, "LE_RedeW.smk", "", XY (210, 140), SPM_LISTENING, CRepeat(1,1), CPostWait(10,10), SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, &KommVarTippNow,
                            "A9A2E1E1E1E1A1", 7, 8, 9, 10, 11, 13, 15);
   SP_Mann.Clips[8].ReSize (8, "LE_RedeK.smk", "", XY (210, 140), SPM_LISTENING, CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, &KommVarTippNow,
                            "A9E1E1", 7, 9, 13);
   SP_Mann.Clips[9].ReSize (9, "LE_RedeA.smk", "", XY (210, 140), SPM_TALKING,   CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, &KommVarTippNow,
                            "A4A4A4E1E1", 9, 10, 11, 13, 7);
   SP_Mann.Clips[10].ReSize (10, "LE_RedeB.smk", "", XY (210, 140), SPM_TALKING, CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                             NULL, SMACKER_CLIP_SET, 0, &KommVarTippNow,
                             "A4A4A4E1E1", 9, 10, 11, 13, 7);
   SP_Mann.Clips[11].ReSize (11, "LE_RedeC.smk", "", XY (210, 140), SPM_TALKING, CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                             NULL, SMACKER_CLIP_SET, 0, &KommVarTippNow,
                             "A4A4A4E1E1", 9, 10, 11, 13, 7);

   SP_Mann.Clips[12].ReSize (12, "LE_taste.smk", "", XY (210, 140), SPM_LISTENING,  CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                             &KommVarTippNow, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, NULL,
                             "A9E1E1", 7, 9, 13);
   SP_Mann.Clips[15].ReSize (15, "LE_taste.smk", "", XY (210, 140), SPM_LISTENING,  CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                             &KommAlien, SMACKER_CLIP_SET|SMACKER_CLIP_FRAME+6*SMACKER_CLIP_MULT, 1, NULL,
                             "A9E1E1", 7, 9, 13);

   SP_Mann.Clips[13].ReSize (13, "LE_TurnZ.smk", "", XY (210, 140), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                             NULL, SMACKER_CLIP_SET, 14, NULL,
                             "A9", 14);

   SP_Alien.ReSize (2);
   SP_Alien.Clips[0].ReSize (0, "Chick_.smk", "" /*"Chick_.raw"*/, XY (445, 175), SPM_IDLE, CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, &KommAlien,
                            "A1", 0);
   SP_Alien.Clips[1].ReSize (1, "Alien_.smk", "", XY (445, 175), SPM_IDLE, CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_CANCANCEL,
                            &KommAlien, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, NULL,
                            "A9", 0);

   SDL_ShowWindow(FrameWnd->m_hWnd);
   SDL_UpdateWindowSurface(FrameWnd->m_hWnd);

   StartDialog (TALKER_NASA, MEDIUM_AIR, 0);
#endif
}

//--------------------------------------------------------------------------------------------
//Sehr destruktiv!
//--------------------------------------------------------------------------------------------
CNasa::~CNasa()
{
   Talkers.Talkers[TALKER_NASA].DecreaseReference ();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CNasa message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//void CNasa::OnPaint()
//--------------------------------------------------------------------------------------------
void CNasa::OnPaint()
{
#ifndef DEMO
   if (!bHandy) SetMouseLook (CURSOR_NORMAL, 0, ROOM_SABOTAGE, 0);

   if (Sim.Date>5) Sim.GiveHint (HINT_NASA);

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   SP_Alien.Pump ();
   SP_Alien.BlitAtT (RoomBm);
   SP_Mann.Pump ();
   SP_Mann.BlitAtT (RoomBm);

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   if (!IsDialogOpen() && !MenuIsOpen())
   {
      if (gMousePosition.IfIsWithin (195,373,424,440)) SetMouseLook (CURSOR_EXIT, 0, ROOM_NASA, 999);
      else if (gMousePosition.IfIsWithin (197, 140,398,377)) SetMouseLook (CURSOR_HOT, 0, ROOM_NASA, 10);
   }

   CStdRaum::PostPaint ();
   CStdRaum::PumpToolTips ();
#endif
}

//--------------------------------------------------------------------------------------------
//void CNasa::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CNasa::OnLButtonDown(UINT nFlags, CPoint point)
{
#ifndef DEMO
   XY RoomPos;

   DefaultOnLButtonDown ();

   if (!ConvertMousePosition (point, &RoomPos))
   {
	   CStdRaum::OnLButtonDown(nFlags, point);
      return;
   }

   if (!PreLButtonDown (point))
   {
      if (MouseClickArea==ROOM_NASA && MouseClickId==999)
      {
         if (Sim.Players.Players[PlayerNum].TalkedToNasa==FALSE)
         {
            Sim.Players.Players[PlayerNum].TalkedToNasa=TRUE;
            StartDialog (TALKER_NASA, MEDIUM_AIR, 2);
            DontDisplayPlayer=Sim.localPlayer;
         }
         else Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
      }
      else if (MouseClickArea==ROOM_NASA && MouseClickId==10) StartDialog (TALKER_NASA, MEDIUM_AIR, 1);
      else CStdRaum::OnLButtonDown(nFlags, point);
   }
#endif
}

//--------------------------------------------------------------------------------------------
//void CNasa::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CNasa::OnRButtonDown(UINT nFlags, CPoint point)
{
#ifndef DEMO
   DefaultOnRButtonDown ();

   //Auﬂerhalb geklickt? Dann Default-Handler!
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
         {
            if (Sim.Players.Players[PlayerNum].TalkedToNasa==FALSE)
            {
               Sim.Players.Players[PlayerNum].TalkedToNasa=TRUE;
               StartDialog (TALKER_NASA, MEDIUM_AIR, 2);
               DontDisplayPlayer=Sim.localPlayer;
            }
            else
            {
               Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
               CStdRaum::OnRButtonDown(nFlags, point);
            }
         }
         else CStdRaum::OnRButtonDown(nFlags, point);
      }
   }
#endif
}
