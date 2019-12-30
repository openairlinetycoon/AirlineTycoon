//============================================================================================
// Security.h : Der Security-Raum!
//============================================================================================

class CSecurity : public CStdRaum
{
// Construction
public:
	CSecurity(BOOL bHandy, ULONG PlayerNum);

// Data
public:
   CSmackerPerson SP_Secman;
   CSmackerPerson SP_Tuer;
   CSmackerPerson SP_Gasmaske;
   CSmackerPerson SP_Cam1;
   CSmackerPerson SP_Cam2;
   CSmackerPerson SP_Cam3;
   CSmackerPerson SP_Cam4;
   CSmackerPerson SP_Cam5;
   CSmackerPerson SP_Spion;
   CSmackerPerson SP_Tresor;

   SBFX           BackFx;

   SB_CFont       FontBankBlack;
   SB_CFont       FontBankRed;

   SLONG          KommVar;         //Work-Around

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSecurity)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSecurity();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSecurity)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
