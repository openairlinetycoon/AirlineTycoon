
class CBlenden
{
   protected:
      LPDIRECTDRAWSURFACE _pSurface1;
      LPDIRECTDRAWSURFACE _pSurface2;
      LPDIRECTDRAWSURFACE _pTmpSurface;
      long                _SizeX, _SizeY;

   public:
      CBlenden ();
      CBlenden (LPDIRECTDRAWSURFACE pSurface1, LPDIRECTDRAWSURFACE pSurface2, LPDIRECTDRAWSURFACE pTmpSurface);
      virtual ~CBlenden ();
      void virtual UpdateTmpSurface (double Prozent) = NULL;
};

class CCoolBlend : public CBlenden
{
   protected:
      long     _ClickX, _ClickY;

   public:
      CCoolBlend () {}
      CCoolBlend (LPDIRECTDRAWSURFACE pSurface1, LPDIRECTDRAWSURFACE pSurface2, LPDIRECTDRAWSURFACE pTmpSurface, long ClickX, long ClickY);
      void UpdateTmpSurface (double Prozent);
};

class CFallBlend : public CBlenden
{
   protected:
      long     _ClickX, _ClickY;

   public:
      CFallBlend () {}
      CFallBlend (LPDIRECTDRAWSURFACE pSurface1, LPDIRECTDRAWSURFACE pSurface2, LPDIRECTDRAWSURFACE pTmpSurface, long ClickX, long ClickY);
      void UpdateTmpSurface (double Prozent);
};

class CRandomBlend : public CBlenden
{
   protected:
      long     _RandomX, _RandomY;

   public:
      CRandomBlend () {}
      CRandomBlend (LPDIRECTDRAWSURFACE pSurface1, LPDIRECTDRAWSURFACE pSurface2, LPDIRECTDRAWSURFACE pTmpSurface, long RandomX, long RandomY);
      void UpdateTmpSurface (double Prozent);
};

class CSlimeBlend : public CBlenden
{
   public:
      CSlimeBlend () {}
      CSlimeBlend (LPDIRECTDRAWSURFACE pSurface1, LPDIRECTDRAWSURFACE pSurface2, LPDIRECTDRAWSURFACE pTmpSurface);
      void UpdateTmpSurface (double Prozent);
};