//============================================================================================
// HLine.cpp : Die HLine-Engine
//============================================================================================
// Link: "HLine.h"
//============================================================================================
#include "stdafx.h"
#include "HLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Zum debuggen:
static const char FileId[] = "HLin";

//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
CHLObj::CHLObj ()
{
   graphicID = 0;
   pHLPool   = 0;
}

//--------------------------------------------------------------------------------------------
//Destruktor:
//--------------------------------------------------------------------------------------------
CHLObj::~CHLObj ()
{
   Destroy ();
}

//--------------------------------------------------------------------------------------------
//Pseudo-Destruktor:
//--------------------------------------------------------------------------------------------
void CHLObj::Destroy (void)
{
   graphicID = 0;
   pHLPool   = 0;
   HLines.ReSize (0);
}

#pragma optimize("agptwy", on)
//--------------------------------------------------------------------------------------------
//Blittet ein HL-Objekt an eine Stelle, bis jetzt aber ohne Clipping
//--------------------------------------------------------------------------------------------
void CHLObj::BlitAt (SB_CBitmapCore *pBitmap, XY Target)
{
   static char FirstTime;
   static ULONG JumpBuffer[8];  //Für Wiederholungen

   SLONG cx, cy;
   SLONG count;

   CRect ClipRect = pBitmap->GetClipRect();

   //Knock-Out für horizontales Clipping?
   if (Target.x>=ClipRect.right || Target.x+Size.x<=ClipRect.left) return;

   //Pool und Sub-Pool vorbereiten:
   pHLPool->Load();

   SB_CBitmapKey Key(*pBitmap);
   if (!Key.Bitmap) return;
   UWORD *bm=(UWORD *)(((UWORD*)Key.Bitmap)+Target.x+Target.y*(Key.lPitch>>1));

   SLONG Min, Max; //Vertikales Clipping
   Min = max (0, ClipRect.top-Target.y);
   Max = min (HLineEntries.AnzEntries(), ClipRect.bottom-Target.y);

   ClipRect.left-=Target.x;
   ClipRect.right-=Target.x;

   bm   += Key.lPitch/2*Min;

   //count für die übersprungenen Zeilen initialisieren
   for (count=cy=0; cy<Min; cy++)
      count+=HLineEntries[cy];

   for (cy=Min; cy<Max; cy++)
   {
      for (cx=HLineEntries[cy]; cx>0; cx--)
      {
         CHLGene &qHLGene = HLines[count];

         UWORD *target=((UWORD*)bm)+qHLGene.Offset;
         UBYTE *source=qHLGene.pPixel;
         UWORD *table=pHLPool->PaletteMapper;
         long   anz=qHLGene.Anz;

         if (qHLGene.Anz<=4) source = (UBYTE*)&qHLGene.pPixel;
         else                source = qHLGene.pPixel;

         if (qHLGene.Offset<ClipRect.left)
         {
            source+=ClipRect.left-qHLGene.Offset;
            target+=ClipRect.left-qHLGene.Offset;
            anz-=ClipRect.left-SLONG(qHLGene.Offset);
         }
         if (SLONG(qHLGene.Offset)+anz>ClipRect.right)
         {
            anz=ClipRect.right-SLONG(qHLGene.Offset);
         }

         /*if (anz>0)
            for (SLONG c=anz-1; c>=0; c--)
               target[c]=pHLPool->PaletteMapper[(SLONG)source[c]];*/

         if (anz>0)
         _asm
         {
             //Heute schon initialisiert?
             mov     al, FirstTime
             test    al, al
             jnz     AlreadyInitialized

             //Nein, wir führen also die First-Time Initialisierung durch:
             lea     edx, JumpBuffer

             lea     eax, CopyWords08
             mov     [edx+0],eax
             lea     eax, CopyWords01
             mov     [edx+4],eax
             lea     eax, CopyWords02
             mov     [edx+8],eax
             lea     eax, CopyWords03
             mov     [edx+12],eax
             lea     eax, CopyWords04
             mov     [edx+16],eax
             lea     eax, CopyWords05
             mov     [edx+20],eax
             lea     eax, CopyWords06
             mov     [edx+24],eax
             lea     eax, CopyWords07
             mov     [edx+28],eax
             lea     eax, CopyWords08

             mov     al, 1
             mov     FirstTime,1
             //Die First-Time Initialisierung ist abgeschlossen:

AlreadyInitialized:
             mov     esi, source
             mov     edi, target
             mov     eax, anz
             mov     edx, table

             xor     ecx, ecx

             and     eax, 7
             mov     ebx, JumpBuffer[eax*4]       ;Load Label-Table
             jmp     ebx

//Gerade Anzahl kopieren:
CopyWords08: mov     eax, 8

             mov     cl, [esi+7]
             mov     bx, WORD PTR [edx+ecx*2]
             shl     ebx, 16
             mov     cl, [esi+6]
             mov     bx, WORD PTR [edx+ecx*2]
             mov     [edi+12], ebx
CopyWords06: mov     cl, [esi+5]
             mov     bx, WORD PTR [edx+ecx*2]
             shl     ebx, 16
             mov     cl, [esi+4]
             mov     bx, WORD PTR [edx+ecx*2]
             mov     [edi+8], ebx
CopyWords04: mov     cl, [esi+3]
             mov     bx, WORD PTR [edx+ecx*2]
             shl     ebx, 16
             mov     cl, [esi+2]
             mov     bx, WORD PTR [edx+ecx*2]
             mov     [edi+4], ebx
CopyWords02: mov     cl, [esi+1]
             mov     bx, WORD PTR [edx+ecx*2]
             shl     ebx, 16
             mov     cl, [esi+0]
             mov     bx, WORD PTR [edx+ecx*2]
             mov     [edi+0], ebx

             lea     esi, [esi+eax]
             lea     edi, [edi+eax*2]

             sub     anz, eax
             jz      game_over

             mov     eax, anz
             and     eax, 7
             mov     ebx, JumpBuffer[eax*4]       ;Load Label-Table
             jmp     ebx

//Ungerade Anzahl kopieren:
CopyWords07: mov     cl, [esi+6]
             mov     bx, WORD PTR [edx+ecx*2]
             shl     ebx, 16
             mov     cl, [esi+5]
             mov     bx, WORD PTR [edx+ecx*2]
             mov     [edi+10], ebx
CopyWords05: mov     cl, [esi+4]
             mov     bx, WORD PTR [edx+ecx*2]
             shl     ebx, 16
             mov     cl, [esi+3]
             mov     bx, WORD PTR [edx+ecx*2]
             mov     [edi+6], ebx
CopyWords03: mov     cl, [esi+2]
             mov     bx, WORD PTR [edx+ecx*2]
             shl     ebx, 16
             mov     cl, [esi+1]
             mov     bx, WORD PTR [edx+ecx*2]
             mov     [edi+2], ebx
CopyWords01: mov     cl, [esi+0]
             mov     bx, WORD PTR [edx+ecx*2]
             mov     [edi+0], bx

             lea     esi, [esi+eax]
             lea     edi, [edi+eax*2]

             sub     anz, eax
             jz      game_over

             mov     eax, anz
             and     eax, 7
             mov     ebx, JumpBuffer[eax*4]       ;Load Label-Table
             jmp     ebx

game_over:
         }

         count++;
      }
      bm += Key.lPitch/2;
   }
}

//--------------------------------------------------------------------------------------------
//Blittet und vergrößert:
//--------------------------------------------------------------------------------------------
void CHLObj::BlitLargeAt (SB_CBitmapCore *pBitmap, XY Target)
{
   static char FirstTime;
   static ULONG JumpBuffer[8];  //Für Wiederholungen

   SLONG cx, cy;
   SLONG count;

   CRect ClipRect = pBitmap->GetClipRect();

   //Knock-Out für horizontales Clipping?
   if (Target.x>=ClipRect.right || Target.x+Size.x*2<=ClipRect.left) return;

   //Pool und Sub-Pool vorbereiten:
   pHLPool->Load();

   SB_CBitmapKey Key(*pBitmap);
   if (!Key.Bitmap) return;
   UWORD *bm=(UWORD *)(((UWORD*)Key.Bitmap)+Target.x+Target.y*(Key.lPitch>>1));

   SLONG Min, Max; //Vertikales Clipping
   Min = max (0, ClipRect.top-Target.y);
   Max = min (HLineEntries.AnzEntries(), (ClipRect.bottom-Target.y)/2);

   ClipRect.left-=Target.x;
   ClipRect.right-=Target.x;

   bm   += Key.lPitch/2*Min;
   count = Min;

   for (cy=Min; cy<Max; cy++)
   {
      for (cx=HLineEntries[cy]; cx>0; cx--)
      {
         CHLGene &qHLGene = HLines[count];

         UWORD *target=((UWORD*)bm)+qHLGene.Offset*2;
         UBYTE *source=qHLGene.pPixel;
         UWORD *table=pHLPool->PaletteMapper;
         long   anz=qHLGene.Anz*2;

         if (qHLGene.Anz<=4) source = (UBYTE*)&qHLGene.pPixel;
         else                source = qHLGene.pPixel;

         if (qHLGene.Offset*2<ClipRect.left)
         {
            source+=(ClipRect.left-qHLGene.Offset*2)/2;
            target+=ClipRect.left-qHLGene.Offset*2;
            anz-=ClipRect.left-qHLGene.Offset*2;
         }
         if (qHLGene.Offset*2+anz>ClipRect.right)
         {
            anz=ClipRect.right-qHLGene.Offset*2;
         }

         if (anz>0)
            for (SLONG c=anz-1; c>=0; c--)
               target[c+Key.lPitch/2]=target[c]=pHLPool->PaletteMapper[(SLONG)(ULONG)source[c/2]];

         count++;
      }
      bm += Key.lPitch/2*2;
   }
}

#pragma optimize("agptwy", off)

//--------------------------------------------------------------------------------------------
//CHLPool::
//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
CHLPool::CHLPool ()
{
   pPool        = NULL;
   PoolSize     = 0;
   pHLBasepool1 = NULL;
   pHLBasepool2 = NULL;

   Loaded      = 0;

   BytesReal   = BytesCompressed = BytesAdministration = 0;
   LinesInPool = LinesRepeated   = 0;
}

//--------------------------------------------------------------------------------------------
//Destruktor:
//--------------------------------------------------------------------------------------------
CHLPool::~CHLPool ()
{
   Destroy ();
}

//--------------------------------------------------------------------------------------------
//Pseudo-Destruktor:
//--------------------------------------------------------------------------------------------
void CHLPool::Destroy (void)
{
   HLObjects.ReSize (0);

   if (pPool)
   {
      delete [] pPool;
      pPool=NULL;
   }

   PoolSize     = 0;
   PoolMaxSize  = 0;

   Loaded       = 0;

   pHLBasepool1 = NULL;
   pHLBasepool2 = NULL;

   BytesReal = BytesCompressed = BytesAdministration = 0;

   Filename.Empty();
}

//--------------------------------------------------------------------------------------------
//Lädt die Bitmap-Daten ohne die eigentlichen Bitmaps...
//--------------------------------------------------------------------------------------------
BOOL CHLPool::PreLoad (void)
{
   //Ggf. Basepool-Objekte laden
   /*if (pHLBasepool1)
      if (!pHLBasepool1->Load ()) return (FALSE);

   if (pHLBasepool2)
      if (!pHLBasepool2->Load ()) return (FALSE);*/

   if (Loaded==0)
   {
      CString CompleteFilename = FullFilename ((LPCTSTR)Filename, GliPath);

      if (DoesFileExist (CompleteFilename))
      {
         TEAKFILE File (CompleteFilename, TEAKFILE_READ);

         File >> PoolSize  >> PoolMaxSize;
         File >> BytesReal >> BytesCompressed >> BytesAdministration >> LinesInPool >> LinesRepeated;

         File >> HLObjects;
         for (SLONG c=HLObjects.AnzEntries()-1; c>=0; c--)
            HLObjects[c].pHLPool=this;
      }
      else
      {
         HLObjects.ReSize (0);
      }

      Loaded = 1;
   }

   return (FALSE);
}

//--------------------------------------------------------------------------------------------
//Lädt den Pool in den Speicher
//--------------------------------------------------------------------------------------------
BOOL CHLPool::Load (void)
{
   //Ggf. Basepool-Objekte laden
   if (pHLBasepool1)
      if (!pHLBasepool1->Load ()) return (FALSE);

   if (pHLBasepool2)
      if (!pHLBasepool2->Load ()) return (FALSE);

   if (Loaded!=2)
   {
      CString CompleteFilename = FullFilename ((LPCTSTR)Filename, GliPath);

      if (DoesFileExist (CompleteFilename))
      {
         SLONG    c;
         SBBM     TmpBm (10,10);
         TEAKFILE File (CompleteFilename, TEAKFILE_READ);

         File >> PoolSize  >> PoolMaxSize;
         File >> BytesReal >> BytesCompressed >> BytesAdministration >> LinesInPool >> LinesRepeated;

         //Wurden diese Daten schon vorher geladen? Dann reicht einmal!
         if (HLObjects.AnzEntries())
         {
            FBUFFER<CHLObj> DummyHLObjects;

            File >> DummyHLObjects;
         }
         else
         {
            File >> HLObjects;
            for (c=HLObjects.AnzEntries()-1; c>=0; c--)
               HLObjects[c].pHLPool=this;
         }

         if (PoolMaxSize>0)
         {
            pPool = new UBYTE[PoolMaxSize];
            File.Read ((UBYTE*)pPool, PoolSize);
         }
         else pPool = new UBYTE[1];

         PaletteMapper.ReSize (256);
         File.Read ((UBYTE*)&PaletteMapper[0], 512);

         DoBaseObjects ();

         //Map colors to graphic card bits 555 or 565:
         for (c=0; c<256; c++)
         {
			   ULONG p = PaletteMapper[c];

            p= ((p&31)<<(3)) + (((p>>6)&31)<<(3+8)) + (((p>>11)&31)<<(3+16));

            PaletteMapper[c]=(UWORD)TmpBm.pBitmap->GetHardwarecolor (p);
         }
      }

      Loaded = 2;
   }

   return (TRUE);
}

//--------------------------------------------------------------------------------------------
//Speichert das Objekt:
//--------------------------------------------------------------------------------------------
BOOL CHLPool::Save (void)
{
   if (BytesCompressed)
   {
      TEAKFILE File (FullFilename ((LPCTSTR)Filename, GliPath), TEAKFILE_WRITE);

      UnBaseObjects ();

      File << PoolSize  << PoolMaxSize;
      File << BytesReal << BytesCompressed << BytesAdministration << LinesInPool << LinesRepeated;

      File << HLObjects;

      if (PoolSize) File.Write ((UBYTE*)pPool, PoolSize);

      File.Write ((UBYTE*)&PaletteMapper[0], 512);

      DoBaseObjects ();
   }

   return (TRUE);
}
 
//--------------------------------------------------------------------------------------------
//Wirft den gesammten Pool aus dem Speicher raus:
//--------------------------------------------------------------------------------------------
void CHLPool::Unload (void)
{
   if (pPool)
   {
      UnBaseObjects ();

      if (pPool)
      {
         delete [] pPool;
         pPool=NULL;
      }

      Loaded       = min (1, Loaded);

      PoolSize     = 0;
      PoolMaxSize  = 0;
   }
}

//--------------------------------------------------------------------------------------------
//Konstruirt den Pool anhand einer Datei
//--------------------------------------------------------------------------------------------
void CHLPool::ReSize (const CString &Filename, CHLPool *pHLBasepool1, CHLPool *pHLBasepool2)
{
   Destroy ();  //Altes Objekt zerstören

   CHLPool::Filename     = Filename;
   CHLPool::pHLBasepool1 = pHLBasepool1;
   CHLPool::pHLBasepool2 = pHLBasepool2;
}

//--------------------------------------------------------------------------------------------
//Fügt dem Pool eine neue Bitmap hinzu:
// * Quality : Bildqualität der Kompression in % (bis zu 100%)
// * Speed   : Entpackgeschindigkkeit, 100% ist Maximum
//--------------------------------------------------------------------------------------------
//#pragma optimize("agptwy", on)
void CHLPool::AddBitmap (__int64 graphicID, SB_CBitmapCore *pBitmap, PALETTE *Pal, SLONG Quality, SLONG Speed)
{
   //Zielobjekt vorbereiten:
   HLObjects.ReSize (HLObjects.AnzEntries()+1);
   CHLObj &qObj = HLObjects [HLObjects.AnzEntries()-1];
   qObj.HLines.ReSize (pBitmap->GetYSize()*8);
   qObj.graphicID = graphicID;
   qObj.pHLPool   = this;
   qObj.Size      = XY (pBitmap->GetXSize(), pBitmap->GetYSize());


   if (pHLBasepool1)
   {
      pHLBasepool1Pool = pHLBasepool1->pPool;
      HLBasepool1Size  = pHLBasepool1->PoolSize;
   }
   else
   {
      pHLBasepool1Pool = NULL;
      HLBasepool1Size  = NULL;
   }

   if (pHLBasepool2)
   {
      pHLBasepool2Pool = pHLBasepool2->pPool;
      HLBasepool2Size  = pHLBasepool2->PoolSize;
   }
   else
   {
      pHLBasepool2Pool = NULL;
      HLBasepool2Size  = NULL;
   }

   SLONG MaxDelta = (100-Quality)*768/100;

   SLONG AnzObjHLines = 0;
   BUFFER<UBYTE> Equal (65536);

   BytesReal           += pBitmap->GetXSize() * pBitmap->GetYSize() * 2;
   BytesAdministration += pBitmap->GetYSize();  //Anz Genes für jede Scanline
   BytesCompressed     += pBitmap->GetYSize();  //Anz Genes für jede Scanline

   qObj.HLineEntries.ReSize (pBitmap->GetYSize());

   SB_CBitmapKey Key(*pBitmap);

   //ggf. Palette übernehmen:
   if (PaletteMapper.AnzEntries()==0)
   {
      PaletteMapper.ReSize (256);

      for (SLONG c=0; c<256; c++)
      {
         PaletteMapper[c]=UWORD((Pal->Pal[c].peBlue>>3)+(Pal->Pal[c].peGreen>>2<<5)+(Pal->Pal[c].peRed>>3<<11));
      }
   }

   //Bitmaps runterrechnen:
   if ( ((__int64*)(((UWORD*)Key.Bitmap)+pBitmap->GetYSize()*Key.lPitch/2))[-1]!=*(__int64*)"CONVERTD")
   {
      SLONG  x, y, c, d, dc=0, n;

      for (y=0; y<pBitmap->GetYSize(); y++)
         for (x=0; x<pBitmap->GetXSize(); x++)
         {
            n=x+y*Key.lPitch/2;

            SLONG r,g,b;
            UWORD p=*(((UWORD *)Key.Bitmap)+n);

            if (p!=0)
            {
               b=(p&31)<<3;
               g=((p>>5)&63)<<2;
               r=((p>>11)&31)<<3;

               d=999;

               for (c=1; c<256; c++)
                  if (d>abs(Pal->Pal[c].peBlue-b)+abs(Pal->Pal[c].peGreen-g)+abs(Pal->Pal[c].peRed-r))
                  {
                     d=abs(Pal->Pal[c].peBlue-b)+abs(Pal->Pal[c].peGreen-g)+abs(Pal->Pal[c].peRed-r);
                     dc=c;
                  }

               *(((UBYTE *)Key.Bitmap)+n)=(UBYTE)dc;
            }
            else *(((UBYTE *)Key.Bitmap)+n)=0;
         }

      //Bitmap als bekehrt markieren:
      ((__int64*)(((UWORD*)Key.Bitmap)+pBitmap->GetYSize()*Key.lPitch/2))[-1]=*(__int64*)"CONVERTD";
   }

   //Ähnlichkeitstabelle berechnen:
   {
      SLONG x, y;

      for (y=0; y<256; y++)
         for (x=0; x<=y; x++)
         {
            if (abs(Pal->Pal[x].peBlue-Pal->Pal[y].peBlue)+abs(Pal->Pal[x].peGreen-Pal->Pal[y].peGreen)+abs(Pal->Pal[x].peRed-Pal->Pal[y].peRed)<=MaxDelta)
               Equal[x+(y<<8)]=Equal[y+(x<<8)]=(UBYTE)min (255,abs(Pal->Pal[x].peBlue-Pal->Pal[y].peBlue)+abs(Pal->Pal[x].peGreen-Pal->Pal[y].peGreen)+abs(Pal->Pal[x].peRed-Pal->Pal[y].peRed));
            else
               Equal[x+(y<<8)]=Equal[y+(x<<8)]=255;
         }
   }

   //Datenkompression beginnt hier:
   SLONG x, y;
   SLONG cx;
   SLONG Pass;

   UBYTE *bm=(UBYTE *)Key.Bitmap;

   //Alle Pixel der Bitmap zeilenweise durchgehen:
   for (y=0; y<pBitmap->GetYSize(); y++)
   {
      qObj.HLineEntries[y]=0;
      for (x=0; x<pBitmap->GetXSize(); x++)
      {
         //...und nach nicht.transparenten Pixeln suchen:
         if (bm[x]!=0)
         {
            qObj.HLineEntries[y]++; //Erhöhe, die Zahl der Gen-Strings für diese Zeile

            //Suchen, wie weit die nicht-transparente HLine geht...
            for (cx=0; cx+x<pBitmap->GetXSize() && bm[cx+x]; cx++);

            if (cx<=4)
            {
               //Für HLines mit einer Länge <=2 gilt eine Sonderregelung:
               qObj.HLines[AnzObjHLines].Offset = (UBYTE)x;
               qObj.HLines[AnzObjHLines].Anz    = (UBYTE)cx;

               qObj.HLines[AnzObjHLines].pPixel = (UBYTE*)(*((ULONG*)(bm+x)));

               AnzObjHLines++;

               BytesCompressed     += sizeof (CHLGene);
               BytesAdministration += sizeof (CHLGene);
            }
            else
            {
               UBYTE *BestP=NULL;
               SLONG  BestDist=999;

               //In drei verschiedenen Pools suchen:
               for (Pass=1; Pass<=3; Pass++)
               {
                  UBYTE   *p;
                  CHLPool *pCHLPool;

                  //Die Basepools der Basepools werden bis jetzt übersehen:
                  pCHLPool = NULL;
                  if (Pass==1) pCHLPool = pHLBasepool1;
                  if (Pass==2) pCHLPool = pHLBasepool2;
                  if (Pass==3) pCHLPool = this;

                  if (pCHLPool==NULL && Pass<3) continue;

                  if (pCHLPool || Pass==3)
                  {
                     p = pCHLPool->pPool;

                     //Den Pool nach der HLine durchsuchen:
                     while (p+cx-1<pCHLPool->pPool+pCHLPool->PoolSize)
                     {
                        if (Equal[p[0]+(bm[x]<<8)]<255 && Equal[p[cx-1]+(bm[x+cx-1]<<8)]<255)
                        {
                           SLONG cmp;
                           SLONG Dist=0;

                           for (cmp=cx-1; cmp>=0; cmp--)
                              if (Equal[p[cmp]+(bm[x+cmp]<<8)]>=255)
                                 break;
                              else
                              {
                                 Dist=max(Dist, Equal[p[cmp]+(bm[x+cmp]<<8)]);
                                 if (Dist>=BestDist) break;
                              }

                           //Haben wir eine bessere Alternative gefunden?
                           if (cmp<0 && Dist<BestDist)
                           {
                              BestP    = p;
                              BestDist = Dist;

                              if (Dist==0) break;
                           }
                        }

                        p++;
                     }

                     if (Pass==3 && BestP && BestDist<MaxDelta)
                     {
                        //Erfolg! Eine Kopie wurde gefunden:
                        qObj.HLines[AnzObjHLines].Offset = UBYTE(x);
                        qObj.HLines[AnzObjHLines].Anz    = UBYTE(cx);

                        qObj.HLines[AnzObjHLines].pPixel = BestP;

                        if (qObj.HLines[AnzObjHLines].Anz>4)
                           if (qObj.HLines[AnzObjHLines].pPixel>=pPool && qObj.HLines[AnzObjHLines].pPixel<=pPool+PoolSize)
                           ;
                           else if (pHLBasepool1 && qObj.HLines[AnzObjHLines].pPixel>=pHLBasepool1->pPool && qObj.HLines[AnzObjHLines].pPixel<=pHLBasepool1->pPool+pHLBasepool1->PoolSize)
                           ;
                           else if (pHLBasepool2 && qObj.HLines[AnzObjHLines].pPixel>=pHLBasepool2->pPool && qObj.HLines[AnzObjHLines].pPixel<=pHLBasepool2->pPool+pHLBasepool2->PoolSize)
                           ;
                           else DebugBreak();

                        AnzObjHLines++;

                        BytesCompressed     += sizeof (CHLGene);
                        BytesAdministration += sizeof (CHLGene);
                        LinesRepeated++;
                        break;
                     }

                     //if (p+cx-1<pCHLPool->pPool+pCHLPool->PoolSize) break;

                     if (Pass==3)
                     {
                        //Gen-String wurde nicht gefunden! Also in den Pool einfügen:
                        if (pCHLPool->pPool==NULL)
                        {
                           //Erst einmal den Pool anlegen:
                           pCHLPool->pPool = new UBYTE [1000];

                           pCHLPool->PoolSize    = 0;
                           pCHLPool->PoolMaxSize = 1000;
                        }

                        //Pool ggf. vergrößern?
                        if (PoolSize+cx>PoolMaxSize)
                        {
                           /*{
                              //Für alle HLines eines Objektes...
                              for (SLONG d=AnzObjHLines-1; d>=0; d--)
                              {
                                 //Falls die HLine aus dem aktuellen Pool stammt...
                                 if (qObj.HLines[d].Anz>4)
                                    if (qObj.HLines[d].pPixel>=pPool && qObj.HLines[d].pPixel<=pPool+PoolSize)
                                       ;
                                    else if (pHLBasepool1 && qObj.HLines[d].pPixel>=pHLBasepool1->pPool && qObj.HLines[d].pPixel<=pHLBasepool1->pPool+pHLBasepool1->PoolSize)
                                       ;
                                    else if (pHLBasepool2 && qObj.HLines[d].pPixel>=pHLBasepool2->pPool && qObj.HLines[d].pPixel<=pHLBasepool2->pPool+pHLBasepool2->PoolSize)
                                       ;
                                    else DebugBreak();
                              }
                           }*/

                           UBYTE *pNew = new UBYTE [PoolMaxSize+1000];
                           ReBaseObjects (pCHLPool->pPool, pNew);

                           memcpy (pNew, pCHLPool->pPool, PoolSize);

                           delete [] pCHLPool->pPool;

                           pCHLPool->pPool = pNew;
                           PoolMaxSize+=1000;

                           /*{
                              //Für alle HLines eines Objektes...
                              for (SLONG d=AnzObjHLines-1; d>=0; d--)
                              {
                                 //Falls die HLine aus dem aktuellen Pool stammt...
                                 if (qObj.HLines[d].Anz>4)
                                    if (qObj.HLines[d].pPixel>=pPool && qObj.HLines[d].pPixel<=pPool+PoolSize)
                                       ;
                                    else if (pHLBasepool1 && qObj.HLines[d].pPixel>=pHLBasepool1->pPool && qObj.HLines[d].pPixel<=pHLBasepool1->pPool+pHLBasepool1->PoolSize)
                                       ;
                                    else if (pHLBasepool2 && qObj.HLines[d].pPixel>=pHLBasepool2->pPool && qObj.HLines[d].pPixel<=pHLBasepool2->pPool+pHLBasepool2->PoolSize)
                                       ;
                                    else DebugBreak();
                              }
                           }*/
                        }

                        //Im Pool referenzieren:
                        qObj.HLines[AnzObjHLines].Offset = UBYTE(x);
                        qObj.HLines[AnzObjHLines].Anz    = UBYTE(cx);
                        qObj.HLines[AnzObjHLines].pPixel = pCHLPool->pPool+PoolSize;
                        AnzObjHLines++;
                        LinesInPool++;

                        //Und jetzt endlich in den Pool eintragen:
                        memcpy (pCHLPool->pPool+PoolSize, bm+x, cx);
                        PoolSize+=cx;

                        BytesCompressed     += sizeof (CHLGene) + cx;
                        BytesAdministration += sizeof (CHLGene);
                     }
                  }
               }
            }

            x+=cx-1;
         }
      }

      bm+=Key.lPitch/2;
   }

   //HLines zurechtstutzen, wir wollen nichts verschwenden:
   if (AnzObjHLines>=qObj.HLines.AnzEntries()) DebugBreak();

   qObj.HLines.ReSize (AnzObjHLines);
}
//#pragma optimize("agptwy", off)

//--------------------------------------------------------------------------------------------
//Gibt allen Objekten und Unterobjekten eine Basis:
//--------------------------------------------------------------------------------------------
void CHLPool::DoBaseObjects (void)
{
   SLONG c, d;

   if (pHLBasepool1)
   {
      pHLBasepool1Pool = pHLBasepool1->pPool;
      HLBasepool1Size  = pHLBasepool1->PoolSize;
   }
   else
   {
      pHLBasepool1Pool = NULL;
      HLBasepool1Size  = 0;
   }

   if (pHLBasepool2)
   {
      pHLBasepool2Pool = pHLBasepool2->pPool;
      HLBasepool2Size  = pHLBasepool2->PoolSize;
   }
   else
   {
      pHLBasepool2Pool = NULL;
      HLBasepool2Size  = 0;
   }

   //Für alle Objekte...
   for (c=HLObjects.AnzEntries()-1; c>=0; c--)
   {
      CHLObj &qObj = HLObjects [c];

      //Für alle HLines eines Objektes...
      for (d=qObj.HLines.AnzEntries()-1; d>=0; d--)
      {
         //Falls die HLine aus dem aktuellen Pool stammt...
         if (qObj.HLines[d].Anz>4)
            if (qObj.HLines[d].pPixel>=(UBYTE*)0x00000000 && qObj.HLines[d].pPixel<=(UBYTE*)0x00000000+PoolSize)
               qObj.HLines[d].pPixel = (UBYTE*) (((UBYTE*)qObj.HLines[d].pPixel)-(ULONG)0x00000000+(ULONG)pPool);
            else if (pHLBasepool1 && qObj.HLines[d].pPixel>=(UBYTE*)0x10000000 && qObj.HLines[d].pPixel<=(UBYTE*)0x10000000+pHLBasepool1->PoolSize)
               qObj.HLines[d].pPixel = (UBYTE*) (((UBYTE*)qObj.HLines[d].pPixel)-(ULONG)0x10000000+(ULONG)pHLBasepool1->pPool);

            else if (pHLBasepool2 && qObj.HLines[d].pPixel>=(UBYTE*)0x20000000 && qObj.HLines[d].pPixel<=(UBYTE*)0x20000000+pHLBasepool2->PoolSize)
               qObj.HLines[d].pPixel = (UBYTE*) (((UBYTE*)qObj.HLines[d].pPixel)-(ULONG)0x20000000+(ULONG)pHLBasepool2->pPool);

            else DebugBreak();
      }
   }
}

//--------------------------------------------------------------------------------------------
//Nimm allen Objekten und Unterobjekten die Basis weg:
//--------------------------------------------------------------------------------------------
void CHLPool::UnBaseObjects (void)
{
   SLONG c, d;

   //Für alle Objekte...
   for (c=HLObjects.AnzEntries()-1; c>=0; c--)
   {
      CHLObj &qObj = HLObjects [c];

      //Für alle HLines eines Objektes...
      for (d=qObj.HLines.AnzEntries()-1; d>=0; d--)
      {
         //Falls die HLine aus dem aktuellen Pool stammt...
         if (qObj.HLines[d].Anz>4)
            if (qObj.HLines[d].pPixel>=pPool && qObj.HLines[d].pPixel<=pPool+PoolSize)
               qObj.HLines[d].pPixel = (UBYTE*) (((UBYTE*)qObj.HLines[d].pPixel)-(ULONG)pPool+(ULONG)0x00000000);

            else if (pHLBasepool1 && qObj.HLines[d].pPixel>=pHLBasepool1Pool && qObj.HLines[d].pPixel<=pHLBasepool1Pool+HLBasepool1Size)
               qObj.HLines[d].pPixel = (UBYTE*) (((UBYTE*)qObj.HLines[d].pPixel)-(ULONG)pHLBasepool1Pool+(ULONG)0x10000000);

            else if (pHLBasepool2 && qObj.HLines[d].pPixel>=pHLBasepool2Pool && qObj.HLines[d].pPixel<=pHLBasepool2Pool+HLBasepool2Size)
               qObj.HLines[d].pPixel = (UBYTE*) (((UBYTE*)qObj.HLines[d].pPixel)-(ULONG)pHLBasepool2Pool+(ULONG)0x20000000);

            else DebugBreak();
      }
   }

   pHLBasepool1Pool = NULL;   HLBasepool1Size=0;
   pHLBasepool2Pool = NULL;   HLBasepool2Size=0;
}

//--------------------------------------------------------------------------------------------
//Gibt allen Objekten eine neue Basis:
//--------------------------------------------------------------------------------------------
void CHLPool::ReBaseObjects (UBYTE *pOldPool, UBYTE *pNewPool)
{
   SLONG c, d;

   //Für alle Objekte...
   for (c=HLObjects.AnzEntries()-1; c>=0; c--)
   {
      CHLObj &qObj = HLObjects [c];

      //Für alle HLines eines Objektes...
      for (d=qObj.HLines.AnzEntries()-1; d>=0; d--)
      {
         //Falls die HLine aus dem aktuellen Pool stammt...
         if (qObj.HLines[d].Anz>4)
            if (qObj.HLines[d].pPixel>=pOldPool && qObj.HLines[d].pPixel<=pOldPool+PoolSize)
               qObj.HLines[d].pPixel = (UBYTE*) (((UBYTE*)qObj.HLines[d].pPixel)-(ULONG)pOldPool+(ULONG)pNewPool);
      }
   }
}

//--------------------------------------------------------------------------------------------
//Sucht ein HLObj anhand seiner id heraus:
//--------------------------------------------------------------------------------------------
CHLObj *CHLPool::GetHLObj (__int64 graphicID)
{
   SLONG c;

   for (c=HLObjects.AnzEntries()-1; c>=0; c--)
      if (HLObjects[c].graphicID==graphicID) return (&HLObjects[c]);

   return (NULL);
}

//--------------------------------------------------------------------------------------------
//Sucht ein HLObj anhand seiner id heraus:
//--------------------------------------------------------------------------------------------
CHLObj *CHLPool::GetHLObj (const CString &String)
{
   SLONG   c;
   __int64 graphicId;

   graphicId=0;
   for (c=0; c<(SLONG)strlen(String); c++)
      graphicId+=__int64(String[(int)c])<<(8*c);

   for (c=HLObjects.AnzEntries()-1; c>=0; c--)
      if (HLObjects[c].graphicID==graphicId) return (&HLObjects[c]);

   return (NULL);
}

//--------------------------------------------------------------------------------------------
//CHLBm::
//--------------------------------------------------------------------------------------------
//Eine HL-Bitmap ist eine Referenz auf ein HL-Objekt; Es kann mehrere HLBMs für ein HLOBJ geben
//--------------------------------------------------------------------------------------------
CHLBm::CHLBm ()
{
}

//--------------------------------------------------------------------------------------------
//Destruktor:
//--------------------------------------------------------------------------------------------
CHLBm::~CHLBm ()
{
   Destroy ();
}

//--------------------------------------------------------------------------------------------
//Pseudo-Destruktor:
//--------------------------------------------------------------------------------------------
void CHLBm::Destroy (void)
{
   pObj = NULL;
}

//--------------------------------------------------------------------------------------------
//Initialisiert ein Objekt:
//--------------------------------------------------------------------------------------------
void CHLBm::ReSize (CHLPool *pHLPool, __int64 graphicID)
{
   pObj = pHLPool->GetHLObj (graphicID);
}

//--------------------------------------------------------------------------------------------
//CHLBms::
//--------------------------------------------------------------------------------------------
//Eine Sequenz von HL-Bitmaps initialisieren:
//--------------------------------------------------------------------------------------------
void CHLBms::ReSize (CHLPool *pHLPool, __int64 graphicID, ...)
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

   ReSize (pHLPool, graphicIds);
}

//--------------------------------------------------------------------------------------------
//Eine Sequenz von HL-Bitmaps initialisieren:
//--------------------------------------------------------------------------------------------
void CHLBms::ReSize (CHLPool *pHLPool, const BUFFER<__int64> &graphicIds)
{
   SLONG c;

   Bitmaps.ReSize (graphicIds.AnzEntries());

   for (c=0; c<graphicIds.AnzEntries(); c++)
      Bitmaps[c].ReSize (pHLPool, graphicIds[c]);
}

//--------------------------------------------------------------------------------------------
//Eine Sequenz von HL-Bitmaps initialisieren:
//--------------------------------------------------------------------------------------------
void CHLBms::ReSize (CHLPool *pHLPool, const CString &graphicstr)
{
   SLONG           Anz;
   char           *Texts[100];
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
      for (SLONG d=0; d<(SLONG)strlen(Texts[c]); d++)
         graphicIds[c]+=__int64((Texts[c])[d])<<(8*d);
   }

   ReSize (pHLPool, graphicIds);
}

//--------------------------------------------------------------------------------------------
//Eine Sequenz von HL-Bitmaps initialisieren:
//--------------------------------------------------------------------------------------------
void CHLBms::ReSize (CHLPool *pHLPool, const CString &graphicstr, SLONG Anzahl)
{
   BUFFER<__int64> graphicIds;
   BUFFER<char>    Str (graphicstr.GetLength()+1);

   strcpy (Str, graphicstr);

   graphicIds.ReSize (Anzahl);

   for (SLONG c=0; c<Anzahl; c++)
   {
      graphicIds[c]=0;
      for (SLONG d=0; d<(SLONG)strlen(Str); d++)
         graphicIds[c]+=__int64(Str[d])<<(8*d);

      Str[SLONG(strlen(Str)-1)]++;
      if (Str[SLONG(strlen(Str)-1)]>'9')
      {
         Str[SLONG(strlen(Str)-1)]='0';
         Str[SLONG(strlen(Str)-2)]++;
      }
   }

   ReSize (pHLPool, graphicIds);
}

//--------------------------------------------------------------------------------------------
//Bringt alle HLine-Pool auf den neusten Stand:
//--------------------------------------------------------------------------------------------
void UpdateHLinePool (void)
{
   SLONG c=0, d, e;
   PALETTE AnimPal;
   PALETTE SkelPal;

   BOOL DontSaveSkeletons=FALSE;

   SkelettPool.ReSize (bprintf ("Skelett.pol", c), NULL, NULL);
   SkelPal.RefreshPalFromLbm((char*)(LPCTSTR)FullFilename ("Skel_pal.lbm", "CLAN\\LBM\\%s"));

   for (c=Clans.AnzEntries()-1; c>=0; c--)
      if (Clans.IsInAlbum (c) && Clans[c].UpdateNow)
      {
         hprintf ("Compressing %li", c);

         AnimPal.RefreshPalFromLbm((char*)(LPCTSTR)FullFilename (Clans[c].PalFilename, "CLAN\\LBM\\%s"));

         if (c<SLONG(Clans.AnzEntries()-1) && Clans.IsInAlbum(c+1) && Clans[c].PalFilename==Clans[SLONG(c+1)].PalFilename && Clans[c].Type!=30)
         {
            Clans[c].ClanPool.ReSize (bprintf ("clan%li.pol", c), &Clans[SLONG(c+1)].ClanPool, NULL);
            Clans[c].ClanGimmick.ReSize (bprintf ("clang%li.pol", c), &Clans[SLONG(c+1)].ClanPool, &Clans[c].ClanPool);
            Clans[c].ClanWarteGimmick.ReSize (bprintf ("clanw%li.pol", c), &Clans[SLONG(c+1)].ClanPool, &Clans[c].ClanPool);
         }
         else
         {
            Clans[c].ClanPool.ReSize (bprintf ("clan%li.pol", c), NULL, NULL);
            Clans[c].ClanGimmick.ReSize (bprintf ("clang%li.pol", c), &Clans[c].ClanPool, NULL);
            Clans[c].ClanWarteGimmick.ReSize (bprintf ("clanw%li.pol", c), &Clans[c].ClanPool, NULL);
         }

         for (d=0; d<14; d++)
            if (d!=4 && d!=9) //Kein Gimmick hier:
               for (e=0; e<Clans[c].PhasenIds[d].AnzEntries(); e++)
               {
                  if (!Clans[c].ClanPool.GetHLObj ((Clans[c].PhasenIds[d])[e]) && (Clans[c].Phasen[d])[e].Size.x)
                     Clans[c].ClanPool.AddBitmap ((Clans[c].PhasenIds[d])[e], (Clans[c].Phasen[d])[e].pBitmap, &AnimPal, 95, 100);
               }

         for (d=4; d<=4; d++) //Gimmick:
            for (e=0; e<Clans[c].PhasenIds[d].AnzEntries(); e++)
            {
               if (!Clans[c].ClanGimmick.GetHLObj ((Clans[c].PhasenIds[d])[e]) && (Clans[c].Phasen[d])[e].Size.x)
                  Clans[c].ClanGimmick.AddBitmap ((Clans[c].PhasenIds[d])[e], (Clans[c].Phasen[d])[e].pBitmap, &AnimPal, 95, 100);
            }

         for (d=9; d<=9; d++) //Wartegimmick:
            for (e=0; e<Clans[c].PhasenIds[d].AnzEntries(); e++)
            {
               if (!Clans[c].ClanWarteGimmick.GetHLObj ((Clans[c].PhasenIds[d])[e]) && (Clans[c].Phasen[d])[e].Size.x)
                  Clans[c].ClanWarteGimmick.AddBitmap ((Clans[c].PhasenIds[d])[e], (Clans[c].Phasen[d])[e].pBitmap, &AnimPal, 95, 100);
            }

         for (d=0; d<14; d++)
            for (e=0; e<Clans[c].SkelettIds[d].AnzEntries(); e++)
            {
               if (!SkelettPool.GetHLObj ((Clans[c].SkelettIds[d])[e]) && (Clans[c].Skelett[d])[e].Size.x)
                  SkelettPool.AddBitmap ((Clans[c].SkelettIds[d])[e], (Clans[c].Skelett[d])[e].pBitmap, &SkelPal, 98, 100);
            }

         Clans[c].ClanWarteGimmick.Save();
         Clans[c].ClanGimmick.Save();
         Clans[c].ClanPool.Save();
      }
      else if (Clans.IsInAlbum (c)) DontSaveSkeletons=TRUE;

   if (!DontSaveSkeletons) SkelettPool.Save();
}