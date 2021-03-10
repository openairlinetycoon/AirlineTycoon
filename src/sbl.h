#pragma once

#include <list>
#include <map>

typedef unsigned short word;
typedef unsigned int dword;

// Can you spot the bug? (x is executed two more times just to get the error codes)
// Bonus points if you spot that FAILED() should've been used to check the HRESULT.
#define DD_ERROR(x) if (!(x)) ODS("DDError in File: %s Line: %d Code: %d [%x]",__FILE__,__LINE__,x,x);

extern void ODS(const char*, ...);
extern SLONG GetLowestSetBit(SLONG mask);
extern SLONG GetHighestSetBit(SLONG mask);

#define CREATE_SYSMEM      0
#define CREATE_VIDMEM      1
#define CREATE_USECOLORKEY 2
#define CREATE_USEZBUFFER  4
#define CREATE_USEALPHA    8
#define CREATE_FULLSCREEN  16
#define CREATE_INDEXED     32

class GfxLib
{
public:
    struct _GfxStruct* ReloadSurface(__int64);
    SLONG Restore(void);
    void Release(void);
    class GfxLib* ReleaseSurface(SLONG);
    class GfxLib* ReleaseSurface(__int64);
    SDL_Surface* GetSurface(__int64);
    SDL_Surface* GetSurface(SLONG);
    SLONG AddRef(__int64);
    SLONG AddRef(SLONG);
    __int64 LongName2Id(char*);
    char* Id2LongName(__int64);
    SLONG GetGfxHeader(SLONG, struct _GfxChunkInfo*);
    __int64 GetGfxShortId(SLONG);

protected:
    GfxLib(void*, SDL_Renderer*, char*, SLONG, SLONG, SLONG*);
    void ErrorProc(SLONG);

    friend class GfxMain;

private:
    SLONG CountGfxChunks(struct _uniChunk*, SLONG);
    struct _GfxLibHeader* LoadHeader(SDL_RWops*);
    void ReadPaletteChunk(int, struct _PaletteInfo);
    void ReadNameChunk(int, struct _LongNameChunk);
    void* DeCompData(void*, struct _GfxChunkInfo, SLONG);
    void* ConvertData(void*, SLONG, char*, SLONG, SLONG, SLONG, SLONG, SLONG);
    struct IDirectDrawSurface* FillSurface(int, struct _GfxChunkInfo, char*, struct IDirectDrawSurface*);
    struct IDirectDrawSurface* ReadPixelData(int, struct _GfxChunkInfo, char*, SLONG);
    struct IDirectDrawPalette* ReadPalette(int, struct _GfxChunkInfo);
    void* ReadZBuffer(SDL_RWops*, struct _GfxChunkInfo);
    void* ReadAlphaBuffer(SDL_RWops*, struct _GfxChunkInfo);
    SLONG ReadGfxChunk(SDL_RWops*, struct _GfxChunkHeader, SLONG, SLONG);
    SLONG Load(SDL_RWops*, struct _GfxLibHeader*);
    SLONG FindId(__int64);
    void RelSurface(SLONG);

    std::map<__int64, SDL_Surface*> Surfaces;
};

#define L_LOCMEM 0

class GfxMain
{
public:
    GfxMain(SDL_Renderer*);
    ~GfxMain(void);
    SLONG Restore(void);
    SLONG LoadLib(char*, class GfxLib**, SLONG);
    SLONG ReleaseLib(class GfxLib*);
    void KillLib(class GfxLib*);
    SLONG GetListId(class GfxLib*, __int64);
    struct _GfxStruct* GetSurface(SLONG);
    SLONG AddRef(SLONG);
    class GfxLib* ReleaseSurface(SLONG);

private:
    void ErrorProc(SLONG);

    std::list<GfxLib> Libs;
};

class SB_CString
{
public:
    SB_CString(void);
    SB_CString(class SB_CString const&);
    SB_CString(int, int);
    SB_CString(char*);
    ~SB_CString(void);
    int operator==(class SB_CString const&);
    operator char* () { return Buffer; }
    class SB_CString const& operator=(class SB_CString const&);
    class SB_CString const& operator=(char const*);
    class SB_CString const& operator=(int b) { Empty(); *this += b; return *this; }
    class SB_CString const& operator=(char b) { Empty(); *this += b; return *this; }
    class SB_CString const& operator+=(char const*);
    class SB_CString const& operator+=(char);
    class SB_CString const& operator+=(class SB_CString const&);
    class SB_CString const& operator+=(int);
    class SB_CString& AttachChar(char);
    class SB_CString& DetachChar(char);
    class SB_CString& AddSlash(void);
    class SB_CString& RemoveSlash(void);
    void SplitPath(class SB_CString*, class SB_CString*, class SB_CString*, class SB_CString*);
    class SB_CString& MakePath(char*, char*, char*, char*);
    class SB_CString Mid(ULONG) const;
    class SB_CString Mid(ULONG, ULONG) const;
    class SB_CString Right(ULONG) const;
    class SB_CString Left(ULONG) const;
    ULONG ReverseFind(char) const;
    ULONG Find(char const*) const;
    bool LoadStringA(unsigned int, struct HINSTANCE__*);
    bool LoadStringA(ULONG, class SB_CDatabase*);
    void MakeUpper(void);
    void MakeLower(void);
    void Show(void) const;
    ULONG Length() { return Anz; }

    static SB_CString& Format(char const*, ...);

protected:
    void AllocCopy(class SB_CString&, unsigned int, unsigned int, unsigned int) const;
    void AllocBuffer(unsigned int);
    void ConcatCopy(unsigned int, char const*, unsigned int, char const*);
    void ConcatInPlace(int, char const*);
    void AssignCopy(unsigned int, char const*);
    void Init(void);
    void Empty(void);
    void SafeDelete(char*);

private:
    ULONG Anz;
    ULONG Size;
    char* Buffer;
};

struct SB_Hardwarecolor
{
    word Color;

    SB_Hardwarecolor(word c = 0) : Color(c) {}
    operator word() { return Color; }
};

class SB_CBitmapCore
{
public:
    ULONG AddAlphaMsk(void);
    ULONG AddZBuffer(ULONG, ULONG);
    SB_Hardwarecolor GetHardwarecolor(ULONG);
    ULONG SetPixel(SLONG, SLONG, SB_Hardwarecolor);
    ULONG GetPixel(SLONG, SLONG);
    ULONG Clear(SB_Hardwarecolor, struct tagRECT const* = NULL);
    ULONG Line(SLONG, SLONG, SLONG, SLONG, SB_Hardwarecolor);
    ULONG LineTo(SLONG, SLONG, SB_Hardwarecolor);
    ULONG Rectangle(const RECT*, SB_Hardwarecolor);
    void InitClipRect(void);
    void SetClipRect(const RECT*);
    void SetClipRect(const CRect&);
    void SetColorKey(ULONG);
    virtual ULONG Release(void);
    ULONG BlitFast(class SB_CBitmapCore*, SLONG, SLONG, const RECT* = NULL, unsigned short = 0);
    ULONG BlitChar(SDL_Surface*, SLONG, SLONG, const SDL_Rect* = NULL, unsigned short = 0);
    ULONG Blit(class SB_CBitmapCore*, SLONG, SLONG, const RECT* = NULL, unsigned short = 0, ULONG = 0);
    SLONG BlitA(class SB_CBitmapCore*, SLONG, SLONG, const RECT*, SB_Hardwarecolor);
    SLONG BlitA(class SB_CBitmapCore*, SLONG, SLONG, const RECT*);
    SLONG BlitAT(class SB_CBitmapCore*, SLONG, SLONG, const RECT*, SB_Hardwarecolor);
    SLONG BlitAT(class SB_CBitmapCore*, SLONG, SLONG, const RECT*);

    ULONG BlitT(class SB_CBitmapCore* bm, SLONG x, SLONG y, const RECT* rect = NULL, short flags = 16, ULONG unk = 0) { return Blit(bm, x, y, rect, flags, unk); }
    ULONG SetPixel(SLONG x, SLONG y, SLONG color) { return SetPixel(x, y, GetHardwarecolor(color)); }
    ULONG Line(SLONG x1, SLONG y1, SLONG x2, SLONG y2, DWORD color) { return Line(x1, y1, x2, y2, GetHardwarecolor(color)); }
    SLONG GetXSize() { return Size.x; }
    SLONG GetYSize() { return Size.y; }
    RECT GetClipRect() { const SDL_Rect& r = lpDDSurface->clip_rect; return CRect(r.x, r.y, r.x + r.w, r.y + r.h); }
    SDL_Surface* GetSurface() { return lpDDSurface; }
    SDL_PixelFormat* GetPixelFormat(void) { return lpDDSurface->format; }
    SDL_Texture* GetTexture() { return lpTexture; }

protected:
    friend class SB_CBitmapMain;
    friend class SB_CBitmapKey;

    SDL_Renderer* lpDD;
    SDL_Surface* lpDDSurface;
    SDL_Texture* lpTexture;
    XY Size;
};

//static_assert(sizeof(SB_CBitmapCore) == 0x5Cu, "SB_CBitmapCore size check");

class SB_CCursor
{
public:
    SB_CCursor(class SB_CPrimaryBitmap*, class SB_CBitmapCore* = NULL);
    ~SB_CCursor(void);
    SLONG Create(class SB_CBitmapCore*);
    SLONG SetImage(class SB_CBitmapCore*);
    SLONG MoveImage(SLONG, SLONG);
    SLONG FlipBegin();
    SLONG FlipEnd();
    SLONG Show(bool);
    SLONG Render(SDL_Renderer*);

private:
    SLONG BlitImage(SLONG, SLONG);
    SLONG RestoreBackground(struct SDL_Surface*);
    SLONG SaveBackground(struct SDL_Surface*);
    SLONG CreateBackground(void);
    SLONG CreateSurface(struct SDL_Surface**, SLONG, SLONG);

    SB_CPrimaryBitmap* Primary;
    SB_CBitmapCore* Cursor;
    SDL_Surface* Background;
    XY Position;
    dword Unknown[22];
};

//static_assert(sizeof(SB_CCursor) == 0x6Cu, "SB_CCursor size check");

class SB_CPrimaryBitmap : public SB_CBitmapCore
{
public:
    SB_CPrimaryBitmap(void);
    ~SB_CPrimaryBitmap(void);

    SLONG Create(SDL_Renderer**, SDL_Window*, unsigned short, SLONG, SLONG, unsigned char, unsigned short);
    virtual ULONG Release(void);
    SLONG Flip(void);
    SLONG Present(void);
    void SetPos(POINT);

    void AssignCursor(SB_CCursor* c) { Cursor = c; }
    SDL_Window* GetPrimarySurface() { return Window; }
    bool FastClip(CRect clipRect, POINT* pPoint, RECT* pRect);

private:
    void Delete(void);

    SDL_Window* Window;
    dword Unknown[9];
    SB_CCursor* Cursor;
};

//static_assert(sizeof(SB_CPrimaryBitmap) == 0x88u, "SB_CPrimaryBitmap size check");

class SB_CBitmapMain
{
public:
    SB_CBitmapMain(SDL_Renderer*);
    ~SB_CBitmapMain(void);
    ULONG Release(void);
    ULONG CreateBitmap(SB_CBitmapCore**, GfxLib*, __int64, ULONG);
    ULONG CreateBitmap(SB_CBitmapCore**, SLONG, SLONG, ULONG, ULONG = 16, ULONG = 0);
    ULONG ReleaseBitmap(SB_CBitmapCore*);
    ULONG DelEntry(SB_CBitmapCore*);

private:
    SDL_Renderer* Renderer;
    std::list<SB_CBitmapCore> Bitmaps;
    dword Unknown[3];
};

////static_assert(sizeof(SB_CBitmapMain) == 0x1Cu, "SB_CBitmapMain size check");

class SB_CBitmapKey
{
public:
    SB_CBitmapKey(class SB_CBitmapCore&);
    ~SB_CBitmapKey(void);

    SDL_Surface* Surface;
    dword Unknown[27];
    void* Bitmap;
    int lPitch;
};

//static_assert(sizeof(SB_CBitmapKey) == 0x78u, "SB_CBitmapKey size check");

typedef struct
{
    SB_CPrimaryBitmap* pBitmap;
    SB_CBitmapCore* pBitmapCore;
} CFRONTDATA;

#define TEC_FONT_LEFT      1
#define TEC_FONT_RIGHT     2
#define TEC_FONT_CENTERED  3

#define TAB_STYLE_LEFT     1
#define TAB_STYLE_DOT      2
#define TAB_STYLE_CENTER   3
#define TAB_STYLE_RIGHT    4

typedef struct tagTabs
{
    dword Style;
    dword Width;
} TABS;

//static_assert(sizeof(TABS) == 8, "TABS size check");

typedef struct tagTextStyle
{
    dword Unknown[3];
} TEXT_STYLE;

//static_assert(sizeof(TEXT_STYLE) == 12, "TEXT_STYLE size check");

class SB_CFont
{
public:
#pragma pack(push)
#pragma pack(1)
    struct FontHeader
    {
        word HeaderSize;
        word Unknown0;
        word Flags;
        word Width;
        word Height;
        SLONG Unknown1;
        SLONG BitDepth;
        word NumColors;
        word Unknown2;
        word LoChar;
        word HiChar;
        word Unknown3;
        SLONG szPixels;
        SLONG szColors;
        SLONG Unknown4;
        SLONG szFooter;
    };
#pragma pack(pop)

    SB_CFont(void);
    ~SB_CFont(void);
    void DrawTextA(class SB_CBitmapCore*, SLONG, SLONG, const char*, SLONG = 0, bool = false);
    void DrawTextWithTabs(class SB_CBitmapCore*, SLONG, SLONG, const char*, SLONG = 0, bool = false);
    SLONG DrawTextBlock(class SB_CBitmapCore*, struct tagRECT*, const char*, SLONG = 0, SLONG = 0, bool = false);
    SLONG PreviewTextBlock(class SB_CBitmapCore*, struct tagRECT*, const char*, SLONG = 0, SLONG = 0, bool = false);
    SLONG DrawTextBlock(class SB_CBitmapCore*, SLONG, SLONG, SLONG, SLONG, const char*, SLONG = 0, SLONG = 0, bool = false);
    SLONG PreviewTextBlock(class SB_CBitmapCore*, SLONG, SLONG, SLONG, SLONG, const char*, SLONG = 0, SLONG = 0, bool = false);
    SLONG GetWidthAt(const char*, SLONG, char);
    SLONG GetWordLength(const char*, SLONG);
    SLONG GetWidth(const char*, SLONG);
    SLONG GetWidth(unsigned char);
    bool Load(SDL_Renderer*, const char*, struct HPALETTE__* = NULL);
    bool CopyMemToSurface(struct HPALETTE__*);
    void SetTabulator(struct tagTabs*, ULONG);

    void SetLineSpace(float LineSpace) { this->LineSpace = LineSpace; }

protected:
    void Init(void);
    void Delete(void);
    bool GetSurface(struct _DDSURFACEDESC*);
    void ReleaseSurface(struct _DDSURFACEDESC*);
    bool DrawChar(unsigned char, bool);
    bool DrawWord(const char*, SLONG);
    unsigned char* GetDataPtr(void);
    bool CreateFontSurface(SDL_Renderer*);
    bool CopyBitmapToMem(struct tagCreateFont*);

private:
    FontHeader Header;
    dword Unknown0[4];
    SDL_Surface* Surface;
    SDL_Texture* Texture;
    BYTE* VarWidth;
    BYTE* VarHeight;
    bool Hidden;
    TABS* Tabulator;
    word NumTabs;
    XY Pos;
    XY Start;
    float LineSpace;
    SB_CBitmapCore* Bitmap;
};

//static_assert(sizeof(SB_CFont) == 0x70u, "SB_CFont size check");

class SB_CXList
{
public:
    SB_CXList(void);
    ~SB_CXList(void);
    unsigned short AddRef(void);
    unsigned short Release(void);
    unsigned short AddElementAtTop(void*);
    unsigned short AddElementAtBottom(void*);
    unsigned short AddElement(void*, unsigned short);
    bool DeleteElementAtTop(void);
    bool DeleteElementAtBottom(void);
    bool DeleteElement(unsigned short);
    void DeleteAllElements(void);
    bool DeleteElementAtValue(void*);
    bool Swap(unsigned short, unsigned short);
    bool SetElementOneBack(unsigned short);
    bool SetElementBack(unsigned short);
    bool SetElementOneAhead(unsigned short);
    bool SetElementAhead(unsigned short);

private:
    void Init(void);
    void Delete(void);

    dword Unknown[4];
};

//static_assert(sizeof(SB_CXList) == 0x10u, "SB_CXList size check");