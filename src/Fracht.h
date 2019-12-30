//============================================================================================
// Fracht.h : Der Fracht Schalter (neu im Add-On)
//============================================================================================

class CFrachtRaum : public CStdRaum
{
// Construction
public:
	CFrachtRaum(BOOL bHandy, ULONG PlayerNum);

// Data
public:
   CSmackerPerson SP_Fracht;
   CSmackerPerson SP_Stapler;

   SLONG       KommVar;         //Work-Around
   SLONG       DropItNow;       //Work-Around

   SBFX        KistenFx[2];
   SBFX        WarningFx;

   SBBM        NoGlueBm;
   SBBM        KranMaskeBm;
   SBBM        StempelBm;

   SLONG       LastTip;
   SBBM        ZettelBms[6];
   SBBM        TipBm;
   SBBMS       MapPlaneBms[5];
   SBBMS       SeileBms;
   SBBMS       KistenBms;
   GfxLib     *pMenuLib;
   GfxLib     *pZettelLib;

   bool        bCanPaint;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFracht)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFrachtRaum();

   void RepaintZettel (SLONG n);

	// Generated message map functions
protected:
	//{{AFX_MSG(CFracht)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
