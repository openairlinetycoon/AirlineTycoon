// Aufsicht.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAufsicht window

class CAufsicht : public CStdRaum
{
// Construction
public:
	CAufsicht(BOOL bHandy, ULONG PlayerNum);

   CAnimation     MonitorAnim;
   SBBM           PostcardBm;
   bool           bIsMorning;
   bool           bExitASAP;           //Exit, as soon as the other network players agree
   bool           bExited;

   CSmackerPerson SP_Boss;
   CSmackerPerson SP_Player[4];
   BOOL           IsOut[4];

   BOOL           bOkayToAct;          //Spieler darf morgends nur handeln, wenn alle Spieler hier sind oder waren

   CSmackerPerson SP_Halo[4];
   SLONG          HaloKomm[4];
   SLONG          PersonHaloKomm[4];

// Attributes
public:
   SLONG TimeClick;
   SBBM  OpaqueBm;
   SBBM  TransBm;
   SBBMS PlayerStuff;
   SBBM  FrauFuss;

   SBBMS LeereZettelBms;

   SLONG ExitFromMiddle;     //Komm-Var um aus der Mittelanimation auszubrechen
   SLONG ExitFromLeft;       //Komm-Var um aus der linken animation auszubrechen
   SLONG ExitFromRight;      //Komm-Var um aus der rechten Animation auszubrechen

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAufsicht)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAufsicht();
   void     TryLeaveAufsicht (void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CAufsicht)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
