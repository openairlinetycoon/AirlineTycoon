//============================================================================================
// Makler.h : Der Raum des Flugzeugmaklers
//============================================================================================

/////////////////////////////////////////////////////////////////////////////
// Makler window

class CMakler : public CStdRaum
{
// Construction
public:
	CMakler(BOOL bHandy, ULONG PlayerNum);

// Attributes
public:
   SBBM        DoorOpaqueBm;
   SBBMS       DoorTransBms;
   CAnimation  BubbleAnim;
   CAnimation  FishAnim;
   CAnimation  Lights1Anim;
   CAnimation  Lights2Anim;
   CAnimation  KlappenAnim;

   SBBMS       WaterBms;
   SLONG       WaterFrame;
   SLONG       LastWaterTime;

   SLONG       KommVarLicht;
   SLONG       KommVarWasser;

   SBFX        KlappeFx;
   SBFX        SpringFx;
   SBFX        NeonFx;
   SLONG       SpringState;

   SBBM        TipBm;               //Fenster für Flugzeug-Tips
   SLONG       CurrentTip;          //Dieser Tip wird gerade angezeigt

   CSmackerPerson SP_Makler;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMakler)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMakler();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMakler)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
