// NewGamePopup.h : header file
//
#include "sblib\include\SbLib.h"
#include "sblib\include\network.h"

/////////////////////////////////////////////////////////////////////////////
// NewGamePopup window

class NewGamePopup : public CStdRaum
{
// Construction
public:
	NewGamePopup(BOOL bHandy, SLONG PlayerNum);

// Attributes
private:
   BOOL            TimerFailure;
   UBYTE           PageNum;       //Seite 1 oder 2
   SLONG           PageSub;       //Sub-Id für die Seite
   SLONG           bad;
   KLACKER         KlackerTafel;  //Die Tafel auf der alles angezeigt wird
   ULONG           PlayerReadyAt;
   ULONG           UnselectedNetworkIDs[4];

   SB_CFont        VersionFont;

   SLONG           CursorX, CursorY; //Der blinkende Eingabecursor; -1 = kein Cursor sichtbar
   UBYTE           BlinkState;
   SBBMS           PlayerBms;
   SBBM            HakenBm;
   SLONG           Selection;

   BOOL            NamesOK;       //Sind alle Namen eindeutig?

   SBFX            ClickFx;

   SBList<SBStr>           *pNetworkSessions;
   SBList<SBStr>           *pNetworkConnections;
   SBList<SBNetworkPlayer> *pNetworkPlayers;

   ULONG                    PlayerID;
   bool                     bThisIsSessionMaster;

   SBStr                    NetworkConnection;
   static SBStr             NetworkSession;

   long NetMediumCount;
   long NetMediumMapper[32];

// Operations
public:
   void Konstruktor (BOOL bHandy, SLONG PlayerNum);
   void RefreshKlackerField (void);
   void CheckNames (void);
   void PushNames (void);
   void PushName (SLONG n);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(NewGamePopup)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~NewGamePopup();

	// Generated message map functions
protected:
	//{{AFX_MSG(NewGamePopup)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
