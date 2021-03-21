//============================================================================================
//Schedule.cpp - Der Bildschirm für den Scheduler
//============================================================================================
#include "stdafx.h"
#include "AtNet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char FileId[] = "Sche";

extern SLONG FoodCosts[];

//Daten des aktuellen Savegames beim laden:
extern SLONG SaveVersion;
extern SLONG SaveVersionSub;

__int64 abs64 (__int64 v);

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
__int64 min64 (__int64 a, __int64 b)
{
   if (a<b) return (a); else return (b);
}

//============================================================================================
//CFlugplanEintrag::
//============================================================================================
//Konstruktor:
//============================================================================================
CFlugplanEintrag::CFlugplanEintrag ()
{
   CFlugplanEintrag::Okay        = 0;
   CFlugplanEintrag::Gate        = -1;
   CFlugplanEintrag::GateWarning = FALSE;
   CFlugplanEintrag::Startzeit   = 0;
   CFlugplanEintrag::Landezeit   = 0;
   CFlugplanEintrag::ObjectType  = 0;
   CFlugplanEintrag::ObjectId    = -1;
}

//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
CFlugplanEintrag::CFlugplanEintrag (BOOL ObjectType, ULONG ObjectId)
{
   CFlugplanEintrag::Okay        = 0;
   CFlugplanEintrag::Gate        = -1;
   CFlugplanEintrag::GateWarning = FALSE;
   CFlugplanEintrag::Startzeit   = 0;
   CFlugplanEintrag::Landezeit   = 0;
   CFlugplanEintrag::ObjectType  = ObjectType;
   CFlugplanEintrag::ObjectId    = ObjectId;
}

//--------------------------------------------------------------------------------------------
//Speichert ein CFlugplanEintrag Datum:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CFlugplanEintrag &Eintrag)
{
   if (SaveVersion==1 && SaveVersionSub==1)
   {
      File << Eintrag.Okay      << Eintrag.HoursBefore << Eintrag.Passagiere << Eintrag.PassagiereFC
           << Eintrag.PArrived  << Eintrag.Gate        << Eintrag.VonCity    << Eintrag.GateWarning
           << Eintrag.NachCity  << Eintrag.Startzeit   << Eintrag.Landezeit
           << Eintrag.Startdate << Eintrag.Landedate   << Eintrag.ObjectType
           << Eintrag.ObjectId  << Eintrag.Ticketpreis << Eintrag.TicketpreisFC;
   }
   else if (SaveVersion==1 && SaveVersionSub>=2)
   {
      File << Eintrag.ObjectType;

      if (Eintrag.ObjectType)
      {
         File << Eintrag.Okay      << Eintrag.HoursBefore << Eintrag.Passagiere << Eintrag.PassagiereFC
              << Eintrag.PArrived  << Eintrag.Gate        << Eintrag.VonCity    << Eintrag.GateWarning
              << Eintrag.NachCity  << Eintrag.Startzeit   << Eintrag.Landezeit
              << Eintrag.Startdate << Eintrag.Landedate
              << Eintrag.ObjectId  << Eintrag.Ticketpreis << Eintrag.TicketpreisFC;
      }
   }

   return (File);
}

//--------------------------------------------------------------------------------------------
//Läd ein CFlugplanEintrag Datum:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CFlugplanEintrag &Eintrag)
{
   if (SaveVersion==1 && SaveVersionSub==1)
   {
      File >> Eintrag.Okay      >> Eintrag.HoursBefore >> Eintrag.Passagiere >> Eintrag.PassagiereFC
           >> Eintrag.PArrived  >> Eintrag.Gate        >> Eintrag.VonCity    >> Eintrag.GateWarning
           >> Eintrag.NachCity  >> Eintrag.Startzeit   >> Eintrag.Landezeit
           >> Eintrag.Startdate >> Eintrag.Landedate   >> Eintrag.ObjectType
           >> Eintrag.ObjectId  >> Eintrag.Ticketpreis >> Eintrag.TicketpreisFC;
   }
   else if (SaveVersion==1 && SaveVersionSub>=2)
   {
      File >> Eintrag.ObjectType;

      if (Eintrag.ObjectType)
      {
         File >> Eintrag.Okay      >> Eintrag.HoursBefore >> Eintrag.Passagiere >> Eintrag.PassagiereFC
              >> Eintrag.PArrived  >> Eintrag.Gate        >> Eintrag.VonCity    >> Eintrag.GateWarning
              >> Eintrag.NachCity  >> Eintrag.Startzeit   >> Eintrag.Landezeit
              >> Eintrag.Startdate >> Eintrag.Landedate
              >> Eintrag.ObjectId  >> Eintrag.Ticketpreis >> Eintrag.TicketpreisFC;
      }
      else
      {
         Eintrag.Okay        = 0;
         Eintrag.Gate        = -1;
         Eintrag.GateWarning = FALSE;
         Eintrag.Startzeit   = 0;
         Eintrag.Landezeit   = 0;
         Eintrag.ObjectType  = 0;
         Eintrag.ObjectId    = -1;
      }
   }

   return (File);
}

//============================================================================================
//CFlugplan::
//============================================================================================
//Konstruktor des Flugplans:
//============================================================================================
CFlugplan::CFlugplan ()
{
   SLONG c;

   NextFlight = -1;
   NextStart  = -1;
   Flug.ReSize (10*6);  //Maximal 10 Flüge pro Tag

   for (c=Flug.AnzEntries()-1; c>=0; c--)
      Flug[c].ObjectType=0;
}

//--------------------------------------------------------------------------------------------
//Aktualisiert "Next Flight":
//--------------------------------------------------------------------------------------------
void CFlugplan::UpdateNextFlight (void)
{
   NextFlight = -1;
   for (SLONG e=0; e<Flug.AnzEntries(); e++)
   {
      if (NextFlight==-1 && Flug[e].ObjectType && (Flug[e].Landedate>Sim.Date || (Flug[e].Landedate==Sim.Date && Flug[e].Landezeit>=Sim.GetHour())))
      {
         NextFlight = e;
      }
   }
}

//--------------------------------------------------------------------------------------------
//Gibt true zurück, wenn der Flugplan einen der beiden ObjectIds enthält:
//--------------------------------------------------------------------------------------------
BOOL CFlugplan::ContainsFlight (ULONG ObjectType, SLONG ObjectId1, SLONG ObjectId2)
{
   for (SLONG e=0; e<Flug.AnzEntries(); e++)
      if (Flug[e].ObjectType==SLONG(ObjectType) && (Flug[e].ObjectId==ObjectId1 || Flug[e].ObjectId==ObjectId2))
         return (TRUE);

   return (FALSE);
}

//--------------------------------------------------------------------------------------------
//Aktualisiert "Next Start":
//--------------------------------------------------------------------------------------------
void CFlugplan::UpdateNextStart (void)
{
   NextStart = -1;
   for (SLONG e=0; e<Flug.AnzEntries(); e++)
      if (Flug[e].ObjectType && (Flug[e].Startdate>Sim.Date || (Flug[e].Startdate==Sim.Date && Flug[e].Startzeit+1>=Sim.GetHour())))
      {
          NextStart = e;
          break;
      }
}

//--------------------------------------------------------------------------------------------
//Druckt einen Flugplan auf dem Herkules-Schirm:
//--------------------------------------------------------------------------------------------
void CFlugplan::Dump (bool bHercules)
{
   char Buffer[500];

   hprintf ("------------------------------------------------------------");
   for (SLONG e=0; e<Flug.AnzEntries(); e++)
   {
      switch (Flug[e].ObjectType)
      {
          case 1:
             sprintf (Buffer, "[%02d] %02d/%02d-%02d/%02d Route   %s-%s", e, Flug[e].Startzeit, Flug[e].Startdate, Flug[e].Landezeit, Flug[e].Landedate, (LPCTSTR)Cities[Flug[e].VonCity].Name, (LPCTSTR)Cities[Flug[e].NachCity].Name);
             break;

          case 2:
             sprintf (Buffer, "[%02d] %02d/%02d-%02d/%02d Auftrag %s-%s", e, Flug[e].Startzeit, Flug[e].Startdate, Flug[e].Landezeit, Flug[e].Landedate, (LPCTSTR)Cities[Flug[e].VonCity].Name, (LPCTSTR)Cities[Flug[e].NachCity].Name);
             break;

          case 3:
             sprintf (Buffer, "[%02d] %02d/%02li-%02d/%02d Auto    %s-%s", e, Flug[e].Startzeit, Flug[e].Startdate, Flug[e].Landezeit, Flug[e].Landedate, (LPCTSTR)Cities[Flug[e].VonCity].Name, (LPCTSTR)Cities[Flug[e].NachCity].Name);
             break;

          case 4:
             sprintf (Buffer, "[%02d] %02d/%02li-%02d/%02d Fracht  %s-%s", e, Flug[e].Startzeit, Flug[e].Startdate, Flug[e].Landezeit, Flug[e].Landedate, (LPCTSTR)Cities[Flug[e].VonCity].Name, (LPCTSTR)Cities[Flug[e].NachCity].Name);
             break;

          default:
             continue;
      }

      if (bHercules) hprintf (Buffer);
                else DisplayBroadcastMessage (Buffer);
   }
}

//--------------------------------------------------------------------------------------------
//Speichert ein CFlugplan Datum:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CFlugplan &Plan)
{
   File << Plan.Flug << Plan.StartCity << Plan.NextFlight << Plan.NextStart;
   return (File);
}

//--------------------------------------------------------------------------------------------
//Läd ein CFlugplan Datum:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CFlugplan &Plan)
{
   File >> Plan.Flug >> Plan.StartCity >> Plan.NextFlight >> Plan.NextStart;
   return (File);
} 

//--------------------------------------------------------------------------------------------
//Berechnet die Zahl der Passagiere auf dem Flug, wenn der Flug in diesem Augenblick erstellt/
//geändert wird. Kurzfristige Flüge haben weniger passagiere
//--------------------------------------------------------------------------------------------
void CFlugplanEintrag::CalcPassengers (SLONG PlayerNum, CPlane &qPlane)
{
   PLAYER &qPlayer=Sim.Players.Players[PlayerNum];

   //Routen automatisch ergänzen:
   if (ObjectType==1)
   {
      VonCity  = Routen[ObjectId].VonCity;
      NachCity = Routen[ObjectId].NachCity;

      SLONG Dauer = Cities.CalcFlugdauer (VonCity, NachCity, qPlane.ptGeschwindigkeit);
      Landezeit = (Startzeit+Dauer)%24;
      Landedate = Startdate;
      if (Landezeit<Startzeit) Landedate++;
   }

   if (ObjectType==1) //Route
   {
      CRoute &qRoute = Routen[ObjectId];

      //Normale Passagiere: Aber nicht mehr kurz vor dem Start ändern:
      if (Startdate>Sim.Date || (Startdate==Sim.Date && Sim.GetHour()+1<Startzeit))
      {
         SLONG c, tmp;
         SLONG Gewichte[4], Gesamtgewicht=0;

         //Bonusregelgungen für Spieler anhand von Image, Rang, Preis, ...
         for (c=0; c<4; c++)
         {
            PLAYER     &qPlayer=Sim.Players.Players[c];
            CRentRoute &qRentRoute = qPlayer.RentRouten.RentRouten[(SLONG)Routen(ObjectId)];

            if (qRentRoute.Rang!=0)
            {
               //Ticketpreis ist Basis der Rechnung:
               if (Ticketpreis!=0)
                  Gewichte[c]=10000/Ticketpreis;
               else 
                  Gewichte[c]=100000;

               SLONG Costs = CalculateFlightCost (Routen[ObjectId].VonCity, Routen[ObjectId].NachCity, 800, 800, -1)*3/180*2;

               if (Ticketpreis>Costs*3) Gewichte[c]=Gewichte[c]*9/10;
               if (Ticketpreis>Costs*5) Gewichte[c]=Gewichte[c]*9/10;
               if (Ticketpreis>Costs*6) Gewichte[c]=Gewichte[c]*8/10;

               if (Gewichte[c]==0) Gewichte[c]=1;

               //Zuschläge für den Rang:
               if (qRentRoute.Rang==1) Gewichte[c]=Gewichte[c]*150/100;
               if (qRentRoute.Rang==2) Gewichte[c]=Gewichte[c]*120/100;

               //Abschläge, wenn er selten fliegt:
               if (qRentRoute.AvgFlown>4)
               {
                  Gewichte[c]=Gewichte[c]*10/(10+min(qRentRoute.AvgFlown-3, 10));
                  if (Gewichte[c]<1) Gewichte[c]=1;
               }

               //25% Bonus für den Computer
               if (qPlayer.Owner==1) Gewichte[c]+=Gewichte[c]/4;
            }
            else 
            {
               Gewichte[c]=0;

               if (c==PlayerNum) Gewichte[c]=1;
            }

            //log: hprintf ("Gewichte[c]=%Li",Gewichte[c]);

            Gesamtgewicht+=Gewichte[c];
         }

         //log: hprintf ("qRoute.Bedarf=%Li",qRoute.Bedarf);

         //Wie viele wollen mitfliegen?
#ifdef _DEBUG
         tmp = qRoute.Bedarf*Gewichte[PlayerNum]/Gesamtgewicht;
#else
         if (Gesamtgewicht>0) tmp = qRoute.Bedarf*Gewichte[PlayerNum]/Gesamtgewicht;
                         else tmp = 0;
#endif
         //Wie viele können mitfliegen (plus Toleranz)?
         tmp = min (tmp, qPlane.MaxPassagiere+qPlane.MaxPassagiere/2);

         //NetGenericAsync (24004+ObjectId+Sim.Date*100, tmp);

         //Fliegt er heute oder morgen? Dann machen wir Abstriche!
         if (HoursBefore>48) HoursBefore=48;
         if (HoursBefore<48) tmp = tmp * (HoursBefore+48) / (48+48);

         //NetGenericAsync (30000+ObjectId+Sim.Date*100, HoursBefore);
         //NetGenericAsync (23003+ObjectId+Sim.Date*100, tmp);

         //Abschläge bei Wucherpreisen:
         SLONG Cost = CalculateFlightCost (qRoute.VonCity, qRoute.NachCity, 800, 800, -1)*3/180*2;

         //NetGenericAsync (31001+ObjectId+Sim.Date*100, Cost);
         //NetGenericAsync (32002+ObjectId+Sim.Date*100, Ticketpreis);

         Cost*=3;
         if (Ticketpreis>Cost && Cost>10 && Ticketpreis>0)
            tmp=tmp * (Cost-10) / Ticketpreis;

         //NetGenericAsync (19009+ObjectId+Sim.Date*100, tmp);

         //NetGenericAsync (33003+ObjectId+Sim.Date*100, Startzeit);
         //NetGenericAsync (34004+ObjectId+Sim.Date*100, Landezeit);

         //Schlechte Zeiten? Das gibt Abstriche!
         if (Startzeit<5 || Startzeit>22) tmp=tmp*5/6;
         if (Landezeit<5 || Landezeit>22) tmp=tmp*5/6;

         //NetGenericAsync (22002+ObjectId+Sim.Date*100, tmp);

         //Image berücksichtigen:
         SLONG   ImageTotal;

         {
            CRentRoute &qRentRoute = qPlayer.RentRouten.RentRouten[(SLONG)Routen(ObjectId)];

            //log: hprintf ("qRentRoute.Image=%li, qPlayer.Image=%li",qRentRoute.Image, qPlayer.Image);
            ImageTotal  = qRentRoute.Image*4;
            ImageTotal += qPlayer.Image;
            ImageTotal += 200;

            if (ImageTotal<0)    ImageTotal=0;
            if (ImageTotal>1000) ImageTotal=1000;

            //NetGenericAsync (30003+ObjectId+Sim.Date*100, qRentRoute.Image);
            //NetGenericAsync (30004+ObjectId+Sim.Date*100, qPlayer.Image);
         }
         tmp = UWORD(tmp*(400+ImageTotal)/1100);

         if (qPlane.ptLaerm>60) tmp = UWORD(tmp*1000/1100);
         if (qPlane.ptLaerm>80) tmp = UWORD(tmp*1000/1100);
         if (qPlane.ptLaerm>100) tmp = UWORD(tmp*1000/1100);
         if (qPlane.ptLaerm>110) tmp = UWORD(tmp*1000/1100);

         //NetGenericAsync (21001+ObjectId+Sim.Date*100, tmp);

         //Wie viele können wirklich mitfliegen?
         if (Gesamtgewicht>0) tmp = min (tmp, qRoute.Bedarf*Gewichte[PlayerNum]/Gesamtgewicht);
         tmp = min (tmp, qPlane.MaxPassagiere);

         Passagiere = (UWORD)tmp;

         if (qPlayer.Presseerklaerung)
            Passagiere = 1+(Passagiere%3);

         //NetGenericAsync (20000+ObjectId+Sim.Date*100, Passagiere);
      }

      //Erste Klasse Passagiere: Aber nicht mehr kurz vor dem Start ändern:
      if (Startdate>Sim.Date || (Startdate==Sim.Date && Sim.GetHour()+1<Startzeit))
      {
         SLONG c, tmp;
         SLONG Gewichte[4], Gesamtgewicht=0;

         //Bonusregelgungen für Spieler anhand von Image, Rang, Preis, ...
         for (c=0; c<4; c++)
         {
            PLAYER     &qPlayer=Sim.Players.Players[c];
            CRentRoute &qRentRoute = qPlayer.RentRouten.RentRouten[(SLONG)Routen(ObjectId)];

            if (qRentRoute.Rang!=0)
            {
               //Ticketpreis ist Basis der Rechnung:
               if (TicketpreisFC!=0)
                  Gewichte[c]=10000/ TicketpreisFC;
               else 
                  Gewichte[c]=100000;

               SLONG Costs = CalculateFlightCost (Routen[ObjectId].VonCity, Routen[ObjectId].NachCity, 800, 800, -1)*3/180*2;

               if (TicketpreisFC >Costs*3*3) Gewichte[c]=Gewichte[c]*9/10;
               if (TicketpreisFC >Costs*3*5) Gewichte[c]=Gewichte[c]*9/10;
               if (TicketpreisFC >Costs*3*6) Gewichte[c]=Gewichte[c]*8/10;

               if (Gewichte[c]==0) Gewichte[c]=1;

               //Zuschläge für den Rang:
               if (qRentRoute.Rang==1) Gewichte[c]=Gewichte[c]*170/100;
               if (qRentRoute.Rang==2) Gewichte[c]=Gewichte[c]*120/100;

               //Abschläge, wenn er selten fliegt:
               if (qRentRoute.AvgFlown>4)
               {
                  Gewichte[c]=Gewichte[c]*10/(10+min(qRentRoute.AvgFlown-3, 10));
                  if (Gewichte[c]<1) Gewichte[c]=1;
               }

               //25% Bonus für den Computer
               if (qPlayer.Owner==1) Gewichte[c]+=Gewichte[c]/4;
            }
            else 
            {
               Gewichte[c]=0;

               if (c==PlayerNum) Gewichte[c]=1;
            }

            Gesamtgewicht+=Gewichte[c];
         }

         //Wie viele wollen mitfliegen?
#ifdef _DEBUG
         tmp = qRoute.Bedarf*Gewichte[PlayerNum]/Gesamtgewicht;
#else
         if (Gesamtgewicht>0) tmp = qRoute.Bedarf*Gewichte[PlayerNum]/Gesamtgewicht;
                         else tmp = 0;
#endif
         //Wie viele können mitfliegen (plus Toleranz)?
         tmp = min (tmp, qPlane.MaxPassagiereFC+qPlane.MaxPassagiereFC/2);

         //Fliegt er heute oder morgen oder übermorgen oder überübermorgen? Dann machen wir Abstriche!
         if (HoursBefore>72) HoursBefore=72;
         if (HoursBefore<72) tmp = tmp * (HoursBefore+48) / (72+48);

         //Abschläge bei Wucherpreisen:
         SLONG Cost = CalculateFlightCost (qRoute.VonCity, qRoute.NachCity, 800, 800, -1)*3/180*2;

         Cost*=3;
         if (TicketpreisFC >Cost*3 && Cost>10 && TicketpreisFC >0)
            tmp=tmp * (Cost-10) / TicketpreisFC;

         //Schlechte Zeiten? Das gibt Abstriche!
         if (Startzeit<5 || Startzeit>22) tmp=tmp*5/6;
         if (Landezeit<5 || Landezeit>22) tmp=tmp*5/6;

         //Luxus ist ganz wichtig für die: (Summe e [0..16])
         long LuxusSumme = qPlane.Sitze+qPlane.Essen+qPlane.Tabletts+qPlane.Deco+qPlane.Triebwerk+qPlane.Reifen+qPlane.Elektronik+qPlane.Sicherheit;

         //Sicherheit ist auch wichtig: (Summe e [0..18])
         LuxusSumme += ((qPlayer.SecurityFlags&(1<<10))!=0)+((qPlayer.SecurityFlags&(1<<11))!=0);

         if (LuxusSumme<6) tmp=0;
         if (LuxusSumme>=6 && LuxusSumme<=18) tmp=tmp*(LuxusSumme-6)/8;

         //Image berücksichtigen:
         SLONG   ImageTotal;

         {
            CRentRoute &qRentRoute = qPlayer.RentRouten.RentRouten[(SLONG)Routen(ObjectId)];

            ImageTotal  = qRentRoute.Image*4;
            ImageTotal += qPlayer.Image;
            ImageTotal += 200;

            if (ImageTotal<0)    ImageTotal=0;
            if (ImageTotal>1000) ImageTotal=1000;
         }
         tmp = UWORD(tmp*(400+ImageTotal)/1100);

         if (qPlane.ptLaerm>40)  tmp = UWORD(tmp*1000/1100);
         if (qPlane.ptLaerm>60)  tmp = UWORD(tmp*1000/1100);
         if (qPlane.ptLaerm>80)  tmp = UWORD(tmp*1000/1100);
         if (qPlane.ptLaerm>100) tmp = UWORD(tmp*1000/1100);

         //Wie viele können wirklich mitfliegen?
         if (Gesamtgewicht>0) tmp = min (tmp, qRoute.Bedarf*Gewichte[PlayerNum]/Gesamtgewicht);
         tmp = min (tmp, qPlane.MaxPassagiereFC);

         PassagiereFC = (UWORD)tmp;

         if (qPlayer.Presseerklaerung)
            PassagiereFC = 1+(PassagiereFC%3);
      }
   }
   else if (ObjectType==2)
   {
      //Auftrag:
      Passagiere   = (UWORD)min(qPlane.MaxPassagiere, long(qPlayer.Auftraege[ObjectId].Personen));

      PassagiereFC = (UWORD)(qPlayer.Auftraege[ObjectId].Personen-Passagiere);
   }
}

//--------------------------------------------------------------------------------------------
//Flug wird gestartet, die Kosten und Einnahmen werden verbucht:
//--------------------------------------------------------------------------------------------
void CFlugplanEintrag::BookFlight (CPlane *Plane, SLONG PlayerNum)
{
   __int64 Saldo;
   SLONG   Einnahmen=0, Ausgaben=0;
   CString CityString;
   PLAYER &qPlayer = Sim.Players.Players[(SLONG)PlayerNum];
     
   //Hat angebliche Asynchronitäten berichtet, obwohl der Flugplan gleich war!
   //NetGenericAsync (90000+ObjectId+Sim.Date*100+PlayerNum*1000, Startzeit);
   //if (ObjectType==1)
   //   NetGenericAsync (80000+ObjectId+Sim.Date*100+PlayerNum*1000, qPlayer.RentRouten.RentRouten[(SLONG)Routen(ObjectId)].Image);

   SLONG BadKerosinFaktor=1;  //Wird bei schlechtem Kerosin erhöht

   if (qPlayer.TankOpen && qPlayer.TankInhalt>0 && qPlayer.BadKerosin*100/qPlayer.TankInhalt>50)
      BadKerosinFaktor=3;
   else if ((qPlayer.TankOpen==0 || qPlayer.TankInhalt==0) && qPlayer.KerosinKind==2)
      BadKerosinFaktor=3;

   if (qPlayer.Owner==0 && ObjectType==1)
   {
      qPlayer.DoRoutes++;
      if (qPlayer.DoRoutes==1) qPlayer.DoRoutes++;
   }

   CalcPassengers (PlayerNum, *Plane);

   if (Plane->MaxPassagiere>0)
   {
      if (Plane->Auslastung==0) Plane->Auslastung = Passagiere*100/Plane->MaxPassagiere;
      else                      Plane->Auslastung = (Plane->Auslastung+Passagiere*100/Plane->MaxPassagiere)/2;
   }
   else Plane->Auslastung=0;

   if (Plane->MaxPassagiereFC>0)
   {
      if (Plane->AuslastungFC==0) Plane->AuslastungFC = PassagiereFC*100/Plane->MaxPassagiereFC;
      else                        Plane->AuslastungFC = (Plane->AuslastungFC+PassagiereFC*100/Plane->MaxPassagiereFC)/2;
   }
   else Plane->AuslastungFC=0;

   Plane->Kilometer       += Cities.CalcDistance (VonCity, NachCity)/1000;
   Plane->SummePassagiere += Passagiere;

   //Insgesamt geflogene Meilen:
   qPlayer.NumMiles += Cities.CalcDistance (VonCity, NachCity)/1609;

   if (ObjectType==2 && Okay!=0)
      Ausgaben  = GetRealAusgaben(PlayerNum, *Plane, Plane->Name);
      //Ausgaben  = GetRealAusgaben(PlayerNum, Plane->TypeId, Plane->Name);
   else if (ObjectType==4)
   {
      Ausgaben  = GetRealAusgaben(PlayerNum, *Plane, Plane->Name);

      if (qPlayer.Frachten[ObjectId].TonsLeft>0)
      {
         qPlayer.NumFracht+=min(qPlayer.Frachten[ObjectId].TonsLeft, Plane->ptPassagiere/10);

         if (qPlayer.Frachten[ObjectId].Praemie==0)
            qPlayer.NumFrachtFree+=min(qPlayer.Frachten[ObjectId].TonsLeft, Plane->ptPassagiere/10);

         qPlayer.Frachten[ObjectId].TonsLeft-=Plane->ptPassagiere/10;

         if (qPlayer.Frachten[ObjectId].TonsLeft<=0)
         {
            qPlayer.Frachten[ObjectId].TonsLeft=0;
            Einnahmen = GetEinnahmen(PlayerNum, *Plane);
         }
      }
   }
   else
   {
      //Einnahmen = GetEinnahmen(PlayerNum, Plane->TypeId);
      Einnahmen = GetEinnahmen(PlayerNum, *Plane);
      Ausgaben  = GetRealAusgaben(PlayerNum, *Plane, Plane->Name);
      //Ausgaben  = GetRealAusgaben(PlayerNum, Plane->TypeId, Plane->Name);
   }

   if (ObjectType==1 || ObjectType==2)
      Ausgaben += Passagiere*FoodCosts[Plane->Essen];

   //Für den Statistik-Screen:
   if (ObjectType==1 || ObjectType==2)
   {
      qPlayer.Statistiken[STAT_PASSAGIERE].AddAtPastDay (0, Passagiere);
      qPlayer.Statistiken[STAT_PASSAGIERE].AddAtPastDay (0, PassagiereFC);
      qPlayer.Statistiken[STAT_FLUEGE].AddAtPastDay (0, 1);

      if (SLONG(VonCity)==Sim.HomeAirportId || SLONG(NachCity)==Sim.HomeAirportId)
      {
         qPlayer.Statistiken[STAT_PASSAGIERE_HOME].AddAtPastDay (0, Passagiere);
         qPlayer.Statistiken[STAT_PASSAGIERE_HOME].AddAtPastDay (0, PassagiereFC);
      }
   }

   if (ObjectType==1)
   {
      //In beide Richtungen vermerken:
      qPlayer.RentRouten.RentRouten[(SLONG)Routen(ObjectId)].HeuteBefoerdert+=Passagiere;

      for (SLONG c=Routen.AnzEntries()-1; c>=0; c--)
         if (Routen.IsInAlbum(c) && Routen[c].VonCity==Routen[ObjectId].NachCity && Routen[c].NachCity==Routen[ObjectId].VonCity)
         {
            qPlayer.RentRouten.RentRouten[c].HeuteBefoerdert+=Passagiere+PassagiereFC;
            break;
         }
   }

   qPlayer.Bilanz.Kerosin+=Ausgaben;
   qPlayer.Statistiken[STAT_A_KEROSIN].AddAtPastDay (0, -Ausgaben);
   Plane->Salden[0]-=Ausgaben;

   if (ObjectType==1) qPlayer.Bilanz.Tickets+=Einnahmen;
   if (ObjectType==2) qPlayer.Bilanz.Auftraege+=Einnahmen;
   if (ObjectType==2) qPlayer.NumAuftraege++;
   Plane->Salden[0]+=Einnahmen;

   if (ObjectType==1) qPlayer.Statistiken[STAT_E_ROUTEN].AddAtPastDay (0, Einnahmen);
   if (ObjectType==2) qPlayer.Statistiken[STAT_E_AUFTRAEGE].AddAtPastDay (0, Einnahmen);

   Saldo=Einnahmen-Ausgaben;

   CityString = Cities[VonCity].Kuerzel + "-" + Cities[NachCity].Kuerzel;

   //Versteckter Bonus für Computerspieler:
   Saldo+=min64(qPlayer.Bonus, abs64(Saldo)/10);
   qPlayer.Bonus-=min64(qPlayer.Bonus, abs64(Saldo)/10);

   //Müssen wir das Flugzeug umrüsten (Sitze raus/rein wegen Fracht)?
   if (ObjectType!=3)
   {
      if (Plane->OhneSitze!=(ObjectType==4))
      {
         Plane->OhneSitze=(ObjectType==4);
         qPlayer.ChangeMoney (-15000, 2111, Plane->Name);
      }
   }

   qPlayer.ChangeMoney (Saldo, 2100-1+ObjectType, CityString);
   Sim.Players.Players[PlayerNum].Gewinn+=Saldo;

   //Kerosin aus dem Vorrat verbuchen:
   if (Sim.Players.Players[(SLONG)PlayerNum].TankOpen)
   {
      //SLONG Kerosin = CalculateFlightKerosin (VonCity, NachCity, PlaneTypes[Plane->TypeId].Verbrauch, PlaneTypes[Plane->TypeId].Geschwindigkeit);
      SLONG Kerosin = CalculateFlightKerosin (VonCity, NachCity, Plane->ptVerbrauch, Plane->ptGeschwindigkeit);
      SLONG tmp = min (qPlayer.TankInhalt, Kerosin); 
      qPlayer.TankInhalt -= tmp;
      qPlayer.BadKerosin -= tmp;
      if (qPlayer.BadKerosin<0) qPlayer.BadKerosin=0;

      if (qPlayer.TankInhalt==0 && tmp>0 && Sim.Players.Players[(SLONG)PlayerNum].HasBerater (BERATERTYP_KEROSIN))
         Sim.Players.Players[(SLONG)PlayerNum].Messages.AddMessage (BERATERTYP_KEROSIN, bprintf (StandardTexte.GetS (TOKEN_ADVICE, 3030), (LPCTSTR)Plane->Name));

      qPlayer.Bilanz.Kerosin+=SLONG(tmp*qPlayer.TankPreis);     //Kalkulatorische Kosten
      qPlayer.Statistiken[STAT_A_KEROSIN].AddAtPastDay (0, -long(tmp*qPlayer.TankPreis));
      Plane->Salden[0]-=SLONG(tmp*qPlayer.TankPreis);
   } 

   //Bei Routen den Bedarf bei den Leuten entsprechend verringern und die Bekanntheit verbessern:
   if (ObjectType==1)
   {
      CRentRoute &qRRoute = qPlayer.RentRouten.RentRouten[(SLONG)Routen(ObjectId)];

      qRRoute.LastFlown=0;

      if (!Sim.Players.Players[(SLONG)PlayerNum].HasFlownRoutes)
      {
         SLONG c, Anz;

         for (c=Anz=0; c<4; c++)
            if (Sim.Players.Players[c].HasFlownRoutes) Anz++;

         Sim.Headlines.AddOverride (1, bprintf ((LPCTSTR)(CString)StandardTexte.GetS (TOKEN_MISC, 2010+Anz), Sim.Players.Players[(SLONG)PlayerNum].AirlineX, Sim.Players.Players[(SLONG)PlayerNum].NameX), GetIdFromString ("1")+Anz+PlayerNum*100, 60);

         Sim.Players.Players[(SLONG)PlayerNum].HasFlownRoutes = TRUE;
      }

      //Auslastung der Route aktualisieren:
      if (Plane->MaxPassagiere>0)
      {
         if (qRRoute.Auslastung==0) qRRoute.Auslastung = Passagiere*100/Plane->MaxPassagiere;
         else                       qRRoute.Auslastung = (qRRoute.Auslastung*3+(Passagiere*100/Plane->MaxPassagiere))/4;
      }
      else qRRoute.Auslastung=0;

      if (Plane->MaxPassagiereFC>0)
      {
         if (qRRoute.AuslastungFC==0) qRRoute.AuslastungFC = PassagiereFC*100/Plane->MaxPassagiereFC;
         else                         qRRoute.AuslastungFC = (qRRoute.AuslastungFC*3+(PassagiereFC*100/Plane->MaxPassagiereFC))/4;
      }
      else qRRoute.AuslastungFC=0;

      Routen[ObjectId].Bedarf-=Passagiere;
      if (Routen[ObjectId].Bedarf<0) Routen[ObjectId].Bedarf=0;

      if (qRRoute.Image<100) qRRoute.Image++;
   }
   //Bei Aufträgen, die Prämie verbuchen; Aufträge als erledigt markieren
   else if (ObjectType==2)
   {
      Hdu.HercPrintf("Player %li flies %li Passengers from %s to %s\n", PlayerNum, qPlayer.Auftraege[ObjectId].Personen, (LPCTSTR)Cities[qPlayer.Auftraege[ObjectId].VonCity].Name, (LPCTSTR)Cities[qPlayer.Auftraege[ObjectId].NachCity].Name);

      if (Okay==0 || Okay==1)
      {
         qPlayer.Auftraege[ObjectId].InPlan=-1; //Auftrag erledigt
      }
      else
      {
         qPlayer.Messages.AddMessage (BERATERTYP_GIRL, bprintf (StandardTexte.GetS (TOKEN_ADVICE, 2313), Plane->Name));
      }

      //Add-On Mission 9
      if (Sim.Difficulty==DIFF_ADDON09)
      {
         if ((qPlayer.Owner!=1 && qPlayer.Auftraege[ObjectId].bUhrigFlight) || (qPlayer.NumOrderFlightsToday<5-(((Sim.Date+qPlayer.PlayerNum)%5)==1)-(((Sim.Date+qPlayer.PlayerNum)%11)==2)-(((Sim.Date+qPlayer.PlayerNum)%7)==0)-((Sim.Date+qPlayer.PlayerNum)%2)))
         {
            qPlayer.NumOrderFlights++;
            qPlayer.NumOrderFlightsToday++;
         }
      }
   }

   //Auswirkungen auf's Image verbuchen
   if (ObjectType==1 || ObjectType==2)
   {
      long pn = qPlayer.PlayerNum;
      if (Sim.Players.Players[qPlayer.PlayerNum].WerbeBroschuere!=-1)
      {
         pn=Sim.Players.Players[qPlayer.PlayerNum].WerbeBroschuere;
         if (Sim.Players.Players[qPlayer.PlayerNum].Owner==0)
            Sim.Players.Players[qPlayer.PlayerNum].Messages.AddMessage (BERATERTYP_GIRL, bprintf(StandardTexte.GetS (TOKEN_ADVICE, 2357), Plane->Name.c_str(), Sim.Players.Players[pn].AirlineX.c_str()));
      }

      PLAYER &qPlayerX = Sim.Players.Players[pn];

      if (qPlayer.Owner!=1)
      {
         Saldo = Plane->PersonalQuality;

         //ObjectId wirkt als deterministisches Random
         //log: hprintf ("Player[%li].Image now (deter) = %li", PlayerNum, qPlayer.Image);
         if (Saldo<50 && (ObjectId)%10==0) qPlayerX.Image--;
         if (Saldo>90 && (ObjectId)%10==0) qPlayerX.Image++;
         //log: hprintf ("Player[%li].Image now (deter) = %li", PlayerNum, qPlayer.Image);

         if (ObjectType==1) //Auswirkung auf Routenimage
            if (qPlayer.RentRouten.RentRouten[(SLONG)Routen(ObjectId)].Image>0)
               qPlayer.RentRouten.RentRouten[(SLONG)Routen(ObjectId)].Image--;

         Limit (SLONG(-1000), qPlayerX.Image, SLONG(1000));
      }

      //Zustand des Flugzeugs:
      SLONG Add=0;
      if (Plane->Zustand<20) Add-=50;
      if (Plane->Zustand<60) Add-=10;
      if (Plane->Zustand>98) Add+=10;

      //Ausstattung des Flugzeuges:
      if (Plane->Sitze==0)    Add--;
      if (Plane->Essen==0)    Add--;
      if (Plane->Tabletts==0) Add--;
      if (Plane->Deco==0)     Add--;
      if (Plane->Sitze==2)    Add+=2;
      if (Plane->Essen==2)    Add+=2;
      if (Plane->Tabletts==2) Add+=2;
      if (Plane->Deco==2)     Add+=2;

      //Wieviel Personal haben wir an Bord?

      if (qPlayer.Owner!=1) //Nur für reale Spieler:
         Add+=(3*Plane->AnzBegleiter/Plane->ptAnzBegleiter);
      //Add+=(3*Plane->AnzBegleiter/PlaneTypes[Plane->TypeId].AnzBegleiter);

      //Preispolitik bewerten:
      SLONG TooExpensive=0;
      if (ObjectType==1)
      {
         SLONG Costs1 = Ticketpreis;
         SLONG Costs2 = CalculateFlightCost (VonCity, NachCity, 800, 800, -1)*3/180*2*3;

         //Computerspieler erhalten Ihre 10% Kerosinrabatt hier, statt im Araber-Menü
         if (qPlayer.Owner==1) Costs2=Costs2*9/10;

         if (Costs1<Costs2/2) Add+=10;
         if (Costs1>Costs2) Add-=10;
         if (Costs1>Costs2+Costs2/2) Add-=10;
         if (Costs1>Costs2+Costs2) Add-=10;

         if (TooExpensive>100) TooExpensive=100;
      }

      //NetGenericAsync (10000+ObjectId+Sim.Date*100, Add);

      qPlayerX.Image+=Add/10;
      if (ObjectType==1 && SLONG(qPlayer.RentRouten.RentRouten[(SLONG)Routen(ObjectId)].Image)+Add/10>0)
      {
         qPlayer.RentRouten.RentRouten[(SLONG)Routen(ObjectId)].Image+=Add/10;

         Limit ((UBYTE)0, qPlayer.RentRouten.RentRouten[(SLONG)Routen(ObjectId)].Image, (UBYTE)100);
      }
      Limit (SLONG(-1000), qPlayerX.Image, SLONG(1000));
   }

   if (ObjectType==1 || ObjectType==2)
   {
      Sim.Players.Players[PlayerNum].NumPassengers+=Passagiere;
   }

   //Flugzeugabnutzung verbuchen:
   Plane->Zustand = UBYTE(Plane->Zustand-(1+Cities.CalcDistance (VonCity, NachCity)*10/40040174)*(2015-Plane->Baujahr)*BadKerosinFaktor/15);
   if (Plane->Zustand>200) Plane->Zustand=0;
}

//--------------------------------------------------------------------------------------------
//Flug wurde geändert:
//--------------------------------------------------------------------------------------------
void CFlugplanEintrag::FlightChanged (void)
{
   HoursBefore = UBYTE((Startdate-Sim.Date)*24+(Startzeit-Sim.GetHour()));
}

//--------------------------------------------------------------------------------------------
//Sagt, wieviel der Flug bringen wird:
//--------------------------------------------------------------------------------------------
SLONG CFlugplanEintrag::GetEinnahmen (SLONG PlayerNum, const CPlane &qPlane)
{
   switch (ObjectType)
   {
      //Route:
      case 1:
         return (Ticketpreis*Passagiere + TicketpreisFC*PassagiereFC);
         break;

      //Auftrag:
      case 2:
         return (Sim.Players.Players[(SLONG)PlayerNum].Auftraege[ObjectId].Praemie);
         break;

      //Leerflug:
      case 3:
         return (qPlane.ptPassagiere*Cities.CalcDistance (VonCity, NachCity)/1000/40);
         break;

      //Frachtauftrag:
      case 4:
         return (Sim.Players.Players[(SLONG)PlayerNum].Frachten[ObjectId].Praemie);
         break;

      default: //Eigentlich unmöglich:
         return (0);
   }

   return (0);
}

//--------------------------------------------------------------------------------------------
//Sagt, wieviel der Flug kosten wird:
//--------------------------------------------------------------------------------------------
SLONG CFlugplanEintrag::GetAusgaben (SLONG PlayerNum, const CPlane &qPlane)
{
   return (CalculateFlightCost (
         VonCity,
         NachCity,
         qPlane.ptVerbrauch,
         qPlane.ptGeschwindigkeit,
         PlayerNum));
}

//--------------------------------------------------------------------------------------------
//Sagt, wieviel der Flug kosten wird und verbucht das Kerosin:
//--------------------------------------------------------------------------------------------
SLONG CFlugplanEintrag::GetRealAusgaben (SLONG PlayerNum, const CPlane &qPlane, CString Name)
{
   return (CalculateRealFlightCost (
         VonCity,
         NachCity,
         qPlane.ptVerbrauch,
         qPlane.ptGeschwindigkeit,
         PlayerNum,
         Name));
}

//--------------------------------------------------------------------------------------------
//Speichert ein CGate-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CGate &Gate)
{
   File << Gate.Nummer << Gate.Miete;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt ein CGate-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CGate &Gate)
{
   File >> Gate.Nummer >> Gate.Miete;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Speichert ein CGates-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CGates &Gates)
{
   File << Gates.Gates;
   File.Write ((unsigned char*)Gates.Auslastung, 24*7);
   File << Gates.NumRented;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt ein CGates-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CGates &Gates)
{
   File >> Gates.Gates;
   File.Read (Gates.Auslastung, 24*7);
   File >> Gates.NumRented;

   return (File);
}