//============================================================================================
// Outro.cpp : Der Render-Outro
//============================================================================================
#include "stdafx.h"
#include "Outro.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//--------------------------------------------------------------------------------------------
//ULONG PlayerNum
//--------------------------------------------------------------------------------------------
COutro::COutro (BOOL bHandy, SLONG PlayerNum, CString SmackName) : CStdRaum (bHandy, PlayerNum, "", NULL)
{
   RoomBm.ReSize (640, 480);
   RoomBm.FillWith (0);
   PrimaryBm.BlitFrom (RoomBm);

   FrameNum=0;
   FrameNext=0;

   StopMidi ();

   gMouseStartup = TRUE;

   pSmack = smk_open_file(FullFilename (SmackName, IntroPath), SMK_MODE_DISK);
   smk_enable_video(pSmack, true);
   smk_info_video(pSmack, &Width, &Height, NULL);
   Height *= 2;
   State = smk_first(pSmack);

   Bitmap.ReSize (Width, Height);
   SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormatFrom((void*)smk_get_video(pSmack), Width, Height / 2, 8, Width, SDL_PIXELFORMAT_INDEX8);
   SDL_Palette* pal = SDL_AllocPalette(256);
   CalculatePalettemapper(smk_get_palette(pSmack), pal);
   SDL_SetSurfacePalette(surf, pal);
   State = smk_next(pSmack);

   SDL_Surface* scaleSurf = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGB565, 0);
   SDL_FreeSurface(surf);

   SDL_BlitScaled(scaleSurf, NULL, Bitmap.pBitmap->GetSurface(), NULL);
   SDL_FreePalette(pal);
   SDL_FreeSurface(scaleSurf);

   ShowWindow(SW_SHOW);
   UpdateWindow();
}

//--------------------------------------------------------------------------------------------
//COutro-Fenster zerstören:
//--------------------------------------------------------------------------------------------
COutro::~COutro()
{
   if (pSmack) smk_close(pSmack);
   pSmack = NULL;

   gMouseStartup = FALSE;
   pCursor->SetImage (gCursorBm.pBitmap);

   if (Sim.Options.OptionEnableMidi) NextMidi ();
   SetMidiVolume(Sim.Options.OptionMusik);
}

//--------------------------------------------------------------------------------------------
//BEGIN_MESSAGE_MAP(COutro, CWnd)
//--------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(COutro, CWnd)
	//{{AFX_MSG_MAP(COutro)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COutro message handlers

//--------------------------------------------------------------------------------------------
// void COutro::OnPaint():
//--------------------------------------------------------------------------------------------
void COutro::OnPaint() 
{
   { CPaintDC dc(this); }

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();
   
   if (FrameNum++<2) PrimaryBm.BlitFrom (RoomBm);

   if (timeGetTime() >= FrameNext && State == SMK_MORE)
   {
      //Take the next frame:
      Bitmap.ReSize(Width, Height);
      SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormatFrom((void*)smk_get_video(pSmack), Width, Height / 2, 8, Width, SDL_PIXELFORMAT_INDEX8);
      SDL_Palette* pal = SDL_AllocPalette(256);
      CalculatePalettemapper(smk_get_palette(pSmack), pal);
      SDL_SetSurfacePalette(surf, pal);
      State = smk_next(pSmack);

      SDL_Surface* scaleSurf = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGB565, 0);
      SDL_FreeSurface(surf);

      SDL_BlitScaled(scaleSurf, NULL, Bitmap.pBitmap->GetSurface(), NULL);
      SDL_FreePalette(pal);
      SDL_FreeSurface(scaleSurf);

      double usf;
      smk_info_all(pSmack, NULL, NULL, &usf);
      FrameNext = timeGetTime() + (usf / 1000.0);
   }

   PrimaryBm.BlitFrom (Bitmap, 320-Width/2, 240-Height/2);

   if (State != SMK_MORE)
      Sim.Gamestate = GAMESTATE_BOOT;
}

//--------------------------------------------------------------------------------------------
//void COutro::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void COutro::OnLButtonDown(UINT, CPoint) 
{
   Sim.Gamestate = GAMESTATE_BOOT;
}

//--------------------------------------------------------------------------------------------
// void COutro::OnRButtonDown(UINT nFlags, CPoint point):
//--------------------------------------------------------------------------------------------
void COutro::OnRButtonDown(UINT, CPoint) 
{
   DefaultOnRButtonDown ();
   Sim.Gamestate = GAMESTATE_BOOT;
}

//--------------------------------------------------------------------------------------------
//BOOL CStdRaum::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) : AG:
//--------------------------------------------------------------------------------------------
BOOL COutro::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return (FrameWnd->OnSetCursor(pWnd, nHitTest, message));
}

//--------------------------------------------------------------------------------------------
//void CStdRaum::OnMouseMove(UINT nFlags, CPoint point): AG:
//--------------------------------------------------------------------------------------------
void COutro::OnMouseMove(UINT nFlags, CPoint point) 
{
	FrameWnd->OnMouseMove(nFlags, point);
}

//--------------------------------------------------------------------------------------------
//void COutro::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//--------------------------------------------------------------------------------------------
void COutro::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   if (nChar==VK_ESCAPE)
   {
      Sim.Gamestate = GAMESTATE_BOOT;
   }
}