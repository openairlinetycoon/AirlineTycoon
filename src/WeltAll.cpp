//============================================================================================
// WeltAll.cpp : Das Fernglass ins WeltAll
//============================================================================================
#include "stdafx.h"
#include "WeltAll.h"
#include "glweltall.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SB_CColorFX ColorFX;

//Zum debuggen:
static const char FileId[] = "Spac";

//--------------------------------------------------------------------------------------------
//Die Schalter wird eröffnet:
//--------------------------------------------------------------------------------------------
CWeltAll::CWeltAll(BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "", NULL)
{
   ReSize ("WeltAll.gli", NULL);
   RoomBm.ReSize (640, 440);

   FernglasBms.ReSize (pRoomLib, "O1 O2 ATOP ABOTTOM AL1 AL2 AL3 AR1 AR2 AR3");
   SpaceBm.ReSize (pRoomLib, GFX_WELTALL, CREATE_SYSMEM);

   StationBms.ReSize (pRoomLib, "RING1 RING2 RING3 (O2) COMM CENTRAL LIVING BUSINES (POWER) SOLAR");
   O2ColorBms.ReSize (pRoomLib, "O2B O2G O2R O2Y");
   Ring1ColorBms.ReSize (pRoomLib, "RING1B RING1G RING1R RING1Y");
   Ring2ColorBms.ReSize (pRoomLib, "RING2B RING2G RING2R RING2Y");
   Ring3ColorBms.ReSize (pRoomLib, "RING3B RING3G RING3R RING3Y");

   CentralL.ReSize (pRoomLib, "CENTRALL");
   CommL.ReSize (pRoomLib, "COMML");
   BusinessL.ReSize (pRoomLib, "BUSINESL");
   LivingL.ReSize (pRoomLib, "LIVINGL");

   StarBms.ReSize (pRoomLib, "STAR1", 3);

   SetRoomVisited (PlayerNum, ROOM_WELTALL);
   HandyOffset = 320;

   Sim.Players.Players[(SLONG)PlayerNum].ChangeMoney (-1, 3300, "");
   Sim.FocusPerson=-1;

   LastTime = timeGetTime();

   if (!bHandy) AmbientManager.SetGlobalVolume (60);

   //Hintergrundsounds:
   /*if (Sim.Options.OptionEffekte)
   {
      SetBackgroundFx (0, "moeve.raw",    50000, 25000);

      WellenFx.ReInit ("wellen.raw");
      WellenFx.Play(DSBPLAY_NOSTOP|DSBPLAY_LOOPING, Sim.Options.OptionEffekte*100/7);
   } */

   BlinkArrowsTimer=0;

   KonstruktorFinished=TRUE;

   for (SLONG c=0; c<4; c++)
      SP_Flags[c].ReSize (1);

   SP_Flags[0].Clips[0].ReSize (0, "FlagB.smk", "", XY ( 142,  63), SPM_IDLE, CRepeat(9,9), CPostWait(0,0), SMACKER_CLIP_CANCANCEL, NULL, SMACKER_CLIP_SET, 0, NULL, "A9", 0);
   SP_Flags[1].Clips[0].ReSize (0, "FlagG.smk", "", XY ( 442, 163), SPM_IDLE, CRepeat(9,9), CPostWait(0,0), SMACKER_CLIP_CANCANCEL, NULL, SMACKER_CLIP_SET, 0, NULL, "A9", 0);
   SP_Flags[2].Clips[0].ReSize (0, "FlagR.smk", "", XY ( 782,  63), SPM_IDLE, CRepeat(9,9), CPostWait(0,0), SMACKER_CLIP_CANCANCEL, NULL, SMACKER_CLIP_SET, 0, NULL, "A9", 0);
   SP_Flags[3].Clips[0].ReSize (0, "FlagY.smk", "", XY (1072, 103), SPM_IDLE, CRepeat(9,9), CPostWait(0,0), SMACKER_CLIP_CANCANCEL, NULL, SMACKER_CLIP_SET, 0, NULL, "A9", 0);

   SP_Ufo.ReSize (1);
   SP_Ufo.Clips[0].ReSize (0, "UFO.smk", "", XY (0, 0), SPM_IDLE, CRepeat(9,9), CPostWait(0,0), SMACKER_CLIP_CANCANCEL, NULL, SMACKER_CLIP_SET, 0, NULL, "A9", 0);

   UfoPosition=XY(640,800);
   UfoVelocity=XY(  0, -12);
   UfoNumSteps=50;
   UfoWaitTill=timeGetTime()+3000+rand()%2000;

   SDL_ShowWindow(FrameWnd->m_hWnd);
   SDL_UpdateWindowSurface(FrameWnd->m_hWnd);
}

//--------------------------------------------------------------------------------------------
//Sehr destruktiv!
//--------------------------------------------------------------------------------------------
CWeltAll::~CWeltAll()
{
   Talkers.Talkers[TALKER_ARAB].DecreaseReference ();

   FernglasBms.Destroy();
   SpaceBm.Destroy();

   StationBms.Destroy();
   Ring1ColorBms.Destroy();
   Ring2ColorBms.Destroy();
   Ring3ColorBms.Destroy();
   O2ColorBms.Destroy();
   BusinessL.Destroy();
   LivingL.Destroy();

   CentralL.Destroy();
   CommL.Destroy();

   StarBms.Destroy();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CWeltAll message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//void CWeltAll::OnPaint()
//--------------------------------------------------------------------------------------------
void CWeltAll::OnPaint()
{
   SLONG c;

   SLONG Time=timeGetTime();

   if (!KonstruktorFinished) return;

   static SLONG gMouseScrollSpeed=0;
   XY   &ViewPos = Sim.Players.Players[(SLONG)PlayerNum].IslandViewPos;

   SBBM TempBm;

   if (!bHandy) SetMouseLook (CURSOR_NORMAL, 0, ROOM_INSEL, 0);

   if (!Sim.UsedTelescope)
   {
      BlinkArrowsTimer=timeGetTime();
      Sim.UsedTelescope=TRUE;
   }

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   //Weltraum:
   RoomBm.BlitFrom (SpaceBm, -ViewPos.x, 0);

   //Die Sternschnuppen bewegen und zeichnen
   for (c=0; c<20; c++)
      if (Sternschnuppen[c].StartIndex!=-1)
      {
         Sternschnuppen[c].Position+=Sternschnuppen[c].Velocity*(Time-LastTime);

         if ((Sternschnuppen[c].Position.y>>8) < 500)
            RoomBm.BlitFromT (StarBms[Sternschnuppen[c].StartIndex], (Sternschnuppen[c].Position.x>>8)-150-ViewPos.x, (Sternschnuppen[c].Position.y>>8)-20);
         else
            Sternschnuppen[c].StartIndex=-1;
      }

   //Evtl. Sternschnuppen hinzufügen:
   if (((Time/1000)!=(LastTime/1000)) && ((rand()%3)==0))
   {
      for (c=0; c<20; c++)
         if (Sternschnuppen[c].StartIndex==-1)
         {
            Sternschnuppen[c].StartIndex = rand ()%3;
            Sternschnuppen[c].Position   = XY ((rand()%(1280+300))*256, 0);

            switch (Sternschnuppen[c].StartIndex)
            {
               case 0: Sternschnuppen[c].Velocity = XY(5,9)*SLONG(4); break;
               case 1: Sternschnuppen[c].Velocity = XY(-11,13)*SLONG(4); break;
               case 2: Sternschnuppen[c].Velocity = XY(18,12)*SLONG(4); break;
            }

            break;
         }
   }

   //Ufo:
   SP_Ufo.Pump ();
   SP_Ufo.BlitAtT (RoomBm, UfoPosition-ViewPos);
   if (Time>UfoWaitTill)
   {
      UfoPosition+=UfoVelocity;

      if (!UfoPosition.IfIsWithin(-300, -300, 1580, 810))
      {
         UfoPosition=XY(640,800);
         UfoVelocity=XY(  0, -4);
         UfoNumSteps=140;
         UfoWaitTill=timeGetTime()+13000+rand()%2000;
      }

      UfoNumSteps--;
      if (UfoNumSteps<=0)
      {
         XY Way;

         do
         {
            Way = XY(rand()%300-150, rand()%180-90);
            if (rand()%6==0) Way = XY(0, -500);

            UfoNumSteps = SLONG(Way.abs()/12);
         }
         while (UfoNumSteps<2);

         UfoVelocity = Way / UfoNumSteps;
         UfoWaitTill = timeGetTime()+1000+rand()%1000;
      }
   }

   //Raumstationen:
   for (c=0; c<4; c++)
   {
      PLAYER &qPlayer = Sim.Players.Players[c];
      XY      StationPos;

      switch (c)
      {
         case 0: StationPos = XY(  70,  60); break;
         case 1: StationPos = XY( 370, 160); break;
         case 2: StationPos = XY( 710,  60); break;
         case 3: StationPos = XY(1000, 100); break;
      }

      StationPos -= ViewPos;

      //Abschnitt 1/3:
      if (qPlayer.RocketFlags & STATION_RINGC)
      {
         RoomBm.BlitFromT (StationBms[2], XY(44,41)+StationPos);
         RoomBm.BlitFromT (Ring3ColorBms[c], XY(44,41)+XY(44,3)+StationPos);
      }
      if (qPlayer.RocketFlags & STATION_RINGB)
      {
         RoomBm.BlitFromT (StationBms[1], XY(49,65)+StationPos);
         RoomBm.BlitFromT (Ring2ColorBms[c], XY(49,65)+XY(45,2)+StationPos);
      }
      if (qPlayer.RocketFlags & STATION_RINGA)
      {
         RoomBm.BlitFromT (StationBms[0], XY(47,82)+StationPos);
         RoomBm.BlitFromT (Ring1ColorBms[c], XY(47,82)+XY(42,2)+StationPos);
      }

      //Abschnitt 2/3:
      if (qPlayer.RocketFlags & STATION_COM)
      {
         RoomBm.BlitFromT (StationBms[4], XY(97,105)+StationPos);

         if ((qPlayer.RocketFlags & STATION_SOLAR) && (qPlayer.RocketFlags & STATION_POWER))
            RoomBm.BlitFromT (CommL, XY(97,105)+XY(6,11)+StationPos);
      }
      if (qPlayer.RocketFlags & STATION_OXYGEN)
      {
         RoomBm.BlitFromT (O2ColorBms[c], XY(50,31)+StationPos);
      }
      if (qPlayer.RocketFlags & STATION_OFFICE)
      {
         RoomBm.BlitFromT (StationBms[7], XY(90,13)+StationPos);

         if ((qPlayer.RocketFlags & STATION_SOLAR) && (qPlayer.RocketFlags & STATION_POWER))
            RoomBm.BlitFromT (BusinessL, XY(90,13)+StationPos);
      }
      if (qPlayer.RocketFlags & STATION_MAIN)
      {
         RoomBm.BlitFromT (StationBms[5], XY(5,4)+StationPos);

         if ((qPlayer.RocketFlags & STATION_SOLAR) && (qPlayer.RocketFlags & STATION_POWER))
            RoomBm.BlitFromT (CentralL, XY(5,4)+XY(25,30)+StationPos);
      }
      if (qPlayer.RocketFlags & STATION_LIVING)
      {
         RoomBm.BlitFromT (StationBms[6], XY(10,114)+StationPos);

         if ((qPlayer.RocketFlags & STATION_SOLAR) && (qPlayer.RocketFlags & STATION_POWER))
            RoomBm.BlitFromT (LivingL, XY(10,114)+StationPos);
      }

      //Abschnitt 3/3:
      if (qPlayer.RocketFlags & STATION_SOLAR)
      {
         RoomBm.BlitFromT (StationBms[9], XY(65+6,31+5)+StationPos);
      }

      if (qPlayer.RocketFlags == STATION_ALL)
      {
         SP_Flags[c].Pump ();
         SP_Flags[c].BlitAtT (RoomBm, -ViewPos);
      }
   }   

   //Fernglas (Alpha):
   ColorFX.BlitAlpha (FernglasBms[2].pBitmap, RoomBm.pBitmap, XY (108,0));
   ColorFX.BlitAlpha (FernglasBms[3].pBitmap, RoomBm.pBitmap, XY (108,411));
   ColorFX.BlitAlpha (FernglasBms[4].pBitmap, RoomBm.pBitmap, XY ( 39,30));
   ColorFX.BlitAlpha (FernglasBms[5].pBitmap, RoomBm.pBitmap, XY (  0,95));
   ColorFX.BlitAlpha (FernglasBms[6].pBitmap, RoomBm.pBitmap, XY ( 39,346));
   ColorFX.BlitAlpha (FernglasBms[7].pBitmap, RoomBm.pBitmap, XY (513,30));
   ColorFX.BlitAlpha (FernglasBms[8].pBitmap, RoomBm.pBitmap, XY (589,94));
   ColorFX.BlitAlpha (FernglasBms[9].pBitmap, RoomBm.pBitmap, XY (515,346));

   //Fernglas (Non-Alpha):
   RoomBm.BlitFrom (FernglasBms[0], 532, 0);    //O1
   RoomBm.BlitFrom (FernglasBms[0], 532, 410);
   RoomBm.BlitFrom (FernglasBms[0],   0, 0);
   RoomBm.BlitFrom (FernglasBms[0],   0, 410);
   RoomBm.BlitFrom (FernglasBms[1], 601, 30);   //O2
   RoomBm.BlitFrom (FernglasBms[1], 601, 346);
   RoomBm.BlitFrom (FernglasBms[1],   0, 30);
   RoomBm.BlitFrom (FernglasBms[1],   0, 346);

   if (BlinkArrowsTimer && timeGetTime()-BlinkArrowsTimer<5000)
   {
      if ((timeGetTime()-BlinkArrowsTimer)%1000<500)
      {
         RoomBm.BlitFromT (gCursorLBm, 0, 220);
         RoomBm.BlitFromT (gCursorRBm, 640-gCursorRBm.Size.x, 220);
      }
   }

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   //Scrolling:
	if (!IsDialogOpen() && !MenuIsOpen() && !Sim.bPause)
	{
		if (gMousePosition.x<=10  && ViewPos.x>0)   SetMouseLook (CURSOR_LEFT,  0, ROOM_INSEL, 6010);
		if (gMousePosition.x>=630 && ViewPos.x<640) SetMouseLook (CURSOR_RIGHT, 0, ROOM_INSEL, 6011);
	}

   //Command&Conquer-Scrolling:
	if (MouseClickArea==ROOM_INSEL && MouseClickId==6010)
   {
      if (gMouseScrollSpeed>-44-(gMouseLButton-1)*30) gMouseScrollSpeed-=2;
      if (gMouseScrollSpeed<-66) gMouseScrollSpeed=-66;
      gMouseScroll=TRUE;
   }
	else if (MouseClickArea==ROOM_INSEL && MouseClickId==6011)
	{
      if (gMouseScrollSpeed<44+(gMouseLButton-1)*30) gMouseScrollSpeed+=2;
      if (gMouseScrollSpeed>66) gMouseScrollSpeed=66;
      gMouseScroll=TRUE;
   }

   //Weiches Scrolling abbremsen
   if ((MouseClickId!=6010 && MouseClickId!=6011) || (gMouseLButton==0 && abs(gMouseScrollSpeed)>8))
   {
      if (gMouseScrollSpeed>0) gMouseScrollSpeed = max (0, gMouseScrollSpeed-4);
      if (gMouseScrollSpeed<0) gMouseScrollSpeed = min (0, gMouseScrollSpeed+4);
   }

   if (gMouseScroll)
   {
      ViewPos.x+=gMouseScrollSpeed;
      if (ViewPos.x<0)   ViewPos.x=0;
      if (ViewPos.x>640) ViewPos.x=640;
	}
   else gMouseScrollSpeed=0;

   if (!IsDialogOpen() && !MenuIsOpen())
   {
      if (gMousePosition.IfIsWithin (0,0,100,50) ||      gMousePosition.IfIsWithin (0,0,50,100) ||
          gMousePosition.IfIsWithin (0,390,100,440) ||   gMousePosition.IfIsWithin (0,340,50,440) ||
          gMousePosition.IfIsWithin (590,0,640,100) ||   gMousePosition.IfIsWithin (540,0,640,50) ||
          gMousePosition.IfIsWithin (590,340,640,440) || gMousePosition.IfIsWithin (540,390,640,440))
         SetMouseLook (CURSOR_EXIT, 0, ROOM_INSEL, 999);
   }

   CStdRaum::PostPaint ();
   CStdRaum::PumpToolTips ();

   LastTime = Time;
}

//--------------------------------------------------------------------------------------------
//void CWeltAll::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CWeltAll::OnLButtonDown(UINT nFlags, CPoint point)
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
      if (MouseClickArea==ROOM_WELTALL && MouseClickId==999) Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
      else CStdRaum::OnLButtonDown(nFlags, point);
   }
}

//--------------------------------------------------------------------------------------------
//void CWeltAll::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CWeltAll::OnRButtonDown(UINT nFlags, CPoint point)
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
