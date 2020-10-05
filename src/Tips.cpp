//============================================================================================
// Tips.cpp : Tips sind die PopUp-Windows mit Detailinformationen zu Flugzeugen, etc..
//============================================================================================
// Link to "proto.h"
//============================================================================================
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char FileId[] = "Tips";

extern ULONG AktienKursLineColor [];

extern SLONG FoodCosts[];

//--------------------------------------------------------------------------------------------
//Zeichnet einen kennzeichnen Stern:
//--------------------------------------------------------------------------------------------
void PaintStarAt (SBBM &Bitmap, XY Pos)
{
   SB_Hardwarecolor color;

   if (Bitmap.Size.x && Bitmap.pBitmap)
   {
      color = Bitmap.pBitmap->GetHardwarecolor (0xffff00);
      Bitmap.pBitmap->SetPixel (Pos.x, Pos.y, color);

      color = Bitmap.pBitmap->GetHardwarecolor (0xa0f000);
      Bitmap.pBitmap->SetPixel (Pos.x-1, Pos.y, color);
      Bitmap.pBitmap->SetPixel (Pos.x+1, Pos.y, color);
      Bitmap.pBitmap->SetPixel (Pos.x, Pos.y+1, color);
      Bitmap.pBitmap->SetPixel (Pos.x, Pos.y-1, color);

      color = Bitmap.pBitmap->GetHardwarecolor (0x20ff20);
      Bitmap.pBitmap->SetPixel (Pos.x-2, Pos.y, color);
      Bitmap.pBitmap->SetPixel (Pos.x+2, Pos.y, color);
      Bitmap.pBitmap->SetPixel (Pos.x, Pos.y+2, color);
      Bitmap.pBitmap->SetPixel (Pos.x, Pos.y-2, color);
      Bitmap.pBitmap->SetPixel (Pos.x-1, Pos.y-1, color);
      Bitmap.pBitmap->SetPixel (Pos.x+1, Pos.y-1, color);
      Bitmap.pBitmap->SetPixel (Pos.x-1, Pos.y+1, color);
      Bitmap.pBitmap->SetPixel (Pos.x+1, Pos.y+1, color);

      color = Bitmap.pBitmap->GetHardwarecolor (0x00c000);
      Bitmap.pBitmap->SetPixel (Pos.x-3, Pos.y, color);
      Bitmap.pBitmap->SetPixel (Pos.x+3, Pos.y, color);
      Bitmap.pBitmap->SetPixel (Pos.x, Pos.y+3, color);
      Bitmap.pBitmap->SetPixel (Pos.x, Pos.y-3, color);
   }
}

//--------------------------------------------------------------------------------------------
//Zeichnet die Infos über eine Stadt in die Bitmap:
//--------------------------------------------------------------------------------------------
void DrawCityTip (SBBM &TipBm, ULONG CityId)
{
   SLONG      c;
   XY         CityPos;

   //Marker setzen:
   CityPos.x = Cities[CityId].MapPosition.x*48/60+109+17;
   CityPos.y = Cities[CityId].MapPosition.y*95/90+107;

   PaintStarAt (TipBm, CityPos);

   //Text:
   TipBm.PrintAt (bprintf ("%s (%s), %s", (LPCTSTR)Cities[CityId].Name, (LPCTSTR)Cities[CityId].Kuerzel, (LPCTSTR)Cities[CityId].Lage), FontSmallWhite, TEC_FONT_LEFT, 16, 4, 265, 82);

   if (CityPos.y<17+160-60) CityPos.y+=5; else CityPos.y-=60;
   CityPos.x = min (max (17, CityPos.x-60), 80);

   TipBm.PrintAt (StandardTexte.GetS (TOKEN_CITY, 1000), FontSmallGrey, TEC_FONT_LEFT, CityPos.x, CityPos.y, 278, 181);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_CITY, 1001), FontSmallGrey, TEC_FONT_LEFT, CityPos.x, CityPos.y+11, 278, 181);
   TipBm.PrintAt (":", FontSmallGrey, TEC_FONT_LEFT, CityPos.x+84, CityPos.y, 278, 181);
   TipBm.PrintAt (":", FontSmallGrey, TEC_FONT_LEFT, CityPos.x+84, CityPos.y+11, 278, 181);
   TipBm.PrintAt (bitoa (Cities[CityId].Einwohner), FontSmallGrey, TEC_FONT_LEFT, CityPos.x+90, CityPos.y, 278, 181);

   if (Sim.Players.Players[Sim.localPlayer].RentCities.RentCities[(SLONG)Cities(CityId)].Rang==0) TipBm.PrintAt (Einheiten[EINH_DM].bString (Cities[CityId].BuroRent), FontSmallGrey, TEC_FONT_LEFT, CityPos.x+90, CityPos.y+11, 278, 181);
   else                                                                                           TipBm.PrintAt (Einheiten[EINH_DM].bString (Sim.Players.Players[Sim.localPlayer].RentCities.RentCities[(SLONG)Cities(CityId)].Miete), FontSmallGrey, TEC_FONT_LEFT, CityPos.x+90, CityPos.y+11, 278, 181);

   //Hit-Liste der Leute in der Stadt
   for (c=0; c<Sim.Players.AnzPlayers; c++)
      if (Cities.GetIdFromIndex(CityId)!=(ULONG)Sim.HomeAirportId)
      {
         if (Sim.Players.Players[c].RentCities.RentCities[(SLONG)Cities(CityId)].Rang!=0)
            TipBm.PrintAt (bprintf ("%li.%s", Sim.Players.Players[c].RentCities.RentCities[(SLONG)Cities(CityId)].Rang, (LPCTSTR)Sim.Players.Players[c].Airline), FontSmallGrey, TEC_FONT_LEFT, CityPos.x, CityPos.y+11*(Sim.Players.Players[c].RentCities.RentCities[(SLONG)Cities(CityId)].Rang+1), 278, 181);
      }
      else
         TipBm.PrintAt (bprintf ("%s", (LPCTSTR)Sim.Players.Players[c].Airline), FontSmallGrey, TEC_FONT_LEFT, CityPos.x, CityPos.y+11*(c+2), 278, 181);
}

//--------------------------------------------------------------------------------------------
//Zeichnet die Infos über eine Stadt in die Bitmap:
//--------------------------------------------------------------------------------------------
void DrawCityTipContents (SBBM &TipBm, ULONG CityId, XY Headline, XY Contents, XY Map, SB_CFont *pHeadFont, SB_CFont *pFont)
{
   SLONG      c;
   XY         CityPos;

   //Marker setzen:
   CityPos.x = Map.x+Cities[CityId].MapPosition.x*34/60+97;
   CityPos.y = Map.y+Cities[CityId].MapPosition.y*73/90+80;

   PaintStarAt (TipBm, CityPos);

   //Text:
   TipBm.PrintAt (bprintf ("%s (%s)", (LPCTSTR)Cities[CityId].Name, (LPCTSTR)Cities[CityId].Kuerzel), *pHeadFont, TEC_FONT_LEFT, Headline.x, Headline.y, TipBm.Size.x, TipBm.Size.y);

   if (CityPos.y<17+160-60) CityPos.y+=5; else CityPos.y-=60;
   CityPos.x = min (max (17, CityPos.x-60), 80);

   TipBm.PrintAt (StandardTexte.GetS (TOKEN_CITY, 1000), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+120, TipBm.Size.x, TipBm.Size.y);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_CITY, 1001), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+131, TipBm.Size.x, TipBm.Size.y);
   TipBm.PrintAt (bitoa (Cities[CityId].Einwohner), *pFont, TEC_FONT_LEFT, Contents.x+90, Contents.y+120, TipBm.Size.x, TipBm.Size.y);

   if (Sim.Players.Players[Sim.localPlayer].RentCities.RentCities[(SLONG)Cities(CityId)].Rang==0) TipBm.PrintAt (Einheiten[EINH_DM].bString (Cities[CityId].BuroRent), *pFont, TEC_FONT_LEFT, Contents.x+90, Contents.y+131, TipBm.Size.x, TipBm.Size.y);
   else                                                                                           TipBm.PrintAt (Einheiten[EINH_DM].bString (Sim.Players.Players[Sim.localPlayer].RentCities.RentCities[(SLONG)Cities(CityId)].Miete), *pFont, TEC_FONT_LEFT, Contents.x+90, Contents.y+131, TipBm.Size.x, TipBm.Size.y);

   //Hit-Liste der Leute in der Stadt
   for (c=0; c<Sim.Players.AnzPlayers; c++)
      if (Cities.GetIdFromIndex(Cities(CityId))!=(ULONG)Sim.HomeAirportId)
      {
         if (Sim.Players.Players[c].RentCities.RentCities[(SLONG)Cities(CityId)].Rang!=0)
            TipBm.PrintAt (bprintf ("%li.%s", Sim.Players.Players[c].RentCities.RentCities[(SLONG)Cities(CityId)].Rang, (LPCTSTR)Sim.Players.Players[c].Airline), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+122+11*(Sim.Players.Players[c].RentCities.RentCities[(SLONG)Cities(CityId)].Rang+1), TipBm.Size.x, TipBm.Size.y);
      }
      else
         TipBm.PrintAt (bprintf ("%s", (LPCTSTR)Sim.Players.Players[c].Airline), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+122+11*(c+2), TipBm.Size.x, TipBm.Size.y);
}

//--------------------------------------------------------------------------------------------
//Zeichnet die Infos über ein Flugzeug in die Bitmap:
//--------------------------------------------------------------------------------------------
void DrawPlaneTip (SBBM &TipBm, SBBM *pTipOriginBm, CPlaneType *PlaneType, CPlane *Plane)
{
   XY         Max;
   XY         Offset;

   //Hintergrund machen:
   TipBm.ReSize (pTipOriginBm->Size);
   TipBm.BlitFrom (*pTipOriginBm);
   
   DrawPlaneTipContents (TipBm, PlaneType, Plane, XY(32,3), XY(32,20), &FontSmallBlack, &FontSmallBlack);
}

//--------------------------------------------------------------------------------------------
//Zeichnet die Infos über ein Flugzeug in die Bitmap:
//--------------------------------------------------------------------------------------------
void DrawPlaneTipContents (SBBM &TipBm, CPlaneType *PlaneType, CPlane *Plane, XY Headline, XY Contents, SB_CFont *pHeadFont, SB_CFont *pFont, BOOL OwnPlane, SLONG OwnedByComputer)
{
   XY Max;

   Max=TipBm.Size-XY(5,5);

   if (Plane)
   {
      //Name (Hersteller Modell):
      TipBm.PrintAt (Plane->Name,  *pHeadFont, TEC_FONT_LEFT, Headline, Max);
      //TipBm.PrintAt (bprintf ("%s %s", Plane->ptHersteller, Plane->ptName), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+0, Max.x, Max.y);
      TipBm.PrintAt (Plane->ptHersteller, *pHeadFont, TEC_FONT_LEFT, Contents.x, Contents.y-5, Max.x, Max.y);
      TipBm.PrintAt (Plane->ptName, *pHeadFont, TEC_FONT_LEFT, Contents.x, Contents.y-5+12, Max.x, Max.y);

      Contents.y+=12;
   }
   else
   {
      //Hersteller Modell:
      TipBm.PrintAt (PlaneType->Hersteller, *pHeadFont, TEC_FONT_LEFT, Headline, Max);
      TipBm.PrintAt (PlaneType->Name, *pHeadFont, TEC_FONT_LEFT, Headline+XY(0,12), Max);
   }

   Contents.y+=15;

   //Block 1 Überschriften:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1000), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+0, Max.x, Max.y);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1017), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+11, Max.x, Max.y);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1001), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+22, Max.x, Max.y);
   //Block 1 Inhalt:
   if (Plane)
   {
      TipBm.PrintAt (bitoa (Plane->ptPassagiere), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+0, Max.x, Max.y);
      TipBm.PrintAt (Einheiten[EINH_T].bString (Plane->ptPassagiere/10), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+11, Max.x, Max.y);
      TipBm.PrintAt (Einheiten[EINH_KM].bString (Plane->ptReichweite), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+22, Max.x, Max.y);
   }
   else
   {
      TipBm.PrintAt (bitoa (PlaneType->Passagiere), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+0, Max.x, Max.y);
      TipBm.PrintAt (Einheiten[EINH_T].bString (PlaneType->Passagiere/10), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+11, Max.x, Max.y);
      TipBm.PrintAt (Einheiten[EINH_KM].bString (PlaneType->Reichweite), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+22, Max.x, Max.y);
   }

   Contents.y+=4;

   //Block 2 Überschriften:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1002), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+33, Max.x, Max.y);
   //Block 2 Inhalt:
   if (Plane)
      TipBm.PrintAt (Einheiten[EINH_KMH].bString (Plane->ptGeschwindigkeit), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+33, Max.x, Max.y);
   else
      TipBm.PrintAt (Einheiten[EINH_KMH].bString (PlaneType->Geschwindigkeit), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+33, Max.x, Max.y);

   if (Plane)
   {
      //Block 3 Überschriften:
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1008), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+44, Max.x, Max.y);
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1009), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+55, Max.x, Max.y);
      //Block 3 Inhalt:
      TipBm.PrintAt (Einheiten[EINH_L].bString (Plane->ptTankgroesse), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+44, Max.x, Max.y);
      TipBm.PrintAt (Einheiten[EINH_LH].bString (Plane->ptVerbrauch),  *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+55, Max.x, Max.y);
   }
   else
   {
      Contents.y+=11;

      //Block 3 Überschriften:
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1007), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+33, Max.x, Max.y);
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1008), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+44, Max.x, Max.y);
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1009), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+55, Max.x, Max.y);
      //Block 3 Inhalt:
      TipBm.PrintAt (Einheiten[EINH_KN].bString (PlaneType->Schub),      *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+33, Max.x, Max.y);
      TipBm.PrintAt (Einheiten[EINH_L].bString (PlaneType->Tankgroesse), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+44, Max.x, Max.y);
      TipBm.PrintAt (Einheiten[EINH_LH].bString (PlaneType->Verbrauch),  *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+55, Max.x, Max.y);
   }

   Contents.y+=4;

   if (OwnPlane)
   {
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1020), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+66, Max.x, Max.y);
      TipBm.PrintAt (Einheiten[EINH_P].bString (Plane->Zustand),   *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+66, Max.x, Max.y);
   }

   Contents.y+=15;

   //Block 4 Überschriften:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1010), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+66, Max.x, Max.y);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1011), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+77, Max.x, Max.y);
   //Block 4 Inhalt:
   if (Plane)
   {
      if (OwnedByComputer)
      {
         TipBm.PrintAt (bprintf (StandardTexte.GetS (TOKEN_PLANE, 1014), Plane->ptAnzPiloten, Plane->ptAnzPiloten),   *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+66, Max.x, Max.y);
         TipBm.PrintAt (bprintf (StandardTexte.GetS (TOKEN_PLANE, 1014), Plane->ptAnzBegleiter, Plane->ptAnzBegleiter), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+77, Max.x, Max.y);
      }
      else
      {
         TipBm.PrintAt (bprintf (StandardTexte.GetS (TOKEN_PLANE, 1014), Plane->AnzPiloten, Plane->ptAnzPiloten),   *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+66, Max.x, Max.y);
         TipBm.PrintAt (bprintf (StandardTexte.GetS (TOKEN_PLANE, 1014), Plane->AnzBegleiter, Plane->ptAnzBegleiter), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+77, Max.x, Max.y);
      }
   }
   else
   {
      TipBm.PrintAt (bitoa (PlaneType->AnzPiloten),   *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+66, Max.x, Max.y);
      TipBm.PrintAt (bitoa (PlaneType->AnzBegleiter), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+77, Max.x, Max.y);
   }

   Contents.y+=4;

   //Block 5 Überschriften:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1012), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+88, Max.x, Max.y);

   if (Plane)
   {
      //Block 5 Inhalt:
      TipBm.PrintAt (Einheiten[EINH_DM].bString (Plane->CalculatePrice()), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+88, Max.x, Max.y);

      //Baujahr:
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1013), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+99, Max.x, Max.y);
      TipBm.PrintAt (bitoa (Plane->Baujahr), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+99, Max.x, Max.y);
   }
   else
   {
      //Block 5 Inhalt:
      TipBm.PrintAt (Einheiten[EINH_DM].bString (PlaneType->Preis), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+88, Max.x, Max.y);
   }
}

//--------------------------------------------------------------------------------------------
//Zeichnet die Infos über ein Flugzeug in die Bitmap:
//--------------------------------------------------------------------------------------------
void DrawXPlaneTipContents (SBBM &TipBm, CString Planename, XY Headline, XY Contents, SB_CFont *pHeadFont, SB_CFont *pFont)
{
   XY Max;

   Max=TipBm.Size-XY(5,5);

   CXPlane plane;
   //CString fn = FullFilename (Planename+".plane", MyPlanePath);
   if (Planename!="") plane.Load (Planename);

   //Hersteller Modell:
   TipBm.PrintAt (plane.Name, *pHeadFont, TEC_FONT_LEFT, Headline, Max);

   Contents.y+=15;

   //Block 1 Überschriften:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1000), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+0, Max.x, Max.y);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1017), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+11, Max.x, Max.y);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1001), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+22, Max.x, Max.y);
   //Block 1 Inhalt:
   TipBm.PrintAt (bitoa (plane.CalcPassagiere()), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+0, Max.x, Max.y);
   TipBm.PrintAt (Einheiten[EINH_T].bString (plane.CalcPassagiere()/10), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+11, Max.x, Max.y);
   TipBm.PrintAt (Einheiten[EINH_KM].bString (plane.CalcReichweite()), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+22, Max.x, Max.y);

   Contents.y+=4;

   //Block 2 Überschriften:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1002), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+33, Max.x, Max.y);
   //Block 2 Inhalt:
   TipBm.PrintAt (Einheiten[EINH_KMH].bString (plane.CalcSpeed()), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+33, Max.x, Max.y);

   Contents.y+=11;

   //Block 3 Überschriften:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1008), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+44, Max.x, Max.y);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1009), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+55, Max.x, Max.y);
   //Block 3 Inhalt:
   TipBm.PrintAt (Einheiten[EINH_L].bString (plane.CalcTank()), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+44, Max.x, Max.y);
   TipBm.PrintAt (Einheiten[EINH_LH].bString (plane.CalcVerbrauch()),  *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+55, Max.x, Max.y);

   Contents.y+=4;

   Contents.y+=15;

   //Block 4 Überschriften:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1010), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+66, Max.x, Max.y);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1011), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+77, Max.x, Max.y);

   TipBm.PrintAt (bitoa (plane.CalcPiloten()),   *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+66, Max.x, Max.y);
   TipBm.PrintAt (bitoa (plane.CalcBegleiter()), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+77, Max.x, Max.y);

   Contents.y+=4;

   //Block 5 Überschriften:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_PLANE, 1012), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+88, Max.x, Max.y);

   //Block 5 Inhalt:
   TipBm.PrintAt (Einheiten[EINH_DM].bString (plane.CalcCost()), *pFont, TEC_FONT_LEFT, Contents.x+70, Contents.y+88, Max.x, Max.y);
}

//--------------------------------------------------------------------------------------------
//Zeichnet die Infos über eine Route in die Bitmap:
//--------------------------------------------------------------------------------------------
void DrawRouteTipContents (SBBM &TipBm, SLONG PlayerNum, ULONG RouteId, SLONG Gate, SLONG Passagiere, SLONG PassagiereFC, SLONG Costs, SLONG Ticketpreis, SLONG TicketpreisFC, XY Headline, XY Contents, SB_CFont *pHeadFont, SB_CFont *pFont, BOOL Unlocked)
{
   SLONG      c;

   if (TipBm.Size.x==0 || TipBm.pBitmap==NULL) return;

   //Fenster-Überschrift:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 900), *pHeadFont, TEC_FONT_LEFT, Headline.x, Headline.y, Contents.x+170, Headline.y+15);

   //Sub-Überschrift:
   if (TipBm.TryPrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[Routen[RouteId].VonCity].Name, (LPCTSTR)Cities[Routen[RouteId].NachCity].Name), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y, Contents.x+170, Contents.y+170)<12)
      TipBm.PrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[Routen[RouteId].VonCity].Name, (LPCTSTR)Cities[Routen[RouteId].NachCity].Name), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y, Contents.x+170, Contents.y+170);
   else if (TipBm.TryPrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[Routen[RouteId].VonCity].Name, (LPCTSTR)Cities[Routen[RouteId].NachCity].Kuerzel), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y, Contents.x+170, Contents.y+170)<12)
      TipBm.PrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[Routen[RouteId].VonCity].Name, (LPCTSTR)Cities[Routen[RouteId].NachCity].Kuerzel), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y, Contents.x+170, Contents.y+170);
   else
      TipBm.PrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[Routen[RouteId].VonCity].Kuerzel, (LPCTSTR)Cities[Routen[RouteId].NachCity].Kuerzel), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y, Contents.x+170, Contents.y+170);

   //Konkrete oder abstrakte Version des Tips?
   if (Costs!=0)
   {
      SLONG tmp=Ticketpreis;

      //Block 1 Überschriften:
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 1014), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+44, Contents.x+170, Contents.y+170);
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 1000), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+55, Contents.x+170, Contents.y+170);
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 1007), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+66, Contents.x+170, Contents.y+170);
      //Block 1 Inhalt:
      TipBm.PrintAt (Einheiten[EINH_KM].bString(Cities.CalcDistance (Routen[RouteId].VonCity, Routen[RouteId].NachCity)/1000), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+44, Contents.x+170, Contents.y+170);
      TipBm.PrintAt ((Passagiere+PassagiereFC)>0?(CString(bitoa(Passagiere))+"/"+CString(bitoa(PassagiereFC))):"?", *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+55, Contents.x+170, Contents.y+170);
      TipBm.PrintAt (CString(bitoa (Ticketpreis))+"/"+CString(Einheiten[EINH_DM].bString (TicketpreisFC)), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+66, Contents.x+170, Contents.y+170);
      
      tmp=Ticketpreis*Passagiere+TicketpreisFC*PassagiereFC;

      //Block 2 Überschriften:
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 1008), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+88, Contents.x+170, Contents.y+170); 
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 1009), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+99, Contents.x+170, Contents.y+170);
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 1010), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+110, Contents.x+170, Contents.y+170);
      //Block 2 Inhalt:
      TipBm.PrintAt ((Passagiere+PassagiereFC)>0?Einheiten[EINH_DM].bString (tmp):"?", *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+88, Contents.x+170, Contents.y+170);
      TipBm.PrintAt (Einheiten[EINH_DM].bString (Costs), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+99, Contents.x+170, Contents.y+170);
      TipBm.PrintAt ((Passagiere+PassagiereFC)>0?Einheiten[EINH_DM].bString (tmp-Costs):"?", *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+110, Contents.x+170, Contents.y+170);

      if (Gate==-1)
      {
         TipBm.PrintAt (StandardTexte.GetS (TOKEN_SCHED, 2002), *pFont, TEC_FONT_LEFT, Contents.x+10, Contents.y+132, Contents.x+170, Contents.y+170); 
         TipBm.BlitFromT (FlugplanBms[18], Contents.x, Contents.y+132);
      }
      else if (Gate>=0)  TipBm.PrintAt (bprintf (StandardTexte.GetS (TOKEN_SCHED, 2001), Gate+1), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+132, Contents.x+170, Contents.y+170); 

      if (!Unlocked)
      {
         TipBm.PrintAt (StandardTexte.GetS (TOKEN_SCHED, 2004), *pFont, TEC_FONT_LEFT, Contents.x+10, Contents.y+152, Contents.x+170, Contents.y+170); 
         TipBm.BlitFromT (FlugplanBms[17], Contents.x-2, Contents.y+152);
      }
   }
   else //abstrakte Version
   {
      //Block 1 Überschriften:
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 1000), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+22, Contents.x+170, Contents.y+170); 
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 1014), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+33, Contents.x+170, Contents.y+170);
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 1001), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+44, Contents.x+170, Contents.y+170);
      //Block 1 Inhalt:
      TipBm.PrintAt ("n/a", FontSmallBlack, TEC_FONT_LEFT, Contents.x+85, Contents.y+22, Contents.x+170, Contents.y+170);
      TipBm.PrintAt (Einheiten[EINH_KM].bString (Cities.CalcDistance (Routen[RouteId].VonCity, Routen[RouteId].NachCity)/1000), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+33, Contents.x+170, Contents.y+170);
      TipBm.PrintAt (CString(bitoa (Ticketpreis))+"/"+CString(Einheiten[EINH_DM].bString (TicketpreisFC)), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+44, Contents.x+170, Contents.y+170);

      //Block 2 Überschriften:
      CRentRoute &qRRoute = Sim.Players.Players[(SLONG)PlayerNum].RentRouten.RentRouten[(SLONG)Routen(RouteId)];
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 1002), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+66, Contents.x+170, Contents.y+170);
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 1003), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+77, Contents.x+170, Contents.y+170);
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 1004), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+88, Contents.x+170, Contents.y+170);
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 1006), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+99, Contents.x+170, Contents.y+170);
      //Block 2 Inhalt:
      TipBm.PrintAt (bitoa (Sim.Players.Players[(SLONG)PlayerNum].AnzPlanesOnRoute(RouteId)), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+66, Contents.x+170, Contents.y+170);
      TipBm.PrintAt (CString(Einheiten[EINH_P].bString (qRRoute.Auslastung)) + " / " + CString(Einheiten[EINH_P].bString (qRRoute.AuslastungFC)), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+77, Contents.x+170, Contents.y+170);
      TipBm.PrintAt (Einheiten[EINH_P].bString (qRRoute.Image),      *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+88, Contents.x+170, Contents.y+170);
      TipBm.PrintAt (Einheiten[EINH_DM].bString (qRRoute.Miete),     *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+99, Contents.x+170, Contents.y+170);

      //Block 3 Überschrift:
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE, 1016), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+121, Contents.x+170, Contents.y+170);

      //Hit-Liste der Leute auf der Route:
      for (c=0; c<Sim.Players.AnzPlayers; c++)
         if (Sim.Players.Players[c].RentRouten.RentRouten[(SLONG)Routen(RouteId)].Rang!=0)
            TipBm.PrintAt (Sim.Players.Players[c].Airline, *pFont, TEC_FONT_LEFT, Contents.x+11, Contents.y+121+11*(Sim.Players.Players[c].RentRouten.RentRouten[(SLONG)Routen(RouteId)].Rang), Contents.x+170, Contents.y+170);
   }
}

//--------------------------------------------------------------------------------------------
//Zeichnet die Infos über ein Flugzeug in die Bitmap:
//--------------------------------------------------------------------------------------------
void DrawAuftragTip (SLONG Player, SBBM &TipBm, SBBMS *pPlaneTips, SBBM *pTipOriginBm, CAuftrag *Auftrag, SLONG Costs, SLONG Okay, XY MapOffset)
{
   //SBBM      &AuftragBm = Costs ? GlobalAuftragProfitTipBm : GlobalAuftragTipBm;

   //Hintergrund machen:
   TipBm.ReSize (pTipOriginBm->Size);
   TipBm.BlitFrom (*pTipOriginBm);

   if (Auftrag) DrawAuftragTipContents (Player, TipBm, pPlaneTips, Auftrag, -2, Costs, Okay, XY(32,3), XY(32,20), MapOffset, &FontSmallBlack, &FontSmallBlack);
}

//--------------------------------------------------------------------------------------------
//Zeichnet die Infos über ein Flugzeug in die Bitmap:
//--------------------------------------------------------------------------------------------
void DrawAuftragTipContents (SLONG PlayerNum, SBBM &TipBm, SBBMS *pPlaneTips, CAuftrag *Auftrag, SLONG Gate, SLONG Costs, SLONG Okay, XY Headline, XY Contents, XY MapOffset, SB_CFont *pHeadFont, SB_CFont *pFont, BOOL Unlocked)
{
   if (TipBm.Size.x==0 || TipBm.pBitmap==NULL) return;

   //Fenster-Überschrift:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 900), *pHeadFont, TEC_FONT_LEFT, Headline.x, Headline.y, Contents.x+170, Headline.y+15);

   //Sub-Überschrift:
   if (TipBm.TryPrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[Auftrag->VonCity].Name, (LPCTSTR)Cities[Auftrag->NachCity].Name), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+0, Contents.x+170, Contents.y+15)<12)
      TipBm.PrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[Auftrag->VonCity].Name, (LPCTSTR)Cities[Auftrag->NachCity].Name), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+0, Contents.x+170, Contents.y+15);
   else if (TipBm.TryPrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[Auftrag->VonCity].Name, (LPCTSTR)Cities[Auftrag->NachCity].Kuerzel), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+0, Contents.x+170, Contents.y+15)<12)
      TipBm.PrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[Auftrag->VonCity].Name, (LPCTSTR)Cities[Auftrag->NachCity].Kuerzel), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+0, Contents.x+170, Contents.y+15);
   else
      TipBm.PrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[Auftrag->VonCity].Kuerzel, (LPCTSTR)Cities[Auftrag->NachCity].Kuerzel), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+0, Contents.x+170, Contents.y+15);

   //Text:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 1003), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+22, Contents.x+170, Contents.y+170);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 1007), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+44, Contents.x+170, Contents.y+170);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 1000), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+55, Contents.x+170, Contents.y+170);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 1002), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+66, Contents.x+170, Contents.y+170);

   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 1001), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+88, Contents.x+170, Contents.y+170);

   //Inhalt:
   if (Sim.Date==Auftrag->BisDate)
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 2002), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+22, Contents.x+170, Contents.y+170);
   else if (Auftrag->Date==Auftrag->BisDate)
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_SCHED, 3010+(Auftrag->Date+Sim.StartWeekday)%7), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+22, Contents.x+170, Contents.y+170);
   else
      TipBm.PrintAt (CString(StandardTexte.GetS (TOKEN_SCHED, 3009))+" "+CString(StandardTexte.GetS (TOKEN_SCHED, 3010+(Auftrag->BisDate+Sim.StartWeekday)%7)), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+22, Contents.x+170, Contents.y+170);

   TipBm.PrintAt ((CString)Einheiten[EINH_KM].bString (Cities.CalcDistance (Auftrag->VonCity, Auftrag->NachCity)/1000), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+44, Contents.x+170, Contents.y+170);
   TipBm.PrintAt (bprintf("%li", Auftrag->Personen), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+55, Contents.x+170, Contents.y+170);
   TipBm.PrintAt (Einheiten[EINH_DM].bString (Auftrag->Strafe), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+66, Contents.x+170, Contents.y+170);

   TipBm.PrintAt (Einheiten[EINH_DM].bString (Auftrag->Praemie), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+88, Contents.x+170, Contents.y+170);

   //Kommentar:
   if (Okay && Okay>=1 && Okay<=3)
   {
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 2320+Okay), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+154, Contents.x+170, Contents.y+178);
   }
   //Kostenrechnung:
   else
   {
      if (Costs)
      {
         TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE,   1009), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+99, Contents.x+170, Contents.y+170);
         TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 1005), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+110, Contents.x+170, Contents.y+170);
         TipBm.PrintAt (Einheiten[EINH_DM].bString (Costs), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+99, Contents.x+170, Contents.y+170);
         TipBm.PrintAt (Einheiten[EINH_DM].bString (Auftrag->Praemie-Costs), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+110, Contents.x+170, Contents.y+170);
      }

      if (Gate==-1)
      {
         TipBm.PrintAt (StandardTexte.GetS (TOKEN_SCHED, 2002), *pFont, TEC_FONT_LEFT, Contents.x+10, Contents.y+132, Contents.x+170, Contents.y+170); 
         TipBm.BlitFromT (FlugplanBms[18], Contents.x, Contents.y+132);
      }
      else if (Gate>=0)  TipBm.PrintAt (bprintf (StandardTexte.GetS (TOKEN_SCHED, 2001), Gate+1), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+132, Contents.x+170, Contents.y+170); 

      if (!Unlocked)
      {
         TipBm.PrintAt (StandardTexte.GetS (TOKEN_SCHED, 2004), *pFont, TEC_FONT_LEFT, Contents.x+10, Contents.y+152, Contents.x+170, Contents.y+170); 
         TipBm.BlitFromT (FlugplanBms[17], Contents.x-2, Contents.y+152);
      }
   }

   //Flugzeuge
   if (PlayerNum!=-1 && pPlaneTips!=NULL)
   {
      SLONG   c;
      PLAYER &qPlayer=Sim.Players.Players[PlayerNum];
      XY      Pos, p, VonPos, NachPos;
      BOOL    bAnyValid=FALSE;

      for (c=qPlayer.Planes.AnzEntries()-1; c>=0; c--)
         if (qPlayer.Planes.IsInAlbum(c))
         {
            //Marker setzen:
            Pos.x = qPlayer.Planes[c].Position.x*34/60+17+101+4+MapOffset.x;
            Pos.y = qPlayer.Planes[c].Position.y*73/90+107+96-15+MapOffset.y;

            //Ungültige Flugzeuge ausgrauen:
            //if ((!Auftrag->FitsInPlane (PlaneTypes[(SLONG)qPlayer.Planes[c].TypeId])) || SLONG(Auftrag->Personen)>qPlayer.Planes[c].MaxPassagiere+qPlayer.Planes[c].MaxPassagiereFC)
            if ((!Auftrag->FitsInPlane (qPlayer.Planes[c])) || SLONG(Auftrag->Personen)>qPlayer.Planes[c].MaxPassagiere+qPlayer.Planes[c].MaxPassagiereFC)
               TipBm.BlitFromT ((pPlaneTips[4])[qPlayer.Planes[c].GlobeAngle], Pos.x-9, Pos.y-9);
         }

      for (c=qPlayer.Planes.AnzEntries()-1; c>=0; c--)
         if (qPlayer.Planes.IsInAlbum(c))
         {
            //Marker setzen:
            Pos.x = qPlayer.Planes[c].Position.x*34/60+17+101+4+MapOffset.x;
            Pos.y = qPlayer.Planes[c].Position.y*73/90+107+96-15+MapOffset.y;

            //gültige Flugzeuge zeichnen:
            //if (Auftrag->FitsInPlane (PlaneTypes[(SLONG)qPlayer.Planes[c].TypeId]) && SLONG(Auftrag->Personen)<=qPlayer.Planes[c].MaxPassagiere+qPlayer.Planes[c].MaxPassagiereFC)
            if (Auftrag->FitsInPlane (qPlayer.Planes[c]) && SLONG(Auftrag->Personen)<=qPlayer.Planes[c].MaxPassagiere+qPlayer.Planes[c].MaxPassagiereFC)
            {
               TipBm.BlitFromT ((pPlaneTips[PlayerNum])[qPlayer.Planes[c].GlobeAngle], Pos.x-9, Pos.y-9);
               bAnyValid=TRUE;
            }
         }

      //Pfeil zeichnen:
      VonPos.x = Cities[Auftrag->VonCity].MapPosition.x*34/60+17+101+4+MapOffset.x;
      VonPos.y = Cities[Auftrag->VonCity].MapPosition.y*73/90+107+96-15+MapOffset.y;
      NachPos.x = Cities[Auftrag->NachCity].MapPosition.x*34/60+17+101+4+MapOffset.x;
      NachPos.y = Cities[Auftrag->NachCity].MapPosition.y*73/90+107+96-15+MapOffset.y;

      //Marker setzen:
      PaintStarAt (TipBm, VonPos);
      PaintStarAt (TipBm, NachPos);

      SB_Hardwarecolor red;

      if (bAnyValid) red = TipBm.pBitmap->GetHardwarecolor (0xffff00);
                else red = TipBm.pBitmap->GetHardwarecolor (0x404040);

      if (abs(Cities[Auftrag->VonCity].MapPosition.x-Cities[Auftrag->NachCity].MapPosition.x)<180)
         TipBm.pBitmap->Line (VonPos.x, VonPos.y, NachPos.x, NachPos.y, red);
      else
      {
         XY p1=VonPos, p2=NachPos;

         if (p1.x>p2.x) Swap (p1, p2);

         if ((190-(p2.x-p1.x))!=0)
         {
            p1-=MapOffset; p2-=MapOffset;
            XY pa (36, p1.y+(p2.y-p1.y)*(p1.x-36)/(190-(p2.x-p1.x)));
            XY pb (190+36, p1.y+(p2.y-p1.y)*(p1.x-36)/(190-(p2.x-p1.x)));
            pa+=MapOffset; pb+=MapOffset;
            p1+=MapOffset; p2+=MapOffset;

            TipBm.pBitmap->Line (p1.x, p1.y, pa.x, pa.y, red);
            TipBm.pBitmap->Line (pb.x, pb.y, p2.x, p2.y, red);

            if (p1==VonPos) VonPos=pb;
            else { VonPos=pa; }
         }
      }

      Pos=NachPos-VonPos;
      if (Pos.abs()>0) Pos=Pos*10l/SLONG(Pos.abs());

      p.x= ((-Pos.y)+Pos.x)/2;
      p.y= (( Pos.x)+Pos.y)/2;
      TipBm.pBitmap->Line (NachPos.x-p.x, NachPos.y-p.y, NachPos.x, NachPos.y, red);
      TipBm.pBitmap->Line (NachPos.x-p.y, NachPos.y+p.x, NachPos.x, NachPos.y, red);
   }
}

//--------------------------------------------------------------------------------------------
//Zeichnet die Infos über ein Flugzeug in die Bitmap:
//--------------------------------------------------------------------------------------------
void DrawFrachtTip (SLONG Player, SBBM &TipBm, SBBMS *pPlaneTips, SBBM *pTipOriginBm, CFracht *Fracht, SLONG TonsThis, SLONG Costs, SLONG Income, SLONG Okay, XY MapOffset)
{
   //Hintergrund machen:
   TipBm.ReSize (pTipOriginBm->Size);
   TipBm.BlitFrom (*pTipOriginBm);

   if (Fracht) DrawFrachtTipContents (Player, TipBm, pPlaneTips, Fracht, TonsThis, Costs, Income, Okay, XY(32,3), XY(32,20), MapOffset, &FontSmallBlack, &FontSmallBlack);
}

//--------------------------------------------------------------------------------------------
//Zeichnet die Infos über ein Flugzeug in die Bitmap:
//--------------------------------------------------------------------------------------------
void DrawFrachtTipContents (SLONG PlayerNum, SBBM &TipBm, SBBMS *pPlaneTips, CFracht *Fracht, SLONG TonsThis, SLONG Costs, SLONG Income, SLONG Okay, XY Headline, XY Contents, XY MapOffset, SB_CFont *pHeadFont, SB_CFont *pFont, BOOL Unlocked)
{
   if (TipBm.Size.x==0 || TipBm.pBitmap==NULL) return;

   SLONG Offset = Costs?30:0;

   //Fenster-Überschrift:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 902), *pHeadFont, TEC_FONT_LEFT, Headline.x, Headline.y, Contents.x+170, Headline.y+15);

   //Sub-Überschrift:
   if (TipBm.TryPrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[Fracht->VonCity].Name, (LPCTSTR)Cities[Fracht->NachCity].Name), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+0, Contents.x+170, Contents.y+15)<12)
      TipBm.PrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[Fracht->VonCity].Name, (LPCTSTR)Cities[Fracht->NachCity].Name), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+0, Contents.x+170, Contents.y+15);
   else if (TipBm.TryPrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[Fracht->VonCity].Name, (LPCTSTR)Cities[Fracht->NachCity].Kuerzel), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+0, Contents.x+170, Contents.y+15)<12)
      TipBm.PrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[Fracht->VonCity].Name, (LPCTSTR)Cities[Fracht->NachCity].Kuerzel), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+0, Contents.x+170, Contents.y+15);
   else
      TipBm.PrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[Fracht->VonCity].Kuerzel, (LPCTSTR)Cities[Fracht->NachCity].Kuerzel), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+0, Contents.x+170, Contents.y+15);

   //Text:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 1003), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+22, Contents.x+170, Contents.y+170);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 1007), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+44, Contents.x+170, Contents.y+170);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 1008), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+55, Contents.x+170, Contents.y+170);

   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 1002), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+66+Offset, Contents.x+170, Contents.y+170);

   //Inhalt:
   if (Sim.Date==Fracht->BisDate)
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 2002), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+22, Contents.x+170, Contents.y+170);
   else if (Fracht->Date==Fracht->BisDate)
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_SCHED, 3010+(Fracht->Date+Sim.StartWeekday)%7), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+22, Contents.x+170, Contents.y+170);
   else
      TipBm.PrintAt (CString(StandardTexte.GetS (TOKEN_SCHED, 3009))+" "+CString(StandardTexte.GetS (TOKEN_SCHED, 3010+(Fracht->BisDate+Sim.StartWeekday)%7)), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+22, Contents.x+170, Contents.y+170);

   TipBm.PrintAt ((CString)Einheiten[EINH_KM].bString (Cities.CalcDistance (Fracht->VonCity, Fracht->NachCity)/1000), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+44, Contents.x+170, Contents.y+170);

   if (TonsThis)
      TipBm.PrintAt (CString (Einheiten[EINH_T].bString (TonsThis))+" / "+Einheiten[EINH_T].bString (Fracht->Tons), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+55, Contents.x+170, Contents.y+170);
   else
      TipBm.PrintAt (Einheiten[EINH_T].bString (Fracht->Tons), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+55, Contents.x+170, Contents.y+170);

   TipBm.PrintAt (Einheiten[EINH_DM].bString (Fracht->Strafe), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+66+Offset, Contents.x+170, Contents.y+170);

   if (Okay==0)
   {
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 1001), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+88+Offset, Contents.x+170, Contents.y+170);
      if ((!Costs && Fracht->Praemie==0) || (Costs && Income==0))
         TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 1011), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+88+Offset, Contents.x+170, Contents.y+170);
      else
         TipBm.PrintAt (Einheiten[EINH_DM].bString (Costs?Income:Fracht->Praemie), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+88+Offset, Contents.x+170, Contents.y+170);
   }

   if (Costs)
   {
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 1010), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+66, Contents.x+170, Contents.y+170);
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 1009), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+77, Contents.x+170, Contents.y+170);

      TipBm.PrintAt (Einheiten[EINH_T].bString (Fracht->TonsLeft), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+66, Contents.x+170, Contents.y+170);
      TipBm.PrintAt (Einheiten[EINH_T].bString (Fracht->TonsOpen), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+77, Contents.x+170, Contents.y+170);
   }

   //Kommentar:
   if (Okay && Okay>=1 && Okay<=3)
   {
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 2320+Okay), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+100+Offset, Contents.x+170, Contents.y+178);
   }
   //Kostenrechnung:
   else
   {
      if (Costs)
      {
         TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE,   1009), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+99+Offset, Contents.x+170, Contents.y+170);
         TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 1005), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+110+Offset, Contents.x+170, Contents.y+170);
         TipBm.PrintAt (Einheiten[EINH_DM].bString (Costs), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+99+Offset, Contents.x+170, Contents.y+170);
         TipBm.PrintAt (Einheiten[EINH_DM].bString (Income-Costs), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+110+Offset, Contents.x+170, Contents.y+170);
      }

      if (!Unlocked)
      {
         TipBm.PrintAt (StandardTexte.GetS (TOKEN_SCHED, 2004), *pFont, TEC_FONT_LEFT, Contents.x+10, Contents.y+152, Contents.x+170, Contents.y+170); 
         TipBm.BlitFromT (FlugplanBms[17], Contents.x-2, Contents.y+152);
      }
      else if (TonsThis==0 && !(PlayerNum!=-1 && pPlaneTips!=NULL))
      {
         TipBm.PrintAt (StandardTexte.GetS (TOKEN_SCHED, 2005), *pFont, TEC_FONT_LEFT, Contents.x+10, Contents.y+132+25, Contents.x+170, Contents.y+180); 
         TipBm.BlitFromT (FlugplanBms[18], Contents.x, Contents.y+132+25);
      }
   }

   //Flugzeuge
   if (PlayerNum!=-1 && pPlaneTips!=NULL)
   {
      SLONG   c;
      PLAYER &qPlayer=Sim.Players.Players[PlayerNum];
      XY      Pos, p, VonPos, NachPos;
      BOOL    bAnyValid=FALSE;

      for (c=qPlayer.Planes.AnzEntries()-1; c>=0; c--)
         if (qPlayer.Planes.IsInAlbum(c))
         {
            //Marker setzen:
            Pos.x = qPlayer.Planes[c].Position.x*34/60+17+101+4+MapOffset.x;
            Pos.y = qPlayer.Planes[c].Position.y*73/90+107+96-15+MapOffset.y;

            //Ungültige Flugzeuge ausgrauen:
            //if (!Fracht->FitsInPlane (PlaneTypes[(SLONG)qPlayer.Planes[c].TypeId]))
            if (!Fracht->FitsInPlane (qPlayer.Planes[c]))
               TipBm.BlitFromT ((pPlaneTips[4])[qPlayer.Planes[c].GlobeAngle], Pos.x-9, Pos.y-9);
         }

      for (c=qPlayer.Planes.AnzEntries()-1; c>=0; c--)
         if (qPlayer.Planes.IsInAlbum(c))
         {
            //Marker setzen:
            Pos.x = qPlayer.Planes[c].Position.x*34/60+17+101+4+MapOffset.x;
            Pos.y = qPlayer.Planes[c].Position.y*73/90+107+96-15+MapOffset.y;

            //gültige Flugzeuge zeichnen:
            //if (Fracht->FitsInPlane (PlaneTypes[(SLONG)qPlayer.Planes[c].TypeId]))
            if (Fracht->FitsInPlane (qPlayer.Planes[c]))
            {
               TipBm.BlitFromT ((pPlaneTips[PlayerNum])[qPlayer.Planes[c].GlobeAngle], Pos.x-9, Pos.y-9);
               bAnyValid=TRUE;
            }
         }

      //Pfeil zeichnen:
      VonPos.x = Cities[Fracht->VonCity].MapPosition.x*34/60+17+101+4+MapOffset.x;
      VonPos.y = Cities[Fracht->VonCity].MapPosition.y*73/90+107+96-15+MapOffset.y;
      NachPos.x = Cities[Fracht->NachCity].MapPosition.x*34/60+17+101+4+MapOffset.x;
      NachPos.y = Cities[Fracht->NachCity].MapPosition.y*73/90+107+96-15+MapOffset.y;

      //Marker setzen:
      PaintStarAt (TipBm, VonPos);
      PaintStarAt (TipBm, NachPos);

      SB_Hardwarecolor red;

      if (bAnyValid) red = TipBm.pBitmap->GetHardwarecolor (0xffff00);
                else red = TipBm.pBitmap->GetHardwarecolor (0x404040);

      if (abs(Cities[Fracht->VonCity].MapPosition.x-Cities[Fracht->NachCity].MapPosition.x)<180)
         TipBm.pBitmap->Line (VonPos.x, VonPos.y, NachPos.x, NachPos.y, red);
      else
      {
         XY p1=VonPos, p2=NachPos;

         if (p1.x>p2.x) Swap (p1, p2);

         if ((190-(p2.x-p1.x))!=0)
         {
            p1-=MapOffset; p2-=MapOffset;
            XY pa (36, p1.y+(p2.y-p1.y)*(p1.x-36)/(190-(p2.x-p1.x)));
            XY pb (190+36, p1.y+(p2.y-p1.y)*(p1.x-36)/(190-(p2.x-p1.x)));
            pa+=MapOffset; pb+=MapOffset;
            p1+=MapOffset; p2+=MapOffset;

            TipBm.pBitmap->Line (p1.x, p1.y, pa.x, pa.y, red);
            TipBm.pBitmap->Line (pb.x, pb.y, p2.x, p2.y, red);

            if (p1==VonPos) VonPos=pb;
            else { VonPos=pa; }
         }
      }

      Pos=NachPos-VonPos;
      if (Pos.abs()>0) Pos=Pos*10l/SLONG(Pos.abs());

      p.x= ((-Pos.y)+Pos.x)/2;
      p.y= (( Pos.x)+Pos.y)/2;
      TipBm.pBitmap->Line (NachPos.x-p.x, NachPos.y-p.y, NachPos.x, NachPos.y, red);
      TipBm.pBitmap->Line (NachPos.x-p.y, NachPos.y+p.x, NachPos.x, NachPos.y, red);
   }
}

//--------------------------------------------------------------------------------------------
//Zeichnet die Infos über ein Flugzeug in die Bitmap:
//--------------------------------------------------------------------------------------------
void DrawAutoflugTipContents (SBBM &TipBm, SLONG Costs, SLONG NotPassengers, SLONG VonCity, SLONG NachCity, XY Headline, XY Contents, SB_CFont *pHeadFont, SB_CFont *pFont, BOOL Unlocked)
{
   //Fenster-Überschrift:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 901), *pHeadFont, TEC_FONT_LEFT, Headline.x, Headline.y, Contents.x+170, Headline.y+15);

   //Sub-Überschrift:
   if (TipBm.TryPrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[VonCity].Name, (LPCTSTR)Cities[NachCity].Name), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+0, Contents.x+170, Contents.y+15)<12) TipBm.PrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[VonCity].Name, (LPCTSTR)Cities[NachCity].Name), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+0, Contents.x+170, Contents.y+15);
   else if (TipBm.TryPrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[VonCity].Name, (LPCTSTR)Cities[NachCity].Kuerzel), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+0, Contents.x+170, Contents.y+15)<12) TipBm.PrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[VonCity].Name, (LPCTSTR)Cities[NachCity].Kuerzel), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+0, Contents.x+170, Contents.y+15);
   else TipBm.PrintAt (bprintf ("%s - %s", (LPCTSTR)Cities[VonCity].Kuerzel, (LPCTSTR)Cities[NachCity].Kuerzel), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+0, Contents.x+170, Contents.y+15);

   //Text:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AUFTRAG, 1007), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+44, Contents.x+170, Contents.y+170);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE,   1300), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+55, Contents.x+170, Contents.y+170);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE,   1008), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+88, Contents.x+170, Contents.y+170); 
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE,   1009), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+99, Contents.x+170, Contents.y+170);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_ROUTE,   1010), *pFont, TEC_FONT_LEFT, Contents.x, Contents.y+110, Contents.x+170, Contents.y+170);

   //Inhalt:
   SLONG Einnahmen=NotPassengers*Cities.CalcDistance (VonCity, NachCity)/1000/40;

   TipBm.PrintAt (Einheiten[EINH_KM].bString (Cities.CalcDistance (VonCity, NachCity)/1000), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+44, Contents.x+170, Contents.y+170);
   TipBm.PrintAt ((CString)Einheiten[EINH_KG].bString (NotPassengers*30), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+55, Contents.x+170, Contents.y+170);
   TipBm.PrintAt ((CString)Einheiten[EINH_DM].bString (Einnahmen), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+88, Contents.x+170, Contents.y+170);
   TipBm.PrintAt ((CString)Einheiten[EINH_DM].bString (Costs), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+99, Contents.x+170, Contents.y+170);
   TipBm.PrintAt ((CString)Einheiten[EINH_DM].bString (Einnahmen-Costs), *pFont, TEC_FONT_LEFT, Contents.x+85, Contents.y+110, Contents.x+170, Contents.y+170);

   if (!Unlocked)
   {
      TipBm.PrintAt (StandardTexte.GetS (TOKEN_SCHED, 2004), *pFont, TEC_FONT_LEFT, Contents.x+10, Contents.y+152, Contents.x+170, Contents.y+170); 
      TipBm.BlitFromT (FlugplanBms[17], Contents.x-2, Contents.y+152);
   }
}

//--------------------------------------------------------------------------------------------
//Zeichnet die Infos über eine Aktie in die Bitmap:
//--------------------------------------------------------------------------------------------
void DrawKursTipContents (SBBM &TipBm, SLONG PlayerView, SLONG PlayerAktie, SB_CFont *pHeadFont, SB_CFont *pFont)
{
   SLONG c, Max;

   if (TipBm.Size.x==0 || TipBm.pBitmap==NULL) return;

   //Fenster-Überschrift:
   TipBm.PrintAt (Sim.Players.Players[PlayerAktie].Airline, *pHeadFont, TEC_FONT_LEFT, 32, 3, 218, 15);

   //Block 1 - Aktie am Markt:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AKTIE, 1000), *pFont, TEC_FONT_LEFT, 32, 20, 170, 160);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AKTIE, 1001), *pFont, TEC_FONT_LEFT, 42, 31, 170, 160);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AKTIE, 1002), *pFont, TEC_FONT_LEFT, 42, 42, 170, 160);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AKTIE, 1003), *pFont, TEC_FONT_LEFT, 42, 53, 170, 160);
   TipBm.PrintAt (Einheiten[EINH_DM].bString((SLONG)Sim.Players.Players[PlayerAktie].Kurse[0]), *pFont, TEC_FONT_LEFT, 98, 32, 170, 160);
   TipBm.PrintAt (bitoa(Sim.Players.Players[PlayerAktie].Dividende), *pFont, TEC_FONT_LEFT, 98, 42, 170, 160);
   TipBm.PrintAt (Einheiten[EINH_P].bString((SLONG)(__int64(Sim.Players.Players[PlayerAktie].Dividende)*100/Sim.Players.Players[PlayerAktie].Kurse[0])), *pFont, TEC_FONT_LEFT, 98, 53, 170, 160);

   //Block 2 - Aktie im Protefeuille:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AKTIE, 1004), *pFont, TEC_FONT_LEFT, 32, 75, 200, 160);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AKTIE, 1005), *pFont, TEC_FONT_LEFT, 42, 86, 200, 160);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AKTIE, 1006), *pFont, TEC_FONT_LEFT, 42, 97, 200, 160);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AKTIE, 1007), *pFont, TEC_FONT_LEFT, 42, 108, 200, 160);
   TipBm.PrintAt (bitoa(Sim.Players.Players[PlayerView].OwnsAktien[PlayerAktie]), *pFont, TEC_FONT_LEFT, 98, 86, 200, 160);
   TipBm.PrintAt (Einheiten[EINH_DM].bString64(Sim.Players.Players[PlayerView].AktienWert[PlayerAktie]), *pFont, TEC_FONT_LEFT, 98, 97, 200, 160);
   TipBm.PrintAt (Einheiten[EINH_DM].bString64(__int64(Sim.Players.Players[PlayerAktie].Kurse[0])*Sim.Players.Players[PlayerView].OwnsAktien[PlayerAktie]), *pFont, TEC_FONT_LEFT, 98, 108, 200, 160);

   //Block 3 - Anteilseigner:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_AKTIE, 1008), *pFont, TEC_FONT_LEFT, 32, 130, 200, 160);

   //Alle vier Spieler auflisten:
   for (c=0; c<4; c++)
   {
      TipBm.PrintAt (Sim.Players.Players[c].Name, *pFont, TEC_FONT_LEFT, 42, 141+c*11, 200, 210);

      TipBm.PrintAt (Einheiten[EINH_P].bString(SLONG(Sim.Players.Players[c].OwnsAktien[PlayerAktie]*__int64(100)/Sim.Players.Players[PlayerAktie].AnzAktien)), *pFont, TEC_FONT_LEFT, 150,141+c*11, 180, 210);
   }

   //Graph zeichnen:
   for (Max=c=0; c<10; c++)
      if (Max<Sim.Players.Players[PlayerAktie].Kurse[c])
         Max=SLONG(Sim.Players.Players[PlayerAktie].Kurse[c]);
   Max=Max+Max/2;

   TipBm.Line (140, 79-31, 200, 79-31, 0x084848);
   TipBm.Line (140, 79, 140, 79-62, 0x080808);
   TipBm.Line (140, 79, 200, 79, 0x080808);

   for (c=0; c<9; c++)
      TipBm.Line (141+60*c/9,
                  SLONG(79-Sim.Players.Players[PlayerAktie].Kurse[9-c]*61/Max),
                  141+60*(c+1)/9,
                  SLONG(79-Sim.Players.Players[PlayerAktie].Kurse[9-(c+1)]*61/Max),
                  AktienKursLineColor[PlayerAktie]);
}

//--------------------------------------------------------------------------------------------
//Zeichnet die Infos über einen Gegenstand in die Bitmap:
//--------------------------------------------------------------------------------------------
void DrawItemTipContents (SBBM &TipBm, SLONG Item, SB_CFont *pHeadFont, SB_CFont *pFont)
{
   //Fenster-Überschrift:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_ITEM, 1000+Item), *pHeadFont, TEC_FONT_LEFT, 32, 4, 218, 15);

   //Text:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_ITEM, 100), *pFont, TEC_FONT_LEFT, 32, 20, 188, 160);
   TipBm.PrintAt (Einheiten[EINH_DM].bString(atoi(StandardTexte.GetS (TOKEN_ITEM, 2000+Item))), *pFont, TEC_FONT_LEFT, 81, 20, 188, 160);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_ITEM, 3000+Item), *pFont, TEC_FONT_LEFT, 32, 42, 188, 160);
}

//--------------------------------------------------------------------------------------------
//Zeichnet den Tip fürs Geld, das man ausgegeben hat:
//--------------------------------------------------------------------------------------------
void DrawMoneyTip (SBBM &TipBm, SLONG PlayerNum, SLONG Page)
{
   SLONG      c;
   __int64    money;
   PLAYER    &qPlayer = Sim.Players.Players[PlayerNum];

   SB_CFont       FontBankBlack;
   SB_CFont       FontBankRed;

   Hdu.HercPrintf (0, "bank_bl.mcf");
   FontBankBlack.Load (lpDD, (char*)(LPCTSTR)FullFilename ("bank_bl.mcf", MiscPath));
   Hdu.HercPrintf (0, "bank_ro.mcf");
   FontBankRed.Load (lpDD, (char*)(LPCTSTR)FullFilename ("bank_ro.mcf", MiscPath));

   //Fenster-Überschrift:
   {
      time_t     Time = Sim.StartTime + Sim.Date*60*60*24;
      struct tm *pTimeStruct = localtime (&Time);

      TipBm.PrintAt (bprintf ((LPCTSTR)(CString)StandardTexte.GetS (TOKEN_MONEY, 1000), (LPCTSTR)(CString)StandardTexte.GetS (TOKEN_SCHED, 3010+(pTimeStruct->tm_wday+6)%7), pTimeStruct->tm_mday, pTimeStruct->tm_mon+1), FontBankRed, TEC_FONT_LEFT, 28, 11, 400, 214);
   }

   for (c=0; c<Page*10; c++)
      if (Sim.Players.Players[PlayerNum].History.HistoryLine[c].Description != "*") break;

   TipBm.PrintAt (bprintf ("%li/%li", 10-Page, 10-c/10), FontBankRed, TEC_FONT_RIGHT, 28, 11, 390, 214);

   money=Sim.Players.Players[PlayerNum].History.HistoricMoney;
   for (c=0; c<Page*10; c++)
      money+=Sim.Players.Players[PlayerNum].History.HistoryLine[c].Money;

   //Text:
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_MONEY, 1003), FontBankBlack, TEC_FONT_LEFT, 28, 34, 400, 214);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_MONEY, 1004), FontBankBlack, TEC_FONT_LEFT, 28, 54, 400, 214);
   TipBm.PrintAt (StandardTexte.GetS (TOKEN_MONEY, 1005), FontBankBlack, TEC_FONT_LEFT, 28, 193, 400, 214);

   //Numbers:
   TipBm.PrintAt (Einheiten[EINH_DM].bString64(Sim.Players.Players[PlayerNum].Credit), FontBankBlack, TEC_FONT_RIGHT, 28, 34, 378, 162);
   TipBm.PrintAt (Einheiten[EINH_DM].bString64(money), FontBankBlack, TEC_FONT_RIGHT, 28, 54, 378, 162);

   //Kontobewegungen:
   for (c=Page*10; c<Page*10+10; c++)
   {
      if (Sim.Players.Players[PlayerNum].History.HistoryLine[c].Description!="*" && strncmp(Sim.Players.Players[PlayerNum].History.HistoryLine[c].Description, "----", 4)!=0)
      {
         TipBm.PrintAt ((LPCTSTR)Sim.Players.Players[PlayerNum].History.HistoryLine[c].Description, FontBankBlack, TEC_FONT_LEFT, 36, 72+(c-Page*10)*11, 378, 214);
         TipBm.PrintAt (Einheiten[EINH_DM].bString64(Sim.Players.Players[PlayerNum].History.HistoryLine[c].Money), FontBankBlack, TEC_FONT_RIGHT, 36, 72+(c-Page*10)*11, 378, 214);
         money+=Sim.Players.Players[PlayerNum].History.HistoryLine[c].Money;
      }
   }

   TipBm.PrintAt (Einheiten[EINH_DM].bString64(money), FontBankBlack, TEC_FONT_RIGHT, 28, 193, 378, 162);
}

//--------------------------------------------------------------------------------------------
//Zeichnet die Flugzeugliste auf das Notepad:
//--------------------------------------------------------------------------------------------
void DrawPlanesNotepad (SBBM &NotepadBm, CDataTable *Table, SLONG CountFrom, SBBMS *pMenuBms, BOOL DisplayWartungskosten)
{
   SLONG c;

   //Hintergrund machen:
   NotepadBm.ReSize ((*pMenuBms)[0].Size);  //gNotepadBm
   NotepadBm.BlitFrom ((*pMenuBms)[0]);

   //Überschriften:
   NotepadBm.PrintAt (Table->ColTitle[0], FontSmallBlack, TEC_FONT_LEFT, XY(216, 12), (*pMenuBms)[0].Size);

   if (DisplayWartungskosten)
      NotepadBm.PrintAt (Table->ColTitle[2], FontSmallBlack, TEC_FONT_RIGHT, XY(335, 12), (*pMenuBms)[0].Size-XY(2,0));
   else
      NotepadBm.PrintAt (Table->ColTitle[1], FontSmallBlack, TEC_FONT_RIGHT, XY(335, 12), (*pMenuBms)[0].Size-XY(2,0));

   for (c=CountFrom; c<CountFrom+13 && c<Table->AnzRows; c++)
   {
      SB_CFont *pFont1;
      CString   Text2;

      if (Table->ValueFlags[0+c*Table->AnzColums]) pFont1=&FontSmallRed;
                                              else pFont1=&FontSmallBlack;
      if (DisplayWartungskosten) Text2=Table->Values[1+c*Table->AnzColums];
                            else Text2=Table->Values[1+c*Table->AnzColums];

      for (SLONG x=170; x>=0; x-=4)
         if (NotepadBm.TryPrintAt (Text2, FontSmallBlack, TEC_FONT_RIGHT, XY(216+x, 25+(c-CountFrom)*13), XY((*pMenuBms)[0].Size.x-2,25+(c-CountFrom)*13+13))<13)
         {
            NotepadBm.PrintAt (Text2, FontSmallBlack, TEC_FONT_RIGHT, XY(216+x, 25+(c-CountFrom)*13), XY((*pMenuBms)[0].Size.x-2,25+(c-CountFrom)*13+13));

            if (NotepadBm.TryPrintAt (Table->Values[0+c*Table->AnzColums], *pFont1, TEC_FONT_LEFT, XY(216, 25+(c-CountFrom)*13), XY(216+x, 25+(c-CountFrom)*13+13))<13)
               NotepadBm.PrintAt (Table->Values[0+c*Table->AnzColums], *pFont1, TEC_FONT_LEFT, XY(216, 25+(c-CountFrom)*13), XY(216+x, 25+(c-CountFrom)*13+13));
            else
            {
               x-=10;
               CString str=Table->Values[0+c*Table->AnzColums]+"...";

               while (str.GetLength()>3)
               {
                  str.SetAt (str.GetLength()-3, '.');
                  str=str.Left(str.GetLength()-1);

                  if (NotepadBm.TryPrintAt (str, *pFont1, TEC_FONT_LEFT, XY(216, 25+(c-CountFrom)*13), XY(216+x, 25+(c-CountFrom)*13+13))<13)
                  {
                     NotepadBm.PrintAt (str, *pFont1, TEC_FONT_LEFT, XY(216, 25+(c-CountFrom)*13), XY(216+x, 25+(c-CountFrom)*13+13));
                     break;
                  }
               }
            }
            break;
         }
   }

   if (CountFrom>0) NotepadBm.BlitFrom ((*pMenuBms)[1], 196, 191);
   if (CountFrom+13<Table->AnzRows) NotepadBm.BlitFrom ((*pMenuBms)[2], 374, 199);
}
