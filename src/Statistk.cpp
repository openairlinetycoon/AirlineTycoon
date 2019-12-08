//============================================================================================
// Statistk.cpp : Der Statistik-Screen
//============================================================================================
// Link: "Statistk.h"
//============================================================================================
#include "stdafx.h"
#include "Statistk.h"
#include "glstat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Zum debuggen:
static const char FileId[] = "Stat";

DWORD const ClickHighlightTime=300;     //300 ms bleibt ein Button nach einem Click ganz hell

extern SB_CColorFX ColorFX;

//Die Farben der Fluglinien; definiert in Bank.cpp; Aber bitte erst mittels GetHardwarecolor()
//in die grafikkartenspezifische Farbe umrechnen...
extern ULONG AktienKursLineColor [4];
ULONG  DarkColors [4] = { 0x00407f, 0x007f20, 0x7f0000, 0x7f7f00 };

// Das Ausgabefenster für den Graphen
#define G_LEFT		(6)
#define G_TOP		(30)
#define G_RIGHT	(439)
#define G_BOTTOM	(329)
#define G_HEIGHT  (G_BOTTOM-G_TOP)
#define G_WIDTH   (G_RIGHT-G_LEFT)

long days[] = { 7, 14, 21, 30, 90, 180, 270, 360 };

const char TOKEN_STAT[]="STAT";

//--------------------------------------------------------------------------------------------
//Die Screen wird eröffnet:
//--------------------------------------------------------------------------------------------
CStatistik::CStatistik (BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "Statistk.gli", GFX_STATISTK)
{
   SLONG c = 0;

   DropDownPos   = XY(0,0);
   DropDownSpeed = 0;

	memcpy (_playerMask, Sim.StatplayerMask, sizeof(_playerMask));
	_group         = Sim.Statgroup;
	_fGraphVisible = Sim.StatfGraphVisible;
   DropDownPos.y  = Sim.DropDownPosY;

	// Umständlich, aber wahr...
	{
		for (short p = 0 ; p < MAX_GROUP ; p++)
		for (short i = 0 ; i < MAX_ITEMS ; i++)
		{
			_iArray[p][i].visible   = Sim.StatiArray[p][i];
			_iArray[p][i].typOfItem = TYP_LINEFEED;
		}

		// Erste Gruppe
		short c = 0;
		short g = 0;
		_iArray[g][c].typOfItem = TYP_GROUP;		_iArray[g][c].textId = 8002;	c++;
		_iArray[g][c].typOfItem = TYP_CURRENCY;	_iArray[g][c].textId = 8003;	_iArray[g][c++].define = STAT_E_ROUTEN;
		_iArray[g][c].typOfItem = TYP_CURRENCY;	_iArray[g][c].textId = 8004;	_iArray[g][c++].define = STAT_E_AUFTRAEGE;
		_iArray[g][c].typOfItem = TYP_CURRENCY;	_iArray[g][c].textId = 8006;	_iArray[g][c++].define = STAT_E_SONSTIGES;
		_iArray[g][c].typOfItem = TYP_SUM_CURR;	_iArray[g][c].textId = 8014;	_iArray[g][c++].define = 0;
		c++;
		_iArray[g][c].typOfItem = TYP_GROUP;		_iArray[g][c].textId = 8007;	c++;
		_iArray[g][c].typOfItem = TYP_CURRENCY;	_iArray[g][c].textId = 8008;	_iArray[g][c++].define = STAT_A_MIETEN;
		_iArray[g][c].typOfItem = TYP_CURRENCY;	_iArray[g][c].textId = 8009;	_iArray[g][c++].define = STAT_A_GEHAELTER;
		_iArray[g][c].typOfItem = TYP_CURRENCY;	_iArray[g][c].textId = 8010;	_iArray[g][c++].define = STAT_A_KEROSIN;
		_iArray[g][c].typOfItem = TYP_CURRENCY;	_iArray[g][c].textId = 8011;	_iArray[g][c++].define = STAT_A_WARTUNG;
		_iArray[g][c].typOfItem = TYP_CURRENCY;	_iArray[g][c].textId = 8012;	_iArray[g][c++].define = STAT_A_STRAFEN;
		_iArray[g][c].typOfItem = TYP_CURRENCY;	_iArray[g][c].textId = 8006;	_iArray[g][c++].define = STAT_A_SONSTIGES;
		_iArray[g][c].typOfItem = TYP_SUM_CURR;	_iArray[g][c].textId = 8014;	_iArray[g][c++].define = 0;
		c++;
		_iArray[g][c].typOfItem = TYP_SUM_DIFF;	_iArray[g][c].textId = 8015;	_iArray[g][c++].define = 0;

		// Zweite Seite
		c = 0;
		g = 1;
		_iArray[g][c].typOfItem = TYP_GROUP;		_iArray[g][c].textId = 8100;	c++;
		_iArray[g][c].typOfItem = TYP_VALUE;		_iArray[g][c].textId = 8101;	_iArray[g][c++].define = STAT_AKTIEN_ANZAHL;
		_iArray[g][c].typOfItem = TYP_CURRENCY;	_iArray[g][c].textId = 8102;	_iArray[g][c++].define = STAT_AKTIENKURS;
		_iArray[g][c].typOfItem = TYP_VALUE;		_iArray[g][c].textId = 8103;	_iArray[g][c++].define = STAT_AKTIEN_SA;
		_iArray[g][c].typOfItem = TYP_VALUE;		_iArray[g][c].textId = 8104;	_iArray[g][c++].define = STAT_AKTIEN_FL;
		_iArray[g][c].typOfItem = TYP_VALUE;		_iArray[g][c].textId = 8105;	_iArray[g][c++].define = STAT_AKTIEN_PT;
		_iArray[g][c].typOfItem = TYP_VALUE;		_iArray[g][c].textId = 8106;	_iArray[g][c++].define = STAT_AKTIEN_HA;
		_iArray[g][c].typOfItem = TYP_SHAREVALUE;	_iArray[g][c].textId = 8107;	_iArray[g][c++].define = 0;

		c++;
		_iArray[g][c].typOfItem = TYP_GROUP;		_iArray[g][c].textId = 8110;	c++;
		_iArray[g][c].typOfItem = TYP_CURRENCY;	_iArray[g][c].textId = 8114;	_iArray[g][c++].define = STAT_KONTO;
		_iArray[g][c].typOfItem = TYP_CURRENCY;	_iArray[g][c].textId = 8116;	_iArray[g][c++].define = STAT_KREDIT;
		_iArray[g][c].typOfItem = TYP_CURRENCY;	_iArray[g][c].textId = 8115;	_iArray[g][c++].define = STAT_FIRMENWERT;
		_iArray[g][c].typOfItem = TYP_SINGLE_PERCENT;	_iArray[g][c].textId = 8113;	_iArray[g][c++].define = STAT_BEKANNTHEIT;

		// Dritte Seite
		c = 0;
		g = 2;
		_iArray[g][c].typOfItem = TYP_GROUP;		_iArray[g][c].textId = 8200;	c++;
		_iArray[g][c].typOfItem = TYP_VALUE;		_iArray[g][c].textId = 8201;	_iArray[g][c++].define = STAT_FLUGZEUGE;
		_iArray[g][c].typOfItem = TYP_VALUE;		_iArray[g][c].textId = 8202;	_iArray[g][c++].define = STAT_ROUTEN;
		_iArray[g][c].typOfItem = TYP_VALUE;		_iArray[g][c].textId = 8221;	_iArray[g][c++].define = STAT_NIEDERLASSUNGEN;
		_iArray[g][c].typOfItem = TYP_VALUE;		_iArray[g][c].textId = 8203;	_iArray[g][c++].define = STAT_FLUEGE;
		_iArray[g][c].typOfItem = TYP_VALUE;		_iArray[g][c].textId = 8204;	_iArray[g][c++].define = STAT_AUFTRAEGE;
		_iArray[g][c].typOfItem = TYP_VALUE;		_iArray[g][c].textId = 8205;	_iArray[g][c++].define = STAT_LMAUFTRAEGE;
		_iArray[g][c].typOfItem = TYP_VALUE;		_iArray[g][c].textId = 8206;	_iArray[g][c++].define = STAT_VERSPAETUNG;
		_iArray[g][c].typOfItem = TYP_VALUE;		_iArray[g][c].textId = 8207;	_iArray[g][c++].define = STAT_UNFAELLE;
		_iArray[g][c].typOfItem = TYP_VALUE;		_iArray[g][c].textId = 8208;	_iArray[g][c++].define = STAT_SABOTIERT;

		c++;
		_iArray[g][c].typOfItem = TYP_GROUP;		_iArray[g][c].textId = 8210;	c++;
		_iArray[g][c].typOfItem = TYP_VALUE;		_iArray[g][c].textId = 8211;	_iArray[g][c++].define = STAT_PASSAGIERE;
		_iArray[g][c].typOfItem = TYP_PERCENT;		_iArray[g][c].textId = 8212;	_iArray[g][c++].define = STAT_ZUFR_PASSAGIERE;
		_iArray[g][c].typOfItem = TYP_VALUE;		_iArray[g][c].textId = 8111;	_iArray[g][c++].define = STAT_MITARBEITER;
		_iArray[g][c].typOfItem = TYP_PERCENT;		_iArray[g][c].textId = 8112;	_iArray[g][c++].define = STAT_ZUFR_PERSONAL;
	}

	// Temporäre Dinge
	_fRepaint        = true;
	_selectedItem    = -1;			// Kein (Font-)Item selektiert
	_oldSelectedItem = -1;
	//_days = _newDays = 7;			// Soviele Tage werden angezeigt
   _days            = Sim.Statdays;
   _newDays         = Sim.StatnewDays;

	// Thomas' Part
   Sim.Players.UpdateStatistics ();

   for (c=1; c<5; c++)
   {
      Hdu.HercPrintf (0, "stat_%li.mcf", c);
      StatFonts[c-1].Load (lpDD, (char*)(LPCTSTR)FullFilename ("stat_%li.mcf", MiscPath, c));
   }

   HighlightBar.ReSize(439,21);
   HighlightBar.FillWith (0xffffff);

   TopSaver.ReSize (pRoomLib, "TOPSAVER");

   DropDownBackgroundBm.ReSize (pRoomLib, GFX_DECKEL);
   DropDownBm.ReSize (DropDownBackgroundBm.Size.x, DropDownBackgroundBm.Size.y);
   UpDownArrows.ReSize (pRoomLib, "DOWN DOWNHIGH UPHIGH");

   TextTableBm.ReSize (440, 330);
                             
   LupeLogoBms.ReSize (pRoomLib, bprintf("BUTA%liOFF BUTA%liHI BUTA%liON", PlayerNum+1, PlayerNum+1, PlayerNum+1));
   LupeZoomBms.ReSize (pRoomLib, "ZOOMIN ZOOMOUT");
   ButtonGeldBms.ReSize (pRoomLib, "BUTBHI BUTBON");
   ButtonRouteBms.ReSize (pRoomLib, "BUTCHI BUTCON");
                             
   ExitBms.ReSize (pRoomLib, "EXIT EXITHIGH");
   LogoBms.ReSize (pRoomLib, "SWTCH1ON SWTCH1HI SWTCH1SU SWTCH2ON SWTCH2HI SWTCH2SU SWTCH3ON SWTCH3HI SWTCH3SU SWTCH4ON SWTCH4HI SWTCH4SU");
   PobelBms.ReSize (pRoomLib, "POBEL_1 POBEL_2 POBEL_3 POBEL_4");

	Haeckchen.ReSize (pRoomLib, "HAKCHEN");

	CalcGraph();
	RepaintGraphWindow ();
	RepaintTextWindow ();

   StatButtons.ReSize (11);

   //Die Buttons:         .ID HelpId  ........MouseRect......  .BlitOffset  & #2  ..NormalBm.  ...HighBm..  ..ClickBm..  ClckHi Toggle
   StatButtons[0].ReSize (999,     0, CRect(  0,440,  79,479), XY(  0,440), NULL, &ExitBms[0], &ExitBms[1], NULL,        NULL,  FALSE);
   //Hinweis: Der Exit-Button muß immer Button 0 sein!

   //Der Buttons für die DropDown Liste:
   StatButtons[1].ReSize (100,     0, CRect(190, 11, 639, 38), XY(390, 10), &DropDownPos, NULL, &UpDownArrows[1], NULL,  NULL,  FALSE);

   //Die vier Spielerportraits:
   StatButtons[2].ReSize (110, -1000, CRect(190,370, 299,429), XY(190,370), NULL, NULL,        &LogoBms[1],  &LogoBms[0], &LogoBms[2],  TRUE);
   StatButtons[3].ReSize (111, -1001, CRect(300,370, 409,429), XY(300,370), NULL, NULL,        &LogoBms[4],  &LogoBms[3], &LogoBms[5],  TRUE);
   StatButtons[4].ReSize (112, -1002, CRect(410,370, 519,429), XY(410,370), NULL, NULL,        &LogoBms[7],  &LogoBms[6], &LogoBms[8],  TRUE);
   StatButtons[5].ReSize (113, -1003, CRect(520,370, 629,429), XY(520,370), NULL, NULL,        &LogoBms[10], &LogoBms[9], &LogoBms[11], TRUE);

   //Die 3 Radio-Buttons links unten
   StatButtons[6].ReSize (120,  3300, CRect( 10,370,  65,429), XY( 10,370), NULL, &LupeLogoBms[0],	&LupeLogoBms[1],    &LupeLogoBms[2],    &LupeLogoBms[2],    TRUE);
   StatButtons[7].ReSize (121,  3301, CRect( 70,370, 125,429), XY( 70,370), NULL, NULL,				&ButtonGeldBms[0],  &ButtonGeldBms[1],  &ButtonGeldBms[1],  TRUE);
   StatButtons[8].ReSize (122,  3302, CRect(130,370, 185,429), XY(130,370), NULL, NULL,				&ButtonRouteBms[0], &ButtonRouteBms[1], &ButtonRouteBms[1], TRUE);

   //Die Zoom-Buttons für die Drop-Down Liste:
   StatButtons[ 9].ReSize (130,     0, CRect(374, -318, 408, -293), XY(374, -318), &DropDownPos, NULL, &LupeZoomBms[1], &LupeZoomBms[1], NULL,  FALSE);
   StatButtons[10].ReSize (131,     0, CRect(412, -318, 446, -293), XY(412, -318), &DropDownPos, NULL, &LupeZoomBms[0], &LupeZoomBms[0], NULL,  FALSE);

	// Den entsprechenden Group-Button hilighten
	for (c=StatButtons.AnzEntries()-1; c>=0; c--)
	{
		if (StatButtons[c].Id == 120 + _group) StatButtons[c].LastClicked=1;

		if (StatButtons[c].Id == 110 && _playerMask[0]) StatButtons[c].LastClicked=1;
		if (StatButtons[c].Id == 111 && _playerMask[1]) StatButtons[c].LastClicked=1;
		if (StatButtons[c].Id == 112 && _playerMask[2]) StatButtons[c].LastClicked=1;
		if (StatButtons[c].Id == 113 && _playerMask[3]) StatButtons[c].LastClicked=1;
	}

   if (DropDownPos.y>0)
      RepaintGraphWindow();

   ShowWindow(SW_SHOW);
   UpdateWindow();
}

//--------------------------------------------------------------------------------------------
//Die Welt geht unter:
//--------------------------------------------------------------------------------------------
CStatistik::~CStatistik()
{
	memcpy (Sim.StatplayerMask, _playerMask, sizeof(Sim.StatplayerMask));
	Sim.Statgroup         = _group;
	Sim.StatfGraphVisible = _fGraphVisible;
   Sim.Statdays          = _days;
   Sim.StatnewDays       = _newDays;
   Sim.DropDownPosY      = DropDownPos.y;

   if (Sim.DropDownPosY<329/2) Sim.DropDownPosY=0;
                          else Sim.DropDownPosY=329;

	for (short p = 0 ; p < MAX_GROUP ; p++)
	   for (short i = 0 ; i < MAX_ITEMS ; i++)
	   {
		   Sim.StatiArray[p][i]=_iArray[p][i].visible;
	   }

   HighlightBar.Destroy();
   TopSaver.Destroy();

   DropDownBackgroundBm.Destroy();
   DropDownBm.Destroy();
   UpDownArrows.Destroy();

   TextTableBm.Destroy();
                             
   LupeLogoBms.Destroy();
   LupeZoomBms.Destroy();
   ButtonGeldBms.Destroy();
   ButtonRouteBms.Destroy();
                             
   ExitBms.Destroy();
   LogoBms.Destroy();

	Haeckchen.Destroy();
	PobelBms.Destroy();

   StatusCount=20;
}

//--------------------------------------------------------------------------------------------
//BEGIN_MESSAGE_MAP(CStatistik, CStdRaum)
//--------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CStatistik, CStdRaum)
	//{{AFX_MSG_MAP(CStatistik)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////////////////////
// CStatistik message handlers
//////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------
//void CStatistik::OnPaint()
//--------------------------------------------------------------------------------------------
void CStatistik::OnPaint() 
{
   SLONG  c;
   SBBM  *ExitBm=NULL;
   XY     ExitBmPos;
   DWORD  CurrentTime=timeGetTime();
   static SLONG RefreshStatistics=0;

   if (((++RefreshStatistics)&7)==0)
      Sim.Players.UpdateStatistics ();

   { CPaintDC dc(this); }

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   //Die Tafel bewegen:
   if (DropDownSpeed>0)
   {
      DropDownSpeed+=10;
      while (Calc1nSum(DropDownSpeed)/3>329-DropDownPos.y) DropDownSpeed--;

      DropDownPos.y+=DropDownSpeed;
      if (DropDownPos.y>=329)
      {
         DropDownSpeed = 0;
         DropDownPos.y = 329;
         StatButtons[1].BitmapNormal  = NULL;
         StatButtons[1].BitmapHi      = &UpDownArrows[2];
         StatButtons[1].BitmapClicked = NULL;
      }
   }
   else if (DropDownSpeed<0)
   {
      DropDownSpeed-=10;
      while (Calc1nSum(-DropDownSpeed)/3>DropDownPos.y) DropDownSpeed++;

      DropDownPos.y+=DropDownSpeed;
      if (DropDownPos.y<=0) {DropDownSpeed=0; DropDownPos.y=0; }
   }

	// Den Graphen in X animieren
	if (_newDays != _days)
	{
		long diff = (_newDays - _days) / 3 * 2;
		if (diff == 0)
			_days = _newDays;
		else
			_days = _days + diff;

		_fRepaint = true;

      if (DropDownPos.y!=0) CalcGraph();
	}

   //Den Screen bemalen:
   if (DropDownPos.y!=329) RoomBm.BlitFrom (TextTableBm, 190, 40);
   if (DropDownPos.y!=0)
   {
      RoomBm.pBitmap->SetClipRect(&CRect(190,10,630,370));
      RoomBm.BlitFrom (DropDownBm, 190, 40-360+DropDownPos.y);
      RoomBm.pBitmap->SetClipRect(&CRect(0,0,640,440));
   }

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   //Highlights und ToolTips der Buttons verwalten:
   if (!IsDialogOpen() && !MenuIsOpen())
   {
      for (c=StatButtons.AnzEntries()-1; c>=0; c--)
      {
         XY BlitPos = StatButtons[c].BitmapOffset;
         XY off     = XY(0,0);

         if (StatButtons[c].HelpId<=-1000)
            if (Sim.Players.Players[-StatButtons[c].HelpId-1000].IsOut) continue;

         ExitBmPos=BlitPos;

         if (StatButtons[c].BitmapOffset2) { off=*StatButtons[c].BitmapOffset2; BlitPos+=off; }

         if (DropDownPos.y!=329 && StatButtons[c].HelpId<0 && !IsDialogOpen() && !MenuIsOpen() && gMousePosition.IfIsWithin (StatButtons[c].HotArea.left, StatButtons[c].HotArea.top, StatButtons[c].HotArea.right, StatButtons[c].HotArea.bottom))
         {
            SetMouseLook (CURSOR_HOT, -StatButtons[c].HelpId, Sim.Players.Players[-StatButtons[c].HelpId-1000].AirlineX+" ("+Sim.Players.Players[-StatButtons[c].HelpId-1000].NameX+")", ROOM_STATISTICS, -1);
            if (StatButtons[c].BitmapClicked) RoomBm.BlitFromT (*StatButtons[c].BitmapClicked, BlitPos);
         }
         else if (!IsDialogOpen() && !MenuIsOpen() && gMousePosition.IfIsWithin (StatButtons[c].HotArea.left+off.x, StatButtons[c].HotArea.top+off.y, StatButtons[c].HotArea.right+off.x, StatButtons[c].HotArea.bottom+off.y))
         {
            //Highlight:
            if (StatButtons[c].IsToggle && StatButtons[c].LastClicked!=0)
            {
					if (StatButtons[c].BitmapSuperHi)
					{
						if (c==0) ExitBm = StatButtons[c].BitmapSuperHi;
						RoomBm.BlitFromT (*StatButtons[c].BitmapSuperHi, BlitPos);
					}
            }
            else if (StatButtons[c].BitmapHi)
            {
               if (c==0) ExitBm = StatButtons[c].BitmapHi;
               RoomBm.BlitFromT (*StatButtons[c].BitmapHi, BlitPos);
            }

            if (StatButtons[c].HelpId>=0)
               SetMouseLook ((c==0)?CURSOR_EXIT:CURSOR_HOT, StatButtons[c].HelpId, ROOM_STATISTICS, StatButtons[c].Id);
            else
               SetMouseLook ((c==0)?CURSOR_EXIT:CURSOR_HOT, -StatButtons[c].HelpId, Sim.Players.Players[-StatButtons[c].HelpId-1000].AirlineX+" ("+Sim.Players.Players[-StatButtons[c].HelpId-1000].NameX+")", ROOM_STATISTICS, StatButtons[c].Id);
         }
         else
         {
            //Kein Highlight, aber vielleicht geklickt?
            if ((StatButtons[c].IsToggle && StatButtons[c].LastClicked!=0) || (!StatButtons[c].IsToggle && CurrentTime-StatButtons[c].LastClicked<ClickHighlightTime) || (DropDownPos.y!=329 && StatButtons[c].HelpId<0))
            {
               if (StatButtons[c].BitmapClicked || (DropDownPos.y!=329 && StatButtons[c].HelpId<0))
               {
                  if (c==0) ExitBm = StatButtons[c].BitmapClicked;
                  RoomBm.BlitFromT (*StatButtons[c].BitmapClicked, BlitPos);
               }
            }
            else //ganz normal:
            {
               if (StatButtons[c].BitmapNormal)
               {
                  if (c==0) ExitBm = StatButtons[c].BitmapNormal;
                  RoomBm.BlitFromT (*StatButtons[c].BitmapNormal, BlitPos);
               }
            }
         }
      }
   }

   RoomBm.BlitFrom (TopSaver, 374, 0);


	// Den wievielte Tag spielen wir gerade?
   CString output;
   if (timeGetTime()%6000>3000 && (Sim.Difficulty==DIFF_ADDON03 || Sim.Difficulty==DIFF_ADDON04 || Sim.Difficulty==DIFF_ADDON06))
   {
      SLONG n;

      switch (Sim.Difficulty)
      {
         case DIFF_ADDON03: n = TARGET_DAYS      - Sim.Date; break;
         case DIFF_ADDON04: n = TARGET_MILESDAYS - Sim.Date; break;
         case DIFF_ADDON06: n = TARGET_DAYS      - Sim.Date; break;
      }

	   output = bprintf (StandardTexte.GetS (TOKEN_STAT, 9010+(n==1)), n);
   }
   else
   {
	   output = bitoa(Sim.Date + 1);
	   output += ". ";
	   output += StandardTexte.GetS (TOKEN_STAT, 9000);
   }
	
	long length = FontDialogPartner.GetWidth((LPSTR)(LPCSTR)output, output.GetLength());
	long xPos   = 35 + ((160-35)>>1)	- (length>>1);
	RoomBm.PrintAt (output, FontDialogPartner, TEC_FONT_LEFT, xPos, 15, xPos + length + 5, 45);

	// Zeichnet die Einträge einer Gruppe. Ist der Mauscursor
	// über einem Eintrag wird dieser focusiert.
	long xOffset[] = { 0, -18, -11, -11, -11, -11 };
	long xOffset2[] = { -11, -18, -11, -11, -11, -11 };
	RECT rc = { 45, 56, 155, 66 };

	_selectedItem = -1;
	for (short i = 0 ; i < MAX_ITEMS ; i++)
	{
		if (_iArray[_group][i].typOfItem != TYP_LINEFEED)
		{
			word group = _iArray[_group][i].typOfItem;
			if (group >= 20)
				group = 0;

			CString text = StandardTexte.GetS ("STAT", _iArray[_group][i].textId);
			RoomBm.PrintAt (text, (!group) ? FontSmallBlack : FontSmallPlastic, TEC_FONT_LEFT, rc.left + xOffset[group], rc.top, rc.right, rc.bottom);

			if (_iArray[_group][i].visible == true && group == 0)
				RoomBm.BlitFromT (Haeckchen, rc.left - 11, rc.top);

			if (_selectedItem == -1)
				if (!IsDialogOpen() && !MenuIsOpen() && CheckCursorHighlight (gMousePosition, CRect (rc.left + xOffset2[group], rc.top-5, rc.right, rc.bottom+4), ColorOfFontBlack))
            {
					_selectedItem = i;
					if (_oldSelectedItem != _selectedItem)
					{
						_oldSelectedItem = _selectedItem;
						_fRepaint = true;
					}

               if (DropDownPos.y==0)
						ColorFX.BlitTrans (HighlightBar.pBitmap, RoomBm.pBitmap, XY(190, rc.top-3-4), NULL, 6);
            }
				else
				{
					if (_oldSelectedItem != _selectedItem)
					{
						_oldSelectedItem = _selectedItem;
						_fRepaint = true;
					}
				}
		}

		rc.top   += LINE_DISTANCE;
		rc.bottom = rc.top + 10;
	}


	// Die Werte sollten gelegentlich neu gezeichnet werden
	static int tickers = 0;
	if (_fRepaint || (GetTickCount() - tickers > 1500 && DropDownSpeed == 0))
	{
		tickers = GetTickCount();
		_fRepaint = false;

		if (DropDownPos.y == 0)				// Ganz oben...
			RepaintTextWindow();
		else										// Unten
			RepaintGraphWindow();
	}

   //Statuszeile zeichnen:
   CStdRaum::PostPaint ();

   //Ein Patch: Die Exit-Bitmap nach der Statuszeile erneut zeichnen:
   if (ExitBm)
   {
      PrimaryBm.PrimaryBm.SetClipRect(&CRect(0,0,640,480));
      PrimaryBm.BlitFrom (*ExitBm, ExitBmPos);
   }

   CStdRaum::PumpToolTips ();
}

//--------------------------------------------------------------------------------------------
// Berechnet einige Sachen für den Graphen
//--------------------------------------------------------------------------------------------
void CStatistik::CalcGraph()
{
	__int64 min = 0;
	__int64 max = 0;

	// Max-Min Werte
	if (_days <= 30)
	{
		for (short i = 0 ; i < MAX_ITEMS ; i++)
			if (_iArray[_group][i].typOfItem >= TYP_VALUE && _iArray[_group][i].visible)
				for (int p = 0 ; p < 4 ; p++)
               if (!Sim.Players.Players[p].IsOut)
					   if (_playerMask[p])
						   for (long d = 0 ; d <= _days ; d++)
						   {
                        if (_iArray[_group][i].typOfItem == TYP_PERCENT)
                        {
                           if (Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i-1].define].GetAtPastDay(d))
                           {
							         min = Min (__int64(Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i].define].GetAtPastDay(d)*__int64(100)/Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i-1].define].GetAtPastDay(d)), min);
							         max = Max (__int64(Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i].define].GetAtPastDay(d)*__int64(100)/Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i-1].define].GetAtPastDay(d)), max);
                           }
                        }
                        else
                        {
							      min = Min (__int64(Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i].define].GetAtPastDay(d)), min);
							      max = Max (__int64(Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i].define].GetAtPastDay(d)), max);
                        }
						   }
	}
	else
	{
		for (short i = 0 ; i < MAX_ITEMS ; i++)
			if (_iArray[_group][i].typOfItem >= TYP_VALUE && _iArray[_group][i].visible)
				for (int p = 0 ; p < 4 ; p++)
               if (!Sim.Players.Players[p].IsOut)
					   if (_playerMask[p])
						   for (long d = 0 ; d <= min(11, ((_days+29)/30)) ; d++)
						   {
                        if (_iArray[_group][i].typOfItem == TYP_PERCENT)
                        {
                           if (Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i-1].define].GetAtPastMonth(d))
                           {
							         min = Min (__int64(Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i].define].GetAtPastMonth(d)*__int64(100)/Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i-1].define].GetAtPastMonth(d)), min);
							         max = Max (__int64(Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i].define].GetAtPastMonth(d)*__int64(100)/Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i-1].define].GetAtPastMonth(d)), max);
                           }
                        }
                        else
                        {
							      min = Min (__int64(Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i].define].GetAtPastMonth(d)), min);
							      max = Max (__int64(Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i].define].GetAtPastMonth(d)), max);
                        }
						   }
	}

	double summe = double(max) + double(-min);
	_yGraph = (double)G_HEIGHT / summe;
	_yAxis  = (double)-min;
}

//--------------------------------------------------------------------------------------------
//Zeichnet das Menü mit dem Graphen neu:
//--------------------------------------------------------------------------------------------
void CStatistik::RepaintGraphWindow (void)
{
	long	x1, y1, x2, y2 = -1;
	long	value;
	bool	fDrawAxis = false;

   SB_Hardwarecolor red = DropDownBm.pBitmap->GetHardwarecolor (0xff0000);
   DropDownBm.BlitFrom (DropDownBackgroundBm);


	// Ausgabe des Zoom-Faktors in Tage oder Monaten
	CString output;
	if (_days <= 30)
	{
		output = bitoa(_days);
		output += " ";
		output += StandardTexte.GetS (TOKEN_STAT, 9001);
	}
	else
	{
		output = bitoa(_days/30);
		output += " ";
		output += StandardTexte.GetS (TOKEN_STAT, 9002);
	}

	DropDownBm.PrintAt (output, StatFonts[1], TEC_FONT_LEFT, 263, 12, G_RIGHT, G_BOTTOM);

	if (!Sim.Date)	// Am ersten Tag keine Statistik möglich
		return;

	// Y-Axis
	long yAxis = (long)(_yAxis * _yGraph);

	_xGraph = (double)G_WIDTH / (double)_days;


	long days = _days;
	if (days > (Sim.Date+1))
		days = Sim.Date+1;

	DropDownBm.pBitmap->SetClipRect(&CRect(G_LEFT,G_TOP,G_RIGHT,G_BOTTOM+1));

	// Max-Min Werte
	if (_days <= 30)
	{
		for (short i = 0 ; i < MAX_ITEMS ; i++)
			if (_iArray[_group][i].typOfItem >= TYP_VALUE && _iArray[_group][i].visible)
				for (int p = 0 ; p < 4 ; p++)
               if (!Sim.Players.Players[p].IsOut)
					   if (_playerMask[p])
					   {
                     if (_iArray[_group][i].typOfItem == TYP_PERCENT)
                        if (Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i-1].define].GetAtPastDay(0))
                           value = (long)((double)Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i].define].GetAtPastDay(0)*__int64(100)/Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i-1].define].GetAtPastDay(0) * _yGraph);
                        else
                           value = 0;
                     else
						      value = (long)(double(Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i].define].GetAtPastDay(0)) * _yGraph);

						   x2 = G_RIGHT;
						   y2 = G_BOTTOM - yAxis - value;

						   for (long d = 1 ; d <= days ; d++)
						   {
                        if (_iArray[_group][i].typOfItem == TYP_PERCENT)
                           if (Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i-1].define].GetAtPastDay(d))
                              value = (long)((double)Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i].define].GetAtPastDay(d)*__int64(100)/Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i-1].define].GetAtPastDay(d) * _yGraph);
                           else
                              value = 0;
                        else
						         value = (long)(double(Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i].define].GetAtPastDay(d)) * _yGraph);

							   x1 = G_RIGHT - (long)((double)d * _xGraph);
							   y1 = G_BOTTOM - yAxis - value;

							   if (_selectedItem != -1 && _selectedItem != i && _iArray[_group][_selectedItem].typOfItem != TYP_GROUP)
							   {
								   DropDownBm.pBitmap->Line (x1, y1, x2, y2, DropDownBm.pBitmap->GetHardwarecolor(DarkColors[p]));
							   }
							   else
							   {
								   DropDownBm.pBitmap->Line (x1, y1, x2, y2, DropDownBm.pBitmap->GetHardwarecolor(AktienKursLineColor[p]));
							   }

							   DropDownBm.BlitFrom (PobelBms[p], x2-2, y2-2);
							   DropDownBm.BlitFrom (PobelBms[p], x1-2, y1-2);

							   x2 = x1;
							   y2 = y1;
							   fDrawAxis = true;
						   }
					   }
	}
	else if (days / 30)
	{
		long month = min (11, (days+29/30));
	   _xGraph = (double)G_WIDTH / (((double)_days)/30);

		for (short i = 0 ; i < MAX_ITEMS ; i++)
			if (_iArray[_group][i].typOfItem >= TYP_VALUE && _iArray[_group][i].visible)
				for (int p = 0 ; p < 4 ; p++)
               if (!Sim.Players.Players[p].IsOut)
					   if (_playerMask[p])
					   {
                     if (_iArray[_group][i].typOfItem == TYP_PERCENT)
                        if (Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i-1].define].GetAtPastMonth(0))
                           value = (long)((double)Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i].define].GetAtPastMonth(0)*__int64(100)/Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i-1].define].GetAtPastMonth(0) * _yGraph);
                        else
                           value = 0;
                     else
						      value = (long)(double(Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i].define].GetAtPastMonth(0)) * _yGraph);

						   x2 = G_RIGHT;
						   y2 = G_BOTTOM - yAxis - value;

						   for (long d = 1 ; d <= month; d++)
						   {
                        if (_iArray[_group][i].typOfItem == TYP_PERCENT)
                           if (Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i-1].define].GetAtPastMonth(d))
                              value = (long)((double)Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i].define].GetAtPastMonth(d)*__int64(100)/Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i-1].define].GetAtPastMonth(d) * _yGraph);
                           else
                              value = 0;
                        else
						         value = (long)(double(Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i].define].GetAtPastMonth(d)) * _yGraph);

							   x1 = G_RIGHT - (long)((double)d * _xGraph);
							   y1 = G_BOTTOM - yAxis - value;

                        if (x1>G_LEFT || x2>G_LEFT)
							      if (_selectedItem != -1 && _selectedItem != i && _iArray[_group][_selectedItem].typOfItem != TYP_GROUP)
							      {
								      DropDownBm.pBitmap->Line (x1, y1, x2, y2, DropDownBm.pBitmap->GetHardwarecolor(DarkColors[p]));
							      }
							      else
							      {
								      DropDownBm.pBitmap->Line (x1, y1, x2, y2, DropDownBm.pBitmap->GetHardwarecolor(AktienKursLineColor[p]));
							      }

							   DropDownBm.BlitFrom (PobelBms[p], x2-2, y2-2);
							   DropDownBm.BlitFrom (PobelBms[p], x1-2, y1-2);

							   x2 = x1;
							   y2 = y1;
							   fDrawAxis = true;
						   }
					   }
	}

	DropDownBm.pBitmap->InitClipRect();

	// Die Y-Achse zeichnen
	if (fDrawAxis)
	{
		long yPos = G_BOTTOM - yAxis;
		DropDownBm.pBitmap->Line (G_LEFT, yPos, G_RIGHT, yPos, DropDownBm.pBitmap->GetHardwarecolor(0xffffff));
	}
}

//--------------------------------------------------------------------------------------------
//Zeichnet das Menü mit dem Text neu:
//--------------------------------------------------------------------------------------------
void CStatistik::RepaintTextWindow (void)
{
   int p;

   TextTableBm.BlitFrom (PicBitmap, -191, -40);

	for (p = 0 ; p < 4 ; p++)
      if (!Sim.Players.Players[p].IsOut)
	   {
		   CString	    output;
		   __int64		summe = 0;
		   __int64		differenz = 0;

		   RECT rc;
		   rc.left   = p * 109;
		   rc.top    = 16;
		   rc.right  = rc.left + 109;
		   rc.bottom = rc.top + 10;

		   for (short i = 0 ; i < MAX_ITEMS ; i++)
		   {
			   if (_iArray[_group][i].typOfItem > TYP_GROUP)
			   {
				   __int64 val = Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i].define].GetAtPastDay(0);
				   switch (_iArray[_group][i].typOfItem)
				   {
					   case TYP_VALUE:
						   output = bprintf("%I64i", val);

						   if (_iArray[_group][i].visible)
							   summe += val;
						   break;

					   case TYP_CURRENCY:
						   output = Einheiten[EINH_DM].bString64 (val);
						   
						   if (_iArray[_group][i].visible)
							   summe += val;
						   break;

                  case TYP_SINGLE_PERCENT:
						   output = Einheiten[EINH_P].bString64 (val);
						   
						   if (_iArray[_group][i].visible)
							   summe += val;
						   break;

					   case TYP_PERCENT:
                     if (Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i-1].define].GetAtPastDay(0))
						      output = Einheiten[EINH_P].bString64 (val*100/Sim.Players.Players[(int)p].Statistiken[(int)_iArray[_group][i-1].define].GetAtPastDay(0));
                     else
                        output = "0%";
						   
						   if (_iArray[_group][i].visible)
							   summe += val;
						   break;

					   case TYP_SUM_CURR:
						   output = Einheiten[EINH_DM].bString64(summe);

						   differenz += summe;
						   summe = 0;
						   break;

					   case TYP_SUM_DIFF:
						   output = Einheiten[EINH_DM].bString64(differenz);
						   differenz = 0;
						   break;

					   case TYP_SHAREVALUE:
					   {
						   __int64 shares = 0;

						   if (_iArray[_group][2].visible)
						   {
							   if (_iArray[_group][3].visible) shares += Sim.Players.Players[(int)p].Statistiken[STAT_AKTIEN_SA].GetAtPastDay(0) * Sim.Players.Players[(int)0].Statistiken[STAT_AKTIENKURS].GetAtPastDay(0);
							   if (_iArray[_group][4].visible) shares += Sim.Players.Players[(int)p].Statistiken[STAT_AKTIEN_FL].GetAtPastDay(0) * Sim.Players.Players[(int)1].Statistiken[STAT_AKTIENKURS].GetAtPastDay(0);
							   if (_iArray[_group][5].visible) shares += Sim.Players.Players[(int)p].Statistiken[STAT_AKTIEN_PT].GetAtPastDay(0) * Sim.Players.Players[(int)2].Statistiken[STAT_AKTIENKURS].GetAtPastDay(0);
							   if (_iArray[_group][6].visible) shares += Sim.Players.Players[(int)p].Statistiken[STAT_AKTIEN_HA].GetAtPastDay(0) * Sim.Players.Players[(int)3].Statistiken[STAT_AKTIENKURS].GetAtPastDay(0);
						   }


                     //if (shares>2147483647) shares=2147483647;    //Overflow verhindern
						   output = Einheiten[EINH_DM].bString64(shares);
						   break;
					   }
				   }

				   TextTableBm.PrintAt (output, StatFonts[p], TEC_FONT_RIGHT, rc.left, rc.top, rc.right - 5, rc.bottom);
			   }

			   rc.top += LINE_DISTANCE;
			   rc.bottom = rc.top + 10;
		   }
	   }
}

//--------------------------------------------------------------------------------------------
//void CStatistik::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CStatistik::OnLButtonDown(UINT nFlags, CPoint point)
{
   XY      RoomPos;
   SLONG   c;
   PLAYER &qPlayer = Sim.Players.Players[(SLONG)PlayerNum];

   DefaultOnLButtonDown ();

   if (!ConvertMousePosition (point, &RoomPos))
   {
	   CStdRaum::OnLButtonDown(nFlags, point);
      return;
   }

   if (!PreLButtonDown (point))
   {
      if (MouseClickArea==ROOM_STATISTICS)
      {
         //Die Internen Daten des Buttons anpassen:
         for (c=StatButtons.AnzEntries()-1; c>=0; c--)
            if (StatButtons[c].Id==MouseClickId)
               if (StatButtons[c].IsToggle) StatButtons[c].LastClicked^=1;
                                       else StatButtons[c].LastClicked=timeGetTime();

         //Hier kann man auf einen Button-Click reagieren:
         switch (MouseClickId)
         {
            case 999: qPlayer.LeaveRoom(); break;
            case 100:
               if (DropDownPos.y>0)
               {
                  DropDownSpeed = -25;
                  StatButtons[1].BitmapHi=&UpDownArrows[1];
						RepaintTextWindow();
               }
               else
               {
                  DropDownSpeed = 25;
                  StatButtons[1].BitmapHi      = NULL;
                  StatButtons[1].BitmapClicked = &UpDownArrows[0];
                  StatButtons[1].BitmapNormal  = &UpDownArrows[0];
						RepaintGraphWindow();
               }
               break;

				case 110:	_playerMask[0] = !_playerMask[0];	CalcGraph(); _fRepaint = true; break;
				case 111:	_playerMask[1] = !_playerMask[1];	CalcGraph(); _fRepaint = true; break;
				case 112:	_playerMask[2] = !_playerMask[2];	CalcGraph(); _fRepaint = true; break;
				case 113:	_playerMask[3] = !_playerMask[3];	CalcGraph(); _fRepaint = true; break;

				case 120:
					for (c=StatButtons.AnzEntries()-1; c>=0; c--)
					{
						if (StatButtons[c].Id==120) StatButtons[c].LastClicked=1;
						if (StatButtons[c].Id==122 || StatButtons[c].Id==121)	StatButtons[c].LastClicked=0;
					}

					_group = 0;
					RepaintTextWindow ();
					CalcGraph();
					RepaintGraphWindow ();
					break;

				case 121:
					for (c=StatButtons.AnzEntries()-1; c>=0; c--)
					{
						if (StatButtons[c].Id==121) StatButtons[c].LastClicked=1;
						if (StatButtons[c].Id==120 || StatButtons[c].Id==122)	StatButtons[c].LastClicked=0;
					}

					_group = 1;
					RepaintTextWindow ();
					CalcGraph();
					RepaintGraphWindow ();
					break;

				case 122:
					for (c=StatButtons.AnzEntries()-1; c>=0; c--)
					{
						if (StatButtons[c].Id==122) StatButtons[c].LastClicked=1;
						if (StatButtons[c].Id==120 || StatButtons[c].Id==121)	StatButtons[c].LastClicked=0;
					}

					_group = 2;
					RepaintTextWindow ();
					CalcGraph();
					RepaintGraphWindow ();
					break;

				// Zoom out
				case 130:
				{
					if (_days > days[0])
						for (word i = 0 ; i < (sizeof(days)/sizeof(days[0])) ; i++)
							if (days[i] == _days)
							{
								_newDays = days[i-1];
								break;
							}
				}
					break;

				// Zoom in
				case 131:
					if (_days < days[ (sizeof(days)/sizeof(days[0])) -1 ])
						for (word i = 0 ; i < (sizeof(days)/sizeof(days[0])) ; i++)
							if (days[i] == _days)
							{
								_newDays = days[i+1];
								break;
							}
					break;
         }
      }

		// Prüfen ob ein (Font-)Item angeklickt wurde
		if (_selectedItem != -1)
		{
			// Falls das gewählte Icon eine Gruppe war diese komplett
			// ein- oder ausschalten.
			if (_iArray[_group][_selectedItem].typOfItem == TYP_GROUP)
			{
				while (++_selectedItem < MAX_ITEMS && _iArray[_group][_selectedItem].typOfItem != TYP_GROUP)
					_iArray[_group][_selectedItem].visible = !_iArray[_group][_selectedItem].visible;
			}
			else
			{
            if ((GetAsyncKeyState (VK_SHIFT)&0xff00))
            {
               SLONG c=_selectedItem;

               while (c>=0 && _iArray[_group][c].typOfItem!=TYP_GROUP) c--;

				   while (++c<MAX_ITEMS && _iArray[_group][c].typOfItem!=TYP_GROUP)
					   _iArray[_group][c].visible = FALSE;

               _iArray[_group][_selectedItem].visible = TRUE;
            }
            else _iArray[_group][_selectedItem].visible = !_iArray[_group][_selectedItem].visible;
			}

			_selectedItem = -1;
			RepaintTextWindow();
			
			CalcGraph();
			RepaintGraphWindow();
		}
		else
			CStdRaum::OnLButtonDown(nFlags, point);
   }
}

//--------------------------------------------------------------------------------------------
//void CStatistik::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CStatistik::OnRButtonDown(UINT nFlags, CPoint point) 
{
   DefaultOnRButtonDown ();

   //Außerhalb geklickt? Dann Default-Handler!
   if (point.x<WinP1.x || point.y<WinP1.y || point.x>WinP2.x || point.y>WinP2.y)
   {
      CWnd::OnRButtonDown(nFlags, point);
      return;
   }
   else
   {
      if (MenuIsOpen())
      {
         MenuRightClick (point);
      }
      else
      {
         if (!IsDialogOpen() && point.y<440)
            Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();

         CStdRaum::OnRButtonDown(nFlags, point);
      }
   }
}

//--------------------------------------------------------------------------------------------
//Initialisieren:
//--------------------------------------------------------------------------------------------
void CStatButton::ReSize (SLONG Id, SLONG  HelpId, CRect HotArea, XY BitmapOffset, XY *BitmapOffset2, SBBM *BitmapNormal, SBBM *BitmapHi, SBBM *BitmapClicked, SBBM *BitmapSuperHi, BOOL IsToggle)
{
   CStatButton::Id            = Id;
   CStatButton::HelpId        = HelpId;
   CStatButton::HotArea       = HotArea;
   CStatButton::BitmapOffset  = BitmapOffset;
   CStatButton::BitmapOffset2 = BitmapOffset2;
   CStatButton::BitmapNormal  = BitmapNormal;
   CStatButton::BitmapHi      = BitmapHi;
   CStatButton::BitmapClicked = BitmapClicked;
   CStatButton::BitmapSuperHi = BitmapSuperHi;
   CStatButton::IsToggle      = IsToggle;
   CStatButton::LastClicked   = 0;
}
