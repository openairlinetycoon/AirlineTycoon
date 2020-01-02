//============================================================================================
// Personal.h - Das Personalbüro des Spielers
//============================================================================================

/////////////////////////////////////////////////////////////////////////////
// CPersonal window

class CPersonal : public CStdRaum
{
// Construction
public:
	CPersonal(BOOL bHandy, ULONG PlayerNum);

// Attributes
public:
   SLONG          PaperDropping;
   SLONG          PaperOnTable;
   SLONG          StapelBrief;
   SLONG          Zeitung;
   SLONG          KommVar;         //Work-Around
   SLONG          KommVar2;        //Work-Around

   SLONG          FlugbahnType;
   SLONG          FlugbahnCounter;

   SBBM           KugelBm;
   SBBM           WildBm;
   SBBM           LetterBm;
   SBBM           StapelBriefBm;
   SBBM           NoSaboBm;

   CSmackerPerson SP_Mann;
   CSmackerPerson SP_Frau;
   CSmackerPerson SP_DroppingPaper;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPersonal)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPersonal();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPersonal)
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnPaint();
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
