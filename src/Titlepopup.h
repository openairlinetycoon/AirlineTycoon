// TitlePopup.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TitlePopup window

class TitlePopup : public CStdRaum
{
// Construction
public:
	TitlePopup(BOOL bHandy, SLONG PlayerNum);

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
	virtual void OnPaint();
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
