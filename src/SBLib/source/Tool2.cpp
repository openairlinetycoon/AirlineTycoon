#include "stdafx.h"
#include <SDL_bits.h>

#ifdef ENABLE_ASM
long GetLowestSetBit(long mask)
{
    unsigned long result;
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

long GetHighestSetBit(long mask)
{
#ifdef ENABLE_ASM
    unsigned long result;
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
