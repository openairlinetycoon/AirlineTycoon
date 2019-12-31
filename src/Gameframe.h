// GameFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// GameFrame frame

class GameFrame
{
public:
	GameFrame();           // protected constructor used by dynamic creation

// Attributes
public:
   GfxLib  *pGLibPause;
   SBBM     PauseBm;
   SLONG    PauseFade;

   SDL_Event Mess;
   SDL_Window *m_hWnd;

// Operations
public:
   void Invalidate (void);
   void RePostMessage (CPoint point);
   void RePostClick (SLONG PlayerNum, UINT message, WPARAM wParam, LPARAM lParam);
   void PrepareFade (void);
   bool Pause(bool fPause);	// AG

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
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(void* pDC);
#if _MSC_VER >= 1400
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#else
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#endif
   afx_msg BOOL OnSetCursor(void* pWnd, UINT nHitTest, UINT message);
   afx_msg void OnCaptureChanged(void* pWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
   afx_msg BOOL OnHelpInfo(HELPINFO* lpHelpInfo);
	//}}AFX_MSG
   afx_msg void OnSysKeyDown( UINT, UINT, UINT );
   afx_msg void OnSysKeyUp( UINT, UINT, UINT );

   friend class CStdRaum;
};

/////////////////////////////////////////////////////////////////////////////
