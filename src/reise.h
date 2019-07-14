//============================================================================================
// Reise.cpp : Der Reisebuero-Schalter
//============================================================================================

/////////////////////////////////////////////////////////////////////////////
// CReisebuero window
/////////////////////////////////////////////////////////////////////////////
class CReisebuero : public CStdRaum
{
// Construction
public:
	CReisebuero(BOOL bHandy, ULONG PlayerNum);

// Attributes
public:
   SLONG       LastTip;
   SBBM        ZettelBms[10];
   SBBM        TipBm;
   SBBMS       MapPlaneBms[5];
   GfxLib     *pMenuLib;
   GfxLib     *pZettelLib;

   SBFX        WellenFx;

   SLONG       KommVar;
   SLONG       KommVar2;
   SLONG       KommMonitor;

   CSmackerPerson SP_Girl;
   CSmackerPerson SP_Monitor;
   CSmackerPerson SP_Schlange;
   CSmackerPerson SP_Spinne;

// Operations
public:
   void RepaintZettel (SLONG n);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReisebuero)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CReisebuero();

	// Generated message map functions
protected:
	//{{AFX_MSG(CReisebuero)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};