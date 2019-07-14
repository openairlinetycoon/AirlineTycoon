//============================================================================================
// MainFrm.cpp : implementation of the CMainFrame class
//============================================================================================
#include "stdafx.h"
//#include "TakeOff.h"

/*
//#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIMER_ID        1
#define TIMER_RATE      500

LPDIRECTDRAW            lpDD;           // DirectDraw object
LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface
LPDIRECTDRAWSURFACE     lpDDSBack;      // DirectDraw back surface
BOOL                    bActive;        // is application active?

char szMsg[] = "Page Flipping Test: Press F12 to exit";
char szFrontMsg[] = "Front buffer (F12 to quit)";
char szBackMsg[] = "Back buffer (F12 to quit)";

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

//IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
    DDSURFACEDESC       ddsd;
    DDSCAPS             ddscaps;
    HRESULT             ddrval;
    HDC                 hdc;
    char                buf[256];
    RECT                rect;

    rect.top = 0;
    rect.left = 0;
    rect.right = GetSystemMetrics( SM_CXSCREEN );
    rect.bottom = GetSystemMetrics( SM_CYSCREEN );

    // CreateWindowEx
    if (!Create(NULL, "Takeoff", WS_POPUP|WS_VISIBLE, rect, this))
       ::MessageBox (NULL, "Create failed", "ERROR", MB_OK );

    //create the main DirectDraw object
    ddrval = DirectDrawCreate( NULL, &lpDD, NULL );
    if( ddrval == DD_OK )
    {
        // Get exclusive mode
        ddrval = lpDD->SetCooperativeLevel( GetSafeHwnd(), DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
        if(ddrval == DD_OK )
        {
            ddrval = lpDD->SetDisplayMode( 640, 480, 8 );
            if( ddrval == DD_OK )
            {
                // Create the primary surface with 1 back buffer
                ddsd.dwSize = sizeof( ddsd );
                ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
                ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE  |
                                      DDSCAPS_FLIP | 
                                      DDSCAPS_COMPLEX;
                ddsd.dwBackBufferCount = 1;
                ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );
                if( ddrval == DD_OK )
                {
                    // Get a pointer to the back buffer
                    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
                    ddrval = lpDDSPrimary->GetAttachedSurface(&ddscaps, 
                                                          &lpDDSBack);
                    if( ddrval == DD_OK )
                    {
                        // draw some text.
                        if (lpDDSPrimary->GetDC(&hdc) == DD_OK)
                        {
                            SetBkColor( hdc, RGB( 0, 0, 255 ) );
                            SetTextColor( hdc, RGB( 255, 255, 0 ) );
                            TextOut( hdc, 0, 0, szFrontMsg, lstrlen(szFrontMsg) );
                            lpDDSPrimary->ReleaseDC(hdc);
                        }

                        if (lpDDSBack->GetDC(&hdc) == DD_OK)
                        {
                            SetBkColor( hdc, RGB( 0, 0, 255 ) );
                            SetTextColor( hdc, RGB( 255, 255, 0 ) );
                            TextOut( hdc, 0, 0, szBackMsg, lstrlen(szBackMsg) );
                            lpDDSBack->ReleaseDC(hdc);
                        }

                        // Create a timer to flip the pages
                        if( ::SetTimer( GetSafeHwnd(), TIMER_ID, TIMER_RATE, NULL ) )
                        {
                             //return TRUE;
                             return;
                        }
                    }
                }
            }
        }
    }

    wsprintf(buf, "Direct Draw Init Failed (%08lx)\n", ddrval );
    ::MessageBox (NULL, buf, "ERROR", MB_OK );
    //finiObjects();
    //DestroyWindow( hwnd );
    //return FALSE;
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
*/