//============================================================================================
// HallDiskMenu - Das Lade- und Speichermenü für die einzelnen Airport Halls
//============================================================================================
#include "stdafx.h"
#include "HallDiskMenu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Anmerkung:
// HallDiskMenu immer und nur dynamisch allokieren und nie de-allokieren. Das wird automatisch
// erledigt. Also immer nur:
// "new HallDiskMenu (...);"

static const char FileId[] = "Hall";

//--------------------------------------------------------------------------------------------
// HallDiskMenu
//--------------------------------------------------------------------------------------------
HallDiskMenu::HallDiskMenu(BOOL bHandy, SLONG PlayerNum) : CStdRaum(bHandy, PlayerNum, "", NULL)
{
   SLONG c, d;

   CRect rect (10,10,620,460);

   pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("HallDisk.gli", GliPath), &pMenuLib, L_LOCMEM);
   MenuBm.ReSize (pMenuLib, "HALLDISK");

   /*if (!Create(NULL, "HallDiskMenu", WS_VISIBLE|WS_CHILD, rect, ParentWnd, 42))
   {
      ::MessageBox (NULL, "Create failed", "ERROR", MB_OK );
      return;
   }
   if (bFullscreen) SetWindowPos (&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSIZE);*/

   memset (bFiles, 0, sizeof (bFiles));

   SLONG difflevel = Sim.Difficulty;
   if (difflevel==DIFF_FREEGAME) difflevel=DIFF_FREEGAMEMAP;

   for (c=0; c<10; c++)
      for (d=0; d<10; d++)
         if (DoesFileExist (FullFilename (HallFilenames [c+1], MiscPath, 100*difflevel+d)))
            bFiles[c+d*10]=1;

   SDL_ShowWindow(FrameWnd->m_hWnd);
   SDL_UpdateWindowSurface(FrameWnd->m_hWnd);
}

//--------------------------------------------------------------------------------------------
// ~HallDiskMenu
//--------------------------------------------------------------------------------------------
HallDiskMenu::~HallDiskMenu()
{
   MenuBm.Destroy();
   if (pMenuLib && pGfxMain) pGfxMain->ReleaseLib (pMenuLib);
}

/////////////////////////////////////////////////////////////////////////////
// HallDiskMenu message handlers

//--------------------------------------------------------------------------------------------
// HallDiskMenu::OnPaint() 
//--------------------------------------------------------------------------------------------
void HallDiskMenu::OnPaint() 
{
   SLONG xOffset []= {79, 104, 126, 155, 187, 225, 255, 301, 350, 386, 446 };
   SLONG c, d, e;

   if (bActive && MenuBm.Size.x>0)
   {
      PrimaryBm.BlitFrom (MenuBm, 10, 10);

      for (c=0; c<10; c++)
         for (d=1; d<10; d++)
            if (d==9 || !bFiles[c+d*10])
            {
               for (e=0; e<7; e++)
                  PrimaryBm.PrimaryBm.Line (xOffset[c]+10, d*9+53+1+e, xOffset[c+1]+10-1, d*9+1+53+e, (DWORD)0xd0d0d0);
            }

      SLONG difflevel = Sim.Difficulty;
      if (difflevel==DIFF_FREEGAME) difflevel=DIFF_FREEGAMEMAP;

      SLONG xOffset = (difflevel>10)*75;
      difflevel%=11;

      PrimaryBm.PrimaryBm.Line (10+459+xOffset, 10+162+difflevel*9, 10+459+xOffset, 10+166+difflevel*9, (DWORD)0);
      PrimaryBm.PrimaryBm.Line (10+491+xOffset, 10+162+difflevel*9, 10+491+xOffset, 10+166+difflevel*9, (DWORD)0);
      PrimaryBm.PrimaryBm.Line (10+459+xOffset, 10+162+difflevel*9, 10+491+xOffset, 10+162+difflevel*9, (DWORD)0);
      PrimaryBm.PrimaryBm.Line (10+459+xOffset, 10+166+difflevel*9, 10+491+xOffset, 10+166+difflevel*9, (DWORD)0);
   }
}

//--------------------------------------------------------------------------------------------
// HallDiskMenu::OnClose() 
//--------------------------------------------------------------------------------------------
void HallDiskMenu::OnClose() 
{
   TopWin = NULL;

   //Self-Destruct (zerstört Fenster und gibt anschließend Speicher frei):
   delete this;
}

//--------------------------------------------------------------------------------------------
// HallDiskMenu::OnLButtonDown() 
//--------------------------------------------------------------------------------------------
void HallDiskMenu::OnLButtonDown(UINT nFlags, CPoint point) 
{
   SLONG n;

   point.x-=9; point.y-=9;

   SLONG x=-1;

   if (point.x>=76 && point.x<=592)
   {
      if (point.x>=79  && point.x<=102) x=0;
      if (point.x>=104 && point.x<=126) x=1;
      if (point.x>=128 && point.x<=153) x=2;
      if (point.x>=155 && point.x<=185) x=3;
      if (point.x>=187 && point.x<=223) x=4;
      if (point.x>=225 && point.x<=253) x=5;
      if (point.x>=255 && point.x<=299) x=6;
      if (point.x>=301 && point.x<=348) x=7;
      if (point.x>=350 && point.x<=384) x=8;
      if (point.x>=386 && point.x<=415) x=9;
      if (point.x>=446 && point.x<=592) x=99;
   }

   //Im oberen Click-Bereich?
   if (x!=-1 && point.y>=53 && point.y<=131)
   {
      if (((point.y-53)%9) < 8)
      {
			//Sicherheitshalber alle Personen entfernen & Links freigeben:
			Sim.Persons.ClearAlbum();

         n = (point.y-53)/9+1;

         if (x>=0 && x<=9 && bFiles[x+n*10]) Airport.Load (1+x, n);
         if (x==99)
         {
            Editor=FALSE;
            Airport.LoadAirport (n, n, n, n, n, n, n, n, n, n);
         }
      }
   }
	//Im unteren Click-Bereich?
   else if (x!=-1 && x!=99 && point.x>=76 && point.y>=161)
   {
      if (((point.y-161)%9) < 8)
      {
         SLONG  localLevel[10];

			//Sicherheitshalber alle Personen entfernen & Links freigeben:
			Sim.Persons.ClearAlbum();

         n = (point.y-161)/9+1;

         memcpy (localLevel, Airport.HallLevel, sizeof (Airport.HallLevel));
			
         localLevel[x]=n;
         Airport.LoadAirport (localLevel[0], localLevel[1], localLevel[2], localLevel[3], localLevel[4],
                              localLevel[5], localLevel[6], localLevel[7], localLevel[8], localLevel[9]);
      }
   }

   if (point.x>451 && point.x<492 && ((point.y>161 && point.y<212) || (point.y>212-9+18 && point.y<212+18)))
   {
      Sim.Difficulty=UBYTE((point.y-161)/9);

      if (Sim.Difficulty==DIFF_FREEGAMEMAP) Sim.Difficulty=DIFF_FREEGAME;

      SLONG c, d;

      memset (bFiles, 0, sizeof (bFiles));

      SLONG difflevel = Sim.Difficulty;
      if (difflevel==DIFF_FREEGAME) difflevel=DIFF_FREEGAMEMAP;

      for (c=0; c<10; c++)
         for (d=0; d<10; d++)
            if (DoesFileExist (FullFilename (HallFilenames [c+1], MiscPath, 100*difflevel+d))) bFiles[c+d*10]=1;
   }
   else if (point.x>451+75 && point.x<492+75 && (point.y>161 && point.y<161+99-1))
   {
      Sim.Difficulty=UBYTE((point.y-161)/9)+11;

      SLONG c, d;

      memset (bFiles, 0, sizeof (bFiles));

      for (c=0; c<10; c++)
         for (d=0; d<10; d++)
            if (DoesFileExist (FullFilename (HallFilenames [c+1], MiscPath, 100*Sim.Difficulty+d))) bFiles[c+d*10]=1;
   }

   ReferTo (nFlags);
}

//--------------------------------------------------------------------------------------------
// HallDiskMenu::OnRButtonDown() 
//--------------------------------------------------------------------------------------------
void HallDiskMenu::OnRButtonDown(UINT nFlags, CPoint point) 
{
   ReferTo (nFlags);
   ReferTo (point);
}


//--------------------------------------------------------------------------------------------
//BOOL CStdRaum::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) : AG:
//--------------------------------------------------------------------------------------------
BOOL HallDiskMenu::OnSetCursor(void* pWnd, UINT nHitTest, UINT message) 
{
	return (FrameWnd->OnSetCursor(pWnd, nHitTest, message));
}

//--------------------------------------------------------------------------------------------
//void CStdRaum::OnMouseMove(UINT nFlags, CPoint point): AG:
//--------------------------------------------------------------------------------------------
void HallDiskMenu::OnMouseMove(UINT nFlags, CPoint point) 
{
	FrameWnd->OnMouseMove(nFlags, point);
}