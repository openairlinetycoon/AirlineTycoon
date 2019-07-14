// GameFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// GameFrame frame

class GameFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(GameFrame)
public:
	GameFrame();           // protected constructor used by dynamic creation

// Attributes
public:
   GfxLib  *pGLibPause;
   SBBM     PauseBm;
   SLONG    PauseFade;

// Operations
public:
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

	// Generated message map functions
   afx_msg LONG OnMciNotify(WPARAM wParam, LPARAM lParam);
	//{{AFX_MSG(GameFrame)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
   afx_msg void OnCaptureChanged(CWnd* pWnd);
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
	DECLARE_MESSAGE_MAP()

   friend class CStdRaum;
};

/////////////////////////////////////////////////////////////////////////////
