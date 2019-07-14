//============================================================================================
// PlaneType.cpp : Routinen zum verwalten der PlaneType-Tabelle (Flugzeugtypen):
//============================================================================================
#include "Stdafx.h"

static const char FileId[] = "PlnT";

//Preise verstehen sich pro Sitzplatz:
extern SLONG SeatCosts[];
extern SLONG FoodCosts[];
extern SLONG TrayCosts[];
extern SLONG DecoCosts[];

//Preise pro Flugzeuge:
extern SLONG TriebwerkCosts[];
extern SLONG ReifenCosts[];
extern SLONG ElektronikCosts[];
extern SLONG SicherheitCosts[];

SLONG ReadLine (BUFFER<UBYTE> &Buffer, SLONG BufferStart, char *Line, SLONG LineLength);

//Daten des aktuellen Savegames beim laden:
extern SLONG SaveVersion;
extern SLONG SaveVersionSub;

//Speedup für die Flugzeuge:
SLONG SkipPlaneCalculation=0;

//============================================================================================
//CPlaneTypes::
//============================================================================================
//Konstruktor:
//============================================================================================
CPlaneTypes::CPlaneTypes (const CString &TabFilename) : ALBUM<CPlaneType> (PlaneTypes, "PlaneTypes")
{
   ReInit (TabFilename);
}

//--------------------------------------------------------------------------------------------
//Dateien neu laden:
//--------------------------------------------------------------------------------------------
void CPlaneTypes::ReInit (const CString &TabFilename)
{
   //CStdioFile    Tab;
   BUFFER<char>  Line(800);
   long          Id;

   //Load Table header:
   BUFFER<UBYTE> FileData (*LoadCompleteFile (FullFilename (TabFilename, ExcelPath)));
   SLONG         FileP=0;

   //Die erste Zeile einlesen
   FileP=ReadLine (FileData, FileP, Line, 800);

   PlaneTypes.ReSize (MAX_PLANETYPES);

   while (1)
   {
      if (FileP>=FileData.AnzEntries()) break;
      FileP=ReadLine (FileData, FileP, Line, 800);

      //if (!Tab.ReadString (Line, 800)) break;
      TeakStrRemoveEndingCodes (Line, "\xd\xa\x1a\r");

      //Tabellenzeile hinzufügen:
      Id=atol (strtok (Line, ";\x8\""))+0x10000000;

      //Hinzufügen (darf noch nicht existieren):
      if (IsInAlbum (Id)) TeakLibW_Exception (FNL, ExcNever);
      (*this)+=Id;

      //SpeedUp durch direkten Zugriff:
      Id=(*this)(Id);
      (*this)[Id].Hersteller        = strtok (NULL, TabSeparator);
      (*this)[Id].Name              = strtok (NULL, TabSeparator);
      (*this)[Id].NotizblockPhoto   = StringToInt64(strtok (NULL, TabSeparator));
      (*this)[Id].AnzPhotos         = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].FirstMissions     = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].FirstDay          = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].Erstbaujahr       = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].Spannweite        = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].Laenge            = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].Hoehe             = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].Startgewicht      = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].Passagiere        = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].Geschwindigkeit   = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].Reichweite        = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].Triebwerke        = strtok (NULL, TabSeparator);
      (*this)[Id].Schub             = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].AnzPiloten        = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].AnzBegleiter      = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].Tankgroesse       = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].Verbrauch         = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].Preis             = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].Wartungsfaktor    = float(atof (strtok (NULL, TabSeparator)));
      (*this)[Id].Kommentar         = strtok (NULL, TabSeparator);
   }
}

//--------------------------------------------------------------------------------------------
//Sucht einen zufälligen (bereits erfundenen) Flugzeugtyp raus:
//--------------------------------------------------------------------------------------------
ULONG CPlaneTypes::GetRandomExistingType (TEAKRAND *pRand)
{
   SLONG c, Anz, rnd;

   for (c=Anz=0; c<(SLONG)AnzEntries(); c++)
      if (IsInAlbum (c))
      if ((*this)[c].FirstMissions<Sim.Difficulty || (((*this)[c].FirstMissions==Sim.Difficulty || Sim.Difficulty==-1) && (*this)[c].FirstDay<=Sim.Date))
         Anz++;

   if (pRand) rnd = pRand->Rand(Anz) + 1;
         else rnd = rand() % Anz + 1;

   for (c=0; c<(SLONG)AnzEntries(); c++)
      if (IsInAlbum (c))
      if ((*this)[c].FirstMissions<Sim.Difficulty || (((*this)[c].FirstMissions==Sim.Difficulty || Sim.Difficulty==-1) && (*this)[c].FirstDay<=Sim.Date))
      {
         rnd--;
         if (!rnd) return (GetIdFromIndex(c));
      }

   TeakLibW_Exception (FNL, ExcNever);
   return (NULL);
}

//--------------------------------------------------------------------------------------------
//Blittet das Flugzeug (Footpos):
//--------------------------------------------------------------------------------------------
void CPlaneTypes::BlitPlaneAt (SBPRIMARYBM &TargetBm, SLONG PlaneType, SLONG Size, XY Pos, SLONG OwningPlayer)
{
   switch (Size)
   {
      //Auf Runway:
      case 1:
         TargetBm.BlitFromT (gUniversalPlaneBms[OwningPlayer], Pos.x-gUniversalPlaneBms[OwningPlayer].Size.x/2, Pos.y-gUniversalPlaneBms[OwningPlayer].Size.y);
         break;

      //Hinter Glas:
      case 2:
         {
            //Breite für's zentrieren berechnen:
            SLONG c, x=-(gUniversalPlaneBms[9+OwningPlayer*5+0].Size.x+gUniversalPlaneBms[9+OwningPlayer*5+2].Size.x+gUniversalPlaneBms[9+OwningPlayer*5+1].Size.x*((*this)[PlaneType].Passagiere/60))/2;

            //Bug blitten:
            TargetBm.BlitFromT (gUniversalPlaneBms[9+               0], Pos.x+x, Pos.y-gUniversalPlaneBms[9+0].Size.y);
            TargetBm.BlitFromT (gUniversalPlaneBms[9+OwningPlayer*5+0], Pos.x+x, Pos.y-gUniversalPlaneBms[9+0].Size.y);
            x+=gUniversalPlaneBms[9+OwningPlayer*5+0].Size.x;

            //Kabine blitten:
            for (c=0; c<(*this)[PlaneType].Passagiere/60; c++)
            {
               TargetBm.BlitFrom (gUniversalPlaneBms[9+               1], Pos.x+x, Pos.y-gUniversalPlaneBms[9+1].Size.y);
               TargetBm.BlitFrom (gUniversalPlaneBms[9+OwningPlayer*5+1], Pos.x+x, Pos.y-gUniversalPlaneBms[9+1].Size.y);

               x+=gUniversalPlaneBms[9+OwningPlayer*5+1].Size.x;
            }

            //Triebwerke:
            TargetBm.BlitFromT (gUniversalPlaneBms[8], Pos.x-gUniversalPlaneBms[4].Size.x/2+28, Pos.y-20-42-57);

            //Heck blitten:
            TargetBm.BlitFromT (gUniversalPlaneBms[9+               2], Pos.x+x, Pos.y-gUniversalPlaneBms[9+2].Size.y+19);
            TargetBm.BlitFromT (gUniversalPlaneBms[9+OwningPlayer*5+2], Pos.x+x, Pos.y-gUniversalPlaneBms[9+2].Size.y+19);

            //Triebwerke:
            TargetBm.BlitFromT (gUniversalPlaneBms[7], Pos.x-gUniversalPlaneBms[4].Size.x/2, Pos.y-20-8-19);
         }
         break;
   }
}

//============================================================================================
//CPlane::
//============================================================================================
//Konstruktor:
//============================================================================================
CPlane::CPlane ()
{
   Name="noname";
   WorstZustand=TargetZustand=Zustand=100;
   AnzPiloten=AnzBegleiter=0;
   Salden.ReSize (7);
   ClearSaldo();
   Wartungskosten=0;
   Sitze        = SitzeTarget      = 0;
   Essen        = EssenTarget      = 0;
   Tabletts     = TablettsTarget   = 0;
   Deco         = DecoTarget       = 0;
   Triebwerk    = TriebwerkTarget  = 0;
   Reifen       = ReifenTarget     = 0;
   Elektronik   = ElektronikTarget = 0;
   Sicherheit   = SicherheitTarget = 0;
   GlobeAngle   = 0;
   Auslastung   = Kilometer  = SummePassagiere = 0;
   AuslastungFC = 0;
   Sponsored    = FALSE;
   NumPannen    = 0;
   Problem      = 0;
   PseudoProblem = 0;
}

//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
CPlane::CPlane (CString Name, ULONG TypeId, UBYTE Zustand, SLONG Baujahr)
{
   CPlane::Name         = Name;
   CPlane::WorstZustand = Zustand;
   CPlane::Zustand      = Zustand;
   CPlane::TargetZustand= Zustand;
   CPlane::Baujahr      = Baujahr;
   CPlane::TypeId       = TypeId;
   CPlane::Ort          = Sim.HomeAirportId;
   CPlane::Position     = Cities[CPlane::Ort].GlobusPosition;
   CPlane::AnzPiloten   =
   CPlane::AnzBegleiter = 0;
   CPlane::Sitze        = CPlane::SitzeTarget = 0;
   CPlane::Essen        = CPlane::EssenTarget = 0;
   CPlane::Tabletts     = CPlane::TablettsTarget = 0;
   CPlane::Deco         = CPlane::DecoTarget = 0;
   CPlane::Triebwerk    = CPlane::TriebwerkTarget  = 0;
   CPlane::Reifen       = CPlane::ReifenTarget     = 0;
   CPlane::Elektronik   = CPlane::ElektronikTarget = 0;
   CPlane::Sicherheit   = CPlane::SicherheitTarget = 0;
   CPlane::GlobeAngle   = 0;
   CPlane::Sponsored    = FALSE;
   CPlane::NumPannen    = 0;
   CPlane::Problem      = 0;
   CPlane::PseudoProblem = 0;
   CPlane::OhneSitze    = FALSE;

   if (TypeId!=-1)
   {
      CPlane::MaxPassagiere   = PlaneTypes[TypeId].Passagiere*6/8;
      CPlane::MaxPassagiereFC = PlaneTypes[TypeId].Passagiere*1/8;
      CPlane::MaxBegleiter    = PlaneTypes[TypeId].AnzBegleiter;

      CPlaneType &qPlaneType = PlaneTypes[TypeId];
      ptHersteller      = qPlaneType.Hersteller;
      ptErstbaujahr     = qPlaneType.Erstbaujahr;
      ptName            = qPlaneType.Name;
      ptReichweite      = qPlaneType.Reichweite;
      ptGeschwindigkeit = qPlaneType.Geschwindigkeit;
      ptPassagiere      = qPlaneType.Passagiere;
      ptAnzPiloten      = qPlaneType.AnzPiloten;
      ptAnzBegleiter    = qPlaneType.AnzBegleiter;
      ptTankgroesse     = qPlaneType.Tankgroesse;
      ptVerbrauch       = qPlaneType.Verbrauch;
      ptPreis           = qPlaneType.Preis;
      ptLaerm           = 0;
      ptWartungsfaktor  = qPlaneType.Wartungsfaktor;
      ptKommentar       = qPlaneType.Kommentar;
   }

   CPlane::Flugplan.StartCity = CPlane::Ort;
   Salden.ReSize (7);

   ClearSaldo();
   Wartungskosten=0;

   Auslastung = AuslastungFC = Kilometer  = SummePassagiere = 0;
}

//--------------------------------------------------------------------------------------------
// Repairiert die Referenzen in dem CXPlane Objekt. Dort zeigt das Album auf den Buffer und
// wenn die CPlanes resizet wurden liegt der Buffer woanders und das Album fragt sich warum
// es davon nichts weiß.
//--------------------------------------------------------------------------------------------
void CPlane::RepairReferences (void)
{
   XPlane.Parts.Repair (XPlane.Parts.PlaneParts);
}

//--------------------------------------------------------------------------------------------
//Berechnet den Marktwert des Flugzeuges:
//--------------------------------------------------------------------------------------------
SLONG CPlane::CalculatePrice (void)
{
   //SLONG rc = SLONG(__int64(PlaneTypes[TypeId].Preis) * Zustand/10000 * Zustand * (Baujahr-1900) / 120);
   SLONG rc = SLONG(__int64(ptPreis) * Zustand/10000 * Zustand * (Baujahr-1900) / 120);

   if (Sponsored) rc/=10;

   return (rc);
}

//--------------------------------------------------------------------------------------------
//Bewegt das Flugzeug für Animationen etwas weiter:
//--------------------------------------------------------------------------------------------
void CPlane::DoOneStep (SLONG PlayerNum)
{
   switch (Ort)
   {
      //-1=Landend
      case -1:
         SkipPlaneCalculation=0;
         AirportPos.x+=6;
         if (AirportPos.y<22+18) AirportPos.y++;

         if (AirportPos.y==21+18)
         {
            //Durchsage:
            if (Cities[GetFlugplanEintrag()->VonCity].Name!="-" && !VoiceScheduler.IsVoicePlaying() &&
                !Sim.CallItADay && Sim.Time>=9*60000 && Sim.Time<=18*60000 && GetFlugplanEintrag()->ObjectType!=3 && GetFlugplanEintrag()->ObjectType!=4)
            {
               BOOL Ignore=FALSE;

               if (PlayerNum!=Sim.localPlayer)
               {
                  for (SLONG d=0; d<(SLONG)Sim.Players.Players[Sim.localPlayer].Planes.AnzEntries(); d++)
                     if (Sim.Players.Players[Sim.localPlayer].Planes.IsInAlbum (d))
                        if (Sim.Players.Players[Sim.localPlayer].Planes[d].Ort==-1)
                           Ignore=TRUE;
               }

               if (!Ignore && Cities[GetFlugplanEintrag()->VonCity].Wave!="-" && Cities[GetFlugplanEintrag()->VonCity].Wave!="")
               {
                  VoiceScheduler.AddVoice ((CString)bprintf ("Der%liFlug", PlayerNum));
                  VoiceScheduler.AddVoice (Cities[GetFlugplanEintrag()->VonCity].Wave);
               
                  VoiceScheduler.AddVoice ("gelande2");
               }
            }
         }

         //Fertig gelandet? Dann dich hinter die Fenster
         if (AirportPos.x>Airport.RightEnd/2+600)
         {
            if (GetFlugplanEintrag()->ObjectType!=3 && GetFlugplanEintrag()->ObjectType!=4 && GetFlugplanEintrag()->Gate!=-2)
            {
               if (GetFlugplanEintrag()->Gate==-1)
               {
                  //Kein Gate frei:
                  CRentRoute *pRoute  = NULL;
                  PLAYER     &qPlayer = Sim.Players.Players[PlayerNum];

                  if (GetFlugplanEintrag()->ObjectType==1)
                     pRoute = &qPlayer.RentRouten.RentRouten[(SLONG)Routen(GetFlugplanEintrag()->ObjectId)];

                  for (SLONG c=GetFlugplanEintrag()->Passagiere/CUSTOMERS_PER_PERSON; c>=0; c--)
                     qPlayer.Statistiken[STAT_UNZUFR_PASSAGIERE].AddAtPastDay (0, CUSTOMERS_PER_PERSON);

                  Ort = Sim.HomeAirportId;

                  if (Flugplan.NextFlight!=-1) qPlayer.Messages.AddMessage (BERATERTYP_GIRL, bprintf(StandardTexte.GetS (TOKEN_ADVICE, 2354), Name, Cities[Flugplan.Flug[Flugplan.NextFlight].NachCity].Name));
                  qPlayer.Image-=2;
                  Limit (-1000l, qPlayer.Image, 1000l);

                  if (pRoute && (qPlayer.Image)%10==0)
                  {
                     if (pRoute->Image>1) pRoute->Image-=2;
                                     else pRoute->Image=0;
                  }

                  CalculateHappyPassengers(PlayerNum);
               }
               else
               {
                  //Gate vorgesehen:
                  Ort        = -3;     //Hinterm Fenstern
                  AirportPos = XY(Airport.RightEnd+600, 190);

                  TargetX   = 200+Airport.GetRandomTypedRune (RUNE_2WAIT, UBYTE(GetFlugplanEintrag()->Gate)).x;
                  Startzeit = 255; //Kein Start
               }
            }
            else //Leerflug ==> nicht mehr anzeigen
               Ort = GetFlugplanEintrag()->NachCity;
         }
         break;

      //-2=Startend
      case -2:
         SkipPlaneCalculation=0;
         AirportPos.x+=7;
         if (AirportPos.x*2>(Airport.LeftEnd+Airport.RightEnd*3)/4 && (AirportPos.x&1)==0) AirportPos.y--;
         if (AirportPos.y<-30)
         {
            if (Flugplan.NextFlight!=-1) Flugplan.Flug[Flugplan.NextFlight].BookFlight (this, PlayerNum);
            Ort = -5;
         }
         break;

      //-3=CloseUp direkt hinterm Fenster
      case -3:
         SkipPlaneCalculation=0;
         {
            SLONG Speed;
            SLONG Temp;

            Speed = max(3, min (10, abs (AirportPos.x-TargetX)/30));

            //Lautstärke für Flugzeuge berechnen:
            Temp  = 150-abs(AirportPos.x-320-Sim.Players.Players[Sim.localPlayer].ViewPos.x)/4;
            if (Temp<0) Temp=0;

            if (Temp>200) Temp=200;

            SLONG RangeDrawn=0;
            for (SLONG c=1; c<Airport.ClipMarkers.AnzEntries(); c++)
            {
               if (Airport.ClipMarkers[c-1].Type==RUNE_CLIPFULL)
                  if (AirportPos.x>=RangeDrawn && AirportPos.x<=Airport.ClipMarkers[c].Position)
                     Temp = Temp * 170 / (170+min (abs (AirportPos.x-RangeDrawn), abs (AirportPos.x-Airport.ClipMarkers[c].Position)));

               RangeDrawn=Airport.ClipMarkers[c].Position;
            }

            AnzPlanesOnScreen += (Speed+3)*Temp/6;

            if (AirportPos.x>TargetX)
            {
               AirportPos.x-=Speed;
               AirportPos.y = 190;

               if (AirportPos.x<=TargetX) //Ist das Flugzeug angekommen?
               {
                  AirportPos.x=TargetX;
                  if (PlayerNum==Sim.localPlayer && Sim.Tutorial==1604 && Sim.IsTutorial)
                  {
                     Sim.Tutorial=1605;
                     Sim.Players.Players[Sim.localPlayer].Messages.NextMessage();
                     Sim.Players.Players[Sim.localPlayer].Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 1605));
                  }

                  if (Startzeit==255) //Passagiere absetzen?
                  {
                     SLONG c, Saldo;
                     XY    tmp;

                     CRentRoute *pRoute = NULL;
                     if (GetFlugplanEintrag()->ObjectType==1)
                        pRoute = &Sim.Players.Players[PlayerNum].RentRouten.RentRouten[(SLONG)Routen(GetFlugplanEintrag()->ObjectId)];

                     Saldo = PersonalQuality;

                     tmp = Airport.GetRandomTypedRune (RUNE_WAITPLANE, UBYTE(GetFlugplanEintrag()->Gate), true);

                     //hprintf ("Trying to add incomming passengers..");

                     if (tmp!=XY(-9999,-9999))
                     {
                        SLONG TooExpensive=0;
                        SLONG TooExpensiveFC=0;
                        if (GetFlugplanEintrag()->ObjectType==1)
                        {
                           SLONG Costs1 = pRoute->Ticketpreis;
                           SLONG Costs2 = CalculateFlightCost (GetFlugplanEintrag()->VonCity, GetFlugplanEintrag()->NachCity, 800, 800, -1)*3/180*2*3;

                           if (Costs1>Costs2) TooExpensive=Costs1*100/Costs2;
                           if (Costs1>Costs2*2) TooExpensive=Costs1*100/(Costs2*2);

                           if (TooExpensive>100) TooExpensive=100;
                           if (TooExpensiveFC>100) TooExpensiveFC=100;
                        }

                        CalculateHappyPassengers(PlayerNum);

                        SLONG QualitySum=Sitze+Essen+Tabletts+Deco+AnzBegleiter-ptAnzBegleiter;
                        //SLONG QualitySum=Sitze+Essen+Tabletts+Deco+AnzBegleiter-PlaneTypes[TypeId].AnzBegleiter;

                        if (PlayerNum==3 && Sim.Players.Players[Sim.localPlayer].Owner==1) { QualitySum-=3; Saldo/=2; }

                        TEAKRAND localRand;

                        //hprintf ("Trying (continued) to add incomming passengers..");

                        //Forcing the spot time to a certain (rounded) value. Since the
                        //planes are not 100% in sync, we need trick around a bit to
                        //ensure that the passengers are.
                        Sim.PersonQueue.SetSpotTime (tmp, (Sim.TimeSlice+50)/80*80);

                        localRand.SRand (GetFlugplanEintrag()->Gate + GetFlugplanEintrag()->Startzeit + Sim.GetHour() + Sim.Date);

                        if (GetFlugplanEintrag()->Passagiere>0)
                           for (c=GetFlugplanEintrag()->Passagiere/CUSTOMERS_PER_PERSON; c>=0; c--)
                           {
                              long pn = PlayerNum;
                              if (Sim.Players.Players[PlayerNum].WerbeBroschuere!=-1)
                                 pn=Sim.Players.Players[PlayerNum].WerbeBroschuere;

                              //hprintf ("Adding incomming passengers..");
                              if (localRand.Rand(70)>Zustand-30*(PlayerNum==3 && Sim.Players.Players[Sim.localPlayer].Owner==1))
                              {
                                 //Reparatur notwendig:
                                 Sim.PersonQueue.AddPerson (Clans.GetCustomerId(1, localRand.Rand(2)-1, &localRand), tmp, REASON_LEAVING, (UBYTE)pn, 0, 0, (UBYTE)MoodPersonRepair);
                              }
                              else if (QualitySum>4 && localRand.Rand(100)<40)
                              {
                                 //recht gut:
                                 Sim.PersonQueue.AddPerson (Clans.GetCustomerId(1, localRand.Rand(2)-1, &localRand), tmp, REASON_LEAVING, (UBYTE)pn, 0, 0, (UBYTE)MoodPerson2Stars);
                              }
                              else if (QualitySum>8 && localRand.Rand(100)<55)
                              {
                                 //sehr gut:
                                 Sim.PersonQueue.AddPerson (Clans.GetCustomerId(1, localRand.Rand(2)-1, &localRand), tmp, REASON_LEAVING, (UBYTE)pn, 0, 0, (UBYTE)MoodPerson3Stars);
                              }
                              else if (localRand.Rand(65)>Saldo)
                              {
                                 //schlechte Crew:
                                 Sim.PersonQueue.AddPerson (Clans.GetCustomerId(1, localRand.Rand(2)-1, &localRand), tmp, REASON_LEAVING, (UBYTE)pn, 0, 0, (UBYTE)MoodPersonSick);
                              }
                              else if (TooExpensive && localRand.Rand(130)<TooExpensive)
                              {
                                 //zu teuer:
                                 Sim.PersonQueue.AddPerson (Clans.GetCustomerId(1, localRand.Rand(2)-1, &localRand), tmp, REASON_LEAVING, (UBYTE)pn, 0, 0, (UBYTE)MoodPersonExpensive);
                              }
                              else //okay:
                              {
                                 Sim.PersonQueue.AddPerson (Clans.GetCustomerId(1, localRand.Rand(2)-1, &localRand), tmp, REASON_LEAVING, (UBYTE)pn, 0, 0, (UBYTE)MoodPersonSmile);
                              }

                        if (GetFlugplanEintrag()->PassagiereFC>0)
                           for (c=GetFlugplanEintrag()->PassagiereFC/CUSTOMERS_PER_PERSON; c>=0; c--)
                           {
                              long pn = PlayerNum;
                              if (Sim.Players.Players[PlayerNum].WerbeBroschuere!=-1)
                                 pn=Sim.Players.Players[PlayerNum].WerbeBroschuere;

                              //hprintf ("Adding incomming passengers..");
                              if (localRand.Rand(90)>Zustand-30*(PlayerNum==3 && Sim.Players.Players[Sim.localPlayer].Owner==1))
                              {
                                 //Reparatur notwendig:
                                 Sim.PersonQueue.AddPerson (Clans.GetCustomerId(1, localRand.Rand(2)-1, &localRand), tmp, REASON_LEAVING, (UBYTE)pn, 0, 0, (UBYTE)MoodPersonRepairFC, true);
                              }
                              else if (QualitySum>8 && localRand.Rand(100)<40)
                              {
                                 //recht gut:
                                 Sim.PersonQueue.AddPerson (Clans.GetCustomerId(1, localRand.Rand(2)-1, &localRand), tmp, REASON_LEAVING, (UBYTE)pn, 0, 0, (UBYTE)MoodPerson2StarsFC, true);
                              }
                              else if (QualitySum>10 && localRand.Rand(100)<55)
                              {
                                 //sehr gut:
                                 Sim.PersonQueue.AddPerson (Clans.GetCustomerId(1, localRand.Rand(2)-1, &localRand), tmp, REASON_LEAVING, (UBYTE)pn, 0, 0, (UBYTE)MoodPerson3StarsFC, true);
                              }
                              else if (localRand.Rand(100)>Saldo)
                              {
                                 //schlechte Crew:
                                 Sim.PersonQueue.AddPerson (Clans.GetCustomerId(1, localRand.Rand(2)-1, &localRand), tmp, REASON_LEAVING, (UBYTE)pn, 0, 0, (UBYTE)MoodPersonSickFC, true);
                              }
                              else if (TooExpensiveFC && localRand.Rand(130)<TooExpensiveFC)
                              {
                                 //zu teuer:
                                 Sim.PersonQueue.AddPerson (Clans.GetCustomerId(1, localRand.Rand(2)-1, &localRand), tmp, REASON_LEAVING, (UBYTE)pn, 0, 0, (UBYTE)MoodPersonExpensiveFC, true);
                              }
                              else //okay:
                              {
                                 Sim.PersonQueue.AddPerson (Clans.GetCustomerId(1, localRand.Rand(2)-1, &localRand), tmp, REASON_LEAVING, (UBYTE)pn, 0, 0, (UBYTE)MoodPersonSmileFC, true);
                              }
                           }
                        }
                     }
                     else Sim.bReloadAirport=true;
                  }
                  else  //nein, abholen
                  {
                     SLONG c, Gate=GetFlugplanEintrag()->Gate;

                     //Personen brauchen nicht mehr warten, sondern können das Flugzeug stürmen:
                     for (c=Sim.Persons.AnzEntries()-1; c>=0; c--)
                     {
                        if (Sim.Persons.IsInAlbum(c) && 
                            ((Sim.Persons[c].State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))==PERSON_WAITING || (Sim.Persons[c].State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))==PERSON_SITWAITING) &&
                            Sim.Persons[c].GetFlugplanEintrag()->Gate == Gate)
                        {
                           //Aufstehen, Sitze freigeben:
                           if ((Sim.Persons[c].State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))==PERSON_SITWAITING)
                           {
                              XY ArrayPos;

                              ArrayPos.x = (Sim.Persons[c].Position.x+4400)/44-100;
                              ArrayPos.y = (Sim.Persons[c].Position.y+2200)/22-100;

                              if (Sim.Persons[c].LookDir==7)
                                 Airport.SeatsTaken[ArrayPos.x]&=(~(1<<((ArrayPos.y-1)*2+1)));
                              else
                                 Airport.SeatsTaken[ArrayPos.x]&=(~(1<<((ArrayPos.y+1)*2)));
                           }

                           //Und an Bord gehen:
                           if (Sim.Persons[c].Dir==6) Sim.Persons[c].Dir=4;
                           if (Sim.Persons[c].Dir==7) Sim.Persons[c].Dir=1;
                           //!//Sim.Persons[c].State  = PERSON_BOARDING;
                           //!//Sim.Persons[c].Target = Airport.GetRandomTypedRune (RUNE_WAIT, (UBYTE)Gate);
                           Sim.Persons[c].State  = PERSON_2DURCHLEUCHTER;
                           Sim.Persons[c].Target = Airport.GetRandomTypedRune (RUNE_DURCHLEUCHTER, (UBYTE)Gate);
                        }
                     }

                  }
               }
            }
            else if (Startzeit!=255 && ((Startzeit==Sim.GetHour() && Sim.GetMinute()>=55) || Startzeit<Sim.GetHour() || (Startzeit==23 && Sim.GetHour()==0)))
            {  //Passagiere zum Start abgeholt haben:
               AirportPos.x-=Speed;
               AirportPos.y = 190;

               if (PlayerNum==Sim.localPlayer && Speed!=0 && Sim.Tutorial==1605 && Sim.IsTutorial)
               {
                  Sim.Tutorial=2000;
                  Sim.Players.Players[Sim.localPlayer].Messages.NextMessage();
                  Sim.Players.Players[Sim.localPlayer].Messages.IsMonolog = TRUE;
                  Sim.Players.Players[Sim.localPlayer].Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 1606));
                  Sim.Players.Players[Sim.localPlayer].Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 2000));
               }

               if (AirportPos.x<Airport.LeftEnd-180)
               {
                  //Und auf die Startbahn:
                  Ort      = -2;     //Startend
                  AirportPos = XY(-40, 26+18);
               }
            }
            else if (Startzeit==255 && (Sim.GetMinute()>=45 || abs(AirportPos.x-TargetX)>20))
            {  //Passagiere nach der Landung absetzt haben:
               AirportPos.x-=Speed;
               AirportPos.y = 190;

               if (AirportPos.x<Airport.LeftEnd-180)
               {
                  Ort = Sim.HomeAirportId;
               }
            }
         }
         break;

      //-5 in der Luft
      case -5:
         break;

      //Am Boden (in einer Stadt) ==> Müssen wir es ausrüsten?
      default:
         if (PlayerNum>=0 && PlayerNum<=3 && (Sim.Players.Players[PlayerNum].GetRoom()!=ROOM_PLANEPROPS || Sim.CallItADay))
         {
            SLONG Costs=0;
            SLONG Delta;

            PLAYER &qPlayer=Sim.Players.Players[PlayerNum];

            if (Sitze!=SitzeTarget)
            {
               //Delta=PlaneTypes[TypeId].Passagiere*(SeatCosts[SitzeTarget]-SeatCosts[Sitze]/2);
               Delta=ptPassagiere*(SeatCosts[SitzeTarget]-SeatCosts[Sitze]/2);
               Costs+=Delta;
               if (Delta<0) qPlayer.Statistiken[STAT_E_SONSTIGES].AddAtPastDay (0, -Delta);
               if (Delta>0) qPlayer.Statistiken[STAT_A_SONSTIGES].AddAtPastDay (0, -Delta);
               Sitze=SitzeTarget;
            }
            if (Essen!=EssenTarget)
            {
               Essen=EssenTarget;
            }
            if (Tabletts!=TablettsTarget)
            {
               //Delta=PlaneTypes[TypeId].Passagiere*(TrayCosts[TablettsTarget]-TrayCosts[Tabletts]/2);
               Delta=ptPassagiere*(TrayCosts[TablettsTarget]-TrayCosts[Tabletts]/2);
               Costs+=Delta;
               if (Delta<0) qPlayer.Statistiken[STAT_E_SONSTIGES].AddAtPastDay (0, -Delta);
               if (Delta>0) qPlayer.Statistiken[STAT_A_SONSTIGES].AddAtPastDay (0, -Delta);
               Tabletts=TablettsTarget;
            }
            if (Deco!=DecoTarget)
            {
               //Delta=PlaneTypes[TypeId].Passagiere*(DecoCosts[DecoTarget]-DecoCosts[Deco]/2);
               Delta=ptPassagiere*(DecoCosts[DecoTarget]-DecoCosts[Deco]/2);
               Costs+=Delta;
               if (Delta<0) qPlayer.Statistiken[STAT_E_SONSTIGES].AddAtPastDay (0, -Delta);
               if (Delta>0) qPlayer.Statistiken[STAT_A_SONSTIGES].AddAtPastDay (0, -Delta);
               Deco=DecoTarget;
            }

            if (Triebwerk!=TriebwerkTarget)
            {
               Delta=(TriebwerkCosts[TriebwerkTarget]-TriebwerkCosts[Triebwerk]/2);
               Costs+=Delta;
               if (Delta<0) qPlayer.Statistiken[STAT_E_SONSTIGES].AddAtPastDay (0, -Delta);
               if (Delta>0) qPlayer.Statistiken[STAT_A_SONSTIGES].AddAtPastDay (0, -Delta);
               Triebwerk=TriebwerkTarget;
            }
            if (Reifen!=ReifenTarget)
            {
               Delta=(ReifenCosts[ReifenTarget]-ReifenCosts[Reifen]/2);
               Costs+=Delta;
               if (Delta<0) qPlayer.Statistiken[STAT_E_SONSTIGES].AddAtPastDay (0, -Delta);
               if (Delta>0) qPlayer.Statistiken[STAT_A_SONSTIGES].AddAtPastDay (0, -Delta);
               Reifen=ReifenTarget;
            }
            if (Elektronik!=ElektronikTarget)
            {
               Delta=(ElektronikCosts[ElektronikTarget]-ElektronikCosts[Elektronik]/2);
               Costs+=Delta;
               if (Delta<0) qPlayer.Statistiken[STAT_E_SONSTIGES].AddAtPastDay (0, -Delta);
               if (Delta>0) qPlayer.Statistiken[STAT_A_SONSTIGES].AddAtPastDay (0, -Delta);
               Elektronik=ElektronikTarget;
            }
            if (Sicherheit!=SicherheitTarget)
            {
               Delta=(SicherheitCosts[SicherheitTarget]-SicherheitCosts[Sicherheit]/2);
               Costs+=Delta;
               if (Delta<0) qPlayer.Statistiken[STAT_E_SONSTIGES].AddAtPastDay (0, -Delta);
               if (Delta>0) qPlayer.Statistiken[STAT_A_SONSTIGES].AddAtPastDay (0, -Delta);
               Sicherheit=SicherheitTarget;
            }

            if (Costs && (qPlayer.Owner==0 || qPlayer.Owner==2)) qPlayer.ChangeMoney (-Costs, 2110, Name);
         }
         break;
   }
}

//--------------------------------------------------------------------------------------------
//Überprüft, die Flugpläne für ein Flugzeug und streicht ggf. Flüge
//--------------------------------------------------------------------------------------------
void CPlane::CheckFlugplaene (SLONG PlayerNum, BOOL Sort, BOOL PlanGates)
{
   SLONG c, d;

   //Zeitlich sortieren:
   for (c=Flugplan.Flug.AnzEntries()-1; c>0; c--)
      if ((Flugplan.Flug[c-1].ObjectType==0 && Flugplan.Flug[c].ObjectType!=0) ||
          (Sort && Flugplan.Flug[c-1].ObjectType!=0 && Flugplan.Flug[c].ObjectType!=0 && Flugplan.Flug[c-1].Startdate>Flugplan.Flug[c].Startdate) ||
          (Sort && Flugplan.Flug[c-1].ObjectType!=0 && Flugplan.Flug[c].ObjectType!=0 && Flugplan.Flug[c-1].Startdate==Flugplan.Flug[c].Startdate && Flugplan.Flug[c-1].Startzeit>Flugplan.Flug[c].Startzeit))
      {
         CFlugplanEintrag tmpFPE;

         tmpFPE=Flugplan.Flug[c-1];
         Flugplan.Flug[c-1]=Flugplan.Flug[c];
         Flugplan.Flug[c]=tmpFPE;

         #ifdef DEMO
            if (Flugplan.Flug[c-1].Startdate>109)
            {
               Flugplan.Flug[c-1].ObjectId++;
            }
         #endif

         c+=2;
         if (c>Flugplan.Flug.AnzEntries()) c=Flugplan.Flug.AnzEntries();
      }

   //Automatikflüge löschen, andere ergänzen:
   for (c=Flugplan.Flug.AnzEntries()-1; c>=0; c--)
      if (Flugplan.Flug[c].Startdate>Sim.Date || (Flugplan.Flug[c].Startdate==Sim.Date && Flugplan.Flug[c].Startzeit>Sim.GetHour()+1))
      {
         if (Flugplan.Flug[c].ObjectType==3)
         {
            Flugplan.Flug[c].ObjectType=0;
         }
         else if (Flugplan.Flug[c].ObjectType)
         {
            if (Flugplan.Flug[c].ObjectType==1) //Typ: Route
            {
               Flugplan.Flug[c].VonCity  = Routen[Flugplan.Flug[c].ObjectId].VonCity;
               Flugplan.Flug[c].NachCity = Routen[Flugplan.Flug[c].ObjectId].NachCity;
            }
            else if (Flugplan.Flug[c].ObjectType==2) //Typ: Auftrag
            {
               Flugplan.Flug[c].VonCity  = Sim.Players.Players[PlayerNum].Auftraege[Flugplan.Flug[c].ObjectId].VonCity;
               Flugplan.Flug[c].NachCity = Sim.Players.Players[PlayerNum].Auftraege[Flugplan.Flug[c].ObjectId].NachCity;
            }
            else if (Flugplan.Flug[c].ObjectType==4) //Typ: Fracht
            {
               Flugplan.Flug[c].VonCity  = Sim.Players.Players[PlayerNum].Frachten[Flugplan.Flug[c].ObjectId].VonCity;
               Flugplan.Flug[c].NachCity = Sim.Players.Players[PlayerNum].Frachten[Flugplan.Flug[c].ObjectId].NachCity;
            }

            //SLONG Dauer = Cities.CalcFlugdauer (Flugplan.Flug[c].VonCity, Flugplan.Flug[c].NachCity, PlaneTypes[TypeId].Geschwindigkeit);
            SLONG Dauer = Cities.CalcFlugdauer (Flugplan.Flug[c].VonCity, Flugplan.Flug[c].NachCity, ptGeschwindigkeit);
            Flugplan.Flug[c].Landezeit = (Flugplan.Flug[c].Startzeit+Dauer)%24;

            Flugplan.Flug[c].Landedate=Flugplan.Flug[c].Startdate;
            if (Flugplan.Flug[c].Landezeit<Flugplan.Flug[c].Startzeit)
               Flugplan.Flug[c].Landedate++;

            if (Dauer>=24)
            {
               Flugplan.Flug[c].ObjectType=0;
               Sim.Players.Players[PlayerNum].Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_ADVICE, 2310));
            }
         }
      }

   //Zeitlich sortieren:
   for (c=Flugplan.Flug.AnzEntries()-1; c>0; c--)
      if ((Flugplan.Flug[c-1].ObjectType==0 && Flugplan.Flug[c].ObjectType!=0) ||
          (Sort && Flugplan.Flug[c-1].ObjectType!=0 && Flugplan.Flug[c].ObjectType!=0 && Flugplan.Flug[c-1].Startdate>Flugplan.Flug[c].Startdate) ||
          (Sort && Flugplan.Flug[c-1].ObjectType!=0 && Flugplan.Flug[c].ObjectType!=0 && Flugplan.Flug[c-1].Startdate==Flugplan.Flug[c].Startdate && Flugplan.Flug[c-1].Startzeit>Flugplan.Flug[c].Startzeit))
      {
         CFlugplanEintrag tmpFPE;

         tmpFPE=Flugplan.Flug[c-1];
         Flugplan.Flug[c-1]=Flugplan.Flug[c];
         Flugplan.Flug[c]=tmpFPE;

         #ifdef DEMO
            if (Flugplan.Flug[c-1].Startdate>39)
            {
               Flugplan.Flug[c-1].NachCity++;
            }
         #endif

         c+=2;
         if (c>Flugplan.Flug.AnzEntries()) c=Flugplan.Flug.AnzEntries();
      }

   //Nötigenfalls am Anfang automatische Flüge einbauen:
   if (Flugplan.Flug[0].ObjectType!=0 && Cities(Flugplan.Flug[0].VonCity)!=(ULONG)Cities(Flugplan.StartCity))
   {
      //Automatik-Flug einfügen:
      for (d=Flugplan.Flug.AnzEntries()-1; d>0; d--)
         Flugplan.Flug[d]=Flugplan.Flug[d-1];

      Flugplan.Flug[0].ObjectType = 3;
      Flugplan.Flug[0].VonCity   = Flugplan.StartCity;
      Flugplan.Flug[0].NachCity  = Flugplan.Flug[1].VonCity;
      Flugplan.Flug[0].Startdate = Sim.Date;
      Flugplan.Flug[0].Startzeit = Sim.GetHour()+2;

      //SLONG Dauer = Cities.CalcFlugdauer (Flugplan.Flug[0].VonCity, Flugplan.Flug[0].NachCity, PlaneTypes[TypeId].Geschwindigkeit);
      SLONG Dauer = Cities.CalcFlugdauer (Flugplan.Flug[0].VonCity, Flugplan.Flug[0].NachCity, ptGeschwindigkeit);
      Flugplan.Flug[0].Landezeit = (Flugplan.Flug[0].Startzeit+Dauer)%24;
      Flugplan.Flug[0].Landedate = Flugplan.Flug[0].Startdate+(Flugplan.Flug[0].Startzeit+Dauer)/24;
   }

   //Nötigenfalls zwischendurch automatische Flüge einbauen:
   for (c=0; c<Flugplan.Flug.AnzEntries()-1; c++)
   {
      if (Flugplan.Flug[c+1].ObjectType==0) break;

      SLONG VonCity  = Flugplan.Flug[c+1].VonCity;
      SLONG NachCity = Flugplan.Flug[c].NachCity;

      if (VonCity>0x01000000)  VonCity = Cities(VonCity);
      if (NachCity>0x01000000) NachCity = Cities(NachCity);

      if (VonCity!=NachCity)
      {
         //Automatik-Flug einfügen:
         for (d=Flugplan.Flug.AnzEntries()-1; d>c+1; d--)
            Flugplan.Flug[d]=Flugplan.Flug[d-1];

         Flugplan.Flug[c+1].ObjectType = 3;
         Flugplan.Flug[c+1].VonCity   = Flugplan.Flug[c+0].NachCity;
         Flugplan.Flug[c+1].NachCity  = Flugplan.Flug[c+2].VonCity;
         Flugplan.Flug[c+1].Startzeit = Flugplan.Flug[c+0].Landezeit;
         Flugplan.Flug[c+1].Startdate = Flugplan.Flug[c+0].Landedate;

         //Prüfen, ob Startzeit- und Datum auch in der Zukunft liegen:
         if (Flugplan.Flug[c+1].Startdate<Sim.Date) Flugplan.Flug[c+1].Startdate=Sim.Date;
         if (Flugplan.Flug[c+1].Startdate==Sim.Date && Flugplan.Flug[c+1].Startzeit<=Sim.GetHour()+1) Flugplan.Flug[c+1].Startzeit=Sim.GetHour()+2;

         //SLONG Dauer = Cities.CalcFlugdauer (Flugplan.Flug[c+1].VonCity, Flugplan.Flug[c+1].NachCity, PlaneTypes[TypeId].Geschwindigkeit);
         SLONG Dauer = Cities.CalcFlugdauer (Flugplan.Flug[c+1].VonCity, Flugplan.Flug[c+1].NachCity, ptGeschwindigkeit);
         Flugplan.Flug[c+1].Landezeit = (Flugplan.Flug[c+1].Startzeit+Dauer)%24;
         Flugplan.Flug[c+1].Landedate = Flugplan.Flug[c+1].Startdate+(Flugplan.Flug[c+1].Startzeit+Dauer)/24;
      }
   }

   //Ggf. verschieben, damit die Zeiten passen:
   for (c=0; c<Flugplan.Flug.AnzEntries()-1; c++)
   {
      if (Flugplan.Flug[c+1].ObjectType==0) break;

      SLONG tTime=(Flugplan.Flug[c].Landezeit+1)%24;
      SLONG tDate=Flugplan.Flug[c].Landedate+(Flugplan.Flug[c].Landezeit+1)/24;

      if (Flugplan.Flug[c+1].Startdate<tDate)
      {
         Flugplan.Flug[c+1].Startdate=tDate;
         Flugplan.Flug[c+1].Startzeit=tTime;

         //SLONG Dauer = Cities.CalcFlugdauer (Flugplan.Flug[c+1].VonCity, Flugplan.Flug[c+1].NachCity, PlaneTypes[TypeId].Geschwindigkeit);
         SLONG Dauer = Cities.CalcFlugdauer (Flugplan.Flug[c+1].VonCity, Flugplan.Flug[c+1].NachCity, ptGeschwindigkeit);
         Flugplan.Flug[c+1].Landezeit = (Flugplan.Flug[c+1].Startzeit+Dauer)%24;
         Flugplan.Flug[c+1].Landedate = Flugplan.Flug[c+1].Startdate+(Flugplan.Flug[c+1].Startzeit+Dauer)/24;
      }
      else if (Flugplan.Flug[c+1].Startdate==tDate && Flugplan.Flug[c+1].Startzeit<tTime)
      {
         Flugplan.Flug[c+1].Startzeit = tTime;

         //SLONG Dauer = Cities.CalcFlugdauer (Flugplan.Flug[c+1].VonCity, Flugplan.Flug[c+1].NachCity, PlaneTypes[TypeId].Geschwindigkeit);
         SLONG Dauer = Cities.CalcFlugdauer (Flugplan.Flug[c+1].VonCity, Flugplan.Flug[c+1].NachCity, ptGeschwindigkeit);
         Flugplan.Flug[c+1].Landezeit = (Flugplan.Flug[c+1].Startzeit+Dauer)%24;
         Flugplan.Flug[c+1].Landedate = Flugplan.Flug[c+1].Startdate+(Flugplan.Flug[c+1].Startzeit+Dauer)/24;
      }
   }

   //Überschüssige Flüge abschneiden:
   if (gMouseLButton==0 || Sim.Players.Players[PlayerNum].Owner!=0 || Sim.Players.Players[PlayerNum].LocationWin==NULL || (Sim.Players.Players[PlayerNum].GetRoom()!=ROOM_GLOBE && Sim.Players.Players[PlayerNum].GetRoom()!=ROOM_LAPTOP) && ((CPlaner*)Sim.Players.Players[PlayerNum].LocationWin)->DragFlightMode==0)
      for (c=0; c<Flugplan.Flug.AnzEntries(); c++)
      {
         if (Flugplan.Flug[c].ObjectType==0) break;

         if (Flugplan.Flug[c].Startdate>=Sim.Date+7)
         {
            Flugplan.Flug[c].ObjectType=0;
            if (c>0 && Flugplan.Flug[c-1].ObjectType==3)
               Flugplan.Flug[c-1].ObjectType=0;
         }
      }

   //Automatik-Flüge möglichst spät ansetzen:
   for (c=0; c<Flugplan.Flug.AnzEntries()-1; c++)
   {
      if (Flugplan.Flug[c+1].ObjectType==0) break;

      if (Flugplan.Flug[c].ObjectType==3 && (Flugplan.Flug[c].Startdate>Sim.Date || (Flugplan.Flug[c].Startdate==Sim.Date && Flugplan.Flug[c].Startzeit>Sim.GetHour()+1)))
      {
         SLONG tTime=(Flugplan.Flug[c+1].Startzeit-1+24)%24;
         SLONG tDate=Flugplan.Flug[c+1].Startdate-(Flugplan.Flug[c+1].Startzeit==0);

         if (Flugplan.Flug[c].Landedate<tDate)
         {
            Flugplan.Flug[c].Landedate=tDate;
            Flugplan.Flug[c].Landezeit=tTime;

            //SLONG Dauer = Cities.CalcFlugdauer (Flugplan.Flug[c].VonCity, Flugplan.Flug[c].NachCity, PlaneTypes[TypeId].Geschwindigkeit);
            SLONG Dauer = Cities.CalcFlugdauer (Flugplan.Flug[c].VonCity, Flugplan.Flug[c].NachCity, ptGeschwindigkeit);
            Flugplan.Flug[c].Startzeit = (Flugplan.Flug[c].Landezeit-Dauer+480)%24;
            Flugplan.Flug[c].Startdate = Flugplan.Flug[c].Landedate+(Flugplan.Flug[c].Landezeit-Dauer-23)/24;
         }
         else if (Flugplan.Flug[c].Landedate==tDate && Flugplan.Flug[c].Landezeit<tTime)
         {
            Flugplan.Flug[c].Landezeit=tTime;

            //SLONG Dauer = Cities.CalcFlugdauer (Flugplan.Flug[c].VonCity, Flugplan.Flug[c].NachCity, PlaneTypes[TypeId].Geschwindigkeit);
            SLONG Dauer = Cities.CalcFlugdauer (Flugplan.Flug[c].VonCity, Flugplan.Flug[c].NachCity, ptGeschwindigkeit);
            Flugplan.Flug[c].Startzeit = (Flugplan.Flug[c].Landezeit-Dauer+480)%24;
            Flugplan.Flug[c].Startdate = Flugplan.Flug[c].Landedate+(Flugplan.Flug[c].Landezeit-Dauer-23)/24;
         }
      }
   }

   if (PlanGates) Sim.Players.Players[PlayerNum].PlanGates();
}

//--------------------------------------------------------------------------------------------
//Setzt den Flugplan auf Knopfdruck für die restliche Woche fort:
//--------------------------------------------------------------------------------------------
void CPlane::ExtendFlugplaene (SLONG PlayerNum)
{
   SLONG c;
   SLONG RapportL;    //Länge des Rapports in Tagen
   SLONG AnzOkay, AnzSource, SourceStart;

   for (c=0; c<Flugplan.Flug.AnzEntries(); c++)
      if (Flugplan.Flug[c].ObjectType==0) break;

   AnzOkay = AnzSource = c;

   for (c=0; c<Flugplan.Flug.AnzEntries(); c++)
      if (Flugplan.Flug[c].ObjectType==1 && Flugplan.Flug[c].Startdate>=Sim.Date) break;

   if (c<Flugplan.Flug.AnzEntries())
   {
      SourceStart=c;
      AnzSource-=c;

      RapportL =  (Flugplan.Flug[SourceStart+AnzSource-1].Landedate*24+Flugplan.Flug[SourceStart+AnzSource-1].Landezeit)
                 -(Flugplan.Flug[SourceStart].Startdate*24+Flugplan.Flug[SourceStart].Startzeit)+1;
      if (Flugplan.Flug[SourceStart+AnzSource-1].NachCity!=Flugplan.Flug[SourceStart].VonCity)
         RapportL += Cities.CalcFlugdauer (Flugplan.Flug[SourceStart+AnzSource-1].VonCity, Flugplan.Flug[SourceStart].NachCity, ptGeschwindigkeit)+1;
         //RapportL += Cities.CalcFlugdauer (Flugplan.Flug[SourceStart+AnzSource-1].VonCity, Flugplan.Flug[SourceStart].NachCity, PlaneTypes[TypeId].Geschwindigkeit)+1;

      if (AnzOkay>0)
      {
         //Copy again and again, until it's too large:
         while (Flugplan.Flug[AnzOkay-1].Landedate*24+Flugplan.Flug[AnzOkay-1].Landezeit+RapportL<(Sim.Date+7)*24)
         {
            for (c=0; c<AnzSource; c++)
            {
               if (Flugplan.Flug[AnzOkay+c-AnzSource].ObjectType==1 && Sim.Players.Players[PlayerNum].RentRouten.RentRouten[(SLONG)Routen(Flugplan.Flug[AnzOkay+c-AnzSource].ObjectId)].Rang)
               {
                  Flugplan.Flug[AnzOkay+c]=Flugplan.Flug[AnzOkay+c-AnzSource];
                  Flugplan.Flug[AnzOkay+c].Startzeit+=RapportL;
                  Flugplan.Flug[AnzOkay+c].Landezeit+=RapportL;
                  while (Flugplan.Flug[AnzOkay+c].Landezeit>24)
                  {
                     Flugplan.Flug[AnzOkay+c].Landezeit-=24;
                     Flugplan.Flug[AnzOkay+c].Landedate++;
                  }
                  while (Flugplan.Flug[AnzOkay+c].Startzeit>24)
                  {
                     Flugplan.Flug[AnzOkay+c].Startzeit-=24;
                     Flugplan.Flug[AnzOkay+c].Startdate++;
                  }
               }
            }
            AnzOkay+=AnzSource;
         }
      }
   }
}

//--------------------------------------------------------------------------------------------
//Die Flugplaene auf den neuen Tag vorbereiten:
//--------------------------------------------------------------------------------------------
void CPlane::FlugplaeneFortfuehren (SLONG PlayerNum)
{
   SLONG c, d;
   SLONG LastDate=0, LastTime=0;

   //Den letzten Flug als Maßstab heraussuchen:
   for (c=0; c<Flugplan.Flug.AnzEntries(); c++)
   {
      if (Flugplan.Flug[c].ObjectType)
         if (Flugplan.Flug[c].Landedate>LastDate || (Flugplan.Flug[c].Landedate==LastDate && Flugplan.Flug[c].Landezeit>LastTime))
         {
            LastDate=Flugplan.Flug[c].Landedate;
            LastTime=Flugplan.Flug[c].Landezeit+1;

            if (LastTime>=24) { LastTime=0; LastDate++; }
         }
   }

   for (c=0; c<Flugplan.Flug.AnzEntries(); c++)
   {
      if (Flugplan.Flug[c].Landedate<Sim.Date)
      {
         //Alte Aufträge oder Automatikflüge löschen:
         if (Flugplan.Flug[c].ObjectType==2 || Flugplan.Flug[c].ObjectType==3 || Flugplan.Flug[c].ObjectType==4)
         {
            Flugplan.StartCity=Flugplan.Flug[c].NachCity;
            Flugplan.Flug[c].ObjectType=0;
         }

         //Routen in die Zukunft verschieben:
         if (Flugplan.Flug[c].ObjectType==1 && Sim.Players.Players[PlayerNum].RentRouten.RentRouten[(SLONG)Routen(Flugplan.Flug[c].ObjectId)].Rang)
         {
            Flugplan.StartCity=Flugplan.Flug[c].NachCity;
            Flugplan.Flug[c].Startdate+=7;
            Flugplan.Flug[c].Landedate+=7;
            Flugplan.Flug[c].PArrived  =0;
            Flugplan.Flug[c].HoursBefore=48;
            Flugplan.Flug[c].Ticketpreis=Sim.Players.Players[PlayerNum].RentRouten.RentRouten[(SLONG)Routen(Flugplan.Flug[c].ObjectId)].Ticketpreis; //new

            //Zeiten ggf. anpassen (Feintuning)
            if (Flugplan.Flug[c].Startdate<LastDate || (Flugplan.Flug[c].Startdate==LastDate && Flugplan.Flug[c].Startzeit<LastTime))
            {
               SLONG Delta = (LastDate*24+LastTime) - (Flugplan.Flug[c].Startdate*24+Flugplan.Flug[c].Startzeit);

               Flugplan.Flug[c].Landezeit += Delta;
               Flugplan.Flug[c].Startzeit += Delta;

               while (Flugplan.Flug[c].Startzeit>=24) { Flugplan.Flug[c].Startzeit-=24; Flugplan.Flug[c].Startdate++; }
               while (Flugplan.Flug[c].Landezeit>=24) { Flugplan.Flug[c].Landezeit-=24; Flugplan.Flug[c].Landedate++; }

               LastDate=Flugplan.Flug[c].Landedate;
               LastTime=Flugplan.Flug[c].Landezeit+1;

               while (LastTime>=24) { LastTime-=24; LastDate++; }
            }
         }

         //Ist der Flug trotz Verschiebung noch von gestern? Dann löschen:
         if (Flugplan.Flug[c].ObjectType==1 && Flugplan.Flug[c].Landedate<Sim.Date)
         {
            Flugplan.Flug[c].ObjectType=0;
         }
      }
   }

   CheckFlugplaene (PlayerNum);

   //Autoflüge ggf. durch Routen ersetzen:
   for (c=0; c<Flugplan.Flug.AnzEntries(); c++)
      if (Flugplan.Flug[c].ObjectType==3)
         for (d=0; d<Flugplan.Flug.AnzEntries(); d++)
            if (Flugplan.Flug[d].ObjectType==1)
               if (Flugplan.Flug[c].VonCity==Flugplan.Flug[d].VonCity && Flugplan.Flug[c].NachCity==Flugplan.Flug[d].NachCity)
               {
                  Flugplan.Flug[c].ObjectType  = Flugplan.Flug[d].ObjectType;
                  Flugplan.Flug[c].ObjectId    = Flugplan.Flug[d].ObjectId;
                  Flugplan.Flug[c].Ticketpreis = Flugplan.Flug[d].Ticketpreis;
               }
}

//--------------------------------------------------------------------------------------------
//Kann das Flugzeug zur Zeit verkauft werden, oder sind Flüge darauf gebucht?
//--------------------------------------------------------------------------------------------
BOOL CPlane::CanBeSold (void)
{
   SLONG c;

   for (c=Flugplan.Flug.AnzEntries()-1; c>=0; c--)
   {   
      if (Flugplan.Flug[c].ObjectType==1 || Flugplan.Flug[c].ObjectType==2 || Flugplan.Flug[c].ObjectType==4)
      if (Flugplan.Flug[c].Landedate>Sim.Date || (Flugplan.Flug[c].Landedate==Sim.Date && Flugplan.Flug[c].Landezeit>Sim.GetHour()-2))
      {
         return (FALSE);
      }
   }

   return (TRUE);
}

//--------------------------------------------------------------------------------------------
//Aktualisiert die zweidimensionale Position auf dem Globus:
//--------------------------------------------------------------------------------------------
void CPlane::UpdateGlobePos (UWORD EarthAlpha)
{
   BehindGlobe=EarthProjectize (Position, EarthAlpha, &GlobePos);
   if (strlen(Name)>0) GlobePos.x+=(Name[0]&3)-1;
   if (strlen(Name)>1) GlobePos.y+=(Name[1]&3)-1;

   if (Ort==-5 && Flugplan.NextFlight!=-1)
   {
      XY VonPos, NachPos, Delta;

      CFlugplanEintrag &qFPE = Flugplan.Flug[Flugplan.NextFlight];

      VonPos  = Cities[qFPE.VonCity].GlobusPosition;
      NachPos = Cities[qFPE.NachCity].GlobusPosition;
      Delta   = NachPos-VonPos;

      if (Delta.x>180)  Delta.x-=360;
      if (Delta.x<-180) Delta.x+=360;

      //In der Luft:
      GlobeAngle = UBYTE((GetAlphaFromXY (Delta)+22.5)*8/360);

      if (GlobeAngle<0) GlobeAngle=0;
      if (GlobeAngle>7) GlobeAngle=7;

      GlobeAngle++;
   }
   else
   {
      //Am Boden:
      GlobeAngle = 0;
   }
}

//--------------------------------------------------------------------------------------------
//Berechnet den Saldo einer Woche:
//--------------------------------------------------------------------------------------------
SLONG CPlane::GetSaldo (void)
{
   SLONG c, Summe;

   for (c=Summe=0; c<7; c++)
      Summe+=Salden[c];

   return (Summe);
}

//--------------------------------------------------------------------------------------------
// Gibt die größte Passagieranzahl eines noch offenen Fluges zurück:
//--------------------------------------------------------------------------------------------
SLONG CPlane::GetMaxPassengerOpenFlight (SLONG PlayerNum)
{
   SLONG rc=0;

   for (SLONG c=0; c<Flugplan.Flug.AnzEntries(); c++)
      if (Flugplan.Flug[c].Landedate>=Sim.Date)
      {
         CFlugplanEintrag &qFPE=Flugplan.Flug[c];

         if (qFPE.ObjectType==2) //Typ: Auftrag
         {
            if (Sim.Players.Players[PlayerNum].Auftraege[qFPE.ObjectId].Personen>ULONG(rc))
               rc=Sim.Players.Players[PlayerNum].Auftraege[qFPE.ObjectId].Personen;
         }
         else if (qFPE.ObjectType==4) //Typ: Fracht
         {
            if (Sim.Players.Players[PlayerNum].Frachten[qFPE.ObjectId].Tons*10>rc)
               rc=Sim.Players.Players[PlayerNum].Frachten[qFPE.ObjectId].Tons*10;
         }
      }

   return (rc);
}

//--------------------------------------------------------------------------------------------
//Löscht den Saldo:
//--------------------------------------------------------------------------------------------
void CPlane::ClearSaldo (void)
{
   Salden.FillWith (0);
}

//--------------------------------------------------------------------------------------------
//Fügt einem Flugzeug eine Panne hinzu:
//--------------------------------------------------------------------------------------------
void CPlane::AddPanne (SLONG Code)
{
   SLONG c;

   if (Pannen.AnzEntries()<10)
      Pannen.ReSize (Pannen.AnzEntries()+1);

   for (c=Pannen.AnzEntries()-1; c>0; c--)
      Pannen[c] = Pannen[c-1];

   Pannen[0].Date = Sim.Date;
   Pannen[0].Time = Sim.Time;
   Pannen[0].Code = Code;

   NumPannen++;
}

//--------------------------------------------------------------------------------------------
//Gibt eine Referenz auf den aktuellen Flugplan zurück:
//--------------------------------------------------------------------------------------------
const CFlugplanEintrag *CPlane::GetFlugplanEintrag (void)
{
   if (Flugplan.NextFlight==-1) return (NULL);

   return (&Flugplan.Flug[Flugplan.NextFlight]);
}

//--------------------------------------------------------------------------------------------
//Berechnet, wie vielen Passagieren der Flug gefallen hat:
//--------------------------------------------------------------------------------------------
void CPlane::CalculateHappyPassengers (SLONG PlayerNum)
{
   //SLONG QualitySum=Sitze+Essen+Tabletts+Deco+AnzBegleiter-PlaneTypes[TypeId].AnzBegleiter;
   SLONG QualitySum=Sitze+Essen+Tabletts+Deco+AnzBegleiter-ptAnzBegleiter;
   SLONG c, Saldo;
   SLONG TooExpensive=0;

   //hprintf ("QualitySum=%li", QualitySum);

   //TEAKRAND LocalRand (PlayerNum+Sim.Date+Sim.GetHour()+TypeId+QualitySum+GetFlugplanEintrag()->VonCity);
   TEAKRAND LocalRand (PlayerNum+Sim.Date+Sim.GetHour()+QualitySum+GetFlugplanEintrag()->VonCity);

   //hprintf ("LocalRand=%li", LocalRand.Rand()%1000);

   if (GetFlugplanEintrag()->ObjectType==1)
   {
      CRentRoute *pRoute = &Sim.Players.Players[PlayerNum].RentRouten.RentRouten[(SLONG)Routen(GetFlugplanEintrag()->ObjectId)];

      SLONG Costs1 = pRoute->Ticketpreis;
      SLONG Costs2 = CalculateFlightCost (GetFlugplanEintrag()->VonCity, GetFlugplanEintrag()->NachCity, 800, 800, -1)*3/180*2*3;

      if (Costs1>Costs2) TooExpensive=Costs1*100/Costs2;

      if (TooExpensive>100) TooExpensive=100;
   }

   /*Saldo=0;
   SLONG n;
   for (c=n=0; c<Workers.Workers.AnzEntries(); c++)
      if (Workers.Workers[c].Employer==PlayerNum && Workers.Workers[c].PlaneId!=-1 && (&Sim.Players.Players[PlayerNum].Planes[Workers.Workers[c].PlaneId])==this)
      {
         Saldo+=Workers.Workers[c].Talent;
         n++;
      }
   hprintf ("Saldo (n)=%li (%li)", Saldo, n);
   if (n) Saldo/=n; else Saldo=60;*/

   Saldo = PersonalQuality;

   long passtotal = GetFlugplanEintrag()->Passagiere+GetFlugplanEintrag()->PassagiereFC;
   if (GetFlugplanEintrag()->ObjectType==1 || GetFlugplanEintrag()->ObjectType==2)
      for (c=passtotal/CUSTOMERS_PER_PERSON; c>=0; c--)
      {
         SLONG Anz=CUSTOMERS_PER_PERSON;

         if (c==0) Anz=passtotal-passtotal/CUSTOMERS_PER_PERSON*CUSTOMERS_PER_PERSON;

         if (PlayerNum==3 && Sim.Players.Players[Sim.localPlayer].Owner==1) { QualitySum-=3; Saldo/=2; }

         //hprintf ("LocalRand=%li", LocalRand.Rand(1000));

         if (LocalRand.Rand(70)>Zustand)
         {
            //Reparatur notwendig:
            //hprintf ("Zustand=%li", Zustand);
            Sim.Players.Players[PlayerNum].Statistiken[STAT_UNZUFR_PASSAGIERE].AddAtPastDay (0, Anz);
         }
         else if (QualitySum>4 && LocalRand.Rand(100)<40)
         {
            //recht gut:
            //hprintf ("QualitySum=%li", QualitySum);
            Sim.Players.Players[PlayerNum].Statistiken[STAT_ZUFR_PASSAGIERE].AddAtPastDay (0, Anz);
         }
         else if (QualitySum>8 && LocalRand.Rand(100)<55)
         {
            //sehr gut:
            //hprintf ("QualitySum=%li", QualitySum);
            Sim.Players.Players[PlayerNum].Statistiken[STAT_ZUFR_PASSAGIERE].AddAtPastDay (0, Anz);
         }
         else if (LocalRand.Rand(65)>Saldo)
         {
            //schlechte Crew:
            //hprintf ("Saldo=%li", Saldo);
            Sim.Players.Players[PlayerNum].Statistiken[STAT_UNZUFR_PASSAGIERE].AddAtPastDay (0, Anz);
         }
         else if (TooExpensive && LocalRand.Rand(130)<TooExpensive)
         {
            //zu teuer:
            //hprintf ("TooExpensive=%li", TooExpensive);
            Sim.Players.Players[PlayerNum].Statistiken[STAT_UNZUFR_PASSAGIERE].AddAtPastDay (0, Anz);
         }
         else //okay:
         {
            //hprintf ("Okay");
            Sim.Players.Players[PlayerNum].Statistiken[STAT_ZUFR_PASSAGIERE].AddAtPastDay (0, Anz);
         }
      }
}

//--------------------------------------------------------------------------------------------
// Updates the PersonalQuality member variable of this object:
//--------------------------------------------------------------------------------------------
void CPlane::UpdatePersonalQuality (SLONG PlayerNum)
{
   SLONG Saldo, c, n;

   Saldo=0;

   for (c=n=0; c<Workers.Workers.AnzEntries(); c++)
      if (Workers.Workers[c].Employer==PlayerNum && Workers.Workers[c].PlaneId!=-1 && (&Sim.Players.Players[PlayerNum].Planes[Workers.Workers[c].PlaneId])==this)
      {
         Saldo+=Workers.Workers[c].Talent;
         n++;
      }

   if (n) Saldo/=n; else Saldo=60;

   PersonalQuality = Saldo;
}

//--------------------------------------------------------------------------------------------
//Speichert ein CPanne Datum:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CPanne &Panne)
{
   File << Panne.Date << Panne.Time << Panne.Code;
   return File;
}

//--------------------------------------------------------------------------------------------
//Lädt ein CPanne Datum:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CPanne &Panne)
{
   File >> Panne.Date >> Panne.Time >> Panne.Code;
   return File;
}

//--------------------------------------------------------------------------------------------
//Speichert ein CPlane Datum:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CPlane &Plane)
{
   File << Plane.Name     << Plane.Ort         << Plane.Position;
   File << Plane.Salden;
   File << Plane.GlobePos << Plane.BehindGlobe << Plane.AirportPos   << Plane.GlobeAngle;
   File << Plane.TargetX  << Plane.Startzeit   << Plane.TypeId;
   File << Plane.Flugplan << Plane.Zustand     << Plane.TargetZustand;
   File << Plane.WorstZustand;
   File << Plane.Baujahr  << Plane.AnzPiloten  << Plane.AnzBegleiter;
   File << Plane.MaxBegleiter;

   File << Plane.Sitze    << Plane.SitzeTarget;
   File << Plane.Essen    << Plane.EssenTarget;
   File << Plane.Tabletts << Plane.TablettsTarget;
   File << Plane.Deco     << Plane.DecoTarget;

   File << Plane.Auslastung    << Plane.Kilometer << Plane.SummePassagiere << Plane.AuslastungFC;
   File << Plane.NumPannen     << Plane.Pannen;
   File << Plane.MaxPassagiere << Plane.MaxPassagiereFC;

   if (SaveVersion==1 && SaveVersionSub>=3)
   {
      File << Plane.Sponsored;
   }
   if (SaveVersion==1 && SaveVersionSub>=4)
   {
      File << Plane.Problem;
      File << Plane.PseudoProblem;
   }
   if (SaveVersion==1 && SaveVersionSub>=13)
   {
      File << Plane.Wartungskosten;
   }
   if (SaveVersion==1 && SaveVersionSub>=100)
   {
      File << Plane.Triebwerk  << Plane.TriebwerkTarget;
      File << Plane.Reifen     << Plane.ReifenTarget;
      File << Plane.Elektronik << Plane.ElektronikTarget;
      File << Plane.Sicherheit << Plane.SicherheitTarget;
      File << Plane.OhneSitze;
      File << Plane.PersonalQuality;
   }

   File << Plane.ptHersteller      << Plane.ptErstbaujahr    << Plane.ptName           << Plane.ptReichweite <<
           Plane.ptGeschwindigkeit << Plane.ptPassagiere     << Plane.ptAnzPiloten <<
           Plane.ptAnzBegleiter    << Plane.ptTankgroesse    << Plane.ptVerbrauch  <<
           Plane.ptPreis           << Plane.ptWartungsfaktor << Plane.ptKommentar;

   File << Plane.ptLaerm;

   File << Plane.XPlane;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Läd ein CPlane Datum:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CPlane &Plane)
{
   File >> Plane.Name     >> Plane.Ort         >> Plane.Position;
   File >> Plane.Salden;
   File >> Plane.GlobePos >> Plane.BehindGlobe >> Plane.AirportPos  >> Plane.GlobeAngle;
   File >> Plane.TargetX  >> Plane.Startzeit   >> Plane.TypeId;
   File >> Plane.Flugplan >> Plane.Zustand     >> Plane.TargetZustand;
   File >> Plane.WorstZustand;
   File >> Plane.Baujahr  >> Plane.AnzPiloten  >> Plane.AnzBegleiter;
   File >> Plane.MaxBegleiter;

   File >> Plane.Sitze    >> Plane.SitzeTarget;
   File >> Plane.Essen    >> Plane.EssenTarget;
   File >> Plane.Tabletts >> Plane.TablettsTarget;
   File >> Plane.Deco     >> Plane.DecoTarget;

   File >> Plane.Auslastung >> Plane.Kilometer >> Plane.SummePassagiere >> Plane.AuslastungFC;
   File >> Plane.NumPannen  >> Plane.Pannen;
   File >> Plane.MaxPassagiere >> Plane.MaxPassagiereFC;

   if (SaveVersion==1 && SaveVersionSub>=3)
   {
      File >> Plane.Sponsored;
   }
   if (SaveVersion==1 && SaveVersionSub>=4)
   {
      File >> Plane.Problem;
      File >> Plane.PseudoProblem;
   }
   if (SaveVersion==1 && SaveVersionSub>=13)
   {
      File >> Plane.Wartungskosten;
   }
   if (SaveVersion==1 && SaveVersionSub>=100)
   {
      File >> Plane.Triebwerk  >> Plane.TriebwerkTarget;
      File >> Plane.Reifen     >> Plane.ReifenTarget;
      File >> Plane.Elektronik >> Plane.ElektronikTarget;
      File >> Plane.Sicherheit >> Plane.SicherheitTarget;
      File >> Plane.OhneSitze;
      File >> Plane.PersonalQuality;
   }
   else
   {
      Plane.Triebwerk   = Plane.TriebwerkTarget  = 0;
      Plane.Reifen      = Plane.ReifenTarget     = 0;
      Plane.Elektronik  = Plane.ElektronikTarget = 0;
      Plane.Sicherheit  = Plane.SicherheitTarget = 0;
      Plane.OhneSitze   = 0;
   }

   File >> Plane.ptHersteller      >> Plane.ptErstbaujahr    >> Plane.ptName           >> Plane.ptReichweite >>
           Plane.ptGeschwindigkeit >> Plane.ptPassagiere     >> Plane.ptAnzPiloten >>
           Plane.ptAnzBegleiter    >> Plane.ptTankgroesse    >> Plane.ptVerbrauch  >>
           Plane.ptPreis           >> Plane.ptWartungsfaktor >> Plane.ptKommentar;

   File >> Plane.ptLaerm;

   File >> Plane.XPlane;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Speichert ein CPlanes Datum:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CPlanes &Planes)
{
   File << Planes.Planes;
   File << *((ALBUM<CPlane>*)&Planes);

   return (File);
}

//--------------------------------------------------------------------------------------------
//Läd ein CPlanes Datum:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CPlanes &Planes)
{
   File >> Planes.Planes;
   File >> *((ALBUM<CPlane>*)&Planes);

   return (File);
}

//============================================================================================
//CPlanes::
//============================================================================================
//Schaut nach, ob der Flugzeugname schon verwendet wird:
//============================================================================================
BOOL CPlanes::IsPlaneNameInUse (const CString &PlaneName)
{
   SLONG c;

   for (c=0; c<(SLONG)AnzEntries(); c++)
      if (IsInAlbum(c) && Planes[c].Name==PlaneName) return (TRUE);

   return (FALSE);
}

//--------------------------------------------------------------------------------------------
// Repairiert die Referenzen in dem CXPlane Objekt. Dort zeigt das Album auf den Buffer und
// wenn die CPlanes resizet wurden liegt der Buffer woanders und das Album fragt sich warum
// es davon nichts weiß.
//--------------------------------------------------------------------------------------------
void CPlanes::RepairReferences (void)
{
   SLONG c;

   for (c=0; c<(SLONG)AnzEntries(); c++)
      if (IsInAlbum(c))
         Planes[c].RepairReferences();
}

//--------------------------------------------------------------------------------------------
//Bewegt alle Flugzeuge für Animationen etwas weiter:
//--------------------------------------------------------------------------------------------
void CPlanes::DoOneStep (SLONG PlayerNum)
{
   SLONG c;

   for (c=0; c<(SLONG)AnzEntries(); c++)
      if (IsInAlbum(c))
         Planes[c].DoOneStep (PlayerNum);
}

//--------------------------------------------------------------------------------------------
//Wieviel mehr Begleiter als normal haben wir im Durchschnitt?:
//--------------------------------------------------------------------------------------------
double CPlanes::GetAvgBegleiter (void)
{
   SLONG Anz=0, Max=0;

   for (SLONG c=this->AnzEntries()-1; c>=0; c--)
      if (this->IsInAlbum(c))
      {
         //Anz+=min ((*this)[c].AnzBegleiter, PlaneTypes[(*this)[c].TypeId].AnzBegleiter);
         Anz+=min ((*this)[c].AnzBegleiter, (*this)[c].ptAnzBegleiter);
         Max+=(*this)[c].MaxBegleiter;
      }

   if (Anz) return (double(Max)/double(Anz));
       else return (1);
}

//--------------------------------------------------------------------------------------------
//Aktualisiert alle projeziert Positionen:
//--------------------------------------------------------------------------------------------
void CPlanes::UpdateGlobePos (UWORD EarthAlpha)
{
   for (SLONG c=this->AnzEntries()-1; c>=0; c--)
      if (this->IsInAlbum(c))
      {
         (*this)[c].UpdateGlobePos (EarthAlpha);
      }
}

//--------------------------------------------------------------------------------------------
//Hat der Spieler ein Flugzeug, wo es Probleme gibt?
//--------------------------------------------------------------------------------------------
BOOL CPlanes::HasProblemPlane (void)
{
   for (SLONG c=this->AnzEntries()-1; c>=0; c--)
      if (this->IsInAlbum(c))
         if ((*this)[c].Problem) return (TRUE);

   return (FALSE);
}

//============================================================================================
//CPlaneNames::
//============================================================================================
//Konstruktor:
//============================================================================================
CPlaneNames::CPlaneNames ()
{
}

//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
CPlaneNames::CPlaneNames (const CString &TabFilename)
{
   ReInit (TabFilename);
}

//--------------------------------------------------------------------------------------------
//Destruktor:
//--------------------------------------------------------------------------------------------
CPlaneNames::~CPlaneNames ()
{
}

//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
void CPlaneNames::ReInit (const CString &TabFilename)
{
   //CStdioFile    Tab;
   CString       str;
   BUFFER<char>  Line(300);
   SLONG         Anz1=0, Anz2=0;

   //Load Table header:
   BUFFER<UBYTE> FileData (*LoadCompleteFile (FullFilename (TabFilename, ExcelPath)));
   SLONG         FileP=0;

   //Die erste Zeile einlesen
   FileP=ReadLine (FileData, FileP, Line, 300);

   NameBuffer1.ReSize (MAX_PNAMES1);
   NameBuffer2.ReSize (MAX_PNAMES1);

   while (1)
   {
      if (FileP>=FileData.AnzEntries()) break;
      FileP=ReadLine (FileData, FileP, Line, 300);

      TeakStrRemoveEndingCodes (Line, "\xd\xa\x1a\r");

      //Tabellenzeile hinzufügen:
      str = strtok (Line, ";\x8\"");

      if (atoi (strtok (NULL, TabSeparator))==1)
      {
         if (Anz1>=MAX_PNAMES1) TeakLibW_Exception (FNL, ExcImpossible, "");
         NameBuffer1[Anz1++] = str;
      }
      else
      {
         if (Anz2>=MAX_PNAMES2) TeakLibW_Exception (FNL, ExcImpossible, "");
         NameBuffer2[Anz2++] = str;
      }
   }

   //Keine freien Plätze offen lassen:
   NameBuffer1.ReSize (Anz1);
   NameBuffer2.ReSize (Anz2);
}

//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
CString CPlaneNames::GetRandom (TEAKRAND *pRnd)
{
   if (pRnd)
      return (NameBuffer1[1+pRnd->Rand((NameBuffer1.AnzEntries()-1))]);
   else
      return (NameBuffer1[1+rand()%(NameBuffer1.AnzEntries()-1)]);
}

//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
CString CPlaneNames::GetUnused (TEAKRAND *pRnd)
{
   SLONG   c;
   CString Name;

   //Namen erster Qualitätsstufe
   for (c=0; c<NameBuffer1.AnzEntries(); c++)
   {
      Name = GetRandom(pRnd);
      if (!Sim.Players.IsPlaneNameInUse(Name)) return (Name);
   }

   for (c=1; c<NameBuffer1.AnzEntries(); c++)
   {
      if (!Sim.Players.IsPlaneNameInUse(NameBuffer1[c])) return (NameBuffer1[c]);
   }

   //Namen zweiter Qualitätsstufe
   for (c=0; c<NameBuffer2.AnzEntries(); c++)
   {
      if (pRnd)
         Name = NameBuffer2[1+pRnd->Rand((NameBuffer2.AnzEntries()-1))];
      else
         Name = NameBuffer2[1+rand()%(NameBuffer2.AnzEntries()-1)];

      if (!Sim.Players.IsPlaneNameInUse(Name)) return (Name);
   }

   for (c=1; c<NameBuffer2.AnzEntries(); c++)
   {
      if (!Sim.Players.IsPlaneNameInUse(NameBuffer2[c])) return (NameBuffer2[c]);
   }

   return ("ohne Name");
}