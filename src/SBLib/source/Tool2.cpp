#include "stdafx.h"

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

long GetHighestSetBit(long mask)
{
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
}

void ODS(char const *, ...)
{
}
