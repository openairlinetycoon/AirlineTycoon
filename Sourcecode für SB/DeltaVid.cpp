//============================================================================================
// DeltaVid.cpp : Funktionen zum laden und speichern von Delta Videos
//============================================================================================
#include "stdafx.h"

extern BOOL MakeUnvideoOn555;

static const char  gDeltaHeader[]     = "DeltaHeader\x1a";
static const UBYTE gDeltaTokenDelta1  = 252;
static const UBYTE gDeltaTokenDelta2  = 253;
static const UBYTE gDeltaTokenDelta3  = 254;
static const UBYTE gDeltaTokenNoDelta = 255;

//--------------------------------------------------------------------------------------------
//Dekomprimiert einen Frame mit einem Delta-Verfahren:
//--------------------------------------------------------------------------------------------
#pragma optimize("agptwy", on)
BOOL deltaCompressFrame (FILE *TargetFile, SB_CBitmapCore &OldFrame, SB_CBitmapCore &NewFrame, XY OffsetA, XY OffsetB)
{
   if (OldFrame.GetXSize()!=NewFrame.GetXSize()) return (FALSE);
   if (OldFrame.GetYSize()!=NewFrame.GetYSize()) return (FALSE);

   SLONG         x, y, y_pitch, pass;
   UBYTE         cx;

   BUFFER<UBYTE> Buffer(OldFrame.GetXSize()*OldFrame.GetYSize()*3);
   SLONG         BufferIndex=0;
   UBYTE       *pBufferCounter=NULL;

   SLONG         DirectOffsets[3];
   XY            Offsets[3];

   Offsets[0] = XY (0,0);
   Offsets[1] = OffsetA;
   Offsets[2] = OffsetA;

   //Direkten Zugriff auf Bitmaps beantragen:
   SB_CBitmapKey OldKey(OldFrame);
   SB_CBitmapKey NewKey(NewFrame);

   if (OldKey.lPitch!=NewKey.lPitch) return (FALSE);

   //Speed-Up für Offset-Berechnung
   for (pass=0; pass<3; pass++)
      DirectOffsets[pass]=Offsets[pass].x+Offsets[pass].y*NewKey.lPitch/2;

   //Kompression:
   for (y=y_pitch=0; y<NewFrame.GetYSize(); y++, y_pitch+=NewKey.lPitch/2)
   {
      for (x=0; x<NewFrame.GetXSize(); x++)
      {
         //Drei verschiedene Offsets probieren:
         if (x+3<NewFrame.GetXSize())
         {
            for (pass=0; pass<3; pass++)
               if ((-Offsets[pass].x)<=x && (-Offsets[pass].y)<=y && Offsets[pass].x<NewFrame.GetXSize()-x-3 && Offsets[pass].y<NewFrame.GetYSize()-y)
                  if ( ((__int64*)(((UWORD*)NewKey.Bitmap)+x+y_pitch))[0] == ((__int64*)(((UWORD*)OldKey.Bitmap)+x+y_pitch+DirectOffsets[pass]))[0] )
                  {
                     //Ein Offset hat gepasst! Für wieviele Pixel?
                     for (cx=4; x+cx<NewFrame.GetXSize() && cx<250; cx++)
                     {
                        if (Offsets[pass].x<NewFrame.GetXSize()-x-cx)
                        {
                           if ( ((__int64*)(((UWORD*)NewKey.Bitmap)+x+cx+y_pitch))[0] != ((__int64*)(((UWORD*)OldKey.Bitmap)+x+cx+y_pitch+DirectOffsets[pass]))[0] )
                              break;
                        }
                        else
                           break;
                     }

                     //Kompression speichern:
                     Buffer[BufferIndex++]=UBYTE(gDeltaTokenDelta1+pass);
                     Buffer[BufferIndex++]=cx;

                     //Und Rückweg zu den äußerer Schleifen vorbereiten:
                     x             += cx-1;
                     pBufferCounter = NULL;

                     break;
                  }
         }
         else pass=3;

         //Keine Kompression gefunden?
         if (pass==3)
         {
            //War das gerade eben auch schon so?
            if (pBufferCounter && (*pBufferCounter)<250 && x!=0)
            {
               (*pBufferCounter)++;
               *((UWORD*)(Buffer+BufferIndex)) = ((UWORD*)NewKey.Bitmap)[x+y_pitch];
               BufferIndex+=2;
            }
            else
            {
               Buffer[BufferIndex++]=gDeltaTokenNoDelta;  //Token
               Buffer[BufferIndex++]=1;                   //Anzahl
               pBufferCounter=Buffer+BufferIndex-1;

               *((UWORD*)(Buffer+BufferIndex)) = ((UWORD*)NewKey.Bitmap)[x+y_pitch];
               BufferIndex+=2;
            }
         }
      }
   }

   for (y=0; y<NewFrame.GetYSize(); y++)
      memcpy ((char*)OldKey.Bitmap+y*OldKey.lPitch, (char*)NewKey.Bitmap+y*NewKey.lPitch, NewFrame.GetXSize()*2);

   //Datei-Frame-header schreiben:
   fwrite (gDeltaHeader, 1, 13,             TargetFile);
   fwrite (&OffsetA,     1, sizeof (XY),    TargetFile);
   fwrite (&OffsetB,     1, sizeof (XY),    TargetFile);

   //Die Daten schreiben:
   fwrite (&BufferIndex, 1, sizeof (SLONG), TargetFile);
   fwrite (Buffer,       1, BufferIndex,    TargetFile);

   //Erfolg:
   return (TRUE);
}
#pragma optimize("agptwy", off)

//--------------------------------------------------------------------------------------------
//Dekomprimiert einen Frame mit einem Delta-Verfahren:
//--------------------------------------------------------------------------------------------
BOOL deltaDecompressFrame (FILE *SourceFile, SB_CBitmapCore &OldFrame, SB_CBitmapCore &NewFrame)
{
   if (OldFrame.GetXSize()!=NewFrame.GetXSize()) return (FALSE);
   if (OldFrame.GetYSize()!=NewFrame.GetYSize()) return (FALSE);

   SLONG         Anz, c, x, y;
   XY            OffsetA, OffsetB;

   BUFFER<UBYTE> Buffer;
   char          Header[13];

   XY            Offsets[3];

   Offsets[0] = XY (0,0);

   //Datei-Frame-header lesen:
   fread (Header,      1, 13,          SourceFile);
   fread (&Offsets[1], 1, sizeof (XY), SourceFile);
   fread (&Offsets[2], 1, sizeof (XY), SourceFile);

   //Prüfen, ob der Header stimmt:
   if (strcmp (Header, gDeltaHeader)) return (FALSE);

   //Die Daten lesen:
   fread (&x,      1, sizeof (SLONG), SourceFile);
   Buffer.ReSize (x);
   fread (Buffer,  1, x,              SourceFile);

   SB_CBitmapKey OldKey(OldFrame);
   SB_CBitmapKey NewKey(NewFrame);

   if (OldKey.lPitch!=NewKey.lPitch) return (FALSE);

   //Den Datenstrom decodieren:
   for (c=x=y=0; c<Buffer.AnzEntries();)
   {
      switch (Buffer[c])
      {
         case gDeltaTokenNoDelta:
            Anz=Buffer[c+1];
            memcpy (((UWORD*)NewKey.Bitmap)+x+y*NewKey.lPitch/2, Buffer+c+2, Anz*2);
            c+=1+1+Anz*2;
            x+=Anz;
            break;

         case gDeltaTokenDelta1:
         case gDeltaTokenDelta2:
         case gDeltaTokenDelta3:
            Anz=Buffer[c+1];
            memcpy (((UWORD*)NewKey.Bitmap)+x+y*NewKey.lPitch/2, ((UWORD*)OldKey.Bitmap)+(x+Offsets[Buffer[c]-gDeltaTokenDelta1].x)+(y+Offsets[Buffer[c]-gDeltaTokenDelta1].y)*OldKey.lPitch/2, Anz*2);
            c+=1+1;
            x+=Anz;
            break;

         default:
            return (FALSE);
      }

      if (x>=640)
      {
         x=0; y++;
      }
   }

   //Erfolg:
   return (TRUE);
}

//--------------------------------------------------------------------------------------------
//Dekomprimiert ein komplettes Video
//--------------------------------------------------------------------------------------------
void Unvideo (CString Filename, CString TargetFilename)
{
   SLONG cy;
   FILE  *pFile = fopen (Filename, "rb");

   SBBM OldBm(640,480), NewBm(640,480);

   char Pre  [] = "\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x02\xE0\x01"
                  "\x10";
   char Post [] = "\xEF\x01\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x00\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x00\x0B\x00\x03\x00\xCE\x07\x0E\x00\x05\x00"
                  "\x2A\x00\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x00\x00\x00\x00\x00\x00"
                  "\x00\x50\x61\x69\x6E\x74\x20\x53\x68\x6F\x70\x20\x50\x72\x6F\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
                  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x00\x00\x05\x00\x00\x00\x00"
                  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                  "\x00\x00\x00\x00\x00\x00\x12\x60\x09\x00\x00\x00\x00\x00\x54\x52"
                  "\x55\x45\x56\x49\x53\x49\x4F\x4E\x2D\x58\x46\x49\x4C\x45\x2E\x00";

   if (pFile)
   {
      SLONG num=0;

      while (deltaDecompressFrame (pFile, *OldBm.pBitmap, *NewBm.pBitmap))
      {
         FILE *pOut = fopen (bprintf((LPCTSTR)TargetFilename, num), "wb");

         fwrite (Pre, 1, sizeof(Pre), pOut);

         {
            SB_CBitmapKey NewKey(*NewBm.pBitmap);

            UWORD tmp[640];

            for (cy=NewBm.Size.y-1; cy>=0; cy--)
            {
               for (SLONG cx=0; cx<NewBm.Size.x; cx++)
               {
                  tmp[cx] = ((UWORD*)NewKey.Bitmap)[cy*NewKey.lPitch/2+cx];

                  UWORD &w = tmp[cx];

                  if (!MakeUnvideoOn555)
                     w = (w&31) + ((w&(0xffff-31-32))>>1);

                  //w = ((w&255)<<8)  + (w>>8);
               }

               fwrite (tmp, 1, NewBm.Size.x*2, pOut);
            }
         }

         fwrite (Post, 1, sizeof(Post), pOut);

         fclose (pOut);

         OldBm.BlitFrom (NewBm);

         num++;
      }
   }
}