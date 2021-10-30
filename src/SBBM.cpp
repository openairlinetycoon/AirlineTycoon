//============================================================================================
// SBBM.cpp : Spezielle Bitmap-Routinen
//============================================================================================
// Link: "SBBM.h"
//============================================================================================
#include "stdafx.h"

extern SB_CColorFX ColorFX;
extern XY MouseCursorOffset;
extern CString MakeVideoPath;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL    deltaCompressFrame (FILE *TargetFile, SB_CBitmapCore &OldFrame, SB_CBitmapCore &NewFrame, XY OffsetA, XY OffsetB);
extern CString MakeVideoPath;
XY      gScrollOffsetA, gScrollOffsetB;
FILE   *pSoundLogFile=NULL;
SLONG   SoundLogFileStartTime=0;

__int64 GetIdFromString (CString Text)
{
   __int64 id;

   id=0;
   for (SLONG d=0; d<SLONG(Text.GetLength()); d++)
      id+=__int64(Text[(int)d])<<(8*d);

   return (id);
}

BOOL SBBM::ShiftUp (SLONG y)
{
   //the previous method was to draw onto yourself with blit
   //tha (maybe) caused some crashes, so now we copy and then redraw
   SDL_Surface *orig = this->pBitmap->GetSurface();
   SDL_Surface *buffer = SDL_CreateRGBSurfaceWithFormat(0, Size.x, Size.y, orig->format->BitsPerPixel, orig->format->format);
   
   SDL_BlitSurface(orig, NULL, buffer, NULL); //copy content from original to buffer

   SDL_Rect src = { 0, 0, Size.x,Size.y };
   SDL_Rect dst = { 0, -y, Size.x, Size.y };
   SDL_BlitSurface(buffer, &src, orig, &dst); //redraw content to original

   SDL_FreeSurface(buffer); //just a buffer, throw out

   for (SLONG cy=0; cy<y+4; cy++)
      Line (XY(0,Size.y-cy-1), XY(Size.x-1,Size.y-cy-1), dword(0));

   return (true);
}

BOOL SBBM::BlitFrom (SBBM &TecBitmap, XY Target)
{
   Bench.BlitTime.Start();

   if (TecBitmap.pBitmap)
      TecBitmap.pBitmap->BlitFast (pBitmap, Target.x, Target.y);
   if (TecBitmap.pHLObj)
      TecBitmap.pHLObj->BlitAt (pBitmap, Target.x, Target.y);

   Bench.BlitTime.Stop();
   return (TRUE);
}

BOOL SBBM::BlitFromT (SBBM &TecBitmap, XY Target)
{
   Bench.BlitTime.Start();

   if (TecBitmap.pBitmap)
      TecBitmap.pBitmap->BlitT (pBitmap, Target.x, Target.y);
   if (TecBitmap.pHLObj)
      TecBitmap.pHLObj->BlitAt (pBitmap, Target.x, Target.y);

   Bench.BlitTime.Stop();
   return (TRUE);
}

BOOL SBBM::BlitPartFrom (SBBM &TecBitmap, XY Target, const XY &p1, const XY &p2)
{
   if (TecBitmap.pBitmap)
      TecBitmap.pBitmap->Blit (pBitmap, Target.x, Target.y, &CRect (p1.x, p1.y, p2.x, p2.y));

   return (TRUE);
}

BOOL SBBM::BlitPartFromT (SBBM &TecBitmap, XY Target, const XY &p1, const XY &p2)
{
   if (TecBitmap.pBitmap)
      TecBitmap.pBitmap->BlitT (pBitmap, Target.x, Target.y, &CRect (p1.x, p1.y, p2.x, p2.y));

   return (TRUE);
}

BOOL SBBM::BlitFrom (SBBM &TecBitmap, RECT *r1, const XY &dest)
{
   Bench.BlitTime.Start(); 

   if (TecBitmap.pBitmap)
      TecBitmap.pBitmap->Blit (pBitmap, dest.x, dest.y, r1);
   if (TecBitmap.pHLObj)
      TecBitmap.pHLObj->BlitAt (pBitmap, dest.x, dest.y);

   Bench.BlitTime.Stop(); 
   return (TRUE); 
}

// Eine einfarbige Linie ziehen (Farb¸bergabe in HardwareColor (Hardwareabh‰ngig))
void SBBM::Line (XY p1, XY p2, BOOL Fat, SB_Hardwarecolor *pColor, SLONG NumColors)
{
   RECT            DefClip;
   SLONG           cc=0;         //Count Colors
	
   if (Fat)
   {
      Line (p1, p2, FALSE, pColor, NumColors);
      Line (p1+XY(1,0), p2+XY(1,0), FALSE, pColor, NumColors);
      Line (p1+XY(0,1), p2+XY(0,1), FALSE, pColor, NumColors);
      Line (p1-XY(1,0), p2-XY(1,0), FALSE, pColor, NumColors);
      Line (p1-XY(0,1), p2-XY(0,1), FALSE, pColor, NumColors);
      return;
   }

   SB_CBitmapKey Key(*pBitmap);

   DefClip.left   = 0;
   DefClip.top    = 0;
   DefClip.right  = Size.x;
   DefClip.bottom = Size.y;

	// ---------------------
	// VLine
	// ---------------------
	if (p1.x == p2.x)
	{
		if (p1.x >= DefClip.left && p1.x < DefClip.right)
		{
			// Von oben nach unten
			if (p1.y > p2.y)
				Swap (p1.y, p2.y);

			if (p1.y < DefClip.top)
				p1.y = DefClip.top;

			if (p2.y >= DefClip.bottom)
				p2.y = DefClip.bottom - 1;

			char * pixelPtr=(char *)Key.Bitmap+(Key.lPitch*p1.y)+p1.x*2;
			long points = p2.y - p1.y;
			for (long i = 0 ; i < points ; i++, pixelPtr += Key.lPitch, cc++)
				*((word *)pixelPtr)=(word)pColor[(cc>>2)%NumColors];
				
		}
	}
	// ---------------------
	// HLine
	// ---------------------
	else if (p1.y == p2.y)
	{
		if (p1.y >= DefClip.top && p1.y < DefClip.bottom)
		{
			// Von links nach rechts
			if (p1.x > p2.x)
				Swap (p1.x, p2.x);

			if (p1.x < DefClip.left)
				p1.x = DefClip.left;

			if (p2.x >= DefClip.right)
				p2.x = DefClip.right - 1;

			char * pixelPtr=(char *)Key.Bitmap+(Key.lPitch*p1.y)+p1.x*2;
			long points = p2.x - p1.x;
			for (long i = 0 ; i < points ; i++, pixelPtr += 2, cc++)
				*((word *)pixelPtr)=(word)pColor[(cc>>2)%NumColors];
		}
	}
	// ---------------------
	// Complex line.
	// ---------------------
	else
	{
		long   error, x, y, dx, dy, dxa, dya, incr;

		dxa = abs(dx = p2.x - p1.x);
		dya = abs(dy = p2.y - p1.y);

		if (dxa > dya)
		{
			if (dx < 0)
			{
				Swap (p1.y, p2.y);
				Swap (p1.x, p2.x);
			}

			if (p2.y > p1.y)
				incr = 1;
			else
				incr = -1;

			error = -(dxa >> 1);

			for (x = p1.x, y = p1.y; x < p2.x; x++, cc++)
			{
				if ((x >= DefClip.left) && (x < DefClip.right) && (y >= DefClip.top) && (y < DefClip.bottom))
				{
					char * pixelPtr= (char *)Key.Bitmap+(Key.lPitch*y)+x*2;
					*((word *)pixelPtr)=(word)pColor[(cc>>2)%NumColors];
				};
         
				error += dya;
				if (error >= 0)
				{
					y += incr;
					error -= dxa;
				}
			}
		}
		else
		{
			if (dy < 0)
			{
				Swap (p1.y, p2.y);
				Swap (p1.x, p2.x);
			}

			if (p2.x > p1.x)
				incr = 1;
			else
				incr = -1;

			error = -(dya >> 1);

			for (y = p1.y, x = p1.x; y < p2.y; y++, cc++)
			{
				if ((x >= DefClip.left) && (x < DefClip.right) && (y >= DefClip.top) && (y < DefClip.bottom))
				{
					char * pixelPtr= (char *)Key.Bitmap+(Key.lPitch*y)+x*2;
					*((word *)pixelPtr)=(word)pColor[(cc>>2)%NumColors];
				};


				error += dxa;
				if (error >=0)
				{
					x += incr;
					error -= dya;
				}
			}
		}
	}
}

BOOL SBPRIMARYBM::BlitFrom (SBBM &TecBitmap, RECT *r1, const XY &dest)
{
   Bench.BlitTime.Start(); 

   if (TecBitmap.pBitmap)
      TecBitmap.pBitmap->Blit (&PrimaryBm, dest.x, dest.y, r1);
   if (TecBitmap.pHLObj)
      TecBitmap.pHLObj->BlitAt (&PrimaryBm, dest.x, dest.y);

   Bench.BlitTime.Stop(); 
   return (TRUE); 
}
BOOL  SBPRIMARYBM::BlitFrom (SBBM &TecBitmap, XY Target)
{
   Bench.BlitTime.Start(); 

   if (TecBitmap.pBitmap)
      TecBitmap.pBitmap->BlitFast (&PrimaryBm, Target.x, Target.y);
   if (TecBitmap.pHLObj)
      TecBitmap.pHLObj->BlitAt (&PrimaryBm, Target.x, Target.y);

   Bench.BlitTime.Stop(); 
   return (TRUE);
}

BOOL  SBPRIMARYBM::BlitFromT (SBBM &TecBitmap, XY Target)
{
   Bench.BlitTime.Start(); 

   if(&TecBitmap == nullptr)
    return false;

   if (TecBitmap.pBitmap)
      TecBitmap.pBitmap->BlitT (&PrimaryBm, Target.x, Target.y);
   if (TecBitmap.pHLObj)
      TecBitmap.pHLObj->BlitAt (&PrimaryBm, Target.x, Target.y);

   Bench.BlitTime.Stop();
   return (TRUE);
}
BOOL  SBPRIMARYBM::BlitFromT (SBBM &TecBitmap, RECT *r1, const XY &dest)
{
   Bench.BlitTime.Start(); 

   if (TecBitmap.pBitmap)
      TecBitmap.pBitmap->BlitT (&PrimaryBm, dest.x, dest.y, r1);
   if (TecBitmap.pHLObj)
      TecBitmap.pHLObj->BlitAt (&PrimaryBm, dest.x, dest.y);

   Bench.BlitTime.Stop(); 
   return (TRUE); 
}

//============================================================================================
//Klasse f¸r meine Bitmaps:
//============================================================================================
BOOL  SBBM::BlitFrom (SBBM &TecBitmap, XY p1, XY p2)
{
   /*Bench.BlitTime.Start();
   
   TecBitmap.pBitmap->BlitFast (pBitmap, Target.x, Target.y);
   
   Bench.BlitTime.Stop();
   return (TRUE);   */
   return 0;
}
BOOL  SBBM::BlitFrom (SBBM &TecBitmap, SLONG tx, SLONG ty, SLONG tx2, SLONG ty2)
{
   return 0;
}
BOOL  SBBM::BlitFromT (SBBM &TecBitmap, XY p1, XY p2)
{
   return 0;
}
BOOL  SBBM::BlitFromT (SBBM &TecBitmap, SLONG tx, SLONG ty, SLONG tx2, SLONG ty2)
{
   return 0; 
}

BOOL SBPRIMARYBM::FlipBlitFromT (SBBM &TecBitmap, XY Target)
{
   Bench.BlitTime.Start(); 

	// Validate
	if ((!PrimaryBm.GetSurface()) || (!(TecBitmap.pBitmap->GetSurface())) )
		return FALSE;

	// Source-Rechteck f¸llen
	SDL_Rect srcRect;

	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = TecBitmap.pBitmap->GetXSize();
	srcRect.h = TecBitmap.pBitmap->GetYSize();

	// Zielpunkt f¸llen
	POINT pt = {Target.x, Target.y};

   /*DDBLTFX DDBltFx;
   memset (&DDBltFx, 0, sizeof(DDBltFx));
   DDBltFx.dwSize = sizeof (DDBltFx);
   DDBltFx.dwDDFX = DDBLTFX_MIRRORLEFTRIGHT;*/

   SDL_Rect destRect = { pt.x, pt.y, srcRect.w, srcRect.h };

   // TODO: Mirror the blit
   SDL_BlitSurface(TecBitmap.pBitmap->GetFlippedSurface(), &srcRect, PrimaryBm.GetSurface(), &destRect);
	// Clippen
	/*if (PrimaryBm.FastClip(PrimaryBm.GetClipRect(), &pt, &srcRect))
		DD_ERROR (PrimaryBm.GetSurface()->Blt (&destRect, TecBitmap.pBitmap->GetSurface(), &srcRect, DDBLT_DDFX | DDBLT_KEYSRC | DDBLT_WAIT, &DDBltFx));

	return (DD_OK);*/

   Bench.BlitTime.Stop();
   return (TRUE);
}

SLONG SBBM::PrintAt (const char *Str, SB_CFont &Font, SLONG Flags, const XY &p1, const XY &p2)
{
   Bench.TextTime.Start();
   if ((Flags&3)==TEC_FONT_RIGHT)
   {
      TABS t1[] = { TAB_STYLE_RIGHT, p2.x-2 }; 
      Font.SetTabulator (t1, sizeof(t1));
      Font.DrawTextWithTabs(pBitmap, p1.x, p1.y, (char*)(LPCTSTR)(CString("\t")+Str));
      Bench.TextTime.Stop();
      return (0);
   }
   else if ((Flags&3)==TEC_FONT_CENTERED)
   {
      TABS t1[] = { TAB_STYLE_CENTER, (p2.x+p1.x)/2 }; 
      Font.SetTabulator (t1, sizeof(t1));
      Font.DrawTextWithTabs(pBitmap, p1.x, p1.y, (char*)(LPCTSTR)(CString("\t")+Str));
      Bench.TextTime.Stop();
      return (0);
   }
   else
   {
      SLONG rc=Font.DrawTextBlock(pBitmap, p1.x, p1.y, p2.x, p2.y, (char*)Str);
      Bench.TextTime.Stop();
      return (rc);
   }
}

SLONG SBBM::TryPrintAt (const char *Str, SB_CFont &Font, SLONG Flags, const XY &p1, const XY &p2)
{
   Bench.TextTime.Start();
   SLONG rc=Font.PreviewTextBlock(pBitmap, p1.x, p1.y, p2.x, p2.y, (char*)Str);
   Bench.TextTime.Stop();
   return (rc);
}

BOOL  SBBM::TextOut (SLONG x, SLONG y, COLORREF Back, COLORREF Front, const CString &String)
{
   SDL_Surface* Surf;

   /*if (bFullscreen)*/ Surf=PrimaryBm.PrimaryBm.GetSurface();
               /*else Surf=PrimaryBm.Offscreen.pBitmap->GetSurface();*/

   TTF_Font* Font = TTF_OpenFont("arial.ttf", 9);
   if (Font)
   {
      SDL_Color bg = { GetRValue(Back), GetGValue(Back), GetBValue(Back) };
      SDL_Color fg = { GetRValue(Front), GetGValue(Front), GetBValue(Front) };
      SDL_Surface* Text = TTF_RenderText_Shaded(Font, String, fg, bg);
      SDL_Rect Dst = { x, y, Text->w, Text->h };
      SDL_BlitSurface(Text, NULL, Surf, &Dst);
      SDL_FreeSurface(Text);
      TTF_CloseFont(Font);
   }

   return 0; 
}

void SBBM::ReSize (CHLPool *pHLPool, CString graphicID)
{
   Destroy();

   pHLObj = pHLPool->GetHLObj (graphicID);

   Size=pHLObj->GetSize();
}
void SBBM::ReSize (CHLPool *pHLPool, __int64 graphicID)
{
   Destroy();

   pHLObj = pHLPool->GetHLObj (graphicID);

   if (pHLObj) Size=pHLObj->GetSize();
          else Size=XY(0,0);
}

void SBBM::ReSize (GfxLib* gfxLibrary, CString graphicStr)
{
   __int64         graphicId=0;
   BUFFER<char>    Str (graphicStr.GetLength()+1);

   for (int d=0; d<graphicStr.GetLength(); d++)
      graphicId+=__int64(graphicStr[d])<<(8*d);

   Destroy();
   bitmapMain->CreateBitmap(&pBitmap, gfxLibrary, graphicId, CREATE_USECOLORKEY|CREATE_SYSMEM);
   SBBM::Size.x = pBitmap->GetXSize(); SBBM::Size.y = pBitmap->GetYSize();
}


BOOL  SBPRIMARYBM::TextOut (SLONG x, SLONG y, COLORREF Back, COLORREF Front, const CString &String)
{
   SDL_Surface* Surf;

   /*if (bFullscreen)*/ Surf=PrimaryBm.GetSurface();
               /*else Surf=Offscreen.pBitmap->GetSurface();*/

   Bench.TextTime.Start();
   
   TTF_Font* Font = TTF_OpenFont("arial.ttf", 9);
   if (Font)
   {
      SDL_Color bg = { GetRValue(Back), GetGValue(Back), GetBValue(Back) };
      SDL_Color fg = { GetRValue(Front), GetGValue(Front), GetBValue(Front) };
      SDL_Surface* Text = TTF_RenderText_Shaded(Font, String, fg, bg);
      SDL_Rect Dst = { x, y, Text->w, Text->h };
      SDL_BlitSurface(Text, NULL, Surf, &Dst);
      SDL_FreeSurface(Text);
      TTF_CloseFont(Font);
   }
   Bench.TextTime.Stop();

   return 0; 
}

BOOL  SBPRIMARYBM::BlitFrom (SBBM &TecBitmap, XY p1, XY p2) { return 0; }
BOOL  SBPRIMARYBM::BlitFrom (SBBM &TecBitmap, SLONG tx, SLONG ty, SLONG tx2, SLONG ty2) { return 0; }
BOOL  SBPRIMARYBM::BlitFromT (SBBM &TecBitmap, XY p1, XY p2) { return 0; }
BOOL  SBPRIMARYBM::BlitFromT (SBBM &TecBitmap, SLONG tx, SLONG ty, SLONG tx2, SLONG ty2) { return 0; }

void  SBPRIMARYBM::Flip (XY WindowPos, BOOL ShowFPS)
{
   if (gFramesToDrawBeforeFirstBlend==0 && gBlendState!=-1 && Sim.Options.OptionBlenden && !bLeaveGameLoop)
   {
      if (gBlendState==-2)
      {
         gBlendState=8;
      }
      //else
      {
         if (gBlendState>=0 && gBlendState<=8)
         {
            if (Sim.Players.Players[Sim.localPlayer].LocationWin)
            {
               if (gBlendBm.Size.y==0) DebugBreak();

               //if (((CStdRaum*)Sim.Players.Players[Sim.localPlayer].LocationWin)->PicBitmap.Size.y==480)
                  PrimaryBm.SetClipRect(&CRect(0,0,640,480));
               /*else
                  PrimaryBm.SetClipRect(&CRect(0,0,640,440)); */

               if (/*gBlendState==7 ||*/ gBlendState==8)
               {
                  if (gBlendState==8)
                  {
                     gBlendBm2.ReSize (Size);

                     {
                        SB_CBitmapKey SrcKey(PrimaryBm);
                        SB_CBitmapKey TgtKey(*gBlendBm2.pBitmap);

                        if (SrcKey.Bitmap)
                           for (SLONG y=0; y<480; y++)
                              memcpy ((char*)TgtKey.Bitmap+y*TgtKey.lPitch, (char*)SrcKey.Bitmap+y*SrcKey.lPitch, 640*2);
                     }
                  }
                  BlitFrom (gBlendBm);
               }
               else if (gBlendState!=0)
               {
                  ColorFX.ApplyOn2 (gBlendState, gBlendBm.pBitmap, 8-gBlendState, gBlendBm2.pBitmap, &PrimaryBm);
               }
               PrimaryBm.SetClipRect(&CRect(0,0,640,480));

               gBlendState--;
            }
         }
         else gBlendState--;

         /*if (gBlendState>4)
         {
            UpdateStatusBar ();
            ColorFX.Apply (8-(8-gBlendState)*2, gBlendBm.pBitmap, &PrimaryBm);
         }

         if (gBlendState==4)
         {
            //Jaja, sehr seltsam. Warum, daﬂ steht in Gameframe.cpp
            SLONG c=0;

            SB_CBitmapKey SrcKey(PrimaryBm);
            SB_CBitmapKey TgtKey(*gBlendBm.pBitmap);

            for (SLONG y=0; y<480; y++)
               memcpy ((char*)TgtKey.Bitmap+y*TgtKey.lPitch, (char*)SrcKey.Bitmap+y*SrcKey.lPitch, 640*2);
         }

         if (gBlendState<=4)
         {
            UpdateStatusBar ();
            ColorFX.Apply (8-gBlendState*2, gBlendBm.pBitmap, &PrimaryBm);
         } */

         if (gBlendState==0)
         {
            gBlendBm.Destroy();
            gBlendBm2.Destroy();
            gBlendState--;
         }
      }
   }
   else if (FrameWnd && FrameWnd->PauseFade>0 && !Sim.bPause)
   {
      UpdateStatusBar ();
      if (Sim.Options.OptionBlenden)
         ColorFX.ApplyOn2 (8-FrameWnd->PauseFade, &PrimaryBm, FrameWnd->PauseFade, FrameWnd->PauseBm.pBitmap);

      FrameWnd->PauseFade--;

      if (FrameWnd->PauseFade==0)
      {
         FrameWnd->PauseBm.Destroy();
         if (FrameWnd->pGLibPause && pGfxMain) pGfxMain->ReleaseLib (FrameWnd->pGLibPause);
         FrameWnd->pGLibPause=NULL;
      }
   }

   if (gFramesToDrawBeforeFirstBlend>0) gFramesToDrawBeforeFirstBlend--;

   if (gNetworkBms.AnzEntries()>0 && gNetworkBms[0].Size.y && Sim.Players.Players[Sim.localPlayer].LocationWin)
   {
      BlitFrom (gNetworkBms[0], (640-((gNetworkBms[0].Size.x+3)&0xfffc))/2, (440-gNetworkBms[0].Size.y)/2);
      
      if (gNetworkBmsType && gNetworkBms.AnzEntries()>1)
      {
         for (SLONG c=0; c<4; c++)
            if (Sim.Players.Players[c].IsOut==0 && (Sim.Players.Players[c].Owner==0 || Sim.Players.Players[c].Owner==2))
            {
               SLONG add=0;

               if (gNetworkBmsType==1) add=Sim.Players.Players[c].bReadyForMorning;
               if (gNetworkBmsType==2) add=Sim.Players.Players[c].bReadyForBriefing;

               BlitFrom (gNetworkBms[1+c+4-4*add], (640-((gNetworkBms[0].Size.x+3)&0xfffc))-202+c*48, (440-gNetworkBms[0].Size.y)/2+76);
            }
      }
   }

   //TextOut (0, 20, RGB(0,0,255), RGB(255,255,0), bprintf ("%f FPS", GetFrameRate()));
   //TextOut (0, 32, RGB(0,0,255), RGB(255,255,0), bprintf ("%li Personen", Sim.Persons.GetNumUsed()));
   PrimaryBm.SetPos (CPoint(WindowPos.x, WindowPos.y));
   Bench.FlipTime.Start();
   PrimaryBm.Flip();
   Bench.FlipTime.Stop(); 

   if (MakeVideoPath.GetLength()>0 && MakeVideoPath[0]!=':')
   {
      //1/2 * 1/2 Einzelbilder:
      /*BUFFER<UBYTE> buf(320*240*3);
      UBYTE *p=buf;

      ((CFRONTDATA*)pCursor)->pBitmap->BlitFast (&PrimaryBm, gMousePosition.x, gMousePosition.y);

      SB_CBitmapKey SrcKey(PrimaryBm);
      UWORD *src=(UWORD *)SrcKey.Bitmap;

      if (SrcKey.Bitmap)
         for (SLONG y=0; y<240; y++)
         {
            for (SLONG x=0; x<320; x++)
            {
               p[0]=((src[x<<1]>>11)&31)<<3; p++;
               p[0]=((src[x<<1]>>5)&63)<<2;  p++;
               p[0]=((src[x<<1]&31)<<3);     p++;
            }

            src+=SrcKey.lPitch;
         }

      static n=0;

      TEAKFILE OutputFile (MakeVideoPath+bprintf ("vid%05li.raw", n), TEAKFILE_WRITE);

      OutputFile.Write ((UBYTE*)buf, 320*240*3);

      n++;  */

      static FILE *pFile=NULL;
      static SLONG FileIndex=0;
      static SBBM  OldFrame;

      //Vollbild Delta Video:
      const Uint8* States = SDL_GetKeyboardState(NULL);
      if (States [VK_SCROLL]&0x0001)
      {
         if (pFile==NULL)
         {
            pFile         = fopen (bprintf (MakeVideoPath, FileIndex), "wb");
            pSoundLogFile = fopen (CString (bprintf (MakeVideoPath, FileIndex))+".txt", "wb");

            SoundLogFileStartTime = timeGetTime();

            CString tmp = CString(":")+bprintf("%06i", timeGetTime()-SoundLogFileStartTime)+" Video recording begins\xd\xa";
            fwrite (tmp, 1, strlen(tmp), pSoundLogFile);

            OldFrame.ReSize (640, 480);
            OldFrame.FillWith (0);
         }

         ((CFRONTDATA*)pCursor)->pBitmap->BlitT (&PrimaryBm, gMousePosition.x-MouseCursorOffset.x, gMousePosition.y-MouseCursorOffset.y);

         deltaCompressFrame (pFile, *OldFrame.pBitmap, PrimaryBm, gScrollOffsetA, gScrollOffsetB);

         //PrimaryBm.BlitFast (OldFrame.pBitmap, 0, 0);

         gScrollOffsetA = XY(0,0);
         gScrollOffsetB = XY(0,0);
      }
      else
      {
         if (pFile)
         {
            CString tmp = CString(":")+bprintf("%06i", timeGetTime()-SoundLogFileStartTime)+" Video recording ends\xd\xa";
            fwrite (tmp, 1, strlen(tmp), pSoundLogFile);

            fclose (pFile);
            fclose (pSoundLogFile);
            pFile         = NULL;
            pSoundLogFile = NULL;
            FileIndex++;
         }
      }
   }
}

SBBMS::SBBMS () {}

void SBBMS::ReSize (GfxLib* gfxLibrary, __int64 graphicID, ...)
{
   SLONG           count=0;
   __int64         i=graphicID;
   va_list         marker;
   BUFFER<__int64> graphicIds;

   //Anzahl ermitteln:
   va_start (marker, graphicID);
   while ((i&0xffffffff)!=NULL)
   {
      count++;
      i = va_arg(marker, __int64);
   }
   va_end (marker);
   
   graphicIds.ReSize (count);

   //Und initialisieren:
   count=0, i=graphicID;

   va_start (marker, graphicID);
   while ((i&0xffffffff)!=NULL)
   {
      graphicIds[count++]=i;
      i = va_arg(marker, __int64);
   }
   va_end (marker);

   ReSize (gfxLibrary, graphicIds);
}

void SBBMS::ReSize (GfxLib* gfxLibrary, const BUFFER<__int64> &graphicIds, SLONG flags)
{
   SLONG c;

   Bitmaps.ReSize (graphicIds.AnzEntries());

   for (c=0; c<graphicIds.AnzEntries(); c++)
      Bitmaps[c].ReSize(gfxLibrary, graphicIds[c], flags);
}

void SBBMS::ReSize (CHLPool* pPool, const BUFFER<__int64> &graphicIds)
{
   SLONG c;

   Bitmaps.ReSize (graphicIds.AnzEntries());

   for (c=0; c<graphicIds.AnzEntries(); c++)
      Bitmaps[c].ReSize(pPool, graphicIds[c]);
}

void SBBMS::ReSize (GfxLib* gfxLibrary, const CString &graphicstr)
{
   SLONG           Anz;
   char           *Texts[200];
   BUFFER<__int64> graphicIds;
   BUFFER<char>    Str (graphicstr.GetLength()+1);

   strcpy (Str, graphicstr);

   for (Anz=0; ; Anz++)
   {
      if (Anz==0) Texts[Anz]=strtok (Str, " ");
             else Texts[Anz]=strtok (NULL, " ");
      if (!Texts[Anz]) break;
   }

   graphicIds.ReSize (Anz);

   for (SLONG c=0; c<Anz; c++)
   {
      graphicIds[c]=0;
      for (SLONG d=0; d<SLONG(strlen(Texts[c])); d++)
         graphicIds[c]+=__int64((Texts[c])[d])<<(8*d);
   }

   ReSize (gfxLibrary, graphicIds);
}

void SBBMS::ReSize (GfxLib* gfxLibrary, const CString &graphicstr, SLONG Anzahl, SLONG flags)
{
   BUFFER<__int64> graphicIds;
   BUFFER<char>    Str (graphicstr.GetLength()+1);

   strcpy (Str, graphicstr);

   graphicIds.ReSize (Anzahl);

   for (SLONG c=0; c<Anzahl; c++)
   {
      graphicIds[c]=0;
      for (SLONG d=0; d<SLONG(strlen(Str)); d++)
         graphicIds[c]+=__int64(Str[d])<<(8*d);

      Str[SLONG(strlen(Str)-1)]++;
      if (Str[SLONG(strlen(Str)-1)]>'9')
      {
         Str[SLONG(strlen(Str)-1)]='0';
         Str[SLONG(strlen(Str)-2)]++;
      }
   }

   ReSize(gfxLibrary, graphicIds, flags);
}

SBBMS::~SBBMS () {}

void SBPRIMARYBM::ReSize (SDL_Window *Wnd, BOOL Fullscreen, const XY &Resolution)
{
   if (MakeVideoPath.GetLength())
      PrimaryBm.Create(&lpDD, Wnd, 0, Resolution.x, Resolution.y, 16, 1);
   else if (Fullscreen)
      PrimaryBm.Create(&lpDD, Wnd, /*CREATE_VIDMEM|*/CREATE_FULLSCREEN, Resolution.x, Resolution.y, 16, 1);
   else
      PrimaryBm.Create(&lpDD, Wnd, CREATE_VIDMEM /*|CREATE_DONTUSECOLORKEY*/, Resolution.x, Resolution.y, 16, 1);

   SBPRIMARYBM::Fullscreen=Fullscreen;
   SBPRIMARYBM::Size.x = PrimaryBm.GetXSize();
   SBPRIMARYBM::Size.y = PrimaryBm.GetYSize();
}
