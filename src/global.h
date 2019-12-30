#ifdef DEBUG
extern void __cdecl Msg( LPSTR fmt, ... );
#else
#define Msg ; / ## /
#endif

//--------------------------------------------------------------------------------------------
//Zu erst die Ausnahmen:
//--------------------------------------------------------------------------------------------
extern char                   *ExcNever;         //Interne Dinge; Sicherheitshalber abgefragt
extern char                   *ExcCreateWindow;  //Fenster konnte nicht erzeugt werden

//--------------------------------------------------------------------------------------------
//Dinge für den Überblendeffekt:
//--------------------------------------------------------------------------------------------
extern SBBM                    gBlendBm;
extern SBBM                    gBlendBm2;
extern SLONG                   gBlendState;
extern SLONG                   gFramesToDrawBeforeFirstBlend;

//--------------------------------------------------------------------------------------------
//Flags für Cheats:
//--------------------------------------------------------------------------------------------
extern BOOL                    CheatRunningman;
extern BOOL                    CheatMoreNuns;
extern BOOL                    CheatMoreGimmicks;
extern BOOL                    CheatBubbles;
extern BOOL                    CheatShowAll;
extern BOOL                    CheatMissions;
extern BOOL                    CheatBerater;
extern BOOL                    CheatAnimNow;
extern BOOL                    CheatTestGame;

//--------------------------------------------------------------------------------------------
//Die Zufallsgeneratoren:
//--------------------------------------------------------------------------------------------
extern TEAKRAND                PersonRandCreate; //Alles, was die Personen angeht, die erzeugt werden
extern TEAKRAND                PersonRandMisc;   //Alles, was die Personen angeht
extern TEAKRAND                HeadlineRand;     //Alles, was die zeitungen Angeht

//--------------------------------------------------------------------------------------------
//Die Registratur:
//--------------------------------------------------------------------------------------------
extern CRegistration           Registration;
extern SLONG                   MissionKeys[];   //Die Schlüsselwerte für die Registry
extern SLONG                   MissionKeys2[];  //Die Schlüsselwerte für die Registry
extern SLONG                   MissionKeys3[];  //Die Schlüsselwerte für die Registry

//--------------------------------------------------------------------------------------------
//Pointer auf Fenster:
//--------------------------------------------------------------------------------------------
extern CStdRaum               *TopWin;          //Übergeordnetes Fenster, z.B. load Airport

//--------------------------------------------------------------------------------------------
//Einige *WIRKLICH* globale Ressourcen:
//--------------------------------------------------------------------------------------------
extern SLONG                   gLanguage;
extern SLONG                   gShowAllPools;
extern SLONG                   gUpdatingPools;
extern CBench                  Bench;
extern GameFrame              *FrameWnd;         //Das Rahmenfenster;
extern SDL_Renderer           *lpDD;             //DirectDraw object
extern SBPRIMARYBM             PrimaryBm;        //DirectDraw primary & back surface
extern SB_CCursor             *pCursor;
extern SLONG                   gRepairPrice[];
extern SLONG                   gWerbePrice[];
extern SLONG                   RightAirportClip;
extern SB_CBitmapMain         *bitmapMain;
extern GfxMain                *pGfxMain;
extern GfxLib                 *pGLibStd;
extern GfxLib                 *pGLibBrick;
extern GfxLib                 *pGLibClan;
extern GfxLib                 *pGLibBasis;
extern GfxLib                 *pGLibPlanes;
extern GfxLib                 *pGLibBerater;
extern GfxLib                 *pGlowEffectLib;
extern SLONG                   PlayerDidntMove;  //Zeit in 20stel Sekunden

extern SBBMS                   gGlowBms;
extern SBBM                    gNumberTemplate;

extern CHLPool                 SkelettPool;

extern CVoiceScheduler         VoiceScheduler;

extern bool                    bVoicesNotFound;

//--------------------------------------------------------------------------------------------
//Maus & Tooltips:
//--------------------------------------------------------------------------------------------
extern BOOL							 gUseWindowsMouse; //Keine eigene, sondern die Maus von Windows?
extern XY                      gMousePosition;   //Position des Mauscursors
extern XY                      gMouseClickPosition;   //Position des Mauscursors zum Zeitpunkt des MouseDown-Events
extern BOOL							 gMouseLButton;	 //Status des linken Mausknopfes
extern BOOL							 gMouseRButton;	 //Status des rechten Mausknopfes
extern SLONG						 gMouseLButtonDownTimer;	 //Timerinhalt, wenn der LMB gedrückt wurde
extern BOOL							 gMouseScroll;     //Mouse-Scrolling für den Airport?
extern SLONG                   MouseLook;        //Normal, Hot, Page
extern SLONG                   MouseClickArea;   //In Statusleiste/Raum
extern SLONG                   MouseClickId;     //Der Id
extern SLONG                   MouseClickPar1;
extern SLONG                   MouseClickPar2;
extern XY                      ToolTipPos;
extern SLONG                   ToolTipId;
extern SLONG                   ToolTipTimer;
extern BOOL                    ToolTipState;
extern XY                      ToolTipNewPos;
extern SLONG                   ToolTipNewId;
extern CString                 ToolTipString;
extern SLONG                   MouseWait;
extern SLONG                   gMouseStartup;

extern SBBM                    gToolTipBm;
extern SBBM                    gCursorBm;        //Bitmap des Cursors
extern SBBM                    gCursorLBm;       //Pfeil nach Link
extern SBBM                    gCursorRBm;       //Pfeil nach Rechts
extern SBBM                    gCursorHotBm;
extern SBBMS                   gCursorExitBms;
extern SBBM                    gCursorMoveHBm;
extern SBBM                    gCursorMoveVBm;
extern SBBM                    gCursorSandBm; 
extern SBBM                    gCursorNoBm;

extern SBBMS                   gCursorFeetBms[2]; //Schuhe nach links und nach rechts
extern SLONG                   gShowCursorFeet;

extern SBBM                    gDialogBarBm; 
extern SBBMS                   gToolTipBms; 

extern BOOL                    gRoomJustLeft;    //TRUE, wenn Raum gerade verlassen wurde (wegen messagePump während TransBlit)

//--------------------------------------------------------------------------------------------
//Text-Ressourcen:
//--------------------------------------------------------------------------------------------
extern TEXTRES                 DialogTexte;      //Die Text-Ressourcen der Dialoge
extern TEXTRES                 StandardTexte;    //Allgemeine Texte
extern TEXTRES                 ETexte;           //Die Einheiten

//--------------------------------------------------------------------------------------------
//einige Flags:
//--------------------------------------------------------------------------------------------
extern BOOL                    bTest;            //Schneller Ablauf zum testen
extern BOOL                    bActive;          //is application active?
extern BOOL                    bFullscreen;      //is application Fullscreen or in Window?
extern BOOL                    bCheatMode;       //Ist der Cheatmode zum testen aktiviert?
extern BOOL                    bQuick;           //Depeche Mode = Alles auf die Schnelle..
extern BOOL                    bgWarp;           //Spieler warpt zum Ziel
extern BOOL                    bNoVgaRam;        //Keine Bitmaps ins VGA-Ram legen
extern BOOL                    bCheatMode;       //Ist der Cheatmode zum testen aktiviert?
extern BOOL                    bNoSpeedyBar;     //Kein SpeedUp für die Statuszeile
extern BOOL                    bNoQuickMouse;    //Keine Maus-Aufrufe beim OnMouseMove
extern BOOL                    bCursorCaptured;  //Cursor gekidnappt und muß jetzt selbst gemalt werden?
extern BOOL                    bMidiAvailable;   //Ist ein Midi-Sequenzer vorhanden?
extern SLONG                   nOptionsOpen;     //Fummelt gerade wer an den Options?
extern SLONG                   nAppsDisabled;    //Ist ein anderer Spieler gerade in einer anderen Anwendung?
extern SLONG                   nWaitingForPlayer;//Müssen wir da auf einen anderen Spieler warten?
extern BOOL                    bTwoWins;         //SplitScreen Testen?
extern SLONG                   Editor;           //Ist der Flughafeneditor aktiv ? (=0)
extern BOOL                    gDisablePauseKey; //Pause-Key für Texteingabe abgeschaltet?
extern BOOL                    bgJustDidLotsOfWork; //Gegen Sprünge nach Load/Save
extern BOOL                    bLeaveGameLoop;   //Hauptschleife verlassen?
extern BOOL                    bFirstClass;

//--------------------------------------------------------------------------------------------
//Das Spiel höchstpersönlich:
//--------------------------------------------------------------------------------------------
extern CTakeOffApp             theApp;

//--------------------------------------------------------------------------------------------
//Einfache globale Variablen:
//--------------------------------------------------------------------------------------------
extern char                   *TabSeparator;          //Zum Import der Excel-Tabellen
extern char                   *DateSeparator;         //Erlaubte Datums-Separatoren
extern char                   *HallFilenames[];       //Die Filenamen der Airport-Hallen


//--------------------------------------------------------------------------------------------
//Die Pfade der einzelnen Dateigruppen:
//--------------------------------------------------------------------------------------------
extern CString                 BitmapPath;            //diverse Bitmaps
extern CString                 BrickPath;             //Hier sind die Part-Dateien
extern CString                 CityPath;              //Städte Bitmaps
extern CString                 ClanPath;              //Clan Bitmaps
extern CString                 ExcelPath;             //Hier sind die csv-Tabellen
extern CString                 MiscPath;              //Sonstige Kleinigkeiten
extern CString                 SavegamePath;          //Die Spielstände
extern CString                 ScanPath;              //gescannte Bilder
extern CString                 SoundPath;             //Musikstücke und Samples
extern CString                 RoomPath;              //Bilder für die Räume
extern CString                 PlanePath;             //Bilder der Flugzeuge
extern CString                 GliPath;               //diverse GLI-Dateien
extern CString                 SmackerPath;           //Die Smacker-Filmchen
extern CString                 IntroPath;             //Der Intro-Film
extern CString                 AppPath;               //Der Ganze Pfad, z.B. "f:\project\takeoff\"
extern CString                 VoicePath;             //die Sprache
extern CString                 MyPlanePath;           //die eigenen Flugzeuge

//--------------------------------------------------------------------------------------------
//Die Simulationswelt mit ihren Parameter (Zeit, Spieler, Schwierigkeit, ..)
//--------------------------------------------------------------------------------------------
extern SIM                     Sim;
extern CWorkers                Workers;
extern CTalkers                Talkers;
extern CKlackerPlanes          gKlackerPlanes;

//--------------------------------------------------------------------------------------------
//Globale Daten und Tabellen:
//--------------------------------------------------------------------------------------------
extern BRICKS                  Bricks;      //Die Bauteile
extern AIRPORT                 Airport;     //und ihre Verwendung
extern CLANS                   Clans;
extern CPlaneTypes             PlaneTypes;
extern CITIES                  Cities;
extern CPlaneNames             PlaneNames;
extern CRouten                 Routen;
extern BUFFER<CEinheit>        Einheiten;
extern BUFFER<BUFFER_UWORD>    GlobeMapper;
extern BUFFER<BUFFER_UBYTE>    GlobeLight;
extern BUFFER<SLONG>           GlobeMapperY;
extern BUFFER<UWORD>           GlobeMixTab;
extern BUFFER<XY>              GlobeWindows;
       
//--------------------------------------------------------------------------------------------
//Der Klacker-Font:
//--------------------------------------------------------------------------------------------
extern const char *KlackerFntDef;

//--------------------------------------------------------------------------------------------
//Text-Style des Spielers
//--------------------------------------------------------------------------------------------
extern SB_CFont                FontCash;
extern SB_CFont                FontSmallBlack;
extern SB_CFont                FontSmallGrey;
extern SB_CFont                FontSmallRed;
extern SB_CFont                FontSmallWhite;
extern SB_CFont                FontSmallWhiteX;
extern SB_CFont                FontSmallPlastic;

extern SB_CFont                FontCondensedBlack;
extern SB_CFont                FontCondensedGrey;
extern SB_CFont                FontCondensedRed;

extern SB_CFont                FontDialogPartner;
extern SB_CFont                FontDialogInk;
extern SB_CFont                FontDialog;
extern SB_CFont                FontDialogLight;
extern SB_CFont                FontNormalGrey;
extern SB_CFont                FontNormalGreen;
extern SB_CFont                FontBigGrey;
extern SB_CFont                FontBigWhite;

extern UWORD                   ColorOfFontBlack;
extern UWORD                   ColorOfFontGrey;
extern UWORD                   ColorOfFontRed;

extern XY                      ReferenceCursorPos;      //Kopie von CursorPos, die nur pro Frame upgedaten wird
extern CRect                   gHighlightArea;          //Für das Text-Highlighting
extern UWORD                   gHighlightFontColor;
extern CRect                   gLastHighlightArea;

extern TABS                    TABLeft[];
extern TABS                    TABRight[];

//--------------------------------------------------------------------------------------------
//Die Statuszeilen und Textzeilen: normal+Editoren
//--------------------------------------------------------------------------------------------
extern SBBMS                   StatusLineBms;
extern SBBMS                   gTutoriumBms;            //Statuszeile: Tutorium läuft
extern SBBMS                   gRepeatMessageBms;       //Statuszeile: Message noch einmal, bitte
extern SBBMS                   gStatisticBms;           //Statuszeile: Statistiken
extern SBBMS                   gStatLineHigh;           //Statuszeile: Highlights für Geld und Zeit
extern SBBMS                   gClockBms;
extern SLONG                   gStatButton;
extern SLONG                   gStatButtonTimer;
extern BUFFER<SBBM>            TextBricks;
extern BUFFER<CString>         TextBrickTexts;

extern SBBMS                   gNetworkBms;             //Network-Message: Waiting
extern SLONG                   gNetworkBmsType;         //Waiting for unknown (0) bReadyForMorning (1), bReadyForBriefing (2)
extern SBBM                    gBroadcastBm;            //Bitmap mit dem Broadcast-chattext
extern SLONG                   gBroadcastTimeout;       //Noch soviele 1/20 Sekunden sichtbar

//--------------------------------------------------------------------------------------------
//Bitmaps der Berater:
//--------------------------------------------------------------------------------------------
extern BUFFER<SBBMS>           BeraterBms;
extern SBBMS                   SprechblasenBms;
extern SBBMS                   XBubbleBms;

extern BOOL                    IsPaintingTextBubble;
extern BOOL                    PleaseCancelTextBubble;

//--------------------------------------------------------------------------------------------
//Der Mehrzwecke Menü-Hintergrund fürs aktuelle Zeitalter & die aktuelle Bildgröße und die Tips
//--------------------------------------------------------------------------------------------
extern SBBM                    gNotepadButtonL;
extern SBBM                    gNotepadButtonM;
extern SBBM                    gNotepadButtonR;
extern SBBMS                   gZettelBms;

extern SBBMS                   gPostItBms;           //Small & Blue/Green

extern SBBM                    gCrossBm;             //vom Scheduler
extern SBBM                    gAusrufBm;            //vom Scheduler
extern SBBM                    gLockBm;              //vom Scheduler

extern SBBMS                   LogoBms;              //Die Logos der Fluggesellschaften
extern SBBMS                   SmallLogoBms;         //Die kleinen Logos der Fluggesellschaften
extern SBBMS                   TinyLogoBms;          //Für die Sprechblasen
extern SBBMS                   MoodBms;              //Die Spimmungs-Sprechblasen
extern SBBMS                   SmileyBms;            //Die Spimmungs-Köpfe für die Aussagen des Beraters

extern SBBM                    gCityMarkerBm;        //Marker für eine Stadt
extern SBBMS                   FlugplanBms;          
extern BUFFER<SBBMS>           FlugplanIconBms;
extern SBBMS                   gInfoBms;             //Der Infobutton in der Routenliste

extern SBBMS                   RuneBms;

extern SBBMS                   gUniversalPlaneBms;
extern SBBMS                   gEditorPlane2dBms;

extern SBBMS                   gSmokeBms;
extern SBBMS                   gStenchBms;
extern CSmoker                 Smokers[5];

//--------------------------------------------------------------------------------------------
//Das Inventar und die Zettel vom schwarzen Brett:
//--------------------------------------------------------------------------------------------
extern SBBMS                   gItemBms;
extern CTafelData              TafelData;
extern CAuftraege              LastMinuteAuftraege;    //Die hängen gerade aus
extern CAuftraege              ReisebueroAuftraege;    //Die hängen gerade aus
extern CFrachten               gFrachten;              //Die Frachtaufträge
extern CAuftraege              AuslandsAuftraege[MAX_CITIES];  //Aus dem Ausland
extern SLONG                   AuslandsRefill[MAX_CITIES];     //Aus dem Ausland
extern CFrachten               AuslandsFrachten[MAX_CITIES];   //Aus dem Ausland
extern SLONG                   AuslandsFRefill[MAX_CITIES];    //Aus dem Ausland

//--------------------------------------------------------------------------------------------
//Die Soundeffekte:
//--------------------------------------------------------------------------------------------
extern SSE                    *gpSSE;           // Die Sound-Engine ist global
extern MIDI                   *gpMidi;
extern FX                     *gpClickFx;
extern FX                     *gpPlaneFx;
extern SBFX                    gDoorOpen;
extern SBFX                    gDoorClose;
extern SBFX                    gMovePaper;
extern SBFX                    gToiletFx;
extern SBFX                    gArabDoorFx;
extern SBFX                    gUniversalFx;
extern SBFX                    gUniversalPlaneFx;
extern SLONG                   gUniversalPlaneFxCountdown;


extern CAmbienteManager        AmbientManager;

extern BOOL                    bIngnoreNextDoor;
extern SLONG                   AnzPeopleOnScreen;
extern SLONG                   AnzPlanesOnScreen;
extern SLONG                   LastAnzPeopleOnScreen;
extern SLONG                   LastAnzPlanesOnScreen;

//--------------------------------------------------------------------------------------------
//Für Dialog-Ressourcen:
//--------------------------------------------------------------------------------------------
extern const char              TOKEN_ADVICE[];
extern const char              TOKEN_AKTIE[];
extern const char              TOKEN_ARAB[];
extern const char              TOKEN_AUFSICHT[];
extern const char              TOKEN_AUFTRAG[];
extern const char              TOKEN_BANK[];
extern const char              TOKEN_BOSS[];
extern const char              TOKEN_BUERO[];
extern const char              TOKEN_CITY[];
extern const char              TOKEN_CREDITS[];
extern const char              TOKEN_DUTYFREE[];
extern const char              TOKEN_EXPERT[];
extern const char              TOKEN_FILOFAX[];
extern const char              TOKEN_FRACHT[];
extern const char              TOKEN_ITEM[];
extern const char              TOKEN_JOBS[];
extern const char              TOKEN_KIOSK[];
extern const char              TOKEN_KONTO[];
extern const char              TOKEN_LASTMIN[];
extern const char              TOKEN_LETTER[];
extern const char              TOKEN_MAKLER[];
extern const char              TOKEN_MECH[];
extern const char              TOKEN_MISC[];
extern const char              TOKEN_MUSEUM[];
extern const char              TOKEN_MONEY[];
extern const char              TOKEN_NASA[];
extern const char              TOKEN_PASSENGER[];
extern const char              TOKEN_PERSONAL[];
extern const char              TOKEN_PLANE[];
extern const char              TOKEN_PLAYER[];
extern const char              TOKEN_ROUTE[];
extern const char              TOKEN_RICK[];
extern const char              TOKEN_SABOTAGE[];
extern const char              TOKEN_SCHED[];
extern const char              TOKEN_TANK[];
extern const char              TOKEN_TOOLTIP[];
extern const char              TOKEN_TUTORIUM[];
extern const char              TOKEN_WELT[];
extern const char              TOKEN_WERBUNG[];
extern const char              TOKEN_SECURITY[];
extern const char              TOKEN_DESIGNER[];

//--------------------------------------------------------------------------------------------
//Die Mood-Bms:
//--------------------------------------------------------------------------------------------
extern const SLONG             MoodPlayerBase1;
extern const SLONG             MoodPersonBase2;
extern const SLONG             MoodPersonBase3;
extern const SLONG             MoodPlayerBase;
extern const SLONG             MoodPersonRepair;
extern const SLONG             MoodPersonSick;
extern const SLONG             MoodPersonSmile;
extern const SLONG             MoodPersonFlying;
extern const SLONG             MoodPersonMissed;
extern const SLONG             MoodPersonTicket;
extern const SLONG             MoodPersonExpensive;
extern const SLONG             MoodPerson2Stars;
extern const SLONG             MoodPerson3Stars;
extern const SLONG             MoodPersonBank;
extern const SLONG             MoodPersonBankRobber;
extern const SLONG             MoodPersonBeverage;
extern const SLONG             MoodPersonSchokolade;
extern const SLONG             MoodPersonNone;
extern const SLONG             MoodPersonReise;
extern const SLONG             MoodPersonHome;
extern const SLONG             MoodPersonScreen;
extern const SLONG             MoodPersonMuseum;
extern const SLONG             MoodPersonToilet;
extern const SLONG             MoodPersonBone;
extern const SLONG             MoodPersonStench;
extern const SLONG             MoodPersonStuck;
extern const SLONG             MoodPersonStenchDog;
extern const SLONG             MoodPersonEmpty;
extern const SLONG             MoodPersonRepairFC;
extern const SLONG             MoodPersonSickFC;
extern const SLONG             MoodPersonSmileFC;
extern const SLONG             MoodPersonFlyingFC;
extern const SLONG             MoodPersonMissedFC;
extern const SLONG             MoodPersonTicketFC;
extern const SLONG             MoodPersonExpensiveFC;
extern const SLONG             MoodPerson2StarsFC;
extern const SLONG             MoodPerson3StarsFC;

//--------------------------------------------------------------------------------------------
//Die Spielziele:
//--------------------------------------------------------------------------------------------
extern const SLONG             TARGET_PASSENGERS;     //Spielziel Level 0
extern const SLONG             TARGET_GEWINN;         //Spielziel Level 1
extern const SLONG             TARGET_FLAGS;          //Spielziel Level 2
extern const SLONG             TARGET_IMAGE;          //Spielziel Level 3
extern const SLONG             TARGET_FRACHT;         //AddOn-Level 2
extern const SLONG             TARGET_DAYS;           //AddOn-Level 3
extern const SLONG             TARGET_MILESDAYS;      //AddOn-Level 4
extern const SLONG             TARGET_SERVICE;        //AddOn-Level 5
extern const SLONG             TARGET_VALUEDAYS;      //AddOn-Level 6
extern const SLONG             TARGET_SHARES;         //AddOn-Level 8
extern const SLONG             TARGET_NUM_UHRIG;      //AddOn-Level 9

extern const SLONG             BTARGET_PASSENGERS;    //Spielziel ATFS-Level 1
extern const SLONG             BTARGET_KONTO;         //Spielziel ATFS-Level 1
extern const SLONG             BTARGET_ZUSTAND;       //Spielziel ATFS-Level 2
extern const SLONG             BTARGET_PASSAVG;       //Spielziel ATFS-Level 3
extern const SLONG             BTARGET_DAYSSABO;      //Spielziel ATFS-Level 4+6
extern const SLONG             BTARGET_PLANESIZE;     //Spielziel ATFS-Level 5
extern const SLONG             BTARGET_KURS;          //Spielziel ATFS-Level 7
extern const SLONG             BTARGET_MEINANTEIL;    //Spielziel ATFS-Level 7
extern const SLONG             BTARGET_DAYS7;         //Spielziel ATFS-Level 7
extern const SLONG             BTARGET_VERBRAUCH;     //Spielziel ATFS-Level 8
extern const SLONG             BTARGET_NDAYS9;        //Spielziel ATFS-Level 9
extern const SLONG             BTARGET_NDAYS10;       //Spielziel ATFS-Level 10
