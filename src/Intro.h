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

    GfxLib      *pRoomLib;               //Library für den Raum

    SLONG        FrameNum;
    SLONG        FadeCount;
    DWORD        FrameNext;
    ULONG        Width;
    ULONG        Height;

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
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	SDL_AudioDeviceID audioDevice;
};

/////////////////////////////////////////////////////////////////////////////
