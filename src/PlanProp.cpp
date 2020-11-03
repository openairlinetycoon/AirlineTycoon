//============================================================================================
// PlaneProps.cpp : Der Schalter und das Hinterzimmer von PlaneProps!
//============================================================================================
#include "stdafx.h"
#include "glplanpr.h"
#include "PlanProp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SB_CColorFX ColorFX;

//Zum debuggen:
static const char FileId[] = "Prop";

//Top-Left
static XY SeatOffsets[] = { XY(95,130), XY(94,124), XY(75,104) };

//Centered-Hotspot, AB, AB, AB relativ zum SeatOffset (Spalten=Tabletts, Zeilen=Sitze)
static XY FoodOffsets[] = {  XY(116,98),XY(104,999), XY(116,90),XY(116,286),  XY(116,102),XY(116,330),
                            XY(116,108),XY(116,292), XY(116,108),XY(116,303), XY(116,109),XY(116,330),
                            XY(135,110),XY(135,330), XY(135,112),XY(135,325), XY(135,118),XY(135,999) };

static XY StarOffsets[] = { XY(498,8), XY(498,87), XY(498,167), XY(498,284),
                            XY( 66,8), XY( 66,87), XY( 66,167), XY( 66,246) };

//Preise verstehen sich pro Sitzplatz:
SLONG SeatCosts[] = {  300, 1200, 3000 };
SLONG FoodCosts[] = {   10,   20,   50 };
SLONG TrayCosts[] = {  100,  800, 3400 };
SLONG DecoCosts[] = {  500, 1500, 6000 };

//Preise pro Flugzeuge:
SLONG TriebwerkCosts[]  = {  100000,  750000, 3500000 };
SLONG ReifenCosts[]     = {   50000,  300000,  900000 };
SLONG ElektronikCosts[] = {  600000, 1500000, 8000000 };
SLONG SicherheitCosts[] = {       0, 2000000, 6000000 };

//--------------------------------------------------------------------------------------------
//Die Schalter wird eröffnet:
//--------------------------------------------------------------------------------------------
CPlaneProps::CPlaneProps(BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "planprop.gli", NULL)
{
   SetRoomVisited (PlayerNum, ROOM_PLANEPROPS);
   HandyOffset = 320;

   BlinkArrowsTimer=0;

   RoomBm.ReSize (640, 440);

   Back1.ReSize (pRoomLib, GFX_BACK);
   Back2.ReSize (pRoomLib, GFX_BACK2);
   MenuMaskBm.ReSize (pRoomLib, GFX_MENUMASK);
   BadgeBm.ReSize (pRoomLib, "BADGE");

   Floors.ReSize (pRoomLib, "DECO1", 2);
   Seats.ReSize (pRoomLib, "SITZ1_1 SITZ1_2 SITZ1_3 SITZ2_1 SITZ2_2 SITZ2_3 SITZ3_1 SITZ3_2 SITZ3_3");
   Food.ReSize (pRoomLib, "ESSEN1A ESSEN1B ESSEN2A ESSEN2B ESSEN3A ESSEN3B");
   Prozente.ReSize (pRoomLib, "0_PRZ 10_PRZ 20_PRZ 30_PRZ 40_PRZ 50_PRZ 60_PRZ 70_PRZ 80_PRZ 90_PRZ 100_PRZ");

   SosBms.ReSize (pRoomLib, "SOS2", 2);
   ReifenBms.ReSize (pRoomLib, "REIFEN2", 2);
   CockpitBms.ReSize (pRoomLib, "COCKPIT2", 2);
   TurbinenBms.ReSize (pRoomLib, "TURBINE2", 2);

   CursorBms.ReSize (pRoomLib, "HIGH01", 8, CREATE_VIDMEM);
   StarBms.ReSize (pRoomLib, "STERN1", 3);
   MenuBms.ReSize (pRoomLib, "LEFT COPY OK PASTE RIGHT MINUS PLUS USEL USER USELOFF USEROFF");
   DigitBms.ReSize (pRoomLib, "KL_0", 10);
   StewardessBms.ReSize (pRoomLib, "S_A_1 S_A_2 S_A_3 S_B_1 S_B_2 S_B_3 S_BC_2 S_C_1 S_C_2 S_C_3");

   ActivePage = Sim.RFActivePage;
   ActiveDir  = 0;

   PlaneIndex=-1;
   PlaneDataTable.FillWithPlanes (&Sim.Players.Players[(SLONG)PlayerNum].Planes, FALSE);

   if (!Sim.Players.Players[(SLONG)PlayerNum].Planes.IsInAlbum(Sim.PlanePropId) && PlaneDataTable.AnzRows>0)
   {
      PlaneIndex=0;
   }
   else if (Sim.Players.Players[(SLONG)PlayerNum].Planes.IsInAlbum(Sim.PlanePropId))
   {
      for (SLONG c=0; c<PlaneDataTable.AnzRows; c++)
         if (PlaneDataTable.LineIndex[c]==Sim.PlanePropId)
            PlaneIndex=c;
   }

   if (!bHandy) AmbientManager.SetGlobalVolume (40);

   SDL_ShowWindow(FrameWnd->m_hWnd);
   SDL_UpdateWindowSurface(FrameWnd->m_hWnd);
}

//--------------------------------------------------------------------------------------------
//Sehr destruktiv!
//--------------------------------------------------------------------------------------------
CPlaneProps::~CPlaneProps()
{
   if (PlaneIndex!=-1)
      Sim.PlanePropId=PlaneDataTable.LineIndex[PlaneIndex];

   if (ActivePage>50) Sim.RFActivePage=100;
                 else Sim.RFActivePage=0;

   Back1.Destroy ();
   Back2.Destroy ();
   MenuMaskBm.Destroy ();
   BadgeBm.Destroy ();

   Floors.Destroy ();
   Seats.Destroy ();
   Food.Destroy ();
   Prozente.Destroy ();

   SosBms.Destroy ();
   ReifenBms.Destroy ();
   CockpitBms.Destroy ();
   TurbinenBms.Destroy ();

   CursorBms.Destroy ();
   StarBms.Destroy ();
   MenuBms.Destroy ();
   StewardessBms.Destroy ();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CPlaneProps message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//void CPlaneProps::OnPaint()
//--------------------------------------------------------------------------------------------
void CPlaneProps::OnPaint()
{
   static SLONG LastTimer;
   SLONG Timer=timeGetTime();

   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

   //Scrolling:
   ActivePage+=((Timer-LastTimer)*ActiveDir)/10;
   if (ActivePage<0) { ActivePage=0; ActiveDir=0; }
   if (ActivePage>100) { ActivePage=100; ActiveDir=0; }
   LastTimer=Timer;

   if (!Sim.UsedPlaneProp2)
   {
      BlinkArrowsTimer=timeGetTime();
      Sim.UsedPlaneProp2=TRUE;
   }

   if (!bHandy) SetMouseLook (CURSOR_NORMAL, 0, ROOM_ARAB_AIR, 0);

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   //Rechte Originalseite blitten:
   if (ActivePage>=50)
   {
      SLONG ScrollOffsetX=(100-ActivePage)*229/100;
      XY    ScrollOffset=XY(ScrollOffsetX, 0);

      RoomBm.BlitFrom (Back1, ScrollOffset);

      if (PlaneIndex!=-1)
      {
         CPlane &qPlane  = Sim.Players.Players[PlayerNum].Planes[PlaneDataTable.LineIndex[PlaneIndex]];
         SBBM   &qCursor = CursorBms[(SLONG)((timeGetTime()/150)%8)];

         //Großes Fenster (links):
         if (qPlane.DecoTarget<2) RoomBm.BlitFrom (Floors[qPlane.DecoTarget], 0+ScrollOffsetX, 27);
         RoomBm.BlitFromT (Seats[qPlane.SitzeTarget*3+qPlane.TablettsTarget], SeatOffsets[qPlane.SitzeTarget]+ScrollOffset);
         RoomBm.BlitFromT (Food[qPlane.EssenTarget*2], SeatOffsets[qPlane.SitzeTarget]+FoodOffsets[(qPlane.SitzeTarget*3+qPlane.TablettsTarget)*2]-Food[qPlane.EssenTarget*2].Size/SLONG(2)+ScrollOffset);
         RoomBm.BlitFromT (Food[qPlane.EssenTarget*2+1], SeatOffsets[qPlane.SitzeTarget]+FoodOffsets[(qPlane.SitzeTarget*3+qPlane.TablettsTarget)*2+1]-Food[qPlane.EssenTarget*2+1].Size/SLONG(2)+ScrollOffset);

         //Sterne:
         RoomBm.BlitFrom (StarBms[qPlane.Sitze],    StarOffsets[0] + XY(qPlane.Sitze*47+ScrollOffsetX, 0));
         RoomBm.BlitFrom (StarBms[qPlane.Tabletts], StarOffsets[1] + XY(qPlane.Tabletts*47+ScrollOffsetX, 0));
         RoomBm.BlitFrom (StarBms[qPlane.Deco],     StarOffsets[2] + XY(qPlane.Deco*47+ScrollOffsetX, 0));
         RoomBm.BlitFrom (StarBms[qPlane.Essen],    StarOffsets[3] + XY(qPlane.Essen*47+ScrollOffsetX, 0));

         //Highlights um die Sterne
         if (qPlane.Sitze!=qPlane.SitzeTarget)       RoomBm.BlitFromT (qCursor, StarOffsets[0] + XY(qPlane.SitzeTarget*47-1, -1)+ScrollOffset);
         if (qPlane.Tabletts!=qPlane.TablettsTarget) RoomBm.BlitFromT (qCursor, StarOffsets[1] + XY(qPlane.TablettsTarget*47-1, -1)+ScrollOffset);
         if (qPlane.Deco!=qPlane.DecoTarget)         RoomBm.BlitFromT (qCursor, StarOffsets[2] + XY(qPlane.DecoTarget*47-1, -1)+ScrollOffset);
         if (qPlane.Essen!=qPlane.EssenTarget)       RoomBm.BlitFromT (qCursor, StarOffsets[3] + XY(qPlane.EssenTarget*47-1, -1)+ScrollOffset);

         //Kosten für aktuelle Auswahl
         UBYTE Backup=0;
         for (SLONG c=0; c<3; c++)
            for (SLONG d=0; d<4; d++)
               if (gMousePosition.IfIsWithin (StarOffsets[d].x+c*47+ScrollOffsetX,StarOffsets[d].y,StarOffsets[d].x+43+c*47+ScrollOffsetX,StarOffsets[d].y+36))
                  switch (d)
                  {
                     case 0: Backup = qPlane.SitzeTarget;    qPlane.SitzeTarget    = (UBYTE)c; break;
                     case 1: Backup = qPlane.TablettsTarget; qPlane.TablettsTarget = (UBYTE)c; break;
                     case 2: Backup = qPlane.DecoTarget;     qPlane.DecoTarget     = (UBYTE)c; break;
                     case 3: Backup = qPlane.EssenTarget;    qPlane.EssenTarget    = (UBYTE)c; break;
                  }

         //Kosten rechts:
         //SLONG cost1 = PlaneTypes[qPlane.TypeId].Passagiere*(SeatCosts[qPlane.SitzeTarget]    - SeatCosts[qPlane.Sitze]/2);
         SLONG cost1 = qPlane.ptPassagiere*(SeatCosts[qPlane.SitzeTarget]    - SeatCosts[qPlane.Sitze]/2);
         //SLONG cost2 = PlaneTypes[qPlane.TypeId].Passagiere*(TrayCosts[qPlane.TablettsTarget] - TrayCosts[qPlane.Tabletts]/2);
         SLONG cost2 = qPlane.ptPassagiere*(TrayCosts[qPlane.TablettsTarget] - TrayCosts[qPlane.Tabletts]/2);
         //SLONG cost3 = PlaneTypes[qPlane.TypeId].Passagiere*(DecoCosts[qPlane.DecoTarget]     - DecoCosts[qPlane.Deco]/2);
         SLONG cost3 = qPlane.ptPassagiere*(DecoCosts[qPlane.DecoTarget]     - DecoCosts[qPlane.Deco]/2);

         if (qPlane.SitzeTarget!=qPlane.Sitze)       RoomBm.PrintAt (bprintf ("%s %s", (LPCTSTR)(CString)StandardTexte.GetS (TOKEN_SCHED, 1800), (LPCTSTR)(CString)Einheiten[EINH_DM].bString (cost1)), FontSmallBlack, TEC_FONT_RIGHT, XY(502, 58)+ScrollOffset, XY(631,72)+ScrollOffset);   else cost1=0;
         if (qPlane.TablettsTarget!=qPlane.Tabletts) RoomBm.PrintAt (bprintf ("%s %s", (LPCTSTR)(CString)StandardTexte.GetS (TOKEN_SCHED, 1800), (LPCTSTR)(CString)Einheiten[EINH_DM].bString (cost2)), FontSmallBlack, TEC_FONT_RIGHT, XY(502,138)+ScrollOffset, XY(631,155)+ScrollOffset);  else cost2=0;
         if (qPlane.DecoTarget!=qPlane.Deco)         RoomBm.PrintAt (bprintf ("%s %s", (LPCTSTR)(CString)StandardTexte.GetS (TOKEN_SCHED, 1800), (LPCTSTR)(CString)Einheiten[EINH_DM].bString (cost3)), FontSmallBlack, TEC_FONT_RIGHT, XY(502,218)+ScrollOffset, XY(631,305)+ScrollOffset);  else cost3=0;
         if (cost1+cost2+cost3>0)                    RoomBm.PrintAt (bprintf ("%s %s", (LPCTSTR)(CString)StandardTexte.GetS (TOKEN_SCHED, 1800), (LPCTSTR)(CString)Einheiten[EINH_DM].bString (cost1+cost2+cost3)), FontSmallBlack, TEC_FONT_RIGHT, XY(502,252)+ScrollOffset, XY(631,271)+ScrollOffset);
         RoomBm.PrintAt (bprintf ("%s %s", (LPCTSTR)(CString)Einheiten[EINH_DM].bString (qPlane.ptPassagiere*FoodCosts[qPlane.EssenTarget]), (LPCTSTR)StandardTexte.GetS (TOKEN_SCHED, 1801)),    FontSmallBlack, TEC_FONT_RIGHT, XY(502,334)+ScrollOffset, XY(631,353)+ScrollOffset);
         //RoomBm.PrintAt (bprintf ("%s %s", (LPCTSTR)(CString)Einheiten[EINH_DM].bString (PlaneTypes[qPlane.TypeId].Passagiere*FoodCosts[qPlane.EssenTarget]), (LPCTSTR)StandardTexte.GetS (TOKEN_SCHED, 1801)),    FontSmallBlack, TEC_FONT_RIGHT, XY(502,334)+ScrollOffset, XY(631,353)+ScrollOffset);

         //Kosten für aktuelle Auswahl
         for (SLONG c=0; c<3; c++)
            for (SLONG d=0; d<4; d++)
               if (gMousePosition.IfIsWithin (StarOffsets[d].x+c*47+ScrollOffsetX,StarOffsets[d].y,StarOffsets[d].x+43+c*47+ScrollOffsetX,StarOffsets[d].y+36))
                  switch (d)
                  {
                     case 0: qPlane.SitzeTarget    = Backup; break;
                     case 1: qPlane.TablettsTarget = Backup; break;
                     case 2: qPlane.DecoTarget     = Backup; break;
                     case 3: qPlane.EssenTarget    = Backup; break;
                  }

         if (!IsDialogOpen() && !MenuIsOpen() && ActivePage==100)
         {
            /*if (gMousePosition.IfIsWithin (134,26,164,44) && PlaneIndex>0)
            {
               SetMouseLook (CURSOR_HOT, 3100, ROOM_PLANEPROPS, 10);
               RoomBm.BlitFromT (MenuBms[0], 135, 27);

               //Wiederholfunktion für blättern links:
               if (gMouseLButton && timeGetTime()-gMouseLButtonDownTimer>800)
               {
                  PlaneIndex--;
                  gMouseLButtonDownTimer=timeGetTime()-400;
               }
            }
            else if (gMousePosition.IfIsWithin (164,26,190,50))
            {
               SetMouseLook (CURSOR_HOT, 3101, ROOM_PLANEPROPS, 11);
               RoomBm.BlitFromT (MenuBms[1], 164, 27);
            }
            else if (gMousePosition.IfIsWithin (172,50,192,55) || gMousePosition.IfIsWithin (178,56,195,69) || gMousePosition.IfIsWithin (182,60,207,67))
            {
               SetMouseLook (CURSOR_HOT, 3105, ROOM_PLANEPROPS, 90);
               RoomBm.BlitFromT (MenuBms[7], 164, 27);
            }
            else if (gMousePosition.IfIsWithin (190,26,226,62))
            {
               SetMouseLook (CURSOR_EXIT, 3102, ROOM_PLANEPROPS, 12);
               RoomBm.BlitFromT (MenuBms[2], 191, 27);

               SLONG Costs=Sim.Players.Players[PlayerNum].CalcPlanePropSum();

               if (Costs>0)
                  Sim.Players.Players[(SLONG)PlayerNum].Messages.AddMessage (BERATERTYP_GIRL, bprintf (StandardTexte.GetS (TOKEN_SCHED, 1820), Insert1000erDots(Costs)), MESSAGE_COMMENT);
               else if (Costs<0)
                  Sim.Players.Players[(SLONG)PlayerNum].Messages.AddMessage (BERATERTYP_GIRL, bprintf (StandardTexte.GetS (TOKEN_SCHED, 1821), Insert1000erDots(-Costs)), MESSAGE_COMMENT);
            }
            else if (gMousePosition.IfIsWithin (226,26,253,60))
            {
               SetMouseLook (CURSOR_HOT, 3103, ROOM_PLANEPROPS, 13);
               RoomBm.BlitFromT (MenuBms[3], 210, 27);
            }
            else if (gMousePosition.IfIsWithin (253,26,282,44) && PlaneIndex<PlaneDataTable.AnzRows-1)
            {
               SetMouseLook (CURSOR_HOT, 3104, ROOM_PLANEPROPS, 14);
               RoomBm.BlitFromT (MenuBms[4], 248, 27);

               //Wiederholfunktion für blättern rechts:
               if (gMouseLButton && timeGetTime()-gMouseLButtonDownTimer>800 && PlaneIndex<PlaneDataTable.AnzRows-1)
               {
                  PlaneIndex++;
                  gMouseLButtonDownTimer=timeGetTime()-400;
               }
            }*/

            if (gMousePosition.IfIsWithin (497,367,541,402))  //Minus
            {
               SetMouseLook (CURSOR_HOT, 3210, ROOM_PLANEPROPS, 15);
               RoomBm.BlitFromT (MenuBms[5], 497, 367);
            }
            else if (gMousePosition.IfIsWithin (592,367,636,402))  //Plus
            {
               SetMouseLook (CURSOR_HOT, 3211, ROOM_PLANEPROPS, 16);
               RoomBm.BlitFromT (MenuBms[6], 592,367);
            }

            for (SLONG c=0; c<3; c++)
               for (SLONG d=0; d<4; d++)
               {
                  if (gMousePosition.IfIsWithin (StarOffsets[d].x+c*47,StarOffsets[d].y,StarOffsets[d].x+43+c*47,StarOffsets[d].y+36))
                  {
                     SetMouseLook (CURSOR_HOT, 3200+d, ROOM_PLANEPROPS, 40+c+d*10);
                     RoomBm.BlitFromT (qCursor, StarOffsets[d]+XY(c*47-1, -1));
                  }
               }
         }

         //Badge für die Stewardess:
         if (qPlayer.SecurityFlags & (1<<6))
            RoomBm.BlitFrom (BadgeBm, 420+ScrollOffsetX, 352);

         //Die Stewardess Anzeige:
         {
            SLONG px=480-7+ScrollOffsetX; //y=408
            SLONG m,n;

            //n=min(qPlane.AnzBegleiter, PlaneTypes[qPlane.TypeId].AnzBegleiter);
            n=min(qPlane.AnzBegleiter, qPlane.ptAnzBegleiter);
            if (n)
            {
               RoomBm.BlitFromT (StewardessBms[0], px, 408); px+=StewardessBms[0].Size.x;
               for (; n>1; n--)
               {
                  RoomBm.BlitFromT (StewardessBms[1], px, 408); px+=StewardessBms[1].Size.x;
               }
               RoomBm.BlitFromT (StewardessBms[2], px, 408); px+=StewardessBms[2].Size.x+3;
            }

            //m=n=qPlane.AnzBegleiter-min(qPlane.AnzBegleiter, PlaneTypes[qPlane.TypeId].AnzBegleiter);
            m=n=qPlane.AnzBegleiter-min(qPlane.AnzBegleiter, qPlane.ptAnzBegleiter);
            if (n)
            {
               RoomBm.BlitFromT (StewardessBms[3], px, 408); px+=StewardessBms[3].Size.x;
               for (; n>1; n--)
               {
                  RoomBm.BlitFromT (StewardessBms[4], px, 408); px+=StewardessBms[4].Size.x;
               }
               RoomBm.BlitFromT (StewardessBms[5], px, 408); px+=StewardessBms[5].Size.x;
            }

            n=qPlane.MaxBegleiter-qPlane.AnzBegleiter;
            if (n)
            {
               if (m)
               {
                  px-=9;
                  RoomBm.BlitFromT (StewardessBms[6], px, 408); px+=StewardessBms[6].Size.x;
               }
               else
               {
                  RoomBm.BlitFromT (StewardessBms[7], px, 408); px+=StewardessBms[7].Size.x;
               }

               for (; n>1; n--)
               {
                  RoomBm.BlitFromT (StewardessBms[8], px, 408); px+=StewardessBms[8].Size.x;
               }
               RoomBm.BlitFromT (StewardessBms[9], px, 408); px+=StewardessBms[9].Size.x+3;
            }

            RoomBm.BlitFrom (DigitBms[qPlane.MaxBegleiter%10], 567+ScrollOffsetX, 374);

            if (qPlane.MaxBegleiter>9)
               RoomBm.BlitFrom (DigitBms[qPlane.MaxBegleiter/10], 551+ScrollOffsetX, 374);
         }
      }

      if (ActivePage!=100)
      {
         RoomBm.pBitmap->SetClipRect (&CRect(0,0,400+ScrollOffsetX,440));
         ColorFX.Apply ((50-(100-ActivePage))*8/50, RoomBm.pBitmap);

         for (SLONG c=0; c<8; c++)
         {
            RoomBm.pBitmap->SetClipRect (&CRect(min(640,400+c*25+ScrollOffsetX),0,min(640,425+c*25+ScrollOffsetX),440));

            if (min(640,400+c*25+ScrollOffsetX)<min(640,425+c*25+ScrollOffsetX))
            {
               SLONG n;

               n = 8-(8-(50-(100-ActivePage))*8/50)*((7-c))/7;

               if (ActivePage>75) n+=(ActivePage-75)/5;

               if (n>8) n=8;

               ColorFX.Apply (n, RoomBm.pBitmap);
            }
         }

         RoomBm.pBitmap->SetClipRect (&CRect(0,0,640,480));
      }
   }
   else
   {
      SLONG ScrollOffsetX=-(ActivePage)*229/100;
      XY    ScrollOffset=XY(ScrollOffsetX, 0);

      RoomBm.BlitFrom (Back2, ScrollOffsetX, 0);

      if (PlaneIndex!=-1)
      {
         CPlane &qPlane  = Sim.Players.Players[PlayerNum].Planes[PlaneDataTable.LineIndex[PlaneIndex]];
         SBBM   &qCursor = CursorBms[(SLONG)((timeGetTime()/150)%8)];

         //Großes Fenster (rechts):
         if (qPlane.ReifenTarget)     RoomBm.BlitFrom (ReifenBms[qPlane.ReifenTarget-1], 325+ScrollOffsetX, 74);
         if (qPlane.TriebwerkTarget)  RoomBm.BlitFrom (TurbinenBms[qPlane.TriebwerkTarget-1], 444+ScrollOffsetX, 42);
         if (qPlane.SicherheitTarget) RoomBm.BlitFrom (SosBms[qPlane.SicherheitTarget-1], 305+ScrollOffsetX, 247);
         if (qPlane.ElektronikTarget) RoomBm.BlitFrom (CockpitBms[qPlane.ElektronikTarget-1], 442+ScrollOffsetX, 237);

         //Sterne:
         RoomBm.BlitFrom (StarBms[qPlane.Reifen],     StarOffsets[4] + XY(qPlane.Reifen*47+ScrollOffsetX, 0));
         RoomBm.BlitFrom (StarBms[qPlane.Triebwerk],  StarOffsets[5] + XY(qPlane.Triebwerk*47+ScrollOffsetX, 0));
         RoomBm.BlitFrom (StarBms[qPlane.Sicherheit], StarOffsets[6] + XY(qPlane.Sicherheit*47+ScrollOffsetX, 0));
         RoomBm.BlitFrom (StarBms[qPlane.Elektronik], StarOffsets[7] + XY(qPlane.Elektronik*47+ScrollOffsetX, 0));

         //Highlights um die Sterne
         if (qPlane.Reifen!=qPlane.ReifenTarget)         RoomBm.BlitFromT (qCursor, StarOffsets[4] + XY(qPlane.ReifenTarget*47-1, -1)+ScrollOffset);
         if (qPlane.Triebwerk!=qPlane.TriebwerkTarget)   RoomBm.BlitFromT (qCursor, StarOffsets[5] + XY(qPlane.TriebwerkTarget*47-1, -1)+ScrollOffset);
         if (qPlane.Sicherheit!=qPlane.SicherheitTarget) RoomBm.BlitFromT (qCursor, StarOffsets[6] + XY(qPlane.SicherheitTarget*47-1, -1)+ScrollOffset);
         if (qPlane.Elektronik!=qPlane.ElektronikTarget) RoomBm.BlitFromT (qCursor, StarOffsets[7] + XY(qPlane.ElektronikTarget*47-1, -1)+ScrollOffset);

         //Kosten für aktuelle Auswahl
         UBYTE Backup=0;
         if (!IsDialogOpen() && !MenuIsOpen() && ActivePage==0)
            for (SLONG c=0; c<3; c++)
               for (SLONG d=0; d<4; d++)
                  if (gMousePosition.IfIsWithin (StarOffsets[d+4].x+c*47,StarOffsets[d+4].y,StarOffsets[d+4].x+43+c*47,StarOffsets[d+4].y+36))
                     switch (d)
                     {
                        case 0: Backup = qPlane.ReifenTarget;      qPlane.ReifenTarget     = (UBYTE)c; break;
                        case 1: Backup = qPlane.TriebwerkTarget;   qPlane.TriebwerkTarget  = (UBYTE)c; break;
                        case 2: Backup = qPlane.SicherheitTarget;  qPlane.SicherheitTarget = (UBYTE)c; break;
                        case 3: Backup = qPlane.ElektronikTarget;  qPlane.ElektronikTarget = (UBYTE)c; break;
                     }

         //Kosten rechts:
         SLONG cost1=0, cost2=0, cost3=0, cost4=0;

         if (qPlane.Reifen!=qPlane.ReifenTarget)
         {
            cost1=(ReifenCosts[qPlane.ReifenTarget]-ReifenCosts[qPlane.Reifen]/2);
            RoomBm.PrintAt (bprintf ("%s %s", (LPCTSTR)(CString)StandardTexte.GetS (TOKEN_SCHED, 1800), (LPCTSTR)(CString)Einheiten[EINH_DM].bString (cost1)), FontSmallBlack, TEC_FONT_LEFT, XY(8, 58)+ScrollOffset, XY(205,72)+ScrollOffset);
         }
         if (qPlane.Triebwerk!=qPlane.TriebwerkTarget)
         {
            cost2=(TriebwerkCosts[qPlane.TriebwerkTarget]-TriebwerkCosts[qPlane.Triebwerk]/2);
            RoomBm.PrintAt (bprintf ("%s %s", (LPCTSTR)(CString)StandardTexte.GetS (TOKEN_SCHED, 1800), (LPCTSTR)(CString)Einheiten[EINH_DM].bString (cost2)), FontSmallBlack, TEC_FONT_LEFT, XY(8,137)+ScrollOffset, XY(205,151)+ScrollOffset);
         }
         if (qPlane.Sicherheit!=qPlane.SicherheitTarget)
         {
            cost3=(SicherheitCosts[qPlane.SicherheitTarget]-SicherheitCosts[qPlane.Sicherheit]/2);
            RoomBm.PrintAt (bprintf ("%s %s", (LPCTSTR)(CString)StandardTexte.GetS (TOKEN_SCHED, 1800), (LPCTSTR)(CString)Einheiten[EINH_DM].bString (cost3)), FontSmallBlack, TEC_FONT_LEFT, XY(8,216)+ScrollOffset, XY(205,230)+ScrollOffset);
         }
         if (qPlane.Elektronik!=qPlane.ElektronikTarget)
         {
            cost4=(ElektronikCosts[qPlane.ElektronikTarget]-ElektronikCosts[qPlane.Elektronik]/2);
            RoomBm.PrintAt (bprintf ("%s %s", (LPCTSTR)(CString)StandardTexte.GetS (TOKEN_SCHED, 1800), (LPCTSTR)(CString)Einheiten[EINH_DM].bString (cost4)), FontSmallBlack, TEC_FONT_LEFT, XY(8,295)+ScrollOffset, XY(205,309)+ScrollOffset);
         }

         if (cost1+cost2+cost3+cost4>0) RoomBm.PrintAt (bprintf ("%s %s", (LPCTSTR)(CString)StandardTexte.GetS (TOKEN_SCHED, 1800), (LPCTSTR)(CString)Einheiten[EINH_DM].bString (cost1+cost2+cost3+cost4)), FontSmallBlack, TEC_FONT_LEFT, XY(8,329)+ScrollOffset, XY(205,345)+ScrollOffset);

         //Putzcrew-Anzahl blitten:
         /*RoomBm.BlitFrom (DigitBms[qPlane.AnzPutzcrew%10], 133+ScrollOffsetX, 374);
         if (qPlane.AnzPutzcrew>9)
            RoomBm.BlitFrom (DigitBms[qPlane.AnzPutzcrew/10], 117+ScrollOffsetX, 374);
         if (qPlane.AnzPutzcrew>0)
            RoomBm.PrintAt (bprintf ("%s %s", (LPCTSTR)(CString)StandardTexte.GetS (TOKEN_SCHED, 1802), (LPCTSTR)(CString)Einheiten[EINH_DM].bString (qPlane.AnzPutzcrew*250)), FontSmallBlack, TEC_FONT_LEFT, XY(58,329+87)+ScrollOffset, XY(205,345+87)+ScrollOffset);*/

         //Erster-Klasse Anzahl blitten:
         {
            long total   = qPlane.MaxPassagiere+qPlane.MaxPassagiereFC*2;
            long prozent = qPlane.MaxPassagiereFC*2*100/total;

            prozent=(prozent+5)/10*10/10; //Runden

            RoomBm.BlitFrom (DigitBms[prozent%10], 133+ScrollOffsetX, 374);
            if (prozent>9) RoomBm.BlitFrom (DigitBms[prozent/10], 117+ScrollOffsetX, 374);

            RoomBm.BlitFrom (Prozente[prozent], 62+ScrollOffsetX, 408);
         }

         //Kosten für aktuelle Auswahl
         SLONG c, d;
         if (!IsDialogOpen() && !MenuIsOpen() && ActivePage==0)
            for (c=0; c<3; c++)
               for (d=0; d<4; d++)
                  if (gMousePosition.IfIsWithin (StarOffsets[d+4].x+c*47,StarOffsets[d+4].y,StarOffsets[d+4].x+43+c*47,StarOffsets[d+4].y+36))
                     switch (d)
                     {
                        case 0: qPlane.ReifenTarget     = Backup; break;
                        case 1: qPlane.TriebwerkTarget  = Backup; break;
                        case 2: qPlane.SicherheitTarget = Backup; break;
                        case 3: qPlane.ElektronikTarget = Backup; break;
                     }

         if (!IsDialogOpen() && !MenuIsOpen() && ActivePage==0)
         {
            //Cursorüberwachung bei den Sternen
            for (c=0; c<3; c++)
               for (d=0; d<4; d++)
               {
                  if (gMousePosition.IfIsWithin (StarOffsets[d+4].x+c*47,StarOffsets[d+4].y,StarOffsets[d+4].x+43+c*47,StarOffsets[d+4].y+36))
                  {
                     SetMouseLook (CURSOR_HOT, 3204+d, ROOM_PLANEPROPS, 140+c+d*10);
                     RoomBm.BlitFromT (qCursor, StarOffsets[d+4]+XY(c*47-1, -1));
                  }
               }

            //Plusminus:
            if (gMousePosition.IfIsWithin (161,367,204,402))  //Plus
            {
               SetMouseLook (CURSOR_HOT, 3212, ROOM_PLANEPROPS, 116);
               RoomBm.BlitFromT (MenuBms[6], 160, 367);
            }
            else if (gMousePosition.IfIsWithin (65,367,106,402))  //Minus
            {
               SetMouseLook (CURSOR_HOT, 3213, ROOM_PLANEPROPS, 115);
               RoomBm.BlitFromT (MenuBms[5], 64,367);
            }
         }
      }

      if (ActivePage!=0)
      {
         RoomBm.pBitmap->SetClipRect (&CRect(240+ScrollOffsetX,0,640,440));
         ColorFX.Apply ((50-ActivePage)*8/50, RoomBm.pBitmap);

         for (SLONG c=0; c<8; c++)
         {
            RoomBm.pBitmap->SetClipRect (&CRect(max(0,215-c*25+ScrollOffsetX),0,max(0,240-c*25+ScrollOffsetX),440));

            if (max(0,215-c*25+ScrollOffsetX)<max(0,240-c*25+ScrollOffsetX))
            {
               SLONG n;

               n = 8-(8-(50-ActivePage)*8/50)*((7-c))/7;

               if (ActivePage<25) n+=(25-ActivePage)/5;

               if (n>8) n=8;

               ColorFX.Apply (n, RoomBm.pBitmap);
            }
         }

         RoomBm.pBitmap->SetClipRect (&CRect(0,0,640,480));
      }
   }

   //Überschrift:
   SLONG HeadlineOffset = 229*(100-ActivePage)/100;
   RoomBm.BlitFromT (MenuMaskBm, HeadlineOffset, 2);

   if (PlaneIndex!=-1)
   {
      CPlane &qPlane  = Sim.Players.Players[PlayerNum].Planes[PlaneDataTable.LineIndex[PlaneIndex]];

      RoomBm.PrintAt (qPlane.Name, FontBigGrey, TEC_FONT_CENTERED, XY(20+HeadlineOffset,3), XY(400+HeadlineOffset,20));
   }

   if (!IsDialogOpen() && !MenuIsOpen() && (ActivePage==0 || ActivePage==100))
   {
      gMousePosition.x-=HeadlineOffset;
      if (gMousePosition.IfIsWithin (134,26,164,44) && PlaneIndex>0)
      {
         SetMouseLook (CURSOR_HOT, 3100, ROOM_PLANEPROPS, 10);
         RoomBm.BlitFromT (MenuBms[0], 135+HeadlineOffset, 27);

         //Wiederholfunktion für blättern links:
         if (gMouseLButton && timeGetTime()-gMouseLButtonDownTimer>800)
         {
            PlaneIndex--;
            gMouseLButtonDownTimer=timeGetTime()-400;
         }
      }
      else if (gMousePosition.IfIsWithin (164,26,190,50))
      {
         SetMouseLook (CURSOR_HOT, 3101, ROOM_PLANEPROPS, 11);
         RoomBm.BlitFromT (MenuBms[1], 164+HeadlineOffset, 27);
      }
      else if ((gMousePosition.IfIsWithin (172,50,192,55) || gMousePosition.IfIsWithin (178,56,195,69) || gMousePosition.IfIsWithin (182,60,207,67)))
      {
         if (ActivePage==100)
         {
            SetMouseLook (CURSOR_HOT, 3105, ROOM_PLANEPROPS, 90);
            RoomBm.BlitFromT (MenuBms[7], 172+HeadlineOffset, 50);
         }
      }
      else if ((gMousePosition.IfIsWithin (224,48,243,52) || gMousePosition.IfIsWithin (218,53,239,56) || gMousePosition.IfIsWithin (210,57,234,64)))
      {
         if (ActivePage==0)
         {
            SetMouseLook (CURSOR_HOT, 3106, ROOM_PLANEPROPS, 91);
            RoomBm.BlitFromT (MenuBms[8], 210+HeadlineOffset, 50);
         }
      }
      else if (gMousePosition.IfIsWithin (190,26,226,62))
      {
         SetMouseLook (CURSOR_EXIT, 3102, ROOM_PLANEPROPS, 12);
         RoomBm.BlitFromT (MenuBms[2], 191+HeadlineOffset, 27);

         SLONG Costs=Sim.Players.Players[PlayerNum].CalcPlanePropSum();

         if (Costs>0)
            Sim.Players.Players[(SLONG)PlayerNum].Messages.AddMessage (BERATERTYP_GIRL, bprintf (StandardTexte.GetS (TOKEN_SCHED, 1820), Insert1000erDots(Costs)), MESSAGE_COMMENT);
         else if (Costs<0)
            Sim.Players.Players[(SLONG)PlayerNum].Messages.AddMessage (BERATERTYP_GIRL, bprintf (StandardTexte.GetS (TOKEN_SCHED, 1821), Insert1000erDots(-Costs)), MESSAGE_COMMENT);
      }
      else if (gMousePosition.IfIsWithin (226,26,253,50))
      {
         SetMouseLook (CURSOR_HOT, 3103, ROOM_PLANEPROPS, 13);
         RoomBm.BlitFromT (MenuBms[3], 210+HeadlineOffset, 27);
      }
      else if (gMousePosition.IfIsWithin (253,26,282,44) && PlaneIndex<PlaneDataTable.AnzRows-1)
      {
         SetMouseLook (CURSOR_HOT, 3104, ROOM_PLANEPROPS, 14);
         RoomBm.BlitFromT (MenuBms[4], 248+HeadlineOffset, 27);

         //Wiederholfunktion für blättern rechts:
         if (gMouseLButton && timeGetTime()-gMouseLButtonDownTimer>800 && PlaneIndex<PlaneDataTable.AnzRows-1)
         {
            PlaneIndex++;
            gMouseLButtonDownTimer=timeGetTime()-400;
         }
      }
      gMousePosition.x+=HeadlineOffset;
   }

   if (BlinkArrowsTimer && timeGetTime()-BlinkArrowsTimer<5000)
   {
      if ((timeGetTime()-BlinkArrowsTimer)%1000<500)
         RoomBm.BlitFromT (MenuBms[7], 172+HeadlineOffset, 50);
   }

   if (ActivePage!=100) RoomBm.BlitFromT (MenuBms[9],  172+HeadlineOffset, 50);
   if (ActivePage!=0)   RoomBm.BlitFromT (MenuBms[10], 210+HeadlineOffset, 50);


   CStdRaum::PostPaint ();
   CStdRaum::PumpToolTips ();
}

//--------------------------------------------------------------------------------------------
//void CPlaneProps::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CPlaneProps::OnLButtonDown(UINT nFlags, CPoint point)
{
   XY RoomPos;
   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

   DefaultOnLButtonDown ();

   if (!ConvertMousePosition (point, &RoomPos))
   {
	   CStdRaum::OnLButtonDown(nFlags, point);
      return;
   }

   if (!PreLButtonDown (point))
   {
      SLONG   PlaneId = PlaneDataTable.LineIndex[PlaneIndex];
      CPlane &qPlane  = qPlayer.Planes[PlaneId];

      if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==999) qPlayer.LeaveRoom();
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==10)
      {
         if (PlaneIndex>0) PlaneIndex--;
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==11)
      {
         Sim.RFSitzeRF  = qPlane.SitzeTarget;
         Sim.RFEssen    = qPlane.EssenTarget;
         Sim.RFTabletts = qPlane.TablettsTarget;
         Sim.RFDeco     = qPlane.DecoTarget;
         Sim.RFTriebwerk  = qPlane.TriebwerkTarget;
         Sim.RFReifen     = qPlane.ReifenTarget;
         Sim.RFElektronik = qPlane.ElektronikTarget;
         Sim.RFSicherheit = qPlane.SicherheitTarget;

         //Sim.RFPutzFaktor      = float(qPlane.AnzPutzcrew*1.0/qPlane.ptAnzBegleiter);
         //Sim.RFBegleiterFaktor = float(qPlane.MaxBegleiter*1.0/PlaneTypes[qPlane.TypeId].AnzBegleiter);
         Sim.RFBegleiterFaktor = float(qPlane.MaxBegleiter*1.0/qPlane.ptAnzBegleiter);
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==12) qPlayer.LeaveRoom();
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==13)
      {
         UBYTE back1 = qPlane.SitzeTarget;
         UBYTE back2 = qPlane.TablettsTarget;
         UBYTE back3 = qPlane.DecoTarget;

         UBYTE back4 = qPlane.TriebwerkTarget;
         UBYTE back5 = qPlane.ReifenTarget;
         UBYTE back6 = qPlane.ElektronikTarget;
         UBYTE back7 = qPlane.SicherheitTarget;

         qPlane.TriebwerkTarget  = Sim.RFTriebwerk;
         qPlane.ReifenTarget     = Sim.RFReifen;
         qPlane.ElektronikTarget = Sim.RFElektronik;
         qPlane.SicherheitTarget = Sim.RFSicherheit;
         //qPlane.AnzPutzcrew      = long(Sim.RFPutzFaktor*qPlane.ptAnzBegleiter);

         if (qPlayer.CalcPlanePropSum()>qPlayer.Money)
         {
            qPlane.SitzeTarget    = back1;
            qPlane.TablettsTarget = back2;
            qPlane.DecoTarget     = back3;

            qPlane.TriebwerkTarget  = back4;
            qPlane.ReifenTarget     = back5;
            qPlane.ElektronikTarget = back6;
            qPlane.SicherheitTarget = back7;

            qPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_SCHED, 1810), MESSAGE_COMMENT);
         }
         else qPlayer.NetUpdatePlaneProps (PlaneId);
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==14)
      {
         if (PlaneIndex<PlaneDataTable.AnzRows-1) PlaneIndex++;
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==15)
      {
         //if (qPlane.MaxBegleiter>PlaneTypes[qPlane.TypeId].AnzBegleiter) qPlane.MaxBegleiter--;
         if (qPlane.MaxBegleiter>qPlane.ptAnzBegleiter) qPlane.MaxBegleiter--;
         qPlayer.MapWorkers (FALSE);
         qPlayer.NetUpdatePlaneProps (PlaneId);
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==16)
      {
         //if (qPlane.MaxBegleiter<PlaneTypes[qPlane.TypeId].AnzBegleiter*2) qPlane.MaxBegleiter++;
         if (qPlane.MaxBegleiter<qPlane.ptAnzBegleiter*2) qPlane.MaxBegleiter++;
         qPlayer.MapWorkers (FALSE);
         qPlayer.NetUpdatePlaneProps (PlaneId);
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId>=40 && MouseClickId<=42)
      {
         UBYTE back = qPlane.SitzeTarget;

         qPlane.SitzeTarget    = UBYTE(MouseClickId-40);

         if (qPlayer.CalcPlanePropSum()>qPlayer.Money)
         {
            qPlane.SitzeTarget = back;
            qPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_SCHED, 1810), MESSAGE_COMMENT);
         }
         else qPlayer.NetUpdatePlaneProps (PlaneId);
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId>=50 && MouseClickId<=52)
      {
         UBYTE back = qPlane.TablettsTarget;

         qPlane.TablettsTarget = UBYTE(MouseClickId-50);

         if (qPlayer.CalcPlanePropSum()>qPlayer.Money)
         {
            qPlane.TablettsTarget = back;
            qPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_SCHED, 1810), MESSAGE_COMMENT);
         }
         else qPlayer.NetUpdatePlaneProps (PlaneId);
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId>=60 && MouseClickId<=62)
      {
         UBYTE back = qPlane.DecoTarget;

         qPlane.DecoTarget = UBYTE(MouseClickId-60);

         if (qPlayer.CalcPlanePropSum()>qPlayer.Money)
         {
            qPlane.DecoTarget = back;
            qPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_SCHED, 1810), MESSAGE_COMMENT);
         }
         else qPlayer.NetUpdatePlaneProps (PlaneId);
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId>=70 && MouseClickId<=72)
      {
         qPlane.EssenTarget = UBYTE(MouseClickId-70);
         qPlayer.NetUpdatePlaneProps (PlaneId);
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId>=140 && MouseClickId<=142)
      {
         UBYTE back = qPlane.ReifenTarget;

         qPlane.ReifenTarget    = UBYTE(MouseClickId-140);

         if (qPlayer.CalcPlanePropSum()>qPlayer.Money)
         {
            qPlane.ReifenTarget = back;
            qPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_SCHED, 1810), MESSAGE_COMMENT);
         }
         else qPlayer.NetUpdatePlaneProps (PlaneId);
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId>=150 && MouseClickId<=152)
      {
         UBYTE back = qPlane.TriebwerkTarget;

         qPlane.TriebwerkTarget = UBYTE(MouseClickId-150);

         if (qPlayer.CalcPlanePropSum()>qPlayer.Money)
         {
            qPlane.TriebwerkTarget = back;
            qPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_SCHED, 1810), MESSAGE_COMMENT);
         }
         else qPlayer.NetUpdatePlaneProps (PlaneId);
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId>=160 && MouseClickId<=162)
      {
         UBYTE back = qPlane.SicherheitTarget;

         qPlane.SicherheitTarget = UBYTE(MouseClickId-160);

         if (qPlayer.CalcPlanePropSum()>qPlayer.Money)
         {
            qPlane.SicherheitTarget = back;
            qPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_SCHED, 1810), MESSAGE_COMMENT);
         }
         else qPlayer.NetUpdatePlaneProps (PlaneId);
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId>=170 && MouseClickId<=172)
      {
         UBYTE back = qPlane.ElektronikTarget;

         qPlane.ElektronikTarget = UBYTE(MouseClickId-170);

         if (qPlayer.CalcPlanePropSum()>qPlayer.Money)
         {
            qPlane.ElektronikTarget = back;
            qPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_SCHED, 1810), MESSAGE_COMMENT);
         }
         else qPlayer.NetUpdatePlaneProps (PlaneId);
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==115)
      {
         long total   = qPlane.MaxPassagiere+qPlane.MaxPassagiereFC*2;
         long prozent = qPlane.MaxPassagiereFC*2*100/total;

         prozent=(prozent+5)/10*10; //Runden

         prozent-=10;
         long newMaxPassagiereFC = total*(prozent)/2/100;
         long newMaxPassagiere   = total-newMaxPassagiereFC*2;

         if (newMaxPassagiere==qPlane.MaxPassagiere)
         {
            newMaxPassagiereFC--;
            newMaxPassagiere+=2;
         }

         if (newMaxPassagiereFC>=0 && newMaxPassagiere>=0 && newMaxPassagiereFC+newMaxPassagiere>=qPlane.GetMaxPassengerOpenFlight(PlayerNum))
         {
            qPlane.MaxPassagiere   = newMaxPassagiere;
            qPlane.MaxPassagiereFC = newMaxPassagiereFC;
         }

         //if (qPlane.AnzPutzcrew>0) qPlane.AnzPutzcrew--;
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==116)
      {
         long total   = qPlane.MaxPassagiere+qPlane.MaxPassagiereFC*2;
         long prozent = qPlane.MaxPassagiereFC*2*100/total;

         prozent=(prozent+5)/10*10; //Runden

         prozent+=10;
         long newMaxPassagiereFC = total*(prozent)/2/100;
         long newMaxPassagiere   = total-newMaxPassagiereFC*2;

         if (newMaxPassagiere==qPlane.MaxPassagiere)
         {
            newMaxPassagiereFC++;
            newMaxPassagiere-=2;
         }

         if (newMaxPassagiereFC>=0 && newMaxPassagiere>=0 && newMaxPassagiereFC+newMaxPassagiere>=qPlane.GetMaxPassengerOpenFlight(PlayerNum))
         {
            qPlane.MaxPassagiere   = newMaxPassagiere;
            qPlane.MaxPassagiereFC = newMaxPassagiereFC;
         }

         //if (qPlane.AnzPutzcrew<qPlane.ptAnzBegleiter*2) qPlane.AnzPutzcrew++;
      }

      /*else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==115)
      {
         if (qPlane.AnzPutzcrew>0)
         {
            qPlane.AnzPutzcrew--;
            qPlayer.NetUpdatePlaneProps (PlaneId);
         }
      } */
      /*else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==116)
      {
         //if (qPlane.AnzPutzcrew<PlaneTypes[qPlane.TypeId].AnzBegleiter*2)
         if (qPlane.AnzPutzcrew<qPlane.ptAnzBegleiter*2)
         {
            qPlane.AnzPutzcrew++;
            qPlayer.NetUpdatePlaneProps (PlaneId);
         }
      }*/
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==90)
      {
         ActiveDir        = -1;
         BlinkArrowsTimer = 0;
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==91)
      {
         ActiveDir        = 1;
         BlinkArrowsTimer = 0;
      }
      else CStdRaum::OnLButtonDown(nFlags, point);
   }
}

//--------------------------------------------------------------------------------------------
//void CPlaneProps::OnLButtonDblClk(UINT, CPoint point)
//--------------------------------------------------------------------------------------------
void CPlaneProps::OnLButtonDblClk(UINT, CPoint point)
{
   //Ist das Fenster hier zuständig? Ist der Klick in diesem Fenster?
   if (point.x>=WinP1.x && point.x<=WinP2.x && point.y>=WinP1.y && point.y<=WinP2.y && !Editor)
   {
      CPlane &qPlane  = Sim.Players.Players[PlayerNum].Planes[PlaneDataTable.LineIndex[PlaneIndex]];

      if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==10)
      {
         if (PlaneIndex>0) PlaneIndex--;
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==14)
      {
         if (PlaneIndex<PlaneDataTable.AnzRows-1) PlaneIndex++;
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==15)
      {
         //if (qPlane.MaxBegleiter>PlaneTypes[qPlane.TypeId].AnzBegleiter) qPlane.MaxBegleiter--;
         if (qPlane.MaxBegleiter>qPlane.ptAnzBegleiter) qPlane.MaxBegleiter--;
         Sim.Players.Players[(SLONG)PlayerNum].MapWorkers (FALSE);
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==16)
      {
         //if (qPlane.MaxBegleiter<PlaneTypes[qPlane.TypeId].AnzBegleiter*2) qPlane.MaxBegleiter++;
         if (qPlane.MaxBegleiter<qPlane.ptAnzBegleiter*2) qPlane.MaxBegleiter++;
         Sim.Players.Players[(SLONG)PlayerNum].MapWorkers (FALSE);
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==115)
      {
         long total   = qPlane.MaxPassagiere+qPlane.MaxPassagiereFC*2;
         long prozent = qPlane.MaxPassagiereFC*2*100/total;

         prozent=(prozent+5)/10*10; //Runden

         prozent-=10;
         long newMaxPassagiereFC = total*(prozent)/2/100;
         long newMaxPassagiere   = total-newMaxPassagiereFC*2;

         if (newMaxPassagiere==qPlane.MaxPassagiere)
         {
            newMaxPassagiereFC--;
            newMaxPassagiere+=2;
         }

         if (newMaxPassagiereFC>=0 && newMaxPassagiere>=0 && newMaxPassagiereFC+newMaxPassagiere>=qPlane.GetMaxPassengerOpenFlight(PlayerNum))
         {
            qPlane.MaxPassagiere   = newMaxPassagiere;
            qPlane.MaxPassagiereFC = newMaxPassagiereFC;
         }

         //if (qPlane.AnzPutzcrew>0) qPlane.AnzPutzcrew--;
      }
      else if (MouseClickArea==ROOM_PLANEPROPS && MouseClickId==116)
      {
         long total   = qPlane.MaxPassagiere+qPlane.MaxPassagiereFC*2;
         long prozent = qPlane.MaxPassagiereFC*2*100/total;

         prozent=(prozent+5)/10*10; //Runden

         prozent+=10;
         long newMaxPassagiereFC = total*(prozent)/2/100;
         long newMaxPassagiere   = total-newMaxPassagiereFC*2;

         if (newMaxPassagiere==qPlane.MaxPassagiere)
         {
            newMaxPassagiereFC++;
            newMaxPassagiere-=2;
         }

         if (newMaxPassagiereFC>=0 && newMaxPassagiere>=0 && newMaxPassagiereFC+newMaxPassagiere>=qPlane.GetMaxPassengerOpenFlight(PlayerNum))
         {
            qPlane.MaxPassagiere   = newMaxPassagiere;
            qPlane.MaxPassagiereFC = newMaxPassagiereFC;
         }

         //if (qPlane.AnzPutzcrew<qPlane.ptAnzBegleiter*2) qPlane.AnzPutzcrew++;
      }
   }
}

//--------------------------------------------------------------------------------------------
//void CPlaneProps::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CPlaneProps::OnRButtonDown(UINT nFlags, CPoint point)
{
   DefaultOnRButtonDown ();

   //Außerhalb geklickt? Dann Default-Handler!
   if (point.x<WinP1.x || point.y<WinP1.y || point.x>WinP2.x || point.y>WinP2.y)
   {
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
