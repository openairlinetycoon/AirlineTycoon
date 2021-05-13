
//============================================================================================
// Defines.h - globale Konstanten:
//============================================================================================

//Bei DEMO werden folgende Änderungen gemacht:
// * Keine Werbeagentur, Insel, Nasa
// * Exception, wenn der Boss für die höheren Missionen spricht
// * Keine Voices
// * Beschränkung auf 100 Tage
//#define DEMO            //Beschränkung der Missionen und einiges mehr
//#define NO_D_VOICES     //Keine deutschen Voices, Sprechblasen werden eingeschaltet
//#define NO_E_VOICES     //Keine englischen Voices, Sprechblasen werden eingeschaltet
//#define NO_N_VOICES     //Keine niernerländischen Voices, Sprechblasen werden eingeschaltet
//#define NO_INTRO        //Kein Intro, d.h. er wird übersprungen

//#define VOICES_OPTIONAL //Voices werden verwendet, wenn sie da sind; ansonsten auch nicht schlimm

//Meine (konsequenten) Variablentypen
typedef unsigned short UWORD;
typedef   signed short SWORD;

typedef unsigned char UBYTE;
typedef   signed char SBYTE;

#ifdef WIN32
typedef   signed long SLONG;
#else
typedef   signed int SLONG;
typedef unsigned int ULONG;
typedef          int LONG;
typedef          int LPARAM;
typedef unsigned int DWORD;
typedef unsigned int COLORREF;

typedef          short SHORT;
typedef unsigned short USHORT;
typedef unsigned short WORD;

typedef          float FLOAT;

typedef          int BOOL;
typedef          int INT;
typedef unsigned int UINT;
typedef unsigned int WPARAM;

typedef          char CHAR;
typedef unsigned char UCHAR;
typedef unsigned char BYTE;

typedef const char* LPCSTR;
typedef       char* LPSTR;

typedef const wchar_t* LPCWSTR;
typedef       wchar_t* LPWSTR;

typedef const char* LPCTSTR;
typedef       char* LPTSTR;

typedef long long __int64;

typedef struct tagPOINT
{
   LONG x;
   LONG y;
} POINT;

typedef struct tagRECT
{
   LONG left;
   LONG top;
   LONG right;
   LONG bottom;
} RECT;
#endif

class CPoint : public tagPOINT
{
public:
   CPoint()
   {
      x = y = 0;
   }

   CPoint(const POINT& initPoint)
   {
      x = initPoint.x;
      y = initPoint.y;
   }

   CPoint(LONG initX, LONG initY)
   {
      x = initX;
      y = initY;
   }

   CPoint(LPARAM dwPoint)
   {
      x = dwPoint & 0xFFFF;
      y = (dwPoint >> 16) & 0xFFFF;
   }
};

class CRect : public tagRECT
{
public:
   CRect()
   {
      left = top = right = bottom = 0;
   }

   CRect(const RECT& srcRect)
   {
      left = srcRect.left;
      top = srcRect.top;
      right = srcRect.right;
      bottom = srcRect.bottom;
   }

   CRect(LONG l, LONG t, LONG r, LONG b)
   {
      left = l;
      top = t;
      right = r;
      bottom = b;
   }

   LONG Width() const { return right - left; }
   LONG Height() const { return bottom - top; }
};

inline CRect operator+(CRect r, const CPoint& p)
{
   return CRect(r.left + p.x, r.top + p.y, r.right + p.x, r.bottom + p.y);
}

inline CRect operator-(CRect r, const CPoint& p)
{
   return CRect(r.left - p.x, r.top - p.y, r.right - p.x, r.bottom - p.y);
}

inline bool operator==(const CPoint& lhs, const CPoint& rhs)
{
   return lhs.x == rhs.x && lhs.y == lhs.y;
}

inline bool operator!=(const CPoint& lhs, const CPoint& rhs)
{
   return !(lhs == rhs);
}

//Was wohl:
#define FEMALE 0
#define MALE   1

//Generelles
#define CUSTOMERS_PER_PERSON  15       //Eine Person (Bildschirm) repräsentiert 10 Kunden
#define CUSTOMERS_PER_PERSONB 45       //Wenn schon 100 an Bord sind, geht es hiermit weiter
#define DEBT_LIMIT           -1000000  //Max. Kontokorrentkredit
#define DEBT_WARNLIMIT1       -500000  //Warnung: Limit ist 1 Mio
#define DEBT_WARNLIMIT2      -1000000  //Warnung: Kein weiterer Kredit
#define DEBT_WARNLIMIT3      -3000000  //Warnung: Konkurs
#define DEBT_GAMEOVER        -5000000  //Konkurs
#define MOODCOUNT_START       60
#define BUILDHASHSIZE         320
#define DAYS_WITHOUT_LAPTOP   7

//Der Einfachheit halber Maximalwerte für ein paar Tabellen:
#define MAX_BODIES     30
#define MAX_TYCOONS    30
#define MAX_PLANETYPES 80
#define MAX_BRICKS     340
#define MAX_CITIES     80
#define MAX_CLANS      90
#define MAX_SPECIALS   128             //Zahl der Gates, Rooms, Check-Ins
#define MAX_ROUTES     370             //Zahl der vordefinierten Flugrouten
#define MAX_PNAMES1    200
#define MAX_PNAMES2    100
#define MAX_WORKERS    350             //Maximale Zahl der Angestellten

//Sprache:
#define LANGUAGE_D       0             //D-Deutsch
#define LANGUAGE_E       1             //E-Englisch
#define LANGUAGE_F       2             //F-Französisch
#define LANGUAGE_T       3             //T-Taiwanesisch
#define LANGUAGE_P       4             //P-Polnisch
#define LANGUAGE_N       5             //N-Netherlands
#define LANGUAGE_I       6             //I-Italienisch
#define LANGUAGE_S       7             //S-Spanisch
#define LANGUAGE_O       8             //O-Portugisisch
#define LANGUAGE_B       9             //B-Brasiliasnisch (Fremdproduktion)
#define LANGUAGE_1      10             //J-noch frei
#define LANGUAGE_2      11             //K-noch frei
#define LANGUAGE_3      12             //L-noch frei
#define LANGUAGE_4      13             //M-noch frei
#define LANGUAGE_5      14             //N-noch frei
#define LANGUAGE_6      15             //Q-noch frei
#define LANGUAGE_7      16             //R-noch frei
#define LANGUAGE_8      17             //T-noch frei
#define LANGUAGE_9      18             //U-noch frei
#define LANGUAGE_10     19             //V-noch frei

//Die Bitmaps für jedes Flugzeug:
#define PLANE_SIDE       1             //Die Seitenansicht
#define PLANE_ICON       2             //Ikonifizierte Ansicht des Flugzeuges
#define PLANE_BLUEPRINT  3             //Technische Zeichnung von oben

//Die Schwierigkeitsgrade fürs Spiel:
#define DIFF_TUTORIAL    0             //Tutorium: 3 Aufträge
#define DIFF_FIRST       1             //5000 Passagiere befördern; keine 
#define DIFF_EASY        2
#define DIFF_NORMAL      3
#define DIFF_HARD        4
#define DIFF_FINAL       5             //Rakete zusammenbauen

#define DIFF_FREEGAME    -1
#define DIFF_FREEGAMEMAP  7

#define DIFF_ADDON       10            //Keine Mission, sondern nur ein Vergleichsmarker
#define DIFF_ADDON01     11            //Fracht: x Tonnen
#define DIFF_ADDON02     12            //Sanierung einer bankrotten Fluglinie
#define DIFF_ADDON03     13            //Fracht: Hilfsflüge x Tonnen
#define DIFF_ADDON04     14            //Flugkilometer
#define DIFF_ADDON05     15            //Kein Reisebüro
#define DIFF_ADDON06     16            //Alles modernisieren
#define DIFF_ADDON07     17            //Aktienkurs
#define DIFF_ADDON08     18            //Viele Aufträge
#define DIFF_ADDON09     19            //Service & Luxus
#define DIFF_ADDON10     20            //Weltraumstation

#define DIFF_ATFS        40            //Keine Mission, sondern nur ein Vergleichsmarker
#define DIFF_ATFS01      41            //Back again
#define DIFF_ATFS02      42            //Sicher ist sicher
#define DIFF_ATFS03      43            //Schnell und viel
#define DIFF_ATFS04      44            //Safety first
#define DIFF_ATFS05      45            //Schnell und viel mehr
#define DIFF_ATFS06      46            //Krisenmanagement
#define DIFF_ATFS07      47            //Schwarzer Freitag
#define DIFF_ATFS08      48            //Das 3 Liter Flugzeug
#define DIFF_ATFS09      49            //Die Kerosin Krise
#define DIFF_ATFS10      50            //Der Tycoon

//Die Art eines geplanten/durchgeführten Fluges:
#define FLUGTYPE_AUFTRAG   1
#define FLUGTYPE_EIGENFLUG 2
#define FLUGTYPE_LEERFLUG  3
#define FLUGTYPE_REPARATUR 4

//Die Dinge, die man dabei haben kann:
#define ITEM_NONE     0xff        //Leereintrag
#define ITEM_LAPTOP      0        //Der Laptop fürs Scheduling
#define ITEM_HANDY       1        //Telefon für Gespräche
#define ITEM_NOTEBOOK    2        //Notizbuch für die Warp-Funktion
#define ITEM_FILOFAX     2        //Synonym
#define ITEM_MG          3        //Für ArabAir
#define ITEM_BIER        4        //Für Mechaniker
#define ITEM_ZIGARRE     5        //
#define ITEM_OEL         6        //Öl für den Globus
#define ITEM_POSTKARTE   7        //
#define ITEM_TABLETTEN   8        //
#define ITEM_SPINNE      9        //
#define ITEM_DART       10        //
#define ITEM_DISKETTE   11        //
#define ITEM_BH         12        //
#define ITEM_HUFEISEN   13        //
#define ITEM_PRALINEN   14        //
#define ITEM_PRALINEN_A 15        //
#define ITEM_PAPERCLIP  16        //
#define ITEM_GLUE       17        //
#define ITEM_GLOVE      18        //
#define ITEM_REDBULL    19        //
#define ITEM_STINKBOMBE 20        //
#define ITEM_GLKOHLE    21        //
#define ITEM_KOHLE      22        //
#define ITEM_ZANGE      23        //
#define ITEM_PARFUEM    24        //
#define ITEM_XPARFUEM   25        //

//Die Eingabegeräte:
#define DEVICE_MOUSE       1
#define DEVICE_JOYSTICK1   2
#define DEVICE_JOYSTICK2   3
#define DEVICE_KEYBOARD    4

//Der aktuelle Initialisierungszustand der Sim-Daten
#define GAMESTATE_BOOT     0     //Direkt nach Programmstart; Inhalt zufällig
#define GAMESTATE_TITLE    1     //Titelbild/Intro wurde gezeigt
#define GAMESTATE_INIT     2     //Startangaben (Schwierigkeitsgraf, ..) wurden gemacht
#define GAMESTATE_PLAYING  3     //Es wird gespielt
#define GAMESTATE_QUIT     4     //Das Spiel wird verlassen
#define GAMESTATE_CREDITS  5     //Die Credits
#define GAMESTATE_OPTIONS  6     //Die Optionen
#define GAMESTATE_INTRO    7     //Der gerenderte Intro
#define GAMESTATE_OUTRO    8     //Der gerenderte Outro
#define GAMESTATE_OUTRO2   9     //Der gerenderte Outro vom Add-On
#define GAMESTATE_WORKING  32    //Wird gerade von einem parallelen Fenster verändert
#define GAMESTATE_DONE     64    //Wurde gerade verändert. Der alte Pointer kann gelöscht werden
#define GAMESTATE_PAUSED   128   //Zusätzliches Pauseflag

//Die verschiedenen Editoren:
#define EDITOR_NONE        0     //Kein Editor, sondern Flughafenansicht
#define EDITOR_BUILDS      1     //Die baulichen Objekte des Flughafens
#define EDITOR_TEST        2     //Der Editor zum Testen der Links
#define EDITOR_NODES       3     //Der Editor für die Nodes
#define EDITOR_LINKS       4     //Der Editor für die Node-Verbindungen

//Die verschiendenen Zustände der Personen
#define PERSON_BROWSING         0     //Person strohmert nur herrum
#define PERSON_2CHECKIN         1     //Person will zum Check-In
#define PERSON_CHECKINGIN       2     //Person steht in der Schlange
#define PERSON_2WAITROOM        3     //Person will zum Warteraum
#define PERSON_WAITING          4     //Person wartet im Warteraum
#define PERSON_BOARDING         5     //Person geht an Bord
#define PERSON_SITWAITING       6     //Person sitzt & wartet im Warteraum
#define PERSON_ENTERINGPL       7     //Person geht ins Flugzeug
#define PERSON_LEAVINGPL        8     //Person kommt aus dem Flugzeug
#define PERSON_2SHOP            10    //Person sucht ein Geschäft
#define PERSON_SHOPPING         11    //Person kauft im Geschäft ein
#define PERSON_EXSHOP           12    //Person geht zum Eingang/Ausgang
#define PERSON_DROPPING_SUICASE 13    //Person läßt den Koffer fallen
#define PERSON_2CHECKIN_EXIT    14    //Person will aus dem Check-In Bereich raus
#define PERSON_WAYPOINTING      15    //Person geht zum nächsten WayPoint
#define PERSON_2DURCHLEUCHTER   16    //Person geht durch den Durchleuchter am Gate
#define PERSON_2EXIT            30    //Person such den Ausgang
#define PERSON_LEAVING          31    //Person wird demnächst aussortiert
#define PERSON_TALKING          32    //Person schnackt mit Spieler
#define PERSON_BROWSEFLAG       64    //Person stöbert und läßt Platz für viele andere
#define PERSON_WAITFLAG         128   //zusätzliches Flag, wenn Person nur ab & zu betreut wird

//Die verschiedenen Clantypen:
#define CLAN_FEMALE        10
#define CLAN_MALE          11
#define CLAN_BROWNFEMALE   18
#define CLAN_BROWNMALE     19
#define CLAN_PLAYER1       21
#define CLAN_PLAYER2       22
#define CLAN_PLAYER3       23
#define CLAN_PLAYER4       24
#define CLAN_SUITCASE      30
#define CLAN_ANIMATION     40    //Einige spezielle Animationen werden auch als Clan verwaltet

//Der Verwendungszweck der Raumnummern:
#define ROOM_AIRPORT       1     //Der Flughafen an sich:
#define ROOM_OPTIONS       2     //Das Optionsmenü (Der Ausgang)
#define ROOM_STAIRSVON     50    //Bereich für...
#define ROOM_STAIRSBIS     55    //...alle Treppen
#define ROOM_STAIRS1UP     50    //Treppen nach oben fangen hier an
#define ROOM_STAIRS1DOWN   51    //Treppen nach unten fangen hier an
#define ROOM_STAIRS2UP     52    //Treppen nach oben fangen hier an
#define ROOM_STAIRS2DOWN   53    //Treppen nach unten fangen hier an
#define ROOM_STAIRS3UP     54    //Treppen nach oben fangen hier an
#define ROOM_STAIRS3DOWN   55    //Treppen nach unten fangen hier an
#define ROOM_GLOBE         6     //Globus
#define ROOM_LAPTOP        7     //Laptop
#define ROOM_ABEND         8     //Feierabend und Tagesanfang
#define ROOM_BURO_A        10    //Spieler1
#define ROOM_PERSONAL_A    11    //Spieler1
#define ROOM_BURO_B        20    //Spieler2
#define ROOM_PERSONAL_B    21    //Spieler2
#define ROOM_BURO_C        30    //Spieler3
#define ROOM_PERSONAL_C    31    //Spieler3
#define ROOM_BURO_D        40    //Spieler4
#define ROOM_PERSONAL_D    41    //Spieler4
#define ROOM_SHOP1         80    //Ein Raum zur Zierde (Duty Free)
#define ROOM_SHOP2         81    //Ein Raum zur Zierde
#define ROOM_SHOP3         82    //Ein Raum zur Zierde
#define ROOM_SHOP4         83    //Ein Raum zur Zierde
#define ROOM_RICKS         84    //Ein Raum zur Zierde
#define ROOM_ROUTEBOX      85    //Der Kasten mit den Routen
#define ROOM_BANK          100   //Das Kreditinstitut
#define ROOM_KIOSK         101   //Der Zeitungsstand
#define ROOM_MUSEUM        102   //Das Museum mit den gebrauchten Flugzeugen
#define ROOM_LAST_MINUTE   103   //Der Schalter der Last-Minute Agentur
#define ROOM_ARAB_AIR      104   //Der Schalter der ominösen Fluglinie
#define ROOM_MAKLER        105   //Der Makler für neue Flugzeug
#define ROOM_AUFSICHT      107   //Die Flughafenaufsicht beim Cheffe vom Airport
#define ROOM_TAFEL         108   //Die Tafel in der Flugaufsicht
#define ROOM_WERBUNG       109   //Werbeagentur
#define ROOM_WERKSTATT     110   //Werkstatt
#define ROOM_REISEBUERO    111   //Das Reisebüro der anderen Linien
#define ROOM_SABOTAGE      112   //Raum für die Sabotage
#define ROOM_WC_F          113   //Das Damen-WC
#define ROOM_WC_M          114   //Das Herren-WC
#define ROOM_NASA          115   //Das Büro der Nasa, wo man Dinge kaufen kann
#define ROOM_INSEL         116   //Die Raketenabschußrampe
#define ROOM_WORLD         117   //Die Weltkugel für internationale Gespräche
#define ROOM_RUSHMORE      118   //Die Weltkugel für internationale Gespräche
#define ROOM_STATISTICS    119   //Armin's Statistikliebling
#define ROOM_PLANEPROPS    120   //Die Planeproperties
#define ROOM_FRACHT        121   //Der Frachtschalter
#define ROOM_WELTALL       122   //Weltall und Raumstations
#define ROOM_SECURITY      123   //Der Sicherheitsraum
#define ROOM_EDITOR        124   //Der Raum des Flugzeugeditors
#define ROOM_DESIGNER      125   //Der Raum vor dem Flugzeugeditor
#define ROOM_FLIGHTDISPLAY 140   //140-169 bzw. 170-199 sind reserviert für die Fluganzeigetafeln
#define ROOM_REISE_X1      200   //Reisebüro für Gäste #1
#define ROOM_REISE_X2      201   //Reisebüro für Gäste #2
#define ROOM_REISE_X3      202   //Reisebüro für Gäste #3
#define ROOM_REISE_X4      203   //Reisebüro für Gäste #4
#define ROOM_LASTMIN_X1    210   //LastMinute für Gäste #1
#define ROOM_LASTMIN_X2    211   //LastMinute für Gäste #2
#define ROOM_LASTMIN_X3    212   //LastMinute für Gäste #3
#define ROOM_LASTMIN_X4    213   //LastMinute für Gäste #4
#define ROOM_BELT_X1       220   //Kofferband #1
#define ROOM_BELT_X2       221   //Kofferband #2
#define ROOM_BELT_X3       222   //Kofferband #3
#define ROOM_BELT_X4       223   //Kofferband #4
#define ROOM_BELT_X5       224   //Kofferband #5
#define ROOM_BELT_X6       225   //Kofferband #6
#define ROOM_BELT_X7       226   //Kofferband #7
#define ROOM_BELT_X8       227   //Kofferband #8
#define ROOM_BELT_X9       228   //Kofferband #9
#define ROOM_BELT_X10      229   //Kofferband #10
#define ROOM_MONITOR1      230   //Fluglisten-Monitor #1 (für Passagiere)
#define ROOM_MONITOR2      231   //Fluglisten-Monitor #2 (für Passagiere)
#define ROOM_MONITOR3      232   //Fluglisten-Monitor #3 (für Passagiere)
#define ROOM_MONITOR4      233   //Fluglisten-Monitor #4 (für Passagiere)
#define ROOM_MONITOR5      234   //Fluglisten-Monitor #5 (für Passagiere)
#define ROOM_MONITOR6      235   //Fluglisten-Monitor #6 (für Passagiere)
#define ROOM_MONITOR7      236   //Fluglisten-Monitor #7 (für Spieler)
#define ROOM_MONITOR8      237   //Fluglisten-Monitor #8 (für Spieler)
#define ROOM_MONITOR9      238   //Fluglisten-Monitor #9 (für Spieler)
#define ROOM_PHONE1        240   //Öffentliches Telefon #1
#define ROOM_PHONE2        241   //Öffentliches Telefon #2
#define ROOM_PHONE3        242   //Öffentliches Telefon #3
#define ROOM_PHONE4        243   //Öffentliches Telefon #4
#define ROOM_ELECTRO       244   //Der Colaautomat
#define ROOM_AIRPORT_EXIT  250   //Flughafenausgang
#define ROOM_WALL          254   //Mauer, Spieler muß davor stehen
#define ROOM_ANTIMARKER    255   //Löscht eine Markierung; für Löcherm z.B. für Briefkasten
#define ROOM_TITLE         900   //
#define ROOM_ENTERING      1024  //Flag: Spieler betritt Raum
#define ROOM_LEAVING       2048  //Flag: Spieler verläßt Raum

//Die Fenstermaße:
#define WIN_LO             1     //Links oben
#define WIN_RO             2     //Rechts oben
#define WIN_LU             4     //Links unten
#define WIN_RU             8     //Rechts unten
#define WIN_O              3     //Oben
#define WIN_U              12    //Unten
#define WIN_BIG            15    //Vollbild

//Art der Verkehrsbehinderung (Obstacles = Hindernisse)
#define OBST_NONE           00   //Kein Hindernis
#define OBST_POINT          01   //Punktförmiges Hindernis
#define OBST_WIDE           02   //Breites Hindernis
#define OBST_SITPOINT       03   //Punktförmiges Hindernis (Sitz)
#define OBST_HORIZONTAL     10   //Horizontale Abgrenzung
#define OBST_VERTIKAL       11   //Vertikale Abgrenzung
#define OBST_BIGVERTIKAL    12   //Große vertikale Abgrenzung
#define OBST_HORIZONTALEXIT 20   //Horizontale Ausgang in der Fensterreihe hinten
#define OBST_SHOPFRONT      21   //Der Eingang zu einem Geschäft
#define OBST_STAIRFRONT     22   //Der Eingang zur Treppe
#define OBST_BUROFRONT_R    23   //Der Eingang zum Büro (Eingang rechts)
#define OBST_NOENTRY1       24   //Kein Eingang (z.B. wegen Kiosk)
#define OBST_NOENTRY2       25   //Kein Eingang (z.B. wegen Arab-Air)
#define OBST_MUSEUM         26   //Der Eingang zum Museum
#define OBST_BUROFRONT_L    27   //Der Eingang zum Büro (Eingang links)
#define OBST_UP             40   //Pfeil nach oben
#define OBST_RIGHT          41   //Pfeil nach rechts
#define OBST_DOWN           42   //Pfeil nach unten
#define OBST_LEFT           43   //Pfeil nach links
#define OBST_RUNE           99   //Kein echtes Hindernis sondern eine Markierungs-Rune

//Die Rune-Ids:
#define RUNE_CREATION        2000  //Creation Node
#define RUNE_PCREATION       2001  //Player Creation Node
#define RUNE_DESTRUCTION     2002  //Der Ausgang
#define RUNE_PCREATION2      2003  //Player Creation Node beim Büro
#define RUNE_PDESTRUCTION    2003  //Der Ausgang für den Spieler
#define RUNE_UP              2010  //Pflicht: nach Norden
#define RUNE_RIGHT           2011  //Pflicht: nach Osten
#define RUNE_SOUTH           2012  //Pflicht: nach Süden
#define RUNE_LEFT            2013  //Pflicht: nach Westen
#define RUNE_BELT_RIGHT      2020  //Laufband nach rechts
#define RUNE_BELT_LEFT       2021  //Laufband nach links
#define RUNE_WESTEAST        2022  //Pflicht: West oder Ost
#define RUNE_NORTHSOUTH      2023  //Pflicht: Norden oder Süden
#define RUNE_NOPLAYER        2025  //Verboten für Spieler
#define RUNE_NOCLIENT        2026  //Verboten für Kunden
#define RUNE_CHECKIN         2030  //An dieser Stelle wird eingecheckt
#define RUNE_2CHECKIN        2031  //Hier beginnt die Warteschlange
#define RUNE_2CHECKIN_EXIT   2044  //Rechtes Ende des Check-Ins
#define RUNE_DURCHLEUCHTER   2045  //Der Durchleuchter
#define RUNE_SHOP            2032  //An dieser Stelle ist der Shop
#define RUNE_2SHOP           2033  //Hier ist der Shop-Eingang
#define RUNE_WAITPLANE       2040  //An dieser Stelle besteigt man das Flugzeug
#define RUNE_DROPSUITCASE    2041  //Hier gibt der Spieler den Koffer ab
#define RUNE_EXIT_SUITCASE   2042  //Hier verschwindet der Koffer im Nirvana
#define RUNE_CREATE_SUITCASE 2043  //Hier wird ein Koffer erzeugt
#define RUNE_WAIT            2034  //An dieser Stelle ist der Knick im Gang zum Flugzeug
#define RUNE_2WAIT           2035  //Hier ist der Eingang zum Warteraum
#define RUNE_UPRIGHT         2036  //Pflicht: nach Norden o. Osten
#define RUNE_RIGHTSOUTH      2037  //Pflicht: nach Osten o. Süden
#define RUNE_SOUTHLEFT       2038  //Pflicht: nach Süden o. Westen
#define RUNE_LEFTUP          2039  //Pflicht: nach Westen o. Norden
#define RUNE_LEFTEND         2050  //linker Scrollbegrenzer
#define RUNE_RIGHTEND        2051  //rechter Scrollbegrenzer
#define RUNE_WAYPOINT_START  2054  //WayPoint-Creation Rune
#define RUNE_WAYPOINT        2055  //WayPoint
#define RUNE_WAYPOINT_G      2056  //WayPoint+Gimmick
#define RUNE_WAYPOINT_WAIT   2057  //WayPoint: Wait für 3 Seconds
#define RUNE_NOLEFT          2060  //kein Weg nach links
#define RUNE_NODOWN          2061  //kein Weg nach süden
#define RUNE_NOWAY           2062  //kein Weg in diese Kachel
#define RUNE_AREALO          2063  //Klick-Area Links/Oben
#define RUNE_AREARU          2064  //Klick-Area Rechts/Unten
#define RUNE_CLIPFULL        2065  //von hier bis nach rechts
#define RUNE_CLIPMIDDLE      2066  //Clipping Areas wirken immer
#define RUNE_CLIPNONE        2067  //zur nächsten Area
#define RUNE_CONDBLOCK       2070  //Conditional Block

#define BRICK_ELECTRO         303  //Colaautomat
#define BRICK_KASTEN          471  //Kofferschlucker
#define BRICK_SCANNER_DARKLY  283  //Durchleuchter für Skelette

//Die Gründe, warum eine Person am Flughafen sein kann:
#define REASON_SHOPPING      50    //Will nur in Geschäfte
#define REASON_LEAVING       51    //Ist per Flugzeug angekommen und will nur weg
#define REASON_FLYING        52    //Will per Flugzeug verreisen
#define REASON_SUITCASE_EXIT 53    //Koffer will nur zum Koffer-Ausgang   
#define REASON_WAYPOINT      54    //Person geht auf einer Reihe von WayPoints

//Die Einheiten
#define EINH_KM            0
#define EINH_KMH           1
#define EINH_M             2
#define EINH_KN            3
#define EINH_L             4
#define EINH_LH            5
#define EINH_DM            6
#define EINH_P             7
#define EINH_J             8
#define EINH_XDM           9
#define EINH_KG            10
#define EINH_T             11
#define EINH_MIODM         12
#define EINH_MIO           13

//Die Tip-Typen:
#define TIP_NONE           0
#define TIP_CITY           10
#define TIP_PLANE          20
#define TIP_BUYPLANE       21
#define TIP_AUFTRAG        30
#define TIP_BUYAUFTRAG     31
#define TIP_BUYAUFTRAGR    32
#define TIP_BUYFRACHT      33
#define TIP_FRACHT         34
#define TIP_ROUTE          40
#define TIP_BUYROUTE       41
#define TIP_AKTIE          50
#define TIP_ITEM           60
#define TIP_MONEY          70
#define TIP_AUTOFLUG       90

//Dinge für den Roboter (Computerspieler):
#define ACTION_NONE             0     //Leereintrag
#define ACTION_WAIT           100     //Warten - kein Leereintrag!
#define ACTION_RAISEMONEY     200
#define ACTION_DROPMONEY      201
#define ACTION_VISITBANK      202
#define ACTION_EMITSHARES     203
#define ACTION_CHECKAGENT1    210     //Last-Minute
#define ACTION_CHECKAGENT2    211     //Reisebüro
#define ACTION_CHECKAGENT3    212     //Frachtraum
#define ACTION_MEETING        220     //Geht zur morgentlichen Besprechung
#define ACTION_STARTDAY       221     //Geht ins Büro
#define ACTION_BUERO          221     //Geht ins Büro
#define ACTION_PERSONAL       222     //Geht ins Personalbüro
#define ACTION_VISITARAB      230
#define ACTION_VISITKIOSK     231
#define ACTION_VISITMECH      232
#define ACTION_VISITMUSEUM    233
#define ACTION_VISITDUTYFREE  234
#define ACTION_VISITAUFSICHT  235
#define ACTION_VISITNASA      236
#define ACTION_VISITTELESCOPE 237
#define ACTION_VISITMAKLER    238
#define ACTION_VISITRICK      239
#define ACTION_VISITROUTEBOX  240
#define ACTION_VISITSECURITY  241
#define ACTION_VISITDESIGNER  242
#define ACTION_VISITSECURITY2 243

#define ACTION_SABOTAGE       500
#define ACTION_BUYUSEDPLANE   300
#define ACTION_BUYNEWPLANE    301
#define ACTION_WERBUNG        400

//Die Arten von Beratern:
#define BERATERTYP_GIRL        0    //Die Sekretärin
#define BERATERTYP_PERSONAL    1
#define BERATERTYP_KEROSIN     2
#define BERATERTYP_OEL         2
#define BERATERTYP_ROUTE       3
#define BERATERTYP_AUFTRAG     4
#define BERATERTYP_GELD        5
#define BERATERTYP_INFO        6
#define BERATERTYP_FLUGZEUG    7
#define BERATERTYP_FITNESS     8
#define BERATERTYP_SICHERHEIT  9
#define WORKER_PILOT          10
#define WORKER_STEWARDESS     11

//Wo befindet sich ein Worker?
#define WORKER_RESERVE      100     //Ist in der stillen Reserve
#define WORKER_JOBLESS      101     //Sucht einen Job

//Die Leute die sprechen können:
#define TALKER_NONE          -1
#define TALKER_PERSONAL1a     0     //Die Leute in den Personalräumen
#define TALKER_PERSONAL1b     1
#define TALKER_PERSONAL2a     2
#define TALKER_PERSONAL2b     3
#define TALKER_PERSONAL3a     4
#define TALKER_PERSONAL3b     5
#define TALKER_PERSONAL4a     6
#define TALKER_PERSONAL4b     7
#define TALKER_BANKER1        8
#define TALKER_BANKER2        9
#define TALKER_MUSEUM        10
#define TALKER_ARAB          11
#define TALKER_BOSS          12
#define TALKER_MAKLER        13
#define TALKER_MECHANIKER    14
#define TALKER_WERBUNG       15
#define TALKER_DUTYFREE      16
#define TALKER_SABOTAGE      17
#define TALKER_NASA          18
#define TALKER_WORLD         19
#define TALKER_COMPETITOR    20
#define TALKER_KIOSK         21
#define TALKER_RICK          22
#define TALKER_TRINKER       23
#define TALKER_FRACHT        24
#define TALKER_SECURITY      25
#define TALKER_DESIGNER      26
#define TALKER_ANZ           27

#define TALKER_PASSENGER    998
#define TALKER_PASSAGIER    998
#define TALKER_PLAYER       999

//Die Arten auf die man sprechen kann:
#define MEDIUM_AIR            0     //Man ist im Raum anwesend
#define MEDIUM_HANDY          1     //Hier gehen Menüs nicht

//Die On-screen Menüs:
#define MENU_NONE             0
#define MENU_BANK             1     //Der Kontostand + Kredit
#define MENU_FILOFAX          2
#define MENU_PERSONAL         3
#define MENU_SELLPLANE        4     //Verkauf von Flugzeugen im Museum
#define MENU_REQUEST          5     //Irgenteine Rückfrage
#define MENU_BUYPLANE         6     //Kauf beim Makler
#define MENU_LETTERS          7     //Briefe
#define MENU_ADROUTE          9     //Welche Route bewerben?
#define MENU_SETRENDITE      10     //Rendite einstellen
#define MENU_AKTIE           11     //Aktien kaufen/verkaufen
#define MENU_EMMISSION       12     //Neue Aktien ausgeben
#define MENU_FLUEGE          13     //Liste mit allen Flügen
#define MENU_WC_F            14     //Pseudo-Menü, um die Tastatureingaben zu blockieren
#define MENU_WC_M            15     //Pseudo-Menü, um die Tastatureingaben zu blockieren
#define MENU_RENAMEPLANE     16     //Ein Flugzeug umbennen
#define MENU_GAMEOVER        17     //Spiel ist vorbei; Missionsauswertung
#define MENU_BRANCHLIST      18     //Telefon: Die Niederlassungen zur Auswahl
#define MENU_PLAYERLIST      19     //Telefon: Die Spieler zur Auswahl
#define MENU_AUSLANDSAUFTRAG 20     //Telefon: Die Aufträge aus dem Ausland
#define MENU_PLANECOSTS      21     //Die Reparaturkosten für die Flugzeuge
#define MENU_PLANEREPAIRS    22     //Die Reparaturbefehle für die Flugzeuge
#define MENU_PLANEJOB        23     //Auf welchem Flugzeug arbeitet jemand?
#define MENU_KEROSIN         25     //Wird das Kerosin freigegeben?
#define MENU_BUYKEROSIN      26     //Kerosin beim Araber kaufen
#define MENU_FLIGHTLIST      27     //Liste der vergangenen Flüge
#define MENU_KONTOAUSZUG     28     //Der Kontoauszug
#define MENU_SABOTAGEPLANE   29     //Welches Flugzeug will man sabotieren
#define MENU_SABOTAGEFAX     30     //Ein Fax, daß es Sabotage gab
#define MENU_PANNEN          31     //Die Liste der Pannen, die die Flugzeuge hatten
#define MENU_CLOSED          32     //Ein Menü mit den Öffnungszeiten
#define MENU_EXTRABLATT      33     //Einer der Spieler ist rausgeflogen
#define MENU_QUITMESSAGE     34     //Wird beim beenden der Demoversion angezeigt
#define MENU_CHAT            35     //Der Screen zum chatten
#define MENU_BRIEFING        36     //Zusammenfassung vom Briefing
#define MENU_BROADCAST       37     //Der Screen zum broadcasten
#define MENU_CALLITADAY      38     //Ein Spieler will Feierabend machen
#define MENU_ENTERTCPIP      39     //Host für's Internet eingeben
#define MENU_ENTERPROTECT    40     //Den Code für den Kopierschutz eingeben
#define MENU_SECURITY        41     //Die Security-Features
#define MENU_RENAMEEDITPLANE 42     //Ein Flugzeug im Editor umbennen
#define MENU_SABOTAGEROUTE   43     //Welche Route will man übernehmen
#define MENU_BUYXPLANE       44     //Kauf des Flugzeuges beim Designer

//Die der der MENU_REQUEST Menüs:
#define MENU_REQUEST_CALLITADAY     1  //Feierabend! Sicher?
#define MENU_REQUEST_THROWNOUT      2  //In dem Raum ist schon wer
#define MENU_REQUEST_KILLCITY       3  //Stadt kündigen
#define MENU_REQUEST_KILLROUTE      4  //Route kündigen
#define MENU_REQUEST_WRONGROOM      5  //Falscher Raum. Wollen Sie zu ...?
#define MENU_REQUEST_KILLITEM       6  //Ein Item wegwerfen
#define MENU_REQUEST_KILLAUFTRAG    7  //Einen Auftrag zurückgeben und die Strafe bezahlen
#define MENU_REQUEST_KILLAUFTRAG0   8  //Einen Auftrag zurückgeben
#define MENU_REQUEST_KILLPLAN       9  //Flugplan löschen?
#define MENU_REQUEST_WRONGROOM2    10  //Wenn der entsprechende Spieler bei einem leeren Büro rausgeflogen ist
#define MENU_REQUEST_THROWNOUT2    11  //Kein Raum, sondern ein offenes Büro
#define MENU_REQUEST_RENTROUTE     12  //Wollen Sie die Route mieten
#define MENU_REQUEST_NORENTROUTE1  13  //Route kann nicht gemietet werden: 3 Gegner
#define MENU_REQUEST_NORENTROUTE2  14  //Route kann nicht gemietet werden: Keine angrenzende Route
#define MENU_REQUEST_NORENTROUTE3  15  //Route kann nicht gemietet werden: Wird noch verwendet
#define MENU_REQUEST_NORENTROUTE4  16  //Route kann nicht gemietet werden: Gerade erst verloren
#define MENU_REQUEST_FORBIDDEN     17  //Kein Einlaß, weil man vorher zu lange da war
#define MENU_REQUEST_DESTROYED     18  //Kein Einlaß, weil man vorher zu lange da war
#define MENU_REQUEST_ITEMS         19  //Nicht genug Platz für Items
#define MENU_REQUEST_NO_LM         20  //Text: Demoversion
#define MENU_REQUEST_NO_WERKSTATT  21  //Text: Demoversion
#define MENU_REQUEST_NO_MAKLER     22  //Text: Demoversion
#define MENU_REQUEST_NO_SABOTAGE   23  //Text: Demoversion
#define MENU_REQUEST_NO_WORLD      24  //Text: Demoversion
#define MENU_REQUEST_NO_PLAYER     25  //Text: Demoversion
#define MENU_REQUEST_NO_WERBUNG    26  //Text: Demoversion
#define MENU_REQUEST_KILLFAUFTRAG  27  //Einen Auftrag zurückgeben und die Strafe bezahlen
#define MENU_REQUEST_KILLFAUFTRAG0 28  //Einen Auftrag zurückgeben
#define MENU_REQUEST_NOGLUE        30  //Item: Nicht hier verwenden
#define MENU_REQUEST_NOSTENCH      31  //Item: Nicht hier verwenden
#define MENU_REQUEST_NET_NUM       32  //Zahl der Spieler stimmt nicht
#define MENU_REQUEST_NET_LOADONE   33  //Ein Spieler konnte nicht laden
#define MENU_REQUEST_NET_LOADTHIS  34  //Dieser Spieler konnte nicht laden
#define MENU_REQUEST_NET_VERSION   35  //Versionskonflikt zwischen zwei AT's mit Netzwerk
#define MENU_REQUEST_BETATEST      36  //Betatest: Nicht lizensiert
#define MENU_REQUEST_BETATEST2     37  //Betatest: Veraltet
#define MENU_REQUEST_KILLPLANE     38  //Flugzeug im Editor komplett löschen

//Die Öffnungszeiten:
#define MENU_CLOSED_DUTYFREE       0
#define MENU_CLOSED_ARAB_AIR       1
#define MENU_CLOSED_LAST_MINUTE    2
#define MENU_CLOSED_MUSEUM         3
#define MENU_CLOSED_REISEBUERO     4
#define MENU_CLOSED_MAKLER         5
#define MENU_CLOSED_WERBUNG        6
#define MENU_CLOSED_PROTECTION     7
#define MENU_CLOSED_NOSECURITY     8  //Wegen Stromausfall kein Security

#define MESSAGE_NORMAL        0     //Kommt in die Queue
#define MESSAGE_URGENT        1     //An den Anfang der Queue
#define MESSAGE_COMMENT       2     //Ersetzt bei gleichem Berater

//So kann die Maus aussehen:
#define CURSOR_NORMAL         0     //Normal
#define CURSOR_HOT            1     //Click-Fähig
#define CURSOR_EXIT           3     //Raum verlassen
#define CURSOR_LEFT           4     //Man kann blättern
#define CURSOR_RIGHT          5     //Man kann blättern
#define CURSOR_MOVE_H         6     //Man kann verschieben
#define CURSOR_MOVE_V         7     //Man kann verschieben

//Modi für die Animationsroutine
#define ANIMATION_MODE_ONCE    0
#define ANIMATION_MODE_REPEAT  1
#define ANIMATION_MODE_RANDOM  2
#define ANIMATION_MODE_NEVER   3

//Die verschiedenen Ambientes:
#define AMBIENT_PEOPLE         0    //Das Gemurmel der Leute am Flughafen
#define AMBIENT_JET_OUTSIDE    1    //Ein Jet fährt draußen vorbei
#define AMBIENT_JET_FIELD      2    //Man ist draußen auf dem Airfield

//Die Flags für die Raktenbauteile:
#define ROCKET_BASE            1    //Die Standbasis
#define ROCKET_TOWER           2    //Das Haltegerüst
#define ROCKET_ARM             4    //Der Arm am Haltegerüst
#define ROCKET_PART_ONE        7    //Der erste Teil

#define ROCKET_AIRFRAME        8    //Der Rahmen
#define ROCKET_WINGS          16    //Flügel
#define ROCKET_CAPSULE        32    //Die Passagierkapsel
#define ROCKET_PROP           64    //Seitliche Antriebe
#define ROCKET_MAINPROP      128    //Hauptantrieb
#define ROCKET_HECK          256    //Das Heck
#define ROCKET_COCKPIT       512    //Die Steuerkanzel
#define ROCKET_PART_TWO_A    127    //Der zweite Teil halb (für die Zeitungsmeldung)
#define ROCKET_ALL          1023    //Die gesammte Rakete

//Die Flags für die Stationsbauteile:
#define STATION_RINGA           1    //
#define STATION_RINGB           2    //
#define STATION_RINGC           4    //
#define STATION_PART_ONE        7    //Der erste Teil

#define STATION_OXYGEN          8    //
#define STATION_COM            16    //
#define STATION_MAIN           32    //
#define STATION_LIVING         64    //
#define STATION_OFFICE        128    //
#define STATION_PART_TWO      255    //Der zweite Teil halb (für die Zeitungsmeldung)

#define STATION_POWER         256    //
#define STATION_SOLAR         512    //
#define STATION_ALL          1023    //Die gesammte Rakete

//Die Statistik-Daten:
#define STAT_ANZ               40    //Die Anzahl der überwachten Eigenschaften
#define STAT_KONTO              0
#define STAT_KREDIT             1
#define STAT_FLUGZEUGE          2
#define STAT_PASSAGIERE         3    //Die Summe der beförderten Passagiere
#define STAT_PASSAGIERE_HOME    4      //davon die, die über den Heimatflughafen geflogen sind
#define STAT_AKTIENKURS         5
#define STAT_FLUEGE             6    //Flüge (egal welcher Art), die durchgeführt wurden
#define STAT_AUFTRAEGE          7    //Gesammtanzahl der angenommen (nicht geflogenen!) Aufträge
#define STAT_LMAUFTRAEGE        8    //und soviele waren davon LastMinute
#define STAT_STRAFE             9
#define STAT_FIRMENWERT        10
#define STAT_ZUFR_PASSAGIERE   11
#define STAT_UNZUFR_PASSAGIERE 12
#define STAT_ZUFR_PERSONAL     13
#define STAT_VERSPAETUNG       14
#define STAT_UNFAELLE          15    //Soviele Unfälle gab's beim Spieler
#define STAT_SABOTIERT         16    //Soviele Unfälle hat der Spieler in Auftrag gegeben
#define STAT_MITARBEITER       17
#define STAT_MISSIONSZIEL      18    //Soweit vom Missionsziel (in%) ist erreicht; Vorsicht! Kann größer als 100 sein!!!
#define STAT_WARTUNG           19
#define STAT_NIEDERLASSUNGEN   20
#define STAT_ROUTEN            21
#define STAT_GEHALT            22
#define STAT_BEKANNTHEIT       23  //-1000=sehr unbeliebt, 0=unbekannt, 1000=bekannt & beliebt

#define STAT_AKTIEN_ANZAHL		 25		// Ausgegebene Aktien
#define STAT_AKTIEN_SA			 26		// Aktien an SA
#define STAT_AKTIEN_FL			 27		// Aktien an FL
#define STAT_AKTIEN_PT			 28		// Aktien an PT
#define STAT_AKTIEN_HA			 29		// Aktien an HA

#define STAT_E_ROUTEN			 30		// -- Die Einnahmen --
#define STAT_E_AUFTRAEGE		 31
#define STAT_E_VERKAEUFE		 32		// Flugzeuge, Aktien, Ausrüstung u.s.w.
#define STAT_E_SONSTIGES		 33		// Neue Aktien, Dividenden, Zinsen

#define STAT_A_MIETEN			 34		// -- Die Ausgaben --
#define STAT_A_GEHAELTER		 STAT_GEHALT
#define STAT_A_KEROSIN			 35
#define STAT_A_WARTUNG			 STAT_WARTUNG
#define STAT_A_STRAFEN			 36
#define STAT_A_KAEUFE			 37		// Flugzeuge, Aktien, Ausrüstung u.s.w.
#define STAT_A_SONSTIGES		 38		// Essen, Zinsen, Dividenden, Telefon ** +STRAFEN **

#define STAT_FRACHTEN          39    //Gesammtanzahl der angenommen (nicht geflogenen!) Frachtaufträge

//Eröffnungen für die Dialoge:
#define DIALOG_WERBUNG          1
#define DIALOG_MUSEUM           2
#define DIALOG_MUSEUM2          4
#define DIALOG_NASA             8
#define DIALOG_MAKLER          16
#define DIALOG_RICK            32
#define DIALOG_RICK_TODAY      64

//Die Smileys für den Advisor:
#define SMILEY_GREAT            0
#define SMILEY_GOOD             1
#define SMILEY_NEUTRAL          2
#define SMILEY_BAD              3

//Die Hinweise für den Spieler
#define HINT_GAMESPEED          0
#define HINT_FEIERABEND         1
#define HINT_BUERO              2
#define HINT_PERSONALBUERO      3
#define HINT_ROUTEBOX           4
#define HINT_DUTYFREE           5
#define HINT_AIRTRAVEL          6
#define HINT_LASTMINUTE         7
#define HINT_MUSEUM             8
#define HINT_KIOSK              9
#define HINT_WERKSTATT         10
#define HINT_FERNGLASS         11
#define HINT_GLOBE             12

#define HINT_RICKS             13
#define HINT_WERBUNG           14
#define HINT_AUFSICHT          15
#define HINT_NASA              16
#define HINT_FLUGZEUGMAKLER    17
#define HINT_BANK              18
#define HINT_ARABAIR           19

#define HINT_RICK              20
#define HINT_FRACHT            21

//Die Fahigkeiten des Computerspielers: Hiermit kann man abfragen, ob sie aktiviert sind:
#define ROBOT_USE_SABOTAGE           1  //Sabotage betreiben
#define ROBOT_USE_FRACHT             2  //Frachtaufträge annehmen
#define ROBOT_USE_WERBUNG            3  //Werbung machen
#define ROBOT_USE_AUFSICHT           4  //Die Korktafel in der Aufsicht regelmäßig nutzen (Niederlasstungen erwerben)
#define ROBOT_USE_NASA               5  //Bei der Nasa vorbeischauen
#define ROBOT_USE_NEVERGIVEROUTEUP   6  //Routen auch bei Verlust niemals aufgeben
#define ROBOT_USE_ROUTES             7  //Routen verwenden
#define ROBOT_USE_SUGGESTROUTES      8  //Routen bevorzugt verwenden (Routenmission)
#define ROBOT_USE_FORCEROUTES        9  //Routen immer verwenden (Ohne-Aufträge Mission)
#define ROBOT_USE_HARDWERBUNG       10  //Viel Geld für Werbung ausgeben?
#define ROBOT_USE_SELLSHARES        11  //Computerspieler 2 kann schon frühzeitig Aktien verkaufen
#define ROBOT_USE_BUYOWNSHARES      12  //Computerspieler kann eigene Aktien zurückkaufen
#define ROBOT_USE_REBUYSHARES       13  //Teil der eigenen Aktien nach Emission zurückkaufen
#define ROBOT_USE_TUTORIALLIMIT     14  //Zahl der Aufträge anhand des Tutoriallimits begrenzen
#define ROBOT_USE_ROUTEMISSION      15  //Levelziel: bestimmte Routen
#define ROBOT_USE_MUCHWERBUNG       16  //Der Werbung einen hohen Stellenwert einräumen? (z.B. für Image-Mission)
#define ROBOT_USE_BONUS_X2          17  //Doppelter Bonus für Computergegner in diesem Level?
#define ROBOT_USE_BONUS_X4          18  //Vierfacher Bonus für Computergegner in diesem Level?
#define ROBOT_USE_BONUS_X8          19  //Achtfacher Bonus für Computergegner in diesem Level?
#define ROBOT_USE_ROUTEBOX          20  //Ist da ein Routenkasten an der Wand
#define ROBOT_USE_ABROAD            21  //Auch mal im Ausland anrufen
#define ROBOT_USE_MUCH_SABOTAGE     22  //Etwas mehr Sabotage betreiben
#define ROBOT_USE_PAYBACK_CREDIT    23  //Spielziel: Kredit zurückzahlen
#define ROBOT_USE_MUCH_FRACHT       24  //Frachtaufträge sind wichtig
#define ROBOT_USE_FREE_FRACHT       25  //Frachtaufträge ohne Gewinn sind sehr attraktiv
#define ROBOT_USE_RUN_FRACHT        26  //Spieler rennt morgens zu Fracht-Aufträgen
#define ROBOT_USE_LUXERY            27  //Robots setzen auf Luxus
#define ROBOT_USE_TRAVELHOLDING     28  //Gibt es die Holdinggesellschaft für die Reisebüros
#define ROBOT_USE_IMAGEBONUS        29  //Robots erhälten täglich einen Imagebonus
#define ROBOT_USE_GOODPLANES        30  //Robots reparieren immer alles und sofort
#define ROBOT_USE_REPAIRPLANES      31  //Flugzeug ist kaputt und muß vorsichtig repariert werden
#define ROBOT_USE_HIGHSHAREPRICE    32  //Robots verwenden hohen Aktienzielkurs
#define ROBOT_USE_WORKQUICK         33  //Robots sind in den Räumen sehr schnell fertig
#define ROBOT_USE_GROSSESKONTO      34  //Robots sparen viel auf dem Konto an
#define ROBOT_USE_EMITMUCHSHARES    35  //Robots geben Aktien aus wann immer sie können
#define ROBOT_USE_WORKVERYQUICK     36  //Robots sind in den Räumen sehr sehr schnell fertig
#define ROBOT_USE_ALLRUN            37  //Alle Robots rennen immer
#define ROBOT_USE_DONTBUYANYSHARES  38  //Robots kaufen nie Aktien
#define ROBOT_USE_MAXKREDIT         39  //Immer maximal Kredit aufnehmen
#define ROBOT_USE_MAX4PLANES        40  //Nicht mehr als 4 Flugzeuge kaufen
#define ROBOT_USE_MAX5PLANES        41  //Nicht mehr als 5 Flugzeuge kaufen
#define ROBOT_USE_MAX10PLANES       42  //Nicht mehr als 10 Flugzeuge kaufen
#define ROBOT_USE_NOCHITCHAT        43  //Keine Plauderei am Kiosk, bei Rick und so
#define ROBOT_USE_OFTENMECH         44  //Mechanikerbesuch statt Plauderei am Kiosk, bei Rick und so
#define ROBOT_USE_SHORTFLIGHTS      45  //Kurze Flüge bevorzugen
#define ROBOT_USE_EXTREME_SABOTAGE  46  //Sehr viel mehr Sabotage betreiben
#define ROBOT_USE_SECURTY_OFFICE    47  //Das neue SecurityOffice nutzen
#define ROBOT_USE_MAKLER            48  //In diesem Level den Flugzeugmakler nutzen
#define ROBOT_USE_PETROLAIR         49  //Die Kerosin-Abteilung nutzen?
#define ROBOT_USE_MAX20PERCENT      50  //Darf maximal 20% der eigenen Aktien besitzen
#define ROBOT_USE_TANKS             51  //Spieler verwendet die Tanks um günstig Kerosin zu horten
#define ROBOT_USE_DESIGNER          52  //Spieler verwendet Flugzeugeditor
#define ROBOT_USE_DESIGNER_BUY      53  //Spieler verwendet Flugzeugeditor und kauft nur dort Flugzeuge


//Die Netzwerk-Medien
#define NET_MEDIUM_UNKNOWN           0
#define NET_MEDIUM_SERIAL            1
#define NET_MEDIUM_MODEM             2
#define NET_MEDIUM_TCPIP             3
#define NET_MEDIUM_IPX               4
#define NET_MEDIUM_SIMTCPIP          5

#ifndef WIN32

#define MAX_PATH                    256

#define WM_MOUSEMOVE                0x0200
#define WM_LBUTTONDOWN              0x0201
#define WM_LBUTTONUP                0x0202
#define WM_LBUTTONDBLCLK            0x0203
#define WM_RBUTTONDOWN              0x0204
#define WM_RBUTTONUP                0x0205
#define WM_RBUTTONDBLCLK            0x0206
#define WM_MBUTTONDOWN              0x0207
#define WM_MBUTTONUP                0x0208
#define WM_MBUTTONDBLCLK            0x0209

#define VK_CONTROL                  KMOD_CTRL
#define VK_SHIFT                    KMOD_SHIFT
#define VK_ALT                      KMOD_ALT
#define VK_MENU                     KMOD_GUI
#define VK_RETURN                   SDLK_RETURN
#define VK_ESCAPE                   SDLK_ESCAPE
#define VK_BACK                     SDLK_BACKSPACE
#define VK_TAB                      SDLK_TAB
#define VK_PAUSE                    SDLK_PAUSE
#define VK_SCROLL                   SDLK_SCROLLLOCK
#define VK_SPACE                    SDLK_SPACE
#define VK_UP                       SDLK_UP
#define VK_DOWN                     SDLK_DOWN
#define VK_LEFT                     SDLK_LEFT
#define VK_RIGHT                    SDLK_RIGHT
#define VK_PRIOR                    SDLK_PAGEUP
#define VK_NEXT                     SDLK_PAGEDOWN
#define VK_INSERT                   SDLK_INSERT
#define VK_DELETE                   SDLK_DELETE
#define VK_F1                       SDLK_F1
#define VK_F2                       SDLK_F2
#define VK_F3                       SDLK_F3
#define VK_F4                       SDLK_F4
#define VK_F5                       SDLK_F5
#define VK_F6                       SDLK_F6
#define VK_F7                       SDLK_F7
#define VK_F8                       SDLK_F8
#define VK_F9                       SDLK_F9
#define VK_F10                      SDLK_F10
#define VK_F11                      SDLK_F11
#define VK_F12                      SDLK_F12

#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define GetRValue(rgb) ((rgb)&0xFF)
#define GetGValue(rgb) ((((WORD)(rgb)) >> 8)&0xFF)
#define GetBValue(rgb) (((rgb)>>16)&0xFF)
#define ZeroMemory(ptr,size) memset(ptr,0,size)
#define stricmp strcasecmp
#define strnicmp strncasecmp

#define FALSE 0
#define TRUE  1

#define HTNOWHERE 0
#define HTCLIENT  1

#define __forceinline inline

#endif
