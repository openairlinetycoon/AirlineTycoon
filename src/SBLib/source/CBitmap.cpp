#include "stdafx.h"

SB_CBitmapMain::SB_CBitmapMain(SDL_Renderer* render)
    : Renderer(render)
{
}

SB_CBitmapMain::~SB_CBitmapMain()
{
    for (std::list<SB_CBitmapCore>::iterator it = Bitmaps.begin(); it != Bitmaps.end(); ++it)
    {
        SDL_FreeSurface(it->lpDDSurface);
        //SDL_DestroyTexture(it->Texture);
    }
}

ULONG SB_CBitmapMain::CreateBitmap(SB_CBitmapCore** out, GfxLib* lib, __int64 name, ULONG flags)
{
    Bitmaps.push_back(SB_CBitmapCore());
    SB_CBitmapCore* core = &Bitmaps.back();
    SDL_Surface* surface = lib->GetSurface(name);
    if (surface)
    {
        core->lpDD = Renderer;
        core->lpDDSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB565, 0);
        //core->Texture = SDL_CreateTextureFromSurface(Renderer, core->lpDDSurface);
        core->Size.x = core->lpDDSurface->w;
        core->Size.y = core->lpDDSurface->h;
        core->InitClipRect();

        if (flags & CREATE_USECOLORKEY)
            core->SetColorKey(0);
        SDL_SetSurfaceRLE(core->lpDDSurface, SDL_TRUE);
    }
    else
    {
        core->lpDD = Renderer;
        core->lpDDSurface = NULL;
        core->Texture = NULL;
        core->Size.x = 0;
        core->Size.y = 0;
    }
    *out = core;
    return 0;
}

ULONG SB_CBitmapMain::CreateBitmap(SB_CBitmapCore** out, SLONG w, SLONG h, ULONG, ULONG flags, ULONG)
{
    Bitmaps.push_back(SB_CBitmapCore());
    SB_CBitmapCore* core = &Bitmaps.back();
    core->lpDD = Renderer;
    core->lpDDSurface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 16, SDL_PIXELFORMAT_RGB565);
    //core->Texture = NULL;
    core->Size.x = w;
    core->Size.y = h;
    if ( !(flags & CREATE_USEALPHA) )
        core->SetColorKey(0);
    core->InitClipRect();
    *out = core;
    return 0;
}

ULONG SB_CBitmapMain::ReleaseBitmap(SB_CBitmapCore* core)
{
    if (core->lpDDSurface)
        SDL_FreeSurface(core->lpDDSurface);
    //if (core->Texture)
    //    SDL_DestroyTexture(core->Texture);
    for (std::list<SB_CBitmapCore>::iterator it = Bitmaps.begin(); it != Bitmaps.end(); ++it)
    {
        if (&*it == core)
        {
            Bitmaps.erase(it);
            break;
        }
    }
    return 0;
}

void SB_CBitmapCore::SetColorKey(ULONG key)
{
    SDL_SetColorKey(lpDDSurface, SDL_TRUE, key);
}

ULONG SB_CBitmapCore::Line(SLONG x1, SLONG y1, SLONG x2, SLONG y2, SB_Hardwarecolor pColor)
{
#if 0
    if (SDL_SetRenderTarget(lpDD, lpDDSurface) < 0)
        return -1;

    dword color = (dword)pColor;
    SDL_SetRenderDrawColor(lpDD, (color & 0xFF0000) >> 16, (color & 0xFF00) >> 8, color & 0xFF, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(lpDD, x1, y1, x2, y2);
#else
    // Bresenham's Line Algorithm
    int x,y,dx,dy,dx1,dy1,px,py,xe,ye,i;
    dx=x2-x1;
    dy=y2-y1;
    dx1=fabs(dx);
    dy1=fabs(dy);
    px=2*dy1-dx1;
    py=2*dx1-dy1;
    if(dy1<=dx1)
    {
        if(dx>=0)
        {
            x=x1;
            y=y1;
            xe=x2;
        }
        else
        {
            x=x2;
            y=y2;
            xe=x1;
        }
        SetPixel(x,y,pColor);
        for(i=0;x<xe;i++)
        {
            x=x+1;
            if(px<0)
            {
                px=px+2*dy1;
            }
            else
            {
                if((dx<0 && dy<0) || (dx>0 && dy>0))
                {
                    y=y+1;
                }
                else
                {
                    y=y-1;
                }
                px=px+2*(dy1-dx1);
            }
            SetPixel(x,y,pColor);
        }
    }
    else
    {
        if(dy>=0)
        {
            x=x1;
            y=y1;
            ye=y2;
        }
        else
        {
            x=x2;
            y=y2;
            ye=y1;
        }
        SetPixel(x,y,pColor);
        for(i=0;y<ye;i++)
        {
            y=y+1;
            if(py<=0)
            {
                py=py+2*dx1;
            }
            else
            {
                if((dx<0 && dy<0) || (dx>0 && dy>0))
                {
                    x=x+1;
                }
                else
                {
                    x=x-1;
                }
                py=py+2*(dx1-dy1);
            }
            SetPixel(x,y,pColor);
        }
    }
#endif
    return 0;
}

void SB_CBitmapCore::SetClipRect(const RECT* pRect)
{
    SetClipRect(*(const CRect*)pRect);
}

void SB_CBitmapCore::SetClipRect(const CRect& rect)
{
   SDL_Rect clip = { rect.left, rect.top, rect.Width(), rect.Height() };
   SDL_SetClipRect(lpDDSurface, &clip);
}

SB_Hardwarecolor SB_CBitmapCore::GetHardwarecolor(ULONG color)
{
#if 0
    SLONG r = GetHighestSetBit(Format.redMask) - GetHighestSetBit(0xFF0000);
    SLONG g = GetHighestSetBit(Format.greenMask) - GetHighestSetBit(0xFF00);
    SLONG b = GetHighestSetBit(Format.blueMask) - GetHighestSetBit(0xFF);

    SLONG result;
    if ( r >= 0 )
        result = Format.redMask & ((color & 0xFF0000) << r);
    else
        result = Format.redMask & ((color & 0xFF0000) >> -(char)r);
    if ( g >= 0 )
        result |= Format.greenMask & ((word)(color & 0xFF00) << g);
    else
        result |= Format.greenMask & ((color & 0xFF00) >> -(char)g);
    if ( b >= 0 )
        result |= Format.blueMask & ((unsigned char)color << b);
    else
        result |= Format.blueMask & ((dword)(unsigned char)color >> -(char)b);
    return (SB_Hardwarecolor)(result);
#else
    char r = (color & 0xFF0000) >> 16;
    char g = (color & 0xFF00) >> 8;
    char b = (color & 0xFF);
    return (SB_Hardwarecolor)SDL_MapRGB(lpDDSurface->format, r, g, b);
#endif
}

ULONG SB_CBitmapCore::Clear(SB_Hardwarecolor color, const RECT* pRect)
{
    if (pRect)
    {
        const CRect& rect = *(const CRect*)pRect;
        SDL_Rect dst = { rect.left, rect.top, rect.Width(), rect.Height() };
        return SDL_FillRect(lpDDSurface, &dst, color.Color);
    }
    else
    {
        return SDL_FillRect(lpDDSurface, NULL, color.Color);
    }
}

ULONG SB_CBitmapCore::SetPixel(SLONG x, SLONG y, SB_Hardwarecolor color)
{
#if 0
    if (SDL_SetRenderTarget(lpDD, lpDDSurface) < 0)
        return -1;

    SDL_SetRenderDrawColor(lpDD, (color & 0xFF0000) >> 16, (color & 0xFF00) >> 8, color & 0xFF, SDL_ALPHA_OPAQUE);
    return SDL_RenderDrawPoint(lpDD, x, y);
#else
    SDL_Rect rect = { x, y, 1, 1 };
    return SDL_FillRect(lpDDSurface, &rect, color.Color);
#endif
}

ULONG SB_CBitmapCore::GetPixel(SLONG x, SLONG y)
{
#if 0
    if (SDL_SetRenderTarget(lpDD, lpDDSurface) < 0)
        return 0;

    SDL_Rect rect = { x, y, 1, 1 };
    ULONG pixel;
    if (SDL_RenderReadPixels(lpDD, &rect, SDL_PIXELFORMAT_RGBA8888, &pixel, sizeof(pixel)) == 0)
        return pixel;
    return 0;
#else
    Uint8 bpp = lpDDSurface->format->BytesPerPixel;
    Uint8 bits = lpDDSurface->format->BitsPerPixel;

    SDL_LockSurface(lpDDSurface);
    Uint8 *p = (Uint8 *)lpDDSurface->pixels + y * lpDDSurface->pitch + x * bpp;
    dword result = *(Uint32*)p;
    SDL_UnlockSurface(lpDDSurface);
    return result & (1 << bits) - 1;
#endif
}

ULONG SB_CBitmapCore::Blit(class SB_CBitmapCore* core, SLONG x, SLONG y, const RECT* pRect, unsigned short, ULONG)
{
    if (pRect)
    {
        const CRect& rect = *(const CRect*)pRect;
        SDL_Rect src = { rect.left, rect.top, rect.Width(), rect.Height() };
        SDL_Rect dst = { x, y, rect.Width(), rect.Height() };
        SDL_BlitSurface(lpDDSurface, &src, core->lpDDSurface, &dst);
    }
    else
    {
        SDL_Rect dst = { x, y, Size.x, Size.y };
        SDL_BlitSurface(lpDDSurface, NULL, core->lpDDSurface, &dst);
    }
    return 0;
}

ULONG SB_CBitmapCore::BlitFast(class SB_CBitmapCore* core, SLONG x, SLONG y, const RECT* pRect, unsigned short)
{
    // Ignore source color key
    Uint32 key = 0;
    int result = SDL_GetColorKey(lpDDSurface, &key);
    if (result != -1)
        SDL_SetColorKey(lpDDSurface, SDL_FALSE, key);

    if (pRect)
    {
        const CRect& rect = *(const CRect*)pRect;
        SDL_Rect src = { rect.left, rect.top, rect.Width(), rect.Height() };
        SDL_Rect dst = { x, y, rect.Width(), rect.Height() };
        SDL_BlitSurface(lpDDSurface, &src, core->lpDDSurface, &dst);
    }
    else
    {
        SDL_Rect dst = { x, y, Size.x, Size.y };
        SDL_BlitSurface(lpDDSurface, NULL, core->lpDDSurface, &dst);
    }

    // Restore color key
    if (result != -1)
        SDL_SetColorKey(lpDDSurface, SDL_TRUE, key);
    return 0;
}


ULONG SB_CBitmapCore::BlitChar(SDL_Surface* font, SLONG x, SLONG y, const RECT* pRect, unsigned short flags)
{
    const CRect& rect = *(const CRect*)pRect;
    SDL_Rect src = { rect.left, rect.top, rect.Width(), rect.Height() };
    SDL_Rect dst = { x, y, rect.Width(), rect.Height() };
    SDL_BlitSurface(font, &src, lpDDSurface, &dst);
    return 0;
}

void SB_CBitmapCore::InitClipRect()
{
    SDL_SetClipRect(lpDDSurface, NULL);
}

SLONG SB_CBitmapCore::Lock(struct _DDSURFACEDESC*) const
{
    return 0;
}

SLONG SB_CBitmapCore::Unlock(struct _DDSURFACEDESC*) const
{
    return 0;
}

ULONG SB_CBitmapCore::Release()
{
    return 0;
}

SB_CPrimaryBitmap::SB_CPrimaryBitmap()
{
}

SB_CPrimaryBitmap::~SB_CPrimaryBitmap()
{
}

bool SB_CPrimaryBitmap::FastClip(CRect clipRect, POINT* pPoint, RECT* pRect)
{
   POINT offset;
   offset.x = 0;
   if (pRect->top <= 0)
      offset.y = 0;
   else
      offset.y = pRect->top;
   if (offset.x || offset.y)
      OffsetRect(pRect, -offset.x, -offset.y);
   if (pRect->right + pPoint->x >= clipRect.right)
      pRect->right = clipRect.right - pPoint->x;
   if (pPoint->x < clipRect.left)
   {
      pRect->left += clipRect.left - pPoint->x;
      pPoint->x = clipRect.left;
   }
   if (pRect->bottom + pPoint->y > clipRect.bottom)
      pRect->bottom = clipRect.bottom - pPoint->y;
   if (pPoint->y < clipRect.top)
   {
      pRect->top += clipRect.top - pPoint->y;
      pPoint->y = clipRect.top;
   }
   if (offset.x || offset.y)
      OffsetRect(pRect, offset.x, offset.y);
   return pRect->right - pRect->left > 0 && pRect->bottom - pRect->top > 0;
}

SLONG SB_CPrimaryBitmap::Flip()
{
    if (Cursor)
        Cursor->FlipBegin();

    //SDL_RenderPresent(lpDD);
    SDL_BlitSurface(lpDDSurface, NULL, SDL_GetWindowSurface(Window), NULL);
    SDL_UpdateWindowSurface(Window);

    if (Cursor)
        Cursor->FlipEnd();
    return 0;
}

void SB_CPrimaryBitmap::SetPos(POINT)
{
}

SLONG SB_CPrimaryBitmap::Create(SDL_Renderer** out, SDL_Window* Wnd, unsigned short flags, SLONG w, SLONG h, unsigned char, unsigned short)
{
    Window = Wnd;
    //lpDD = SDL_CreateRenderer(Window, -1, SDL_RENDERER_SOFTWARE);
    lpDDSurface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 16, SDL_PIXELFORMAT_RGB565);
    Size.x = w;
    Size.y = h;
    Cursor = NULL;
    InitClipRect();
    *out = lpDD;
    return 0;
}

ULONG SB_CPrimaryBitmap::Release()
{
    //SDL_DestroyRenderer(lpDD);
    SDL_DestroyWindow(Window);
    return 0;
}

struct IDirectDrawSurface* SB_CPrimaryBitmap::GetLastPage()
{
    return NULL;
}

SB_CBitmapKey::SB_CBitmapKey(class SB_CBitmapCore& core)
    : Surface(core.lpDDSurface)
    , Bitmap(Surface->pixels)
    , lPitch(Surface->pitch)
{
    SDL_LockSurface(Surface);
}

SB_CBitmapKey::~SB_CBitmapKey()
{
    SDL_UnlockSurface(Surface);
}
