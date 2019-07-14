// Tafel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTafel window

class CTafel : public CStdRaum
{
// Construction
public:
	CTafel (BOOL bHandy, ULONG PlayerNum);

// Attributes
public:
   TEXT_STYLE  StyleTafel;
   SBBM        ZettelBms[21];
   SBBM        PostcardBm;
   SBBMS       LeereZettelBms;
   SLONG       LastTip;

   CSmackerPerson SP_Fliege;
   SLONG          KommVar;

   CTafelData  LastTafelData;

// Operations
public:
   void ReloadBitmaps (void);
   void RepaintZettel (SLONG n);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTafel)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTafel();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTafel)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
