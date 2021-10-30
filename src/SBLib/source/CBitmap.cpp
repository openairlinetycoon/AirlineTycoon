#include "stdafx.h"

SB_CBitmapMain::SB_CBitmapMain(SDL_Renderer* render)
    : Renderer(render)
{
}

SB_CBitmapMain::~SB_CBitmapMain()
{
    for (std::list<SB_CBitmapCore>::iterator it = Bitmaps.begin(); it != Bitmaps.end(); ++it)
        it->Release();
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
        if (flags & CREATE_USECOLORKEY)
            core->SetColorKey(0);

        core->lpTexture = Renderer && flags & CREATE_VIDMEM ?
            SDL_CreateTextureFromSurface(Renderer, core->lpDDSurface) : NULL;
        core->Size.x = core->lpDDSurface->w;
        core->Size.y = core->lpDDSurface->h;
        core->InitClipRect();

        SDL_SetSurfaceRLE(core->lpDDSurface, SDL_TRUE);
    }
    else
    {
        core->lpDD = Renderer;
        core->lpDDSurface = NULL;
        core->lpTexture = NULL;
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
    if (flags & CREATE_INDEXED)
        core->lpDDSurface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 8, SDL_PIXELFORMAT_INDEX8);
    else if (flags & CREATE_USEALPHA)
        core->lpDDSurface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA8888);
    else
        core->lpDDSurface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 16, SDL_PIXELFORMAT_RGB565);
    if (flags & CREATE_USECOLORKEY)
        core->SetColorKey(0);
    core->lpTexture = Renderer && flags & CREATE_VIDMEM ?
        SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h) : NULL;
    if (core->lpTexture && flags & (CREATE_USEALPHA || CREATE_USECOLORKEY))
        SDL_SetTextureBlendMode(core->lpTexture, SDL_BLENDMODE_BLEND);
    core->Size.x = w;
    core->Size.y = h;
    core->InitClipRect();
    //SDL_SetSurfaceRLE(core->lpDDSurface, SDL_TRUE);
    *out = core;
    return 0;
}

ULONG SB_CBitmapMain::ReleaseBitmap(SB_CBitmapCore* core)
{
    core->Release();
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

ULONG SB_CBitmapCore::Line(SLONG x1, SLONG y1, SLONG x2, SLONG y2, SB_Hardwarecolor hwcolor)
{
    if (lpTexture)
    {
        if (SDL_SetRenderTarget(lpDD, lpTexture) < 0)
            return 1;

        dword key;
        dword color = (dword)hwcolor;
        SDL_GetColorKey(lpDDSurface, &key);
        SDL_SetRenderDrawColor(lpDD, (color & 0xFF0000) >> 16, (color & 0xFF00) >> 8, color & 0xFF,
            color == key ? SDL_ALPHA_TRANSPARENT : SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine(lpDD, x1, y1, x2, y2);
    }

    // Bresenham's Line Algorithm
    int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
    dx = x2 - x1;
    dy = y2 - y1;
    dx1 = fabs(dx);
    dy1 = fabs(dy);
    px = 2 * dy1 - dx1;
    py = 2 * dx1 - dy1;
    if (dy1 <= dx1)
    {
        if (dx >= 0)
        {
            x = x1;
            y = y1;
            xe = x2;
        }
        else
        {
            x = x2;
            y = y2;
            xe = x1;
        }
        SetPixel(x, y, hwcolor);
        for (i = 0; x < xe; i++)
        {
            x = x + 1;
            if (px < 0)
            {
                px = px + 2 * dy1;
            }
            else
            {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                {
                    y = y + 1;
                }
                else
                {
                    y = y - 1;
                }
                px = px + 2 * (dy1 - dx1);
            }
            SetPixel(x, y, hwcolor);
        }
    }
    else
    {
        if (dy >= 0)
        {
            x = x1;
            y = y1;
            ye = y2;
        }
        else
        {
            x = x2;
            y = y2;
            ye = y1;
        }
        SetPixel(x, y, hwcolor);
        for (i = 0; y < ye; i++)
        {
            y = y + 1;
            if (py <= 0)
            {
                py = py + 2 * dx1;
            }
            else
            {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
                {
                    x = x + 1;
                }
                else
                {
                    x = x - 1;
                }
                py = py + 2 * (dx1 - dy1);
            }
            SetPixel(x, y, hwcolor);
        }
    }
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
    if (r >= 0)
        result = Format.redMask & ((color & 0xFF0000) << r);
    else
        result = Format.redMask & ((color & 0xFF0000) >> -(char)r);
    if (g >= 0)
        result |= Format.greenMask & ((word)(color & 0xFF00) << g);
    else
        result |= Format.greenMask & ((color & 0xFF00) >> -(char)g);
    if (b >= 0)
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

ULONG SB_CBitmapCore::Clear(SB_Hardwarecolor hwcolor, const RECT* pRect)
{
    dword color = (dword)hwcolor;

    if (lpTexture)
    {
        if (SDL_SetRenderTarget(lpDD, lpTexture) < 0)
            return 1;

        dword key;
        SDL_GetColorKey(lpDDSurface, &key);
        SDL_SetRenderDrawColor(lpDD, (color & 0xFF0000) >> 16, (color & 0xFF00) >> 8, color & 0xFF,
            color == key ? SDL_ALPHA_TRANSPARENT : SDL_ALPHA_OPAQUE);
    }

    if (pRect)
    {
        const CRect& rect = *(const CRect*)pRect;
        SDL_Rect dst = { rect.left, rect.top, rect.Width(), rect.Height() };
        if (lpTexture)
            SDL_RenderFillRect(lpDD, &dst);
        return SDL_FillRect(lpDDSurface, &dst, color);
    }
    else
    {
        if (lpTexture)
            SDL_RenderFillRect(lpDD, NULL);
        return SDL_FillRect(lpDDSurface, NULL, color);
    }
}

ULONG SB_CBitmapCore::SetPixel(SLONG x, SLONG y, SB_Hardwarecolor hwcolor)
{
    if (SDL_MUSTLOCK(lpDDSurface) && SDL_LockSurface(lpDDSurface) < 0)
        return 1;
    Uint8 bpp = lpDDSurface->format->BytesPerPixel;
    Uint8* p = (Uint8*)lpDDSurface->pixels + y * lpDDSurface->pitch + x * bpp;
    *(Uint32*)p = (dword)hwcolor;
    if (SDL_MUSTLOCK(lpDDSurface))
        SDL_UnlockSurface(lpDDSurface);
    return 0;
}

ULONG SB_CBitmapCore::GetPixel(SLONG x, SLONG y)
{
    if (SDL_MUSTLOCK(lpDDSurface) && SDL_LockSurface(lpDDSurface) < 0)
        return 1;
    Uint8 bpp = lpDDSurface->format->BytesPerPixel;
    Uint8 bits = lpDDSurface->format->BitsPerPixel;
    Uint8* p = (Uint8*)lpDDSurface->pixels + y * lpDDSurface->pitch + x * bpp;
    dword result = *(Uint32*)p;
    if (SDL_MUSTLOCK(lpDDSurface))
        SDL_UnlockSurface(lpDDSurface);
    return result & (1 << bits) - 1;
}

Uint16 get_pixel16(SDL_Surface* surface, int x, int y)
{
    //Convert the pixels to 32 bit
    Uint16* pixels = (Uint16*)surface->pixels;

    //Get the requested pixel
    return pixels[(y * surface->pitch / 2) + x];
}

void put_pixel16(SDL_Surface* surface, int x, int y, Uint16 pixel)
{
    //Convert the pixels to 32 bit
    Uint16* pixels = (Uint16*)surface->pixels;

    //Set the pixel
    pixels[(y * surface->pitch / 2) + x] = pixel;
}

SDL_Surface* SB_CBitmapCore::GetFlippedSurface() {
    if (flippedBufferSurface != nullptr)
        return flippedBufferSurface;

    flippedBufferSurface = SDL_CreateRGBSurfaceWithFormat(lpDDSurface->flags, lpDDSurface->w, lpDDSurface->h, lpDDSurface->format->BitsPerPixel, lpDDSurface->format->format);

    if (SDL_MUSTLOCK(lpDDSurface)) {
        //Lock the surface
        SDL_LockSurface(lpDDSurface);
        SDL_LockSurface(flippedBufferSurface);
    }

    for (int x = 0, rx = lpDDSurface->w - 1; x < lpDDSurface->w; x++, rx--) {
        //Go through rows
        for (int y = 0, ry = lpDDSurface->h - 1; y < lpDDSurface->h; y++, ry--) {
            Uint16 pixel = get_pixel16(lpDDSurface, x, y);
            put_pixel16(flippedBufferSurface, rx, y, pixel);
        }
    }

    if (SDL_MUSTLOCK(lpDDSurface)) {
        //Lock the surface
        SDL_UnlockSurface(lpDDSurface);
        SDL_UnlockSurface(flippedBufferSurface);
    }
	
    UINT32 key;
    if (SDL_GetColorKey(lpDDSurface, &key) == 0)
    {
        SDL_SetColorKey(flippedBufferSurface, true, key);
    }

    return flippedBufferSurface;
}


ULONG SB_CBitmapCore::Blit(class SB_CBitmapCore* core, SLONG x, SLONG y, const RECT* pRect, unsigned short, ULONG)
{
    if (pRect)
    {
        const CRect& rect = *(const CRect*)pRect;
        SDL_Rect src = { rect.left, rect.top, rect.Width(), rect.Height() };
        SDL_Rect dst = { x, y, rect.Width(), rect.Height() };
        return SDL_BlitSurface(lpDDSurface, &src, core->lpDDSurface, &dst);
    }
    else
    {
        SDL_Rect dst = { x, y, Size.x, Size.y };
        return SDL_BlitSurface(lpDDSurface, NULL, core->lpDDSurface, &dst);
    }
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

ULONG SB_CBitmapCore::BlitChar(SDL_Surface* font, SLONG x, SLONG y, const SDL_Rect* pRect, unsigned short flags)
{
    SDL_Rect dst = { x, y, pRect->w, pRect->h };
    return SDL_BlitSurface(font, pRect, lpDDSurface, &dst);
}

void SB_CBitmapCore::InitClipRect()
{
    SDL_SetClipRect(lpDDSurface, NULL);
}

ULONG SB_CBitmapCore::Release()
{
    if (lpDDSurface)
        SDL_FreeSurface(lpDDSurface);
    if (flippedBufferSurface)
        SDL_FreeSurface(flippedBufferSurface);
    if (lpTexture)
        SDL_DestroyTexture(lpTexture);
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
    if (lpDD)
    {
        /*
         * None of the SDL renderers actually lock the GPU resource,
         * they all use either staging memory or a staging texture.
         * Thus we can still use the texture while it's locked and
         * we simply cycle through lock/unlock to update the texture.
         */
        SDL_UnlockTexture(lpTexture);
        if (SDL_LockTextureToSurface(lpTexture, NULL, &lpDDSurface) < 0)
            return -1;
    }
    else
    {
        if (Cursor)
            Cursor->FlipBegin();

        SDL_Rect dst = { 0, 0, Size.x, Size.y };
        if (SDL_BlitScaled(lpDDSurface, NULL, SDL_GetWindowSurface(Window), &dst) < 0)
            return -2;

        if (Cursor)
            Cursor->FlipEnd();
    }

    return Present();
}

SLONG SB_CPrimaryBitmap::Present()
{
    if (lpDD) {
        SDL_SetRenderDrawColor(lpDD, 0, 0, 0, 255);
        SDL_RenderClear(lpDD);

        // Set the backbuffer as the render target
        if (SDL_SetRenderTarget(lpDD, NULL) < 0)
            return -1;

        // Copy our primary texture to the backbuffer
        if (SDL_RenderCopy(lpDD, lpTexture, NULL, NULL) < 0)
            return -2;

        // Render the cursor onto the backbuffer
        if (Cursor)
            Cursor->Render(lpDD);

        SDL_RenderPresent(lpDD);
    }
    else
    {
        if (SDL_UpdateWindowSurface(Window) < 0)
            return -3;
        SDL_Delay(10); // Ensure we don't run too fast without v-sync
    }
    return 0;
}

void SB_CPrimaryBitmap::SetPos(POINT)
{
}

SLONG SB_CPrimaryBitmap::Create(SDL_Renderer** out, SDL_Window* Wnd, unsigned short flags, SLONG w, SLONG h, unsigned char, unsigned short)
{
    Window = Wnd;
    lpDD = SDL_CreateRenderer(Window, -1, SDL_RENDERER_PRESENTVSYNC);

    if (lpDD)
    {
        Hdu.HercPrintf("Using hardware accelerated presentation");
        lpTexture = SDL_CreateTexture(lpDD, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, w, h);
        if (SDL_LockTextureToSurface(lpTexture, NULL, &lpDDSurface) < 0)
        {
            Hdu.HercPrintf("Unable to lock backbuffer to surface");
            return -1;
        }
    }
    else
    {
        Hdu.HercPrintf("Falling back to software presentation");
        lpTexture = NULL;
        lpDDSurface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 16, SDL_PIXELFORMAT_RGB565);
    }
    Size.x = w;
    Size.y = h;
    Cursor = NULL;
    InitClipRect();
    *out = lpDD;
    return 0;
}

ULONG SB_CPrimaryBitmap::Release()
{
    if (lpTexture)
        SDL_DestroyTexture(lpTexture);
    if (lpDD)
        SDL_DestroyRenderer(lpDD);
    else if (lpDDSurface)
        SDL_FreeSurface(lpDDSurface);
    return 0;
}

SB_CBitmapKey::SB_CBitmapKey(class SB_CBitmapCore& core)
    : Surface(core.lpDDSurface)
{
    if(SDL_MUSTLOCK(Surface))
        SDL_LockSurface(Surface);
    Bitmap = Surface->pixels;
    lPitch = Surface->pitch;
}

SB_CBitmapKey::~SB_CBitmapKey()
{
    if (SDL_MUSTLOCK(Surface))
        SDL_UnlockSurface(Surface);
}