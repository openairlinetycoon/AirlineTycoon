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

void ConvertBitmapTo16Bit (const UBYTE *SourcePic, SBBM *pBitmap, UWORD *pPaletteMapper, SLONG SmackWidth, SLONG SourceSizeY, XY TargetOffset);

//--------------------------------------------------------------------------------------------
//ULONG PlayerNum
//--------------------------------------------------------------------------------------------
COutro::COutro (BOOL bHandy, SLONG PlayerNum, CString SmackName) : CStdRaum (bHandy, PlayerNum, "", NULL)
{
   RoomBm.ReSize (640, 480);
   RoomBm.FillWith (0);
   PrimaryBm.BlitFrom (RoomBm);

   FrameNum=0;

   StopMidi ();

   gMouseStartup = TRUE;

   pSmack = smk_open_file (FullFilename (SmackName, IntroPath), SMK_MODE_DISK);
   //SmackPic.ReSize (pSmack->Width*pSmack->Height);
   CalculatePalettemapper (smk_get_palette(pSmack), PaletteMapper+1);

   ULONG Width, Height;
   UBYTE Mask;
   smk_info_video(pSmack, &Width, &Height, NULL);
   smk_info_audio(pSmack, &Mask, NULL, NULL, NULL);
   smk_enable_all(pSmack, SMK_VIDEO_TRACK | Mask);
   Bitmap.ReSize (Width, Height);
   ConvertBitmapTo16Bit (smk_get_video(pSmack), &Bitmap, PaletteMapper+1, Width, Height, XY(0, 0));

   ShowWindow(SW_SHOW);
   UpdateWindow();
}

//--------------------------------------------------------------------------------------------
//COutro-Fenster zerstören:
//--------------------------------------------------------------------------------------------
COutro::~COutro()
{
   if (pSmack) smk_close (pSmack);
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

   ULONG Width, Height, CurFrame, Frames;
   smk_info_video(pSmack, &Width, &Height, NULL);
   smk_info_all(pSmack, &CurFrame, &Frames, NULL);
   if (smk_next(pSmack) != SMK_DONE && CurFrame < Frames-1)
   {
      //Take the next frame:
      Bitmap.ReSize (Width, Height);

      CalculatePalettemapper (smk_get_palette(pSmack), PaletteMapper+1);

      ConvertBitmapTo16Bit (smk_get_video(pSmack), &Bitmap, PaletteMapper+1, Width, Height, XY(0, 0));
   }

   PrimaryBm.BlitFrom (Bitmap, 320-Width/2, 240-Height/2);

   if (CurFrame >= Frames-1)
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
