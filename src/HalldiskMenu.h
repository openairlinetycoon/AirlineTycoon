// HallDiskMenu.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// HallDiskMenu window

class HallDiskMenu : public CStdRaum
{
// Construction
public:
	HallDiskMenu(BOOL bHandy, SLONG PlayerNum);

// Attributes
public:
   SBBM           MenuBm;
   GfxLib        *pMenuLib;
   BOOL           bFiles[10*10];

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(HallDiskMenu)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~HallDiskMenu();

	// Generated message map functions
protected:
	//{{AFX_MSG(HallDiskMenu)
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
