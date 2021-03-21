//============================================================================================
//CColorFx - Klasse für Helligkeits- und Farbeffekte mit 16Bit Bitmaps
//============================================================================================
//Anleitung: "i:\projekt\sbl\doku\CColorFx.txt"
//Link:      "Colorfx.h"
//============================================================================================
#include "stdafx.h"

#define RDTSC __asm _emit 0x0F __asm _emit 0x31

//--------------------------------------------------------------------------------------------
// Default-Konstruktor
//--------------------------------------------------------------------------------------------
SB_CColorFX::SB_CColorFX ()
{
}

//--------------------------------------------------------------------------------------------
// Konstruktor
//--------------------------------------------------------------------------------------------
SB_CColorFX::SB_CColorFX (SB_CColorFXType FXType, SLONG Steps, SB_CBitmapCore *Bitmap)
{
   ReInit (FXType, Steps, Bitmap);
}

//--------------------------------------------------------------------------------------------
// Nachträglicher Konstruktor:
//--------------------------------------------------------------------------------------------
void SB_CColorFX::ReInit (SB_CColorFXType FXType, SLONG Steps, SB_CBitmapCore *Bitmap)
{
   SLONG c, d;
   SLONG AnzRBits=0, ShiftR=0, MaskR;
   SLONG AnzGBits=0, ShiftG=0, MaskG;
   SLONG AnzBBits=0, ShiftB=0, MaskB;

   //Bitstruktor ermitteln:
   MaskR = Bitmap->GetPixelFormat()->Rmask;
   MaskG = Bitmap->GetPixelFormat()->Gmask;
   MaskB = Bitmap->GetPixelFormat()->Bmask;

   for (c=0; c<32; c++)
   {
      if (MaskR & (1<<c))
      {
         AnzRBits++;
         if (c>ShiftR) ShiftR=c;
      }
      if (MaskG & (1<<c))
      {
         AnzGBits++;
         if (c>ShiftG) ShiftG=c;
      }
      if (MaskB & (1<<c))
      {
         AnzBBits++;
         if (c>ShiftB) ShiftB=c;
      }
   }

   AnzSteps=Steps+1;

   //Speicher für Tabelle reservieren:
   BlendTables.ReSize ((Steps+1)*256*2);

   //Effekte berechnen:
   if (FXType==SB_COLORFX_FADE)
   {
      for (c=0; c<=Steps; c++)
      {
         for (d=0; d<256; d++)
         {
            //Lower-Byte:
            BlendTables[(c<<9)+d]=UWORD(
               (((d&MaskR)*c/Steps)&MaskR)+
               (((d&MaskG)*c/Steps)&MaskG)+
               (((d&MaskB)*c/Steps)&MaskB));

            //High-Byte
            BlendTables[(c<<9)+d+256]=UWORD(
               ((((d<<8)&MaskR)*c/Steps)&MaskR)+
               ((((d<<8)&MaskG)*c/Steps)&MaskG)+
               ((((d<<8)&MaskB)*c/Steps)&MaskB));
         }
      }
   }
   else if (FXType==SB_COLORFX_GREY)
   {
      ShiftR -= ShiftB;
      ShiftG -= ShiftB;
      ShiftB -= ShiftB;

      for (c=0; c<=Steps; c++)
      {
         for (d=0; d<256; d++)
         {
            //Lower-Byte:
            BlendTables[(c<<9)+d]=UWORD(
               (( ((((d&MaskR)>>ShiftR)+((d&MaskG)>>ShiftG)+((d&MaskB)>>ShiftB))/3)<<ShiftR )&MaskR)+
               (( ((((d&MaskR)>>ShiftR)+((d&MaskG)>>ShiftG)+((d&MaskB)>>ShiftB))/3)<<ShiftG )&MaskG)+
               (( ((((d&MaskR)>>ShiftR)+((d&MaskG)>>ShiftG)+((d&MaskB)>>ShiftB))/3)<<ShiftB )&MaskB));

            //High-Byte
            BlendTables[(c<<9)+d+256]=UWORD(
               (( (((((d<<8)&MaskR)>>ShiftR)+(((d<<8)&MaskG)>>ShiftG)+(((d<<8)&MaskB)>>ShiftB))/3)<<ShiftR )&MaskR)+
               (( (((((d<<8)&MaskR)>>ShiftR)+(((d<<8)&MaskG)>>ShiftG)+(((d<<8)&MaskB)>>ShiftB))/3)<<ShiftG )&MaskG)+
               (( (((((d<<8)&MaskR)>>ShiftR)+(((d<<8)&MaskG)>>ShiftG)+(((d<<8)&MaskB)>>ShiftB))/3)<<ShiftB )&MaskB));
         }
      }
   }
   else if (FXType==(SB_CColorFXType)999) //Feuer
   {
      ShiftR -= ShiftB;
      ShiftG -= ShiftB;
      ShiftB -= ShiftB;

      for (c=0; c<=Steps; c++)
      {
         for (d=0; d<256; d++)
         {
            SLONG r,g,b;

            r=((d>>11)&31);
            g=((d>>5)&63);
            b=(d&31);

            r=min(31,r+4);
            g=min(63, g+4);
            b=min(31,b+1);

            //Lower-Byte:
            BlendTables[(c<<9)+d]=UBYTE((r<<11)+(g<<5)+b);

            r=(((d<<8)>>11)&31);
            g=(((d<<8)>>5)&63);
            b=((d<<8)&31);

            r=min(31,r+4);
            g=min(63, g+4);
            b=min(31,b+1);

            //High-Byte
            BlendTables[(c<<9)+d+256]=UWORD(((r<<11)+(g<<5)+b))&0xff00;
         }
      }
   }
}

//--------------------------------------------------------------------------------------------
//Wendet eine Effekt an:
//--------------------------------------------------------------------------------------------
void SB_CColorFX::Apply (SLONG Step, SB_CBitmapCore *Bitmap)
{
   SLONG  cx, cy;
   UWORD *p;
   UWORD *Table = BlendTables+(Step<<9);
   static SLONG sizex;

   SB_CBitmapKey Key(*Bitmap);
   if (Key.Bitmap==NULL) return;

   CRect ClipRect = Bitmap->GetClipRect();

   //sizex=Bitmap->GetXSize();
   sizex=ClipRect.right-ClipRect.left;

   for (cy=ClipRect.top; cy<ClipRect.bottom; cy++)
   //for (cy=0; cy<Bitmap->GetYSize(); cy++)
   {
      p = ((UWORD*) (((char*)Key.Bitmap) + cy*Key.lPitch))+ClipRect.left;

#ifndef ENABLE_ASM
      for (cx=sizex; cx>0; cx--)
      {
         *p = Table[((UBYTE*)p)[0]]+
              Table[256+((UBYTE*)p)[1]];
         p++;
      }
#else
      __asm
      {
         push  ebp
         push  esi
         push  edi

         mov   edi, p
         mov   eax, Table
         xor   edx, edx
         xor   ecx, ecx
         mov   ebx, 256
         mov   ebp, sizex
         ;shr   ebp, 1

Looping2:
         mov   dl, BYTE PTR [edi]
         mov   bl, BYTE PTR [edi+1]
         mov   cx, WORD PTR [eax+edx*2]
         add   cx, WORD PTR [eax+ebx*2]
         mov   dl, BYTE PTR [edi]
         test  ecx, ecx

         jz    oops_we_dont_want_transparency

         mov   WORD PTR [edi], cx

back_again:
         add   edi, 2
         ;rcl   ecx, 16
         ;mov   bl, BYTE PTR [edi+1]
         ;mov   cx, WORD PTR [eax+edx*2]
         ;add   cx, WORD PTR [eax+ebx*2]
         ;mov   DWORD PTR [edi], ecx
         ;add   edi, 4

         dec   ebp
         jnz   Looping2

         pop   edi
         pop   esi
         pop   ebp

         jmp   ende

oops_we_dont_want_transparency:
         mov   WORD PTR [edi], 1
         jmp   back_again

ende:
      }
#endif
   }
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
void SB_CColorFX::Apply (SLONG Step, SB_CBitmapCore *SrcBitmap, SB_CBitmapCore *TgtBitmap)
{
   SLONG  cx, cy;
   UWORD *p, *pp;
   UWORD *Table = BlendTables+(Step<<9);
   static SLONG sizex;

   SB_CBitmapKey SrcKey(*SrcBitmap);
   SB_CBitmapKey TgtKey(*TgtBitmap);
   if (SrcKey.Bitmap==NULL || TgtKey.Bitmap==NULL) return;

   sizex=SrcBitmap->GetXSize();

   for (cy=0; cy<SrcBitmap->GetYSize(); cy++)
   {
      p  = (UWORD*) (((char*)SrcKey.Bitmap) + cy*SrcKey.lPitch);
      pp = (UWORD*) (((char*)TgtKey.Bitmap) + cy*TgtKey.lPitch);

#ifndef ENABLE_ASM
      for (cx=SrcBitmap->GetXSize(); cx>0; cx--)
      {
         *pp = Table[((UBYTE*)p)[0]]+
               Table[256+((UBYTE*)p)[1]];
         p++; pp++;
      }
#else
      __asm
      {
         push  ebp
         push  esi
         push  edi

         mov   esi, p
         mov   edi, pp
         mov   eax, Table
         xor   edx, edx
         mov   ebx, 256
         mov   ebp, sizex
         shr   ebp, 1

Looping3:
         mov   dl, BYTE PTR [esi+2]
         mov   bl, BYTE PTR [esi+3]
         mov   cx, WORD PTR [eax+edx*2]
         mov   dl, BYTE PTR [esi]
         add   cx, WORD PTR [eax+ebx*2]
         add   edi, 4
         rcl   ecx, 16
         mov   bl, BYTE PTR [esi+1]
         mov   cx, WORD PTR [eax+edx*2]
         add   esi, 4
         add   cx, WORD PTR [eax+ebx*2]
         dec   ebp
         mov   DWORD PTR [edi-4], ecx

         jnz   Looping3

         pop   edi
         pop   esi
         pop   ebp
      }
#endif
   }
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
void SB_CColorFX::ApplyOn2 (SLONG Step, SB_CBitmapCore *DestBitmap, SLONG Step2, SB_CBitmapCore *SrcBitmap2)
{
   SLONG  cx, cy;
   UWORD *p, *pp;
   UWORD *Table = BlendTables+(Step<<9);
   UWORD *Table2 = BlendTables+(Step2<<9);
   static ULONG _ESP;
   static SLONG sizex;
   BUFFER<UWORD> PixelBuffer(640);

   SB_CBitmapKey Key(*DestBitmap);
   SB_CBitmapKey Key2(*SrcBitmap2);
   if (Key.Bitmap==NULL || Key2.Bitmap==NULL) return;

   sizex=DestBitmap->GetXSize();

   CRect ClipRect = DestBitmap->GetClipRect();

   for (cy=ClipRect.top; cy<ClipRect.bottom; cy++)
   {
      p = (UWORD*) (((char*)Key.Bitmap) + cy*Key.lPitch);
      pp = (UWORD*) (((char*)Key2.Bitmap) + cy*Key2.lPitch);

      memcpy (PixelBuffer, p, sizex*2);
      p = PixelBuffer;

#ifdef ENABLE_ASM
      __asm
      {
         push  ebp
         push  esi
         push  edi
         mov   _ESP, esp

         mov   edi, p
         mov   esi, pp
         mov   eax, Table
         mov   esp, Table2
         xor   edx, edx
         mov   ebx, 256
         mov   ebp, sizex
         shr   ebp, 1

         ;mov   dl, BYTE PTR [esi]
         ;mov   bl, BYTE PTR [esi+1]
         ;mov   cx, WORD PTR [esp+edx*2]
         ;add   cx, WORD PTR [esp+ebx*2]
         ;mov   dl, BYTE PTR [edi]
         ;mov   bl, BYTE PTR [edi+1]
         ;add   cx, WORD PTR [eax+edx*2]
         ;add   cx, WORD PTR [eax+ebx*2]

Looping:
         mov   dl, BYTE PTR [esi]
         mov   bl, BYTE PTR [esi+1]
         mov   cx, WORD PTR [esp+edx*2]
         add   cx, WORD PTR [esp+ebx*2]
         mov   dl, BYTE PTR [edi]
         mov   bl, BYTE PTR [edi+1]
         add   cx, WORD PTR [eax+edx*2]
         add   cx, WORD PTR [eax+ebx*2]
         mov   dl, BYTE PTR [esi+2]
         mov   WORD PTR [edi], cx
         mov   bl, BYTE PTR [esi+3]
         mov   cx, WORD PTR [esp+edx*2]
         add   cx, WORD PTR [esp+ebx*2]
         mov   dl, BYTE PTR [edi+2]
         mov   bl, BYTE PTR [edi+3]
         add   cx, WORD PTR [eax+edx*2]
         add   cx, WORD PTR [eax+ebx*2]
         add   esi, 4
         mov   WORD PTR [edi+2], cx

         add   edi, 4

         dec   ebp
         jnz   Looping

         mov   esp, _ESP
         pop   edi
         pop   esi
         pop   ebp
      }
#else
      for (cx=sizex; cx>0; cx--)
      {
         *p = Table[((UBYTE*)p)[0]]+Table[256+((UBYTE*)p)[1]]+
              Table2[((UBYTE*)pp)[0]]+Table2[256+((UBYTE*)pp)[1]];

         p++;
         pp++;
      }
#endif

      memcpy ((((char*)Key.Bitmap) + cy*Key.lPitch), PixelBuffer, sizex*2);
   }
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
void SB_CColorFX::ApplyOn2 (SLONG Step, SB_CBitmapCore *SrcBitmap, SLONG Step2, SB_CBitmapCore *SrcBitmap2, SB_CBitmapCore *TgtBitmap)
{
   SLONG  cx, cy;
   UWORD *p, *pp, *ppp;
   UWORD *Table = BlendTables+(Step<<9);
   UWORD *Table2 = BlendTables+(Step2<<9);
   static ULONG _ESP;
   static SLONG sizex;

   if (SrcBitmap==NULL || SrcBitmap2==NULL || TgtBitmap==NULL) return;

   SB_CBitmapKey Key(*SrcBitmap);
   SB_CBitmapKey Key2(*SrcBitmap2);
   SB_CBitmapKey TgtKey(*TgtBitmap);
   if (Key.Bitmap==NULL || Key2.Bitmap==NULL || TgtKey.Bitmap==NULL) return;

   CRect ClipRect = TgtBitmap->GetClipRect();

   for (cy=ClipRect.top; cy<ClipRect.bottom; cy++)
   {
      p = (UWORD*) (((char*)Key.Bitmap) + cy*Key.lPitch);
      pp = (UWORD*) (((char*)Key2.Bitmap) + cy*Key2.lPitch);
      ppp = (UWORD*) (((char*)TgtKey.Bitmap) + cy*TgtKey.lPitch);

#ifdef ENABLE_ASM
      sizex=SrcBitmap->GetXSize()/2;

      __asm
      {
         push  ebp
         push  esi
         push  edi
         mov   _ESP, esp

         mov   edi, p
         mov   esi, pp
         mov   eax, Table
         mov   esp, Table2
         xor   edx, edx
         mov   ebx, 256
         mov   ebp, ppp

Looping:
         mov   dl, BYTE PTR [esi]
         mov   bl, BYTE PTR [esi+1]
         mov   cx, WORD PTR [esp+edx*2]
         add   cx, WORD PTR [esp+ebx*2]
         mov   dl, BYTE PTR [edi]
         mov   bl, BYTE PTR [edi+1]
         add   cx, WORD PTR [eax+edx*2]
         add   cx, WORD PTR [eax+ebx*2]
         mov   dl, BYTE PTR [esi+2]
         mov   WORD PTR [ebp], cx
         mov   bl, BYTE PTR [esi+3]
         mov   cx, WORD PTR [esp+edx*2]
         add   cx, WORD PTR [esp+ebx*2]
         mov   dl, BYTE PTR [edi+2]
         mov   bl, BYTE PTR [edi+3]
         add   cx, WORD PTR [eax+edx*2]
         add   cx, WORD PTR [eax+ebx*2]
         add   esi, 4
         mov   WORD PTR [ebp+2], cx

         add   edi, 4
         add   ebp, 4

         dec   sizex
         jnz   Looping

         mov   esp, _ESP
         pop   edi
         pop   esi
         pop   ebp
      }
#else
      sizex = SrcBitmap->GetXSize();

      for (cx = sizex; cx > 0; cx--)
      {
         *ppp = Table[((UBYTE*)p)[0]] + Table[256 + ((UBYTE*)p)[1]] +
            Table2[((UBYTE*)pp)[0]] + Table2[256 + ((UBYTE*)pp)[1]];

         p++;
         pp++;
         ppp++;
      }
#endif
   }
}

//--------------------------------------------------------------------------------------------
//Blitten mit transparenten Whitespaces:
//--------------------------------------------------------------------------------------------
#pragma optimize("agptwy", on)
void SB_CColorFX::BlitWhiteTrans (BOOL DoMessagePump, SB_CBitmapCore *SrcBitmap, SB_CBitmapCore *TgtBitmap, const XY &TargetPos, const CRect *SrcRect, SLONG Grade)
{
   SLONG  cx, cy;
   UWORD *p, *pp;
   static UWORD *Table1 = BlendTables+(2<<9);
   static UWORD *Table2 = BlendTables+(6<<9);
   static SLONG  sizex;
   static ULONG _ESP;
   BUFFER<UWORD> PixelBuffer(640);

   IsPaintingTextBubble = TRUE;
   gRoomJustLeft        = FALSE;

   //DDSURFACEDESC DDSurfaceDesc;
   BOOL          bVgaRam = FALSE;

   //ZeroMemory (&DDSurfaceDesc, sizeof (DDSurfaceDesc));

   //DDSurfaceDesc.dwSize  = sizeof (DDSurfaceDesc);
   //DDSurfaceDesc.dwFlags = DDSD_CAPS;

   //TgtBitmap->GetSurface()->GetSurfaceDesc (&DDSurfaceDesc);

   //bVgaRam = ((DDSurfaceDesc.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)!=0);

   if (Grade!=-1)
   {
      Table1 = BlendTables+(Grade<<9);
      Table2 = BlendTables+((AnzSteps-Grade-1)<<9);
   }

   XY t=TargetPos;

   static UWORD White;
   CRect Rect;

   {
      SB_CBitmapKey Key(*XBubbleBms[9].pBitmap);
      White=*(UWORD*)Key.Bitmap;
   }

   if (SrcRect) Rect=*SrcRect; else Rect=CRect(0,0,SrcBitmap->GetXSize()-1,SrcBitmap->GetYSize()-1);

   if (t.x<0)
   {
      Rect.left-=t.x;
      t.x=0;
   }
   if (t.y<0)
   {
      Rect.top-=t.y;
      t.y=0;
   }
   if (t.x+Rect.right-Rect.left+1>=TgtBitmap->GetXSize())
   {
      Rect.right-=(t.x+Rect.right-Rect.left+1)-TgtBitmap->GetXSize();
   }
   if (t.y+Rect.bottom-Rect.top+1>=TgtBitmap->GetYSize())
   {
      Rect.bottom-=(t.y+Rect.bottom-Rect.top+1)-TgtBitmap->GetYSize();
   }

   SB_CBitmapKey Key(*TgtBitmap);
   SB_CBitmapKey Key2(*SrcBitmap);
   if (Key.Bitmap==NULL || Key2.Bitmap==NULL)
   {
      IsPaintingTextBubble = FALSE;
      return;
   }

   sizex=Rect.right-Rect.left+1;

   if (sizex>0 && sizex<=640)
      for (cy=0; cy<Rect.bottom-Rect.top+1; cy++)
      {
         //Zwischendurch mal einen Message-Pump einlegen:
         if ((cy&15)==15 && DoMessagePump)
         {
            //delete Key; delete Key2;

            SLONG LastSize = SrcBitmap->GetXSize();

            MessagePump ();

            if (gRoomJustLeft || TgtBitmap->GetXSize()==0 || SrcBitmap->GetXSize()==0)
            {
               IsPaintingTextBubble = FALSE;
               return;
            }

            if (LastSize != SrcBitmap->GetXSize())
            {
               IsPaintingTextBubble = FALSE;
               return;
            }

            Key2=SB_CBitmapKey(*SrcBitmap);

            //Falls der Key nicht mehr erhältlich ist, wurde die Sprechblase inzwischen geschlossen:
            if (Key2.Bitmap==0)
            {
               //delete Key2;
               IsPaintingTextBubble = FALSE;
               return;
            }

            Key= SB_CBitmapKey(*TgtBitmap);

            //Falls der Key nicht mehr erhältlich ist, wurde die Sprechlblase inzwischen geschlossen:
            if (Key.Bitmap==0 || Key2.Bitmap==0)
            {
               //delete Key;
               //delete Key2;
               IsPaintingTextBubble = FALSE;
               return;
            }
         }

         p = (UWORD*) (((char*)Key.Bitmap) + t.x*2 + (cy+t.y)*Key.lPitch);
         pp = (UWORD*) (((char*)Key2.Bitmap) + Rect.left*2 + (cy+Rect.top)*Key2.lPitch);

         if (bVgaRam)
         {
            memcpy (PixelBuffer, p, sizex*2);
            p = PixelBuffer;
         }

         if (Table1==Table2)
         {
            for (cx=sizex; cx>0; cx--)
            {
               if (*pp)
               {
                  if (*pp==(UWORD)(int)White)
                  {
                     register UWORD vga=*p;

                     *p = UWORD(Table1[vga&255]+Table1[256+(vga>>8)]+
                                Table1[((UBYTE*)pp)[0]]+Table1[256+((UBYTE*)pp)[1]]);
                  }
                  else
                     *p=*pp;
               }

               p++;
               pp++;
            }
         }
         else
         {
            for (cx=sizex; cx>0; cx--)
            {
               if (*pp)
               {
                  if (*pp==(UWORD)(int)White)
                     *p = UWORD(Table1[((UBYTE*)p)[0]]+Table1[256+((UBYTE*)p)[1]]+
                                Table2[((UBYTE*)pp)[0]]+Table2[256+((UBYTE*)pp)[1]]);
                  else
                     *p=*pp;
               }

               p++;
               pp++;
            }
         }

         if (bVgaRam)
            memcpy ((((char*)Key.Bitmap) + t.x*2 + (cy+t.y)*Key.lPitch), PixelBuffer, sizex*2);
      }

   //delete Key;
   //delete Key2;

   IsPaintingTextBubble = FALSE;
}
#pragma optimize("", off)

//--------------------------------------------------------------------------------------------
//Blitten mit transparenten Whitespaces:
//--------------------------------------------------------------------------------------------
#pragma optimize("agptwy", on)
void SB_CColorFX::BlitOutline (SB_CBitmapCore *SrcBitmap, SB_CBitmapCore *TgtBitmap, const XY &TargetPos, ULONG LineColor)
{
   SLONG  cx, cy;
   UWORD *p, *pp;

   XY t=TargetPos;
   CRect Rect=CRect(0,0,SrcBitmap->GetXSize()-1,SrcBitmap->GetYSize()-1);

   WORD pen = (WORD)TgtBitmap->GetHardwarecolor (LineColor);

   if (t.x<0)
   {
      Rect.left-=t.x;
      t.x=0;
   }
   if (t.y<0)
   {
      Rect.top-=t.y;
      t.y=0;
   }
   if (t.x+Rect.right-Rect.left+1>=TgtBitmap->GetXSize())
   {
      Rect.right-=(t.x+Rect.right-Rect.left+1)-TgtBitmap->GetXSize();
   }
   if (t.y+Rect.bottom-Rect.top+1>=TgtBitmap->GetYSize())
   {
      Rect.bottom-=(t.y+Rect.bottom-Rect.top+1)-TgtBitmap->GetYSize();
   }

   SB_CBitmapKey Key(*TgtBitmap);
   SB_CBitmapKey Key2(*SrcBitmap);
   if (Key.Bitmap==NULL || Key2.Bitmap==NULL)
      return;

   long sizex=Rect.right-Rect.left+1;
   long sizey=Rect.bottom-Rect.top+1;

   if (sizex>0 && sizex<=640)
      for (cy=0; cy<sizey; cy++)
      {
         p = (UWORD*) (((char*)Key.Bitmap) + t.x*2 + (cy+t.y)*Key.lPitch);
         pp = (UWORD*) (((char*)Key2.Bitmap) + Rect.left*2 + (cy+Rect.top)*Key2.lPitch);

         for (cx=sizex; cx>0; cx--)
         {
            //Falls transparent
            if (*pp==0)
            {
               //Dann in der Umgebung nach transparenten Pixeln suchen:
               if (cx>1)
               {
                  if (pp[1]) goto draw;
                  if (cy>0 && pp[1-Key2.lPitch/2]) goto draw;
                  if (cy<sizey-1 && pp[1+Key2.lPitch/2]) goto draw;
                  if (cx>2 && pp[2]) goto draw;
               }
               if (cx<sizex)
               {
                  if (pp[-1]) goto draw;
                  if (cy>0 && pp[-1-Key2.lPitch/2]) goto draw;
                  if (cy<sizey-1 && pp[-1+Key2.lPitch/2]) goto draw;
                  if (cx<sizex-1 && pp[-2]) goto draw;
               }

               if (cy>1 && pp[-Key2.lPitch])      goto draw;
               if (cy<sizey-2 && pp[Key2.lPitch]) goto draw;

               goto next;
draw:
               *p=pen;
next:;
            }

            p++;
            pp++;
         }
      }

   //delete Key;
   //delete Key2;
}
#pragma optimize("", off)

//--------------------------------------------------------------------------------------------
//Blitten mit transparenz (=fester Alpha-Wert) und clipping:
//--------------------------------------------------------------------------------------------
#pragma optimize("agptwy", on)
void SB_CColorFX::BlitTrans (SB_CBitmapCore *SrcBitmap, SB_CBitmapCore *TgtBitmap, const XY &TargetPos, const CRect *SrcRect, SLONG Grade)
{
   SLONG  cx, cy;
   UWORD *p, *pp;
   UWORD *Table1 = BlendTables+((AnzSteps/2)<<9);
   UWORD *Table2 = BlendTables+((AnzSteps/2)<<9);
   static SLONG sizex;
   BUFFER<UWORD> PixelBuffer(640);

   CRect ClipRect = TgtBitmap->GetClipRect();

   if (Grade!=-1)
   {
      Table1 = BlendTables+(Grade<<9);
      Table2 = BlendTables+((AnzSteps-Grade-1)<<9);
   }

   XY t=TargetPos;

   UWORD White;
   CRect Rect;

   {
      SB_CBitmapKey Key(*XBubbleBms[9].pBitmap);
      White=*(UWORD*)Key.Bitmap;
   }

   if (SrcRect) Rect=*SrcRect; else Rect=CRect(0,0,SrcBitmap->GetXSize()-1,SrcBitmap->GetYSize()-1);

   if (t.x<ClipRect.left)
   {
      Rect.left+=ClipRect.left-t.x;
      t.x=ClipRect.left;
   }
   if (t.y<ClipRect.top)
   {
      Rect.top+=ClipRect.top-t.y;
      t.y=ClipRect.top;
   }
   if (t.x+Rect.right-Rect.left+1>=ClipRect.right)
   {
      Rect.right-=(t.x+Rect.right-Rect.left+1)-ClipRect.right;
   }
   if (t.y+Rect.bottom-Rect.top+1>=ClipRect.bottom)
   {
      Rect.bottom-=(t.y+Rect.bottom-Rect.top+1)-ClipRect.bottom;
   }
   /*if (t.x<0)
   {
      Rect.left-=t.x;
      t.x=0;
   }
   if (t.y<0)
   {
      Rect.top-=t.y;
      t.y=0;
   }
   if (t.x+Rect.right-Rect.left+1>=TgtBitmap->GetXSize())
   {
      Rect.right-=(t.x+Rect.right-Rect.left+1)-TgtBitmap->GetXSize();
   }
   if (t.y+Rect.bottom-Rect.top+1>=TgtBitmap->GetYSize())
   {
      Rect.bottom-=(t.y+Rect.bottom-Rect.top+1)-TgtBitmap->GetYSize();
   }*/

   SB_CBitmapKey Key(*TgtBitmap);
   SB_CBitmapKey Key2(*SrcBitmap);
   if (Key.Bitmap==NULL || Key2.Bitmap==NULL) return;

   sizex=Rect.right-Rect.left+1;

   if (sizex>0)
      for (cy=0; cy<Rect.bottom-Rect.top+1; cy++)
      {
         p = (UWORD*) (((char*)Key.Bitmap) + t.x*2 + (cy+t.y)*Key.lPitch);
         pp = (UWORD*) (((char*)Key2.Bitmap) + Rect.left*2 + (cy+Rect.top)*Key2.lPitch);

         memcpy (PixelBuffer, p, sizex*2);
         p = PixelBuffer;

         if (Table1==Table2)
         {
            for (cx=sizex; cx>0; cx--)
            {
               if (*pp)
               {
                  *p = UWORD(Table1[((UBYTE*)p)[0]]+Table1[256+((UBYTE*)p)[1]]+
                             Table1[((UBYTE*)pp)[0]]+Table1[256+((UBYTE*)pp)[1]]);
               }

               p++;
               pp++;
            }
         }
         else
         {
            for (cx=sizex; cx>0; cx--)
            {
               if (*pp)
               {
                  *p = UWORD(Table1[((UBYTE*)p)[0]]+Table1[256+((UBYTE*)p)[1]]+
                             Table2[((UBYTE*)pp)[0]]+Table2[256+((UBYTE*)pp)[1]]);
               }

               p++;
               pp++;
            }
         }

         memcpy ((((char*)Key.Bitmap) + t.x*2 + (cy+t.y)*Key.lPitch), PixelBuffer, sizex*2);
      }
}
#pragma optimize("", off)

//--------------------------------------------------------------------------------------------
//Zeichnet einen transparenzen Highlight um einen Text:
//--------------------------------------------------------------------------------------------
#pragma optimize("agptwy", on)
void SB_CColorFX::HighlightText (SB_CBitmapCore *pBitmap, const CRect &HighRect, UWORD FontColor, ULONG HighlightColor)
{
   SLONG  x, y;
   SLONG  cx, cy;
   UWORD *p;
   UWORD *Table1 = BlendTables+(7<<9);
   UWORD *Table2 = BlendTables+(1<<9);
   static SLONG sizex, sizey;

   SLONG max = 3;

   //Calculate transparency color stuff:
   SB_Hardwarecolor color    = pBitmap->GetHardwarecolor (HighlightColor);
   UWORD            coloradd = UWORD(Table2[((UBYTE*)&color)[0]]+Table2[256+((UBYTE*)&color)[1]]);

   CRect ClipRect = pBitmap->GetClipRect();

   ClipRect.right--;
   ClipRect.bottom--;

   if (HighRect.left>ClipRect.left)     ClipRect.left=HighRect.left;
   if (HighRect.top>ClipRect.top)       ClipRect.top=HighRect.top;
   if (HighRect.right<ClipRect.right)   ClipRect.right=HighRect.right;
   if (HighRect.bottom<ClipRect.bottom) ClipRect.bottom=HighRect.bottom;

   sizex=ClipRect.right-ClipRect.left+1;
   sizey=ClipRect.bottom-ClipRect.top+1;

   SB_CBitmapKey Key(*pBitmap);
   if (Key.Bitmap==NULL) return;

   register SLONG Width = Key.lPitch/2;

   if (sizex>0)
      for (cy=0; cy<sizey; cy++)
      {
         p = (UWORD*) (((char*)Key.Bitmap) + ClipRect.left*2 + (cy+ClipRect.top)*Key.lPitch);

         for (cx=sizex; cx>0; cx--)
         {
            if (*p==FontColor)
            {
               for (x=-max; x<=max; x++)
                  for (y=-max+abs(x); y<=max-abs(x); y++)
                     if (cx+x>=0 && cx+x<sizex && cy+y>=0 && cy+y<sizey && p[x+y*Width]!=FontColor)
                        p[x+y*Width] = UWORD(Table1[((UBYTE*)(p+x+y*Width))[0]]+Table1[256+((UBYTE*)(p+x+y*Width))[1]]+coloradd);

               /*if (cx>1)
               {
                  if (p[-1]!=FontColor)         p[-1] = UWORD(Table1[((UBYTE*)(p-1))[0]]+Table1[256+((UBYTE*)(p-1))[1]]+coloradd);
                  if (cx>2 && p[-2]!=FontColor) p[-2] = UWORD(Table1[((UBYTE*)(p-2))[0]]+Table1[256+((UBYTE*)(p-2))[1]]+coloradd);
                  if (cy>0 && p[-1-Width]!=FontColor) p[-1-Width] = UWORD(Table1[((UBYTE*)(p-1-Width))[0]]+Table1[256+((UBYTE*)(p-1-Width))[1]]+coloradd);
                  if (cy<sizey-1 && p[-1+Width]!=FontColor) p[-1+Width] = UWORD(Table1[((UBYTE*)(p-1+Width))[0]]+Table1[256+((UBYTE*)(p-1+Width))[1]]+coloradd);
               }
               if (cx<sizex)
               {
                  if (p[1]!=FontColor)               p[1] = UWORD(Table1[((UBYTE*)(p+1))[0]]+Table1[256+((UBYTE*)(p+1))[1]]+coloradd);
                  if (cx<sizex-1 && p[2]!=FontColor) p[2] = UWORD(Table1[((UBYTE*)(p+2))[0]]+Table1[256+((UBYTE*)(p+2))[1]]+coloradd);
                  if (cy>0 && p[1-Width]!=FontColor) p[1-Width] = UWORD(Table1[((UBYTE*)(p+1-Width))[0]]+Table1[256+((UBYTE*)(p+1-Width))[1]]+coloradd);
                  if (cy<sizey-1 && p[1+Width]!=FontColor) p[1+Width] = UWORD(Table1[((UBYTE*)(p+1+Width))[0]]+Table1[256+((UBYTE*)(p+1+Width))[1]]+coloradd);
               }
               if (cy>0)
               {
                  if (p[-Width]!=FontColor)              p[-Width] = UWORD(Table1[((UBYTE*)(p-Width))[0]]+Table1[256+((UBYTE*)(p-Width))[1]]+coloradd);
                  if (cy>1 && p[-(Width<<1)]!=FontColor) p[-(Width<<1)] = UWORD(Table1[((UBYTE*)(p-(Width<<1)))[0]]+Table1[256+((UBYTE*)(p-(Width<<1)))[1]]+coloradd);
               }
               if (cy<sizey-1)
               {
                  if (p[Width]!=FontColor)                  p[Width] = UWORD(Table1[((UBYTE*)(p+Width))[0]]+Table1[256+((UBYTE*)(p+Width))[1]]+coloradd);
                  if (cy<sizey-2 && p[Width<<1]!=FontColor) p[Width<<1] = UWORD(Table1[((UBYTE*)(p+(Width<<1)))[0]]+Table1[256+((UBYTE*)(p+(Width<<1)))[1]]+coloradd);
               } */
            }

            p++;
         }
      }
}
#pragma optimize("", off)

//--------------------------------------------------------------------------------------------
//Blitten mit Alpha-Kanal für Schatten:
//--------------------------------------------------------------------------------------------
#pragma optimize("agptwy", on)
void SB_CColorFX::BlitAlpha (SB_CBitmapCore *SrcBitmap, SB_CBitmapCore *TgtBitmap, const XY &TargetPos)
{
   if (SrcBitmap==NULL) return;
   if (TargetPos.x>=640 || TargetPos.x+SrcBitmap->GetXSize()<0) return;

   SLONG  cx, cy;
   UWORD *p, *pp;
   static SLONG sizex;
   static ULONG _ESP;
   BUFFER<UWORD> PixelBuffer(640);

   XY t=TargetPos;

   if (SrcBitmap->GetXSize()<=0 || SrcBitmap->GetXSize()>=640) DebugBreak();

   UWORD White;
   CRect Rect;

   {
      SB_CBitmapKey Key(*XBubbleBms[9].pBitmap);
      White=*(UWORD*)Key.Bitmap;
   }

   Rect=CRect(0,0,SrcBitmap->GetXSize()-1,SrcBitmap->GetYSize()-1);

   if (t.x<0)
   {
      Rect.left-=t.x;
      t.x=0;
   }
   if (t.y<0)
   {
      Rect.top-=t.y;
      t.y=0;
   }
   if (t.x+Rect.right-Rect.left+1>=TgtBitmap->GetXSize())
   {
      Rect.right-=(t.x+Rect.right-Rect.left+1)-TgtBitmap->GetXSize();
   }
   if (t.y+Rect.bottom-Rect.top+1>=min(TgtBitmap->GetYSize(), 440))
   {
      Rect.bottom-=(t.y+Rect.bottom-Rect.top+1)-min(TgtBitmap->GetYSize(), 440);
   }

   SB_CBitmapKey Key(*TgtBitmap);
   SB_CBitmapKey Key2(*SrcBitmap);
   if (Key.Bitmap==NULL || Key2.Bitmap==NULL) return;

   sizex=Rect.right-Rect.left+1;

   if (sizex>0)
      for (cy=0; cy<Rect.bottom-Rect.top+1; cy++)
      {
         p = (UWORD*) (((char*)Key.Bitmap) + t.x*2 + (cy+t.y)*Key.lPitch);
         pp = (UWORD*) (((char*)Key2.Bitmap) + Rect.left*2 + (cy+Rect.top)*Key2.lPitch);

         memcpy (PixelBuffer, p, sizex*2);
         p = PixelBuffer;

         UWORD *Table = BlendTables;

#ifdef ENABLE_ASM
         __asm
         {
            push  ebp
            push  esi
            push  edi
            mov   _ESP, esp

            mov   edi, p
            mov   esi, pp
            mov   eax, Table
            ;mov   esp, Table2
            xor   esp, esp
            xor   edx, edx
            mov   ebx, 256
            mov   ebp, sizex

   Looping:
            mov   sp, WORD PTR [esi]
            ;mov   dl, BYTE PTR [esi]
            ;mov   bl, BYTE PTR [esi+1]
            ;mov   cx, WORD PTR [esp+edx*2]
            ;add   cx, WORD PTR [esp+ebx*2]
            mov   dl, BYTE PTR [edi]
            shl   esp, 10
            mov   bl, BYTE PTR [edi+1]
            add   eax, esp
            mov   cx, WORD PTR [eax+edx*2]
            add   cx, WORD PTR [eax+ebx*2]
            mov   WORD PTR [edi], cx
            sub   eax, esp

            add   esi, 2
            add   edi, 2

            dec   ebp
            jnz   Looping

            mov   esp, _ESP
            pop   edi
            pop   esi
            pop   ebp
         }
#else
         /*for (cx=sizex; cx>0; cx--)
         {
            *p = Table[((UBYTE*)p)[0]]+Table[256+((UBYTE*)p)[1]]+
                 Table2[((UBYTE*)pp)[0]]+Table2[256+((UBYTE*)pp)[1]];

            p++;
            pp++;
         }*/

         for (cx=sizex; cx>0; cx--)
         {
            UWORD *Table1 = BlendTables+(SLONG(*pp)<<9);

            *p = UWORD(Table1[((UBYTE*)p)[0]]+Table1[256+((UBYTE*)p)[1]]);

            p++;
            pp++;
         }
#endif
         memcpy ((((char*)Key.Bitmap) + t.x*2 + (cy+t.y)*Key.lPitch), PixelBuffer, sizex*2);
      }
}
#pragma optimize("", off)

//--------------------------------------------------------------------------------------------
//Blitten mit Glow-Effekt fürs Tutorial:
//--------------------------------------------------------------------------------------------
//#pragma optimize("agptwy", on)
void SB_CColorFX::BlitGlow (SB_CBitmapCore *SrcBitmap, SB_CBitmapCore *TgtBitmap, const XY &TargetPos)
{
   if (TargetPos.x>=640 || TargetPos.x+SrcBitmap->GetXSize()<0) return;

   SLONG  cx, cy;
   UWORD *p, *pp;
   static SLONG sizex;
   static ULONG _ESP;
   BUFFER<UWORD> PixelBuffer(640);

   XY t=TargetPos;

   if (SrcBitmap->GetXSize()<=0 || SrcBitmap->GetXSize()>=640) DebugBreak();

   UWORD White;
   CRect Rect;

   {
      SB_CBitmapKey Key(*XBubbleBms[9].pBitmap);
      White=*(UWORD*)Key.Bitmap;
   }

   Rect=CRect(0,0,SrcBitmap->GetXSize()-1,SrcBitmap->GetYSize()-1);

   if (t.x<0)
   {
      Rect.left-=t.x;
      t.x=0;
   }
   if (t.y<0)
   {
      Rect.top-=t.y;
      t.y=0;
   }
   if (t.x+Rect.right-Rect.left+1>=TgtBitmap->GetXSize())
   {
      Rect.right-=(t.x+Rect.right-Rect.left+1)-TgtBitmap->GetXSize();
   }
   if (t.y+Rect.bottom-Rect.top+1>=min(TgtBitmap->GetYSize(), 440))
   {
      Rect.bottom-=(t.y+Rect.bottom-Rect.top+1)-min(TgtBitmap->GetYSize(), 440);
   }

   SB_CBitmapKey Key(*TgtBitmap);
   SB_CBitmapKey Key2(*SrcBitmap);
   if (Key.Bitmap==NULL || Key2.Bitmap==NULL) return;

   sizex=Rect.right-Rect.left+1;

   BUFFER<SLONG> Map1(BlendTables.AnzEntries()/512);
   BUFFER<SLONG> Map2(BlendTables.AnzEntries()/512);

   SLONG *pMap1=Map1;
   SLONG *pMap2=Map2;

   SLONG Strength = SLONG(sin(timeGetTime()/150.0)*4+4);

   for (cy=0; cy<BlendTables.AnzEntries()/512; cy++)
   {
      pMap1[cy]=BlendTables.AnzEntries()/512-1-(8-cy)*(8-Strength)/16;
      pMap2[cy]=(8-cy)*(8-Strength)/16;
   }

   if (sizex>0)
      for (cy=0; cy<Rect.bottom-Rect.top+1; cy++)
      {
         p = (UWORD*) (((char*)Key.Bitmap) + t.x*2 + (cy+t.y)*Key.lPitch);
         pp = (UWORD*) (((char*)Key2.Bitmap) + Rect.left*2 + (cy+Rect.top)*Key2.lPitch);

         memcpy (PixelBuffer, p, sizex*2);
         p = PixelBuffer;

         UWORD *Table = BlendTables;

         for (cx=sizex; cx>0; cx--)
         {
            UWORD *Table1 = BlendTables+(pMap1[*pp]<<9);
            UWORD *Table2 = BlendTables+(pMap2[*pp]<<9);

            *p = UWORD(Table1[((UBYTE*)p)[0]]+Table1[256+((UBYTE*)p)[1]])+UWORD(Table2[255]+Table2[256+255]);

            p++;
            pp++;
         }

         memcpy ((((char*)Key.Bitmap) + t.x*2 + (cy+t.y)*Key.lPitch), PixelBuffer, sizex*2);
      }
}
//#pragma optimize("", off)

#pragma optimize("agptwy", on)
void RemapColor (SB_CBitmapCore *pBitmap, const CRect &HighRect, UWORD OldFontColor, ULONG NewFontColor)
{
   SLONG  cx, cy;
   UWORD *p;
   static SLONG sizex, sizey;

   CRect ClipRect = pBitmap->GetClipRect();

   ClipRect.right--;
   ClipRect.bottom--;

   if (HighRect.left>ClipRect.left)     ClipRect.left=HighRect.left;
   if (HighRect.top>ClipRect.top)       ClipRect.top=HighRect.top;
   if (HighRect.right<ClipRect.right)   ClipRect.right=HighRect.right;
   if (HighRect.bottom<ClipRect.bottom) ClipRect.bottom=HighRect.bottom;

   sizex=ClipRect.right-ClipRect.left+1;
   sizey=ClipRect.bottom-ClipRect.top+1;

   SB_CBitmapKey Key(*pBitmap);
   if (Key.Bitmap==NULL) return;

   register SLONG Width = Key.lPitch/2;

   if (sizex>0)
      for (cy=0; cy<sizey; cy++)
      {
         p = (UWORD*) (((char*)Key.Bitmap) + ClipRect.left*2 + (cy+ClipRect.top)*Key.lPitch);

         for (cx=sizex; cx>0; cx--)
         {
            if (*p==OldFontColor) *p=(UWORD)NewFontColor;

            p++;
         }
      }
}
#pragma optimize("", off)
