// CCredits.cpp : implementation file
//
#include "stdafx.h"
#include "Credits.h"
#include "glcredit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const char TOKEN_NEWGAME[]  = "Cred";

//Anzahl aus dem Ressource-Dateien:
SLONG MaxCredits;

//////////////////////////////////////////////////////////////////////////////////////////////
// Credits
//////////////////////////////////////////////////////////////////////////////////////////////
CCredits::CCredits()
{
   SLONG c;
   CRect rect (0,0,640,480);
               
   gMouseStartup = TRUE;
   LastTime      = timeGetTime();

   pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("credits.gli", RoomPath), &pGLibCredits, L_LOCMEM);
   Background.ReSize (pGLibCredits, GFX_BACK);
   Left.ReSize (pGLibCredits, GFX_LEFT);
   Right.ReSize (pGLibCredits, GFX_RIGHT);

   for (c=0; c<25; c++)
   {
      TextLines[c].ReSize (640-80, 40);
      TextLines[c].Clear (0);
   }

   if (!Create(NULL, "CCredits", WS_VISIBLE|WS_CHILD, rect, theApp.m_pMainWnd, 42))
   {
      ::MessageBox (NULL, "Create failed", "ERROR", MB_OK );
      return;
   }
   if (bFullscreen) SetWindowPos (&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSIZE);

   ScrollPos=-2;

   for (MaxCredits=0; ;MaxCredits++)
      if (strcmp (StandardTexte.GetS (TOKEN_CREDITS, 1000+MaxCredits), "EOF")==0)
      {
         MaxCredits--;
         break;
      }

   ShowWindow(SW_SHOW);
   UpdateWindow();
}

//--------------------------------------------------------------------------------------------
//CCredits::~CCredits()
//--------------------------------------------------------------------------------------------
CCredits::~CCredits()
{
   gMouseStartup = FALSE;
   Background.Destroy();
   Left.Destroy();
   Right.Destroy();
   if (pGfxMain && pGLibCredits) pGfxMain->ReleaseLib (pGLibCredits);
   if (pCursor) pCursor->SetImage (gCursorBm.pBitmap);
}

//--------------------------------------------------------------------------------------------
//Lädt die Bitmap neu:
//--------------------------------------------------------------------------------------------
void CCredits::ReloadBitmap(void)
{
}

//--------------------------------------------------------------------------------------------
//MESSAGE_MAP(CCredits, CWnd)
//--------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CCredits, CWnd)
	//{{AFX_MSG_MAP(CCredits)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_RBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCredits message handlers

//--------------------------------------------------------------------------------------------
//CCredits::OnPaint
//--------------------------------------------------------------------------------------------
void CCredits::OnPaint() 
{
   SLONG c;
   SLONG Time = timeGetTime();

   while (Time-LastTime>0)
   {
      if (GetAsyncKeyState (VK_CONTROL)/256) LastTime+=10;
                                        else LastTime+=60;
      OnTimer(0);
   }

   { CPaintDC dc(this); }

   if (bActive)
   {
      PrimaryBm.BlitFrom (Background, 320-Background.Size.x/2, 0);
      
      for (c=0; c<25; c++)
      {
         SLONG y=480-((ScrollPos-c*40+25*40*100)%(25*40));

         PrimaryBm.BlitFromT (TextLines[c], 40, y);
      }
   }

   PrimaryBm.BlitFromT (Left,0,0);
   PrimaryBm.BlitFromT (Right,640-Right.Size.x,0);
}

//--------------------------------------------------------------------------------------------
//CCredits::OnLButtonDown
//--------------------------------------------------------------------------------------------
void CCredits::OnLButtonDown(UINT, CPoint) 
{
   //"Abbrechen"
   Sim.Gamestate = GAMESTATE_BOOT;
}

//--------------------------------------------------------------------------------------------
//CCredits::OnRButtonDown
//--------------------------------------------------------------------------------------------
void CCredits::OnRButtonDown(UINT, CPoint) 
{
   DefaultOnRButtonDown ();

   //"Abbrechen"
   Sim.Gamestate = GAMESTATE_BOOT;
}

//--------------------------------------------------------------------------------------------
//CCredits::OnTimer
//--------------------------------------------------------------------------------------------
void CCredits::OnTimer(UINT) 
{
   ScrollPos++;

   if ((ScrollPos%40)==0)
   {
      TextLines[(ScrollPos/40)%25].Clear (0);
      if ((ScrollPos/40)<=MaxCredits && strlen(StandardTexte.GetS (TOKEN_CREDITS, 1000+ScrollPos/40))>1)
      {
         TextLines[(ScrollPos/40)%25].PrintAt (StandardTexte.GetS (TOKEN_CREDITS, 1000+ScrollPos/40), FontBigGrey, TEC_FONT_CENTERED, XY(00,0), XY(640-80,40));
      }
   }
}

//--------------------------------------------------------------------------------------------
//CCredits::OnChar
//--------------------------------------------------------------------------------------------
void CCredits::OnChar(UINT key, UINT, UINT) 
{
   //"Abbrechen"
   if (key!=VK_CONTROL) Sim.Gamestate = GAMESTATE_BOOT;
}

//--------------------------------------------------------------------------------------------
//CCredits::OnKeyDown
//--------------------------------------------------------------------------------------------
void CCredits::OnKeyDown(UINT key, UINT, UINT) 
{
   //"Abbrechen"
   if (key!=VK_CONTROL) Sim.Gamestate = GAMESTATE_BOOT;
}

//--------------------------------------------------------------------------------------------
//BOOL CStdRaum::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) : AG:
//--------------------------------------------------------------------------------------------
BOOL CCredits::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return (FrameWnd->OnSetCursor(pWnd, nHitTest, message));
}

//--------------------------------------------------------------------------------------------
//void CStdRaum::OnMouseMove(UINT nFlags, CPoint point): AG:
//--------------------------------------------------------------------------------------------
void CCredits::OnMouseMove(UINT nFlags, CPoint point) 
{
	FrameWnd->OnMouseMove(nFlags, point);
}