// Options.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Options window

class Options : public CStdRaum
{
	// Construction
public:
	Options(BOOL bHandy, SLONG PlayerNum);

private:
	BOOL          ChangedDisplay;
	enum PAGE_TYPE : UBYTE {
		GRAPHICS,
		MUSIC,
		SOUND,
	};

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
	FBUFFER<SBBM>* CursorBms;     //Die Eingabemarkierung
	BOOL            BlinkState;

	SBFX            AmbientFX;
	SBFX            EffektFX;
	SBFX            PlaneFX;
	SBFX            TalkFX;
	SBFX            DurchsagenFX;
	SBFX            ClickFx;

	// Operations
public:
	void ReloadBitmaps(void);
	void RefreshKlackerField(void);
	void UpdateSavegameNames(void);

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
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnPaint();
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnTimer(UINT nIDEvent);
	virtual void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
