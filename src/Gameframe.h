// GameFrame.h : header file
//

enum InputFlags {
	None = 0,
	FromTextInput = 1,
};

/////////////////////////////////////////////////////////////////////////////
// GameFrame frame

class GameFrame {
public:
	GameFrame();           // protected constructor used by dynamic creation

// Attributes
public:
	GfxLib* pGLibPause;
	SBBM     PauseBm;
	SLONG    PauseFade;

	SDL_Event Mess;
	SDL_Window* m_hWnd;

	// Operations
public:
	void Invalidate(void);
	void UpdateWindow();
	void UpdateFrameSize();
	void RePostMessage(CPoint point);
	void RePostClick(SLONG PlayerNum, UINT message, WPARAM wParam, LPARAM lParam);
	void PrepareFade(void);
	bool Pause(bool fPause);	// AG
	void TranslatePointToGameSpace(CPoint* p);
	void TranslatePointToScreenSpace(int& x, int& y);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(GameFrame)
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~GameFrame();

	void ProcessEvent(const SDL_Event& event);

	// Generated message map functions
	//{{AFX_MSG(GameFrame)
	virtual void OnPaint();
	virtual BOOL OnEraseBkgnd(void* pDC);
	virtual void OnActivateApp(BOOL bActive, DWORD hTask);
	virtual BOOL OnSetCursor(void* pWnd, UINT nHitTest, UINT message);
	virtual void OnCaptureChanged(void* pWnd);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnHelpInfo(void* lpHelpInfo);
	//}}AFX_MSG
	virtual void OnSysKeyDown(UINT, UINT, UINT);
	virtual void OnSysKeyUp(UINT, UINT, UINT);

	friend class CStdRaum;
};

/////////////////////////////////////////////////////////////////////////////
