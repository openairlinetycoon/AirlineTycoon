//============================================================================================
// RouteBox.cpp : Die Routen Mieten und anschauen!
//============================================================================================
#include "stdafx.h"
#include "RouteBox.h"
#include "glrb.h"
#include "atnet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern ULONG AktienKursLineColor [4];

//Zum debuggen:
static const char FileId[] = "Rout";

static const XY ListOffset   = XY(461, 29);
static const XY TipOffset    = XY(462, 16);
static const XY MapOffset    = XY(198, 150);    //Für den Zoom der Karte den Faktor
static const SLONG MapScale = 1925;             //..und den Offset
static const SLONG ListSize = 22;

static const XY PlayerOffsets[] = { XY(241,267), XY(248,335), XY(304,258), XY(313,328) };

//--------------------------------------------------------------------------------------------
//Berechnet den Abstand von einem Punkt zu einer Linie:
//--------------------------------------------------------------------------------------------
SLONG CalcDistanceLine2Dot (XY LineP1, XY LineP2, XY p)
{
   float Abschnitt;

   Abschnitt = ((LineP1-p)*(LineP1-LineP2)) / (float)(LineP2-LineP1).abs() / (float)(LineP2-LineP1).abs();

   //if ((LineP1.x<LineP2.x && p.x<=LineP1.x) || (LineP1.x>LineP2.x && p.x>=LineP1.x) || (LineP1.y<LineP2.y && p.y<=LineP1.y) || (LineP1.y>LineP2.y && p.y>=LineP1.y))
   if (Abschnitt<0.0)
   {
      return (SLONG((p-LineP1).abs()));
   }
   else if (Abschnitt>1.0)
   //else if ((LineP2.x<LineP1.x && p.x<=LineP2.x) || (LineP2.x>LineP1.x && p.x>=LineP2.x) || (LineP2.y<LineP1.y && p.y<=LineP2.y) || (LineP2.y>LineP1.y && p.y>=LineP2.y))
   {
      return (SLONG((p-LineP2).abs()));
   }
   else //DebugBreak();
   {
      XY r (LineP1.y-LineP2.y, LineP2.x-LineP1.x);

      return (abs(SLONG((LineP1-p)*r/r.abs())));
   }

   return (0);
}

//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
CRouteBox::CRouteBox(BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "routebox.gli", GFX_ROUTEBOX)
{
   SetRoomVisited (PlayerNum, ROOM_ROUTEBOX);
   Sim.FocusPerson=-1;

   if (!bHandy) AmbientManager.SetGlobalVolume (60);

   Filter       = 0;
   RoutePage    = Sim.Players.Players[(SLONG)PlayerNum].RoutePage;
   RoutePageMax = (Routen.GetNumUsed()-1)/ListSize+1;

   DisplayPlayer   = 1<<PlayerNum;
   CurrentTipIndex = -1;

   PlayerBms.ReSize (pRoomLib, "P1_01 P1_02 P1_03 P2_01 P2_02 P2_03 P3_01 P3_02 P3_03 P4_01 P4_02 P4_03");
   EckenBms.ReSize (pRoomLib, "LINKS RECHTS");
   FilterBms.ReSize (pRoomLib, "FILTER1 FILTER2 FILTER3");
   HakenBm.ReSize (pRoomLib, "HAKEN");
   HereBm.ReSize (pRoomLib, "HERE");
   NoPaperBm.ReSize (pRoomLib, "NOPAPER");

   IsBuyable.ReSize (Routen.AnzEntries());
   IsBuyable.FillWith(0);

   CRentRouten &qRRouten = Sim.Players.Players[(SLONG)PlayerNum].RentRouten;

   for (SLONG d=Routen.AnzEntries()-1; d>=0; d--)
      if (Routen.IsInAlbum(d) && qRRouten.RentRouten[d].Rang==0)
         if (Routen[d].VonCity==(ULONG)Sim.HomeAirportId || Routen[d].NachCity==(ULONG)Sim.HomeAirportId)
            IsBuyable[d]=TRUE;
   for (SLONG c=Routen.AnzEntries()-1; c>=0; c--)
      if (Routen.IsInAlbum(c))
         if (qRRouten.RentRouten[c].RoutenAuslastung>=20)
         {
            for (SLONG d=Routen.AnzEntries()-1; d>=0; d--)
               if (Routen.IsInAlbum(d))
                  if (Routen[c].VonCity==Routen[d].VonCity || Routen[c].VonCity==Routen[d].NachCity || Routen[c].NachCity==Routen[d].VonCity || Routen[c].NachCity==Routen[d].NachCity)
                     IsBuyable[d]=TRUE;
         }
   for (SLONG d=Routen.AnzEntries()-1; d>=0; d--)
      if (Routen.IsInAlbum(d) && qRRouten.RentRouten[d].Rang==0 && qRRouten.RentRouten[d].TageMitGering<7)
         IsBuyable[d]=FALSE;

   UpdateDataTable();
   RepaintList ();
   RepaintMap ();

   SDL_ShowWindow(FrameWnd->m_hWnd);
   SDL_UpdateWindowSurface(FrameWnd->m_hWnd);
}

//--------------------------------------------------------------------------------------------
//Sehr destruktiv!
//--------------------------------------------------------------------------------------------
CRouteBox::~CRouteBox()
{
   Sim.Players.Players[(SLONG)PlayerNum].RoutePage = RoutePage;

   Sim.Persons.Persons[(SLONG)Sim.Persons.GetPlayerIndex(PlayerNum)].LookDir=2;
   Sim.Players.Players[(SLONG)PlayerNum].DirectToRoom=0;
   Sim.Persons.Persons[(SLONG)Sim.Persons.GetPlayerIndex(PlayerNum)].StatePar = 0;

   HereBm.Destroy();
   ListBm.Destroy();
   HakenBm.Destroy();
   PlayerBms.Destroy();
   EckenBms.Destroy();
   FilterBms.Destroy();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CRouteBox message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//void CRouteBox::OnPaint()
//--------------------------------------------------------------------------------------------
void CRouteBox::OnPaint()
{
   if (!bHandy) SetMouseLook (CURSOR_NORMAL, 0, ROOM_ROUTEBOX, 0);

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   RoomBm.BlitFrom (ListBm, ListOffset); //461, 14+13);
   if (CurrentTip!=-1 && Sim.ItemClips)
      RoomBm.BlitFrom (TipBm, 26, 275);

   RoomBm.BlitFrom (MapBm, 19, 28);
   RoomBm.BlitFrom (FilterBms[Filter], 453, 326);

   if (!Sim.ItemClips)
      RoomBm.BlitFromT (NoPaperBm, 0, 440-NoPaperBm.Size.y);

   if (RoutePage>0) RoomBm.BlitFrom (EckenBms[0], 451, 313);
   if (RoutePage<RoutePageMax-1) RoomBm.BlitFrom (EckenBms[1], 607, 313);

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   if (!IsDialogOpen() && !MenuIsOpen())
   {
      if (gMousePosition.IfIsWithin (444, 380, 640, 439)) SetMouseLook (CURSOR_EXIT, 0, ROOM_ROUTEBOX, 999);

      //Die Eselsohren:
      if (gMousePosition.IfIsWithin (449,314,469,332) && RoutePage>0) SetMouseLook (CURSOR_LEFT, 0, ROOM_ROUTEBOX, 10);
      else if (gMousePosition.IfIsWithin (607,314,628,332) && RoutePage<RoutePageMax-1) SetMouseLook (CURSOR_RIGHT, 0, ROOM_ROUTEBOX, 11);

      //Die Filter:
      if (gMousePosition.IfIsWithin (456,328,500,361) && Filter!=0) SetMouseLook (CURSOR_HOT, 4600, ROOM_ROUTEBOX, 30);
      else if (gMousePosition.IfIsWithin (501,330,541,363) && Filter!=1) SetMouseLook (CURSOR_HOT, 4601, ROOM_ROUTEBOX, 31);
      else if (gMousePosition.IfIsWithin (542,331,582,362) && Filter!=2) SetMouseLook (CURSOR_HOT, 4602, ROOM_ROUTEBOX, 32);

      //Die Büroklammern:
      if (Sim.ItemClips && !Sim.Players.Players[PlayerNum].HasItem (ITEM_PAPERCLIP))
      if (gMousePosition.IfIsWithin (0,245,36,287) || gMousePosition.IfIsWithin (186,276,214,322) || gMousePosition.IfIsWithin (225,269,245,314) || gMousePosition.IfIsWithin (347,280,383,320)) SetMouseLook (CURSOR_HOT, 0, ROOM_ROUTEBOX, 200);

      if (gMousePosition.IfIsWithin (461,14+15,461+171,14+15+ListSize*13-1))
      {
         SLONG i=(gMousePosition.y-14-15)/13;
         UWORD HighlightColor = ColorOfFontGrey;

         if (i>=0 && i+RoutePage*ListSize<Table.AnzRows && i<(RoutePage+1)*ListSize)
         {
            HighlightColor=ColorOfFontBlack;

            if (IsBuyable[(SLONG)Routen(Table.LineIndex[i+RoutePage*ListSize])] || Sim.Players.Players[(SLONG)PlayerNum].RentRouten.RentRouten[(SLONG)Routen(Table.LineIndex[i+RoutePage*ListSize])].Rang)
               CheckCursorHighlight (gMousePosition, CRect (458, 11+15+3+i*13, 461+174, 14+15+12+3+i*13), HighlightColor);

            SetMouseLook (CURSOR_HOT, 0, ROOM_ROUTEBOX, 2000);
         }
         else i=-1;

         if ((i!=-1 && i+RoutePage*ListSize!=CurrentTipIndex) || (i==-1 && i!=CurrentTip))
         {
            CurrentTip=Table.LineIndex[i+RoutePage*ListSize];
            if (i!=-1) CurrentTipIndex=i+RoutePage*ListSize;
                  else CurrentTipIndex=-1;
            RepaintTip();
            RepaintMap();
         }
      }
      else if (gMousePosition.IfIsWithin (17,27,434,295) && !gMousePosition.IfIsWithin (9,268,200,434) && !gMousePosition.IfIsWithin (230,254,361,387))
      {
         SLONG c, minc, mindist=-1;

         for (c=Routen.AnzEntries()-1; c>=0; c--)
         if (Routen.IsInAlbum(c) && Routen[c].VonCity<Routen[c].NachCity)
         {
            CRoute &qRoute = Routen[c];

            XY von  = XY(Cities[Routen[c].VonCity].MapPosition);
            XY nach = XY(Cities[Routen[c].NachCity].MapPosition);

            XY p1 = von*MapScale/SLONG(1000);
            XY p2 = nach*MapScale/SLONG(1000);

            p1.x=p1.x*42/60;  p1.y=p1.y*95/90;
            p2.x=p2.x*42/60;  p2.y=p2.y*95/90;

            p1 += MapOffset + XY (19,28);
            p2 += MapOffset + XY (19,28);

            if (abs(nach.x-von.x)<180)
            {
               if (mindist==-1 || CalcDistanceLine2Dot (p1, p2, gMousePosition)<mindist)
               {
                  minc=c;

                  mindist=CalcDistanceLine2Dot (p1, p2, gMousePosition);
               }
            }
            else
            {
               if (p1.x>p2.x) Swap (p1, p2);

               long mitte;
               if (p1.y>p2.y) mitte = p1.y+(p2.y-p1.y)*(p1.x-16)/(437-16-(p2.x-p1.x));
               else           mitte = p2.y+(p1.y-p2.y)*(p1.x-16)/(437-16-(p2.x-p1.x));
               XY pa (1, mitte);
               XY pb (437, mitte);

               if (mindist==-1 || CalcDistanceLine2Dot (p1, pa, gMousePosition)<mindist)
               {
                  minc=c;

                  mindist=CalcDistanceLine2Dot (p1, pa, gMousePosition);
               }
               if (mindist==-1 || CalcDistanceLine2Dot (pb, p2, gMousePosition)<mindist)
               {
                  minc=c;

                  mindist=CalcDistanceLine2Dot (pb, p2, gMousePosition);
               }
            }
         }

         SLONG i=-1;

         if (mindist!=-1 && mindist<10)
         {
            for (SLONG d=0; d<(SLONG)Table.LineIndex.AnzEntries(); d++)
               if (Routen(Table.LineIndex[d])==ULONG(minc))
               {
                  SetMouseLook (CURSOR_HOT, 0, ROOM_ROUTEBOX, 2000);

                  i=d;
                  break;
               }
         }

         if ((i!=-1 && i!=CurrentTipIndex) || (i==-1 && i!=CurrentTip))
         {
            if (i!=-1) CurrentTip=Table.LineIndex[i];
            if (i!=-1) CurrentTipIndex=i;
                  else CurrentTipIndex=-1;
            RepaintTip();
            RepaintMap();
         }
      }
      else if (-1!=CurrentTip)
      {
         CurrentTipIndex=-1;
         RepaintTip();
         RepaintMap();
      }

      if (Sim.ItemClips)
         for (SLONG c=0; c<4; c++)
            if (!Sim.Players.Players[c].IsOut)
            {
               RoomBm.BlitFromT (PlayerBms[c*3+((DisplayPlayer&(1<<c))!=0)*2], PlayerOffsets[c]);

               if (c==PlayerNum || Sim.Players.Players[(SLONG)PlayerNum].HasBerater(BERATERTYP_INFO))
               if (gMousePosition.IfIsWithin (PlayerOffsets[c].x, PlayerOffsets[c].y, PlayerOffsets[c].x+PlayerBms[c*3].Size.x, PlayerOffsets[c].y+PlayerBms[c*3].Size.y))
               {
                  RoomBm.BlitFromT (PlayerBms[c*3+1], PlayerOffsets[c]);
                  SetMouseLook (CURSOR_HOT, 0, ROOM_ROUTEBOX, 20+c);
               }
            }

      //Berater:
      if (CurrentTipIndex!=-1 && 
          (gMousePosition.IfIsWithin (461,14+15,461+171,14+15+ListSize*13-1) || (gMousePosition.IfIsWithin (17,27,434,295) && !gMousePosition.IfIsWithin (9,268,200,434) && !gMousePosition.IfIsWithin (230,254,361,387))))
      {
         if (CurrentTip!=LastTip)
         {
            CString Tip;
            PLAYER &qPlayer = Sim.Players.Players[(SLONG)PlayerNum];

            LastTip=CurrentTip;

            if (qPlayer.HasBerater(BERATERTYP_ROUTE))
            {
               if (qPlayer.RentRouten.RentRouten[(SLONG)Routen(CurrentTip)].Rang==0 && qPlayer.RentRouten.RentRouten[(SLONG)Routen(CurrentTip)].TageMitGering<7)
               {
                  Tip = StandardTexte.GetS (TOKEN_ADVICE, 8020);
               }
               else
               {
                  if (Cities[Routen[CurrentTip].VonCity].Einwohner>2000000 && Cities[Routen[CurrentTip].NachCity].Einwohner>2000000)
                     Tip = bprintf (StandardTexte.GetS (TOKEN_ADVICE, 8000), (LPCTSTR)Cities[Routen[CurrentTip].VonCity].Name, (LPCTSTR)Cities[Routen[CurrentTip].NachCity].Name);
                  else if (Cities[Routen[CurrentTip].VonCity].Einwohner>2000000)
                     Tip = bprintf (StandardTexte.GetS (TOKEN_ADVICE, 8001), (LPCTSTR)Cities[Routen[CurrentTip].VonCity].Name);
                  else if (Cities[Routen[CurrentTip].NachCity].Einwohner>2000000)
                     Tip = bprintf (StandardTexte.GetS (TOKEN_ADVICE, 8001), (LPCTSTR)Cities[Routen[CurrentTip].NachCity].Name);
                  else
                     Tip = bprintf (StandardTexte.GetS (TOKEN_ADVICE, 8002), (LPCTSTR)Cities[Routen[CurrentTip].NachCity].Name);

                  if (Routen[CurrentTip].Faktor > Routen[CurrentTip].Ebene+0.5)
                  {
                     Tip += " ";
                     Tip += StandardTexte.GetS (TOKEN_ADVICE, 8010);
                  }
                  else if (Routen[CurrentTip].Faktor < Routen[CurrentTip].Ebene-0.3)
                  {
                     Tip += " ";
                     Tip += StandardTexte.GetS (TOKEN_ADVICE, 8011);
                  }
               }

               qPlayer.Messages.AddMessage (BERATERTYP_ROUTE, Tip, MESSAGE_COMMENT);
            }
         }
      }
   }
   else
      if (Sim.ItemClips)
         for (SLONG c=0; c<4; c++)
            if (!Sim.Players.Players[c].IsOut)
            {
               RoomBm.BlitFromT (PlayerBms[c*3+((DisplayPlayer&(1<<c))!=0)*2], PlayerOffsets[c]);

               if (c==PlayerNum || Sim.Players.Players[(SLONG)PlayerNum].HasBerater(BERATERTYP_INFO))
               if (gMousePosition.IfIsWithin (PlayerOffsets[c].x, PlayerOffsets[c].y, PlayerOffsets[c].x+PlayerBms[c*3].Size.x, PlayerOffsets[c].y+PlayerBms[c*3].Size.y))
                  RoomBm.BlitFromT (PlayerBms[c*3+1], PlayerOffsets[c]);
            }

   CStdRaum::PostPaint ();
   CStdRaum::PumpToolTips ();
}

//--------------------------------------------------------------------------------------------
//Malt die Liste neu:
//--------------------------------------------------------------------------------------------
void CRouteBox::RepaintList(void)
{
   SLONG c, i;

   ListBm.ReSize (172, 310);
   ListBm.BlitFrom (PicBitmap, -461, -29);

   if (RoutePage>RoutePageMax) RoutePage=RoutePageMax-1;

   for (c=RoutePage*ListSize; c<(RoutePage+1)*ListSize && c<Table.AnzRows; c++)
   {
      SB_CFont *s;
      BOOL      Haken=FALSE;

      i=c-RoutePage*ListSize;
      s=&FontSmallGrey;

      if (Table.ValueFlags[0+c*Table.AnzColums]) { s=&FontSmallBlack; Haken=TRUE; }
      if (IsBuyable[(SLONG)Routen(Table.LineIndex[c])]) s=&FontSmallBlack;

      if (Haken) ListBm.BlitFromT (HakenBm, 4, 1+i*13+2);

      if (ListBm.TryPrintAt (bprintf ("%s - %s", (LPCTSTR)Table.Values[0+c*Table.AnzColums], (LPCTSTR)Table.Values[1+c*Table.AnzColums]), *s, TEC_FONT_LEFT, 13, 3+i*13, 161, 3+i*13+14)<3+12)
         ListBm.PrintAt (bprintf ("%s - %s", (LPCTSTR)Table.Values[0+c*Table.AnzColums], (LPCTSTR)Table.Values[1+c*Table.AnzColums]), *s, TEC_FONT_LEFT, 13, 3+i*13, 161, 3+i*13+14);
      else if (ListBm.TryPrintAt (bprintf ("%s - %s", (LPCTSTR)Table.Values[0+c*Table.AnzColums], (LPCTSTR)Table.Values[3+c*Table.AnzColums]), *s, TEC_FONT_LEFT, 13, 3+i*13, 161, 3+i*13+14)<3+12)
         ListBm.PrintAt (bprintf ("%s - %s", (LPCTSTR)Table.Values[0+c*Table.AnzColums], (LPCTSTR)Table.Values[3+c*Table.AnzColums]), *s, TEC_FONT_LEFT, 13, 3+i*13, 161, 3+i*13+14);
      else
         ListBm.PrintAt (bprintf ("%s - %s", (LPCTSTR)Table.Values[2+c*Table.AnzColums], (LPCTSTR)Table.Values[3+c*Table.AnzColums]), *s, TEC_FONT_LEFT, 13, 3+i*13, 161, 3+i*13+14);
   }

   ListBm.PrintAt (bprintf ("%li / %li", RoutePage+1, RoutePageMax), FontSmallBlack, TEC_FONT_CENTERED, 4, 3+ListSize*13, 161, 3+ListSize*13+14);
}

//--------------------------------------------------------------------------------------------
//Malt den Tip neu:
//--------------------------------------------------------------------------------------------
void CRouteBox::RepaintTip (void)
{
   SLONG c;

   TipBm.ReSize (177, 157);
   TipBm.BlitFrom (PicBitmap, -26, -275);

   if (CurrentTipIndex!=-1)
   {
      CRoute     &qRoute = Routen[CurrentTip];
      CRentRoute &qRRoute = Sim.Players.Players[PlayerNum].RentRouten.RentRouten[(SLONG)Routen(CurrentTip)];

      //Von/Nach
      if (TipBm.TryPrintAt (bprintf ("%s - %s", (LPCTSTR)Table.Values[0+CurrentTipIndex*Table.AnzColums], (LPCTSTR)Table.Values[1+CurrentTipIndex*Table.AnzColums]), FontSmallBlack, TEC_FONT_LEFT, 27+4, 3, 125, 3+14)<3+12)
         TipBm.PrintAt (bprintf ("%s - %s", (LPCTSTR)Table.Values[0+CurrentTipIndex*Table.AnzColums], (LPCTSTR)Table.Values[1+CurrentTipIndex*Table.AnzColums]), FontSmallBlack, TEC_FONT_LEFT, 27+4, 3, 125, 3+14);
      else if (TipBm.TryPrintAt (bprintf ("%s - %s", (LPCTSTR)Table.Values[0+CurrentTipIndex*Table.AnzColums], (LPCTSTR)Table.Values[3+CurrentTipIndex*Table.AnzColums]), FontSmallBlack, TEC_FONT_LEFT, 27+4, 3, 125, 3+14)<3+12)
         TipBm.PrintAt (bprintf ("%s - %s", (LPCTSTR)Table.Values[0+CurrentTipIndex*Table.AnzColums], (LPCTSTR)Table.Values[3+CurrentTipIndex*Table.AnzColums]), FontSmallBlack, TEC_FONT_LEFT, 27+4, 3, 125, 3+14);
      else
         TipBm.PrintAt (bprintf ("%s - %s", (LPCTSTR)Table.Values[2+CurrentTipIndex*Table.AnzColums], (LPCTSTR)Table.Values[3+CurrentTipIndex*Table.AnzColums]), FontSmallBlack, TEC_FONT_LEFT, 27+4, 3, 125, 3+14);

      //Entfernung, Potentielle Nachfrage, Nachfrage
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 1007), FontSmallBlack, TEC_FONT_LEFT, XY(4,27), XY(177, 157));
      TipBm.PrintAt ((CString)Einheiten[EINH_KM].bString (Cities.CalcDistance (qRoute.VonCity, qRoute.NachCity)/1000), FontSmallBlack, TEC_FONT_LEFT, XY(100,27), XY(177, 157));
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 998), FontSmallBlack, TEC_FONT_LEFT, XY(4,40), XY(177, 157));
      TipBm.PrintAt ((CString)Insert1000erDots (qRoute.AnzPassagiere()), FontSmallBlack, TEC_FONT_LEFT, XY(100,40), XY(177, 157));
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 999), FontSmallBlack, TEC_FONT_LEFT, XY(4,53), XY(177, 157));
      TipBm.PrintAt ((CString)Insert1000erDots (qRoute.Bedarf), FontSmallBlack, TEC_FONT_LEFT, XY(100,53), XY(177, 157));

      //Auslastung, Miete
      if (qRRoute.Rang)
      {
         TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 1006), FontSmallBlack, TEC_FONT_LEFT, XY(4,77), XY(177, 157));
         TipBm.PrintAt ((CString)Einheiten[EINH_DM].bString (qRRoute.Miete), FontSmallBlack, TEC_FONT_LEFT, XY(100,77), XY(177, 157));
         TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 1003), FontSmallBlack, TEC_FONT_LEFT, XY(4,90), XY(177, 157));
         TipBm.PrintAt ((CString)Einheiten[EINH_P].bString (qRRoute.RoutenAuslastung), FontSmallBlack, TEC_FONT_LEFT, XY(100,90), XY(177, 157));
      }
      else
      {
         TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 1006), FontSmallBlack, TEC_FONT_LEFT, XY(4,77), XY(177, 157));
         TipBm.PrintAt ((CString)Einheiten[EINH_DM].bString (qRoute.Miete), FontSmallBlack, TEC_FONT_LEFT, XY(100,77), XY(177, 157));
      }

      //Mieter
      for (c=0; c<4; c++)
         if (Sim.Players.Players[c].RentRouten.RentRouten[(SLONG)Routen(CurrentTip)].Rang && (c==PlayerNum || Sim.Players.Players[(SLONG)PlayerNum].HasBerater (BERATERTYP_INFO)))
         {
            TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 1016), FontSmallBlack, TEC_FONT_LEFT, XY(4,114), XY(177, 157));

            for (c=0; c<4; c++)
               if (Sim.Players.Players[c].RentRouten.RentRouten[(SLONG)Routen(CurrentTip)].Rang)
                  if (Sim.Players.Players[(SLONG)PlayerNum].HasBerater(BERATERTYP_INFO))
                     TipBm.PrintAt ((CString)bprintf ("%li. %s (%li%%)", Sim.Players.Players[c].RentRouten.RentRouten[(SLONG)Routen(CurrentTip)].Rang, (LPCTSTR)Sim.Players.Players[c].AirlineX, Sim.Players.Players[c].RentRouten.RentRouten[(SLONG)Routen(CurrentTip)].RoutenAuslastung), FontSmallBlack, TEC_FONT_LEFT, XY(4,114+Sim.Players.Players[c].RentRouten.RentRouten[(SLONG)Routen(CurrentTip)].Rang*12), XY(172,166));
                  else
                     if (c==PlayerNum)
                        TipBm.PrintAt ((CString)bprintf ("%li. %s", Sim.Players.Players[c].RentRouten.RentRouten[(SLONG)Routen(CurrentTip)].Rang, (LPCTSTR)Sim.Players.Players[c].AirlineX), FontSmallBlack, TEC_FONT_LEFT, XY(4,114+Sim.Players.Players[c].RentRouten.RentRouten[(SLONG)Routen(CurrentTip)].Rang*12), XY(172,166));
                     else
                        TipBm.PrintAt ((CString)bprintf ("%li. %s", Sim.Players.Players[c].RentRouten.RentRouten[(SLONG)Routen(CurrentTip)].Rang, StandardTexte.GetS (TOKEN_ROUTE, 997)), FontSmallBlack, TEC_FONT_LEFT, XY(4,114+Sim.Players.Players[c].RentRouten.RentRouten[(SLONG)Routen(CurrentTip)].Rang*12), XY(172,166));
            break;
         }
   }
}

//--------------------------------------------------------------------------------------------
//Malt die Karte mit den Routen neu:
//--------------------------------------------------------------------------------------------
void CRouteBox::RepaintMap (void)
{
   SLONG c, d, Pass;

   MapBm.ReSize (415, 240);
   MapBm.BlitFrom (PicBitmap, -19, -28);

   for (Pass=0; Pass<3; Pass++)
   for (c=Routen.AnzEntries()-1; c>=0; c--)
   if (Routen.IsInAlbum(c))
   {
      CRoute &qRoute = Routen[c];

      if (qRoute.VonCity < qRoute.NachCity)
      {
         BOOL             Fat=FALSE;
         BOOL             CanHaveIt=FALSE;
         BOOL             HasAPlayer=FALSE;
         SB_Hardwarecolor HardwareColors[4];
         SLONG            NumHardwareColors=0;

         if (CurrentTipIndex!=-1)
            if (Routen(CurrentTip)==ULONG(c) || (Routen[CurrentTip].VonCity==Routen[c].NachCity && Routen[CurrentTip].NachCity==Routen[c].VonCity))
               Fat=TRUE;

         if (IsBuyable[c]) CanHaveIt=TRUE;

         for (d=0; d<4; d++)
            if (!Sim.Players.Players[d].IsOut && Sim.Players.Players[d].RentRouten.RentRouten[c].Rang && (DisplayPlayer&(1<<d)))
               HasAPlayer=TRUE;

         if ((CanHaveIt==0 && Pass==0) || (CanHaveIt==1 && Pass==1) || (HasAPlayer==1 && Pass==2))
         {
            for (d=0; d<4; d++)
               if (!Sim.Players.Players[d].IsOut && Sim.Players.Players[d].RentRouten.RentRouten[c].Rang && (DisplayPlayer&(1<<d)))
                  HardwareColors[NumHardwareColors++]=MapBm.pBitmap->GetHardwarecolor (AktienKursLineColor[d]);

            if (NumHardwareColors==0) HardwareColors[NumHardwareColors++]=MapBm.pBitmap->GetHardwarecolor (CanHaveIt ? 0xffffff : (0x5e5e5e + 0xd0a890)/2);

            XY von  = XY(Cities[Routen[c].VonCity].MapPosition);
            XY nach = XY(Cities[Routen[c].NachCity].MapPosition);

            XY p1 = von*MapScale/SLONG(1000);
            XY p2 = nach*MapScale/SLONG(1000);

            p1.x=p1.x*42/60;  p1.y=p1.y*95/90;
            p2.x=p2.x*42/60;  p2.y=p2.y*95/90;

            p1 += MapOffset;
            p2 += MapOffset;

            if (abs(nach.x-von.x)<180)
               MapBm.Line (p1, p2, Fat, HardwareColors, NumHardwareColors);
            else
            {
               if (p1.x>p2.x) Swap (p1, p2);

               long mitte;
               if (p1.y>p2.y) mitte = p1.y+(p2.y-p1.y)*(p1.x-16)/(437-16-(p2.x-p1.x));
               else           mitte = p2.y+(p1.y-p2.y)*(p1.x-16)/(437-16-(p2.x-p1.x));
               XY pa (1, mitte);
               XY pb (437, mitte);

               MapBm.Line (p1, pa, Fat, HardwareColors, NumHardwareColors);
               MapBm.Line (pb, p2, Fat, HardwareColors, NumHardwareColors);
            }
         }
      }
   }

   //Die markierung für die Mission:
   if (Sim.Difficulty==DIFF_NORMAL)
      for (c=0; c<=5; c++)
      {
         XY p1 = XY(Cities[Sim.MissionCities[c]].MapPosition)*MapScale/SLONG(1000);

         p1.x=p1.x*42/60;  p1.y=p1.y*95/90;

         p1 += MapOffset;

         MapBm.BlitFromT (HereBm, p1-XY(3,3));
      }
}

//--------------------------------------------------------------------------------------------
//Füllt den DataTable mit neuen Daten:
//--------------------------------------------------------------------------------------------
void CRouteBox::UpdateDataTable (void)
{
   switch (Filter)
   {
      case 0:
         Table.FillWithAllRouten (&Routen, &Sim.Players.Players[(SLONG)PlayerNum].RentRouten, TRUE);
         break;

      case 1:
         {
            SLONG c, d, e;

            Table.FillWithAllRouten (&Routen, &Sim.Players.Players[(SLONG)PlayerNum].RentRouten, TRUE);

            for (c=d=0; c<Table.AnzRows; c++)
               if (IsBuyable[(SLONG)Routen(Table.LineIndex[c])] && Sim.Players.Players[(SLONG)PlayerNum].RentRouten.RentRouten[(SLONG)Routen(Table.LineIndex[c])].Rang==0)
               {
                  if (c!=d)
                  {
                     Table.LineIndex[d]=Table.LineIndex[c];

                     for (e=0; e<Table.AnzColums; e++)
                     {
                        Table.Values[d*Table.AnzColums+e]=Table.Values[c*Table.AnzColums+e];
                        Table.ValueFlags[d*Table.AnzColums+e]=Table.ValueFlags[c*Table.AnzColums+e];
                     }
                  }

                  d++;
               }

            Table.AnzRows=d;
         }
         break;

      case 2:
         {
            SLONG c, d, e;

            Table.FillWithAllRouten (&Routen, &Sim.Players.Players[(SLONG)PlayerNum].RentRouten, TRUE);

            for (c=d=0; c<Table.AnzRows; c++)
               if (Sim.Players.Players[(SLONG)PlayerNum].RentRouten.RentRouten[(SLONG)Routen(Table.LineIndex[c])].Rang)
               {
                  if (c!=d)
                  {
                     Table.LineIndex[d]=Table.LineIndex[c];

                     for (e=0; e<Table.AnzColums; e++)
                     {
                        Table.Values[d*Table.AnzColums+e]=Table.Values[c*Table.AnzColums+e];
                        Table.ValueFlags[d*Table.AnzColums+e]=Table.ValueFlags[c*Table.AnzColums+e];
                     }
                  }

                  d++;
               }

            Table.AnzRows=d;
         }
         break;
   }
   RoutePageMax = (Table.AnzRows-1)/ListSize+1; 
   RepaintList ();
}

//--------------------------------------------------------------------------------------------
//void CRouteBox::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CRouteBox::OnLButtonDown(UINT nFlags, CPoint point)
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
      //if (gMousePosition.IfIsWithin (179,215,242,261)) RadioFX.Play (0, Sim.Options.OptionEffekte*100/7);

      if (MouseClickArea==ROOM_ROUTEBOX)
      {
         if (MouseClickId==999) qPlayer.LeaveRoom();
         else if (MouseClickId==200)
         {
            qPlayer.BuyItem (ITEM_PAPERCLIP);
            if (qPlayer.HasItem (ITEM_PAPERCLIP))
            {
               Sim.ItemClips=0;
               Sim.SendSimpleMessage (ATNET_TAKETHING, NULL, ITEM_PAPERCLIP);
            }
         }
         else if (MouseClickId==10) { RoutePage--; RepaintList(); gMovePaper.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7); }
         else if (MouseClickId==11) { RoutePage++; RepaintList(); gMovePaper.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7); }
         else if (MouseClickId>=20 && MouseClickId<=23)
         {
            DisplayPlayer^=(1<<(MouseClickId-20));
            RepaintMap ();
         }
         else if (MouseClickId>=30 && MouseClickId<=32) { RoutePage=0; Filter=MouseClickId-30; UpdateDataTable(); gMovePaper.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7); }
         else if (MouseClickId==2000)
         {
            CRoute     &qRoute  = Routen[CurrentTip];
            CRentRoute &qRRoute = Sim.Players.Players[PlayerNum].RentRouten.RentRouten[(SLONG)Routen(CurrentTip)];

            if (qRRoute.Rang)
            {
               SLONG d, RouteB=-1;
            
               for (d=0; d<Routen.Routen.AnzEntries(); d++)
                  if (Routen.IsInAlbum(d) && Routen[d].VonCity==Routen[CurrentTip].NachCity && Routen[d].NachCity==Routen[CurrentTip].VonCity)
                  {
                     RouteB=Routen(d);
                     break;
                  }

               for (SLONG c=qPlayer.Planes.AnzEntries()-1; c>=0; c--)
                  if (qPlayer.Planes.IsInAlbum(c))
                  {
                     for (SLONG d=qPlayer.Planes[c].Flugplan.Flug.AnzEntries()-1; d>=0; d--)
                        if (qPlayer.Planes[c].Flugplan.Flug[d].ObjectType==1)
                        if (Routen(qPlayer.Planes[c].Flugplan.Flug[d].ObjectId)==Routen (CurrentTip) || SLONG(Routen(qPlayer.Planes[c].Flugplan.Flug[d].ObjectId))==RouteB)
                        {
                           MenuStart (MENU_REQUEST, MENU_REQUEST_NORENTROUTE3);
                           return;
                        }
                  }

               MenuStart (MENU_REQUEST, MENU_REQUEST_KILLROUTE, Routen (CurrentTip), RouteB);
            }
            else
            {
               SLONG d, Rang=1, RouteB=-1;

               if (!IsBuyable[(SLONG)Routen(CurrentTip)])
               {
                  if (qPlayer.RentRouten.RentRouten[(SLONG)Routen(CurrentTip)].Rang==0 && qPlayer.RentRouten.RentRouten[(SLONG)Routen(CurrentTip)].TageMitGering<7)
                     MenuStart (MENU_REQUEST, MENU_REQUEST_NORENTROUTE4);
                  else
                     MenuStart (MENU_REQUEST, MENU_REQUEST_NORENTROUTE2);
                  return;
               }

               for (d=0; d<4; d++)
                  if (!Sim.Players.Players[d].IsOut && Sim.Players.Players[d].RentRouten.RentRouten[(SLONG)Routen(CurrentTip)].Rang>=Rang)
                     Rang = Sim.Players.Players[d].RentRouten.RentRouten[(SLONG)Routen(CurrentTip)].Rang+1;
            
               for (d=0; d<Routen.Routen.AnzEntries(); d++)
                  if (Routen.IsInAlbum(d) && Routen[d].VonCity==Routen[CurrentTip].NachCity && Routen[d].NachCity==Routen[CurrentTip].VonCity)
                  {
                     RouteB=Routen(d);
                     break;
                  }

               if (Rang>3)
                  MenuStart (MENU_REQUEST, MENU_REQUEST_NORENTROUTE1);
               else
                  MenuStart (MENU_REQUEST, MENU_REQUEST_RENTROUTE, Routen (CurrentTip), RouteB);
            }
         }
      }
      else CStdRaum::OnLButtonDown(nFlags, point);
   }
}

//--------------------------------------------------------------------------------------------
//void CRouteBox::OnLButtonDblClk(UINT, CPoint point)
//--------------------------------------------------------------------------------------------
void CRouteBox::OnLButtonDblClk(UINT, CPoint point)
{
   if (!IsDialogOpen() && !MenuIsOpen())
   {
      if (MouseClickArea==ROOM_ROUTEBOX)
      {
         if (MouseClickId==10) { RoutePage--; RepaintList(); gMovePaper.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7); }
         else if (MouseClickId==11) { RoutePage++; RepaintList(); gMovePaper.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7); }
      }
   }
}

//--------------------------------------------------------------------------------------------
//void CRouteBox::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CRouteBox::OnRButtonDown(UINT nFlags, CPoint point)
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
