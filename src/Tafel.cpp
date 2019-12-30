//============================================================================================
// Tafel.cpp : Das schwarze Brett der Flugaufsicht
//============================================================================================
// Link: "Tafel.h"
//============================================================================================
#include "stdafx.h"
#include "gltafel.h"
#include "Tafel.h"         //Die Tafel bei der Flughafenaufsicht
#include "atnet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Zum debuggen:
static const char FileId[] = "Tafl";

SLONG ZettelPos [14*3]=
   {
       41,15,   245,278, 180,32,  375,281, 322,12,  000,000, 000,000,     //Route
       318,138, 470,61,   18,156, 155,170, 000,000, 000,000, 000,000,     //City
       10,289,  132,258, 466,182,  18,156, 375,281, 318,138, 245,278      //Gate
   };

//Daten des aktuellen Savegames beim laden:
extern SLONG SaveVersion;
extern SLONG SaveVersionSub;

//--------------------------------------------------------------------------------------------
//Konstruktor
//--------------------------------------------------------------------------------------------
CTafel::CTafel (BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "tafel.gli", GFX_TAFEL)
{
   ReloadBitmaps ();

   KommVar = -1;

   LastTafelData = TafelData;

   LeereZettelBms.ReSize (pRoomLib, "ZETTEL04", 3);
   PostcardBm.ReSize (pRoomLib, "NOCARD");

   for (SLONG c=0; c<21; c++) RepaintZettel (c);

   SP_Fliege.ReSize (5);
   SP_Fliege.Clips[0].ReSize (0, "FlyW01.smk", "", XY (554, 253), SPM_IDLE,   CRepeat(5,9), CPostWait(0,0),     SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, &KommVar,
                              "A9A9", 0, 1);
   SP_Fliege.Clips[1].ReSize (1, "FlyW02.smk", "", XY (554, 253), SPM_IDLE,   CRepeat(6,9), CPostWait(0,0),     SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, &KommVar,
                              "A9A9", 0, 1);
   SP_Fliege.Clips[2].ReSize (2, "Fly.smk", "Fly.raw", XY (554, 253), SPM_IDLE,      CRepeat(1,1), CPostWait(0,0),     SMACKER_CLIP_DONTCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,
                              "A9", 3);
   SP_Fliege.Clips[3].ReSize (3, "FlyOut.smk", "", XY (554, 253), SPM_IDLE,   CRepeat(9,9), CPostWait(200,600), SMACKER_CLIP_DONTCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,
                              "A9", 4);
   SP_Fliege.Clips[4].ReSize (4, "FlyB.smk", "FlyB.raw", XY (554, 253), SPM_IDLE,     CRepeat(1,1), CPostWait(0,0),     SMACKER_CLIP_DONTCANCEL,
                              &KommVar, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, NULL,
                              "A9", 0);

   SDL_ShowWindow(FrameWnd->m_hWnd);
   SDL_UpdateWindowSurface(FrameWnd->m_hWnd);
}

//--------------------------------------------------------------------------------------------
//Destruktor:
//--------------------------------------------------------------------------------------------
CTafel::~CTafel()
{
   LeereZettelBms.Destroy();
   PostcardBm.Destroy();
   Sim.Players.Players[(SLONG)PlayerNum].Messages.AddMessage (BERATERTYP_ROUTE, "", MESSAGE_COMMENT);

   TEAKFILE Message;

   Message.Announce(1024);

   Message << ATNET_TAKE_CITY;

   for (SLONG c=0; c<7; c++)
   {
      Message << TafelData.City[c].Player << TafelData.City[c].Preis;
      Message << TafelData.Gate[c].Player << TafelData.Gate[c].Preis;
   }

   Sim.SendMemFile (Message);
}

//--------------------------------------------------------------------------------------------
//Bitmaps reloaden:
//--------------------------------------------------------------------------------------------
void CTafel::ReloadBitmaps (void)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CTafel message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//void CTafel::OnPaint()
//--------------------------------------------------------------------------------------------
void CTafel::OnPaint()
{
   SLONG   c;
   BOOL    OnTip=FALSE;
   PLAYER &qPlayer = Sim.Players.Players[(SLONG)PlayerNum];

   if (!bHandy) SetMouseLook (CURSOR_NORMAL, 0, ROOM_ARAB_AIR, 0);

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   if (!(Sim.ItemPostcard && qPlayer.SeligTrust==0 && Sim.Difficulty!=DIFF_TUTORIAL))
      RoomBm.BlitFromT (PostcardBm, 16, 290);

   for (c=0; c<7; c++)
   {
      //Zettel malen:
      if (TafelData.City[c].ZettelId!=-1 && Sim.Players.Players[Sim.localPlayer].RentCities.RentCities[TafelData.City[c].ZettelId].Rang==0)
         if (!RoomBm.BlitFromT (ZettelBms[c+7], ZettelPos[(c+7)*2], ZettelPos[(c+7)*2+1]))
         {
            RepaintZettel (c+7);
            RoomBm.BlitFromT (ZettelBms[c+7], ZettelPos[(c+7)*2], ZettelPos[(c+7)*2+1]);
         }

      if (TafelData.Gate[c].ZettelId!=-1)
         if (!RoomBm.BlitFromT (ZettelBms[c+14], ZettelPos[(c+14)*2], ZettelPos[(c+14)*2+1]))
         {
            RepaintZettel (c+14);
            RoomBm.BlitFromT (ZettelBms[c+14], ZettelPos[(c+14)*2], ZettelPos[(c+14)*2+1]);
         }
   }

   if (OnTip==FALSE) LastTip=-1;

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   SP_Fliege.Pump ();
   SP_Fliege.BlitAtT (RoomBm);

   if (!IsDialogOpen() && !MenuIsOpen())
   {
      if (gMousePosition.IfIsWithin (580,369,640,423)) SetMouseLook (CURSOR_EXIT, 0, ROOM_TAFEL, 999);
      if (gMousePosition.IfIsWithin (500,280,640,424)) KommVar=2;

      if (Sim.ItemPostcard && qPlayer.SeligTrust==0 && Sim.Difficulty!=DIFF_TUTORIAL)
         if (gMousePosition.IfIsWithin (25,317,188,410) || gMousePosition.IfIsWithin (116,299,182,334) || gMousePosition.IfIsWithin (37,385,116,425)) SetMouseLook (CURSOR_HOT, 0, ROOM_TAFEL, 800);

      if (!(MouseClickArea==ROOM_TAFEL && MouseClickId==999))
      {
         //Auf einen der Zettel geklickt?
         for (c=0; c<21; c++)
         {
            if (gMousePosition.y<440 && ((c<7 && TafelData.Route[c].ZettelId) || (c>=7 && c<14 && TafelData.City[c-7].ZettelId) || (c>=14 && TafelData.Gate[c-14].ZettelId!=-1)))
            {
               if (XY(gMousePosition).IfIsWithin(ZettelPos[c*2], ZettelPos[c*2+1], ZettelPos[c*2]+LeereZettelBms[c%3].Size.x, ZettelPos[c*2+1]+LeereZettelBms[c%3].Size.y))
               {
                  if (c>=7 && c<14 && TafelData.City[c-7].Player!=PlayerNum && Sim.Players.Players[Sim.localPlayer].RentCities.RentCities[TafelData.City[c-7].ZettelId].Rang==0)
                     SetMouseLook (CURSOR_HOT, 0, ROOM_TAFEL, 0);
                  else if (c>=14 && TafelData.Gate[c-14].Player!=PlayerNum)
                     SetMouseLook (CURSOR_HOT, 0, ROOM_TAFEL, 0);
               }
            }
         }
      }
   }

   CStdRaum::PostPaint ();
   CStdRaum::PumpToolTips ();
}

//--------------------------------------------------------------------------------------------
//void CTafel::OnPaint()
//--------------------------------------------------------------------------------------------
void CTafel::RepaintZettel (SLONG n)
{
   SLONG p=0;

   if (n<7) //Route
   {
      if (TafelData.Route[n].ZettelId==0)
      {
         ZettelBms[n].Destroy();
      }
      else
      {
         ZettelBms[n].ReSize(LeereZettelBms[n%3].Size);
         ZettelBms[n].BlitFrom(LeereZettelBms[n%3]);

         ZettelBms[n].PrintAt (bprintf ("%s-%s", Cities [Routen[TafelData.Route[n].ZettelId].VonCity].Kuerzel, Cities [Routen[TafelData.Route[n].ZettelId].NachCity].Kuerzel),
                         FontSmallBlack, TEC_FONT_CENTERED,
                         XY(3, 30), XY(ZettelBms[n].Size.x-3, 202));

         ZettelBms[n].PrintAt (Cities [Routen[TafelData.Route[n].ZettelId].VonCity].Name,
                         FontSmallBlack, TEC_FONT_CENTERED,
                         XY(3, 34+15), XY(ZettelBms[n].Size.x-3, 202));
         ZettelBms[n].PrintAt ("-",
                         FontSmallBlack, TEC_FONT_CENTERED,
                         XY(3, 45+10), XY(ZettelBms[n].Size.x-3, 202));
         ZettelBms[n].PrintAt (Cities [Routen[TafelData.Route[n].ZettelId].NachCity].Name,
                         FontSmallBlack, TEC_FONT_CENTERED,
                         XY(3, 56+6), XY(ZettelBms[n].Size.x-3, 202));

         //Bisheriger Höchstbieter & Gebot:
         if (TafelData.Route[n].Player!=-1)
         {
            ZettelBms[n].PrintAt (Sim.Players.Players[(SLONG)TafelData.Route[n].Player].Airline,
                            FontSmallBlack, TEC_FONT_CENTERED,
                            XY(3, 72+30), XY(ZettelBms[n].Size.x-3, 202));
         }
         ZettelBms[n].PrintAt (Einheiten[EINH_DM].bString (TafelData.Route[n].Preis),
                         FontSmallBlack, TEC_FONT_CENTERED,
                         XY(3, 95+20), XY(ZettelBms[n].Size.x-3, 202));
      }
   }
   else if (n<14) //City
   {
      if (TafelData.City[n-7].ZettelId==-1)
      {
         ZettelBms[n].Destroy();
      }
      else
      {
         ZettelBms[n].ReSize(LeereZettelBms[n%3].Size);
         ZettelBms[n].BlitFrom(LeereZettelBms[n%3]);

         ZettelBms[n].PrintAt (StandardTexte.GetS (TOKEN_MISC, 2501),
                         FontSmallBlack, TEC_FONT_CENTERED,
                         XY(3, 28+8), XY(ZettelBms[n].Size.x-3, 132));
         ZettelBms[n].PrintAt (Cities [TafelData.City[n-7].ZettelId].Name,
                         FontSmallBlack, TEC_FONT_CENTERED,
                         XY(3, 28+30), XY(ZettelBms[n].Size.x-3, 132));

         //Bisheriger Höchstbieter & Gebot:
         if (TafelData.City[n-7].Player!=-1)
         {
            ZettelBms[n].PrintAt (Sim.Players.Players[(SLONG)TafelData.City[n-7].Player].Airline,
                            FontSmallBlack, TEC_FONT_LEFT,
                            XY(13, 55+30), XY(ZettelBms[n].Size.x-3, 132));
         }
         p+=ZettelBms[n].PrintAt (Einheiten[EINH_DM].bString (TafelData.City[n-7].Preis),
                         FontSmallBlack, TEC_FONT_LEFT,
                         XY(13, 70+30), XY(ZettelBms[n].Size.x-3, 132));
      }
   }
   else if (n>=14) //Gate
   {
      if (TafelData.Gate[n-14].ZettelId==-1)
      {
         ZettelBms[n].Destroy();
      }
      else
      {
         ZettelBms[n].ReSize(LeereZettelBms[n%3].Size);
         ZettelBms[n].BlitFrom(LeereZettelBms[n%3]);

         ZettelBms[n].PrintAt (bprintf (StandardTexte.GetS (TOKEN_MISC, 2500), TafelData.Gate[n-14].ZettelId+1),
                         FontSmallBlack, TEC_FONT_CENTERED,
                         XY(3, 18+15), XY(ZettelBms[n].Size.x-3, 102));

         //Bisheriger Höchstbieter & Gebot:
         if (TafelData.Gate[n-14].Player!=-1)
         {
            ZettelBms[n].PrintAt (Sim.Players.Players[(SLONG)TafelData.Gate[n-14].Player].Airline,
                            FontSmallBlack, TEC_FONT_LEFT,
                            XY(13, 45+30), XY(ZettelBms[n].Size.x-3, 202));
         }
         ZettelBms[n].PrintAt (Einheiten[EINH_DM].bString (TafelData.Gate[n-14].Preis),
                         FontSmallBlack, TEC_FONT_LEFT,
                         XY(13, 60+30), XY(ZettelBms[n].Size.x-3, 202));
      }
   }
}

//--------------------------------------------------------------------------------------------
//void CTafel::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CTafel::OnLButtonDown(UINT nFlags, CPoint point)
{
   SLONG   c;
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
      if (MouseClickArea==ROOM_TAFEL && MouseClickId==999)
         qPlayer.LeaveRoom();
      else if (MouseClickArea==ROOM_TAFEL && MouseClickId==800 && Sim.ItemPostcard)
      {
         if (qPlayer.HasSpaceForItem ())
         {
            qPlayer.BuyItem (ITEM_POSTKARTE);

            if (qPlayer.HasItem (ITEM_POSTKARTE))
            {
               Sim.ItemPostcard=false;
               Sim.SendSimpleMessage (ATNET_TAKETHING, NULL, ITEM_POSTKARTE);
            }
         }
      }
      else
      {
         //Auf einen der Zettel geklickt?
         for (c=0; c<21; c++)
         {
            if (point.y<440 && ((c<7 && TafelData.Route[c].ZettelId) || (c>=7 && c<14 && TafelData.City[c-7].ZettelId!=-1) || (c>=14 && TafelData.Gate[c-14].ZettelId!=-1)))
            {
               if (XY(point).IfIsWithin(ZettelPos[c*2], ZettelPos[c*2+1], ZettelPos[c*2]+LeereZettelBms[c%3].Size.x, ZettelPos[c*2+1]+LeereZettelBms[c%3].Size.y))
               {
                  if (c>=7 && c<14 && TafelData.City[c-7].Player!=LastTafelData.City[c-7].Player)
                  {
                     //Undo City
                     TafelData.City[c-7]=LastTafelData.City[c-7];
                  }
                  else if (c>=14 && TafelData.Gate[c-14].Player!=LastTafelData.Gate[c-14].Player)
                  {
                     //Undo Gate
                     TafelData.Gate[c-14]=LastTafelData.Gate[c-14];
                  }
                  if (c>=7 && c<14 && TafelData.City[c-7].Player!=PlayerNum && Sim.Players.Players[Sim.localPlayer].RentCities.RentCities[TafelData.City[c-7].ZettelId].Rang==0)
                  {
                     if (Sim.bNetwork && TafelData.City[c-7].Player!=-1 && Sim.Players.Players[TafelData.City[c-7].Player].Owner==2)
                        Sim.SendSimpleMessage (ATNET_ADVISOR, Sim.Players.Players[TafelData.City[c-7].Player].NetworkID, 0, PlayerNum, c);

                     TafelData.City[c-7].Preis += TafelData.City[c-7].Preis/10;
                     TafelData.City[c-7].Player = PlayerNum;
                     TafelData.City[c-7].WasInterested = TRUE;
                  }
                  else if (c>=14 && TafelData.Gate[c-14].Player!=PlayerNum)
                  {
                     if (Sim.bNetwork && TafelData.City[c-7].Player!=-1 && Sim.Players.Players[TafelData.City[c-7].Player].Owner==2)
                        Sim.SendSimpleMessage (ATNET_ADVISOR, Sim.Players.Players[TafelData.City[c-7].Player].NetworkID, 0, PlayerNum, c);

                     TafelData.Gate[c-14].Preis += TafelData.Gate[c-14].Preis/10;
                     TafelData.Gate[c-14].Player = PlayerNum;
                     TafelData.Gate[c-14].WasInterested = TRUE;
                  }
                  RepaintZettel (c);
                  return;
               }
            }
         }
         CStdRaum::OnLButtonDown(nFlags, point);
      }
   }
}

//--------------------------------------------------------------------------------------------
//void CTafel::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CTafel::OnRButtonDown(UINT nFlags, CPoint point)
{
   DefaultOnRButtonDown ();

   //Außerhalb geklickt? Dann Default-Handler!
   if (point.x<WinP1.x || point.y<WinP1.y || point.x>WinP2.x || point.y>WinP2.y)
   {
      return;
   }

   if (MenuIsOpen())
   {
      MenuRightClick (point);
   }
   else
   {
      if (!IsDialogOpen() && point.y<440)
      {
         Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
      }
      CStdRaum::OnRButtonDown(nFlags, point);
   }
}

//--------------------------------------------------------------------------------------------
//CTafelData::
//--------------------------------------------------------------------------------------------
//Daten alle löschen ==> keine Zettel
//--------------------------------------------------------------------------------------------
void CTafelData::Clear (void)
{
   SLONG c;

   for (c=0; c<7; c++)
   {
      Route[c].ZettelId=0;
      City[c].ZettelId=-1;
      Gate[c].ZettelId=-1;
      Route[c].WasInterested = FALSE;
      City[c].WasInterested  = FALSE;
      Gate[c].WasInterested  = FALSE;
   }
}

//--------------------------------------------------------------------------------------------
//Zettel für n Tage auslosen:
//--------------------------------------------------------------------------------------------
void CTafelData::Randomize (SLONG Day)
{
   SLONG c, d, e, f, Anz, Id=0;
   SLONG ObjId;
   SLONG PlayerIndex, PlayerUsed;

   ULONG CityIds[7];

   SLONG NumCities=0;
   SLONG NumRoutes=0;
   SLONG NumGates=0;

   for (c=0; c<7; c++)
   {
      Route[c].WasInterested = FALSE;
      City[c].WasInterested  = FALSE;
      Gate[c].WasInterested  = FALSE;
      Route[c].Player=-1;
      City[c].Player=-1;
      Gate[c].Player=-1;
      Gate[c].ZettelId=-1;
   }

   if (Sim.Difficulty==DIFF_NORMAL)
   {
      CityIds[0]=Cities(Sim.HomeAirportId);
      CityIds[1]=Cities(Sim.MissionCities[0]);
      CityIds[2]=Cities(Sim.MissionCities[1]);
      CityIds[3]=Cities(Sim.MissionCities[2]);
      CityIds[4]=Cities(Sim.MissionCities[3]);
      CityIds[5]=Cities(Sim.MissionCities[4]);
      CityIds[6]=Cities(Sim.MissionCities[5]);
   }

   TEAKRAND localRand (Sim.Date+Sim.StartTime);

   if (RobotUse(ROBOT_USE_TRAVELHOLDING))
   {
      //Dann die Orte:
      for (c=0; c<min(Day,10); c++)
      {
         if ((localRand.Rand(100))<30 && NumCities<3)
         {
            //Stadt raussuchen, fünf Anläufe:
            for (e=0; e<5; e++)
            {
               ObjId = Cities(Cities.GetRandomUsedIndex (&localRand));

               PlayerUsed=0;

               for (PlayerIndex=0; PlayerIndex<Sim.Players.AnzPlayers; PlayerIndex++)
                  if (Sim.Players.Players[PlayerIndex].RentCities.RentCities[ObjId].Rang)
                     PlayerUsed++;

               //Route ablehnen, wenn sie schon 3x vermietet ist:
               if (PlayerUsed>=3) continue;

               //Und Routen auch nicht doppelt versteigern:
               for (f=0; f<7; f++)
                  if (City[f].ZettelId==ObjId) break;
               if (f<7) continue;

               break;
            }

            //Ggf. Daten auf den Zettel kopieren:
            if (e<5)
            {
               City[NumCities].ZettelId = ObjId;
               City[NumCities].Player   = -1;
               City[NumCities].Preis    = ::Cities[ObjId].BuroRent;
               City[NumCities].Rang     = PlayerUsed+1;

               NumCities++;
            }
         }

         //Für die Routenmission:
         if (Sim.Difficulty==DIFF_NORMAL && NumCities<4)
         {
            //Stadt raussuchen, zehn Anläufe:
            for (e=0; e<10; e++)
            {
               ObjId = Cities(Cities.GetRandomUsedIndex (&localRand));

               PlayerUsed=0;

               for (PlayerIndex=0; PlayerIndex<Sim.Players.AnzPlayers; PlayerIndex++)
                  if (Sim.Players.Players[PlayerIndex].RentCities.RentCities[ObjId].Rang)
                     PlayerUsed++;

               //Stadt ablehnen, wenn sie schon 3x vermietet ist:
               if (PlayerUsed>=3) continue;

               //Und Stadt auch nicht doppelt versteigern:
               for (f=0; f<7; f++)
                  if (City[f].ZettelId==ObjId) break;
               if (f<7) continue;

               for (f=1; f<7; f++)
                  if (Cities(CityIds[f])==(ULONG)ObjId) break;
               if (f<7) continue;

               break;
            }

            //Ggf. Daten auf den Zettel kopieren:
            if (e<5)
            {
               City[NumCities].ZettelId = ObjId;
               City[NumCities].Player   = -1;
               City[NumCities].Preis    = ::Cities[ObjId].BuroRent;
               City[NumCities].Rang     = PlayerUsed+1;

               NumCities++;
            }
         }
      }
   }

   //Und zu letzt die Gates:
   Anz = Airport.GetNumberOfShops (RUNE_2WAIT);
   if (Sim.Date==0) Anz=4;

   for (c=f=0; c<Anz && f<7; c++)
   {
      BOOL Found=FALSE;

      for (d=0; d<Sim.Players.Players.AnzEntries(); d++)
         if (!Sim.Players.Players[d].IsOut)
            for (e=0; e<Sim.Players.Players[d].Gates.Gates.AnzEntries(); e++)
               if (Sim.Players.Players[d].Gates.Gates[e].Miete!=-1 &&
                   Sim.Players.Players[d].Gates.Gates[e].Nummer==c)
                  Found=TRUE;

      if (Found==FALSE)
      {
         Gate[NumGates].ZettelId = c;
         Gate[NumGates].Player   = -1;
         Gate[NumGates].Preis    = 3000-c*100;
         Gate[NumGates].Rang     = 0;
         f++;

         NumGates++;
      }
   }
}

//--------------------------------------------------------------------------------------------
//Speichert ein TafelZettel-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CTafelZettel &TafelZettel)
{
   File << TafelZettel.ZettelId << TafelZettel.Player;
   File << TafelZettel.Preis    << TafelZettel.Rang;
   File << TafelZettel.WasInterested;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt ein TafelZettel-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CTafelZettel &TafelZettel)
{
   File >> TafelZettel.ZettelId >> TafelZettel.Player;
   File >> TafelZettel.Preis    >> TafelZettel.Rang;
   File >> TafelZettel.WasInterested;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Speichert ein TafelData-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CTafelData &TafelData)
{
   SLONG c;

   for (c=0; c<7; c++)
      File << TafelData.Route[c] << TafelData.City[c] << TafelData.Gate[c];

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt ein TafelData-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CTafelData &TafelData)
{
   SLONG c;

   for (c=0; c<7; c++)
   {
      File >> TafelData.Route[c] >> TafelData.City[c] >> TafelData.Gate[c];
      if (SaveVersion==1 && SaveVersionSub<102)
      {
         if (TafelData.City[c].ZettelId==0)
            TafelData.City[c].ZettelId=-1; //Amsterdam-Bugfix
      }
   }

   return (File);
}