//============================================================================================
// laptop.cpp : Der Laptop zum herumschleppen
//============================================================================================
// Link: "Laptop.h"
//============================================================================================
#include "stdafx.h"
#include "glglobeb.h"
#include "glglobec.h"
#include "glglobed.h"
#include "glglobe.h"

#define RDTSC __asm _emit 0x0F __asm _emit 0x31

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SLONG IconsPos[];

static XY GlobeOffset [] = { XY( 141, 16 ), XY( 177, 29 ) };

static const char FileId[] = "Lapt";

extern double _a;

class CWaitCursorNow
{
public:
   CWaitCursorNow ()
   {
      MouseWait++;
      pCursor->SetImage (gCursorSandBm.pBitmap);
   };

   ~CWaitCursorNow ()
   {
      MouseWait--;
      if (MouseWait==0) pCursor->SetImage (gCursorBm.pBitmap);
   };
};

//--------------------------------------------------------------------------------------------
//Konstruktor
//--------------------------------------------------------------------------------------------
CLaptop::CLaptop (BOOL bHandy, ULONG PlayerNum) : CPlaner (bHandy, PlayerNum, Sim.Players.Players[(SLONG)PlayerNum].EarthAlpha, TRUE)
{
   SLONG   c;
   PLAYER &qPlayer = Sim.Players.Players[(SLONG)PlayerNum];

   CWaitCursorNow wc; //CD-Cursor anzeigen

   CurrentBlock = -1;
   ScrollBlock  = -1;
   CWait MouseWait;

   FensterVisible = TRUE;

   KonstruktorFinished = 0;

   CPlaner::IsLaptop = TRUE;
   DragFlightMode    = FALSE;

   CurrentDragId     = -1;
   pBlock            = NULL;
   CurrentIcon       = -1;
   LastTime          = 0xffffffff;
   EarthTargetAlpha  = EarthAlpha;
   IsInClientArea    = FALSE;
   IsInClientAreaB   = FALSE;

   CurrentPostItType = 0;
   CurrentPostItId   = -1;

   Copyprotection    = (Sim.bExeChanged==1);

   KommVarLampe=-1;
   SP_Lampe.ReSize (4);
   SP_Lampe.Clips[0].ReSize (0, "LapLigV2.smk", "", XY(80,403), SPM_IDLE,      CRepeat(1,1), CPostWait(30,30), SMACKER_CLIP_CANCANCEL,
                             NULL, SMACKER_CLIP_SET, 0, &KommVarLampe,
                             "A1", 0);
   SP_Lampe.Clips[1].ReSize (1, "LapLigh.smk", "", XY(80,403), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                             &KommVarLampe, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, NULL,
                             "A3A1", 0, 2);
   SP_Lampe.Clips[2].ReSize (2, "LapLigW1.smk", "", XY(80,403), SPM_IDLE,      CRepeat(1,1), CPostWait(10,30), SMACKER_CLIP_DONTCANCEL,
                             &KommVarLampe, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, NULL,
                             "A1", 0);
   SP_Lampe.Clips[3].ReSize (3, "LapLigW1.smk", "", XY(80,403), SPM_IDLE,      CRepeat(1,1), CPostWait(5,5),   SMACKER_CLIP_DONTCANCEL,
                             &KommVarLampe, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, NULL,
                             "A1", 0);

   SP_Antivir.ReSize (1);
   SP_Antivir.Clips[0].ReSize (1, "Antivir.smk", "", XY(522,57), SPM_IDLE,      CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A1", 0);

   if (qPlayer.LaptopVirus)
   {
      EarthAlpha=EarthTargetAlpha=8000;

      for (c=0; c<6; c++)
      {
         SP_Buttons[c].ReSize (2);
         KommVarButtons[c]=-1;
      }

      SP_Buttons[1].Clips[0].ReSize (0, "TDMStand.smk", "", XY(88,53), SPM_IDLE,      CRepeat(1,1), CPostWait(30,30), SMACKER_CLIP_CANCANCEL,
                                     NULL, SMACKER_CLIP_SET, 0, &KommVarButtons[1], "A1", 0);
      SP_Buttons[1].Clips[1].ReSize (1, "TDM.smk", "tdm.raw", XY(88,53), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                                     &KommVarButtons[1], SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, NULL, "A1", 0);

      SP_Buttons[0].Clips[0].ReSize (0, "TDMStand.smk", "", XY(67,128), SPM_IDLE,      CRepeat(1,1), CPostWait(30,30), SMACKER_CLIP_CANCANCEL,
                                     NULL, SMACKER_CLIP_SET, 0, &KommVarButtons[0], "A1", 0);
      SP_Buttons[0].Clips[1].ReSize (1, "TDM.smk", "tdm.raw", XY(67,128), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                                     &KommVarButtons[0], SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, NULL, "A1", 0);

      SP_Buttons[4].Clips[0].ReSize (0, "TDMStand.smk", "", XY(58,207), SPM_IDLE,      CRepeat(1,1), CPostWait(30,30), SMACKER_CLIP_CANCANCEL,
                                     NULL, SMACKER_CLIP_SET, 0, &KommVarButtons[4], "A1", 0);
      SP_Buttons[4].Clips[1].ReSize (1, "TDM.smk", "tdm.raw", XY(58,207), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                                     &KommVarButtons[4], SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, NULL, "A1", 0);

      SP_Buttons[3].Clips[0].ReSize (0, "VirF_GLU.smk", "", XY(68,278), SPM_IDLE,      CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_CANCANCEL,
                                     NULL, SMACKER_CLIP_SET, 0, &KommVarButtons[3], "A1", 0);
      SP_Buttons[3].Clips[1].ReSize (1, "VirF_TOU.smk", "", XY(68,278), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                                     &KommVarButtons[3], SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, NULL, "A1", 0);

      SP_Buttons[2].Clips[0].ReSize (0, "VirR_GLU.smk", "", XY(94,349), SPM_IDLE,      CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_CANCANCEL,
                                     NULL, SMACKER_CLIP_SET, 0, &KommVarButtons[2], "A1", 0);
      SP_Buttons[2].Clips[1].ReSize (1, "VirR_TOU.smk", "", XY(94,349), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                                     &KommVarButtons[2], SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, NULL, "A1", 0);

      SP_Buttons[5].Clips[0].ReSize (0, "ok.smk", "", XY(545,359), SPM_IDLE,      CRepeat(1,1), CPostWait(30,30), SMACKER_CLIP_CANCANCEL,
                                     NULL, SMACKER_CLIP_SET, 0, NULL, "A1", 0);

      EarthBm.ReSize ((char*)(LPCTSTR)FullFilename ("EarthVir.lbm", GliPath), SYSRAMBM);

      for (c=qPlayer.Blocks.AnzEntries()-1; c>=1; c--)
         if (qPlayer.Blocks.IsInAlbum(c))
            qPlayer.Blocks-=c;
   }
   else EarthBm.ReSize ((char*)(LPCTSTR)FullFilename ("EarthLap.lbm", GliPath), SYSRAMBM);

   MessagePump();

   pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("globe.gli", RoomPath), &pGLibGlobe, L_LOCMEM);

   WarningLightModes[0]=WarningLightModes[1]=WarningLightModes[2]=0;

   if (Copyprotection)
      ReSize ("globe_d.gli", GFX_VR3);
   else
      if (qPlayer.LaptopBattery>0)
      {
         ReSize ("globe_b.gli", GFX_VR);
         WarningLightBms.ReSize (pRoomLib, "WARNING1", 3);

         UpdateWarningLightModes ();
      }
      else
         ReSize ("globe_c.gli", GFX_VR2);

   MapPlaneBms[0].ReSize (pRoomLib, "PL_B00", 1+8);
   MapPlaneBms[1].ReSize (pRoomLib, "PL_V00", 1+8);
   MapPlaneBms[2].ReSize (pRoomLib, "PL_R00", 1+8);
   MapPlaneBms[3].ReSize (pRoomLib, "PL_J00", 1+8);
   LockBm.ReSize (pRoomLib, "LOCK");

   KonstruktorFinished = 1;
   SDL_ShowWindow(FrameWnd->m_hWnd);
   FrameWnd->Invalidate(); MessagePump();
   FrameWnd->Invalidate(); MessagePump();

   HighlightBm.ReSize (pGLibGlobe, GFX_HELL);
   IconsDefaultBms.ReSize (pGLibGlobe, GFX_EXIT_000, GFX_CITY_000, GFX_PLANE_00, GFX_AUFTR_00, GFX_ROUTE_00, GFX_INFO_000, NULL);

   for (c=0; c<6; c++)
   {
      IconRotSpeed[c]=IconRot[c]=1000;
      pGLibIcons[c]=NULL;
   }

   MessagePump();

   if (Sim.Options.OptionEffekte)
   {
      StartupFX.ReInit("laptop.raw");
      StartupFX.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7);
      MessagePump();
   }

   MonitorEdges.ReSize (pGLibGlobe, "VR_EDGE1", 4);

   Window.ReSize (pRoomLib, GFX_WINDOW);
   BigWin.ReSize (pRoomLib, GFX_BIGWIN);
   Wallpapers.ReSize (pRoomLib, "BACK1", 6);
   Close.ReSize (pRoomLib, "CLOSE1", 2);
   DownLeft.ReSize (pRoomLib, "DWNLEFT1", 2);
   DownRight.ReSize (pRoomLib, "DWNRIGT1", 2);
   UpLeft.ReSize (pRoomLib, "UPLEFT1", 2);
   UpRight.ReSize (pRoomLib, "UPRIGHT1", 2);
   Inhalt.ReSize (pRoomLib, "INHALT0", 3);
   Jump.ReSize (pRoomLib, "JUMP0", 5);
   Switch.ReSize (pRoomLib, "SWITCH1", 7);
   Karte.ReSize (pRoomLib, GFX_KARTE);
   EckBms.ReSize (pRoomLib, GFX_LEFT1, GFX_RIGHT1, GFX_LEFT2, GFX_RIGHT2, NULL);
   PlaneRouteBms.ReSize (pRoomLib, GFX_PLANE, GFX_PLANE_B0, GFX_PLANE_B1, GFX_PLANE_V0, GFX_PLANE_V1, GFX_PLANE_R0, GFX_PLANE_R1, GFX_PLANE_G0, GFX_PLANE_G1,
                                   GFX_ROUTE, GFX_ROUTE_B0, GFX_ROUTE_B1, GFX_ROUTE_B2,
                                              GFX_ROUTE_V0, GFX_ROUTE_V1, GFX_ROUTE_V2,
                                              GFX_ROUTE_R0, GFX_ROUTE_R1, GFX_ROUTE_R2,
                                              GFX_ROUTE_G0, GFX_ROUTE_G1, GFX_ROUTE_G2, NULL);
   ScrollBms.ReSize (pRoomLib, GFX_SCROLLL, GFX_SCROLLR, NULL);

   qPlayer.Blocks.RepaintAll=FALSE;

   //Wenn jemand keinen Berater (mehr) hat, dann alle fremden Routen/Flugzeuge unsichtbar machen:
   if (!qPlayer.HasBerater(BERATERTYP_INFO))
   {
      for (SLONG c=0; c<Sim.Players.AnzPlayers; c++)
         if (c!=SLONG(PlayerNum))
         {
            qPlayer.DisplayRoutes[c]=FALSE;
            qPlayer.DisplayPlanes[c]=FALSE;
         }
   }

   //Und wenn jemand Pleite ist, dann den auch unsichtbar machen:
   for (c=0; c<Sim.Players.AnzPlayers; c++)
      if (c!=SLONG(PlayerNum) && Sim.Players.Players[c].IsOut)
      {
         qPlayer.DisplayRoutes[c]=FALSE;
         qPlayer.DisplayPlanes[c]=FALSE;
      }

   //Base-Pointer der Blöcke initialisieren:
   for (c=qPlayer.Blocks.AnzEntries(); c>=0; c--)
      if (qPlayer.Blocks.IsInAlbum(ULONG(c)))
         qPlayer.Blocks[c].Base=this;

   if (qPlayer.Blocks.IsInAlbum(ULONG(0)))
      qPlayer.Blocks[ULONG(0)].Base=NULL;

   for (c=qPlayer.Blocks.AnzEntries()-1; c>=1; c--)
   {
      if (qPlayer.Blocks.IsInAlbum(c))
      {
         if (qPlayer.Blocks[c].BlockType==2 && qPlayer.Blocks[c].Index==0 && !qPlayer.Planes.IsInAlbum (qPlayer.Blocks[c].SelectedId))
         {
            qPlayer.Blocks[c].Table.Destroy ();
            qPlayer.Blocks[c].Bitmap.Destroy ();
            qPlayer.Blocks -= c;
            if (CurrentBlock==c) CurrentBlock=-1;
         }
         else
         {
            BLOCK &qBlock = qPlayer.Blocks[ULONG(c)];

            qBlock.RefreshData (PlayerNum);
            qBlock.Page  = min (qBlock.Page, qBlock.AnzPages-1);
            qBlock.PageB = min (qBlock.PageB, qBlock.AnzPagesB-1);
            if (qPlayer.LaptopBattery>0) qBlock.Refresh (PlayerNum, TRUE);

            Limit ((SLONG)(49-qBlock.Bitmap.Size.x/2), qBlock.ScreenPos.x, (SLONG)(600-qBlock.Bitmap.Size.x/2));
            Limit ((SLONG)29, qBlock.ScreenPos.y, (SLONG)380);
         }
      }
      if (qPlayer.Blocks.IsInAlbum(c))
      {
         if (qPlayer.Blocks[c].Destructing==0 && qPlayer.Blocks[c].AnimationStart>Sim.TimeSlice-40)
         {
            qPlayer.Blocks[c].AnimationStart=-50;
            qPlayer.Blocks[c].Refresh (PlayerNum, TRUE);
         }
      }
   }

   for (c=0; c<4; c++)
   {
      MessagePump();

	   pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ((CString)bprintf ("icon%li.gli", c+1), RoomPath), &pGLibIcons[c], L_LOCMEM);

	   switch (c)
	   {
		  case 0: IconBms[0].ReSize (pGLibIcons[c], "EXIT_000", 51); break;
		  case 1: IconBms[1].ReSize (pGLibIcons[c], "CITY_000", 51); break;
		  case 2: IconBms[2].ReSize (pGLibIcons[c], "PLANE_00", 51); break;
		  case 3: IconBms[3].ReSize (pGLibIcons[c], "INFO_000", 51); break;
	   }
	}

   Sim.Players.Players[Sim.localPlayer].UpdateAuftragsUsage();

   for (c=0; c<4; c++)
      if (!Sim.Players.Players[c].IsOut)
         Sim.Players.Players[c].Planes.UpdateGlobePos (EarthAlpha);

   KonstruktorFinished = 2;

   PaintGlobe ();
   PaintGlobeRoutes ();
   SDL_ShowWindow(FrameWnd->m_hWnd);
   SDL_UpdateWindowSurface(FrameWnd->m_hWnd);
}

//--------------------------------------------------------------------------------------------
//Destruktor
//--------------------------------------------------------------------------------------------
CLaptop::~CLaptop()
{
   PicBitmap.Destroy();
   HighlightBm.Destroy();
   IconsDefaultBms.Destroy();
   MonitorEdges.Destroy();
   LockBm.Destroy();

   Window.Destroy();
   BigWin.Destroy();
   Wallpapers.Destroy();
   Close.Destroy();
   DownLeft.Destroy();
   DownRight.Destroy();
   UpLeft.Destroy();
   UpRight.Destroy();
   Inhalt.Destroy();
   Jump.Destroy();
   Switch.Destroy();
   Karte.Destroy();
   EckBms.Destroy();
   PlaneRouteBms.Destroy();
   ScrollBms.Destroy();
   WarningLightBms.Destroy();

   MapPlaneBms[0].Destroy();
   MapPlaneBms[1].Destroy();
   MapPlaneBms[2].Destroy();
   MapPlaneBms[3].Destroy();

   if (pGLibGlobe && pGfxMain) pGfxMain->ReleaseLib (pGLibGlobe);
   pGLibGlobe=NULL;

   for (SLONG c=0; c<6; c++)
   {
      if (pGLibIcons[c] && pGfxMain)
      {
         IconBms[c].Destroy();
         pGfxMain->ReleaseLib (pGLibIcons[c]);
         pGLibIcons[c]=NULL;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Globe message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//void CLaptop::OnPaint()
//--------------------------------------------------------------------------------------------
void CLaptop::OnPaint()
{
   SLONG   c;
   DWORD   Time=timeGetTime();
   static  DWORD LastTime;
   PLAYER &qPlayer = Sim.Players.Players[(SLONG)PlayerNum];
   XY      RoomPos = qPlayer.CursorPos;

   static SLONG LastHour;
   static ULONG RefreshTicker=0;

   if (KonstruktorFinished!=2)
   {
      if (KonstruktorFinished==1)
      {
         CStdRaum::OnPaint ();
         CStdRaum::InitToolTips ();
         CStdRaum::PostPaint ();
         CStdRaum::PumpToolTips ();
      }

      return;
   }

   //Blöcke: Tips löschen:
   for (c=qPlayer.Blocks.AnzEntries()-1; c>=1; c--)
      if (qPlayer.Blocks.IsInAlbum(c)) qPlayer.Blocks[c].SetTip (TIP_NONE, TIP_NONE, 0);

   RefreshTicker++;
   if ((RefreshTicker&1)==0 && qPlayer.LaptopBattery>0)
   {
      if (qPlayer.Blocks.IsInAlbum((RefreshTicker>>1)%qPlayer.Blocks.AnzEntries()))
         //qPlayer.Blocks[(RefreshTicker>>1)%qPlayer.Blocks.AnzEntries()].Refresh(PlayerNum, Background);
         qPlayer.Blocks[(RefreshTicker>>1)%qPlayer.Blocks.AnzEntries()].Refresh(Sim.localPlayer, TRUE);
   }

   static SLONG LastMinute;

   if (LastMinute!=Sim.GetMinute())
   {
      LastMinute=Sim.GetMinute();
      qPlayer.LaptopBattery--;

      if (qPlayer.LaptopBattery==2)
      {
         gUniversalFx.Stop();
         gUniversalFx.ReInit("lapwarn.raw");
         gUniversalFx.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7);
      }

      if (qPlayer.LaptopBattery==0)
      {
         PicBitmap.Destroy();
         HighlightBm.Destroy();
         IconsDefaultBms.Destroy();
         MonitorEdges.Destroy();
         LockBm.Destroy();

         Window.Destroy();
         BigWin.Destroy();
         Wallpapers.Destroy();
         Close.Destroy();
         DownLeft.Destroy();
         DownRight.Destroy();
         UpLeft.Destroy();
         UpRight.Destroy();
         Inhalt.Destroy();
         Jump.Destroy();
         Switch.Destroy();
         Karte.Destroy();
         EckBms.Destroy();
         PlaneRouteBms.Destroy();
         ScrollBms.Destroy();
         WarningLightBms.Destroy();

         MapPlaneBms[0].Destroy();
         MapPlaneBms[1].Destroy();
         MapPlaneBms[2].Destroy();
         MapPlaneBms[3].Destroy();

         if (Sim.Options.OptionEffekte)
         {
            gUniversalFx.Stop();
            gUniversalFx.ReInit("lapdown.raw");
            gUniversalFx.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7);
         }

         ReSize ("globe_c.gli", GFX_VR2);
         //PicBitmap.ReSize (pRoomLib, GFX_VR2);
      }
   }

   if (Sim.GetHour()!=LastHour)
   {
      Sim.Players.Players[Sim.localPlayer].UpdateAuftragsUsage();
      LastHour=Sim.GetHour();
   }

   if (UsedToRotate==2) UsedToRotate=1;

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();
   CStdRaum::InitToolTips ();

   if (Sim.Players.Players[(SLONG)PlayerNum].LaptopBattery>0)
   {
      SP_Lampe.Pump ();
      SP_Lampe.BlitAtT (RoomBm);
   }

   if (Sim.Players.Players[(SLONG)PlayerNum].SecurityFlags&(1<<1))
   {
      SP_Antivir.Pump ();
      SP_Antivir.BlitAtT (RoomBm);
   }

   //CPlaner::DoPollingStuff ();

   //Laptop-Inhalt nur zeichnen, wenn die Batterie voll ist:
   if (qPlayer.LaptopBattery>0 && !Copyprotection)
   {
      //Die Warnlampen für die Flüge:
      if (WarningLightModes[0]==1 || (WarningLightModes[0]==2 && (Time&1023)<512)) RoomBm.BlitFrom (WarningLightBms[0], 59, 40);
      if (WarningLightModes[1]==1 || (WarningLightModes[1]==2 && (Time&1023)<512)) RoomBm.BlitFrom (WarningLightBms[1], 48, 61);
      if (WarningLightModes[2]==1 || (WarningLightModes[2]==2 && (Time&1023)<512)) RoomBm.BlitFrom (WarningLightBms[2], 60, 82);

      if (!IsDialogOpen() && !MenuIsOpen())
      {
         //Alle 10 Minuten ein Repaint erzwingen:
         if (LastPaintedMinute!=Sim.GetMinute() && Sim.GetMinute()%10==0)
         {
            for (c=0; c<(SLONG)qPlayer.Blocks.AnzEntries(); c++)
               if (qPlayer.Blocks.IsInAlbum(c))
                  qPlayer.Blocks[c].Refresh (PlayerNum, TRUE);
         }
         LastPaintedMinute=Sim.GetMinute();

         ///////////////////////////////////////////////////////////////////////////////////////////
         //// Mouse evaluation                                                                  ////
         ///////////////////////////////////////////////////////////////////////////////////////////

         if (gMousePosition.IfIsWithin (37, 398, 74, 439)) SetMouseLook (CURSOR_EXIT, 0, ROOM_LAPTOP, 999);

         //Is Cursor over Block?
         CurrentBlock    = -1;
         IsInClientArea  = FALSE;
         IsInClientAreaB = FALSE;
         pBlock          = NULL;
         for (c=qPlayer.Blocks.AnzEntries()-1; c>=1; c--)
            if (FensterVisible && qPlayer.Blocks.IsInAlbum(c) && qPlayer.Blocks[c].Destructing==0)
            {
               qPlayer.Blocks[c].IsTopWindow  = FALSE;

               if (gMousePosition.IfIsWithin (qPlayer.Blocks[c].ScreenPos, qPlayer.Blocks[c].ScreenPos+qPlayer.Blocks[c].Bitmap.Size-XY(1,1)))
                  if (qPlayer.Blocks[c].Bitmap.GetPixel ((gMousePosition-qPlayer.Blocks[c].ScreenPos).x, (gMousePosition-qPlayer.Blocks[c].ScreenPos).y)!=0)
                  {
                     CurrentBlock = c;
                     pBlock       = &qPlayer.Blocks[c];

                     CurrentBlockPos = gMousePosition - pBlock->ScreenPos;

                     if (CurrentBlockPos.IfIsWithin (20,21, 20+172,21+170))
                     {
                        IsInClientArea = TRUE;
                        ClientPos = CurrentBlockPos-XY(20,21);
                        ReferenceCursorPos = gMousePosition;
                     }
                     else IsInClientArea = FALSE;

                     if (CurrentBlockPos.IfIsWithin (20+187,21, 20+172+187,21+170))
                     {
                        IsInClientAreaB = TRUE;
                        ClientPosB = CurrentBlockPos-XY(20+187,21);
                        ReferenceCursorPos = gMousePosition;
                     }
                     else IsInClientAreaB = FALSE;
                  }
             }

         //Top-Fenster markieren
         for (c=1; c<(SLONG)qPlayer.Blocks.AnzEntries(); c++)
            if (FensterVisible && qPlayer.Blocks.IsInAlbum(c) && qPlayer.Blocks[c].Destructing==0)
            {
               qPlayer.Blocks[c].IsTopWindow  = TRUE;
               break;
            }

         if (gMouseLButton)
         {
            if (ScrollBlock!=-1 && qPlayer.Blocks.IsInAlbum(ScrollBlock))
            {
               if (ScrollSide==0)
               {
                  SLONG NewPos = (gMousePosition.y-qPlayer.Blocks[ScrollBlock].ScreenPos.y-ScrollOffset.y-43)
                                 *qPlayer.Blocks[ScrollBlock].PageSize*(qPlayer.Blocks[ScrollBlock].AnzPages-1)/(138-43);

                  if (NewPos<0) NewPos=0;

                  if (NewPos>(qPlayer.Blocks[ScrollBlock].AnzPages-1)*qPlayer.Blocks[ScrollBlock].PageSize)
                     NewPos=(qPlayer.Blocks[ScrollBlock].AnzPages-1)*qPlayer.Blocks[ScrollBlock].PageSize;

                  if (NewPos!=qPlayer.Blocks[ScrollBlock].Page)
                  {
                     qPlayer.Blocks[ScrollBlock].Page=NewPos;
                     qPlayer.Blocks[ScrollBlock].Refresh (PlayerNum, IsLaptop);
                  }
               }
               else if (ScrollSide==1)
               {
                  SLONG NewPos = (gMousePosition.y-qPlayer.Blocks[ScrollBlock].ScreenPos.y-ScrollOffset.y-43)
                                 *qPlayer.Blocks[ScrollBlock].PageSizeB*(qPlayer.Blocks[ScrollBlock].AnzPagesB-1)/(138-43);

                  if (NewPos<0) NewPos=0;

                  if (NewPos>(qPlayer.Blocks[ScrollBlock].AnzPagesB-1)*qPlayer.Blocks[ScrollBlock].PageSizeB)
                     NewPos=(qPlayer.Blocks[ScrollBlock].AnzPagesB-1)*qPlayer.Blocks[ScrollBlock].PageSizeB;

                  if (NewPos!=qPlayer.Blocks[ScrollBlock].PageB)
                  {
                     qPlayer.Blocks[ScrollBlock].PageB=NewPos;
                     qPlayer.Blocks[ScrollBlock].Refresh (PlayerNum, IsLaptop);
                  }
               }
            }
         }
         else ScrollBlock=-1;

         CPlaner::DoPollingStuff ();

         if (pBlock)
         {
            if (pBlock->IsTopWindow)
            {
               if (pBlock->Page/pBlock->PageSize<pBlock->AnzPages-1 && CurrentBlockPos.IfIsWithin (0,165,19,186))
               {
                  SetMouseLook (CURSOR_HOT, 0, ROOM_LAPTOP, 100, 31);  //Next
                  if (gMouseLButton && timeGetTime()-gMouseLButtonDownTimer>1000)
                  {
                     HandleLButtonDown();
                     gMouseLButtonDownTimer = timeGetTime()-870;
                  }
               }
               else if (pBlock->Page>0 && CurrentBlockPos.IfIsWithin (0,23,19,44))
               {
                  SetMouseLook (CURSOR_HOT, 0, ROOM_LAPTOP, 100, 30);  //Prev
                  if (gMouseLButton && timeGetTime()-gMouseLButtonDownTimer>1000)
                  {
                     HandleLButtonDown();
                     gMouseLButtonDownTimer = timeGetTime()-870;
                  }
               }
               else if (pBlock->Index!=1 && CurrentBlockPos.IfIsWithin (16,0,33,19))           //Klick auf "Index"...
                  SetMouseLook (CURSOR_HOT, 0, ROOM_LAPTOP, 100, 35);
               else if (pBlock->AnzPages>1 && CurrentBlockPos.IfIsWithin (5, 43+(138-43)*pBlock->Page/pBlock->PageSize/(pBlock->AnzPages-1), 19, 43+29+(138-43)*pBlock->Page/pBlock->PageSize/(pBlock->AnzPages-1)))
                  SetMouseLook (CURSOR_HOT, 0, ROOM_LAPTOP, 100, 88);

               if (pBlock->DoubleBlock)
               {
                  if (pBlock->PageB/pBlock->PageSizeB<pBlock->AnzPagesB-1 && CurrentBlockPos.IfIsWithin (375,165,395,186)) //"Next" B...
                  {
                     SetMouseLook (CURSOR_HOT, 0, ROOM_LAPTOP, 100, 41);
                     if (gMouseLButton && timeGetTime()-gMouseLButtonDownTimer>1000)
                     {
                        OnLButtonDown (0,CPoint(0,0));
                        gMouseLButtonDownTimer = timeGetTime()-870;
                     }
                  }
                  else if (pBlock->PageB>0 && CurrentBlockPos.IfIsWithin (375,23,395,44))   //"Prev" B...
                  {
                     SetMouseLook (CURSOR_HOT, 0, ROOM_LAPTOP, 100, 40);
                     if (gMouseLButton && timeGetTime()-gMouseLButtonDownTimer>1000)
                     {
                        OnLButtonDown (0,CPoint(0,0));
                        gMouseLButtonDownTimer = timeGetTime()-870;
                     }
                  }
                  else if (pBlock->IndexB!=1 && CurrentBlockPos.IfIsWithin (202,0,219,19))   //Klick auf "Index" B...
                     SetMouseLook (CURSOR_HOT, 0, ROOM_LAPTOP, 100, 45);
                  else if (CurrentBlockPos.IfIsWithin (359,0,380,20))  //Klick auf "CloseWin"...
                     SetMouseLook (CURSOR_HOT, 0, ROOM_LAPTOP, 100, 50);
                  else if (pBlock->BlockType==2 && pBlock->IndexB==1 && pBlock->BlockTypeB!=3 && CurrentBlockPos.IfIsWithin (217,5,236,5+16))  //Switch...
                     SetMouseLook (CURSOR_HOT, 0, ROOM_LAPTOP, 100, 60);
                  else if (pBlock->BlockType==2 && pBlock->IndexB==1 && pBlock->BlockTypeB!=6 && CurrentBlockPos.IfIsWithin (237,5,256,5+16))  //Switch...
                     SetMouseLook (CURSOR_HOT, 0, ROOM_LAPTOP, 100, 61);
                  else if (pBlock->BlockType==2 && pBlock->IndexB==1 && pBlock->BlockTypeB!=4 && CurrentBlockPos.IfIsWithin (257,5,276,5+16))  //Switch...
                     SetMouseLook (CURSOR_HOT, 0, ROOM_LAPTOP, 100, 62);
                  else if (pBlock->IndexB!=1 && CurrentBlockPos.IfIsWithin (331,4,331+10,4+15))  //JumpL
                     SetMouseLook (CURSOR_HOT, 0, ROOM_LAPTOP, 100, 172);
                  else if (pBlock->IndexB!=1 && CurrentBlockPos.IfIsWithin (331+11,4,331+21,4+15))  //JumpR
                     SetMouseLook (CURSOR_HOT, 0, ROOM_LAPTOP, 100, 173);
                  else if (pBlock->AnzPagesB>1 && CurrentBlockPos.IfIsWithin (376, 43+(138-43)*pBlock->PageB/pBlock->PageSizeB/(pBlock->AnzPagesB-1), 376+15, 43+29+(138-43)*pBlock->PageB/pBlock->PageSizeB/(pBlock->AnzPagesB-1)))
                     SetMouseLook (CURSOR_HOT, 0, ROOM_LAPTOP, 100, 89);
               }
               else
               {
                  if (CurrentBlockPos.IfIsWithin (173,3,191,19))  //Klick auf "CloseWin"...
                     SetMouseLook (CURSOR_HOT, 0, ROOM_LAPTOP, 100, 50);
               }

               if (pBlock->Index!=1 && pBlock->BlockType!=5 && CurrentBlockPos.IfIsWithin (145+22,4,145+10+22,4+15))  //JumpL
                  SetMouseLook (CURSOR_HOT, 0, ROOM_LAPTOP, 100, 170);
               else if (pBlock->Index!=1 && pBlock->BlockType!=5 && CurrentBlockPos.IfIsWithin (145+11+22,4,145+21+22,4+15))  //JumpR
                  SetMouseLook (CURSOR_HOT, 0, ROOM_LAPTOP, 100, 171);
            }
         }
         else
         {
            //Planes/Routen Buttons
            if (gMousePosition.IfIsWithin (66, 277, 66+23, 277+23)       && !Sim.Players.Players[0].IsOut) SetMouseLook (CURSOR_HOT, 1, bprintf (StandardTexte.GetS (TOKEN_TOOLTIP, 3006), Sim.Players.Players[0].AirlineX), ROOM_LAPTOP, 100, 70);
            if (gMousePosition.IfIsWithin (66, 277+23, 66+23, 277+46)    && !Sim.Players.Players[1].IsOut) SetMouseLook (CURSOR_HOT, 1, bprintf (StandardTexte.GetS (TOKEN_TOOLTIP, 3006), Sim.Players.Players[1].AirlineX), ROOM_LAPTOP, 100, 71);
            if (gMousePosition.IfIsWithin (66+23, 277, 66+46, 277+23)    && !Sim.Players.Players[2].IsOut) SetMouseLook (CURSOR_HOT, 1, bprintf (StandardTexte.GetS (TOKEN_TOOLTIP, 3006), Sim.Players.Players[2].AirlineX), ROOM_LAPTOP, 100, 72);
            if (gMousePosition.IfIsWithin (66+23, 277+23, 66+46, 277+46) && !Sim.Players.Players[3].IsOut) SetMouseLook (CURSOR_HOT, 1, bprintf (StandardTexte.GetS (TOKEN_TOOLTIP, 3006), Sim.Players.Players[3].AirlineX), ROOM_LAPTOP, 100, 73);
            if (gMousePosition.IfIsWithin (91+1, 346+2, 91+1+23, 346+2+23)       && !Sim.Players.Players[0].IsOut) SetMouseLook (CURSOR_HOT, 1, bprintf (StandardTexte.GetS (TOKEN_TOOLTIP, 3007), Sim.Players.Players[0].AirlineX), ROOM_LAPTOP, 100, 80);
            if (gMousePosition.IfIsWithin (91+1, 346+2+23, 91+1+23, 346+2+46)    && !Sim.Players.Players[1].IsOut) SetMouseLook (CURSOR_HOT, 1, bprintf (StandardTexte.GetS (TOKEN_TOOLTIP, 3007), Sim.Players.Players[1].AirlineX), ROOM_LAPTOP, 100, 81);
            if (gMousePosition.IfIsWithin (91+1+23, 346+2, 91+1+46, 346+2+23)    && !Sim.Players.Players[2].IsOut) SetMouseLook (CURSOR_HOT, 1, bprintf (StandardTexte.GetS (TOKEN_TOOLTIP, 3007), Sim.Players.Players[2].AirlineX), ROOM_LAPTOP, 100, 82);
            if (gMousePosition.IfIsWithin (91+1+23, 346+2+23, 91+1+46, 346+2+46) && !Sim.Players.Players[3].IsOut) SetMouseLook (CURSOR_HOT, 1, bprintf (StandardTexte.GetS (TOKEN_TOOLTIP, 3007), Sim.Players.Players[3].AirlineX), ROOM_LAPTOP, 100, 83);

            //Kein Planes/Routes Button, wenn man den entsprechenden Informaten nicht hat:
            if (MouseClickPar1>=70 && MouseClickPar1<=83 && MouseClickArea==ROOM_LAPTOP && MouseClickId==100 && !Sim.Players.Players[(SLONG)PlayerNum].HasBerater(BERATERTYP_INFO))
            {
               if (MouseClickPar1>=70 && MouseClickPar1<=73 && PlayerNum!=MouseClickPar1-70) SetMouseLook (CURSOR_NORMAL, 0, ROOM_LAPTOP, 0, 0);
               if (MouseClickPar1>=80 && MouseClickPar1<=83 && PlayerNum!=MouseClickPar1-80) SetMouseLook (CURSOR_NORMAL, 0, ROOM_LAPTOP, 0, 0);
            }

            //Display Windows?
            if (gMousePosition.IfIsWithin (608,312,638,350)) SetMouseLook (CURSOR_HOT, 0, ROOM_LAPTOP, 100, 90);
         }

         //Is Cursor over Icon?
         CurrentIcon = -1;

         if (CurrentBlock==-1)
         {
            for (c=0; c<12; c++)
               if ((c<6 && c!=3 && c!=4 && RoomPos.IfIsWithin (IconsPos[c*2+24], IconsPos[c*2+1+24], IconsPos[c*2+24]+HighlightBm.Size.x, IconsPos[c*2+1+24]+HighlightBm.Size.y)) ||
                   (c>=9 && RoomPos.IfIsWithin (IconsPos[c*2+24], IconsPos[c*2+1+24], IconsPos[c*2+24]+FlugplanIconBms[c][0].Size.x, IconsPos[c*2+1+24]+FlugplanIconBms[c][0].Size.y)) ||
                   ((c>=6 && c<9) && RoomPos.IfIsWithin (IconsPos[c*2+24], IconsPos[c*2+1+24], IconsPos[c*2+24]+FlugplanIconBms[c][0].Size.x, IconsPos[c*2+1+24]+FlugplanIconBms[c][0].Size.y)))
               {
                  CurrentIcon = c;

                  if (c<6 || c>9)
                     if (c==0) SetMouseLook (CURSOR_EXIT, 3000+CurrentIcon, ROOM_LAPTOP, 100, CurrentIcon);
                          else SetMouseLook (CURSOR_HOT, 3000+CurrentIcon, ROOM_LAPTOP, 100, CurrentIcon);
                  //SetToolTip (3000+CurrentIcon, RoomPos);
               }
         }

         //Window moving:
         if (CurrentDragId!=-1 && (qPlayer.Buttons&1))
         {
            qPlayer.Blocks[CurrentDragId].ScreenPos = gMousePosition-DragOffset;
            Limit ((SLONG)(49-qPlayer.Blocks[CurrentDragId].Bitmap.Size.x/2), qPlayer.Blocks[CurrentDragId].ScreenPos.x, (SLONG)(600-qPlayer.Blocks[CurrentDragId].Bitmap.Size.x/2));
            Limit ((SLONG)29, qPlayer.Blocks[CurrentDragId].ScreenPos.y, (SLONG)380);
         }

         //Ggf. Icons beschleunigen lassen:
         if (CurrentBlock==-1 && CurrentDragId==-1 && MouseClickPar1<6 && MouseClickArea==ROOM_LAPTOP && MouseClickId==100)
         {
            IconRotSpeed[MouseClickPar1]+=(Time-LastTime)*2;
            if (IconRotSpeed[MouseClickPar1]>1600) IconRotSpeed[MouseClickPar1]=1600;
         }

         //Ggf. Erde rotieren lassen:
         if (CurrentBlock==-1 && CurrentDragId==-1 && (qPlayer.Buttons&1))
         {
            if (MouseClickArea==ROOM_LAPTOP && MouseClickId==100 && MouseClickPar1==11)
               CPlaner::TurnGlobe (1000);   //Turn Left
            else if (MouseClickArea==ROOM_LAPTOP && MouseClickId==100 && MouseClickPar1==10)
               CPlaner::TurnGlobe (-1000);  //Turn Right
         }
      }
      else CPlaner::DoPollingStuff ();

      ///////////////////////////////////////////////////////////////////////////////////////////
      //// Painting                                                                          ////
      ///////////////////////////////////////////////////////////////////////////////////////////

      //Icon Rotation
      for (c=0; c<6; c++)
      {
         IconRot[c]+=IconRotSpeed[c];
         IconRotSpeed[c]-=20;
         if (IconRotSpeed[c]<0) IconRotSpeed[c]=0;
      }

      if (bActive)
      {
         PaintGlobeInScreen (GlobeOffset[1]);

         if (Sim.Players.Players[(SLONG)PlayerNum].LaptopVirus)
         {
            for (c=0; c<6; c++)
            {
               SP_Buttons[c].Pump ();
               SP_Buttons[c].BlitAtT (RoomBm);
            }

            for (c=10; c<12; c++)
               RoomBm.BlitFromT (FlugplanIconBms[c][c==CurrentIcon], IconsPos[c*2+24], IconsPos[c*2+1+24]);
         }
         else
         {
            for (c=0; c<12; c++)
               if (c<6 && c!=3 && c!=4)
               {
                  if (IconRot[c] && pGLibIcons[c]==0)
                  {
                     pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ((CString)bprintf ("icon%li.gli", c+1), RoomPath), &pGLibIcons[c], L_LOCMEM);

                     switch (c)
                     {
                        case 0: IconBms[0].ReSize (pGLibIcons[c], "EXIT_000", 51); break;
                        case 1: IconBms[1].ReSize (pGLibIcons[c], "CITY_000", 51); break;
                        case 2: IconBms[2].ReSize (pGLibIcons[c], "PLANE_00", 51); break;
                        case 3: IconBms[3].ReSize (pGLibIcons[c], "AUFTR_00", 51); break;
                        case 4: IconBms[4].ReSize (pGLibIcons[c], "ROUTE_00", 51); break;
                        case 5: IconBms[5].ReSize (pGLibIcons[c], "INFO_000", 51); break;
                     }
                  }

                  if (c==CurrentIcon)
                     RoomBm.BlitFromT (HighlightBm, IconsPos[c*2+24], IconsPos[c*2+1+24]);

                  if (IconRot[c])
                     RoomBm.BlitFromT (IconBms[c][(IconRot[c]>>10)%51], IconsPos[c*2+24], IconsPos[c*2+1+24]);
                  else
                     RoomBm.BlitFromT (IconsDefaultBms[c], IconsPos[c*2+24], IconsPos[c*2+1+24]);
               }
               else if (c>9)
                  RoomBm.BlitFromT (FlugplanIconBms[c][c==CurrentIcon], IconsPos[c*2+24], IconsPos[c*2+1+24]);

            RoomBm.BlitFromT (PlaneRouteBms[0], 66, 277);
            RoomBm.BlitFromT (PlaneRouteBms[9], 91+1, 346+2);

            //Highlight, wenn gedrückt:
            if (qPlayer.DisplayPlanes[0]) RoomBm.BlitFromT (PlaneRouteBms[2], 66, 277);
            if (qPlayer.DisplayPlanes[1]) RoomBm.BlitFromT (PlaneRouteBms[4], 66, 277+23);
            if (qPlayer.DisplayPlanes[2]) RoomBm.BlitFromT (PlaneRouteBms[6], 66+23, 277);
            if (qPlayer.DisplayPlanes[3]) RoomBm.BlitFromT (PlaneRouteBms[8], 66+23, 277+23);
            if (qPlayer.DisplayRoutes[0]) RoomBm.BlitFromT (PlaneRouteBms[10+qPlayer.DisplayRoutes[0]], 91+1, 346+2);
            if (qPlayer.DisplayRoutes[1]) RoomBm.BlitFromT (PlaneRouteBms[13+qPlayer.DisplayRoutes[1]], 91+1, 346+2+23);
            if (qPlayer.DisplayRoutes[2]) RoomBm.BlitFromT (PlaneRouteBms[16+qPlayer.DisplayRoutes[2]], 91+1+24, 346+2);
            if (qPlayer.DisplayRoutes[3]) RoomBm.BlitFromT (PlaneRouteBms[19+qPlayer.DisplayRoutes[3]], 91+1+24, 346+2+23);

            //Highlight, wenn Cursor drüber
            if (MouseClickArea==ROOM_LAPTOP && MouseClickId==100)
            {
               if (MouseClickPar1==70) RoomBm.BlitFromT (PlaneRouteBms[1], 66+9, 277+8);
               if (MouseClickPar1==71) RoomBm.BlitFromT (PlaneRouteBms[3], 65+9, 277+23);
               if (MouseClickPar1==72) RoomBm.BlitFromT (PlaneRouteBms[5], 65+24, 277+8);
               if (MouseClickPar1==73) RoomBm.BlitFromT (PlaneRouteBms[7], 65+24, 277+23);
               if (MouseClickPar1==80) RoomBm.BlitFromT (PlaneRouteBms[10], 91+1+9, 346+2+8);
               if (MouseClickPar1==81) RoomBm.BlitFromT (PlaneRouteBms[13], 91+1+9, 346+2+23);
               if (MouseClickPar1==82) RoomBm.BlitFromT (PlaneRouteBms[16], 91+1+24, 346+2+8);
               if (MouseClickPar1==83) RoomBm.BlitFromT (PlaneRouteBms[19], 91+1+24, 346+2+23);
            }
         }

         RoomBm.pBitmap->SetClipRect (CRect (39,29,600,409));

         //Blöcke zeichnen:
         for (c=qPlayer.Blocks.AnzEntries()-1; c>=1; c--)
            if (qPlayer.Blocks.IsInAlbum(c) && FensterVisible)
            {
               BLOCK &qBlock=qPlayer.Blocks[c];

               qBlock.UpdateTip(PlayerNum, TRUE);

               qBlock.Refresh(PlayerNum, TRUE);

               qBlock.BlitAt (RoomBm);

               //Jump-Bms
               if (qBlock.Destructing==0)
               {
                  //Switch Icons:
                  if (qBlock.BlockType==2)
                  {
                     if (qBlock.IndexB==1 && qBlock.TipInUseB==TIP_NONE)
                     {
                        if (qBlock.BlockTypeB==3) RoomBm.BlitFromT (Switch[0], qBlock.ScreenPos+XY(217,5));
                        if (qBlock.BlockTypeB==6) RoomBm.BlitFromT (Switch[1], qBlock.ScreenPos+XY(237,5));
                        if (qBlock.BlockTypeB==4) RoomBm.BlitFromT (Switch[2], qBlock.ScreenPos+XY(257,5));
                     }
                  }

                  if (qBlock.Index!=1 && qBlock.BlockType!=5)   RoomBm.BlitFromT (Jump[0], qBlock.ScreenPos+XY(145+22, 4));
                  if (qBlock.IndexB!=1 && qBlock.BlockTypeB!=5) RoomBm.BlitFromT (Jump[0], qBlock.ScreenPos+XY(331, 4));
               }

               //Highlight the icons?
               if (c==CurrentBlock && MouseClickArea==ROOM_LAPTOP && MouseClickId==100)
               {
                  if (MouseClickPar1==30) RoomBm.BlitFromT (UpLeft[0+gMouseLButton],    pBlock->ScreenPos+XY(0, 80-58));
                  else if (MouseClickPar1==31) RoomBm.BlitFromT (DownLeft[0+gMouseLButton],  pBlock->ScreenPos+XY(0, 105+63));
                  else if (MouseClickPar1==35) RoomBm.BlitFromT (Inhalt[1+gMouseLButton],    pBlock->ScreenPos+XY(16,0));
                  else if (MouseClickPar1==40) RoomBm.BlitFromT (UpRight[0+gMouseLButton],   pBlock->ScreenPos+XY(376,81-58-1));
                  else if (MouseClickPar1==41) RoomBm.BlitFromT (DownRight[0+gMouseLButton], pBlock->ScreenPos+XY(376,105+63+1));
                  else if (MouseClickPar1==45) RoomBm.BlitFromT (Inhalt[1+gMouseLButton],    pBlock->ScreenPos+XY(203,0));

                  if (MouseClickPar1==60) RoomBm.BlitFromT (Switch[3], pBlock->ScreenPos+XY(217,5));
                  if (MouseClickPar1==61) RoomBm.BlitFromT (Switch[4], pBlock->ScreenPos+XY(237,5));
                  if (MouseClickPar1==62) RoomBm.BlitFromT (Switch[5], pBlock->ScreenPos+XY(257,5));
                  if (MouseClickPar1==50)
                     if (pBlock->DoubleBlock) RoomBm.BlitFromT (Close[0+gMouseLButton], pBlock->ScreenPos+XY(360,0));
                                         else RoomBm.BlitFromT (Close[0+gMouseLButton], pBlock->ScreenPos+XY(174,0));

                       if (MouseClickPar1==170) RoomBm.BlitFromT (Jump[1+gMouseLButton], pBlock->ScreenPos+XY(145+22, 4));
                  else if (MouseClickPar1==171) RoomBm.BlitFromT (Jump[3+gMouseLButton], pBlock->ScreenPos+XY(145+22, 4));
                  else if (MouseClickPar1==172) RoomBm.BlitFromT (Jump[1+gMouseLButton], pBlock->ScreenPos+XY(331, 4));
                  else if (MouseClickPar1==173) RoomBm.BlitFromT (Jump[3+gMouseLButton], pBlock->ScreenPos+XY(331, 4));
               }
               else if (c==CurrentBlock && MouseClickArea==ROOM_LAPTOP && pBlock->IsTopWindow)
               {
                       if (MouseClickId==150) RoomBm.BlitFromT (FlugplanBms[52],  pBlock->ScreenPos+XY(45,67)+XY(0, 105));
                  else if (MouseClickId==151) RoomBm.BlitFromT (FlugplanBms[53],  pBlock->ScreenPos+XY(34,67)+XY(120, 105));
               }
            }

         RoomBm.pBitmap->SetClipRect (CRect (0,0,640,480));

         RoomBm.BlitFromT (MonitorEdges[0], 39, 29);
         RoomBm.BlitFromT (MonitorEdges[1], 601-MonitorEdges[1].Size.x, 29);
         RoomBm.BlitFromT (MonitorEdges[2], 39, 410-MonitorEdges[2].Size.y);
         RoomBm.BlitFromT (MonitorEdges[3], 601-MonitorEdges[3].Size.x, 410-MonitorEdges[3].Size.y);

         CPlaner::DoPostPaintPollingStuff(XY(20,21)+XY(21,-2)+XY(3,20));
      }

      ///////////////////////////////////////////////////////////////////////////////////////////
      //// Cursor & Onscreen Stuff                                                           ////
      ///////////////////////////////////////////////////////////////////////////////////////////
      if (CurrentPostItType!=0)
      {
         XY PostItPos = gMousePosition-PostItBm.Size/SLONG(2);

         if (ClientPos.IfIsWithin (24,17,167,149))
         {
            //An diese Stelle (links vom Cursor) kommt der Flug hin:
            SLONG Date=Sim.Date+((ClientPos-PostItBm.Size/SLONG(2)+XY(3,9)).y-17)/19;
            SLONG Time=((ClientPos-PostItBm.Size/SLONG(2)+XY(3,9)).x-24)/6;

            PostItPos.x = Time*6+24+1;
            PostItPos.y = (Date-Sim.Date)*19+17+1;

            PostItPos += (gMousePosition-ClientPos);
         }

         if (!RoomBm.BlitFromT (PostItBm, PostItPos))
         {
            PaintPostIt ();
            RoomBm.BlitFromT (PostItBm, PostItPos);
         }
      }

      LastTime=Time;
   }
   else
   {
      CPlaner::DoPollingStuff ();
      CurrentBlock=-1;
   }

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::PostPaint ();
   CStdRaum::PumpToolTips ();
}

//--------------------------------------------------------------------------------------------
//void CLaptop::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CLaptop::OnLButtonDown(UINT nFlags, CPoint point)
{
   SLONG c;
   XY    RoomPos;       //Position im Raum
   XY    ClientPos;     //Position in der Client-Area des Blocks
   PLAYER &qPlayer = Sim.Players.Players[(SLONG)PlayerNum];

   if (gMousePosition.y>=440)
   {
      PreLButtonDown (gMousePosition);
      DefaultOnLButtonDown();
      CStdRaum::OnLButtonDown(nFlags, point);
      return;
   }

   if (DragFlightMode || MouseClickId==105) { DefaultOnLButtonDown (); return; }

   if (!KonstruktorFinished)
      return;

   if (!ConvertMousePosition (point, &RoomPos))
   {
      return;
   }

   //Laptop Buttons:
   if (MouseClickArea==ROOM_LAPTOP && MouseClickId==999)
   {
      PlayUniversalFx ("laptop1.raw", Sim.Options.OptionEffekte);
      Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
   }
   else if (gMousePosition.IfIsWithin (74, 398, 111, 439))
   {
      PlayUniversalFx ("laptop2.raw", Sim.Options.OptionEffekte);
   }
   else if (gMousePosition.IfIsWithin (608, 313, 640, 350))
   {
      PlayUniversalFx ("laptop3.raw", Sim.Options.OptionEffekte);
   }
   else if (gMousePosition.IfIsWithin (608, 358, 640, 394))
   {
      PlayUniversalFx ("laptop4.raw", Sim.Options.OptionEffekte);
   }

   if (Copyprotection) return;
   if (PreLButtonDown (gMousePosition) || qPlayer.LaptopBattery<=0) return;

   CPlaner::HandleLButtonDown ();

   //Klick auf ein FLugzeug auf der Weltkarte:
   if (CurrentBlock==-1 && MouseClickArea==ROOM_LAPTOP && MouseClickId==101)
      if (Sim.Players.Players[(SLONG)PlayerNum].LaptopVirus==0)
      {
         ULONG Id;

         KommVarLampe=1;  //richtig flackern

         Id = Sim.Players.Players[PlayerNum].Blocks.GetUniqueId();

         Sim.Players.Players[PlayerNum].Blocks += Id;

         BLOCK &qBlock = Sim.Players.Players[(SLONG)PlayerNum].Blocks[Id];

         qBlock.PlayerNum  = PlayerNum;
         qBlock.ScreenPos  = point;

         qBlock.Destructing    = 0;
         qBlock.AnimationStart = Sim.TimeSlice;

         qBlock.BlockType  = 2;
         qBlock.SelectedId = MouseClickPar1;
         qBlock.Index      = FALSE;
         qBlock.Page       = 0;
         qBlock.Base       = this;

         qBlock.UpdatePageSize ();

         for (SLONG d=0; d<6; d++)
         {
            qBlock.Indexes[d]     = 1;
            qBlock.SelectedIds[d] = 0;
            qBlock.Pages[d]       = 0;
         }

         qBlock.BlockTypeB   = 3;
         qBlock.SelectedIdB  = 0;
         qBlock.IndexB       = TRUE;
         qBlock.PageB        = 0;

         qBlock.DoubleBlock = (qBlock.BlockType!=5);

         qBlock.AnzPages = 3;
         if (Sim.Players.Players[(SLONG)PlayerNum].Planes[qBlock.SelectedId].TypeId!=-1) qBlock.AnzPages += PlaneTypes[Sim.Players.Players[(SLONG)PlayerNum].Planes[qBlock.SelectedId].TypeId].AnzPhotos;

         qBlock.RefreshData (PlayerNum);
         qBlock.Refresh (PlayerNum, TRUE);

         GlobeBm.Clear (0);
         PaintGlobe ();
         PaintGlobeRoutes();

         Limit ((SLONG)39, Sim.Players.Players[PlayerNum].Blocks[Id].ScreenPos.x, 600-Sim.Players.Players[PlayerNum].Blocks[Id].Bitmap.Size.x);
         Limit ((SLONG)29, Sim.Players.Players[PlayerNum].Blocks[Id].ScreenPos.y, 409-Sim.Players.Players[PlayerNum].Blocks[Id].Bitmap.Size.y);

         //Block ggf. nach vorne bringen:
         Id = Sim.Players.Players[PlayerNum].Blocks (Id);
         while (Id>1)
         {
            Sim.Players.Players[PlayerNum].Blocks.Swap (Id, Id-1);
            Id--;
         }
      }

   //Test, ob Klick auf Block
   if (CurrentBlock!=-1)
   {
      while (CurrentBlock > 1)
      {
          Sim.Players.Players[PlayerNum].Blocks.Swap(CurrentBlock, CurrentBlock - 1);
          CurrentBlock--;
          pBlock = &Sim.Players.Players[PlayerNum].Blocks[CurrentBlock];
      }
      BLOCK &qBlock = Sim.Players.Players[(SLONG)PlayerNum].Blocks[CurrentBlock];

      if (MouseClickArea==ROOM_LAPTOP && MouseClickId==100)
      {
         //Quick-Jump zum nächsten Element:
         if (MouseClickPar1>=170 && MouseClickPar1<=173)
         {
            SLONG c;

            if (MouseClickPar1==170 || MouseClickPar1==171) //Left Block
            {
               for (c=0; c<pBlock->Table.AnzRows; c++)
                  if (pBlock->SelectedId==pBlock->Table.LineIndex[c]) break;

               if (c<pBlock->Table.AnzRows)
               {
                  if (MouseClickPar1==171)
                  {
                     c=(c+1)%pBlock->Table.AnzRows;
                     pBlock->SelectedId=pBlock->Table.LineIndex[c];
                  }
                  else
                  {
                     c=(c+pBlock->Table.AnzRows-1)%pBlock->Table.AnzRows;
                     pBlock->SelectedId=pBlock->Table.LineIndex[c];
                  }
               }

               switch (pBlock->BlockType)
               {
                  //Städte-Index:
                  case 1:
                     pBlock->AnzPages = 1+Cities[pBlock->SelectedId].AnzTexts + Cities[pBlock->SelectedId].AnzPhotos;
                     EarthTargetAlpha    = UWORD((Cities[pBlock->SelectedId].GlobusPosition.x+170)*(3200/18)-16000+1300);
                     pBlock->LoadLib (Cities[pBlock->SelectedId].PhotoName);
                     break;

                  //Flugzeug-Details:
                  case 2:
                     if (pBlock->Index==0)
                     {
                        pBlock->AnzPages = 3;
                        if (Sim.Players.Players[(SLONG)PlayerNum].Planes[pBlock->SelectedId].TypeId!=-1) pBlock->AnzPages += PlaneTypes[Sim.Players.Players[(SLONG)PlayerNum].Planes[pBlock->SelectedId].TypeId].AnzPhotos;
                     }
                     if (pBlock->Index==2) pBlock->AnzPages = 7;
                     GlobeBm.Clear (0);
                     PaintGlobe ();
                     PaintGlobeRoutes();
                     break;
               }

               if (pBlock->Page>=pBlock->AnzPages) pBlock->Page=pBlock->AnzPages-1;

               pBlock->RefreshData (PlayerNum);
               pBlock->Refresh (PlayerNum, IsLaptop);
            }
            else if (MouseClickPar1==172 || MouseClickPar1==173) //Right Block
            {
               for (c=0; c<pBlock->TableB.AnzRows; c++)
                  if (pBlock->SelectedIdB==pBlock->TableB.LineIndex[c]) break;

               if (c<pBlock->TableB.AnzRows)
               {
                  if (MouseClickPar1==173)
                  {
                     c=(c+1)%pBlock->TableB.AnzRows;
                     pBlock->SelectedIdB=pBlock->TableB.LineIndex[c];
                  }
                  else
                  {
                     c=(c+pBlock->TableB.AnzRows-1)%pBlock->TableB.AnzRows;
                     pBlock->SelectedIdB=pBlock->TableB.LineIndex[c];
                  }
               }

               switch (pBlock->BlockTypeB)
               {
                  //Auftrags-Details:
                  case 3:
                     pBlock->AnzPagesB = 2;
                     break;

                  //Routen-Details:
                  case 4:
                     pBlock->AnzPagesB = 3;
                     break;

                  //Fracht-Details:
                  case 6:
                     pBlock->AnzPagesB = 2;
                     break;
               }

               if (pBlock->PageB>=pBlock->AnzPagesB) pBlock->PageB=pBlock->AnzPagesB-1;

               pBlock->RefreshData (PlayerNum);
               pBlock->Refresh (PlayerNum, IsLaptop);
            }
         }

         if (MouseClickPar1==31) ButtonNext ();           //Klick auf "Next"...
         else if (MouseClickPar1==30) ButtonPrev ();      //Klick auf "Prev"...
         else if (MouseClickPar1==35) ButtonIndex ();     //Klick auf "Index"...
         else if (MouseClickPar1==41) ButtonNextB ();     //Klick auf "NextB"...
         else if (MouseClickPar1==40) ButtonPrevB ();     //Klick auf "PrevB"...
         else if (MouseClickPar1==45) ButtonIndexB ();    //Klick auf "IndexB"...
         else if (MouseClickPar1==88)
         {
            ScrollOffset = gMousePosition-qPlayer.Blocks[CurrentBlock].ScreenPos-XY(5, 43+(138-43)*pBlock->Page/pBlock->PageSize/(pBlock->AnzPages-1))-XY(0,10);
            ScrollBlock  = CurrentBlock;
            ScrollSide   = 0;
         }
         else if (MouseClickPar1==89)
         {
            ScrollOffset = gMousePosition-qPlayer.Blocks[CurrentBlock].ScreenPos-XY(376, 43+(138-43)*pBlock->PageB/pBlock->PageSizeB/(pBlock->AnzPagesB-1))-XY(0,10);
            ScrollBlock  = CurrentBlock;
            ScrollSide   = 1;
         }
         else if (MouseClickPar1==50)
         {
            KommVarLampe=3;  //kurz blinken
            ButtonClose ();
         }
         else if (MouseClickPar1==60 || MouseClickPar1==61 || MouseClickPar1==62)
         {
            //Neue Parameter laden:
            if (MouseClickPar1==60) qBlock.BlockTypeB = 3; //Aufträge
            if (MouseClickPar1==61) qBlock.BlockTypeB = 6; //Fracht
            if (MouseClickPar1==62) qBlock.BlockTypeB = 4; //Routen

            qBlock.IndexB      = qBlock.Indexes[qBlock.BlockTypeB-1];
            qBlock.SelectedIdB = qBlock.SelectedIds[qBlock.BlockTypeB-1];
            qBlock.PageB       = qBlock.Pages[qBlock.BlockTypeB-1];
            qBlock.RefreshData (PlayerNum);
            qBlock.AnzPagesB  = max (0, (qBlock.TableB.AnzRows-1)/6)+1;

            if (qBlock.IndexB==1) qBlock.AnzPagesB = max (0, (qBlock.TableB.AnzRows-1)/6)+1;
            if (qBlock.IndexB==2) qBlock.AnzPagesB = 7;
            if (qBlock.IndexB==0) qBlock.AnzPagesB = 1;

            qBlock.Refresh (PlayerNum, TRUE);
         }
      }

      //Ggf. Window moving...
      if (CurrentBlockPos.IfIsWithin (34,2,165,19) || CurrentBlockPos.IfIsWithin (223,2,356,19))
      {
         if (CurrentDragId==-1) DragStart=gMousePosition;
         CurrentDragId = CurrentBlock;
         DragOffset    = gMousePosition - pBlock->ScreenPos;
      }

      //Block ggf. nach vorne bringen:
      //int b = CurrentBlock;
      //  while (b>1)
      //  {
      //      //pBlock = &Sim.Players.Players[PlayerNum].Blocks[b];
      //      //pBlock->Refresh(PlayerNum, TRUE);
      //      Sim.Players.Players[PlayerNum].Blocks.Swap (b , b- 1);
      //      b--;
      //      //pBlock = &Sim.Players.Players[PlayerNum].Blocks[b];
      //  }
        //pBlock = &Sim.Players.Players[PlayerNum].Blocks[b];
        //pBlock->Refresh(PlayerNum, TRUE);

   }//Ende: Test, ob Klick auf Block
   else if (MouseClickArea==ROOM_LAPTOP && MouseClickId==100)
   {
      if (Sim.Players.Players[(SLONG)PlayerNum].LaptopVirus==0)
      {
              if (MouseClickPar1==90) FensterVisible^=1;
         else if (MouseClickPar1==70) qPlayer.DisplayPlanes[0]^=1;
         else if (MouseClickPar1==71) qPlayer.DisplayPlanes[1]^=1;
         else if (MouseClickPar1==72) qPlayer.DisplayPlanes[2]^=1;
         else if (MouseClickPar1==73) qPlayer.DisplayPlanes[3]^=1;
         else if (MouseClickPar1==80) qPlayer.DisplayRoutes[0]=UBYTE((qPlayer.DisplayRoutes[0]+1)%3);
         else if (MouseClickPar1==81) qPlayer.DisplayRoutes[1]=UBYTE((qPlayer.DisplayRoutes[1]+1)%3);
         else if (MouseClickPar1==82) qPlayer.DisplayRoutes[2]=UBYTE((qPlayer.DisplayRoutes[2]+1)%3);
         else if (MouseClickPar1==83) qPlayer.DisplayRoutes[3]=UBYTE((qPlayer.DisplayRoutes[3]+1)%3);

         if (MouseClickPar1>=70 && MouseClickPar1<=73)
            if (Sim.Players.Players[PlayerNum].DisplayPlanes[MouseClickPar1-70])
               Sim.Players.Players[MouseClickPar1-70].Planes.UpdateGlobePos (EarthAlpha);

         if (MouseClickPar1>=80 && MouseClickPar1<=83)
         {
            GlobeBm.Clear (0);
            PaintGlobe();
            PaintGlobeRoutes();
         }
      }
      else
      {
         if (MouseClickPar1/10==7) KommVarButtons[3]=1;
         if (MouseClickPar1/10==8) KommVarButtons[2]=1;
      }
   }

   //Klick auf Ikone behandeln:
   if (CurrentIcon==0) Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
   else if (CurrentIcon>=1 && CurrentIcon<=5 && Sim.Players.Players[PlayerNum].Blocks.GetNumFree())
   {
      if (Sim.Players.Players[(SLONG)PlayerNum].LaptopVirus)
      {
         KommVarButtons[CurrentIcon-1]=1;
      }
      else
      {
         FensterVisible=TRUE;

         //Neuen Block oder Block nach vorne bringen?
         for (c=Sim.Players.Players[(SLONG)PlayerNum].Blocks.AnzEntries(); c>0; c--)
            if (Sim.Players.Players[(SLONG)PlayerNum].Blocks.IsInAlbum(c))
               if (Sim.Players.Players[(SLONG)PlayerNum].Blocks[c].BlockType==CurrentIcon && Sim.Players.Players[(SLONG)PlayerNum].Blocks[c].Index==1)
                  break;

         if (c>=1)
         {
            //Block ggf. nach vorne bringen:
            while (c>1)
            {
               Sim.Players.Players[PlayerNum].Blocks.Swap (c, c-1);
               c--;
            }
         }
         else //Nein, Block neu erzeugen:
         {
            ULONG Id;

            KommVarLampe=1;  //richtig flackern
            IconRotSpeed[CurrentIcon]+=1000;

            Id = Sim.Players.Players[PlayerNum].Blocks.GetUniqueId();

            Sim.Players.Players[PlayerNum].Blocks += Id;

            {
               BLOCK &qBlock=Sim.Players.Players[PlayerNum].Blocks[Id];

               qBlock.PlayerNum  = PlayerNum;
               //qBlock.ScreenPos  = XY (IconsPos[CurrentIcon*2+Background*24]+60, IconsPos[CurrentIcon*2+1+Background*24]-gNotepad2Bm.Size.y/6);
               qBlock.ScreenPos  = GlobeWindows [CurrentIcon-1];

               qBlock.Destructing    = 0;
               qBlock.AnimationStart = Sim.TimeSlice;

               qBlock.BlockType  = CurrentIcon;
               qBlock.DoubleBlock = (qBlock.BlockType!=5);

               qBlock.SelectedId = 0;
               qBlock.Index      = TRUE;
               qBlock.Page       = 0;
               qBlock.Base       = this;

               for (SLONG d=0; d<6; d++)
               {
                  qBlock.Indexes[d]     = 1;
                  qBlock.SelectedIds[d] = 0;
                  qBlock.Pages[d]       = 0;
               }

               qBlock.BlockTypeB   = 3;
               qBlock.SelectedIdB  = 0;
               qBlock.IndexB       = TRUE;
               qBlock.PageB        = 0;

               qBlock.UpdatePageSize ();
               qBlock.RefreshData (PlayerNum);

               if (qBlock.BlockType==2) //Flugzeuge in einer Doppellliste:
                  qBlock.AnzPages   = max (0, (Sim.Players.Players[PlayerNum].Blocks[Id].Table.AnzRows-1)/6)+1;
               else
                  qBlock.AnzPages   = max (0, (Sim.Players.Players[PlayerNum].Blocks[Id].Table.AnzRows-1)/13)+1;

               qBlock.Refresh (PlayerNum, TRUE);

               Limit ((SLONG)(49-qBlock.Bitmap.Size.x/2), qBlock.ScreenPos.x, (SLONG)(600-qBlock.Bitmap.Size.x/2));
               Limit ((SLONG)29, qBlock.ScreenPos.y, (SLONG)380);
            }

            //Block ggf. nach vorne bringen:
            Id = Sim.Players.Players[PlayerNum].Blocks (Id);
            while (Id>1)
            {
               Sim.Players.Players[PlayerNum].Blocks.Swap (Id, Id-1);
               Id--;
            }
         }
      }
   }

   CStdRaum::OnLButtonDown(nFlags, point);
   UpdateWarningLightModes ();
}

//--------------------------------------------------------------------------------------------
//void CLaptop::OnLButtonUp(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CLaptop::OnLButtonUp(UINT nFlags, CPoint point)
{
   DefaultOnLButtonUp();

   if (!KonstruktorFinished)
      return;

	CPlaner::HandleLButtonUp ();
   CStdRaum::OnLButtonUp(nFlags, point);
   UpdateWarningLightModes ();
}

//--------------------------------------------------------------------------------------------
//void CGlobe::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CLaptop::OnLButtonDblClk(UINT, CPoint)
{
   if (!KonstruktorFinished)
      return;

   CPlaner::HandleLButtonDouble ();
   UpdateWarningLightModes ();
}

//--------------------------------------------------------------------------------------------
//void CLaptop::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CLaptop::OnRButtonDown(UINT nFlags, CPoint point)
{
   if (CurrentBlock!=-1) KommVarLampe=3;  //kurz blinken
   CPlaner::HandleRButtonDown ();
   CStdRaum::OnLButtonUp(nFlags, point);
   UpdateWarningLightModes ();
}

//--------------------------------------------------------------------------------------------
//void CLaptop::OnRButtonUp(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CLaptop::OnRButtonUp(UINT nFlags, CPoint point)
{
   DefaultOnRButtonUp();
   CPlaner::HandleRButtonUp ();
   CStdRaum::OnLButtonUp(nFlags, point);
   UpdateWarningLightModes ();
}

//--------------------------------------------------------------------------------------------
//Nachrichten weiterreichen:
//--------------------------------------------------------------------------------------------
void CLaptop::OnChar(UINT nChar, UINT a, UINT b) 
{
	CStdRaum::OnChar(nChar, a, b);
}
void CLaptop::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CStdRaum::OnKeyDown(nChar, nRepCnt, nFlags);
}

//--------------------------------------------------------------------------------------------
//Updates the modes (off/on/blinking) of the warning lights:
//--------------------------------------------------------------------------------------------
void CLaptop::UpdateWarningLightModes (void)
{
   PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];

   if (qPlayer.Auftraege.GetNumOpen())
   {
      WarningLightModes[0] = 1;
      if (qPlayer.Auftraege.GetNumDueToday()) WarningLightModes[0] = 2;
   }
   else WarningLightModes[0] = 0;

   if (qPlayer.Frachten.GetNumOpen())
   {
      WarningLightModes[1] = 1;
      if (qPlayer.Frachten.GetNumDueToday()) WarningLightModes[1] = 2;
   }
   else WarningLightModes[1] = 0;

   if (qPlayer.RentRouten.GetNumUsed())
      WarningLightModes[2] = 1;
   else
      WarningLightModes[2] = 0;
}