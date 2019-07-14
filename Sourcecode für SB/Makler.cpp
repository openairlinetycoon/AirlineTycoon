//============================================================================================
// Makler.cpp : Der Raum des Flugzeugmaklers
//============================================================================================
#include "stdafx.h"
#include "glMakler.h"
#include "Makler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SB_CColorFX ColorFX;

//Zum debuggen:
static const char FileId[] = "Makl";

extern SLONG timeMaklClose;

//////////////////////////////////////////////////////////////////////////////////////////////
// Makler Konstruktion, Initialisation, Destruction
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//Konstruktor
//--------------------------------------------------------------------------------------------
CMakler::CMakler(BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "makler.gli", GFX_MAKLER)
{
   SetRoomVisited (PlayerNum, ROOM_MAKLER);

   Sim.FocusPerson=-1;

   AirportRoomPos   = Airport.GetRandomTypedRune (RUNE_2SHOP, ROOM_MAKLER);
   AirportRoomPos.y = AirportRoomPos.y-5000+93;

   KommVarLicht  = 0;
   KommVarWasser = 0;

   LastWaterTime = timeGetTime();

   WaterFrame    = 0;

   WaterBms.ReSize (pRoomLib, "SPRITZW0 SPRITZA0 SPRITZA1 SPRITZ00 SPRITZ01 SPRITZ02 SPRITZE0 SPRITZE1 SPRITZE2");

   DoorOpaqueBm.ReSize (pRoomLib, GFX_OPAQUE);
   DoorTransBms.ReSize (pRoomLib, "TRANS1", 4);

   SpringState=0;

   KlappeFx.ReInit ("Klappe.raw");
   SpringFx.ReInit ("Spring.raw");

   SP_Makler.ReSize (9);
   SP_Makler.Clips[0].ReSize (0, "M_Wait.smk", "", XY (470, 218), SPM_IDLE,      CRepeat(1,1), CPostWait(15,15), SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                              "A9A2A2E1", 0, 1, 2, 3);
   SP_Makler.Clips[1].ReSize (1, "M_Dreh.smk", "M_Dreh.raw", XY (470, 218), SPM_IDLE,      CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                              &KommVarLicht, SMACKER_CLIP_XOR|SMACKER_CLIP_FRAME+6*SMACKER_CLIP_MULT, 1, NULL,
                              "A9E1", 0, 3);
   SP_Makler.Clips[2].ReSize (2, "M_Druc.smk", "M_Druc.raw", XY (470, 218), SPM_IDLE,      CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                              &KommVarWasser, SMACKER_CLIP_XOR|SMACKER_CLIP_FRAME+4*SMACKER_CLIP_MULT, 1, NULL,
                              "A9E1", 0, 3);

   //Zum Spieler drehen und sprechen:
   SP_Makler.Clips[3].ReSize (3, "M_Turn.smk", "", XY (470, 218), SPM_IDLE,      CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,
                              "A1E1", 4, 5);
   SP_Makler.Clips[4].ReSize (4, "M_Rede.smk", "", XY (470, 218), SPM_TALKING,   CRepeat(2,2), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,
                              "A9A5E1E1", 4, 7, 5, 6);
   SP_Makler.Clips[7].ReSize (7, "M_RedeB.smk", "", XY (470, 218), SPM_TALKING,  CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,
                              "A9E1E1", 4, 5, 6);
   SP_Makler.Clips[5].ReSize (5, "M_List.smk", "", XY (470, 218), SPM_LISTENING, CRepeat(1,1), CPostWait(10,20), SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,
                              "A9A9E1E1", 5, 8, 4, 6);
   SP_Makler.Clips[8].ReSize (8, "M_ListK.smk", "", XY (470, 218), SPM_LISTENING, CRepeat(1,1), CPostWait(1,1),   SMACKER_CLIP_DONTCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL, 
                              "A9", 5);
   SP_Makler.Clips[6].ReSize (6, "M_back.smk", "", XY (470, 218), SPM_IDLE,      CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL, 
                              "A9", 0);

   //Raumanimationen
   {
      PLAYER &qPlayer = Sim.Players.Players[(SLONG)PlayerNum];

      NeonFx.ReInit ("neon.raw");

      BubbleAnim.ReSize  (pRoomLib, "BUL00",    7, NULL,    FALSE, ANIMATION_MODE_REPEAT, 0,  2);
      FishAnim.ReSize    (pRoomLib, "FISH00",  35, NULL,    FALSE, ANIMATION_MODE_REPEAT, 400, 2, 300, 1);
      Lights1Anim.ReSize (pRoomLib, "LICHTL00", 5, &NeonFx, FALSE, ANIMATION_MODE_REPEAT, 0,  5);
      Lights2Anim.ReSize (pRoomLib, "LICHTR00", 5, NULL,    FALSE, ANIMATION_MODE_REPEAT, 0,  5);

      if (rand()%2==0 || CheatAnimNow)
         if (!qPlayer.HasItem (ITEM_BH) && !qPlayer.HasItem (ITEM_HUFEISEN) && qPlayer.TrinkerTrust==FALSE && Sim.Difficulty!=DIFF_TUTORIAL)
            KlappenAnim.ReSize (pRoomLib, "KLAPPE00", 4, &KlappeFx, FALSE, ANIMATION_MODE_ONCE, 500,  5);
         else
            KlappenAnim.ReSize (pRoomLib, "KLAPOH00", 4, &KlappeFx, FALSE, ANIMATION_MODE_ONCE, 500,  5);
   }

   if (!bHandy) AmbientManager.SetGlobalVolume (40);

   Talkers.Talkers[TALKER_MAKLER].IncreaseReference ();
   DefaultDialogPartner=TALKER_MAKLER;

   #ifdef DEMO
      MenuStart (MENU_REQUEST, MENU_REQUEST_NO_MAKLER);
      MenuSetZoomStuff (XY(320,220), 0.17, FALSE);
   #endif

   ShowWindow(SW_SHOW);
   UpdateWindow();
}

//--------------------------------------------------------------------------------------------
//Destruktor
//--------------------------------------------------------------------------------------------
CMakler::~CMakler()
{
   Talkers.Talkers[TALKER_MAKLER].DecreaseReference ();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Makler message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//BEGIN_MESSAGE_MAP(CMakler, CStdRaum)
//--------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CMakler, CStdRaum)
	//{{AFX_MSG_MAP(CMakler)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//--------------------------------------------------------------------------------------------
//void CMakler::OnPaint()
//--------------------------------------------------------------------------------------------
void CMakler::OnPaint()
{
   SLONG   NewTip;
   PLAYER &qPlayer = Sim.Players.Players[(SLONG)PlayerNum];

   { CPaintDC dc(this); } // device context for painting

   if (Sim.Date>5) Sim.GiveHint (HINT_FLUGZEUGMAKLER);

   if (SLONG(Sim.Time)>=timeMaklClose) Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   //Draw Persons:
   RoomBm.pBitmap->SetClipRect(&CRect(0,149,343,149+86));
   for (SLONG d=0; d<SLONG(Sim.Persons.AnzEntries()); d++)
   {
      //Entscheidung! Person malen:
      if (Sim.Persons.IsInAlbum(d) && Clans.IsInAlbum (Sim.Persons[d].ClanId) && Sim.Persons[d].State!=Sim.localPlayer)
      {
         PERSON &qPerson=Sim.Persons[d];
         CLAN   &qClan=Clans[(SLONG)qPerson.ClanId];
         UBYTE   Dir   = qPerson.LookDir;
         UBYTE   Phase = qPerson.Phase;

         if (Dir==1 || Dir==3) Dir = 4-Dir;
         if (Dir==8 && Phase<4)  Phase = UBYTE((Phase+2)%4);
         if (Dir==8 && Phase>=4) Phase = UBYTE((Phase+2)%4+4);

         XY p=XY(126,331)-(qPerson.ScreenPos-AirportRoomPos);

         if (p.x>-50 && p.y<470 && abs(qPerson.ScreenPos.y-AirportRoomPos.y)<40)
            qClan.BlitLargeAt (RoomBm, Dir, Phase, p);
      }
      else break;
   }
   RoomBm.pBitmap->SetClipRect(&CRect(0,0,640,440));

   RoomBm.BlitFromT (DoorOpaqueBm, 0, 149);
   ColorFX.BlitTrans (DoorTransBms[0].pBitmap, RoomBm.pBitmap, XY(325,200), NULL, 2);
   ColorFX.BlitTrans (DoorTransBms[1].pBitmap, RoomBm.pBitmap, XY(225,184), NULL, 2);
   ColorFX.BlitTrans (DoorTransBms[2].pBitmap, RoomBm.pBitmap, XY(102,172), NULL, 2);
   ColorFX.BlitTrans (DoorTransBms[3].pBitmap, RoomBm.pBitmap, XY(0,157), NULL, 2);
   //ColorFX.BlitOutline (DoorTransBms[3].pBitmap, RoomBm.pBitmap, XY(0,157), 0xffffff);

   //Die Raum-Animationen:
   BubbleAnim.BlitAt  (RoomBm, 394, 400);
   FishAnim.BlitAt    (RoomBm, 394, 400);

   if (KommVarLicht)
   {
      Lights1Anim.BlitAt (RoomBm, 88,  400);
      Lights2Anim.BlitAt (RoomBm, 193, 375);
   }
   else NeonFx.Stop();

   KlappenAnim.BlitAt (RoomBm, 0, 0);

   SP_Makler.Pump ();
   SP_Makler.BlitAtT (RoomBm);

   SLONG  Frames=(timeGetTime()-LastWaterTime)/100;
   if (Frames>20) Frames=20;
   if (Frames<0)  Frames=0;

   if (SpringState==0 && KommVarWasser)
      SpringFx.Play (DSBPLAY_NOSTOP|DSBPLAY_LOOPING, Sim.Options.OptionEffekte*100/7);

   if (SpringState==1 && !KommVarWasser)
      SpringFx.Stop ();

   SpringState=KommVarWasser;

   while (Frames>0)
   {
      if (WaterFrame==0 && KommVarWasser) WaterFrame++;
      else if (KommVarWasser)
      {
         WaterFrame++;
         if (WaterFrame>=6) WaterFrame=3;
      }
      else if (WaterFrame>0)
      {
         WaterFrame++;
         if (WaterFrame>=9) WaterFrame=0;
      }

      LastWaterTime=timeGetTime();
      Frames--;
   }
   RoomBm.BlitFrom (WaterBms[WaterFrame], 226, 253);

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   if (!IsDialogOpen() && !MenuIsOpen())
   {
      if (gMousePosition.IfIsWithin (55,106,196,405)) SetMouseLook (CURSOR_EXIT, 0, ROOM_MAKLER, 999);
      else if (gMousePosition.IfIsWithin (335,199,640,440)) SetMouseLook (CURSOR_HOT, 0, ROOM_MAKLER, 10);
      else if (gMousePosition.IfIsWithin (0,15,38,79) && KlappenAnim.GetFrame()==3 &&
          !qPlayer.HasItem (ITEM_BH) && !qPlayer.HasItem (ITEM_HUFEISEN) &&
          qPlayer.TrinkerTrust==FALSE && Sim.Difficulty!=DIFF_TUTORIAL) SetMouseLook (CURSOR_HOT, 0, ROOM_MAKLER, 20);
   }

   CStdRaum::PostPaint ();

   if (CurrentMenu==MENU_BUYPLANE && ((gMousePosition-MenuPos).IfIsWithin (216,6, 387,212)))
   {
      NewTip = (gMousePosition.y-(MenuPos.y+22))/13 + MenuPage;

      if (NewTip>=0 && NewTip-MenuPage<13 && NewTip<MenuDataTable.LineIndex.AnzEntries() && 
          PlaneTypes.IsInAlbum (MenuDataTable.LineIndex[NewTip]))
      {
         if (NewTip != CurrentTip)
         {
            MenuRepaint ();
            DrawPlaneTipContents (OnscreenBitmap, &PlaneTypes[MenuDataTable.LineIndex[NewTip]], NULL,
                                  XY(6,6), XY(6,28), &FontSmallBlack, &FontSmallBlack);
            CurrentTip = NewTip;
         }

         CheckCursorHighlight (ReferenceCursorPos, CRect (MenuPos.x+216, MenuPos.y+(NewTip-MenuPage)*13+25-3, MenuPos.x+387, MenuPos.y+(NewTip-MenuPage)*13+25+12), ColorOfFontBlack, CURSOR_HOT);
      }
      else NewTip = -1;
   }
   else NewTip = -1;

   #ifdef DEMO
      if (!IsDialogOpen() && !MenuIsOpen()) SetMouseLook (CURSOR_EXIT, 0, ROOM_MAKLER, 999);
   #endif

   CStdRaum::PumpToolTips ();

   if (MenuIsOpen() && NewTip != CurrentTip)
   {
      MenuRepaint ();
      CurrentTip = NewTip;
   }
}

//--------------------------------------------------------------------------------------------
//void CMakler::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CMakler::OnLButtonDown(UINT nFlags, CPoint point)
{
   XY      RoomPos;
   PLAYER &qPlayer = Sim.Players.Players[(SLONG)PlayerNum];

   DefaultOnLButtonDown ();

   if (!ConvertMousePosition (point, &RoomPos))
   {
	   CStdRaum::OnLButtonDown(nFlags, point);
      return;
   }

   if (!PreLButtonDown (point))
   {
      //Raum verlassen:
      if (MouseClickArea==ROOM_MAKLER && MouseClickId==999) Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
      else if (MouseClickArea==ROOM_MAKLER && MouseClickId==10)
      {
         StartDialog (TALKER_MAKLER, MEDIUM_AIR);
      }
      else if (MouseClickArea==ROOM_MAKLER && MouseClickId==20)
      {
         if (qPlayer.HasSpaceForItem() && !Sim.Players.Players[(SLONG)PlayerNum].HasItem (ITEM_BH))
         {
            Sim.Players.Players[(SLONG)PlayerNum].BuyItem (ITEM_BH);

            SLONG cs = KlappenAnim.CounterStart;
            KlappenAnim.ReSize (pRoomLib, "KLAPOH00", 4, &KlappeFx, FALSE, ANIMATION_MODE_ONCE, 500,  5);
            KlappenAnim.CounterStart = cs;
         }
      }
      else CStdRaum::OnLButtonDown(nFlags, point);
   }
}

//--------------------------------------------------------------------------------------------
//void CMakler::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CMakler::OnRButtonDown(UINT nFlags, CPoint point)
{
   DefaultOnRButtonDown ();

   //Außerhalb geklickt? Dann Default-Handler!
   if (point.x<WinP1.x || point.y<WinP1.y || point.x>WinP2.x || point.y>WinP2.y)
   {
      CWnd::OnRButtonDown(nFlags, point);
      return;
   }
   else
   {
      if (MenuIsOpen())
      {
         MenuRightClick (point);
      }
      else
      {
         if (!IsDialogOpen() && point.y<440)
            Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();

         CStdRaum::OnRButtonDown(nFlags, point);
      }
   }
}
