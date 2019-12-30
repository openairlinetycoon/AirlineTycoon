//============================================================================================
// Nasa.h : Der Nasa-Shop
//============================================================================================

class CNasa : public CStdRaum
{
// Construction
public:
	CNasa(BOOL bHandy, ULONG PlayerNum);

   SLONG KommVarTippNow;
   SLONG KommAlien;

   SBFX  FunkFx;

   CSmackerPerson SP_Mann;
   CSmackerPerson SP_Alien;
   CSmackerPerson SP_Kugelschreiber;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNasa)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNasa();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNasa)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
