// CCredits.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCredits window

class CCredits : public CStdRaum
{
// Construction
public:
	CCredits(BOOL bHandy, SLONG PlayerNum);

// Attributes
private:
   GfxLib         *pGLibCredits;
   SBBM            Background;
   SBBM            Left, Right;
   SBBM            TextLines[25];
   BOOL            TimerFailure;
   SLONG           ScrollPos;
   SLONG           LastTime;
   TEXT_STYLE      StyleCredits;

// Operations
public:
   void ReloadBitmap (void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCredits)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCredits();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCredits)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetCursor(void* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
