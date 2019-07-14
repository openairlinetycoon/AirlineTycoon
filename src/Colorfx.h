//============================================================================================
//CColorFx - Klasse für Helligkeits- und Farbeffekte mit 16Bit Bitmaps
//============================================================================================
//Anleitung: "i:\projekt\sbl\doku\CColorFx.txt"
//============================================================================================

class SB_CColorFXTypeHelper {};
typedef SB_CColorFXTypeHelper *SB_CColorFXType;

//Effekte:
#define SB_COLORFX_FADE    ((SB_CColorFXType)0x0001)
#define SB_COLORFX_GREY    ((SB_CColorFXType)0x0002)

typedef BUFFER<UWORD> BUFFER_UWORD;

class SB_CColorFX
{
   private:
      SLONG           AnzSteps;
      BUFFER<UWORD>   BlendTables;

   public:
      SB_CColorFX ();
      SB_CColorFX (SB_CColorFXType FXType, SLONG Steps, SB_CBitmapCore *Bitmap);
      void ReInit (SB_CColorFXType FXType, SLONG Steps, SB_CBitmapCore *Bitmap);
      void Apply (SLONG Step, SB_CBitmapCore *Bitmap);
      void Apply (SLONG Step, SB_CBitmapCore *SrcBitmap, SB_CBitmapCore *TgtBitmap);
      void ApplyOn2 (SLONG Step, SB_CBitmapCore *Bitmap, SLONG Step2, SB_CBitmapCore *Bitmap2);
      void ApplyOn2 (SLONG Step, SB_CBitmapCore *SrcBitmap, SLONG Step2, SB_CBitmapCore *SrcBitmap2, SB_CBitmapCore *TgtBitmap);
      void ApplyRoughOn2 (SLONG Step, SB_CBitmapCore *SrcBitmap, SLONG Step2, SB_CBitmapCore *SrcBitmap2, SB_CBitmapCore *TgtBitmap);
      void BlitWhiteTrans (BOOL DoMessagePump, SB_CBitmapCore *SrcBitmap, SB_CBitmapCore *TgtBitmap, const XY &TargetPos, const CRect *SrcRect=NULL, SLONG Grade=-1);
      void BlitOutline (SB_CBitmapCore *SrcBitmap, SB_CBitmapCore *TgtBitmap, const XY &TargetPos, ULONG LineColor);
      void BlitTrans (SB_CBitmapCore *SrcBitmap, SB_CBitmapCore *TgtBitmap, const XY &TargetPos, const CRect *SrcRect=NULL, SLONG Grade=-1);
      void BlitAlpha (SB_CBitmapCore *SrcBitmap, SB_CBitmapCore *TgtBitmap, const XY &TargetPos);
      void BlitGlow (SB_CBitmapCore *SrcBitmap, SB_CBitmapCore *TgtBitmap, const XY &TargetPos);
      void HighlightText (SB_CBitmapCore *Bitmap, const CRect &Rect, UWORD FontColor, ULONG HighlightColor);
};
