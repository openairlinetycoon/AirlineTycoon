//============================================================================================
// Designer.cpp : Der Designer-Raum
//============================================================================================
#include "stdafx.h"
#include "Designer.h"
#include "glDesigner.h"
#include "atnet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DSBVOLUME_MIN               -10000
#define DSBVOLUME_MAX               0

//Zum debuggen:
static const char FileId[] = "Secu";

#if __cplusplus < 201703L // If the version of C++ is less than 17
#include <experimental/filesystem>
// It was still in the experimental:: namespace
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

//--------------------------------------------------------------------------------------------
//Die Schalter wird eröffnet:
//--------------------------------------------------------------------------------------------
CDesigner::CDesigner(BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "Designer.gli", GFX_DESIGNER)
{
   SetRoomVisited (PlayerNum, ROOM_DESIGNER);
   HandyOffset = 320;
   Sim.FocusPerson=-1;

   //The game expects in dialogue options, that this path exists
   //Path will be "%AppPath%/myplanes/"
   fs::create_directory(LPCSTR(AppPath + MyPlanePath.Left(MyPlanePath.GetLength() - 3)));

   if (!bHandy) AmbientManager.SetGlobalVolume (60);

   Talkers.Talkers[TALKER_DESIGNER].IncreaseReference ();
   DefaultDialogPartner=TALKER_DESIGNER;

   SP_Stan.ReSize (6);
   SP_Stan.Clips[0].ReSize (0, "DeManW.smk", "", XY (198, 18), SPM_IDLE,       CRepeat(1,1), CPostWait(10,10),   SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                              "A9E1E1A1A1A1", 0, 1, 2, 3, 4, 5);
   SP_Stan.Clips[1].ReSize (1, "DeManW.smk", "", XY (198, 18), SPM_LISTENING,  CRepeat(1,1), CPostWait(50,50), SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Listen
                              "A9E5E5", 1, 2, 0);
   SP_Stan.Clips[2].ReSize (2, "DeManS.smk", "", XY (198, 18), SPM_TALKING,    CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Speak
                              "A9E1E1", 2, 1, 0);
   SP_Stan.Clips[3].ReSize (3, "DeManSh.smk", "", XY (198, 18), SPM_IDLE, CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Schuppen
                              "A9", 0);
   SP_Stan.Clips[4].ReSize (4, "DeManFl.smk", "", XY (198, 18), SPM_IDLE, CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Flash
                              "A9", 0);
   SP_Stan.Clips[5].ReSize (5, "DeManCo.smk", "", XY (198, 18), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Couch
                              "A9", 0);

   SP_Bat.ReSize (2);
   SP_Bat.Clips[0].ReSize (0, "DeBatW.smk", "", XY (0, 13), SPM_IDLE,       CRepeat(1,1), CPostWait(50,190),   SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                              "A9A1", 0, 1);
   SP_Bat.Clips[1].ReSize (1, "DeBat.smk", "", XY (0, 13), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                              "A9", 0);

   SP_Schwalbe.ReSize (2);
   SP_Schwalbe.Clips[0].ReSize (0, "DeWorker.smk", "", XY (330, 262), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A1", 0, 1);
   SP_Schwalbe.Clips[1].ReSize (1, "DePlane.smk", "", XY (330, 262), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 0);

   SP_Blender.ReSize (2);
   SP_Blender.Clips[0].ReSize (0, "DeBatW.smk", "", XY (337, 92), SPM_IDLE,       CRepeat(1,1), CPostWait(10,90),   SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                               "A9A1", 0, 1);
   SP_Blender.Clips[1].ReSize (0, "DeBlnder.smk", "", XY (337, 92), SPM_IDLE,       CRepeat(1,1), CPostWait(10,90),   SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                               "A9", 0);

   SP_Bein.ReSize (2);
   SP_Bein.Clips[0].ReSize (0, "DeLeg.smk", "", XY (435, 222), SPM_IDLE,       CRepeat(1,1), CPostWait(10,10),   SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                               "A9", 1);
   SP_Bein.Clips[1].ReSize (1, "DeLegW.smk", "", XY (435, 222), SPM_IDLE,      CRepeat(1,1), CPostWait(10,90),   SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                               "A9A2", 1, 0);

   SDL_ShowWindow(FrameWnd->m_hWnd);
   SDL_UpdateWindowSurface(FrameWnd->m_hWnd);

   //Hintergrundsounds:
   if (Sim.Options.OptionEffekte)
   {
      BackFx.ReInit("design.raw");
      BackFx.Play(DSBPLAY_NOSTOP|DSBPLAY_LOOPING, Sim.Options.OptionEffekte*100/7);
   }
}

//--------------------------------------------------------------------------------------------
//Sehr destruktiv!
//--------------------------------------------------------------------------------------------
CDesigner::~CDesigner()
{
   BackFx.SetVolume(DSBVOLUME_MIN);
   BackFx.Stop();
   Talkers.Talkers[TALKER_DESIGNER].DecreaseReference ();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CDesigner message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//void CDesigner::OnPaint()
//--------------------------------------------------------------------------------------------
void CDesigner::OnPaint()
{
   SLONG   NewTip;

   if (!bHandy) SetMouseLook (CURSOR_NORMAL, 0, ROOM_DESIGNER, 0);

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   SP_Bat.Pump ();       SP_Bat.BlitAtT (RoomBm);
   SP_Schwalbe.Pump ();  SP_Schwalbe.BlitAtT (RoomBm);
   SP_Bein.Pump ();      SP_Bein.BlitAtT (RoomBm);
   SP_Blender.Pump ();   SP_Blender.BlitAtT (RoomBm);
   SP_Stan.Pump ();      SP_Stan.BlitAtT (RoomBm);

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   if (!IsDialogOpen() && !MenuIsOpen())
   {
      if (gMousePosition.IfIsWithin (296, 240, 361, 337)) SetMouseLook (CURSOR_EXIT, 0, ROOM_DESIGNER, 999);
      else if (gMousePosition.IfIsWithin (200,26,354,341)) SetMouseLook (CURSOR_HOT, 0, ROOM_DESIGNER, 10);
      else if (gMousePosition.IfIsWithin (43,112,250,307)) SetMouseLook (CURSOR_HOT, 0, ROOM_DESIGNER, 12);
   }

   if (CurrentMenu==MENU_BUYXPLANE && ((gMousePosition-MenuPos).IfIsWithin (216,6, 387,212)))
   {
      NewTip = (gMousePosition.y-(MenuPos.y+22))/13 + MenuPage;

      if (NewTip>=0 && NewTip-MenuPage<13 && NewTip<MenuDataTable.LineIndex.AnzEntries())
      {
         if (NewTip != CurrentTip)
         {
            MenuRepaint ();
            DrawXPlaneTipContents (OnscreenBitmap, MenuDataTable.Values[NewTip*MenuDataTable.AnzColums+2], 
                                   XY(6,6), XY(6,28), &FontSmallBlack, &FontSmallBlack);
            CurrentTip = NewTip;
         }

         CheckCursorHighlight (ReferenceCursorPos, CRect (MenuPos.x+216, MenuPos.y+(NewTip-MenuPage)*13+25-3, MenuPos.x+387, MenuPos.y+(NewTip-MenuPage)*13+25+12), ColorOfFontBlack, CURSOR_HOT);
      }
      else NewTip = -1;
   }
   else NewTip = -1;

   CStdRaum::PostPaint ();
   CStdRaum::PumpToolTips ();
}

//--------------------------------------------------------------------------------------------
//void CDesigner::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CDesigner::OnLButtonDown(UINT nFlags, CPoint point)
{
   XY RoomPos;

   DefaultOnLButtonDown ();

   if (!ConvertMousePosition (point, &RoomPos))
   {
	   CStdRaum::OnLButtonDown(nFlags, point);
      return;
   }

   if (!PreLButtonDown (point))
   {
      if (MouseClickArea==ROOM_DESIGNER && MouseClickId==999) Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
      else if (MouseClickArea==ROOM_DESIGNER && MouseClickId==10) { StartDialog (TALKER_DESIGNER, MEDIUM_AIR, 1000); }
      else if (MouseClickArea==ROOM_DESIGNER && MouseClickId==12)
      {
         Sim.Players.Players[(SLONG)PlayerNum].EnterRoom(ROOM_EDITOR);
      }
      else CStdRaum::OnLButtonDown(nFlags, point);
   }
}

//--------------------------------------------------------------------------------------------
//void CDesigner::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CDesigner::OnRButtonDown(UINT nFlags, CPoint point)
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
