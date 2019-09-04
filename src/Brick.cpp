//============================================================================================
// Brick.Cpp - Funktionen für die Verwaltung der baulichen Elemente:
//============================================================================================
#include "StdAfx.h"

extern SB_CColorFX ColorFX;

SLONG ReadLine (BUFFER<UBYTE> &Buffer, SLONG BufferStart, char *Line, SLONG LineLength);

//--------------------------------------------------------------------------------------------
//Als Leer initialisieren:
//--------------------------------------------------------------------------------------------
BRICK::BRICK ()
{
}

//--------------------------------------------------------------------------------------------
//Ggf. die Daten vernichten:
//--------------------------------------------------------------------------------------------
BRICK::~BRICK ()
{
}

//--------------------------------------------------------------------------------------------
//Die Ausmaße einer Brick-Bitmap zurückgeben:
//--------------------------------------------------------------------------------------------
XY BRICK::GetBitmapDimension (void) const
{
   return (Bitmap[0l].Size);
}

//--------------------------------------------------------------------------------------------
//Einen Brick an Stelle X blitten:
//--------------------------------------------------------------------------------------------
void BRICK::BlitAt (SBBM &Offscreen, BOOL Ansatz, const XY &ScreenPos, SLONG Phase)
{
   if (ScreenPos.x+Bitmap[0l].Size.x<-2 || ScreenPos.x>RightAirportClip || !bActive) return;

   //Wenn man es mit dem Maus-Cursor besonders anfasst:
   if (Ansatz)
   {
      if (NonTrans==1)
         Offscreen.BlitFrom (Bitmap[0l], ScreenPos.x-Bitmap[0l].Size.x, ScreenPos.y-Bitmap[0l].Size.y);
      else
         Offscreen.BlitFromT (Bitmap[0l], ScreenPos.x-Bitmap[0l].Size.x, ScreenPos.y-Bitmap[0l].Size.y);
   }
   //Normalfall:
   else
   {
      if (AnimSpeed)
      {
         if (Triggered==0 || Editor)
            if (Phase==-1) Phase=(Sim.TickerTime/AnimSpeed)%Bitmap.AnzEntries();
            else if (Phase>=100) Phase=(Phase-100+(Sim.TickerTime/AnimSpeed))%Bitmap.AnzEntries();

         if (Triggered==90)
         {
            SLONG c;
            SLONG Target = (Sim.TickerTime/AnimSpeed)%WaitSum;

            for (c=0; c<Bitmap.AnzEntries(); c++)
            {
               Target-=WaitTimes[c];
               if (Target<=0) { Phase=c; break; }
            }
         }

         if (NonTrans==1)
            Offscreen.BlitFrom (Bitmap[Phase], ScreenPos.x, ScreenPos.y);
         else
            Offscreen.BlitFromT (Bitmap[Phase], ScreenPos.x, ScreenPos.y);
      }
      else //Kein Animierter Brick
      {
         if (NonTrans==1)
            Offscreen.BlitFrom (Bitmap[0], ScreenPos.x, ScreenPos.y);
         else
            Offscreen.BlitFromT (Bitmap[0l], ScreenPos.x, ScreenPos.y);
      }
   }
}

//--------------------------------------------------------------------------------------------
//Einen Brick an Stelle X blitten + skalieren:
//--------------------------------------------------------------------------------------------
void BRICK::BlitAt (SBBM &Offscreen, BOOL Ansatz, const XY &p1, const XY &p2)
{
   //Hier stand überall früher PrimaryBm statt Offscreen...

   if (Ansatz)
   {
      if (NonTrans==1)
         Offscreen.BlitFrom (Bitmap[0l], p1, p2);
      else
         Offscreen.BlitFromT (Bitmap[0l], p1, p2);
   }
   else
   {
      if (NonTrans==1)
         Offscreen.BlitFrom (Bitmap[0l], p1, p2);
      else
         Offscreen.BlitFrom (Bitmap[0l], p1, p2);
   }
}

//--------------------------------------------------------------------------------------------
//Einen Brick an Stelle X blitten:
//--------------------------------------------------------------------------------------------
void BRICK::BlitAt (SBPRIMARYBM &Offscreen, BOOL Ansatz, const XY &ScreenPos, SLONG Phase)
{
   if (ScreenPos.x+Bitmap[0l].Size.x<-2 || ScreenPos.x>640 || !bActive) return;

   //Hier stand überall früher PrimaryBm statt Offscreen...

   //Wenn man es mit dem Maus-Cursor besonders anfasst:
   if (Ansatz)
   {
      if (NonTrans==1)
         Offscreen.BlitFrom (Bitmap[0l], ScreenPos.x-Bitmap[0l].Size.x, ScreenPos.y-Bitmap[0l].Size.y);
      else
         Offscreen.BlitFromT (Bitmap[0l], ScreenPos.x-Bitmap[0l].Size.x, ScreenPos.y-Bitmap[0l].Size.y);
   }
   //Normalfall:
   else
   {
      if (AnimSpeed)
      {
         if (Triggered==0 || Editor)
            if (Phase==-1) Phase=(Sim.TickerTime/AnimSpeed)%Bitmap.AnzEntries();
            else if (Phase>=100) Phase=(Phase-100+(Sim.TickerTime/AnimSpeed))%Bitmap.AnzEntries();

         if (Triggered==90)
         {
            SLONG c;
            SLONG Target = (Sim.TickerTime/AnimSpeed)%WaitSum;

            for (c=0; c<Bitmap.AnzEntries(); c++)
            {
               Target-=WaitTimes[c];
               if (Target<=0) { Phase=c; break; }
            }
         }

         if (NonTrans==1)
            Offscreen.BlitFrom (Bitmap[Phase], ScreenPos.x, ScreenPos.y);
         else
            Offscreen.BlitFromT (Bitmap[Phase], ScreenPos.x, ScreenPos.y);
      }
      else //Kein Animierter Brick
      {
         if (NonTrans==1)
         {
            Offscreen.BlitFrom (Bitmap[0], ScreenPos.x, ScreenPos.y);
         }
         else if (NonTrans==2 && Sim.Options.OptionTransparenz)
            ColorFX.BlitTrans (Bitmap[0].pBitmap, &Offscreen.PrimaryBm, XY (ScreenPos.x, ScreenPos.y), NULL, -1);
         else
         {
            Offscreen.BlitFromT (Bitmap[0l], ScreenPos.x, ScreenPos.y);
         }
      }
   }
}

//--------------------------------------------------------------------------------------------
//Einen Brick an Stelle X blitten + skalieren:
//--------------------------------------------------------------------------------------------
void BRICK::BlitAt (SBPRIMARYBM &Offscreen, BOOL Ansatz, const XY &p1, const XY &p2)
{
   //Hier stand überall früher PrimaryBm statt Offscreen...

   if (Ansatz)
   {
      if (NonTrans==1)
         Offscreen.BlitFrom (Bitmap[0l], p1, p2);
      else
         Offscreen.BlitFromT (Bitmap[0l], p1, p2);
   }
   else
   {
      if (NonTrans==1)
         Offscreen.BlitFrom (Bitmap[0l], p1, p2);
      else
         Offscreen.BlitFrom (Bitmap[0l], p1, p2);
   }
}

//--------------------------------------------------------------------------------------------
//Für Editor: Ist Mauscursor auf Glas oder auf echtem Baustein?
//--------------------------------------------------------------------------------------------
BOOL BRICK::IsGlasAt (SLONG x, SLONG y)
{
   return (Bitmap[0l].GetPixel (x, y)==0);
}

//--------------------------------------------------------------------------------------------
//Ggf. die Bricks an das neue Zeitalter anpassen:
//--------------------------------------------------------------------------------------------
void BRICK::UpdateBrick (void)
{
   BOOL  ReloadNecessary = FALSE;
   long c;

   //Falls Bitmap noch nicht vorhanden ==> laden!
   if (Bitmap.AnzEntries()==0) ReloadNecessary=TRUE;

   if (ReloadNecessary)
   {
      //Bild muß (neu) geladen werden:
      SLONG  AnzPhases;

      //Wie oft kommt diese Periode drin vor ?
      AnzPhases=graphicIDs.AnzEntries();

      //Sind Angaben vorhanden?
      if (AnzPhases>1)
      {
         //Speicher für die Animationsphasen bereitstellen:
         Bitmap.ReSize (AnzPhases);

         //Dies wird jetzt wieder als Zähler verwendet:
         AnzPhases=0;

         //Das richtige Sub-Bild raussuchen:
         for (c=0; c<graphicIDs.AnzEntries(); c++)
         {
            //Bild in Brick-Bitmap einbinden:
            Bitmap[AnzPhases++].ReSize (pGLibBrick, graphicIDs[c]);
                  /*TECBM(FullFilename (Filename, BrickPath), c).Size);*/
         }
      }
      else
      {
         //Speicher für die Animationsphases bereitstellen:
         Bitmap.ReSize (1);

         Bitmap[0l].ReSize (pGLibBrick, graphicIDs[0]);
      }
   }
}

//--------------------------------------------------------------------------------------------
//Berechnet eine intelligentere Position mit Grids & Co.
//--------------------------------------------------------------------------------------------
XY BRICK::GetIntelligentPosition (SLONG x, SLONG y)
{
   XY    rc;
   XY    LocalOffset;
   SLONG BaseOffsetX;

   LocalOffset = XY(0,0);

   //Die Dinge oben etwas verschieben:
   if (GetBitmapDimension().y-2+y<210 && !GetAsyncKeyState (VK_MENU))
   {
      LocalOffset.x=22;
      LocalOffset.y=5;
   }

   if (GetAsyncKeyState (VK_CONTROL)) return (XY(x,y));

   rc.y = (y-(BaseOffset.y+LocalOffset.y)+Bitmap[0].Size.y*1000)/Grid.y*Grid.y+(BaseOffset.y+LocalOffset.y)-Bitmap[0].Size.y*1000;

   if (rc.y<MinY) rc.y=MinY;
   if (rc.y>MaxY) rc.y=MaxY;

   BaseOffsetX = (BaseOffset.x+LocalOffset.x) - (rc.y-(BaseOffset.y+LocalOffset.y))/2;

   rc.x = (x-BaseOffsetX+Bitmap[0].Size.x*1000)/Grid.x*Grid.x+BaseOffsetX-Bitmap[0].Size.x*1000;

   return (rc);
}

//--------------------------------------------------------------------------------------------
//Initialisiert das Album mit den Bricks:
//--------------------------------------------------------------------------------------------
BRICKS::BRICKS (const CString &TabFilename) : ALBUM<BRICK> (Bricks, "Bricks")
{
   ReInit (TabFilename);
}

//--------------------------------------------------------------------------------------------
//Lädt nachträglich die Tabelle mit den Bricks:
//--------------------------------------------------------------------------------------------
void BRICKS::ReInit (const CString &TabFilename)
{
   //CStdioFile    Tab;
   BUFFER<char>  Line(300);
   char         *TimePointer [150];
   long          Id;
   long          AnzTimePointer;

   //Load Table header:
   BUFFER<UBYTE> FileData (*LoadCompleteFile (FullFilename (TabFilename, ExcelPath)));
   SLONG         FileP=0;

   /*if (!Tab.Open (FullFilename (TabFilename, ExcelPath), CFile::modeRead))
   {
      TeakLibW_Exception (FNL, ExcNever);
      return;
   }*/

   //Die erste Zeile einlesen
   //Tab.ReadString (Line, 300);
   FileP=ReadLine (FileData, FileP, Line, 300);

   Bricks.ReSize (MAX_BRICKS);

   while (1)
   {
      //if (!Tab.ReadString (Line, 300)) break;
      if (FileP>=FileData.AnzEntries()) break;
      FileP=ReadLine (FileData, FileP, Line, 300);

      TeakStrRemoveEndingCodes (Line, "\xd\xa\x1a\r");

      //Tabellenzeile hinzufügen:
      Id=atol (strtok (Line, ";\x8\""))+0x10000000;

      //Hinzufügen (darf noch nicht existieren):
      if (IsInAlbum (Id)) TeakLibW_Exception (FNL, ExcNever);
      (*this)+=Id;

      //SpeedUp durch direkten Zugriff:
      Id=(*this)(Id);

      (*this)[Id].Filename = strtok (NULL, ";\x8\"");
      
      (*this)[Id].RamPriority = atoi (strtok (NULL, ";\x8\""));

      (*this)[Id].NonTrans  = (UBYTE)atoi (strtok (NULL, TabSeparator));
      (*this)[Id].Triggered = (UBYTE)atoi (strtok (NULL, TabSeparator));

      (*this)[Id].Layer = (UBYTE)atoi (strtok (NULL, TabSeparator));
      (*this)[Id].AnimSpeed   = (UBYTE)atoi (strtok (NULL, TabSeparator));
      (*this)[Id].FloorOffset = atoi (strtok (NULL, TabSeparator));

      (*this)[Id].BaseOffset.x = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].BaseOffset.y = atoi (strtok (NULL, TabSeparator));

      (*this)[Id].Grid.x = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].Grid.y = atoi (strtok (NULL, TabSeparator));

      (*this)[Id].MinY = atoi (strtok (NULL, TabSeparator));
      (*this)[Id].MaxY = atoi (strtok (NULL, TabSeparator));

      (*this)[Id].ObstacleType = (UBYTE) atoi (strtok (NULL, TabSeparator));

      (*this)[Id].WaitSum = 0;
      (*this)[Id].WaitTimes.ReSize (50);

      for (AnzTimePointer=0; ; AnzTimePointer++)
      {
         TimePointer[AnzTimePointer]=strtok (NULL, " ");

         if (!TimePointer[AnzTimePointer]) break;

         if (strchr (TimePointer[AnzTimePointer], ':'))
         {
            (*this)[Id].Triggered=90;
            (*this)[Id].WaitTimes[AnzTimePointer] = atoi (strchr (TimePointer[AnzTimePointer], ':')+1);

            *(strchr (TimePointer[AnzTimePointer], ':'))=0;
         }
         else 
            (*this)[Id].WaitTimes[AnzTimePointer] = 1;

         (*this)[Id].WaitSum += (*this)[Id].WaitTimes[AnzTimePointer];
      }

      (*this)[Id].WaitTimes.ReSize (AnzTimePointer);

      (*this)[Id].graphicIDs.ReSize (AnzTimePointer);

      for (SLONG c=0; c<AnzTimePointer; c++)
      {
         (*this)[Id].graphicIDs[c]=StringToInt64 (TimePointer[c]);
         /*(*this)[Id].graphicIDs[c]=0;

         for (SLONG d=0; d<strlen(TimePointer[c]); d++)
            (*this)[Id].graphicIDs[c]+=__int64((TimePointer[c])[d])<<(8*d);*/
      }
   }
}

//--------------------------------------------------------------------------------------------
//Bringt alle Bricks auf den neuesten Stand:
//--------------------------------------------------------------------------------------------
void BRICKS::UpdateBricks (void)
{
   SLONG c, d, Anz=0;
   SLONG VidMemFree;
   SLONG Bytes=0;

   //hprintf ("Updating Bricks.");
   for (c=0; c<Bricks.AnzEntries(); c++)
      if (IsInAlbum(c))
         Bricks[c].UpdateBrick ();

   for (d=0; d<20; d++)
      for (c=0; c<Bricks.AnzEntries(); c++)
         if (IsInAlbum(c))
            if (Bricks[c].RamPriority==d && bNoVgaRam==FALSE)
            {
               SBBM tmpBitmap;

               tmpBitmap.ReSize (Bricks[c].Bitmap[0l].Size);
               tmpBitmap.BlitFrom (Bricks[c].Bitmap[0l]);
               Bricks[c].Bitmap[0l].Destroy();

               DDCAPS ddcaps;

               ddcaps.dwSize = sizeof (ddcaps);
               lpDD->GetCaps (&ddcaps, NULL);

               VidMemFree=ddcaps.dwVidMemFree;

               if (Bricks[c].Bitmap[0l].ReSize (tmpBitmap.Size, CREATE_VIDMEM)==DD_OK)
               {
                  Anz++;
                  Bytes+=2*tmpBitmap.Size.x*tmpBitmap.Size.y;

                  ddcaps.dwSize = sizeof (ddcaps);
                  lpDD->GetCaps (&ddcaps, NULL);
                  Bricks[c].Bitmap[0l].BlitFrom (tmpBitmap);
               }
               else
               {
                  Bricks[c].Bitmap[0l].ReSize (tmpBitmap.Size, CREATE_SYSMEM);
                  Bricks[c].Bitmap[0l].BlitFrom (tmpBitmap);

                  hprintf ("%li Bricks (%li Bytes) out-sourced.", Anz, Bytes);
                  hprintf ("Out of Video-RAM. Using normal RAM.");
                  return;
               }
            }

   if (!bFirstClass)
   {
      for (c=0; c<8; c++)
      {
         //Hiermit löschen wir die Smacker-Platzhalter an den Gates. Die dienen beim Editieren als optische Hilfe zur Positionierung, aber im Spiel können wir sie nicht gebrauchen
         (*this)[SLONG(0x10000000+760+c)].Bitmap[0].FillWith(0);
         (*this)[SLONG(0x10000000+768+c)].Bitmap[0].FillWith(0);
      }
   }

   hprintf ("%li Bricks (%li Bytes) out-sourced.", Anz, Bytes);
}

//--------------------------------------------------------------------------------------------
//Wirft alle Bricks aus dem Speicher:
//--------------------------------------------------------------------------------------------
void BRICKS::Destroy (void)
{
   Bricks.ReSize (0);

   IsInAlbum(SLONG(0x01000000));
}

//--------------------------------------------------------------------------------------------
//Resuariert alle Bricks, die "lost" sind:
//--------------------------------------------------------------------------------------------
void BRICKS::RestoreBricks ()
{
   for (SLONG c=0; c<Bricks.AnzEntries(); c++)
      if (IsInAlbum(c))
      {
         if (Bricks[c].RamPriority<20 && bNoVgaRam==FALSE)
         {
            if (Bricks[c].Bitmap[0l].pBitmap->GetSurface()->IsLost()!=DD_OK)
            {
               Bricks[c].Bitmap[0l].pBitmap->GetSurface()->Restore ();

               Bricks[c].Bitmap[0l].Clear ();

               SBBM TempBm (pGLibBrick, Bricks[c].graphicIDs[0]);

               Bricks[c].Bitmap[0l].BlitFrom (TempBm);
            }
         }
      }
}

//============================================================================================
//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
//Primitiver Konstruktor:
//--------------------------------------------------------------------------------------------
BUILD::BUILD (long BrickId, const XY &ScreenPos, BOOL Ansatz)
{
   BUILD::BrickId    = BrickId;
   BUILD::ScreenPos  = ScreenPos;
   BUILD::Par        = 0;

   if (Ansatz) BUILD::ScreenPos -= Bricks[BrickId].GetBitmapDimension();
}

//--------------------------------------------------------------------------------------------
//Initialisiert allgemein:
//--------------------------------------------------------------------------------------------
BUILDS::BUILDS () : ALBUM<BUILD> (Builds, "Builds")
{
}

//--------------------------------------------------------------------------------------------
//Speichert einen Build-Eintrag:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const BUILD &Build)
{
   File << Build.BrickId << Build.ScreenPos << Build.Par;
   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt einen Build-Eintrag:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, BUILD &Build)
{
   File >> Build.BrickId >> Build.ScreenPos >> Build.Par;
   return (File);
}

//--------------------------------------------------------------------------------------------
//Speichert einen Builds-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const BUILDS &Builds)
{
   File << Builds.Builds;
   File << *((ALBUM<BUILD>*)&Builds);

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt einen Builds-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, BUILDS &Builds)
{
   File >> Builds.Builds;
   File >> *((ALBUM<BUILD>*)&Builds);

   return (File);
}

//--------------------------------------------------------------------------------------------
//Löscht alle Elemente des Flughafens:
//--------------------------------------------------------------------------------------------
void BUILDS::Clear (void)
{
   ClearAlbum ();
}

//--------------------------------------------------------------------------------------------
//Lädt einen Airport-Anordnung:
//--------------------------------------------------------------------------------------------
void BUILDS::Load (SLONG Hall, SLONG Level)
{
   CString Filename;
   SLONG   Difficulty = Sim.Difficulty;

   if (Difficulty==DIFF_FREEGAME) Difficulty=DIFF_FREEGAMEMAP;

   //Wenn der Flughafen für einen Level nicht existiert, dann Fallback auf den Difficulty-Level davor probieren
   do
   {
      Filename = FullFilename (HallFilenames [Hall], MiscPath, 100*Difficulty+Level);
      Difficulty--;

      if (Difficulty==10) Difficulty=Difficulty;
   }
   while (Difficulty>=0 && !Editor && !DoesFileExist (Filename));

   if (DoesFileExist (Filename))
   {
      TEAKFILE File (Filename, TEAKFILE_READ);

      File >> Builds;

      //Etwas tricky: Den geerbeten shifting-operator der ALBUM-Klasse aufrufen:
      File >> *((ALBUM<BUILD>*)this);
   }
   else Clear ();
}

//--------------------------------------------------------------------------------------------
//Speichert einen Airport-Anordnung:
//--------------------------------------------------------------------------------------------
void BUILDS::Save (SLONG Hall, SLONG Level) const
{
   CString Filename;
   SLONG   Difficulty = Sim.Difficulty;

   if (Difficulty==DIFF_FREEGAME) Difficulty=DIFF_FREEGAMEMAP;

   if (Level!=0)
   {
      Filename = FullFilename (HallFilenames [Hall], MiscPath, 100*Difficulty+Level);

      TEAKFILE File (Filename, TEAKFILE_WRITE);

      File << Builds;

      //Etwas tricky: Den geerbten shifting-operator der ALBUM-Klasse aufrufen:
      File << *((ALBUM<BUILD>*)this);
   }
}

//--------------------------------------------------------------------------------------------
//Sortiert alle Elemente des Flughafens gemäß ihrer Z-Position:
//--------------------------------------------------------------------------------------------
void BUILDS::Sort (void)
{
   SLONG c;

   for (c=0; c<long(AnzEntries()-1); c++)
      if ((!IsInAlbum(c) && IsInAlbum(c+1)) ||
          (IsInAlbum(c) && IsInAlbum(c+1) && Bricks[(*this)[c].BrickId].Layer >Bricks[(*this)[(SLONG)(c+1)].BrickId].Layer) ||
          (IsInAlbum(c) && IsInAlbum(c+1) && Bricks[(*this)[c].BrickId].Layer==Bricks[(*this)[(SLONG)(c+1)].BrickId].Layer && (*this)[c].ScreenPos.y+Bricks[(*this)[c].BrickId].GetBitmapDimension().y+Bricks[(*this)[c].BrickId].FloorOffset>(*this)[(SLONG)(c+1)].ScreenPos.y+Bricks[(*this)[(SLONG)(c+1)].BrickId].GetBitmapDimension().y+Bricks[(*this)[(SLONG)(c+1)].BrickId].FloorOffset) ||
          (IsInAlbum(c) && IsInAlbum(c+1) && Bricks[(*this)[c].BrickId].Layer==Bricks[(*this)[(SLONG)(c+1)].BrickId].Layer && (*this)[c].ScreenPos.y+Bricks[(*this)[c].BrickId].GetBitmapDimension().y+Bricks[(*this)[c].BrickId].FloorOffset==(*this)[(SLONG)(c+1)].ScreenPos.y+Bricks[(*this)[(SLONG)(c+1)].BrickId].GetBitmapDimension().y+Bricks[(*this)[(SLONG)(c+1)].BrickId].FloorOffset && (*this)[c].ScreenPos.x>(*this)[(SLONG)(c+1)].ScreenPos.x))
      {
         Swap (c, c+1);
         c-=2; if (c<-1) c=-1;
      }
}