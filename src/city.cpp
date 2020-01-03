//============================================================================================
// City.Cpp - Routinen zur Verwaltung von Städten
//============================================================================================
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const char FileId[] = "City";

SLONG ReadLine (BUFFER<UBYTE> &Buffer, SLONG BufferStart, char *Line, SLONG LineLength);

//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
CITIES::CITIES (const CString &TabFilename) : ALBUM<CITY> (Cities, "Cities")
{
   ReInit (TabFilename);
}

//--------------------------------------------------------------------------------------------
//Initialisiert die Städte:
//--------------------------------------------------------------------------------------------
void CITIES::ReInit (const CString &TabFilename)
{
   //CStdioFile    Tab;
   BUFFER<char>  Line(300);
   long          Id;
   SLONG         Anz=0;

   //Load Table header:
   BUFFER<UBYTE> FileData (*LoadCompleteFile (FullFilename (TabFilename, ExcelPath)));
   SLONG         FileP=0;

   //Die erste Zeile einlesen
   FileP=ReadLine (FileData, FileP, Line, 300);

   Cities.ReSize (MAX_CITIES);

   while (1)
   {
      if (FileP>=FileData.AnzEntries()) break;
      FileP=ReadLine (FileData, FileP, Line, 300);

      //if (!Tab.ReadString (Line, 300)) break;
      TeakStrRemoveEndingCodes (Line, "\xd\xa\x1a\r");

      //Tabellenzeile hinzufügen:
      Id=atol (strtok (Line, ";\x8\""))+0x1000000;

      //Hinzufügen (darf noch nicht existieren):
      if (IsInAlbum (Id)) TeakLibW_Exception (FNL, ExcNever);
      (*this)*=Id;

      //SpeedUp durch direkten Zugriff:
      Id=(*this)(Id);

      (*this)[Id].Name              = strtok (NULL, TabSeparator);
      (*this)[Id].Lage              = strtok (NULL, TabSeparator);
      (*this)[Id].Areacode          = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].KuerzelGood       = strtok (NULL, TabSeparator);
      (*this)[Id].KuerzelReal       = strtok (NULL, TabSeparator);
      (*this)[Id].Wave              = strtok (NULL, TabSeparator);
      (*this)[Id].TextRes           = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].AnzTexts          = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].PhotoName         = strtok (NULL, TabSeparator);
      (*this)[Id].AnzPhotos         = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].Einwohner         = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].GlobusPosition.x  = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].GlobusPosition.y  = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].MapPosition.x     = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].MapPosition.y     = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].BuroRent          = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].bNewInAddOn       = atoi (strtok (NULL, TabSeparator));

      (*this)[Id].Name = KorrigiereUmlaute ((*this)[Id].Name);

      #ifdef DEMO
         (*this)[Id].AnzPhotos=0;  //In der Demo keine Städtebilder
      #endif

      Anz++;
   }

   Cities.ReSize (Anz);

   UseRealKuerzel (Sim.Options.OptionRealKuerzel);
}

//--------------------------------------------------------------------------------------------
//Schaltet um zwischen reellen und praktischen Kuerzeln:
//--------------------------------------------------------------------------------------------
void CITIES::UseRealKuerzel (BOOL Real)
{
   SLONG c;

   for (c=AnzEntries()-1; c>=0; c--)
      if (IsInAlbum(c))
         if (Real)
            (*this)[c].Kuerzel=(*this)[c].KuerzelReal;
         else
            (*this)[c].Kuerzel=(*this)[c].KuerzelGood;
}

//--------------------------------------------------------------------------------------------
//Berechnet die Entfernung zweier beliebiger Städte: (in Meter)
//--------------------------------------------------------------------------------------------
SLONG CITIES::CalcDistance (long CityId1, long CityId2)
{
   if (HashTable.AnzEntries()==0)
   {
      HashTable.ReSize (AnzEntries()*AnzEntries());
      HashTable.FillWith (0);
   }

   if (CityId1>=0x1000000) CityId1=(*this)(CityId1);
   if (CityId2>=0x1000000) CityId2=(*this)(CityId2);

   if (HashTable[SLONG(CityId1+CityId2*AnzEntries())])
   {
      SLONG rc = HashTable[SLONG(CityId1+CityId2*AnzEntries())];

      return (min(rc, 16440000));
   }
   else
   {
      FXYZ  Vector1;    //Vektor vom Erdmittelpunkt zu City1
      FXYZ  Vector2;    //Vektor vom Erdmittelpunkt zu City2
      float Alpha;      //Winkel in Grad zwischen den Vektoren (für Kreissegment)

      //Berechnung des ersten Vektors:
      Vector1.x = (float)cos ((*this)[CityId1].GlobusPosition.x * 3.14159 / 180.0);
      Vector1.z = (float)sin ((*this)[CityId1].GlobusPosition.x * 3.14159 / 180.0);

      Vector1.y = (float)sin ((*this)[CityId1].GlobusPosition.y * 3.14159 / 180.0);

      Vector1.x *= (float)sqrt (1-Vector1.y*Vector1.y);
      Vector1.z *= (float)sqrt (1-Vector1.y*Vector1.y);

      //Berechnung des zweiten Vektors:
      Vector2.x = (float)cos ((*this)[CityId2].GlobusPosition.x * 3.14159 / 180.0);
      Vector2.z = (float)sin ((*this)[CityId2].GlobusPosition.x * 3.14159 / 180.0);

      Vector2.y = (float)sin ((*this)[CityId2].GlobusPosition.y * 3.14159 / 180.0);

      Vector2.x *= (float)sqrt (1-Vector2.y*Vector2.y);
      Vector2.z *= (float)sqrt (1-Vector2.y*Vector2.y);

      //Berechnung des Winkels zwischen den Vektoren:
      Alpha = (float)(acos ((Vector1.x*Vector2.x+Vector1.y*Vector2.y+Vector1.z*Vector2.z)/Vector1.abs()/Vector2.abs()) * 180.0 / 3.14159);

      //Berechnung der Länge des Kreissegments: (40040174 = Äquatorumfang)
      HashTable[SLONG(CityId1+CityId2*AnzEntries())] = SLONG(fabs(Alpha) * 40040174 / 360.0);

      SLONG rc = HashTable[SLONG(CityId1+CityId2*AnzEntries())];

      return (min(rc, 16440000));
   }
}

//--------------------------------------------------------------------------------------------
//Berechnet die Dauer eines Fluges:
//--------------------------------------------------------------------------------------------
SLONG CITIES::CalcFlugdauer (long CityId1, long CityId2, long Speed)
{
   SLONG d=(CalcDistance (CityId1, CityId2)/Speed+999)/1000+1+2-2;

   if (d<2) d=2;

   return (d);
}

//--------------------------------------------------------------------------------------------
//Gibt eine zufällige Stadt zurück:
//--------------------------------------------------------------------------------------------
SLONG CITIES::GetRandomUsedIndex (TEAKRAND *pRand)
{
   SLONG c, n=0;

   for (c=AnzEntries()-1; c>=0; c--)
      if (IsInAlbum(c))
         n++;

   if (pRand) n = pRand->Rand(n)+1;
         else n = (rand()%n)+1;

   for (c=AnzEntries()-1; c>=0; c--)
      if (IsInAlbum(c))
      {
         n--;
         if (n==0) return (c);
      }

   DebugBreak();
   return (0);
}

//--------------------------------------------------------------------------------------------
//Gibt eine zufällige Stadt von diesem AreaCode zurück:
//--------------------------------------------------------------------------------------------
SLONG CITIES::GetRandomUsedIndex (SLONG AreaCode, TEAKRAND *pRand)
{
   SLONG c, n=0;

   for (c=AnzEntries()-1; c>=0; c--)
      if (IsInAlbum(c) && (*this)[c].Areacode==AreaCode)
         n++;

   if (pRand) n = pRand->Rand(n)+1;
         else n = (rand()%n)+1;

   for (c=AnzEntries()-1; c>=0; c--)
      if (IsInAlbum(c) && (*this)[c].Areacode==AreaCode)
      {
         n--;
         if (n==0) return (c);
      }

   DebugBreak();
   return (0);
}

//--------------------------------------------------------------------------------------------
//Gibt die Nummer der Stadt mit dem angegebnen Namen zurück:
//--------------------------------------------------------------------------------------------
ULONG CITIES::GetIdFromName (char *Name)
{
   SLONG c;

   for (c=0; c<(SLONG)AnzEntries(); c++)
      if (IsInAlbum(c) && stricmp (Name, (LPCTSTR)Cities[c].Name)==0)
         return (GetIdFromIndex(c));

   TeakLibW_Exception (FNL, ExcNever);
   return (0);
}

//--------------------------------------------------------------------------------------------
// Für jedes Argument aus einer ... Parameterreihe
//  SMPL: foreacharg (long, Value1, 0)
//--------------------------------------------------------------------------------------------
#define foreacharg(type, firstargname, finalvalue) \
   if (va_list va_marker = (va_list)true) \
      for (long arghelper1=0, arghelper2=0, arghelper3=0; arghelper1==0; arghelper1=1) \
         for (type q##type=firstargname; arghelper2==0; arghelper2=1) \
            for (va_start (va_marker, firstargname); q##type!=finalvalue; q##type=va_arg(va_marker, type))

//--------------------------------------------------------------------------------------------
//Gibt die Nummer der Stadt mit dem angegebnen Namen zurück:
//--------------------------------------------------------------------------------------------
ULONG CITIES::GetIdFromNames (char *Name, ...)
{
   SLONG c;

   foreacharg (LPSTR, Name, NULL)
   {
      for (c=0; c<(SLONG)AnzEntries(); c++)
         if (IsInAlbum(c) && stricmp (qLPSTR, (LPCTSTR)Cities[c].Name)==0)
            return (GetIdFromIndex(c));
   }

   TeakLibW_Exception (FNL, ExcNever);
   return (0);
}

//--------------------------------------------------------------------------------------------
//CRentCity::
//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
CRentCity::CRentCity ()
{
   Rang = 0;
}

//--------------------------------------------------------------------------------------------
//Gibt zurück, wieviele Routen der Spieler besitzt:
//--------------------------------------------------------------------------------------------
SLONG CRentCities::GetNumUsed(void)
{
   SLONG c, Anz=0;

   for (c=0; c<(SLONG)Cities.AnzEntries(); c++)
      if (Cities.IsInAlbum(c) && RentCities[c].Rang!=0)
      {
         Anz++;
      }

   return (Anz);
}

//--------------------------------------------------------------------------------------------
//Speichert ein RentCity-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CRentCity &r)
{
   File << r.Rang << r.Image << r.Miete;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt ein RentCity-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CRentCity &r)
{
   File >> r.Rang >> r.Image >> r.Miete;

   return (File);
}