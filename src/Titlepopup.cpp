//============================================================================================
//TitlePopup.Cpp : Titelbild mit primitiven Auswahlmöglichkeiten
//============================================================================================
#include "stdafx.h"
#include "TitlePopup.h"
#include "gltitel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char FileId[] = "Titl";

/////////////////////////////////////////////////////////////////////////////
// TitlePopup

TitlePopup::TitlePopup()
{
   Sim.Gamestate = UBYTE((Sim.Gamestate & (~GAMESTATE_WORKING)) | GAMESTATE_DONE);
}

TitlePopup::~TitlePopup()
{
}


BEGIN_MESSAGE_MAP(TitlePopup, CWnd)
	//{{AFX_MSG_MAP(TitlePopup)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// TitlePopup message handlers

void TitlePopup::OnPaint() 
{
}

void TitlePopup::OnLButtonDown(UINT, CPoint) 
{
}
