#pragma once

extern void memswap(void*, void*, ULONG);
extern char* bprintf(char const*, ...);
extern char* bitoa(int);
extern void here(char*, SLONG);

extern const char* ExcAssert;
extern const char* ExcGuardian;
extern const char* ExcImpossible;
extern const char* ExcNotImplemented;
extern const char* ExcOutOfMem;
extern const char* ExcStrangeMem;

#define FNL 0, 0

extern SLONG TeakLibW_Exception(char*, SLONG, const char*, ...);
extern char* TeakStrRemoveCppComment(char*);
extern char* TeakStrRemoveEndingCodes(char*, char const*);
extern unsigned char GerToLower(unsigned char);
extern unsigned char GerToUpper(unsigned char);
extern unsigned char* RecapizalizeString(unsigned char*);
extern const char* GetSuffix(const char*);

template <typename T>
inline void Limit(T min, T& value, T max)
{
    if (value < min) value = min;
    if (value > max) value = max;
}

template <typename T>
inline void Swap(T& a, T& b)
{
    T c;
    c = a;
    a = b;
    b = c;
}

template <typename T>
inline const T& Min(const T& a, const T& b)
{
    return (b < a) ? b : a;
}

template <typename T>
inline const T& Max(const T& a, const T& b)
{
    return (a < b) ? b : a;
}

template <typename A, typename B>
inline A min(const A& a, const B& b)
{
    return (b < a) ? b : a;
}

template <typename A, typename B>
inline A max(const A& a, const B& b)
{
    return (a < b) ? b : a;
}

inline void ReferTo(...) {}

inline void MB() {}

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

    BUFFER(BUFFER&& rhs)
        : MemPointer(std::move(rhs.MemPointer))
        , DelPointer(std::move(rhs.DelPointer))
        , Size(std::move(rhs.Size))
    {
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
                //memswap(m, MemPointer, sizeof(T) * num);
                for (SLONG i = 0; i < num; i++)
                {
                    T tmp(m[i]);
                    m[i] = MemPointer[i];
                    MemPointer[i] = tmp;
                }
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

    SLONG AnzEntries() const { return Size; }

    void Clear()
    {
        if (Size > 0)
        {
            if (MemPointer)
                delete [] MemPointer;
            MemPointer = DelPointer = NULL;
            Size = 0;
        }
    }

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

    void operator=(BUFFER<T>&& rhs)
    {
        MemPointer = std::move(rhs.MemPointer);
        DelPointer = std::move(rhs.DelPointer);
        Size = std::move(rhs.Size);
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
    TEAKFILE(char const*, SLONG);
    ~TEAKFILE(void);

    void ReadLine(char*, SLONG);
    int IsEof(void);
    void Close(void);
    SLONG GetFileLength(void);
    SLONG GetPosition(void);
    void Open(char const*, SLONG);
    int IsOpen(void);
    unsigned char* Read(SLONG);
    void Read(unsigned char*, SLONG);
    char* ReadLine(void);
    void ReadTrap(SLONG);
    void WriteTrap(SLONG);
    void SetPasswort(char*);
    void SetPosition(SLONG);
    void Skip(SLONG);
    void Write(unsigned char*, SLONG);
    void WriteLine(char*);
    void Announce(SLONG);

    SDL_RWops* Ctx;
    SLONG Unknown[3];
    char* Path;
    SLONG Unknown1[7];
    BUFFER<UBYTE> MemBuffer;
    SLONG MemPointer;
    ULONG MemBufferUsed;

    friend TEAKFILE& operator << (TEAKFILE& File, const bool& b) { File.Write((UBYTE*)& b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, bool& b) { File.Read((UBYTE*)& b, sizeof(b)); return File; }

    friend TEAKFILE& operator << (TEAKFILE& File, const signed char& b) { File.Write((UBYTE*)& b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, signed char& b) { File.Read((UBYTE*)& b, sizeof(b)); return File; }

    friend TEAKFILE& operator << (TEAKFILE& File, const unsigned char& b) { File.Write((UBYTE*)& b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, unsigned char& b) { File.Read((UBYTE*)& b, sizeof(b)); return File; }

    friend TEAKFILE& operator << (TEAKFILE& File, const unsigned short& b) { File.Write((UBYTE*)& b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, unsigned short& b) { File.Read((UBYTE*)& b, sizeof(b)); return File; }

    friend TEAKFILE& operator << (TEAKFILE& File, const signed short& b) { File.Write((UBYTE*)& b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, signed short& b) { File.Read((UBYTE*)& b, sizeof(b)); return File; }

    friend TEAKFILE& operator << (TEAKFILE& File, const unsigned int& b) { File.Write((UBYTE*)&b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, unsigned int& b) { File.Read((UBYTE*)&b, sizeof(b)); return File; }

    friend TEAKFILE& operator << (TEAKFILE& File, const signed int& b) { File.Write((UBYTE*)&b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, signed int& b) { File.Read((UBYTE*)&b, sizeof(b)); return File; }

    friend TEAKFILE& operator << (TEAKFILE& File, const unsigned long& b) { File.Write((UBYTE*)& b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, unsigned long& b) { File.Read((UBYTE*)& b, sizeof(b)); return File; }

    friend TEAKFILE& operator << (TEAKFILE& File, const signed long& b) { File.Write((UBYTE*)& b, sizeof(b)); return File; }
    friend TEAKFILE& operator >> (TEAKFILE& File, signed long& b) { File.Read((UBYTE*)& b, sizeof(b)); return File; }

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
        File.Write((UBYTE*)(PCSTR)b, b.GetLength() + 1);
        return File;
    }
    friend TEAKFILE& operator >> (TEAKFILE& File, CString& b)
    {
        ULONG size;
        File >> size;
        BUFFER<BYTE> str(size);
        File.Read(str, size);
        b = (PCSTR)(BYTE*)str;
        return File;
    }

    template <typename T>
    friend TEAKFILE& operator << (TEAKFILE& File, const BUFFER<T>& buffer)
    {
        File << buffer.Size;
        File << SLONG(buffer.DelPointer - buffer.MemPointer);
        for (SLONG i = 0; i < buffer.Size; i++)
            File << buffer.MemPointer[i];
        return File;
    }

    template <typename T>
    friend TEAKFILE& operator >> (TEAKFILE& File, BUFFER<T>& buffer)
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

private:
    void CodeBlock(unsigned char*, SLONG, SLONG);
};

//static_assert(sizeof(TEAKFILE) == 68, "TEAKFILE_size_check");

class CRLEReader
{
public:
    CRLEReader(const char* path);
    ~CRLEReader(void);

    bool Close(void);
    bool Buffer(void*, SLONG);
    bool NextSeq(void);
    bool Read(BYTE*, SLONG, bool);

    SLONG GetSize() { return Size; }

private:
    SDL_RWops* Ctx;
    char SeqLength;
    char SeqUsed;
    bool IsSeq;
    BYTE Sequence[132];

    bool IsRLE;
    SLONG Size;
    int Key;
};

template <typename T>
class FBUFFER : public BUFFER<T>
{
public:
    FBUFFER(void) : BUFFER<T>(0) {}

    FBUFFER(FBUFFER& buffer) : BUFFER<T>(buffer) {}

    FBUFFER(FBUFFER&& buffer) : BUFFER<T>(buffer) {}

    FBUFFER(SLONG anz) : BUFFER<T>(anz) {}

    void operator=(FBUFFER<T>& rhs) { BUFFER<T>::operator=(rhs); }

    void operator=(FBUFFER<T>&& rhs) { BUFFER<T>::operator=(rhs); }
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
        return TXY<T>(x + b.x, y + b.y, z + b.z);
    }

    TXYZ operator-(const TXYZ& b) const
    {
        return TXY<T>(x - b.x, y - b.y, z - b.z);
    }

    TXYZ operator*(const TXYZ& b) const
    {
        return TXY<T>(x * b.x, y * b.y, z * b.z);
    }

    TXYZ operator/(const TXYZ& b) const
    {
        return TXY<T>(x / b.x, y / b.y, z / b.z);
    }

    TXYZ operator*(const T& b) const
    {
        return TXY<T>(x * b, y * b, z * b);
    }

    TXYZ operator/(const T& b) const
    {
        return TXY<T>(x / b, y / b, z / b);
    }

    TXYZ operator-() const
    {
        return TXY<T>(-x, -y, -z);
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

    void Blend(SLONG, SLONG) const;
    void BlendIn(void) const;
    void BlendOut(void) const;
    void RefreshDD(int) const;
    void RefreshDD(SLONG, int) const;
    void RefreshPalFromLbm(CString const&);
    void RefreshPalFromPcx(CString const&);
    void ConvertToBlackWhite(void);
    unsigned char FindColorClosestToRGB(unsigned char, unsigned char, unsigned char) const;
    void RotateArea(SLONG, SLONG);
    void CopyArea(SLONG, SLONG, SLONG);
    void CopyAreaFrom(PALETTE const&, SLONG, SLONG, SLONG);

    BUFFER<SDL_Color> Pal;
    SLONG Unknown;
};

//static_assert(sizeof(PALETTE) == 16, "PALETTE size check");

struct TEXTRES_CACHE_ENTRY
{
    SLONG Group, Id;
    char* Text;
};

//static_assert(sizeof(TEXTRES_CACHE_ENTRY) == 12, "TEXTRES_CACHE_ENTRY size check");

#define TEXTRES_CACHED (void*)1

class TEXTRES
{
public:
    TEXTRES();
    TEXTRES(char const*, void*);
    ~TEXTRES(void);

    void Open(char const*, void*);
    BUFFER<char>& GetB(ULONG, ULONG);
    char* GetP(ULONG, ULONG);
    char* GetS(ULONG, ULONG);
    //char* GetS(ULONG, char const*);
    char* GetS(char const* c, ULONG i) { return GetS(*(ULONG*)c, i); }

private:
    BUFFER<char> Path;
    BUFFER<char> Strings;
    BUFFER<TEXTRES_CACHE_ENTRY> Entries;
};

//static_assert(sizeof(TEXTRES) == 36, "TEXTRES size check");

class CRegistration
{
public:
    CRegistration(void);
    CRegistration(CString const&, ULONG);
    void ReSize(CString const&, ULONG);
    CString GetDisplayString(void);
    SLONG GetMode(void);
    CString GetSomeString(char*);
    ULONG CalcChecksum(CString);
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
    TECBM(CString const&, SLONG, void*);
    TECBM(SLONG, SLONG, void*);
    TECBM(TXY<SLONG>, void*);
    ~TECBM(void);

    void Destroy(void);
    TECBM& operator=(TECBM&);
    virtual int Refresh(void);
    int TextOutA(SLONG, SLONG, ULONG, ULONG, CString const&);
    int IsOk(void) const;
    static int IsMemCritical(void);
    static int IsHardwareCritical(void);
    static int IsEitherCritical(void);
    static void SetCriticalMem(SLONG);
    void ReSize(CString const&, void*);
    void ReSize(CString const&, SLONG, void*);
    void ReSizeLbm(CString const&, void*);
    void ReSizeLbm(CString const&, SLONG, void*);
    void ReSize(TXY<SLONG>, void*);
    void ReSizePcx(CString const&, SLONG, void*);
    void ReSizePcx(CString const&, void*);
    int SavePCX(CString const&, PALETTE const&) const;
    void ShiftColors(SLONG);
    void RemapColor(unsigned char, unsigned char);
    int BlitFrom(TECBM const&, TXY<SLONG>, TXY<SLONG>);
    int BlitFromT(TECBM const&, TXY<SLONG>, TXY<SLONG>, unsigned char);
    void InterleaveBitmaps(TECBM const&, TECBM const&, SLONG);
    int UniversalClip(TXY<SLONG>*, CRect*);
    void GetClipRegion(TXY<SLONG>*, TXY<SLONG>*);
    void SetClipRegion(void);
    void SetClipRegion(TXY<SLONG>, TXY<SLONG>);
    int IsLost(void) const;
    int SetPixel(TXY<SLONG>, unsigned char);
    unsigned char GetPixel(TXY<SLONG>) const;
    int FillWith(unsigned char);
    int FillWith(TECBM const&);
    int FillWith(TECBM const&, TXY<SLONG>);
    int Line(TXY<SLONG> const&, TXY<SLONG> const&, unsigned char);
    int HLine(SLONG, SLONG, SLONG, unsigned char);
    int VLine(SLONG, SLONG, SLONG, unsigned char);
    int DotLine(TXY<SLONG> const&, TXY<SLONG> const&, unsigned char);
    int Rectangle(TXY<SLONG> const&, TXY<SLONG> const&, unsigned char);
    int Box(TXY<SLONG>, TXY<SLONG>, unsigned char);
    int Box(TXY<SLONG> const&, TXY<SLONG> const&, TECBM const&);
    int Box(TXY<SLONG> const&, TXY<SLONG> const&, TECBM const&, TXY<SLONG>);
    int Circle(TXY<SLONG> const&, SLONG, unsigned char);
    int BlitFrom(TECBM&, TXY<SLONG>);
    int BlitFromT(TECBM&, TXY<SLONG>);
    int BlitPartFrom(TECBM&, TXY<SLONG>, TXY<SLONG> const&, TXY<SLONG> const&);
    int BlitPartFromT(TECBM&, TXY<SLONG>, TXY<SLONG> const&, TXY<SLONG> const&);
    SLONG GetAnzSubBitmaps(void) const;
    TECBM* ParseNextVertikalSubBitmap(void);
    TECBM* ParseNextHorizontalSubBitmap(void);
    FBUFFER<TECBM>* ParseVertikalSubBitmaps(void);
    void ParseHorizontalSubBitmapsInto(FBUFFER<TECBM>&);
    FBUFFER<TECBM>* ParseHorizontalSubBitmaps(void);
    TECBM* ParseVertikalSubBitmapNumberX(SLONG);
    TECBM* ParseHorizontalSubBitmapNumberX(SLONG);
    int ParseVertikalSubBitmapNumberXInto(SLONG, TECBM&);
    int ParseHorizontalSubBitmapNumberXInto(SLONG, TECBM&);

    SDL_Surface* Surface;
    SLONG Unknown[6];

    XY Size;

private:
    static bool BltSupport;
    static SLONG CriticalVidMem;
    static SLONG TotalVidMem;
};

//static_assert(sizeof(TECBM) == 40, "TECBM size check");

class TECBMKEY
{
public:
    TECBMKEY(TECBM&);
    ~TECBMKEY(void);

    SDL_Surface* Surface;
    UBYTE* Bitmap;
    SLONG lPitch;
};

//static_assert(sizeof(TECBMKEY) == 12, "TECBMKEY size check");

class TECBMKEYC
{
public:
    TECBMKEYC(const TECBM&);
    ~TECBMKEYC(void);

    SDL_Surface* Surface;
    const UBYTE* Bitmap;
    SLONG lPitch;
};

//static_assert(sizeof(TECBMKEYC) == 12, "TECBMKEYC size check");


class HDU
{
public:
    HDU();
    ~HDU();

    void Close();
    void Disable();
    void ClearScreen();
    void HercPrintf(int, const char* Format, ...);
    void HercPrintf(const char* Format, ...);
    void LogPosition(const char*, int);

private:
    FILE* Log;
};

extern HDU Hdu;

#define hprintf Hdu.HercPrintf
#define hprintvar(x) Hdu.HercPrintf("%d\n", x)

class XID
{
public:
    XID(void);
    void SetValue(ULONG);

    friend TEAKFILE& operator<<(TEAKFILE&, XID const&);
    friend TEAKFILE& operator>>(TEAKFILE&, XID&);

    SLONG Value;
    SLONG Index;
};

extern void TeakAlbumRemoveT(FBUFFER<ULONG>&, ULONG, CString const&, ULONG);
extern void TeakAlbumRefresh(FBUFFER<ULONG>&, ULONG);
extern SLONG TeakAlbumSearchT(FBUFFER<ULONG>&, ULONG, CString const&, ULONG);
extern SLONG TeakAlbumXIdSearchT(FBUFFER<ULONG>&, ULONG, CString const&, XID&);
extern int TeakAlbumIsInAlbum(FBUFFER<ULONG>&, ULONG, ULONG);
extern ULONG TeakAlbumAddT(FBUFFER<ULONG>&, ULONG, CString const&, ULONG);
extern ULONG TeakAlbumFrontAddT(FBUFFER<ULONG>&, ULONG, CString const&, ULONG);
extern ULONG TeakAlbumGetNumFree(FBUFFER<ULONG>&, ULONG);
extern ULONG TeakAlbumGetNumUsed(FBUFFER<ULONG>&, ULONG);
extern ULONG TeakAlbumRandom(FBUFFER<ULONG>&, ULONG, CString const&, TEAKRAND*);

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

    int IsInAlbum(ULONG id)
    {
        return TeakAlbumIsInAlbum(Ids, Values->AnzEntries(), id);
    }

    SLONG AnzEntries()
    {
        return Values->AnzEntries();
    }

    SLONG GetNumFree()
    {
        return TeakAlbumGetNumFree(Ids, Values->AnzEntries());
    }

    SLONG GetNumUsed()
    {
        return TeakAlbumGetNumUsed(Ids, Values->AnzEntries());
    }

    SLONG GetRandomUsedIndex(TEAKRAND* rand = NULL)
    {
        return TeakAlbumRandom(Ids, Values->AnzEntries(), Name, rand);
    }

    SLONG GetUniqueId()
    {
        return ++LastId;
    }

    ULONG GetIdFromIndex(SLONG i)
    {
        return Ids[i];
    }

    void ClearAlbum()
    {
        TeakAlbumRefresh(Ids, Values->AnzEntries());
        for (SLONG i = Ids.AnzEntries() - 1; i >= 0; --i)
            Ids[i] = 0;
    }

    void Swap(SLONG a, SLONG b)
    {
        TeakAlbumRefresh(Ids, Values->AnzEntries());
        if (a >= Ids.Size)
            a = (*this)(a);
        if (b >= Ids.Size)
            b = (*this)(b);

        ::Swap(Ids[a], Ids[b]);
        ::Swap(Values->MemPointer[a], Values->MemPointer[b]);
    }

    void ResetNextId()
    {
        LastId = 0xFFFFFF;
    }

    void Sort()
    {
        TeakAlbumRefresh(Ids, Values->AnzEntries());
        for (SLONG i = 0; i < Values->AnzEntries() - 1; i++)
        {
            if (Ids[i] && Ids[i + 1] && Values->MemPointer[i] > Values->MemPointer[i + 1])
            {
                ::Swap(Ids[i], Ids[i + 1]);
                ::Swap(Values->MemPointer[i], Values->MemPointer[i + 1]);
                i -= 2;
                if ( i < -1 )
                    i = -1;
            }
            else if (!Ids[i])
            {
                if (Ids[i + 1])
                {
                    ::Swap(Ids[i], Ids[i + 1]);
                    ::Swap(Values->MemPointer[i], Values->MemPointer[i + 1]);
                    i -= 2;
                    if (i < -1)
                        i = -1;
                }
            }
        }
    }

    ULONG operator*=(ULONG id)
    {
        return TeakAlbumFrontAddT(Ids, Values->AnzEntries(), Name, id);
    }

    ULONG operator+=(ULONG id)
    {
        return TeakAlbumAddT(Ids, Values->AnzEntries(), Name, id);
    }

    void operator-=(ULONG id)
    {
        TeakAlbumRemoveT(Ids, Values->AnzEntries(), Name, id);
    }

    ULONG operator*=(T& rhs)
    {
        ULONG Id = TeakAlbumFrontAddT(Ids, Values->AnzEntries(), Name, GetUniqueId());
        (*this)[Id] = rhs;
        return Id;
    }

    ULONG operator*=(T&& rhs)
    {
        ULONG Id = TeakAlbumFrontAddT(Ids, Values->AnzEntries(), Name, GetUniqueId());
        (*this)[Id] = rhs;
        return Id;
    }

    ULONG operator+=(T& rhs)
    {
        ULONG Id = TeakAlbumAddT(Ids, Values->AnzEntries(), Name, GetUniqueId());
        (*this)[Id] = rhs;
        return Id;
    }

    ULONG operator+=(T&& rhs)
    {
        ULONG Id = TeakAlbumAddT(Ids, Values->AnzEntries(), Name, GetUniqueId());
        (*this)[Id] = rhs;
        return Id;
    }

    SLONG operator()(ULONG id)
    {
        return TeakAlbumSearchT(Ids, Values->AnzEntries(), Name, id);
    }

    T& operator[](ULONG id)
    {
        ULONG i = TeakAlbumSearchT(Ids, Values->AnzEntries(), Name, id);
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
    ULONG LastId;
    FBUFFER<ULONG> Ids;

    // This self-reference could be stored as an offset to survive reallocations,
    // but instead Spellbound implemented a Repair() function.
    FBUFFER<T>* Values;
    CString Name;
};

extern int DoesFileExist(char const*);
extern BUFFER<BYTE>* LoadCompleteFile(char const*);
extern SLONG CalcInertiaVelocity(SLONG, SLONG);
extern SLONG Calc1nSum(SLONG);
