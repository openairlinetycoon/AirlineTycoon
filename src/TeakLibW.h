#pragma once

extern void memswap(void* param_1, void* param_2, ULONG param_3);
extern char* bprintf(char const*, ...);
extern char* bitoa(long, long = 10);
extern void here(char*, long);
extern void ODS(const char *, ...);

extern const char* ExcAssert;
extern const char* ExcGuardian;
extern const char* ExcImpossible;
extern const char* ExcNotImplemented;
extern const char* ExcOutOfMem;
extern const char* ExcStrangeMem;

#define FNL 0, 0

extern long TeakLibW_Exception(char* param_1, long param_2, const char* param_3, ...);
extern char* TeakStrRemoveEndingCodes(char*, char const*);
extern unsigned char GerToLower(unsigned char);
extern unsigned char GerToUpper(unsigned char);
extern unsigned char* RecapizalizeString(unsigned char*);

template <typename T>
class BUFFER
{
public:
    BUFFER(SLONG anz)
    {
        if (anz == 0)
        {
            MemPointer = DelPointer = NULL;
        }
        else
        {
            MemPointer = DelPointer = new T[anz]();
            if (!MemPointer)
                TeakLibW_Exception(NULL, 0, ExcOutOfMem);
        }

        Size = anz;
    }

    BUFFER(BUFFER& rhs)
    {
        ::Swap(MemPointer, rhs.MemPointer);
        ::Swap(DelPointer, rhs.DelPointer);
        ::Swap(Size, rhs.Size);
    }

    BUFFER(void) : MemPointer(NULL), DelPointer(NULL), Size(0) {}

    ~BUFFER()
    {
        if (MemPointer)
            delete[] MemPointer;
        MemPointer = NULL;
    }

    void ReSize(SLONG anz)
    {
        if (anz == Size)
            return;

        if (anz > 0)
        {
            T* m = new T[anz]();
            if (!m)
                TeakLibW_Exception(NULL, 0, ExcOutOfMem);

            if (MemPointer)
            {
                SLONG num;
                if (anz < Size)
                    num = anz;
                else
                    num = Size;

                // This is *will* break self-referencing pointers
                // ... please don't resize anything that uses ALBUM
                memswap(m, MemPointer, sizeof(T) * num);
                delete[] MemPointer;

                DelPointer = m + ((DelPointer - MemPointer) / sizeof(T));
            }
            else
            {
                DelPointer = m;
            }
            MemPointer = m;
        }
        else
        {
            if (MemPointer)
                delete[] MemPointer;

            MemPointer = DelPointer = NULL;
        }

        Size = anz;
    }

    void ReSize(SLONG anz, T* memory)
    {
        if (MemPointer)
            delete[] MemPointer;

        DelPointer = &memory[DelPointer - MemPointer];
        Size = anz;
        MemPointer = memory;
    }

    long AnzEntries() const { return Size; }

    void FillWith(T value)
    {
        for (int i = 0; i < Size; i++)
            MemPointer[i] = value;
    }

    operator T* () const
    {
        return DelPointer;
    }

    void operator+=(int rhs)
    {
        DelPointer += rhs;
    }

    void operator=(BUFFER<T>& rhs)
    {
        ::Swap(MemPointer, rhs.MemPointer);
        ::Swap(DelPointer, rhs.DelPointer);
        ::Swap(Size, rhs.Size);
    }

    friend class TEAKFILE& operator << (TEAKFILE& File, const BUFFER<T>& buffer)
    {
        File << buffer.Size;
        File << (buffer.DelPointer - buffer.MemPointer);
        for (SLONG i = 0; i < buffer.Size; i++)
            File << buffer.MemPointer[i];
        return File;
    }

    friend class TEAKFILE& operator >> (TEAKFILE& File, BUFFER<T>& buffer)
    {
        SLONG size, offset;
        File >> size;
        buffer.ReSize(0);
        buffer.ReSize(size);
        File >> offset;
        for (SLONG i = 0; i < buffer.Size; i++)
            File >> buffer.MemPointer[i];
        buffer.DelPointer = buffer.MemPointer + offset;
        return File;
    }

    T* MemPointer;
    T* DelPointer;
    SLONG Size;
};

#define TEAKFILE_READ	1
#define TEAKFILE_WRITE	2

class TEAKFILE
{
public:
    TEAKFILE(void);
    TEAKFILE(char const*, long);
    ~TEAKFILE(void);

    void ReadLine(char*, long);
    int IsEof(void);
    void Close(void);
    long GetFileLength(void);
    long GetPosition(void);
    void Open(char const*, long);
    int IsOpen(void);
    unsigned char* Read(long);
    void Read(unsigned char*, long);
    char* ReadLine(void);
    void ReadTrap(long);
    void WriteTrap(long);
    void SetPasswort(char*);
    void SetPosition(long);
    void Skip(long);
    void Write(unsigned char*, long);
    void WriteLine(char*);
    void Announce(long);

    SLONG Unknown[12];
    BUFFER<UBYTE> MemBuffer;
    void* MemPointer;
    ULONG MemBufferUsed;

    friend TEAKFILE& operator << (TEAKFILE& File, const BOOL& b) { File.Write((UBYTE*)& b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, BOOL& b) { File.Read((UBYTE*)& b, sizeof(b)); return File; }

    friend TEAKFILE& operator << (TEAKFILE& File, const UBYTE& b) { File.Write((UBYTE*)& b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, UBYTE& b) { File.Read((UBYTE*)& b, sizeof(b)); return File; }

    friend TEAKFILE& operator << (TEAKFILE& File, const SBYTE& b) { File.Write((UBYTE*)& b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, SBYTE& b) { File.Read((UBYTE*)& b, sizeof(b)); return File; }

    friend TEAKFILE& operator << (TEAKFILE& File, const UWORD& b) { File.Write((UBYTE*)& b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, UWORD& b) { File.Read((UBYTE*)& b, sizeof(b)); return File; }

    friend TEAKFILE& operator << (TEAKFILE& File, const SWORD& b) { File.Write((UBYTE*)& b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, SWORD& b) { File.Read((UBYTE*)& b, sizeof(b)); return File; }

    friend TEAKFILE& operator << (TEAKFILE& File, const ULONG& b) { File.Write((UBYTE*)& b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, ULONG& b) { File.Read((UBYTE*)& b, sizeof(b)); return File; }

    friend TEAKFILE& operator << (TEAKFILE& File, const SLONG& b) { File.Write((UBYTE*)& b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, SLONG& b) { File.Read((UBYTE*)& b, sizeof(b)); return File; }

    friend TEAKFILE& operator << (TEAKFILE& File, const __int64& b) { File.Write((UBYTE*)& b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, __int64& b) { File.Read((UBYTE*)& b, sizeof(b)); return File; }

    friend TEAKFILE& operator << (TEAKFILE& File, const double& b) { File.Write((UBYTE*)& b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, double& b) { File.Read((UBYTE*)& b, sizeof(b)); return File; }

    friend TEAKFILE& operator << (TEAKFILE& File, const float& b) { File.Write((UBYTE*)& b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, float& b) { File.Read((UBYTE*)& b, sizeof(b)); return File; }

    friend TEAKFILE& operator << (TEAKFILE& File, const CPoint& b) { File.Write((UBYTE*)& b, sizeof(CPoint)); return (File); }
    friend TEAKFILE& operator >> (TEAKFILE& File, CPoint& b) { File.Read((UBYTE*)& b, sizeof(CPoint)); return (File); }

    friend TEAKFILE& operator << (TEAKFILE& File, const CString& b)
    {
        File << b.GetLength() + 1;
        File.Write((UBYTE*)(LPCTSTR)b, b.GetLength() + 1);
        return File;
    }
    friend TEAKFILE& operator >> (TEAKFILE& File, CString& b)
    {
        ULONG size;
        File >> size;
        BUFFER<BYTE> str(size);
        File.Read(str, size);
        b = str;
        return File;
    }

private:
    void CodeBlock(unsigned char*, long, long);
};

static_assert<sizeof(TEAKFILE) == 68> TEAKFILE_size_check;

template <typename T>
class FBUFFER : public BUFFER<T>
{
public:
    FBUFFER(void) : BUFFER<T>(0) {}

    FBUFFER(FBUFFER& buffer) : BUFFER<T>(buffer) {}

    FBUFFER(SLONG anz) : BUFFER<T>(anz) {}
};

class TEAKRAND
{
public:
    TEAKRAND(void);
    TEAKRAND(ULONG _Seed);

    void SRand(ULONG _Seed);
    void SRandTime(void);
    void Reset(void);

    UWORD Rand(void);
    UWORD Rand(SLONG Max);
    UWORD Rand(SLONG Min, SLONG Max);
    ULONG GetSeed(void);

    friend TEAKFILE& operator << (TEAKFILE& File, const TEAKRAND& r);
    friend TEAKFILE& operator >> (TEAKFILE& File, TEAKRAND& r);

private:
    ULONG Seed;
    ULONG Value;
};

template <typename T>
class TXY
{
public:
    T x, y;

    TXY() : x(), y() {}
    TXY(T s) : x(s), y(s) {}
    TXY(T x, T y) : x(x), y(y) {}
    TXY(POINT& point) : x(point.x), y(point.y) {}

    TXY operator+(const TXY& b) const
    {
        return TXY(x + b.x, y + b.y);
    }

    TXY operator-(const TXY& b) const
    {
        return TXY(x - b.x, y - b.y);
    }

    TXY operator*(const TXY& b) const
    {
        return TXY(x * b.x, y * b.y);
    }

    TXY operator/(const TXY& b) const
    {
        return TXY(x / b.x, y / b.y);
    }

    TXY operator*(const T& b) const
    {
        return TXY(x * b, y * b);
    }

    TXY operator/(const T& b) const
    {
        return TXY(x / b, y / b);
    }

    TXY operator-() const
    {
        return TXY(-x, -y);
    }

    bool operator==(const TXY& b) const
    {
        return x == b.x
            && y == b.y;
    }

    bool operator!=(const TXY& b) const
    {
        return x != b.x
            || y != b.y;
    }

    bool operator<(const TXY& b) const
    {
        return x < b.x
            && y < b.y;
    }

    bool operator>(const TXY& b) const
    {
        return x > b.x
            && y > b.y;
    }

    TXY& operator-=(const TXY& b)
    {
        x -= b.x;
        y -= b.y;
        return *this;
    }

    TXY& operator+=(const TXY& b)
    {
        x += b.x;
        y += b.y;
        return *this;
    }

    TXY& operator/=(const TXY& b)
    {
        x /= b.x;
        y /= b.y;
        return *this;
    }

    TXY& operator*=(const TXY& b)
    {
        x *= b.x;
        y *= b.y;
        return *this;
    }

    operator POINT& ()
    {
        return reinterpret_cast<POINT&>(*this);
    }

    operator CPoint& ()
    {
        return reinterpret_cast<CPoint&>(*this);
    }

    double abs() const
    {
        return sqrt(x * x + y * y);
    }

    double operator*(const double& b) const
    {
        return (x + y) * b;
    }

    double operator/(const double& b) const
    {
        return (x + y) / b;
    }

    bool IfIsWithin(T _x1, T _y1, T _x2, T _y2) const
    {
        return x >= _x1 && y >= _y1 && x <= _x2 && y <= _y2;
    }

    bool IfIsWithin(const TXY<T>& a, const TXY<T>& b) const
    {
        return a < *this && *this < b;
    }

    friend TEAKFILE& operator << (TEAKFILE& File, const TXY<T>& b) { File.Write((UBYTE*)& b, sizeof(T) * 2); return (File); }
    friend TEAKFILE& operator >> (TEAKFILE& File, TXY<T>& b) { File.Read((UBYTE*)& b, sizeof(T) * 2); return (File); }
};

template <typename T>
class TXYZ
{
public:
    T x, y, z;

    TXYZ() : x(), y(), z() {}
    TXYZ(T s) : x(s), y(s), z(s) {}
    TXYZ(T x, T y, T z) : x(x), y(y), z(z) {}

    TXYZ operator+(const TXYZ& b) const
    {
        return TXY(x + b.x, y + b.y, z + b.z);
    }

    TXYZ operator-(const TXYZ& b) const
    {
        return TXY(x - b.x, y - b.y, z - b.z);
    }

    TXYZ operator*(const TXYZ& b) const
    {
        return TXY(x * b.x, y * b.y, z * b.z);
    }

    TXYZ operator/(const TXYZ& b) const
    {
        return TXY(x / b.x, y / b.y, z / b.z);
    }

    TXYZ operator*(const T& b) const
    {
        return TXY(x * b, y * b, z * b);
    }

    TXYZ operator/(const T& b) const
    {
        return TXY(x / b, y / b, z / b);
    }

    TXYZ operator-() const
    {
        return TXY(-x, -y, -z);
    }

    bool operator==(const TXYZ& b) const
    {
        return x == b.x
            && y == b.y
            && z == b.z;
    }

    bool operator!=(const TXYZ& b) const
    {
        return x != b.x
            || y != b.y
            || z != b.z;
    }

    bool operator<(const TXYZ& b) const
    {
        return x < b.x
            && y < b.y
            && z < b.z;
    }

    bool operator>(const TXYZ& b) const
    {
        return x > b.x
            && y > b.y
            && z > b.z;
    }

    TXYZ& operator-=(const TXYZ& b)
    {
        x -= b.x;
        y -= b.y;
        z -= b.z;
        return *this;
    }

    TXYZ& operator+=(const TXYZ& b)
    {
        x += b.x;
        y += b.y;
        z += b.z;
        return *this;
    }

    TXYZ& operator/=(const TXYZ& b)
    {
        x /= b.x;
        y /= b.y;
        z /= b.z;
        return *this;
    }

    TXYZ& operator*=(const TXYZ& b)
    {
        x *= b.x;
        y *= b.y;
        z *= b.z;
        return *this;
    }

    double abs() const
    {
        return sqrt(x * x + y * y + z * z);
    }

    double operator*(const double& b) const
    {
        return (x + y + z) * b;
    }

    double operator/(const double& b) const
    {
        return (x + y + z) / b;
    }

    friend TEAKFILE& operator << (TEAKFILE& File, const TXYZ<T>& b) { File.Write((UBYTE*)& b, sizeof(T) * 3); return (File); }
    friend TEAKFILE& operator >> (TEAKFILE& File, TXYZ<T>& b) { File.Read((UBYTE*)& b, sizeof(T) * 3); return (File); }
};

typedef TXY<SLONG> XY;
typedef TXYZ<SLONG> XYZ;

typedef TXY<FLOAT> FXY;
typedef TXYZ<FLOAT> FXYZ;

class PALETTE
{
public:
    PALETTE(void);
    PALETTE(IDirectDraw*, CString const&);

    void Blend(long, long) const;
    void BlendIn(void) const;
    void BlendOut(void) const;
    void RefreshDD(int) const;
    void RefreshDD(long, int) const;
    void RefreshPal(IDirectDraw*, CString const&);
    void RefreshPalFromLbm(CString const&);
    void RefreshPalFromPcx(CString const&);
    void ConvertToBlackWhite(void);
    unsigned char FindColorClosestToRGB(unsigned char, unsigned char, unsigned char) const;
    void RotateArea(long, long);
    void CopyArea(long, long, long);
    void CopyAreaFrom(PALETTE const&, long, long, long);

    BUFFER<PALETTEENTRY> Pal;
    SLONG Unknown;
};

static_assert<sizeof(PALETTE) == 16> PALETTE_size_check;

struct TEXTRES_CACHE_ENTRY
{
    SLONG Group, Id;
    const char* Text;
};

static_assert<sizeof(TEXTRES_CACHE_ENTRY) == 12> TEXTRES_CACHE_ENTRY_size_check;

#define TEXTRES_CACHED (void*)1

class TEXTRES
{
public:
    TEXTRES() { memset(Unknown, 0, sizeof(Unknown)); };
    TEXTRES(char const*, void*);
    ~TEXTRES(void);

    void Open(char const*, void*);
    BUFFER<char>& GetB(unsigned long, unsigned long);
    char* GetP(unsigned long, unsigned long);
    char* GetS(unsigned long, unsigned long);
    char* GetS(unsigned long, char const*);
    char* GetS(char const* c, unsigned long i) { return GetS(*(unsigned long*)c, i); }

private:
    SLONG Unknown[6];
    BUFFER<TEXTRES_CACHE_ENTRY> Entries;
};

static_assert<sizeof(TEXTRES) == 36> TEXTRES_size_check;

class CRegistration
{
public:
    CRegistration(void);
    CRegistration(CString const&, unsigned long);
    void ReSize(CString const&, unsigned long);
    CString GetDisplayString(void);
    long GetMode(void);
    CString GetSomeString(char*);
    unsigned long CalcChecksum(CString);
    int IsMaster(void);
    void CheckIfIsMaster(void);
};

#define VIDRAMBM (void*)1
#define SYSRAMBM (void*)2

class TECBM
{
public:
    TECBM(void);
    TECBM(CString const&, void*);
    TECBM(CString const&, long, void*);
    TECBM(long, long, void*);
    TECBM(TXY<long>, void*);
    ~TECBM(void);

    void Destroy(void);
    TECBM& operator=(TECBM&);
    virtual int Refresh(void);
    int TextOutA(long, long, unsigned long, unsigned long, CString const&);
    int IsOk(void) const;
    static int IsMemCritical(void);
    static int IsHardwareCritical(void);
    static int IsEitherCritical(void);
    static void SetCriticalMem(long);
    void ReSize(CString const&, void*);
    void ReSize(CString const&, long, void*);
    void ReSizeLbm(CString const&, void*);
    void ReSizeLbm(CString const&, long, void*);
    void ReSize(TXY<long>, void*);
    void ReSizePcx(CString const&, long, void*);
    void ReSizePcx(CString const&, void*);
    int SavePCX(CString const&, PALETTE const&) const;
    void ShiftColors(long);
    void RemapColor(unsigned char, unsigned char);
    int BlitFrom(TECBM const&, TXY<long>, TXY<long>);
    int BlitFromT(TECBM const&, TXY<long>, TXY<long>, unsigned char);
    void InterleaveBitmaps(TECBM const&, TECBM const&, long);
    int UniversalClip(TXY<long>*, CRect*);
    void GetClipRegion(TXY<long>*, TXY<long>*);
    void SetClipRegion(void);
    void SetClipRegion(TXY<long>, TXY<long>);
    int IsLost(void) const;
    int SetPixel(TXY<long>, unsigned char);
    unsigned char GetPixel(TXY<long>) const;
    int FillWith(unsigned char);
    int FillWith(TECBM const&);
    int FillWith(TECBM const&, TXY<long>);
    int Line(TXY<long> const&, TXY<long> const&, unsigned char);
    int HLine(long, long, long, unsigned char);
    int VLine(long, long, long, unsigned char);
    int DotLine(TXY<long> const&, TXY<long> const&, unsigned char);
    int Rectangle(TXY<long> const&, TXY<long> const&, unsigned char);
    int Box(TXY<long>, TXY<long>, unsigned char);
    int Box(TXY<long> const&, TXY<long> const&, TECBM const&);
    int Box(TXY<long> const&, TXY<long> const&, TECBM const&, TXY<long>);
    int Circle(TXY<long> const&, long, unsigned char);
    int BlitFrom(TECBM&, TXY<long>);
    int BlitFromT(TECBM&, TXY<long>);
    int BlitPartFrom(TECBM&, TXY<long>, TXY<long> const&, TXY<long> const&);
    int BlitPartFromT(TECBM&, TXY<long>, TXY<long> const&, TXY<long> const&);
    long GetAnzSubBitmaps(void) const;
    TECBM* ParseNextVertikalSubBitmap(void);
    TECBM* ParseNextHorizontalSubBitmap(void);
    FBUFFER<TECBM>* ParseVertikalSubBitmaps(void);
    void ParseHorizontalSubBitmapsInto(FBUFFER<TECBM>&);
    FBUFFER<TECBM>* ParseHorizontalSubBitmaps(void);
    TECBM* ParseVertikalSubBitmapNumberX(long);
    TECBM* ParseHorizontalSubBitmapNumberX(long);
    int ParseVertikalSubBitmapNumberXInto(long, TECBM&);
    int ParseHorizontalSubBitmapNumberXInto(long, TECBM&);

    SLONG Unknown[7];

    XY Size;

private:
    static bool BltSupport;
    static long CriticalVidMem;
    static long TotalVidMem;
};

static_assert<sizeof(TECBM) == 40> TECBM_size_check;

class TECBMKEY
{
public:
    TECBMKEY(TECBM&);
    ~TECBMKEY(void);

    LPDIRECTDRAW lpDD;
    UBYTE* Bitmap;
    SLONG lPitch;
};

static_assert<sizeof(TECBMKEY) == 12> TECBMKEY_size_check;

class TECBMKEYC
{
public:
    TECBMKEYC(const TECBM&);
    ~TECBMKEYC(void);

    LPDIRECTDRAW lpDD;
    const UBYTE* Bitmap;
    SLONG lPitch;
};

static_assert<sizeof(TECBMKEYC) == 12> TECBMKEYC_size_check;


class HDU
{
public:
    HDU();
    ~HDU();

    void Close();
    void Disable();
    void ClearScreen();
    void HercPrintf(int, char* Format, ...);
    void HercPrintf(char* Format, ...);
    void LogPosition(const char*, int);
};

extern HDU Hdu;

#define hprintf Hdu.HercPrintf
#define hprintvar(x) Hdu.HercPrintf("%d\n", x)

class XID
{
public:
    XID(void);
    void SetValue(unsigned long);

    friend TEAKFILE& operator<<(TEAKFILE&, XID const&);
    friend TEAKFILE& operator>>(TEAKFILE&, XID&);
};

extern void TeakAlbumRemoveT(FBUFFER<unsigned long>&, unsigned long, CString const&, unsigned long);
extern void TeakAlbumRefresh(FBUFFER<unsigned long>&, unsigned long);
extern long TeakAlbumSearchT(FBUFFER<unsigned long>&, unsigned long, CString const&, unsigned long);
extern long TeakAlbumXIdSearchT(FBUFFER<unsigned long>&, unsigned long, CString const&, XID&);
extern int TeakAlbumIsInAlbum(FBUFFER<unsigned long>&, unsigned long, unsigned long);
extern unsigned long TeakAlbumAddT(FBUFFER<unsigned long>&, unsigned long, CString const&, unsigned long);
extern unsigned long TeakAlbumFrontAddT(FBUFFER<unsigned long>&, unsigned long, CString const&, unsigned long);
extern unsigned long TeakAlbumGetNumFree(FBUFFER<unsigned long>&, unsigned long);
extern unsigned long TeakAlbumGetNumUsed(FBUFFER<unsigned long>&, unsigned long);
extern unsigned long TeakAlbumRandom(FBUFFER<unsigned long>&, unsigned long, CString const&, TEAKRAND*);

template <typename T>
class ALBUM
{
public:
    ALBUM(BUFFER<T>& buffer, CString str)
        : LastId(0xFFFFFF)
        , Values((FBUFFER<T>*)(&buffer))
        , Name(str)
    {
    }

    void Repair(BUFFER<T>& buffer)
    {
        Values = (FBUFFER<T>*)&buffer;
    }

    int IsInAlbum(unsigned long id)
    {
        return TeakAlbumIsInAlbum(Ids, Values->AnzEntries(), id);
    }

    long AnzEntries()
    {
        return Values->AnzEntries();
    }

    long GetNumFree()
    {
        return TeakAlbumGetNumFree(Ids, Values->AnzEntries());
    }

    long GetNumUsed()
    {
        return TeakAlbumGetNumUsed(Ids, Values->AnzEntries());
    }

    long GetRandomUsedIndex(TEAKRAND* rand = NULL)
    {
        return TeakAlbumRandom(Ids, Values->AnzEntries(), Name, rand);
    }

    long GetUniqueId()
    {
        return ++LastId;
    }

    unsigned long GetIdFromIndex(long i)
    {
        return Ids[i];
    }

    void ClearAlbum()
    {
        TeakAlbumRefresh(Ids, Values->AnzEntries());
        for (long i = Ids.AnzEntries() - 1; i >= 0; --i)
            Ids[i] = 0;
    }

    void Swap(long a, long b)
    {
        TeakAlbumRefresh(Ids, Values->AnzEntries());
        if (a >= Ids.Size)
            a = (*this)(a);
        if (b >= Ids.Size)
            b = (*this)(b);

        UBYTE* tmp = new UBYTE[sizeof(T)];
        ::Swap(Ids[a], Ids[b]);
        memcpy(tmp, &Values->MemPointer[a], sizeof(T));
        memcpy(&Values->MemPointer[a], &Values->MemPointer[b], sizeof(T));
        memcpy(&Values->MemPointer[b], tmp, sizeof(T));
        delete [] tmp;
    }

    void ResetNextId()
    {
        LastId = 0xFFFFFF;
    }

    void Sort()
    {
        UBYTE* tmp = new UBYTE[sizeof(T)];
        if (!tmp)
            TeakLibW_Exception(FNL, ExcOutOfMem);
        TeakAlbumRefresh(Ids, Values->AnzEntries());
        for (SLONG i = 0; i < Values->AnzEntries() - 1; i++)
        {
            if (Ids[i] && Ids[i + 1] && Values->MemPointer[i] > Values->MemPointer[i + 1])
            {
                ::Swap(Ids[i], Ids[i + 1]);
                memcpy(tmp, &Values->MemPointer[i], sizeof(T));
                memcpy(&Values->MemPointer[i], &Values->MemPointer[i + 1], sizeof(T));
                memcpy(&Values->MemPointer[i + 1], tmp, sizeof(T));
                i -= 2;
                if ( i < -1 )
                    i = -1;
            }
            else if (!Ids[i])
            {
                if (Ids[i + 1])
                {
                    ::Swap(Ids[i], Ids[i + 1]);
                    memcpy(tmp, &Values->MemPointer[i], sizeof(T));
                    memcpy(&Values->MemPointer[i], &Values->MemPointer[i + 1], sizeof(T));
                    memcpy(&Values->MemPointer[i + 1], tmp, sizeof(T));
                    i -= 2;
                    if (i < -1)
                        i = -1;
                }
            }
        }
        if (tmp)
            delete [] tmp;
    }

    unsigned long operator*=(unsigned long id)
    {
        return TeakAlbumFrontAddT(Ids, Values->AnzEntries(), Name, id);
    }

    unsigned long operator+=(unsigned long id)
    {
        return TeakAlbumAddT(Ids, Values->AnzEntries(), Name, id);
    }

    void operator-=(unsigned long id)
    {
        TeakAlbumRemoveT(Ids, Values->AnzEntries(), Name, id);
    }

    unsigned long operator*=(T& rhs)
    {
        unsigned long Id = TeakAlbumFrontAddT(Ids, Values->AnzEntries(), Name, GetUniqueId());
        (*this)[Id] = rhs;
        return Id;
    }

    unsigned long operator+=(T& rhs)
    {
        unsigned long Id = TeakAlbumAddT(Ids, Values->AnzEntries(), Name, GetUniqueId());
        (*this)[Id] = rhs;
        return Id;
    }

    long operator()(unsigned long id)
    {
        return TeakAlbumSearchT(Ids, Values->AnzEntries(), Name, id);
    }

    T& operator[](unsigned long id)
    {
        unsigned long i = TeakAlbumSearchT(Ids, Values->AnzEntries(), Name, id);
        return (*Values)[i];
    }

    friend TEAKFILE& operator<< (TEAKFILE& File, const ALBUM<T>& r)
    {
        File << r.LastId;
        File << r.Ids;
        return File;
    }

    friend TEAKFILE& operator>> (TEAKFILE& File, ALBUM<T>& r)
    {
        File >> r.LastId;
        File >> r.Ids;
        return File;
    }

private:
    unsigned long LastId;
    FBUFFER<unsigned long> Ids;

    // This self-reference could be stored as an offset to survive reallocations,
    // but instead Spellbound implemented a Repair() function.
    FBUFFER<T>* Values;
    CString Name;
};

extern int DoesFileExist(char const*);
extern void LoadCompleteFile(char const*, unsigned char*);
extern BUFFER<unsigned char>* LoadCompleteFile(char const*);
extern long CalcInertiaVelocity(long, long);
extern long Calc1nSum(long);

template <class T>
inline void Limit(T min, T& value, T max)
{
    if (value < min) value = min;
    if (value > max) value = max;
}

template <class T>
inline void Swap(T& a, T& b)
{
    T c(a);
    a = b;
    b = c;
}

template <class T>
inline const T& Min(const T& a, const T& b)
{
    return (b < a) ? b : a;
}


template <class T>
inline const T& Max(const T& a, const T& b)
{
    return (a < b) ? b : a;
}

inline ReferTo(...) {}

inline MB() {}
