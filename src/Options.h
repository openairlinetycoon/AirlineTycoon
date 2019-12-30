// Options.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Options window

class Options : public CStdRaum
{
// Construction
public:
	Options(BOOL bHandy, SLONG PlayerNum);

// Attributes
public:
   BOOL           TimerFailure;
   UBYTE          PageNum;       //Seite 1 oder 2
   KLACKER        KlackerTafel;  //Die Tafel auf der alles angezeigt wird
   ULONG          PlayerNum;

   SDL_TimerID     TimerId;
   SB_CFont        VersionFont;

   SLONG           CursorX;
   SLONG           CursorY;
   CString         SavegameNames[12]; 
   BOOL            SavenamesValid[12];
   CString         SavegameInfos[12]; 
   FBUFFER<SBBM>  *CursorBms;     //Die Eingabemarkierung
   BOOL            BlinkState;

   SBFX            AmbientFX;
   SBFX            EffektFX;
   SBFX            PlaneFX;
   SBFX            TalkFX;
   SBFX            DurchsagenFX;
   SBFX            ClickFx;

// Operations
public:
   void ReloadBitmaps (void);
   void RefreshKlackerField(void);
   void UpdateSavegameNames (void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Options)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~Options();

	// Generated message map functions
protected:
	//{{AFX_MSG(Options)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
