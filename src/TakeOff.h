//============================================================================================
// TakeOff.h : main header file for the TAKEOFF application
//============================================================================================

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

/*#include "resource.h"		// main symbols
#include "mainfrm.h"
#include "global.h"
#include "proto.h"*/

/////////////////////////////////////////////////////////////////////////////
// CTakeOffApp:
// See TakeOff.cpp for the implementation of this class
//

class CTakeOffApp : public CWinApp
{
public:
	CTakeOffApp();
	~CTakeOffApp();

   void GameLoop(void*);
   void CheckSystem (void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTakeOffApp)
	public:
	virtual BOOL InitInstance();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTakeOffApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	HICON  m_hSmallIcon;
	HICON  m_hBigIcon;
};


/////////////////////////////////////////////////////////////////////////////
