#include "stdafx.h"
#include <SDL_bits.h>

#ifdef ENABLE_ASM
SLONG GetLowestSetBit(SLONG mask)
{
    unsigned SLONG result;
    //_BitScanForward(&result, mask);
    __asm
    {
        push eax
        bsf eax, mask
        mov result, eax
        pop eax
    }
    return result;
}
#endif

SLONG GetHighestSetBit(SLONG mask)
{
#ifdef ENABLE_ASM
    unsigned SLONG result;
    //_BitScanReverse(&result, mask);
    __asm
    {
        push eax
        bsr eax, mask
        mov result, eax
        pop eax
    }
    return result;
#else
    return SDL_MostSignificantBitIndex32(mask);
#endif
}

void ODS(char const *, ...)
{
}
