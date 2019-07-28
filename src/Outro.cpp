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

void ConvertBitmapTo16Bit (UBYTE *SourcePic, SBBM *pBitmap, UWORD *pPaletteMapper, SLONG SmackWidth, SLONG SourceSizeY, XY TargetOffset);

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

   pSmack = SmackOpen (FullFilename (SmackName, IntroPath), SMACKTRACKS, SMACKAUTOEXTRA);
   SmackPic.ReSize (pSmack->Width*pSmack->Height);
   CalculatePalettemapper (pSmack->Palette, PaletteMapper+1);

   Bitmap.ReSize (pSmack->Width, pSmack->Height);
   SmackToBuffer (pSmack, 0, 0, Bitmap.Size.x, Bitmap.Size.y, (UBYTE*)SmackPic, FALSE);
   SmackDoFrame (pSmack);
   SmackNextFrame (pSmack);
   ConvertBitmapTo16Bit ((UBYTE*)SmackPic, &Bitmap, PaletteMapper+1, pSmack->Width, pSmack->Height, XY(0, 0));

   ShowWindow(SW_SHOW);
   UpdateWindow();
}

//--------------------------------------------------------------------------------------------
//COutro-Fenster zerstören:
//--------------------------------------------------------------------------------------------
COutro::~COutro()
{
   if (pSmack) SmackClose (pSmack);
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

   if (!SmackWait (pSmack) && pSmack->FrameNum<pSmack->Frames-1)
   {
      //Take the next frame:
      Bitmap.ReSize (pSmack->Width, pSmack->Height);
      SmackToBuffer (pSmack, 0, 0, Bitmap.Size.x, Bitmap.Size.y, (UBYTE*)SmackPic, FALSE);

      if (pSmack->NewPalette) CalculatePalettemapper (pSmack->Palette, PaletteMapper+1);

      SmackDoFrame (pSmack);
      SmackNextFrame (pSmack);

      ConvertBitmapTo16Bit ((UBYTE*)SmackPic, &Bitmap, PaletteMapper+1, pSmack->Width, pSmack->Height, XY(0, 0));
   }

   PrimaryBm.BlitFrom (Bitmap, 320-pSmack->Width/2, 240-pSmack->Height/2);

   if (pSmack->FrameNum >= pSmack->Frames-1)
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