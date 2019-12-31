//============================================================================================
// ArabAir.cpp : Der Schalter und das Hinterzimmer von ArabAir!
//============================================================================================
#include "stdafx.h"
#include "ArabAir.h"
#include "glarab.h"
#include "atnet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DSBVOLUME_MIN               -10000
#define DSBVOLUME_MAX               0

//Zum debuggen:
static const char FileId[] = "Arab";

//--------------------------------------------------------------------------------------------
//Die Schalter wird erˆffnet:
//--------------------------------------------------------------------------------------------
CArabAir::CArabAir(BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "arab.gli", GFX_ARAB_AUS)
{
   SetRoomVisited (PlayerNum, ROOM_ARAB_AIR);
   HandyOffset = 320;
   Sim.FocusPerson=-1;

   if (!bHandy) AmbientManager.SetGlobalVolume (60);

   //if (gLanguage==LANGUAGE_D) FunkelAnim.ReSize (pRoomLib, "FUNK00", 7, NULL, FALSE, ANIMATION_MODE_REPEAT, 300, 2, 600, 1);

   Talkers.Talkers[TALKER_ARAB].IncreaseReference ();
   DefaultDialogPartner=TALKER_ARAB;

   SP_Araber.ReSize (7);
   //SP_Araber.SetSpeakFx ("SayArab.raw");
   SP_Araber.Clips[0].ReSize (0, "ArabW.smk", "", XY (425, 108), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                              "A9E1", 0, 1);
   SP_Araber.Clips[1].ReSize (1, "ArabA.smk", "", XY (425, 108), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten->Speak
                              "E1E1E1", 2, 4, 6);
   SP_Araber.Clips[2].ReSize (2, "ArabL.smk", "", XY (425, 108), SPM_LISTENING,  CRepeat(1,1), CPostWait(50,50), SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Listen
                              "A9E5E5", 2, 4, 6);
   SP_Araber.Clips[3].ReSize (3, "ArabP.smk", "", XY (425, 108), SPM_TALKING,    CRepeat(1,1), CPostWait(20,20), SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Psst!, danach Speaking
                              "A9", 4);
   SP_Araber.Clips[4].ReSize (4, "ArabS.smk", "", XY (425, 108), SPM_TALKING,    CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Speak
                              "A9E1E1", 4, 2, 6);
   SP_Araber.Clips[5].ReSize (5, "ArabZ.smk", "ArabZ.raw", XY (425, 108), SPM_IDLE, CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Speak->Wait
                              "A9", 0);
   SP_Araber.Clips[6].ReSize (0, "ArabG.smk", "", XY (425, 108), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Umschauen
                              "A9", 5);

   GloveBm.ReSize (pRoomLib, "GLOVE");

   SDL_ShowWindow(FrameWnd->m_hWnd);
   SDL_UpdateWindowSurface(FrameWnd->m_hWnd);

   if (Sim.Options.OptionEffekte)
   {
      RadioFX.ReInit("radio.raw");

      StartupFX.ReInit("arab.raw");
      StartupFX.Play(0, Sim.Options.OptionEffekte*100/7);
   }

   if (Sim.Players.Players[(SLONG)PlayerNum].HasBerater(BERATERTYP_KEROSIN))
   {
      if (Sim.Kerosin<400 && Sim.Players.Players[(SLONG)PlayerNum].TankInhalt*2<Sim.Players.Players[(SLONG)PlayerNum].Tank*4 && Sim.Players.Players[(SLONG)PlayerNum].Money>20000)
      {
         //Kerosin kaufen:
         Sim.Players.Players[(SLONG)PlayerNum].Messages.AddMessage (BERATERTYP_KEROSIN, StandardTexte.GetS (TOKEN_ADVICE, 3000));
      }
      else if (Sim.Kerosin>600)
      {
         //Kerosin nicht kaufen:
         Sim.Players.Players[(SLONG)PlayerNum].Messages.AddMessage (BERATERTYP_KEROSIN, StandardTexte.GetS (TOKEN_ADVICE, 3001));
      }
   }
}

//--------------------------------------------------------------------------------------------
//Sehr destruktiv!
//--------------------------------------------------------------------------------------------
CArabAir::~CArabAir()
{
   StartupFX.SetVolume(DSBVOLUME_MIN);
   StartupFX.Stop();
   Talkers.Talkers[TALKER_ARAB].DecreaseReference ();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CArabAir message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//void CArabAir::OnPaint()
//--------------------------------------------------------------------------------------------
void CArabAir::OnPaint()
{
   if (!bHandy) SetMouseLook (CURSOR_NORMAL, 0, ROOM_ARAB_AIR, 0);

   if (Sim.Date>5) Sim.GiveHint (HINT_ARABAIR);

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   DrawChart (RoomBm, 0xff2020, Sim.KerosinPast, -1, -1, XY(313,136), XY(371,146), XY(313,166), XY(371,176));

   SP_Araber.Pump ();
   SP_Araber.BlitAtT (RoomBm);

   //FunkelAnim.BlitAt (RoomBm, 232, 202);

   if (!Sim.Players.Players[PlayerNum].HasItem (ITEM_GLOVE) && Sim.ItemGlove)
      RoomBm.BlitFromT (GloveBm, 314, 280);

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   if (!IsDialogOpen() && !MenuIsOpen())
   {
      if (gMousePosition.IfIsWithin (0, 232, 144, 398) || gMousePosition.IfIsWithin (0, 288, 314, 380) || gMousePosition.IfIsWithin (0, 351, 639, 439)) SetMouseLook (CURSOR_EXIT, 0, ROOM_ARAB_AIR, 999);
      else if (gMousePosition.IfIsWithin (314,280,314+72,280+62) && Sim.ItemGlove && !Sim.Players.Players[PlayerNum].HasItem (ITEM_GLOVE)) SetMouseLook (CURSOR_HOT, 0, ROOM_ARAB_AIR, 12);
      else if (gMousePosition.IfIsWithin (437,89,600,323)) SetMouseLook (CURSOR_HOT, 0, ROOM_ARAB_AIR, 10);
      else if (gMousePosition.IfIsWithin (313,136,371,176))
      {
         SetMouseLook (CURSOR_NORMAL, 5000, bprintf(LPCTSTR(CString(StandardTexte.GetS (TOKEN_TOOLTIP, 4500))),Sim.Kerosin), ROOM_ARAB_AIR, 0);

         if (ToolTipState==FALSE)
            ToolTipTimer=timeGetTime()-601;
      }
   }

   CStdRaum::PostPaint ();
   CStdRaum::PumpToolTips ();
}

//--------------------------------------------------------------------------------------------
//void CArabAir::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CArabAir::OnLButtonDown(UINT nFlags, CPoint point)
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
      if (gMousePosition.IfIsWithin (179,215,242,261)) RadioFX.Play (0, Sim.Options.OptionEffekte*100/7);

      if (MouseClickArea==ROOM_ARAB_AIR && MouseClickId==999) Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
      else if (MouseClickArea==ROOM_ARAB_AIR && MouseClickId==10) { StartDialog (TALKER_ARAB, MEDIUM_AIR, 0); }
      else if (MouseClickArea==ROOM_ARAB_AIR && MouseClickId==12)
      {
         Sim.Players.Players[(SLONG)PlayerNum].BuyItem (ITEM_GLOVE);

         if (Sim.Players.Players[(SLONG)PlayerNum].HasItem (ITEM_GLOVE))
         {
            Sim.ItemGlove=0;
            Sim.SendSimpleMessage (ATNET_TAKETHING, NULL, ITEM_GLOVE);
         }
      }
      else CStdRaum::OnLButtonDown(nFlags, point);
   }
}

//--------------------------------------------------------------------------------------------
//void CArabAir::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CArabAir::OnRButtonDown(UINT nFlags, CPoint point)
{
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
            Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();

         CStdRaum::OnRButtonDown(nFlags, point);
      }
   }
}
