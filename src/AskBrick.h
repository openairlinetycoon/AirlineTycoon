// AskBrick.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AskBrick window

class AskBrick : public CStdRaum
{
// Construction
public:
	AskBrick(BOOL bHandy, SLONG PlayerNum, ULONG Group, ULONG *rc1);

// Attributes
public:
   SBBM           AskBrickBm;
   GfxLib        *pMenuLib;

private:
   ULONG Group;
   ULONG *rc1;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AskBrick)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~AskBrick();

	// Generated message map functions
protected:
	//{{AFX_MSG(AskBrick)
	virtual void OnPaint();
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnClose();
	virtual BOOL OnSetCursor(void* pWnd, UINT nHitTest, UINT message);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
