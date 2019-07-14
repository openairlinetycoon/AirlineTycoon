#pragma once

typedef UWORD word;
typedef ULONG dword;

#define DD_ERROR(x) (x)

#define CREATE_SYSMEM      0
#define CREATE_VIDMEM      1
#define CREATE_USECOLORKEY 2
#define CREATE_FULLSCREEN  4

class GfxLib
{
public:
    struct _GfxStruct* ReloadSurface(__int64);
    long Restore(void);
    void Release(void);
    class GfxLib* ReleaseSurface(long);
    class GfxLib* ReleaseSurface(__int64);
    struct _GfxStruct* GetSurface(__int64);
    struct _GfxStruct* GetSurface(long);
    long AddRef(__int64);
    long AddRef(long);
    __int64 LongName2Id(char*);
    char* Id2LongName(__int64);
    long GetGfxHeader(long, struct _GfxChunkInfo*);
    __int64 GetGfxShortId(long);

protected:
    GfxLib(void*, struct IDirectDraw*, char*, long, long, long*);
    ~GfxLib(void);
    void ErrorProc(long);

private:
    long CountGfxChunks(struct _uniChunk*, long);
    void* LoadHeader(int);
    void ReadPaletteChunk(int, struct _PaletteInfo);
    void ReadNameChunk(int, struct _LongNameChunk);
    void* DeCompData(void*, struct _GfxChunkInfo, long);
    void* ConvertData(void*, long, char*, long, long, long, long, long);
    struct IDirectDrawSurface* FillSurface(int, struct _GfxChunkInfo, char*, struct IDirectDrawSurface*);
    struct IDirectDrawSurface* ReadPixelData(int, struct _GfxChunkInfo, char*, long);
    struct IDirectDrawPalette* ReadPalette(int, struct _GfxChunkInfo);
    void* ReadZBuffer(int, struct _GfxChunkInfo);
    void* ReadAlphaBuffer(int, struct _GfxChunkInfo);
    long ReadGfxChunk(int, struct _GfxChunkHeader, long, long);
    long Load(long);
    long FindId(__int64);
    void RelSurface(long);
};

#define L_LOCMEM 0

class GfxMain
{
public:
    GfxMain(struct IDirectDraw*);
    ~GfxMain(void);
    long Restore(void);
    long LoadLib(char*, class GfxLib**, long);
    long ReleaseLib(class GfxLib*);
    void KillLib(class GfxLib*);
    long GetListId(class GfxLib*, __int64);
    struct _GfxStruct* GetSurface(long);
    long AddRef(long);
    class GfxLib* ReleaseSurface(long);

private:
    void ErrorProc(long);
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
	operator char*() { return Buffer; }
    class SB_CString const& operator=(class SB_CString const&);
    class SB_CString const& operator=(char const*);
    class SB_CString const& operator=(int b) { DebugBreak(); Empty(); *this += b; return *this; }
    class SB_CString const& operator=(char b) { DebugBreak(); Empty(); *this += b; return *this; }
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
    class SB_CString Mid(unsigned long) const;
    class SB_CString Mid(unsigned long, unsigned long) const;
    class SB_CString Right(unsigned long) const;
    class SB_CString Left(unsigned long) const;
    unsigned long ReverseFind(char) const;
    unsigned long Find(char const*) const;
    bool LoadStringA(unsigned int, struct HINSTANCE__*);
    bool LoadStringA(unsigned long, class SB_CDatabase*);
    void MakeUpper(void);
    void MakeLower(void);
    void Show(void) const;
	unsigned long Length() { return Anz; }

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
	unsigned long Anz;
	unsigned long Size;
	char* Buffer;
};

class SB_CHardwarecolorHelper;

class SB_Hardwarecolor
{
public:
    SB_Hardwarecolor() : color(0) { DebugBreak(); }
    SB_Hardwarecolor(const SB_CHardwarecolorHelper* helper) : color(reinterpret_cast<SLONG>(helper)) { DebugBreak(); }
    operator SB_CHardwarecolorHelper* () { return reinterpret_cast<SB_CHardwarecolorHelper*>(this); }
	operator word() { DebugBreak(); return 0; }

private:
    SLONG color;
};

class PixelFormat
{
    SLONG redMask, greenMask, blueMask;
};

class SB_CBitmapCore
{
public:
    unsigned long AddAlphaMsk(void);
    unsigned long AddZBuffer(unsigned long, unsigned long);
    class SB_CHardwarecolorHelper* GetHardwarecolor(unsigned long);
    unsigned long SetPixel(long, long, class SB_CHardwarecolorHelper*);
    unsigned long GetPixel(long, long);
    unsigned long Clear(class SB_CHardwarecolorHelper*, struct tagRECT const* = NULL);
    unsigned long Line(long, long, long, long, class SB_CHardwarecolorHelper*);
    unsigned long LineTo(long, long, class SB_CHardwarecolorHelper*);
    unsigned long Rectangle(struct tagRECT const*, class SB_CHardwarecolorHelper*);
    void InitClipRect(void);
    void SetClipRect(struct tagRECT const*);
    void SetColorKey(unsigned long);
    virtual unsigned long Release(void);
    unsigned long BlitFast(class SB_CBitmapCore*, long, long, struct tagRECT const* = NULL, unsigned short = 0);
    unsigned long BlitChar(struct IDirectDrawSurface*, long, long, struct tagRECT const* = NULL, unsigned short = 0);
    unsigned long Blit(class SB_CBitmapCore*, long, long, struct tagRECT const* = NULL, unsigned short = 0, unsigned long = 0);
    long BlitA(class SB_CBitmapCore*, long, long, struct tagRECT const*, class SB_CHardwarecolorHelper*);
    long BlitA(class SB_CBitmapCore*, long, long, struct tagRECT const*);
    long BlitAT(class SB_CBitmapCore*, long, long, struct tagRECT const*, class SB_CHardwarecolorHelper*);
    long BlitAT(class SB_CBitmapCore*, long, long, struct tagRECT const*);

    unsigned long BlitT(class SB_CBitmapCore* bm, long x, long y, struct tagRECT const* rect = NULL, short flags = 16, unsigned long unk = 0) { return Blit(bm, x, y, rect, flags, unk); }
    unsigned long SetPixel(long x, long y, SLONG color) { return SetPixel(x, y, (SB_CHardwarecolorHelper*)color); }
    unsigned long Line(long x1, long y1, long x2, long y2, DWORD color) { return Line(x1, y1, x2, y2, (SB_CHardwarecolorHelper*)color); }
    SLONG GetXSize() { return *((SLONG*)this + 16); }
    SLONG GetYSize() { return *((SLONG*)this + 17); }
    RECT GetClipRect() { return *(RECT*)((UBYTE*)this + 16); }
    LPDIRECTDRAWSURFACE GetSurface() { DebugBreak(); return *(LPDIRECTDRAWSURFACE*)((UBYTE*)this + 8); }
    PixelFormat* GetPixelFormat(void) { DebugBreak(); return NULL; }

protected:
    void FillPixelFormat(void);
    virtual long Lock(struct _DDSURFACEDESC*) const;
    virtual long Unlock(struct _DDSURFACEDESC*) const;

    ~SB_CBitmapCore(void);
};

class SB_CCursor
{
public:
    SB_CCursor(class SB_CPrimaryBitmap*, class SB_CBitmapCore* = NULL);
    ~SB_CCursor(void);
    long Create(class SB_CBitmapCore*);
    long SetImage(class SB_CBitmapCore*);
    long MoveImage(long, long);
    long FlipBegin(void);
    long FlipEnd(void);
    long Show(bool);

private:
    long BlitImage(enum tagEnumBlitImage, long, long);
    long RestoreBackground(enum tagEnumBlitImage, struct IDirectDrawSurface*, unsigned short);
    long SaveBackground(enum tagEnumBlitImage, struct IDirectDrawSurface*, unsigned short);
    long CreateBackground(void);
    long CreateSurface(struct IDirectDrawSurface**, long, long);
};

class SB_CPrimaryBitmap : public SB_CBitmapCore
{
public:
    SB_CPrimaryBitmap(void);
    ~SB_CPrimaryBitmap(void);

    long Create(struct IDirectDraw*, struct HWND__*, unsigned short, long, long, unsigned char, unsigned short);
    virtual unsigned long Release(void);
    long Flip(void);
    void SetPos(struct tagPOINT&);
    struct IDirectDrawSurface* GetLastPage(void);

    void AssignCursor(SB_CCursor*) { DebugBreak(); }
    LPDIRECTDRAWSURFACE GetPrimarySurface() { DebugBreak(); return NULL; }
    bool FastClip(struct tagRECT, struct tagPOINT const*, struct tagRECT const*) { DebugBreak(); return false; }

private:
    void Delete(void);
};

class SB_CBitmapMain
{
public:
    SB_CBitmapMain(struct IDirectDraw*);
    ~SB_CBitmapMain(void);
    unsigned long Release(void);
    unsigned long CreateBitmap(SB_CBitmapCore**, GfxLib*, __int64, unsigned long);
    unsigned long CreateBitmap(SB_CBitmapCore**, long, long, unsigned long, unsigned long = 0, unsigned long = 0);
    unsigned long ReleaseBitmap(SB_CBitmapCore*);
    unsigned long DelEntry(SB_CBitmapCore*);
};

class SB_CBitmapKey
{
public:
    SB_CBitmapKey(class SB_CBitmapCore&);
    ~SB_CBitmapKey(void);

    void* Bitmap;
    unsigned long lPitch;
};

typedef struct
{
    SB_CPrimaryBitmap* pBitmap;
    SB_CBitmapCore* pBitmapCore;
} CFRONTDATA;

#define TEC_FONT_LEFT      0
#define TEC_FONT_CENTERED  1
#define TEC_FONT_RIGHT     2

#define TAB_STYLE_LEFT     0
#define TAB_STYLE_CENTER   1
#define TAB_STYLE_RIGHT    2

typedef struct tagTabs
{
    dword Unknown[2];
} TABS;

typedef struct tagTextStyle
{
    dword Unknown[1];
} TEXT_STYLE;

class SB_CFont
{
public:
    SB_CFont(void);
    ~SB_CFont(void);
    void DrawTextA(class SB_CBitmapCore*, long, long, char*, long = 0, bool = false);
    void DrawTextWithTabs(class SB_CBitmapCore*, long, long, char*, long = 0, bool = false);
    long DrawTextBlock(class SB_CBitmapCore*, struct tagRECT*, char*, long = 0, long = 0, bool = false);
    long PreviewTextBlock(class SB_CBitmapCore*, struct tagRECT*, char*, long = 0, long = 0, bool = false);
    long GetWidthAt(char*, long, char);
    long GetWordLength(char*, long);
    long GetWidth(char*, long);
    long GetWidth(char);
    bool Load(struct IDirectDraw*, char*, struct HPALETTE__*);
    bool CopyMemToSurface(struct HPALETTE__*);
    void SetTabulator(struct tagTabs*, unsigned long);

    bool Load(struct IDirectDraw* pDD, char* fn)
    {
        DebugBreak();
        return Load(pDD, fn, NULL);
    }

    void SetLineSpace(long) { DebugBreak(); }

protected:
    void Init(void);
    void Delete(void);
    bool GetSurface(struct _DDSURFACEDESC*);
    void ReleaseSurface(struct _DDSURFACEDESC*);
    bool DrawChar(char, bool);
    bool DrawWord(char*, long);
    unsigned char* GetDataPtr(void);
    bool CreateFontSurface(struct IDirectDraw*);
    bool CopyBitmapToMem(struct tagCreateFont*);
};

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
};
