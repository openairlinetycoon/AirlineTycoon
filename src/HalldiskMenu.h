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
	virtual void OnPaint();
	virtual void OnClose();
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnSetCursor(void* pWnd, UINT nHitTest, UINT message);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
