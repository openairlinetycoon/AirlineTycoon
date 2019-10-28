#include "stdafx.h"

SB_CCursor::SB_CCursor(class SB_CPrimaryBitmap* primary, class SB_CBitmapCore* core)
    : Cursor(NULL)
{
    SetImage(core);
}

SB_CCursor::~SB_CCursor(void)
{
    if (Cursor)
        SDL_FreeCursor(Cursor);
}

long SB_CCursor::MoveImage(long, long)
{
    return 0;
}

long SB_CCursor::SetImage(class SB_CBitmapCore* core)
{
    if (!core)
        return -1;

    if (Cursor)
        SDL_FreeCursor(Cursor);

    Surface = core->GetSurface();
    if (Surface)
        Cursor = SDL_CreateColorCursor(Surface, 0, 0);
    return 0;
}

long SB_CCursor::Show(bool show)
{
    SDL_SetCursor(Cursor);
    return SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
}
