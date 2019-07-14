//============================================================================================
// AskBrick.Cpp : Auswahlmenü für die Bricks des Editors
//============================================================================================
#include "Stdafx.h"
#include "AskBrick.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Anmerkung:
// Askbrick immer und nur dynamisch allokieren und nie de-allokieren. Das wird automatisch
// erledigt. Also immer nur:
// "new AskBrick (this, Groupm, &EditObject, &EditObjects[1]);"

//--------------------------------------------------------------------------------------------
// AskBrick
//--------------------------------------------------------------------------------------------
AskBrick::AskBrick(CWnd *ParentWnd, ULONG Group, ULONG *rc1)
{
   RECT rect;

   rect.top = 10;
   rect.left = 10;
   rect.right = 620;
   rect.bottom = 460;

   AskBrick::rc1 = rc1;
   AskBrick::Group = Group;

   if (!Create(NULL, "AskBrick", WS_VISIBLE|WS_CHILD, rect, ParentWnd, 42))
   {
      ::MessageBox (NULL, "Create failed", "ERROR", MB_OK );
      return;
   }
   if (bFullscreen) SetWindowPos (&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOREDRAW|SWP_NOSIZE);

   pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("Askbrick.gli", GliPath), &pMenuLib, L_LOCMEM);
   AskBrickBm.ReSize (pMenuLib, "ASKBRICK");

   ShowWindow(SW_SHOW);
   UpdateWindow();
   SetActiveWindow();

   ParentWnd->EnableWindow (0);
}

//--------------------------------------------------------------------------------------------
// ~AskBrick
//--------------------------------------------------------------------------------------------
AskBrick::~AskBrick()
{
   AskBrickBm.Destroy();
   if (pGfxMain && pMenuLib) pGfxMain->ReleaseLib (pMenuLib);

   TopWin = NULL;

   //Altes Fenster wieder aktiv schalten:
   GetParent()->EnableWindow (1);
}


BEGIN_MESSAGE_MAP(AskBrick, CWnd)
	//{{AFX_MSG_MAP(AskBrick)
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_CLOSE()
	ON_WM_KEYDOWN()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////////////////////
// AskBrick message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

void AskBrick::OnPaint() 
{
   SLONG g, x, y;

   CPaintDC dc(this); // device context for painting

   if (bActive)
   {
      PrimaryBm.BlitFrom (AskBrickBm, 10, 10);

      g = Group;

      for (y=0; y<4; y++)
         for (x=0; x<5; x++)
         {
            while (!Bricks.IsInAlbum (g) && g-Group<100) g++;

            if (Bricks.IsInAlbum (g))
            {
               /*if (Bricks[g].GetBitmapDimension().x<120 && Bricks[g].GetBitmapDimension().y<110)
               { */
                  Bricks[g].BlitAt (PrimaryBm,
                                    0,
                                    124*x+10+(124-Bricks[g].GetBitmapDimension().x)/2,
                                    115*y+10+(115-Bricks[g].GetBitmapDimension().y)/2, 0);
               /*}
               else
               {
                  double f=min(119.0/Bricks[g].GetBitmapDimension().x, 109.0/Bricks[g].GetBitmapDimension().y);

                  Bricks[g].BlitAt (PrimaryBm,
                                    0,
                                    long(124*x+10+(124-Bricks[g].GetBitmapDimension().x*f)/2),
                                    long(115*y+10+(115-Bricks[g].GetBitmapDimension().y*f)/2),
                                    long(124*x+10+(124-Bricks[g].GetBitmapDimension().x*f)/2+Bricks[g].GetBitmapDimension().x*f),
                                    long(115*y+10+(115-Bricks[g].GetBitmapDimension().y*f)/2+Bricks[g].GetBitmapDimension().y*f));
               } */
            }

            g++;
         }
   }
}

void AskBrick::OnLButtonDown(UINT nFlags, CPoint point) 
{
   SLONG g, x, y;

   g = Group;

   for (y=0; y<5; y++)
      for (x=0; x<5; x++)
      {
         while (!Bricks.IsInAlbum (g) && g-Group<100) g++;

         if (Bricks.IsInAlbum (g))
            if (point.x>=124*x+10 && point.x<=124*(x+1)+10 && point.y>=112*y+10 && point.y<=112*(y+1)+10)
            {
               *rc1 = g;
               PostMessage (WM_CLOSE);
            }

         g++;
      }

   ReferTo (nFlags);
   ReferTo (point);
}

void AskBrick::OnRButtonDown(UINT nFlags, CPoint point) 
{
   PostMessage (WM_CLOSE);
   ReferTo (nFlags);
   ReferTo (point);
}

void AskBrick::OnClose() 
{
   //Self-Destruct (zerstört Fenster und gibt anschließend Speicher frei):
   delete this;
}

void AskBrick::OnKeyDown(UINT nChar, UINT, UINT)
{
   switch (nChar)
   {
      case VK_UP:
         Group-=5;
         break;

      case VK_DOWN:
         Group+=5;
         break;

      case VK_LEFT:
         Group--;
         break;

      case VK_RIGHT:
         Group++;
         break;

      case VK_PRIOR:
         Group-=20;
         break;

      case VK_NEXT:
         Group+=20;
         break;
   }
}

//--------------------------------------------------------------------------------------------
//BOOL CStdRaum::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) : AG:
//--------------------------------------------------------------------------------------------
BOOL AskBrick::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return (FrameWnd->OnSetCursor(pWnd, nHitTest, message));
}

//--------------------------------------------------------------------------------------------
//void CStdRaum::OnMouseMove(UINT nFlags, CPoint point): AG:
//--------------------------------------------------------------------------------------------
void AskBrick::OnMouseMove(UINT nFlags, CPoint point) 
{
	FrameWnd->OnMouseMove(nFlags, point);
}