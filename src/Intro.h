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
    UWORD        PaletteMapper [257];    //Tabelle zum Mappen von 8 auf 16 Bit
    //BUFFER<BYTE> SmackPic;               //Das Bild, wie der Smacker es sieht
    smk_t       *pSmack;
    SBBM         Bitmap;

    SBBM         FadeFrom;
    SBBM         FadeTo;
    bool         bWasIntroPlayed;

    GfxLib      *pRoomLib;               //Library für den Raum

    SLONG        FrameNum;
    SLONG        FadeCount;

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
};

/////////////////////////////////////////////////////////////////////////////
