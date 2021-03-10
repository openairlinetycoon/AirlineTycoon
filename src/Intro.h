// Intro.cpp : Der Render-Intro

/////////////////////////////////////////////////////////////////////////////
// CIntro window

class CIntro : public CStdRaum
{
	// Construction
public:
	CIntro(BOOL bHandy, SLONG PlayerNum);

	// Attributes
public:
	smk          pSmack;
	char         State;
	SBBM         Bitmap;

	SBBM         FadeFrom;
	SBBM         FadeTo;
	bool         bWasIntroPlayed;

	GfxLib* pRoomLib;               //Library für den Raum

	SLONG        FrameNum;
	SLONG        FadeCount;
	DWORD        FrameNext;
	unsigned long Width;
	unsigned long Height;
	unsigned char Scale;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CIntro)
		//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CIntro();

	// Generated message map functions
public:
	//{{AFX_MSG(CIntro)
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