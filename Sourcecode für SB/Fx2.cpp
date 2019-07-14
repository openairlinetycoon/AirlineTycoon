#define VC_EXTRALEAN		   // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>        // MFC core and standard components
#include <afxext.h>        // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
   #include <afxcmn.h>	   // MFC support for Windows 95 Common Controls
#endif
#include <afxtempl.h>      // MFC extensions

#include <mmsystem.h>      //Für timeGetTime()
#include <ddraw.h>         //MS Direct Draw 
#include <math.h>

#define FXF 0.33

typedef unsigned long ULONG;
typedef   signed long SLONG;

typedef unsigned short UWORD;
typedef   signed short SWORD;

typedef unsigned char UBYTE;
typedef   signed char SBYTE;

template <class T, SLONG TT> class TBOWVECT
{
   private:
      FIXPOINT<T,TT> x,y,z;

   public:
      void Init (T sT, T syn, T bow, SLONG master)
      {
         x = sT;
         z = FIXPOINT<T,TT>(2*((master-1)*(sT-syn)/(master/2-1)-sT+bow)) / SLONG((master-1)*(master/2));
         y = FIXPOINT<T,TT>((bow-sT-((FIXPOINT<T,TT>)(z*((SLONG)(master*master-3*master+2))))/SLONG(2)))/SLONG(master-1);
      }

      TBOWVECT<T,TT> *operator ++ (int) { x+=y; y+=z; return (this); }
      operator T() { return  (x); }
};

template <class T, SLONG Precision> class FIXPOINT
{
	private:
		T Value;

	public:
      FIXPOINT() {}
		FIXPOINT(T Before) {Value=Before<<Precision;}
      FIXPOINT operator * (SLONG p) const { FIXPOINT rc; rc.Value=Value*p; return (rc); }
      FIXPOINT operator / (SLONG p) const { FIXPOINT rc; rc.Value=Value/p; return (rc); }
      FIXPOINT operator * (const FIXPOINT &p) const { FIXPOINT rc; rc.Value=((Value>>(Precision>>1))*(p.Value>>(Precision>>1))); return (rc); }
      FIXPOINT operator / (const FIXPOINT &p) const { FIXPOINT rc; rc.Value=((Value/p.Value)>>Precision); return (rc); }
      FIXPOINT operator += (const FIXPOINT &p) { Value+=p.Value; return (*this); }
      FIXPOINT operator -= (const FIXPOINT &p) { Value-=p.Value; return (*this); }
      operator T() const { return (Value>>Precision); }
};

//--------------------------------------------------------------------------------------------
//Blittet eine Textur auf eine Kugel; Nur ein Drehfaktor ist dafür erlaubt:
//Die Breite der Quellbitmap muß einer 2erpotenz (16, 32, 64, ...) sein. Sonst explodiert
//alles bekannte Leben im Universum, die Welt geht unter und die PDS gewinnt die nächste Wahl!
//--------------------------------------------------------------------------------------------
void Fx2SphereBlit (LPDIRECTDRAWSURFACE lpDDTargetSurface,
                    LPDIRECTDRAWSURFACE lpDDSourceSurface,
                    SLONG               midx,
                    SLONG               midy,
                    SLONG               r,
                    UWORD               Alpha)
{
   DDSURFACEDESC ddsdSrc, ddsdTgt;
   const UBYTE *s, *stdsource; //Source Bitmap Pointer
   UBYTE       *t, *stdtarget; //TargetBitmap Pointer
   SLONG cx, cy;               //Zähler für die Schleifen
   SLONG xs;                   //x-Größe der aktuellen Zeile
   ULONG Mask;                 //Für Wrap-Around
   SLONG SourceBase;           //Drehung
   TBOWVECT<SLONG,16> si;
   TBOWVECT<SLONG,16> vi;

   if (r<1) return;

   ddsdSrc.dwSize = sizeof (ddsdSrc);
   ddsdTgt.dwSize = sizeof (ddsdTgt);

   lpDDSourceSurface->Lock (NULL, &ddsdSrc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, 0);
   lpDDTargetSurface->Lock (NULL, &ddsdTgt, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, 0);

   stdsource = (UBYTE*)(ddsdSrc.lpSurface) + ddsdSrc.dwHeight/2*ddsdSrc.lPitch;
   stdtarget = (UBYTE*)(ddsdTgt.lpSurface) + midx + midy*ddsdTgt.lPitch;

   SourceBase = ((Alpha*ddsdSrc.dwWidth)>>16) + ddsdSrc.dwWidth;

   Mask = ddsdSrc.dwWidth-1;

   vi.Init (0, (long)((ddsdSrc.dwHeight-1)/2*0.293), (ddsdSrc.dwHeight-1)/2, r);

   for (cy=0; cy<=r; cy++)
   {
      //Radius, dieser waagerechten Kugelscheibe bestimmen:
      xs = SLONG(sqrt (double(r*r-cy*cy)));

      //Clipping auf beiden Seiten (oben & unten)?
      if (midy+cy>=long(ddsdTgt.dwHeight) && midy-cy<0)
      {
         //Ja! ==> Nichts tun!
         continue;
      }
      else
      {
         if (midy+cy>=0 && midy+cy<long(ddsdTgt.dwHeight))
         {
            s = stdsource + SLONG(vi)*ddsdSrc.lPitch;
            t = stdtarget + cy*ddsdTgt.lPitch;

            if (xs>2)
            {
               si.Init (0, (long)(ddsdSrc.dwWidth/4*FXF), ddsdSrc.dwWidth/4, xs+1);
               si++;

               //Schauen, ob clipping für links/rechts notwendig ist:
               if (midx>xs && midx+xs<long(ddsdTgt.dwWidth))
               {
                  t[0]  = s[((SourceBase+SLONG(si))&Mask)];
                  si++;

                  //Nein ==> schnelle Version:
                  for (cx=1; cx<=xs; cx++)
                  {
                     t[cx]  = s[(SourceBase+SLONG(si))&Mask];
                     t[-cx]  = s[(SourceBase-SLONG(si))&Mask];

                     si++;
                  }
               }
               else
               {
                  //Ja, leider ==> langsame clipping Version:
                  for (cx=0; cx<=xs; cx++)
                  {
                     if (midx+cx>=0 && midx+cx < long(ddsdTgt.dwWidth))
                        t[cx]  = s[(SourceBase+SLONG(si))&Mask];

                     if (midx-cx>=0 && midx-cx < long(ddsdTgt.dwWidth))
                        t[-cx] = s[(SourceBase-SLONG(si))&Mask];

                     si++;
                  }
               }
            }
         }

         if (midy-cy>=0 && midy-cy<long(ddsdTgt.dwHeight))
         {
            s = stdsource - SLONG(vi)*ddsdSrc.lPitch;
            t = stdtarget - cy*ddsdTgt.lPitch;

            if (xs>2)
            {
               si.Init (0, (LONG)(ddsdSrc.dwWidth/4*FXF), ddsdSrc.dwWidth/4, xs+1);
               si++;

               //Schauen, ob clipping für links/rechts notwendig ist:
               if (midx > xs && midx+xs < long(ddsdTgt.dwWidth))
               {
                  //Nein ==> schnelle Version:
                  for (cx=0; cx<=xs; cx++)
                  {
                     t[cx]  = s[(SourceBase+SLONG(si))&Mask];
                     t[-cx]  = s[(SourceBase-SLONG(si))&Mask];

                     si++;
                  }
               }
               else
               {
                  //Ja, leider ==> langsame clipping Version:
                  for (cx=0; cx<=xs; cx++)
                  {
                     if (midx+cx>=0 && midx+cx < long(ddsdTgt.dwWidth))
                        t[cx]  = s[(SourceBase+SLONG(si))&Mask];

                     if (midx-cx>=0 && midx-cx < long(ddsdTgt.dwWidth))
                        t[-cx] = s[(SourceBase-SLONG(si))&Mask];

                     si++;
                  }
               }
            }
         }
      }

      //Nächste Zeile vorbereiten:
      vi++;
   }

   lpDDSourceSurface->Unlock(NULL);
   lpDDTargetSurface->Unlock(NULL);
}

//--------------------------------------------------------------------------------------------
//Blittet eine Textur auf eine Kugel; Nur ein Drehfaktor ist dafür erlaubt:
//Die Quellbitmap muß zweimal nebeneinander vorhanden sein, wegen dem Clipping
//--------------------------------------------------------------------------------------------
void Fx2SphereBlitNot2n (LPDIRECTDRAWSURFACE lpDDTargetSurface,
                         LPDIRECTDRAWSURFACE lpDDSourceSurface,
                         SLONG               midx,
                         SLONG               midy,
                         SLONG               r,
                         UWORD               Alpha)
{
   DDSURFACEDESC ddsdSrc, ddsdTgt;
   const UBYTE *s, *stdsource; //Source Bitmap Pointer
   UBYTE       *t, *stdtarget; //TargetBitmap Pointer
   SLONG cx, cy;               //Zähler für die Schleifen
   SLONG xs;                   //x-Größe der aktuellen Zeile
   TBOWVECT<SLONG,16> si;
   TBOWVECT<SLONG,16> vi;

   if (r<1) return;

   ddsdSrc.dwSize = sizeof (ddsdSrc);
   ddsdTgt.dwSize = sizeof (ddsdTgt);

   lpDDSourceSurface->Lock (NULL, &ddsdSrc, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, 0);
   lpDDTargetSurface->Lock (NULL, &ddsdTgt, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, 0);

   stdsource = (UBYTE*)(ddsdSrc.lpSurface) + ddsdSrc.dwHeight/2*ddsdSrc.lPitch + ddsdSrc.dwWidth/8l + ((Alpha*ddsdSrc.dwWidth/2)>>16);
   stdtarget = (UBYTE*)(ddsdTgt.lpSurface) + midx + midy*ddsdTgt.lPitch;

   vi.Init (0, (long)((ddsdSrc.dwHeight-1)/2*0.293), (ddsdSrc.dwHeight-1)/2, r);

   for (cy=0; cy<=r; cy++)
   {
      //Radius, dieser waagerechten Kugelscheibe bestimmen:
      xs = SLONG(sqrt (double(r*r-cy*cy)));

      //Clipping auf beiden Seiten (oben & unten)?
      if (midy+cy>=long(ddsdTgt.dwHeight) && midy-cy<0)
      {
         //Ja! ==> Nichts tun!
         continue;
      }
      else
      {
         if (midy+cy>=0 && midy+cy<long(ddsdTgt.dwHeight))
         {
            s = stdsource + SLONG(vi)*ddsdSrc.lPitch;
            t = stdtarget + cy*ddsdTgt.lPitch;

            if (xs>2)
            {
               //Wenn nicht 2**n x-breiten Verwendet werden, dann 8 statt 4 und Bitmap verdoppeln:
               si.Init (0, (long)(ddsdSrc.dwWidth/8*FXF), ddsdSrc.dwWidth/8, xs+1);
               si++;

               //Schauen, ob clipping für links/rechts notwendig ist:
               if (midx>xs && midx+xs<long(ddsdTgt.dwWidth))
               {
                  register SLONG tmp;

                  t[0]  = s[SLONG(si)];
                  si++;

                  //Nein ==> schnelle Version:
                  for (cx=1; cx<=xs; cx++)
                  {
                     t[cx]  = s[tmp = SLONG(si)];
                     t[-cx] = s[-tmp];

                     si++;
                  }
               }
               else
               {
                  //Ja, leider ==> langsame clipping Version:
                  for (cx=0; cx<=xs; cx++)
                  {
                     if (midx+cx>=0 && midx+cx < long(ddsdTgt.dwWidth))
                        t[cx]  = s[SLONG(si)];

                     if (midx-cx>=0 && midx-cx < long(ddsdTgt.dwWidth))
                        t[-cx] = s[-SLONG(si)];

                     si++;
                  }
               }
            }
         }

         if (midy-cy>=0 && midy-cy<long(ddsdTgt.dwHeight))
         {
            s = stdsource - SLONG(vi)*ddsdSrc.lPitch;
            t = stdtarget - cy*ddsdTgt.lPitch;

            if (xs>2)
            {
               si.Init (0, (LONG)(ddsdSrc.dwWidth/8*FXF), ddsdSrc.dwWidth/8, xs+1);
               si++;

               //Schauen, ob clipping für links/rechts notwendig ist:
               if (midx > xs && midx+xs < long(ddsdTgt.dwWidth))
               {
                  //Nein ==> schnelle Version:
                  for (cx=0; cx<=xs; cx++)
                  {
                     t[cx]  = s[SLONG(si)];
                     t[-cx] = s[-SLONG(si)];

                     si++;
                  }
               }
               else
               {
                  //Ja, leider ==> langsame clipping Version:
                  for (cx=0; cx<=xs; cx++)
                  {
                     if (midx+cx>=0 && midx+cx < long(ddsdTgt.dwWidth))
                        t[cx]  = s[SLONG(si)];

                     if (midx-cx>=0 && midx-cx < long(ddsdTgt.dwWidth))
                        t[-cx] = s[-SLONG(si)];

                     si++;
                  }
               }
            }
         }
      }

      //Nächste Zeile vorbereiten:
      vi++;
   }

   lpDDSourceSurface->Unlock(NULL);
   lpDDTargetSurface->Unlock(NULL);
}
