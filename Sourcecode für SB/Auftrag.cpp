//============================================================================================//============================================================================================
// Auftrag.cpp : Routinen für die Aufträge die die Spieler haben
//============================================================================================
#include "Stdafx.h"
#include "AtNet.h"

static const char FileId[] = "Auft";

//Daten des aktuellen Savegames beim laden:
extern SLONG SaveVersion;
extern SLONG SaveVersionSub;

SLONG PlayerMaxPassagiere=90;
SLONG PlayerMaxLength=2000000;
SLONG PlayerMinPassagiere=90;
SLONG PlayerMinLength=2000000;

void CalcPlayerMaximums (bool bForce=false);

//--------------------------------------------------------------------------------------------
//Berechnet, was für Aufträge der Spieler maximal annehmen kann:
//--------------------------------------------------------------------------------------------
void CalcPlayerMaximums (bool bForce)
{
   static SLONG LastHour=-1;

   if (LastHour==Sim.GetHour() && bForce==false) return;

   LastHour=Sim.GetHour();

   PlayerMaxPassagiere = 90;
   PlayerMaxLength     = 2000;
   PlayerMinPassagiere = 2147483647;
   PlayerMinLength     = 2147483647;

   for (long c=0; c<Sim.Players.AnzPlayers; c++)
      if (!Sim.Players.Players[c].IsOut && (Sim.Players.Players[c].Owner==0 || Sim.Players.Players[c].Owner==2))
      {
         PLAYER &qPlayer=Sim.Players.Players[c];

         for (SLONG d=0; d<(SLONG)qPlayer.Planes.AnzEntries(); d++)
            if (qPlayer.Planes.IsInAlbum(d))
            {
               if (qPlayer.Planes[d].MaxPassagiere+qPlayer.Planes[d].MaxPassagiereFC>PlayerMaxPassagiere)
                  PlayerMaxPassagiere=qPlayer.Planes[d].ptPassagiere;

               if (qPlayer.Planes[d].ptReichweite>PlayerMaxLength)
                  PlayerMaxLength=qPlayer.Planes[d].ptReichweite;

               if (qPlayer.Planes[d].MaxPassagiere+qPlayer.Planes[d].MaxPassagiereFC<PlayerMinPassagiere)
                  PlayerMinPassagiere=qPlayer.Planes[d].ptPassagiere;

               if (qPlayer.Planes[d].ptReichweite<PlayerMinLength)
                  PlayerMinLength=qPlayer.Planes[d].ptReichweite;
            }
      }

   PlayerMaxLength *= 1000;
   PlayerMinLength *= 1000;
}

//============================================================================================
//Läßt nötigenfalls den Auftragsberater mit einem Spruch erscheien
//============================================================================================
void PLAYER::CheckAuftragsBerater (const CAuftrag &Auftrag)
{
   if (HasBerater(BERATERTYP_AUFTRAG))
   {
      SLONG Cost=((CalculateFlightCost (Auftrag.VonCity, Auftrag.NachCity, 8000, 700, -1))+99)/100*100;
      SLONG d, Okay;

      for (d=0, Okay=FALSE; d<(SLONG)Planes.AnzEntries(); d++)
         if (Planes.IsInAlbum(d))
         {
            Okay|=Auftrag.FitsInPlane (Planes[d]);
            //Okay|=Auftrag.FitsInPlane (PlaneTypes[(SLONG)Planes[d].TypeId]);
         }

      for (d=0; d<(SLONG)Planes.AnzEntries(); d++)
         if (Planes.IsInAlbum(d))
         {
            CPlane &qPlane = Planes[d];

            for (SLONG e=0; e<qPlane.Flugplan.Flug.AnzEntries(); e++)
               if (qPlane.Flugplan.Flug[e].ObjectType==3)
               {
                  CFlugplanEintrag &qFPE = qPlane.Flugplan.Flug[e];

                  if (qFPE.Startdate>Sim.Date || (qFPE.Startdate==Sim.Date && qFPE.Startzeit>Sim.GetHour()+1))
                  if (Auftrag.VonCity==qFPE.VonCity && Auftrag.NachCity==qFPE.NachCity &&
                      Auftrag.Date<=qFPE.Startdate  && Auftrag.BisDate>=qFPE.Startdate &&
                      SLONG(Auftrag.Personen)<=qPlane.MaxPassagiere+qPlane.MaxPassagiereFC)
                  {
                     //Auftrag ersetzt Autoflug:
                     Messages.AddMessage (BERATERTYP_AUFTRAG, bprintf (StandardTexte.GetS (TOKEN_ADVICE, 7013), qPlane.Name), MESSAGE_COMMENT, SMILEY_GREAT);
                     return;
                  }
               }
         }

      if (Okay==0)
      {
         //Kein Flugzeug, was die Entfernung schafft:
         Messages.AddMessage (BERATERTYP_AUFTRAG, StandardTexte.GetS (TOKEN_ADVICE, 7010), MESSAGE_COMMENT, SMILEY_BAD);
      }
      else
      {
         for (d=0, Okay=FALSE; d<(SLONG)Planes.AnzEntries(); d++)
            if (Planes.IsInAlbum(d))
            {
               Okay|=(SLONG(Auftrag.Personen)<=Planes[d].MaxPassagiere+Planes[d].MaxPassagiereFC);
            }

         if (Okay==0)
         {
            //Kein Flugzeug, was so viele Personen schafft:
            Messages.AddMessage (BERATERTYP_AUFTRAG, StandardTexte.GetS (TOKEN_ADVICE, 7011), MESSAGE_COMMENT, SMILEY_BAD);
         }
         else
         {
            for (d=0, Okay=FALSE; d<(SLONG)Planes.AnzEntries(); d++)
               if (Planes.IsInAlbum(d))
               {
                  Okay|=( (SLONG(Auftrag.Personen)<=Planes[d].MaxPassagiere+Planes[d].MaxPassagiereFC) &&
                          Auftrag.FitsInPlane (Planes[d]) );
                          //Auftrag.FitsInPlane (PlaneTypes[(SLONG)Planes[d].TypeId]) );
               }

            if (Okay==0)
            {
               //Kein Flugzeug, was so viele Personen schafft:
               Messages.AddMessage (BERATERTYP_AUFTRAG, StandardTexte.GetS (TOKEN_ADVICE, 7012), MESSAGE_COMMENT, SMILEY_BAD);
            }
            else
            {
               SLONG Cost=((CalculateFlightCost (Auftrag.VonCity, Auftrag.NachCity, 8000, 700, -1))+99)/100*100;

               if (Auftrag.Strafe==0)
                  Messages.AddMessage (BERATERTYP_AUFTRAG, StandardTexte.GetS (TOKEN_ADVICE, 7000), MESSAGE_COMMENT, SMILEY_NEUTRAL);
               else if (Cost <= Auftrag.Praemie*5/10)
                  Messages.AddMessage (BERATERTYP_AUFTRAG, StandardTexte.GetS (TOKEN_ADVICE, 7001), MESSAGE_COMMENT, SMILEY_GREAT);
               else if (Cost <= Auftrag.Praemie*9/10)
                  Messages.AddMessage (BERATERTYP_AUFTRAG, StandardTexte.GetS (TOKEN_ADVICE, 7002), MESSAGE_COMMENT, SMILEY_GOOD);
               else if (Cost <= Auftrag.Praemie*14/10)
                  Messages.AddMessage (BERATERTYP_AUFTRAG, StandardTexte.GetS (TOKEN_ADVICE, 7003), MESSAGE_COMMENT, SMILEY_NEUTRAL);
               else
                  Messages.AddMessage (BERATERTYP_AUFTRAG, StandardTexte.GetS (TOKEN_ADVICE, 7004), MESSAGE_COMMENT, SMILEY_BAD);
            }
         }
      }
   }
}

//============================================================================================
//CAuftrag::
//============================================================================================
//Konstruktor:
//============================================================================================
CAuftrag::CAuftrag (ULONG VonCity, ULONG NachCity, ULONG Personen, UWORD Date, SLONG Praemie, SLONG Strafe)
{
   CAuftrag::VonCity      = VonCity;
   CAuftrag::NachCity     = NachCity;
   CAuftrag::Personen     = Personen;
   CAuftrag::Date         = Date;
   CAuftrag::InPlan       = 0;
   CAuftrag::Praemie      = Praemie;
   CAuftrag::Strafe       = Strafe;
   CAuftrag::bUhrigFlight = FALSE;
}

//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
CAuftrag::CAuftrag (ULONG VonCity, ULONG NachCity, ULONG Personen, UWORD Date)
{
   CAuftrag::VonCity      = VonCity;
   CAuftrag::NachCity     = NachCity;
   CAuftrag::Personen     = Personen;
   CAuftrag::Date         = Date;
   CAuftrag::InPlan       = 0;
   CAuftrag::Praemie      = 100;          //==>+<==
   CAuftrag::Strafe       = 100;
   CAuftrag::bUhrigFlight = FALSE;
}

//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
CAuftrag::CAuftrag (char *VonCity, char *NachCity, ULONG Personen, UWORD Date, SLONG Praemie, SLONG Strafe)
{
   CAuftrag::VonCity      = Cities.GetIdFromName(VonCity);
   CAuftrag::NachCity     = Cities.GetIdFromName(NachCity);
   CAuftrag::Personen     = Personen;
   CAuftrag::Date         = Date;
   CAuftrag::InPlan       = 0;
   CAuftrag::Praemie      = Praemie;
   CAuftrag::Strafe       = Strafe;
   CAuftrag::bUhrigFlight = FALSE;
}

//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
CAuftrag::CAuftrag (char *VonCity, char *NachCity, ULONG Personen, UWORD Date)
{
   CAuftrag::VonCity      = Cities.GetIdFromName(VonCity);
   CAuftrag::NachCity     = Cities.GetIdFromName(NachCity);
   CAuftrag::Personen     = Personen;
   CAuftrag::Date         = Date;
   CAuftrag::InPlan       = 0;
   CAuftrag::Praemie      = 100;          //==>+<==
   CAuftrag::Strafe       = 100;
   CAuftrag::bUhrigFlight = FALSE;
}

//--------------------------------------------------------------------------------------------
//Legt Städte fest: (AreaType: 0=Europa-Europa, 1=Region-gleicher Region, 2=alles
//--------------------------------------------------------------------------------------------
void CAuftrag::RandomCities (SLONG AreaType, SLONG HomeCity, TEAKRAND *pRandom)
{
   SLONG TimeOut=0;

   do
   {
      switch (AreaType)
      {
         //Region-Gleiche Region:
         case 0:
            {
               SLONG Area = 1+pRandom->Rand(4);

               VonCity = Cities.GetIdFromIndex(Cities.GetRandomUsedIndex(Area, pRandom));
               NachCity = Cities.GetIdFromIndex(Cities.GetRandomUsedIndex(Area, pRandom));
            }
            break;

         //Europa:
         case 1:
            VonCity = Cities.GetIdFromIndex(Cities.GetRandomUsedIndex(Cities[Sim.HomeAirportId].Areacode, pRandom));
            NachCity = Cities.GetIdFromIndex(Cities.GetRandomUsedIndex(Cities[Sim.HomeAirportId].Areacode, pRandom));
            switch (pRandom->Rand(2))
            {
               case 0: VonCity = HomeCity; break;
               case 1: NachCity = HomeCity; break;
            }
            break;

         //Irgendwas:
         case 2:
            VonCity = Cities.GetIdFromIndex(Cities.GetRandomUsedIndex(pRandom));
            NachCity = Cities.GetIdFromIndex(Cities.GetRandomUsedIndex(pRandom));
            break;

         //Die ersten Tage:
         case 4:
            VonCity = Cities.GetIdFromIndex(Cities.GetRandomUsedIndex(Cities[Sim.HomeAirportId].Areacode, pRandom));
            NachCity = Cities.GetIdFromIndex(Cities.GetRandomUsedIndex(Cities[Sim.HomeAirportId].Areacode, pRandom));
            switch (pRandom->Rand(2))
            {
               case 0: VonCity = HomeCity; break;
               case 1: NachCity = HomeCity; break;
            }
            break;
      }

      TimeOut++;

      if (VonCity<0x1000000)  VonCity  = Cities.GetIdFromIndex(VonCity);
      if (NachCity<0x1000000) NachCity = Cities.GetIdFromIndex(NachCity);

      if (TimeOut>300 && VonCity!=NachCity) break;
   }
   while (VonCity==NachCity || (AreaType==4 && Cities.CalcDistance(VonCity, NachCity)>10000000));
}

//--------------------------------------------------------------------------------------------
//Fügt einen neuen Auftrag ein:
//--------------------------------------------------------------------------------------------
void CAuftrag::RefillForLastMinute (SLONG AreaType, TEAKRAND *pRandom)
{
   SLONG TimeOut=0;

   //NetGenericSync (2000, AreaType);
   //NetGenericSync (2001, pRandom->GetSeed());

too_large:
   RandomCities (AreaType, Sim.HomeAirportId, pRandom);

   Date         = UWORD(Sim.Date);
   BisDate      = Date;
   InPlan       = 0;

   //Kopie dieser Formel auch bei Last-Minute
   Praemie      = ((CalculateFlightCost (VonCity, NachCity, 8000, 700, -1))+99)/100*130;
   Strafe       = Praemie/2*100/100;

   Praemie=Praemie*5/4;  //Last-Minute Zuschlag

   SLONG Type = pRandom->Rand(100);

   //Typ A = Normal, Gewinn möglich, etwas Strafe
   if (Type>=0 && Type<50)
   {
   }
   //Typ B = Hoffmann, Gewinn möglich, keine Strafe
   else if (Type>=50 && Type<60)
   {
      Date++; BisDate++;
   }
   //Typ C = Zeit knapp, viel Gewinn, viel Strafe
   else if (Type>=60 && Type<80)
   {
      Praemie *= 2;
      Strafe  = Praemie * 4;
   }
   //Typ D = Betrug, kein Gewinn möglich, etwas Strafe
   else if (Type>=80 && Type<95)
   {
      Praemie /= 2;
   }
   //Typ E = Glücksfall, viel Gewinn, keine Strafe
   else if (Type>=95 && Type<100)
   {
      Praemie *= 2;
      Strafe  = 0;
   }

   Type = pRandom->Rand (100);

   if (Type==0)
   {
      Personen = 1;
      Praemie  = Praemie*4;
      Strafe   = Praemie*4;
   }
   else if (Type<15 || (Sim.Date<4 && Type<30) || (Sim.Date<8 && Type<20) || (Sim.Difficulty==DIFF_TUTORIAL && Type<70))
   {
      Personen = 90;
      Praemie  = Praemie*3/4;
   }
   else if (Type<40 || Sim.Difficulty==DIFF_TUTORIAL)
   {
      Personen = 180;
      Praemie  = Praemie;
   }
   else if (Type<70)
   {
      Personen = 280;
      Praemie  = Praemie*5/4;
   }
   else if (Type<90)
   {
      Personen = 340;
      Praemie  = Praemie*6/4;
   }
   else
   {
      Personen = 430;
      Praemie  = Praemie*7/4;
   }

   if (AreaType==1) Praemie=Praemie*3/2;
   if (AreaType==2) Praemie=Praemie*8/5;

   if ((SLONG(Personen)>PlayerMaxPassagiere || Cities.CalcDistance(VonCity, NachCity)>PlayerMaxLength) && TimeOut++<100)
      goto too_large;
}

//--------------------------------------------------------------------------------------------
//Fügt einen neuen Auftrag ein:
//--------------------------------------------------------------------------------------------
void CAuftrag::RefillForReisebuero (SLONG AreaType, TEAKRAND *pRandom)
{
   SLONG TimeOut=0;

   //NetGenericSync (3000, AreaType);
   //NetGenericSync (3001, pRandom->GetSeed());

too_large:
   RandomCities (AreaType, Sim.HomeAirportId, pRandom);

   Personen     = 180;
   Date         = UWORD(Sim.Date+1+pRandom->Rand(3));
   BisDate      = Date;
   InPlan       = 0;

   //Kopie dieser Formel auch bei Last-Minute
   Praemie      = ((CalculateFlightCost (VonCity, NachCity, 8000, 700, -1))+99)/100*105;
   Strafe       = Praemie/2*100/100;

   if (pRandom->Rand (5)==4) BisDate+=((UWORD)(pRandom->Rand(5)));

   SLONG Type = pRandom->Rand (100);

   //Typ A = Normal, Gewinn möglich, etwas Strafe
   if (Type>=0 && Type<50)
   {
   }
   //Typ B = Hoffmann, Gewinn möglich
   else if (Type>=50 && Type<60)
   {
      Date    = UWORD(Sim.Date);
      BisDate = UWORD(Sim.Date+4+pRandom->Rand(3));
   }
   //Typ C = Zeit knapp, viel Gewinn, viel Strafe
   else if (Type>=70 && Type<80)
   {
      Praemie *= 2;
      Strafe  = Praemie * 2;
      BisDate = Date = UWORD(Sim.Date+1);
   }
   //Typ D = Betrug, kein Gewinn möglich, etwas Strafe
   else if (Type>=80 && Type<95)
   {
      Praemie /= 2;
   }
   //Typ E = Glücksfall, viel Gewinn, keine Strafe
   else if (Type>=95 && Type<100)
   {
      Praemie *= 2;
      Strafe  = 0;
   }

   Type = pRandom->Rand (100);

   if (Type==0)
   {
      Personen = 2;
      Praemie  = Praemie*4;
      Strafe   = Praemie*4;
   }
   else if (Type<15 || (Sim.Date<4 && Type<30) || (Sim.Date<8 && Type<20) || (Sim.Difficulty==DIFF_TUTORIAL && Type<70))
   {
      Personen = 90;
      Praemie  = Praemie*3/4;
   }
   else if (Type<40 || Sim.Difficulty==DIFF_TUTORIAL)
   {
      Personen = 180;
      Praemie  = Praemie;
   }
   else if (Type<70)
   {
      Personen = 280;
      Praemie  = Praemie*5/4;
   }
   else if (Type<90)
   {
      Personen = 340;
      Praemie  = Praemie*6/4;
   }
   else
   {
      Personen = 430;
      Praemie  = Praemie*7/4;
   }

   if (AreaType==1) Praemie=Praemie*3/2;
   if (AreaType==2) Praemie=Praemie*8/5;
   if (Date!=BisDate)
   {
      Date=(UWORD)Sim.Date;
      Praemie=Praemie*4/5;
   }

   if ((SLONG(Personen)>PlayerMaxPassagiere || Cities.CalcDistance(VonCity, NachCity)>PlayerMaxLength) && TimeOut++<100)
      goto too_large;
}

//--------------------------------------------------------------------------------------------
//Fügt einen neuen Auftrag für den Spielbeginn ein:
//--------------------------------------------------------------------------------------------
void CAuftrag::RefillForBegin (SLONG AreaType, TEAKRAND *pRandom)
{
   SLONG TimeOut=0;

   //NetGenericSync (4000, AreaType);
   //NetGenericSync (4001, pRandom->GetSeed());

too_large:
   do
   {
      RandomCities (AreaType, Sim.HomeAirportId, pRandom);

      VonCity  = Sim.HomeAirportId;
   }
   while (VonCity==NachCity || Cities.CalcDistance(VonCity, NachCity)>5000000);

   Date         = UWORD(Sim.Date);
   BisDate      = UWORD(Sim.Date);
   Personen     = 90;
   InPlan       = 0;
   Praemie      = ((CalculateFlightCost (VonCity, NachCity, 8000, 700, -1))+99)/100*100;
   Strafe       = Praemie/2*100/100;

   SLONG Type = pRandom->Rand (100);

   //Typ A = Normal, Gewinn möglich, etwas Strafe
   if (Type>=0 && Type<50)
   {
   }
   //Typ B = Hoffmann, Gewinn möglich, keine Strafe
   else if (Type>=50 && Type<60)
   {
      BisDate= UWORD(Sim.Date+6);
   }
   //Typ C = Zeit knapp, viel Gewinn, viel Strafe
   else if (Type>=70 && Type<95)
   {
      Praemie *= 2;
      Strafe  = Praemie * 2;
      BisDate = UWORD(Sim.Date+1);
   }
   //Typ E = Glücksfall, viel Gewinn, keine Strafe
   else if (Type>=95 && Type<100)
   {
      Praemie *= 2;
      Strafe  = 0;
   }

   if (AreaType==1) Praemie=Praemie*3/2;
   if (AreaType==2) Praemie=Praemie*8/5;
   if (Date!=BisDate) Praemie=Praemie*4/5;

   TimeOut++;

   if ((SLONG(Personen)>PlayerMaxPassagiere || Cities.CalcDistance(VonCity, NachCity)>PlayerMaxLength) && TimeOut<80)
      goto too_large;

   if ((SLONG(Personen)>PlayerMinPassagiere || Cities.CalcDistance(VonCity, NachCity)>PlayerMinLength) && TimeOut<60)
      goto too_large;

   if ((SLONG(Personen)>PlayerMaxPassagiere || Cities.CalcDistance(VonCity, NachCity)>PlayerMaxLength))
   {
      Strafe  = 0;
      Praemie = Praemie * 2;
      Date    = (UWORD)Sim.Date;
      BisDate = Date+5;
   }
}

//--------------------------------------------------------------------------------------------
//Fügt einen neuen Auftrag für Uhrig ein:
//--------------------------------------------------------------------------------------------
void CAuftrag::RefillForUhrig (SLONG AreaType, TEAKRAND *pRandom)
{
   SLONG TimeOut=0;

   //NetGenericSync (5000, AreaType);
   //NetGenericSync (5001, pRandom->GetSeed());

too_large:
   RandomCities (AreaType, Sim.HomeAirportId, pRandom);

   Personen     = 180;
   Date         = UWORD(Sim.Date+1+pRandom->Rand(3));
   BisDate      = Date;
   InPlan       = 0;

   //Kopie dieser Formel auch bei Last-Minute
   Praemie      = ((CalculateFlightCost (VonCity, NachCity, 8000, 700, -1))+99)/100*115;
   Strafe       = Praemie/2*100/100*2;

   if (pRandom->Rand (5)==4) BisDate+=((UWORD)(pRandom->Rand(5)));

   SLONG Type = pRandom->Rand (100);

   //Typ A = Normal, Gewinn möglich, etwas Strafe
   if (Type>=0 && Type<50)
   {
   }
   //Typ B = Hoffmann, Gewinn möglich
   else if (Type>=50 && Type<60)
   {
      Date    = UWORD(Sim.Date);
      BisDate = UWORD(Sim.Date+4+pRandom->Rand(3));
   }
   //Typ C = Zeit knapp, viel Gewinn, viel Strafe
   else if (Type>=70 && Type<80)
   {
      Praemie *= 2;
      Strafe  = Praemie * 2;
      BisDate = Date = UWORD(Sim.Date+1);
   }
   //Typ D = Betrug, kein Gewinn möglich, etwas Strafe
   else if (Type>=80 && Type<95)
   {
      Praemie /= 2;
   }
   //Typ E = Glücksfall, viel Gewinn, keine Strafe
   else if (Type>=95 && Type<100)
   {
      Praemie *= 2;
      Strafe  = 0;
   }

   Type = pRandom->Rand (100);

   if (Type==0)
   {
      Personen = 2;
      Praemie  = Praemie*4;
      Strafe   = Praemie*4;
   }
   else if (Type<15 || (Sim.Date<4 && Type<30) || (Sim.Date<8 && Type<20) || (Sim.Difficulty==DIFF_TUTORIAL && Type<70))
   {
      Personen = 90;
      Praemie  = Praemie*3/4;
   }
   else if (Type<40 || Sim.Difficulty==DIFF_TUTORIAL)
   {
      Personen = 180;
      Praemie  = Praemie;
   }
   else if (Type<70)
   {
      Personen = 280;
      Praemie  = Praemie*5/4;
   }
   else if (Type<90)
   {
      Personen = 340;
      Praemie  = Praemie*6/4;
   }
   else
   {
      Personen = 430;
      Praemie  = Praemie*7/4;
   }

   if (AreaType==1) Praemie=Praemie*3/2;
   if (AreaType==2) Praemie=Praemie*8/5;
   if (Date!=BisDate)
   {
      Date=(UWORD)Sim.Date;
      Praemie=Praemie*4/5;
   }

   if ((SLONG(Personen)>PlayerMaxPassagiere || Cities.CalcDistance(VonCity, NachCity)>PlayerMaxLength) && TimeOut++<100)
      goto too_large;
}

//--------------------------------------------------------------------------------------------
//Fügt einen neuen Auftrag für den Spielbeginn ein:
//--------------------------------------------------------------------------------------------
void CAuftrag::RefillForAusland (SLONG AreaType, SLONG CityNum, TEAKRAND *pRandom)
{
   SLONG TimeOut=0;

   TEAKRAND localRand;
   localRand.SRand (pRandom->Rand());

   if (CityNum<0x1000000) CityNum=Cities.GetIdFromIndex(CityNum);

too_large:

   do
   {
      VonCity = Cities.GetIdFromIndex(Cities.GetRandomUsedIndex(&localRand));
      NachCity = Cities.GetIdFromIndex(Cities.GetRandomUsedIndex(&localRand));
      switch (localRand.Rand(2))
      {
         case 0: VonCity = CityNum; break;
         case 1: NachCity = CityNum; break;
      }
   }
   while (VonCity==NachCity || (AreaType==4 && Cities.CalcDistance(VonCity, NachCity)>10000000));

   Personen     = 180;
   Date         = UWORD(Sim.Date+1+localRand.Rand(3));
   BisDate      = Date;
   InPlan       = 0;

   //Kopie dieser Formel auch bei Last-Minute
   Praemie      = ((CalculateFlightCost (VonCity, NachCity, 8000, 700, -1))+99)/100*120;
   Strafe       = Praemie/2*100/100;

   if (localRand.Rand (5)==4) BisDate+=((UWORD)(localRand.Rand(5)));

   SLONG Type = localRand.Rand (100);

   //Typ A = Normal, Gewinn möglich, etwas Strafe
   if (Type>=0 && Type<50)
   {
   }
   //Typ B = Hoffmann, Gewinn möglich
   else if (Type>=50 && Type<65)
   {
      Date    = UWORD(Sim.Date);
      BisDate = UWORD(Sim.Date+4+localRand.Rand(3));
   }
   //Typ C = Zeit knapp, viel Gewinn, viel Strafe
   else if (Type>=65 && Type<80)
   {
      Praemie *= 2;
      Strafe  = Praemie * 2;
      BisDate = Date = UWORD(Sim.Date+1);
   }
   //Typ D = Betrug, kein Gewinn möglich, etwas Strafe
   else if (Type>=80 && Type<95)
   {
      Praemie /= 2;
   }
   //Typ E = Glücksfall, viel Gewinn, keine Strafe
   else if (Type>=95 && Type<100)
   {
      Praemie *= 2;
      Strafe  = 0;
   }

   //Type = pRandom->Rand (100);
   Type = localRand.Rand (100);

   if (Type==0)
   {
      Personen = 2;
      Praemie  = Praemie*4;
      Strafe   = Praemie*4;
   }
   else if (Type<15 || (Sim.Date<4 && Type<30) || (Sim.Date<8 && Type<20) || (Sim.Difficulty==DIFF_TUTORIAL && Type<70))
   {
      Personen = 90;
      Praemie  = Praemie*3/4;
   }
   else if (Type<40 || Sim.Difficulty==DIFF_TUTORIAL)
   {
      Personen = 180;
      Praemie  = Praemie;
   }
   else if (Type<70)
   {
      Personen = 280;
      Praemie  = Praemie*5/4;
   }
   else if (Type<90)
   {
      Personen = 340;
      Praemie  = Praemie*6/4;
   }
   else
   {
      Personen = 430;
      Praemie  = Praemie*7/4;
   }

   if (AreaType==1) Praemie=Praemie*3/2;
   if (AreaType==2) Praemie=Praemie*8/5;
   if (Date!=BisDate)
   {
      Date    = (UWORD)Sim.Date;
      Praemie = Praemie*4/5;
   }

   if ((SLONG(Personen)>PlayerMaxPassagiere || Cities.CalcDistance(VonCity, NachCity)>PlayerMaxLength) && TimeOut++<100)
      goto too_large;
}

//--------------------------------------------------------------------------------------------
//Das Raster zum ausgrauen der ungültigen Tage zeichnen:
//--------------------------------------------------------------------------------------------
void CAuftrag::BlitGridAt (SBBM *pBitmap, XY Offset, BOOL Tagesansicht, SLONG Page)
{
   SLONG c;

   if (!Tagesansicht)
   {
      for (c=0; c<7; c++)
         if (Sim.Date+c<Date || Sim.Date+c>BisDate)
            pBitmap->BlitFromT (FlugplanBms[51], Offset.x, Offset.y+FlugplanBms[51].Size.y*c);
   }
}

//--------------------------------------------------------------------------------------------
//Passt der Auftrag in ein Flugzeug von der Sorte?
//--------------------------------------------------------------------------------------------
BOOL CAuftrag::FitsInPlane (const CPlane &Plane) const
{
   if (Cities.CalcDistance (VonCity, NachCity)>Plane.ptReichweite*1000)
      return (FALSE);
   else
   {
      //if ((Cities.CalcDistance (VonCity, NachCity)/PlaneType.Geschwindigkeit+999)/1000+1+2>=24)
      //if (Cities.CalcFlugdauer (VonCity, NachCity, PlaneType.Geschwindigkeit)>=24)
      if (Cities.CalcFlugdauer (VonCity, NachCity, Plane.ptGeschwindigkeit)>=24)
         return (FALSE);
   }

   return (TRUE);
}

//--------------------------------------------------------------------------------------------
//Ein CAuftrag-Datum speichern:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CAuftrag &Auftrag)
{
   File << Auftrag.VonCity << Auftrag.NachCity << Auftrag.Personen << Auftrag.Date
        << Auftrag.InPlan  << Auftrag.Okay     << Auftrag.Praemie  << Auftrag.Strafe
        << Auftrag.BisDate;

   if (SaveVersionSub>=100)
   {
      File << Auftrag.bUhrigFlight;
   }

   return (File);
}

//--------------------------------------------------------------------------------------------
//Ein CAuftrag-Datum laden:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CAuftrag &Auftrag)
{
   File >> Auftrag.VonCity >> Auftrag.NachCity >> Auftrag.Personen >> Auftrag.Date
        >> Auftrag.InPlan  >> Auftrag.Okay     >> Auftrag.Praemie  >> Auftrag.Strafe
        >> Auftrag.BisDate;

   if (SaveVersionSub>=100)
   {
      File >> Auftrag.bUhrigFlight;
   }
   else
   {
      Auftrag.bUhrigFlight = FALSE;
   }

   return (File);
}

//============================================================================================
//CAuftraege::
//============================================================================================
//Fügt eine Reihe von neuen Aufträgen ein:
//============================================================================================
void CAuftraege::FillForLastMinute (void)
{
   SLONG c;

   CalcPlayerMaximums ();

   Auftraege.ReSize (6);  //ex:10

   for (c=0; c<Auftraege.AnzEntries(); c++)
      Auftraege[c].RefillForLastMinute (c/2, &Random);

   if (Sim.Difficulty==DIFF_ATFS10 && Sim.Date>=20 && Sim.Date<=30)
      for (c=0; c<Auftraege.AnzEntries(); c++)
         Auftraege[c].Praemie=0;
}

//--------------------------------------------------------------------------------------------
//Fügt einen neuen Auftrag ein:
//--------------------------------------------------------------------------------------------
void CAuftraege::RefillForLastMinute (SLONG Minimum)
{
   SLONG c;
   SLONG Anz = min (Auftraege.AnzEntries(), Sim.TickLastMinuteRefill);

   NetGenericSync (7000, Minimum);
   NetGenericSync (7001, Sim.TickLastMinuteRefill);
   NetGenericSync (7002, Random.GetSeed());

   CalcPlayerMaximums ();

   Auftraege.ReSize (6); //ex:10

   for (c=0; c<Auftraege.AnzEntries() && Anz>0; c++)
      if (Auftraege[c].Praemie==0)
      {
         Auftraege[c].RefillForLastMinute (c/2, &Random);
         Anz--;
      }

   for (c=0; c<Auftraege.AnzEntries() && Anz>0; c++)
      if (Auftraege[c].Praemie!=0) Minimum--;

   for (c=0; c<Auftraege.AnzEntries() && Anz>0; c++)
      if (Auftraege[c].Praemie==0 && Minimum>0)
      {
         Auftraege[c].RefillForLastMinute (c/2, &Random);
         Minimum--;
      }

   Sim.TickLastMinuteRefill = 0;

   if (Sim.Difficulty==DIFF_ATFS10 && Sim.Date>=20 && Sim.Date<=30)
      for (c=0; c<Auftraege.AnzEntries(); c++)
         Auftraege[c].Praemie=0;
}

//--------------------------------------------------------------------------------------------
//Fügt eine Reihe von neuen Aufträgen ein:
//--------------------------------------------------------------------------------------------
void CAuftraege::FillForReisebuero (void)
{
   SLONG c;

   CalcPlayerMaximums ();

   Auftraege.ReSize (6);

   for (c=0; c<Auftraege.AnzEntries(); c++)
   {
           if (Sim.Date<5 && c<5)  Auftraege[c].RefillForAusland (4, Sim.HomeAirportId, &Random);
      else if (Sim.Date<10 && c<3) Auftraege[c].RefillForAusland (4, Sim.HomeAirportId, &Random);
      else                         Auftraege[c].RefillForAusland (c/2, Sim.HomeAirportId, &Random);
   }

   if (Sim.Difficulty==DIFF_ATFS10 && Sim.Date>=20 && Sim.Date<=30)
      for (c=0; c<Auftraege.AnzEntries(); c++)
         Auftraege[c].Praemie=0;
}

//--------------------------------------------------------------------------------------------
//Fügt einen neuen Auftrag ein:
//--------------------------------------------------------------------------------------------
void CAuftraege::RefillForReisebuero (SLONG Minimum)
{
   SLONG c;
   SLONG Anz = min (Auftraege.AnzEntries(), Sim.TickReisebueroRefill);

   //NetGenericSync (8000, Minimum);
   //NetGenericSync (8001, Random.GetSeed());

   CalcPlayerMaximums ();

   Auftraege.ReSize (6);

   for (c=0; c<Auftraege.AnzEntries() && Anz>0; c++)
      if (Auftraege[c].Praemie==0)
      {
              if (Sim.Date<5 && c<5)  Auftraege[c].RefillForAusland (4, Sim.HomeAirportId, &Random);
         else if (Sim.Date<10 && c<3) Auftraege[c].RefillForAusland (4, Sim.HomeAirportId, &Random);
         else                         Auftraege[c].RefillForAusland (c/2, Sim.HomeAirportId, &Random);

         Anz--;
      }

   for (c=0; c<Auftraege.AnzEntries() && Anz>0; c++)
      if (Auftraege[c].Praemie!=0) Minimum--;

   for (c=0; c<Auftraege.AnzEntries() && Anz>0; c++)
      if (Auftraege[c].Praemie==0 && Minimum>0)
      {
              if (Sim.Date<5 && c<5)  Auftraege[c].RefillForAusland (4, Sim.HomeAirportId, &Random);
         else if (Sim.Date<10 && c<3) Auftraege[c].RefillForAusland (4, Sim.HomeAirportId, &Random);
         else                         Auftraege[c].RefillForAusland (c/2, Sim.HomeAirportId, &Random);

         Minimum--;
      }

   Sim.TickReisebueroRefill = 0;

   if (Sim.Difficulty==DIFF_ATFS10 && Sim.Date>=20 && Sim.Date<=30)
      for (c=0; c<Auftraege.AnzEntries(); c++)
         Auftraege[c].Praemie=0;
}

//--------------------------------------------------------------------------------------------
//Fügt eine Reihe von neuen Aufträgen ein:
//--------------------------------------------------------------------------------------------
void CAuftraege::FillForAusland (SLONG CityNum)
{
   SLONG c;

   CalcPlayerMaximums ();

   Auftraege.ReSize (6);  //ex:10
   IsInAlbum(0xffffffff); //Refresh erzwingen

   for (c=0; c<Auftraege.AnzEntries(); c++)
   {
           if (Sim.Date<5 && c<5)  Auftraege[c].RefillForAusland (4, CityNum, &Random);
      else if (Sim.Date<10 && c<3) Auftraege[c].RefillForAusland (4, CityNum, &Random);
      else                         Auftraege[c].RefillForAusland (c/2, CityNum, &Random);
   }
}

//--------------------------------------------------------------------------------------------
//Fügt einen neuen Auftrag ein:
//--------------------------------------------------------------------------------------------
void CAuftraege::RefillForAusland (SLONG CityNum, SLONG Minimum)
{
   SLONG c;
   SLONG Anz = min (Auftraege.AnzEntries(), AuslandsRefill[CityNum]);

   CalcPlayerMaximums ();

   Auftraege.ReSize (6);
   IsInAlbum(0xffffffff); //Refresh erzwingen

   for (c=0; c<Auftraege.AnzEntries() && Anz>0; c++)
      if (Auftraege[c].Praemie==0)
      {
              if (Sim.Date<5 && c<5)  Auftraege[c].RefillForAusland (4, CityNum, &Random);
         else if (Sim.Date<10 && c<3) Auftraege[c].RefillForAusland (4, CityNum, &Random);
         else                         Auftraege[c].RefillForAusland (c/2, CityNum, &Random);

         Anz--;
      }

   for (c=0; c<Auftraege.AnzEntries() && Anz>0; c++)
      if (Auftraege[c].Praemie!=0) Minimum--;

   for (c=0; c<Auftraege.AnzEntries() && Anz>0; c++)
      if (Auftraege[c].Praemie==0 && Minimum>0)
      {
              if (Sim.Date<5 && c<5)  Auftraege[c].RefillForAusland (4, CityNum, &Random);
         else if (Sim.Date<10 && c<3) Auftraege[c].RefillForAusland (4, CityNum, &Random);
         else                         Auftraege[c].RefillForAusland (c/2, CityNum, &Random);

         Minimum--;
      }

   AuslandsRefill[CityNum] = 0;
}

//--------------------------------------------------------------------------------------------
//Returns the number of open Order flights which are due today:
//--------------------------------------------------------------------------------------------
SLONG CAuftraege::GetNumDueToday (void)
{
   SLONG c, Anz=0;

   for (c=0; c<Auftraege.AnzEntries(); c++)
      if (IsInAlbum(c) && Auftraege[c].BisDate>=Sim.Date)
         if (Auftraege[c].InPlan==0 && Auftraege[c].BisDate==Sim.Date)
            Anz++;

   return (Anz);
}

//--------------------------------------------------------------------------------------------
//Returns the number of open Order flights which still need to be done:
//--------------------------------------------------------------------------------------------
SLONG CAuftraege::GetNumOpen (void)
{
   SLONG c, Anz=0;

   for (c=0; c<Auftraege.AnzEntries(); c++)
      if (IsInAlbum(c) && Auftraege[c].BisDate>=Sim.Date)
         if (Auftraege[c].InPlan==0)
            Anz++;

   return (Anz);
}

//--------------------------------------------------------------------------------------------
//Speichert ein CAuftrag Datum:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CAuftraege &Auftraege)
{
   File << Auftraege.Auftraege;
   File << *((ALBUM<CAuftrag>*)&Auftraege);

   if (SaveVersionSub>=100)
   {
      File << Auftraege.Random;
   }

   return (File);
}

//--------------------------------------------------------------------------------------------
//Läd ein CAuftrag Datum:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CAuftraege &Auftraege)
{
   File >> Auftraege.Auftraege;
   File >> *((ALBUM<CAuftrag>*)&Auftraege);

   if (SaveVersionSub>=100)
   {
      File >> Auftraege.Random;
   }
   else
   {
      Auftraege.Random.SRand (0);
   }

   return (File);
}