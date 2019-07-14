//============================================================================================
// Blenden.h - Überblenden von Bildern
//============================================================================================
#ifndef _CBLENDEN_CLASS
#define _CBLENDEN_CLASS

#define dword unsigned long

//--------------------------------------------------------------------------------------------
//Abstrakte Basisklasse für Blenden; Mit virtual Pointern praktisch für Mehrfachverwendung...
//--------------------------------------------------------------------------------------------
class CBlenden
{
   protected:
      LPDIRECTDRAWSURFACE _pSurface1;
      LPDIRECTDRAWSURFACE _pSurface2;
      LPDIRECTDRAWSURFACE _pTmpSurface;
      long                _SizeX, _SizeY;
		dword					  _dwFlags;

   public:
      CBlenden ();
      CBlenden (LPDIRECTDRAWSURFACE pSurface1, LPDIRECTDRAWSURFACE pSurface2, LPDIRECTDRAWSURFACE pTmpSurface);
      virtual ~CBlenden ();
      void virtual UpdateTmpSurface (double Prozent) = NULL;
};

//--------------------------------------------------------------------------------------------
//Überblenden durch starkes zoomen ins alte Bild
//--------------------------------------------------------------------------------------------
class CCoolBlend : public CBlenden
{
   protected:
      long     _ClickX, _ClickY;

   public:
      CCoolBlend () {}
      CCoolBlend (LPDIRECTDRAWSURFACE pSurface1, LPDIRECTDRAWSURFACE pSurface2, LPDIRECTDRAWSURFACE pTmpSurface, long ClickX, long ClickY, bool fTransparent);
      void UpdateTmpSurface (double Prozent);
};

//--------------------------------------------------------------------------------------------
//Überblenden durch fallen (auf alte Bild) und abprallen (vom neuen Bild)
//--------------------------------------------------------------------------------------------
class CFallBlend : public CBlenden
{
   protected:
      long     _ClickX, _ClickY;

   public:
      CFallBlend () {}
      CFallBlend (LPDIRECTDRAWSURFACE pSurface1, LPDIRECTDRAWSURFACE pSurface2, LPDIRECTDRAWSURFACE pTmpSurface, long ClickX, long ClickY, bool fTransparent);
      void UpdateTmpSurface (double Prozent);
};

//--------------------------------------------------------------------------------------------
//Überblenden durch flackernde Random-Blöcke
//--------------------------------------------------------------------------------------------
class CRandomBlend : public CBlenden
{
   protected:
      long     _RandomX, _RandomY;

   public:
      CRandomBlend () {}
      CRandomBlend (LPDIRECTDRAWSURFACE pSurface1, LPDIRECTDRAWSURFACE pSurface2, LPDIRECTDRAWSURFACE pTmpSurface, long RandomX, long RandomY, bool fTransparent);
      void UpdateTmpSurface (double Prozent);
};

//--------------------------------------------------------------------------------------------
//Überblenden durch Doom-Schleimeffekt
//--------------------------------------------------------------------------------------------
class CSlimeBlend : public CBlenden
{
   public:
      CSlimeBlend () {}
      CSlimeBlend (LPDIRECTDRAWSURFACE pSurface1, LPDIRECTDRAWSURFACE pSurface2, LPDIRECTDRAWSURFACE pTmpSurface, bool fTransparent);
      void UpdateTmpSurface (double Prozent);
};

//--------------------------------------------------------------------------------------------
//Überblenden durch horizontale Balken, die von beiden Seiten kommen
//--------------------------------------------------------------------------------------------
class CHLineBlend : public CBlenden
{
   private:
      long _AnzHLines;

   public:
      CHLineBlend () {}
      CHLineBlend (LPDIRECTDRAWSURFACE pSurface1, LPDIRECTDRAWSURFACE pSurface2, LPDIRECTDRAWSURFACE pTmpSurface, long AnzHLines, bool fTransparent);
      void UpdateTmpSurface (double Prozent);
};

//--------------------------------------------------------------------------------------------
//Überblenden durch sich schließende Rolläden
//--------------------------------------------------------------------------------------------
class CShadesBlend : public CBlenden
{
   private:
      long _AnzHLines;
      bool _Direction;

   public:
      CShadesBlend () {}
      CShadesBlend (LPDIRECTDRAWSURFACE pSurface1, LPDIRECTDRAWSURFACE pSurface2, LPDIRECTDRAWSURFACE pTmpSurface, long AnzHLines, bool Direction, bool fTransparent);
      void UpdateTmpSurface (double Prozent);
};

//--------------------------------------------------------------------------------------------
//Überblenden durch Terminator II FX (fallendes Bild)
//--------------------------------------------------------------------------------------------
class CTerminatorBlend : public CBlenden
{
   public:
      CTerminatorBlend () {}
      CTerminatorBlend (LPDIRECTDRAWSURFACE pSurface1, LPDIRECTDRAWSURFACE pSurface2, LPDIRECTDRAWSURFACE pTmpSurface, bool fTransparent);
      void UpdateTmpSurface (double Prozent);
};

#endif