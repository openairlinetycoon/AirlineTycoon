#include "stdafx.h"

const char* ExcTextResNotOpened         = "TextRes not opened!";
const char* ExcTextResStaticOverflow    = "TextRes is too long: %lx:%lx";
const char* ExcTextResFormat            = "Bad TextRes format: %s (%li)";
const char* ExcTextResNotFound          = "TextRes not found: %lx:%lx";

long gLanguage;

void LanguageSpecifyString(char *Dst)
{
    char langs[24];

    strcpy(langs, "DEFTPNISOBJKLMNQRTUV");
    if (Dst[0] && Dst[1] && Dst[1] == ':' && Dst[2] && Dst[2] == ':')
    {
        for (int i = 0; Dst[i]; ++i)
        {
            if (Dst[i] == langs[gLanguage] && Dst[i + 1] && Dst[i + 1] == ':' && Dst[i + 2] && Dst[i + 2] == ':')
            {
                for (int j = i + 2; Dst[j] && (!Dst[j + 1] || Dst[j + 1] != ':' || !Dst[j + 2] || Dst[j + 2] != ':'); ++j);
                memmove(Dst, &Dst[i + 3], j - i - 3);
                Dst[j - i - 3] = 0;
                if (j - i - 3 > 0 && Dst[j - i - 4] == 32)
                    Dst[j - i - 4] = 0;
                return;
            }
        }

        for (int lang = 1; lang >= 0; --lang)
        {
            for (i = 0; Dst[i]; ++i)
            {
                if (Dst[i] == langs[lang] && Dst[i + 1] && Dst[i + 1] == ':' && Dst[i + 2] && Dst[i + 2] == ':')
                    break;
            }
            if (Dst[i])
            {
                for (int j = i + 2; Dst[j] && (!Dst[j + 1] || Dst[j + 1] != ':' || !Dst[j + 2] || Dst[j + 2] != ':'); ++j);
                memmove(Dst, &Dst[i + 3], j - i - 3);
                Dst[j - i - 3] = 0;
                if ( j - i - 3 > 0 && Dst[j - i - 4] == 32 )
                    Dst[j - i - 4] = 0;
                return;
            }
        }
    }
}

TEXTRES::TEXTRES()
{
}

TEXTRES::~TEXTRES()
{
}

void TEXTRES::Open(char const* source, void* cached)
{
    Strings.Clear();
    Path.Clear();
    Entries.Clear();
    if (cached)
    {
        long Group = -1;
        long Identifier = -1;

        // FIXME: Memory leak!
        BUFFER<BYTE> FileBuffer = *LoadCompleteFile(source);
        char* String = new char[0x400u];
        if (!String)
            TeakLibW_Exception(0, 0, ExcOutOfMem);

        long AnzStrings = 0;
        long AnzEntries = 0;
        for (long i = 0, j = 0; i < FileBuffer.AnzEntries(); i += j)
        {
            if (FileBuffer[i] == '>' && FileBuffer[i + 1] == '>')
                ++AnzEntries;
            long AnzChars = 0;
            long AnzNonSpace = 0;
            for (j = 0; j + i < FileBuffer.AnzEntries() && FileBuffer[j + i] != '\r' &&
                FileBuffer[j + i] != '\n' && FileBuffer[j + i] != '\x1A'; ++j)
            {
                if (FileBuffer[j + i] == '/' && FileBuffer[j + i + 1] == '/')
                    AnzChars = -1;
                if (AnzChars >= 0)
                    ++AnzChars;
                if (FileBuffer[j + i] != ' ' && AnzChars >= 0)
                    AnzNonSpace = AnzChars;
            }
            if (FileBuffer[i] == ' ' && FileBuffer[i + 1] == ' ' && FileBuffer[i + 2] != ' ')
            {
                AnzStrings += AnzNonSpace + 1;
            }
            while (j + i < FileBuffer.AnzEntries() && (FileBuffer[j + i] == '\r'
                || FileBuffer[j + i] == '\n' || FileBuffer[j + i] == '\x1A'))
                ++j;
        }
        Strings.ReSize(AnzStrings + 5);
        Entries.ReSize(AnzEntries);

        for (i = 0; i < Entries.AnzEntries(); ++i)
            Entries.DelPointer[i].Text = NULL;

        AnzStrings = 0;
        AnzEntries = -1;
        for (i = 0, j = 0; i < FileBuffer.AnzEntries(); i += j)
        {
            long Size;
            if (FileBuffer.AnzEntries() - i <= 1023)
                Size = FileBuffer.AnzEntries() - i;
            else
                Size = 1023;
            memcpy(String, FileBuffer + i, Size);
            for (j = 0; i + j < FileBuffer.AnzEntries() && String[j] != '\r' && String[j] != '\n' && String[j] != '\x1A'; ++j);
            String[j] = 0;
            TeakStrRemoveCppComment(String);
            TeakStrRemoveEndingCodes(String, "\r\n\x1A");
            if (String[0] == '>' && String[1] != '>')
            {
                if (strlen(String + 1) == 4)
                    Group = *(long*)(String + 1);
                else
                    Group = atoi(String + 1);
            }
            if (String[0] == '>' && String[1] == '>')
            {
                if (strlen(String + 2) == 4)
                    Identifier = *(long*)(String + 2);
                else
                    Identifier = atoi(String + 2);
                ++AnzEntries;
            }
            if (String[0] == ' ' && String[1] == ' ' && strlen(String) > 2 && AnzEntries >= 0)
            {
                if (Entries[AnzEntries].Text)
                {
                    strcat(Entries[AnzEntries].Text, String + 2);
                    strcat(Entries[AnzEntries].Text, "");
                    AnzStrings += strlen(String + 2) + 1;
                }
                else
                {
                    if (AnzEntries >= Entries.AnzEntries())
                        TeakLibW_Exception(0, 0, ExcImpossible, "");
                    Entries[AnzEntries].Group = Group;
                    Entries[AnzEntries].Id = Identifier;
                    Entries[AnzEntries].Text = Strings + AnzStrings;
                    strcpy(Entries[AnzEntries].Text, String + 2);
                    if (strlen(String + 2) + AnzStrings >= Strings.AnzEntries() )
                        TeakLibW_Exception(0, 0, ExcImpossible, "");
                    AnzStrings += strlen(String + 2) + 1;
                }
            }
            while (j + i < FileBuffer.AnzEntries() && (FileBuffer[j + i] == '\r'
                        || FileBuffer[j + i] == '\n' || FileBuffer[j + i] == '\x1A'))
                ++j;
        }
        if (String)
            delete [] String;
    }
    else
    {
        Path.ReSize(strlen(source) + 1);
        strcpy(Path, source);
    }
}

char* TEXTRES::GetP(unsigned long group, unsigned long id)
{
    char* text = NULL;
    for (long i = 0; i < Entries.AnzEntries(); ++i)
    {
        if (Entries[i].Group == group && Entries[i].Id == id)
        {
            text = Entries[i].Text;
            break;
        }
    }

    if (!text)
        TeakLibW_Exception(0, 0, ExcTextResNotFound, group, id);

    char* buffer = new char[strlen(text) + 1];
    strcpy(buffer, text);
    LanguageSpecifyString(buffer);
    return buffer;
}

char* TEXTRES::GetS(unsigned long group, unsigned long id)
{
  char* str = TEXTRES::GetP(group, id);
  LanguageSpecifyString(str);
  if (strlen(str) > 0x3FF)
  {
    delete [] str;
    TeakLibW_Exception(0, 0, ExcTextResStaticOverflow, group, id);
  }
  static char buffer[0x3FF];
  strcpy(buffer, str);
  delete [] str;
  return buffer;
}
