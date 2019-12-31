// AskBrick.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AskBrick window

class AskBrick : public CStdRaum
{
// Construction
public:
	AskBrick(BOOL bHandy, SLONG PlayerNum, ULONG Group, ULONG *rc1);

// Attributes
public:
   SBBM           AskBrickBm;
   GfxLib        *pMenuLib;

private:
   ULONG Group;
   ULONG *rc1;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AskBrick)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~AskBrick();

	// Generated message map functions
protected:
	//{{AFX_MSG(AskBrick)
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnClose();
	afx_msg BOOL OnSetCursor(void* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
