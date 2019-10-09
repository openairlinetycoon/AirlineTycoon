#include "stdafx.h"

char* bprintf(char const* format, ...)
{
    static char buffer[8192];
    va_list args;
    va_start (args, format);
    vsprintf (buffer, format, args);
    va_end (args);
    return buffer;
}

char* bitoa(long val, long radix)
{
    static char buffer[20];
    ltoa(val, buffer, radix);
    return buffer;
}

char* TeakStrRemoveEndingCodes(char* str, char const* codes)
{
    int i;
    for (i = strlen(str) - 1; i >= 0 && strchr(codes, str[i]); --i);
    str[i + 1] = 0;
    return str;
}

char* TeakStrRemoveCppComment(char* str)
{
    for (int i = 0; str[i]; ++i)
    {
        if (str[i] == '/' && str[i + 1] == '/')
        {
            str[i] = 0;
            return str;
        }
    }
    return str;
}

unsigned char GerToLower(unsigned char c)
{
    if (c >= 0x41 && c <= 0x5A)
        return c + 0x20;
    switch (c + 0x72)
    {
        case 0: return 0x84u;
        case 0xB: return 0x94u;
        case 0xC: return 0x81u;
        case 0x36: return 0xE4u;
        case 0x48: return 0xF6u;
        case 0x4E: return 0xFCu;
    }
    return c;
}

unsigned char GerToUpper(unsigned char c)
{
    if (c >= 0x61 && c <= 0x7A)
        return c - 0x20;
    switch (c + 0x7F)
    {
        case 0: return 0x9Au;
        case 3: return 0x8Eu;
        case 0x13: return 0x99u;
        case 0x63: return 0xC4u;
        case 0x75: return 0xD6u;
        case 0x7B: return 0xDCu;
    }
    return c;
}

unsigned char* RecapizalizeString(unsigned char* str)
{
    for (int i = 0; str[i]; ++i)
    {
        if (i && (i <= 0 || str[i - 1] != ' ' && str[i - 1] != '-'))
            str[i] = GerToLower(str[i]);
        else
            str[i] = GerToUpper(str[i]);
    }
    return str;
}

const char* GetSuffix(const char* str)
{
    for (int i = strlen(str); i > 0; --i)
    {
        if (str[i] == '.')
            return &str[i + 1];
    }
    return "";
}
