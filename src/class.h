//============================================================================================
// Class.h - Prototypen von Klassen, die nicht vom ClassWizard stammen:
//============================================================================================
// CAuftrag CFlugplan CPlane PLAYER
//============================================================================================
#include "HLine.h"

typedef BUFFER<UBYTE> BUFFER_UBYTE;

class  CPlane;
class  CPlaner;
class  CStdRaum;

extern SLONG                   MouseWait;
//SLONG  ReadTimeStampCounter (void);

class /**/CSmoke
{
   public:
      XY    Position;
      SLONG vx;
      SLONG TTL;                       //TimeToLive;

   public:
      CSmoke () { TTL = 0; }
};

//--------------------------------------------------------------------------------------------
//Variablen, die immer im Speicher herumspringen und daher nicht gefaßt werden können:
//--------------------------------------------------------------------------------------------
template <class T> class /**/CJumpingVar
{
   private:
      T    *t;

   public:
      CJumpingVar () { t=new T; }
      CJumpingVar (const T &_t) { t=new T; *t=_t; }
      ~CJumpingVar () { delete t; }

      operator T () const { return (*t); }
      T &operator = (const T &_t) { *t=_t; return *t; }

      T &operator ++() { return ++(*t); }
      T  operator ++(int) { return (*t)++; }
      T &operator --() { return --(*t); }
      T  operator --(int) { return (*t)--; }

      void Pump (void)
      {
         T *tmp=new T;
         char *help=new char [sizeof (T)];

         memcpy (help, tmp, sizeof (T));
         memcpy (tmp, t, sizeof (T));
         memcpy (t, help, sizeof (T));

         delete [] help;
         delete t;

         t=tmp;
      }
};

//--------------------------------------------------------------------------------------------
//Hack-Sichere Variablen:
//--------------------------------------------------------------------------------------------
template <class T, T code1, T code2> class /**/CCodedVar
{
   private:
      T     var;

   public:
      CCodedVar () {}
      CCodedVar (T _var) { var = (_var+code1)^code2; }

      operator T () const { return ((var^code2)-code1); }

      T operator ++() { var = (((var^code2)-code1)+1+code1)^code2; return T(var); }
      T operator --() { var = (((var^code2)-code1)-1+code1)^code2; return T(var); }
      T operator ++(int) { var = (((var^code2)-code1)+1+code1)^code2; return T(var)-1; }
      T operator --(int) { var = (((var^code2)-code1)-1+code1)^code2; return T(var)+1; }
      T operator +=(T Number) { var = (((var^code2)-code1)+Number+code1)^code2; return T(var); }
      T operator -=(T Number) { var = (((var^code2)-code1)-Number+code1)^code2; return T(var); }
      T operator *=(T Number) { var = (((var^code2)-code1)*Number+code1)^code2; return T(var); }
      T operator /=(T Number) { var = (((var^code2)-code1)/Number+code1)^code2; return T(var); }
};

//--------------------------------------------------------------------------------------------
//Ein Soundeffekt:
//--------------------------------------------------------------------------------------------
class /**/SBFX
{
   public:
      FX     *pFX;
      CString Filename;

	public:
      SBFX ();
      ~SBFX ();
      void Destroy (void);
      void ReInit (const CString &Filename, char *Path=NULL);
		void Play(dword dwFlags = 0);
		void Play(dword dwFlags, long PercentVolume);
		void Stop(void);
		void SetVolume (long volume);
      void Fusion (const SBFX **Fx, long NumFx);
      void Fusion (const SBFX *Fx, const SLONG *Von, const SLONG *Bis, long NumFx);
      void Tokenize (BUFFER<SLONG> &Von, BUFFER<SLONG> &Bis);
      void Tokenize (BUFFER<SBFX> &Effects);
};

//--------------------------------------------------------------------------------------------
//Siehe Sound.cpp
//--------------------------------------------------------------------------------------------
class /**/CVoiceScheduler
{
   private:
      BUFFER<CString> Voices;
      SBFX            CurrentVoice;

   public:
      void  AddVoice (const CString &str);
      void  Clear (void);
      SLONG AnzEntries(void);
      BOOL  IsVoicePlaying (void);
      void  Pump (void);
};

//--------------------------------------------------------------------------------------------
//Siehe Sound.cpp
//--------------------------------------------------------------------------------------------
class /**/CUnrepeatedRandom
{
   private:
      SLONG    Last[3];
      TEAKRAND Random;

      public:
         CUnrepeatedRandom () { Last[0]=-1; Last[1]=-1; Last[2]=-1; }
         CUnrepeatedRandom (ULONG Seed) { Last[0]=-1; Last[1]=-1; Last[2]=-1; Random.SRand(Seed); }
         void SRand (ULONG Seed) { Random.SRand(Seed); }
         void SRandTime (void) { Random.SRandTime(); }
         void Reset () { Last[0]=-1; Last[1]=-1; Last[2]=-1; Random.Reset(); }

      public:
         SLONG Rand (SLONG Max) { return (Rand (0,Max)); }
         SLONG Rand (SLONG Min, SLONG Max);
};

//--------------------------------------------------------------------------------------------
//Eine Variable im Zeitverlauf für die Statistik: (siehe Sim.cpp)
//--------------------------------------------------------------------------------------------
class /**/SValue
{
   private:
      FBUFFER<__int64> Days;
      FBUFFER<__int64> Months;

   public:
      SValue ();
      void Init (void);

      //Funktionen zum Erzeugen der Daten:
      void  SetAtPastDay (SLONG Day, __int64 Value) { Days[Day]=Value; }
      void  AddAtPastDay (SLONG Day, __int64 Value)
      {
         if (Value>0 && Days[Day]+Value<Days[Day])
            Days[Day]=0x7fffffffffffffff;
         else if (Value<0 && Days[Day]+Value>Days[Day])
            Days[Day]=-0x7fffffffffffffff;
         else
            Days[Day]+=Value;
      }

      void  NewDay (void);
      void  NewMonth (BOOL MonthIsSumOfDays);

      //Funktionen zum Abfragen der Daten:
      __int64 GetAtPastDay (SLONG Day)           //0=Heute, 1=Gestern, ... 29=letzer Eintrag
      { return Days[Day]; }
      __int64 GetAtPastMonth (SLONG Month)       //0=Heute, 0=Vor einem Monat, ... 11=letzer Eintrag
      { return Months[Month]; }

      __int64 GetMin (void);
      __int64 GetAvg (void);
      __int64 GetMax (void);
      __int64 GetSum (void);

   friend TEAKFILE &operator << (TEAKFILE &File, const SValue &Value)  { File << Value.Days << Value.Months; return (File); }
   friend TEAKFILE &operator >> (TEAKFILE &File, SValue &Value)  { File >> Value.Days >> Value.Months; return (File); }

   friend class CTakeOffApp;
};

//--------------------------------------------------------------------------------------------
//Ein CWait-Objekt für den Waitcursor der Maus:
//--------------------------------------------------------------------------------------------
class /**/CWait
{
   public:
      CWait () { MouseWait++; }
      ~CWait () { MouseWait--; }
};

//--------------------------------------------------------------------------------------------
//Eine Hintergrundanimation in einem Raum: (siehe Werkstatt.cpp)
//--------------------------------------------------------------------------------------------
class /**/CAnimation
{
   public:
      SBBMS    Frames;
      BOOL     StartPic;
      SLONG    Mode;
      SLONG    Prelude;
      SLONG    CounterStart;
      SLONG    Speed;
      SLONG    CyclycWait;
      SLONG    RepeatCore;
      SBFX    *pSoundFx;
      BOOL     JustPlayed;

   public:
      CAnimation();
      ~CAnimation();
      void  Reset (void);
      void  Remove (void);
      void  ReSize (GfxLib *gfxLib, const CString &graphicIDs, SLONG Anzahl, SBFX *Fx, BOOL StartPic, SLONG Mode, SLONG Prelude, SLONG Speed, SLONG CyclycWait=0, SLONG RepeatCore=1);
      void  StartNow (void);
      void  BlitAt (SBBM &RoomBm, SLONG x, SLONG y);
      void  BlitAtT (SBBM &RoomBm, SLONG x, SLONG y);
      SLONG GetFrame (void);
};

//--------------------------------------------------------------------------------------------
//Eine Markierung für Clipping des Horizontes
//--------------------------------------------------------------------------------------------
class /**/CClipMarker
{
   public:
      SLONG Type;          //0-3=links oben, links, rechts oben, rechts unten
      SLONG Position;      //X-Position

   friend TEAKFILE &operator << (TEAKFILE &File, const CClipMarker &Marker);
   friend TEAKFILE &operator >> (TEAKFILE &File, CClipMarker &Marker);
};

//--------------------------------------------------------------------------------------------
//Eine Markierung für den Cursor
//--------------------------------------------------------------------------------------------
class /**/CAreaMarker
{
   public:
      UBYTE Par;         //Brick-Parameter
      XY    p1, p2;      //X-Position

   friend TEAKFILE &operator << (TEAKFILE &File, const CAreaMarker &Marker);
   friend TEAKFILE &operator >> (TEAKFILE &File, CAreaMarker &Marker);
};

//--------------------------------------------------------------------------------------------
//Ein Hintergrundsound: (in sound.cpp)
//--------------------------------------------------------------------------------------------
class /**/CAmbientFx
{
   private:
      SBFX                  Soundeffekt;
      SLONG                 Volume;          //Volume in % (0-100)
      SLONG                 CurrentVolume;   //=Volume*GlobalVolume

   friend class CAmbienteManager;
};

//--------------------------------------------------------------------------------------------
//Die Verwaltung der Hintergrundsound:
//--------------------------------------------------------------------------------------------
class /**/CAmbienteManager
{
   public:
      SLONG                 GlobalVolume;      //Volume in % (0-100)
      BUFFER<CAmbientFx>    AmbientFx;

   public:
      void RecalcVolumes (void);
      void SetGlobalVolume (SLONG Volume);
      void SetFx (SLONG FxId, CString Soundeffekt);
      void SetVolume (SLONG FxId, SLONG Volume);
      void Pause (void);
      void Resume (void);
};

//--------------------------------------------------------------------------------------------
//Die Korrespondenz des Spielers: (in "Buero.cpp")
//--------------------------------------------------------------------------------------------
class /**/CLetter
{
   public:
      SLONG    Date;
      BOOL     IsLetter;               //TRUE=Letter, FALSE=Fax
      CString  Subject;
      CString  Letter;
      CString  Absender;
      SLONG    PictureId;

   friend TEAKFILE &operator << (TEAKFILE &File, const CLetter &l);
   friend TEAKFILE &operator >> (TEAKFILE &File, CLetter &l);
};

class /**/CLetters
{
   public:
      FBUFFER<CLetter> Letters;

   public:
      CLetters () : Letters (25) {}
      void  Clear (void);
      void  AddLetter (BOOL IsLetter, const CString &Subject, const CString &Letter, const CString &Absender, SLONG Picture);
      void  SortLetters (void);
      SLONG AnzLetters (void);

   friend TEAKFILE &operator << (TEAKFILE &File, const CLetters &l) { File << l.Letters; return (File); }
   friend TEAKFILE &operator >> (TEAKFILE &File, CLetters &l) { File >> l.Letters; return (File); }
};

//--------------------------------------------------------------------------------------------
//Eine Tür im Flughafen:
//--------------------------------------------------------------------------------------------
class /**/CDoor
{
   public:
      XY             ArrayPos;      //Auslöser im Boden
      SLONG          Winkel;
      UBYTE          State;
      SLONG          Dir;
      BOOL           ArabDoor;

      friend TEAKFILE &operator << (TEAKFILE &File, const CDoor &d) { File << d.ArrayPos << d.Winkel << d.State << d.Dir << d.ArabDoor; return (File); }
      friend TEAKFILE &operator >> (TEAKFILE &File, CDoor &d) { File >> d.ArrayPos >> d.Winkel >> d.State >> d.Dir >> d.ArabDoor; return (File); }
};

//--------------------------------------------------------------------------------------------
//Eine Maßeinheit:
//--------------------------------------------------------------------------------------------
class /**/CEinheit                               // 0 = km
{                                            // 1 = km/h
   public:                                   // 2 = m
      CString Name;                          // 3 = kN
      double  Faktor;                        // 4 = l
                                             // 5 = l/h
   public:                                   // 6 = DM
      SLONG   Umrechnung (SLONG Value);
      __int64 Umrechnung64 (__int64 Value);
      char   *bString (SLONG Value);
      char   *bString64 (__int64 Value);
};

//--------------------------------------------------------------------------------------------
//Eine Zeiteinheit:
//--------------------------------------------------------------------------------------------
class /**/PERIOD //Eine Zeitperiode vom Datum x bis Datum y
{
   public:
      short VonDatum;
      short BisDatum;

   public:
      PERIOD () {}
      PERIOD (short VonDatum, short BisDatum) { PERIOD::VonDatum=VonDatum; PERIOD::BisDatum=BisDatum; }

      friend TEAKFILE &operator << (TEAKFILE &File, const PERIOD &Period) { File << Period.VonDatum << Period.BisDatum; return (File); }
      friend TEAKFILE &operator >> (TEAKFILE &File, PERIOD &Period) { File >> Period.VonDatum >> Period.BisDatum; return (File); }
};

//--------------------------------------------------------------------------------------------
//Alles was an der Tafel bei der Flugleitung aushängt: (in Tafel.Cpp)
//--------------------------------------------------------------------------------------------
class /**/CTafelZettel
{
   public:
      SLONG ZettelId;               //0 oder Key im Routen/City Array
      SLONG Player;                 //-1 oder der derzeitige Hauptbieter (0-3)
      SLONG Preis;                  //Gebot (=Monatsmiete)
      SLONG Rang;
      BOOL  WasInterested;          //Hat der Spieler mitgeboten?

   friend TEAKFILE &operator << (TEAKFILE &File, const CTafelZettel &TafelZettel);
   friend TEAKFILE &operator >> (TEAKFILE &File, CTafelZettel &TafelZettel);
};

class /**/CTafelData
{
   public:
      CTafelZettel   Route[7];      //Bis zu 7 Routen werden versteigert
      CTafelZettel   City[7];       //Bis zu 7 Orte werden versteigert
      CTafelZettel   Gate[7];       //Bis zu 7 Gates werden versteigert

   public:
      void Clear (void);            //Daten alle löschen ==> keine Zettel
      void Randomize (SLONG Day);   //Zettel für n Tage auslosen

   friend TEAKFILE &operator << (TEAKFILE &File, const CTafelData &TafelData);
   friend TEAKFILE &operator >> (TEAKFILE &File, CTafelData &TafelData);
};

//--------------------------------------------------------------------------------------------
//Die gemieteten Gates: (in Schedule.cpp)
//--------------------------------------------------------------------------------------------
class /**/CGate
{
   public:
      SLONG Nummer;                 //Nummer auf Flughafen (0 bis n)
      SLONG Miete;                  //Monatsmiete in DM

   friend TEAKFILE &operator << (TEAKFILE &File, const CGate &Gate);
   friend TEAKFILE &operator >> (TEAKFILE &File, CGate &Gate);
};

class /**/CGates
{
   public:
      FBUFFER<CGate> Gates;
      UBYTE          Auslastung[24*7]; //Für 2x alle 24 Stunden der Uhr: Wie viele sind belegt?
      SLONG          NumRented;

   friend TEAKFILE &operator << (TEAKFILE &File, const CGates &Gates);
   friend TEAKFILE &operator >> (TEAKFILE &File, CGates &Gates);
};

//--------------------------------------------------------------------------------------------
//Die Flugzeugsorten:
//--------------------------------------------------------------------------------------------
class /**/CPlaneType
{
   //Basisdaten:
   public:
      CString  Name;                //Der Name des Flugzeuges
      __int64  NotizblockPhoto;     //Photos für den Notizblock
      SLONG    AnzPhotos;           //Zahl der Photos
      SLONG    FirstMissions;       //Ist erst verfügbar ab Mission x
      SLONG    FirstDay;            //...und auch dort erst ab Tag y

   //Technische Beschreibung
   public:
      CString  Hersteller;          //Textstring, z.B. "Boing"
      SLONG    Erstbaujahr;         //Zahl, z.B. 1980
      SLONG    Passagiere;          //Maximale Zahl der Passagiere (ein erste Klasse Passagier verbraucht 2 Plätze)
      SLONG    Reichweite;          //Reichweite in km
      SLONG    Geschwindigkeit;     //in km/h
      SLONG    Spannweite;          //in m
      SLONG    Laenge;              //in m
      SLONG    Hoehe;               //in m
      SLONG    Startgewicht;        //maximales Startgewicht
      CString  Triebwerke;          //Als Textstring
      SLONG    Schub;               //in lb
      SLONG    AnzPiloten;          //Piloten und Co-Piloten
      SLONG    AnzBegleiter;        //Zahl der Stewardessen
      SLONG    Tankgroesse;         //Kerosin in l
      SLONG    Verbrauch;           //Kerosin in l/h
      SLONG    Preis;               //Der Neupreis in DM
      float    Wartungsfaktor;      //Faktor für die Wartungskosten
      CString  Kommentar;           //Ggf. allgemeines über diese Maschine

   public:

};

class /**/CPlaneTypes : public ALBUM<CPlaneType>
{
   public:
      FBUFFER<CPlaneType> PlaneTypes;

   public:
      CPlaneTypes () : ALBUM<CPlaneType> (PlaneTypes, "PlaneTypes") {}
      CPlaneTypes (const CString &TabFilename);
      void    ReInit (const CString &TabFilename);
      ULONG   GetRandomExistingType (TEAKRAND *pRand);
      void    BlitPlaneAt (SBPRIMARYBM &TargetBm, SLONG PlaneType, SLONG Size, XY Pos, SLONG OwningPlayer);
};

//--------------------------------------------------------------------------------------------
//Der Auftrag für einen Flug: (Auftrag.cpp)
//--------------------------------------------------------------------------------------------
class /**/CAuftrag
{

   public:
      ULONG          VonCity;             //bezeichnet eine Stadt
      ULONG          NachCity;            //bezeichnet eine Stadt
      ULONG          Personen;            //So viele müssen in die Maschine passen
      UWORD          Date;                //Vertragsbeginn
      UWORD          BisDate;             //Vertragsende
      SBYTE          InPlan;              //0=Nix, -1=Durchgeführt, 1=1x im Plan
      SBYTE          Okay;                //0=Nix, -1=Durchgeführt, 1=1x im Plan
      SLONG          Praemie;             //Prämie bei Erfüllung
      SLONG          Strafe;              //Strafe bei Versagen
      BOOL           bUhrigFlight;        //Von Uhrig in Auftrag gegeben?

   CAuftrag () {}
   CAuftrag (ULONG VonCity, ULONG NachCity, ULONG Personen, UWORD Date, SLONG Praemie, SLONG Strafe);
   CAuftrag (ULONG VonCity, ULONG NachCity, ULONG Personen, UWORD Date);
   CAuftrag (char *VonCity, char *NachCity, ULONG Personen, UWORD Date, SLONG Praemie, SLONG Strafe);
   CAuftrag (char *VonCity, char *NachCity, ULONG Personen, UWORD Date);

   void  BlitGridAt (SBBM *pBitmap, XY Offset, BOOL Tagesansicht, SLONG Page);
   void  RandomCities (SLONG AreaType, SLONG HomeCity, TEAKRAND *pRandom);
   void  RefillForLastMinute (SLONG AreaType, TEAKRAND *pRandom);
   void  RefillForReisebuero (SLONG AreaType, TEAKRAND *pRandom);
   void  RefillForBegin (SLONG AreaType, TEAKRAND *pRandom);
   void  RefillForAusland (SLONG AreaType, SLONG CityNum, TEAKRAND *pRandom);
   void  RefillForUhrig (SLONG AreaType, TEAKRAND *pRandom);
   BOOL  FitsInPlane (const CPlane &Plane) const;

   friend TEAKFILE &operator << (TEAKFILE &File, const CAuftrag &Auftrag);
   friend TEAKFILE &operator >> (TEAKFILE &File, CAuftrag &Auftrag);
};

class CAuftraege : public ALBUM<CAuftrag>
{
   public:
      FBUFFER<CAuftrag> Auftraege;
      TEAKRAND          Random;

   public:
      CAuftraege () : ALBUM<CAuftrag> (Auftraege, "Auftraege") {}
      void  FillForLastMinute (void);
      void  RefillForLastMinute (SLONG Minimum=0);
      void  FillForReisebuero (void);
      void  RefillForReisebuero (SLONG Minimum=0);
      void  FillForAusland (SLONG CityNum);
      void  RefillForAusland (SLONG CityNum, SLONG Minimum=0);
      SLONG GetNumOpen (void);
      SLONG GetNumDueToday (void);

   friend TEAKFILE &operator << (TEAKFILE &File, const CAuftraege &Auftraege);
   friend TEAKFILE &operator >> (TEAKFILE &File, CAuftraege &Auftraege);
};

//--------------------------------------------------------------------------------------------
//Der Frachtauftrag für einen Flug: (Fracht.cpp)
//--------------------------------------------------------------------------------------------
class CFracht
{
   public:
      ULONG          VonCity;             //bezeichnet eine Stadt
      ULONG          NachCity;            //bezeichnet eine Stadt
      SLONG          Tons;                //So viele Tonnen müssen insgesamt geflogen werden
      SLONG          TonsOpen;            //So viele Tonnen sind weder geflogen, noch im Flugplan verplant
      SLONG          TonsLeft;            //So viele Tonnen müssen noch geflogen werden
      UWORD          Date;                //Vertragsbeginn
      UWORD          BisDate;             //Vertragsende
      SBYTE          InPlan;              //0=Nix, -1=Durchgeführt, 1=1x im Plan
      SBYTE          Okay;                //0=Nix, -1=Durchgeführt, 1=1x im Plan
      SLONG          Praemie;             //Prämie bei Erfüllung
      SLONG          Strafe;              //Strafe bei Versagen

   CFracht () {}
   CFracht (ULONG VonCity, ULONG NachCity, SLONG Tons, ULONG Personen, UWORD Date, SLONG Praemie, SLONG Strafe);
   CFracht (ULONG VonCity, ULONG NachCity, SLONG Tons, ULONG Personen, UWORD Date);
   CFracht (char *VonCity, char *NachCity, SLONG Tons, ULONG Personen, UWORD Date, SLONG Praemie, SLONG Strafe);
   CFracht (char *VonCity, char *NachCity, SLONG Tons, ULONG Personen, UWORD Date);

   void  BlitGridAt (SBBM *pBitmap, XY Offset, BOOL Tagesansicht, SLONG Page);
   void  RandomCities (SLONG AreaType, SLONG HomeCity, TEAKRAND *pRand);
   void  RefillForBegin (SLONG AreaType, TEAKRAND *pRand);
   void  Refill (SLONG AreaType, TEAKRAND *pRand);
   void  RefillForAusland (SLONG AreaType, SLONG CityNum, TEAKRAND *pRandom);
   BOOL  FitsInPlane (const CPlane &Plane) const;

   friend TEAKFILE &operator << (TEAKFILE &File, const CFracht &Fracht);
   friend TEAKFILE &operator >> (TEAKFILE &File, CFracht &Fracht);
};

class CFrachten : public ALBUM<CFracht>
{
   public:
      FBUFFER<CFracht> Fracht;
      TEAKRAND         Random;

   public:
      CFrachten () : ALBUM<CFracht> (Fracht, "Fracht") {}

      void  Fill (void);
      void  Refill (SLONG Minimum=0);
      SLONG GetNumOpen (void);
      void  RefillForAusland (SLONG CityNum, SLONG Minimum=0);
      void  FillForAusland (SLONG CityNum);
      SLONG GetNumDueToday (void);

   friend TEAKFILE &operator << (TEAKFILE &File, const CFrachten &Fracht);
   friend TEAKFILE &operator >> (TEAKFILE &File, CFrachten &Fracht);
};

//--------------------------------------------------------------------------------------------
//Eine vordefinierte Flugroute:
//--------------------------------------------------------------------------------------------
class CRoute
{
   public:
      BOOL           bNewInDeluxe;        //wird nicht serialisiert
      SLONG          Ebene;               //1=fein; 2=grob
      ULONG          VonCity;             //bezeichnet eine Stadt
      ULONG          NachCity;            //bezeichnet eine Stadt
      SLONG          Miete;
      double         Faktor;              //Attraktivität der Route
      SLONG          Bedarf;              //Soviele Leute wollen heute fliegen

   public:
      SLONG AnzPassagiere(void);          //So viele fliegen (Potentiell) hier

   BOOL operator > (const CRoute &p) const;
   BOOL operator < (const CRoute &p) const;

   friend TEAKFILE &operator << (TEAKFILE &File, const CRoute &r);
   friend TEAKFILE &operator >> (TEAKFILE &File, CRoute &r);
};

class CRouten : public ALBUM<CRoute>
{
   public:
      FBUFFER<CRoute> Routen;

   public:
      CRouten () : ALBUM<CRoute> (Routen, "Routen") {}
      CRouten (const CString &TabFilename);
      void  ReInit (const CString &TabFilename, bool bNoDoublettes);
      void  ReInitExtend (const CString &TabFilename);
      void  NewDay (void);

   friend TEAKFILE &operator << (TEAKFILE &File, const CRouten &r);
   friend TEAKFILE &operator >> (TEAKFILE &File, CRouten &r);
};

//--------------------------------------------------------------------------------------------
//Die Mieteigenschaften einer vordefinierte Flugroute:
//--------------------------------------------------------------------------------------------
class CRentRoute
{
   public:
      UBYTE          Rang;                //0=nicht gemietet; sonst 1..3
      SLONG          LastFlown;           //Wieviele Tage ist der letzte Flug her?
      SLONG          AvgFlown;            //Alle soviele Tage fliegt man im Schnitt
      SLONG          Auslastung;          //Zu soviel % ist der Flieger i.d.R. ausgelastet
      SLONG          AuslastungFC;        //Zu soviel % ist der Flieger i.d.R. ausgelastet
      SLONG          RoutenAuslastung;    //Soviel % des Routenbedarfes deckt man im Schnitt
      SLONG          HeuteBefoerdert;     //Soviele Passagiere haben wir heute hier befördert
      UBYTE          Image;               //Die Bekanntheit; beginnt bei 0, kann durch Werbung oder Geduld auf 100 gesteigert werden
      SLONG          Miete;               //Soviel zahlt der Spieler wegen der Versteigerung
      SLONG          Ticketpreis;         //Soviel kostet ein Ticket
      SLONG          TicketpreisFC;       //Soviel kostet ein Ticket in der ersten Klasse
      SLONG          TageMitVerlust;      //Solange fliegen wir hier schon mit Verlust
      SLONG          TageMitGering;       //Tage mit geringer Auslastung (<10%); wenn man die Route nicht gemietet hat, dann die Tage, seitdem sie einem aberkannt wurde oder 99

   CRentRoute ();

   friend TEAKFILE &operator << (TEAKFILE &File, const CRentRoute &r);
   friend TEAKFILE &operator >> (TEAKFILE &File, CRentRoute &r);
};

class CRentRouten
{
   public:
      FBUFFER<CRentRoute> RentRouten;

   SLONG GetNumUsed(void);

   friend TEAKFILE &operator << (TEAKFILE &File, const CRentRouten &r) { File << r.RentRouten; return (File); }
   friend TEAKFILE &operator >> (TEAKFILE &File, CRentRouten &r) { File >> r.RentRouten; return (File); }
};

//--------------------------------------------------------------------------------------------
//Der Flugplan eines Flugzeuges: (in Schedule.cpp)
//--------------------------------------------------------------------------------------------
class CFlugplanEintrag
{
   public:
      UBYTE          Okay;                //0=Auftrag Okay 1=falscher Tag, 2=schon durchgeführt, 3=Passagiere passen nicht
      UBYTE          HoursBefore;         //So viele Stunden vor dem Start wurde der Flug festgelegt
      UWORD          Passagiere;          //Zahl der belegten Sitzplätze (Normal)
      UWORD          PassagiereFC;        //Zahl der belegten Sitzplätze (in der ersten Klasse)
      UWORD          PArrived;            //Zahl Passagiere, die schon im Flughafen sind
      SLONG          Gate;                //-1 = kein Gate frei ==> Flugfeld; -2=externer Hafen
      UBYTE          GateWarning;         //Warnung, daß ein anderer Flug zu dieser Zeit Probleme macht
      ULONG          VonCity;             //bezeichnet eine Stadt
      ULONG          NachCity;            //bezeichnet eine Stadt
      SLONG          Startzeit;           //Zu diesem Zeitpunkt (0-24h) beginnt dieser Eintrag
      SLONG          Landezeit;           //Zu diesem Zeitpunkt (0-24h) landet das Flugzeug
      SLONG          Startdate;           //Referenz auf Sim.Date
      SLONG          Landedate;           //Referenz auf Sim.Date
      BOOL           ObjectType;          //0=Nix 1=Route 2=Auftrag 3=Automatik 4=Fracht
      SLONG          ObjectId;            //Bezeichnet Auftrag oder -1
      SLONG          Ticketpreis;         //Ticketpreis für Routen
      SLONG          TicketpreisFC;       //Ticketpreis für Routen (Erste Klasse)

   public:
      CFlugplanEintrag ();
      CFlugplanEintrag (BOOL ObjectType, ULONG ObjectId);
      void  BookFlight (CPlane *Plane, SLONG PlayerNum);
      void  CalcPassengers (SLONG PlayerNum, CPlane &qPlane);
      void  FlightChanged (void);
      SLONG GetEinnahmen (SLONG PlayerNum, const CPlane &qPlane);
      SLONG GetAusgaben (SLONG PlayerNum, const CPlane &qPlane);
      SLONG GetRealAusgaben (SLONG PlayerNum, const CPlane &qPlane, CString Name);

      friend TEAKFILE &operator << (TEAKFILE &File, const CFlugplanEintrag &Eintrag);
      friend TEAKFILE &operator >> (TEAKFILE &File, CFlugplanEintrag &Eintrag);
};

class CFlugplan
{
   public:
      SLONG StartCity;                    //Hier beginnt der Flugplan
      FBUFFER<CFlugplanEintrag>  Flug;    //Eine Zeile des Flugplans
      SLONG NextFlight;                   //Aktueller; sonst nächster Flug
      SLONG NextStart;                    //Verweis auf den nächsten, startenden Flug

   public:
      CFlugplan ();
      void UpdateNextFlight (void);
      void UpdateNextStart (void);
      void Dump (bool Hercules=true);
      BOOL ContainsFlight (ULONG ObjectType, SLONG ObjectId1, SLONG ObjectId2=NULL);

   friend TEAKFILE &operator << (TEAKFILE &File, const CFlugplan &Plan);
   friend TEAKFILE &operator >> (TEAKFILE &File, CFlugplan &Plan);
};

//--------------------------------------------------------------------------------------------
//Eine Panne, die an Bord eines Flugzeuges passiert ist:
//--------------------------------------------------------------------------------------------
class /**/CPanne
{
   public:
      SLONG          Date;                //Sim.Date
      SLONG          Time;                //Sim.Time
      SLONG          Code;                //Referenziert einen Text

   friend TEAKFILE &operator << (TEAKFILE &File, const CPanne &Panne);
   friend TEAKFILE &operator >> (TEAKFILE &File, CPanne &Panne);
};

//--------------------------------------------------------------------------------------------
//Das konkrete Teil in einem selbstgebauten Flugzeug:
//--------------------------------------------------------------------------------------------
class /**/CPlanePart
{
   public:
      XY      Pos2d;             // An dieser Stelle ist das Part untergebracht
      XY      Pos3d;             // An dieser Stelle ist das Part untergebracht
      CString Shortname;         // z.B. C1
      CString ParentShortname;   // Identifiziert das Teil wo wir drankleben z.B. B1
      SLONG   ParentRelationId;  // Identifiziert die Relation durch die wir verbunden sind 100

   public:
      SBBM &GetBm(SBBMS &PartBms);

   friend TEAKFILE &operator << (TEAKFILE &File, const CPlanePart &pp);
   friend TEAKFILE &operator >> (TEAKFILE &File, CPlanePart &pp);
};

class /**/CPlaneParts : public ALBUM<CPlanePart>
{
   public:
      FBUFFER<CPlanePart> PlaneParts;

   public:
      CPlaneParts() : ALBUM<CPlanePart> (PlaneParts, "PlaneParts") {}

      void  Sort (void);
      bool  IsShortnameInAlbum (CString ShortName);
      ULONG IdFrom (CString ShortName);
      bool  IsSlotFree (CString Slotname);

   friend TEAKFILE &operator << (TEAKFILE &File, const CPlaneParts &pp);
   friend TEAKFILE &operator >> (TEAKFILE &File, CPlaneParts &pp);
};

//--------------------------------------------------------------------------------------------
// Ein selbstgebautes Flugzeug:
//--------------------------------------------------------------------------------------------
class /**/CXPlane
{
   public:
      CString     Name;        // Name des Flugzeuges
      SLONG       Cost;        // Soviel kostet es
      CPlaneParts Parts;       // Die Einzelteile des Flugzeugs

   public:
      void BlitPlaneAt (SBPRIMARYBM &TargetBm, SLONG Size, XY Pos, SLONG OwningPlayer);
      void Clear (void);
      long CalcCost (void);
      long CalcPassagiere (void);
      long CalcReichweite (void);
      long CalcVerbrauch (void);
      long CalcWeight (void);
      long CalcPower (void);
      long CalcNoise (void);
      long CalcWartung (void);
      long CalcTank (bool bFaked=false);
      long CalcSpeed (void);
      long CalcPiloten (void);
      long CalcBegleiter (void);
      void Load (CString Filename);
      void Save (CString Filename);
      bool IsBuildable (void);
      bool IstPartVorhanden (CString Shortname, bool bOnlyThisType=false);
      CString GetError (void);

   friend TEAKFILE &operator << (TEAKFILE &File, const CXPlane &p);
   friend TEAKFILE &operator >> (TEAKFILE &File, CXPlane &p);
   void operator = (const CXPlane &p);
};

//--------------------------------------------------------------------------------------------
//Konkrete Flugzeuge, die jemand besitzt:
//--------------------------------------------------------------------------------------------
class /**/CPlane
{
   //Basisdaten:
   public:
      CString        Name;                //Der Name des Flugzeuges
      SLONG          Ort;                 //-1=Landend; -2=Startend; -5 in der Luft; sonst Stadt 
      XY             Position;            //Position am Flughafen oder in der Luft
      XY             GlobePos;            //Position auf der Kugel im Scheduler
      BOOL           BehindGlobe;         //Ist das Flugzeug auf der Rückseite der Erde
      UBYTE          GlobeAngle;          //Der Winkel des Flugzeuges auf dem Globus
      XY             AirportPos;          //Position am Flughafen
      SLONG          TargetX;             //Speicherung der Zielposition als Speed-up
      UBYTE          Startzeit;           //Speicherung als Speed-up oder 255 falls nach Landung
      ULONG          TypeId;              //referenziert CPlaneType oder ist -1
      CFlugplan      Flugplan;            //Der Flugplan
      UBYTE          WorstZustand;        //Der schlimmste Zustand
      UBYTE          Zustand;             //Reparaturzustand: 0%-100%
      UBYTE          TargetZustand;       //So soll es aussehen
      FBUFFER<ULONG> Salden;              //Die täglichen Einnahmen-Ausgaben des Flugzeuges der letzten Woche
      SLONG          Baujahr;             //Das Baujahr dieses Flugzeuges
      SLONG          AnzPiloten;          //Aktuelle Zahl: Piloten und Co-Piloten
      SLONG          AnzBegleiter;        //Aktuelle Zahl: Zahl der Stewardessen
      SLONG          MaxBegleiter;        //Ziel: Zahl der Stewardessen
      SLONG          PersonalQuality;     //So gut sind diese Leute im Schnitt
      SLONG          Wartungskosten;      //Soviel kostet das Flugzeug pro Tag an Wartung
      UBYTE          Sitze, SitzeTarget;
      UBYTE          Essen, EssenTarget;
      UBYTE          Tabletts, TablettsTarget;
      UBYTE          Deco, DecoTarget;
      UBYTE          Triebwerk, TriebwerkTarget;
      UBYTE          Reifen, ReifenTarget;
      UBYTE          Elektronik, ElektronikTarget;
      UBYTE          Sicherheit, SicherheitTarget;
      SLONG          MaxPassagiereTarget, MaxPassagiereTargetFC; //Soviele Leute passen bei der derzeiten Konfiguration rein
      //SLONG          AnzPutzcrew;
      SLONG          Auslastung;          //Zu soviel % ist es gefüllt
      SLONG          AuslastungFC;        //Zu soviel % ist es gefüllt
      SLONG          Kilometer;           //Soviele Kilometer ist es schon geflogen
      SLONG          SummePassagiere;     //Soviele Passagiere hat es schon befördert
      SLONG          MaxPassagiere, MaxPassagiereFC; //Soviele Leute passen bei der derzeiten Konfiguration rein
      BOOL           Sponsored;           //Wenn TRUE, dann ist es staatlich gefördert und schnlecht zu verkaufen
      BOOL           OhneSitze;           //Wenn TRUE, dann wurden die Sitze zum transportieren von Fracht ausgebaut (Umrüstkosten)
      SLONG          NumPannen;           //Anzahl der Pannen insgesamt
      SLONG          Problem;             //0 oder Anzahl der Stunden bis das Flugzeug kein Problem mehr hat
      SLONG          PseudoProblem;       //0 oder Anzahl der Stunden wie das Flugzeug noch festgehalten wird
      FBUFFER<CPanne> Pannen;             //Die letzten 10 Pannen

   //Kopien aus CPlaneType
   public:
      CString  ptHersteller;          //Textstring, z.B. "Boing"
      CString  ptName;                //Der Name des Flugzeuges
      SLONG    ptErstbaujahr;         //Zahl, z.B. 1980
      SLONG    ptReichweite;          //Reichweite in km
      SLONG    ptGeschwindigkeit;     //in km/h
      SLONG    ptPassagiere;          //Maximale Zahl der Passagiere (ein erste Klasse Passagier verbraucht 2 Plätze)
      SLONG    ptAnzPiloten;          //Piloten und Co-Piloten
      SLONG    ptAnzBegleiter;        //Zahl der Stewardessen
      SLONG    ptTankgroesse;         //Kerosin in l
      SLONG    ptVerbrauch;           //Kerosin in l/h
      SLONG    ptPreis;               //Der Neupreis in DM
      SLONG    ptLaerm;               //Der Laerm, den diese Maschine verursacht
      float    ptWartungsfaktor;      //Faktor für die Wartungskosten
      CString  ptKommentar;           //Ggf. allgemeines über diese Maschine

      CXPlane  XPlane;                //Selbstgebautes Flugzeug falls TypeId==-1

   public:
      CPlane ();
      CPlane (CString Name, ULONG TypeId, UBYTE Zustand, SLONG Baujahr);
      void  AddPanne (SLONG Code);
      SLONG CalculatePrice (void);        //Berechnet den Marktwert
      void  CheckFlugplaene (SLONG PlayerNum, BOOL Sort=TRUE, BOOL PlanGates=TRUE);
      void  ExtendFlugplaene (SLONG PlayerNum);
      void  FlugplaeneFortfuehren (SLONG PlayerNum);
      void  DoOneStep (SLONG PlayerNum);
      BOOL  CanBeSold (void);             //Wird's zur Zeit verwendet?
      void  UpdateGlobePos (UWORD EarthAlpha);
      SLONG GetSaldo (void);
      SLONG GetMaxPassengerOpenFlight (SLONG PlayerNum);
      void  ClearSaldo (void);
      void  CalculateHappyPassengers (SLONG PlayerNum);
      void  UpdatePersonalQuality (SLONG PlayerNum);
      void  RepairReferences (void);

      const CFlugplanEintrag *GetFlugplanEintrag (void);

      BOOL operator > (const CPlane &p) const { return (Name>p.Name); }
      BOOL operator < (const CPlane &p) const { return (Name<p.Name); }

   friend TEAKFILE &operator << (TEAKFILE &File, const CPlane &Plane);
   friend TEAKFILE &operator >> (TEAKFILE &File, CPlane &Plane);
};

class /**/CPlanes : public ALBUM<CPlane>
{
   public:
      FBUFFER<CPlane> Planes;

   public:
      CPlanes () : ALBUM<CPlane> (Planes, "Planes") {}
      BOOL   IsPlaneNameInUse (const CString &PlaneName);
      void   DoOneStep (SLONG PlayerNum);
      void   UpdateGlobePos (UWORD EarthAlpha);
      void   RepairReferences (void);
      double GetAvgBegleiter (void);
      BOOL   HasProblemPlane (void);

   friend TEAKFILE &operator << (TEAKFILE &File, const CPlanes &Planes);
   friend TEAKFILE &operator >> (TEAKFILE &File, CPlanes &Planes);
};

//--------------------------------------------------------------------------------------------
//Die Liste mit Flugzeugnamen: (in PlaneTyp.Cpp)
//--------------------------------------------------------------------------------------------
class /**/CPlaneNames
{
   private:
      BUFFER<CString> NameBuffer1;
      BUFFER<CString> NameBuffer2;

   public:
      CPlaneNames ();
      CPlaneNames (const CString &TabFilename);
      ~CPlaneNames ();
      void  ReInit (const CString &TabFilename);
      CString GetRandom (TEAKRAND *pRnd);
      CString GetUnused (TEAKRAND *pRnd);
};

//--------------------------------------------------------------------------------------------
//Die Basisdaten für die Klackertafel:
//--------------------------------------------------------------------------------------------
class /**/CKlackerPlane
{
   public:
      XY             ScreenPos;
      SLONG          Size;
      SLONG          Dir;
      SLONG          Logo;
};

class /**/CKlackerPlanes
{
   public:
      BUFFER<CKlackerPlane> KlackerPlanes;
      SLONG                 TimeSinceStart;

   public:
      CKlackerPlanes () { Reset(); }
      void Reset (void);
      void Pump (XY AvoidPoint);
      void PostPaint (SBBM &PrimaryBm);
      void PostPaint (SBPRIMARYBM &PrimaryBm);
};

class /**/KLACKER
{
   public:
      SBBMS           Cursors;
      SBBMS           KlackerBms;
      SLONG           NewScreen;
      GfxLib         *pGLib;
      bool            LineDisabled[16];
      char            Soll [24*16];  //So sollen die Plättchen zur Zeit sein
      char            Haben [24*16]; //So sind die Plättchen zur Zeit (Indices auf FontDef)
      SBFX            KlackerFx[3];

   public:
      KLACKER ();
      ~KLACKER ();
      void Clear (void);
      BOOL Klack ();
      void Warp (void);
      void PrintAt (SLONG x, SLONG y, const char *Text);
      void PrintVolumeAt (SLONG x, SLONG y, SLONG Maximum, SLONG Current);
      BOOL IsFinished (void);
};

class /**/CHeadline
{
   public:
      CString Headline;                //Die Überschrift
      __int64 PictureId;               //0 oder der Identifier eines Bildes
      SLONG   PicturePriority;

   friend class CKiosk;
   friend class HEADLINES;
};

//--------------------------------------------------------------------------------------------
//Die Schlagzeilen der Tageszeitungen (in Misc.cpp)
//--------------------------------------------------------------------------------------------
class /**/HEADLINES
{
   private:
      CString   HeadlineFile;
      CHeadline Headline[30];       //Die aktuellen Schlagzeilen der Zeitungen (max. 10 pro Zeitung)
      SLONG     CurrentChain[3];   //Die aktuellen Ketten als Zahl (Wild/News/Lokal)
      SLONG     NumRand [3];       //Zahl der Zufallsantworten in einer Kette
      CHeadline Override [30];     //Wird tagsüber gesetzt und am nächsten Tag effektiv

      //Flexibles Zufallszeux:
      CString   FlexiCity[3];      //Zufallsstadt für einen Thread
      SLONG     FlexiNumber[3];    //Zufallszahl für einen Thread

   public:
      BOOL      IsInteresting;     //Ist heute etwas interessantes dabei?

   public:
      HEADLINES ();
      HEADLINES (const CString &TabFilename);
      CHeadline GetHeadline (long Newspaper, SLONG Index);
      void      Init (void);
      void      SetHeadline (long Newspaper, const CString &Headlinetext);
      void      BlitHeadline (long Newspaper, SBBM &Offscreen, CPoint p1, CPoint p2, BYTE Color);
      void      ReloadHeadline (void);
      void      InterpolateHeadline (void);
      void      ComparisonHeadlines (void);
      void      ReInit (const CString &TabFilename);
      void      AddOverride (long Newspaper, const CString &Headlinetext, __int64 Pictureid, SLONG PicturePriority);
      void      SortByPriority (void);

   friend class SIM;
   friend TEAKFILE &operator << (TEAKFILE &File, const HEADLINES &Headlines);
   friend TEAKFILE &operator >> (TEAKFILE &File, HEADLINES &Headlines);
};

//--------------------------------------------------------------------------------------------
// City.Cpp :
//--------------------------------------------------------------------------------------------
class /**/CITY //Eine Stadt
{
   public:
      CString     Name;             //z.B. "Rio de Janeiro"
      CString     Lage;             //z.B. "Südamerika"
      SLONG       Areacode;         //1=Europa, 2=Amerika, 3=Afrika-Indien, 4=Asien&Ozeanien
      CString     Kuerzel;          //aktuelles kuerzel
      CString     KuerzelGood;      //z.B. "MOS" f. Moskau
      CString     KuerzelReal;      //z.B. "SVO" f. Moskau
      CString     Wave;             //Die Wave-Datei
      SLONG       TextRes;          //Base-Ressource Id für Texte
      SLONG       AnzTexts;         //Anzahl der Seiten mit Text
      CString     PhotoName;        //Name des Photos auf die Stadt
      SLONG       AnzPhotos;        //Anzahl der Photos (%li muß dann im Namen vorkommen)
      SLONG       Einwohner;        //Die Zahl der Einwohner
      CPoint      GlobusPosition;   //Die Position auf dem Globus
      CPoint      MapPosition;      //Die Position auf der flachen Karte
      SLONG       BuroRent;         //Die Monatsmiete für eine Niederlassung
      BOOL        bNewInAddOn;      //Ist im Add-On neu hinzugekommen?
      //Vorraussetzung für Anflug

   public:
      void  Update (long Jahr);
      SLONG GetBuroRent (void);
};

class /**/CITIES : public ALBUM<CITY>
{
   public:
      BUFFER<CITY>  Cities;
      BUFFER<SLONG> HashTable;

   public:
      CITIES () : ALBUM<CITY> (Cities, "Cities") {}
      CITIES (const CString &TabFilename);
      void  ReInit (const CString &TabFilename);
      void  Update (long Jahr);
      SLONG CalcDistance (long CityId1, long CityId2);
      SLONG CalcFlugdauer (long CityId1, long CityId2, long Speed);
      SLONG GetRandomUsedIndex (TEAKRAND *pRand=NULL);
      SLONG GetRandomUsedIndex (SLONG AreaCode, TEAKRAND *pRand=NULL);
      ULONG GetIdFromName (const char *Name);
      ULONG GetIdFromNames (const char *Name, ...);
      void  UseRealKuerzel (BOOL Real);
};

//--------------------------------------------------------------------------------------------
//Die Mieteigenschaften einer vordefinierte Stadt:
//--------------------------------------------------------------------------------------------
class /**/CRentCity
{
   public:
      UBYTE          Rang;                //0=nicht gemietet; sonst 1..3
      UBYTE          Image;
      SLONG          Miete;

   public:
      CRentCity ();

   friend TEAKFILE &operator << (TEAKFILE &File, const CRentCity &r);
   friend TEAKFILE &operator >> (TEAKFILE &File, CRentCity &r);
};

class /**/CRentCities
{
   public:
      FBUFFER<CRentCity> RentCities;

   SLONG GetNumUsed(void);

   friend TEAKFILE &operator << (TEAKFILE &File, const CRentCities &r) { File << r.RentCities; return (File); }
   friend TEAKFILE &operator >> (TEAKFILE &File, CRentCities &r) { File >> r.RentCities; return (File); }
};

//--------------------------------------------------------------------------------------------
//Eine Tabelle mit Daten; verwendet im Scheduler oder beim Notepad
//--------------------------------------------------------------------------------------------
class /**/CDataTable
{
   public:
      CString                 Title;               //Die Überschrift der Tabelle
      SLONG                   AnzRows;             //Zahl der Zeilen
      SLONG                   AnzColums;           //Zahl der Spalten
      FBUFFER<CString>        ColTitle;            //Überschriften der Spalten
      FBUFFER<CString>        Values;              //Die Werte der Tabellenfelder
      FBUFFER<UBYTE>          ValueFlags;          //Zusatzangaben für Tabellenfelder
      FBUFFER<SLONG>          LineIndex;           //Verbindung zwischen Tabellenzeile und Tabellenquelle

   public:
      void Destroy (void);
      void Sort (void);
      void FillWithPlanes (CPlanes *Planes, BOOL Expert=FALSE, SLONG FilterType=0, SLONG Filter1=0, SLONG Filter2=0);
      void FillWithPlaneTypes (void);
      void FillWithXPlaneTypes (void);
      void FillWithRouten (CRouten *Routen, CRentRouten *RentRouten, BOOL UniqueOnly=FALSE);
      void FillWithAllRouten (CRouten *Routen, CRentRouten *RentRouten, BOOL UniqueOnly=FALSE);
      void FillWithAuftraege (CAuftraege *Auftraege);
      void FillWithCities (CRentCities *RentCities);
      void FillWithExperts (SLONG PlayerNum);
      void FillWithFracht (CFrachten *Frachten);

   friend TEAKFILE &operator << (TEAKFILE &File, const CDataTable &d);
   friend TEAKFILE &operator >> (TEAKFILE &File, CDataTable &d);
};

//--------------------------------------------------------------------------------------------
//Die Notizblocks: (Block.cpp)
//--------------------------------------------------------------------------------------------
class /**/BLOCK
{
   public:
      CDataTable Table;
      CDataTable TableB;

      CPlaner   *Base;              //Basisdaten fürs malen
      GfxLib    *pGLibPicture;
      XY         ScreenPos;         //Position auf dem Bildschirm
                 
      BOOL       Destructing;       //Block wird gerade zerstört
      SLONG      AnimationStart;    //Startzeit für die Animation
      SLONG      PlayerNum;         
      SLONG      StyleType;         //Block oder Window

      SBBM       Bitmap;            //
      BOOL       IsTopWindow;       //Ist dieses Fenster oben auf?

      //Linker Block:
      SLONG      BlockType;         //1..5=City, Plane, Auftrag, Route, Info
      SLONG      SelectedId;        //CityId, PlaneId, .... (Verzeichniseintrag)
      UBYTE      Index;             //Inhalt oder konkreter Verzeichniseintrag
      SLONG      Page, AnzPages;    //Aktuell angezeigte Seite
      SLONG      Tip, TipInUse;     //Tip auf der linken Seite?
      SLONG      PageSize;          //Zahl der Linien auf einer Seite

      //Rechter Block:
      BOOL       DoubleBlock;       //Ist der rechte Abschnitt sichtbar?
      SLONG      BlockTypeB;        //1..6=City, Plane, Auftrag, Route, Info, Fracht
      SLONG      SelectedIdB;       //CityId, PlaneId, .... (Verzeichniseintrag)
      UBYTE      IndexB;            //Inhalt oder konkreter Verzeichniseintrag
      SLONG      PageB, AnzPagesB;  //Aktuell angezeigte Seite
      SLONG      TipB, TipInUseB;   //Tip auf der rechten Seite?
      SLONG      PageSizeB;         //Zahl der Linien auf einer Seite

      //Tip-Eigenschaften:
      SLONG      CurrentTipId;      //z.B. CityId
      SLONG      LastTipId;         //z.B. CityId
      SLONG      CurrentTipIdPar1;
      SLONG      LastTipIdPar1;
      SLONG      CurrentTipIdPar2;
      SLONG      LastTipIdPar2;
      SLONG      CurrentTipIdPar3;
      SLONG      LastTipIdPar3;
      SLONG      CurrentTipIdPar4;
      SLONG      LastTipIdPar4;
      SLONG      CurrentTipIdPar5;
      SLONG      LastTipIdPar5;
      SLONG      CurrentTipIdPar6;
      SLONG      LastTipIdPar6;
      SLONG      CurrentTipIdPar7;
      SLONG      LastTipIdPar7;

      UBYTE      Indexes[6];        //Die Zustände für verschiedene Bereiche
      SLONG      SelectedIds[6];    //Die Zustände für verschiedene Bereiche
      SLONG      Pages[6];          //Aktuell angezeigte Seite

   public:
      BLOCK ();
      ~BLOCK ();
      void LoadLib (const CString &LibName);
      void BlitAt (SBBM &RoomBm);
      void UpdateTip (SLONG PlayerNum=-1, BOOL StyleType=-1);
      void UpdatePageSize (void);
      void Refresh (SLONG PlayerNum=-1, BOOL StyleType=-1);
      void RefreshData (SLONG PlayerNum=-1);
      void GotoIndex (void);
      void GotoNext (void);
      void GotoPrevious (void);
      void SetTip (SLONG TipTypeA, SLONG TipTypeB, SLONG TipId, SLONG TipPar1=0, SLONG TipPar2=0, SLONG TipPar3=0, SLONG TipPar4=0, SLONG TipPar5=0, SLONG TipPar6=0, SLONG TipPar7=0);

   friend TEAKFILE &operator << (TEAKFILE &File, const BLOCK &b);
   friend TEAKFILE &operator >> (TEAKFILE &File, BLOCK &b);
};

class BLOCKS : public ALBUM<BLOCK>
{
   public:
      BOOL           RepaintAll;
      FBUFFER<BLOCK> Blocks;

   public:
      BLOCKS () : ALBUM<BLOCK> (Blocks, "Blocks") {}
      void RefreshAuftragsBloecke (SLONG PlayerNum, SLONG Background);

   friend TEAKFILE &operator << (TEAKFILE &File, const BLOCKS &b);
   friend TEAKFILE &operator >> (TEAKFILE &File, BLOCKS &b);
};

//--------------------------------------------------------------------------------------------
// Brick.Cpp :
//--------------------------------------------------------------------------------------------
class /**/BRICK //Ein einzelnes Bodenteil eines bestimmten Zeitalters
{
   private:
      CString             Filename;      //Name der lbm-Files
      SLONG               RamPriority;   //Priotität fürs VGA-RAM
      BOOL                NonTrans;      //Flag falls ohne Glass
      BOOL                Triggered;     //Läuft die Animation nicht ständig, sondern getriggert ab?
      BYTE                Layer;         //Die Sortierungsschicht (y übergeordnet)
      BYTE                AnimSpeed;     //Divisor für Ticker als Bitmap-Index
      SLONG               FloorOffset;   //Y-Offset für die Sortierung
      XY                  BaseOffset;    //Basis für iEditor
      XY                  Grid;          //Schrittweiter für iEditor
      SLONG               MinY;          //Limit für iEditor
      SLONG               MaxY;          //Limit für iEditor
      UBYTE               ObstacleType;  //Art der Verkehrsbehinderung
      BUFFER<SBBM>        Bitmap;        //Bitmap(s) ggf. mit Animation
      BUFFER<__int64>     graphicIDs;    //Bitmap-IDs
      BUFFER<SLONG>       WaitTimes;     //Solange bleibt der jeweilige Frame...
      SLONG               WaitSum;       //Summe der Waittimes

   public:
      BRICK ();
      ~BRICK ();
      BRICK (const BRICK &Brick) { Filename=Brick.Filename; }
      BRICK &operator = (const BRICK &Brick) { Filename=Brick.Filename; return *this; }
      XY    GetBitmapDimension (void) const;
      void  BlitAt (SBBM &Offscreen, BOOL Ansatz, const XY &ScreenPos, SLONG Phase=-1);
      void  BlitAt (SBBM &Offscreen, BOOL Ansatz, SLONG x, SLONG y, SLONG Phase=-1) { BlitAt (Offscreen, Ansatz, XY (x,y), Phase); }
      void  BlitAt (SBBM &Offscreen, BOOL Ansatz, const XY &p1, const XY &p2);
      void  BlitAt (SBBM &Offscreen, BOOL Ansatz, SLONG x1, SLONG y1, SLONG x2, SLONG y2) { BlitAt (Offscreen, Ansatz, XY (x1,y1), XY (x2, y2)); }
      void  BlitAt (SBPRIMARYBM &Offscreen, BOOL Ansatz, const XY &ScreenPos, SLONG Phase=-1);
      void  BlitAt (SBPRIMARYBM &Offscreen, BOOL Ansatz, SLONG x, SLONG y, SLONG Phase=-1) { BlitAt (Offscreen, Ansatz, XY (x,y), Phase); }
      void  BlitAt (SBPRIMARYBM &Offscreen, BOOL Ansatz, const XY &p1, const XY &p2);
      void  BlitAt (SBPRIMARYBM &Offscreen, BOOL Ansatz, SLONG x1, SLONG y1, SLONG x2, SLONG y2) { BlitAt (Offscreen, Ansatz, XY (x1,y1), XY (x2, y2)); }
      BOOL  IsGlasAt (SLONG x, SLONG y);
      void  UpdateBrick (void);
      XY    GetIntelligentPosition (SLONG x, SLONG y);

   friend class BRICKS;
   friend class PLAYER;
   friend class BUILDS;
   friend class AIRPORT;
   friend class AirportView;
};

class /**/BRICKS : public ALBUM<BRICK>
{
   public:
      BUFFER<BRICK> Bricks;

   private:
      short         CurrentDate;

   public:
      BRICKS () : ALBUM<BRICK> (Bricks, "Bricks") {}
      BRICKS (const CString &TabFilename);
      void  ReInit (const CString &TabFilename);
      void  Destroy (void);
      void  UpdateBricks (void);
      void  RestoreBricks (void);
};

//--------------------------------------------------------------------------------------------
//Die Verwendung der Bricks (ebenfalls in Brick.cpp)
//--------------------------------------------------------------------------------------------
class /**/BUILD //Die Verwendung eines Bodenteils
{
   private:
      SLONG  BrickId;         //Verweis in Brick-Tabelle
      XY     ScreenPos;       //Position im Flughafen
      UBYTE  Par;             //Parameter, z.B. für die Raumnummer

   public:
      BUILD () {}
      BUILD (long BrickId, const XY &ScreenPos, BOOL Ansatz);
      friend TEAKFILE &operator << (TEAKFILE &File, const BUILD &Build);
      friend TEAKFILE &operator >> (TEAKFILE &File, BUILD &Build);

   friend class PERSON;
   friend class BUILDS;
   friend class AIRPORT;
   friend class PLAYER;
   friend class AirportView;
};

class /**/BUILDS : public ALBUM<BUILD>
{
   public:
      FBUFFER<BUILD> Builds;

   public:
      BUILDS ();
      void Clear ();
      void ReInit (SLONG Hall, SLONG Level) { Load (Hall, Level); }
      void Load (SLONG Hall, SLONG Level);
      void Save (SLONG Hall, SLONG Level) const;
      void Sort (void);

   friend class Airport;
   friend class AirportView;
   friend TEAKFILE &operator << (TEAKFILE &File, const BUILDS &Builds);
   friend TEAKFILE &operator >> (TEAKFILE &File, BUILDS &Builds);
};

//--------------------------------------------------------------------------------------------
// Die 'Personensorten' : (Person.cpp)
//--------------------------------------------------------------------------------------------
class /**/CLAN
{
   private:
      UBYTE          Type;             //Guest, Player, Worker, FX-Charakter
      BOOL           TodayInGame;      //Heute im Spiel?
      SLONG          Group;            //Gruppe, die dieser Clan angehört
      UBYTE          Wkeit;            //Wkeit, daß nicht neu gewürfelt wird
      UBYTE          UpdateNow;        //Ist ein Pool-Update erwünscht?
      SLONG          WalkSpeed;        //Laufgeschwindigkeit
      SLONG          Faktor;           //Animationsgeschwindigkeit
      XY             Offset;           //Offset zum blitten
      XY             ShadowOffset;     //Offset zum blitten für den Schatten
      XY             SkelettOffset;    //Offset zum blitten für das Skelett
      XY             GimmickOffset;    //Offset zum blitten für das Gimmick
      CString        PalFilename;      //Hierdrin ist die Palette zu finden
      SLONG          GimmickArt1;      //Art des Gimmicks (LookDir) wenn Person läuft
      SLONG          GimmickArt2;      //Art des Gimmicks (LookDir) wenn Person wartet
      SBBMS          Phasen[14];       //N,O,S,W,Gimmick,Winken,SitzenN, SitzenS, Stehen(in4Richtungen), Stehen-Gimmick, RunN, RunO, RunS, RunW
      SBBMS          Shadow[14];       //N,O,S,W,Gimmick,Winken,SitzenN, SitzenS, Stehen(in4Richtungen), Stehen-Gimmick, RunN, RunO, RunS, RunW
      SBBMS          Skelett[14];      //N,O,S,W,Gimmick,Winken,SitzenN, SitzenS, Stehen(in4Richtungen), Stehen-Gimmick, RunN, RunO, RunS, RunW
      SLONG          HasSuitcase;      //0=Nein, <0: Kann einen haben; >0 hat einen Koffer

      SLONG          FloorOffset;      //Z-Distanz
      SLONG          GimmickTime;      //Wann wurde das Gimmick zuletzt genutzt?

      CHLPool         ClanPool;
      CHLPool         ClanGimmick;
      CHLPool         ClanWarteGimmick;
      BUFFER<__int64> PhasenIds[14];   //N,O,S,W,Gimmick,Winken,SitzenN, SitzenS, Stehen(in4Richtungen), Stehen-Gimmick, RunN, RunO, RunS, RunW
      BUFFER<__int64> SkelettIds[14];  //N,O,S,W,Gimmick,Winken,SitzenN, SitzenS, Stehen(in4Richtungen), Stehen-Gimmick, RunN, RunO, RunS, RunW

   public:
      CLAN () {}
      void BlitAt (SBPRIMARYBM &Offscreen, SLONG Dir, SLONG Phase, XY ScreenPos, UBYTE Running);
      void BlitAt (SBPRIMARYBM &Offscreen, SLONG Dir, SLONG Phase, SLONG x, SLONG y, UBYTE Running) { BlitAt (Offscreen, Dir, Phase, XY (x,y), Running); }
      void BlitSkelettAt (SBPRIMARYBM &Offscreen, SLONG Dir, SLONG Phase, XY ScreenPos);
      void BlitLargeAt (SBBM &Offscreen, SLONG Dir, SLONG Phase, XY ScreenPos);

   friend class PERSON;
   friend class PERSONS;
   friend class CLANS;
   friend class CTakeOffApp;
   friend class GameFrame;
   friend class AirportView;
   friend class CBuero;
   friend class CDutyFree;
   friend class CStdRaum;
   friend class CBank;
   friend class CMakler;
   friend class CPersonQueue;
   friend class AIRPORT;
   friend class PLAYER;
   friend void UpdateHLinePool (void);

   friend TEAKFILE &operator << (TEAKFILE &File, const CLAN &Clan);
   friend TEAKFILE &operator >> (TEAKFILE &File, CLAN &Clan);
};

class /**/CLANS : public ALBUM<CLAN>
{
   public:
      FBUFFER<CLAN> Clans;

   public:
      CLANS () : ALBUM<CLAN> (Clans, "Clans") {}
      CLANS (const CString &TabFilename);
      void  ReInit (const CString &TabFilename);
      void  LoadBitmaps (void);
      void  ReloadBitmaps (void);
      UBYTE GetCustomerIdByGroup (SLONG Group);
      UBYTE GetCustomerId (SLONG Browned, SLONG Koffer=99, TEAKRAND *pRand=NULL);
      UBYTE GetSuitcaseId (SLONG Koffer);
      UBYTE GetPlayerId (ULONG PlayerType); 
      void  UpdateClansInGame (BOOL FirstDay);
      SLONG GetAnimationId (SLONG AnimationGroup);
};

//--------------------------------------------------------------------------------------------
// Die Ausprägungen der Clan, die gerade so am Flughafen rumlaufen: (Person.cpp)
//--------------------------------------------------------------------------------------------
class /**/PERSON
{
   private:
      UBYTE       ClanId;           //Referenziert das Clan-Array
      UBYTE       Dir;              //In welche Richtung 0-3 bzw. 0-7 geht die Person
      UBYTE       LookDir;          //Hierhin schauen wir
      UBYTE       Phase;            //Die aktuelle Animationsphase
      UBYTE       State;            //Zustand (bei Check-In, bei Wartehalle, ...); bzw Player#
      SLONG       StatePar;         //Zustandsparameter (0:kein Raum, >0:Raumnummer&Entry/Exit-Flag, -1:Raum wird verlassen)
      SLONG       FloorOffset;      //Cache für den Clan-Wert
      UBYTE       WaitCount;        //Wegen Deadlock; wird beim Warten erhöht (20x pro sec); Bei den Werten 200-250 wird gedrängelt
      UBYTE       Reason;           //Der Grund, warum sie am Flughafen ist (z.B. Abflug)
      UBYTE       FlightAirline;    //Die Fluglinie (0-3) der Person..
      ULONG       FlightPlaneId;    //..mit diesem Flugzeug der Linie wird geflogen..
      UBYTE       FlightPlaneIndex; //..und mit diesem Flug (0-6)
      UBYTE       Mood;             //Stimmung
      UBYTE       MoodCountdown;    //Wie lange noch zeigen
      UBYTE       Running;          //Rennt diese Figur?
      UBYTE       FirstClass;       //Ein ErsteKlasse Passagier?

      TEAKRAND    PersonalRand;     //Der private Zufallsgenerator, wenn es darum geht ein neues Ziel zu bestimmen
      TEAKRAND    PersonalRandWalk; //Der private Zufallsgenerator für Laufangelegenheiten

   public:
      XY          Target;           //Hier will die Person hin
      XY          Position;         //Die rechnerische Position (ohne Isometrie)
      XY          ScreenPos;        //aktuelle Position auf dem Flughafen/Bildschirm

   public:
      PERSON ();
      PERSON (UBYTE ClanId, XY Position, UBYTE Reason, UBYTE FlightAirline, SLONG FlightPlaneId, UBYTE FlightPlaneIndex, UBYTE Mood=0, UBYTE FirstClass=0);
      void DoOneCustomerStep (void);
      void DoOnePlayerStep (void);
      void DoOneAnimationStep (void);
      void PersonReachedTarget (void);
      void LookAt (SLONG Dir);

      const CFlugplanEintrag *GetFlugplanEintrag (void);

      BOOL operator > (const PERSON &p) const { SLONG y=ScreenPos.y+FloorOffset, py=p.ScreenPos.y+p.FloorOffset; if ((y<170) == (py<170)) return (y > py); else return (y < py); }
      BOOL operator < (const PERSON &p) const { SLONG y=ScreenPos.y+FloorOffset, py=p.ScreenPos.y+p.FloorOffset; if ((y<170) == (py<170)) return (y < py); else return (y > py); }

   friend TEAKFILE &operator << (TEAKFILE &File, const PERSON &Person);
   friend TEAKFILE &operator >> (TEAKFILE &File, PERSON &Person);

   friend void PumpNetwork (void);

   friend class CAufsicht;
   friend class PERSONS;
   friend class PLAYER;
   friend class PLAYERS;
   friend class CPlane;
   friend class AirportView;
   friend class GameFrame;
   friend class CTakeOffApp;
   friend class CStdRaum;
   friend class CBuero;
   friend class CDutyFree;
   friend class CBuero;
   friend class CMakler;
   friend class CRouteBox;
   friend class SIM;
   friend class AIRPORT;
};

class /**/PERSONS : public ALBUM<PERSON>
{
   public:
      FBUFFER<PERSON> Persons;

   public:
      PERSONS ();
      void  DepthSort (void);
      void  DoOneStep (void);
      ULONG GetPlayerIndex (SLONG Number);
      SLONG GetNumShoppers (void);
      void  TryMoods (void);
      void  RemoveAnimationNear (XY Position);

   friend TEAKFILE &operator << (TEAKFILE &File, const PERSONS &Persons);
   friend TEAKFILE &operator >> (TEAKFILE &File, PERSONS &Persons);
};

//--------------------------------------------------------------------------------------------
//Eine Schlange mit Personen, die erscheinen sollen:
//--------------------------------------------------------------------------------------------
class /**/CQueuedPerson
{
   private:
      SLONG       TimeSlice;        //Dann soll sie erscheinen, (-1=leer)
      SLONG       Priority;         //Zufällige Priorität, falls mehrere Personen erscheinen sollen (durch Netzwerk-Konflikte)

      UBYTE       ClanId;           //Referenziert das Clan-Array
      UBYTE       Reason;           //Der Grund, warum sie am Flughafen ist (z.B. Abflug)
      UBYTE       FlightAirline;    //Die Fluglinie (0-3) der Person..
      ULONG       FlightPlaneId;    //..mit diesem Flugzeug der Linie wird geflogen..
      UBYTE       FlightPlaneIndex; //..und mit diesem Flug (0-6)
      UBYTE       Mood;             //Stimmung
      UBYTE       FirstClass;       //ErsteKlasse Passagier?
      XY          Position;         //Die rechnerische Position (ohne Isometrie)

   public:
      CQueuedPerson () {TimeSlice=-1;} 

   friend TEAKFILE &operator << (TEAKFILE &File, const CQueuedPerson &p);
   friend TEAKFILE &operator >> (TEAKFILE &File, CQueuedPerson &p);

   friend class CPersonQueue;
};

class /**/CQueueSpot
{
   private:
      XY          Position;         //Hier wurde etwas erzeugt
      SLONG       TimeSlice;        //Jetzt zum letzten mal, (-1=nie)

   public:
      CQueueSpot () {TimeSlice=-1;}

   friend TEAKFILE &operator << (TEAKFILE &File, const CQueueSpot &s);
   friend TEAKFILE &operator >> (TEAKFILE &File, CQueueSpot &s);

   friend class CPersonQueue;
};

class /**/CPersonQueue
{
   private:
      FBUFFER<CQueuedPerson>   Person;
      FBUFFER<CQueueSpot>      Spots;

   public:
      void AddPerson (UBYTE ClanId, XY Position, UBYTE Reason, UBYTE FlightAirline, SLONG FlightPlaneId, UBYTE FlightPlaneIndex, UBYTE Mood=0, UBYTE FirstClass=0);
      void SetSpotTime  (XY Position, SLONG TimeSlice);
      void Pump (void);
      void NewDay (void);
      void ResetSpotTimeSlices (void);

   friend TEAKFILE &operator << (TEAKFILE &File, const CPersonQueue &q);
   friend TEAKFILE &operator >> (TEAKFILE &File, CPersonQueue &q);
};

//--------------------------------------------------------------------------------------------
// class History : (in Player.cpp)
//--------------------------------------------------------------------------------------------
class /**/HISTORYLINE //Nein, nicht das von Blue Byte
{
   public:
      BOOL    ByPhone;
      __int64 Money;
      CString Description;

   friend TEAKFILE &operator << (TEAKFILE &File, const HISTORYLINE &h);
   friend TEAKFILE &operator >> (TEAKFILE &File, HISTORYLINE &h);
};

class /**/HISTORY
{
   public:
      HISTORYLINE     HistoryLine[100];
      __int64         HistoricMoney;

   public:
      HISTORY ();
      void ReInit ();
      void AddEntry (__int64 Money, CString Description);
      void AddNewCall (SLONG Type);                         //Bit 0: Handy, Bit 1+2: Ortgespräch, Ferngespräch, Auslandsgespräch
      void AddCallCost (long Money);

   friend TEAKFILE &operator << (TEAKFILE &File, const HISTORY &h);
   friend TEAKFILE &operator >> (TEAKFILE &File, HISTORY &h);
};

class /**/CRobotAction
{
   public:
      SLONG    ActionId;
      UBYTE    TargetRoom;
      SLONG    Par1, Par2;

   friend TEAKFILE &operator << (TEAKFILE &File, const CRobotAction &r) { File<<r.ActionId<<r.TargetRoom<<r.Par1<<r.Par2; return (File); }
   friend TEAKFILE &operator >> (TEAKFILE &File, CRobotAction &r) { File>>r.ActionId>>r.TargetRoom>>r.Par1>>r.Par2; return (File); }
};

//--------------------------------------------------------------------------------------------
// Misc.Cpp:
//--------------------------------------------------------------------------------------------
class /**/CMessage
{
   public:
      SLONG    BeraterTyp;    //-1 = leer
      CString  Message;                     
      SLONG    Urgent;
      SLONG    Mood;
      SLONG    BubbleType;

   friend TEAKFILE &operator << (TEAKFILE &File, const CMessage &Message);
   friend TEAKFILE &operator >> (TEAKFILE &File, CMessage &Message);
};

class /**/CMessages
{
   public:
      FBUFFER<CMessage> Messages;
      CMessage          LastMessage;
      SBBM              SprechblaseBm;
      SLONG             AktuellerBeraterTyp;  // -1 = keiner
      SLONG             AktuelleBeraterBitmap; //0=erster Berater
      SLONG             TalkPhase;
      SLONG             BlinkCountdown;
      SLONG             TalkCountdown;
      SLONG             BeraterPosY;
      SLONG             BeraterWalkState;    // -1=up, 1=down
      BOOL              IsMonolog;           // Bleibt der Berater da?
      BOOL              IsDialog;            // Ist der Spieler unten?
      SLONG             IsDialogTalking;     // Labert der Spieler?
      SLONG             PlayerNum;

   public:
      CMessages ();
      BOOL IsSilent (void);
      void NewDay (void);
      void NoComments (void);
      void AddMessage (SLONG BeraterTyp, CString Message, SLONG Urgent=MESSAGE_NORMAL, SLONG Mood=-1);
      void NextMessage (void);
      void PaintMessage (void);
      void Pump (void);
      void StartDialog (SLONG PlayerNum);
      void StopDialog (void);
      void KillBerater (void);

   friend TEAKFILE &operator << (TEAKFILE &File, const CMessages &Messages);
   friend TEAKFILE &operator >> (TEAKFILE &File, CMessages &Messages);
};

//--------------------------------------------------------------------------------------------
// Personal.Cpp : (Leute die man angestellt hat)
//--------------------------------------------------------------------------------------------
class /**/CWorker
{
   public:
      CString        Name;
      BOOL           Geschlecht;
      SLONG          Typ;
      SLONG          Gehalt, OriginalGehalt;
      SLONG          Talent;
      SLONG          Alter;
      CString        Kommentar;
      SLONG          Employer;      //0-3 oder WORKER_RESERVE, WORKER_JOBLESS
      SLONG          PlaneId;       //Auf diesem Flugzeug wird die Prs ggf. eingesetzt
      SLONG          Happyness;     //-100 bis 100
      BOOL           WarnedToday;   //Heute schon gemeckert?

   public:
      void  Gehaltsaenderung (BOOL Art);

   friend TEAKFILE &operator << (TEAKFILE &File, const CWorker &Worker);
   friend TEAKFILE &operator >> (TEAKFILE &File, CWorker &Worker);
};

class /**/CWorkers
{
   public:
      FBUFFER<CWorker> Workers;
      BUFFER<CString>  FNames;         //Female Names
      BUFFER<CString>  MNames;         //Male Names
      BUFFER<CString>  LNames;         //Last Names

   public:
      CWorkers () {}
      CWorkers (const CString &TabFilename, const CString &TabFilename2);

      CString GetRandomName(BOOL Geschlecht);

      void  CheckShortage (void);
      void  ReInit (const CString &TabFilename, const CString &TabFilename2);
      void  NewDay (void);
      void  Gehaltsaenderung (BOOL Art, SLONG PlayerNum);
      SLONG GetQualityRatio (SLONG c);
      SLONG GetAverageHappyness (SLONG PlayerNum);
      SLONG GetMaxHappyness (SLONG PlayerNum);
      SLONG GetMinHappyness (SLONG PlayerNum);
      void  EnsureBerater (SLONG Typ);
      SLONG GetNumJoblessBerater (void);
      SLONG GetNumJoblessPiloten (void);
      SLONG GetNumJoblessFlugbegleiter (void);
      void  AddHappiness (SLONG PlayerNum, SLONG Value);

   friend TEAKFILE &operator << (TEAKFILE &File, const CWorkers &Workers);
   friend TEAKFILE &operator >> (TEAKFILE &File, CWorkers &Workers);
};

//--------------------------------------------------------------------------------------------
// Stdraum.cpp: Jemand mit dem sich der Spieler unterhalten kann:
//--------------------------------------------------------------------------------------------
class CTalker
{
   public:
      SLONG    OwnNumber;
      SLONG    State;      //0=nix, 1=Gespräch, 2=Telefonat annehmen, 3=Telefonieren, 4=Auflegen
      SLONG    StatePar;   //Fortschritt der Animation
      BOOL     Talking;
      SLONG    Phase;
      SLONG    Locking;    //Computer LOCKen die Figur, wenn sie im Raum sind
      SLONG    NumRef;     //Von wie vielen wird das benutzt

      //Bitmap(s) des Sprechers: Nix tun, zu schauen, sprechen, zu nix tun
      SBBMS    TalkerBms[4];

   public:
      void StartDialog (BOOL Medium);
      void StopDialog (void);
      void StartTalking (void);
      void StopTalking (void);
      void BlitAt (SBBM &Offscreen, XY Pos);
      void IncreaseReference (void);  //Fürs laden/rauswerfen der Bitmap
      void DecreaseReference (void);
      void IncreaseLocking (void);    //Fürs sperren
      void DecreaseLocking (void);
      BOOL IsBusy (void);
      BOOL IsTalking (void);

   friend TEAKFILE &operator << (TEAKFILE &File, const CTalker &Talker);
   friend TEAKFILE &operator >> (TEAKFILE &File, CTalker &Talker);
};

class CTalkers
{
   public:
      FBUFFER<CTalker> Talkers;

   public:
      CTalkers ();
      void Init (void);
      void Pump (void);

   friend TEAKFILE &operator << (TEAKFILE &File, const CTalkers &Talkers);
   friend TEAKFILE &operator >> (TEAKFILE &File, CTalkers &Talkers);
};

//--------------------------------------------------------------------------------------------
// Die Tagesbilanz : (Bank.cpp)
//--------------------------------------------------------------------------------------------
class CBilanz
{
   //Die Habensseite:
   public:
      SLONG    HabenZinsen;
      SLONG    HabenRendite;
      SLONG    Tickets;
      SLONG    Auftraege;

   //Die Sollseite:
   public:
      SLONG    SollZinsen;
      SLONG    SollRendite;
      SLONG    Kerosin;
      SLONG    Personal;
      SLONG    Vertragsstrafen;
      SLONG    Wartung;
      SLONG    Gatemiete;
      SLONG    Citymiete;
      SLONG    Routenmiete;

   public:
      void  Clear(void);
      SLONG GetHaben(void);
      SLONG GetSoll(void);
      SLONG GetSumme(void);

   friend TEAKFILE &operator << (TEAKFILE &File, const CBilanz &Bilanz);
   friend TEAKFILE &operator >> (TEAKFILE &File, CBilanz &Bilanz);
};

typedef struct smk_t* smk;

//--------------------------------------------------------------------------------------------
// Smacker 16-Bit Interface:
//--------------------------------------------------------------------------------------------
class CSmack16
{
   public:
      SDL_Palette* PaletteMapper;          //Tabelle zum Mappen von 8 auf 16 Bit
      smk          pSmack;
      char         State;
      unsigned long Width;
      unsigned long Height;
      DWORD        FrameNext;

   public:
      CSmack16 ();
      ~CSmack16 ();
      void Open (CString Filename);
      BOOL Next (SBBM *pTargetBm);
      void Close (void);
};

class CSmoker
{
   public:
      BUFFER<CSmoke> Smoke;
      SLONG          Smoking;
};

//--------------------------------------------------------------------------------------------
// Smacker 16-Bit Interface für den Flughafen:
//--------------------------------------------------------------------------------------------
class CAirportSmack : public CSmack16
{
   public:
      SBBM         Bitmap;
      SLONG        BrickId;
      XY           Offset;

   public:
      void BlitAt (SBBM *pTargetBm);
};

//--------------------------------------------------------------------------------------------
// Player.Cpp :
//--------------------------------------------------------------------------------------------
class PLAYER
{
   //Generelles:
   public:
      SLONG          PlayerNum;  //Seine Nummer
      BOOL           IsOut;      //Ist der Spieler aus dem Spiel?
      CString        Name;       //Karl Arsch oder Dirk Doof
      CString        NameX;      //ohne Leerstellen
      CString        Airline;    //Name der Fluglinie;
      CString        AirlineX;   //Name der Fluglinie; ohne Leerstellen am Ende
      CString        Abk;        //Kurzname der Fluglinie, z.B. 'LH'
      UBYTE          Owner;      //Spieler=0, Computergegner=1, Netzwerkgegner=2
      ULONG          NetworkID;  //IDs des Spielers im Netzwerk (0=Nicht im Netzwerk)
      UBYTE          Logo;       //Nummer des Logos der Fluglinie
      __int64          Money;      //Seine Barschaft
      __int64          Bonus;      //versteckter Bonus, den der Computerspieler noch erhält
      FBUFFER<__int64> MoneyPast;  //Vergangenheitslinie des Geldes
      __int64          Credit;     //Seine Schulden
      SLONG          Image;      //Firmenimage generell [-1000..1000]
      SLONG          BadKerosin; //Soviel Liter schlechtes Kerosin wurde gekauft
      SLONG          KerosinKind;//Diese Art wird getankt
      SLONG          Tank;       //Soviel kann man auf Reserve Bunkern
      BOOL           TankOpen;   //Tanks sind zur Verwendung freigegeben
      SLONG          TankInhalt; //Soviel ist im Tank drin
      double         TankPreis;  //Korekt berechneter Preis; auch bei mix
      SLONG          GameSpeed;  //0..3
      SLONG          ArabTrust;  //Sabotage möglich?
      SLONG          ArabMode;   //Anschlag unterwegs?
      SLONG          ArabMode2;  //Anschlag unterwegs? Und zwar von der Spieler-Bezogenen Art
      SLONG          ArabMode3;  //Anschlag unterwegs? Und zwar von den Specials
      SLONG          ArabActive; //FALSE=Flugzeug war noch nicht gelandet; TRUE=Flugzeug war am Boden, Anschlag jetzt möglich
      SLONG          ArabOpfer;  //Anschlag auf wen?
      SLONG          ArabOpfer2; //Anschlag auf wen?
      SLONG          ArabOpfer3; //Anschlag auf wen?
      SLONG          ArabPlane;  //Auf welches Flugzeug des Opfers?
      SLONG          ArabHints;  //Wieviele Indizien gibt es, die auf den Spieler hinweisen?
      SLONG          MechMode;   //Wie wird repariert? 0-3
      BOOL           GlobeOiled; //Ist der Globus geölt?
      SLONG          MechTrust;  //0=kein, 1=offen, 2=genehmigt
      SLONG          MechAngry;  //0=nein, 1=wütend
      UWORD          EarthAlpha;
      UBYTE          DisplayRoutes[4];  //Routen bei der Flugplanung anzeigen?
      UBYTE          DisplayPlanes[4];  //Flugzeuge bei der Flugplanung anzeigen?
      SLONG          ReferencePlane;    //Hier wurde zuletzt daran gearbeitet
      BOOL           SickTokay;         //Ist der Spieler heute krank?
      BOOL           RunningToToilet;   //Muß er ganz dringend zur Toilette?
      SLONG          PlayerSmoking;     //Qualmt der Spieler?
      SLONG          PlayerStinking;    //Qualmt der Spieler?
      SLONG          Stunned;           //Kann der Spieler sich bewegen?
      SLONG          OfficeState;       //0=normal, 1=vermint, 2=explodiert, 3=dunkel
      CSmack16      *pSmack;            //Video für die Explosion
      BUFFER<CSmoke> Smoke;
      SLONG          LaptopVirus;       //0=kein Virus, 1=Virus, 2=In Reparatur, 3=abholbereit
      SLONG          WerbeBroschuere;   //-1=keine Broschüre, sonst die vom Spieler
      BOOL           TelephoneDown;     //0=Telefone okay, 1=Telefon ausgefallen
      BOOL           Presseerklaerung;  //1=Ist heute Opfer einer Presseerklärung
      BOOL           GlobeFileOpen;     //Ist das Filofax im Globus offen?
      BOOL           HasAlkohol;        //Alkohol im Glas im Büro drin?
      SLONG          Koffein;           //Ist der Spieler high auf Kerosin
      SLONG          IsDrunk;           //Wieviel Promille hat der Spieler intus?
      ULONG          SecurityFlags;     //Welche Security-Programme hat der Spieler aboniert
      ULONG          SecurityNeeded;    //Welche Security-Programme hat der Spieler aboniert

      SLONG          StrikeHours;       //Soviele Stunden, wollen die Piloten noch streiken
      SLONG          StrikeNotified;    //TRUE, wenn Spieler schon angefaxt wurde
      SLONG          StrikePlanned;     //TRUE ==> Worker gehen zum nächsten fairen Termin auf Streik
      SLONG          StrikeEndCountdown;//In soviel Minuten wird der Streik jetzt beendet
      SLONG          StrikeEndType;     //Und zwar auf die Art: 1=Drohung; 2=Gehaltserhöhung; 3=Trinker; 4=Abwarten
      SLONG          DaysWithoutStrike; //Soviele Tage geht es schon gut
      SLONG          DaysWithoutSabotage; //Soviele Tage geht es schon gut
      SLONG          TrinkerTrust;      //Beim Trinker eingeschleimt?
      SLONG          SeligTrust;        //Bei Fräulein Selig eingeschleimt?
      SLONG          SpiderTrust;       //Beim Araber eingeschleimt?
      SLONG          WerbungTrust;      //Beim Besitzer der Werbeagentur eingeschleimt?
      SLONG          DutyTrust;         //Bei Frau im Duyt-Free Laden eingeschleimt?
      SLONG          KioskTrust;        //Beim Kioskbesitzer eingeschleimt?

      BOOL           bReadyForMorning;  //Ist Spieler bereit (wichtig für Netzwerk)
      BOOL           bReadyForBriefing; //Ist Spieler an Folgetagen bereit für's Briefing

   //Statistik/Spielziel:
   public:
      BOOL           HasFlownRoutes;    //Flag, ob er schon einmal eine Route geflogen ist
      SLONG          NumAuftraege;      //Zahl der geflogenen Aufträge
      SLONG          NumPassengers;     //Zahl der Passagieren, die man befördert hat
      __int64        Gewinn;
      SLONG          ConnectFlags;      //Flags für Städte, die verbunden wurden
      SLONG          RocketFlags;       //Flags für die Raketenbauteile
      SLONG          LastRocketFlags;   //Vom Tag davor
      SLONG          NumFracht;         //Soviele tonnnen wurden bisher transportiert
      SLONG          NumFrachtFree;     //Soviele tonnnen wurden bisher gratis transportiert
      SLONG          NumMiles;          //Soviele meilen sind wir schon geflogen
      SLONG          NumServicePoints;  //Soviele meilen sind wir schon geflogen
      SLONG          NumOrderFlights;   //Soviele von Uhrigs Aufträgen ist man geflogen
      SLONG          NumOrderFlightsToday;   //Soviele von Uhrigs Aufträgen ist man heute geflogen
      SLONG          NumOrderFlightsToday2;  //Soviele von Uhrigs Aufträgen hat der Computer heute angenommen

      FBUFFER<SValue> Statistiken;      //Die diversen Statistiken

   //Größere Daten:
   public:
      CPlanes        Planes;     //Flugzeuge, die der Spieler besitzt
      CAuftraege     Auftraege;  //Verträge die er für Flüge abgeschlossen hat
      CFrachten      Frachten;   //Verträge die er für Flüge abgeschlossen hat
      CGates         Gates;      //Die Gates (immer CheckIn + Abflug) die gemietet wurden
      FBUFFER<UBYTE> Items;      //Dinge die der Spieler besitzt
      SLONG          LaptopBattery;
      SLONG          LaptopQuality;
      CRentCities    RentCities; //Die eröffneten Niederlassungen
      FBUFFER<UBYTE> CalledCities; //TRUE=Man hat schon mit ihnen telefoniert
      CRentRouten    RentRouten; //Diese Routen haben wir gemietet
      CPoint         CursorPos;  //Cursor im 640x480-Bereich
      HISTORY        History;    //Alte Kontobewegungen
      BLOCKS         Blocks;     //Die Notizblöcke
      CMessages      Messages;   //Die Berater
      CLetters       Letters;    //Die Briefe im Postkorb

   //Aktien & Zinsen:
   public:
      UBYTE          SollZins;   //Zins für Schulden
      UBYTE          HabenZins;  //Zins für Guthaben
      CBilanz        Bilanz, BilanzGestern;
      SLONG          AnzAktien;  //Zahl der emmitierten Aktien
      SLONG          MaxAktien;  //Zahl der emmitierbaren Aktien
      SLONG          OwnsAktien[4]; //Soviele Aktien besitzt der Spieler jeweils von der Sorte
      __int64        AktienWert[4]; //Soviele waren die Aktien beim Kauf jeweils Wert
      double         Kurse[10];  //Die letzten 10 Kurse
      SLONG          Dividende;  //Die zuletzt ausgezahlte Dividende
      SLONG          TrustedDividende;  //Das Vertrauen der Anleger

   //Interne Verwaltung:
   public:
      XY             ViewPos;    //SichtPosition im Flughafen
      XY             IslandViewPos; //SichtPosition für die Inseln
      XY             CameraSpeed;   //Trägheitssteuerung. Dies ist die Geschw. der Kamera
      CStdRaum      *LocationWin;//Pointer auf offenes Sub-Fenster (Raum)
      CStdRaum      *DialogWin;  //Pointer auf offenes Dialog-Sub-Fenster
      UBYTE          NewDir;     //Flag, falls die Laufrichtung geändert wurde
      XY             WinP1;      //Die Position des ViewFensters im GameFrame Fenster
      XY             WinP2;
      UWORD          Locations[10];//0=kein, 1=Flughafen; 2-? = Raum
      SLONG          LocationTime; //Seit diesem Sim.Time sind wir in dieser Location (oder -1 für Flughafen)
      SLONG          LocationForbidden; //Diese Location ist verboten, weil man zu lange dort war
      SLONG          LocationForbiddenTime; //Und zwar um diese Uhrzeit
      UWORD          TopLocation;
      BYTE           Windows;    //Bits 1, 2, 4, 8 für die belegten Ausschnitte
      CPoint         Position;   //Position im Flughafen
      BYTE           Buttons;    //Button 1&2 in 0001b und 0010b
      SLONG          xPiloten;   //So viele sind zu viel (zu wenig)
      SLONG          xBegleiter; //So viele sind zu viel (zu wenig)
      BOOL           CallItADay; //Feierabend für heute?
      SLONG          NumFlights; //So viele Flüge hat er durchgeführt
      SLONG          RoutePage;  //Seite in RouteBox
      SLONG          StandCount; //Solange steht der Spieler schon dumm an dieser Stelle herum
      BOOL           TalkedToNasa;     //Nasa-Mann ist nur einmal ausdringlich

   //Laufen:
   public:
      BOOL           iWalkActive;
      BOOL           WalkToGlobe;      //Spieler geht zum Globus
      XY             PrimaryTarget;    //Hierhin wollen wir
      XY             SecondaryTarget;  //für Raumeingänge
      XY             TertiaryTarget;   //für Treppen
      FBUFFER<BOOL>  WasInRoom;        //War der Spieler schon im Raum?
      SLONG          WalkSpeed;        //So schnell sind wir
      UWORD          WaitForRoom;      //0=kein
      UWORD          ThrownOutOfRoom;  //0=kein
      SLONG          DirectToRoom;     //Wird auf einen Raum gezielt?
      SLONG          ExRoom;           //Aus dem Raum kommen wir gerade
      SLONG          IsStuck;          //0 oder Zahl der Sekunden

   //Computerspieler
   public:
      BOOL           ImageGotWorse;    //Für's Briefing
      FBUFFER<CRobotAction> RobotActions;
      SLONG          StandStillSince;  //TimeOut
      SLONG          LastActionId;
      SLONG          WorkCountdown; //Computer "macht" etwas
      SLONG          WaitWorkTill;  //Timeslice-Angabe, wann der Computerspieler seine Aktion beginnen soll; Wichtig, damit im Netzwerk die Aktionen absolut synchron ausgeführt werden
      ULONG          WaitWorkTill2; //Sim.Time-Angabe, nur zweitrangig für die Synchronisierung
      SLONG          TimeBuro;      //Wann war der Computerspieler zuletzt
      SLONG          TimePersonal;  //im Büro, beim Reisebüro, ö.ä.
      SLONG          TimeAufsicht;  //Zeit seit letzen Besuch
      SLONG          TimeReiseburo; //Zeit seit letzen Besuch
      SLONG          GefallenRatio; //Wer schuldet wem einen gefallen?
      FBUFFER<SLONG> Sympathie;     //Sympatiewerte für die Spieler
      FBUFFER<SLONG> Kooperation;   //Arbeiten die Spieler zusammen und teilen die Routen?
      SLONG          DoRoutes;      //Auf Routen umsteigen?
      SLONG          WantToDoRoutes;//Plant er umzusteigen?
      SLONG          OutOfGates;    //Mußte gepatcht werden?
      BOOL           SavesForPlane; //Spart auf ein Flugzeug
      SLONG          BuyBigPlane;   //Wenn != 0 dann brauchen wir unbedingt ein Flugzeug mit mindestens dieser Reichweite
      BOOL           SavesForRocket;//Spart auf ein Raketenteil
      SLONG          PlayerDialog;  //Spricht er mit einem Spieler
      SLONG          PlayerDialogState;  //-1 = kein Dialog, oder im Dialog; 0-3 Dialog mit Spieler x wird aufgebaut
      bool           bDialogStartSent;   //Wurde ATNET_DIALOG_START schon verschickt?
      SLONG          IsWalking2Player;   //Geht er auf einen anderen Spieler zu? (-1 = nein)
      BOOL           IsTalking;     //Redet er mit einem anderen Spieler
      SLONG          CalledPlayer;  //Heute schon den Spieler angerufen?
      BOOL           BoredOfPlayer; //Ist heute schon TimeOut beim Spieler aufgetreten?
      SLONG          SpeedCount;    //Trick für den Rundenwechsel zu beschleunigen
      BOOL           bWasInMuseumToday;
      TEAKRAND       PlayerWalkRandom;
      TEAKRAND       PlayerExtraRandom;

   public:
      PLAYER ();
      ~PLAYER ();
      void  Add5UhrigFlights (void);
      SLONG AnzPlanesOnRoute (ULONG RouteId);
      void  BookBuroRent (void);          //Zieht Miete vom Geld ab
      void  BookSalary (void);            //Zieht Gehalt vom Geld ab
      void  BroadcastPosition (bool bForce=false);
      void  BroadcastRooms (SLONG Message, SLONG RoomLeft=-1);
      void  BuyPlane (CXPlane &plane, TEAKRAND *pRnd);
      void  BuyPlane (ULONG PlaneTypeId, TEAKRAND *pRnd);
      void  BuyItem (UBYTE Item);
      long  CalcSecurityCosts (bool bFixOnly=false, bool bPlaneOnly=false);
      void  DisplayAsTelefoning (void);
      bool  DropItem (UBYTE Item);
      long  CalcCreditLimit (void);
      void  CalcRoom (void);              //Speed-up für GetRoom
      SLONG CalcPlanePropSum (void);      //Berechnet, was die anstehenden Umrüstungen zusammen kosten werden
      void  ChangeMoney (__int64 Money, SLONG Reason, CString Par1, char *Par2=NULL);  //Ändert Barschaft und Profit
      void  CheckAuftragsBerater (const CAuftrag &Auftrag); //in Auftrag.cpp
      void  CheckAuftragsBerater (const CFracht &Fracht);   //in Fracht.cpp
      void  DelayFlightsIfNecessary (void);
      void  DoBodyguardRabatt (SLONG Money);
      void  EnterRoom (SLONG RoomNum, bool bDontBroadcast=false);
	  void  AddRocketPart(SLONG rocketPart, SLONG price);
      UWORD GetRoom (void);                         //Gibt den aktuellen Raum zurück
      SLONG GetMissionRating (bool bAnderer=false); //Gibt aktuellen Missionserfolg als Zahl zurück
      SLONG HasBerater (SLONG Berater);
      BOOL  HasBeraterApplied (SLONG Berater);
      BOOL  HasItem (UBYTE Item);
      BOOL  HasSpaceForItem (void);
      BOOL  HasWon (void);
      BOOL  IsLocationInQueue (UWORD Location);
      BOOL  IsClosedLocationInQueue (void);
      BOOL  IsAuftragInUse (SLONG AuftragsId, CString *PlaneName=NULL);
      BOOL  IsOkayToCallThisPlayer (void);
      void  LeaveRoom (void);             //Verläßt der Raum
      void  LeaveAllRooms (void);         //Verläßt alle Räume
      void  MapWorkers (BOOL Advice);     //Verknüpft worker mit Flugzeugen
      void  MapWorkerOverflow (BOOL Advice);
      void  UpdatePilotCount (void);
      void  NewDay (void);                //Läßt neuen Tag & ggf. Monat beginnen
      void  PlanGates (void);
      void  RandomBeraterMessage (void);
      void  ReformIcons (void);
      void  RentRoute (SLONG City1, SLONG City2, SLONG Miete);
      void  RentGate (SLONG Nummer, SLONG Miete);
      void  RobotPump(void);
      void  RobotPlanRoutes(void);
      void  RobotPlan(void);
      void  RobotInit(void);
      void  RobotExecuteAction(void);
      void  RouteWegnehmen (long Routenindex, long NeuerBesitzer=-1);
      void  SackWorkers (void);
      void  UpdateAuftragsUsage (void);
      void  UpdateFrachtauftragsUsage (void);
      void  UpdateWalkSpeed (void);
      void  UpdateWaypoints (void);
      void  UpdateWaypointWalkingDirection (void);
      void  UpdateAuftraege (void);       //Bringt Aufträge aus neusten Stand; bucht Strafen
      void  UpdatePersonalberater (SLONG Toleranz);
      void  UpdateStatistics (void);
      void  UpdateTicketpreise (SLONG RouteId, SLONG Ticketpreis, SLONG TicketpreisFC);
      BOOL  WalkToRoom (UBYTE RoomId);
      void  WalkToMouseClick (XY AirportClickPos);
      void  WalkToPlate (XY Plate);
      void  WalkStop (void);
      void  WalkStopEx (void);

   public: //Network-Sachen:
      SLONG NetSynchronizeGetNum (void);
      void  NetSynchronizeImage (void);
      void  NetSynchronizeMoney (void);
      void  NetSynchronizeRoutes (void);
      void  NetSynchronizeFlags (void);
      void  NetSynchronizeItems (void);
      void  NetSynchronizeSabotage (void);
      void  NetSynchronizeKooperation (void);
      void  NetRouteUpdateTicketpreise (SLONG RouteId, SLONG Ticketpreis, SLONG TicketpreisFC);
      void  NetUpdateFlightplan (SLONG Plane);
      void  NetUpdateOrder (const CAuftrag &auftrag);
      void  NetUpdatePlaneProps (SLONG Plane=-1);
      void  NetUpdateFreightOrder (const CFracht &auftrag);
      void  NetUpdateTook (SLONG Type, SLONG Index, SLONG City=0);
      void  NetUpdateRentRoute (SLONG Route1Id, SLONG Route2Id);
      void  NetUpdateWorkers (void);
      void  NetUpdateKerosin (void);
      void  NetSynchronizePlanes (void);
      void  NetSynchronizeMeeting (void);
      void  NetBuyXPlane (long Anzahl, CXPlane &plane);
      void  NetSave (DWORD UniqueGameId, SLONG CursorY, CString Name);

   friend TEAKFILE &operator << (TEAKFILE &File, const PLAYER &Player);
   friend TEAKFILE &operator >> (TEAKFILE &File, PLAYER &Player);
};

class PLAYERS
{
   public:
      SLONG           AnzPlayers;
      FBUFFER<PLAYER> Players;
      
   public:
      PLAYERS ();
      SLONG GetAnzHumanPlayers (void);
      SLONG GetAnzRobotPlayers (void);
      SLONG GetIndexOfHumanPlayerNumberX (SLONG x);
      BOOL  IsLogoInUse (SLONG Player, UBYTE Logo);
      BOOL  IsPlaneNameInUse (const CString &PlaneName);
      void  WalkToStartingPoint (void);
      void  RobotPump(void);
      void  RobotInit(void);
      void  BeraterPump(void);
      void  MessagePump(void);
      void  RandomBeraterMessage (void);
      void  UpdateStatistics (void);
      void  CheckFlighplans (void);

   friend TEAKFILE &operator << (TEAKFILE &File, const PLAYERS &Players);
   friend TEAKFILE &operator >> (TEAKFILE &File, PLAYERS &Players);
};

//--------------------------------------------------------------------------------------------
// Hieraus besteht der Flughafen:
//--------------------------------------------------------------------------------------------
class AIRPORT
{
   //Elemente des Flughafens:
   public:
      BUILDS          Builds;
      FBUFFER<BUILDS> HashBuilds;
      SLONG           LeftEnd, RightEnd;
      FBUFFER<SLONG>  GateMapper;
      SLONG           NumBeltSpots;  //Zahl der Stehplätze beim Gepäckband

   //Die Spezialinformationen (Positionsidiciert):
   public:
      XY             PlateOffset;         //Verschiebt die Build-Koordinaten in Plate-Koor.
      XY             PlateDimension;      //Maße des Arrays
      FBUFFER<UBYTE> iPlate;              //XY-Platten mit den Informationen 
      FBUFFER<UBYTE> iPlateDir;           //Kopie mit der Richtung einer Person, die dort läuft
      FBUFFER<UWORD> SeatsTaken;          //X-Array, in Y-Achse die Sitze

   //Koordinaten von Sonderfällen (Nummerierungsindiciert):
   public:
      FBUFFER<BUILD> Runes;               //Die Runen, die alles markieren
      FBUFFER<CDoor> Doors;
      FBUFFER<CDoor> Triggers;
      FBUFFER<CAreaMarker> AreaMarkers;
      FBUFFER<CClipMarker> ClipMarkers;

   private:
      //Die Bauteile des Flughafens:
      SLONG  HallNum;        //Diese Halle (oder evtl. alle) ist geladen
      SLONG  HallLevel[10];  //(ggf) die Ausbaustufe(n) der Halle

   public:
      BOOL   DoesRuneExist (ULONG BrickId, UBYTE Par);
      XY     GetNextWaypointRune (UBYTE StartingWaypoint, UBYTE *CurrentWaypoint, ULONG *Gimmick);
      XY     GetRandomBirthplace (BOOL Type, SLONG PlayerNum=0, TEAKRAND *pRand=NULL);
      XY     GetRandomExit (TEAKRAND *pRand=NULL);
      void   GetRandomShop (XY &ReturnPosition, SLONG &ReturnStatePar, SLONG ClanType, UBYTE *Mood, TEAKRAND *pRand=NULL);
      XY     GetRandomTypedRune (ULONG BrickId, UBYTE Par=0, bool AcceppError=false, TEAKRAND *pRand=NULL);
      XY     GetBestStairs (UBYTE Par, SLONG x1, SLONG x2);
      SLONG  GetNumberOfFreeGates (void);
      SLONG  GetNumberOfShops (ULONG BrickId);
      UBYTE  GetRuneParNear (const XY &Pos, const XY &MaxDist, ULONG RuneType);
      BUILD *GetBuildNear (const XY &Pos, const XY &MaxDist, ULONG RuneType);
      SLONG  IsInMarkedArea (const XY &Pos);
      SLONG  CalcPlateXPosition (SLONG BuildIndex, SLONG BrickXOffset, SLONG Alignment);
      SLONG  CalcPlateYPosition (SLONG BuildIndex, SLONG BrickYOffset);
      SLONG  CalcPlateXPosition (BUILD &qBuild, SLONG BrickXOffset, SLONG Alignment);
      SLONG  CalcPlateYPosition (BUILD &qBuild, SLONG BrickYOffset);
      void   CalcPlates (void);
      void   CreateGateMapper (void);
      void   CalcCoordinates (void);
      void   CalcSeats (void);
      void   DoHashBuilds (void);
      void   PumpDoors (void);
      void   TryDoor (XY ArrayPos, BOOL Player, SLONG PlayerNum);
      void   RemoveRunes (void);
      char  *GetHallFilename (void);
      void   NewDay (void);
      void   UnassociateBuilds (void);
      void   Load (SLONG Hall, SLONG Level);
      void   Save (void) const;
      void   LoadAirport (SLONG LeftEnd, SLONG CheckIn, SLONG Office, SLONG Entry, SLONG Shops, SLONG Cafe, SLONG Security, SLONG Suitcase, SLONG WaitZone, SLONG RightEnd);
      void   RepaintTextBricks (void);
      void   SetConditionBlock (SLONG Id, BOOL Blocking);
      void   UpdateStaticDoorImage (void);

	friend class HallDiskMenu;
   friend TEAKFILE &operator << (TEAKFILE &File, const AIRPORT &Airport);
   friend TEAKFILE &operator >> (TEAKFILE &File, AIRPORT &Airport);
};          

//--------------------------------------------------------------------------------------------
// Sim.Cpp :
//--------------------------------------------------------------------------------------------
class COptions
{
   //Die Optionen:
   public:
      SLONG       OptionFullscreen;
      BOOL        OptionKeepAspectRatio;
      BOOL        OptionPlanes;
      BOOL        OptionPassengers;
      SLONG       OptionMusicType;
      BOOL        OptionEnableDigi;
      SLONG       OptionMusik;
      SLONG       OptionMasterVolume;
      SLONG       OptionLoopMusik;
      BOOL        OptionEffekte;
      BOOL        OptionAmbiente;
      BOOL        OptionDurchsagen;
      BOOL        OptionTalking;
      BOOL        OptionPlaneVolume;
      BOOL        OptionGirl;
      BOOL        OptionBerater;
      BOOL        OptionBriefBriefing;
      BOOL        OptionRealKuerzel;
      SLONG       OptionAirport;
      BOOL        OptionBlenden;
      BOOL        OptionTransparenz;
      BOOL        OptionSchatten;
      BOOL        OptionThinkBubbles;
      BOOL        OptionFlipping;
      BOOL        OptionDigiSound;
      BOOL        OptionAutosave;
      BOOL        OptionFax;
      BOOL        OptionRoundNumber;
      BOOL        OptionSpeechBubble;
      BOOL        OptionRandomStartday;
      CString     OptionPlayerNames[4];
      CString     OptionAirlineNames[4];
      CString     OptionAirlineAbk[4];

      BOOL        OptionViewedIntro;
      SLONG       OptionRoomDescription;
      SLONG       OptionLastPlayer;
      SLONG       OptionLastMission;
      SLONG       OptionLastMission2;
      SLONG       OptionLastMission3;
      SLONG       OptionLastProvider;

   public:
      void  ReadOptions (void);
      void  WriteOptions (void);
};

class CSabotageAct
{
   public:
      SLONG       Player;
      SLONG       ArabMode;
      SLONG       Opfer;

   friend TEAKFILE &operator << (TEAKFILE &File, const CSabotageAct &SabotageAct);
   friend TEAKFILE &operator >> (TEAKFILE &File, CSabotageAct &SabotageAct);
};

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
class CHighscore
{
   public:
      CHighscore () { Name="---"; Score=0; }

   public:
      CString Name;
      DWORD   UniqueGameId2;
      __int64 Score;
};

//--------------------------------------------------------------------------------------------
// Sim.Cpp :
//--------------------------------------------------------------------------------------------
class SIM //Die Simulationswelt; alles was zur aktuellen Partie gehört
{
   //Daten:
   public:
      PERSONS        Persons;    //Die rumlaufenden Kunden & Leute
      CPersonQueue   PersonQueue;//Leute, die erscheinen, kommen erst in eine Warteschlange
      PLAYERS        Players;    //Die agierenden Personen
      CPlanes        UsedPlanes; //Flugzeuge, die zum Verkauf (gebraucht) anstehen
      HEADLINES      Headlines;  //Die aktuellen Schlagzeilen
      SLONG          Kerosin;    //Preis pro Liter
      FBUFFER<SLONG> KerosinPast;
      SLONG          localPlayer;//Welcher Spieler sitzt vorm Rechner?
      SLONG          FocusPerson;//Ihn zeigt die Kamera; normalerweise -1

      SLONG          TicksPlayed; //Solange (in 20stel Sekunden) spielt der Spieler schon
      SLONG          NumRestored; //Sooft wurde diese Partie geladen/gespeichert
      SLONG          TicksSinceLoad; //Solange (in 20stel Sekunden) spielt der Spieler schon seit dem letzten Load
      SLONG          bExeChanged; //Wurde die Exe verändert?
      DWORD          UniqueGameId;//Identifiziert eine Session eindeutig
      DWORD          UniqueGameId2;//Identifiziert eine Session eindeutig, ist auch ohne Netzwerk verfügbar und wird im Spiel nicht verändert (ist aber im Netzwerk nicht synchron)

      FBUFFER<CSabotageAct> SabotageActs;

      CHighscore     Highscores[6];

   //Flags & Co.:
   public:
      BOOL        bNetwork;           //Are we playing by Network rules?
      BOOL        bIsHost;            //Is this computer the network-host?
      CString     SessionName;        //The name of the current Session

      BOOL        bCheatedSession;    //Wenn der Spieler cheatet, gibt's keine Highscore
      BOOL        bReloadAirport;     //Reload the Airport over night because of the update
      BOOL        bWatchForReady;     //Auf die ReadyForMorning Flags achten?
      ULONG       DialogOvertureFlags;//Flags, für diverse Dialoge
      BOOL        ExpandAirport;      //Flughafen bald erweitern?
      SLONG       LastExpansionDate;  //Wurde zuletzt erweitert am...
      UBYTE       Gamestate;          //Schon initialisiert? Oder wird schon gespielt?
      SBYTE       Difficulty;         //Schwierigkeitsgrad
      BOOL        IsTutorial;         //Läuft das Tutorial (mit der Sekretärin noch?)
      SLONG       MaxDifficulty;      //Maximaler Schwierigkeitsgrad
      SLONG       MaxDifficulty2;     //Maximaler Schwierigkeitsgrad für Add-On
      SLONG       MaxDifficulty3;     //Maximaler Schwierigkeitsgrad für Add-On Fligh Security
      ULONG       GameSpeed;          //Time+=Gamespeed
      BOOL        bPause;             //Spiel im Pause-Modus
      BOOL        bNoTime;            //Zeit wird nicht weitergezählt (Briefing)
      BOOL        DayState;           //1=Tag wird gestartet; 2=Spieler spielt; 3=Tag wird beendet; 4=Tag wurde beendet
      BOOL        CallItADay;
      SLONG       CallItADayAt;        //Netzwerk: Feierabend bei dieser TimeSlice einläuten
      BOOL        AnyPersonsInAirport; //Speedup: Ist jemand da?
      SLONG       Tutorial;            //Soweit ist das Tutorium
      SLONG       LaptopSoldTo;        //An wen (oder -1) wurde heute der Laptop verkauft?
      SLONG       MoneyInBankTrash;    //Liegt bei der Bank Geld im Mülleimer  (-1=noch nicht, 1=ja, 0=nein, weil weg
      SLONG       Slimed;              //Werkstatt verschleimt?
      BOOL        UsedTelescope;
      BOOL        UsedPlaneProp2;
      BOOL        DontDisplayPlayer;
      BOOL        bAllowCheating;      //Ist cheaten im Netzwerk erlaubt?
      SLONG       ShowExtrablatt;
      BOOL        ItemGlove;           //Hat heute schon jemand die Handschuhe genommen?
      BOOL        ItemClips;           //Hat heute schon jemand die Büroklammern genommen?
      SLONG       ItemGlue;            //Hat heute schon jemand den Kleber organisiert?
      BOOL        ItemPostcard;        //Hat heute schon jemand die Postkarte genommen?
      BOOL        ItemKohle;           //Hat heute schon jemand die Holzkohle genommen?
      BOOL        ItemParfuem;         //Hat heute schon jemand das Parfüm genommen?
      BOOL        ItemZange;           //Hat heute schon jemand die Zange genommen?
      SLONG       nSecOutDays;         //0 oder n=So viele Tage fällt das Security Office noch aus

      BOOL        b18Uhr;              //ab 18 Uhr wird im Netzwerk nicht mehr synchronisiert
      BOOL        bCompatibleRoutes;   //True if this game uses (old) compatible Routes (some may be double)

      ULONG       KeyHints[3];         //Hilfe-texte für die Tasten: [0]=einmal [1]=heute [2]=zum zweiten Mal
      bool        bThisIsSessionMaster;//Ist dies der Server?

      FBUFFER<SLONG> MissionCities;    //Die Zielstädte für die Missionen
      SLONG          KrisenCity;       //Id der Stadt, wo das Erdbeben ist
      long           ProtectionState;  //0: unchecked, 1=check & okay, -x Countdown till quit

   //Referenzflugzeug:
   public:
      SLONG       PlanePropId;
      SLONG       RFActivePage;
      float       RFBegleiterFaktor;        //Zahl der Stewardessen relativ zum normalen
      //float       RFPutzFaktor;             //Zahl der Putzen relativ zum normalen

      UBYTE       RFSitzeRF;
      UBYTE       RFEssen;
      UBYTE       RFTabletts;
      UBYTE       RFDeco;
      UBYTE       RFTriebwerk;
      UBYTE       RFReifen;
      UBYTE       RFElektronik;
      UBYTE       RFSicherheit;

   //Statistik:
   public:
	   bool		   StatfGraphVisible;							// true -> Der Graph ist sichtbar, ansonsten die schnöden Zahlen
	   bool		   StatplayerMask[4];							// Diese Spieler wurden zur Ansicht ausgewählt
	   BYTE		   Statgroup;										// Die angewählte Gruppe (*0=Finanzen, 1=?, 2=?)
	   SLONG	   Statdays;										// Anzahl der darzustellenden Tage
	   SLONG	   StatnewDays;									// Für eine Animation
	   bool		   StatiArray[3][16];		               // Merkt sich für jede Gruppe welche Einträge selektiert sind.
      SLONG       DropDownPosY;

   //Datum und Zeit:
   public:
      CCodedVar<SLONG,0x7ab8077f,0x6c931a93> Date;    //Tage seit Spielbeginn
      ULONG       Time;          //Die Simulationszeit
      SLONG       Month, MonthDay; //Zur Vereinfachung gespeichert
      UBYTE       QuitCountDown; //Zähler, die lange die Leute bis zum Quit winken
      DWORD       TickerTime;    //Zähler für die AnimBricks
      SLONG       TimeSlice;     //Für die Synchronisation im Netzwerk
      time_t      StartTime;     //Zu diesem Zeitpunkt wurde die Parie gestartet
      SLONG       StartWeekday;  //An diesem Wochentag haben wir das Spiel begonnen
      SLONG       Weekday;       //0-6 für Mo-So
      SLONG       Jahreszeit;    //0-3 = Frühling, Sommer, Herbst, Winter geladen, -1 noch keine Jahrezeit geladen

   //Eine eventuelle Aktienübernahme:
      SLONG       OvertakenAirline;   //Konkurrenzlinie schlucken?
      SLONG       OvertakerAirline;   //Konkurrenzlinie schlucken?
      SLONG       Overtake;           //0=Nix, 1=Schlucken, 2=Auflösen

   //Sonstiges:
      SLONG       TickReisebueroRefill;  //Wird alle 5 Sekunden erhöht und gibt an, wieviel max. gefüllt werden darf
      SLONG       TickLastMinuteRefill;
      SLONG       TickFrachtRefill;
      SLONG       TickMuseumRefill;

   //Die Ausbauzustände des Flughafens:
   public:
      SLONG       LeftEnd, CheckIn, Office, Entry, Shops, Cafe, Security, Suitcase, WaitZone, RightEnd;

   //Sonstiges:
   public:
      SLONG          HomeAirportId; //Id der Heimatstadt
      COptions       Options;
      FBUFFER<UBYTE> RoomBusy;
      FBUFFER<CAirportSmack> AirportSmacks;

   public:
      SIM();
      ~SIM();
      void    AddSmacker (CString Filename, long BrickId, XY Offset);
      void    AddNewPassengers (void);
      void    AddNewShoppers (void);
      void    AddStenchSabotage (XY Position);
      bool    AddGlueSabotage (XY Position, SLONG Dir, SLONG NewDir, SLONG Phase);
      void    ChooseStartup (BOOL GameModeQuick);
      void    DoTimeStep (void);
      SLONG   GetWeek (void);
      SLONG   GetHour (void);
      SLONG   GetMinute (void);
      SLONG   GetWeekday (void);
      CString GetTimeString (void);
      void    NewDay (void);
      void    CreateRandomUsedPlane (SLONG Index);
      void    CreateRandomUsedPlanes (void);
      void    UpdateUsedPlanes (void);
      void    ReformGates (void);
      void    ComputerOnToilet (SLONG ComputerPlayer);
      void    UpdateRoomUsage (void);

      void    CreateMissionCities (void);
      void    CreateEarthquakeCity (void);

      BOOL    LoadGame (SLONG Number);
      void    SaveGame (SLONG Number, const CString &Name);
      DWORD   GetSavegameUniqueGameId (SLONG Index, bool bForceNetwork=false);
      SLONG   GetSavegameLocalPlayer (SLONG Index);
      SLONG   GetSavegameNumHumans (SLONG Index);
      CString GetSavegameSessionName (SLONG Index);
      void    ReadSavegameOwners (SLONG Index);

      void    LoadOptions (void);
      void    SaveOptions (void);

      SLONG   GetSeason (void);
      void    UpdateSeason (void);

      void    GiveHint (SLONG Number);
      void    InvalidateHint (SLONG Number);

      void    NetRefill (SLONG Type, SLONG City=0);
      void    NetSynchronizeOvertake (void);

      //In NewgamePopup.cpp
      bool	  SendMemFile (TEAKFILE &file, ULONG target = NULL, bool useCompression = true);
      bool	  ReceiveMemFile (TEAKFILE &file);
      bool    SendSimpleMessage (ULONG Message, ULONG target=NULL);
      bool    SendSimpleMessage (ULONG Message, ULONG target, SLONG Par1);
      bool    SendSimpleMessage (ULONG Message, ULONG target, SLONG Par1, SLONG Par2);
      bool    SendSimpleMessage (ULONG Message, ULONG target, SLONG Par1, SLONG Par2, SLONG Par3);
      bool    SendSimpleMessage (ULONG Message, ULONG target, SLONG Par1, SLONG Par2, SLONG Par3, SLONG Par4);
      bool    SendSimpleMessage (ULONG Message, ULONG target, SLONG Par1, SLONG Par2, SLONG Par3, SLONG Par4, SLONG Par5);
      bool    SendSimpleMessage (ULONG Message, ULONG target, SLONG Par1, SLONG Par2, SLONG Par3, SLONG Par4, SLONG Par5, SLONG Par6);
      bool    SendChatBroadcast (CString Message, bool bSayFromWhom=false, ULONG target=NULL);

      void    AddHighscore (CString Name, DWORD UniqueGameId2, __int64 Score);
      void    SaveHighscores (void);
      void    LoadHighscores (void);

   friend TEAKFILE &operator << (TEAKFILE &File, const SIM &Sim);
   friend TEAKFILE &operator >> (TEAKFILE &File, SIM &Sim);
};