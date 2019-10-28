#include "stdafx.h"

SB_CBitmapMain::SB_CBitmapMain(SDL_Renderer* render)
    : Renderer(render)
{
}

SB_CBitmapMain::~SB_CBitmapMain()
{
}

unsigned long SB_CBitmapMain::CreateBitmap(SB_CBitmapCore** out, GfxLib* lib, __int64 name, unsigned long flags)
{
    SB_CBitmapCore* core = new SB_CBitmapCore();
    core->lpDDSurface = lib->GetSurface(name);
    if (core->lpDDSurface)
    {
        if (flags & CREATE_USECOLORKEY)
            core->SetColorKey(0);
        core->lpDD = Renderer;
        core->Texture = SDL_CreateTextureFromSurface(Renderer, core->lpDDSurface);
        core->Size.x = core->lpDDSurface->w;
        core->Size.y = core->lpDDSurface->h;
        core->FillPixelFormat(core->lpDDSurface->format);
        core->InitClipRect();
    }
    else
    {
        core->lpDD = Renderer;
        core->lpDDSurface = NULL;
        core->Size.x = 0;
        core->Size.y = 0;
        core->FillPixelFormat(NULL);
    }
    *out = core;
    return 0;
}

unsigned long SB_CBitmapMain::CreateBitmap(SB_CBitmapCore** out, long w, long h, unsigned long, unsigned long flags, unsigned long)
{
    SB_CBitmapCore* core = new SB_CBitmapCore();
    core->lpDD = Renderer;
    core->lpDDSurface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 16, SDL_PIXELFORMAT_RGB565);
    core->Texture = NULL;
    core->Size.x = w;
    core->Size.y = h;
    if ( !(flags & CREATE_USEALPHA) )
        core->SetColorKey(0);
    core->InitClipRect();
    core->FillPixelFormat(core->lpDDSurface->format);
    *out = core;
    return 0;
}

unsigned long SB_CBitmapMain::ReleaseBitmap(SB_CBitmapCore* core)
{
    //SDL_DestroyTexture(core->Texture);
    return 0;
}

void SB_CBitmapCore::FillPixelFormat(SDL_PixelFormat* format)
{
    memset(&Format, 0, sizeof(PixelFormat));
    if (format)
    {
        Format.bitDepth = format->BitsPerPixel;
        Format.bytesperPixel = format->BytesPerPixel;
        Format.redMask = format->Rmask;
        Format.greenMask = format->Gmask;
        Format.blueMask = format->Bmask;
        Format.bitDepthAgain = format->BitsPerPixel;
        Format.bitDepthStatic = 16;
    }
}

void SB_CBitmapCore::SetColorKey(unsigned long key)
{
    SDL_SetColorKey(lpDDSurface, SDL_TRUE, key);
}

unsigned long SB_CBitmapCore::Line(long x1, long y1, long x2, long y2, class SB_CHardwarecolorHelper* pColor)
{
#if 0
    if (SDL_SetRenderTarget(lpDD, lpDDSurface) < 0)
        return -1;

    dword color = (dword)pColor;
    SDL_SetRenderDrawColor(lpDD, (color & 0xFF0000) >> 16, (color & 0xFF00) >> 8, color & 0xFF, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(lpDD, x1, y1, x2, y2);
#endif
    return 0;
}

void SB_CBitmapCore::SetClipRect(const RECT* pRect)
{
    const CRect& rect = *(const CRect*)pRect;
    SDL_Rect clip = { rect.left, rect.top, rect.Width(), rect.Height() };
    SDL_SetClipRect(lpDDSurface, &clip);
}

class SB_CHardwarecolorHelper* SB_CBitmapCore::GetHardwarecolor(unsigned long color)
{
#if 0
    long r = GetHighestSetBit(Format.redMask) - GetHighestSetBit(0xFF0000);
    long g = GetHighestSetBit(Format.greenMask) - GetHighestSetBit(0xFF00);
    long b = GetHighestSetBit(Format.blueMask) - GetHighestSetBit(0xFF);

    long result;
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
    return (class SB_CHardwarecolorHelper*)(result);
#else
    char r = (color & 0xFF0000) >> 16;
    char g = (color & 0xFF00) >> 8;
    char b = (color & 0xFF);
    return (class SB_CHardwarecolorHelper*)(SDL_MapRGB(lpDDSurface->format, r, g, b));
#endif
}

unsigned long SB_CBitmapCore::Clear(class SB_CHardwarecolorHelper* pColor, const RECT* pRect)
{
    dword color = (dword)pColor;
    if (pRect)
    {
        const CRect& rect = *(const CRect*)pRect;
        SDL_Rect dst = { rect.left, rect.top, rect.Width(), rect.Height() };
        return SDL_FillRect(lpDDSurface, &dst, color);
    }
    else
    {
        return SDL_FillRect(lpDDSurface, NULL, color);
    }
}

unsigned long SB_CBitmapCore::SetPixel(long x, long y, class SB_CHardwarecolorHelper* pColor)
{
    dword color = (dword)pColor;
#if 0
    if (SDL_SetRenderTarget(lpDD, lpDDSurface) < 0)
        return -1;

    SDL_SetRenderDrawColor(lpDD, (color & 0xFF0000) >> 16, (color & 0xFF00) >> 8, color & 0xFF, SDL_ALPHA_OPAQUE);
    return SDL_RenderDrawPoint(lpDD, x, y);
#else
    SDL_Rect rect = { x, y, 1, 1 };
    return SDL_FillRect(lpDDSurface, &rect, color);
#endif
}

unsigned long SB_CBitmapCore::GetPixel(long x, long y)
{
#if 0
    if (SDL_SetRenderTarget(lpDD, lpDDSurface) < 0)
        return 0;

    SDL_Rect rect = { x, y, 1, 1 };
    unsigned long pixel;
    if (SDL_RenderReadPixels(lpDD, &rect, SDL_PIXELFORMAT_RGBA8888, &pixel, sizeof(pixel)) == 0)
        return pixel;
#endif
    return 0;
}

unsigned long SB_CBitmapCore::Blit(class SB_CBitmapCore* core, long x, long y, const RECT* pRect, unsigned short, unsigned long)
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

unsigned long SB_CBitmapCore::BlitFast(class SB_CBitmapCore* core, long x, long y, const RECT* pRect, unsigned short)
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


unsigned long SB_CBitmapCore::BlitChar(SDL_Surface* font, long x, long y, const RECT* pRect, unsigned short flags)
{
    SDL_SetColorKey(font, SDL_FALSE, 0);
    SDL_SetColorKey(lpDDSurface, SDL_TRUE, 0);
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

long SB_CBitmapCore::Lock(struct _DDSURFACEDESC*) const
{
    return 0;
}

long SB_CBitmapCore::Unlock(struct _DDSURFACEDESC*) const
{
    return 0;
}

unsigned long SB_CBitmapCore::Release()
{
    return 0;
}

SB_CPrimaryBitmap::SB_CPrimaryBitmap()
{
}

SB_CPrimaryBitmap::~SB_CPrimaryBitmap()
{
}

long SB_CPrimaryBitmap::Flip()
{
    //SDL_RenderPresent(lpDD);
    SDL_UpdateWindowSurface(Window);
    return 0;
}

void SB_CPrimaryBitmap::SetPos(struct tagPOINT&)
{
}

long SB_CPrimaryBitmap::Create(SDL_Renderer** out, HWND& hWnd, unsigned short flags, long w, long h, unsigned char, unsigned short)
{
    Window = SDL_CreateWindowFrom(hWnd);
    //lpDD = SDL_CreateRenderer(Window, -1, SDL_RENDERER_SOFTWARE);
    lpDDSurface = SDL_GetWindowSurface(Window);
    Size.x = w;
    Size.y = h;
    Cursor = NULL;
    InitClipRect();
    FillPixelFormat(lpDDSurface->format);
    *out = lpDD;
    return 0;
}

unsigned long SB_CPrimaryBitmap::Release()
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
