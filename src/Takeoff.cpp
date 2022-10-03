//============================================================================================
// TakeOff.cpp : Defines the class behaviors for the application.
//============================================================================================
// Link: "Takeoff.h"
//============================================================================================
#include "stdafx.h"
#include "HLine.h"
#include "Checkup.h"
#include <stdio.h>
#include <time.h>
#include "Abend.h"
#include "ArabAir.h"
#include "Aufsicht.h"
#include "Bank.h"
#include "Buero.h"
#include "Credits.h"
#include "DutyFree.h"
#include "Fracht.h"
#include "Insel.h"
#include "Intro.h"
#include "Kiosk.h"
#include "Makler.h"
#include "Museum.h"
#include "Nasa.h"
#include "NewGamePopup.h"  //Fenster zum Wahl der Gegner und der Spielstärke
#include "Outro.h"
#include "PlanProp.h"
#include "Reise.h"
#include "Ricks.h"
#include "RouteBox.h"
#include "Rushmore.h"
#include "Sabotage.h"
#include "Statistk.h"
#include "Tafel.h"
#include "TitlePopup.h"
#include "WeltAll.h"
#include "Security.h"
#include "Werbung.h"
#include "Editor.h"
#include "Designer.h"
#include "World.h"

#include "glTitel.h"
#include "cd_prot.h"

#include "AtNet.h"
#include "SbLib.h"
extern SBNetwork gNetwork;

#include <fstream>
#include <filesystem>

#ifdef SENTRY
#include "sentry.h"
#endif

CHLPool HLPool;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void Unvideo (CString Filename, CString TargetFilename);

CJumpingVar<ULONG>   gPhysicalCdRomBitlist=0;
CJumpingVar<CString> gCDPath;

extern char  VersionString[];
extern SLONG bCAbendOpen;
extern SLONG SkipPlaneCalculation;

static const char FileId[] = "Take";

static CString PlaneSounds[] = { "prop.raw", "flyby.raw", "flyby2.raw", "flyby3.raw", "flyby4.raw", "flyby5.raw" };

BOOL gCDFound   = FALSE;
BOOL gSpawnOnly = TRUE;

extern char chRegKey[];

SLONG gLoadGameNumber=-1;

CTakeOffApp *pTakeOffApp;

#define SND_TYPE unsigned char
void CompressWave (BUFFER<SND_TYPE> &Input, BUFFER<SND_TYPE> &Output);
void DecompressWave (BUFFER<SND_TYPE> &Input, BUFFER<SND_TYPE> &Output);

void PumpNetwork (void);

CString MakeVideoPath, MakeVideoPath2;
BOOL    MakeUnvideoOn555=FALSE;

FILE *CreditsSmackerFileHandle = NULL;

SLONG gTimerCorrection=0;                         //Is it necessary to adapt the local clock to the server clock?

//StackSaver MySaver;

//--------------------------------------------------------------------------------------------
// BetaIds zeigen, an wen das Produkt gegeben wurde
//--------------------------------------------------------------------------------------------
// 223372036854775808-922337203685477580  ??.?.98-??.?.98 : Intern; wurde an Attic rausgegeben
// 891234620498163549-214920573141261048  ??.?.98-17.3.98 : Intern; an Bomico gegeben
// 284059172374591027-385018361440193713  17.3.98-25.3.98 : Intern; wurde nicht rausgegeben
// 967182349034787628-198472839471673899  25.3.98-26.3.98 : Intern; an Bomico gegeben
// 942134127836527725-423987123498074234  26.3.98- 7.4.98 : Intern; wurde nicht rausgegeben
// 423149786259875324-231768098574398743   7.4.98-10.4.98 : Intern; wurde nicht rausgegeben
// 987234619283842039-423098123784239874  10.4.98-20.4.98 : Intern; Betatester waren hier; an Ravensburger
// 132518462572847204-810542742871364297  20.4.98-21.4.98 : Intern; Microsoft
// 132518462572847204-810542742871364297  21.4.98-23.4.98 : Intern; Person in den USA (Buzz Software)
// 227419893784723849-827418324729347824  23.4.98-26.4.98 : Intern; Jan Sjovall
// 423764298736123753-423987205843284902  26.4.98-30.4.98 : Intern; Pressetour
// 789427184592174332-204502824319384324  30.4.98-12.5.98 : Intern; 
// 921364237861692844-987412312764239874  12.5.98-13.5.98 : Intern; Interplay
// 342197861237648912-423412786123612394  13.5.98-18.5.98 : Intern;
// 987243896123742334-423984724987650954  18.5.98- 5.6.98 : Intern; Bomico
// 432897342156432198-423554835218452549   5.6.98-??.?.98 : Intern; Bomico; Trinode
// 883610937218917328-719273618973427422  ??.?.98-25.6.98 : Intern; Bomico
// 936249237874523734-224238234810878542  25.6.98- 8.7.98 : Intern; Bomico
// 195236205634827322-964583412397724543   8.7.98- 8.7.98 : Intern; Taiwain (Englisch, mit Markierung in Hauptmenü-Bitmap)
// 889132557179123773-104993427823467831   8.7.98- 9.7.98 : Intern; Jon Benton, Buzz Software
// 883125133229672333-712487216511248790   9.7.98- 9.7.98 : Intern; Via Bomico auch an die Presse
// 321612656732266231-237856732144345622   9.7.98-16.7.98 : Intern; Via Bomico auch an die Presse
// 897893125667432312-922026623427828832  16.7.98-19.7.98 : Intern; Ascaron
// 989423123113267672-123012311723475355  19.7.98-31.7.98 : Intern;
// 123453273129043612-893121723114389453  31.7.98- 7.8.98 : Intern; Mitgenommen nach Mülheim
// 723162316723632131-908090896239016302   7.8.98- 8.9.98 : Intern; An Bomico gegeben
// 423423787892349122-783120312831231255   8.9.98-12.9.98 : Release-Version (erster Versuch)
// 423123479031289074-978789786786122322  12.9.98-15.9.98 : Release-Version
// 312573126784237983-423423789897233332  15.9.98-29.10.98 : Englische Version für Buzz-Software
// 676786789767891234-234534534534563232 29.10.98-  7.4.99 : Englische Version für Buzz-Software
// 234234239467423422-423098146284923443   7.4.99- 19.4.98 : Betatest Version (offiziell)
// 993621834926654222-986294432529344524  19.4.99- 19.4.98 : PC Games
// 312312312397889777-777312893743282949  19.4.99- 19.4.98 : GameStar
// 882638264836778892-992265266635274828  19.4.99-   . .98 : Powerplay
// 999666555423198743-423423423488888222  19.4.99- 21.4.98 : Betatest Version (offiziell)
// 423987402389743333-988437373399993784  21.4.99- 21.4.98 : Version für Petra Maueröder
// 423874612978367654-423423467897689766  21.4.99- 22.4.98 : Betatest Version (offiziell)
// 455675675567567656-423423423244444543  22.4.99- 22.4.98 : Version für Joe Nettelbeck
// 787878787889879433-654445373848292349  22.4.99-  5.6.98 : Betatest Version (offiziell)
// 765423432423432676-432987774377733433   5.6.99-   .4.98 : Release-Version
//--------------------------------------------------------------------------------------------
__int64 betaId[3] = { 123456789876543210, 765423432423432676, 432987774377733433 };

char *UCharToReadableAnsi( const unsigned char *pData, const unsigned uLen );
unsigned char *ReadableAnsiToUChar( const char *pData, const unsigned uLen );

#define SCRAMBLE_ADD_XOR 0xa0febff4

struct protectedValue
{
	union charint
	{
		int		iValue;
		char	acValue[4];
	};

	// methods
	__forceinline			protectedValue( );
	__forceinline void		SetValue( int iValue );
	__forceinline void		AddValue( const int iAdd );
	__forceinline int		GetValue( ) const;

	// Operators
	bool operator > ( const protectedValue &cmp ) const { return GetValue( ) > cmp.GetValue( ); };
	bool operator < ( const protectedValue &cmp ) const { return GetValue( ) < cmp.GetValue( ); };

	// read access
	__forceinline int		GetValueRaw( ) const { return mcharintScore.iValue; }
	__forceinline unsigned	GetScrambleRaw( ) const { return muScramble; }

public:

	// Methods
	__forceinline int	Descramble( ) const;
	__forceinline void	SetScrambled( const int iValue );

	// Members
	charint			mcharintScore;
	unsigned		muScramble;
};


protectedValue::protectedValue( ) :
	muScramble( GetTickCount( ) + ( unsigned ) rand( ) )
{
	SetValue( 0 );
};


void protectedValue::SetValue( int iValue )
{
	iValue += ( muScramble ^ SCRAMBLE_ADD_XOR );
	SetScrambled( iValue );
}


void protectedValue::AddValue( const int iAdd )
{
	int iScore;
	iScore = Descramble( );
	iScore += iAdd;
	SetScrambled( iScore );
}


int protectedValue::GetValue( ) const
{
	int iValue = Descramble( );
	iValue -= ( muScramble ^ SCRAMBLE_ADD_XOR );

	return iValue;
}


int	protectedValue::Descramble( ) const
{
	charint iScore;

	// descramble
	iScore.acValue[0] = mcharintScore.acValue[2] ^ ( unsigned char ) muScramble;
	iScore.acValue[1] = mcharintScore.acValue[0] ^ ( unsigned char ) muScramble;
	iScore.acValue[2] = mcharintScore.acValue[3] ^ mcharintScore.acValue[2];
	iScore.acValue[3] = mcharintScore.acValue[1] ^ mcharintScore.acValue[0];

	return iScore.iValue;
}


void protectedValue::SetScrambled( const int iValue )
{
	// make pointer
	charint* pcharintVar = ( charint* ) &iValue;

	// scramble
	mcharintScore.acValue[2] = pcharintVar->acValue[0] ^ ( unsigned char ) muScramble;
	mcharintScore.acValue[0] = pcharintVar->acValue[1] ^ ( unsigned char ) muScramble;
	mcharintScore.acValue[3] = pcharintVar->acValue[2] ^ mcharintScore.acValue[2];
	mcharintScore.acValue[1] = pcharintVar->acValue[3] ^ mcharintScore.acValue[0];
}
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char* argv[])
{
	protectedValue v;

	v.mcharintScore.iValue = 792628216;
	v.muScramble           = 3556112065;

	long vv = v.GetValue();

#ifdef SENTRY
    const bool disableSentry = DoesFileExist("no-sentry");

    if(!disableSentry){
	    sentry_options_t* options = sentry_options_new();
	    sentry_options_set_dsn(options, "https://6c9b29cfe559442b98417942e221250d@o4503905572225024.ingest.sentry.io/4503905573797888");
	    // This is also the default-path. For further information and recommendations:
	    // https://docs.sentry.io/platforms/native/configuration/options/#database-path
	    sentry_options_set_database_path(options, ".sentry-native");
	    sentry_options_set_release(options, VersionString);
	    sentry_options_set_debug(options, 0);
	    sentry_options_add_attachment(options, "debug.txt");
	    sentry_options_set_on_crash(options, [] (const sentry_ucontext_t* uctx, sentry_value_t event, void* closure) {
			    MessageBoxA(nullptr, "Airline Tycoon experienced an unexpected exception\nCrash information is being send to sentry...", "Airline Tycoon Deluxe Crash Handler", MB_OK);
    			return event;
		    }, nullptr);
	    sentry_init(options);
    }
#endif


	const char* pText = "Hallo, ich bin ein Text";

	// eigentlich wurde UCharToReadableAnsi ja geschaffen um Daten-Streams zu konvertieren, aber man kann es natürlich auch mit einem String machen!
	char* pEncoded = UCharToReadableAnsi( ( unsigned char * )pText, strlen( pText ) + 1 );
	char* pEncoded2 = UCharToReadableAnsi( ( unsigned char * )pEncoded, strlen( pEncoded ) + 1 );

	char* pDecodeBack = ( char * ) ReadableAnsiToUChar( ( char * ) pEncoded2, strlen( pEncoded2 ) + 1 );
	char* pDecodeBack2 = ( char * ) ReadableAnsiToUChar( ( char * ) pDecodeBack , strlen( pDecodeBack  ) + 1 );

	const char* pText2 = "DMCPRCZ5F5Y3D4XV1OHFY4B3HLIQJBP4LIS6STCBSQUUOKL3KSONUPTOGF2BZLXGAZGXEYLSORUW3CIHIR1W3Z3SMVXW3IGOHMAAAAIJL6STTMVMQFBKDYBGC5VP5MMWV5QJQ";   
	char* pDecodeBack3 = ( char * ) ReadableAnsiToUChar( ( char * ) pText2, strlen( pText2 ) );

	delete [] pEncoded;
	delete [] pEncoded2;
	delete [] pDecodeBack;
	delete [] pDecodeBack2;
	delete [] pDecodeBack3;

	theApp.InitInstance(argc, argv);

#ifdef SENTRY
    if (!disableSentry) {
		sentry_close();
    }
#endif

	return 0;
}

void RunLengthCompression( UCHAR *in, UCHAR *out, ULONG &size );
void RunLengthDeCompression( UCHAR *in, UCHAR *out, ULONG &size );

//--------------------------------------------------------------------------------------------
//CTakeOffApp construction:
//--------------------------------------------------------------------------------------------
CTakeOffApp::CTakeOffApp()
{
   if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0)
   {
      printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
   }

   if (TTF_Init() < 0)
   {
      printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
   }

   if (Mix_Init(MIX_INIT_OGG) < 0)
   {
       printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", TTF_GetError());
   }

   //_tmain();

   /*srand(timeGetTime());

   UBYTE Buffer [60000], Buffer2[60000];
   SLONG c, d;
   ULONG n, m;

   while (1)
   {
      n=rand()%30000;

      for (c=0; c<n; c++)
         Buffer[c]=rand()%256;

      m=(n*3/4)+(rand()%n);

      for (c=0; c<m; c++)
      {
         Buffer[rand()%n]=0;
      }

      //Chk:
      SLONG Chk=0;
      for (c=0; c<n; c++)
         Chk=Chk*3+Buffer[c];

      m=n;
      RunLengthCompression (Buffer, Buffer2, m);
      memset (Buffer, 0, 60000);
      RunLengthDeCompression (Buffer2, Buffer, m);

      if (m!=n) DebugBreak();

      SLONG Chk2=0;
      for (c=0; c<n; c++)
         Chk2=Chk2*3+Buffer[c];

      if (Chk!=Chk2) DebugBreak();
   }   */

   /*BUFFER<UBYTE> JapData (*LoadCompleteFile ("C:\\WINDOWS\\Desktop\\incub.zif\\incub.txt"));
   BUFFER<UWORD> Bits(65536);

   SLONG c,d;

   Bits.FillWith (0);

   for (c=0; c<JapData.AnzEntries(); c++)
      if (JapData[c]>=128)
      {
         Bits[(SLONG)*(UWORD*)(JapData+c)]=TRUE;
         c++;
      }

   for (c=d=0; c<Bits.AnzEntries(); c++)
      if (Bits[c]) d++;

   hprintvar (d); */

   /*BUFFER<UBYTE> FileData (*LoadCompleteFile ("test.raw"));
   BUFFER<UBYTE> Compressed;

   CompressWave (FileData, Compressed);
   DecompressWave (Compressed, FileData);

   TEAKFILE OutputFile ("test2.raw", TEAKFILE_WRITE);

   OutputFile.Write (FileData, FileData.AnzEntries());
   exit (-1); */
}

//--------------------------------------------------------------------------------------------
//CTakeOffApp deconstruction:
//--------------------------------------------------------------------------------------------
CTakeOffApp::~CTakeOffApp()
{
   if (gpClickFx) gpClickFx->Release();
   if (gpPlaneFx) gpPlaneFx->Release();

   if (gpSSE)
   {
      gpSSE->EnableSound(false);
      delete gpSSE;
      gpSSE=NULL;
   }

   bLeaveGameLoop=TRUE;

   gNetwork.DisConnect ();
}

#define LOADING_TEXT(text) \
{ \
   PrimaryBm.BlitFrom (TitleBitmap); \
   FontBigWhite.DrawTextBlock(&PrimaryBm.PrimaryBm, 2,450+8,640,480, text); \
   FrameWnd->Invalidate(); MessagePump(); \
   PrimaryBm.BlitFrom (TitleBitmap); \
   FontBigWhite.DrawTextBlock(&PrimaryBm.PrimaryBm, 2,450+8,640,480, text); \
   FrameWnd->Invalidate(); \
   MessagePump(); \
}

//--------------------------------------------------------------------------------------------
//CTakeOffApp initialization:
//--------------------------------------------------------------------------------------------
BOOL CTakeOffApp::InitInstance(int argc, char* argv[])
{
   gPhysicalCdRomBitlist.Pump();

   if (2>6)
   {
      //MySaver.Restore ("crash.dat");
   }

   SLONG mp=MAX_PATH;
   char  localVersionString[80];
   strcpy (localVersionString, VersionString);

   //Hdu.Disable();
   time_t t = time(NULL);
   Hdu.HercPrintf (0, "Airline Tycoon Deluxe logfile");
   Hdu.HercPrintf (0, VersionString);
   Hdu.HercPrintf (0, "===============================================================================");
   Hdu.HercPrintf (0, "Copyright (C) 2002 Spellbound Software");
   Hdu.HercPrintf (0, "TakeOff.Cpp was compiled at %s at %s", __DATE__, __TIME__);
   Hdu.HercPrintf (0, "===============================================================================");
   Hdu.HercPrintf (0, "logging starts %s", asctime(localtime(&t)));
   //gCDPath.Pump();

#ifdef CD_PROTECTION
   gPhysicalCdRomBitlist=GetPhysicalCdRomBitlist ();
#endif
   //gCDPath.Pump(); gCDPath.Pump(); gCDPath.Pump(); gCDPath.Pump(); gCDPath.Pump();

   pTakeOffApp = this;

   //Initialisierung:
   TopWin = NULL;
   bFullscreen = TRUE;
   bCursorCaptured = FALSE;
   gMouseStartup   = TRUE;

   //Die Standardsprachen:
   //#define LANGUAGE_D       0             //D-Deutsch, inklusive              
   //#define LANGUAGE_E       1             //E-Englisch, bezahlt               
   //#define LANGUAGE_F       2             //F-Französisch, bezahlt            
   //#define LANGUAGE_T       3             //T-Taiwanesisch, gilt als englische
   //#define LANGUAGE_P       4             //P-Polnisch, inklusive             
   //#define LANGUAGE_N       5             //N-Niederländisch, bezahlt         
   //#define LANGUAGE_I       6             //I-Italienisch, bezahlt            
   //#define LANGUAGE_S       7             //S-Spanisch, bezahlt               
   //#define LANGUAGE_O       8             //O-Portugisisch, bezahlt           
   //#define LANGUAGE_B       9             //B-Brasiliasnisch, nicht von mir   
   //#define LANGUAGE_1      10             //J-Tschechisch
   //#define LANGUAGE_2      11             //K-noch frei
   //#define LANGUAGE_3      12             //L-noch frei
   //#define LANGUAGE_4      13             //M-noch frei
   //#define LANGUAGE_5      14             //N-noch frei
   //#define LANGUAGE_6      15             //Q-noch frei
   //#define LANGUAGE_7      16             //R-noch frei
   //#define LANGUAGE_8      17             //T-noch frei
   //#define LANGUAGE_9      18             //U-noch frei
   //#define LANGUAGE_10     19             //V-noch frei

   DoAppPath();
   gLanguage=LANGUAGE_D;
   std::ifstream ifil = std::ifstream(AppPath + "misc/sabbel.dat");
   if (ifil.is_open())
   {
      ifil.read((char*)&gLanguage, sizeof(gLanguage));
      ifil.close();
   }

   gPhysicalCdRomBitlist.Pump();
   //gUpdatingPools = TRUE; //Zum testen; für Release auskommentieren

   //Flag-Ersatzstücke aus der Registry lesen:
   {
      CRegistryAccess reg (chRegKey);

      SLONG bConfigNoVgaRam      = false;
      SLONG bConfigNoSpeedyMouse = false;
      SLONG bConfigWinMouse      = false;
      SLONG bConfigNoDigiSound   = false;

      reg.ReadRegistryKey (&bConfigNoVgaRam);
      reg.ReadRegistryKey (&bConfigNoSpeedyMouse);
      reg.ReadRegistryKey (&bConfigWinMouse);
      reg.ReadRegistryKey (&bConfigNoDigiSound);

      if (bConfigNoVgaRam)       bNoVgaRam        = TRUE;
      if (bConfigNoSpeedyMouse)  bNoQuickMouse    = TRUE;
      if (bConfigWinMouse)       gUseWindowsMouse = TRUE;
      if (bConfigNoDigiSound)    Sim.Options.OptionDigiSound = FALSE;
   }

   //Schneller Mode zum Debuggen?
   for (int i = 0; i < argc; i++)
   {
      char* Argument = argv[i];
      gPhysicalCdRomBitlist.Pump();

      if (stricmp (Argument, "/fc")==0) bFirstClass = TRUE;
      if (stricmp (Argument, "/p")==0 || stricmp (Argument, "-p")==0 || stricmp (Argument, "p")==0) return (FALSE);
#ifndef CD_PROTECTION
#ifndef CD_PROTECTION_METALOCK
#ifndef  DISABLE_DEBUG_KEYS
      //if (stricmp (Argument, "/e")==0) gLanguage = LANGUAGE_E;
      //if (stricmp (Argument, "/quick")==0) bQuick = TRUE;
      //if (stricmp (Argument, "/fast")==0) bQuick = TRUE;
      //if (stricmp (Argument, "/d")==0) gLanguage = LANGUAGE_D;
      //if (stricmp (Argument, "/f")==0) gLanguage = LANGUAGE_F;
      //if (stricmp (Argument, "/test")==0) bTest = TRUE;
      if (stricmp (Argument, "/window")==0) bFullscreen = FALSE;
      //if (stricmp (Argument, "/windowed")==0) bFullscreen = FALSE;
#endif
#endif
#endif
      if (stricmp (Argument, "/novgaram")==0) bNoVgaRam = TRUE;
      if (stricmp (Argument, "/noquickmouse")==0) bNoQuickMouse = TRUE;
      if (stricmp (Argument, "/nodigisound")==0) Sim.Options.OptionDigiSound=FALSE;
      if (stricmp (Argument, "/nospeedybar")==0) bNoSpeedyBar = TRUE;
      if (stricmp (Argument, "/winmouse")==0) gUseWindowsMouse = TRUE;
      if (stricmp (Argument, "/showallpools")==0) gShowAllPools = TRUE;
      if (stricmp (Argument, "/load")==0) gLoadGameNumber = atoi(strtok (NULL, " "));
      if (stricmp (Argument, "/savegamelocal")==0) SavegamePath = "d:\\Savegame\\%s";

      if (stricmp (Argument, "/useclangli")==0) gUpdatingPools = TRUE;
      if (stricmp (Argument, "/555")==0)        MakeUnvideoOn555 = TRUE;

      if (stricmp (Argument, "/video")==0)
      {
         MakeVideoPath = strtok (NULL, " ");
         bNoVgaRam     = TRUE;
      }
      if (stricmp (Argument, "/unvideo")==0)
      {
         MakeVideoPath  = CString (":") + strtok (NULL, " ");
         MakeVideoPath2 = strtok (NULL, " ");
      }
#ifndef CD_PROTECTION
#ifndef CD_PROTECTION_METALOCK
#ifndef DISABLE_DEBUG_KEYS
      if (stricmp (Argument, "/updatepools")==0)
      {
         DoAppPath();
         InitPathVars();

         if (!InitDirectX()) return (FALSE);
         FrameWnd = new GameFrame;

         gUpdatingPools = TRUE;

         pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("glclan.gli", GliPath),   &pGLibClan, L_LOCMEM);
         pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("glstd.gli", GliPath),    &pGLibStd, L_LOCMEM);
         Clans.ReInit ("Clan.csv");
         Clans.LoadBitmaps ();

         UpdateHLinePool ();
         exit (0);
      }
#endif
#endif
#endif
   }

   Sim.Options.ReadOptions();

   gPhysicalCdRomBitlist.Pump();

   DoAppPath();
   InitPathVars();
   //UpdateSavegames ();

   bFirstClass |= !DoesFileExist(FullFilename("builds.csv", ExcelPath)) && !DoesFileExist(FullFilename("relation.csv", ExcelPath));

   gPhysicalCdRomBitlist.Pump();

   if (!InitDirectX()) return (FALSE);

   //Computername archivieren:
   #ifdef _DEBUG
   /*{
      char name[100];

      DWORD size=100;
      size=100; GetComputerName (name, &size);

      CFile File (FullFilename ("names.dat", MiscPath), CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate);

      File.SeekToEnd();
      File.Write (name, strlen (name)+1);
      File.Write ("(", 1);
      size=100; GetUserName (name, &size);
      File.Write (name, strlen (name)+1);
      File.Write (")", 1);
      File.Write ("\xd\xa", 2);

      size=100; GetComputerName (name, &size);
      if (stricmp (name, "thomas")!=0 && AppPath[0]!='J' && AppPath[0]!='\\' && AppPath[0]!='j' && !gCDFound)
      {
         CFile File ("i:\\thomas\\copier.dat", CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate);

         File.SeekToEnd();
         size=100; GetComputerName (name, &size);
         File.Write (name, strlen (name)+1);
         File.Write ("(", 1);
         size=100; GetUserName (name, &size);
         File.Write (name, strlen (name)+1);
         File.Write (")", 1);
         File.Write ("\xd\xa", 2);
      }
   }  */
   #endif

   FrameWnd = new GameFrame;

   if (MakeVideoPath.GetLength() && MakeVideoPath[0]==':')
   {
      Unvideo (MakeVideoPath.Mid(1), MakeVideoPath2);
      exit (-1);
   }

   PrimaryBm.Clear();
   FrameWnd->Invalidate(); MessagePump();
   PrimaryBm.Clear();
   FrameWnd->Invalidate(); MessagePump();

   gPhysicalCdRomBitlist.Pump();

   MessagePump();
   InitFonts();

   //Waiting Area
   {
      CWait    Waiting;
      GfxLib  *pRoomLib;
      GfxLib  *pRoomLib2;
      SBBM     TitleBitmap;

      pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("titel.gli", RoomPath),  &pRoomLib, L_LOCMEM);
      pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("titel2.gli", RoomPath),  &pRoomLib2, L_LOCMEM);

      if (Sim.Options.OptionDigiSound==TRUE)
         InitSoundSystem (FrameWnd->m_hWnd);

      if (Sim.Options.OptionViewedIntro==0 && IntroPath.GetLength()!=0)
      {
         Sim.Gamestate = GAMESTATE_INTRO | GAMESTATE_WORKING;
         TopWin = new CIntro(FALSE, 0);
         TitleBitmap.ReSize (pRoomLib, GFX_TITEL);

         while (Sim.Gamestate!=GAMESTATE_BOOT)
         {
            FrameWnd->Invalidate();
            MessagePump();
            SDL_Delay (10);
         }

         delete TopWin;
         TopWin=NULL;
      }
      else
         TitleBitmap.ReSize (pRoomLib, GFX_SPELLOGO);

      PrimaryBm.BlitFrom (TitleBitmap);
      gMousePosition=XY(600,440); FrameWnd->Invalidate(); MessagePump();
      PrimaryBm.BlitFrom (TitleBitmap);
      gMousePosition=XY(600,440); FrameWnd->Invalidate(); MessagePump();
      PrimaryBm.BlitFrom (TitleBitmap);
      gMousePosition=XY(600,440); FrameWnd->Invalidate(); MessagePump();

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Karakters worden opgestart...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Loading People...");

      if (gUpdatingPools)
         pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("glclan.gli", GliPath),   &pGLibClan, L_LOCMEM);

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Adviseurs worden opgestart...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Loading Advisors...");
      pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("glberatr.gli", GliPath),   &pGLibBerater, L_LOCMEM);

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Verscheidene afbeeldingen worden opgestart...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Loading miscellanous grafix...");

      pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("glstd.gli", GliPath),    &pGLibStd, L_LOCMEM);

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Vliegtuigen worden opgestart...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Loading planes...");
      pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("glplanes.gli", GliPath), &pGLibPlanes, L_LOCMEM);

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Dialoogteksten worden opgestart...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Loading dialogue texts...");
      DialogTexte.Open (FullFilename ("Dlg_Ger.res", MiscPath), TEXTRES_CACHED);

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Verscheidene teksten worden opgestart...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Loading miscellanous texts...");
      StandardTexte.Open (FullFilename ("Std_Ger.res", MiscPath), TEXTRES_CACHED);

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Eenheidteksten worden opgestart...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Loading unit texts...");
      InitEinheiten (FullFilename ("ein_ger.res", MiscPath));

      //Großes und kleines Icon setzen:
      //FrameWnd->SetIcon (m_hBigIcon = LoadIcon(IDR_MAINFRAME1), 1);
      //FrameWnd->SetIcon (m_hSmallIcon = LoadIcon(IDR_MAINFRAME), 0);

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Initialiseren...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Initializing...");
      InitItems ();

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Initialiseert de globe...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Initializing globe...");
      InitGlobeMapper ();

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Initialiseert statusbalk...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Initializing status bar...");
      InitStatusLines ();

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Initialiseert scherm...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Initializing screen...");

      CreditsSmackerFileHandle = fopen (gCDPath+FILLFILE_NAME, "rb");

      TitleBitmap.ReSize (pRoomLib, GFX_TITEL);
      PrimaryBm.BlitFrom (TitleBitmap);
      gMousePosition=XY(600,440); FrameWnd->Invalidate(); MessagePump();
      PrimaryBm.BlitFrom (TitleBitmap);
      gMousePosition=XY(600,440); FrameWnd->Invalidate(); MessagePump();
      PrimaryBm.BlitFrom (TitleBitmap);
      gMousePosition=XY(600,440); FrameWnd->Invalidate(); MessagePump();

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Zoekt midi-apparaat...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Looking for midi device...");

      //Hamma Midi?
      FrameWnd->Invalidate(); MessagePump(); //lpDD->FlipToGDISurface();
      bMidiAvailable = IsMidiAvailable();

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Initialiseert geluidssysteem...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Initializing music sound system...");

      if (bMidiAvailable && gpSSE)
      {
         FrameWnd->Invalidate(); MessagePump(); //lpDD->FlipToGDISurface();
		 gpSSE->CreateMidi(&gpMidi);
      	
         if (gpMidi)
         {
                 if (gLanguage==LANGUAGE_N) LOADING_TEXT("Stelt het midi-volume in...")
            else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
            else                            LOADING_TEXT("Setting midi volume...");

            FrameWnd->Invalidate(); MessagePump(); //lpDD->FlipToGDISurface();
            SetMidiVolume(Sim.Options.OptionMusik);
            gpMidi->SetMode(Sim.Options.OptionMusicType);

            if (Sim.Options.OptionViewedIntro)
            {
                    if (gLanguage==LANGUAGE_N) LOADING_TEXT("Start de eerste midi...")
               else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
               else                            LOADING_TEXT("Starting first midi...");

               FrameWnd->Invalidate(); MessagePump(); //lpDD->FlipToGDISurface();
               NextMidi ();

                    if (gLanguage==LANGUAGE_N) LOADING_TEXT("Herstelt het midi-volume...")
               else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
               else                            LOADING_TEXT("Resetting midi volume...");

               FrameWnd->Invalidate(); MessagePump(); //lpDD->FlipToGDISurface();
               SetMidiVolume(Sim.Options.OptionMusik);
            }
         }
         else bMidiAvailable=FALSE;
      }

      //Registration.ReSize ("Misc\\Register.res", 0x54a8fe83);

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Initialiseert de karakterdata...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Initializing people data...");
      Clans.ReInit ("Clan.csv");
      Clans.UpdateClansInGame (TRUE);

      if (pRoomLib2 && gLanguage!=LANGUAGE_N)
      {
         TitleBitmap.ReSize (pRoomLib2, GFX_TITEL);
         PrimaryBm.BlitFrom (TitleBitmap);
         gMousePosition=XY(600,440); FrameWnd->Invalidate(); MessagePump();
         PrimaryBm.BlitFrom (TitleBitmap);
         gMousePosition=XY(600,440); FrameWnd->Invalidate(); MessagePump();
         PrimaryBm.BlitFrom (TitleBitmap);
         gMousePosition=XY(600,440); FrameWnd->Invalidate(); MessagePump();
      }

      if (!gUpdatingPools)
      {
              if (gLanguage==LANGUAGE_N) LOADING_TEXT("Initialiseert de röntgenfoto's...")
         else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
         else                            LOADING_TEXT("Initializing roentgens...");
         SkelettPool.ReSize ("Skelett.pol", NULL, NULL);
         SkelettPool.Load();

         SLONG n=0;

         for (SLONG c=Clans.AnzEntries()-1; c>=0; c--, n++)
            if (Clans.IsInAlbum (c))
            {
               if (c<SLONG(Clans.AnzEntries()-1) && Clans.IsInAlbum(c+1) && Clans[c].PalFilename==Clans[SLONG(c+1)].PalFilename && Clans[c].Type!=30)
               {
                  Clans[c].ClanPool.ReSize (bprintf ("clan%li.pol", c), &Clans[SLONG(c+1)].ClanPool, NULL);
                  Clans[c].ClanGimmick.ReSize (bprintf ("clang%li.pol", c), &Clans[SLONG(c+1)].ClanPool, &Clans[c].ClanPool);
                  Clans[c].ClanWarteGimmick.ReSize (bprintf ("clanw%li.pol", c), &Clans[SLONG(c+1)].ClanPool, &Clans[c].ClanPool);
               }
               else
               {
                  Clans[c].ClanPool.ReSize (bprintf ("clan%li.pol", c), NULL, NULL);
                  Clans[c].ClanGimmick.ReSize (bprintf ("clang%li.pol", c), &Clans[c].ClanPool, NULL);
                  Clans[c].ClanWarteGimmick.ReSize (bprintf ("clanw%li.pol", c), &Clans[c].ClanPool, &Clans[c].ClanGimmick);
               }

               Clans[c].ClanWarteGimmick.PreLoad();
               Clans[c].ClanGimmick.PreLoad();

               if (Clans[c].TodayInGame) Clans[c].ClanPool.Load();
                                    else Clans[c].ClanPool.PreLoad();

                    if (gLanguage==LANGUAGE_N) LOADING_TEXT((char*)(LPCTSTR)(CString("Karakterdata wordt opgestart...")+CString("................................................................").Left(n/4)))
               else if (gLanguage==LANGUAGE_F) LOADING_TEXT((char*)(LPCTSTR)(CString("Initializing people data...")+CString("................................................................").Left(n/4)))
               else                            LOADING_TEXT((char*)(LPCTSTR)(CString("Initializing people data...")+CString("................................................................").Left(n/4)));
               n++;
            }
      }

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Karakterdata wordt opgestart...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Loading people data...");
      Clans.LoadBitmaps ();

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Initialiseert de steden...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Initializing cities...");
      Cities.ReInit ("City.csv");

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Initialiseert de vliegtuigen...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Initializing planes...");
      PlaneTypes.ReInit ("PlaneTyp.csv");

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Initialiseert de namen...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Initializing names...");
      PlaneNames.ReInit ("PNames.csv");

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Initialiseert de kranten...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Initializing newspapers...");
      Sim.Headlines.ReInit ("StdPaper.csv");

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Initialiseert de tips...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Francais...")
      else                            LOADING_TEXT("Initializing tooltips...");
      InitTipBms ();

           if (gLanguage==LANGUAGE_N) LOADING_TEXT("Spel wordt gestart...")
      else if (gLanguage==LANGUAGE_F) LOADING_TEXT("Lancer le jeu...")
      else                            LOADING_TEXT("Starting game...");

      TitleBitmap.Destroy();
      if (pRoomLib && pGfxMain) pGfxMain->ReleaseLib (pRoomLib);
      if (pRoomLib2 && pGfxMain) pGfxMain->ReleaseLib (pRoomLib2);
   }

   gMouseStartup = FALSE;
   pCursor->SetImage (gCursorBm.pBitmap);
   pCursor->Show(TRUE);

   GameLoop (NULL);
	return FALSE;
}

//--------------------------------------------------------------------------------------------
//Sorgt für Screen Refresh und für Ablauf der Simulation:
//--------------------------------------------------------------------------------------------
void CTakeOffApp::GameLoop(void*)
{
   SLONG c, d, e;
   DWORD LastTime=0xffffffff;
   DWORD Time, NumSimSteps=0;
   SLONG Faktor=1;
   BOOL  RefreshNeccessary=FALSE;

   DWORD SimStepsCounter;        //Zählt wieviele SimSteps an einem Stück gemacht wurden um ab&zu einen ScreenRefresh zu erzwingen

   Sim.TimeSlice = 0;

   int startTime = 0, lastTime = 0;
   while (!bLeaveGameLoop)
   {
      Time= SDL_GetTicks();
      startTime = Time;

      int timerFps = Time;
      if (LastTime==0xffffffff || bgJustDidLotsOfWork || bActive==FALSE) LastTime=Time;

      bgJustDidLotsOfWork=FALSE;

      if (bActive)
      {
         Faktor=1;
         RefreshNeccessary=FALSE;
         if (((Time-LastTime)/50)>0)
         {
            DWORD tmp;

            tmp          = (Time-LastTime)/50;    //20 Schritte pro Sekunde
            NumSimSteps += tmp;
            LastTime    += 50*tmp;
         }

         if (Sim.Gamestate==GAMESTATE_BOOT)
         {
            if (TopWin) delete TopWin;
            TopWin=NULL;

            for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
            {
               if (Sim.Players.Players[c].LocationWin)
               {
                  delete Sim.Players.Players[c].LocationWin;
                  Sim.Players.Players[c].LocationWin=NULL;
               }
               if (Sim.Players.Players[c].DialogWin)
               {
                  delete Sim.Players.Players[c].DialogWin;
                  Sim.Players.Players[c].DialogWin=NULL;
               }
            }

            //Titelmenü anzeigen:
            if (Sim.Options.OptionViewedIntro || IntroPath.GetLength()==0)
            {
               Sim.Gamestate = GAMESTATE_TITLE | GAMESTATE_WORKING;
               TopWin = new TitlePopup(FALSE, 0);
            }
            else
            {
               Sim.Gamestate = GAMESTATE_INTRO | GAMESTATE_WORKING;
               TopWin = new CIntro(FALSE, 0);
            }
         }

         if (Sim.Gamestate==(GAMESTATE_TITLE | GAMESTATE_DONE))
         {
            //New Game -> Abfragen für Spielmodus:
            CStdRaum *TmpWin = TopWin; TopWin=NULL;
            delete TmpWin; 

            Sim.Gamestate = GAMESTATE_INIT | GAMESTATE_WORKING;
            TopWin = new NewGamePopup(FALSE, 0);
         }
         else if (Sim.Gamestate==(GAMESTATE_CREDITS))
         {
            CStdRaum *TmpWin = TopWin; TopWin=NULL; delete TmpWin; 
            Sim.Gamestate = GAMESTATE_INIT | GAMESTATE_WORKING;
            TopWin = new CCredits(FALSE, 0);
         }
         else if (Sim.Gamestate==(GAMESTATE_OPTIONS))
         {
            CStdRaum *TmpWin = TopWin; TopWin=NULL; delete TmpWin; 
            Sim.Gamestate = GAMESTATE_INIT | GAMESTATE_WORKING;
            TopWin = new Options(FALSE, 0);
         }
         else if (Sim.Gamestate==GAMESTATE_INTRO)
         {
            CStdRaum *TmpWin = TopWin; TopWin=NULL; delete TmpWin; 
            Sim.Gamestate = GAMESTATE_INTRO | GAMESTATE_WORKING;
            TopWin = new CIntro(FALSE, 0);
         }
         else if (Sim.Gamestate==GAMESTATE_OUTRO || Sim.Gamestate==GAMESTATE_OUTRO2)
         {
            for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
            {
               if (Sim.Players.Players[c].LocationWin)
               {
                  delete Sim.Players.Players[c].LocationWin;
                  Sim.Players.Players[c].LocationWin=NULL;
               }
               if (Sim.Players.Players[c].DialogWin)
               {
                  delete Sim.Players.Players[c].DialogWin;
                  Sim.Players.Players[c].DialogWin=NULL;
               }
            }

            CStdRaum *TmpWin = TopWin; TopWin=NULL; delete TmpWin; 
            Sim.Gamestate |= GAMESTATE_WORKING;

            if ((Sim.Gamestate & (~GAMESTATE_WORKING))==GAMESTATE_OUTRO)
               TopWin = new COutro(FALSE, 0, "outro.smk");
            else
               TopWin = new COutro(FALSE, 0, "outro2.smk");
         }
         else if (Sim.Gamestate==(GAMESTATE_INIT | GAMESTATE_DONE))
         {
            //Das Spielfenster mit Flughafensicht
            CStdRaum *TmpWin = TopWin; TopWin=NULL;
            delete TmpWin; 

            if (gLoadGameNumber>-1)
               Sim.LoadGame (gLoadGameNumber-1);

            if (gLoadGameNumber==-1)
            {
               for (c=0; c<Sim.Players.AnzPlayers; c++)
                  Sim.Players.Players[c].EnterRoom (ROOM_AIRPORT, true);

               UpdateStatusBar();
               Sim.Gamestate = GAMESTATE_PLAYING | GAMESTATE_WORKING;
               Sim.DayState  = 1;

               if (1==0)
               {
                  //Speedup zum testen; für Release beides auskommentieren:
                  Sim.IsTutorial = FALSE;
                  Sim.bNoTime    = FALSE;
                  Sim.DayState   = 2;
               }
               else
               {
                  if (Sim.Difficulty==DIFF_TUTORIAL) Sim.IsTutorial = TRUE;
                  MouseWait++;
               }

               NumSimSteps=0; //Noch nicht mit SimSteps beginnen

               for (c=0; c<Sim.Players.AnzPlayers; c++)
                  Sim.Players.Players[c].WalkToRoom (ROOM_AUFSICHT);
            }

            gLoadGameNumber=-1;
         }

         if (Sim.Gamestate == (GAMESTATE_PLAYING | GAMESTATE_WORKING))
         {
            //Feierabend berechnen:
            if (Sim.bNetwork)
            {
               if (Sim.CallItADay==FALSE && Sim.bIsHost && Sim.CallItADayAt==0)
               {
                  Sim.CallItADay=TRUE;
                  for (c=0; c<Sim.Players.AnzPlayers; c++)
                  {
                     Sim.CallItADay &= (Sim.Players.Players[c].CallItADay | (Sim.Players.Players[c].Owner==1));

                     if (Sim.Players.Players[c].IsOut==0 && Sim.Players.Players[c].Owner==1 && Sim.Players.Players[c].WaitWorkTill!=-1)
                        Sim.CallItADay=FALSE;
                  }

                  if (Sim.CallItADay)
                  {
                     if (Sim.Options.OptionAutosave && Sim.bNetwork)
                        Sim.SaveGame (11, StandardTexte.GetS (TOKEN_MISC, 5000));

                     Sim.CallItADayAt=Sim.TimeSlice+30;
                     Sim.SendSimpleMessage (ATNET_DAYFINISHALL, NULL, Sim.CallItADayAt);
                     Sim.CallItADay=false;
                  }
               }
            }
            else
               Sim.CallItADay=Sim.Players.Players[Sim.localPlayer].CallItADay;

            //Dinge, die nicht beschleunigt werden:
            if (nOptionsOpen==0 && nWaitingForPlayer==0 && !Sim.bPause && !Sim.CallItADay && Sim.Time<18*60000 && Sim.Time>=9*60000)
            {
               for (c=min(NumSimSteps,400); c>0; c--)
                  Sim.Players.MessagePump();
            }

            for (c=0; c<Sim.Players.AnzPlayers; c++)
            {
               //Klickt gerade jemand ins Zeit-Fenster?
               if (!Sim.CallItADay && !Sim.bNetwork && XY(Sim.Players.Players[c].CursorPos).IfIsWithin (102, 440, 255, 459))
               {
                  if (Sim.Players.Players[c].Buttons&1) { NumSimSteps *= 5;  Faktor*=5; }
                  if (Sim.Players.Players[c].Buttons&2) { NumSimSteps *= 20; Faktor*=20; }
               }

               if (Sim.Players.Players[c].WaitForRoom && !IsRoomBusy(Sim.Players.Players[c].WaitForRoom,c))
               {
                  Sim.Players.Players[c].WalkToRoom (UBYTE(Sim.Players.Players[c].WaitForRoom));
               }
            }

            //Tutorium:
            if (Sim.IsTutorial)
            {
               PLAYER &qLocalPlayer = Sim.Players.Players[Sim.localPlayer];

               if (Sim.Tutorial==999)
               {
                  Sim.Tutorial=1000;
                  qLocalPlayer.Messages.IsMonolog = TRUE;
                  qLocalPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 1000));
                  qLocalPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 1001));
               }
               else if (Sim.Tutorial==1000 && qLocalPlayer.Messages.IsSilent ())
               {
                  Sim.Tutorial=1100;
                  qLocalPlayer.Messages.IsMonolog = FALSE;
                  qLocalPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 1100));
                  qLocalPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 1101));
               }
               else if (Sim.Tutorial==1100 && qLocalPlayer.Messages.IsSilent () && qLocalPlayer.GetRoom()==ROOM_AUFSICHT)
               {
                  Sim.Tutorial=1200+30;
                  qLocalPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 1200));
                  qLocalPlayer.Messages.Pump ();
               }
               else if (Sim.Tutorial==1200+30 && qLocalPlayer.Messages.IsSilent ())
               {
                  for (SLONG c=9; c>=0; c--)
                     if (qLocalPlayer.Locations[c]==ROOM_AUFSICHT) break;

                  if (c>=0)
                  {
                     Sim.Tutorial=1200+40;
                     (*(CStdRaum*)qLocalPlayer.LocationWin).StartDialog (TALKER_BOSS, MEDIUM_AIR, 1);
                     (*(CStdRaum*)qLocalPlayer.LocationWin).DontDisplayPlayer=Sim.localPlayer;
                  }
               }
               else if (Sim.Tutorial==1200+40 && qLocalPlayer.GetRoom()==ROOM_AIRPORT)
               {
                  Sim.bNoTime   = FALSE;
                  Sim.DayState  = 2;
                  if (qLocalPlayer.GetRoom()!=ROOM_AIRPORT) qLocalPlayer.LeaveRoom();

                  Sim.Tutorial=1310;
                  qLocalPlayer.Messages.NextMessage();
                  qLocalPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 1300));
                  qLocalPlayer.WalkToRoom (UBYTE(ROOM_BURO_A+Sim.localPlayer*10));
               }
               else if (Sim.Tutorial==1310 && qLocalPlayer.GetRoom()!=ROOM_AIRPORT)
               {
                  Sim.Tutorial=1400;
                  qLocalPlayer.Messages.NextMessage();
                  qLocalPlayer.Messages.AddMessage (BERATERTYP_GIRL, bprintf (StandardTexte.GetS (TOKEN_TUTORIUM, 1400), (LPCTSTR)qLocalPlayer.AirlineX));
               }
               else if (Sim.Tutorial==1400 && qLocalPlayer.Messages.Messages[0].Message.GetLength()==0 && (qLocalPlayer.Messages.IsSilent () || qLocalPlayer.Messages.TalkCountdown<=1))
               {
                  if (qLocalPlayer.LocationWin)
                  {
                     CStdRaum &qRaum = *((CStdRaum*)qLocalPlayer.LocationWin);

                     qRaum.GlowEffects.ReSize(1);
                     qRaum.GlowBitmapIndices.ReSize(1);
                     qRaum.GlowEffects[0]=XY(311,165);
                     qRaum.GlowBitmapIndices[0]=0;
                  }

                  Sim.Tutorial=1401;
                  qLocalPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 1401));
               }
               else if (Sim.Tutorial==1401 && qLocalPlayer.GetRoom()==ROOM_GLOBE)
               {
                  if (qLocalPlayer.LocationWin)
                     ((CStdRaum*)qLocalPlayer.LocationWin)->GlowEffects.ReSize(0);

                  Sim.Tutorial=1500;
                  qLocalPlayer.Messages.NextMessage();
                  qLocalPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 1500));
               }
               else if (Sim.Tutorial==1500)
               {
                  Sim.Tutorial=1501;
                  qLocalPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 1501));
               }
               else if (Sim.Tutorial==1501 && qLocalPlayer.Messages.IsSilent ())
               {
                  if (qLocalPlayer.LocationWin)
                  {
                     CStdRaum &qRaum = *((CStdRaum*)qLocalPlayer.LocationWin);

                     qRaum.GlowEffects.ReSize(1);
                     qRaum.GlowBitmapIndices.ReSize(1);
                     qRaum.GlowEffects[0]=XY(20+20,248+11);
                     qRaum.GlowBitmapIndices[0]=1;
                  }

                  Sim.Tutorial=1502;
                  qLocalPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 1502));
               }
               else if (Sim.Tutorial==1507 && qLocalPlayer.GetRoom()==ROOM_AIRPORT)
               {
                  Sim.Tutorial=1600;
                  qLocalPlayer.Messages.NextMessage();
                  qLocalPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 1600));
                  qLocalPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 1601));
               }
               else if (Sim.Tutorial==2000 && qLocalPlayer.Messages.IsSilent ())
               {
                  Sim.Tutorial=2002;
                  qLocalPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 2002));
                  qLocalPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 2003));
               }
               else if (Sim.Tutorial==2002 && qLocalPlayer.Messages.IsSilent ())
               {
                  qLocalPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 2004));
                  qLocalPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 2005));
                  qLocalPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 2006));
                  qLocalPlayer.Messages.IsMonolog = FALSE;
                  Sim.IsTutorial=FALSE;
               }
            }

            //CurrentGameSpeed:
            d=3;
            for (c=0; c<Sim.Players.AnzPlayers; c++)
            {
               if (Sim.Players.Players[c].Owner!=1 && Sim.Players.Players[c].CallItADay==FALSE)
                  d = min (d, Sim.Players.Players[c].GameSpeed);
            }

            if (Sim.CallItADay || Sim.Time<9*60000) d=5;
            if ((Sim.Time<8*60000 || Sim.Time>=18*60000) && Sim.DayState==4) d=5;

            if (bgWarp)
            {
               PERSON &qPerson = Sim.Persons[Sim.Persons.GetPlayerIndex (Sim.localPlayer)];

               if (Sim.Players.Players[Sim.localPlayer].GetRoom()!=ROOM_AIRPORT ||
                   (qPerson.Dir==8 && Sim.Players.Players[Sim.localPlayer].PrimaryTarget==Sim.Players.Players[Sim.localPlayer].TertiaryTarget))
               {
                  bgWarp = FALSE;

                  if (Sim.Players.Players[Sim.localPlayer].LocationWin)
                     ((AirportView*)Sim.Players.Players[Sim.localPlayer].LocationWin)->CenterCameraOnPlayer();
               }

               if (bgWarp) d=4;
            }

            {
               SLONG Multiplier=1;

               switch (d)
               {
                  case 1: Multiplier = 2;   break;
                  case 2: if (Sim.bNetwork) Multiplier = 3; else Multiplier = 4;   break;
                  case 3: if (Sim.bNetwork) Multiplier = 4; else Multiplier = 8;   break;
                  case 4: Multiplier = 150; break;
                  case 5: Multiplier = 600; break;
               }
               NumSimSteps *= Multiplier;
               Faktor      *= Multiplier;
            }

            if (Sim.bNetwork)
            {
               //Synchronisierung morgends:
               if (Sim.GetHour()>=9 && Sim.bWatchForReady && Sim.CallItADay==0)
               {
                  if (Sim.Players.Players[Sim.localPlayer].bReadyForMorning==false && (Sim.Players.Players[Sim.localPlayer].GetRoom()!=ROOM_AUFSICHT || Sim.GetHour()>9 || Sim.GetMinute()>0))
                  {
                     Sim.SendSimpleMessage (ATNET_READYFORMORNING, NULL, Sim.localPlayer);
                     Sim.Players.Players[Sim.localPlayer].bReadyForMorning=true;
                  }

                  if (Sim.bWatchForReady && (Sim.Players.Players[Sim.localPlayer].GetRoom()!=ROOM_AUFSICHT || (Sim.Players.Players[Sim.localPlayer].GetRoom()==ROOM_AUFSICHT && ((CAufsicht*)Sim.Players.Players[Sim.localPlayer].LocationWin)->bExitASAP==true)) && Sim.Players.Players[Sim.localPlayer].LocationWin)
                  {
                     for (c=0; c<4; c++)
                        if (Sim.Players.Players[c].bReadyForMorning==false && Sim.Players.Players[c].Owner!=1)
                        {
                           NumSimSteps=0;

                           if (Sim.Time==9*60000) SetNetworkBitmap (2);
                        }

                     if (NumSimSteps)
                     {
                        Sim.bWatchForReady=FALSE;
                        SetNetworkBitmap (0);

                        //wirkt wie bgJustDidLotsOfWork:
                        LastTime    = Time;
                        NumSimSteps = min(1, NumSimSteps);
                     }
                  }
                  else if (Sim.Players.Players[Sim.localPlayer].LocationWin==0)
                  {
                     NumSimSteps = min(1, NumSimSteps);
                  }
               }

               //Synchronisierung beim Feierabend:
               if (Sim.GetHour()>=9 && Sim.GetHour()<18 && Sim.CallItADay==1)
               {
                  if (Sim.Players.GetAnzRobotPlayers()>0)
                  {
                     if (Sim.bIsHost)
                     {
                        //Host: Nicht weitermachen, wenn ein Client noch nicht so weit ist:
                        for (c=0; c<4; c++)
                           if (Sim.Players.Players[c].bReadyForMorning==false && Sim.Players.Players[c].Owner==2 && !Sim.Players.Players[c].IsOut)
                              NumSimSteps = 0;
                     }
                     else if (!Sim.b18Uhr)
                     {
                        //Client: Maximal bis zum Punkt rechnen, den der Host vorgegeben hat:
                        SLONG LeastWaitTill=-1;

                        for (c=0; c<4; c++)
                           if (Sim.Players.Players[c].Owner==1 && !Sim.Players.Players[c].IsOut)
                              if (LeastWaitTill==-1 || (Sim.Players.Players[c].WaitWorkTill<LeastWaitTill && Sim.Players.Players[c].WaitWorkTill!=-1))
                                 LeastWaitTill=Sim.Players.Players[c].WaitWorkTill;

                        if (LeastWaitTill==-1)
                           NumSimSteps=0;
                        else if (Sim.TimeSlice>LeastWaitTill)
                           NumSimSteps=1;
                        else
                           NumSimSteps=min(NumSimSteps, ULONG(LeastWaitTill-Sim.TimeSlice+1));
                     }
                  }
                  if (NumSimSteps==0 && bCAbendOpen==0) NumSimSteps=1;
               }
               else if (Sim.GetHour()>=18 && Sim.b18Uhr==FALSE && Sim.bIsHost)
               {
                  Sim.b18Uhr=TRUE;
                  Sim.SendSimpleMessage (ATNET_PLAYER_18UHR);

                  for (c=0; c<4; c++)
                     Sim.Players.Players[c].bReadyForMorning=false;
               }

               if (Sim.GetHour()>=19) Sim.b18Uhr=TRUE;
            }
            else
            {
               NumSimSteps=min((SLONG)NumSimSteps, Faktor);
            }

            if (Sim.Time>8*60000 && CheatTestGame==2 && !Sim.Players.Players[Sim.localPlayer].CallItADay)
            {
               if (Sim.Date%10==0)
                  Sim.SaveGame (11, StandardTexte.GetS (TOKEN_MISC, 5000));

               Sim.Players.Players[Sim.localPlayer].CallItADay=TRUE;
            }

            //Im Netzwerk die Zeit aufgrund der ATNET_TIMEPING Nachricht fliessend anpassen
            if (Sim.bNetwork && NumSimSteps && gTimerCorrection)
            {
               static UBYTE MyPrivateRandom=0;

               MyPrivateRandom++;

               if (((MyPrivateRandom&1)==0 && abs(gTimerCorrection)>10) || (MyPrivateRandom&7)==0)
               {
                  if (gTimerCorrection>0)
                  {
                     NumSimSteps++;
                     gTimerCorrection--;
                  }
                  else if (gTimerCorrection<0)
                  {
                     NumSimSteps--;
                     gTimerCorrection++;
                  }
               }
            }

            //Sind noch Simulationsschritte offen ?
            if (NumSimSteps>0 && (Sim.Gamestate==(GAMESTATE_PLAYING|GAMESTATE_WORKING) || Sim.Gamestate==(GAMESTATE_QUIT|GAMESTATE_WORKING)))
            {
               RefreshNeccessary=TRUE;
               SimStepsCounter=0;

               SLONG NumSimStepsBegin = NumSimSteps;

               while (NumSimSteps>0 && (SimStepsCounter<400 || (SimStepsCounter<1600 && (Sim.CallItADay || Sim.Time>18*60000 || Sim.Time<9*60000))))
               {
                  //Synchronisierung beim Feierabend:
                  if (Sim.GetHour()>=9 && Sim.GetHour()<18 && Sim.CallItADay==1 && Sim.bIsHost && NumSimStepsBegin!=1)
                  {
                     for (c=0; c<4; c++)
                        if (Sim.Players.Players[c].bReadyForMorning==false && Sim.Players.Players[c].Owner==2 && !Sim.Players.Players[c].IsOut)
                           NumSimSteps = 0;

                     if (NumSimSteps == 0) break;
                  }

                  if (Sim.CallItADayAt && Sim.CallItADay==FALSE)
                  {
                     if (Sim.TimeSlice>=Sim.CallItADayAt)
                        Sim.CallItADay=TRUE;
                     else
                     {
                        NumSimSteps=min(NumSimSteps, ULONG(Sim.CallItADayAt-Sim.TimeSlice));
                        if (NumSimSteps<0) NumSimSteps=0;
                     }
                  }

                  if (nOptionsOpen == 0 && Sim.Players.Players[Sim.localPlayer].IsDrunk>0 && ((SDL_GetWindowFlags(FrameWnd->m_hWnd) & SDL_WINDOW_MOUSE_FOCUS) == SDL_WINDOW_MOUSE_FOCUS))
                  {
                     int mouseX = 0,mouseY = 0;
                     SDL_GetGlobalMouseState(&mouseX, &mouseY);

                     SDL_WarpMouseGlobal(SLONG(mouseX +sin(Sim.TimeSlice*70/200.0)*cos(Sim.TimeSlice*70/160.0)*Sim.Players.Players[Sim.localPlayer].IsDrunk/30), SLONG(mouseY +cos(Sim.TimeSlice*70/230.0)*sin(Sim.TimeSlice*70/177.0)*Sim.Players.Players[Sim.localPlayer].IsDrunk/30));
                     Sim.Players.Players[Sim.localPlayer].IsDrunk--;
                  }

                  if ((Sim.Time>=9*60000 && Sim.Time<=18*60000 && !Sim.CallItADay) || (Sim.Players.Players.AnzEntries()>0 && Sim.Players.Players[Sim.localPlayer].GetRoom()!=ROOM_ABEND && bCAbendOpen==0))
                     for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                     {
                        PLAYER &qPlayer = Sim.Players.Players[c];

                        if (qPlayer.Koffein>0) qPlayer.Koffein--;

                        if ((Sim.Time&31)==0) qPlayer.CalcRoom ();

                        //Veränderung am Second-Level-Raum?
                        if (Sim.Difficulty!=DIFF_TUTORIAL || Sim.Tutorial>=1200 || Sim.IsTutorial==0 || Sim.Tutorial==0)
                        {
                           if (qPlayer.Owner == 0) //Nur Spieler bekommen Fenster
                           {
                              BOOL JustLeftRoom=FALSE;

                              //Raum verlassen...?
                              for (d=0; d<10; d++)
                              {
                                 if (qPlayer.Locations[d] & ROOM_LEAVING)
                                 {
                                    if (Sim.RoomBusy[qPlayer.Locations[d]&255]>0)
                                       Sim.RoomBusy[qPlayer.Locations[d]&255]--;

                                    if (qPlayer.LocationWin) 
                                    {
                                       JustLeftRoom=TRUE;
                                       if (Sim.Options.OptionBlenden && qPlayer.Locations[d]!=ROOM_LAPTOP && (Sim.GetHour()>9 || Sim.Date==0))
                                       {
                                          gBlendState=-1;
                                          if (FrameWnd) FrameWnd->PrepareFade();
                                          FrameWnd->Invalidate(); MessagePump();
                                          if (Sim.Players.Players.AnzEntries()==0) break;
                                       }

                                       bIngnoreNextDoor=TRUE;

                                       delete qPlayer.LocationWin;
                                       qPlayer.LocationWin = NULL;
                                    }

                                    qPlayer.Locations[d] = 0;

                                    if (d>0 && qPlayer.Locations[d-1]!=0)
                                       qPlayer.Locations[d-1] = USHORT((qPlayer.Locations[d-1] & (~ROOM_LEAVING)) | ROOM_ENTERING);

                                    qPlayer.CalcRoom ();
                                    qPlayer.BroadcastPosition ();
                                    qPlayer.BroadcastRooms (ATNET_LEAVEROOM);
                                    Sim.UpdateRoomUsage ();
                                 }
                              }
                              if (Sim.Players.Players.AnzEntries()==0) break;

                              //Raum betreten...?
                              for (d=0; d<10; d++)
                              {
                                 if ((qPlayer.Locations[d] & ROOM_ENTERING) && (qPlayer.Locations[d] & ROOM_LEAVING))
                                    qPlayer.Locations[d]=0;

                                 if (qPlayer.Locations[d] & ROOM_ENTERING)
                                 {
                                    if (qPlayer.LocationWin) 
                                    {
                                       if (!JustLeftRoom && Sim.Options.OptionBlenden && qPlayer.Locations[d]!=ROOM_LAPTOP && qPlayer.Locations[d]!=ROOM_GLOBE && (Sim.GetHour()>9 || Sim.Date==0))
                                       {
                                          gBlendState=-1;
                                          if (FrameWnd) FrameWnd->PrepareFade();
                                          FrameWnd->Invalidate(); MessagePump();
                                          if (Sim.Players.Players.AnzEntries()==0) break; //Spiel beendet
                                       }

                                       delete qPlayer.LocationWin;
                                       qPlayer.LocationWin = NULL;
                                    }

                                    qPlayer.Locations[d] &= ~ROOM_ENTERING;
                                    gMouseScroll = 0;

                                    if (qPlayer.Locations[d]!=ROOM_LAPTOP && gBlendBm.Size.y && (Sim.GetHour()>9 || Sim.Date==0))
                                       gBlendState=-2;

                                    SLONG Room = (qPlayer.Locations[d] & ~ROOM_LEAVING);
                                    if (Room==ROOM_ARAB_AIR || Room==ROOM_ROUTEBOX || Room==ROOM_KIOSK || Room==ROOM_RICKS || Room==ROOM_LAST_MINUTE || Room==ROOM_REISEBUERO)
                                    {
                                       if (Room==ROOM_ARAB_AIR || Room==ROOM_ROUTEBOX || Room==ROOM_KIOSK || Room==ROOM_RICKS)
                                          Sim.Persons[Sim.Persons.GetPlayerIndex(c)].LookAt(0);

                                       if (Room==ROOM_LAST_MINUTE)
                                          Sim.Persons[Sim.Persons.GetPlayerIndex(c)].LookAt(1);

                                       if (Room==ROOM_REISEBUERO)
                                          Sim.Persons[Sim.Persons.GetPlayerIndex(c)].LookAt(3);

                                       qPlayer.CalcRoom ();
                                       qPlayer.BroadcastRooms (ATNET_ENTERROOM);
                                       qPlayer.BroadcastPosition (true);
                                    }

                                    //Fenster für Raum eröffnen:
                                    switch (Room)
                                    {
                                       case ROOM_BURO_A:
                                       case ROOM_BURO_B:
                                       case ROOM_BURO_C:
                                       case ROOM_BURO_D:
                                          qPlayer.LocationWin = new CBuero (FALSE, c);
                                          break;

                                       case ROOM_PERSONAL_A:
                                       case ROOM_PERSONAL_B:
                                       case ROOM_PERSONAL_C:
                                       case ROOM_PERSONAL_D:
                                          qPlayer.LocationWin = new CPersonal (FALSE, c);
                                          break;

                                       case ROOM_AIRPORT:
                                          qPlayer.LocationWin = new AirportView (FALSE, c);
                                          break;

                                       case ROOM_PLANEPROPS:  qPlayer.LocationWin = new CPlaneProps (FALSE, c); break;
                                       case ROOM_EDITOR:      qPlayer.LocationWin = new CEditor (FALSE, c);     break;
                                       case ROOM_SHOP1:       qPlayer.LocationWin = new CDutyFree (FALSE, c);   break;
                                       case ROOM_GLOBE:       qPlayer.LocationWin = new CGlobe (FALSE, c);      break;
                                       case ROOM_LAPTOP:      qPlayer.LocationWin = new CLaptop (FALSE, c);     break;
                                       case ROOM_ARAB_AIR:    qPlayer.LocationWin = new CArabAir (FALSE, c);    break;
                                       case ROOM_SABOTAGE:    qPlayer.LocationWin = new CSabotage (FALSE, c);   break;
                                       case ROOM_AUFSICHT:    qPlayer.LocationWin = new CAufsicht (FALSE, c);   break;
                                       case ROOM_ABEND:       qPlayer.LocationWin = new CAbend (FALSE, c);      break;
                                       case ROOM_BANK:        qPlayer.LocationWin = new Bank (FALSE, c);        break;
                                       case ROOM_KIOSK:       qPlayer.LocationWin = new CKiosk (FALSE, c);      break;
                                       case ROOM_DESIGNER:    qPlayer.LocationWin = new CDesigner (FALSE, c);   break;
                                       case ROOM_LAST_MINUTE: qPlayer.LocationWin = new CLastMinute (FALSE, c); break;
                                       case ROOM_FRACHT:      qPlayer.LocationWin = new CFrachtRaum (FALSE, c); break;
                                       case ROOM_MAKLER:      qPlayer.LocationWin = new CMakler (FALSE, c);     break;
                                       case ROOM_TAFEL:       qPlayer.LocationWin = new CTafel (FALSE, c);      break;
                                       case ROOM_OPTIONS:     qPlayer.LocationWin = new Options (FALSE, c);     break;
                                       case ROOM_WERKSTATT:   qPlayer.LocationWin = new CWerkstatt (FALSE, c);  break;
                                       case ROOM_WERBUNG:     qPlayer.LocationWin = new CWerbung (FALSE, c);    break;
                                       case ROOM_REISEBUERO:  qPlayer.LocationWin = new CReisebuero (FALSE, c); break;
                                       case ROOM_NASA:        qPlayer.LocationWin = new CNasa (FALSE, c);       break;
                                       case ROOM_ROUTEBOX:    qPlayer.LocationWin = new CRouteBox (FALSE, c);   break;
                                       case ROOM_STATISTICS:  qPlayer.LocationWin = new CStatistik (FALSE, c);  break;
                                       case ROOM_WORLD:       qPlayer.LocationWin = new CWorld (FALSE, c, Sim.HomeAirportId);  break;
                                       case ROOM_MUSEUM:      qPlayer.LocationWin = new CMuseum (FALSE, c);     break;
                                       case ROOM_RICKS:       qPlayer.LocationWin = new CRicks (FALSE, c);      break;
                                       case ROOM_SECURITY:    qPlayer.LocationWin = new CSecurity (FALSE, c);   break;

                                       case ROOM_INSEL:
                                          PlayUniversalFx ("coin.raw", Sim.Options.OptionEffekte);
                                          qPlayer.LocationWin = new CInsel (FALSE, c);
                                          break;

                                       case ROOM_WELTALL:
                                          PlayUniversalFx ("coin.raw", Sim.Options.OptionEffekte);
                                          qPlayer.LocationWin = new CWeltAll (FALSE, c);
                                          break;

                                       case ROOM_RUSHMORE:
                                          PlayUniversalFx ("coin.raw", Sim.Options.OptionEffekte);
                                          qPlayer.LocationWin = new CRushmore (FALSE, c);
                                          break;

                                       default:
                                          if (qPlayer.Locations[d]!=2049) hprintf (0, "Room %li not implemented!", qPlayer.Locations[d]);
                                          qPlayer.Locations[d] = 0;
                                          break;
                                    }

                                    //Globe wegen den MessagePumps erst jetzt faden...
                                    if (!JustLeftRoom && qPlayer.Locations[d]==ROOM_GLOBE && (Sim.GetHour()>9 || Sim.Date==0))
                                    {
                                       gBlendState=-1;
                                       if (FrameWnd && gBlendBm.Size.y==0) FrameWnd->PrepareFade();
                                       FrameWnd->Invalidate(); MessagePump();
                                    }

                                    qPlayer.CalcRoom ();
                                    qPlayer.BroadcastPosition (true);
                                    qPlayer.BroadcastRooms (ATNET_ENTERROOM);
                                    Sim.UpdateRoomUsage ();
                                 }
                              }
                           }
                           else if (Sim.Players.Players[c].Owner==1) //Und das gleiche für Roboter:
                           {
                              for (d=0; d<10; d++)
                              {
                                 if (qPlayer.Locations[d] & ROOM_ENTERING)
                                 {
                                    if (IsRoomBusy(UWORD(qPlayer.Locations[d] & (~ROOM_ENTERING)), c))
                                    {
                                       //Raum schon besetzt? Erst zweite Priorität ausführen:
                                       for (e=qPlayer.RobotActions.AnzEntries()-1; e>=1; e--)
                                          qPlayer.RobotActions[e]=qPlayer.RobotActions[e-1];
                                       qPlayer.RobotActions[1]=qPlayer.RobotActions[2];
                                       qPlayer.RobotActions[2]=qPlayer.RobotActions[0];

                                       if (qPlayer.RobotActions[1].ActionId==ACTION_NONE)
                                          if (qPlayer.OfficeState==2)
                                             qPlayer.RobotActions[1].ActionId=ACTION_PERSONAL;
                                          else
                                             qPlayer.RobotActions[1].ActionId=ACTION_BUERO;

                                       qPlayer.RobotActions[0].ActionId=ACTION_NONE;
                                       //qPlayer.RobotPump (); //wg. Verdacht auf Netzprobleme rausgenommen
                                    }
                                    else
                                    {
                                       qPlayer.Locations[d] &= ~ROOM_ENTERING;
                                       qPlayer.CalcRoom ();
                                       if (qPlayer.Locations[d]!=ROOM_AIRPORT) qPlayer.RobotExecuteAction ();

                                       SLONG Room = (qPlayer.Locations[d] & ~ROOM_LEAVING);
                                       if (Room==ROOM_ARAB_AIR || Room==ROOM_ROUTEBOX || Room==ROOM_KIOSK || Room==ROOM_RICKS || Room==ROOM_LAST_MINUTE || Room==ROOM_REISEBUERO)
                                       {
                                          if (Room==ROOM_ARAB_AIR || Room==ROOM_ROUTEBOX || Room==ROOM_KIOSK || Room==ROOM_RICKS)
                                             Sim.Persons[Sim.Persons.GetPlayerIndex(c)].LookAt(0);

                                          if (Room==ROOM_LAST_MINUTE)
                                             Sim.Persons[Sim.Persons.GetPlayerIndex(c)].LookAt(1);

                                          if (Room==ROOM_REISEBUERO)
                                             Sim.Persons[Sim.Persons.GetPlayerIndex(c)].LookAt(3);
                                       }

                                       switch (qPlayer.Locations[d])
                                       {
                                          case ROOM_AUFSICHT:  Talkers.Talkers[TALKER_BOSS].IncreaseLocking ();       break;
                                          case ROOM_ARAB_AIR:  Talkers.Talkers[TALKER_ARAB].IncreaseLocking ();       break;
                                          case ROOM_SABOTAGE:  Talkers.Talkers[TALKER_SABOTAGE].IncreaseLocking ();   break;
                                          case ROOM_BANK:      Talkers.Talkers[TALKER_BANKER1].IncreaseLocking ();
                                                               Talkers.Talkers[TALKER_BANKER2].IncreaseLocking ();    break;
                                          case ROOM_MUSEUM:    Talkers.Talkers[TALKER_MUSEUM].IncreaseLocking ();     break;
                                          case ROOM_MAKLER:    Talkers.Talkers[TALKER_MAKLER].IncreaseLocking ();     break;
                                          case ROOM_WERKSTATT: Talkers.Talkers[TALKER_MECHANIKER].IncreaseLocking (); break;
                                          case ROOM_WERBUNG:   Talkers.Talkers[TALKER_WERBUNG].IncreaseLocking ();    break;
                                          case ROOM_SECURITY:  Talkers.Talkers[TALKER_SECURITY].IncreaseLocking ();   break;
                                       }

                                       if (Sim.bNetwork && Sim.bIsHost)
                                       {
                                          qPlayer.BroadcastPosition ();
                                          qPlayer.BroadcastRooms (ATNET_ENTERROOM, qPlayer.Locations[d]);
                                          Sim.UpdateRoomUsage ();
                                       }
                                    }
                                 }
                                 else if (qPlayer.Locations[d] & ROOM_LEAVING)
                                 {
                                    switch (qPlayer.Locations[d] & (~ROOM_LEAVING))
                                    {
                                       case ROOM_AUFSICHT:  Talkers.Talkers[TALKER_BOSS].DecreaseLocking ();       break;
                                       case ROOM_ARAB_AIR:  Talkers.Talkers[TALKER_ARAB].DecreaseLocking ();       break;
                                       case ROOM_SABOTAGE:  Talkers.Talkers[TALKER_SABOTAGE].DecreaseLocking ();   break;
                                       case ROOM_BANK:      Talkers.Talkers[TALKER_BANKER1].DecreaseLocking ();
                                                            Talkers.Talkers[TALKER_BANKER2].DecreaseLocking ();    break;
                                       case ROOM_MUSEUM:    Talkers.Talkers[TALKER_MUSEUM].DecreaseLocking ();     break;
                                       case ROOM_MAKLER:    Talkers.Talkers[TALKER_MAKLER].DecreaseLocking ();     break;
                                       case ROOM_WERKSTATT: Talkers.Talkers[TALKER_MECHANIKER].DecreaseLocking (); break;
                                       case ROOM_WERBUNG:   Talkers.Talkers[TALKER_WERBUNG].DecreaseLocking ();    break;
                                       case ROOM_SECURITY:  Talkers.Talkers[TALKER_SECURITY].DecreaseLocking ();   break;
                                    }

                                    SLONG RoomLeft = qPlayer.Locations[d]&255;

                                    if (Sim.RoomBusy[RoomLeft]) Sim.RoomBusy[RoomLeft]--;

                                    qPlayer.Locations[d] = 0;
                                    qPlayer.Locations[d-1] = USHORT((qPlayer.Locations[d-1] & (~ROOM_LEAVING)) | ROOM_ENTERING);
                                    qPlayer.CalcRoom ();

                                    if (Sim.bNetwork && Sim.bIsHost)
                                    {
                                       qPlayer.BroadcastPosition ();
                                       qPlayer.BroadcastRooms (ATNET_LEAVEROOM, RoomLeft);
                                       Sim.UpdateRoomUsage ();
                                    }
                                 }
                              }
                           }
                        }
                     }

                  if (nOptionsOpen==0 && nWaitingForPlayer==0 && !Sim.bPause)
                  {
                     //Personen bewegen:
                     if (Sim.Time>=8*60000 && Sim.Time<=18*60000 && Editor==EDITOR_NONE && !Sim.CallItADay)
                     {
                        if (Sim.Difficulty!=DIFF_TUTORIAL || Sim.Tutorial>=1100 || !Sim.IsTutorial)
                           for (c=0; c<Sim.Players.AnzPlayers; c++)
                           {
                              if ((Sim.Time&255)==0 && Sim.Players.Players[c].iWalkActive) Sim.Players.Players[c].UpdateWaypoints();
                              Sim.Players.Players[c].UpdateWaypointWalkingDirection ();
                           }
                     }

                     if (!(Sim.Time<8*60000 || (Sim.Time>=18*60000 && Sim.DayState==4)))
                     {
                        if (Editor==EDITOR_NONE)
                        {
                           if (!Sim.CallItADay) Sim.PersonQueue.Pump();
                           Sim.Persons.DoOneStep();

                           if (!Sim.CallItADay)
                           {
                              Talkers.Pump();
                              Airport.PumpDoors ();
                           }
                        }
                     }

                     if (Sim.Time>9*60000 && Sim.Time<=18*60000 && Sim.bNoTime==FALSE && !Editor)
                        if (Sim.bNoTime==FALSE) //Neu, wegen Bugs im Netzwerk
                           if (Sim.Difficulty!=DIFF_TUTORIAL || Sim.Tutorial>=1100 || !Sim.IsTutorial)
                              Sim.Players.RobotPump();

                     //Flugzeuge bewegen:
                     {
                        AnzPlanesOnScreen=0;
                        if (Editor==EDITOR_NONE)
                           if (SkipPlaneCalculation<=0)
                           {
                              SkipPlaneCalculation=10;
                              for (c=0; c<Sim.Players.AnzPlayers; c++)
                                 if (!Sim.Players.Players[c].IsOut)
                                    Sim.Players.Players[c].Planes.DoOneStep(c);
                           }
                           else
                              SkipPlaneCalculation--;

                        if (LastAnzPlanesOnScreen<AnzPlanesOnScreen) LastAnzPlanesOnScreen=min(LastAnzPlanesOnScreen+3,AnzPlanesOnScreen);
                        if (LastAnzPlanesOnScreen>AnzPlanesOnScreen) LastAnzPlanesOnScreen=max(LastAnzPlanesOnScreen-3,AnzPlanesOnScreen);

                        AmbientManager.SetVolume (AMBIENT_JET_OUTSIDE, LastAnzPlanesOnScreen);
                     }

                     //Flugzeuge, die vorbeifliegen:
                     if (Sim.Options.OptionEffekte)
                     {
                        if (gUniversalPlaneFxCountdown==0 || timeGetTime()>DWORD(gUniversalPlaneFxCountdown))
                        {
                           gUniversalPlaneFxCountdown = timeGetTime()+15000+rand()%40*1000;

                           if (gUniversalPlaneFxCountdown!=0)
                           {
                              gUniversalPlaneFx.ReInit (PlaneSounds[rand()%6]);
                              gUniversalPlaneFx.Play (0, Sim.Options.OptionPlaneVolume*100/7*AmbientManager.GlobalVolume/100);
                           }
                        }
                     }

                     //Zeit weitermachen:
                     if (!Sim.bNoTime)
                     {
                        //Zeitabhängige Events auslösen & Uhr weitersetzten:
                        if (Sim.Time<8*60000)
                        {
                           Sim.DoTimeStep ();
                           if (Sim.Time>=8*60000) NumSimSteps=1;
                        }
                        else if (Sim.Time<9*60000)
                        {
                           Sim.DoTimeStep ();
                           if (Sim.Time>=9*60000) NumSimSteps=1;
                        }
                        else
                           Sim.DoTimeStep ();

                        if (!(Sim.CallItADay || Sim.Time<9*60000 || (Sim.Time>=18*60000 && Sim.DayState==4)) && ((Sim.Time+NumSimSteps)&1023)==0)
                           Sim.Players.RandomBeraterMessage();
                     }

                     if (Sim.Time>=18*60000 || (Sim.CallItADay && Sim.DayState==2) || Sim.DayState==4)
                     {
                        if (Sim.Time>=18*60000 && Sim.DayState!=4)
                        {
                           Sim.Players.Players[Sim.localPlayer].CallItADay=FALSE;
                           Sim.bNoTime = TRUE;
                        }

                        if (Sim.DayState==2) //Begin Evening Sequenz
                        {
                           BOOL now=TRUE;

                           if (Sim.Players.Players[Sim.localPlayer].LocationWin && ((CStdRaum *)Sim.Players.Players[Sim.localPlayer].LocationWin)->IsDialogOpen())
                           {
                              ((CStdRaum *)Sim.Players.Players[Sim.localPlayer].LocationWin)->StopDialog ();
                           }

                           for (c=0; c<Sim.Players.AnzPlayers; c++)
                           {
                              //Raum verlassen:
                              if (Sim.Players.Players[c].Owner!=1 && Sim.Players.Players[c].Locations[0] && Sim.Players.Players[c].GetRoom()!=ROOM_ABEND)
                              {
                                 Sim.Players.Players[c].LeaveRoom ();
                                 now=FALSE;
                              }
                           }

                           if (now)
                           {
                              TopWin = new CAbend (FALSE, Sim.localPlayer);
                              Sim.DayState = 3;
                           }
                        }
                        else if (Sim.DayState==4) //End Evening Sequenz
                        {
                           if (Sim.Time>=9*60000 && Sim.Time<=10*60000)
                           {
                              if (TopWin)
                              {
                                 delete TopWin;
                                 TopWin = NULL;
                              }

                              Sim.bNoTime   = TRUE;
                              Sim.DayState  = 1;
                              MouseWait++;

                              for (c=0; c<Sim.Players.AnzPlayers; c++)
                                 if (!Sim.Players.Players[c].IsOut)
                                 {
                                    Sim.Players.Players[c].EnterRoom (ROOM_AIRPORT);
                                    Sim.Players.Players[c].EnterRoom (ROOM_AUFSICHT);
                                 }
                           }
                        }
                     }

                     //Shopper ankommen lassen:
                     if (!Sim.CallItADay && Sim.Time>=8*60000 && Sim.Time<=18*60000)
                        if (nOptionsOpen==0 && nWaitingForPlayer==0 && !Sim.bPause && Sim.bNoTime==false) //Neu, wegen Bugs im Netzwerk
                        {
                           if ((Sim.TimeSlice & 63)==0)      Sim.AddNewShoppers ();
                           if (((Sim.TimeSlice+16) & 31)==0) Sim.AddNewPassengers ();

                           if ((Sim.TimeSlice & 7)==0 && Sim.Players.Players[Sim.localPlayer].GetRoom()==ROOM_AIRPORT)
                              Sim.Persons.TryMoods ();

                           if (Sim.Time>=9*60000 && Sim.GetMinute()==30 && (Sim.GetHour()&1)==0 && VoiceScheduler.AnzEntries()==0 && !bgWarp && Sim.Players.Players[Sim.localPlayer].GameSpeed<2)
                           {
                              TEAKRAND LocalRand (Sim.Date+Sim.StartTime+Sim.GetHour());
                              if (LocalRand.Rand(10)==0)
                                 VoiceScheduler.AddVoice ("Fahrzeug");
                              else
                                 if (((Sim.GetHour()/2)&1)==0)
                                    VoiceScheduler.AddVoice ("ZoneOK");
                                 else
                                    VoiceScheduler.AddVoice ("Hinweis");
                           }
                        }

                     //Gimmicks ggf. rauswerfen:
                     if ((Sim.Time&127)==0)
                     {
                        for (SLONG c=Clans.AnzEntries()-1; c>=0; c--)
                           if (Clans.IsInAlbum (c))
                           {
                              //Gimmick seit 2 Sekunden nicht genutzt?
                              if (Sim.TickerTime-Clans[c].GimmickTime>60)
                              {
                                 Clans[c].ClanGimmick.Unload ();
                                 Clans[c].ClanWarteGimmick.Unload ();
                              }
                           }
                     }

                     if (Sim.Time==9*60000 && Sim.TimeSlice!=0)
                     {
                        Sim.TimeSlice=0;
                        Sim.PersonQueue.NewDay ();
                     }
                     else
                         if (nOptionsOpen==0 && nWaitingForPlayer==0 && !Sim.bPause && Sim.bNoTime==false) //Neu, wegen Bugs im Netzwerk
                            Sim.TimeSlice++;
                  }
 
                  if (Sim.Time>9*60000 && Sim.Time<18*60000) VoiceScheduler.Pump();

                  //if (nOptionsOpen==0 && nWaitingForPlayer==0 && !Sim.bPause) //Neu, wegen Bugs im Netzwerk
                  {
                     NumSimSteps--;
                     SimStepsCounter++;
                     Sim.TicksPlayed++;
                     Sim.TicksSinceLoad++;
                  }

                  PumpNetwork ();

                  //Nur zum testen der Flugpläne
                  //Sim.Players.CheckFlighplans();

                  //Exe Check             10 Minuten                  2 Stunden
                  /*if (Sim.TicksSinceLoad==20*60*10 && Sim.TicksPlayed>20*60*60*2 && Sim.NumRestored>0)
                  {
                     BUFFER<char> Filename(1002);

                     GetModuleFileName (NULL, Filename, 1000);

                     if (!CheckMainFile (CString((char*)Filename)))
                     {
                        Sim.bExeChanged=20*60*5+rand()%100;
                     }
                     else //Re-Legalisieren:
                        Sim.bExeChanged=0;

                  }
                  else if (Sim.bExeChanged>1)
                  {
                     Sim.bExeChanged--;

                     if (Sim.bExeChanged==1)
                     {
                        CRegistryAccess reg (chRegKey);
                        int temp=1;
                        reg.WriteRegistryKeyEx ((int*)&temp, "&OptionViewedSeq");
                     }
                  }*/

                  //Ggf. das Spiel verlassen
                  if (Sim.Gamestate==(GAMESTATE_QUIT|GAMESTATE_WORKING))
                  {
                     Sim.QuitCountDown--;
                     while (!Sim.QuitCountDown) Sim.QuitCountDown--;
                  }

                  PumpBroadcastBitmap ();
               }
            }
            else
               PumpBroadcastBitmap (true);

            if (Sim.Date==0 && Sim.IsTutorial && Sim.Tutorial==0 && Sim.Players.Players[Sim.localPlayer].LocationWin) Sim.Tutorial=999;

            //Zeit für die AnimBricks aktualisieren:
            Sim.TickerTime = Time>>5;
         }
         else {
         	RefreshNeccessary=TRUE;
         	Sim.TickerTime=timeGetTime()>>5;
            PumpBroadcastBitmap(true);
         } //Not In Game ==> Always refresh

         //Bildschirm neu zeichnen:
         if (RefreshNeccessary)
         {
            if (nOptionsOpen==0 && nWaitingForPlayer==0 && !Sim.bPause && !Sim.CallItADay && Editor==EDITOR_NONE)
            {
               for (c=0; c<Sim.Players.AnzPlayers; c++)
               {
                  if (Sim.Players.Players[c].GetRoom()==ROOM_AIRPORT && Sim.Players.Players[c].LocationWin)
                     ((AirportView*)Sim.Players.Players[c].LocationWin)->MoveCamera ();
               }
            }

            FrameWnd->Invalidate();

            RefreshNeccessary=FALSE;
         }else{
             MessagePump();
             PrimaryBm.PrimaryBm.Present();
         }

         PumpNetwork ();
      }

      /*for (c=0; c<Sim.Players.AnzPlayers; c++)
      {
         static SLONG Emergency=0;

       if (Emergency==0)
         {
            for (SLONG d=0; d<Sim.Players.Players[c].Letters.Letters.AnzEntries(); d++)
            {
                if ((*(ULONG*)&Sim.Players.Players[c].Letters.Letters[d].Subject)==NULL)
                   Emergency=TRUE;
                if ((*(ULONG*)&Sim.Players.Players[c].Letters.Letters[d].Letter)==NULL)
                   Emergency=TRUE;
                if ((*(ULONG*)&Sim.Players.Players[c].Letters.Letters[d].Absender)==NULL)
                   Emergency=TRUE;
            }

            for (d=0; d<Sim.Players.Players[c].Statistiken.AnzEntries(); d++)
            {
               if (Sim.Players.Players[c].Statistiken[d].Days.AnzEntries()==0)
                   Emergency=TRUE;
               if (Sim.Players.Players[c].Statistiken[d].Months.AnzEntries()==0)
                   Emergency=TRUE;
            }

            if (Emergency)
            {
               MessageBeep (0xffffffff); Sleep (1000);
               MessageBeep (0xffffffff); Sleep (1000);
               MessageBeep (0xffffffff); Sleep (1000);
               MessageBeep (0xffffffff); Sleep (300);
               MessageBeep (0xffffffff); Sleep (300);
               MessageBeep (0xffffffff); Sleep (300);
               MessageBeep (0xffffffff); Sleep (300);
               MessageBeep (0xffffffff); Sleep (300);
               MessageBeep (0xffffffff); Sleep (300);
               MessageBeep (0xffffffff); Sleep (1000);
               MessageBeep (0xffffffff); Sleep (1000);
               MessageBeep (0xffffffff); Sleep (1000);
               MessageBeep (0xffffffff); Sleep (300);
               MessageBeep (0xffffffff); Sleep (300);
               MessageBeep (0xffffffff); Sleep (300);
               MessageBeep (0xffffffff); Sleep (300);
               MessageBeep (0xffffffff); Sleep (300);
               MessageBeep (0xffffffff); Sleep (300);
               MessageBeep (0xffffffff); Sleep (1000);
               MessageBeep (0xffffffff); Sleep (1000);
               MessageBeep (0xffffffff); Sleep (1000);
            }
         }
      }*/

      MessagePump();
   }

   if (bFullscreen)
   {
      //lpDD->RestoreDisplayMode ();
      //ClipCursor (NULL);
   }

   //Shutdown:
   if (TopWin)
   {
      CStdRaum *TmpWin = TopWin; TopWin=NULL; delete TmpWin;
   }

   if (Sim.Players.Players.AnzEntries()>0)
      for (c=0; c<Sim.Players.AnzPlayers; c++)
      {
         if (Sim.Players.Players[c].LocationWin)
         {
            CStdRaum *TmpWin = (CStdRaum *)Sim.Players.Players[c].LocationWin;
            Sim.Players.Players[c].LocationWin = NULL;
            delete TmpWin;
         }
      }

   VoiceScheduler.Clear();
}

void CTakeOffApp::CheckSystem (void)
{
   for (SLONG c=0; c<Sim.Players.AnzPlayers; c++)
   {
      static SLONG Emergency=0;

      if (Emergency==0)
      {
         for (SLONG d=0; d<Sim.Players.Players[c].Letters.Letters.AnzEntries(); d++)
         {
             if ((*(ULONG*)&Sim.Players.Players[c].Letters.Letters[d].Subject)==NULL)
                Emergency=TRUE;
             if ((*(ULONG*)&Sim.Players.Players[c].Letters.Letters[d].Letter)==NULL)
                Emergency=TRUE;
             if ((*(ULONG*)&Sim.Players.Players[c].Letters.Letters[d].Absender)==NULL)
                Emergency=TRUE;
         }

         for (SLONG d=0; d<Sim.Players.Players[c].Statistiken.AnzEntries(); d++)
         {
            if (Sim.Players.Players[c].Statistiken[d].Days.AnzEntries()==0)
                Emergency=TRUE;
            if (Sim.Players.Players[c].Statistiken[d].Months.AnzEntries()==0)
                Emergency=TRUE;
         }

         if (Emergency)
            DebugBreak();
      }
   }
}

//--------------------------------------------------------------------------------------------
//Initilisiert alle benötigten DirectX-Schnittstellen:
//--------------------------------------------------------------------------------------------
BOOL InitDirectX (void)
{
   //HRESULT ddrval;

   gPhysicalCdRomBitlist.Pump();

   //create the main DirectDraw object:
   /*ddrval = DirectDrawCreate (NULL, &lpDD, NULL);
   if (ddrval != DD_OK)
   {
      ddrval = DirectDrawCreate ((GUID FAR *)DDCREATE_EMULATIONONLY, &lpDD, NULL);
      if (ddrval != DD_OK)
      {
         ::MessageBox (NULL, "Can't init DirectDraw! Is DirectX installed?", "ERROR", MB_OK );
         return (FALSE);
      }
   }*/

   return (TRUE);
}

//--------------------------------------------------------------------------------------------
//Ruft das Help-File auf:
//--------------------------------------------------------------------------------------------
void CTakeOffApp::WinHelp(DWORD, UINT) 
{
   ToolTipState=FALSE;
   
   ToolTipTimer=timeGetTime()-1001;
}























/*!
 * \brief	This method takes a character string and converts it to a string which only contains easy readable characters like A - Z and 1 - 6.
 *
 * \author	Martin Mayer
 *
 * \param   pData	Character string which has to converted to readable characters. No trailing 0 is needed.
 * \param   uLen	Length of the pData buffer.
 *
 * \return	Returns an readable character string which is zero terminated. The returned pointer has to be deleted manually!
 */
char *UCharToReadableAnsi( const unsigned char *pData, const unsigned uLen )
{
	unsigned uBitSize = uLen * 8;
	unsigned i;
	unsigned short int wData;
	char *pReturn;

	pReturn = new char[( uBitSize + 4 ) / 5 + 1]; // ( char * ) malloc( ( ( uBitSize + 4 ) / 5 + 1 ) * sizeof( char ) );
	pReturn[(uBitSize + 4 ) / 5] = '\0';
	for( i = 0; i < uBitSize; i += 5 )
	{
		// Example binary stream which gets split into 5 bit sequences; this string is unique for every input
		// -> so this uses A - Z and 1 - 6 to get 2^5 combinations
		// 11001110 00010101 01001100 00111000
		// 11001 11000 01010 10100 11000 01110 00000

		wData = pData[i / 8] << 8;
		if( i / 8 + 1 < uLen )
		{
			wData |= ( pData[i / 8 + 1] );
		}
		wData = wData >> ( ( 16 - 5 ) - ( i % 8 ) );
		wData &= 0x1f;

		if( 'A' + wData > 'Z' )
		{
			wData -= ( 'Z' - 'A' );
			wData += '0';
		}
		else
		{
			wData += 'A';
		}
		
		pReturn[i / 5] = ( char ) wData;
	}
	return pReturn;
}


/*!
 * \brief	This method converts a encoded to readability string back to its origin data.
 *
 * \author	Martin Mayer
 *
 * \param   pData	Character string which has to be converted to its origin data.
 * \param   uLen	Length of the pData buffer.
 *
 * \return	Returns the origin data string with a trailing zero. The returned pointer has to be deleted manually!
 */
unsigned char *ReadableAnsiToUChar( const char *pData, const unsigned uLen )
{
	unsigned uNewSize = uLen * 5  / 8;
	unsigned char *pReturn = new unsigned char[uNewSize + 1]; //( unsigned char * ) malloc( uNewSize + 1 );
	memset( pReturn, 0, uNewSize + 1 );

	for( unsigned i = 0; i < uLen; i++ )
	{
		char cChar = ( pData[i] < 'A' ? ( pData[i] - '0' ) + ( 'Z' - 'A' ) : pData[i] - 'A' );

		unsigned uBitPos = i * 5;
		unsigned uBitPosMod8 = uBitPos % 8;
		if( uBitPosMod8 > 3 )
		{
			unsigned uRightShift = ( 5 - ( 8 - uBitPosMod8 ) );
			char cTmp = cChar >> uRightShift;
			pReturn[uBitPos / 8] |= cTmp;
			if( uBitPos / 8 + 1 < uNewSize )
			{
				pReturn[uBitPos / 8 + 1] |= cChar << ( 8 - uRightShift );
			}
		}
		else
		{
			cChar <<= ( 3 - uBitPosMod8 );
			pReturn[uBitPos / 8] |= cChar;
		}
	}

	return pReturn;
}
