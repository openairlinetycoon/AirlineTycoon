//============================================================================================
// Person.Cpp - Verwaltung von Clans und Personen (Clan-Ausprägungen)
//============================================================================================
#include "stdafx.h"
#include "AtNet.h"

extern SB_CColorFX ColorFX;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SLONG ReadLine (BUFFER<UBYTE> &Buffer, SLONG BufferStart, char *Line, SLONG LineLength);

//Daten des aktuellen Savegames beim laden:
extern SLONG SaveVersion;
extern SLONG SaveVersionSub;

//--------------------------------------------------------------------------------------------
//Gibt das Vorzeichen (-1, 0 oder 1) des Parameters zurück:
//--------------------------------------------------------------------------------------------
SLONG sign (SLONG Value)
{
        if (Value>0) return (1);
   else if (Value<0) return (-1);
   else return (0);
}

//--------------------------------------------------------------------------------------------
//Konstruire aus Tabelle
//--------------------------------------------------------------------------------------------
CLANS::CLANS (const CString &TabFilename) : ALBUM<CLAN> (Clans, "Clans")
{
   ReInit (TabFilename);
}

//--------------------------------------------------------------------------------------------
//Re-Konstruire aus Tabelle
//--------------------------------------------------------------------------------------------
void CLANS::ReInit (const CString &TabFilename)
{
   //TEAKFILE      Tab;
   BUFFER<char>  Line(5000);
   ULONG         Id;
   char         *DirPhaseLists[10+4];
   SLONG         PhaseListsNumbers[10+4];
   SLONG         c;

   BUFFER<UBYTE> FileData (*LoadCompleteFile (FullFilename (TabFilename, ExcelPath)));
   SLONG         FileP=0;

   //Load Table header:
   //Tab.Open (FullFilename (TabFilename, ExcelPath), TEAKFILE_READ);

   //Die erste Zeile einlesen
   //Tab.ReadLine (Line, 5000);
   FileP=ReadLine (FileData, FileP, Line, 5000);

   Clans.ReSize (MAX_CLANS);

   while (1)
   {
      for (SLONG ShadowPass=0; ShadowPass<3; ShadowPass++)
      {
         //if (Tab.IsEof())
         if (FileP>=FileData.AnzEntries())
         {
            if (ShadowPass==0) return;
                          else DebugBreak();
         }

         FileP=ReadLine (FileData, FileP, Line, 5000);
         //Tab.ReadLine (Line, 5000);
         TeakStrRemoveEndingCodes (Line, "\xd\xa\x1a\r");

         //Tabellenzeile hinzufügen:
         if (ShadowPass==0) 
         {
            Id=(*this).GetUniqueId();
            (*this)+=Id;

            //SpeedUp durch direkten Zugriff:
            Id=(*this)(Id);

            (*this)[Id].Type        = (UBYTE)atoi (strtok (Line, TabSeparator));
            (*this)[Id].Group       = atoi (strtok (NULL, TabSeparator));
            strtok (NULL, TabSeparator);  //Comment-Spalte
            (*this)[Id].UpdateNow   = (UBYTE)atoi (strtok (NULL, TabSeparator));
            (*this)[Id].Wkeit       = (UBYTE)atoi (strtok (NULL, TabSeparator));
            (*this)[Id].WalkSpeed   = atoi (strtok (NULL, TabSeparator));
            (*this)[Id].Faktor      = atoi (strtok (NULL, TabSeparator));
            (*this)[Id].HasSuitcase = atoi (strtok (NULL, TabSeparator));
            (*this)[Id].GimmickArt1 = atoi (strtok (NULL, TabSeparator));
            (*this)[Id].GimmickArt2 = atoi (strtok (NULL, TabSeparator));
            (*this)[Id].Offset.x    = atoi (strtok (NULL, TabSeparator));
            (*this)[Id].Offset.y    = atoi (strtok (NULL, TabSeparator));
            (*this)[Id].PalFilename = strtok (NULL, TabSeparator);

            if ((*this)[Id].Type>11 || (*this)[Id].Group>0 || gShowAllPools)
               (*this)[Id].TodayInGame = TRUE;
            else
               (*this)[Id].TodayInGame = FALSE;

            //Alte Frau weiter nach hinten sortieren:
            if ((*this)[Id].Group==70) (*this)[Id].FloorOffset=-15;
            else                       (*this)[Id].FloorOffset=0;

            (*this)[Id].GimmickTime = 0;
         }
         else if (ShadowPass==1)
         {
            //Spalte 1-7 ignorieren
            strtok (Line, TabSeparator);
            for (c=0; c<7; c++) strtok (NULL, TabSeparator);
            
            (*this)[Id].GimmickOffset.x   = atoi (strtok (NULL, TabSeparator));
            (*this)[Id].GimmickOffset.y   = atoi (strtok (NULL, TabSeparator));
            (*this)[Id].ShadowOffset.x    = atoi (strtok (NULL, TabSeparator));
            (*this)[Id].ShadowOffset.y    = atoi (strtok (NULL, TabSeparator));

            strtok (NULL, TabSeparator);
         }
         else if (ShadowPass==2)
         {
            //Spalte 1-9 ignorieren
            strtok (Line, TabSeparator);
            for (c=0; c<9; c++) strtok (NULL, TabSeparator);
            
            (*this)[Id].SkelettOffset.x    = atoi (strtok (NULL, TabSeparator));
            (*this)[Id].SkelettOffset.y    = atoi (strtok (NULL, TabSeparator));

            strtok (NULL, TabSeparator);
         }

         for (c=0; c<10+4; c++)
         {
            if (c==8+4)
            {
               DirPhaseLists[c]     = strtok (NULL, TabSeparator);
               PhaseListsNumbers[c] = -1;
            }
            else
            {
               if (c<4+4)             strtok (NULL, TabSeparator);
               DirPhaseLists[c]     = strtok (NULL, TabSeparator);
               PhaseListsNumbers[c] = atoi(strtok (NULL, TabSeparator));
            }
         }

         SBBMS *pPhasen;

         for (c=0; c<10+4; c++)
         {
            SLONG d;  //Böser Patch, damit das Laufen erst ab Index 10 kommt, obwohl es in der Excel-Tabelle schon viel eher steht:
            if (c<4) d=c;
            else if (c<8) d=10+c-4;
            else d=c-4;

            if (ShadowPass==1)
            {
               if (PhaseListsNumbers[c]==-1)
                  (*this)[Id].Shadow[d].ReSize (pGLibStd, DirPhaseLists[c]);
               else
                  (*this)[Id].Shadow[d].ReSize (pGLibStd, DirPhaseLists[c], PhaseListsNumbers[c]);
            }
            else
            {
               if (ShadowPass==0) pPhasen=&(*this)[Id].Phasen[d];
               if (ShadowPass==2) pPhasen=&(*this)[Id].Skelett[d];

               BUFFER<__int64> *pIds=NULL;
               SLONG            Anz;

               if (ShadowPass==0) pIds=&(*this)[Id].PhasenIds[d];
               if (ShadowPass==2) pIds=&(*this)[Id].SkelettIds[d];

               if (PhaseListsNumbers[c]==-1)
               {
                  BUFFER<char>  Str (strlen(DirPhaseLists[c])+1);
                  char         *Texts[100];

                  strcpy (Str, DirPhaseLists[c]);

                  for (Anz=0; ; Anz++)
                  {
                     if (Anz==0) Texts[Anz]=strtok (Str, " ");
                            else Texts[Anz]=strtok (NULL, " ");
                     if (!Texts[Anz]) break;
                  }

                  (*pIds).ReSize (Anz);

                  for (SLONG cc=0; cc<Anz; cc++)
                  {
                     (*pIds)[cc]=0;
                     for (SLONG d=0; d<SLONG(strlen(Texts[cc])); d++)
                        (*pIds)[cc]+=__int64((Texts[cc])[d])<<(8*d);
                  }

                  //if (gUpdatingPools) pPhasen->ReSize (pGLibClan, DirPhaseLists[c]);
               }
               else
               {
                  unsigned long long id=GetIdFromString(DirPhaseLists[c]);
                  unsigned long long add;

                       if (id>=0x01ff00ff00ff00ff) add=7;
                  else if (id>=0x01ff00ff00ff00)   add=6;
                  else if (id>=0x01ff00ff00ff)     add=5;
                  else if (id>=0x01ff00ff00)       add=4;
                  else DebugBreak();

                  (*pIds).ReSize (PhaseListsNumbers[c]);
                  for (SLONG e=0; e<PhaseListsNumbers[c]; e++)
                  {
                     (*pIds)[e]=id;

                     id+=1ull<<(add*8);

                     if (((id>>(add*8))&0xff)=='9'+1)
                     {
                        id-=10ull<<(add*8);
                        id+=1ull<<((add-1)*8);
                     }
                  }

                  //if (gUpdatingPools) pPhasen->ReSize (pGLibClan, DirPhaseLists[c], PhaseListsNumbers[c]);
               }
            }
         }
      }
   }
}

//--------------------------------------------------------------------------------------------
//Tut die Bitmaps dann geladen machen:
//--------------------------------------------------------------------------------------------
void CLANS::LoadBitmaps (void)
{
   SLONG c, n;

   for (n=AnzEntries()-1; n>=0; n--)
      if (IsInAlbum(n))
      {
         CLAN &qClan = (*this)[n];

         for (c=0; c<10+4; c++)
         {
            /*SLONG d;  //Böser Patch, damit das Laufen erst ab Index 10 kommt, obwohl es in der Excel-Tabelle schon viel eher steht:
            if (c<4) d=c;
            else if (c<8) d=10+c-4;
            else d=c-4; */

            if (gUpdatingPools)
            {
               qClan.Phasen[c].ReSize (pGLibClan, qClan.PhasenIds[c]);
               qClan.Skelett[c].ReSize (pGLibClan, qClan.SkelettIds[c]);
            }
            else
            {
                    if (c==4) qClan.Phasen[c].ReSize (&qClan.ClanGimmick, qClan.PhasenIds[c]);
               else if (c==9) qClan.Phasen[c].ReSize (&qClan.ClanWarteGimmick, qClan.PhasenIds[c]);
                         else qClan.Phasen[c].ReSize (&qClan.ClanPool, qClan.PhasenIds[c]);
               qClan.Skelett[c].ReSize (&SkelettPool, qClan.SkelettIds[c]);

            }
         }
      }
}

//--------------------------------------------------------------------------------------------
//Blittet ein Person an Stelle x (als FootPos):
//--------------------------------------------------------------------------------------------
void CLAN::BlitAt (SBPRIMARYBM &Offscreen, SLONG Dir, SLONG Phase, XY ScreenPos, UBYTE Running)
{
   SBBM *pbm;
   XY    Size;
   SLONG localDir;
   SLONG localFaktor;

   if (ScreenPos.x+200<0 || ScreenPos.x>RightAirportClip+100 || !bActive) return;

   if (Dir==9) GimmickTime=Sim.TickerTime;
   if (Dir==4)
   {
      GimmickTime=Sim.TickerTime;
      ScreenPos+=GimmickOffset;
   }

   if (Dir<8 && Running)
   {
      localDir=Dir+10;
      localFaktor=Faktor*2;
   }
   else
   {
      localDir=Dir;
      localFaktor=Faktor;
   }

   if (Dir<8 || Dir==9)
   {
      if (Phase/localFaktor>=Phasen[localDir].AnzEntries()) Phase=0;
      pbm=&(Phasen[localDir])[Phase/localFaktor];
   }
   else
   {
      if (Phase>=Phasen[8].AnzEntries()) Phase=0;
      pbm=&(Phasen[8])[Phase];
   }
   
   if (pbm)
   {
      if (ScreenPos.x>-60 && ScreenPos.x<RightAirportClip+50)
      {
         bool bNotSecurity=true;

         //Bei 'Player' wird das Status-byte nicht normal verwendet. Hier ist es die Nummer des Spielers
         if (Type>=CLAN_PLAYER1 && Type<=CLAN_PLAYER4)
         {
            if (Sim.Players.Players[Type-CLAN_PLAYER1].DirectToRoom==ROOM_SECURITY || Sim.Players.Players[Type-CLAN_PLAYER1].ExRoom==ROOM_SECURITY)
               bNotSecurity=false;
         }

         //ggf. Schatten raussuchen und blitten:
         if (Type!=30 && Sim.Options.OptionSchatten && localDir!=5 && bNotSecurity)
         {
            SBBM *pbm;

            if (Dir<8 || Dir==9)
            {
               pbm=&(Shadow[localDir])[(Phase/localFaktor)%Shadow[localDir].AnzEntries()];
            }
            else
            {
               Phase%=Shadow[8].AnzEntries();
               pbm=&(Shadow[8])[Phase];
            }
   
            if (pbm && pbm->Size.x && pbm->Size.y)
            {
               Size = XY(pbm->Size.x/2, pbm->Size.y-1);

               ColorFX.BlitAlpha (pbm->pBitmap, &PrimaryBm.PrimaryBm, ScreenPos-Size+ShadowOffset /*qPerson.ScreenPos-ViewPos+XY(-7-gShadowBm.Size.x/2,-2)*/);
            }
         }

         Size = XY(pbm->Size.x/2, pbm->Size.y-1);

         //Person blitten:
         AnzPeopleOnScreen++;
         Offscreen.BlitFromT (*pbm, ScreenPos-Size+Offset);
      }
      else if (ScreenPos.x>-260 && ScreenPos.x<RightAirportClip+250)
         AnzPeopleOnScreen++;
   }
}

//--------------------------------------------------------------------------------------------
//Blittet ein Skelett an Stelle x (als FootPos):
//--------------------------------------------------------------------------------------------
void CLAN::BlitSkelettAt (SBPRIMARYBM &Offscreen, SLONG Dir, SLONG Phase, XY ScreenPos)
{
   SBBM *pbm;
   
   if (Dir<8)
   {
      if (Phase/Faktor>=Skelett[Dir].AnzEntries()) Phase=0;
      pbm=&(Skelett[Dir])[Phase/Faktor];
   }
   else
   {
      if (Phase>=Skelett[8].AnzEntries()) Phase=0;
      pbm=&(Skelett[8])[Phase];
   }
   
   if (pbm && ScreenPos.x>-60 && ScreenPos.x<RightAirportClip+50)
   {
      //Person blitten:
      XY Size = XY(pbm->Size.x/2, pbm->Size.y-1);

      if (Dir==3 || (Dir==8 && Phase==3))
         Offscreen.BlitFromT (*pbm, ScreenPos-Size+XY(-SkelettOffset.x,SkelettOffset.y)); //Nach westen
      else
         Offscreen.BlitFromT (*pbm, ScreenPos-Size+SkelettOffset);
   }
}

//--------------------------------------------------------------------------------------------
//Blittet ein Person GANZ GROSS an Stelle x (als FootPos):
//--------------------------------------------------------------------------------------------
void CLAN::BlitLargeAt (SBBM &Offscreen, SLONG Dir, SLONG Phase, XY ScreenPos)
{
   SBBM *pbm;
   XY    Size;
   
   if (Dir<8 || Dir==9)
   {
      if (Phase/Faktor>=Phasen[Dir].AnzEntries()) Phase=0;
      pbm=&(Phasen[Dir])[Phase/Faktor];
   }
   else
   {
      if (Phase>=Phasen[8].AnzEntries()) Phase=0;
      pbm=&(Phasen[8])[Phase];
   }
   
   if (pbm)
   {
      if (pbm->pBitmap)
      {
         if (ScreenPos.x>-60 && ScreenPos.x<700)
         {
            Size = XY(pbm->Size.x/2, pbm->Size.y-1)*SLONG(2);
            SDL_Rect SrcRect = { 0,0,pbm->pBitmap->GetXSize(),pbm->pBitmap->GetYSize() };
            SDL_Rect DestRect = { ScreenPos.x-Size.x, ScreenPos.y-pbm->pBitmap->GetYSize()*2, ScreenPos.x-Size.x+pbm->pBitmap->GetXSize()*2, ScreenPos.y };
            SDL_Rect ClipRect = Offscreen.pBitmap->GetSurface()->clip_rect;

            if (DestRect.w>ClipRect.w)
            {
               SrcRect.w-=(DestRect.w-ClipRect.w);
               DestRect.w=ClipRect.w;
            }
            if (DestRect.h>ClipRect.h)
            {
               SrcRect.h-=(DestRect.h-ClipRect.h);
               DestRect.h=ClipRect.h;
            }
            if (DestRect.x<ClipRect.x)
            {
               SrcRect.x+=(ClipRect.x-DestRect.x)/2;
               DestRect.x=ClipRect.x;
            }
            if (DestRect.y<ClipRect.y)
            {
               SrcRect.y+=(ClipRect.y-DestRect.y)/2;
               DestRect.y=ClipRect.y;
            }

            if (DestRect.h>0 && DestRect.w>0)
            {
                SDL_BlitScaled(pbm->pBitmap->GetSurface(), &SrcRect, Offscreen.pBitmap->GetSurface(), &DestRect);
            }
         }
      }
      else if (pbm->pHLObj)
      {
         ScreenPos.x-=pbm->Size.x;
         ScreenPos.y-=pbm->Size.y*2;

         pbm->pHLObj->BlitLargeAt (Offscreen.pBitmap, ScreenPos);
      }
   }
}

//--------------------------------------------------------------------------------------------
//Speichert den Zustand, welche Clans aktiv sind:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CLAN &Clan)
{
   File << Clan.TodayInGame;

   return File;
}

//--------------------------------------------------------------------------------------------
//Lädt den Zustand, welche Clans aktiv sind:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CLAN &Clan)
{
   BOOL tmp;

   File >> tmp;

   if (tmp && !Clan.TodayInGame)
   {
      Clan.ClanPool.Load();
      Clan.ClanGimmick.Load();
      Clan.ClanWarteGimmick.Load();
   }
   else if (tmp && !Clan.TodayInGame)
   {
      Clan.ClanPool.Unload();
      Clan.ClanGimmick.Unload();
      Clan.ClanWarteGimmick.Unload();
   }

   Clan.TodayInGame = tmp;

   return File;
}

//--------------------------------------------------------------------------------------------
//Gibt den Id eines Kunden zurück:  0=nur weiße; 1=beide; 2=nur braune
//--------------------------------------------------------------------------------------------
UBYTE CLANS::GetCustomerId (SLONG Browned, SLONG Koffer, TEAKRAND *pRand)
{
	SLONG Num, c, Rnd;

   if (CheatMoreNuns)
   {
      if (CheatMoreNuns==1) return (UBYTE(AnzEntries()-1));
      else if (CheatMoreNuns==2) return (GetCustomerIdByGroup (10));
      else if (CheatMoreNuns==3) return (GetCustomerIdByGroup (11));
   }

	Num = 0;

   for (c=0; c<(SLONG)AnzEntries(); c++)
      if (IsInAlbum (c) && (*this)[c].TodayInGame && (Koffer==sign((*this)[c].HasSuitcase) || (Koffer==99 && (*this)[c].HasSuitcase<=0)) && ((((*this)[c].Type==CLAN_FEMALE || (*this)[c].Type==CLAN_MALE) && Browned<2) || (((*this)[c].Type==CLAN_BROWNFEMALE || (*this)[c].Type==CLAN_BROWNMALE) && Browned>0)))
         Num+=(*this)[c].Wkeit;

   if (pRand) Rnd = pRand->Rand (Num);
	      else Rnd = PersonRandCreate.Rand (Num);

   Num = 0;

   for (c=0; c<(SLONG)AnzEntries(); c++)
   if (IsInAlbum (c) && (*this)[c].TodayInGame && (Koffer==sign((*this)[c].HasSuitcase) || (Koffer==99 && (*this)[c].HasSuitcase<=0)) && ((((*this)[c].Type==CLAN_FEMALE || (*this)[c].Type==CLAN_MALE) && Browned<2) || (((*this)[c].Type==CLAN_BROWNFEMALE || (*this)[c].Type==CLAN_BROWNMALE) && Browned>0)))
		{
			Num+=(*this)[c].Wkeit;
			if (Num>Rnd)
            return ((UBYTE)c);
		}

   TeakLibW_Exception (FNL, ExcNever);
   return (255);
}

//--------------------------------------------------------------------------------------------
//Gibt den Index einer entsprechenden Figur zurück:
//--------------------------------------------------------------------------------------------
UBYTE CLANS::GetCustomerIdByGroup (SLONG Group)
{
	SLONG Num, c, Rnd;

	Num = 0;

   for (c=0; c<(SLONG)AnzEntries(); c++)
      if (IsInAlbum (c) && (*this)[c].TodayInGame && Group==(*this)[c].Group)
         Num++;

   {
	   Rnd = PersonRandCreate.Rand (Num);

      Num = 0;

      for (c=0; c<(SLONG)AnzEntries(); c++)
         if (IsInAlbum (c) && (*this)[c].TodayInGame && Group==(*this)[c].Group)
		   {
			   Num++;
			   if (Num>Rnd)
            {
               return ((UBYTE)c);
            }
		   }
   }

   TeakLibW_Exception (FNL, ExcNever);
   return (255);
}

//--------------------------------------------------------------------------------------------
//Gibt den Index des entsprechenden koffers zurück:
//--------------------------------------------------------------------------------------------
UBYTE CLANS::GetSuitcaseId (SLONG Koffer)
{
   for (SLONG c=0; c<(SLONG)AnzEntries(); c++)
      if (IsInAlbum (c) && (*this)[c].Type==30 && (*this)[c].HasSuitcase==Koffer)
         return ((UBYTE)c);

   TeakLibW_Exception (FNL, ExcNever);
   return (255);
}

//--------------------------------------------------------------------------------------------
//Gibt den Index des erstbesten Clans mit diesem Typ zurück (gut für Player)
//--------------------------------------------------------------------------------------------
UBYTE CLANS::GetPlayerId (ULONG PlayerType)
{
   for (SLONG c=0; c<(SLONG)AnzEntries(); c++)
      if (IsInAlbum (c) && (*this)[c].Type==PlayerType)
         return ((UBYTE)c);

   TeakLibW_Exception (FNL, ExcNever);
   return (255);
}

//--------------------------------------------------------------------------------------------
//Sucht neue Figuren aus, die im Spiel erscheinen:
//--------------------------------------------------------------------------------------------
void CLANS::UpdateClansInGame (BOOL FirstDay)
{
   SLONG c, d, n, r, anz;
   SLONG AnzAdded=0, AnzRemoved=0;

   if (gShowAllPools) return;

   for (anz=c=0; c<(SLONG)AnzEntries(); c++)
      if (IsInAlbum (c) && (*this)[c].Type<=11 && (*this)[c].Group<=0 && (*this)[c].HasSuitcase>=0)
         anz++;

   //10 Personen an- oder ausknippsen
   for (n=0; n<10; n++)
   {
again:
      r=PersonRandCreate.Rand (anz);

      for (SLONG anz=c=0; c<(SLONG)AnzEntries(); c++)
         if (IsInAlbum (c) && (*this)[c].Type<=11 && (*this)[c].Group<=0 && (*this)[c].HasSuitcase>=0)
         {
            if ((r--)==0)
            {
               if ((*this)[c].Wkeit==1 && PersonRandCreate.Rand(20)!=0) goto again;

               if ((*this)[c].TodayInGame>=128)
               {
                  n--;     //This person was already flipped today; skip it
                  break;
               }

               if (!FirstDay)
               {
                  if (((*this)[c].TodayInGame && AnzRemoved>=5) || ((*this)[c].TodayInGame==FALSE && AnzAdded>=5))
                  {
                     n--;     //Wir haben schon genug Personen gelöscht/hinzugefügt
                     break;
                  }
               }

               if ((*this)[c].TodayInGame)
               {
                  if (!FirstDay)
                  {
                     (*this)[c].ClanPool.Unload();
                     (*this)[c].ClanGimmick.Unload();
                     (*this)[c].ClanWarteGimmick.Unload();
                  }
                  (*this)[c].TodayInGame = 0 + 128;

                  AnzRemoved++;

                  if ((*this)[c].HasSuitcase)
                     for (d=0; d<(SLONG)AnzEntries(); d++)
                        if (IsInAlbum (d) && (*this)[d].Type<=11 && (*this)[d].Group<=0 && (*this)[c].HasSuitcase==-(*this)[d].HasSuitcase)
                        {
                           if (!FirstDay)
                           {
                              (*this)[d].ClanPool.Unload();
                              (*this)[d].ClanGimmick.Unload();
                              (*this)[d].ClanWarteGimmick.Unload();
                           }
                           (*this)[d].TodayInGame = 0 + 128;
                        }
               }
               else
               {
                  if (!FirstDay)
                  {
                     (*this)[c].ClanPool.Load();
                     (*this)[c].ClanGimmick.Load();
                     (*this)[c].ClanWarteGimmick.Load();
                  }
                  (*this)[c].TodayInGame = 1 + 128;

                  AnzAdded++;

                  if ((*this)[c].HasSuitcase)
                     for (d=0; d<(SLONG)AnzEntries(); d++)
                        if (IsInAlbum (d) && (*this)[d].Type<=11 && (*this)[d].Group<=0 && (*this)[c].HasSuitcase==-(*this)[d].HasSuitcase)
                        {
                           if (!FirstDay)
                           {
                              (*this)[d].ClanPool.Load();
                              (*this)[d].ClanGimmick.Load();
                              (*this)[d].ClanWarteGimmick.Load();
                           }
                           (*this)[d].TodayInGame = 1 + 128;
                        }
               }
            }
         }
   }

   //Clear the "flipped today" flag:
   for (anz=c=0; c<(SLONG)AnzEntries(); c++)
      if (IsInAlbum (c))
         (*this)[c].TodayInGame &= 127;
}

//--------------------------------------------------------------------------------------------
//Sucht den ClanId einer bestimmten Person raus:
//--------------------------------------------------------------------------------------------
SLONG CLANS::GetAnimationId (SLONG AnimationGroup)
{
   for (SLONG c=0; c<(SLONG)AnzEntries(); c++)
      if (IsInAlbum (c) && (*this)[c].Type==CLAN_ANIMATION && (*this)[c].Group==AnimationGroup)
         return (c);

   TeakLibW_Exception (FNL, ExcNever);
   return (255);
}

//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
PERSON::PERSON ()
{
   ClanId=255;

   //15.11.2001: Hat aus irgendeineinem Grund Asynchronitäten im Spiel verursacht. Darf zwar eigentlich nicht sein. War aber so:
   //PersonalRand.SRand (Sim.TimeSlice);
   //PersonalRandWalk.SRand (Sim.TimeSlice);

   PersonalRand.SRand (Sim.GetHour()+Sim.GetMinute()/5);
   PersonalRandWalk.SRand (Sim.GetHour()+Sim.GetMinute()/5);
}

//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
PERSON::PERSON (UBYTE ClanId, XY Position, UBYTE Reason, UBYTE FlightAirline, SLONG FlightPlaneId, UBYTE FlightPlaneIndex, UBYTE Mood, UBYTE FirstClass)
{
   //Default-Mood
   if ((Clans[SLONG(ClanId)].Type<=CLAN_PLAYER1 || Clans[SLONG(ClanId)].Type>=CLAN_PLAYER4) && Mood==0)
      Mood=(UBYTE)MoodPersonNone;

   PERSON::ClanId           = ClanId;
   PERSON::Dir              = 2;          //Geht nach Süden
   PERSON::LookDir          = 0;
   PERSON::Phase            = 0;
   PERSON::WaitCount        = 0;
   PERSON::Mood             = Mood;
   PERSON::MoodCountdown    = 0;
   PERSON::Reason           = Reason;
   PERSON::FlightAirline    = FlightAirline;
   PERSON::FlightPlaneId    = FlightPlaneId;
   PERSON::FlightPlaneIndex = FlightPlaneIndex;
   PERSON::Position         = Position;
   PERSON::State            = 0;
   PERSON::StatePar         = 0;
   PERSON::Running          = 0;
   PERSON::FirstClass       = FirstClass;
   PERSON::FloorOffset      = Clans[SLONG(ClanId)].FloorOffset;

   //15.11.2001: Hat aus irgendeineinem Grund Asynchronitäten im Spiel verursacht. Darf zwar eigentlich nicht sein. War aber so:
   //PERSON::PersonalRand.SRand (Sim.TimeSlice);
   //PERSON::PersonalRandWalk.SRand (Sim.TimeSlice);

   PERSON::PersonalRand.SRand (Sim.GetHour()+Sim.GetMinute()/5+ClanId);
   PERSON::PersonalRandWalk.SRand (Sim.GetHour()+Sim.GetMinute()/5+ClanId);

   ScreenPos.x = Position.x-Position.y/2+90;
   ScreenPos.y = Position.y+220;

   //Die Person ist aus einem bestimmten Grund am Flughafen. Und daraus ergibt sich ein
   switch (PERSON::Reason)  //Sekundärziel
   {            
      case 0:
         if (Clans[(SLONG)ClanId].Type<CLAN_PLAYER1 || Clans[(SLONG)ClanId].Type>CLAN_PLAYER4)
            DebugBreak();
         break;

      case REASON_SHOPPING:
         PERSON::State  = PERSON_2SHOP;
         PERSON::Target = Position+XY(0,-22);
         Airport.GetRandomShop (Target, StatePar, Clans[(SLONG)ClanId].Type, &(PERSON::Mood), &PersonRandCreate);  //Der Shop wird in StatePar gespeichert
         break;

      //Person kommt mit dem Flugzeug an:
      case REASON_LEAVING:
         PERSON::State  = PERSON_LEAVINGPL;
         PERSON::Target = Position+XY(88,0);
         break;

      case REASON_FLYING:
         //Sicherheitsabfrage, weil durch einen Bug evtl. woanders falsche Werte reinkamen:
         if (GetFlugplanEintrag()->Gate==-2)
         {
            //Bad, remove Person again soo:
            PERSON::State  = PERSON_LEAVING;
            PERSON::Target = Position;
            Airport.GetRandomShop (Target, StatePar, Clans[(SLONG)ClanId].Type, &(PERSON::Mood), &PersonRandCreate);  //Der Shop wird in StatePar gespeichert
         }
         else
         {
            //Good:
            PERSON::State  = PERSON_2CHECKIN;
            PERSON::Target = Airport.GetRandomTypedRune (RUNE_2CHECKIN, (UBYTE)GetFlugplanEintrag()->Gate, false, &PersonRandCreate);
         }
         break;

      //Der Koffer fährt so durch die Gegend:
      case REASON_SUITCASE_EXIT:
         {
            PERSON::Dir    = 0;
            PERSON::State  = PERSON_BROWSING;
            PERSON::Target = Airport.GetRandomTypedRune (RUNE_EXIT_SUITCASE, 0, false, &PersonRandCreate);
            BUILD *pBuild=Airport.GetBuildNear (ScreenPos, XY(180,80), Bricks((SLONG)0x10000000+BRICK_KASTEN));
            if (pBuild) Airport.Triggers[(SLONG)pBuild->Par].Winkel=Sim.TickerTime;
         }
         break;

      //Person klappert WayPoints ab:
      case REASON_WAYPOINT:
         PERSON::State  = PERSON_WAYPOINTING;
         PERSON::Target = Airport.GetRandomTypedRune (RUNE_WAYPOINT, FlightAirline, false, &PersonRandCreate);
         break;

      default:
         DebugBreak();
   }

   //Bei 'Player' wird das Status-byte nicht normal verwendet. Hier ist es die Nummer des Spielers
   if (Clans[(SLONG)ClanId].Type>=CLAN_PLAYER1 && Clans[(SLONG)ClanId].Type<=CLAN_PLAYER4)
   {
      PERSON::State=UBYTE(Clans[(SLONG)ClanId].Type-CLAN_PLAYER1);

      //Außerdem werden die IQ-Walk Variablen initialisiert:
      Sim.Players.Players[(SLONG)PERSON::State].iWalkActive = FALSE;
   }
}

//--------------------------------------------------------------------------------------------
//Verwaltet die Extra-Animationen (Stinkbombe, Klebstoff):
//--------------------------------------------------------------------------------------------
void PERSON::DoOneAnimationStep (void)
{
   CLAN &qClan = Clans[(SLONG)ClanId];
   XY    ArrayPos;
   bool  Upfloor;

   //Obere oder untere Ebene?
   if (Position.y<4000)
   {
      //unten!
      ArrayPos.x = Position.x/44;
      ArrayPos.y = Position.y/22+5;
      Upfloor = false;
   }
   else
   {
      //oben!
      ArrayPos.x = Position.x/44;
      ArrayPos.y = (Position.y-5000)/22;
      Upfloor = true;
   }

   switch (qClan.Group)
   {
      //Klebstoff:
      case 10:
         Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)] |= 1;
         break;

      //Stinkbombe:
      case 20:
         {
            SLONG x;

            if (State!=255)
               if (Smokers[State].Smoke.AnzEntries())
               {
                  for (x=-1; x<=1; x++)
                  {
                     Airport.iPlate[ArrayPos.y+((ArrayPos.x+x)<<4)] |= 1;
                     if (ArrayPos.y>5)  Airport.iPlate[ArrayPos.y-1+((ArrayPos.x+x)<<4)] |= 1;
                     if (ArrayPos.y<15) Airport.iPlate[ArrayPos.y+1+((ArrayPos.x+x)<<4)] |= 1;
                  }

                  if (Smokers[State].Smoking>0)
                  {
                     Smokers[State].Smoking--;
                     if (Smokers[State].Smoking==0)
                     {
                        Smokers[State].Smoke.ReSize(0);
                        State=255;

                        SLONG x;

                        for (x=-1; x<=1; x++)
                        {
                           Airport.iPlate[ArrayPos.y+((ArrayPos.x+x)<<4)] &= (~1);
                           if (ArrayPos.y>5)  Airport.iPlate[ArrayPos.y-1+((ArrayPos.x+x)<<4)] &= (~1);
                           if (ArrayPos.y<15) Airport.iPlate[ArrayPos.y+1+((ArrayPos.x+x)<<4)] &= (~1);
                        }
                     }
                  }
               }
         }
         break;
   }

   //Obere oder untere Ebene?
   if (Upfloor)
   {
      //oben!
      ScreenPos.x = Position.x-(Position.y-5000)/2+86;
      ScreenPos.y = (Position.y-5000)+93;
   }
   else
   {
      //unten!
      ScreenPos.x = Position.x-Position.y/2+86;
      ScreenPos.y = Position.y+220;
   }
}

//--------------------------------------------------------------------------------------------
//Eine Person (Kunde) geht einen Schritt weiter:
//--------------------------------------------------------------------------------------------
void PERSON::DoOneCustomerStep (void)
{
   CLAN &qClan = Clans[(SLONG)ClanId];
   XY    StepSize (qClan.WalkSpeed,qClan.WalkSpeed);
   XY    ArrayPos, NewArrayPos;
   XY    NewPos;

   UBYTE LastPhase=Phase;

   //Serge's Alte Sau (TM) steht nicht früh auf.
   if (qClan.Group==70 && Sim.GetHour()<10) return;

   //Die Post kommt um 11:
   if (qClan.Group==90 && (Sim.GetHour()<13 || Sim.Weekday==5 || Sim.Weekday==6)) return;

   if ((State&PERSON_WAITFLAG) && (State & (~PERSON_WAITFLAG))==PERSON_BOARDING && WaitCount<=250) return;

   if (LookDir==8) //Warten
   {
      Dir     = Phase; //4;
      LookDir = Phase;
      Phase   = 0;
   }
   else if (LookDir==4) //Gimmick-Animation
   {
      Phase++;

      if (Phase<(qClan.Phasen[(SLONG)LookDir].AnzEntries()*qClan.Faktor))
         return;

      if (qClan.Group==70)
      {
         //Serge's Alte Sau (TM)
         Phase=UBYTE(18*qClan.Faktor);
         return;
      }
      else
      {
         Dir=LookDir=UBYTE(qClan.GimmickArt1);
         Phase=0;
      }
   }
   else if (LookDir==9) //Warte-Gimmick-Animation
   {
      Phase++;

      if (Clans[(SLONG)ClanId].Type>=CLAN_PLAYER1 && Clans[(SLONG)ClanId].Type<=CLAN_PLAYER4)
      {
         if (Position==Target)
         {
            if (Phase<(qClan.Phasen[(SLONG)LookDir].AnzEntries()*qClan.Faktor))
               return;

            Phase=0;
            return;
         }
      }
      else
      {
         if (Phase<(qClan.Phasen[(SLONG)LookDir].AnzEntries()*qClan.Faktor))
            return;
      }
      
      Dir=LookDir=UBYTE(qClan.GimmickArt2);
      Phase=0;
   }

   State &= (~PERSON_WAITFLAG);

   //Ist die Person am Ziel ihrer Träume? Dann weiter zum nächsten Checkpoint!
   if (abs (Position.x-Target.x)<=StepSize.x && abs (Position.y-Target.y)<=StepSize.y)
      if ((State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))!=PERSON_WAITING)
      {
         if ((State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG)) == PERSON_EXSHOP || (State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG)) == PERSON_2EXIT)
         {
            PersonReachedTarget ();
         }
         else if ((State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG)) != PERSON_CHECKINGIN)
         {
            PersonReachedTarget ();
            if (LookDir==4) //Gimmick-Animation für WayPoint-Figur?
            {
               ArrayPos.x = (Position.x+4400)/44-100;
               ArrayPos.y = (Position.y+2200)/22-100;
               Airport.iPlate[ArrayPos.y+5+(ArrayPos.x<<4)] |= 2;
               Airport.iPlateDir[ArrayPos.y+5+(ArrayPos.x<<4)] = LookDir;
               return;
            }

            if (WaitCount)
            {
               if (Dir!=8)
                  Phase   = UBYTE((Phase+1)%(qClan.Phasen[(SLONG)LookDir].AnzEntries()*qClan.Faktor));

               return;
            }
         }
      }

   ArrayPos.x = (Position.x+4400)/44-100;
   ArrayPos.y = (Position.y+2200)/22-100;

   //Stinkt es hier?
   if (ArrayPos.y>=0 && ArrayPos.y<=9 && Mood!=MoodPersonSick && Airport.iPlate[ArrayPos.y+5+(ArrayPos.x<<4)]&1)
   {
      if (Mood==MoodPersonBone || Mood==MoodPersonStenchDog)
      {
         Mood=UBYTE(MoodPersonStenchDog);

         if (MoodCountdown) MoodCountdown=max(MoodCountdown,MOODCOUNT_START-16);
         else               MoodCountdown=MOODCOUNT_START+rand()%7;
      }
      else if (Reason==REASON_SHOPPING || Reason==REASON_WAYPOINT)
      {
         Mood=UBYTE(MoodPersonStench);

         if (MoodCountdown) MoodCountdown=max(MoodCountdown,MOODCOUNT_START-16);
         else               MoodCountdown=MOODCOUNT_START+rand()%7;
      }
      else if (Reason==REASON_FLYING || Reason==REASON_LEAVING)
      {
         Mood=UBYTE(MoodPersonSick);
         MoodCountdown=MOODCOUNT_START+rand()%15;

         if (FlightAirline>=0 && FlightAirline<=3)
            if ((ClanId&3)==0)
            {
               Sim.Players.Players[(SLONG)FlightAirline].Image--;
               //log: hprintf ("Player[%li].Image now (stench) = %li", (SLONG)FlightAirline, Sim.Players.Players[(SLONG)FlightAirline].Image);

               if (Sim.Players.Players[(SLONG)FlightAirline].Image<-1000)
                  Sim.Players.Players[(SLONG)FlightAirline].Image=-1000;
            }
      }
   }

   if ((State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))!=PERSON_2CHECKIN_EXIT)
      if (ArrayPos.y>=0 && ArrayPos.y<=9)
      {
         Airport.iPlate[ArrayPos.y+5+(ArrayPos.x<<4)] &= (~2);
         Airport.iPlateDir[ArrayPos.y+5+(ArrayPos.x<<4)] = 255;
      }

   NewPos = Position;

   //Und daher gehen wir per default einfach geradeaus weiter:
   switch (Dir)
   {
      case 0: NewPos.y = Position.y - StepSize.y; break;
      case 1: NewPos.x = Position.x + StepSize.x; break;
      case 2: NewPos.y = Position.y + StepSize.y; break;
      case 3: NewPos.x = Position.x - StepSize.x; break;
   }

   //Hoppla! Sind wir etwa just über die Mittelline einer Bodenplatte gelaufen?
   if ((NewPos.x+4422)/44!=(Position.x+4422)/44 || (NewPos.y+2211)/22!=(Position.y+2211)/22 || Dir==4)
   {
      //Sind wir! Und deshalb ist es Zeit sich umzuschauen und eine Entscheidung zu treffen.
      //Welche Möglichkeiten hat der Kunde dieses Feld zu verlassen?
      UBYTE NewPossibleDirs;
      
      //Müssen wir erst einmal eine Tür aufstoßen?
      if (ArrayPos.y<2) Airport.TryDoor (XY(ArrayPos.x,ArrayPos.y+5), 0, 0);

      if (ArrayPos.y>=0 && ArrayPos.y<=9)
         NewPossibleDirs = UBYTE((Airport.iPlate[ArrayPos.y+5+(ArrayPos.x<<4)] & 240) & (~(128>>((Dir+2)&3))));
      else
         NewPossibleDirs = 128+32;

      UBYTE AnzPossibleDirs = UBYTE(((NewPossibleDirs&16)!=0) + ((NewPossibleDirs&32)!=0) + ((NewPossibleDirs&64)!=0) + ((NewPossibleDirs&128)!=0));

      //Im Warteraum einen Sitz suchen:
      if ((State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))==PERSON_WAITING)
      {
         if (ArrayPos.y==0)
         {
            NewPossibleDirs &= (~128);
            AnzPossibleDirs = UBYTE(((NewPossibleDirs&16)!=0) + ((NewPossibleDirs&32)!=0) + ((NewPossibleDirs&64)!=0) + ((NewPossibleDirs&128)!=0));
         }

         if (ArrayPos.y>0 && ArrayPos.y<7)
         {
            if ((Airport.SeatsTaken[ArrayPos.x]&(1<<((ArrayPos.y-1)*2+1)))==0)
            {
               //Person setzt sich und schaut nach Süden:
               Airport.SeatsTaken[ArrayPos.x]|=(1<<((ArrayPos.y-1)*2+1));
               State=PERSON_SITWAITING|PERSON_WAITFLAG;

               Position.x=(Position.x+4444)/44*44-4444+22;
               Position.y=(Position.y+2222)/22*22-2222+11;

               Position.y-=3;
               Dir=LookDir=7; Phase=0;
               return;
            }
            else if ((Airport.SeatsTaken[ArrayPos.x]&(1<<((ArrayPos.y+1)*2)))==0)
            {
               //Person setzt sich und schaut nach Norden:
               Airport.SeatsTaken[ArrayPos.x]|=(1<<((ArrayPos.y+1)*2));
               State=PERSON_SITWAITING|PERSON_WAITFLAG;

               Position.x=(Position.x+4444)/44*44-4444+22;
               Position.y=(Position.y+2222)/22*22-2222+11;

               Position.y+=3;
               Dir=LookDir=6; Phase=0;
               return;
            }
         }
      }

      if ((State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))==PERSON_WAYPOINTING)
         goto DirtyWaypoingShortcut;

      //Am Check-In warten?
      if ((State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))==PERSON_CHECKINGIN &&
          abs (Position.y-Target.y)<=StepSize.y && abs (Position.x-Target.x)<=StepSize.x)
      {
         Dir=4;
         if (PersonalRandWalk.Rand(100)==0) PersonReachedTarget();
      }
      //Gibt es außer der Herkunft nur einen Ausgang? Dann gehen wir dahin!
      else if (AnzPossibleDirs==1)
      {
         switch (NewPossibleDirs)
         {
            case 16: Dir = 3; break;
            case 32: Dir = 2; break;
            case 64: Dir = 1; break;
            case 128: Dir = 0; break;
         }
      }
      //Oder gibt es gar keinen Ausgang? Dann sind wir in einer Sackgasse und drehen um!
      else if (AnzPossibleDirs==0)
      {
         if (ArrayPos.y<0 && ArrayPos.y>9) TeakLibW_Exception (FNL, ExcNever);

         SLONG death=0;

         NewPos = Position;
         do
         {
            Dir = UBYTE((Dir+1)&3);
            death++;
         }
         while (death<10 && (Airport.iPlate[ArrayPos.y+5+(ArrayPos.x<<4)] & (128>>Dir))==0);

         if (death>=10) State=PERSON_LEAVING;
      }
      //Tja, wer die Wahl hat hat die Qual:
      else
      {
      DirtyWaypoingShortcut:
         if (ArrayPos.y<0 && ArrayPos.y>9) TeakLibW_Exception (FNL, ExcNever);

         switch (State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))
         {
            //Frisch aus der Nervenheilanstalt läuft dieser Typ nur herum wie doof:
            case PERSON_BROWSING:
               do
               {
                  Dir = UBYTE(PersonalRandWalk.Rand(4));
               }
               while ((NewPossibleDirs & (128>>Dir))==0);
               break;

            //Person läuft nervös im Warteraum hin und her:
            case PERSON_WAITING:
               if (ArrayPos.y==4 && (NewPossibleDirs&64) && !(Airport.SeatsTaken[ArrayPos.x+3]&(1<<((ArrayPos.y-1)*2+1)))) Dir=1;
               else if (ArrayPos.y==2 && (NewPossibleDirs&64) && (!(Airport.SeatsTaken[ArrayPos.x+3]&(1<<((ArrayPos.y+1)*2))) || (!(Airport.SeatsTaken[ArrayPos.x+2]&(1<<((ArrayPos.y+1)*2))))) ) Dir=1;
               else do
               {
                  waiting_again:
                  Dir = UBYTE(PersonalRandWalk.Rand(4));
               }
               while ((NewPossibleDirs & (128>>Dir))==0);

               if (ArrayPos.y==4 && Dir==2) goto waiting_again;
               if (ArrayPos.y==0 && Dir==0) goto waiting_again;
               break;

            //Eine Person läuft innerhalb eines Ladens nur in x-richtung:
            case PERSON_SHOPPING:
               if (Position.y==Target.y && abs(Position.x-Target.x)<=44*3)
               {
                  if (Position.x<Target.x) Dir = 1;
                  else if (Position.x>Target.x) Dir = 3;
                  if (abs (Position.x-Target.x)<=StepSize.x)
                  {
                     Position.x = Target.x;
                     Dir = 4;
                  }
               }
               else
               {
                  if (Position.y<Target.y) Dir = 2;
                  else if (Position.y>Target.y) Dir = 0;
                  if (abs (Position.y-Target.y)<=StepSize.y)
                  {
                     Position.y = Target.y;
                     Dir = 4;
                  }
               }
               break;

            case PERSON_EXSHOP:
               if (Position.x==Target.x)
               {
                  if (Position.y<Target.y) Dir = 2;
                  else if (Position.y>Target.y) Dir = 0;
                  if (abs (Position.y-Target.y)<=StepSize.y)
                  {
                     Position.y = Target.y;
                     Dir = 4;
                  }
               }
               else
               {
                  if (Position.x<Target.x) Dir = 1;
                  else if (Position.x>Target.x) Dir = 3;
                  if (abs (Position.x-Target.x)<=StepSize.x)
                  {
                     Position.x = Target.x;
                     Dir = 4;
                  }
               }
               break;

            //Person will zum nächsten Waypoint:
            case PERSON_WAYPOINTING:
               if (FlightPlaneId>1)
               {
                  Phase=0;
                  Dir=4;

                  FlightPlaneId--;
                  if (FlightPlaneId==1) FlightPlaneId=0;
               }
               else if (Position.x==Target.x)
               {
                  if (Position.y<Target.y) Dir = 2;
                  else if (Position.y>Target.y) Dir = 0;
                  if (abs (Position.y-Target.y)<=StepSize.y)
                  {
                     Position.y = Target.y;

                     if (Position.y<Target.y) Dir = 2;
                     else if (Position.y>Target.y) Dir = 0;
                     if (abs (Position.y-Target.y)<=StepSize.y)
                     {
                        Position.y = Target.y;
                        Dir = 4;
                     }
                  }
               }
               else
               {
                  if (Position.x<Target.x) Dir = 1;
                  else if (Position.x>Target.x) Dir = 3;
                  if (abs (Position.x-Target.x)<=StepSize.x)
                  {
                     Position.x = Target.x;

                     if (Position.y<Target.y) Dir = 2;
                     else if (Position.y>Target.y) Dir = 0;
                     if (abs (Position.y-Target.y)<=StepSize.y)
                     {
                        Position.y = Target.y;
                        Dir = 4;
                     }
                  }
               }
               break;

            //Sucht die Person ein bestimmtes Ziel?
            case PERSON_2EXIT:
            case PERSON_2CHECKIN:
            case PERSON_2WAITROOM:
            case PERSON_2SHOP:
               if (Position.x==Target.x)
               {
                  if (Position.y<Target.y) Dir = 2;
                  else if (Position.y>Target.y) Dir = 0;
                  if (abs (Position.y-Target.y)<=StepSize.y)
                  {
                     Position.y = Target.y;
                     //Dir = 4;

                     if (Position.y<Target.y) Dir = 2;
                     else if (Position.y>Target.y) Dir = 0;
                     if (abs (Position.y-Target.y)<=StepSize.y)
                     {
                        Position.y = Target.y;
                        Dir = 4;
                     }
                  }
               }
               else
               {
                  SLONG c, ty;

                  if (Position.x<Target.x) //nach rechts
                  {
                     BOOL  bFast          = StepSize.y>4;
                     BOOL  bAussenErlaubt = (Airport.iPlate[14+(ArrayPos.x<<4)]&64) && (Airport.iPlate[14+((ArrayPos.x+1)<<4)]&64) && (Airport.iPlate[14+((ArrayPos.x-1)<<4)]&64);

                     if (bFast && bAussenErlaubt && (ArrayPos.y+5==14 || (Airport.iPlate[13+((ArrayPos.x+1)<<4)]&2) && Target.x-Position.x>88))
                        ty = 209;    //Schnelle Clans überholen außen, wenn jemand vor ihnen ist...
                     else
                     {
                        if (Airport.iPlateDir[13+((ArrayPos.x)<<4)]==8 || Airport.iPlateDir[13+((ArrayPos.x+1)<<4)]==8 || (bFast && Airport.iPlate[13+((ArrayPos.x+1)<<4)]&2 && Target.x-Position.x>88) || (StepSize.x>1 && (Airport.iPlateDir[13+((ArrayPos.x)<<4)]==254 || Airport.iPlateDir[13+((ArrayPos.x+1)<<4)]==254))) //Falls der Weg blockiert ist....
                        {
                           if (bAussenErlaubt)
                              ty = 209; //Jeder überholt hier außen...
                           else
                           {
                              //Prüfen, ob auf der gemeinsamen Innenspur Platz ist...
                              for (c=0; c<5; c++)
                                 if (((Airport.iPlateDir[12+((ArrayPos.x+c)<<4)]==3 || Airport.iPlateDir[12+((ArrayPos.x+c)<<4)]==254) && (Airport.iPlate[12+((ArrayPos.x+c)<<4)]&2)) || (Airport.iPlate[12+((ArrayPos.x+c)<<4)]&64)==0)
                                    break;

                              if (c==5)
                                 ty = 165; //alles frei, wir überholen innen
                              else
                                 ty = 187; //normale Spur, weil jemand kommt
                           }
                        }
                        else //Nein, der Weg ist nicht blockiert, Aber ggf. behalten wir unsere Überholspur noch ein wenig...
                        {
                           if (ArrayPos.y+5==14 && Dir==1 && bAussenErlaubt && (Airport.iPlate[14+((ArrayPos.x+1)<<4)]&2)==0)
                              ty = 209; //Es ist Platz, also überholen wir außen...
                           else if (ArrayPos.y+5==12 && (Dir==1 || Dir==0))
                           {
                              //Prüfen, ob auf der gemeinsamen Innenspur Platz ist...
                              for (c=0; c<5; c++)
                                 if ((Airport.iPlateDir[12+((ArrayPos.x+c)<<4)]==3 && (Airport.iPlate[12+((ArrayPos.x+c)<<4)]&2)) || (Airport.iPlate[12+((ArrayPos.x+c)<<4)]&64)==0)
                                    break;

                              if (c==5 && (bFast || Dir==0))
                                 ty = 165; //alles frei, wir überholen innen
                              else
                                 ty = 187; //normale Spur, weil jemand kommt
                           }
                           else
                              ty = 187; //normale Spur; hier waren wir, hier bleiben wir
                        }
                     }
                  }
                  else if (Position.x>Target.x) //nach links
                  {
                     BOOL  bFast          = StepSize.y>4;
                     BOOL  bAussenErlaubt = (Airport.iPlate[10+(ArrayPos.x<<4)]&64) && (Airport.iPlate[10+((ArrayPos.x-1)<<4)]&64) && (Airport.iPlate[10+((ArrayPos.x+1)<<4)]&64);

                     if (bFast && bAussenErlaubt && (ArrayPos.y+5==10 || (Airport.iPlate[11+((ArrayPos.x-1)<<4)]&2) && Position.x-Target.x>88))
                        ty = 121;    //Schnelle Clans überholen außen, wenn jemand vor ihnen ist...
                     else
                     {
                        if (Airport.iPlateDir[11+((ArrayPos.x)<<4)]==8 || Airport.iPlateDir[11+((ArrayPos.x-1)<<4)]==8 || (bFast && Airport.iPlate[11+((ArrayPos.x-1)<<4)]&2 && Position.x-Target.x>88) || (StepSize.x>1 && (Airport.iPlateDir[11+((ArrayPos.x)<<4)]==254 || Airport.iPlateDir[11+((ArrayPos.x-1)<<4)]==254))) //Falls der Weg blockiert ist....
                        {
                           if (bAussenErlaubt)
                              ty = 121; //Jeder überholt hier außen...
                           else
                           {
                              //Prüfen, ob auf der gemeinsamen Innenspur Platz ist...
                              for (SLONG c=0; c<5; c++)
                                 if (((Airport.iPlateDir[12+((ArrayPos.x-c)<<4)]==1 || Airport.iPlateDir[12+((ArrayPos.x-c)<<4)]==254) && (Airport.iPlate[12+((ArrayPos.x-c)<<4)]&2)) || (Airport.iPlate[12+((ArrayPos.x-c)<<4)]&16)==0)
                                    break;

                              if (c==5)
                                 ty = 165; //alles frei, wir überholen innen
                              else
                                 ty = 143; //normale Spur, weil jemand kommt
                           }
                        }
                        else //Nein, der Weg ist nicht blockiert, Aber ggf. behalten wir unsere Überholspur noch ein wenig...
                        {
                           if (ArrayPos.y+5==10 && Dir==3 && bAussenErlaubt && (Airport.iPlate[10+((ArrayPos.x-1)<<4)]&2)==0)
                              ty = 209; //Es ist Platz, also überholen wir außen...
                           else if (ArrayPos.y+5==12 && (Dir==3 || Dir==2))
                           {
                              //Prüfen, ob auf der gemeinsamen Innenspur Platz ist...
                              for (SLONG c=0; c<5; c++)
                                 if ((Airport.iPlateDir[12+((ArrayPos.x-c)<<4)]==1 && (Airport.iPlate[12+((ArrayPos.x-c)<<4)]&2)) || (Airport.iPlate[12+((ArrayPos.x-c)<<4)]&16)==0)
                                    break;

                              if (c==5 && (bFast || Dir==2))
                                 ty = 165; //alles frei, wir überholen innen
                              else
                                 ty = 143; //normale Spur, weil jemand kommt
                           }
                           else
                              ty = 143; //normale Spur; hier waren wir, hier bleiben wir
                        }
                     }
                  }
                  if (abs (Position.x-Target.x)<=StepSize.x)
                  {
                     Position.x = Target.x;

                     if (Position.y<Target.y) Dir = 2;
                     else if (Position.y>Target.y) Dir = 0;
                     if (abs (Position.y-Target.y)<=StepSize.y)
                     {
                        Position.y = Target.y;
                        Dir = 4;
                     }
                  }

                  if (Dir!=4)
                  {
                     if (Position.y<ty) Dir = 2;
                     else if (Position.y>ty) Dir = 0;
                     if (abs (Position.y-ty)<=StepSize.y)
                     {
                        Position.y = ty;
                        if (Position.x<Target.x) Dir = 1;
                        else if (Position.x>Target.x) Dir = 3;
                        if (abs (Position.x-Target.x)<=StepSize.x)
                        {
                           Position.x = Target.x;

                           if (Position.y<Target.y) Dir = 2;
                           else if (Position.y>Target.y) Dir = 0;
                           if (abs (Position.y-Target.y)<=StepSize.y)
                           {
                              Position.y = Target.y;
                              Dir = 4;
                           }
                        }
                     }
                  }
               }

               if (ArrayPos.y>=0 && ArrayPos.y<=9)
                  NewPossibleDirs = UBYTE((Airport.iPlate[ArrayPos.y+5+(ArrayPos.x<<4)] & 240) & (~(128>>((Dir+2)&3))));
               else
                  NewPossibleDirs = 128+32;

               //Versuchen wir nach unten zu gehen, obwohl wir nur nach links können?
               if (Dir==2 && (NewPossibleDirs&48)==16) Dir=3;

               //Versuchen wir nach links zu gehen, obwohl wir nur nach unten können?
               if (Dir==3 && (NewPossibleDirs&48)==32) Dir=2;
               break;

            //Person will nur raus aus dem Check-In Bereich und rennt dafür auch andere über den Haufen...
            case PERSON_2CHECKIN_EXIT:
               Dir=1;
               break;

            case PERSON_ENTERINGPL:
               if (Position.x==Target.x)
               {
                  Dir = 0;
               }
               else if (Position.x>Target.x)
               {
                  Dir=3;
                  if (abs (Position.x-Target.x)<=StepSize.x)
                  {
                     Position.x = Target.x;
                     Dir = 4;
                  }
               }
               break;

            case PERSON_LEAVINGPL:
               if (Position.x<Target.x)
               {
                  Dir=1;
                  if (abs (Position.x-Target.x)<=StepSize.x)
                  {
                     Position.x = Target.x;
                     Dir = 4;
                  }
               }
               if (Position.x>=Target.x)
               {
                  Dir = 2;
                  if (Mood==MoodPersonSick)
                  {
                     if (qClan.Type==CLAN_MALE)   { Target=Airport.GetRandomTypedRune (RUNE_2SHOP, ROOM_WC_M, false, &PersonalRandWalk); StatePar=ROOM_WC_M; }
                     else if (qClan.Type==CLAN_FEMALE) { Target=Airport.GetRandomTypedRune (RUNE_2SHOP, ROOM_WC_F, false, &PersonalRandWalk); StatePar=ROOM_WC_F; }
                     else DebugBreak ();
                     State  = PERSON_2SHOP;
                  }
                  else
                  {
                     State  = PERSON_2EXIT;
                     Target = Airport.GetRandomExit(&PersonalRand);

                     if (qClan.HasSuitcase<0 && Clans.IsInAlbum(ClanId-1) && Clans[(SLONG)(ClanId-1)].HasSuitcase==-qClan.HasSuitcase)
                     {
                        SLONG n = PersonalRand.Rand(Airport.NumBeltSpots), c;

                        //Max 2 Personen an die gleiche Stelle lassen:
                        if (Sim.RoomBusy[ROOM_BELT_X1+n]==0 || Sim.RoomBusy[ROOM_BELT_X1+n]==1)
                        {
                           //log: hprintf ("Add Suitcase at n=%li..", n);
                           for (c=0; c<SLONG(Airport.Runes.AnzEntries()); c++)
                              if (Airport.Runes[c].BrickId==(RUNE_2SHOP|0x10000000) && Airport.Runes[c].Par==ROOM_BELT_X1+n)
                              {
                                 State    = PERSON_2SHOP;
                                 StatePar = ROOM_BELT_X1+n;
                                 Target   = Airport.Runes[c].ScreenPos;
                                 StatePar = Airport.Runes[c].Par;
                                 Sim.RoomBusy[ROOM_BELT_X1+n]++;

                                 //Koffer erzeugen:
                                 Sim.PersonQueue.AddPerson (
                                        Clans.GetSuitcaseId(-qClan.HasSuitcase),
                                        Airport.GetRandomTypedRune (RUNE_CREATE_SUITCASE, 0, false, &PersonalRandWalk), 
                                        REASON_SUITCASE_EXIT,
                                        0,
                                        0,
                                        0);
                                 break;
                              }

                           if (c==SLONG(Airport.Runes.AnzEntries())) DebugBreak();
                        }
                     }
                  }
               }
               break;

            case PERSON_2DURCHLEUCHTER:
            case PERSON_BOARDING:
               if (Position.x==Target.x || NewPossibleDirs&128)
               {
                  if (NewPossibleDirs&128)
                     Dir = 0;
                  else
                     Dir = 1;
               }
               else if (Position.x<Target.x)
               {
                  if (NewPossibleDirs&64) Dir=1; else Dir=0;
                  if (abs (Position.x-Target.x)<=StepSize.x)
                  {
                     Position.x = Target.x;
                     Dir = 4;
                  }
               }
               else
               {
                  if (NewPossibleDirs&16) Dir=3; else Dir=0;
                  if (abs (Position.x-Target.x)<=StepSize.x && (NewPossibleDirs&128))
                  {
                     Position.x = Target.x;
                     Dir = 4;
                  }
               }
               break;

            case PERSON_LEAVING:
            case PERSON_CHECKINGIN:
            case PERSON_DROPPING_SUICASE:
               break;

            default:
               TeakLibW_Exception (FNL, ExcNever);
               break;
         }
      }
   }

   if (Dir!=4)
   {
DirtyLabel:
      NewPos = Position;

      //Spätenstens jetzt stimmt die Richtung:
      switch (Dir)
      {
         case 0: NewPos.y = Position.y - StepSize.y; break;
         case 1: NewPos.x = Position.x + StepSize.x; break;
         case 2: NewPos.y = Position.y + StepSize.y; break;
         case 3: NewPos.x = Position.x - StepSize.x; break;
      }

      ArrayPos.x = (Position.x+4400)/44-100;
      ArrayPos.y = (Position.y+2200)/22-100;
      NewArrayPos.x = (NewPos.x+4400)/44-100;
      NewArrayPos.y = (NewPos.y+2200)/22-100;

      //Sind wir noch im Bodenplatten-Array?
      if (NewArrayPos.y>=0 && NewArrayPos.y<=9 && (State&PERSON_BROWSEFLAG)==0)
      {
         SLONG Index=NewArrayPos.y+5+(NewArrayPos.x<<4);

         //Steht auf der Zielkachel schon jemand? Ist vielleicht Stau?
         if ((Airport.iPlate[Index]&2)==0 || WaitCount>=200 || (State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))==PERSON_2CHECKIN_EXIT || (Dir==1 && Airport.iPlateDir[Index]==3) || (Dir==3 && Airport.iPlateDir[Index]==1))
         {
            //Nö. Aber jetzt! (evtl. war da auch ein Mega-Stau...)
            Position = NewPos;

            if ((State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))!=PERSON_BOARDING &&
                (State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))!=PERSON_2CHECKIN_EXIT &&
                (State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))!=PERSON_WAITING)
            {
                if ((Airport.iPlate[Index]&8)==8)
                  Airport.iPlate[Index] |= 2;
            }

            LookDir = Dir;
            Phase   = UBYTE((Phase+1)%(qClan.Phasen[(SLONG)LookDir].AnzEntries()*qClan.Faktor));
         }
         else
         {
            //Da ist schon wer; können wir ggf. nach links ausweichen?
            if (ArrayPos.y>=1+2 && ArrayPos.y<9 && Dir==2 && Position.x>Target.x && (Airport.iPlate[ArrayPos.y+5+((ArrayPos.x-1)<<4)]&2)==0 && (Airport.iPlate[ArrayPos.y+5+(ArrayPos.x<<4)]&16) && abs (Position.x-Target.x)>StepSize.x*4)
            {
               Dir=3; //Wir weichen nach links aus
               StepSize.x+=4;
               goto DirtyLabel;
            }
            //...oder nach rechts?
            else if (ArrayPos.y>1+1 && ArrayPos.y<9 && Dir==2 && Position.x<Target.x && (Airport.iPlate[ArrayPos.y+5+((ArrayPos.x+1)<<4)]&2)==0 && (Airport.iPlate[ArrayPos.y+5+(ArrayPos.x<<4)]&64) && abs (Position.x-Target.x)>StepSize.x*4)
            {
               Dir=1; //Wir weichen nach rechts aus
               StepSize.x+=4;
               goto DirtyLabel;
            }
            //Falls Frontalzusammenstoß und wir nicht ausweichen können, dann gehen wir skrupellos durch den anderen durch:
            else if (Dir<0 || Dir>3 || Airport.iPlateDir[Index]!=(Dir+2)%4)
            {
               State |= PERSON_WAITFLAG;

               if ((Airport.iPlate[ArrayPos.y+5+(ArrayPos.x<<4)]&8)==8 && (State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))!=PERSON_2CHECKIN_EXIT)
                  Airport.iPlate[ArrayPos.y+5+(ArrayPos.x<<4)] |= 2;

               LookDir = Dir;
               Phase   = LookDir;
               Dir     = LookDir = 8;

               //Geringe Geduld, wenn man die Wege kreuzt
               if ((ArrayPos.y>=6 && ArrayPos.y<=8 && (Dir==0 || Dir==2)) || (ArrayPos.y==5 && Dir==2))
                  WaitCount=UBYTE(max(180, WaitCount));

               if ((State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))==PERSON_2CHECKIN_EXIT && ArrayPos.y==3 && Dir==1)
                  WaitCount=UBYTE(max(198, WaitCount));
            }
         }
      }
      else
      {
         Position = NewPos;

         Phase   = UBYTE((Phase+1)%(qClan.Phasen[(SLONG)LookDir].AnzEntries()*qClan.Faktor));
         LookDir = Dir;
      }
   }
   else
   {
      //Sind wir noch im Bodenplatten-Array?
      if (ArrayPos.y>=0 && ArrayPos.y<=9 && (State&PERSON_BROWSEFLAG)==0 &&
          (State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))!=PERSON_BOARDING &&
          (State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))!=PERSON_2CHECKIN_EXIT &&
          (State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))!=PERSON_WAITING)
      {
         if ((Airport.iPlate[ArrayPos.y+5+(ArrayPos.x<<4)]&8)==8)
         {
            Airport.iPlate[ArrayPos.y+5+(ArrayPos.x<<4)] |= 2;
         }
      }

      if (qClan.Type!=30)
      {
         Phase   = LookDir;
         Dir     = LookDir = 8;
      }
   }

   if (ArrayPos.y>=0 && ArrayPos.y<=9 && (Airport.iPlate[ArrayPos.y+5+(ArrayPos.x<<4)]&2))
      if (StepSize.x==1)
         Airport.iPlateDir[ArrayPos.y+5+(ArrayPos.x<<4)] = 254;
      else
         Airport.iPlateDir[ArrayPos.y+5+(ArrayPos.x<<4)] = Dir;

   ScreenPos.x = Position.x-Position.y/2+90;
   ScreenPos.y = Position.y+220;
}

//--------------------------------------------------------------------------------------------
//Einer der Spieler geht einen Schritt weiter:
//--------------------------------------------------------------------------------------------
void PERSON::DoOnePlayerStep (void)
{
   BOOL    Upfloor;
   SLONG   c;
   CLAN   &qClan = Clans[(SLONG)ClanId];
   PLAYER &qPlayer = Sim.Players.Players[(SLONG)State];
   XY      StepSize (qPlayer.WalkSpeed*2,qPlayer.WalkSpeed);
   XY      ArrayPos, NewArrayPos;
   XY      NewPos;
   UBYTE   LastDir = Dir;
   bool    bDoBroadcastPosition = false;

   if (qPlayer.IsStuck)
   {
      qPlayer.IsStuck--;

      Running = false;
      Phase   = UBYTE((Phase+1)%(Clans[(SLONG)ClanId].Phasen[(SLONG)LookDir].AnzEntries()*Clans[(SLONG)ClanId].Faktor));
      LookDir = 5; //Gimmick beibehalten

      if (qPlayer.IsStuck)
         return;
      else
      {
         //Ende IsStuck
         LookDir = 2;
         Phase   = 0;

         if (qPlayer.Owner!=1) Mood = UBYTE(0);
      }
   }

   if (LookDir==4)
   {
      //Während einer Explosion wird der Spieler ans Geländer gedrückt und kann sich nicht bewegen
      return;
   }
   else if (LookDir==8)
   {
      LookDir=UBYTE(Phase%4);
      Phase  =0;
   }
   else if (LookDir==9) //Warte-Gimmick-Animation
   {
      Phase++;

      ArrayPos.x = Position.x/44;

      //Obere oder untere Ebene?
      if (Position.y<4000) ArrayPos.y = Position.y/22+5;  //unten!
      else                 ArrayPos.y = (Position.y-5000)/22; //oben!

      Airport.TryDoor (ArrayPos, 1, State);

      if (Position==Target && qPlayer.NewDir==8)
      {
         if (Phase<(qClan.Phasen[(SLONG)LookDir].AnzEntries()*qClan.Faktor))
            return;

         TEAKRAND LocalRand (Sim.Date+Sim.Time);
         if (LocalRand.Rand(100)<90 || qPlayer.Owner==2)
         {
            Phase=0;
            return;
         }

         qPlayer.StandCount=-100;
      }
      
      Dir=8;
      LookDir=UBYTE(qClan.GimmickArt2);
      Phase=0;
      Sim.SendSimpleMessage (ATNET_GIMMICK, NULL, qPlayer.PlayerNum, 0);
   }

   if (!StatePar)
   {
      Target = qPlayer.TertiaryTarget;
      Target.x = Target.x*44+22;
      
      if (Target.y<5)
         Target.y = Target.y*22+5000+11;
      else
         Target.y = (Target.y-5)*22+11;
   }

   //Obere oder untere Ebene?
   if (Position.y<4000)
   {
      //unten!
      ArrayPos.x = Position.x/44;
      ArrayPos.y = Position.y/22+5;
      Upfloor    = FALSE;
   }
   else
   {
      //oben!
      Position.y -= 5000;
      ArrayPos.x = Position.x/44;
      ArrayPos.y = (Position.y+2200)/22-100;  //21.2.02: war mal "Position.y/22", wegen Eingang Security-Raum geändert
      Upfloor    = TRUE;
   }

   NewPos = Position;

   //Und daher gehen wir per default einfach geradeaus weiter:
   switch (Dir)
   {
      case 0: NewPos.y = Position.y - StepSize.y*2; break;
      case 1: NewPos.y = Position.y - StepSize.y; NewPos.x = Position.x + StepSize.x; break;
      case 2: NewPos.x = Position.x + StepSize.x; break;
      case 3: NewPos.y = Position.y + StepSize.y; NewPos.x = Position.x + StepSize.x; break;
      case 4: NewPos.y = Position.y + StepSize.y*2; break;
      case 5: NewPos.y = Position.y + StepSize.y; NewPos.x = Position.x - StepSize.x; break;
      case 6: NewPos.x = Position.x - StepSize.x; break;
      case 7: NewPos.y = Position.y - StepSize.y; NewPos.x = Position.x - StepSize.x; break;
   }

   if (qPlayer.PlayerStinking>0 && Position.y<4000)
      for (long x=-2; x<=2; x++)
      {
         if (ArrayPos.y>=6) Airport.iPlate[ArrayPos.y+((ArrayPos.x+x)<<4)] &= ~1;
         if (ArrayPos.y>6)  Airport.iPlate[ArrayPos.y-1+((ArrayPos.x+x)<<4)] &= ~1;
         if (ArrayPos.y<14) Airport.iPlate[ArrayPos.y+1+((ArrayPos.x+x)<<4)] &= ~1;
         if (ArrayPos.y-1>6)  Airport.iPlate[ArrayPos.y-2+((ArrayPos.x+x)<<4)] &= ~1;
         if (ArrayPos.y+1<14) Airport.iPlate[ArrayPos.y+2+((ArrayPos.x+x)<<4)] &= ~1;
      }

   //Hoppla! Sind wir etwa just über die Mittelline einer Bodenplatte gelaufen?
   if ((NewPos.x+4422)/44!=(Position.x+4422)/44 || (NewPos.y+2211)/22!=(Position.y+2211)/22 || Dir==8)
   {
      //Klebstoff?
      if (ArrayPos.y>=0 && ArrayPos.y<=4)
         if (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)] & 1)
         {
            Sim.Persons.RemoveAnimationNear (Position+XY(0,5000));

            Mood=(UBYTE)MoodPersonStuck;
            if (MoodCountdown) MoodCountdown = MOODCOUNT_START-16;
            else               MoodCountdown = MOODCOUNT_START+rand()%15;

            State   = State & ~PERSON_WAITFLAG;
            LookDir = 5; //Klebe-Gimmick starten (ehem. Winken)
            Phase   = 0;
            Running = false;

            qPlayer.IsStuck=20*30;
            Position.y += 5000;

            if (qPlayer.PlayerNum==Sim.localPlayer && qPlayer.GetRoom()==ROOM_AIRPORT)
               ((AirportView*)qPlayer.LocationWin)->CenterCameraOnPlayer ();

            if (Sim.bNetwork) qPlayer.BroadcastPosition();
            return;
         }

      //Sind wir! Und deshalb ist es Zeit sich umzuschauen und eine Entscheidung zu treffen.
      //Welche Möglichkeiten hat der Kunde dieses Feld zu verlassen?
      UBYTE NewPossibleDirs;

      //Müssen wir erst einmal eine Tür aufstoßen?
      Airport.TryDoor (ArrayPos, 1, State);
      if (LookDir==4)
      {
         if (Upfloor) Position.y += 5000;
         return;
      }

      //Nebenbei runden wir noch die Position!
      if (Dir!=1 && Dir!=3 && Dir!=5 && Dir!=7)
      {
         Position.x = Position.x / 44 * 44 + 22;
         Position.y = ((Position.y+2200) / 22) * 22 - 2200 + 11; //21.2.02: war mal "Position.y / 22 * 22 + 11", wegen Eingang Security-Raum geändert
      }

      if (ArrayPos.y>=0 && ArrayPos.y<=14)
         NewPossibleDirs = UBYTE(Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)] & 240);
      else
         NewPossibleDirs = 128+32;

      //Sind einige womöglich für Spieler verboten?
      if (ArrayPos.y>=0 && ArrayPos.y<15)
      {
         if (ArrayPos.y>0  && (Airport.iPlate[ArrayPos.y-1+(ArrayPos.x<<4)]&4)==0) NewPossibleDirs&=(~128);
         if (ArrayPos.y<14 && (Airport.iPlate[ArrayPos.y+1+(ArrayPos.x<<4)]&4)==0) NewPossibleDirs&=(~32);
         if ((Airport.iPlate[ArrayPos.y+((ArrayPos.x-1)<<4)]&4)==0) NewPossibleDirs&=(~16);
         if ((Airport.iPlate[ArrayPos.y+((ArrayPos.x+1)<<4)]&4)==0) NewPossibleDirs&=(~64);
      }

      //Läuft der Spieler zur Zeit automatisch oder vom Mensch gesteuert?
      if (StatePar)
      {
         //Und suchen wir ein Ziel oder laufen wir nur in einem Treppengang?
         if (StatePar<ROOM_STAIRSVON || StatePar>ROOM_STAIRSBIS)
         {
            //Ein Ziel (z.B. wenn der Spieler einen Raum betritt):
            if (Position.x>Target.x && (StatePar==ROOM_WERKSTATT || (StatePar==-1 && Position.x>Airport.RightEnd-600))) Dir = 6;
            else if (Position.x>Target.x && (StatePar==ROOM_FRACHT || (StatePar==-1 && Position.x<Airport.LeftEnd+600))) Dir = 6;
            else if ((Position.y+1000)>((Target.y+1000)%5000)) Dir = 0;
            else if (Position.x>Target.x) Dir = 6;
            else if (Position.x<Target.x) Dir = 2;
            else if ((Position.y+1000)<((Target.y+1000)%5000)) Dir = 4;
         }

         if (StatePar==ROOM_WERKSTATT || (StatePar==-1 && Position.x>Airport.RightEnd-600))
         {
            if (Dir==2 && (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&64)==0) Dir=3;
            if (Dir==6 && (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&16)==0 && ((Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&64)==0 || (NewPos.x+4444)/44<33)) Dir=7;
         }
         else if (StatePar==ROOM_FRACHT || (StatePar==-1 && Position.x<Airport.LeftEnd+600 && qPlayer.ExRoom!=ROOM_MUSEUM))
         {
            if (Dir==2 && (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&64)==0) Dir=3;
            if (Dir==6 && (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&16)==0) Dir=7;
         }
      }
      else
      {
         //Manuelle Steuerung:
         Dir = qPlayer.NewDir;
         if (ArrayPos.y>=5 && ArrayPos.y<15)
         {
            switch (Dir)
            {
               case 2:
                  if (ArrayPos.y>Target.y/22+5 &&
                      Target.x/44-ArrayPos.x<=ArrayPos.y-(Target.y/22+5) &&
                      (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&128) &&
                      (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&64) &&
                      (Airport.iPlate[ArrayPos.y-1+(ArrayPos.x<<4)]&64) &&
                      (Airport.iPlate[ArrayPos.y+((ArrayPos.x+1)<<4)]&128) &&
                      (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&4) &&
                      (Airport.iPlate[ArrayPos.y-1+(ArrayPos.x<<4)]&4) &&
                      (Airport.iPlate[ArrayPos.y+((ArrayPos.x+1)<<4)]&4))
                      Dir=1; //Nord-Ost
                  else if (ArrayPos.y<Target.y/22+5 &&
                      Target.x/44-ArrayPos.x<=(Target.y/22+5)-ArrayPos.y &&
                      (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&32) &&
                      (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&64) &&
                      (Airport.iPlate[ArrayPos.y+1+(ArrayPos.x<<4)]&64) &&
                      (Airport.iPlate[ArrayPos.y+((ArrayPos.x+1)<<4)]&32) &&
                      (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&4) &&
                      (Airport.iPlate[ArrayPos.y+1+(ArrayPos.x<<4)]&4) &&
                      (Airport.iPlate[ArrayPos.y+((ArrayPos.x+1)<<4)]&4))
                      Dir=3; //Süd-Ost
                  break;

               case 6:
                  if (ArrayPos.y>Target.y/22+5 &&
                      ArrayPos.x-Target.x/44<=ArrayPos.y-(Target.y/22+5) &&
                      (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&128) &&
                      (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&16) &&
                      (Airport.iPlate[ArrayPos.y-1+(ArrayPos.x<<4)]&16) &&
                      (Airport.iPlate[ArrayPos.y+((ArrayPos.x-1)<<4)]&128) &&
                      (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&4) &&
                      (Airport.iPlate[ArrayPos.y-1+(ArrayPos.x<<4)]&4) &&
                      (Airport.iPlate[ArrayPos.y+((ArrayPos.x-1)<<4)]&4))
                      Dir=7; //Nord-West
                  else if (ArrayPos.y<Target.y/22+5 &&
                      ArrayPos.x-Target.x/44<=(Target.y/22+5)-ArrayPos.y &&
                      (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&32) &&
                      (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&16) &&
                      (Airport.iPlate[ArrayPos.y+1+(ArrayPos.x<<4)]&16) &&
                      (Airport.iPlate[ArrayPos.y+((ArrayPos.x-1)<<4)]&32) &&
                      (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&4) &&
                      (Airport.iPlate[ArrayPos.y+1+(ArrayPos.x<<4)]&4) &&
                      (Airport.iPlate[ArrayPos.y+((ArrayPos.x-1)<<4)]&4))
                      Dir=5; //Süd-West
                  break;
            }
         }

         if (Dir==8 && qPlayer.Owner!=1)
            Target = Position+XY(0, Upfloor*5000);
      }

      //Ja geht das denn mit der Richtung überhaupt? (Bei Werkstatt übersehen wir das 'mal)
      if (((128>>(Dir>>1)) & NewPossibleDirs)==0 && !(StatePar==ROOM_FRACHT || StatePar==ROOM_WERKSTATT || StatePar==ROOM_NASA || qPlayer.ExRoom==ROOM_WERKSTATT || qPlayer.ExRoom==ROOM_FRACHT))
      {
         if (qPlayer.Owner==0 && Dir!=8)
            CWait c;

         if (qPlayer.iWalkActive)
         {
            //Nein, aber wir versuchen zu korrigieren (nach oben & unten):
            if (ArrayPos.y>=5 && ArrayPos.y<=14)
            {
               //Sonderregelung: Nach oben ausweichen, wenn nahebei:
               if (Dir==2 && ArrayPos.y>Target.y/22+5 && ArrayPos.y>5 &&
                   Target.x/44-ArrayPos.x<=ArrayPos.y-(Target.y/22+5) &&
                   Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&128 &&
                   Airport.iPlate[ArrayPos.y-1+(ArrayPos.x<<4)]&64 &&
                   Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&4 &&
                   Airport.iPlate[ArrayPos.y-1+(ArrayPos.x<<4)]&4)
               {
                  Dir=0;
               }
               //Sonderregelung: Nach oben ausweichen, wenn nahebei:
               else if (Dir==6 && ArrayPos.y>Target.y/22+5 && ArrayPos.y>5 &&
                   ArrayPos.x-Target.x/44<=ArrayPos.y-(Target.y/22+5) &&
                   Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&128 &&
                   Airport.iPlate[ArrayPos.y-1+(ArrayPos.x<<4)]&16 &&
                   Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&4 &&
                   Airport.iPlate[ArrayPos.y-1+(ArrayPos.x<<4)]&4)
               {
                  Dir=0;
               }
               else
               {
                  SLONG ty=ArrayPos.y;

                  if (Dir==2) ty=13;
                  if (Dir==6)
                  {
                     ty = 11;
                  }

                  //Würde in der Std-Ebene überhaupt Platz sein?
                  if (UBYTE((Airport.iPlate[ty+(ArrayPos.x<<4)] & (128>>(Dir>>1))))==0 ||
                      UBYTE((Airport.iPlate[ty+(ArrayPos.x<<4)] & 4))==0)
                     ty=Target.y/22+5;

                  if (ty<ArrayPos.y && ((128>>0) & NewPossibleDirs)) Dir=0;
                  else if (ty>ArrayPos.y && ((128>>2) & NewPossibleDirs)) Dir=4;
                  else Dir = 8;
               }
            }
            else if (ArrayPos.y>=0 && ArrayPos.y<=2 && (Dir==2 || Dir==6))
            {
               XY p;

               p.y = (Position.y+2200)/22-100;

                    if (Dir==2 && ((128>>2) & NewPossibleDirs) && UBYTE(Airport.iPlate[ArrayPos.y+1+(ArrayPos.x<<4)] & (128>>1))) Dir=4;
               else if (Dir==2 && ((128>>0) & NewPossibleDirs) && UBYTE(Airport.iPlate[ArrayPos.y-1+(ArrayPos.x<<4)] & (128>>1))) Dir=0;
               else if (Dir==6 && ((128>>2) & NewPossibleDirs) && UBYTE(Airport.iPlate[ArrayPos.y+1+(ArrayPos.x<<4)] & (128>>3))) Dir=4;
               else if (Dir==6 && ((128>>0) & NewPossibleDirs) && UBYTE(Airport.iPlate[ArrayPos.y-1+(ArrayPos.x<<4)] & (128>>3))) Dir=0;
               else if (p.y>qPlayer.TertiaryTarget.y && ((128>>0) & NewPossibleDirs)) Dir=0;
               else if (p.y<qPlayer.TertiaryTarget.y && ((128>>2) & NewPossibleDirs)) { Dir=4; if (StatePar==-1) StatePar=0; }
               else if (((128>>2) & NewPossibleDirs) && ArrayPos.y==0) Dir=4;
               else if (((128>>0) & NewPossibleDirs) && ArrayPos.y==2) Dir=0;
               else Dir=8;
            }
            else Dir = 8;
         }
         else Dir = 8;
      }

      //Haben wir eine Raumankündigung betreten?
      if (StatePar == 0)
      {
         c=Airport.GetRuneParNear (XY(NewPos.x,NewPos.y+Upfloor*5000), XY(StepSize.x,StepSize.y*2), RUNE_2SHOP);

         if (c==ROOM_AIRPORT_EXIT)
         {
            if (qPlayer.LocationWin && Dir!=0 && !(*(CStdRaum*)qPlayer.LocationWin).MenuIsOpen())
            {
               if (Sim.Date>1) Sim.GiveHint (HINT_FEIERABEND);
               (*(CStdRaum*)qPlayer.LocationWin).MenuStart (MENU_REQUEST, MENU_REQUEST_CALLITADAY, 1);
               (*(CStdRaum*)qPlayer.LocationWin).MenuSetZoomStuff (XY(ScreenPos.x, ScreenPos.y-20), 0, TRUE);
            }

            qPlayer.ExRoom=c;
            StatePar=-1;
         }

         if (c>=ROOM_PHONE1 && c<=ROOM_PHONE4)
         {
            if (qPlayer.Owner!=1 && qPlayer.DirectToRoom==c && (qPlayer.Owner==2 || !(*(CStdRaum*)qPlayer.LocationWin).MenuIsOpen() && !(*(CStdRaum*)qPlayer.LocationWin).IsDialogOpen()))
            {
               if (qPlayer.Owner==0)
               {
                  (*(CStdRaum*)qPlayer.LocationWin).MenuStart (MENU_FILOFAX, 2);
                  (*(CStdRaum*)qPlayer.LocationWin).MenuSetZoomStuff (XY(ScreenPos.x, ScreenPos.y-60), 0, TRUE);
               }
               LookDir=0; Phase=0;
               qPlayer.DirectToRoom=0;

               bDoBroadcastPosition = true;
            }
         }
         else if (c>=ROOM_MONITOR7 && c<=ROOM_MONITOR9)
         {
            if (qPlayer.Owner!=1 && qPlayer.DirectToRoom==c && (qPlayer.Owner==2 || !(*(CStdRaum*)qPlayer.LocationWin).MenuIsOpen() && !(*(CStdRaum*)qPlayer.LocationWin).IsDialogOpen()))
            {
               if (c==qPlayer.DirectToRoom)
               {
                  if (qPlayer.Owner==0)
                  {
                     (*(CStdRaum*)qPlayer.LocationWin).MenuStart (MENU_FLUEGE);
                     (*(CStdRaum*)qPlayer.LocationWin).MenuSetZoomStuff (XY(ScreenPos.x-31, ScreenPos.y-182), 0.175, TRUE);
                  }
                  qPlayer.DirectToRoom=0;
                  LookDir=0; Phase=0;
               }
            }
         }
         else if (c==ROOM_ELECTRO)
         {
            if (qPlayer.Owner!=1 && qPlayer.DirectToRoom==c && (qPlayer.Owner==2 || !(*(CStdRaum*)qPlayer.LocationWin).MenuIsOpen() && !(*(CStdRaum*)qPlayer.LocationWin).IsDialogOpen()))
            {
               if (c==qPlayer.DirectToRoom)
               {
                  if (qPlayer.HasItem(ITEM_GLOVE))
                  {
                     qPlayer.DirectToRoom=0;

                     if (Sim.Options.OptionEffekte && State==Sim.localPlayer)
                     {
                        gUniversalFx.ReInit ("vending.raw");
                        gUniversalFx.Play (0, Sim.Options.OptionEffekte*100/7*AmbientManager.GlobalVolume/100);
                     }

                     if (State==Sim.localPlayer)
                        for (SLONG c=0; c<6; c++)
                           if (qPlayer.Items[c]==ITEM_GLOVE)
                           {
                              qPlayer.Items[c]=ITEM_REDBULL;
                              break;
                           }
                  }
                  else
                  {
                     BUILD *pBuild=Airport.GetBuildNear (ScreenPos, XY(180,160), Bricks((SLONG)0x10000000+BRICK_ELECTRO));
                     if (pBuild) Airport.Triggers[(SLONG)pBuild->Par].Winkel=Sim.TickerTime;

                     Sim.DontDisplayPlayer=qPlayer.PlayerNum;
                     LookDir=2; Phase=0;

                     qPlayer.DirectToRoom=0;
                     qPlayer.IsDrunk=0;

                     bDoBroadcastPosition = true;

                     if (Sim.Options.OptionEffekte && State==Sim.localPlayer)
                     {
                        gUniversalFx.ReInit ("fused.raw");
                        gUniversalFx.Play (0, Sim.Options.OptionEffekte*100/7*AmbientManager.GlobalVolume/100);
                     }
                  }
               }
            }
         }
         else if (c && (c<ROOM_REISE_X1 || c>ROOM_MONITOR6) && c!=ROOM_AIRPORT_EXIT && ((c!=ROOM_KIOSK && c!=ROOM_INSEL && c!=ROOM_SECURITY && c!=ROOM_RUSHMORE && c!=ROOM_WELTALL && c!=ROOM_RICKS && c!=ROOM_ROUTEBOX && c!=ROOM_SECURITY) || c==qPlayer.DirectToRoom))
         {
            //Ja! Ab jetzt laufen wir automatisch!
            qPlayer.ExRoom=-1;
            StatePar = c;

            //Für 'echte' Räume schauen wir auch das nächste Ziel nach:
            if (StatePar<ROOM_STAIRSVON || StatePar>ROOM_STAIRSBIS)
            {
               //Raum (wenn keine Toilette) als besetzt markieren:
               if (StatePar!=ROOM_WC_F && StatePar!=ROOM_WC_M) Sim.RoomBusy[StatePar]++;

               if (StatePar>=ROOM_BURO_A && StatePar<=ROOM_PERSONAL_D && StatePar/10!=State+1)
               {
                  Target.x=Position.x;
                  Target.y=Position.y+22+Upfloor*5000;   //New (Network)
                  qPlayer.NewDir=4;
                  qPlayer.TertiaryTarget.x=(Target.x-22)/44;
                  qPlayer.TertiaryTarget.y=(Target.y-5000-11)/22+1;
                  qPlayer.WalkStop();

                  //Raum (wenn keine Toilette) als besetzt entmarkieren:
                  if (StatePar!=ROOM_WC_F && StatePar!=ROOM_WC_M && Sim.RoomBusy[StatePar]>0) Sim.RoomBusy[StatePar]--;

                  if (qPlayer.LocationWin && !(*(CStdRaum*)qPlayer.LocationWin).IsDialogOpen() && !(*(CStdRaum*)qPlayer.LocationWin).MenuIsOpen())
                  {
                     qPlayer.ThrownOutOfRoom=UWORD(StatePar%10 + State*10+10);

                     bgWarp = FALSE;
                     if (Sim.Players.Players[StatePar/10-1].IsOut)
                        (*(CStdRaum*)qPlayer.LocationWin).MenuStart (MENU_REQUEST, MENU_REQUEST_WRONGROOM2);
                     else
                     {
                        (*(CStdRaum*)qPlayer.LocationWin).MenuStart (MENU_REQUEST, MENU_REQUEST_WRONGROOM);

                        if (qPlayer.HasItem (ITEM_ZANGE) && Sim.Players.Players[StatePar/10-1].OfficeState==0 && (StatePar==ROOM_BURO_A || StatePar==ROOM_BURO_B || StatePar==ROOM_BURO_C || StatePar==ROOM_BURO_D))
                        {
                           qPlayer.DropItem (ITEM_ZANGE);
                           qPlayer.NetSynchronizeItems ();
                           Sim.Players.Players[StatePar/10-1].OfficeState=3;

                           Sim.SendSimpleMessage (ATNET_SYNC_OFFICEFLAG, NULL, StatePar/10-1, 3);
                        }
                     }

                     (*(CStdRaum*)qPlayer.LocationWin).MenuSetZoomStuff (XY(ScreenPos.x, ScreenPos.y-20), 0, TRUE);
                  }

                  Dir=4; StatePar=0; qPlayer.ExRoom=0;
               }
               //Doch nicht in einen Raum gehen, falls Dialog offen ist oder eröffnet wird:
               else if (qPlayer.LocationWin && ((*(CStdRaum*)qPlayer.LocationWin).IsDialogOpen() || qPlayer.IsWalking2Player!=-1))
               {
                  //Raum (wenn keine Toilette) als besetzt entmarkieren:
                  if (StatePar!=ROOM_WC_F && StatePar!=ROOM_WC_M && Sim.RoomBusy[StatePar]>0) Sim.RoomBusy[StatePar]--;

                  if (StatePar==ROOM_WERKSTATT)
                  {
                     Target.x=Position.x-44;
                     Target.y=Position.y;
                  }
                  else if (StatePar==ROOM_FRACHT)
                  {
                     Target.x=Position.x+44;
                     Target.y=Position.y;
                  }
                  else
                  {
                     Target.x=Position.x;
                     Target.y=Position.y+22+Upfloor*5000;   //New (Network)
                  }

                  qPlayer.NewDir=8;
                  qPlayer.TertiaryTarget.x=(Target.x-22)/44;
                  qPlayer.TertiaryTarget.y=(Target.y-5000-11)/22+1;
                  qPlayer.WalkStop();
               }
               else if ((Sim.RoomBusy[StatePar]>1 || qPlayer.LocationForbidden==StatePar) && !Sim.bNoTime && (StatePar<ROOM_BURO_A || StatePar>ROOM_PERSONAL_D))
               {
                  //Raum (wenn keine Toilette) als besetzt entmarkieren:
                  if (StatePar!=ROOM_WC_F && StatePar!=ROOM_WC_M && Sim.RoomBusy[StatePar]>0) Sim.RoomBusy[StatePar]--;

                  //Nein, Standard-Raum, d.h. jetzt müssen wir vor die Tür:
                  if (StatePar==ROOM_NASA || StatePar==ROOM_FRACHT)
                  {
                     Target.x=Position.x+44;
                     Target.y=Position.y+22+Upfloor*5000;   //New (Network)
                  }
                  else if (StatePar == ROOM_WERKSTATT)
                  {
                     Target.x=Position.x-44;
                     Target.y=Position.y;
                  }
                  else if (StatePar==ROOM_ROUTEBOX || StatePar==ROOM_SECURITY)
                  {
                     LookDir=2;
                     qPlayer.DirectToRoom=0;

                     bDoBroadcastPosition = true;

                     if (qPlayer.LocationWin)
                     {
                        qPlayer.ThrownOutOfRoom=UWORD(StatePar);
                        (*(CStdRaum*)qPlayer.LocationWin).MenuStart (MENU_REQUEST, MENU_REQUEST_THROWNOUT2);
                        (*(CStdRaum*)qPlayer.LocationWin).MenuSetZoomStuff (XY(ScreenPos.x, ScreenPos.y-20), 0, TRUE);
                     }
                     StatePar=0; qPlayer.ExRoom=0;
                  }
                  else 
                  {
                     Target.x=Position.x;
                     Target.y=Position.y+22+Upfloor*5000;   //New (Network)
                  }

                  qPlayer.NewDir=8;
                  qPlayer.TertiaryTarget.x=(Target.x-22)/44;
                  qPlayer.TertiaryTarget.y=(Target.y-5000-11)/22+1;
                  qPlayer.WalkStop();

                  if (qPlayer.Owner==0)
                  {
                     if (qPlayer.LocationWin && !(*(CStdRaum*)qPlayer.LocationWin).IsDialogOpen() && !(*(CStdRaum*)qPlayer.LocationWin).MenuIsOpen())
                     {
                        qPlayer.ThrownOutOfRoom=UWORD(StatePar);

                        if (StatePar==ROOM_RICKS || StatePar==ROOM_KIOSK || StatePar==ROOM_LAST_MINUTE || StatePar==ROOM_ARAB_AIR || StatePar==ROOM_REISEBUERO || StatePar==ROOM_INSEL || StatePar==ROOM_RUSHMORE || StatePar==ROOM_ROUTEBOX || StatePar==ROOM_WELTALL)
                           (*(CStdRaum*)qPlayer.LocationWin).MenuStart (MENU_REQUEST, (qPlayer.LocationForbidden!=StatePar) ? MENU_REQUEST_THROWNOUT : MENU_REQUEST_FORBIDDEN);
                        else
                           (*(CStdRaum*)qPlayer.LocationWin).MenuStart (MENU_REQUEST, (qPlayer.LocationForbidden!=StatePar) ? MENU_REQUEST_THROWNOUT : MENU_REQUEST_FORBIDDEN);

                        (*(CStdRaum*)qPlayer.LocationWin).MenuSetZoomStuff (XY(ScreenPos.x, ScreenPos.y-20), 0, TRUE);
                     }
                  }
                  else if (qPlayer.Owner==1 && (Sim.bIsHost || Sim.bNetwork==0))
                  {
                     SLONG e;

                     //Roboter: Raum schon besetzt? Erst zweite Priorität ausführen:
                     for (e=qPlayer.RobotActions.AnzEntries()-1; e>=1; e--)
                        qPlayer.RobotActions[e]=qPlayer.RobotActions[e-1];
                     qPlayer.RobotActions[1]=qPlayer.RobotActions[2];
                     qPlayer.RobotActions[2]=qPlayer.RobotActions[0];

                     if (qPlayer.RobotActions[1].ActionId==ACTION_NONE)
                        if (qPlayer.OfficeState==2)
                           qPlayer.RobotActions[1].ActionId=ACTION_PERSONAL;
                        else
                           qPlayer.RobotActions[1].ActionId=ACTION_BUERO;

                     qPlayer.RobotActions[0].ActionId=ACTION_NONE;
                  }
               }
               else Target = Airport.GetRandomTypedRune (RUNE_SHOP, (UBYTE)StatePar, false);
            }
         }
      }
   }
   else if (ArrayPos.y==2 && Dir==0) Airport.TryDoor (ArrayPos, 1, State);

   //Wenn wir im Begriff sind, einen Raum zu verlassen, dann laufen wir nicht manuell
   c=Airport.GetRuneParNear (XY(NewPos.x,NewPos.y+Upfloor*5000), XY(StepSize.x,StepSize.y*2), RUNE_SHOP);
   if (c==ROOM_ELECTRO || c==ROOM_ROUTEBOX || ROOM_WELTALL) c=0;

   if (!(StatePar == 0 && qPlayer.GetRoom()==ROOM_AIRPORT && c!=0 && (c<ROOM_REISE_X1 || c>ROOM_MONITOR9) && c!=ROOM_ELECTRO && c!=ROOM_ROUTEBOX && c!=ROOM_RICKS))
   {
      //Spätenstens jetzt stimmt die Richtung:
      switch (Dir)
      {
         case 0: Position.y -= StepSize.y*2; break;
         case 1: Position.y -= StepSize.y; Position.x += StepSize.x; break;
         case 2: Position.x += StepSize.x; break;
         case 3: Position.y += StepSize.y; Position.x += StepSize.x; break;
         case 4: Position.y += StepSize.y*2; break;
         case 5: Position.y += StepSize.y; Position.x -= StepSize.x; break;
         case 6: Position.x -= StepSize.x; break;
         case 7: Position.y -= StepSize.y; Position.x -= StepSize.x; break;
      }
   }

   //Laufen wir automatisch?
   if (StatePar)
   {
      //Sind wir im Pseudo-Raum "Treppe"?
      if (StatePar>=ROOM_STAIRSVON && StatePar<=ROOM_STAIRSBIS)
      {
         //Ja! Und dieser wird etwas anders behandelt!
         if (Upfloor) //AI für "oben sein"
         {
            //AI für "oben sein" & oben raus wollen
            if (StatePar==ROOM_STAIRS1UP || StatePar==ROOM_STAIRS2UP || StatePar==ROOM_STAIRS3UP)
            {
               Dir = 6;
               if ((Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&16)==0 && abs((Position.x+4422)%44)<=StepSize.x)
               {
                  Dir=4;
                  StatePar=0;
                  qPlayer.ExRoom=0;
               }
            }
            //AI für "oben sein" & nach unten wollen
            else if (StatePar==ROOM_STAIRS1DOWN || StatePar==ROOM_STAIRS2DOWN || StatePar==ROOM_STAIRS3DOWN)
            {
               Dir = 2;
               if ((Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&64)==0)
               {
                  Position.y -= 22;
                  Position.x = (Position.x+22) / 44 * 44 + 22*0;
                  Position.y = (Position.y+11) / 22 * 22 + 11*0;
                  Upfloor = FALSE;
                  Dir=5;
               }
            }
         }
         else //AI für "unten sein"
         {
            //AI für "unten sein" & nach oben wollen
            if (StatePar==ROOM_STAIRS1UP || StatePar==ROOM_STAIRS2UP || StatePar==ROOM_STAIRS3UP)
            {
               Dir = 1;
               if ((Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&64)==0)
               {
                  Position.x = (Position.x+22) / 44 * 44 + 22;
                  Position.y = (Position.y+11) / 22 * 22 + 11;
                  Upfloor = TRUE;
                  Dir=6;
               }
            }
            //AI für "unten sein" & unten raus wollen
            else if (StatePar==ROOM_STAIRS1DOWN || StatePar==ROOM_STAIRS2DOWN || StatePar==ROOM_STAIRS3DOWN)
            {
               Dir = 5;
               if ((Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&16)==0 && abs((Position.y+2211)%22)<=StepSize.y)
               {
                  Dir=4;
                  StatePar=0;
                  qPlayer.ExRoom=0;
               }
            }
         }
      }
      else
      {
         //Wir laufen autom. zu einem normalen Raum & daher müssen wir ggf. d. Position runden!
         if (abs(Position.x-Target.x)<=StepSize.x) Position.x=Target.x;
         if (abs(Position.y-Target.y)<=StepSize.y*2) Position.y=Target.y;

         Dir = 8;

         //Und die Richtung anschließend überprüfen:
         if (Position.x>Target.x && (StatePar==ROOM_WERKSTATT || (StatePar==-1 && Position.x>Airport.RightEnd-600))) Dir = 6;
         else if (Position.x>Target.x && (StatePar==ROOM_FRACHT || (StatePar==-1 && Position.x<Airport.LeftEnd+600))) Dir = 6;
         else if ((Position.y+1000)>((Target.y+1000)%5000)) Dir = 0;
         //else if (Position.y>(Target.y%5000)) Dir = 0;
         else if (Position.x>Target.x) Dir = 6;
         else if (Position.x<Target.x) Dir = 2;
         else if ((Position.y+1000)<((Target.y+1000)%5000)) Dir = 4;
         //else if (Position.y<(Target.y%5000)) Dir = 4;

         if (StatePar==ROOM_WERKSTATT || (StatePar==-1 && Position.x>Airport.RightEnd-600))
         {
            if (Dir==2 && (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&64)==0) Dir=3;
            if (Dir==6 && (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&16)==0 && ((Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&64)==0 || (NewPos.x+4444)/44<33)) Dir=7;
         }
         else if (StatePar==ROOM_FRACHT || (StatePar==-1 && Position.x<Airport.LeftEnd+600 && qPlayer.ExRoom!=ROOM_MUSEUM))
         {
            if (Dir==2 && (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&64)==0) Dir=3;
            if (Dir==6 && (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&16)==0) Dir=7;
         }

         //Und natürlich überprüfen, ob wir angekommen sind (und nicht gerade raus wollen):
         if (StatePar!=-1)
         {
            c=Airport.GetRuneParNear (XY(NewPos.x,NewPos.y+Upfloor*5000), XY(StepSize.x,StepSize.y*2), RUNE_SHOP);
            if (c==ROOM_ELECTRO) c=0;

                 if (c==ROOM_WC_F) { qPlayer.RunningToToilet=FALSE; if (State==Sim.localPlayer) { if (qPlayer.LocationWin) (*(CStdRaum*)qPlayer.LocationWin).MenuStart (MENU_WC_F, Sim.TimeSlice, 0); } else Sim.ComputerOnToilet (State); }
            else if (c==ROOM_WC_M) { qPlayer.RunningToToilet=FALSE; if (State==Sim.localPlayer) { if (qPlayer.LocationWin) (*(CStdRaum*)qPlayer.LocationWin).MenuStart (MENU_WC_M, Sim.TimeSlice, 0); } else Sim.ComputerOnToilet (State); }
            else if (c && (c<ROOM_REISE_X1 || c>ROOM_MONITOR6) && c!=ROOM_ELECTRO)
            {
               //Ja, deshalb betreten wir jetzt den Raum:
               if (State!=Sim.localPlayer || !Sim.CallItADay)
               {
                  if (qPlayer.Owner!=2)
                  {
                     if (Upfloor) Position.y += 5000;
                     qPlayer.EnterRoom (c);
                     if (qPlayer.Owner==0 && Sim.bNetwork && Sim.bIsHost)
                     {
                        if (c==ROOM_ARAB_AIR || c==ROOM_ROUTEBOX || c==ROOM_KIOSK || c==ROOM_RICKS)
                        {
                           qPlayer.BroadcastRooms (ATNET_ENTERROOM);
                           qPlayer.BroadcastPosition (true);
                        }
                     }

                     if (Upfloor) Position.y -= 5000;
                  }

                  qPlayer.iWalkActive=0;
               }

               Dir=qPlayer.NewDir=8;
               StatePar=0;
               qPlayer.ExRoom=0;

               if (c==ROOM_LAST_MINUTE)     LookDir=1;
               else if (c==ROOM_REISEBUERO) LookDir=3;
               else if (c==ROOM_ARAB_AIR)   LookDir=0;
               else if (c==ROOM_KIOSK)      LookDir=0;
               else if (c==ROOM_RICKS)      LookDir=0;
               else if (c==ROOM_ROUTEBOX)   LookDir=0;

               bDoBroadcastPosition=true;
            }
         }

         //Evtl. sind wir irgendwo angekommen wo keine Rune ist:
         if (Position.x==Target.x && Position.y+Upfloor*5000==Target.y) { StatePar = 0; qPlayer.ExRoom=0; bDoBroadcastPosition=true; }
      }
   }

   //Sind wir am Raum, aber nicht in der Location? Dann sind wir wohl gerade raus:
   if (StatePar == 0 && qPlayer.GetRoom()==ROOM_AIRPORT)
   {
      c=Airport.GetRuneParNear (XY(NewPos.x,NewPos.y+Upfloor*5000), XY(StepSize.x,StepSize.y*2), RUNE_SHOP);
      if (c==ROOM_ELECTRO || c==ROOM_ROUTEBOX || c==ROOM_WELTALL) c=0;

      if (c && (c!=ROOM_ELECTRO || c==qPlayer.DirectToRoom) && (c<ROOM_REISE_X1 || c>ROOM_MONITOR9) && c!=ROOM_RICKS)  //ex:ROOM_MONITOR6
      {
         //Es sei denn, wir haben es mir der Treppe als virtuellen Raum zu tun: (oder mit einem Monitor)
         if (c<ROOM_STAIRSVON || c>ROOM_STAIRSBIS)
         {
            UBYTE RunePar = Airport.GetRuneParNear (XY(Position.x,Position.y+Upfloor*5000), XY(StepSize.x,StepSize.y*2), RUNE_SHOP);

            //Nein, Standard-Raum, d.h. jetzt müssen wir vor die Tür:
            if (RunePar==ROOM_NASA || RunePar==ROOM_FRACHT)
            {
               SLONG Room = RunePar;
               if (Room) Target = Airport.GetRandomTypedRune (RUNE_2SHOP, (UBYTE)Room)+XY(44,0);
            }
            else if (RunePar==ROOM_WELTALL || RunePar==ROOM_RUSHMORE || RunePar==ROOM_INSEL || RunePar==ROOM_SECURITY)
            {
               SLONG Room = RunePar;
               if (Room) Target = Airport.GetRandomTypedRune (RUNE_2SHOP, (UBYTE)Room);
               qPlayer.DirectToRoom = 0;
            }
            else if (RunePar!=ROOM_WERKSTATT)
            {
               SLONG Room = RunePar;
               if (Room) Target = Airport.GetRandomTypedRune (RUNE_2SHOP, (UBYTE)Room)+XY(0,22);
            }
            else 
            {
               SLONG Room = RunePar;
               if (Room) Target = Airport.GetRandomTypedRune (RUNE_2SHOP, (UBYTE)Room)+XY(-44, 0);
            }

            qPlayer.ExRoom=c;
            StatePar = -1;
         }
      }
   }

   if (Dir!=8)
   {
      LookDir=Dir;

      if (State==Sim.localPlayer) PlayerDidntMove=0;
   }
   else
   {
      Phase   = LookDir;
      LookDir = 8;

      //Handy-Stehphasen:
      if (State==Sim.localPlayer && qPlayer.LocationWin && ((CStdRaum*)qPlayer.LocationWin)->IsDialogOpen() && ((CStdRaum*)qPlayer.LocationWin)->DialogMedium==MEDIUM_HANDY) Phase+=4;

      if (qPlayer.IsWalking2Player!=-1)
      {
         XY p=Sim.Persons.Persons[(SLONG)Sim.Persons.GetPlayerIndex(qPlayer.IsWalking2Player)].Position;

         if (Position.y+Upfloor*5000>p.y+7) Phase=0;
         else if (Position.y+Upfloor*5000<p.y-7) Phase=2;
         else if (Position.x>p.x+7) Phase=3;
         else if (Position.x<p.x-7) Phase=1;

         if (qPlayer.PlayerDialogState==-1)
         {
            ((CStdRaum*)qPlayer.LocationWin)->StartDialog (TALKER_COMPETITOR, MEDIUM_AIR, qPlayer.IsWalking2Player, !(qPlayer.Owner==0));

            bDoBroadcastPosition=true;

            if (Sim.bNetwork && Sim.Players.Players[qPlayer.IsWalking2Player].Owner==2)
            {
               TEAKFILE Message;

               Message.Announce(1024);

               //Start the dialog on the other computer, too:
               if (Upfloor) Position.y += 5000;
               Message << ATNET_DIALOG_START << qPlayer.PlayerNum
                       << Position.x         << Position.y
                       << Phase              << LookDir;
               if (Upfloor) Position.y -= 5000;

               qPlayer.bDialogStartSent=true;

               Sim.SendMemFile (Message, Sim.Players.Players[qPlayer.IsWalking2Player].NetworkID);
            }
         }

         Sim.Players.Players[qPlayer.IsWalking2Player].IsWalking2Player=-1;
         qPlayer.IsWalking2Player=-1;
      }
   }

   switch (Dir)
   {
      case 0:                 LookDir=0; break;
      case 1: case 2: case 3: LookDir=1; break;
      case 4:                 LookDir=2; break;
      case 5: case 6: case 7: LookDir=3; break;
      case 8:
         if (Position.x==Target.x && (Position.y%5000)==(Target.y%5000) && qPlayer.PrimaryTarget.x==(Target.x-22)/44 && qPlayer.PrimaryTarget.y==((Target.y+22+Upfloor*5000)-5000-11)/22+1)
         {
            if (!StatePar)
            {
               if (qPlayer.TertiaryTarget==qPlayer.PrimaryTarget)
                  if (qPlayer.Owner!=1) Running=FALSE;
            }
            else
            {
               qPlayer.iWalkActive=FALSE;
            }
         }
         else if (Position.x==Target.x && (Position.y%5000)>(Target.y%5000) && qPlayer.PrimaryTarget.x==(Target.x-22)/44 && qPlayer.PrimaryTarget.y==((Target.y+22+Upfloor*5000)-5000-11)/22+1)
         {
            if (StatePar==0 && (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&128)==0)
            {
               qPlayer.iWalkActive=FALSE;
            }
         }
         break;
   }

   if (Dir!=8)
      if (Running && Dir<8 && (Position.y/22+5!=13 || (Airport.iPlate[(Position.y/22+5)+((Position.x/44)<<4)] & 240)!=80))
         Phase = UBYTE((Phase+1)%(Clans[(SLONG)ClanId].Phasen[(SLONG)(LookDir+10)].AnzEntries()*Clans[(SLONG)ClanId].Faktor*2));
      else
         Phase = UBYTE((Phase+1)%(Clans[(SLONG)ClanId].Phasen[(SLONG)LookDir].AnzEntries()*Clans[(SLONG)ClanId].Faktor));

   if (Dir==8 && LastDir!=8 && qPlayer.PrimaryTarget.x==(Target.x-22)/44 && qPlayer.PrimaryTarget.y==((Target.y+22+Upfloor*5000)-5000-11)/22+1)
   {
      bDoBroadcastPosition = true;
      qPlayer.iWalkActive  = false;
      qPlayer.NewDir       = 8;
   }

   //Obere oder untere Ebene?
   if (Upfloor)
   {
      //oben!
      ScreenPos.x = Position.x-Position.y/2+86;
      ScreenPos.y = Position.y+93;
      Position.y += 5000;
   }
   else
   {
      //unten!
      ScreenPos.x = Position.x-Position.y/2+86;
      ScreenPos.y = Position.y+220;
   }

   if (bDoBroadcastPosition && Sim.bNetwork)
      qPlayer.BroadcastPosition();

   if (qPlayer.PlayerStinking>16 && Position.y<4000)
      for (long x=-1; x<=1; x++)
      {
         if (ArrayPos.y>=6) Airport.iPlate[ArrayPos.y+((ArrayPos.x+x)<<4)] |= 1;
         if (ArrayPos.y>6  && ArrayPos.y<14) Airport.iPlate[ArrayPos.y-1+((ArrayPos.x+x)<<4)] |= 1;
         if (ArrayPos.y>=6 && ArrayPos.y<14) Airport.iPlate[ArrayPos.y+1+((ArrayPos.x+x)<<4)] |= 1;
      }
}

//--------------------------------------------------------------------------------------------
// Makes the Person look into that direction (while standing)
//--------------------------------------------------------------------------------------------
void PERSON::LookAt (SLONG Dir)
{
   PERSON::Dir     = 8;
   PERSON::LookDir = 8;
   PERSON::Phase   = (UBYTE)Dir;
}

//--------------------------------------------------------------------------------------------
//Eine Person hat einen Checkpunkt (Sekundärziel) erreicht und braucht neue Anweisungen:
//--------------------------------------------------------------------------------------------
void PERSON::PersonReachedTarget (void)
{
   //Das hängt davon ab, warum sie am Flughafen ist:
   switch (Reason)
   {
      //Die Person geht nur einkaufen:
      case REASON_SHOPPING:
         switch (State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))
         {
            //Person ist am Shopeingang/ausgang angekommen:
            case PERSON_2SHOP:
               State = PERSON_SHOPPING | PERSON_BROWSEFLAG;
               Target = Airport.GetRandomTypedRune (RUNE_SHOP, (UBYTE)StatePar, false, &PersonalRand);
               break;

            //Person ist im Kern des Ladens angekommen:
            case PERSON_SHOPPING:
               Dir = LookDir = 0;

               if (Clans[(SLONG)ClanId].Type!=30)
               {
                  Phase   = 0;
                  Dir     = LookDir = 8;
               }

               WaitCount = 150+(Target.y+Target.x+ClanId*33)%100;

               State  = PERSON_EXSHOP | PERSON_BROWSEFLAG | PERSON_WAITFLAG;
               Target = Airport.GetRandomTypedRune (RUNE_2SHOP, (UBYTE)StatePar, false, &PersonalRand);
               break;

            //Person ist am Shopeingang/ausgang angekommen:
            case PERSON_EXSHOP:
               if (StatePar>=ROOM_REISE_X1 && StatePar<=ROOM_MONITOR6)
                  if (Sim.RoomBusy[StatePar]>0)
                     Sim.RoomBusy[StatePar]--;

               if (PersonalRand.Rand(4)==0)
               {
                  MoodCountdown   = 0;
                  State  = PERSON_2EXIT;
                  Target = Airport.GetRandomExit(&PersonalRand);
                  Mood   = (UBYTE)MoodPersonHome;
               }
               else
               {
                  PERSON::State  = PERSON_2SHOP;
                  MoodCountdown  = 0;
                  Airport.GetRandomShop (Target, StatePar, Clans[(SLONG)ClanId].Type, &Mood, &PersonalRand);
               }
               break;

            case PERSON_2EXIT:
               State = PERSON_LEAVING;
               break;
         }
         break;

      //Der Koffer ist am Ausgang angekommen:
      case REASON_SUITCASE_EXIT:
         {
            BUILD *pBuild=Airport.GetBuildNear (ScreenPos, XY(180,80), Bricks((SLONG)0x10000000+BRICK_KASTEN));
            if (pBuild) Airport.Triggers[(SLONG)pBuild->Par].Winkel=Sim.TickerTime;

            State = PERSON_LEAVING;
         }
         break;

      //Die Person wollte zum Ausgang und ist dort angekommen:
      case REASON_LEAVING:
         switch (State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))
         {
            //Person ist am Shopeingang/ausgang angekommen:
            case PERSON_2SHOP:
               State = PERSON_SHOPPING | PERSON_BROWSEFLAG;
               Target = Airport.GetRandomTypedRune (RUNE_SHOP, (UBYTE)StatePar, false, &PersonalRand);
               break;

            //Person ist im Kern des Ladens angekommen:
            case PERSON_SHOPPING:
               Phase = 0;
               Dir = LookDir = 0;
               WaitCount = 150+(Target.y+Target.x+ClanId*33)%100;
               State |= PERSON_WAITFLAG;

               if (Clans[(SLONG)ClanId].Type!=30)
               {
                  Phase   = LookDir;
                  Dir     = LookDir = 8;
               }

               //Ggf. Koffer nehmen
               {
                  XY ArrayPos;

                  ArrayPos.x = (Position.x+4400)/44-100;
                  ArrayPos.y = (Position.y+2200)/22-100;

                  if (Airport.iPlate[ArrayPos.y+5-1+(ArrayPos.x<<4)]&2)
                  {
                     for (SLONG c=Sim.Persons.AnzEntries()-1; c>=0; c--)
                        if (Sim.Persons.IsInAlbum (c) && Clans[(SLONG)Sim.Persons[c].ClanId].Type==CLAN_SUITCASE)
                           if (abs(Sim.Persons[c].Position.x-Position.x)<16 && abs(Sim.Persons[c].Position.y-(Position.y-22))<8)
                           if (Clans[(SLONG)Sim.Persons[c].ClanId].HasSuitcase==-Clans[(SLONG)ClanId].HasSuitcase)
                           {
                              //Koffer entfernen:
                              Sim.Persons[c].State=PERSON_LEAVING;
                              WaitCount = 110;
                              State   = PERSON_2EXIT | PERSON_BROWSEFLAG | PERSON_WAITFLAG;
                              Target  = Airport.GetRandomExit(&PersonalRand);
                              Phase   = 0;
                              Dir     = 2;
                              LookDir = 2;

                              if (StatePar>=ROOM_BELT_X1 && StatePar<=ROOM_BELT_X10)
                                 if (Sim.RoomBusy[StatePar]>0)
                                    Sim.RoomBusy[StatePar]--;

                              //Person verwandeln:
                              for (SLONG c=0; c<SLONG(Clans.AnzEntries()); c++)
                                 if (Clans.IsInAlbum(c) && Clans[c].HasSuitcase==-Clans[(SLONG)ClanId].HasSuitcase && Clans[c].Type!=30)
                                 {
                                    ClanId = (UBYTE)c;
                                    break;
                                 }

                              break;
                           }
                  }
               }
               break;

            //Person ist am Shopeingang/ausgang angekommen:
            case PERSON_EXSHOP:
               if (StatePar>=ROOM_REISE_X1 && StatePar<=ROOM_MONITOR6)
                  if (Sim.RoomBusy[StatePar]>0)
                     Sim.RoomBusy[StatePar]--;

               MoodCountdown   = 0;
               State  = PERSON_2EXIT;
               Target = Airport.GetRandomExit(&PersonalRand);
               Mood   = (UBYTE)MoodPersonHome;
               break;

            case PERSON_2EXIT:
               State = PERSON_LEAVING;
               break;
         }
         break;

      case REASON_FLYING:
         {
            const CFlugplanEintrag *fpe = GetFlugplanEintrag();

            switch (State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))
            {
               //Person steht jetzt in der Warteschlange drin:
               case PERSON_2CHECKIN:
                  State  = PERSON_CHECKINGIN;
                  if (fpe->Gate==-1) State = PERSON_LEAVING;
                                else Target = Airport.GetRandomTypedRune (RUNE_CHECKIN, (UBYTE)fpe->Gate, false, &PersonalRand);
                  if (FlightAirline==Sim.localPlayer && Sim.Tutorial==1602)
                  {
                     Sim.Tutorial=1603;
                     Sim.Players.Players[Sim.localPlayer].Messages.NextMessage();
                     Sim.Players.Players[Sim.localPlayer].Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 1603));
                  }
                  break;

               //Hat in Sekundenschnelle ein Ticket erhalten:
               case PERSON_CHECKINGIN:
                  Mood   = (UBYTE)(FirstClass?MoodPersonFlyingFC:MoodPersonFlying);
                  Dir    = 1;

                  if (Clans[(SLONG)ClanId].HasSuitcase>0)
                  {
                     State  = PERSON_DROPPING_SUICASE;
                     if (fpe->Gate==-1) State = PERSON_LEAVING;
                                   else Target = Airport.GetRandomTypedRune (RUNE_DROPSUITCASE, (UBYTE)fpe->Gate, false, &PersonalRand);
                  }
                  else
                  {
                     //State  = PERSON_2WAITROOM;
                     Airport.iPlate[((Position.y+2200)/22-100)+5+(((Position.x+4400)/44-100)<<4)] &= (~2);
                     State  = PERSON_2CHECKIN_EXIT;
                     Target = Airport.GetRandomTypedRune (RUNE_2CHECKIN_EXIT, 0, false, &PersonalRand);
                  }
                  break;

               //Hat in Sekundenschnelle ein Ticket erhalten:
               case PERSON_DROPPING_SUICASE:
                  {
                     State     = PERSON_2CHECKIN_EXIT | PERSON_WAITFLAG;
                     WaitCount = 200;

                     //Koffer erzeugen:
                     Sim.PersonQueue.AddPerson (
                            Clans.GetSuitcaseId(Clans[(SLONG)ClanId].HasSuitcase),
                            Position-XY(0,22), 
                            REASON_SUITCASE_EXIT,
                            0,
                            0,
                            0);

                     if (Clans[(SLONG)ClanId].HasSuitcase)
                        for (SLONG c=0; c<SLONG(Clans.AnzEntries()); c++)
                           if (Clans.IsInAlbum(c) && Clans[c].HasSuitcase==-Clans[(SLONG)ClanId].HasSuitcase)
                           {
                              ClanId=(UBYTE)c;
                              break;
                           }

                     Airport.iPlate[((Position.y+2200)/22-100)+5+(((Position.x+4400)/44-100)<<4)] &= (~2);

                     Target  = Airport.GetRandomTypedRune (RUNE_2CHECKIN_EXIT, 0, false, &PersonalRand);
                     LookDir = 0;
                     Phase   = 0;
                     Dir     = 1;
                  }
                  break;

               //Person ist am Ausgang des engen Check-In Bereichs angekommen:
               case PERSON_2CHECKIN_EXIT:
                     State   = PERSON_2WAITROOM | PERSON_WAITFLAG;
                     if (fpe->Gate==-1) State = PERSON_LEAVING;
                                   else Target  = Airport.GetRandomTypedRune (RUNE_2WAIT, (UBYTE)fpe->Gate, false, &PersonalRand);
                     break;

               //Ist am Warteraum angekommen und läuft dort ein bischen umher, steigt direkt ein oder geht wieder:
               case PERSON_2WAITROOM:
                  {
                     if (FlightAirline==Sim.localPlayer && Sim.Tutorial==1603 && Sim.IsTutorial)
                     {
                        Sim.Tutorial=1604;
                        Sim.Players.Players[Sim.localPlayer].Messages.NextMessage();
                        Sim.Players.Players[Sim.localPlayer].Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_TUTORIUM, 1604));
                     }

                     if (Sim.Players.Players[(SLONG)FlightAirline].Planes[FlightPlaneId].Ort==-3 &&
                         Sim.Players.Players[(SLONG)FlightAirline].Planes[FlightPlaneId].AirportPos.x==Sim.Players.Players[(SLONG)FlightAirline].Planes[FlightPlaneId].TargetX)
                     {
                        //Sofort boarden:
                        State = bFirstClass ? PERSON_BOARDING : PERSON_2DURCHLEUCHTER;
                        if (fpe->Gate==-1) State = PERSON_LEAVING;
                                      else if (bFirstClass) Target= Airport.GetRandomTypedRune (RUNE_WAIT, (UBYTE)fpe->Gate, false, &PersonalRand);
                                      else Target= Airport.GetRandomTypedRune (RUNE_DURCHLEUCHTER, (UBYTE)fpe->Gate, false, &PersonalRand);
                     }
                     else if (fpe==NULL || (fpe->Startzeit==(SLONG)Sim.GetHour() && Sim.GetMinute()>=55) || fpe->Startzeit<(SLONG)Sim.GetHour())
                     {
                        //Flugzeug ist schon weg:
                        Mood   = (UBYTE)(FirstClass?MoodPersonMissedFC:MoodPersonMissed);
                        State  = PERSON_2EXIT;
                        Target = Airport.GetRandomExit(&PersonalRand);
                     }
                     else
                     {
                        //Warten:
                        State = PERSON_WAITING | PERSON_BROWSEFLAG;
                     }
                  }
                  break;

               //Am Durchleuchter angekommen, ist das Flugzeug noch da?
               case PERSON_2DURCHLEUCHTER:
                  State  = PERSON_BOARDING;
                  Target = Airport.GetRandomTypedRune (RUNE_WAIT, (UBYTE)fpe->Gate, false, &PersonalRand);

                  if (Sim.Players.Players[(SLONG)FlightAirline].SecurityFlags&(1<<8))
                  {
                     if (Sim.Players.Players[(SLONG)FlightAirline].SecurityFlags&(1<<11))
                     {
                        State    |= PERSON_WAITFLAG;
                        WaitCount = 240;
                        Sim.AddSmacker ("gate-shw.smk", 768+fpe->Gate, XY(2,0));
                     }
                     else if (Sim.Players.Players[(SLONG)FlightAirline].SecurityFlags&(1<<10))
                     {
                        State    |= PERSON_WAITFLAG;
                        WaitCount = 240;
                        Sim.AddSmacker ("gate-hb.smk", 760+fpe->Gate, XY(1,0));
                        Sim.AddSmacker ("gate-ha.smk", 768+fpe->Gate, XY(2,0));
                     }
                     else
                     {
                        //Trap!
                        if (PersonalRand.Rand(30)==0)
                        {
                           State = PERSON_LEAVING;
                           Sim.AddSmacker ("gate-trp.smk", 760+fpe->Gate, XY(10-3,55+1));
                        }
                     }
                  }

                  //!//State = PERSON_ENTERINGPL;
                  //!//Target= Airport.GetRandomTypedRune (RUNE_WAITPLANE, (UBYTE)fpe->Gate, false, &PersonalRand);
                  break;

               //Am Schlauch angekommen, ist das Flugzeug noch da?
               case PERSON_BOARDING:
                  if (Sim.Players.Players[(SLONG)FlightAirline].Planes[FlightPlaneId].Ort==-3 &&
                      Sim.Players.Players[(SLONG)FlightAirline].Planes[FlightPlaneId].AirportPos.x==Sim.Players.Players[(SLONG)FlightAirline].Planes[FlightPlaneId].TargetX)
                  {
                     State = PERSON_ENTERINGPL;
                     if (fpe->Gate==-1) State = PERSON_LEAVING;
                                   else Target= Airport.GetRandomTypedRune (RUNE_WAITPLANE, (UBYTE)fpe->Gate, false, &PersonalRand);
                  }
                  else
                  {
                     //Nö!
                     Mood   = (UBYTE)(FirstClass?MoodPersonMissedFC:MoodPersonMissed);
                     State  = PERSON_2EXIT;
                     Target = Airport.GetRandomExit(&PersonalRand);
                  }
                  break;

               //Person ist jetzt am Flugzeug und völlig verdeckt
               case PERSON_ENTERINGPL:
                  State = PERSON_LEAVING;
                  break;

               case PERSON_LEAVINGPL:
                  break;

               //Person ist am Shopeingang/ausgang angekommen:
               case PERSON_2SHOP:
                  if (Clans[(SLONG)ClanId].Type!=30)
                  {
                     Phase   = 0;
                     Dir     = LookDir = 8;
                  }

                  State = PERSON_SHOPPING | PERSON_BROWSEFLAG;
                  Target = Airport.GetRandomTypedRune (RUNE_SHOP, (UBYTE)StatePar, false, &PersonalRand);
                  break;

               //Person ist im Kern des Ladens angekommen:
               case PERSON_SHOPPING:
                  Dir = LookDir = 0;
                  WaitCount = 150+(Target.y+Target.x+ClanId*33)%100;
                  State  = PERSON_EXSHOP | PERSON_BROWSEFLAG | PERSON_WAITFLAG;
                  Target = Airport.GetRandomTypedRune (RUNE_2SHOP, (UBYTE)StatePar, false, &PersonalRand);
                  break;

               //Person ist am Shopeingang/ausgang angekommen:
               case PERSON_EXSHOP:
                  State  = PERSON_2EXIT;
                  Target = Airport.GetRandomExit(&PersonalRand);
                  Mood   = (UBYTE)MoodPersonHome;
                  break;

               case PERSON_2EXIT:
                  State = PERSON_LEAVING;
                  break;

               default:
                  break;
            }
         }
         break;

      case REASON_WAYPOINT:
         if (FlightPlaneId==1)
         {
            State   = State & ~PERSON_WAITFLAG;
            LookDir = 4; //Gimmick starten
            Phase   = 0;
         }
         {
            UBYTE OldWayPoint=FlightPlaneIndex;

            Target = Airport.GetNextWaypointRune (FlightAirline, &FlightPlaneIndex, &FlightPlaneId);

            if (OldWayPoint>FlightPlaneIndex && Clans[(SLONG)ClanId].Group==90)
               State = PERSON_LEAVING;
         }
         break;
   }
}

//--------------------------------------------------------------------------------------------
//Gibt eine Referenz auf den aktuellen Flugplan zurück:
//--------------------------------------------------------------------------------------------
const CFlugplanEintrag *PERSON::GetFlugplanEintrag (void)
{
   if (FlightAirline>=0 && FlightAirline<=3)
   {
      return (&Sim.Players.Players[(SLONG)FlightAirline].Planes[FlightPlaneId].Flugplan.Flug[(SLONG)FlightPlaneIndex]);
   }

   return (NULL);
}

//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
PERSONS::PERSONS () : ALBUM<PERSON> (Persons, "Persons")
{
   Persons.ReSize (800);
}

//--------------------------------------------------------------------------------------------
//Sortiert alle Personen gemäß ihrer Tiefe:
//--------------------------------------------------------------------------------------------
void PERSONS::DepthSort (void)
{
   Sort ();  //Album-Template Routine verwenden
}

//--------------------------------------------------------------------------------------------
//Entfernt alle Animationen an der genannten Stelle:
//--------------------------------------------------------------------------------------------
void PERSONS::RemoveAnimationNear (XY Position)
{
   SLONG Max=AnzEntries();

   for (SLONG c=0; c<Max; c++)
   {
      if (IsInAlbum (c))
      {
         PERSON &qPerson = (*this)[c];

         if (abs(qPerson.Position.x-Position.x)<44 && abs(qPerson.Position.y-Position.y)<22 && Clans[(SLONG)qPerson.ClanId].Type==CLAN_ANIMATION)
         {
            qPerson.State=PERSON_LEAVING;
         }
      }
   }
}

//--------------------------------------------------------------------------------------------
//Alle Personen gehen einen Schritt weiter:
//--------------------------------------------------------------------------------------------
void PERSONS::DoOneStep (void)
{
   SLONG c;
   static BOOL LastMode=-1;
   static BOOL LastDate=-1;
   static SLONG Indexes[4]; //Speedup-Tabelle

   if (Sim.AnyPersonsInAirport==FALSE && Sim.Time<9*60000) return;

   if (Sim.CallItADay)
   {
      for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
         if (Sim.Players.Players[c].SpeedCount && !Sim.Players.Players[c].IsOut && Sim.Players.Players[c].Owner)
         {
            Sim.Players.Players[c].SpeedCount--;

            if (Sim.Players.Players[c].SpeedCount==0)
               Sim.Players.Players[c].RobotExecuteAction();

         }
      return;
   }

   if (Sim.CallItADay!=LastMode || Sim.Date!=LastDate)
   {
      LastMode=Sim.CallItADay;
      LastDate=Sim.Date;

      if (LastMode)
         for (c=0; c<4; c++)
            if (!Sim.Players.Players[c].IsOut)
               Indexes[c]=Sim.Persons.GetPlayerIndex(c);
   }

   if (!Sim.CallItADay)
   {
      //Personen, die warten werden nur berücksichtigt wenn Ostern auf Pfingsten fällt:
      UBYTE Waiting = UBYTE(Sim.TimeSlice);

      if (Sim.Gamestate&GAMESTATE_QUIT)
      {
         //Quitting ==> Winke-Modus
      }
      else
      {
         Bench.WalkTime.Start();
         //Normaler Spiel-Modus ==> Einen Schritt weiter gehen:
         SLONG Max=AnzEntries();

         for (SLONG c=0; c<Max; c++)
         {
            if (IsInAlbum (c))
            {
               PERSON &qPerson = (*this)[c];

               if (qPerson.MoodCountdown)
               {
                  qPerson.MoodCountdown--;
                  if (qPerson.MoodCountdown==(MOODCOUNT_START>>1))
                     qPerson.MoodCountdown-=rand()%30;
               }

               switch (Clans[(SLONG)qPerson.ClanId].Type)
               {
                  case CLAN_MALE:
                  case CLAN_FEMALE:
                  case CLAN_BROWNMALE:
                  case CLAN_BROWNFEMALE:
                  case CLAN_SUITCASE:
                     if (!Sim.CallItADay && (Sim.Time>8*60000 && Sim.Time<18*60000))
                     {
                        if ((qPerson.State&PERSON_TALKING)==0)
                        {
                           if (qPerson.State == (PERSON_EXSHOP | PERSON_BROWSEFLAG | PERSON_WAITFLAG))
                           {
                              //Im Geschäft warten
                              qPerson.WaitCount-=5;
                              if (qPerson.WaitCount<20) qPerson.State = (PERSON_EXSHOP | PERSON_BROWSEFLAG);
                           }
                           else
                           {
                              //if ((qPerson.State & PERSON_WAITFLAG) && (qPerson.State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))!=PERSON_CHECKINGIN && (qPerson.State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))!=PERSON_BOARDING)
                              if ((qPerson.State & PERSON_WAITFLAG) && (qPerson.State & (~PERSON_WAITFLAG) & (~PERSON_BROWSEFLAG))!=PERSON_CHECKINGIN)
                              {
                                 if (qPerson.WaitCount<=250) qPerson.WaitCount++;
                              }
                              else if (qPerson.WaitCount>10)
                              {
                                 qPerson.WaitCount-=10;
                              }

                              if (!(qPerson.State & PERSON_WAITFLAG) || !((Waiting+qPerson.ClanId)&3))
                              {
                                 if (Sim.Time>9*60000 || qPerson.Reason!=REASON_WAYPOINT)
                                 {
                                    qPerson.DoOneCustomerStep();

                                    if (qPerson.Running)
                                       qPerson.DoOneCustomerStep();
                                 }

                                 //Falls Person am Konstruktor angekommen, ggf. entfernen
                                 if (qPerson.State==PERSON_LEAVING)
                                 {
                                    XY ArrayPos;

                                    ArrayPos.x = qPerson.Position.x/44;
                                    ArrayPos.y = qPerson.Position.y/22;

                                    Airport.iPlate[ArrayPos.y+5+(ArrayPos.x<<4)] &= (~2);

                                    qPerson.ClanId=255;
                                    (*this)-=c;
                                 }
                              }
                           }
                        }
                     }
                     break;

                  case CLAN_PLAYER1:
                  case CLAN_PLAYER2:
                  case CLAN_PLAYER3:
                  case CLAN_PLAYER4: //Falls Person auf Flughafensicht, dann laufen:
                     {
                        PLAYER &qPlayer = Sim.Players.Players[(SLONG)qPerson.State];
                        if (qPlayer.GetRoom()==ROOM_AIRPORT && !qPlayer.IsTalking && (qPlayer.LocationWin==NULL || ((*(CStdRaum*)qPlayer.LocationWin).CurrentMenu!=MENU_WC_F && (*(CStdRaum*)qPlayer.LocationWin).CurrentMenu!=MENU_WC_M)))
                        if (qPlayer.Stunned==0)
                        if (Sim.Difficulty!=DIFF_TUTORIAL || Sim.Tutorial>=1100 || !Sim.IsTutorial)
                        {
                           qPerson.DoOnePlayerStep();

                           if (qPlayer.IsStuck==0 && qPerson.Running && (qPerson.Position.y/22+5!=13 || (Airport.iPlate[(qPerson.Position.y/22+5)+((qPerson.Position.x/44)<<4)] & 240)!=80))
                           if (qPlayer.GetRoom()==ROOM_AIRPORT)
                           {
                              qPlayer.UpdateWaypointWalkingDirection ();

                              UBYTE Phase = qPerson.Phase;
                              qPerson.DoOnePlayerStep();

                              if (qPlayer.IsStuck==0)
                                 if (qPerson.Dir<8) qPerson.Phase = Phase;
                           }

                           if (qPlayer.Koffein)
                           {
                              if (qPlayer.GetRoom()==ROOM_AIRPORT) { qPlayer.UpdateWaypointWalkingDirection (); qPerson.DoOnePlayerStep(); }
                              if (qPlayer.GetRoom()==ROOM_AIRPORT) { qPlayer.UpdateWaypointWalkingDirection (); qPerson.DoOnePlayerStep(); }
                           }

                           if (CheatRunningman)
                           {
                              if (qPlayer.GetRoom()==ROOM_AIRPORT) { qPlayer.UpdateWaypointWalkingDirection (); qPerson.DoOnePlayerStep(); }
                              if (qPlayer.GetRoom()==ROOM_AIRPORT) { qPlayer.UpdateWaypointWalkingDirection (); qPerson.DoOnePlayerStep(); }
                              if (qPlayer.GetRoom()==ROOM_AIRPORT) { qPlayer.UpdateWaypointWalkingDirection (); qPerson.DoOnePlayerStep(); }
                           }
                        }
                     }
                     break;

                  case CLAN_ANIMATION:
                     if (qPerson.State==PERSON_LEAVING)
                     {
                        if (Clans[(SLONG)qPerson.ClanId].Group==10)
                        {
                           XY    ArrayPos;

                           //Klebstoff:
                           ArrayPos.x = qPerson.Position.x/44;
                           ArrayPos.y = (qPerson.Position.y-5000)/22;
                           Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)] &= (~1);
                        }

                        (*this)-=c;
                     }
                     else
                        qPerson.DoOneAnimationStep ();
                     break;
               }
            }
            else break;
         }
         Bench.WalkTime.Stop();
         Bench.SortTime.Start();
         DepthSort();
         Bench.SortTime.Stop();
      }
   }
   else //schnelle Version tür Tagesabbruch
   {
      for (c=0; c<Sim.Players.AnzPlayers; c++)
         if (!Sim.Players.Players[c].IsOut)
         {
            if (Clans[(SLONG)(*this)[Indexes[c]].ClanId].Type==CLAN_MALE) DebugBreak();
            if (Clans[(SLONG)(*this)[Indexes[c]].ClanId].Type==CLAN_FEMALE) DebugBreak();
            if (Clans[(SLONG)(*this)[Indexes[c]].ClanId].Type==CLAN_BROWNMALE) DebugBreak();
            if (Clans[(SLONG)(*this)[Indexes[c]].ClanId].Type==CLAN_BROWNFEMALE) DebugBreak();
            if (Sim.Players.Players[c].GetRoom()==ROOM_AIRPORT) (*this)[Indexes[c]].DoOnePlayerStep();
         }
   }
}

//--------------------------------------------------------------------------------------------
//Sucht den Spieler mit dem angebenen Index raus:
//--------------------------------------------------------------------------------------------
ULONG PERSONS::GetPlayerIndex (SLONG Number)
{
   //for (SLONG c=0; c<(SLONG)AnzEntries(); c++)
   for (SLONG c=AnzEntries()-1; c>=0; c--)
      if (IsInAlbum (c))
         if (Clans[(SLONG)(*this)[c].ClanId].Type == CLAN_PLAYER1+Number)
            return (c);

   return (0xffffffff);
}

//--------------------------------------------------------------------------------------------
//Wer will gerade nur einkaufen:
//--------------------------------------------------------------------------------------------
SLONG PERSONS::GetNumShoppers (void)
{
   SLONG c, n=0;

   for (c=(SLONG)AnzEntries()-1; c>=0; c--)
      if (IsInAlbum (c))
         if (Clans[(SLONG)(*this)[c].ClanId].Type<CLAN_PLAYER1 &&
             (*this)[c].Reason==REASON_SHOPPING)
               n++;

   return (n);
}

//--------------------------------------------------------------------------------------------
//Schaut, ob Stimmungen angezeigt werden müssen:
//--------------------------------------------------------------------------------------------
void PERSONS::TryMoods (void)
{
   SLONG Indexes[4];
   XY    Positions[4];
   SLONG c, d;
   XY    p;
   XY   &qViewPos = Sim.Players.Players[(SLONG)Sim.localPlayer].ViewPos;

   if (Sim.GetHour()==9 && Sim.GetMinute()==0) return;

   for (c=0; c<4; c++)
      Indexes[c]=-1;

   if (Editor==EDITOR_NONE)
   {
      for (c=AnzEntries()-1; c>=0; c--)
         if (IsInAlbum (c))
            if (Clans[(SLONG)(*this)[c].ClanId].Type>=CLAN_PLAYER1 && Clans[(SLONG)(*this)[c].ClanId].Type<=CLAN_PLAYER4)
               if (!(*this)[c].StatePar && Sim.Players.Players[(SLONG)(*this)[c].State].GetRoom()==ROOM_AIRPORT)
               {
                  Indexes [(*this)[c].State]=c;
                  Positions[(*this)[c].State]=(*this)[c].Position;
               }

      for (c=0; c<4; c++)
         if (Indexes[c]!=-1)
            for (d=0; d<4; d++)
               if (c!=d && Indexes[d]!=-1)
               {
                  p = Positions[c]-Positions[d];
                  p.x = abs(p.x);
                  p.y = abs(p.y);

                  if ((p.x<80 && p.y<60) || CheatBubbles)
                  {
                     if (Sim.Players.Players[c].IsStuck)
                     {
                        (*this)[Indexes[c]].Mood=(UBYTE)MoodPersonStuck;

                        if ((*this)[Indexes[c]].MoodCountdown)
                           (*this)[Indexes[c]].MoodCountdown=MOODCOUNT_START-16;
                        else
                           (*this)[Indexes[c]].MoodCountdown=MOODCOUNT_START+rand()%15;
                     }
                     else if (Sim.Players.Players[c].Owner==1)
                     {
                        SLONG s=Sim.Players.Players[c].Sympathie[d];

                        if (s>50) (*this)[Indexes[c]].Mood=0;
                        else if (s>10) (*this)[Indexes[c]].Mood=1;
                        else if (s>-10) (*this)[Indexes[c]].Mood=2;
                        else if (s>-50) (*this)[Indexes[c]].Mood=3;
                        else (*this)[Indexes[c]].Mood=4;

                        if (Sim.Players.Players[c].RunningToToilet)
                           (*this)[Indexes[c]].Mood = (UBYTE)MoodPersonToilet;

                        if ((*this)[Indexes[c]].MoodCountdown)
                           (*this)[Indexes[c]].MoodCountdown=MOODCOUNT_START-16;
                        else
                           (*this)[Indexes[c]].MoodCountdown=MOODCOUNT_START+rand()%15;
                     }
                  }
               }

      for (c=AnzEntries()-1; c>=0; c--)
         if (IsInAlbum (c))
         {
            PERSON &qPerson = (*this)[c];

            p = gMousePosition+qViewPos-(qPerson.ScreenPos-XY(0,40));
            p.x = abs(p.x);
            p.y = abs(p.y);

            if ((p.x<40 && p.y<80) || CheatBubbles)
            {
               if (Clans[(SLONG)qPerson.ClanId].Type>=CLAN_PLAYER1 && Clans[(SLONG)qPerson.ClanId].Type<=CLAN_PLAYER4)
               {
                  PLAYER &qPlayer = Sim.Players.Players[(SLONG)qPerson.State];

                  if (qPlayer.IsStuck)
                  {
                     qPerson.Mood=(UBYTE)MoodPersonStuck;

                     if (qPerson.MoodCountdown)
                        qPerson.MoodCountdown=MOODCOUNT_START-16;
                     else
                        qPerson.MoodCountdown=MOODCOUNT_START+rand()%15;
                  }
                  else if (Indexes[(SLONG)qPerson.State]!=-1 && Sim.Players.Players[(SLONG)qPerson.State].Owner==1)
                  {
                     SLONG s=qPlayer.Sympathie[Sim.localPlayer];

                     if (s>50) qPerson.Mood=0;
                     else if (s>10) qPerson.Mood=1;
                     else if (s>-10) qPerson.Mood=2;
                     else if (s>-50) qPerson.Mood=3;
                     else qPerson.Mood=4;

                     if (qPlayer.RunningToToilet)
                        qPerson.Mood = (UBYTE)MoodPersonToilet;

                     if (qPerson.MoodCountdown)
                     {
                        if (qPerson.MoodCountdown<16)
                           qPerson.MoodCountdown=MOODCOUNT_START-qPerson.MoodCountdown;
                        else if (qPerson.MoodCountdown<MOODCOUNT_START-16)
                           qPerson.MoodCountdown=MOODCOUNT_START-16-1;
                     }
                     else
                        qPerson.MoodCountdown=MOODCOUNT_START;
                  }
               }
               else if ((Clans[(SLONG)qPerson.ClanId].Type<30 && Clans[(SLONG)qPerson.ClanId].Group<30) || Clans[(SLONG)qPerson.ClanId].Group==240)
               {
                  if (qPerson.Mood)
                     if (qPerson.MoodCountdown)
                     {
                        if (qPerson.MoodCountdown<16)
                           qPerson.MoodCountdown=MOODCOUNT_START-qPerson.MoodCountdown;
                        else if (qPerson.MoodCountdown<MOODCOUNT_START-16)
                           qPerson.MoodCountdown=MOODCOUNT_START-16-1;
                     }
                     else
                        qPerson.MoodCountdown=MOODCOUNT_START+rand()%15;
               }
            }
         }
   }
}

//--------------------------------------------------------------------------------------------
//Speichert ein PERSON-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const PERSON &Person)
{
   if (SaveVersion==1 && SaveVersionSub==1)
   {
      File << Person.ClanId  << Person.Dir      << Person.Phase       << Person.Running;
      File << Person.State   << Person.StatePar << Person.WaitCount   << Person.FloorOffset;
      File << Person.Reason  << Person.FlightAirline;
      File << Person.FlightPlaneId     << Person.FlightPlaneIndex;
      File << Person.Target  << Person.Position << Person.ScreenPos;
      File << Person.LookDir << Person.Mood     << Person.MoodCountdown;
   }
   else if (SaveVersion==1 && SaveVersionSub>=2)
   {
      File << Person.ClanId;
      
      if (Person.ClanId!=255)
      {
         File << Person.Dir     << Person.Phase    << Person.Running;
         File << Person.State   << Person.StatePar << Person.WaitCount   << Person.FloorOffset;
         File << Person.Reason  << Person.FlightAirline;
         File << Person.FlightPlaneId     << Person.FlightPlaneIndex;
         File << Person.Target  << Person.Position << Person.ScreenPos;
         File << Person.LookDir << Person.Mood     << Person.MoodCountdown;
      }
   }
   else DebugBreak();

   File << Person.PersonalRand << Person.PersonalRandWalk << Person.FirstClass;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt ein PERSON-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, PERSON &Person)
{
   if (SaveVersion==1 && SaveVersionSub==1)
   {
      File >> Person.ClanId  >> Person.Dir      >> Person.Phase       >> Person.Running;
      File >> Person.State   >> Person.StatePar >> Person.WaitCount   >> Person.FloorOffset;
      File >> Person.Reason  >> Person.FlightAirline;
      File >> Person.FlightPlaneId     >> Person.FlightPlaneIndex;
      File >> Person.Target  >> Person.Position >> Person.ScreenPos;
      File >> Person.LookDir >> Person.Mood     >> Person.MoodCountdown;
   }
   else if (SaveVersion==1 && SaveVersionSub>=2)
   {
      File >> Person.ClanId;
      
      if (Person.ClanId!=255)
      {
         File >> Person.Dir     >> Person.Phase    >> Person.Running;
         File >> Person.State   >> Person.StatePar >> Person.WaitCount   >> Person.FloorOffset;
         File >> Person.Reason  >> Person.FlightAirline;
         File >> Person.FlightPlaneId     >> Person.FlightPlaneIndex;
         File >> Person.Target  >> Person.Position >> Person.ScreenPos;
         File >> Person.LookDir >> Person.Mood     >> Person.MoodCountdown;
      }
   }
   else DebugBreak();

   if (SaveVersion==1 && SaveVersionSub>=100)
   {
      File >> Person.PersonalRand >> Person.PersonalRandWalk >> Person.FirstClass;
   }
   else
   {
      Person.PersonalRand.SRand (0);
      Person.PersonalRandWalk.SRand (0);
   }

   return (File);
}

//--------------------------------------------------------------------------------------------
//Speichert ein PERSONS-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const PERSONS &Persons)
{
   File << Persons.Persons;
   File << *((ALBUM<PERSON>*)&Persons);

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt ein PERSONS-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, PERSONS &Persons)
{
   File >> Persons.Persons;
   File >> *((ALBUM<PERSON>*)&Persons);

   return (File);
}

//--------------------------------------------------------------------------------------------
//Person-Queues:
//--------------------------------------------------------------------------------------------
//Fügt eine Person in die Queue ein:
//--------------------------------------------------------------------------------------------
void CPersonQueue::AddPerson (UBYTE ClanId, XY Position, UBYTE Reason, UBYTE FlightAirline, SLONG FlightPlaneId, UBYTE FlightPlaneIndex, UBYTE Mood, UBYTE FirstClass)
{
   SLONG c, d;

   if (Sim.CallItADay || Sim.Time>18*60000 || Sim.Time<8*60000) return;

   // ==>+<== Nur zum testen und finden von Netzwerk-Bugs:
   //for (c=0; c<Reason; c++)
   //   LastMinuteAuftraege.Random.Rand(1, 10);

   //log: hprintf ("CPersonQueue::AddPerson (%li, %li|%li...", ClanId, Position.x, Position.y);

   if (!Clans[(SLONG)ClanId].TodayInGame) DebugBreak();

   Sim.AnyPersonsInAirport=TRUE;

   //Suchen, ob an der Stelle erst kürzlich jemand erschaffen wurde:
   for (c=0; c<Spots.AnzEntries(); c++)
   {
      if (Spots[c].TimeSlice==-1)
         Spots[c].Position=Position;

      if (Spots[c].Position==Position)
      {
         //Die Personenliste nach einem freien Platz durchsuchen
         for (d=0; d<Person.AnzEntries(); d++)
         {
            if (d==Person.AnzEntries()-2) //Ggf. Array vergrößern
               Person.ReSize (Person.AnzEntries()+10);

            //Wenn das zutrifft, dann haben wir einen freien Platz gefunden:
            if (Person[d].TimeSlice==-1)
            {
               if (Spots[c].TimeSlice==-1 || Spots[c].TimeSlice<Sim.TimeSlice)
               {
                  //Koffer sofort erzeugen:
                  if (Clans[SLONG(ClanId)].Type==30)
                     Spots[c].TimeSlice = Person[d].TimeSlice = Sim.TimeSlice; //Koffer!
                  else
                     Spots[c].TimeSlice = Person[d].TimeSlice = Sim.TimeSlice+10;  //in 0,5 sec erzeugen
               }
               else
               {
                  Spots[c].TimeSlice+=20;  //eine Sekunde später
                  Person[d].TimeSlice = Spots[c].TimeSlice;
               }

               //log: hprintf ("CPersonQueue::AddPerson (%li, %li|%li, d=%li, TimeSlice=%li (vs %li)...", ClanId, Position.x, Position.y, (SLONG)d, (SLONG)Person[d].TimeSlice, Sim.TimeSlice);

               Person[d].ClanId           = ClanId;
               Person[d].Position         = Position;
               Person[d].Reason           = Reason;
               Person[d].FlightAirline    = FlightAirline;
               Person[d].FlightPlaneId    = FlightPlaneId;
               Person[d].FlightPlaneIndex = FlightPlaneIndex;
               Person[d].Mood             = Mood;
               Person[d].FirstClass       = FirstClass;
               return;
            }
         }

         DebugBreak();
      }
   }

   DebugBreak();
}

//--------------------------------------------------------------------------------------------
// Forces the TimeSlice of a certain spot to a certain value:
//--------------------------------------------------------------------------------------------
void CPersonQueue::SetSpotTime (XY Position, SLONG TimeSlice)
{
   SLONG c;

   if (Sim.CallItADay || Sim.Time>18*60000 || Sim.Time<8*60000) return;

   //Suchen, ob an der Stelle erst kürzlich jemand erschaffen wurde:
   for (c=0; c<Spots.AnzEntries(); c++)
   {
      if (Spots[c].TimeSlice==-1)
         Spots[c].Position=Position;

      if (Spots[c].Position==Position)
      {
         Spots[c].TimeSlice = TimeSlice;
         return;
      }
   }
}

//--------------------------------------------------------------------------------------------
//Überwacht die Queue:
//--------------------------------------------------------------------------------------------
void CPersonQueue::Pump (void)
{
   SLONG c;
   SLONG Anz; //, MinPriority;

   do
   {
      Anz=0;
      //MinPriority=2147483647;

      //Die Personenliste nach einer fälligen Person durchsuchen:
      /*for (c=Person.AnzEntries()-1; c>=0; c--)
      {
         if (Person[c].TimeSlice!=-1 && Person[c].TimeSlice<=Sim.TimeSlice)
         {
            Anz++;

            if (Person[c].Priority<MinPriority)
               MinPriority=Person[c].Priority;
         }
      } */

      //Die Personenliste nach einer fälligen Person durchsuchen:
      for (c=Person.AnzEntries()-1; c>=0; c--)
      {
         if (Person[c].TimeSlice!=-1 && Person[c].TimeSlice<=Sim.TimeSlice /*&& Person[c].Priority==MinPriority*/)
         {
            Person[c].TimeSlice=-1;

            if (Sim.Persons.GetNumFree()<2) //Ggf. Array vergrößern
               Sim.Persons.Persons.ReSize (Sim.Persons.Persons.AnzEntries()+10);

            //log: hprintf ("CPersonQueue::Pump (%li, %li|%li...", Person[c].ClanId, Person[c].Position.x, Person[c].Position.y);

            Sim.Persons*=PERSON (
               Person[c].ClanId,
               Person[c].Position,
               Person[c].Reason,
               Person[c].FlightAirline,
               Person[c].FlightPlaneId,
               Person[c].FlightPlaneIndex,
               Person[c].Mood,
               Person[c].FirstClass);

            Anz--;
            break;
         }
      }
   }
   while (Anz>0);
}

//--------------------------------------------------------------------------------------------
//neuer Tag ==> Queue leeren:
//--------------------------------------------------------------------------------------------
void CPersonQueue::NewDay (void)
{
   Person.ReSize (0);
   Person.ReSize (50);
   Spots.ReSize (0);
   Spots.ReSize (50);
}

//--------------------------------------------------------------------------------------------
// Resets all TimeSlice field from the Spots Array:
//--------------------------------------------------------------------------------------------
void CPersonQueue::ResetSpotTimeSlices (void)
{
   SLONG c;

   for (c=0; c<Spots.AnzEntries(); c++)
      Spots[c].TimeSlice=-1;
}

//--------------------------------------------------------------------------------------------
//Speichert ein CQueuedPerson-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CQueuedPerson &p)
{
   if (SaveVersion==1 && SaveVersionSub==1)
   {
      File << p.TimeSlice     << p.Priority;
      File << p.ClanId        << p.Reason;
      File << p.FlightAirline << p.FlightPlaneId << p.FlightPlaneIndex;
      File << p.Mood          << p.Position;
   }
   else if (SaveVersion==1 && SaveVersionSub>=2)
   {
      File << p.TimeSlice;

      if (p.TimeSlice!=-1)
      {
         File << p.Priority;
         File << p.ClanId        << p.Reason;
         File << p.FlightAirline << p.FlightPlaneId << p.FlightPlaneIndex;
         File << p.Mood          << p.Position;
      }
   }
   else DebugBreak();

   File << p.FirstClass;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt ein CQueuedPerson-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CQueuedPerson &p)
{
   if (SaveVersion==1 && SaveVersionSub==1)
   {
      File >> p.TimeSlice     >> p.Priority;
      File >> p.ClanId        >> p.Reason;
      File >> p.FlightAirline >> p.FlightPlaneId >> p.FlightPlaneIndex;
      File >> p.Mood          >> p.Position;
   }
   else if (SaveVersion==1 && SaveVersionSub>=2)
   {
      File >> p.TimeSlice;

      if (p.TimeSlice!=-1)
      {
         File >> p.Priority;
         File >> p.ClanId        >> p.Reason;
         File >> p.FlightAirline >> p.FlightPlaneId >> p.FlightPlaneIndex;
         File >> p.Mood          >> p.Position;
      }
   }
   else DebugBreak();

   File >> p.FirstClass;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Speichert ein CQueueSpot-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CQueueSpot &s)
{
   File << s.TimeSlice << s.Position;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt ein CQueueSpot-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CQueueSpot &s)
{
   File >> s.TimeSlice >> s.Position;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Speichert ein CPersonQueue-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CPersonQueue &q)
{
   File << q.Person << q.Spots;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt ein CPersonQueue-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CPersonQueue &q)
{
   File >> q.Person >> q.Spots;

   return (File);
}
