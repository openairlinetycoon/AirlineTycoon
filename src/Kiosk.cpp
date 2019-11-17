//============================================================================================
// Kiosk.cpp : Das Zeitungskiosk
//============================================================================================
#include "stdafx.h"
#include "glkiosk.h"
#include "Kiosk.h"

#define GFX_FEUER			(0x0000005245554546)
#define GFX_REIFEN		(0x00004e4546494552)
#define GFX_SALZ			(0x000000005a4c4153)
#define GFX_SKELETT		(0x005454454c454b53)
#define GFX_SUPERMAN		(0x4e414d5245505553)
#define GFX_INTRVIEW	   (0x5745495652544e49)
#define GFX_1			   (0x0000000000000031)
#define GFX_2			   (0x0000000000000032)
#define GFX_3			   (0x0000000000000033)
#define GFX_4			   (0x0000000000000034)
#define GFX_LIEBSTE		(0x004554534245494c)
#define GFX_PLANES		(0x000053454e414c50)
#define GFX_GELD			(0x00000000444c4547)
#define GFX_ROUTEN		(0x00004e4554554f52)

#define GFX_PASSAG		(0x0000474153534150)
#define GFX_PILOT			(0x000000544f4c4950)
#define GFX_STEWARD		(0x0044524157455453)
#define GFX_TRASHP0		(0x0030504853415254)
#define GFX_TRASHP1		(0x0031504853415254)
#define GFX_TRASHP2		(0x0032504853415254)
#define GFX_TRASHP3		(0x0033504853415254)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char FileId[] = "Kios";

static XY HeadlineStart[3] = { XY(8, 55),    XY(3, 34),    XY(14, 68-24) };
static XY HeadlineMax[3]   = { XY(165, 242), XY(194, 290), XY(170, 240) };

/*SLONG SinTab[256]; */
/*SBBM  HausBm, ShadowBm;
TECBM HausZBm;
SLONG Alpha=60, Beta=1*256; /**/

//////////////////////////////////////////////////////////////////////////////////////////////
// Kiosk Konstruktion, Initialisation, Destruction
//////////////////////////////////////////////////////////////////////////////////////////////

SBBM WaterStaticBm;
SBBM WaterMovingBm;

//--------------------------------------------------------------------------------------------
//Konstruktor
//--------------------------------------------------------------------------------------------
CKiosk::CKiosk(BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "kiosk.gli", GFX_KIOSK)
{
   CurrentTip = -1;

   Sim.FocusPerson=-1;

   if (!bHandy) AmbientManager.SetGlobalVolume (60);

   if (Sim.Headlines.IsInteresting)
   {
      SP_Mann.ReSize (5);
      SP_Mann.Clips[0].ReSize (0, "K_LeseR.smk", "", XY (120, 205), SPM_IDLE,  CRepeat(1,1), CPostWait(40,80), SMACKER_CLIP_CANCANCEL,
                                  NULL, SMACKER_CLIP_SET, 0, NULL,
                                  "A1", 1);
      SP_Mann.Clips[1].ReSize (1, "K_Seite.smk", "", XY (120, 205), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                                  NULL, SMACKER_CLIP_SET, 0, NULL,
                                  "A1", 2);
      SP_Mann.Clips[2].ReSize (2, "K_LeseL.smk", "", XY (120, 205), SPM_IDLE,  CRepeat(1,1), CPostWait(40,80), SMACKER_CLIP_CANCANCEL,
                                  NULL, SMACKER_CLIP_SET, 0, NULL,
                                  "A1", 3);
      SP_Mann.Clips[3].ReSize (3, "K_Turn.smk", "K_Turn.raw", XY (120, 205), SPM_IDLE,   CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                                  NULL, SMACKER_CLIP_SET, 0, NULL,
                                  "A3A1", 0, 4);
      SP_Mann.Clips[4].ReSize (4, "K_Seite.smk", "", XY (120, 205), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                                  NULL, SMACKER_CLIP_SET, 0, NULL,
                                  "A1", 3);

      DefaultDialogPartner=TALKER_KIOSK;
   }
   else
   {
      SleeperBm.ReSize (pRoomLib, "SLEEPER");
      PicBitmap.BlitFrom(SleeperBm, 180, 170);
      SleeperBm.Destroy();

      SchnarchFx.ReInit ("Schnarch.raw");
      SchnarchFx.Play(DSBPLAY_NOSTOP|DSBPLAY_LOOPING, Sim.Options.OptionEffekte*100/7);
   }

   NewspaperZoom[0]=NewspaperZoom[1]=NewspaperZoom[2]=0;

   StinkBm.ReSize (pRoomLib, "STINK");

   NewspaperTemplates[0].ReSize (pRoomLib, "PAPER1 PAPER1A");
   NewspaperTemplates[1].ReSize (pRoomLib, "PAPER2");
   NewspaperTemplates[2].ReSize (pRoomLib, "PAPER3");

   NewspaperDefs[0].ReSize (pRoomLib, "PAPER1B PAPER1C PAPER1D PAPER1E");
   NewspaperDefs[1].ReSize (pRoomLib, "INTRVIEW STREIK PAPER2B PAPER2C PAPER2D PAPER2A PAPER2E WET01 WET02 WET03 WET04 WET05 WET06");
   //NewspaperDefs[2].Destroy();

   /*HausBm.ReSize (pRoomLib, "HAUS");
   ShadowBm.ReSize (pRoomLib, "SHADOW");
   HausZBm.ReSize ("Haus.lbm");/**/

   /*WaterStaticBm.ReSize (pRoomLib, "WATER");
   WaterMovingBm.ReSize (WaterStaticBm.Size); 

   for (SLONG c=0; c<256; c++)
      SinTab[c] = SLONG(sin (c*3.24159/128)*6); */

   RepaintTip ();

   SetRoomVisited (PlayerNum, ROOM_KIOSK);

   ShowWindow(SW_SHOW);
   UpdateWindow();
}

//--------------------------------------------------------------------------------------------
//Destruktor
//--------------------------------------------------------------------------------------------
CKiosk::~CKiosk()
{
   Newspapers[0].Destroy();
   Newspapers[1].Destroy();
   Newspapers[2].Destroy();

   NewspaperTemplates[0].Destroy();
   NewspaperTemplates[1].Destroy();
   NewspaperTemplates[2].Destroy();

   NewspaperDefs[0].Destroy();
   NewspaperDefs[1].Destroy();
   NewspaperDefs[2].Destroy();
}

//--------------------------------------------------------------------------------------------
//Malt einen Tip neu:
//--------------------------------------------------------------------------------------------
void CKiosk::RepaintTip (void)
{
   SLONG c, d, e;
   XY    p;
   BOOL  WasPicture;
   BOOL  HasPictures;
   SBBM  Picture;

   for (c=0; c<3; c++)
   {
      WasPicture=FALSE;
      HasPictures=FALSE;
      p=HeadlineStart[c];

      for (d=0; d<10; d++)
         HasPictures |= (Sim.Headlines.GetHeadline (c, d).PictureId!=NULL);

      if (c==0 && !HasPictures) p.y+=85;

      Newspapers[c].ReSize ((NewspaperTemplates[c])[0].Size);
      if (c==0)
         if (HasPictures!=0)
            Newspapers[c].BlitFrom ((NewspaperTemplates[c])[0]);
         else
            Newspapers[c].BlitFrom ((NewspaperTemplates[c])[1]);
      else
         Newspapers[c].BlitFrom ((NewspaperTemplates[c])[0]);

      //Background Pictures:
      for (d=0; d<10; d++)
      {
         CHeadline hl=Sim.Headlines.GetHeadline (c, d);

         if (!WasPicture && hl.PictureId)
         {
            if (hl.PictureId==GFX_PASSAG || hl.PictureId==GFX_PILOT || hl.PictureId==GFX_STEWARD || hl.PictureId==GFX_TRASHP0 || hl.PictureId==GFX_TRASHP1 || hl.PictureId==GFX_TRASHP2 || hl.PictureId==GFX_TRASHP3)
            {
               WasPicture=TRUE;

               GfxLib *pGLibLocal=NULL;

               pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("KioskLoc.gli", GliPath), &pGLibLocal, L_LOCMEM);

               Picture.ReSize (pRoomLib, GFX_PAPER3A);
               Newspapers[c].BlitFrom (Picture);

               Picture.ReSize (pGLibLocal, hl.PictureId);
               Newspapers[c].BlitFrom (Picture, Newspapers[c].Size.x-Picture.Size.x-3, Newspapers[c].Size.y-Picture.Size.y);

               Picture.Destroy ();
               pGfxMain->ReleaseLib (pGLibLocal);
            }
         }
      }

      for (d=0; d<10; d++)
      {
         CHeadline hl=Sim.Headlines.GetHeadline (c, d);

         if (hl.Headline.GetLength()>0)
         {
            Newspapers[c].PrintAt (hl.Headline, FontSmallBlack, TEC_FONT_LEFT, p.x, p.y, HeadlineMax[c].x, HeadlineMax[c].y);
            p.y+=Newspapers[c].TryPrintAt (hl.Headline, FontSmallBlack, TEC_FONT_LEFT, p.x, p.y, HeadlineMax[c].x, HeadlineMax[c].y)+5;
         }

         if (!WasPicture && hl.PictureId)
         {
            WasPicture=TRUE;

            if (hl.PictureId==GFX_FEUER || hl.PictureId==GFX_REIFEN || hl.PictureId==GFX_SALZ || hl.PictureId==GFX_SKELETT || hl.PictureId==GFX_SUPERMAN)
            {
               GfxLib *pGLibNews=NULL;

               pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("KioskS.gli", GliPath), &pGLibNews, L_LOCMEM);
               Picture.ReSize (pGLibNews, hl.PictureId);

               if (c==0)
               {
                  Newspapers[c].BlitFrom (Picture, p.x+37, p.y);
                  Newspapers[c].BlitFromT ((NewspaperDefs[0])[0], 0, p.y);
                  Newspapers[c].BlitFromT ((NewspaperDefs[0])[1], 0, p.y+(NewspaperDefs[0])[0].Size.y+3);
               }
               else Newspapers[c].BlitFrom (Picture, p.x+10, p.y);

               p.y+=Picture.Size.y+5;

               Picture.Destroy ();
               pGfxMain->ReleaseLib (pGLibNews);
            }
            else if (hl.PictureId==GFX_INTRVIEW || hl.PictureId==GFX_STREIK)
            {
               Picture.ReSize (pRoomLib, hl.PictureId);

               Newspapers[c].BlitFrom (Picture, 10+p.x, p.y);

               p.y+=Picture.Size.y+5;

               Picture.Destroy ();
            }
            else
            {
               for (e=0; e<4; e++)
                  if (hl.PictureId==GFX_1+e*100 || hl.PictureId==GFX_2+e*100 || hl.PictureId==GFX_3+e*100 || hl.PictureId==GFX_4+e*100 || hl.PictureId==GFX_LIEBSTE+e*100 || hl.PictureId==GFX_PLANES+e*100 || hl.PictureId==GFX_GELD+e*100 || hl.PictureId==GFX_ROUTEN+e*100)
                  {
                     GfxLib *pGLibNews=NULL;

                     pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename (CString(bprintf ("KioskP%li.gli", e+1)), GliPath), &pGLibNews, L_LOCMEM);
                     Picture.ReSize (pGLibNews, hl.PictureId-e*100);

                     Newspapers[c].BlitFrom ((NewspaperDefs[1])[5], p.x-3, p.y);
                     Newspapers[c].BlitFrom (Picture, p.x+50-3, p.y);

                     p.y+=Picture.Size.y+5;

                     Picture.Destroy ();
                     pGfxMain->ReleaseLib (pGLibNews);
                  }
            }
         }
      }

      //Verzierungen auf der Wild
      if (WasPicture && c==0)
      {
         if (p.y<Newspapers[c].Size.y-(NewspaperDefs[0])[2].Size.y-(NewspaperDefs[0])[3].Size.y-10)
         {
            Newspapers[c].BlitFromT ((NewspaperDefs[0])[2], 0, p.y);

            p.y+=(NewspaperDefs[0])[2].Size.y+5;
         }

         if (p.y-5<Newspapers[c].Size.y-(NewspaperDefs[0])[3].Size.y)
            Newspapers[c].BlitFromT ((NewspaperDefs[0])[3], 0, Newspapers[c].Size.y-(NewspaperDefs[0])[3].Size.y);
      }

      //Verzierungen auf der News:
      if (c==1)
      {
         for (SLONG d=5; d>=2; d--)
            if (p.y<Newspapers[c].Size.y-(NewspaperDefs[1])[d].Size.y-10)
            {
               Newspapers[c].BlitFrom ((NewspaperDefs[1])[d], 0, p.y);

               p.y+=(NewspaperDefs[1])[d].Size.y+5;
            }

         Newspapers[c].BlitFrom ((NewspaperDefs[1])[7+Sim.Date%6], 134, 4);
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Kiosk message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//BEGIN_MESSAGE_MAP(CKiosk, CStdRaum)
//--------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CKiosk, CStdRaum)
	//{{AFX_MSG_MAP(CKiosk)
	ON_WM_RBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//--------------------------------------------------------------------------------------------
//void CKiosk::OnPaint()
//--------------------------------------------------------------------------------------------
void CKiosk::OnPaint()
{
   SLONG NewTip=-1;
   SLONG c;

   { CPaintDC dc(this); }

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   if (Sim.Headlines.IsInteresting)
   {
      SP_Mann.Pump ();
      SP_Mann.BlitAtT (RoomBm);
   }

   if (!Sim.Players.Players[PlayerNum].HasItem(ITEM_STINKBOMBE))
      RoomBm.BlitFrom (StinkBm, 158, 397);

   if (bActive && (!IsDialogOpen()))
   {
      for (c=0; c<3; c++)
      {
         if (NewspaperZoom[c]>0)
         {
            SDL_Rect SrcRect = { 0,0,Newspapers[c].Size.x,Newspapers[c].Size.y };
            SDL_Rect DestRect;

            XY p;

            if (c==0) p=XY(183,236);
            else if (c==1) p=XY(349,246);
            else if (c==2) p=XY(140,185);

            DestRect.x = long(p.x-Newspapers[c].Size.x/2*NewspaperZoom[c]/100);
            DestRect.y = long(p.y-Newspapers[c].Size.x/2*NewspaperZoom[c]/100);
            DestRect.w = long(Newspapers[c].Size.x*NewspaperZoom[c]/100);
            DestRect.h = long(Newspapers[c].Size.y*NewspaperZoom[c]/100);

            SDL_BlitScaled(Newspapers[c].pBitmap->GetSurface(), &SrcRect, RoomBm.pBitmap->GetSurface(), &DestRect);
         }
      }
   }

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   if (!IsDialogOpen() && !MenuIsOpen())
   {
      if (gMousePosition.IfIsWithin (560, 0, 639, 439)) SetMouseLook (CURSOR_EXIT, 0, ROOM_KIOSK, 999);
      if (gMousePosition.IfIsWithin (158, 397, 209, 439) && !Sim.Players.Players[PlayerNum].HasItem(ITEM_STINKBOMBE)) SetMouseLook (CURSOR_HOT, 0, ROOM_KIOSK, 20);
      else if (gMousePosition.IfIsWithin (494,169,555,325)) NewspaperZoom[1] += 20;
      else if (gMousePosition.IfIsWithin (419,239,512,403)) NewspaperZoom[0] += 20;
      else if (gMousePosition.IfIsWithin (426,98,516,247))  NewspaperZoom[2] += 20;
      else if (gMousePosition.IfIsWithin (167,179,339,429) && Sim.Headlines.IsInteresting) SetMouseLook (CURSOR_HOT, 0, ROOM_KIOSK, 10);
   }

   for (c=0; c<3; c++)
   {
      NewspaperZoom[c]-=10;
      Limit (float(0), NewspaperZoom[c], float(100));
   }

   /*{
      SB_CBitmapKey SrcKey(*WaterStaticBm.pBitmap);
      SB_CBitmapKey TgtKey(*WaterMovingBm.pBitmap);

      SLONG x, y, t, xx, yy;

      t=timeGetTime()/10;

      for (x=0; x<256; x++)
         for (y=0; y<256; y++)
         {
            xx = (x + SinTab[(t+x+y)&255] + 256)&255;
            yy = (y + SinTab[(t+x+y+90)&255] + 256)&255;

            ((UWORD*)TgtKey.Bitmap) [x+y*(TgtKey.lPitch>>1)] = ((UWORD*)SrcKey.Bitmap) [xx+yy*(SrcKey.lPitch>>1)];
         }
   }

   RoomBm.BlitFrom (WaterMovingBm, 320-WaterMovingBm.Size.x, 220-WaterMovingBm.Size.y/2);
   RoomBm.BlitFrom (WaterMovingBm, 320, 220-WaterMovingBm.Size.y/2);*/

    /*if ((GetAsyncKeyState (VK_SHIFT)/256) && GetAsyncKeyState (VK_CONTROL)/256) Alpha++;
   if (!(GetAsyncKeyState (VK_SHIFT)/256) && GetAsyncKeyState (VK_CONTROL)/256) Alpha--;
    if ((GetAsyncKeyState (VK_SHIFT)/256) && GetAsyncKeyState (VK_MENU)/256) Beta++;
   if (!(GetAsyncKeyState (VK_SHIFT)/256) && GetAsyncKeyState (VK_MENU)/256) Beta--;

   RoomBm.BlitFrom (HausBm);
   XY Mouse=gMousePosition-XY(0,40);
   if (Mouse.x<320 && Mouse.y<200 && Mouse.y>=0)
   {
      SLONG x, y;

      SB_CBitmapKey RoomKey(*RoomBm.pBitmap);
      SB_CBitmapKey ShadowKey(*ShadowBm.pBitmap);
      TECBMKEYC     ZKey(HausZBm);

      SLONG mx = SLONG(sin (Alpha*3.14159/180)*256);
      SLONG my = SLONG(cos (Alpha*3.14159/180)*256);

      for (y=0; y<200; y++)
         for (x=0; x<320; x++)
         {
            SLONG shadowbm_x, shadowbm_y;
            XY    diff = XY(x,y+ZKey.Bitmap[x+y*ZKey.lPitch])-Mouse;

            shadowbm_x = (mx*diff.x+my*diff.y);

            if (shadowbm_x>=0)
            {
               shadowbm_x>>=8;
               if (shadowbm_x<ShadowBm.Size.x)
               {
                  XY h=Mouse+XY(shadowbm_x,shadowbm_x)-XY(x,y+ZKey.Bitmap[x+y*ZKey.lPitch]);

                  shadowbm_y=(h.y-h.x)*Beta+(ZKey.Bitmap[x+y*ZKey.lPitch]<<8);

                  if (shadowbm_y>=0)
                  {
                     shadowbm_y>>=8;
                     if (shadowbm_y<ShadowBm.Size.y)
                        if (((UWORD*)ShadowKey.Bitmap)[shadowbm_x+(ShadowBm.Size.y-1-shadowbm_y)*(ShadowKey.lPitch>>1)])
                           ((UWORD*)RoomKey.Bitmap)[x+y*(RoomKey.lPitch>>1)]=0;
                  }
               }
            }
         }
   }

   RoomBm.BlitFromT (ShadowBm, Mouse.x-6, Mouse.y-ShadowBm.Size.y+24);/**/

   CStdRaum::PostPaint ();
   CStdRaum::PumpToolTips ();
}

//--------------------------------------------------------------------------------------------
//void CKiosk::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CKiosk::OnLButtonDown(UINT nFlags, CPoint point)
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
      if (MouseClickArea==ROOM_KIOSK && MouseClickId==999) Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
      else if (MouseClickArea==ROOM_KIOSK && MouseClickId==10) { StartDialog (TALKER_KIOSK, MEDIUM_AIR); }
      else if (MouseClickArea==ROOM_KIOSK && MouseClickId==20 && !Sim.Players.Players[PlayerNum].HasItem(ITEM_STINKBOMBE)) { StartDialog (TALKER_KIOSK, MEDIUM_AIR, 1000); }
      else CStdRaum::OnLButtonDown(nFlags, point);
   }
}

//--------------------------------------------------------------------------------------------
//void CKiosk::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CKiosk::OnRButtonDown(UINT nFlags, CPoint point)
{
   DefaultOnRButtonDown ();

   //Auﬂerhalb geklickt? Dann Default-Handler!
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