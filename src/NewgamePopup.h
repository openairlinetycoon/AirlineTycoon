// NewGamePopup.h : header file
//
#include "SbLib.h"
#include "network.h"

/////////////////////////////////////////////////////////////////////////////
// NewGamePopup window

class NewGamePopup : public CStdRaum
{
// Construction
public:
	NewGamePopup(BOOL bHandy, SLONG PlayerNum);

// Attributes
private:
	enum PAGE_TYPE : UBYTE {
		MAIN_MENU = 0,
		MISSION_SELECT = 1,
		CAMPAIGN_SELECT = 150,
		ADDON_MISSION_SELECT = 12,
		FLIGHT_SECURITY_MISSION_SELECT = 122,
		HIGHSCORES = 7,

		SELECT_PLAYER_SINGLEPLAYER = 2,
		SELECT_PLAYER_MULTIPLAYER = 18,
		SELECT_PLAYER_CAMPAIGN = 14,

		MULTIPLAYER_SELECT_NETWORK = 13,
		MULTIPLAYER_SELECT_SESSION = 15,
		MULTIPLAYER_CREATE_SESSION = 17,

		SETTINGS_CHOOSE_AIRPORT = 5,

		OTHERS_LOADING = 5,
		MP_LOADING = 99,
	};

   BOOL            TimerFailure;
   PAGE_TYPE       PageNum;       //Seite 1 oder 2
   SLONG           PageSub;       //Sub-Id für die Seite
   SLONG           bad;
   KLACKER         KlackerTafel;  //Die Tafel auf der alles angezeigt wird
   ULONG           PlayerReadyAt;
   ULONG           UnselectedNetworkIDs[4];

   SDL_TimerID     TimerId;
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
   SBList<SBNetworkPlayer*> *pNetworkPlayers;

   int						SessionMissionID;

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
	virtual void OnPaint();
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	void CheckNetEvents();
	virtual void OnTimer(UINT nIDEvent);
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnSetCursor(void* pWnd, UINT nHitTest, UINT message);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
