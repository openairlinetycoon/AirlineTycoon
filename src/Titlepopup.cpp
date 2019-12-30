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

TitlePopup::TitlePopup (BOOL bHandy, SLONG PlayerNum) : CStdRaum(bHandy, PlayerNum, "", NULL)
{
   Sim.Gamestate = UBYTE((Sim.Gamestate & (~GAMESTATE_WORKING)) | GAMESTATE_DONE);
}

TitlePopup::~TitlePopup()
{
}

/////////////////////////////////////////////////////////////////////////////
// TitlePopup message handlers

void TitlePopup::OnPaint() 
{
}

void TitlePopup::OnLButtonDown(UINT, CPoint) 
{
}
