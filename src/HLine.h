//============================================================================================
// HLine.h : Die HLine-Engine
//============================================================================================

#ifndef hline_h
   #define hline_h

   class CHLPool;

#pragma pack (push, pragma_gene)
#pragma pack (2)
   //--------------------------------------------------------------------------------------------
   //Eine Referenz in den HLine-Pool:
   //--------------------------------------------------------------------------------------------
   class CHLGene
   {
      private:
         UBYTE  Offset;                     //X-Offset für die Bitmap
         UBYTE  Anz;                        //Anzahl der Pixel im String
         UBYTE *pPixel;                     //Pointer in den Pool oder wenn Anz<=2, direkt die Pixel

      private:
         CHLGene () { Offset = 0; Anz = 0; pPixel = NULL; }
         ~CHLGene () {}

      friend class BUFFER<CHLGene>;
      friend class CHLObj;
      friend class CHLPool;

      friend TEAKFILE &operator << (TEAKFILE &File, const CHLGene &gene)  {  File << gene.Offset << gene.Anz; File.Write((UBYTE*)&gene.pPixel, 4); return (File); }
      friend TEAKFILE &operator >> (TEAKFILE &File, CHLGene &gene)  { File >> gene.Offset >> gene.Anz; File.Read((UBYTE*)&gene.pPixel, 4); return (File); }
   };
#pragma pack (pop, pragma_gene)

   //--------------------------------------------------------------------------------------------
   //Ein Object, was aus HLines besteht: (es gehört immer dem Pool)
   //--------------------------------------------------------------------------------------------
   class CHLObj
   {
      private:
         __int64          graphicID;          //Identifiziert die Bitmap;
         XY               Size;
         CHLPool        *pHLPool;             //Zeiger auf Parent (H-Line Pool)
         FBUFFER<CHLGene> HLines;             //Elemente aus dem Pool
         FBUFFER<UBYTE>   HLineEntries;       //Array, daß angibtm wieviele Einträge pro Zeile da sind

      private: //Konstruktion/Destruktion:
         CHLObj ();
         ~CHLObj ();
         void  Destroy (void);

      public: //Blitting & Bitmap Services:
         const XY &GetSize (void) { return (Size); }
         void  BlitAt (SB_CBitmapCore *pBitmap, XY Target);
         void  BlitAt (SB_CBitmapCore *pBitmap, SLONG tx, SLONG ty) { BlitAt (pBitmap, XY (tx, ty)); }
         void  BlitLargeAt (SB_CBitmapCore *pBitmap, XY Target);
         void  BlitLargeAt (SB_CBitmapCore *pBitmap, SLONG tx, SLONG ty) { BlitLargeAt (pBitmap, XY (tx, ty)); }

      friend TEAKFILE &operator << (TEAKFILE &File, const CHLObj &obj)  { File.Write((UBYTE*)&obj.graphicID, 8); File << obj.Size << obj.HLines << obj.HLineEntries; return (File); }
      friend TEAKFILE &operator >> (TEAKFILE &File, CHLObj &obj)  { File.Read((UBYTE*)&obj.graphicID, 8); File >> obj.Size >> obj.HLines >> obj.HLineEntries; return (File); }

      friend class BUFFER<CHLObj>;
      friend class CHLPool;
      friend class CHLBm;
   };

   //--------------------------------------------------------------------------------------------
   //Ein Pool mit HLines:
   //--------------------------------------------------------------------------------------------
   class CHLPool
   {
      private: //Pool-informationen
         CString   Filename;
         UBYTE   *pPool;                    //Eine Folge von 2-byte Pixeln (64k Farben)
         SLONG     PoolSize;                //Aktuelle Größe in Pixeln (1 Pixel = 2 Bytes)
         SLONG     PoolMaxSize;             //Maximale Größe in Pixeln
         CHLPool *pHLBasepool1;             //Eltern-Pool #1
         CHLPool *pHLBasepool2;             //Eltern-Pool #2

         UBYTE   *pHLBasepool1Pool;         //Eltern-Pool #1 (Cache; damit UnBase auch nach unload des Elternpools möglich ist)
         UBYTE   *pHLBasepool2Pool;         //Eltern-Pool #2 (Cache; damit UnBase auch nach unload des Elternpools möglich ist)
         SLONG     HLBasepool1Size;         //Größe Eltern-Pool #1 (Cache; damit UnBase auch nach unload des Elternpools möglich ist)
         SLONG     HLBasepool2Size;         //Größe Eltern-Pool #2 (Cache; damit UnBase auch nach unload des Elternpools möglich ist)

         FBUFFER<UWORD> PaletteMapper;      //256 Paletteneinträge zum remappen

         SLONG     Loaded;                  //0=Nein, 1=Preloaded, 2=Loaded

      public: //Pool-Statistiken:
         SLONG     BytesReal;               //So groß waren die Bitmaps einmal
         SLONG     BytesCompressed;         //So groß sind sie jetzt
         SLONG     BytesAdministration;     //Und soviel ging für die Verwaltung bei drauf
         SLONG     LinesInPool;             //Soviele Lines sind im Pool
         SLONG     LinesRepeated;           //Soviele Lines wurde nicht in den Pool getan, sondern referenziert

      private: //Child-Informationen
         FBUFFER<CHLObj> HLObjects;         //Bitmaps, die Pool-Informationen verwenden

      public: //Konstruktion/Destruktion:
         CHLPool ();
         ~CHLPool ();
         void Destroy (void);
         BOOL PreLoad (void);
         BOOL Load (void);
         BOOL Save (void);
         void Unload (void);
         void ReSize (const CString &Filename, CHLPool *pHLBasepool1=NULL, CHLPool *pHLBasepool2=NULL);
         void AddBitmap (__int64 graphicID, SB_CBitmapCore *pBitmap, PALETTE *Pal, SLONG Quality, SLONG Speed);
         void ReBaseObjects (UBYTE *pOldPool, UBYTE *pNewPool);
         void DoBaseObjects (void);
         void UnBaseObjects (void);

      public: //Verwaltung:
         CHLObj *GetHLObj (__int64 graphicID);
         CHLObj *GetHLObj (const CString &String);

      friend class CHLObj;
   };

   //--------------------------------------------------------------------------------------------
   //Eine HL-Bitmap ist eine Referenz auf ein HL-Objekt; Es kann mehrere HLBMs für ein HLOBJ geben
   //--------------------------------------------------------------------------------------------
   class CHLBm
   {
      private:
         CHLObj  *pObj;

      public: //Konstruktion/Destruktion:
         CHLBm ();
         ~CHLBm ();
         void  Destroy (void);
         void  ReSize (CHLPool *pHLPool, __int64 graphicID);

      public: //Blitting & Bitmap Services:
         const XY &GetSize (void) { return (pObj->GetSize()); }
         void  BlitAt (SB_CBitmapCore *pBitmap, XY Target) { if (pObj) pObj->BlitAt (pBitmap, Target); }
         void  BlitAt (SB_CBitmapCore *pBitmap, SLONG tx, SLONG ty) { if (pObj) pObj->BlitAt (pBitmap, tx, ty); }
   };

   //--------------------------------------------------------------------------------------------
   //Eine Sequenz von HL-Bitmaps
   //--------------------------------------------------------------------------------------------
   class CHLBms
   {
      private:
         BUFFER<CHLBm>     Bitmaps;

      public:
         void  Destroy() { Bitmaps.ReSize(0); }
         void  ReSize (CHLPool *pHLPool, __int64 graphicID, ...);
         void  ReSize (CHLPool *pHLPool, const BUFFER<__int64> &graphicsIds);
         void  ReSize (CHLPool *pHLPool, const CString &graphicIDs);
         void  ReSize (CHLPool *pHLPool, const CString &graphicIDs, SLONG Anzahl);
         SLONG AnzEntries() const { return (Bitmaps.AnzEntries()); }

         CHLBm& operator [](const SLONG Index) const { return Bitmaps[Index]; }
         CHLBm& operator [](const int Index) const { return Bitmaps[Index]; }
   };
#endif hline_h