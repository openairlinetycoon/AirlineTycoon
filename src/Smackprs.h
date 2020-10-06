//============================================================================================
//SmackPrs.h - Wrapper-Klasse für den Smacker für 16Bit Farben bei 8-Bit Videos:
//============================================================================================

//--------------------------------------------------------------------------------------------
//Eine einfache Animation mit dem Smacker:
//--------------------------------------------------------------------------------------------
/*class CSmackAnim
{
   private:
      Smack   *pSmack;
      BOOL     IsActive;

   public:
      CSmackAnim ();
      ~CSmackAnim ();
      void Reset (void);
      void ReSize (const CString &Filename,
                   SBFX *Fx,
                   BOOL  StartPic,
                   SLONG Mode,
                   SLONG Prelude,
                   SLONG Speed,
                   SLONG CyclycWait=0,
                   SLONG RepeatCore=1);

      void Pump (void);
      void BlitAt (SBBM &RoomBm, SLONG x, SLONG y);
      void BlitAtT (SBBM &RoomBm, SLONG x, SLONG y);
};*/

//--------------------------------------------------------------------------------------------
//Eine Animation einer kompletten Figur:
//--------------------------------------------------------------------------------------------
typedef XY CRepeat;
typedef XY CPostWait;

void  CalculatePalettemapper (const UBYTE *pPalette, SDL_Palette *pPaletteMapper);

//Werte für CSmackerClip.State:
#define SMACKER_CLIP_INACTIVE   0
#define SMACKER_CLIP_PLAYING    1
#define SMACKER_CLIP_WAITING    2

//Werte für die Variablenverknüpfunng
#define SMACKER_CLIP_MULT       8192    //Hiermit multipliziert man um den Frame anzugeben

#define SMACKER_CLIP_FRAME      4096
#define SMACKER_CLIP_PRE        1024
#define SMACKER_CLIP_POST       2048
#define SMACKER_CLIP_ADD        1
#define SMACKER_CLIP_SUB        2
#define SMACKER_CLIP_SET        3
#define SMACKER_CLIP_XOR        4

//Smacker-Person Moods:
#define SPM_IDLE                1     //Macht seine Arbeit
#define SPM_HOLY                2     //..ist dabei scheinheilig
#define SPM_TALKING             3     //redet
#define SPM_ANGRY_TALKING       4     //redet und ist wütend dabei
#define SPM_LISTENING           5     //hört zu, während der Spieler redet
#define SPM_LETTER              6     //liest einen Brief

#define SMACKER_CLIP_CANCANCEL  1
#define SMACKER_CLIP_DONTCANCEL 0

//Eine Teilanimation einer Figur:
class CSmackerClip
{
   private: //Dateieigenschaften...
       CString      Filename;
       CString      SoundFilename;
       SDL_Palette* PaletteMapper;       //Tabelle zum Mappen von 8 auf 16 Bit

       smk          pSmack;
       unsigned long Width;
       unsigned long Height;
       unsigned long FrameNum;
       unsigned long Frames;
       DWORD        FrameNext;

       SLONG        LastFrame;

       SBFX              SoundFx;        
       SLONG             NumSoundFx;     //Anzahl der Soundeffekte
       CUnrepeatedRandom SoundRandom;    //Wenn es mehrere Soundeffekte gibt
       DWORD             TickerNext;     //Zu diesem Zeitpunkt muß das nächste Sample gestartet werden
       BOOL              IsFXPlaying;

   private: //Statische Eigenschaften...
       SLONG     ClipId;
       XY        ScreenOffset;
       SLONG     MoodId;
       CRepeat   Repeat;                 //Min..Max
       CPostWait PostWait;               //Min..Max in 20/stel Sekunden
       BOOL      CanCancelClip;          //Kann der Clip vorzeitig abgebrochen werden?

       SLONG    *PostVar;                //NULL oder Variable, die am Animationsende gesetzt werden soll
       SLONG     PostOperation;          //Die Art der Operation, die durchgeführt werden soll
       SLONG     PostValue;
       const SLONG *DecisionVar;         //wenn !=NULL, legt sie fest, welche Folgeanimation gespielt wird...

       CString   SuccessorTokens;        //A="Always" S="only when Staying in this mood", E="Exit this mood only"
       BUFFER<SLONG> SuccessorIds;

   private: //Dynamische Eigenschaften...
       SLONG     State;                  //Inaktiv, Spielt Anim, In Postwait
       SLONG     RepeatCount;
       SLONG     WaitCount;

   public:
       CSmackerClip ();
       ~CSmackerClip ();
       void Start ();
       void Stop ();
       void ReSize (SLONG          ClipId,
                    const CString &Filename,
                    const CString &SoundFilename,
                    XY             ScreenOffset,
                    SLONG          MoodId,
                    CRepeat        Repeat,                 //Min..Max
                    CPostWait      PostWait,               //Min..Max in 20/stel Sekunden
                    BOOL           CanCancelClip,          //Kann der Clip vorzeitig abgebrochen werden?
      
                    SLONG         *PostVar,                //NULL oder Variable, die am Animationsende gesetzt werden soll
                    SLONG          PostOperation,
                    SLONG          PostValue,
      
                    const SLONG   *DecisionVar,            //wenn !=NULL, legt sie fest, welche Folgeanimation gespielt wird...
                    
                    CString        SuccessorTokens,        //z.B. "A2X8"...
                    SLONG          SuccessorIds, ...);

      void NextSyllable (void);
      void PlaySyllable (void);

   friend class CSmackerPerson;
};

//Die komplette Figur:
class CSmackerPerson
{
   private:
      SBBM    Bitmap;
      XY      BitmapPos;
      SLONG   ActiveClip;       //Nummer vom aktuellen Clip
      SLONG   CurrentMood;      //Idle, Talking, Listening, ...
      SLONG   DesiredMood;      //Diese Stimmung (z.B. "Listening") soll die Figur haben
      SLONG   AcceptedMood1;    //Dieser Zustand ist zwar nicht erwünscht, würde aber als weg zu DesiredMood akzeptiert
      SLONG   AcceptedMood2;    //Dieser Zustand ist zwar nicht erwünscht, würde aber als weg zu DesiredMood akzeptiert

      SBFX    SpeakFx;

   public:
      BUFFER<CSmackerClip> Clips;

   public: //State & Initialization:
      CSmackerPerson ();
      ~CSmackerPerson ();
      void  ReSize (SLONG NumberOfClips);
      void  SetSpeakFx (CString Filename);
      void  SetDesiredMood (SLONG DesiredMood, SLONG AcceptedMood1=-1, SLONG AcceptedMood2=-1);
      SLONG GetDesiredMood (void);
      SLONG GetMood (void);
      SLONG GetClip (void);
      SLONG GetFrame (void);

      void  ForceNextClip (void);

   private:
      void  NextClip (void);

   public: //Blitting:
      void  Pump (void);
      void  BlitAtT (SBBM &RoomBm);
      void  BlitAtT (SBBM &RoomBm, XY Offset);
};    