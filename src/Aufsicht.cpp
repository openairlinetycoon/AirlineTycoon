//============================================================================================
// Aufsicht.cpp : Das Büro der Flugaufsicht
//============================================================================================
#include "stdafx.h"
#include "glauf.h"
#include "Aufsicht.h"
#include "AtNet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SB_CColorFX ColorFX;
extern SLONG ZettelPos [14*3];

//Zum debuggen:
static const char FileId[] = "Aufs";

//////////////////////////////////////////////////////////////////////////////////////////////
// Flugaufsicht Konstruktion, Initialisation, Destruction
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//Konstruktor
//--------------------------------------------------------------------------------------------
CAufsicht::CAufsicht (BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "aufsicht.gli", GFX_AUFSICHT)
{
   SLONG c, d, e;

   Sim.ShowExtrablatt = -1;
   Sim.FocusPerson    = -1;
   Sim.b18Uhr         = FALSE;

   bIsMorning = (Sim.Time==9*60000);
   bExitASAP  = false;
   bExited    = false;

   if (bIsMorning)
   {
      PLAYER &qLocalPlayer = Sim.Players.Players[Sim.localPlayer];

      NetGenericSync(0x4211014);
      qLocalPlayer.NetSynchronizeMeeting();
      NetGenericSync(0x4211015);
   }

   KonstruktorFinished = 0;
   LeereZettelBms.ReSize (pRoomLib, "ZETTELK04", 3);
   PostcardBm.ReSize (pRoomLib, "NOCARD");

   //Morgends verhindern, daß jemand rausgeht, noch bevor alle da sind:
   bOkayToAct = TRUE;
   if (bIsMorning) bOkayToAct = FALSE;
   if (!Sim.bNetwork) bOkayToAct=true;
   if (!bOkayToAct) SetNetworkBitmap (3, 2); //Waitung for Players

   Sim.Players.Players[Sim.localPlayer].bReadyForBriefing=true;
   Sim.SendSimpleMessage (ATNET_READYFORBRIEFING, NULL, Sim.localPlayer);

   //DaysWithoutSabotage aktualisieren:
   for (c=0; c<Sim.SabotageActs.AnzEntries(); c++)
   {
      Sim.Players.Players[Sim.SabotageActs[c].Opfer].DaysWithoutSabotage = 0;
   }

   //Prüfen wer, welche Routen hat (wg. Spielziel):
   if (Sim.GetHour()==9 && Sim.GetMinute()==0 && Sim.Difficulty==DIFF_NORMAL)
   {
      ULONG CityIds[7];

      CityIds[0]=Cities(Sim.HomeAirportId);
      CityIds[1]=Cities(Sim.MissionCities[0]);
      CityIds[2]=Cities(Sim.MissionCities[1]);
      CityIds[3]=Cities(Sim.MissionCities[2]);
      CityIds[4]=Cities(Sim.MissionCities[3]);
      CityIds[5]=Cities(Sim.MissionCities[4]);
      CityIds[6]=Cities(Sim.MissionCities[5]);

      for (c=0; c<Sim.Players.AnzPlayers; c++)
      {
         PLAYER &qPlayer = Sim.Players.Players[c];

         if (qPlayer.IsOut==0)
         {
            qPlayer.ConnectFlags=0;
            for (d=qPlayer.Planes.AnzEntries()-1; d>=0; d--)
               if (qPlayer.Planes.IsInAlbum(d))
               {
                  CFlugplan &qPlan = qPlayer.Planes[d].Flugplan;

                  for (e=qPlan.Flug.AnzEntries()-1; e>=0; e--)
                  {
                     if (qPlan.Flug[e].ObjectType==1)
                     if (qPlayer.RentRouten.RentRouten[(SLONG)Routen(qPlan.Flug[e].ObjectId)].Rang)
                     if (qPlayer.RentRouten.RentRouten[(SLONG)Routen(qPlan.Flug[e].ObjectId)].Auslastung>20)
                     {
                        ULONG a=Routen[qPlan.Flug[e].ObjectId].VonCity;
                        ULONG b=Routen[qPlan.Flug[e].ObjectId].NachCity;

                        if (a>0x1000000) a=Cities(a);
                        if (b>0x1000000) b=Cities(b);

                        if (CityIds[0]==a && CityIds[1]==b) qPlayer.ConnectFlags|=0x0001;
                        if (CityIds[0]==a && CityIds[2]==b) qPlayer.ConnectFlags|=0x0002;
                        if (CityIds[0]==a && CityIds[3]==b) qPlayer.ConnectFlags|=0x0004;
                        if (CityIds[0]==a && CityIds[4]==b) qPlayer.ConnectFlags|=0x0008;
                        if (CityIds[0]==a && CityIds[5]==b) qPlayer.ConnectFlags|=0x0010;
                        if (CityIds[0]==a && CityIds[6]==b) qPlayer.ConnectFlags|=0x0020;

                        if (CityIds[0]==b && CityIds[1]==a) qPlayer.ConnectFlags|=0x0100;
                        if (CityIds[0]==b && CityIds[2]==a) qPlayer.ConnectFlags|=0x0200;
                        if (CityIds[0]==b && CityIds[3]==a) qPlayer.ConnectFlags|=0x0400;
                        if (CityIds[0]==b && CityIds[4]==a) qPlayer.ConnectFlags|=0x0800;
                        if (CityIds[0]==b && CityIds[5]==a) qPlayer.ConnectFlags|=0x1000;
                        if (CityIds[0]==b && CityIds[6]==a) qPlayer.ConnectFlags|=0x2000;
                     }
                  }
               }

            for (d=e=0; d<32; d++)
               if (qPlayer.ConnectFlags & (1<<d))
                  e++;

            qPlayer.ConnectFlags=e;
         }
      }
   }

   SetRoomVisited (PlayerNum, ROOM_AUFSICHT);

   HandyOffset = 320;

   ExitFromMiddle = -1;
   ExitFromLeft   = -1;
   ExitFromRight  = -1;

   Talkers.Talkers[TALKER_BOSS].IncreaseReference ();
   DefaultDialogPartner=TALKER_BOSS;

   for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
   {
      IsOut[c]    = Sim.Players.Players[c].IsOut;
      HaloKomm[c] = -1;
   }

   AirportRoomPos   = Airport.GetRandomTypedRune (RUNE_2SHOP, ROOM_AUFSICHT);
   AirportRoomPos.y = AirportRoomPos.y-5000+93;
   AirportRoomPos += XY (-22,-33);

   OpaqueBm.ReSize (pRoomLib, GFX_OPAQUE);
   TransBm.ReSize (pRoomLib, GFX_TRANS);

   if (!bHandy) AmbientManager.SetGlobalVolume (30);

   TimeClick = timeGetTime();

   SP_Boss.ReSize (17);

   //Raumanimationen
   MonitorAnim.ReSize (pRoomLib, "MONI01",  3, NULL, FALSE, ANIMATION_MODE_RANDOM, 0, 3, 350);

   //Mitte:
   SP_Boss.Clips[0].ReSize (0, "bb_wait.smk", "", XY (344, 105), SPM_IDLE,  CRepeat(1,1), CPostWait(5,5), SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, &ExitFromMiddle,
                               "A9E1E1", 0, 1, 11);
   SP_Boss.Clips[1].ReSize (1, "bb_base.smk", "", XY (344, 105), SPM_TALKING,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, &ExitFromMiddle,
                               "A9A1E1E1", 1,  3, 0, 11);
   SP_Boss.Clips[3].ReSize (3, "bb_hand.smk", "", XY (344, 105), SPM_TALKING,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A1", 1);

   //Für Sabotage:
   SP_Boss.Clips[2].ReSize (2, "bb_fing.smk", "", XY (344, 105), SPM_TALKING,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                               &ExitFromMiddle, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, &ExitFromMiddle,
                               "A1", 1);

   //Left:
   SP_Boss.Clips[4].ReSize (4, "bb_left.smk", "", XY (344, 105), SPM_TALKING,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                               &ExitFromMiddle, SMACKER_CLIP_SET|SMACKER_CLIP_POST, -1, NULL,
                               "A1", 5);
   SP_Boss.Clips[5].ReSize (5, "bb_leftR.smk", "", XY (344, 105), SPM_TALKING,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, &ExitFromLeft,
                               "A1E1", 5, 6);
   SP_Boss.Clips[6].ReSize (6, "bb_leftZ.smk", "", XY (344, 105), SPM_TALKING,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A1", 0);

   //Right:
   SP_Boss.Clips[7].ReSize (7, "bb_righ.smk", "", XY (344, 105), SPM_TALKING,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                               &ExitFromMiddle, SMACKER_CLIP_SET|SMACKER_CLIP_POST, -1, NULL,
                               "A1", 8);
   SP_Boss.Clips[8].ReSize (8, "bb_righR.smk", "", XY (344, 105), SPM_TALKING,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_CANCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, &ExitFromRight,
                               "A1E1", 8, 9);
   SP_Boss.Clips[9].ReSize (9, "bb_righZ.smk", "", XY (344, 105), SPM_TALKING,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,
                               "A1", 1);

   //Left-Right:
   SP_Boss.Clips[10].ReSize (10, "bb_turnR.smk", "", XY (344, 105), SPM_TALKING,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                                 NULL, SMACKER_CLIP_SET, 0, NULL,
                                 "A1", 8);

   //Left-Right-Left (ermahnung wegen Sabotage):
   SP_Boss.Clips[12].ReSize (12, "bb_left.smk", "", XY (344, 105),  SPM_IDLE,    CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                                 &ExitFromMiddle, SMACKER_CLIP_SET|SMACKER_CLIP_POST, -1, NULL,
                                 "A1", 13);
   SP_Boss.Clips[13].ReSize (13, "bb_turnR.smk", "", XY (344, 105), SPM_IDLE,    CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                                 NULL, SMACKER_CLIP_SET, 0, NULL,
                                 "A1", 14);
   SP_Boss.Clips[14].ReSize (14, "bb_turnL.smk", "", XY (344, 105), SPM_IDLE,    CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                                 NULL, SMACKER_CLIP_SET, 0, NULL,
                                 "A1", 15);
   SP_Boss.Clips[15].ReSize (15, "bb_turnR.smk", "", XY (344, 105), SPM_IDLE,    CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                                 NULL, SMACKER_CLIP_SET, 0, NULL,
                                 "A1", 16);
   SP_Boss.Clips[16].ReSize (16, "bb_righZ.smk", "", XY (344, 105), SPM_IDLE,    CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                                 NULL, SMACKER_CLIP_SET, 0, NULL,
                                 "A1", 0);

   //Listening
   SP_Boss.Clips[11].ReSize (0, "bb_wait.smk", "", XY (344, 105), SPM_LISTENING,  CRepeat(1,1), CPostWait(5,5), SMACKER_CLIP_CANCANCEL,
                                NULL, 0, -1, NULL,
                                "A9E1E1", 11, 1, 0);

   if (Sim.GetHour()==9 && Sim.GetMinute()==0)
   {
      PlayerStuff.ReSize (pRoomLib, "OBJ_PB OBJ_PV OBJ_PR OBJ_PJ");
      FrauFuss.ReSize (pRoomLib, "FRAUFUSS");

      if (Sim.bNetwork && Sim.Date==0)
         for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
            Sim.Players.Players[c].bReadyForMorning=false;

      if (!IsOut[0]) //Blau:
      {
         SP_Player[0].ReSize(9);
         SP_Player[0].Clips[0].ReSize (0, "Pb_wait.smk", "", XY (70, 135), SPM_IDLE,  CRepeat(1,1), CPostWait(5,5), SMACKER_CLIP_CANCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A6A1A1E1", 0, 1, 4, 1);
         SP_Player[0].Clips[8].ReSize (8, "Pb_guckW.smk", "", XY (70, 135), SPM_HOLY,  CRepeat(1,1), CPostWait(9999,9999), SMACKER_CLIP_CANCANCEL,
                                       &(HaloKomm[0]), SMACKER_CLIP_SET|SMACKER_CLIP_PRE, 1, NULL,
                                       "A1E1", 8, 3);

         SP_Player[0].Clips[1].ReSize (1, "Pb_guck.smk", "", XY (70, 135), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 2);
         SP_Player[0].Clips[2].ReSize (2, "Pb_guckW.smk", "", XY (70, 135), SPM_IDLE,  CRepeat(1,1), CPostWait(30,60), SMACKER_CLIP_CANCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A7A1E1", 3, 7, 8);
         SP_Player[0].Clips[3].ReSize (3, "Pb_guckZ.smk", "", XY (70, 135), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 0);

         SP_Player[0].Clips[4].ReSize (4, "Pb_fing.smk", "", XY (70, 135), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 5);
         SP_Player[0].Clips[5].ReSize (5, "Pb_fingW.smk", "", XY (70, 135), SPM_IDLE,  CRepeat(1,1), CPostWait(20,80), SMACKER_CLIP_DONTCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 6);
         SP_Player[0].Clips[6].ReSize (6, "Pb_fingZ.smk", "", XY (70, 135), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 0);
         SP_Player[0].Clips[7].ReSize (7, "Pb_GuckK.smk", "", XY (70, 135), SPM_IDLE,  CRepeat(1,1), CPostWait(1,1), SMACKER_CLIP_DONTCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 0, 2);

         SP_Halo[0].ReSize(2);
         SP_Halo[0].Clips[0].ReSize (0, "BombeW.smk", "", XY (94,119), SPM_IDLE,  CRepeat(1,1), CPostWait(50,50), SMACKER_CLIP_CANCANCEL,
                                     NULL, SMACKER_CLIP_SET, 0, &(HaloKomm[0]),
                                     "A1", 0);
         SP_Halo[0].Clips[1].ReSize (1, "P1Halo.smk", "", XY (94,119), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_CANCANCEL,
                                     &(HaloKomm[0]), SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, NULL,
                                     "A1", 0);
     }

      if (!IsOut[1]) //Grün:
      {
         SP_Player[1].ReSize(7);
         SP_Player[1].Clips[0].ReSize (0, "Pv_wait.smk", "", XY (165, 110), SPM_IDLE,  CRepeat(1,1), CPostWait(15,45), SMACKER_CLIP_CANCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A8A1A2A2E1", 0, 5, 1, 2, 6);
         SP_Player[1].Clips[6].ReSize (6, "Pv_wait.smk", "", XY (165, 110), SPM_HOLY,  CRepeat(1,1), CPostWait(9999,9999), SMACKER_CLIP_CANCANCEL,
                                       &(HaloKomm[1]), SMACKER_CLIP_SET|SMACKER_CLIP_PRE, 1, NULL,
                                       "A1E1", 6, 0);

         SP_Player[1].Clips[1].ReSize (1, "Pv_hand.smk", "", XY (165, 110), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 0);
         SP_Player[1].Clips[2].ReSize (2, "Pv_handB.smk", "", XY (165, 110), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 3);
         SP_Player[1].Clips[3].ReSize (3, "Pv_handW.smk", "", XY (165, 110), SPM_IDLE,  CRepeat(1,1), CPostWait(40,90), SMACKER_CLIP_DONTCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 4);
         SP_Player[1].Clips[4].ReSize (4, "Pv_handZ.smk", "", XY (165, 110), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 0);
         SP_Player[1].Clips[5].ReSize (5, "Pv_WaitK.smk", "", XY (165, 110), SPM_IDLE,  CRepeat(1,1), CPostWait(1,1), SMACKER_CLIP_DONTCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 0);

         SP_Halo[1].ReSize(2);
         SP_Halo[1].Clips[0].ReSize (0, "BombeW.smk", "", XY (165,93), SPM_IDLE,  CRepeat(1,1), CPostWait(50,50), SMACKER_CLIP_CANCANCEL,
                                     NULL, SMACKER_CLIP_SET, 0, &(HaloKomm[1]),
                                     "A1", 0);
         SP_Halo[1].Clips[1].ReSize (1, "P2Halo.smk", "", XY (165,93), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_CANCANCEL,
                                     &(HaloKomm[1]), SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, NULL,
                                     "A1", 0);
     }

      if (!IsOut[2]) //Rot:
      {
         SP_Player[2].ReSize(6);
         SP_Player[2].Clips[0].ReSize (0, "Pr_wait.smk", "", XY (422, 142), SPM_IDLE,  CRepeat(1,1), CPostWait(5,5), SMACKER_CLIP_CANCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A2A1A1E1", 0, 1, 4, 5);
         SP_Player[2].Clips[5].ReSize (5, "Pr_wait.smk", "", XY (422, 142), SPM_HOLY,  CRepeat(1,1), CPostWait(9999,9999), SMACKER_CLIP_CANCANCEL,
                                       &(HaloKomm[2]), SMACKER_CLIP_SET|SMACKER_CLIP_PRE, 1, NULL,
                                       "A1E1", 5, 0);

         SP_Player[2].Clips[1].ReSize (1, "Pr_auf.smk", "", XY (422, 142), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 2);
         SP_Player[2].Clips[2].ReSize (2, "Pr_aufw.smk", "", XY (422, 142), SPM_IDLE,  CRepeat(1,1), CPostWait(40,120), SMACKER_CLIP_CANCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 3);
         SP_Player[2].Clips[3].ReSize (3, "Pr_aufz.smk", "", XY (422, 142), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 0);
         SP_Player[2].Clips[4].ReSize (4, "Pr_uhr.smk", "", XY (422, 142), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 0);

         SP_Halo[2].ReSize(2);
         SP_Halo[2].Clips[0].ReSize (0, "BombeW.smk", "", XY (528, 130), SPM_IDLE,  CRepeat(1,1), CPostWait(50,50), SMACKER_CLIP_CANCANCEL,
                                     NULL, SMACKER_CLIP_SET, 0, &(HaloKomm[2]),
                                     "A1", 0);
         SP_Halo[2].Clips[1].ReSize (1, "P3Halo.smk", "", XY (528, 130), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_CANCANCEL,
                                     &(HaloKomm[2]), SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, NULL,
                                     "A1", 0);
     }

      if (!IsOut[3]) //Gelb:
      {
         SP_Player[3].ReSize(6);
         SP_Player[3].Clips[0].ReSize (0, "Pj_wait.smk", "", XY (446, 186), SPM_IDLE,  CRepeat(2,2), CPostWait(15,20), SMACKER_CLIP_CANCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A9A1A1A1A1E1", 0, 4, 1, 2, 3, 5);
         SP_Player[3].Clips[5].ReSize (5, "Pj_wait.smk", "", XY (446, 186), SPM_HOLY,  CRepeat(2,2), CPostWait(9999,9999), SMACKER_CLIP_CANCANCEL,
                                       &(HaloKomm[3]), SMACKER_CLIP_SET|SMACKER_CLIP_PRE, 1, NULL,
                                       "A1E1", 5, 0);

         SP_Player[3].Clips[1].ReSize (1, "Pj_arm.smk", "", XY (446, 186), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 0);
         SP_Player[3].Clips[2].ReSize (0, "Pj_fing.smk", "", XY (446, 186), SPM_IDLE,  CRepeat(1,1), CPostWait(5,5), SMACKER_CLIP_CANCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 0);
         SP_Player[3].Clips[3].ReSize (1, "Pj_schu.smk", "", XY (446, 186), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_DONTCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 0);
         SP_Player[3].Clips[4].ReSize (1, "Pj_k.smk", "", XY (446, 186), SPM_IDLE,  CRepeat(1,1), CPostWait(1,1), SMACKER_CLIP_DONTCANCEL,
                                       NULL, SMACKER_CLIP_SET, 0, NULL,
                                       "A1", 0);

         SP_Halo[3].ReSize(2);
         SP_Halo[3].Clips[0].ReSize (0, "BombeW.smk", "", XY (583, 164), SPM_IDLE,  CRepeat(1,1), CPostWait(50,50), SMACKER_CLIP_CANCANCEL,
                                     NULL, SMACKER_CLIP_SET, 0, &(HaloKomm[3]),
                                     "A1", 0);
         SP_Halo[3].Clips[1].ReSize (1, "P4Halo.smk", "", XY (583, 164), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0), SMACKER_CLIP_CANCANCEL,
                                     &(HaloKomm[3]), SMACKER_CLIP_SET|SMACKER_CLIP_PRE, -1, NULL,
                                     "A1", 0);
     }
   }

   //Das Briefing machen wir später & erlösen die Figuren sofort:
   if (Sim.DayState==1 && !Sim.IsTutorial)
   {
      #ifdef DEMO
         if (Sim.Date>=100)
            StartDialog (TALKER_BOSS, MEDIUM_AIR, 30);
         else 
      #endif

      //Uhrig's Aufträge:
      if (Sim.Difficulty==DIFF_ADDON09)
      {
         for (SLONG c=0; c<4; c++)
         {
            PLAYER &qPlayer = Sim.Players.Players[c];

            if (!qPlayer.IsOut)
            {
               if (Sim.Date==0)
                  qPlayer.NumOrderFlightsToday = 2+(qPlayer.PlayerNum&1);
               else
                  qPlayer.NumOrderFlightsToday = 0;

               qPlayer.NumOrderFlightsToday2 = qPlayer.NumOrderFlightsToday;
               qPlayer.Statistiken[STAT_AUFTRAEGE].AddAtPastDay (0, 5);

               if (qPlayer.Owner!=1)
                  qPlayer.Add5UhrigFlights();
            }
         }
      }

      if (Sim.Date==0 || Sim.Options.OptionBriefBriefing==0)
      {
         if (Sim.ProtectionState<0) StartDialog (TALKER_BOSS, MEDIUM_AIR, 30);
         else                       StartDialog (TALKER_BOSS, MEDIUM_AIR, 1);
         DontDisplayPlayer=Sim.localPlayer;
      }
      else MenuStart (MENU_BRIEFING);
   }

   if (Sim.GetHour()==9 && Sim.GetMinute()==0 && MouseWait>0)
      MouseWait--;

   KonstruktorFinished = 1;
   ShowWindow(SW_SHOW);
   UpdateWindow();
}

//--------------------------------------------------------------------------------------------
//Destruktor
//--------------------------------------------------------------------------------------------
CAufsicht::~CAufsicht()
{
   SLONG c;

   LeereZettelBms.Destroy();
   PostcardBm.Destroy();

   if (Sim.bNetwork && bIsMorning)
   {
      PLAYER &qLocalPlayer = Sim.Players.Players[Sim.localPlayer];

      qLocalPlayer.NetSynchronizeMoney();
      qLocalPlayer.NetSynchronizeImage();
      qLocalPlayer.NetSynchronizeRoutes();
      qLocalPlayer.NetUpdateWorkers();
   }

   Sim.Players.Players[Sim.localPlayer].Messages.KillBerater ();

   if (CheatTestGame) Sim.Players.Players[PlayerNum].GameSpeed = 3;

   Talkers.Talkers[TALKER_BOSS].DecreaseReference ();

   if (bIsMorning)
   {
      Sim.SabotageActs.ReSize(0);

      for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
         if (!Sim.Players.Players[c].IsOut)
         {
            PERSON &qPerson = Sim.Persons[Sim.Persons.GetPlayerIndex(c)];

            if (Sim.Date!=0)
               qPerson.Position=Airport.GetRandomTypedRune (RUNE_PCREATION2, (UBYTE)((c+1)*10));
            else
               qPerson.Position=Airport.GetRandomTypedRune (RUNE_PCREATION2, (UBYTE)((c+1+4)*10));

            qPerson.MoodCountdown = 0;
            qPerson.ScreenPos.x   = qPerson.Position.x-(qPerson.Position.y-5000)/2+86;
            qPerson.ScreenPos.y   = (qPerson.Position.y-5000)+93;

            if (c==0 || c==1) qPerson.LookDir=3;
                         else qPerson.LookDir=1;

            qPerson.Dir=qPerson.LookDir*2;

            qPerson.StatePar=0;
         }

      bool bAnyBombs=false;
      for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
      if (!Sim.Players.Players[c].IsOut)
      {
         PLAYER &qPlayer = Sim.Players.Players[c];

         if (c!=Sim.localPlayer && (Sim.bNetwork==0 || Sim.bIsHost)) qPlayer.LeaveRoom();

         qPlayer.NumFlights=0;
         qPlayer.WaitWorkTill=-1;
         qPlayer.WorkCountdown=1;
         qPlayer.WaitWorkTill = 0;

         if (qPlayer.Owner==1)
            qPlayer.WalkToRoom (UBYTE(ROOM_BURO_A+c*10));

         bool bFremdsabotage=false;
         if (Sim.Players.Players[c].ArabMode2<0)
         {
            Sim.Players.Players[c].ArabMode2 = -Sim.Players.Players[c].ArabMode2;
            bFremdsabotage=true;
         }

         if (qPlayer.ArabMode2)
         {
            PLAYER &qOpfer=Sim.Players.Players[qPlayer.ArabOpfer2];

            if (!bFremdsabotage) Sim.Players.Players[c].Statistiken[STAT_SABOTIERT].AddAtPastDay (0, 1);

            switch (qPlayer.ArabMode2)
            {
               case 1: //Bakterien im Kaffee
                  if (!bFremdsabotage) qPlayer.ArabHints+=8;
                  qOpfer.Sympathie[c]-=10;
                  qOpfer.SickTokay = TRUE;
                  qOpfer.NetSynchronizeFlags();
                  break;

               case 2: //Virus im Notepad
                  if (qOpfer.HasItem (ITEM_LAPTOP) && qOpfer.LaptopVirus==0)
                     qOpfer.LaptopVirus=1;
                  break;

               case 3: //Bombe im Büro
                  bAnyBombs=true;
                  if (!bFremdsabotage) qPlayer.ArabHints+=25;
                  qOpfer.Sympathie[c]-=50;
                  qOpfer.OfficeState=1;
                  qOpfer.WalkToRoom (UBYTE(ROOM_BURO_A+qOpfer.PlayerNum*10));
                  break;

               case 4: //Streik provozieren
                  if (!bFremdsabotage) qPlayer.ArabHints+=40;
                  qOpfer.StrikePlanned = TRUE;
                  break;
            }

            //Für's nächste Briefing vermerken:
            Sim.SabotageActs.ReSize (Sim.SabotageActs.AnzEntries()+1);
            Sim.SabotageActs[Sim.SabotageActs.AnzEntries()-1].Player   = bFremdsabotage?-2:c;
            Sim.SabotageActs[Sim.SabotageActs.AnzEntries()-1].ArabMode = 2075+qPlayer.ArabMode2-1;
            Sim.SabotageActs[Sim.SabotageActs.AnzEntries()-1].Opfer    = qPlayer.ArabOpfer2;

            qPlayer.ArabMode2 = 0;
         }

         bFremdsabotage=false;
         if (Sim.Players.Players[c].ArabMode3<0)
         {
            Sim.Players.Players[c].ArabMode3 = -Sim.Players.Players[c].ArabMode3;
            bFremdsabotage=true;
         }

         if (qPlayer.ArabMode3)
         {
            PLAYER &qOpfer=Sim.Players.Players[qPlayer.ArabOpfer3];

            Sim.Players.Players[c].Statistiken[STAT_SABOTIERT].AddAtPastDay (0, 1);

            switch (qPlayer.ArabMode3)
            {
               case 1: //Fremde Broschüren
                  if (!bFremdsabotage) qPlayer.ArabHints+=8;
                  qOpfer.WerbeBroschuere = qPlayer.PlayerNum;
                  qOpfer.NetSynchronizeFlags();
                  break;

               case 2: //Telefone sperren
                  if (!bFremdsabotage) qPlayer.ArabHints+=15;
                  qOpfer.TelephoneDown=1;
                  qOpfer.NetSynchronizeFlags();
                  break;

               case 3: //Presseerklärung
                  if (!bFremdsabotage) qPlayer.ArabHints+=25;
                  qOpfer.Presseerklaerung=true;
                  qOpfer.NetSynchronizeFlags();
                  Sim.Players.Players[Sim.localPlayer].Letters.AddLetter (FALSE,
                     bprintf (StandardTexte.GetS (TOKEN_LETTER, 509), qOpfer.AirlineX, qOpfer.NameX, qOpfer.AirlineX),
                     "", "", 0);
                  if (qOpfer.PlayerNum==Sim.localPlayer)
                     qOpfer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_ADVICE, 2020));

                  {
                     //Für alle Flugzeuge die er besitzt, die Passagierzahl aktualisieren:
                     for (long d=0; d<(SLONG)qOpfer.Planes.AnzEntries(); d++)
                        if (qOpfer.Planes.IsInAlbum (d))
                        {
                           CPlane &qPlane=qOpfer.Planes[d];

                           for (long e=0; e<qPlane.Flugplan.Flug.AnzEntries(); e++)
                              if (qPlane.Flugplan.Flug[e].ObjectType==1)
                                 qPlane.Flugplan.Flug[e].CalcPassengers (qOpfer.PlayerNum, qPlane);
                                 //qPlane.Flugplan.Flug[e].CalcPassengers (qPlane.TypeId, qOpfer.PlayerNum, qPlane);
                        }
                  }
                  break;

               case 4: //Bankkonto hacken
                  qOpfer.ChangeMoney (-1000000, 3502, "");
                  if (!bFremdsabotage) qPlayer.ChangeMoney (1000000, 3502, "");
                  if (!bFremdsabotage) qPlayer.ArabHints+=30;                     
                  break;

               case 5: //Flugzeug festsetzen
                  if (!bFremdsabotage) qPlayer.ArabHints+=50;
                  if (qOpfer.Planes.IsInAlbum(qPlayer.ArabPlane))
                     qOpfer.Planes[qPlayer.ArabPlane].PseudoProblem=15;
                  break;

               case 6: //Route klauen
                  qPlayer.ArabHints+=70;
                  qOpfer.RouteWegnehmen (Routen(qPlayer.ArabPlane), qPlayer.PlayerNum);
                  {
                     for (long d=0; d<Routen.Routen.AnzEntries(); d++)
                        if (Routen.IsInAlbum(d) && Routen[d].VonCity==Routen[qPlayer.ArabPlane].NachCity && Routen[d].NachCity==Routen[qPlayer.ArabPlane].VonCity)
                        {
                           qOpfer.RouteWegnehmen (d, qPlayer.PlayerNum);
                           break;
                        }
                  }
                  if (qOpfer.PlayerNum==Sim.localPlayer)
                     qOpfer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_ADVICE, 2021));
                  break;
            }

            //Für's nächste Briefing vermerken:
            Sim.SabotageActs.ReSize (Sim.SabotageActs.AnzEntries()+1);
            Sim.SabotageActs[Sim.SabotageActs.AnzEntries()-1].Player   = bFremdsabotage?-2:c;
            Sim.SabotageActs[Sim.SabotageActs.AnzEntries()-1].ArabMode = 2090+qPlayer.ArabMode3;
            Sim.SabotageActs[Sim.SabotageActs.AnzEntries()-1].Opfer    = qPlayer.ArabOpfer3;

            qPlayer.ArabMode3 = 0;
         }
      }

      if (!bAnyBombs)
         for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
            if (!Sim.Players.Players[c].IsOut)
               Sim.Players.Players[c].WalkToRoom (UBYTE(ROOM_BURO_A+c*10));
   }

   if (Sim.Players.Players[Sim.localPlayer].OfficeState!=1)
      for (c=0; c<4; c++)
         if (Sim.Players.Players[c].OfficeState==1 && Sim.Players.Players[c].IsOut==0)
         {
            Sim.Players.Players[Sim.localPlayer].WalkStop ();
            Sim.Players.Players[Sim.localPlayer].WalkStopEx ();
            Sim.FocusPerson=Sim.Persons.GetIdFromIndex (Sim.Persons.GetPlayerIndex(c));
         }

   //Flugzeuge von allen relevanten Spielern aktualisieren:
   for (c=0; c<4; c++)
      if (!Sim.Players.Players[c].IsOut)
      {
         PLAYER &qPlayer = Sim.Players.Players[c];

         //Für alle Flugzeuge die er besitzt
         for (SLONG d=0; d<(SLONG)qPlayer.Planes.AnzEntries(); d++)
         {
            if (qPlayer.Planes.IsInAlbum (d))
            {
               CPlane &qPlane=qPlayer.Planes[d];

               if (qPlane.Ort>0)
               {
                  qPlane.Position = Cities[qPlane.Ort].GlobusPosition;
                  qPlane.UpdateGlobePos (Sim.Players.Players[Sim.localPlayer].EarthAlpha);
               }
            }
         }
      }

   if (Sim.Difficulty==DIFF_ATFS10)
   {
      PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];

      if (Sim.Date==3)  qPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_ADVICE, 2400));
      if (Sim.Date==18) qPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_ADVICE, 2401));
      if (Sim.Date==20) qPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_ADVICE, 2402));
      if (Sim.Date==25) qPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_ADVICE, 2403));
      if (Sim.Date==35) qPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_ADVICE, 2404));
      if (Sim.Date==40) qPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_ADVICE, 2405));
      if (Sim.Date==45) qPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_ADVICE, 2406));
      if (Sim.Date==55) qPlayer.Messages.AddMessage (BERATERTYP_GIRL, StandardTexte.GetS (TOKEN_ADVICE, 2407));
   }
}

//--------------------------------------------------------------------------------------------
//BEGIN_MESSAGE_MAP(CAufsicht, CStdRaum)
//--------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CAufsicht, CStdRaum)
	//{{AFX_MSG_MAP(CAufsicht)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////////////////////
// CAufsicht message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//void CAufsicht::OnPaint() 
//--------------------------------------------------------------------------------------------
void CAufsicht::OnPaint() 
{
   PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];
   UBYTE   Painted = 0;

   { CPaintDC dc(this); }

   if (bExitASAP && Sim.Time>9*60000 && Sim.bWatchForReady==0)
   {
      if (!bExited)
      {
         Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
         bExited = true;
      }
   }

   if (Sim.Date>5 && Sim.GetHour()>9) Sim.GiveHint (HINT_AUFSICHT);

   if (!KonstruktorFinished) return;

   if (!Sim.bNetwork) bOkayToAct=true;

   if (!bOkayToAct)
   {
      bOkayToAct = TRUE;

      for (SLONG c=0; c<4; c++)
         if (Sim.Players.Players[c].bReadyForBriefing==false && Sim.Players.Players[c].Owner==2 && !Sim.Players.Players[c].IsOut)
            bOkayToAct = FALSE;

      if (bOkayToAct) SetNetworkBitmap (0);
   }


   //Die Standard Paint-Sachen kann der Basisraum erledigen
   if (CurrentMenu!=MENU_BRIEFING)
      CStdRaum::OnPaint ();

   if (!(Sim.ItemPostcard && qPlayer.SeligTrust==0 && Sim.Difficulty!=DIFF_TUTORIAL))
      RoomBm.BlitFromT (PostcardBm, 260, 106);

   if (CurrentMenu!=MENU_BRIEFING || gMouseLButton || gMouseRButton)
   {
do_the_painting_again:
      Painted++;
      for (SLONG c=0; c<7; c++)
      {
         //Zettel malen:
         if (TafelData.Route[c].ZettelId && qPlayer.RentRouten.RentRouten[TafelData.Route[c].ZettelId].Rang==0)
            RoomBm.BlitFromT (LeereZettelBms[c%3], (ZettelPos[c*2]-91)*74/441+274, (ZettelPos[c*2+1]-20)*68/366+55);

         if (TafelData.City[c].ZettelId && qPlayer.RentCities.RentCities[TafelData.City[c].ZettelId].Rang==0)
            RoomBm.BlitFromT (LeereZettelBms[c%3], (ZettelPos[(c+7)*2]-91)*74/441+274, (ZettelPos[(c+7)*2+1]-20)*68/366+55);

         if (TafelData.Gate[c].ZettelId!=-1)
            RoomBm.BlitFromT (LeereZettelBms[c%3], (ZettelPos[(c+14)*2]-91)*74/441+274, (ZettelPos[(c+14)*2+1]-20)*68/366+55);
      }

      //Draw Persons:
      RoomBm.pBitmap->SetClipRect(&CRect(422,0,640,174));
      for (SLONG d=0; d<SLONG(Sim.Persons.AnzEntries()); d++)
      {
         //Entscheidung! Person malen:
         if (Sim.Persons.IsInAlbum(d) && Clans.IsInAlbum (Sim.Persons[d].ClanId))
         {
            PERSON &qPerson=Sim.Persons[d];
            CLAN   &qClan=Clans[(SLONG)qPerson.ClanId];
            UBYTE   Dir=qPerson.LookDir;
            UBYTE   Phase=qPerson.Phase;

            //if (Dir==1 || Dir==3) Dir = 4-Dir;
            if (Dir<4) UBYTE(Dir = (Dir+1)&3);
            if (Dir==8 && Phase<4)  Phase = UBYTE((Phase+1)%4);
            if (Dir==8 && Phase>=4) Phase = UBYTE((Phase+1)%4+4);

            XY p=qPerson.ScreenPos-AirportRoomPos;
            XY pp;

            if (abs(p.y)<150)
            {
               pp.x=640-p.y*4-p.x*2;
               pp.y=220+p.x;

               if (pp.x>380 && pp.x<700 && abs(pp.y-220)<40)
                  qClan.BlitLargeAt (RoomBm, Dir, Phase, pp);
            }
         }
         else break;
      }
      RoomBm.pBitmap->SetClipRect(&CRect(0,0,640,440));

      //DrawTransparency:
      ColorFX.BlitTrans (TransBm.pBitmap, RoomBm.pBitmap, XY(422,0), NULL, 2);
      RoomBm.BlitFromT (OpaqueBm, 422,0);

      SP_Boss.Pump ();
      SP_Boss.BlitAtT (RoomBm);

      //Draw Smacker Persons:
      if (Sim.GetHour()==9 && Sim.GetMinute()==0)
      {
         if (!IsOut[1]) RoomBm.BlitFromT (PlayerStuff[1], 244-20, 192+9);
         if (!IsOut[0])
         {
            RoomBm.BlitFrom (FrauFuss, 138, 373);
            RoomBm.BlitFromT (PlayerStuff[0], 72, 245);
         }
      }

      //Die Raum-Animationen:
      MonitorAnim.BlitAtT (RoomBm, 329, 188);

      //Draw Smacker Persons:
      if (Sim.GetHour()==9 && Sim.GetMinute()==0)
      {
         if (!IsOut[2]) RoomBm.BlitFromT (PlayerStuff[2], 403, 233);
         if (!IsOut[3]) RoomBm.BlitFromT (PlayerStuff[3], 242, 249);

         if (!IsOut[1])
         {
            SP_Player[1].Pump ();
            SP_Player[1].BlitAtT (RoomBm);
            SP_Halo[1].Pump ();
            SP_Halo[1].BlitAtT (RoomBm);
         }

         for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
            if (!IsOut[c] && c!=1)
            {
               SP_Player[c].Pump ();
               SP_Player[c].BlitAtT (RoomBm);
               SP_Halo[c].Pump ();
               SP_Halo[c].BlitAtT (RoomBm);
            }
      }

      if (Painted==4)
      {
         CStdRaum::PumpToolTips ();
         return;
      }
   }

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   if (!IsDialogOpen() && !MenuIsOpen() && Sim.bNoTime==FALSE)
   {
      if (gMousePosition.IfIsWithin (64, 18, 172, 240)) SetMouseLook (CURSOR_EXIT, 0, ROOM_AUFSICHT, 999);
      else if (gMousePosition.IfIsWithin (269,41,362,137)) SetMouseLook (CURSOR_HOT, 0, ROOM_AUFSICHT, 10);
      else if (gMousePosition.IfIsWithin (400,100,505,241)) SetMouseLook (CURSOR_HOT, 0, ROOM_AUFSICHT, 11);
   }
   else if (Sim.GetHour()==9 && Sim.GetMinute()==0)
   {
      if ((timeGetTime()-TimeClick>40000 && Sim.Options.OptionTalking*Sim.Options.OptionDigiSound==0) || ((bTest || CheatTestGame) && timeGetTime()-TimeClick>5000))
      {
         if (bTest || CheatTestGame) OnRButtonDown(0, CPoint(160,100));
                                else CStdRaum::PreLButtonDown(XY (160, 100));

         if (bTest) TimeClick=timeGetTime()-39000;
               else TimeClick=timeGetTime()-30000;
      }
   }

   if (Sim.bPause) TimeClick = timeGetTime();

   CStdRaum::PostPaint ();

   if (CurrentMenu!=MENU_BRIEFING && Painted==0)
   {
      Painted=3;
      goto do_the_painting_again;
   }

   CStdRaum::PumpToolTips ();
}

//--------------------------------------------------------------------------------------------
//void CAufsicht::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CAufsicht::OnLButtonDown(UINT nFlags, CPoint point)
{
   XY RoomPos;

   if (!bOkayToAct) return;

   DefaultOnLButtonDown ();

   TimeClick = timeGetTime();

   if (CurrentMenu == MENU_GAMEOVER && timeGetTime()-TimeAtStart<3000) return;

   if (!ConvertMousePosition (point, &RoomPos))
   {
	   CStdRaum::OnLButtonDown(nFlags, point);
      return;
   }

   if (!PreLButtonDown (point))
   {
      if (MouseClickArea==ROOM_AUFSICHT && MouseClickId==999)
      {
         Sim.bNoTime  = FALSE;
         Sim.DayState = 2;
         //Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
         TryLeaveAufsicht ();
      }
      else if (MouseClickArea==ROOM_AUFSICHT && MouseClickId==10)
         Sim.Players.Players[(SLONG)PlayerNum].EnterRoom (ROOM_TAFEL);
      else if (MouseClickArea==ROOM_AUFSICHT && MouseClickId==11)
         StartDialog (TALKER_BOSS, MEDIUM_AIR, 2);
      else CStdRaum::OnLButtonDown(nFlags, point);
   }
}

//--------------------------------------------------------------------------------------------
//void CAufsicht::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CAufsicht::OnRButtonDown(UINT nFlags, CPoint point)
{
   BOOL  CanCancel=TRUE;
   SLONG c;

   if (!bOkayToAct) return;

   DefaultOnRButtonDown ();

   if (MenuIsOpen() && CurrentMenu==MENU_BRIEFING)
   {
      MenuStop();

      if (Sim.ProtectionState<0)
      {
         StartDialog (TALKER_BOSS, MEDIUM_AIR, 30);
         return;
      }
      else
         StartDialog (TALKER_BOSS, MEDIUM_AIR, 1);
      DontDisplayPlayer=Sim.localPlayer;
   }
   if (Sim.ProtectionState<0) return;

   if (Sim.GetHour()==9 && Sim.GetMinute()==0)
   {
      for (c=0; c<Sim.Players.AnzPlayers; c++)
         if (Sim.Players.Players[c].IsOut==0)
         {
            if (Sim.Players.Players[c].Image<-990 || Sim.Players.Players[c].Money<DEBT_GAMEOVER) CanCancel=FALSE;
            if (Sim.Players.Players[c].HasWon())
            {
               //Add-On Mission #3 Dauert immer 30 Tage:
               if ((!(Sim.Difficulty==DIFF_ADDON03 && Sim.Date<TARGET_DAYS)) && (!(Sim.Difficulty==DIFF_ADDON04 && Sim.Date<TARGET_MILESDAYS)) && (!(Sim.Difficulty==DIFF_ADDON06 && Sim.Date<TARGET_VALUEDAYS)) && (!(Sim.Difficulty==DIFF_ATFS09 && Sim.Date<BTARGET_NDAYS9)) && (!(Sim.Difficulty==DIFF_ATFS10 && Sim.Date<BTARGET_NDAYS10)))
                  CanCancel=FALSE;
            }

            #ifdef DEMO
               if (Sim.Date>=30) CanCancel=FALSE;
            #endif

         }

      for (c=0; c<Sim.SabotageActs.AnzEntries(); c++)
      {
         if (Sim.SabotageActs[c].Player!=-1)
            if (Sim.Players.Players[Sim.SabotageActs[c].Player].ArabHints>=100)
               CanCancel=FALSE;
      }

      if (Sim.Overtake) CanCancel=FALSE;
   }

   //Außerhalb geklickt? Dann Default-Handler!
   if (point.x<WinP1.x || point.y<WinP1.y || point.x>WinP2.x || point.y>WinP2.y)
   {
      CWnd::OnRButtonDown(nFlags, point);
      return;
   }
   else
   {
      if (MenuIsOpen())
      {
         if (CanCancel) MenuRightClick (point);
      }
      else
      {
         if (!IsDialogOpen() && (Sim.IsTutorial==FALSE || Sim.Tutorial>1310))
         {
            if (point.y<440)
            {
               Sim.bNoTime  = FALSE;
               Sim.DayState = 2;
               TryLeaveAufsicht ();
            }
         }

         if (CanCancel && (Sim.IsTutorial==FALSE || Sim.Tutorial>1310))
         {
            if (Sim.bNoTime)
            {
               Sim.bNoTime  = FALSE;
               Sim.DayState = 2;
               TryLeaveAufsicht ();
            }
         }

         if (!(IsDialogOpen() && Sim.IsTutorial) && CanCancel)
            CStdRaum::OnRButtonDown(nFlags, point);
      }
   }
}

//--------------------------------------------------------------------------------------------
// Raum verlassen:
//--------------------------------------------------------------------------------------------
void CAufsicht::TryLeaveAufsicht (void)
{
   if (Sim.bNetwork && bIsMorning)
   {
      bExitASAP = true;
      Sim.bWatchForReady=TRUE;
      Sim.SendSimpleMessage (ATNET_READYFORMORNING, NULL, Sim.localPlayer);
      Sim.Players.Players[Sim.localPlayer].bReadyForMorning=true;
      SetNetworkBitmap (3, 1);
      FrameWnd->Invalidate(); MessagePump();
   }
   else
      Sim.Players.Players[Sim.localPlayer].LeaveRoom();
}