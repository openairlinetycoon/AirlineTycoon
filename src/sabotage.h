//============================================================================================
// Sabotage.h : Sabotageraum
//============================================================================================

class CSabotage : public CStdRaum
{
// Construction
public:
	CSabotage(BOOL bHandy, ULONG PlayerNum);

// Operations
public:
   CAnimation  DampfAnim;
   CAnimation  DynamitAnim;
   CAnimation  KamelAnim;
   CAnimation  LampeAnim;
   SBFX        ZischFx;
   SBFX        LunteFx;
   SBFX        BackFx;
   SLONG       CurrentTip;          //Dieser Tip wird gerade angezeigt

   SLONG          PlayEyeAnim;
   SBBM           AraberBm;
   SBBM           DartBm;
   SBBM           ZangeBm;

   CSmackerPerson SP_Araber;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSabotage)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSabotage();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSabotage)
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnPaint();
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
