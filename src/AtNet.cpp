//============================================================================================
// AtNet.cpp : Handling the Network things:
//============================================================================================
// Link: "AtNet.h"
//============================================================================================
#include "stdafx.h"
#include "atnet.h"
#include "Buero.h"

#include "SbLib.h"
#include "network.h"
extern SBNetwork gNetwork;

#define GFX_MENU			 (0x00000000554e454d)

SLONG nPlayerOptionsOpen[4]={0,0,0,0};     //Fummelt gerade wer an den Options?
SLONG nPlayerAppsDisabled[4]={0,0,0,0};    //Ist ein anderer Spieler gerade in einer anderen Anwendung?
SLONG nPlayerWaiting[4]={0,0,0,0};         //Hinkt jemand hinterher?

extern SLONG gTimerCorrection;                    //Is it necessary to adapt the local clock to the server clock?

//Zum Debuggen:
SLONG rChkTime=0;
ULONG rChkPersonRandCreate=0, rChkPersonRandMisc=0, rChkHeadlineRand=0;
ULONG rChkLMA=0, rChkRBA=0, rChkAA[MAX_CITIES], rChkFrachen=0;
SLONG rChkGeneric, CheckGeneric=0;
SLONG rChkActionId[5*4];

long GenericSyncIds[4]         = { 0,0,0,0 };
long GenericSyncIdPars[4]      = { 0,0,0,0 };
long GenericAsyncIds[4*100]    = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
long GenericAsyncIdPars[4*100] = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };

//--------------------------------------------------------------------------------------------
//Sets the bitmap for the network to display: (0=none; 1=player in options, 2=player in windows; 3=waiting for player)
//--------------------------------------------------------------------------------------------
void SetNetworkBitmap (SLONG Number, SLONG WaitingType)
{
   static SLONG CurrentNumber=-1;

   if (CurrentNumber!=Number || gNetworkBmsType!=WaitingType)
   {
      GfxLib *pGLib=NULL;

      if (Number==0)
      {
         gNetworkBms.Destroy();
         if (pGLib) pGfxMain->ReleaseLib (pGLib);
      }

      if (Number==1) pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("Network1.gli", GliPath), &pGLib, L_LOCMEM);
      if (Number==2) pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("Network2.gli", GliPath), &pGLib, L_LOCMEM);
      if (Number==3) pGfxMain->LoadLib ((char*)(LPCTSTR)FullFilename ("Network3.gli", GliPath), &pGLib, L_LOCMEM);

      if (pGLib)
      {
         if (Number==3)
            gNetworkBms.ReSize (pGLib, "MENU PL0 PL1 PL2 PL3 PL4 PL5 PL6 PL7");
         else
            gNetworkBms.ReSize (pGLib, "MENU");
      }

      CurrentNumber   = Number;
      gNetworkBmsType = WaitingType;
   }
}

//--------------------------------------------------------------------------------------------
//Look for new messages:
//--------------------------------------------------------------------------------------------
void DisplayBroadcastMessage (CString str, SLONG FromPlayer)
{
   SBBM    TempBm (gBroadcastBm.Size);
   SLONG   sy, oldy, offy;

   if (!Sim.bNetwork)
      return;

   if (FromPlayer!=Sim.localPlayer)
   {
      static SLONG LastTime=0;

      if (timeGetTime()-LastTime>500)
         PlayUniversalFx ("netmsg.raw", Sim.Options.OptionEffekte);

      LastTime = timeGetTime();
   }

   if (FromPlayer>=0 && FromPlayer<4) str = Sim.Players.Players[FromPlayer].NameX + ": " + str;

   sy   = gBroadcastBm.TryPrintAt (str, FontSmallBlack, TEC_FONT_LEFT, XY(10, 10), XY(320, 1000));
   oldy = gBroadcastBm.Size.y;
   offy = gBroadcastBm.Size.y;

   if (oldy<10) oldy=10;
   if (offy<10) offy=10;

   TempBm.BlitFrom (gBroadcastBm);
   gBroadcastBm.ReSize (320, oldy+sy+10);
   if (gBroadcastBm.Size.y!=TempBm.Size.y) 
   {
      SB_CBitmapKey Key(*XBubbleBms[9].pBitmap);
      gBroadcastBm.FillWith (*(UWORD*)Key.Bitmap);
   }
   gBroadcastBm.BlitFrom (TempBm);

   TempBm.ReSize (320, sy+5);
   {
      SB_CBitmapKey Key(*XBubbleBms[9].pBitmap);
      TempBm.FillWith (*(UWORD*)Key.Bitmap);
   }
   TempBm.PrintAt (str, FontSmallBlack, TEC_FONT_LEFT, XY(10, 0), XY(320, 1000));
   gBroadcastBm.BlitFrom (TempBm, 0, offy);

   if (gBroadcastBm.Size.y>220)
   {
      SBBM TempBm (gBroadcastBm.Size);

      TempBm.BlitFrom (gBroadcastBm);
      gBroadcastBm.ReSize (gBroadcastBm.Size.x, 220);
      gBroadcastBm.BlitFrom (TempBm, 0, -(TempBm.Size.y-220));
   }

   gBroadcastTimeout = 60;
}

//--------------------------------------------------------------------------------------------
//Updates the Broadcast Bitmap:
//  bJustForEmergency : if true, then this call will only be used it this function hasn't been
//                      called normally for half a second
//--------------------------------------------------------------------------------------------
void PumpBroadcastBitmap (bool bJustForEmergency)
{
   if (!Sim.bNetwork && gBroadcastBm.Size.y==0)
      return;

   static SLONG LastTimeCalled=0;

   if (bJustForEmergency)
   {
      if (timeGetTime()-LastTimeCalled<500) return;
   }
   else
   {
      LastTimeCalled=timeGetTime();
   }

   if (gBroadcastTimeout>0 && gBroadcastBm.Size.y<200) gBroadcastTimeout--;
   else if (gBroadcastBm.Size.y)
   {
      static SLONG p=0;

      gBroadcastTimeout=0;

      if (p++>2 || gBroadcastBm.Size.y<=10 || gBroadcastBm.Size.y>=150)
      {
         p=0;

         if (gBroadcastBm.Size.y-1<=0)
            gBroadcastBm.Destroy ();
         else
         {
            SBBM TempBm (gBroadcastBm.Size);

            TempBm.BlitFrom (gBroadcastBm);
            gBroadcastBm.ReSize (gBroadcastBm.Size.x, gBroadcastBm.Size.y-1);
            gBroadcastBm.BlitFrom (TempBm, 0, -1);
         }
      }
   }
}

//--------------------------------------------------------------------------------------------
//Look for new messages:
//--------------------------------------------------------------------------------------------
void PumpNetwork (void)
{
   SLONG c=0, d=0, e=0; //Universell, können von jedem case verwendet werden.

   if (!Sim.bNetwork) return;

   if (Sim.bThisIsSessionMaster && Sim.Time>9*60000 && Sim.Time<18*60000 && !Sim.CallItADay && !Sim.CallItADayAt)
   {
      static DWORD LastTime=0;

      if (timeGetTime()-LastTime>1000)
      {
         Sim.SendSimpleMessage (ATNET_TIMEPING, NULL, Sim.TimeSlice);
         LastTime = timeGetTime();
      }
   }

   bool bReturnAfterThisMessage=false;
   while (gNetwork.GetMessageCount() && !bReturnAfterThisMessage)
   {
      TEAKFILE Message;

      if (Sim.ReceiveMemFile (Message))
      {
         ULONG MessageType, Par1=0, Par2=0, Par3=0;

         Message >> MessageType;

         switch (MessageType)
         {
             case ATNET_SETSPEED:
                Message >> Par1 >> Par2;
                Sim.Players.Players[(SLONG)Par1].GameSpeed=Par2;
                if (Sim.Players.Players[Sim.localPlayer].LocationWin)
                   ((CStdRaum*)Sim.Players.Players[Sim.localPlayer].LocationWin)->StatusCount=3;
                break;

             case ATNET_FORCESPEED:
                Message >> Par1;
                for (c=0; c<4; c++) Sim.Players.Players[c].GameSpeed=Par1;
                if (Sim.Players.Players[Sim.localPlayer].LocationWin)
                   ((CStdRaum*)Sim.Players.Players[Sim.localPlayer].LocationWin)->StatusCount=3;
                break;

             case ATNET_READYFORMORNING:
                Message >> Par1;
                Sim.Players.Players[SLONG(Par1)].bReadyForMorning=true;
                break;

             case ATNET_READYFORBRIEFING:
                Message >> Par1;
                Sim.Players.Players[SLONG(Par1)].bReadyForBriefing=true;
                break;

             case ATNET_PAUSE:
                FrameWnd->Pause(!Sim.bPause);
                break;

             case ATNET_WANNAJOIN:
                if (Sim.bIsHost)
                   Sim.SendSimpleMessage (ATNET_SORRYFULL, NULL);
                break;

             case ATNET_CHATBROADCAST:
                {
                   CString str;
                   SLONG   from;

                   Message >> from >> str;

                   DisplayBroadcastMessage (str, from);
                }
                break;

             case DPSYS_HOST:
                Sim.bIsHost=TRUE;
                DisplayBroadcastMessage (StandardTexte.GetS (TOKEN_MISC, 7000));
                break;

             case DPSYS_SESSIONLOST:
                DisplayBroadcastMessage (StandardTexte.GetS (TOKEN_MISC, 7001));
                for (c=0; c<4; c++)
                   if (Sim.Players.Players[c].Owner==2)
                   {
                      Sim.Players.Players[c].Owner=1;
                      Sim.Players.Players[c].GameSpeed=3;
                      nPlayerOptionsOpen[c]=0;
                      nPlayerAppsDisabled[c]=0;
                      nPlayerWaiting[c]=0;
                   }

                if (Sim.Players.GetAnzHumanPlayers()==1)
                {
                   gNetwork.DisConnect ();
                   Sim.bNetwork = false;
                   return;
                }

                nOptionsOpen  = 0;
                nAppsDisabled = 0;

                if (nOptionsOpen==0 && nAppsDisabled==0 && Sim.bPause==0) SetNetworkBitmap(0);
                break;

             case DPSYS_DESTROYPLAYERORGROUP:
                {
                   DWORD dwPlayerType;
                   DPID  dpId;

                   Message >> dwPlayerType >> dpId;

                   if (dwPlayerType==DPPLAYERTYPE_PLAYER)
                   {
                      for (c=0; c<4; c++)
                         if (Sim.Players.Players[c].NetworkID==dpId)
                         {
                            nOptionsOpen  -= nPlayerOptionsOpen[c];
                            nAppsDisabled -= nPlayerAppsDisabled[c];
                            nWaitingForPlayer -= nPlayerWaiting[c];

                            if (nOptionsOpen<0) nOptionsOpen=0;
                            if (nAppsDisabled<0) nOptionsOpen=0;
                            if (nWaitingForPlayer<0) nOptionsOpen=0;

                            if (nOptionsOpen==0 && nAppsDisabled==0 && Sim.bPause==0) SetNetworkBitmap(0);

                            Sim.Players.Players[c].Owner=1;
                            Sim.Players.Players[c].NetworkID=0;
                            Sim.Players.Players[c].GameSpeed=3;
                            DisplayBroadcastMessage (bprintf (StandardTexte.GetS (TOKEN_MISC, 7002), (LPCTSTR)Sim.Players.Players[c].NameX));
                         }

                      if (Sim.Players.GetAnzHumanPlayers()==1)
                      {
                         gNetwork.DisConnect ();
                         nOptionsOpen = 0;
                         nAppsDisabled = 0;
                         nWaitingForPlayer = 0;
                         SetNetworkBitmap(0);
                         Sim.bNetwork = false;
                         return;
                      }
                   }
                }
                break;

             case ATNET_OPTIONS:
                Message >> Par1 >> Par2;
                nOptionsOpen+=Par1;
                nPlayerOptionsOpen[(SLONG)Par2]+=Par1;
                SetNetworkBitmap ((nOptionsOpen>0)*1);
                break;

             case ATNET_ACTIVATEAPP:
                Message >> Par1 >> Par2;
                nAppsDisabled+=Par1;
                nOptionsOpen+=Par1;
                nPlayerOptionsOpen[(SLONG)Par2]+=Par1;
                nPlayerAppsDisabled[(SLONG)Par2]+=Par1;
                SetNetworkBitmap ((nOptionsOpen>0)*2);
                break;

             case ATNET_TIMEPING:
                Message >> Par1;
                gTimerCorrection = SLONG(Par1)-SLONG(Sim.TimeSlice);
                break;

             case ATNET_PLAYERPOS:
                {
                   SLONG PlayerNum;
                   SLONG MessageTime, LocalTime;

                   Message >> PlayerNum;
                   //if (Sim.Players.Players[PlayerNum].Owner!=1) hprintf ("Received Message ATNET_PLAYERPOS (%li)", PlayerNum);

                   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];
                   PERSON &qPerson = Sim.Persons[Sim.Persons.GetPlayerIndex(PlayerNum)];

                   XY OldPosition = qPerson.Position;

                   //Read the message data:
                   Message >> qPlayer.PrimaryTarget.x   >> qPlayer.PrimaryTarget.y;
                   Message >> qPlayer.SecondaryTarget.x >> qPlayer.SecondaryTarget.y;
                   Message >> qPlayer.TertiaryTarget.x  >> qPlayer.TertiaryTarget.y;
                   Message >> qPlayer.DirectToRoom      >> qPlayer.iWalkActive;
                   Message >> qPlayer.TopLocation       >> qPlayer.ExRoom;
                   Message >> qPlayer.NewDir            >> qPlayer.WalkSpeed;
                   Message >> qPerson.Target.x          >> qPerson.Target.y;
                   Message >> qPerson.Position.x        >> qPerson.Position.y;
                   Message >> qPerson.ScreenPos.x       >> qPerson.ScreenPos.y;
                   Message >> qPerson.StatePar          >> qPerson.Running;
                   Message >> qPerson.Dir               >> qPerson.LookDir;
                   Message >> qPerson.Phase;

                   qPlayer.UpdateWaypointWalkingDirection ();

                   /* (Sim.Players.Players[PlayerNum].Owner==1 && PlayerNum==2)
                   {
                      hprintf ("Received Message ATNET_PLAYERPOS (%li) (%li,%li)->(%li,%li) [%li]", PlayerNum, qPerson.Position.x, qPerson.Position.y, qPerson.Target.x, qPerson.Target.y, SLONG(qPlayer.NewDir));

                      if ((OldPosition-qPerson.Position).abs()>100) hprintf ("!!Big Pos Delta. OldPos: (%li,%li) NewPos: (%li,%li)", OldPosition.x, OldPosition.y, qPerson.Position.x, qPerson.Position.y);
                   }*/

                   //Message time is different from local time. Adapt data:
                   Message >> MessageTime;
                   LocalTime = Sim.TimeSlice;

                   Sim.TimeSlice = MessageTime;
                   /*hprintf ("Diff=%li", LocalTime-Sim.TimeSlice);*/
                   if (qPlayer.GetRoom()==ROOM_AIRPORT && !qPlayer.IsTalking && (qPlayer.LocationWin==NULL || ((*(CStdRaum*)qPlayer.LocationWin).CurrentMenu!=MENU_WC_F && (*(CStdRaum*)qPlayer.LocationWin).CurrentMenu!=MENU_WC_M)))
                      while (Sim.TimeSlice<LocalTime)
                      {
                         qPerson.DoOnePlayerStep ();
                         qPlayer.UpdateWaypointWalkingDirection ();
                         Sim.TimeSlice++;
                      }

                   Sim.TimeSlice = LocalTime;
                }
                break;

             case ATNET_ADD_EXPLOSION:
                {
                   SLONG PlayerNum;

                   Message >> PlayerNum;

                   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                   qPlayer.OfficeState = 2;
                   qPlayer.pSmack      = new CSmack16;
                   qPlayer.pSmack->Open ("expl.smk");
                   qPlayer.NetSynchronizeFlags();

                   gUniversalFx.Stop();
                   gUniversalFx.ReInit("explode.raw");
                   gUniversalFx.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7);

                   Airport.SetConditionBlock (20+PlayerNum, 1);
                }
                break;

             case ATNET_CAFFEINE:
                {
                   SLONG PlayerNum;

                   Message >> PlayerNum;

                   Message >> Sim.Players.Players[PlayerNum].Koffein;
                }
                break;

             case ATNET_GIMMICK:
                {
                   SLONG PlayerNum, Mode;

                   Message >> PlayerNum >> Mode;

                   PERSON &qPerson = Sim.Persons.Persons[(SLONG)Sim.Persons.GetPlayerIndex(PlayerNum)];

                   if (Mode==1)
                   {
                      qPerson.State   = qPerson.State & ~PERSON_WAITFLAG;
                      qPerson.LookDir = 9; //Gimmick starten
                      qPerson.Phase   = 0;
                      qPerson.Target  = qPerson.Position;

                      Sim.Players.Players[PlayerNum].NewDir     = 8;
                      Sim.Players.Players[PlayerNum].StandCount = 0;
                   }
                   else
                   {
                      Sim.Players.Players[PlayerNum].StandCount = -100;
                      qPerson.Dir        = 8;
                      qPerson.LookDir    = 2;
                      qPerson.Phase      = 0;
                   }
                }
                break;

             case ATNET_PLAYERLOOK:
                {
                   SLONG PlayerNum, Dir;

                   Message >> PlayerNum >> Dir;

                   Sim.Persons.Persons[(SLONG)Sim.Persons.GetPlayerIndex(PlayerNum)].LookAt(Dir);
                }
                break;

             case ATNET_PLAYERSTOP:
                {
                   SLONG PlayerNum;

                   Message >> PlayerNum;

                   Sim.Players.Players[PlayerNum].WalkStopEx();
                }
                break;

             case ATNET_ENTERROOM:
                {
                   SLONG PlayerNum, RoomEntered;

                   Message >> PlayerNum;

                   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                   Message >> qPlayer.DirectToRoom >> RoomEntered;

                   if (RoomEntered!=-1)
                   {
                      CTalker *pTalker=NULL, *pTalker2=NULL;

                      switch (RoomEntered)
                      {
                         case ROOM_AUFSICHT:  pTalker  = &Talkers.Talkers[TALKER_BOSS];       break;
                         case ROOM_ARAB_AIR:  pTalker  = &Talkers.Talkers[TALKER_ARAB];       break;
                         case ROOM_SABOTAGE:  pTalker  = &Talkers.Talkers[TALKER_SABOTAGE];   break;
                         case ROOM_BANK:      pTalker  = &Talkers.Talkers[TALKER_BANKER1];
                                              pTalker2 = &Talkers.Talkers[TALKER_BANKER2];    break;
                         case ROOM_MUSEUM:    pTalker  = &Talkers.Talkers[TALKER_MUSEUM];     break;
                         case ROOM_MAKLER:    pTalker  = &Talkers.Talkers[TALKER_MAKLER];     break;
                         case ROOM_WERKSTATT: pTalker  = &Talkers.Talkers[TALKER_MECHANIKER]; break;
                         case ROOM_WERBUNG:   pTalker  = &Talkers.Talkers[TALKER_WERBUNG];    break;
                      }

                      if (pTalker  && !pTalker->IsBusy())  pTalker->IncreaseLocking();
                      if (pTalker2 && !pTalker2->IsBusy()) pTalker2->IncreaseLocking();

                      if (Sim.RoomBusy[RoomEntered]==0) Sim.RoomBusy[RoomEntered]++;
                   }

                   //if (qPlayer.Owner!=1) hprintf ("Received Message ATNET_ENTERROOM (%li)", PlayerNum);

                   for (c=9; c>=0; c--)
                   {
                      Message >> qPlayer.Locations[c];
                      //if (qPlayer.Owner!=1) hprintf ("qPlayer.Locations[%li]=%li", c, qPlayer.Locations[c]);
                   }

                   qPlayer.CalcRoom ();
                   Sim.UpdateRoomUsage ();

                   //Bei menschlichen nicht-lokalen Mitspielern eine Fehlerbehandlung:
                   if (qPlayer.Owner==2 && Sim.Time>9*60000)
                      for (c=9; c>=0; c--)
                         if (qPlayer.GetRoom()==Sim.Players.Players[Sim.localPlayer].Locations[c])
                            if (qPlayer.GetRoom()!=ROOM_STATISTICS && qPlayer.GetRoom()!=ROOM_GLOBE && qPlayer.GetRoom()!=ROOM_LAPTOP && qPlayer.GetRoom()!=ROOM_PLANEPROPS && qPlayer.GetRoom()!=ROOM_WC_F && qPlayer.GetRoom()!=ROOM_WC_M)
                               Sim.SendSimpleMessage (ATNET_ENTERROOMBAD, qPlayer.NetworkID);
                }
                break;

             case ATNET_ENTERROOMBAD:
                Sim.Players.Players[Sim.localPlayer].LeaveAllRooms ();
                break;

             case ATNET_LEAVEROOM:
                {
                   SLONG PlayerNum, RoomLeft;

                   Message >> PlayerNum;

                   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                   Message >> qPlayer.DirectToRoom >> RoomLeft;

                   if (RoomLeft!=-1)
                   {
                      switch (RoomLeft)
                      {
                         case ROOM_AUFSICHT:  Talkers.Talkers[TALKER_BOSS].DecreaseLocking ();       break;
                         case ROOM_ARAB_AIR:  Talkers.Talkers[TALKER_ARAB].DecreaseLocking ();       break;
                         case ROOM_SABOTAGE:  Talkers.Talkers[TALKER_SABOTAGE].DecreaseLocking ();   break;
                         case ROOM_BANK:      Talkers.Talkers[TALKER_BANKER1].DecreaseLocking ();
                                              Talkers.Talkers[TALKER_BANKER2].DecreaseLocking ();    break;
                         case ROOM_MUSEUM:    Talkers.Talkers[TALKER_MUSEUM].DecreaseLocking ();     break;
                         case ROOM_MAKLER:    Talkers.Talkers[TALKER_MAKLER].DecreaseLocking ();     break;
                         case ROOM_WERKSTATT: Talkers.Talkers[TALKER_MECHANIKER].DecreaseLocking (); break;
                         case ROOM_WERBUNG:   Talkers.Talkers[TALKER_WERBUNG].DecreaseLocking ();    break;
                      }

                      if (Sim.RoomBusy[RoomLeft]) Sim.RoomBusy[RoomLeft]--;
                   }
                   //if (qPlayer.Owner!=1) hprintf ("Received Message ATNET_LEAVEROOM (%li)", PlayerNum);

                   for (c=9; c>=0; c--)
                   {
                      Message >> qPlayer.Locations[c];
                      //if (qPlayer.Owner!=1) hprintf ("qPlayer.Locations[%li]=%li", c, qPlayer.Locations[c]);
                   }

                   qPlayer.CalcRoom ();
                   Sim.UpdateRoomUsage ();
                }
                break;

             case ATNET_CHEAT:
                {
                   SLONG PlayerNum;
                   SLONG Cheat;

                   Message >> PlayerNum >> Cheat;

                   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                   switch (Cheat)
                   {
                      case 0: qPlayer.Money += 10000000; break;
                      case 1: qPlayer.Credit = 0;        break;
                      case 2: qPlayer.Image  = 1000;     break;
                   }
               }
               break;

             case ATNET_SYNC_IMAGE:
                {
                   SLONG Anz, PlayerNum;

                   Message >> Anz;

                   while (Anz>0)
                   {
                      Message >> PlayerNum;

                      PLAYER &qPlayer = Sim.Players.Players[PlayerNum];
                      SLONG   d;

                      Message >> qPlayer.Image >> qPlayer.ImageGotWorse;

                      for (d=0; d<4; d++) Message >> qPlayer.Sympathie[d];
  
                      for (d=Routen.AnzEntries()-1; d>=0; d--) Message >> qPlayer.RentRouten.RentRouten[d].Image;
                      for (d=Cities.AnzEntries()-1; d>=0; d--) Message >> qPlayer.RentCities.RentCities[d].Image;

                      Anz--;
                   }
                }
                break;

             case ATNET_SYNC_MONEY:
                {
                   SLONG Anz, PlayerNum;

                   Message >> Anz;

                   while (Anz>0)
                   {
                      Message >> PlayerNum;

                      PLAYER &qPlayer = Sim.Players.Players[PlayerNum];
                      SLONG   d;

                      Message >> qPlayer.Money     >> qPlayer.Credit    >> qPlayer.Bonus
                              >> qPlayer.AnzAktien >> qPlayer.MaxAktien >> qPlayer.TrustedDividende
                              >> qPlayer.Dividende;

                      for (d=0; d<4;  d++) Message >> qPlayer.OwnsAktien[d] >> qPlayer.AktienWert[d];
                      for (d=0; d<10; d++) Message >> qPlayer.Kurse[d];

                      Anz--;
                   }
                }
                break;

             case ATNET_SYNC_ROUTES:
                {
                   SLONG Anz, PlayerNum;

                   Message >> Anz;

                   while (Anz>0)
                   {
                      Message >> PlayerNum;

                      PLAYER &qPlayer = Sim.Players.Players[PlayerNum];
                      SLONG   d;

                      for (d=Routen.AnzEntries()-1; d>=0; d--)
                      {
                         Message >> qPlayer.RentRouten.RentRouten[d].Rang           >> qPlayer.RentRouten.RentRouten[d].LastFlown
                                 >> qPlayer.RentRouten.RentRouten[d].Image          >> qPlayer.RentRouten.RentRouten[d].Miete
                                 >> qPlayer.RentRouten.RentRouten[d].Ticketpreis    >> qPlayer.RentRouten.RentRouten[d].TicketpreisFC
                                 >> qPlayer.RentRouten.RentRouten[d].TageMitVerlust >> qPlayer.RentRouten.RentRouten[d].TageMitGering;
                      }

                      Anz--;
                   }
                }
                break;

             case ATNET_SYNC_FLAGS:
                {
                   SLONG Anz, PlayerNum;

                   Message >> Anz;

                   while (Anz>0)
                   {
                      Message >> PlayerNum;

                      PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                      Message >> qPlayer.SickTokay       >> qPlayer.RunningToToilet >> qPlayer.PlayerSmoking
                              >> qPlayer.Stunned         >> qPlayer.OfficeState     >> qPlayer.Koffein
                              >> qPlayer.NumFlights      >> qPlayer.WalkSpeed
                              >> qPlayer.WerbeBroschuere >> qPlayer.TelephoneDown   >> qPlayer.Presseerklaerung
                              >> qPlayer.SecurityFlags   >> qPlayer.PlayerStinking
                   			  >> qPlayer.RocketFlags     >> qPlayer.LastRocketFlags;

                      Anz--;
                   }
                }
                break;

             case ATNET_SYNC_OFFICEFLAG:
                {
                   SLONG PlayerNum;

                   Message >> PlayerNum;

                   if (PlayerNum==55)
                   {
                      Message >> Sim.nSecOutDays;
                   }
                   else
                   {
                      PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                      Message >> qPlayer.OfficeState;
                   }
                }
                break;

             case ATNET_SYNC_ITEMS:
                {
                   SLONG Anz, PlayerNum;

                   Message >> Anz;

                   while (Anz>0)
                   {
                      Message >> PlayerNum;

                      PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                      for (SLONG c=0; c<6; c++)
                         Message >> qPlayer.Items[c];

                      Anz--;
                   }
                }
                break;

             case ATNET_SYNC_PLANES:
                {
                   SLONG Anz, PlayerNum;

                   Message >> Anz;

                   while (Anz>0)
                   {
                      Message >> PlayerNum;

                      PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                      Message >> qPlayer.Planes >> qPlayer.Auftraege >> qPlayer.Frachten >> qPlayer.RentCities;

                      Anz--;
                   }
                }
                break;

             case ATNET_SYNC_MEETING:
                {
                   SLONG Anz, PlayerNum;

                   Message >> Anz;

                   while (Anz>0)
                   {
                      Message >> PlayerNum;

                      PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                      Message >> qPlayer.ArabTrust >> qPlayer.ArabMode   >> qPlayer.ArabMode2  >> qPlayer.ArabMode3 >> qPlayer.ArabActive;
                      Message >> qPlayer.ArabOpfer >> qPlayer.ArabOpfer2 >> qPlayer.ArabOpfer3 >> qPlayer.ArabPlane >> qPlayer.ArabHints;
                      Message >> qPlayer.NumPassengers >> qPlayer.NumFracht;

                      Anz--;
                   }

                   BOOL SentFromHost;
                   Message >> SentFromHost;

                   if (SentFromHost)
                   {
                      Message >> Sim.SabotageActs;
                   }
                }
                break;

             case ATNET_ADD_SYMPATHIE:
                {
                   SLONG Anz, PlayerNum, SympathieTarget;

                   Message >> PlayerNum >> SympathieTarget >> Anz;

                   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                   qPlayer.Sympathie[SympathieTarget] += Anz;
                   Limit ((SLONG)-1000, qPlayer.Sympathie[SympathieTarget], (SLONG)1000);
                }
                break;

             case ATNET_SYNCROUTECHANGE:
                {
                   SLONG PlayerNum, RouteId, Ticketpreis, TicketpreisFC;

                   Message >> PlayerNum >> RouteId >> Ticketpreis >> TicketpreisFC;

                   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];
                   if (qPlayer.RentRouten.RentRouten[(SLONG)Routen(RouteId)].Ticketpreis!=Ticketpreis)
                      DebugBreak();
                   if (qPlayer.RentRouten.RentRouten[(SLONG)Routen(RouteId)].TicketpreisFC!=TicketpreisFC)
                      DebugBreak();

                   qPlayer.UpdateTicketpreise (RouteId, Ticketpreis, TicketpreisFC);
                }
                break;

             //--------------------------------------------------------------------------------------------
             //Robot:
             //--------------------------------------------------------------------------------------------
             case ATNET_ROBOT_EXECUTE:
                {
                   SLONG c, PlayerNum;

                   Message >> PlayerNum;
                   
                   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];
                   
                   Message >> qPlayer.WaitWorkTill >> qPlayer.WaitWorkTill2;

                   for (c=0; c<4; c++) Message >> qPlayer.Sympathie[c];
                   for (c=0; c<qPlayer.RobotActions.AnzEntries(); c++) Message >> qPlayer.RobotActions[c];
                }
                break;

             //--------------------------------------------------------------------------------------------
             //Player:
             //--------------------------------------------------------------------------------------------
             case ATNET_PLAYER_REFILL:
                {
                   SLONG Type;
                   SLONG City;
                   SLONG Delta;
                   SLONG Time;
               
                   Message >> Type >> City >> Delta >> Time;
               
                   switch (Type)
                   {
                      case 1: Sim.TickLastMinuteRefill = Delta; LastMinuteAuftraege.RefillForLastMinute();      break;
                      case 2: Sim.TickReisebueroRefill = Delta; ReisebueroAuftraege.RefillForReisebuero();      break;
                      case 3: Sim.TickFrachtRefill     = Delta; gFrachten.Refill();                             break;
                      case 4: AuslandsRefill[City]     = Delta; AuslandsAuftraege[City].RefillForAusland(City); break;
                      case 5: AuslandsFRefill[City]    = Delta; AuslandsFrachten[City].RefillForAusland(City);  break;
                   }
                }
                break;

             case ATNET_PLAYER_TOOK:
                {
                   SLONG Type;
                   SLONG City;
                   SLONG Index;
                   SLONG PlayerNum;
                  
                   Message >> PlayerNum >> Type >> Index >> City;
                  
                   PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];

                   switch (Type)
                   {
                      case 1: LastMinuteAuftraege.Auftraege[Index].Praemie=0;     break;
                      case 2: ReisebueroAuftraege.Auftraege[Index].Praemie=0;     break;
                      case 3: gFrachten.Fracht[Index].Praemie=-1;                 break;
                      case 4: AuslandsAuftraege[City].Auftraege[Index].Praemie=0; break;
                      case 5: AuslandsFrachten[City].Fracht[Index].Praemie=0;     break;
                   }
                }
                break;

             case ATNET_PLAYER_18UHR:
                Sim.b18Uhr = TRUE;

                for (c=0; c<4; c++)
                   Sim.Players.Players[c].bReadyForMorning=false;

                break;

             //--------------------------------------------------------------------------------------------
             //Flugplan:
             //--------------------------------------------------------------------------------------------
             case ATNET_FP_UPDATE:
                {
                   SLONG PlaneId, PlayerNum;

                   Message >> PlaneId >> PlayerNum;

                   PLAYER   &qPlayer = Sim.Players.Players[PlayerNum];
                   if (!qPlayer.Planes.IsInAlbum(PlaneId))
                   {
                      hprintf ("Plane not in Album: %li, %li", PlayerNum, PlaneId);
                   }

                   CPlane &qPlane = qPlayer.Planes[PlaneId];

                   Message >> qPlane.Flugplan;

                   //Daten aktualisieren
                   qPlane.Flugplan.UpdateNextFlight();
                   qPlane.Flugplan.UpdateNextStart();

                   //Alle Aufträge überprüfen:
                   CFlugplan &qPlan = qPlane.Flugplan;
        
                   for (e=qPlan.Flug.AnzEntries()-1; e>=0; e--)
                   {
                      if (qPlan.Flug[e].ObjectType==2)
                         if (!qPlayer.Auftraege.IsInAlbum(qPlan.Flug[e].ObjectId))
                         {
                            hprintf ("Err: Flight %li, %lx", qPlan.Flug[e].ObjectType, qPlan.Flug[e].ObjectId);
                            qPlan.Flug[e].ObjectType=0;
                         }
                      if (qPlan.Flug[e].ObjectType==4)
                         if (!qPlayer.Frachten.IsInAlbum(qPlan.Flug[e].ObjectId))
                         {
                            hprintf ("Err: Flight %li, %lx", qPlan.Flug[e].ObjectType, qPlan.Flug[e].ObjectId);
                            qPlan.Flug[e].ObjectType=0;
                         }

                      if (qPlan.Flug[e].ObjectType!=0)
                      {
                         if (!Cities.IsInAlbum(qPlan.Flug[e].VonCity))
                         {
                            hprintf ("Err: Flight %li, VonCity %lx", qPlan.Flug[e].ObjectType, qPlan.Flug[e].VonCity);
                            qPlan.Flug[e].ObjectId=0;
                         }
                         if (!Cities.IsInAlbum(qPlan.Flug[e].NachCity))
                         {
                            hprintf ("Err: Flight %li, NachCity %lx", qPlan.Flug[e].ObjectType, qPlan.Flug[e].NachCity);
                            qPlan.Flug[e].ObjectId=0;
                         }
                      }
                   }

                   qPlayer.UpdateAuftragsUsage();
                   qPlayer.UpdateFrachtauftragsUsage();
                   qPlayer.Planes[PlaneId].CheckFlugplaene (PlayerNum);
                }
                break;

             case ATNET_TAKE_ORDER:
                {
                   SLONG    PlayerNum;
                   CAuftrag a;

                   Message >> PlayerNum >> a;

                   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                   if (qPlayer.Auftraege.GetNumFree()<3)
                      qPlayer.Auftraege.Auftraege.ReSize (qPlayer.Auftraege.AnzEntries()+10);

                   qPlayer.Auftraege+=a;
                }
                break;

             case ATNET_TAKE_FREIGHT:
                {
                   SLONG   PlayerNum;
                   CFracht a;

                   Message >> PlayerNum >> a;

                   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                   if (qPlayer.Frachten.GetNumFree()<3)
                      qPlayer.Frachten.Fracht.ReSize (qPlayer.Frachten.AnzEntries()+10);

                   qPlayer.Frachten+=a;
                }
                break;

             case ATNET_TAKE_CITY:
               {
                  for (SLONG c=0; c<7; c++)
                  {
                     Message >> TafelData.City[c].Player >> TafelData.City[c].Preis;
                     Message >> TafelData.Gate[c].Player >> TafelData.Gate[c].Preis;
                  }
               }
               break;

             case ATNET_TAKE_ROUTE:
                {
                   SLONG PlayerNum, Route1Id, Route2Id;

                   Message >> PlayerNum >> Route1Id >> Route2Id;

                   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                   Message >> qPlayer.RentRouten.RentRouten[Route1Id];
                   Message >> qPlayer.RentRouten.RentRouten[Route2Id];
                }
                break;

             case ATNET_ADVISOR:
                {
                   SLONG   Art, From, Generic1;
                   PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];

                   Message >> Art >> From >> Generic1;

                   PLAYER &qFromPlayer = Sim.Players.Players[From];

                   switch (Art)
                   {
                      //Tafel: Jemand hat einen überboten
                      case 0:
                         if (qPlayer.HasBerater (BERATERTYP_INFO))
                         {
                            if (Generic1>=14) qPlayer.Messages.AddMessage (BERATERTYP_INFO, bprintf (StandardTexte.GetS (TOKEN_ADVICE, 9001), (LPCTSTR)qFromPlayer.NameX, (LPCTSTR)qFromPlayer.AirlineX));
                            if (Generic1>=7 && Generic1<14) qPlayer.Messages.AddMessage (BERATERTYP_INFO, bprintf (StandardTexte.GetS (TOKEN_ADVICE, 9002), (LPCTSTR)qFromPlayer.NameX, (LPCTSTR)qFromPlayer.AirlineX, (LPCTSTR)Cities[TafelData.City[Generic1-7].ZettelId].Name));
                         }
                         break;

                      //Jemand kauft gebrauchtes Flugzeug:
                      case 1:
                         if (qPlayer.HasBerater (BERATERTYP_INFO))
                         {
                            qPlayer.Messages.AddMessage (BERATERTYP_INFO, bprintf (StandardTexte.GetS (TOKEN_ADVICE, 9000), (LPCTSTR)qFromPlayer.NameX, (LPCTSTR)qFromPlayer.AirlineX, Sim.UsedPlanes[0x1000000+Generic1].CalculatePrice()));
                         }
                         break;

                      //Jemand gibt Aktien aus:
                      case 3:
                         if (qPlayer.HasBerater (BERATERTYP_INFO))
                         {
                            qPlayer.Messages.AddMessage (BERATERTYP_INFO, bprintf (StandardTexte.GetS (TOKEN_ADVICE, 9004), (LPCTSTR)qFromPlayer.NameX, (LPCTSTR)qFromPlayer.AirlineX, Generic1));
                         }
                         break;

                      //Jemand kauft Aktien vom localPlayer:
                      case 4:
                         if (qPlayer.HasBerater (BERATERTYP_INFO))
                         {
                             qPlayer.Messages.AddMessage (BERATERTYP_INFO, bprintf (StandardTexte.GetS (TOKEN_ADVICE, 9005), (LPCTSTR)qFromPlayer.NameX, (LPCTSTR)qFromPlayer.AirlineX, Generic1));
                         }
                         break;
                   }
                }
                break;

             case ATNET_BUY_USED:
                {
                   SLONG PlayerNum, PlaneIndex, Time;

                   Message >> PlayerNum >> PlaneIndex >> Time;

                   PLAYER &qFromPlayer = Sim.Players.Players[PlayerNum];

                   if(qFromPlayer.Planes.GetNumFree() < 2) {
                       qFromPlayer.Planes.Planes.ReSize(qFromPlayer.Planes.AnzEntries() + 10);
                       qFromPlayer.Planes.RepairReferences();
                   }
             		
                   qFromPlayer.Planes += Sim.UsedPlanes[0x1000000+PlaneIndex];

                   Sim.UsedPlanes[0x1000000+PlaneIndex].Name.Empty();
                   Sim.TickMuseumRefill = 0;
                }
                break;

             case ATNET_SELL_USED:
                {
                   SLONG PlayerNum, PlaneId;

                   Message >> PlayerNum >> PlaneId;

                   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                   qPlayer.Planes -= PlaneId;
                }
                break;

             case ATNET_BUY_NEW:
                {
                   SLONG    PlayerNum, Anzahl, Type;
                   TEAKRAND rnd;

                   Message >> PlayerNum >> Anzahl >> Type;

                   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                   rnd.SRand (Sim.Date);
                  
                   for (c=0; c<Anzahl; c++)
                      qPlayer.BuyPlane (Type, &rnd);
                }
                break;

             case ATNET_BUY_NEWX:
                {
                   SLONG    PlayerNum, Anzahl;
                   CXPlane  plane;
                   TEAKRAND rnd;

                   Message >> PlayerNum >> Anzahl >> plane;

                   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                   rnd.SRand (Sim.Date);
                  
                   for (c=0; c<Anzahl; c++)
                      qPlayer.BuyPlane (plane, &rnd);
                }
                break;

             case ATNET_PERSONNEL:
                {
                   SLONG PlayerNum, m, n;
              
                   Message >> PlayerNum >> m >> n;
              
                   if (PlayerNum>=0 && PlayerNum<Sim.Players.Players.AnzEntries())
                   {
                      PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                      qPlayer.Statistiken[STAT_ZUFR_PERSONAL].SetAtPastDay(0, m);
                      qPlayer.Statistiken[STAT_MITARBEITER].SetAtPastDay(0, n);
              
                      SLONG c=0;
                      while (1)
                      {
                         Message >> c;

                         if (c==-1) break;
                         if (qPlayer.Planes.IsInAlbum(c))
                         {
                            Message >> qPlayer.Planes[c].AnzPiloten;
                            Message >> qPlayer.Planes[c].AnzBegleiter;
                            Message >> qPlayer.Planes[c].PersonalQuality;
                         }
                         else
                         {
                           SLONG dummy;

                           Message >> dummy >> dummy >> dummy;
                         }
                      }
                   }
                }
                break;

             case ATNET_PLANEPROPS:
                {
                   SLONG PlayerNum, PlaneId;
              
                   Message >> PlayerNum >> PlaneId;
                   if(PlayerNum > 4)
                    break;
                   Message >> Sim.Players.Players[PlayerNum].MechMode;

                   if (PlaneId!=-1 && Sim.Players.Players[PlayerNum].Planes.IsInAlbum(PlaneId))
                   {
                      CPlane &qPlane = Sim.Players.Players[PlayerNum].Planes[PlaneId];

                      Message >> qPlane.Sitze      >> qPlane.SitzeTarget
                              >> qPlane.Essen      >> qPlane.EssenTarget
                              >> qPlane.Tabletts   >> qPlane.TablettsTarget
                              >> qPlane.Deco       >> qPlane.DecoTarget
                              >> qPlane.Triebwerk  >> qPlane.TriebwerkTarget
                              >> qPlane.Reifen     >> qPlane.ReifenTarget
                              >> qPlane.Elektronik >> qPlane.ElektronikTarget
                              >> qPlane.Sicherheit >> qPlane.SicherheitTarget;
    
                      Message >> qPlane.WorstZustand >> qPlane.Zustand >> qPlane.TargetZustand;
                      Message >> qPlane.AnzBegleiter >> qPlane.MaxBegleiter;
                   }
                }
                break;

             //--------------------------------------------------------------------------------------------
             //Dialog:
             //--------------------------------------------------------------------------------------------
             case ATNET_DIALOG_REQUEST:
                {
                   PLAYER   &qPlayer = Sim.Players.Players[Sim.localPlayer];
                   CStdRaum *pRaum   = (CStdRaum*)qPlayer.LocationWin;

                   SLONG RequestingPlayer;

                   //Ist Spieler bereit, einen Dialog zu beginnen?
                   if (qPlayer.GetRoom()==ROOM_AIRPORT && !qPlayer.IsStuck && 
                       pRaum && pRaum->MenuIsOpen()==FALSE && pRaum->IsDialogOpen()==FALSE)
                   {
                      PERSON &qPerson = Sim.Persons.Persons[(SLONG)Sim.Persons.GetPlayerIndex(Sim.localPlayer)];

                      qPlayer.WalkStopEx();
                      qPlayer.IsTalking = TRUE;

                      Message >> qPerson.Phase      >> RequestingPlayer
                              >> qPerson.Position.x >> qPerson.Position.y;

                      qPerson.Dir     = 8;
                      qPerson.LookDir = 8;

                      Sim.SendSimpleMessage (ATNET_PLAYERLOOK, Sim.Players.Players[RequestingPlayer].NetworkID, qPerson.State, qPerson.Phase);
                      Sim.SendSimpleMessage (ATNET_DIALOG_YES, Sim.Players.Players[RequestingPlayer].NetworkID, Sim.localPlayer, qPerson.Phase);
                   }
                   else
                   {
                      UBYTE Dummy;
                      XY    Dummy2;

                      Message >> Dummy    >> RequestingPlayer
                              >> Dummy2.x >> Dummy2.y;

                      //Nein! Keine Interviews!
                      Sim.SendSimpleMessage (ATNET_DIALOG_NO, Sim.Players.Players[RequestingPlayer].NetworkID);
                   }
                }
                break;

             case ATNET_DIALOG_YES:
                {
                   PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];
                   SLONG   TargetPlayer, Phase;

                   Message >> TargetPlayer >> Phase;

                   Sim.Persons.Persons[(SLONG)Sim.Persons.GetPlayerIndex(TargetPlayer)].Phase=UBYTE(Phase);

                   if (!qPlayer.bDialogStartSent)
                      qPlayer.IsWalking2Player=TargetPlayer;
                }
                break;

             case ATNET_DIALOG_START:
                {
                   PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];
                   SLONG   OtherPlayerNum;

                   CStdRaum *pRaum   = (CStdRaum*)qPlayer.LocationWin;

                   Message >> OtherPlayerNum;

                   //Erneute Abfrage: Ist Spieler bereit, einen Dialog zu beginnen?
                   if (qPlayer.GetRoom()==ROOM_AIRPORT && !qPlayer.IsStuck && pRaum && pRaum->MenuIsOpen()==FALSE && pRaum->IsDialogOpen()==FALSE)
                   {
                      //JA!
                      PERSON &qPerson = Sim.Persons[Sim.Persons.GetPlayerIndex(OtherPlayerNum)];

                      Message >> qPerson.Position.x >> qPerson.Position.y
                              >> qPerson.Phase      >> qPerson.LookDir;

                      if (qPlayer.LocationWin)
                         ((CStdRaum*)qPlayer.LocationWin)->StartDialog (TALKER_COMPETITOR, MEDIUM_AIR, OtherPlayerNum, 1);

                      qPlayer.PlayerDialogState = -1;
                   }
                   else
                   {
                      UBYTE DummyPhase, DummyLookDir;
                      SLONG DummyX, DummyY;

                      Message >> DummyX >> DummyY >> DummyPhase >> DummyLookDir;

                      //Nein! Keine Interviews!
                      Sim.SendSimpleMessage (ATNET_DIALOG_NO, Sim.Players.Players[OtherPlayerNum].NetworkID);
                   }
                }
                break;

             case ATNET_DIALOG_NO:
                {
                   PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];

                   qPlayer.IsWalking2Player  = -1;
                   qPlayer.IsTalking         = 0;

                   qPlayer.PlayerDialogState = -1;
                   qPlayer.WalkStop ();
                   qPlayer.NewDir=8;

                   if (qPlayer.LocationWin) ((CStdRaum*)qPlayer.LocationWin)->StopDialog ();
                }
                break;

             case ATNET_DIALOG_SAY:
                {
                  SLONG   id;
                  PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];

                  Message >> id;

                  MouseClickArea = -102;
                  MouseClickId   = 1;
                  MouseClickPar1 = id;

                  if (qPlayer.LocationWin) ((CStdRaum*)qPlayer.LocationWin)->PreLButtonDown (CPoint (0,0));
                }
                break;

             case ATNET_DIALOG_TEXT:
                {
                   PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];
                   CString Answer;
                   SLONG   id;
                   BOOL    TextAlign;

                   Message >> TextAlign >> id >> Answer;

                   if (qPlayer.LocationWin) ((CStdRaum*)qPlayer.LocationWin)->MakeSayWindow (!TextAlign, id, Answer, ((CStdRaum*)qPlayer.LocationWin)->pFontNormal);
                }
                break;

             case ATNET_DIALOG_NEXT:
                {
                   PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];

                   if (qPlayer.LocationWin) ((CStdRaum*)qPlayer.LocationWin)->PreLButtonDown (CPoint (0,0));
                }
                break;

             case ATNET_DIALOG_DRUNK:
                Sim.Players.Players[Sim.localPlayer].IsDrunk += 400;
                break;

             case ATNET_DIALOG_LOCK:
                {
                   SLONG PlayerNum;

                   Message >> PlayerNum;

                   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];
                   qPlayer.IsTalking = TRUE;
                }
                break;

             case ATNET_DIALOG_UNLOCK:
                {
                   SLONG PlayerNum;

                   Message >> PlayerNum;

                   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];
                   qPlayer.IsTalking = FALSE;

                   //Aus irgendeinem Grund fängt die Spielfigur sonst an zu laufen:
                   if (qPlayer.Owner==2) qPlayer.WalkStopEx ();
                }
                break;

             case ATNET_DIALOG_END:
                {
                   PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];

                   if (qPlayer.LocationWin) ((CStdRaum*)qPlayer.LocationWin)->StopDialog ();

                   qPlayer.PlayerDialogState = -1;
                }
                break;

             case ATNET_DIALOG_KOOP:
                {
                   SLONG PlayerNum;

                   Message >> PlayerNum;

                   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                   Message >> qPlayer.Kooperation;
                }
                break;

             case ATNET_DIALOG_DROPITEM:
                {
                   SLONG PlayerNum, Item;

                   Message >> PlayerNum >> Item;

                   Sim.Players.Players[PlayerNum].DropItem (UBYTE(Item));
                }
                break;

             //--------------------------------------------------------------------------------------------
             //Dialogaufbau per Telefon:
             //--------------------------------------------------------------------------------------------
             case ATNET_PHONE_DIAL:
                {
                   PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];
                   SLONG   OtherPlayerNum, bHandy;

                   Message >> OtherPlayerNum >> bHandy;

                   if (qPlayer.LocationWin)
                   {
                      CStdRaum &qRoom = *((CStdRaum*)qPlayer.LocationWin);

                      bool bImpossible = false; //Kein Telefonat annehmen, wenn wir gerade den Höhrer in die Hand nehmen:
                      if (qPlayer.GetRoom()==ROOM_BURO_A+Sim.localPlayer*10 && (((CBuero*)qPlayer.LocationWin)->KommVarTelefon!=0))
                         bImpossible=true;

                      if (qRoom.IsDialogOpen()==0 && qRoom.MenuIsOpen()==0 && bImpossible==false && Sim.Persons[Sim.Persons.GetPlayerIndex(Sim.localPlayer)].StatePar==0 && qPlayer.TelephoneDown==FALSE)
                      {
                         if (bHandy==0 && qPlayer.GetRoom()!=ROOM_BURO_A+Sim.localPlayer*10)
                            Sim.SendSimpleMessage (ATNET_PHONE_NOTHOME, Sim.Players.Players[OtherPlayerNum].NetworkID);
                         else
                         {
                            Sim.SendSimpleMessage (ATNET_PHONE_ACCEPT, Sim.Players.Players[OtherPlayerNum].NetworkID, Sim.localPlayer, bHandy);

                            gUniversalFx.Stop();
                            gUniversalFx.ReInit("Phone.raw");
                            gUniversalFx.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7);

                            qPlayer.GameSpeed = 0;
                            Sim.SendSimpleMessage (ATNET_SETSPEED, NULL, Sim.localPlayer, qPlayer.GameSpeed);

                            qRoom.StartDialog (TALKER_COMPETITOR, MEDIUM_HANDY, OtherPlayerNum, 1);
                            qRoom.PayingForCall=FALSE;

                            qPlayer.DisplayAsTelefoning();
                            Sim.Players.Players[OtherPlayerNum].DisplayAsTelefoning();
                         }
                      }
                      else Sim.SendSimpleMessage (ATNET_PHONE_BUSY, Sim.Players.Players[OtherPlayerNum].NetworkID);
                   }
                   else Sim.SendSimpleMessage (ATNET_PHONE_BUSY, Sim.Players.Players[OtherPlayerNum].NetworkID);
                }
                break;

             case ATNET_PHONE_ACCEPT:
                {
                   PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];
                   SLONG   OtherPlayerNum, bHandy;

                   Message >> OtherPlayerNum >> bHandy;

                   if (qPlayer.LocationWin)
                      ((CStdRaum*)qPlayer.LocationWin)->StartDialog (TALKER_COMPETITOR, MEDIUM_HANDY, OtherPlayerNum, 0);

                   qPlayer.GameSpeed = 0;
                   Sim.SendSimpleMessage (ATNET_SETSPEED, NULL, Sim.localPlayer, qPlayer.GameSpeed);

                   qPlayer.DisplayAsTelefoning();
                   Sim.Players.Players[OtherPlayerNum].DisplayAsTelefoning();
                }
                break;

             case ATNET_PHONE_BUSY:
                {
                   PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];

                   if (qPlayer.LocationWin)
                   {
                      CStdRaum &qRoom = *((CStdRaum*)qPlayer.LocationWin);

                      qRoom.DialBusyFX.ReInit ("busypure.raw"); //busy pure (without dialing first)
                      qRoom.DialBusyFX.Play(0, Sim.Options.OptionEffekte*100/7);

                      if (qPlayer.GetRoom()==ROOM_BURO_A+Sim.localPlayer*10)
                         ((CBuero*)&qRoom)->SP_Player.SetDesiredMood (SPM_IDLE);
                   }
                }
                break;

             case ATNET_PHONE_NOTHOME:
                {
                   PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];

                   if (qPlayer.LocationWin)
                   {
                      CStdRaum &qRoom = *((CStdRaum*)qPlayer.LocationWin);

                      qRoom.DialBusyFX.ReInit ("noanpure.raw"); //No Answer pure (without dialing first)
                      qRoom.DialBusyFX.Play(0, Sim.Options.OptionEffekte*100/7);

                      if (qPlayer.GetRoom()==ROOM_BURO_A+Sim.localPlayer*10)
                         ((CBuero*)&qRoom)->SP_Player.SetDesiredMood (SPM_IDLE);
                   }
                }
                break;

             //case ATNET_CHATSTART: //Veraltet, wird nicht mehr gebraucht
             //   break;

             //--------------------------------------------------------------------------------------------
             //Chatten:
             //--------------------------------------------------------------------------------------------
             case ATNET_CHATSTOP:
                {
                   PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];

                   if (qPlayer.LocationWin)
                      ((CStdRaum*)qPlayer.LocationWin)->MenuStop();
                }
                break;

             case ATNET_CHATMESSAGE:
                {
                   CString str;
                   PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];

                   Message >> str;

                   if (qPlayer.LocationWin)
                   {
                      ((CStdRaum*)qPlayer.LocationWin)->MenuBms[1].ShiftUp (10);
                      ((CStdRaum*)qPlayer.LocationWin)->MenuBms[1].PrintAt (str, FontSmallRed, TEC_FONT_LEFT, 6, 119, 279, 147);
                      ((CStdRaum*)qPlayer.LocationWin)->MenuRepaint();
                   }
                }
                break;

             case ATNET_CHATMONEY:
                {
                   SLONG   Money, OtherPlayer;
                   PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];

                   Message >> Money >> OtherPlayer;

                   Sim.Players.Players[Sim.localPlayer].ChangeMoney (Money, 3700, Sim.Players.Players[OtherPlayer].NameX);
                   Sim.Players.Players[OtherPlayer].ChangeMoney (-Money, 3701, Sim.Players.Players[Sim.localPlayer].NameX);

                   if (qPlayer.LocationWin)
                   {
                      ((CStdRaum*)qPlayer.LocationWin)->MenuBms[1].ShiftUp (10);
                      ((CStdRaum*)qPlayer.LocationWin)->MenuBms[1].PrintAt (bprintf (StandardTexte.GetS (TOKEN_MISC, 3010), Money), FontNormalGrey, TEC_FONT_LEFT, 6, 119, 279, 147);
                      ((CStdRaum*)qPlayer.LocationWin)->MenuRepaint();
                   }
                }
                break;

             //--------------------------------------------------------------------------------------------
             // Sabotage:
             //--------------------------------------------------------------------------------------------
             case ATNET_SABOTAGE_DIRECT:
                {
                   SLONG Type;
                   XY    Position;

                   Message >> Type;
                   Message >> Position.x >> Position.y;

                   if (Type==ITEM_STINKBOMBE) //Stinkbombe
                   {
                      Sim.AddStenchSabotage (XY(Position.x,Position.y));
                   }
                   else if (Type==ITEM_GLUE) //Klebstoff
                   {
                      UBYTE Dir, NewDir, Phase;

                      Message >> Dir >> NewDir >> Phase;

                      Sim.AddGlueSabotage (Position, Dir, NewDir, Phase);
                   }
                }
                break;

             case ATNET_SABOTAGE_ARAB:
                {
                   SLONG PlayerNum;

                   Message >> PlayerNum;

                   PLAYER &qPlayer = Sim.Players.Players[PlayerNum];

                   Message >> qPlayer.ArabOpfer  >> qPlayer.ArabMode  >> qPlayer.ArabActive >> qPlayer.ArabPlane
                           >> qPlayer.ArabOpfer2 >> qPlayer.ArabMode2 >> qPlayer.ArabOpfer3 >> qPlayer.ArabMode3;
                }
                break;

             case ATNET_WAITFORPLAYER:
                Message >> Par1 >> Par2;
                nWaitingForPlayer+=Par1;
                nPlayerWaiting[(SLONG)Par2]+=Par1;
                if (nPlayerWaiting[(SLONG)Par2]<0) nPlayerWaiting[(SLONG)Par2]=0;
                SetNetworkBitmap ((nWaitingForPlayer>0)*3);
                break;

             case ATNET_TAKETHING:
                {
                   SLONG Item;

                   Message >> Item;

                   switch (Item)
                   {
                      case ITEM_POSTKARTE: Sim.ItemPostcard=false; break;
                      case ITEM_PAPERCLIP: Sim.ItemClips=false;    break;
                      case ITEM_GLUE:      Sim.ItemGlue=2;         break;
                      case ITEM_GLOVE:     Sim.ItemGlove=false;    break;
                      case ITEM_LAPTOP:    Message >> Sim.LaptopSoldTo; break;
                      case ITEM_NONE:      Sim.MoneyInBankTrash=false; break;
                      case ITEM_KOHLE:     Sim.ItemKohle=false;    break;
                      case ITEM_PARFUEM:   Sim.ItemParfuem=false;  break;
                      case ITEM_ZANGE:     Sim.ItemZange=false;    break;
                   }
                }
                break;

             //--------------------------------------------------------------------------------------------
             //Day control:
             //--------------------------------------------------------------------------------------------
             case ATNET_DAYFINISH:
                {
                   SLONG FromPlayer;

                   Message >> FromPlayer;

                   Sim.Players.Players[FromPlayer].CallItADay = TRUE;
                }
                break;
             case ATNET_DAYBACK:
                if (Sim.CallItADayAt==0)
                {
                   SLONG FromPlayer;

                   Message >> FromPlayer;

                   Sim.Players.Players[FromPlayer].CallItADay = FALSE;
                }
                break;
             case ATNET_DAYFINISHALL:
                {
                   PLAYER &qPlayer = Sim.Players.Players[Sim.localPlayer];

                   if (Sim.Options.OptionAutosave && Sim.bNetwork)
                      Sim.SaveGame (11, StandardTexte.GetS (TOKEN_MISC, 5000));

                   Message >> Sim.CallItADayAt;

                   for (SLONG c=0; c<4; c++)
                      if (Sim.Players.Players[c].Owner==2)
                         Sim.Players.Players[c].CallItADay = TRUE;

                   if (qPlayer.CallItADay==0)
                   {
                      Sim.SendSimpleMessage (ATNET_DAYFINISH, NULL, Sim.localPlayer);
                      Sim.SendSimpleMessage (ATNET_DAYFINISH, qPlayer.NetworkID, Sim.localPlayer);
                   }
                }
                break;

             //--------------------------------------------------------------------------------------------
             //Miscellaneous:
             //--------------------------------------------------------------------------------------------
             case ATNET_EXPAND_AIRPORT:
                Sim.ExpandAirport = true;
                break;

             case ATNET_OVERTAKE:
                Message >> Sim.OvertakenAirline >> Sim.OvertakerAirline >> Sim.Overtake;
                break;

             //--------------------------------------------------------------------------------------------
             //Savegames:
             //--------------------------------------------------------------------------------------------
             case ATNET_IO_SAVE:
                {
                   SLONG   CursorY;
                   CString Name;

                   Message >> Sim.UniqueGameId >> CursorY >> Name;

                   Sim.SaveGame (CursorY, Name);
                }
                break;

             case ATNET_IO_LOADREQUEST:
                {
                   SLONG Index, FromPlayer;
                   DWORD UniqueGameId;

                   Message >> FromPlayer >> Index >> UniqueGameId;

                   if (Sim.GetSavegameUniqueGameId(Index, true)==UniqueGameId)
                      Sim.SendSimpleMessage (ATNET_IO_LOADREQUEST_OK, Sim.Players.Players[FromPlayer].NetworkID, Sim.localPlayer, Index);
                   else
                   {
                      Sim.SendSimpleMessage (ATNET_IO_LOADREQUEST_BAD, Sim.Players.Players[FromPlayer].NetworkID, Sim.localPlayer);

                      if (Sim.Players.Players[Sim.localPlayer].LocationWin)
                         ((CStdRaum*)Sim.Players.Players[Sim.localPlayer].LocationWin)->MenuStart (MENU_REQUEST, MENU_REQUEST_NET_LOADTHIS);
                   }
                }
                break;

             case ATNET_IO_LOADREQUEST_OK:
                {
                   SLONG c, FromPlayer, Index;

                   Message >> FromPlayer >> Index;

                   Sim.Players.Players[FromPlayer].bReadyForBriefing=true;

                   //Haben alle ihr okay gegeben?
                   for (c=0; c<4; c++)
                      if (Sim.Players.Players[c].bReadyForBriefing==false && !Sim.Players.Players[c].IsOut && Sim.Players.Players[c].Owner==2)
                         break;

                   if (c==4)
                   {
                      nOptionsOpen--;
                      Sim.SendSimpleMessage (ATNET_OPTIONS, NULL, -1, Sim.localPlayer);
                      Sim.SendSimpleMessage (ATNET_IO_LOADREQUEST_DOIT, NULL, Index);
                      Sim.LoadGame (Index);
                   }
                }
                break;

             case ATNET_IO_LOADREQUEST_BAD:
                if (Sim.Players.Players[Sim.localPlayer].bReadyForBriefing==false)
                {
                   nOptionsOpen--;
                   Sim.SendSimpleMessage (ATNET_OPTIONS, NULL, -1, Sim.localPlayer);
                   Sim.Players.Players[Sim.localPlayer].bReadyForBriefing=true;

                   if (Sim.Players.Players[Sim.localPlayer].LocationWin)
                      ((CStdRaum*)Sim.Players.Players[Sim.localPlayer].LocationWin)->MenuStart (MENU_REQUEST, MENU_REQUEST_NET_LOADONE);
                }
                break;

             case ATNET_IO_LOADREQUEST_DOIT:
                {
                   SLONG Index;

                   Message >> Index;

                   Sim.LoadGame (Index);
                }
                break;

             //--------------------------------------------------------------------------------------------
             //Testing & Debugging:
             //--------------------------------------------------------------------------------------------
             case ATNET_CHECKRANDS:
                {
                   SLONG rTime, rGeneric;
                   ULONG rPersonRandCreate, rPersonRandMisc, rHeadlineRand;
                   ULONG rLMA, rRBA, rAA[MAX_CITIES], rFrachen;
                   SLONG rActionId[5*4];

                   Message >> rTime;
                   Message >> rPersonRandCreate >> rPersonRandMisc >> rHeadlineRand;
                   Message >> rLMA >> rRBA >> rFrachen >> rGeneric;

                   for (long c=0; c<MAX_CITIES; c++) Message >> rAA[c];
                   for (c=0; c<20; c++) Message >> rActionId[c];

#ifdef _DEBUG
                   if (rTime!=rChkTime)                         DisplayBroadcastMessage (bprintf("rTime: %li vs %li\n", rTime, rChkTime));
                   if (rPersonRandCreate!=rChkPersonRandCreate) DisplayBroadcastMessage (bprintf("rPersonRandCreate: %li vs %li\n", rPersonRandCreate, rChkPersonRandCreate));
                   if (rPersonRandMisc!=rChkPersonRandMisc)     DisplayBroadcastMessage (bprintf("rPersonRandMisc: %li vs %li\n", rPersonRandMisc, rChkPersonRandMisc));
                   if (rHeadlineRand!=rChkHeadlineRand)         DisplayBroadcastMessage (bprintf("rHeadlineRand: %li vs %li\n", rHeadlineRand, rChkHeadlineRand));
                   if (rLMA!=rChkLMA)                           DisplayBroadcastMessage (bprintf("rLMA: %li vs %li\n", rLMA, rChkLMA));
                   if (rRBA!=rChkRBA)                           DisplayBroadcastMessage (bprintf("rRBA: %li vs %li\n", rRBA, rChkRBA));
                   //if (rChkGeneric!=rGeneric)                   DisplayBroadcastMessage (bprintf("rChkGeneric: %li vs %li\n", rChkGeneric, rGeneric));
                   if (rFrachen!=rChkFrachen)                   DisplayBroadcastMessage (bprintf("rFrachen: %li vs %li\n", rFrachen, rChkFrachen));

                   for (c=0; c<MAX_CITIES; c++)
                      if (rAA[c]!=rChkAA[c])
                         DisplayBroadcastMessage (bprintf("rAA[%li]: %li vs %li\n", c, rAA[c], rChkAA[c]));

                   for (c=0; c<20; c++)
                      if (rActionId[c]!=rChkActionId[c])
                         DisplayBroadcastMessage (bprintf("rActionId[%li]: %li vs %li\n", c, rActionId[c], rChkActionId[c]));
#endif
                }
                break;

             case ATNET_GENERICSYNC:
                {
                   long localPlayer;

                   Message >> localPlayer;
                   Message >> GenericSyncIds[localPlayer];

                   bReturnAfterThisMessage=true;
                }
                break;

             case ATNET_GENERICSYNCX:
                {
                   long localPlayer;

                   Message >> localPlayer;
                   Message >> GenericSyncIds[localPlayer] >> GenericSyncIdPars[localPlayer];

                   bReturnAfterThisMessage=true;
                }
                break;

             case ATNET_GENERICASYNC:
                {
                   long SyncId, Par, player;

                   Message >> player;
                   Message >> SyncId >> Par;

                   bReturnAfterThisMessage=true;

                   NetGenericAsync (SyncId, Par, player);
                }
                break;

             //--------------------------------------------------------------------------------------------
             //Weitere Synchronisierungen:
             //--------------------------------------------------------------------------------------------
             case ATNET_BODYGUARD:
                {
                   long localPlayer, delta;

                   Message >> localPlayer >> delta;

                   if (localPlayer!=Sim.localPlayer)
                   {
                      Sim.Players.Players[localPlayer].ChangeMoney (delta, 3130, "");
                   }
                }
                break;

             case ATNET_CHANGEMONEY:
                {
                   long localPlayer, delta, statistikid;

                   Message >> localPlayer >> delta >> statistikid;

                   Sim.Players.Players[localPlayer].ChangeMoney (delta, 9999, "");
                   if (statistikid!=-1) Sim.Players.Players[localPlayer].Statistiken[statistikid].AddAtPastDay (0, delta);
                }
                break;

             case ATNET_SYNCKEROSIN:
                {
                   SLONG  localPlayer, _TankOpen, _TankInhalt, _BadKerosin, _KerosinKind;
                   BOOL   _Tank;
                   double _TankPreis;

                   Message >> localPlayer >> _Tank >> _TankOpen >> _TankInhalt >> _BadKerosin >> _KerosinKind >> _TankPreis;

                   if (localPlayer!=Sim.localPlayer)
                   {
                      PLAYER &qPlayer = Sim.Players.Players[localPlayer];

                      qPlayer.Tank        = _Tank;
                      qPlayer.TankOpen    = _TankOpen;
                      qPlayer.TankInhalt  = _TankInhalt;
                      qPlayer.BadKerosin  = _BadKerosin;
                      qPlayer.KerosinKind = _KerosinKind;
                      qPlayer.TankPreis   = _TankPreis;
                   }
                }
                break;

             case ATNET_SYNCGEHALT:
                {
                   SLONG  localPlayer, gehalt;

                   Message >> localPlayer >> gehalt;

                   Sim.Players.Players[localPlayer].Statistiken[STAT_GEHALT].SetAtPastDay (0, -gehalt);
                }
                break;

             case ATNET_SYNCNUMFLUEGE:
                {
                   SLONG  localPlayer, auftrag, lm;

                   Message >> localPlayer >> auftrag >> lm;

                   Sim.Players.Players[localPlayer].Statistiken[STAT_AUFTRAEGE].SetAtPastDay (0, auftrag);
                   Sim.Players.Players[localPlayer].Statistiken[STAT_LMAUFTRAEGE].SetAtPastDay (0, lm);
                }
                break;

             //--------------------------------------------------------------------------------------------
             //Microsoft and SBLib internal codes:
             //--------------------------------------------------------------------------------------------
             case 0x0003: case 0x0007: case 0x0021:
             case 0x0102: case 0x0103: case 0x0104: case 0x0105:
             case 0x0106: case 0x0107: case 0x0108: case 0x0109:
             case 0x010A: case 0x010D: case 0xDEADBEEF:
                break;

             default:
                //Something is wrong
                hprintf ("Unknown Message %lx", MessageType);
                break;
         }

         //if (Message.MemPointer!=Message.MemBufferUsed)
         //   __asm { int 3 }
      }
   }
}

//--------------------------------------------------------------------------------------------
// Returns a (short) String describing the Medium
//--------------------------------------------------------------------------------------------
CString GetMediumName (SLONG Medium)
{
   return (StandardTexte.GetS (TOKEN_MISC, 7100+Medium));
}


//--------------------------------------------------------------------------------------------
// Kehrt erst zurück, wenn die anderen Spieler hier auch waren:
//--------------------------------------------------------------------------------------------
void NetGenericSync (long SyncId)
{
    if (!Sim.bNetwork) return;
    if (Sim.localPlayer<0 || Sim.localPlayer>3) return;

    Sim.SendSimpleMessage (ATNET_GENERICSYNC, NULL, Sim.localPlayer, SyncId);  //Requesting Sync

    GenericSyncIds[Sim.localPlayer]=SyncId;

    while (1)
    {
       long c;
       for (c=0; c<4; c++)
          if (Sim.Players.Players[c].Owner!=1 && GenericSyncIds[c]!=SyncId && !Sim.Players.Players[c].IsOut)
             break;

       if (c==4) return;

       PumpNetwork();
    }
}

//--------------------------------------------------------------------------------------------
// Kehrt erst zurück, wenn die anderen Spieler hier auch waren:
// Gibt Warnung aus, falls die Parameter unterschiedlich waren.
//--------------------------------------------------------------------------------------------
void NetGenericSync (long SyncId, long Par)
{
#ifdef _DEBUG
   static bool bReentrant=false;

   if (bReentrant) return;
   if (!Sim.bNetwork) return;
   if (Sim.localPlayer<0 || Sim.localPlayer>3) return;
   if (Sim.Players.Players[Sim.localPlayer].Owner==1) return;
   if (Sim.Time==9*60000) return;

   bReentrant=true;

   Sim.SendSimpleMessage (ATNET_GENERICSYNCX, NULL, Sim.localPlayer, SyncId, Par);  //Requesting Sync

   GenericSyncIds[Sim.localPlayer]=SyncId;
   GenericSyncIdPars[Sim.localPlayer]=Par;

   while (1)
   {
      for (long c=0; c<4; c++)
         if (Sim.Players.Players[c].Owner!=1 && GenericSyncIds[c]!=SyncId && !Sim.Players.Players[c].IsOut)
            break;

      if (c==4)
      {
         for (c=0; c<4; c++)
            if (Sim.Players.Players[c].Owner!=1 && !Sim.Players.Players[c].IsOut && GenericSyncIdPars[c]!=Par)
            {
               DisplayBroadcastMessage (bprintf("NetGenericSync (%li): %li vs. %li\n", SyncId, Par, GenericSyncIdPars[c]));
               DebugBreak();
            }

         for (c=0; c<4; c++)
            GenericSyncIds[c]=0;

         bReentrant=false;
         return;
      }

      PumpNetwork();
   }
#endif
}

//--------------------------------------------------------------------------------------------
// Kehrt erst zurück, wenn die anderen Spieler hier auch waren:
// Gibt Warnung aus, falls die Parameter unterschiedlich waren.
//--------------------------------------------------------------------------------------------
void NetGenericAsync (long SyncId, long Par, long player)
{
#ifdef _DEBUG
   if (!Sim.bNetwork) return;
   if (Sim.localPlayer<0 || Sim.localPlayer>3) return;
   if (Sim.Players.Players[Sim.localPlayer].Owner==1) return;
   if (Sim.Time==9*60000) return;

   if (player==-1)
   {
      Sim.SendSimpleMessage (ATNET_GENERICASYNC, NULL, Sim.localPlayer, SyncId, Par);  //Requesting Sync
      player=Sim.localPlayer;
   }

   long d;

   //Gibt es den Eintrag schon?
   for (d=0; d<400; d++)
      if (GenericAsyncIds[d]==SyncId)
         break;

   //Eventuell müssen wir einen Leereintrag suchen:
   if (d==400)
   {
      for (d=0; d<400; d+=4)
         if (GenericAsyncIds[d]==0 && GenericAsyncIds[d+1]==0 && GenericAsyncIds[d+2]==0 && GenericAsyncIds[d+3]==0)
            break;

      if (d==400)
      {
         DisplayBroadcastMessage ("NetGenericAsync overflow\n");
         return;
      }
   }

   d=d/4*4;
   GenericAsyncIds[d+player]=SyncId;
   GenericAsyncIdPars[d+player]=Par;

   for (long c=0; c<4; c++)
      if (Sim.Players.Players[c].Owner!=1 && GenericAsyncIds[d+c]!=SyncId && !Sim.Players.Players[c].IsOut)
         break;

   if (c==4)
   {
      for (c=0; c<4; c++)
         if (Sim.Players.Players[c].Owner!=1 && !Sim.Players.Players[c].IsOut && GenericAsyncIdPars[d+c]!=Par)
         {
            DisplayBroadcastMessage (bprintf("NetGenericAsync (%li): %li vs. %li\n", SyncId, Par, GenericAsyncIdPars[d+c]));
            DebugBreak();
         }

      for (c=0; c<4; c++)
         GenericAsyncIds[d+c]=0;
   }
#endif
}