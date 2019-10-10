#include "stdafx.h"

const char* ExcOpen     = "Can't open %s!";
const char* ExcRead     = "Can't read %s!";
const char* ExcWrite    = "Can't write %s!";
const char* ExcSeek     = "Can't seek %s at %li!";

TEAKFILE::TEAKFILE()
    : Ctx(NULL)
    , Path(NULL)
    , MemPointer(0)
    , MemBufferUsed(0)

{
}

TEAKFILE::TEAKFILE(char const* path, long mode)
    : Ctx(NULL)
    , Path(NULL)
    , MemPointer(0)
    , MemBufferUsed(0)
{
    Open(path, mode);
}

TEAKFILE::~TEAKFILE()
{
    Close();
}

void TEAKFILE::ReadLine(char* buffer, long size)
{
    int i;
    for (i = 0; i < size && !IsEof(); i++)
    {
        char c = SDL_ReadU8(Ctx);

        if (c == '\r')
            continue;

        if (c == '\n')
            break;

        buffer[i] = c;
    }

    if (i < size)
        buffer[i] = '\0';
}

int TEAKFILE::IsEof() { return SDL_RWtell(Ctx) >= SDL_RWsize(Ctx); }

void TEAKFILE::Close()
{
    if (Ctx)
        SDL_RWclose(Ctx);
    Ctx = NULL;
    if (Path)
        SDL_free(Path);
    Path = NULL;
}

long TEAKFILE::GetFileLength(void) { return (long)SDL_RWsize(Ctx); }

long TEAKFILE::GetPosition(void) { return (long)SDL_RWtell(Ctx); }

void TEAKFILE::Open(char const* path, long mode)
{
    Ctx = SDL_RWFromFile(path, mode == TEAKFILE_WRITE ? "wb" : "rb");
    if (!Ctx)
        TeakLibW_Exception(0, 0, ExcOpen, Path);

    Path = SDL_strdup(path);

}

int TEAKFILE::IsOpen() { return Ctx != NULL; }

void TEAKFILE::Read(unsigned char* buffer, long size)
{
    if (MemBuffer.AnzEntries() > 0)
    {
        long anz;
        if ( size >= MemBufferUsed - MemPointer )
            anz = MemBufferUsed - MemPointer;
        else
            anz = size;
        memcpy(buffer, MemPointer + MemBuffer, anz);
        MemPointer += size;
    }
    else
    {
        if (SDL_RWread(Ctx, buffer, 1, size) != size)
            TeakLibW_Exception(0, 0, ExcRead, Path);
    }
}

void TEAKFILE::Write(unsigned char* buffer, long size)
{
    if (MemBuffer.AnzEntries() > 0)
    {
        if (MemPointer + size > MemBuffer.AnzEntries())
        {
            long slack = MemBuffer.AnzEntries() / 10;
            MemBuffer.ReSize(slack + size + MemPointer);
        }
        memcpy(MemBuffer + MemPointer, buffer, size);
        MemPointer += size;
        MemBufferUsed += size;
    }
    else
    {
        if (SDL_RWwrite(Ctx, buffer, 1, size) != size)
            TeakLibW_Exception(0, 0, ExcWrite, Path);
    }
}

void TEAKFILE::ReadTrap(long trap)
{
    if (SDL_ReadLE32(Ctx) != trap)
        DebugBreak();
}

void TEAKFILE::WriteTrap(long trap)
{
    SDL_WriteLE32(Ctx, trap);
}

void TEAKFILE::SetPosition(long pos)
{
    if (SDL_RWseek(Ctx, pos, RW_SEEK_SET) < 0)
        TeakLibW_Exception(0, 0, ExcSeek, Path, pos);
}

void TEAKFILE::Announce(long size)
{
    MemBuffer.ReSize(size);
}

CRLEReader::CRLEReader(const char* path)
    : Ctx(NULL)
    , SeqLength(0)
    , SeqUsed(0)
    , IsSeq(false)
    , Sequence()
    , IsRLE(false)
    , Size(0)
    , Key(0)
{
    Ctx = SDL_RWFromFile(path, "rb");
    if (Ctx)
    {
        char str[6];
        SDL_RWread(Ctx, str, sizeof(str), 1);
        if (!strcmp(str, "xtRLE"))
        {
            IsRLE = true;
            int version = SDL_ReadLE32(Ctx);
            if (version >= 0x102)
                Key = 0xA5;
            if (version >= 0x101)
                Size = SDL_ReadLE32(Ctx);
        }
        else
        {
            Size = (long)SDL_RWsize(Ctx);
            SDL_RWseek(Ctx, 0, RW_SEEK_SET);
        }
    }
}

CRLEReader::~CRLEReader()
{
    Close();
}

bool CRLEReader::Close()
{
    if (!Ctx)
        return false;
    return SDL_RWclose(Ctx) == 0;
}

bool CRLEReader::Buffer(void* buffer, long size)
{
    return SDL_RWread(Ctx, buffer, size, 1) > 0;
}

bool CRLEReader::NextSeq()
{
    if (!Buffer(&SeqLength, 1))
        return false;

    if (SeqLength & 0x80)
    {
        SeqLength &= 0x7Fu;
        SeqUsed = 0;
        IsSeq = true;
        if (!Buffer(Sequence, SeqLength))
            return false;
        for (int i = 0; i < SeqLength; i++)
            Sequence[i] ^= Key;
    }
    else
    {
        IsSeq = false;
        if (!Buffer(Sequence, 1))
            return false;
    }
    return true;
}

bool CRLEReader::Read(BYTE* buffer, long size, bool decode)
{
    if (!decode || !IsRLE)
        return Buffer(buffer, size);

    for (long i = 0; i < size; i++)
    {
        if (!SeqLength && !NextSeq())
            return false;

        if (IsSeq)
        {
            buffer[i] = Sequence[SeqUsed++];
            if (SeqUsed == SeqLength)
                SeqLength = 0;
        }
        else
        {
            buffer[i] = Sequence[0];
            SeqLength--;
        }
    }
    return true;
}

int DoesFileExist(char const* path)
{
    SDL_RWops *ctx = SDL_RWFromFile(path, "rb");
    if (ctx)
    {
        SDL_RWclose(ctx);
        return true;
    }
    return false;
}

BUFFER<BYTE>* LoadCompleteFile(char const* path)
{
    CRLEReader reader(path);
    BUFFER<BYTE>* buffer = new BUFFER<BYTE>(reader.GetSize());
    if (!reader.Read(*buffer, buffer->AnzEntries(), true))
    {
        delete buffer;
        return NULL;
    }
    return buffer;
}
