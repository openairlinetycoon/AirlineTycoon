//============================================================================================
// LastMin.cpp : Der Last-Minute Schalter
//============================================================================================

/////////////////////////////////////////////////////////////////////////////
// CLastMinute window
/////////////////////////////////////////////////////////////////////////////
class CLastMinute : public CStdRaum
{
// Construction
public:
	CLastMinute(BOOL bHandy, ULONG PlayerNum);

// Attributes
public:
   SLONG       LastTip;
   SBBM        ZettelBms[10];
   SBBM        TipBm;
   SBBMS       MapPlaneBms[5];
   GfxLib     *pMenuLib;

   CSmackerPerson SP_Girl;
   CSmackerPerson SP_Monitor;
   CSmackerPerson SP_Kran;
   SLONG          MoveKran;
   SLONG          NewKranDir;
   SLONG          KranArrived;

   SLONG          KommMonitor;

   CAnimation     CursorAnim;

// Operations
public:
   void RepaintZettel (SLONG n);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLastMinute)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLastMinute();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLastMinute)
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnPaint();
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};