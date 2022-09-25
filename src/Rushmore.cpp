//============================================================================================
// Rushmore.cpp : Der Berg mit den Tafeln
//============================================================================================
#include "stdafx.h"
#include "glrush.h"
#include "Rushmore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Zum debuggen:
static const char FileId[] = "Sabo";

static SLONG LogoBmsYPositions [] = { 261, 302, 346, 388 };

//--------------------------------------------------------------------------------------------
//Der Sabotageraum
//--------------------------------------------------------------------------------------------
CRushmore::CRushmore(BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "rushmore.gli", GFX_RUSHMORE)
{
   SetRoomVisited (PlayerNum, ROOM_RUSHMORE);
   HandyOffset = 320;

   Sim.FocusPerson=-1;

   LogoBms.ReSize (pRoomLib, "LOGO00", 4);

   Sim.Players.Players[(SLONG)PlayerNum].ChangeMoney (-1, 3300, "");

   if (!bHandy) AmbientManager.SetGlobalVolume (60);

   //LogoBms initialisieren

   SDL_ShowWindow(FrameWnd->m_hWnd);
   SDL_UpdateWindowSurface(FrameWnd->m_hWnd);
}

//--------------------------------------------------------------------------------------------
//Sehr destruktiv!
//--------------------------------------------------------------------------------------------
CRushmore::~CRushmore()
{
   LogoBms.Destroy();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CRushmore message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//void CRushmore::OnPaint()
//--------------------------------------------------------------------------------------------
void CRushmore::OnPaint()
{
   SLONG c;
   SLONG Remapper[4];
   SLONG Ratings[4];
   SLONG RatingsB[4];

   if (!bHandy) SetMouseLook (CURSOR_NORMAL, 0, ROOM_SABOTAGE, 0);

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   for (c=0; c<4; c++)
   {
      Remapper[c]=c;

      Ratings[c]=Sim.Players.Players[c].GetMissionRating ();
      RatingsB[c]=Sim.Players.Players[c].GetMissionRating (true);

      //Hack: Damit die Sortierung stimmt, wirde das Vorzeichen verdreht.
      if (Sim.Difficulty==DIFF_ADDON01) Ratings[c]=-Ratings[c];
   }

   for (c=0; c<3; c++)
      if (Ratings[Remapper[c]]<Ratings[Remapper[c+1]])
      {
         SLONG d;

         d=Remapper[c]; Remapper[c]=Remapper[c+1]; Remapper[c+1]=d;
         c-=2;
         if (c<-1) c=-1;
      }

   for (c=0; c<4; c++)
      if (Remapper[c]!=-1 && !Sim.Players.Players[Remapper[c]].IsOut)
      {
         RoomBm.BlitFrom (LogoBms[Remapper[c]], 181, LogoBmsYPositions[c]);

         SLONG diff=Sim.Difficulty;
         if (diff==DIFF_FREEGAME) diff=DIFF_FIRST;

         SLONG r =Ratings[Remapper[c]];
         SLONG r2=RatingsB[Remapper[c]];

         //Hack: Damit die Sortierung stimmt, wurde vorher das Vorzeichen verdreht. Jetzt machen wir es wieder richtig.
         if (Sim.Difficulty==DIFF_ADDON01) r=abs(r);

         RoomBm.PrintAt (bprintf (StandardTexte.GetS (TOKEN_MISC, 1200+diff), (LPCTSTR)Insert1000erDots(r), (LPCTSTR)Insert1000erDots(r2)), FontBigGrey, TEC_FONT_LEFT, XY(225,LogoBmsYPositions[c]-1), XY(640,440));
      }

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   if (!IsDialogOpen() && !MenuIsOpen())
   {
      if (gMousePosition.IfIsWithin (0,0,100,50) ||      gMousePosition.IfIsWithin (0,0,50,100) ||
          gMousePosition.IfIsWithin (0,390,100,440) ||   gMousePosition.IfIsWithin (0,340,50,440) ||
          gMousePosition.IfIsWithin (590,0,640,100) ||   gMousePosition.IfIsWithin (540,0,640,50) ||
          gMousePosition.IfIsWithin (590,340,640,440) || gMousePosition.IfIsWithin (540,390,640,440))
         SetMouseLook (CURSOR_EXIT, 0, ROOM_RUSHMORE, 999);
   }

   CStdRaum::PostPaint ();
   CStdRaum::PumpToolTips ();
}

//--------------------------------------------------------------------------------------------
//void CRushmore::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CRushmore::OnLButtonDown(UINT nFlags, CPoint point)
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
      if (MouseClickArea==ROOM_RUSHMORE && MouseClickId==999) Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
      else CStdRaum::OnLButtonDown(nFlags, point);
   }
}

//--------------------------------------------------------------------------------------------
//void CRushmore::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CRushmore::OnRButtonDown(UINT nFlags, CPoint point)
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
         {
            Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
         }

         CStdRaum::OnRButtonDown(nFlags, point);
      }
   }
}
