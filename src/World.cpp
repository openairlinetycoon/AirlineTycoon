//============================================================================================
// World.cpp : Telefonat mit den Niederlassungen
//============================================================================================
#include "stdafx.h"
#include "World.h"

#define GFX_SAT1			 (0x0000000031544153)
#define GFX_WORLD			 (0x000000444c524f57)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Zum debuggen:
static const char FileId[] = "Wrld";

extern SB_CColorFX ColorFX;

//--------------------------------------------------------------------------------------------
//Telefonat mit der Niederlassung
//--------------------------------------------------------------------------------------------
CWorld::CWorld(BOOL bHandy, ULONG PlayerNum, SLONG CityId) : CStdRaum (bHandy, PlayerNum, "World.gli", GFX_WORLD)
{
   CWorld::CityId=CityId;

   SLONG EarthAlpha = UWORD((Cities[CityId].GlobusPosition.x+170)*(3200/18)-16000+1300);

   EarthBm.ReSize ((char*)(LPCTSTR)FullFilename ("EarthAll.lbm", GliPath), SYSRAMBM);

   Satellite.ReSize   (pRoomLib, GFX_SAT1);
   LightAnim.ReSize   (pRoomLib, "SATLICHT",  1, NULL, FALSE, ANIMATION_MODE_REPEAT, 100, 2, 100);
   SendingAnim.ReSize (pRoomLib, "SENDER1",   3, NULL, FALSE, ANIMATION_MODE_REPEAT,   0, 5);

   HandyOffset = 170;

   XY tmp = Cities[CityId].GlobusPosition;
   EarthProjectize (tmp, (UWORD)EarthAlpha, &BubblePos);

   BubblePos+=XY(100+HandyOffset,20);

   if (BubblePos.y>200)
   {
      BubbleStyle  = 2;
      BubblePos   += XY(20,-30);
   }
   else
   {
      BubbleStyle  = 1;
      BubblePos   += XY(35,0);
   }

   ::PaintGlobe (EarthBm, &PicBitmap, (UWORD)EarthAlpha, XY(100, 20));

   for (SLONG c=Sim.Players.Players[(SLONG)PlayerNum].Planes.AnzEntries()-1; c>=0; c--)
      if (Sim.Players.Players[(SLONG)PlayerNum].Planes.IsInAlbum(c))
         Sim.Players.Players[(SLONG)PlayerNum].Planes[c].UpdateGlobePos (0);

   ShowWindow(SW_SHOW);
   UpdateWindow();
}

//--------------------------------------------------------------------------------------------
//Sehr destruktiv!
//--------------------------------------------------------------------------------------------
CWorld::~CWorld()
{
   Sim.Players.Players[(SLONG)Sim.localPlayer].Messages.AddMessage (BERATERTYP_AUFTRAG, "", MESSAGE_COMMENT);
}

//--------------------------------------------------------------------------------------------
//Die Bank wird eröffnet:
//--------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CWorld, CStdRaum)
	//{{AFX_MSG_MAP(CWorld)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////////////////////
// CWorld message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//void CWorld::OnPaint()
//--------------------------------------------------------------------------------------------
void CWorld::OnPaint()
{
   { CPaintDC dc(this); }

   if (Sim.Gamestate==GAMESTATE_BOOT) return;

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   RoomBm.BlitFromT (Satellite, 30, 285);

   LightAnim.BlitAtT  (RoomBm, 30+153, 285+104);
   SendingAnim.BlitAtT (RoomBm, 30+173, 285+5-127-16);

   CStdRaum::PostPaint ();
}

//--------------------------------------------------------------------------------------------
//void CWorld::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CWorld::OnLButtonDown(UINT nFlags, CPoint point)
{
   XY RoomPos;

   if (Sim.Players.Players[Sim.localPlayer].ArabMode==6) return;

   DefaultOnLButtonDown ();

   if (!ConvertMousePosition (point, &RoomPos))
   {
	   CStdRaum::OnLButtonDown(nFlags, point);
      return;
   }

}

//--------------------------------------------------------------------------------------------
//void CWorld::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CWorld::OnRButtonDown(UINT nFlags, CPoint point)
{
   if (Sim.Players.Players[Sim.localPlayer].ArabMode==6) return;

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
         {
            Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
         }
         CStdRaum::OnRButtonDown(nFlags, point);
      }
   }
}
