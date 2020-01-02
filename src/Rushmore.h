//============================================================================================
// Rushmore.h : Der Nasa-Shop
//============================================================================================

class CRushmore : public CStdRaum
{
// Construction
public:
	CRushmore(BOOL bHandy, ULONG PlayerNum);

   SBBMS   LogoBms;
   SBBM    TextBm;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRushmore)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRushmore();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRushmore)
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnPaint();
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
