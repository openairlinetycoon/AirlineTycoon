#include "stdafx.h"

#pragma pack(push)
#pragma pack(1)
typedef struct _GfxLibHeader
{
    dword Length; // 50 bytes
    dword Unknown0;
    word Unknown1;
    dword Unknown2;
    dword Unknown3;
    dword Unknown4;
    dword Unknown5;
    dword BitDepth;
    dword Files;
    dword Pos;
    dword Unknown6;
    dword Unknown7;
    dword Unknown8;
} GfxLibHeader;

typedef struct _GfxChunkHeader
{
    union
    {
        char Name[8];
        __int64 Id;
    };
    dword Offset;
} GfxChunkHeader;

typedef struct _GfxChunkInfo
{
    dword Size;
    char Type;
} GfxChunkInfo;

typedef struct _GfxChunkImage
{
    dword Length; // 76 bytes
    dword Size;
    dword Width;
    dword Height;
    dword Unknown0;
    dword Flags;
    dword BitDepth;
    dword PlaneSize;
    dword Rmask;
    dword Gmask;
    dword Bmask;
    dword OffsetColor;
    dword OffsetAlpha;
    dword OffsetZ;
    dword Unknown1;
    dword Unknown2;
    dword Unknown3;
    dword Unknown4;
    dword Unknown5;
} GfxChunkImage;
#pragma pack(pop)

enum
{
    CHUNK_GFX = 1,
    CHUNK_NAME,
    CHUNK_PALETTE
};

GfxMain::GfxMain(SDL_Renderer*)
{
}

GfxMain::~GfxMain()
{
    for (std::list<GfxLib>::iterator it = Libs.begin(); it != Libs.end(); ++it)
        it->Release();
}

SLONG GfxMain::LoadLib(char* path, class GfxLib** out, SLONG)
{
    Libs.push_back(GfxLib(this, NULL, path, 0, 0, NULL));
    *out = &Libs.back();
    return 0;
}

SLONG GfxMain::ReleaseLib(class GfxLib* lib)
{
    lib->Release();

    for (std::list<GfxLib>::iterator it = Libs.begin(); it != Libs.end(); ++it)
    {
        if (&*it == lib)
        {
            Libs.erase(it);
            break;
        }
    }
    return 0;
}

GfxLib::GfxLib(void*, SDL_Renderer*, char* path, SLONG, SLONG, SLONG*)
{
    SDL_RWops* file = SDL_RWFromFile(path, "rb");
    if (file)
    {
        struct _GfxLibHeader* header = LoadHeader(file);
        if (header)
        {
            Load(file, header);
            delete header;
        }
        SDL_RWclose(file);
    }
}

GfxLibHeader* GfxLib::LoadHeader(SDL_RWops* file)
{
    if (!file)
        return NULL;

    char magic[5] = { '\0' };
    SDL_RWread(file, magic, 1, 4);
    if (strcmp(magic, "GLIB") != 0)
        return NULL;

    GfxLibHeader* header = new GfxLibHeader;
    header->Length = SDL_ReadLE32(file);
    if (SDL_RWread(file, &header->Unknown0, 1, header->Length - 4) != header->Length - 4)
    {
        delete header;
        return NULL;
    }
    return header;
}

SLONG GfxLib::Load(SDL_RWops* file, GfxLibHeader* header)
{
    if (!header)
        return -1;

    if (SDL_RWseek(file, header->Pos, RW_SEEK_SET) == -1)
        return -2;

    for (SLONG i = 0; i < header->Files; i++)
    {
        __int64 pos = SDL_RWtell(file);

        GfxChunkInfo info = { 0 };
        if (SDL_RWread(file, &info, sizeof(info), 1) != 1)
            return -3;

        GfxChunkHeader chunk = { 0 };
        switch (info.Type)
        {
        case CHUNK_GFX:
            if (SDL_RWread(file, &chunk, sizeof(chunk), 1) != 1)
                return -4;
            ReadGfxChunk(file, chunk, 0, 0);
            break;
        case CHUNK_NAME:
            //DebugBreak();
            break;
        case CHUNK_PALETTE:
            DebugBreak();
            break;
        }

        SDL_RWseek(file, pos + info.Size, RW_SEEK_SET);
    }

    return 0;
}

void ODS() {}

SLONG GfxLib::ReadGfxChunk(SDL_RWops* file, GfxChunkHeader header, SLONG, SLONG)
{
    SDL_RWseek(file, header.Offset, RW_SEEK_SET);

    GfxChunkImage image = { 0 };
    if (SDL_RWread(file, &image, sizeof(image), 1) != 1)
        return -1;

    word bpp = image.BitDepth / 8;
    char* pixels = new char[image.Size];
    SDL_RWread(file, pixels, 1, image.Size);
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
        pixels,
        image.Width,
        image.Height,
        image.BitDepth,
        image.Size / image.Height,
        image.Rmask,
        image.Gmask,
        image.Bmask,
        0);
    Surfaces[header.Id] = surface;
    return 0;
}

SDL_Surface* GfxLib::GetSurface(__int64 name)
{
    std::map<__int64, SDL_Surface*>::iterator it = Surfaces.find(name);
    if (it != Surfaces.end())
        return it->second;
    return NULL;
}

class GfxLib* GfxLib::ReleaseSurface(__int64 name)
{
    std::map<__int64, SDL_Surface*>::iterator it = Surfaces.find(name);
    if (it != Surfaces.end())
    {
        delete[](char*)it->second->pixels;
        SDL_FreeSurface(it->second);
        Surfaces.erase(it);
    }
    return this;
}

void GfxLib::Release()
{
    if(Surfaces.size() <= 0)
        return;

    for (std::map<__int64, SDL_Surface*>::iterator it = Surfaces.begin(); it != Surfaces.end(); ++it)
    {
        delete[](char*)it->second->pixels;
        SDL_FreeSurface(it->second);
    }
    Surfaces.clear();
}

SLONG GfxLib::Restore()
{
    return 0;
}

SLONG GfxLib::AddRef(__int64)
{
    return 0;
}