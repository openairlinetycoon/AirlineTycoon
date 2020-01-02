// CCredits.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCredits window

class CCredits : public CStdRaum
{
// Construction
public:
	CCredits(BOOL bHandy, SLONG PlayerNum);

// Attributes
private:
   GfxLib         *pGLibCredits;
   SBBM            Background;
   SBBM            Left, Right;
   SBBM            TextLines[25];
   BOOL            TimerFailure;
   SLONG           ScrollPos;
   SLONG           LastTime;
   TEXT_STYLE      StyleCredits;

// Operations
public:
   void ReloadBitmap (void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCredits)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCredits();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCredits)
	virtual void OnPaint();
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
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
