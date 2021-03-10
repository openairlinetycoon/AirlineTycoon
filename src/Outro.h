// Outro.cpp : Der Render-Outro

/////////////////////////////////////////////////////////////////////////////
// COutro window

class COutro : public CStdRaum
{
// Construction
public:
	COutro(BOOL bHandy, SLONG PlayerNum, CString SmackName);

// Attributes
public:
    smk          pSmack;
    char         State;
    SBBM         Bitmap;

    SLONG        FrameNum;
    DWORD        FrameNext;
    unsigned long Width;
    unsigned long Height;
	unsigned char Scale;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutro)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COutro();

	// Generated message map functions
public:
	//{{AFX_MSG(COutro)
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnPaint();
	virtual void OnRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnSetCursor(void* pWnd, UINT nHitTest, UINT message);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()

private:
	SDL_AudioDeviceID audioDevice;
};

/////////////////////////////////////////////////////////////////////////////
