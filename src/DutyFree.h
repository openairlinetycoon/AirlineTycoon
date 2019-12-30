// DutyFree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDutyFree window

class CDutyFree : public CStdRaum
{
// Construction
public:
	CDutyFree(BOOL bHandy, ULONG PlayerNum);

// Attributes
public:
   SLONG       Topic;               //Darüber reden wir gerade (zum Verkauf)
   SBBM        TipBm;               //Bitmap der Tips
   SBBM        HufeisenBm;          //Das Hufeisen über der Tür

   SLONG       CurrTip;             //zeigt, welcher Tip gerade angezeigt wird

   SBBM        ZettelBm;
   GfxLib     *pMenuLib;

   SBBM        OpaqueBm;
   SBBM        TransBm;     
   SBBM        GeigenkastenBm;

   SBBMS       SchilderBms;         //Ausverkauft, in D, E, oder F

   SBFX        PayFX;
   SBFX        StartupFX;

   CSmackerPerson SP_Frau;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDutyFree)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDutyFree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDutyFree)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
