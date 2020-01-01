//============================================================================================
// Checkup.cpp : Prüft diverse Sachen der Rechnerkonfiguration [TH]
//============================================================================================
// Link: "Checkup.h"
//============================================================================================
//  Infos:
// ------------------------------------------------------------------------------------------
//  CSystemCheckup fragt diverse Sachen vom System ab. Dazu erzeugt man eine Instanz und
//  übergibt als Flags, welche Sachen abgefragt werden sollen (oder CHECKUP_ALL) und wie die
//  Registry mitspielt.
//
//  Die Daten können ohne Bezug auf die Registry einfach ermittelt werden (default) oder
//  können mittels CHECKUP_WRITE nach dem ermittelt in die Registry geschrieben werden.
//  Bei übergabe des Parameters CHECKUP_READ werden die Werte nicht ermittelt, sondern nur
//  aus der Registry gelesen. Ist eine bestimmte Eigenschaft in den Registry-Infos nicht
//  vorhanden, so wird diese noch ermittelt.
//
//  Läßt man den CDFile-Parameter leer, so wird der CD-Speed Check ggf. übersprungen.
//============================================================================================
#include "stdafx.h"

#include "Checkup.h"

#ifdef SYSTEM_CHECKUP
#include <fcntl.h>
#include <stdio.h>
#include <io.h>
#include <dinput.h>
#include <dsound.h>
#include <ddraw.h>

void GetDXVersion(LPDWORD pdwDXVersion, LPDWORD pdwDXPlatform);

void test (void)
{
   CSystemCheckup sc (CHECKUP_ALL | CHECKUP_WRITE, "f:\\setup.exe");
}

//--------------------------------------------------------------------------------------------
// Gibt TRUE zurück, wenn mindestens ein Pentium vorhanden ist; bei 486 gibt es FALSE zurück
//--------------------------------------------------------------------------------------------
BOOL IsPentiumOrBetter (void)
{
   SYSTEM_INFO SystemInfo;

   GetSystemInfo (&SystemInfo);

   if (SystemInfo.wProcessorArchitecture!=PROCESSOR_ARCHITECTURE_INTEL) return (FALSE);

   //Methode 1: alt
   if (SystemInfo.dwProcessorType==PROCESSOR_INTEL_386) return (FALSE);
   if (SystemInfo.dwProcessorType==PROCESSOR_INTEL_486) return (FALSE);
   if (SystemInfo.dwProcessorType==PROCESSOR_INTEL_PENTIUM) return (TRUE);
   if (SystemInfo.dwProcessorType==686) return (TRUE);
   if (SystemInfo.dwProcessorType==786) return (TRUE);
   if (SystemInfo.dwProcessorType==886) return (TRUE);

   //Methode 2: neu, für Windows NT
   if (SystemInfo.wProcessorLevel==3) return (FALSE);    //386er
   if (SystemInfo.wProcessorLevel==4) return (FALSE);    //486er
   if (SystemInfo.wProcessorLevel==5) return (TRUE);     //586er

   //Letzer Versuch: Vielleicht ein Pentium-Nachfolger?
   if (SystemInfo.wProcessorLevel>5 && SystemInfo.wProcessorLevel<10) return (TRUE);     //586er

   //Konnte nicht erkannt werden:
   return FALSE;
}
#endif

//--------------------------------------------------------------------------------------------
// CRegistryAccess::
//--------------------------------------------------------------------------------------------
// Konstruktor:
//--------------------------------------------------------------------------------------------
CRegistryAccess::CRegistryAccess ()
{
#ifdef WIN32
   hKey = NULL;
#endif
}

//--------------------------------------------------------------------------------------------
// Konstruktor+Open:
//--------------------------------------------------------------------------------------------
CRegistryAccess::CRegistryAccess (CString RegistryPath)
{
#ifdef WIN32
   hKey = NULL;
#endif
   Open (RegistryPath);
}

//--------------------------------------------------------------------------------------------
// Öffnet den Zugriff auf einen Bereich der Registry; Gibt FALSE im Fehlerfall zurück:
//--------------------------------------------------------------------------------------------
bool CRegistryAccess::Open (CString RegistryPath)
{
#ifdef WIN32
   Close ();   //Alten Zugriff schließen

   dword dwDisposition;

   if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, RegistryPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition))
      return (1); //Erfolg
   else
      return (0); //Geht nicht
#else
return false;
#endif
}

//--------------------------------------------------------------------------------------------
// Destruktor:
//--------------------------------------------------------------------------------------------
CRegistryAccess::~CRegistryAccess ()
{
   Close();
}

//--------------------------------------------------------------------------------------------
// Alten Zugriff schließen:
//--------------------------------------------------------------------------------------------
void CRegistryAccess::Close (void)
{
#ifdef WIN32
   if (hKey)
   {
      RegCloseKey(hKey);
      hKey=NULL;
   }
#endif
}

//--------------------------------------------------------------------------------------------
// Gibt TRUE zurück, wenn z.Zt ein Registry-Zugriff offen ist:
//--------------------------------------------------------------------------------------------
bool CRegistryAccess::IsOpen (void)
{
#ifdef WIN32
   return (hKey!=NULL);
#else
   return false;
#endif
}

//--------------------------------------------------------------------------------------------
// Schreibt einen Registry-Key; Gibt FALSE im Fehlerfall zurück, sonst TRUE
//--------------------------------------------------------------------------------------------
bool CRegistryAccess::WriteRegistryKeyEx (const char *Text, CString EntryName)
{
#ifdef WIN32
   if (!hKey) return (0);

   return (ERROR_SUCCESS == RegSetValueEx (hKey, EntryName, 0, REG_SZ, (UBYTE*)Text, strlen(Text)+1));
#else
   return false;
#endif
}
bool CRegistryAccess::WriteRegistryKeyEx (const BOOL *Bool, CString EntryName)
{
#ifdef WIN32
   char *Temp = new char [500];

   sprintf (Temp, "%li", (long)*Bool);

   bool rc=WriteRegistryKeyEx (Temp, EntryName);

   delete [] Temp;
   return (rc);
#else
return false;
#endif
}
bool CRegistryAccess::WriteRegistryKeyEx (const long *Long, CString EntryName)
{
#ifdef WIN32
   char *Temp = new char [500];

   sprintf (Temp, "%li", *Long);

   bool rc=WriteRegistryKeyEx (Temp, EntryName);

   delete [] Temp;
   return (rc);
#else
return false;
#endif
}
bool CRegistryAccess::WriteRegistryKeyEx (const double *Double, CString EntryName)
{
#ifdef WIN32
   char *Temp = new char [500];

   sprintf (Temp, "%f", *Double);

   bool rc=WriteRegistryKeyEx (Temp, EntryName);

   delete [] Temp;
   return (rc);
#else
return false;
#endif
}

//--------------------------------------------------------------------------------------------
// Ließt einen Registry-Key; Gibt FALSE im Fehlerfall zurück, sonst TRUE
//--------------------------------------------------------------------------------------------
bool CRegistryAccess::ReadRegistryKeyEx (char *Text, CString EntryName)
{
#ifdef WIN32
   unsigned long TempSize=500;

   if (!hKey) return (0);

   return (ERROR_SUCCESS == RegQueryValueEx (hKey, EntryName, NULL, NULL, (UBYTE*)Text, &TempSize));
#else
return false;
#endif
}
bool CRegistryAccess::ReadRegistryKeyEx (BOOL *Bool, CString EntryName)
{
#ifdef WIN32
   if (!hKey) return (0);

   char *Temp = new char [500];
   bool  rc   = ReadRegistryKeyEx (Temp, EntryName);

   if (rc) *Bool = (BOOL)atoi (Temp);

   delete [] Temp;
   return (rc);
#else
return false;
#endif
}
bool CRegistryAccess::ReadRegistryKeyEx (long *Long, CString EntryName)
{
#ifdef WIN32
   if (!hKey) return (0);

   char *Temp = new char [500];
   bool  rc   = ReadRegistryKeyEx (Temp, EntryName);

   if (rc) *Long = atoi (Temp);

   delete [] Temp;
   return (rc);
#else
return false;
#endif
}
bool CRegistryAccess::ReadRegistryKeyEx (double *Double, CString EntryName)
{
#ifdef WIN32
   if (!hKey) return (0);

   char *Temp = new char [500];
   bool  rc   = ReadRegistryKeyEx (Temp, EntryName);

   if (rc) *Double = atof (Temp);

   delete [] Temp;
   return (rc);
#else
   return false;
#endif
}
#ifdef SYSTEM_CHECKUP
//--------------------------------------------------------------------------------------------
// CSystemCheckup::
//--------------------------------------------------------------------------------------------
// Die Systemdaten abprüfen:
//--------------------------------------------------------------------------------------------
CSystemCheckup::CSystemCheckup (long Flags, CString CDFile)
{
   memset (this, 0, sizeof(*this));
   CheckupVersion = CHECKUP_VERSION;

   Checkup (Flags, CDFile);
}

//--------------------------------------------------------------------------------------------
// Die Systemdaten abprüfen:
//--------------------------------------------------------------------------------------------
void CSystemCheckup::Checkup (long Flags, CString CDFile)
{
   //Das, was wir holen wollen, als noch nicht geholt markieren:
   CheckupFlags = CheckupFlags & (~Flags);

   //Alte Daten aus der Registry laden:
   if (Flags & CHECKUP_READ)
   {
      char RegName[100];

      sprintf (RegName, CHECKUP_REGISTRY_PATH, CheckupVersion);

      CRegistryAccess reg;

      if (reg.Open (RegName))
      {
         if (Flags & CHECKUP_CD)
            if (reg.ReadRegistryKey (&KBSec) &&
                reg.ReadRegistryKey (&Faktor))
               CheckupFlags |= CHECKUP_CD;

         if (Flags & CHECKUP_CD)
            if (reg.ReadRegistryKey (&PlatformID) &&
                reg.ReadRegistryKey (&OSMajorVersion) &&
                reg.ReadRegistryKey (&OSMinorVersion) &&
                reg.ReadRegistryKey (&OSBuild))
               CheckupFlags |= CHECKUP_OS;

         if (Flags & CHECKUP_CPU)
            if (reg.ReadRegistryKey (&bMMX) &&
                reg.ReadRegistryKey (&bCoprozessor) &&
                reg.ReadRegistryKey (&Megahertz))
               CheckupFlags |= CHECKUP_CPU;

         if (Flags & CHECKUP_RAM)
            if (reg.ReadRegistryKey (&RealMB) &&
                reg.ReadRegistryKey (&VirtualMB) &&
                reg.ReadRegistryKey (&VgaRamMB))
               CheckupFlags |= CHECKUP_RAM;

         if (Flags & CHECKUP_DIRECTX)
            if (reg.ReadRegistryKey (&bDXInstalled) &&
                reg.ReadRegistryKey (&DXVersion) &&
                reg.ReadRegistryKey (&bMidi) &&
                reg.ReadRegistryKey (&bWave) &&
                reg.ReadRegistryKey (&b3D) &&
                reg.ReadRegistryKey (&bAlpha) &&
                reg.ReadRegistryKey (&bZBuffer))
               CheckupFlags |= CHECKUP_DIRECTX;
      }
   }

   //Alles, was der benutzer haben will (und was noch nicht aus der Registry geladen
   //wurde), jetzt ermitteln:
   if ((Flags & CHECKUP_CD) && !(CheckupFlags & CHECKUP_CD)) CheckupCD (CDFile);
   if ((Flags & CHECKUP_OS) && !(CheckupFlags & CHECKUP_OS)) CheckupOS ();
   if ((Flags & CHECKUP_CPU) && !(CheckupFlags & CHECKUP_CPU)) CheckupCPU ();
   if ((Flags & CHECKUP_RAM) && !(CheckupFlags & CHECKUP_RAM)) CheckupRAM ();
   if ((Flags & CHECKUP_DIRECTX) && !(CheckupFlags & CHECKUP_DIRECTX)) CheckupDirectX ();

   //Ergebnis ggf. in der Registry speichern:
   if (Flags & CHECKUP_WRITE)
   {
      char RegName[100];

      sprintf (RegName, CHECKUP_REGISTRY_PATH, CheckupVersion);

      CRegistryAccess reg;

      if (reg.Open (RegName))
      {
         if (CheckupFlags & CHECKUP_CD)
         {
            reg.WriteRegistryKey (&KBSec);
            reg.WriteRegistryKey (&Faktor);
         }

         if (CheckupFlags & CHECKUP_OS)
         {
            reg.WriteRegistryKey (&PlatformID);
            reg.WriteRegistryKey (&OSMajorVersion);
            reg.WriteRegistryKey (&OSMinorVersion);
            reg.WriteRegistryKey (&OSBuild);
         }

         if (CheckupFlags & CHECKUP_CPU)
         {
            reg.WriteRegistryKey (&bMMX);
            reg.WriteRegistryKey (&bCoprozessor);
            reg.WriteRegistryKey (&Megahertz);
         }

         if (CheckupFlags & CHECKUP_RAM)
         {
            reg.WriteRegistryKey (&RealMB);
            reg.WriteRegistryKey (&VirtualMB);
            reg.WriteRegistryKey (&VgaRamMB);
         }

         if (CheckupFlags & CHECKUP_DIRECTX)
         {
            reg.WriteRegistryKey (&bDXInstalled);
            reg.WriteRegistryKey (&DXVersion);
            reg.WriteRegistryKey (&bMidi);
            reg.WriteRegistryKey (&bWave);
            reg.WriteRegistryKey (&b3D);
            reg.WriteRegistryKey (&bAlpha);
            reg.WriteRegistryKey (&bZBuffer);
         }
      }
   }
}

//--------------------------------------------------------------------------------------------
//Prüft die CD-Geschwindigkeit:
//--------------------------------------------------------------------------------------------
void CSystemCheckup::CheckupCD (const CString &CDFile)
{
   DWORD SectorsPerCluster;
   DWORD BytesPerSector;
   DWORD NumberOfFreeClusters;
   DWORD TotalNumberOfClusters;
   char  RootPath[] = "x:\\";

   RootPath[0]=CDFile.GetAt(0);

   if (!GetDiskFreeSpace (RootPath, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters))
      return;

   long  BufferSize = (300000/BytesPerSector+1)*BytesPerSector;
   char *buffer = new char[BufferSize];
   DWORD Dummy;

   HANDLE hFile = CreateFile (CDFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING, NULL);

   if (hFile==INVALID_HANDLE_VALUE)
   {
      delete [] buffer;
      return;
   }

   ReadFile (hFile, buffer, BytesPerSector, &Dummy, NULL);

   DWORD Time = GetTickCount();
   ReadFile (hFile, buffer, BufferSize, &Dummy, NULL);
   Time = GetTickCount() - Time;

   CloseHandle (hFile);

   delete [] buffer;

   if (Time>0)
   {
      KBSec  = BufferSize / Time;
      Faktor = BufferSize * 2000.0 / 300000 / Time;
   }

   CheckupFlags |= CHECKUP_CD;
}

//--------------------------------------------------------------------------------------------
//Prüft die CPU-Geschwindigkeit
//--------------------------------------------------------------------------------------------
void CSystemCheckup::CheckupCPU (void)
{
   if (IsPentiumOrBetter())
   {
      //------------------------------------------------------------
      //Die CPU-Geschwindigkeit messen:
      //-------------------------------------------
      __int64 Stamp = Read64TimeStampCounter();

      //Eine halbe Sekunde warten:
      Sleep (500);

      Stamp=Read64TimeStampCounter()-Stamp;

      Megahertz=Stamp*2/1000.0/1000.0;

      //------------------------------------------------------------
      //Haben wir einen MMX?
      //-------------------------------------------
      unsigned long CpuIdResult;

      __asm
      {
         mov    eax, 1
         _emit  0x0F
         _emit  0xA2  //CPUID

         mov    CpuIdResult, edx
      }

      bMMX         = ((CpuIdResult & (2<<23))!=0);
      bCoprozessor = ((CpuIdResult & (2<<0))!=0);
   }
   else
   {
      Megahertz    = 33;
      bMMX         = 0;
      bCoprozessor = 0;
   }

   CheckupFlags |= CHECKUP_CPU;
}

//--------------------------------------------------------------------------------------------
//Prüft, welches OS und welche Version vorhanden ist:
//--------------------------------------------------------------------------------------------
void CSystemCheckup::CheckupOS (void)
{
   OSVERSIONINFO osvi;

   memset(&osvi, 0, sizeof(OSVERSIONINFO));
   osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
   GetVersionEx (&osvi);

   if (osvi.dwPlatformId == VER_PLATFORM_WIN32s)        PlatformID=CHECK_PLATFORM_WIN31;
   if (osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) PlatformID=CHECK_PLATFORM_WIN95;
   if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)      PlatformID=CHECK_PLATFORM_WINNT;

   OSMajorVersion = osvi.dwMajorVersion;
   OSMinorVersion = osvi.dwMinorVersion;
   OSBuild        = osvi.dwBuildNumber & 0xFFFF;

   CheckupFlags |= CHECKUP_OS;
}

//--------------------------------------------------------------------------------------------
//Schaut nach, was denn so an RAM vorhanden ist:
//--------------------------------------------------------------------------------------------
void CSystemCheckup::CheckupRAM (void)
{
   //------------------------------------------------------------
   //Speicher vom System:
   //------------------------------------------------------------
   MEMORYSTATUS MemoryStatus;

   MemoryStatus.dwLength=sizeof (MEMORYSTATUS);

   GlobalMemoryStatus (&MemoryStatus);

   //Daten übertragen:
   RealMB    = ((MemoryStatus.dwTotalPhys-1) / 1024 / 1024)+1;
   VirtualMB = MemoryStatus.dwTotalPageFile / 1024 / 1024;

   //------------------------------------------------------------
   //Speicher von der VGA-karte:
   //------------------------------------------------------------
   LPDIRECTDRAW  lpDD;
   LPDIRECTDRAW2 lpDD2;

   if (!FAILED (DirectDrawCreate(NULL, &lpDD, NULL)))
   {
      if (!FAILED(lpDD->QueryInterface(IID_IDirectDraw2, (LPVOID *)&lpDD2)))
      {
         DWORD   AvailMemory;
         DDSCAPS Caps;
         DWORD   Dummy;

         Caps.dwCaps = DDSCAPS_VIDEOMEMORY;

         if (!FAILED(lpDD2->GetAvailableVidMem (&Caps, &Dummy, &AvailMemory)))
         {
            DDSURFACEDESC DDSurfaceDesc;

            ZeroMemory (&DDSurfaceDesc, sizeof (DDSurfaceDesc));
            DDSurfaceDesc.dwSize = sizeof (DDSurfaceDesc);

            if (!FAILED (lpDD2->GetDisplayMode(&DDSurfaceDesc)))
            {
               VgaRamMB = AvailMemory + DDSurfaceDesc.dwHeight*DDSurfaceDesc.lPitch;

               VgaRamMB = (((VgaRamMB/1024)+512) / 1024);
            }
         }
         lpDD2->Release();
      }
      lpDD->Release();
   }

   CheckupFlags |= CHECKUP_RAM;
}

//--------------------------------------------------------------------------------------------
//Schaut nach, ob DirectX vorhanden ist:
//--------------------------------------------------------------------------------------------
void CSystemCheckup::CheckupDirectX (void)
{
   LPDIRECTDRAW lpDD;
   DWORD        Platform;
   DWORD        Version;

   //------------------------------------------------------------
   //Haben wir DirectX?
   //------------------------------------------------------------
   HRESULT ddrval = DirectDrawCreate(NULL, &lpDD, NULL);
   if (ddrval == DD_OK)
   {
       lpDD->Release ();
       bDXInstalled=TRUE;
   }
   else bDXInstalled=FALSE;

   //------------------------------------------------------------
   //Welche DirectX-Version?
   //------------------------------------------------------------
   GetDXVersion (&Version, &Platform);
   DXVersion=Version/256;

   if (Platform==0 || Version==0) bDXInstalled=FALSE;

   //------------------------------------------------------------
   //Haben wir Midi?
   //------------------------------------------------------------
   bMidi=midiOutGetNumDevs()>0;

   //------------------------------------------------------------
   //Können wir WAV's wiedergeben?
   //------------------------------------------------------------
   LPDIRECTSOUND lpDS;

   HRESULT dsval = DirectSoundCreate (NULL, &lpDS, NULL);

   if (dsval==DS_OK)
   {
      bWave=TRUE;
      lpDS->Release();
   }
   else bWave=FALSE;

   //------------------------------------------------------------
   //Haben wir Hardware-Support für 3d-Befehle?
   //------------------------------------------------------------
   DDCAPS  Caps;

   ddrval = DirectDrawCreate (NULL, &lpDD, NULL);
   if (ddrval == DD_OK)
   {
      memset (&Caps, 0, sizeof (Caps));
      Caps.dwSize = sizeof (Caps);

      lpDD->GetCaps (&Caps, NULL); 
   
      b3D      = (Caps.dwCaps & DDCAPS_3D)!=0;
      bAlpha   = (Caps.dwCaps & DDCAPS_ALPHA)!=0;
      bZBuffer = (Caps.dwCaps & DDCAPS_ZBLTS)!=0;

      lpDD->Release ();
   }
   else
   {
      b3D      = FALSE;
      bAlpha   = FALSE;
      bZBuffer = FALSE;
   }

   CheckupFlags |= CHECKUP_DIRECTX;
}

//--------------------------------------------------------------------------------------------
//Von Mickysoft:
//--------------------------------------------------------------------------------------------
void GetDXVersion(LPDWORD pdwDXVersion, LPDWORD pdwDXPlatform)
{
   typedef HRESULT (WINAPI *DIRECTDRAWCREATE)(GUID *, LPDIRECTDRAW *, IUnknown *);
   typedef HRESULT (WINAPI *DIRECTINPUTCREATE)(HINSTANCE, DWORD, LPDIRECTINPUT *, IUnknown *);

    HRESULT		    hr;
    HINSTANCE		    DDHinst = 0;
    HINSTANCE		    DIHinst = 0;
    LPDIRECTDRAW	    pDDraw = 0;
    LPDIRECTDRAW2	    pDDraw2 = 0;
    DIRECTDRAWCREATE	    DirectDrawCreate = 0;
    DIRECTINPUTCREATE	    DirectInputCreate = 0;
    OSVERSIONINFO	    osVer;
    LPDIRECTDRAWSURFACE	    pSurf = 0;
    LPDIRECTDRAWSURFACE3    pSurf3 = 0;

    /*
     * First get the windows platform
     */
    osVer.dwOSVersionInfoSize = sizeof(osVer);
    if (!GetVersionEx(&osVer))
    {
	*pdwDXVersion = 0;
	*pdwDXPlatform = 0;
	return;
    }

    if (osVer.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
	*pdwDXPlatform = VER_PLATFORM_WIN32_NT;
	/*
	 * NT is easy... NT 4.0 is DX2, 4.0 SP3 is DX3, 5.0 is DX5
	 * and no DX on earlier versions.
	 */
	if (osVer.dwMajorVersion < 4)
	{
	    *pdwDXPlatform = 0;	//No DX on NT3.51 or earlier
	    return;
	}
	if (osVer.dwMajorVersion == 4)
	{
	    /*
	     * NT4 up to SP2 is DX2, and SP3 onwards is DX3, so we are at least DX2
	     */
	    *pdwDXVersion = 0x200;

            /*
             * We're not supposed to be able to tell which SP we're on, so check for dinput
             */
            DIHinst = LoadLibrary("DINPUT.DLL");
            if (DIHinst == 0) 
            {
                /*
                 * No DInput... must be DX2 on NT 4 pre-SP3
                 */
                OutputDebugString("Couldn't LoadLibrary DInput\r\n");
	        return;
            }

            DirectInputCreate = (DIRECTINPUTCREATE)
                                    GetProcAddress(DIHinst, "DirectInputCreateA");
            FreeLibrary(DIHinst);

            if (DirectInputCreate == 0) 
            {
                /*
                 * No DInput... must be pre-SP3 DX2
                 */
                OutputDebugString("Couldn't GetProcAddress DInputCreate\r\n");
	        return;
            }

	    /*
	     * It must be NT4, DX2
	     */
	    *pdwDXVersion = 0x300; //DX3 on NT4 SP3 or higher
	    return;
	}
	/*
	 * Else it's NT5 or higher, and it's DX5a or higher:
	 */
	*pdwDXVersion = 0x501; //DX5a on NT5
	return;
    }

    /*
     * Not NT... must be Win9x
     */
    *pdwDXPlatform = VER_PLATFORM_WIN32_WINDOWS;

    /*
     * If we are on Memphis or higher, then we are at least DX5a
     */
    if ( (osVer.dwBuildNumber & 0xffff) > 1353) //Check for higher than developer release
    {
	*pdwDXVersion = 0x501; //DX5a on Memphis or higher
	return;
    }

    /*
     * Now we know we are in Windows 9x (or maybe 3.1), so anything's possible.
     * First see if DDRAW.DLL even exists.
     */
    DDHinst = LoadLibrary("DDRAW.DLL");
    if (DDHinst == 0) 
    {
	*pdwDXVersion = 0;
	*pdwDXPlatform = 0;
	FreeLibrary(DDHinst);
	return;
    }

    /*
     *  See if we can create the DirectDraw object.
     */
    DirectDrawCreate = (DIRECTDRAWCREATE)
                            GetProcAddress(DDHinst, "DirectDrawCreate");
    if (DirectDrawCreate == 0) 
    {
	*pdwDXVersion = 0;
	*pdwDXPlatform = 0;
	FreeLibrary(DDHinst);
        OutputDebugString("Couldn't LoadLibrary DDraw\r\n");
	return;
    }

    hr = DirectDrawCreate(NULL, &pDDraw, NULL);
    if (FAILED(hr)) 
    {
	*pdwDXVersion = 0;
	*pdwDXPlatform = 0;
	FreeLibrary(DDHinst);
        OutputDebugString("Couldn't create DDraw\r\n");
	return;
    }

    /*
     *  So DirectDraw exists.  We are at least DX1.
     */
    *pdwDXVersion = 0x100;

    /*
     *  Let's see if IID_IDirectDraw2 exists.
     */
    hr = pDDraw->QueryInterface(IID_IDirectDraw2, (LPVOID *)&pDDraw2);
    if (FAILED(hr)) 
    {
	/*
	 * No IDirectDraw2 exists... must be DX1
	 */
	pDDraw->Release();
	FreeLibrary(DDHinst);
        OutputDebugString("Couldn't QI DDraw2\r\n");
	return;
    }
    /*
     * IDirectDraw2 exists. We must be at least DX2
     */
    pDDraw2->Release();
    *pdwDXVersion = 0x200;

    /*
     *  See if we can create the DirectInput object.
     */
    DIHinst = LoadLibrary("DINPUT.DLL");
    if (DIHinst == 0) 
    {
        /*
         * No DInput... must be DX2
         */
        OutputDebugString("Couldn't LoadLibrary DInput\r\n");
	pDDraw->Release();
	FreeLibrary(DDHinst);
	return;
    }

    DirectInputCreate = (DIRECTINPUTCREATE)
                            GetProcAddress(DIHinst, "DirectInputCreateA");
    FreeLibrary(DIHinst);

    if (DirectInputCreate == 0) 
    {
        /*
         * No DInput... must be DX2
         */
	FreeLibrary(DDHinst);
	pDDraw->Release();
        OutputDebugString("Couldn't GetProcAddress DInputCreate\r\n");
	return;
    }

    /*
     * DirectInputCreate exists. That's enough to tell us that we are at least DX3
     */
    *pdwDXVersion = 0x300;

    /*
     * Checks for 3a vs 3b?
     */

    /*
     * We can tell if DX5 is present by checking for the existence of IDirectDrawSurface3.
     * First we need a surface to QI off of.
     */
    DDSURFACEDESC desc;

    ZeroMemory(&desc, sizeof(desc));
    desc.dwSize = sizeof(desc);
    desc.dwFlags = DDSD_CAPS;
    desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    hr = pDDraw->SetCooperativeLevel(NULL,DDSCL_NORMAL);
    if (FAILED(hr)) 
    {
	/*
	 * Failure. This means DDraw isn't properly installed.
	 */
	pDDraw->Release();
	FreeLibrary(DDHinst);
	*pdwDXVersion = 0;
        OutputDebugString("Couldn't Set coop level\r\n");
	return;
    }

    hr = pDDraw->CreateSurface(&desc, &pSurf, NULL);
    if (FAILED(hr)) 
    {
	/*
	 * Failure. This means DDraw isn't properly installed.
	 */
	pDDraw->Release();
	FreeLibrary(DDHinst);
	*pdwDXVersion = 0;
        OutputDebugString("Couldn't CreateSurface\r\n");
	return;
    }

    /*
     * Try for the IDirectDrawSurface3 interface. If it works, we're on DX5 at least
     */
    if ( FAILED(pSurf->QueryInterface(IID_IDirectDrawSurface3,(LPVOID*)&pSurf3)) )
    {
        pDDraw->Release();
        FreeLibrary(DDHinst);

        return;
    }

    /*
     * QI for IDirectDrawSurface3 succeeded. We must be at least DX5
     */
    *pdwDXVersion = 0x500;

    pSurf->Release();
    pDDraw->Release();
    FreeLibrary(DDHinst);

    return;
}
#endif
