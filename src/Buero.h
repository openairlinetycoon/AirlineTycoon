// CBuero.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Buero window

class CBuero : public CStdRaum
{
// Construction
public:
	CBuero(BOOL bHandy, ULONG PlayerNum);

// Attributes
public:
   SBBM    DarkBm;
   SBBM    NoSaboBm;
   GfxLib *pGLibDark;

   CSmackerPerson SP_Player;
   CSmackerPerson SP_Ticker;

   SLONG       KommVarLetters;
   SLONG       KommVarTelefon;
   SLONG       KommVarCloseLetter;

   long        OfficeState;
   CAnimation  UhrZeigerAnim;

   SBBM  LetterBm;
   SBBM  DoorOpaqueBm;
   SBBM  DoorTransBm;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Buero)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBuero();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBuero)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
