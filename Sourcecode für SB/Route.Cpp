//============================================================================================
// Route.cpp : Routinen zum verwalten der Flugrouten (CRoute, CRouten)
//============================================================================================
#include "Stdafx.h"

static const char FileId[] = "Rout";

SLONG ReadLine (BUFFER<UBYTE> &Buffer, SLONG BufferStart, char *Line, SLONG LineLength);

//Daten des aktuellen Savegames beim laden:
extern SLONG SaveVersion;
extern SLONG SaveVersionSub;

//============================================================================================
//CRoute::
//============================================================================================
//So viele fliegen (Potentiell) hier
//============================================================================================
SLONG CRoute::AnzPassagiere(void)
{
   SLONG DayFaktor;

        if (Sim.Date<3)  DayFaktor=1000;
   else if (Sim.Date<9)  DayFaktor=900;
   else if (Sim.Date<29) DayFaktor=750;
   else                  DayFaktor=650;

   return ((SLONG)(sqrt (Cities[VonCity].Einwohner * double(Cities[NachCity].Einwohner))*Faktor/DayFaktor/2*3/4));
}

BOOL CRoute::operator > (const CRoute &p) const { return (Cities[VonCity].Name>Cities[p.VonCity].Name); }
BOOL CRoute::operator < (const CRoute &p) const { return (Cities[VonCity].Name<Cities[p.VonCity].Name); }

//--------------------------------------------------------------------------------------------
//Eine CRoute-Objekt speichern:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CRoute &r)
{
   File << r.Ebene << r.VonCity << r.NachCity;
   File << r.Miete << r.Faktor  << r.Bedarf;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Eine CRoute-Objekt laden:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CRoute &r)
{
   File >> r.Ebene >> r.VonCity >> r.NachCity;
   File >> r.Miete >> r.Faktor  >> r.Bedarf;

   return (File);
}

//============================================================================================
//CRouten::
//============================================================================================
//Konstruktor:
//============================================================================================
CRouten::CRouten (const CString &TabFilename) : ALBUM<CRoute> (Routen, "Routen")
{
   //ReInit (TabFilename);
   DebugBreak();
}

//--------------------------------------------------------------------------------------------
//Dateien neu laden:
//--------------------------------------------------------------------------------------------
void CRouten::ReInit (const CString &TabFilename, bool bNoDoublettes)
{
   //CStdioFile    Tab;
   BUFFER<char>  Line(300);
   long          Id, Id2;

   //Load Table header:
   BUFFER<UBYTE> FileData (*LoadCompleteFile (FullFilename (TabFilename, ExcelPath)));
   SLONG         FileP=0;

   //Die erste Zeile einlesen
   FileP=ReadLine (FileData, FileP, Line, 300);

   Routen.ReSize (0);
   IsInAlbum (0x11000000);
   Routen.ReSize (MAX_ROUTES);

   while (1)
   {
      if (FileP>=FileData.AnzEntries()) break;
      FileP=ReadLine (FileData, FileP, Line, 300);

      //if (!Tab.ReadString (Line, 300)) break;
      TeakStrRemoveEndingCodes (Line, "\xd\xa\x1a\r");

      SLONG   HelperEbene = atoi (strtok (Line, TabSeparator));
      CString Helper1     = strtok (NULL, TabSeparator);
      CString Helper2     = strtok (NULL, TabSeparator);
      ULONG   VonCity     = Cities.GetIdFromName ((char*)(LPCTSTR)KorrigiereUmlaute (Helper1));
      ULONG   NachCity    = Cities.GetIdFromName ((char*)(LPCTSTR)KorrigiereUmlaute (Helper2));

      //Looking for doubles (may be turned off for compatibility)
      if (bNoDoublettes)
      {
         for (SLONG c=0; c<(SLONG)AnzEntries(); c++)
            if (IsInAlbum(c) && (*this)[c].VonCity==VonCity && (*this)[c].NachCity==NachCity)
               goto skip_this_city_because_it_exists_twice;
      }

      //Tabellenzeile hinzufügen:
      Id=GetUniqueId();
      (*this)+=Id;

      //SpeedUp durch direkten Zugriff:
      Id=(*this)(Id);
      (*this)[Id].Ebene    = HelperEbene;
      (*this)[Id].VonCity  = VonCity;
      (*this)[Id].NachCity = NachCity;
      (*this)[Id].Miete    = atol (strtok (NULL, TabSeparator));
      (*this)[Id].Faktor   = atof (strtok (NULL, TabSeparator));
      (*this)[Id].Bedarf   = 0;
      (*this)[Id].bNewInDeluxe = (Cities[VonCity].bNewInAddOn==2 || Cities[NachCity].bNewInAddOn==2);

      //Tabellenzeile hinzufügen:
      Id2=GetUniqueId();
      (*this)+=Id2;

      //SpeedUp durch direkten Zugriff:
      Id2=(*this)(Id2);
      (*this)[Id2].Ebene    = (*this)[Id].Ebene;
      (*this)[Id2].VonCity  = (*this)[Id].NachCity;
      (*this)[Id2].NachCity = (*this)[Id].VonCity;
      (*this)[Id2].Miete    = (*this)[Id].Miete;
      (*this)[Id2].Faktor   = (*this)[Id].Faktor;
      (*this)[Id2].Bedarf   = 0;
      (*this)[Id2].bNewInDeluxe = (*this)[Id].bNewInDeluxe;

      skip_this_city_because_it_exists_twice:;
   }

   this->Sort ();
}

//--------------------------------------------------------------------------------------------
//Dateien neu laden:
//--------------------------------------------------------------------------------------------
void CRouten::ReInitExtend (const CString &TabFilename)
{
   //CStdioFile    Tab;
   BUFFER<char>  Line(300);
   long          Id, Id2;
   long          linenumber=0;

   //Load Table header:
   BUFFER<UBYTE> FileData (*LoadCompleteFile (FullFilename (TabFilename, ExcelPath)));
   SLONG         FileP=0;

   //Die erste Zeile einlesen
   FileP=ReadLine (FileData, FileP, Line, 300);

   Routen.ReSize (MAX_ROUTES);
   SLONG NumUsed = (SLONG)GetNumUsed();

   while (1)
   {
      if (FileP>=FileData.AnzEntries()) break;
      FileP=ReadLine (FileData, FileP, Line, 300);

      if (linenumber<NumUsed/2) { linenumber++; continue; }
      linenumber++;

      TeakStrRemoveEndingCodes (Line, "\xd\xa\x1a\r");

      SLONG   HelperEbene = atoi (strtok (Line, TabSeparator));
      CString Helper1     = strtok (NULL, TabSeparator);
      CString Helper2     = strtok (NULL, TabSeparator);
      ULONG   VonCity     = Cities.GetIdFromName ((char*)(LPCTSTR)KorrigiereUmlaute (Helper1));
      ULONG   NachCity    = Cities.GetIdFromName ((char*)(LPCTSTR)KorrigiereUmlaute (Helper2));

      //Tabellenzeile hinzufügen:
      Id=GetUniqueId();
      (*this)+=Id;

      //SpeedUp durch direkten Zugriff:
      Id=(*this)(Id);
      (*this)[Id].Ebene    = HelperEbene;
      (*this)[Id].VonCity  = VonCity;
      (*this)[Id].NachCity = NachCity;
      (*this)[Id].Miete    = atol (strtok (NULL, TabSeparator));
      (*this)[Id].Faktor   = atof (strtok (NULL, TabSeparator));
      (*this)[Id].Bedarf   = 0;
      (*this)[Id].bNewInDeluxe = (Cities[VonCity].bNewInAddOn==2 || Cities[NachCity].bNewInAddOn==2);

      //Tabellenzeile hinzufügen:
      Id2=GetUniqueId();
      (*this)+=Id2;

      //SpeedUp durch direkten Zugriff:
      Id2=(*this)(Id2);
      (*this)[Id2].Ebene    = (*this)[Id].Ebene;
      (*this)[Id2].VonCity  = (*this)[Id].NachCity;
      (*this)[Id2].NachCity = (*this)[Id].VonCity;
      (*this)[Id2].Miete    = (*this)[Id].Miete;
      (*this)[Id2].Faktor   = (*this)[Id].Faktor;
      (*this)[Id2].Bedarf   = 0;
      (*this)[Id2].bNewInDeluxe = (*this)[Id].bNewInDeluxe;
   }

   this->Sort ();
}

//--------------------------------------------------------------------------------------------
//Wie viele Leute warten auf einen Flug?:
//--------------------------------------------------------------------------------------------
void CRouten::NewDay (void)
{
   SLONG c;

   if (Sim.Date==0)
   {
      for (c=0; c<Routen.AnzEntries(); c++)
         if (IsInAlbum(c))
         {
            Routen[c].Bedarf = SLONG(Routen[c].AnzPassagiere()*4.27);
         }
   }
   else
   {
      for (c=0; c<Routen.AnzEntries(); c++)
         if (IsInAlbum(c))
         {
            Routen[c].Bedarf = (Routen[c].Bedarf * 6 + (SLONG(Routen[c].AnzPassagiere()*4.27)))/7;
         }
   }
}

//--------------------------------------------------------------------------------------------
//Speichert ein CRouten Datum:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CRouten &r)
{
   if (SaveVersion==1 && SaveVersionSub<12)
      File << r.Routen;
   else
   {
      File << r.Routen;
      File << *((ALBUM<CRoute>*)&r);
   }

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt ein CRouten Datum:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CRouten &r)
{
   if (SaveVersion==1 && SaveVersionSub<12)
      File >> r.Routen;
   else
   {
      File >> r.Routen;
      File >> *((ALBUM<CRoute>*)&r);
   }

   return (File);
}

//============================================================================================
//CRentRoute::
//============================================================================================
//Konstruktor:
//============================================================================================
CRentRoute::CRentRoute ()
{
   Rang         = 0;
   Auslastung   = 0;
   AuslastungFC = 0;
   Image        = 0;
   Miete        = 0;
   LastFlown    = 99;
   TageMitVerlust = 0;
   TageMitGering  = 99;

   RoutenAuslastung = 0;
   HeuteBefoerdert  = 0;
}

//--------------------------------------------------------------------------------------------
//Gibt zurück, wieviele Routen der Spieler besitzt:
//--------------------------------------------------------------------------------------------
SLONG CRentRouten::GetNumUsed(void)
{
   SLONG c, Anz=0;

   for (c=0; c<SLONG(Routen.AnzEntries()); c++)
      if (Routen.IsInAlbum(c) && RentRouten[c].Rang!=0)
      {
         Anz++;
      }

   return (Anz);
}

//--------------------------------------------------------------------------------------------
//Speichert ein RentRouten-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CRentRoute &r)
{
   File << r.Rang             << r.LastFlown       << r.AvgFlown;
   File << r.Auslastung       << r.Image           << r.Miete;
   File << r.Ticketpreis      << r.TicketpreisFC   << r.TageMitVerlust << r.TageMitGering;
   File << r.RoutenAuslastung << r.HeuteBefoerdert << r.AuslastungFC;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt ein RentRouten-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CRentRoute &r)
{
   File >> r.Rang             >> r.LastFlown       >> r.AvgFlown;
   File >> r.Auslastung       >> r.Image           >> r.Miete;
   File >> r.Ticketpreis      >> r.TicketpreisFC   >> r.TageMitVerlust >> r.TageMitGering;
   File >> r.RoutenAuslastung >> r.HeuteBefoerdert >> r.AuslastungFC;

   return (File);
}
