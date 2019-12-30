//============================================================================================
// Museum.h : Das Museum mit den alten Flugzeugen
//============================================================================================

/////////////////////////////////////////////////////////////////////////////
// Museum window

class CMuseum : public CStdRaum
{
// Construction
public:
	CMuseum(BOOL bHandy, ULONG PlayerNum);

// Attributes
public:
   SBBM        BlockBm;             //Fenster für Flugzeug-Details
   SLONG       CurrentTip;          //Dieser Tip wird gerade angezeigt
   GfxLib     *pMenuLib;
   SBBMS       MiniPlanes;

   SBFX        TriebwerkFx;

   CAnimation  RotorAnim;
               
   SLONG       TriggerSkelett;
   SLONG       TriggerMann;
   SLONG       TriggerEmergency;

   CSmackerPerson SP_Mann;
   CSmackerPerson SP_Skelett;
   CSmackerPerson SP_SwingPlane;

// Operations
public:
   void ReloadBitmaps(void);
   void BuyUsedPlane(void);
   void SellUsedPlane(void);

// Overrides
	// ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CMuseum)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMuseum();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMuseum)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
