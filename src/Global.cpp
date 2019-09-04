//============================================================================================
// Global.h - Inkarnationen der globalen Variablen
//============================================================================================
#include "stdafx.h"

//--------------------------------------------------------------------------------------------
//Zu erst die Ausnahmen:
//--------------------------------------------------------------------------------------------
extern char            *ExcNever="ExcNever";
extern char            *ExcCreateWindow="CreateWindow failed!";

//--------------------------------------------------------------------------------------------
//Dinge für den Überblendeffekt:
//--------------------------------------------------------------------------------------------
SBBM                    gBlendBm;
SBBM                    gBlendBm2;
SLONG                   gBlendState=-1;
SLONG                   gFramesToDrawBeforeFirstBlend=0;   //Paint 2 frames, before reading from the screen for blending reasons

//--------------------------------------------------------------------------------------------
//Flags für Cheats:
//--------------------------------------------------------------------------------------------
BOOL                    CheatRunningman=FALSE;
BOOL                    CheatMoreNuns=FALSE;
BOOL                    CheatMoreGimmicks=FALSE;
BOOL                    CheatBubbles=FALSE;
BOOL                    CheatShowAll=FALSE;
BOOL                    CheatMissions=FALSE;
BOOL                    CheatBerater=FALSE;
BOOL                    CheatAnimNow=FALSE;
BOOL                    CheatTestGame=FALSE;

//--------------------------------------------------------------------------------------------
//Die Zufallsgeneratoren:
//--------------------------------------------------------------------------------------------
TEAKRAND                PersonRandCreate; //Alles, was die Personen angeht, die erzeugt werden
TEAKRAND                PersonRandMisc;   //Alles, was die Personen angeht
TEAKRAND                HeadlineRand;

//--------------------------------------------------------------------------------------------
//Die Registratur:
//--------------------------------------------------------------------------------------------
CRegistration           Registration;
SLONG                   MissionKeys[]  = {142, 372, 814, 1042, 2077, 4525};
SLONG                   MissionKeys2[] = {111, 172, 472, 1099, 2048, 5055, 6789, 7007, 8192, 9092 };
SLONG                   MissionKeys3[] = {298, 346, 532, 2098, 3331, 3333, 4001, 4098, 4800, 8099 };

//--------------------------------------------------------------------------------------------
//Pointer auf Fenster:
//--------------------------------------------------------------------------------------------
CWnd                   *TopWin;          //Übergeordnetes Fenster, z.B. load Airport

//--------------------------------------------------------------------------------------------
//Einige *WIRKLICH* globale Ressourcen:
//--------------------------------------------------------------------------------------------
CBench                  Bench;
SLONG                   gShowAllPools=0;
SLONG                   gUpdatingPools=0;
GameFrame              *FrameWnd;         //Das Rahmenfenster;
LPDIRECTDRAW            lpDD;             //DirectDraw object
SBPRIMARYBM             PrimaryBm;        //DirectDraw primary & back surface
SB_CCursor             *pCursor=NULL;
SLONG                   gRepairPrice[] = { 1200, 18000, 48000, 150000 };
SLONG                   gWerbePrice[] = { 1000, 10000,  50000, 100000,  250000,  2500000,
                                          2000,  8000,  22000,  56000,  120000,  1400000,
                                          5000, 93000, 290000, 870000, 4925000, 60000000 };
SLONG                   RightAirportClip=640;

SB_CBitmapMain         *bitmapMain;
GfxMain                *pGfxMain;
GfxLib                 *pGLibBrick;
GfxLib                 *pGLibClan;
GfxLib                 *pGLibStd;
GfxLib                 *pGLibBasis;
GfxLib                 *pGLibPlanes;
GfxLib                 *pGLibBerater;
GfxLib                 *pGlowEffectLib=NULL;

SBBMS                   gGlowBms;
SBBM                    gNumberTemplate;

SLONG                   PlayerDidntMove=0;//Zeit in 20stel Sekunden

CHLPool                 SkelettPool;

CVoiceScheduler         VoiceScheduler;

extern bool             bVoicesNotFound=false;

//--------------------------------------------------------------------------------------------
//Maus & Tooltips:
//--------------------------------------------------------------------------------------------
BOOL							gUseWindowsMouse=0; //Keine eigene, sondern die Maus von Windows?
XY                      gMousePosition;   //Position des Mauscursors
XY                      gMouseClickPosition=0;   //Position des Mauscursors zum Zeitpunkt des MouseDown-Events
BOOL							gMouseLButton=0;	//Status des linken Mausknopfes
BOOL							gMouseRButton=0;	//Status des rechten Mausknopfes
SLONG					   	gMouseLButtonDownTimer;	 //Timerinhalt, wenn der LMB gedrückt wurde
BOOL							gMouseScroll=0;   //Mouse-Scrolling für den Airport?
SLONG                   MouseLook;        //Normal, Hot, Page
SLONG                   MouseClickArea;   //In Statusleiste/Raum
SLONG                   MouseClickId;     //Der Id
SLONG                   MouseClickPar1;
SLONG                   MouseClickPar2;
XY                      ToolTipPos;
SLONG                   ToolTipId=0;
SLONG                   ToolTipTimer;
BOOL                    ToolTipState=FALSE;
XY                      ToolTipNewPos;
SLONG                   ToolTipNewId=0;
CString                 ToolTipString;
SLONG                   MouseWait=0;
SLONG                   gMouseStartup=0;

SBBM                    gToolTipBm;
SBBM                    gCursorBm;        //Bitmap des Cursors
SBBM                    gCursorLBm;       //Pfeil nach Link
SBBM                    gCursorRBm;       //Pfeil nach Rechts
SBBM                    gCursorHotBm;
SBBMS                   gCursorExitBms;
SBBM                    gCursorMoveHBm;
SBBM                    gCursorMoveVBm;
SBBM                    gCursorSandBm;
SBBM                    gCursorNoBm;

SBBMS                   gCursorFeetBms[2]; //Schuhe nach links und nach rechts
SLONG                   gShowCursorFeet=-1;

SBBM                    gDialogBarBm; 
SBBMS                   gToolTipBms; 

BOOL                    gRoomJustLeft;    //TRUE, wenn Raum gerade verlassen wurde (wegen messagePump während TransBlit)

//--------------------------------------------------------------------------------------------
//Text-Ressourcen:
//--------------------------------------------------------------------------------------------
TEXTRES                 DialogTexte;      //Die Text-Ressourcen der Dialoge
TEXTRES                 StandardTexte;    //Allgemeine Texte
TEXTRES                 ETexte;           //Die Einheiten

//--------------------------------------------------------------------------------------------
//einige Flags:
//--------------------------------------------------------------------------------------------
BOOL                    bTest=FALSE;       //Schneller Ablauf zum testen
BOOL                    bActive=TRUE;      //is application active?
BOOL                    bFullscreen;       //is application Fullscreen or in Window?
BOOL                    bCheatMode=FALSE;  //Ist der Cheatmode zum testen aktiviert?
BOOL                    bQuick=FALSE;      //Depeche Mode = Alles auf die Schnelle..
BOOL                    bgWarp=FALSE;      //Spieler warpt zum Ziel
BOOL                    bNoVgaRam=FALSE;   //Keine Bitmaps ins VGA-Ram legen
BOOL                    bNoSpeedyBar=FALSE;//Kein SpeedUp für die Statuszeile
BOOL                    bNoQuickMouse=FALSE; //Keine Maus-Aufrufe beim OnMouseMove
BOOL                    bCursorCaptured;   //Cursor gekidnappt und muß jetzt selbst gemalt werden?
BOOL                    bMidiAvailable;    //Ist ein Midi-Sequenzer vorhanden?
SLONG                   nOptionsOpen=0;    //Fummelt gerade wer an den Options?
SLONG                   nWaitingForPlayer=0;//Müssen wir da auf einen anderen Spieler warten?
SLONG                   nAppsDisabled=0;   //Ist ein anderer Spieler gerade in einer anderen Anwendung?
BOOL                    bTwoWins;          //SplitScreen Testen?
SLONG                   Editor;            //Ist der Flughafeneditor aktiv ? (=0)
BOOL                    gDisablePauseKey;  //Pause-Key für Texteingabe abgeschaltet?
BOOL                    bgJustDidLotsOfWork=FALSE; //Gegen Sprünge nach Load/Save
BOOL                    bLeaveGameLoop=FALSE;   //Hauptschleife verlassen?
BOOL                    bFirstClass=FALSE;

//--------------------------------------------------------------------------------------------
//Das Spiel höchstpersönlich:
//--------------------------------------------------------------------------------------------
CTakeOffApp             theApp;

//--------------------------------------------------------------------------------------------
//Einfache globale Variablen:
//--------------------------------------------------------------------------------------------
char                   *TabSeparator = ";\x8";   //Zum Import der Excel-Tabellen
char                   *DateSeparator = "-/;.";  //Erlaubte Datums-Separatoren
char                   *HallFilenames[] = { "kompletter Airport (%li-%li-%li-%li-%li-%li-%li-%li-%li-%li)", "Left%03li.dat", "Check%03li.dat", "Ofice%03li.dat", "Entry%03li.dat", "Shops%03li.dat", "Cafe%03li.dat", "Secur%03li.dat", "Suit%03li.dat", "WaitZ%03li.dat", "Right%03li.dat" };

//--------------------------------------------------------------------------------------------
//Die Pfade der einzelnen Dateigruppen:
//--------------------------------------------------------------------------------------------
CString                 BitmapPath;       //diverse Bitmaps
CString                 BrickPath;        //Hier sind die Part-Dateien
CString                 CityPath;         //Städte Bitmaps
CString                 ClanPath;         //Clan Bitmaps
CString                 ExcelPath;        //Hier sind die csv-Tabellen
CString                 MiscPath;         //Sonstige Kleinigkeiten
CString                 SavegamePath;     //Die Spielstände
CString                 ScanPath;         //gescannte Bilder
CString                 SoundPath;        //Musikstücke und Samples
CString                 RoomPath;         //Bilder für die Räume
CString                 PlanePath;        //Bilder der Flugzeuge
CString                 GliPath;          //diverse GLI-Dateien
CString                 SmackerPath;      //Die Smacker-Filmchen
CString                 IntroPath;        //Der Intro-Film
CString                 AppPath;          //Der Ganze Pfad, z.B. "f:\project\takeoff\"
CString                 VoicePath;             //die Sprache
CString                 MyPlanePath;           //die eigenen Flugzeuge

//--------------------------------------------------------------------------------------------
//Die Simulationswelt mit ihren Parameter (Zeit, Spieler, Schwierigkeit, ..)
//--------------------------------------------------------------------------------------------
SIM                     Sim;
CWorkers                Workers;
CTalkers                Talkers;
CKlackerPlanes          gKlackerPlanes;

//--------------------------------------------------------------------------------------------
//Globale Daten und Tabellen:
//--------------------------------------------------------------------------------------------
BRICKS                  Bricks;      //Die Bauteile
AIRPORT                 Airport;
CLANS                   Clans;
CPlaneTypes             PlaneTypes;
CITIES                  Cities;
CPlaneNames             PlaneNames;
CRouten                 Routen;
BUFFER<CEinheit>        Einheiten;
BUFFER<BUFFER_UWORD>    GlobeMapper;
BUFFER<BUFFER_UBYTE>    GlobeLight;
BUFFER<SLONG>           GlobeMapperY;
BUFFER<UWORD>           GlobeMixTab;
BUFFER<XY>              GlobeWindows;

//--------------------------------------------------------------------------------------------
//Der Klacker-Font:
//--------------------------------------------------------------------------------------------
//const char *KlackerFntDef = " ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜ0123456789.!:;,?()-#=*|><+²³";
const char *KlackerFntDef = " ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜ0123456789.!:;,?()-#=*|><+" "\xC1\xC8\xCF\xC9\xCC\xCD\xD2\xD3\xD8\x8A\x8D\xDA\xD9\xDD\x8E" "\xA5\xC6\xCA\xA3\xD1\x8C\x8F\xAF" "\xBC\xC0\xC5" "\xA6\xC3\xC7" "²³";
//const char *KlackerFntDef = " ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜ0123456789.!:;,?()-#=*|><+" "\xC1\xC8\xCF\xC9\xCC\xCD\xD2\xD3\xD8\x8A\x8D\xDA\xD9\xDD\x8E" "\xA5\xC6\xCA\xA3\xD1\x8C\x8F\xAF" "\xBC\xC0\xC5" "\xA6\xC3\x80" "²³";

//--------------------------------------------------------------------------------------------
//Text-Style des Spielers:
//--------------------------------------------------------------------------------------------
SB_CFont                FontCash;
SB_CFont                FontSmallBlack;
SB_CFont                FontSmallGrey;
SB_CFont                FontSmallRed;
SB_CFont                FontSmallWhite;
SB_CFont                FontSmallWhiteX;
SB_CFont                FontSmallPlastic;

SB_CFont                FontCondensedBlack;
SB_CFont                FontCondensedGrey;
SB_CFont                FontCondensedRed;

SB_CFont                FontDialogPartner;       //Gesprächspartner
SB_CFont                FontDialogInk;
SB_CFont                FontDialog;              //Eigene Sätze
SB_CFont                FontDialogLight;         //Eigene Sätze im Highlight
SB_CFont                FontNormalGrey;
SB_CFont                FontNormalGreen;
SB_CFont                FontBigGrey;             //Alter Dialogfont
SB_CFont                FontBigWhite;            //Alter Dialogfont (Hell)

UWORD                   ColorOfFontBlack;
UWORD                   ColorOfFontGrey;
UWORD                   ColorOfFontRed;

XY                      ReferenceCursorPos;      //Kopie von CursorPos, die nur pro Frame upgedaten wird
CRect                   gHighlightArea;          //Für das Text-Highlighting
UWORD                   gHighlightFontColor;
CRect                   gLastHighlightArea;

TABS                    TABLeft[] = { TAB_STYLE_LEFT };
TABS                    TABRight[] = { TAB_STYLE_RIGHT };

//--------------------------------------------------------------------------------------------
//Die Statuszeilen und Textzeilen: normal+Editoren
//--------------------------------------------------------------------------------------------
SBBMS                   StatusLineBms;
SBBMS                   gTutoriumBms;            //Statuszeile: Tutorium läuft
SBBMS                   gRepeatMessageBms;       //Statuszeile: Message noch einmal, bitte
SBBMS                   gStatisticBms;           //Statuszeile: Statistiken
SBBMS                   gStatLineHigh;           //Statuszeile: Highlights für Geld und Zeit
SBBMS                   gClockBms;
SLONG                   gStatButton=0;
SLONG                   gStatButtonTimer=0;
BUFFER<SBBM>            TextBricks;
BUFFER<CString>         TextBrickTexts;

SBBMS                   gNetworkBms;             //Network-Message: Waiting
SLONG                   gNetworkBmsType=0;       //Waiting for unknown (0) bReadyForMorning (1), bReadyForBriefing (2)
SBBM                    gBroadcastBm;            //Bitmap mit dem Broadcast-chattext
SLONG                   gBroadcastTimeout=0;     //Noch soviele 1/20 Sekunden sichtbar

//--------------------------------------------------------------------------------------------
//Bitmaps der Berater:
//--------------------------------------------------------------------------------------------
BUFFER<SBBMS>           BeraterBms;
SBBMS                   SprechblasenBms;
SBBMS                   XBubbleBms;

BOOL                    IsPaintingTextBubble=FALSE;
BOOL                    PleaseCancelTextBubble=FALSE;

//--------------------------------------------------------------------------------------------
//Der Mehrzwecke Menü-Hintergrund fürs aktuelle Zeitalter & die aktuelle Bildgröße und die Tips
//--------------------------------------------------------------------------------------------
SBBM                    gNotepadButtonL;
SBBM                    gNotepadButtonM;
SBBM                    gNotepadButtonR;
SBBMS                   gZettelBms;

SBBMS                   gPostItBms;           //Small & Blue/Green

SBBM                    gCrossBm;             //vom Scheduler
SBBM                    gAusrufBm;            //vom Scheduler
SBBM                    gLockBm;              //vom Scheduler

SBBMS                   LogoBms;              //Die Logos der Fluggesellschaften
SBBMS                   SmallLogoBms;         //Die kleinen Logos der Fluggesellschaften
SBBMS                   TinyLogoBms;          //Für die Sprechblasen
SBBMS                   MoodBms;              //Die Spimmungs-Sprechblasen
SBBMS                   SmileyBms;            //Die Spimmungs-Köpfe für die Aussagen des Beraters

SBBM                    gCityMarkerBm;        //Marker für eine Stadt
SBBMS                   FlugplanBms;          
BUFFER<SBBMS>           FlugplanIconBms;
SBBMS                   gInfoBms;             //Der Infobutton in der Routenliste

SBBMS                   RuneBms;

SBBMS                   gUniversalPlaneBms;
SBBMS                   gEditorPlane2dBms;

SBBMS                   gSmokeBms;
SBBMS                   gStenchBms;
CSmoker                 Smokers[5];

//--------------------------------------------------------------------------------------------
//Das Inventar und das schwarze Brett:
//--------------------------------------------------------------------------------------------
SBBMS                   gItemBms;
CTafelData              TafelData;
CAuftraege              LastMinuteAuftraege;  //Die hängen gerade aus
CAuftraege              ReisebueroAuftraege;  //Die hängen gerade aus
CFrachten               gFrachten;            //Die Frachtaufträge
CAuftraege              AuslandsAuftraege[MAX_CITIES];  //Aus dem Ausland
SLONG                   AuslandsRefill[MAX_CITIES];     //Aus dem Ausland
CFrachten               AuslandsFrachten[MAX_CITIES];   //Aus dem Ausland
SLONG                   AuslandsFRefill[MAX_CITIES];    //Aus dem Ausland

//--------------------------------------------------------------------------------------------
//Die Soundeffekte:
//--------------------------------------------------------------------------------------------
SSE                    *gpSSE=NULL;           // Die Sound-Engine ist global
MIDI                   *gpMidi=NULL;
FX                     *gpClickFx=NULL;
FX                     *gpPlaneFx=NULL;
SBFX                    gDoorOpen;
SBFX                    gDoorClose;
SBFX                    gMovePaper;
SBFX                    gToiletFx;
SBFX                    gArabDoorFx;
SBFX                    gUniversalFx;
SBFX                    gUniversalPlaneFx;
SLONG                   gUniversalPlaneFxCountdown=0;

CAmbienteManager        AmbientManager;

BOOL                    bIngnoreNextDoor=FALSE;
SLONG                   AnzPeopleOnScreen=0;
SLONG                   AnzPlanesOnScreen=0;
SLONG                   LastAnzPeopleOnScreen=0;
SLONG                   LastAnzPlanesOnScreen=0;

//--------------------------------------------------------------------------------------------
//Für Dialog- und Tip-Ressourcen:
//--------------------------------------------------------------------------------------------
const char              TOKEN_ADVICE[]   = "Advi";
const char              TOKEN_AKTIE[]    = "Akti";
const char              TOKEN_ARAB[]     = "Arab";
const char              TOKEN_AUFSICHT[] = "Aufs";
const char              TOKEN_AUFTRAG[]  = "Auft";
const char              TOKEN_BANK[]     = "Bank";
const char              TOKEN_BOSS[]     = "Boss";
const char              TOKEN_BUERO[]    = "Buro";
const char              TOKEN_CITY[]     = "City";
const char              TOKEN_CREDITS[]  = "Cred";
const char              TOKEN_DUTYFREE[] = "Duty";
const char              TOKEN_EXPERT[]   = "XPrt";
const char              TOKEN_FILOFAX[]  = "Filo";
const char              TOKEN_FRACHT[]   = "Frac";
const char              TOKEN_ITEM[]     = "Item";
const char              TOKEN_JOBS[]     = "Jobs";
const char              TOKEN_KIOSK[]    = "Kios";
const char              TOKEN_KONTO[]    = "Kont";
const char              TOKEN_LASTMIN[]  = "Last";
const char              TOKEN_LETTER[]   = "Lett";
const char              TOKEN_MAKLER[]   = "Makl";
const char              TOKEN_MECH[]     = "Mech";
const char              TOKEN_MISC[]     = "Misc";
const char              TOKEN_MUSEUM[]   = "Muse";
const char              TOKEN_MONEY[]    = "Mony";
const char              TOKEN_NASA[]     = "Nasa";
const char              TOKEN_PASSENGER[]= "Pass";
const char              TOKEN_PERSONAL[] = "Pers";
const char              TOKEN_PLANE[]    = "Plne";
const char              TOKEN_PLAYER[]   = "Play";
const char              TOKEN_ROUTE[]    = "Rout";
const char              TOKEN_RICK[]     = "Rick";
const char              TOKEN_SABOTAGE[] = "Sabo";
const char              TOKEN_SCHED[]    = "Schd";
const char              TOKEN_TANK[]     = "Tank";
const char              TOKEN_TOOLTIP[]  = "Tool";
const char              TOKEN_TUTORIUM[] = "Tuto";
const char              TOKEN_WELT[]     = "Welt";
const char              TOKEN_WERBUNG[]  = "Werb";
const char              TOKEN_SECURITY[] = "Secu";
const char              TOKEN_DESIGNER[] = "Desi";

//--------------------------------------------------------------------------------------------
//Die Mood-Bms:
//--------------------------------------------------------------------------------------------
const SLONG             MoodPlayerBase1      = 0;      //Spieler
const SLONG             MoodPersonBase2      = 5;      //Customers
const SLONG             MoodPersonBase3      = 13;     //Anonyme Kunden
const SLONG             MoodPersonRepair     = 5;      //Kunde meint: Flugzeug ist Schrott
const SLONG             MoodPersonSick       = 6;      //Kunde meint: Scheiß Kapitän
const SLONG             MoodPersonSmile      = 7;      //Kunde meint: war guter Flug!
const SLONG             MoodPersonFlying     = 8;      //Kunde will zum Abflug
const SLONG             MoodPersonMissed     = 9;      //Kunde hat Flug verpasst
const SLONG             MoodPersonTicket     = 10;     //Kunde hat Ticket
const SLONG             MoodPersonExpensive  = 11;     //Kunde meint: zu teuer
const SLONG             MoodPerson2Stars     = 12;     //Kunde meint: Recht gut
const SLONG             MoodPerson3Stars     = 13;     //Kunde meint: Sehr Gut
const SLONG             MoodPersonBank       = 14;
const SLONG             MoodPersonBankRobber = 15;
const SLONG             MoodPersonBeverage   = 16;
const SLONG             MoodPersonSchokolade = 17;
const SLONG             MoodPersonNone       = 18;
const SLONG             MoodPersonReise      = 19;
const SLONG             MoodPersonHome       = 20;
const SLONG             MoodPersonScreen     = 21;
const SLONG             MoodPersonMuseum     = 22;
const SLONG             MoodPersonToilet     = 23;
const SLONG             MoodPersonBone       = 24;
const SLONG             MoodPersonStench     = 25;      //Kunde (neutral) meint: Mir stinkts
const SLONG             MoodPersonStuck      = 26;      //Spieler ist klebengeblieben
const SLONG             MoodPersonStenchDog  = 27;      //Gestank aus Hundesicht
const SLONG             MoodPersonEmpty      = 28;      //Leere Gedankenblase für Netzwerkunterhaltungen
const SLONG             MoodPersonRepairFC   = 29;      //Kunde meint: Flugzeug ist Schrott
const SLONG             MoodPersonSickFC     = 30;      //Kunde meint: Scheiß Kapitän
const SLONG             MoodPersonSmileFC    = 31;      //Kunde meint: war guter Flug!
const SLONG             MoodPersonFlyingFC   = 32;      //Kunde will zum Abflug
const SLONG             MoodPersonMissedFC   = 33;      //Kunde hat Flug verpasst
const SLONG             MoodPersonTicketFC   = 34;      //Kunde hat Ticket
const SLONG             MoodPersonExpensiveFC= 35;      //Kunde meint: zu teuer
const SLONG             MoodPerson2StarsFC   = 36;      //Kunde meint: Recht gut
const SLONG             MoodPerson3StarsFC   = 37;      //Kunde meint: Sehr Gut

//--------------------------------------------------------------------------------------------
//Die Spielziele:
//--------------------------------------------------------------------------------------------
const SLONG             TARGET_PASSENGERS=2500;        //Spielziel Level 0
const SLONG             TARGET_GEWINN=5000000;         //Spielziel Level 1
const SLONG             TARGET_FLAGS=10;               //Spielziel Level 2
const SLONG             TARGET_IMAGE=750;              //Spielziel Level 3

//--------------------------------------------------------------------------------------------
//Die Spielziele im Add-On:
//--------------------------------------------------------------------------------------------
const SLONG             TARGET_FRACHT1=2500;
const SLONG             TARGET_FRACHT2=2500;
const SLONG             TARGET_FLUGKILOMETER=2500;
const SLONG             TARGET_DAYSTRAVEL=25;          //Tage ohne Reisebüro
const SLONG             TARGET_AKTIENKURS=200;
const SLONG             TARGET_FRACHT=1000;
const SLONG             TARGET_DAYS=21;
const SLONG             TARGET_MILESDAYS=30;
const SLONG             TARGET_SERVICE=150;
const SLONG             TARGET_VALUEDAYS=21;
const SLONG             TARGET_SHARES=220;
const SLONG             TARGET_NUM_UHRIG=200;

const SLONG             BTARGET_PASSENGERS=5000;  //Spielziel ATFS-Level 1
const SLONG             BTARGET_KONTO=15000000;   //Spielziel ATFS-Level 1
const SLONG             BTARGET_ZUSTAND=90;       //Spielziel ATFS-Level 2
const SLONG             BTARGET_PASSAVG=500;      //Spielziel ATFS-Level 3
const SLONG             BTARGET_DAYSSABO=15;      //Spielziel ATFS-Level 4+6
const SLONG             BTARGET_PLANESIZE=600;    //Spielziel ATFS-Level 5
const SLONG             BTARGET_KURS=200;         //Spielziel ATFS-Level 7
const SLONG             BTARGET_MEINANTEIL=20;    //Spielziel ATFS-Level 7
const SLONG             BTARGET_DAYS7=30;         //Spielziel ATFS-Level 7
const SLONG             BTARGET_VERBRAUCH=500;    //Spielziel ATFS-Level 8
const SLONG             BTARGET_NDAYS9=45;        //Spielziel ATFS-Level 9
const SLONG             BTARGET_NDAYS10=60;       //Spielziel ATFS-Level 10
