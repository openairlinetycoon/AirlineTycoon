// StdRaum.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Das Standard Raum Template

class CStdRaum
{
// Construction
public:
	CStdRaum(BOOL bHandy, ULONG PlayerNum, CString GfxLibName, __int64 graficId);

   BOOL       IsInBuro;             //RTTI: Is this the buro?

// Handy-Dialog-Attributes
protected:
   BOOL       bHandy;               //TRUE, wenn das Fenster zum Handy gehört
   SLONG      TempScreenScroll;     //Rein/Rausfahrposition vom Handy-Fenster
   SLONG      TempScreenScrollV;    //Bewegung vom Handy-Fenster
   SLONG      LastScrollTime;
   SLONG      HandyOffset;          //x-Verschiebung des Bildes für den Handy-Dialog
   SLONG      KonstruktorFinished;

// Attributes
protected:
   SLONG      PlayerNum;            //Auf diesem Spieler ist das Fenster fixiert
   XY         WinP1, WinP2;         //Die Position des ViewFensters im GameFrame Fenster
   XY         AirportRoomPos;       //Diese Position hat der Raum auf dem Flughafen
   SLONG      StatusLineSizeY;      //Die Y-Größe der Statuszeile. Entweder 20 oder 40
   SBBM       RoomBm;               //Bitmap, wenn man in einem Raum ist....
   RECT       WindowRect;           //Create-Window Koordinaten
   CString    Optionen[10];         //Bis zu 10 Anwahlmöglichkeiten
   CString    OrgOptionen[10];      //Bis zu 10 Anwahlmöglichkeiten, noch mit den Speech-Texten
   BOOL       CanCancelEmpty;       //Manchmal darf man Dialogblase beenden, bevor der Typ redet. Nämlich, wenn das Sprachsample leer ist.

// Tip Attributes
protected:
   BOOL       ForceRedrawTip;       //TRUE=Bitmap muß neugezeichnet werden
   SLONG      CurrentTipType;       //Wollen wir darstellen
   SLONG      LastTipType;          //Ist in der Bitmap
   SLONG      CurrentTipId;         //z.B. CityId
   SLONG      LastTipId;
   SLONG      CurrentTipIdPar1;      //z.B. vom Makler oder vom Museum
   SLONG      LastTipIdPar1;
   SLONG      CurrentTipIdPar2;     //z.B. vom Makler oder vom Museum
   SLONG      LastTipIdPar2;
   SBBM       TipBm;                //Der aktuelle Tip, inklusive Text
   SBBM      *pTipOriginBm;         //Die Tip-Bitmap; sie gehört einem Raum
   void      *pTipOriginBm2;        //Die Tip-Bitmap; sie gehört einem Raum
   XY         TipPos;

//Optionale & Dialog-Attribute:   
protected:
   BOOL       UsingHandy;           //Verwendet er das Handy?
   BOOL       Ferngespraech;        //Ist es ein Ferngespräch?
   SLONG      LetzteEinheit;        //Zu welchem Zeitpunkt war die letzte Gebühreneinheit?
   SLONG      TimeBubbleDisplayed;  //Wann wurde diese Sprechblase eröffnet (wg. Timeout beim Gespräch mit Spielern)
   GfxLib    *pRoomLib;             //Library für den Raum;
   SBBM       PicBitmap;            //Der Raum
   SBBM       OnscreenBitmap;       //On-Screen Dialoge & Menüs
   SBBM       NumberBitmap;         //Mit dieser Bitmap werden Zahlen eingeblendet, die früher über Zahlensynthese erzeugt wurden
   XY         NumberBitmapPos;      //Position der NumberBitmap
   SBBM       StatusBm;             //Statuszeile mit Geld/Uhrzeit
   __int64    LastMoney;            //Das *angezeigte* Geld
   ULONG      LastTime;             //Die *angezeigte* Uhrzeit
   BOOL       TextAlign;            //Top oder Bottom
   SLONG      DialogPartner;        //-1=kein Dialog; 999=Customer; DialogPar=CustomerId
   BOOL       DialogMedium;         //0=Direkt, 1=Telefon
   SLONG      DialogPar1;           //Generisch
   SLONG      DialogPar2;           //Generisch
   SLONG      DialogPar3;           //Generisch
   ULONG      CurrentHighlight;     //SubId auf dem der Cursor zur Zeit steht
   ULONG      CurrentTextGroupId;   //Nummer der aktuellen Dialoggruppe
   ULONG      LastTextGroupId;      //Zuletzt verwendete Gruppe
   ULONG      CurrentTextSubIdVon;  //Nummer des aktuellen Dialogelementes
   ULONG      CurrentTextSubIdBis;  //Nötigenfalls ein Bereich...
   SLONG      DefaultDialogPartner; //Der antwortet, wenn man aufs Inventar klickt
   BOOL       DisplayThisBubble;    //TRUE, wenn Sprechblase des Dialoges angezeigt werden soll (FALSE, um sie wegen der Sprachausgabe auszublenden.)

public:
   SLONG      TimeAtStart;          //timeGetTime() als Menü oder Dialogblase geöffnet wurde

   BOOL       PayingForCall;        //Bezahlt der Spieler dieses Gespräch?

   BOOL       DontDisplayPlayer;    //Wenn TRUE, wird der Spieler beim Dialog nicht in der Ecke gezeigt
   XY         PlayerBubblePos;      //Dort erscheint die Sprechblase (nur wenn DontDisplayPlayer==TRUE)

   CSmackerPerson *pSmackerPartner;   //Der Dialogpartner per Smacker
   CSmackerPerson *pSmackerPlayer;    //Der Spieler per Smacker
   SLONG           SmackerTimeToTalk; //Clock-Zeitpunkt für den Gesprächspartner bis zu dem diese Figur labert


   XY         BubblePos;            //Position der Sprechblase
   CRect      BubbleRect;           //Für's Speedup
   CRect      TextRect;             //Für's Speedup
   SLONG      BubbleStyle;

   SB_CFont  *pFontNormal;          //Text im normalen Zustand
   SB_CFont  *pFontHighlight;       //Text im Highlight Zustand
   SB_CFont  *pFontPartner;         //Text vom Dialogpartner

   BUFFER<XY>      GlowEffects;
   BUFFER<SLONG>   GlowBitmapIndices;

   BOOL            TalkedToA;       //Schon mit Person A geredet? (Wichtig für Dialogeröffnung)
   BOOL            TalkedToB;       //Schon mit Person B geredet? (Wichtig für Dialogeröffnung)

//Attribute für On-screen Menüs:
protected:
   GfxLib         *pMenuLib1, *pMenuLib2;  //Libraries das Menü
   SBBMS           MenuBms;
   BUFFER<SBBMS>   MenuBms2;
   SLONG           CurrentMenu;            //-1=kein Menü
   SLONG           LastMenu;               //-1=kein Menü
   XY              MenuStartPos;           //Von hier wird's gezoomt
   BOOL            ZoomFromAirport;        //MenuStartPos ist relativ zum Flughafen
   double          MinimumZoom;            //Minimaler Zoomfaktor
   SLONG           ZoomCounter;            //Von 0%(=MinimumZoom) bis 100%
   SLONG           ZoomSpeed;              //Von 0%(=MinimumZoom) bis 100%
   XY              MenuPos;
   CString         MenuPar4;
   SLONG           MenuPar1, MenuPar2,  MenuPar3;  //Generisch
   SLONG           MenuInfo, MenuInfo2, MenuInfo3;
   SLONG           MenuPage, MenuPageMax;  //Wenn man blättern kann
   BUFFER<SLONG>   MenuRemapper;           //Wenn ein Subset von Daten gewählt wurde
   CDataTable      MenuDataTable;          //Alternative Darstellung
   ULONG           MenuDialogReEntryA;     //Wiedereinsteig in den Dialog
   SLONG           MenuDialogReEntryB;     //Wiedereinsteig in den Dialog

   //VonBis Array für die Auswahl
   SLONG          TextAreaSizeY[20];    //Die Y-Größe von maximal 20 Optionen

//Attribute für den Taschenrechner:
   BOOL           CalculatorIsOpen;
   XY             CalculatorPos;        //Bildschirmposition
   SLONG          CalculatorValue;      //Dargestellte Zahl
   SBBMS          CalculatorBms;
   SLONG          CalculatorKey;        //Zuletzt gedrückte Taste
   SLONG          CalculatorKeyTimer;   //Time für die Taste
   GfxLib        *pCalculatorLib;

   SBFX           CalculatorFX;
   SBFX           SpeechFx;
   BOOL           TalkingSpeechFx;
   BOOL           ReadyToStartSpeechFx;

public:
   SLONG          StatusCount;          //DrawCounter für den StatusBar Speedup

//Sounds:
protected:
   SBFX  PaperFX;
   SBFX  PaperTearFX;
   SBFX  Dial1FX;
   SBFX  Dial2FX;
   SBFX  DialBusyFX;

   SBFX  BackgroundFX[5];
   DWORD BackgroundCount[5];
   SLONG BackgroundWait[5];
   SLONG BackgroundLautstaerke[5];

   void  SetBackgroundFx (SLONG Number, const CString &Filename, SLONG AvgWait, SLONG StartWait=0, SLONG Lautstaerke=100);

//Text-Operationen:
public:
   BOOL  PreLButtonDown (CPoint point);
   void  RepaintText (BOOL RefreshAll);
   void  PostPaint (void);
   void  CheckHighlight (CPoint point);
   void  SetTip (SBBM *TipOrigin, void *TipOrigin2, BOOL ForceRedraw, XY Pos, SLONG TipType, SLONG TipId, SLONG TipPar1=0, SLONG TipPar2=0);
   void  AnnouceTipDataUpdate (SLONG TipType);
   void  RepaintTip (void);
   BOOL  ConvertMousePosition (const XY &WindowsBased, XY *RoomBased);

   //Nicht-allgemeine Dialog-Operationen:
   void  StartDialog (SLONG DialogPartner, BOOL Medium, SLONG DialogPar1=-1, SLONG DialogPar2=-1, SLONG DialogPar3=-1);
   BOOL  IsDialogOpen (void);
   void  MakeSayWindow (BOOL TextAlign, ULONG SubId, const CString &String, SB_CFont *Normal);
   void  MakeSayWindow (BOOL TextAlign, const char *GroupId, ULONG SubId, SB_CFont *Normal, ...);
   void  MakeSayWindow (BOOL TextAlign, const char *GroupId, ULONG SubIdVon, ULONG SubIdBis, BOOL ParameterIndiziert, SB_CFont *Normal, SB_CFont *Highlight, ...);
   void  CloseTextWindow (void);
   void  StopDialog (void);
   void  MakeNumberWindow (CString Text);
   void  MakeNumberWindow (const char *GroupId, ULONG SubId, ...);

//Menü-Operationen:
public:
   bool  MenuIsPlain (void);
   void  MenuStart (SLONG MenuType, SLONG MenuPar1=0, SLONG MenuPar2=0, SLONG MenuPar3=0);
   void  MenuStop (void);
   void  MenuLeftClick (XY Pos);
   void  MenuRightClick (XY Pos);
   void  MenuRepaint (void);
   BOOL  MenuIsOpen (void);
   void  MenuSetZoomStuff (const XY &MenuStartPos, double MinimumZoom, BOOL ZoomFromAirport, SLONG ZoomSpeed=25);
   void  MenuPostPaint (void);
   void  CalcOpen (XY Position, SLONG Value=0);
   void  CalcRepaint (void);
   void  CalcStop (BOOL Cancel);
   void  CalcClick (void);
   void  CalcKey (SLONG Key);

// Operations
public:
   void  ReSize (CString GfxLibName, __int64 graficId);
   void  ReSize (__int64 graficId);
   void  InitToolTips (void);
   void  PumpToolTips (void);
   void  KeepRoomLib (void);

   void  OnPaint(BOOL bHandyDialog);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Bank)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStdRaum();

   void ProcessEvent(const SDL_Event& event, CPoint position);

	// Generated message map functions
protected:
	//{{AFX_MSG(CStdRaum)
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnPaint();
   virtual void OnTimer(UINT nIDEvent);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnSetCursor(void* pWnd, UINT nHitTest, UINT message);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
   virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

   static Uint32 TimerFunc(Uint32 interval, void* param);
   
   friend void MessagePump (void);
   friend void PumpNetwork (void);

   friend class CMessages;
   friend class GameFrame;
   friend class AirportView;
   friend class SBPRIMARYBM;
   friend class PERSONS;
   friend class PERSON;
   friend class PLAYER;
   friend class SIM;
};