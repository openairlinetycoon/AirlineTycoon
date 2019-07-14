// TitlePopup.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TitlePopup window

class TitlePopup : public CWnd
{
// Construction
public:
	TitlePopup();

// Attributes
private:
   GfxLib    *pRoomLib;             //Library für den Raum;
   SBBM TitleBitmap;
   SBBM HeadlineBm;
   BUFFER<SBBMS> LineBms;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TitlePopup)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~TitlePopup();

	// Generated message map functions
protected:
	//{{AFX_MSG(TitlePopup)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
