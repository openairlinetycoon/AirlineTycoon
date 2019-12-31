//============================================================================================
// Proto.h - Funktionsprototypen:
//============================================================================================

//--------------------------------------------------------------------------------------------
//AtNet.Cpp:
//--------------------------------------------------------------------------------------------
void SetNetworkBitmap (SLONG Number, SLONG WaitingType=0);
void DisplayBroadcastMessage (CString String, SLONG FromPlayer=-1);
void PumpBroadcastBitmap (bool bJustForEmergency=false);
void PumpNetwork (void);
SLONG   GetMediumIdentifier (GUID guidServiceProvider);
CString GetMediumName (GUID Guid);

//--------------------------------------------------------------------------------------------
//ColorFx.Cpp:
//--------------------------------------------------------------------------------------------
void RemapColor (SB_CBitmapCore *pBitmap, const CRect &HighRect, UWORD OldFontColor, ULONG NewFontColor);

//--------------------------------------------------------------------------------------------
//DxBm.Cpp:
//--------------------------------------------------------------------------------------------
void LoadLBMPalette (const char *Filename, PALETTEENTRY *Palette);


//--------------------------------------------------------------------------------------------
//GameFrame.Cpp:
//--------------------------------------------------------------------------------------------
void MessagePump (void);
void SetToolTip (SLONG TipId, XY Pos);
void SetMouseLook (SLONG Look, SLONG TipId, SLONG ClickArea, SLONG ClickId, SLONG ClickPar1=0, SLONG ClickPar2=0);
void SetMouseLook (SLONG Look, SLONG TipId, const CString &Description, SLONG ClickArea, SLONG ClickId, SLONG ClickPar1=0, SLONG ClickPar2=0);
void DefaultOnLButtonDown (void);
void DefaultOnLButtonUp (void);
void DefaultOnRButtonDown (void);
void DefaultOnRButtonUp (void);

//--------------------------------------------------------------------------------------------
//HLines.Cpp:
//--------------------------------------------------------------------------------------------
void UpdateHLinePool (void);

//--------------------------------------------------------------------------------------------
//Init.Cpp:
//--------------------------------------------------------------------------------------------
void InitSoundSystem (SDL_Window *AppWnd);
void PaintStarAt (SBBM &Bitmap, XY Pos);
void InitPathVars (void);
void InitFonts (void);
void InitStatusLines (void);
void InitItems (void);
void InitTipBms (void);
void InitGlobeMapper (void);
void UpdateSavegames (void);

//--------------------------------------------------------------------------------------------
//Misc.Cpp:
//--------------------------------------------------------------------------------------------
CString RemoveAccents (CString str);
SLONG   GetAnzBits (ULONG Flags);
double  GetAlphaFromXY (XY Delta);
void    DrawChart (SBBM &TipBm, ULONG Color, const FBUFFER<SLONG> &Values, SLONG MinValue, SLONG MaxValue, const XY &TopLeft, const XY &TopRight, const XY &BottomLeft, const XY &BottomRight);
CString KorrigiereUmlaute (CString &Originaltext);
//SLONG   ReadTimeStampCounter (void);
__int64 StringToInt64 (const CString &String);
BOOL    IsRoomBusy(UWORD RoomId, SLONG ExceptPlayer);
BOOL    EarthProjectize (const XY &NaturalPos, UWORD EarthAlpha, XY *PixelPos);
BOOL    EarthProjectize (const FXY &NaturalPos, UWORD EarthAlpha, XY *PixelPos);
SLONG   AddToNthDigit (SLONG Value, SLONG Digit, SLONG Add);
void    MyMessageBox (LPCTSTR Title, LPCTSTR String, ...);
double  GetFrameRate (void);
CString ConvertDate2String (UWORD Date);
UWORD   ConvertString2Date (char *String);
void    DoAppPath (void);
CString FullFilename (const CString &Filename, const CString &PathString);
CString FullFilename (const CString &Filename, const CString &PathString, SLONG Num);
SLONG   CalculateFlightKerosin (SLONG VonCity, SLONG NachCity, SLONG Verbrauch, SLONG Geschwindigkeit);
SLONG   CalculateFlightCost (SLONG VonCity, SLONG NachCity, SLONG Verbrauch, SLONG Geschwindigkeit, SLONG Player);
SLONG   CalculateRealFlightCost (SLONG VonCity, SLONG NachCity, SLONG Verbrauch, SLONG Geschwindigkeit, SLONG Player, CString PlaneName);
SLONG   CalculateFlightCostRechnerisch (SLONG VonCity, SLONG NachCity, SLONG Verbrauch, SLONG Geschwindigkeit, SLONG PlayerNum);
void    InitEinheiten (const CString &Filename);
CString Insert1000erDots (long Value);
CString Insert1000erDots64 (__int64 Value);
CRect   PaintTextBubble (SBBM &OffscreenBm, const XY &p1, const XY &p2, const XY &Entry);
BOOL    CheckCursorHighlight (const CRect &rect, UWORD FontColor, SLONG Look=CURSOR_HOT, SLONG TipId=0, SLONG ClickArea=0, SLONG ClickId=0, SLONG ClickPar1=0, SLONG ClickPar2=0);
BOOL    CheckCursorHighlight (const XY &CursorPos, const CRect &rect, UWORD FontColor, SLONG Look=CURSOR_HOT, SLONG TipId=0, SLONG ClickArea=0, SLONG ClickId=0, SLONG ClickPar1=0, SLONG ClickPar2=0);
void    CheckCString (CString *String);
void    GetMatchingFilelist (CString DirAndWildcards, BUFFER<CString> &Array);
CString GetMatchingNext (CString DirAndWildcards, CString CurrentFilename, long Add=1);
long    CountMatchingFilelist (CString DirAndWildcards);
CString CreateNumeratedFreeFilename (CString DirAndFilename);
CString GetFilenameFromFullFilename (CString FullFilename);
//DWORD   timeGetTime(void);

//--------------------------------------------------------------------------------------------
//Planer.Cpp:
//--------------------------------------------------------------------------------------------
void PaintGlobe (const TECBM &SourceBm, SBBM *TargetBm, UWORD EarthAlpha, const XY &Offset);

//--------------------------------------------------------------------------------------------
//Player.Cpp:
//--------------------------------------------------------------------------------------------
bool RobotUse (SLONG FeatureId);

//--------------------------------------------------------------------------------------------
//Protect.Cpp:
//--------------------------------------------------------------------------------------------
#define GetPhysicalCdRomBitlist  ApplyColors
#define CDProtection             CheckCursorOutline
#define GetFileChecksum          UpdateCursorPosAndColor
#define CheckMainFile            OverrideMidi
#define CheckSomeCdFileLength    BlitDeltaChannel
#define CheckCDFile              DoStrangeStrings
#define CheckComputerLicense     ConvertToOctal

bool  Protect_CheckFileSize (char *_fname, char *_length);
ULONG GetPhysicalCdRomBitlist (void);
bool  CDProtection(CString *pCdDrive);
ULONG GetFileChecksum (const CString &Filename);
BOOL  CheckMainFile (const CString &Filename);
bool  CheckCDFile (int _filepos, int _filesize, int _code);
void  CheckSomeCdFileLength (void);
bool  CheckComputerLicense (void);

//--------------------------------------------------------------------------------------------
//SBBM.Cpp:
//--------------------------------------------------------------------------------------------
__int64 GetIdFromString (CString Texts);

//--------------------------------------------------------------------------------------------
//SmackPrs.Cpp:
//--------------------------------------------------------------------------------------------
void CalculatePalettemapper (UBYTE *pPalette, UWORD *pPaletteMapper);
void ConvertBitmapTo16Bit (UBYTE *SourcePic, SBBM *pBitmap, UWORD *pPaletteMapper);

//--------------------------------------------------------------------------------------------
//Stdraum.Cpp:
//--------------------------------------------------------------------------------------------
void  UpdateStatusBar (void);
void  SetRoomVisited (SLONG PlayerNum, UBYTE RoomId);
BOOL  WasRoomVisited (SLONG PlayerNum, UBYTE RoomId);
BOOL  WasRoomNumberVisited (SLONG PlayerNum, SLONG RoomNumber);
UBYTE GetRoomIdFromNumber (SLONG PlayerNum, SLONG Number);


//--------------------------------------------------------------------------------------------
//Sound.Cpp:
//--------------------------------------------------------------------------------------------
CString RemoveSpeechFilename (CString String);
CString GetSpeechFilename (CString String, SLONG Index=0, CString *pTextFollows=NULL);
BOOL    CreateSpeechSBFX (CString String, SBFX *pFx, SLONG Player, BOOL *bAnyMissing=NULL);

void  PlayUniversalFx (CString Filename, SLONG Volume);
void  PlayFanfare (void);
SLONG Prozent2Dezibel (SLONG Prozent);
void  SetMidiVolume(long volume);
BOOL  IsMidiAvailable (void);
void  NextMidi (void);
void  PlayMidi (const CString &Filename);
void  PlayMidiFrom (const CString &Filename, SLONG StartPosition);
void  StopMidi (void);
void  SetMidiVolume (SLONG Volume);
void  SetWaveVolume(long volume);
void  PauseMidi (void);
void  ResumeMidi (void);


//--------------------------------------------------------------------------------------------
//TakeOff.Cpp:
//--------------------------------------------------------------------------------------------
BOOL InitDirectX (void);


//--------------------------------------------------------------------------------------------
//Tips.Cpp:
//--------------------------------------------------------------------------------------------
void DrawAutoflugTip (SBBM &TipBm, SLONG Costs, SLONG VonCity, SLONG NachCity);
void DrawCityTip (SBBM &TipBm, ULONG CityId);
void DrawPlaneTip (SBBM &TipBm, SBBM *pTipOriginBm, CPlaneType *PlaneType, CPlane *Plane);
void DrawRouteTip (SBBM &TipBm, SLONG NumPlayer, ULONG RouteId, SLONG Passagiere, SLONG Costs);
void DrawAuftragTip (SLONG Player, SBBM &TipBm, SBBMS *pPlaneTips, SBBM *pTipOriginBm, CAuftrag *Auftrag, SLONG Costs, SLONG Okay, XY MapOffset);
void DrawKursTipContents (SBBM &TipBm, SLONG PlayerView, SLONG PlayerAktie, SB_CFont *pHeadFont, SB_CFont *pFont);
void DrawItemTipContents (SBBM &TipBm, SLONG Item, SB_CFont *pHeadFont, SB_CFont *pFont);
void DrawMoneyTip (SBBM &TipBm, SLONG PlayerNum, SLONG Page);
void DrawTankTip (SBBM &TipBm, SLONG PlayerNum);
void DrawPlanesNotepad (SBBM &NotepadBm, CDataTable *Table, SLONG CountFrom, SBBMS *pMenuBms, BOOL DisplayWartungskosten=FALSE);
void DrawFrachtTip (SLONG Player, SBBM &TipBm, SBBMS *pPlaneTips, SBBM *pTipOriginBm, CFracht *Fracht, SLONG TonsThis, SLONG Costs, SLONG Income, SLONG Okay, XY MapOffset);
void DrawFrachtTipContents (SLONG PlayerNum, SBBM &TipBm, SBBMS *pPlaneTips, CFracht *Fracht, SLONG TonsThis, SLONG Costs, SLONG Income, SLONG Okay, XY Headline, XY Contents, XY MapOffset, SB_CFont *pHeadFont, SB_CFont *pFont, BOOL Unlocked=TRUE);

void DrawAutoflugTipContents (SBBM &TipBm, SLONG Costs, SLONG NotPassengers, SLONG VonCity, SLONG NachCity, XY Headline, XY Contents, SB_CFont *pHeadFont, SB_CFont *pFont, BOOL Unlocked);
void DrawCityTipContents (SBBM &TipBm, ULONG CityId, XY Headline, XY Contents, XY Map, SB_CFont *pHeadFont, SB_CFont *pFont);
void DrawPlaneTipContents (SBBM &TipBm, CPlaneType *PlaneType, CPlane *Plane, XY Headline, XY Contents, SB_CFont *pHeadFont, SB_CFont *pFont, BOOL OwnPlane=FALSE, SLONG OwnedByComputer=FALSE);
void DrawXPlaneTipContents (SBBM &TipBm, CString Planename, XY Headline, XY Contents, SB_CFont *pHeadFont, SB_CFont *pFont);
void DrawRouteTipContents (SBBM &TipBm, SLONG NumPlayer, ULONG RouteId, SLONG Gate, SLONG Passagiere, SLONG PassagiereFC, SLONG Costs, SLONG Ticketpreis, SLONG TicketpreisFC, XY Headline, XY Contents, SB_CFont *pHeadFont, SB_CFont *pFont, BOOL Unlocked=TRUE);
void DrawAuftragTipContents (SLONG Player, SBBM &TipBm, SBBMS *pPlaneTips, CAuftrag *Auftrag, SLONG Costs, SLONG Gate, SLONG Okay, XY Headline, XY Contents, XY MapOffset, SB_CFont *pHeadFont, SB_CFont *pFont, BOOL Unlocked=TRUE);
void DrawKursTipContents (SBBM &TipBm, SLONG PlayerView, SLONG PlayerAktie, XY Headline, XY Contents, SB_CFont *pHeadFont, SB_CFont *pFont);
void DrawItemTipContents (SBBM &TipBm, SLONG Item, XY Headline, XY Contents, SB_CFont *pHeadFont, SB_CFont *pFont);
void DrawMoneyTipContents (SBBM &TipBm, SLONG PlayerNum, XY Headline, XY Contents, SB_CFont *pHeadFont, SB_CFont *pFont);
void DrawTankTipContents (SBBM &TipBm, SLONG PlayerNum, XY Headline, XY Contents, SB_CFont *pHeadFont, SB_CFont *pFont);
