//============================================================================================
// Checkup.h : Prüft diverse Sachen der Rechnerkonfiguration und liefert die Basis für
//             Registry-Zugriffe                                                          [TH]
// Hinweise  : 1. Die CD-Geschwindigkeit kann noch nicht zuverlässig ermittelt werden.
//             2. Auf einem 486er kann die Taktgeschwindigket noch nicht ermittelt
//                werden. Daher wird einfach 33Mhz angenommen. Das Problem wird sich
//                durch Tod des Antragstellers erledigen...
//============================================================================================
// Link: "Checkup.h"
//============================================================================================

//--------------------------------------------------------------------------------------------
// Gibt Zugriff auf die Registry und bedient sich fast wie eine Fileklasse; speichert die
// Daten nicht digital, sondern auf ASCII-Basis (und somit editierbar)
//--------------------------------------------------------------------------------------------
#define ReadRegistryKey(vp)  ReadRegistryKeyEx(vp,#vp)
#define WriteRegistryKey(vp) WriteRegistryKeyEx(vp,#vp)

class CRegistryAccess
{
   private:
      HKEY  hKey;

   public:
      CRegistryAccess ();
      CRegistryAccess (CString RegistryPath);
      ~CRegistryAccess ();
      bool Open (CString RegistryPath);
      void Close (void);

      bool IsOpen (void);

      bool ReadRegistryKeyEx (BOOL *Text, CString EntryName);
      bool ReadRegistryKeyEx (char *Text, CString EntryName);
      bool ReadRegistryKeyEx (long *Long, CString EntryName);
      bool ReadRegistryKeyEx (double *Double, CString EntryName);
      bool WriteRegistryKeyEx (const BOOL *Text, CString EntryName);
      bool WriteRegistryKeyEx (const char *Text, CString EntryName);
      bool WriteRegistryKeyEx (const long *Long, CString EntryName);
      bool WriteRegistryKeyEx (const double *Double, CString EntryName);
};

//Pfad in der Registry; Eintrag ist von der Versionummer abhängig
#define CHECKUP_REGISTRY_PATH "Software\\Spellbound Software\\Setup V%li.0"
#define CHECKUP_VERSION       1

//--------------------------------------------------------------------------------------------
// Prüft verschiedene Sachen vom System
//--------------------------------------------------------------------------------------------
#define CHECKUP_ALL        0x001f //Prüft alle Sachen
#define CHECKUP_CD         0x0001 //Prüft die CD-Geschwindigkeit
#define CHECKUP_CPU        0x0002 //Prüft die CPU-Geschwindigkeit
#define CHECKUP_RAM        0x0004 //Prüft, wieviel RAM vorhanden ist
#define CHECKUP_DIRECTX    0x0008 //Prüft, ob DirectX vorhanden ist
#define CHECKUP_OS         0x0010

#define CHECKUP_NOREGISTRY 0x0000 //Läßt die Finger von der Registry (default)
#define CHECKUP_WRITE      0x0100 //Prüft und schreibt die Sachen in die Registry
#define CHECKUP_READ       0x0200 //Prüft nicht, sondern holt nur aus der Registry

//Arten der Platform (das sind keine Aufrufparameter für Checkup!)
#define CHECK_PLATFORM_WIN31 1
#define CHECK_PLATFORM_WIN95 2
#define CHECK_PLATFORM_WINNT 3

BOOL IsPentiumOrBetter (void);

class CSystemCheckup
{
   //Das Ergebnis der Arbeit:
   public:
      //Generelles:
      long     CheckupVersion;         //Versionsnummer, wichtig für die Registry
      long     CheckupFlags;           //Dieser Informationen wurden ermittelt

      //CD:
      long     KBSec;                  //noch nicht unterstützt: CD in Kilobytes pro Sekunde, z.B. 300..
      double   Faktor;                 //noch nicht unterstützt: ..oder als Faktor, z.B. 2 = Double-Speed
         
      //OS:
      long     PlatformID;             //CHECK_PLATFORM_WIN31 / _WIN95 / WINNT
      long     OSMajorVersion;         //Major- und Minorversion wären z.B. 3 bzw. 51 für
      long     OSMinorVersion;         //Windows NT 3.51
      long     OSBuild;                //Die Build-Version (nicht sooo wichtig)

      //CPU:
      long     bMMX;                   //Ist ein MMX vorhanden?
      long     bCoprozessor;           //Ist eine FPU vorhanden?
      double   Megahertz;              //Die Taktfrequenz des Prozessors in Mhz +/- 10%

      //RAM:
      long     RealMB;                 //Soviel echtes RAM (in MB) ist installiert
      long     VirtualMB;              //Und soviel ist für virtuelles RAM eingestellt
      long     VgaRamMB;               //Soviel RAM steckt in der Grafikkarte

      //DirectX:
      long     bDXInstalled;           //Boolean, ob DirectX überhaupt vorhanden ist
      long     DXVersion;              //Sagt, in welcher Version (z.B. 5) es da ist
      long     bMidi;                  //Boolean, ob Midi abgespielt werden kann
      long     bWave;                  //Boolean, ob Waves abgespielt werden kann
      long     b3D;                    //Boolean, ob 3d-Beschleuniger vorhanden ist
      long     bAlpha;                 //Boolean, ob Alpha-Channel Blitting per Hardware möglich ist
      long     bZBuffer;               //ungetestet: Boolean, ob Z-Buffer Blitting per Hardware möglich ist

   //Diese Routine sollte man aufrufen (mit den Flags)
   public:
      CSystemCheckup () { memset (this, 0, sizeof(*this)); CheckupVersion = CHECKUP_VERSION; }
      CSystemCheckup (long Flags, CString CDFile="");
      void Checkup (long Flags, CString CDFile="");

   //Und diese sind intern und machen die Drecksarbeit:
   private:
      void CheckupCD (const CString &CDFile);
      void CheckupCPU (void);
      void CheckupOS (void);
      void CheckupRAM (void);
      void CheckupDirectX (void);
};