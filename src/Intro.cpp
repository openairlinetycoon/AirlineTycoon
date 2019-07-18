//============================================================================================
// Intro.cpp : Der Render-Intro
//============================================================================================
#include "stdafx.h"
#include "glTitel.h"
#include "Intro.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void ConvertBitmapTo16Bit (const UBYTE *SourcePic, SBBM *pBitmap, UWORD *pPaletteMapper, SLONG SmackWidth, SLONG SourceSizeY, XY TargetOffset);

extern SB_CColorFX ColorFX;

//--------------------------------------------------------------------------------------------
//ULONG PlayerNum
//--------------------------------------------------------------------------------------------
CIntro::CIntro (BOOL bHandy, SLONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "", NULL)
{
   RoomBm.ReSize (640, 480);
   RoomBm.FillWith (0);
   PrimaryBm.BlitFrom (RoomBm);

   FrameNum        = 0;
   bWasIntroPlayed = false;

   StopMidi ();

   gMouseStartup = TRUE;

   pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("titel.gli", RoomPath), &pRoomLib, L_LOCMEM);
   FadeFrom.ReSize (640,480);
   FadeFrom.FillWith (0);
   FadeTo.ReSize (pRoomLib, GFX_TITEL);

   if (IntroPath.GetLength()!=0)
   {
      pSmack = smk_open_file (FullFilename ("intro.smk", IntroPath), SMK_MODE_DISK);

      if (pSmack) bWasIntroPlayed=true;

      ULONG Width, Height;
      UBYTE Mask;
      smk_info_video(pSmack, &Width, &Height, NULL);
      smk_info_audio(pSmack, &Mask, NULL, NULL, NULL);
      smk_enable_all(pSmack, SMK_VIDEO_TRACK | Mask);
      //SmackPic.ReSize (Width*Height);
      CalculatePalettemapper (smk_get_palette(pSmack), PaletteMapper+1);

      Bitmap.ReSize (Width, Height);
      ConvertBitmapTo16Bit (smk_get_video(pSmack), &Bitmap, PaletteMapper+1, Width, Height, XY(0, 0));
   }
   else
   {
      pSmack        = NULL;
      Sim.Gamestate = GAMESTATE_BOOT;
   }

   ShowWindow(SW_SHOW);
   UpdateWindow();
}

//--------------------------------------------------------------------------------------------
//CIntro-Fenster zerstören:
//--------------------------------------------------------------------------------------------
CIntro::~CIntro()
{
   if (bWasIntroPlayed) Sim.Options.OptionViewedIntro = TRUE;

   FadeFrom.Destroy();
   FadeTo.Destroy();

   if (pRoomLib && pGfxMain)
   {
      pGfxMain->ReleaseLib (pRoomLib);
      pRoomLib=NULL;
   }

   if (pSmack) smk_close (pSmack);
   pSmack = NULL;

   gMouseStartup = FALSE;
   pCursor->SetImage (gCursorBm.pBitmap);

   if (Sim.Options.OptionEnableMidi) NextMidi ();
   SetMidiVolume(Sim.Options.OptionMusik);
}

//--------------------------------------------------------------------------------------------
//BEGIN_MESSAGE_MAP(CIntro, CWnd)
//--------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CIntro, CWnd)
	//{{AFX_MSG_MAP(CIntro)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CIntro message handlers

//--------------------------------------------------------------------------------------------
// void CIntro::OnPaint():
//--------------------------------------------------------------------------------------------
void CIntro::OnPaint() 
{
   { CPaintDC dc(this); }

   if (FrameNum++<2) PrimaryBm.BlitFrom (RoomBm);

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();
   
   if (pSmack)
   {
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
      {
         if (Sim.Options.OptionViewedIntro==0)
         {
            FadeFrom.BlitFrom (Bitmap, 320-Width/2, 240-Height/2);

            FadeCount=timeGetTime();

            smk_close (pSmack); pSmack=0;
         }
         else
            Sim.Gamestate = GAMESTATE_BOOT;
      }
   }
   else
   {
      if (timeGetTime()-FadeCount<1000)
      {
         SLONG Level=min(1000, timeGetTime()-FadeCount)*8/1000;
         ColorFX.ApplyOn2 (Level, FadeTo.pBitmap, 8-Level, FadeFrom.pBitmap, &PrimaryBm.PrimaryBm);
      }
      else
      {
         PrimaryBm.BlitFrom (FadeTo);

         if (timeGetTime()-FadeCount>3000)
            Sim.Gamestate = GAMESTATE_BOOT;
      }
   }
}

//--------------------------------------------------------------------------------------------
//void CIntro::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CIntro::OnLButtonDown(UINT, CPoint) 
{
   if (pSmack && Sim.Options.OptionViewedIntro==0)
   {
      ULONG Width, Height;
      smk_info_video(pSmack, &Width, &Height, NULL);
      FadeFrom.BlitFrom (Bitmap, 320-Width/2, 240-Height/2);

      FadeCount=timeGetTime();

      smk_close (pSmack); pSmack=0;
   }
   else
   {
      Sim.Gamestate = GAMESTATE_BOOT;
   }
}

//--------------------------------------------------------------------------------------------
// void CIntro::OnRButtonDown(UINT nFlags, CPoint point):
//--------------------------------------------------------------------------------------------
void CIntro::OnRButtonDown(UINT, CPoint) 
{
   DefaultOnRButtonDown ();

   if (pSmack && Sim.Options.OptionViewedIntro==0)
   {
      ULONG Width, Height;
      smk_info_video(pSmack, &Width, &Height, NULL);
      FadeFrom.BlitFrom (Bitmap, 320-Width/2, 240-Height/2);

      FadeCount=timeGetTime();

      smk_close (pSmack); pSmack=0;
   }
   else
   {
      Sim.Gamestate = GAMESTATE_BOOT;
   }
}

//--------------------------------------------------------------------------------------------
//BOOL CStdRaum::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) : AG:
//--------------------------------------------------------------------------------------------
BOOL CIntro::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return (FrameWnd->OnSetCursor(pWnd, nHitTest, message));
}

//--------------------------------------------------------------------------------------------
//void CStdRaum::OnMouseMove(UINT nFlags, CPoint point): AG:
//--------------------------------------------------------------------------------------------
void CIntro::OnMouseMove(UINT nFlags, CPoint point) 
{
	FrameWnd->OnMouseMove(nFlags, point);
}

//--------------------------------------------------------------------------------------------
//void CIntro::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//--------------------------------------------------------------------------------------------
void CIntro::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   if (nChar==VK_ESCAPE)
   {
      if (pSmack && Sim.Options.OptionViewedIntro==0)
      {
         ULONG Width, Height;
         smk_info_video(pSmack, &Width, &Height, NULL);
         FadeFrom.BlitFrom (Bitmap, 320-Width/2, 240-Height/2);

         FadeCount=timeGetTime();

         smk_close (pSmack); pSmack=0;
      }
      else
      {
         Sim.Gamestate = GAMESTATE_BOOT;
      }
   }
}
