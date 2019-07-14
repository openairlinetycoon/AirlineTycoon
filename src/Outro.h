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
    UWORD        PaletteMapper [257];    //Tabelle zum Mappen von 8 auf 16 Bit
    BUFFER<BYTE> SmackPic;               //Das Bild, wie der Smacker es sieht
    Smack       *pSmack;
    SBBM         Bitmap;

    SLONG        FrameNum;

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
