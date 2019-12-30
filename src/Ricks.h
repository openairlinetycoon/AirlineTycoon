//============================================================================================
// Ricks.h : Der Schalter und das Hinterzimmer von Ricks!
//============================================================================================

class CRicks : public CStdRaum
{
// Construction
public:
	CRicks(BOOL bHandy, ULONG PlayerNum);

// Data
public:
   CSmackerPerson SP_Rick;
   CSmackerPerson SP_Neon;
   CSmackerPerson SP_TrinkerMund;
   CSmackerPerson SP_TrinkerAugen;

   SBFX           CoffeeFx;
   SBFX           GeschirrFx;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRicks)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRicks();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRicks)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
