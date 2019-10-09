#include "stdafx.h"

void memswap(void* dst, void* src, ULONG size)
{
    for (unsigned int i = size; i > 0; --i)
        Swap(((BYTE*)dst)[i - 1], ((BYTE*)src)[i - 1]);
}

long CalcInertiaVelocity(long a, long b)
{
    if ( a <= b )
    {
        if ( a >= b )
            return 0;
        else
            return -(long)sqrt((double)(b - a + 1) / 2.0 + 0.5);
    }
    else
    {
        return (long)sqrt((double)(a - b + 1) / 2.0 + 0.5);
    }
}

long Calc1nSum(long a)
{
    return a * (a + 1) / 2;
}

double GetFrameRate()
{
    TeakLibW_Exception(0, 0, ExcNotImplemented);
    return 42.0;
}
