//============================================================================================
// Dialog.cpp : Alle Dialoge
//============================================================================================
// Link: "StdRaum.h"
//============================================================================================
#include "stdafx.h"
#include "Aufsicht.h"
#include "nasa.h"
#include "Atnet.h"

static SLONG TankInfo [] = {  10000,  10000,
                              30000,  20000,
                              50000,  40000,
                             100000,  60000 };

static SLONG SabotagePrice[]  = {   1000,   5000,   10000,   50000, 100000 };
static SLONG SabotagePrice2[] = {  10000,  25000,   50000,  250000 };
static SLONG SabotagePrice3[] = { 100000, 500000, 1000000, 2000000, 5000000, 8000000 };

SLONG RocketPrices [] = {   200000,  400000,   600000,
                           5000000, 8000000, 10000000, 20000000, 25000000, 50000000, 85000000 };

SLONG StationPrices [] = {  1000000,  2000000,  3000000,
                            2000000, 10000000, 20000000, 35000000,  50000000, 
                           35000000, 80000000 };

CString Space = " ";

extern XY BeraterSprechblasenOffset [];

extern SLONG MoneyTipWait;

extern BOOL   WasLButtonDown;
extern CPoint WasLButtonDownPoint;
extern SLONG  WasLButtonDownMouseClickArea;   //In Statusleiste/Raum
extern SLONG  WasLButtonDownMouseClickId;     //Der Id
extern SLONG  WasLButtonDownMouseClickPar1;
extern SLONG  WasLButtonDownMouseClickPar2;

//--------------------------------------------------------------------------------------------
//Vor allem anderen einen Klick auf den Berater prüfen:
//--------------------------------------------------------------------------------------------
BOOL CStdRaum::PreLButtonDown (CPoint point)
{
   PLAYER    &qPlayer  =Sim.Players.Players[PlayerNum];
   CMessages &qMessages=qPlayer.Messages;
   CString    Answer;

   if (nOptionsOpen && Sim.bNetwork && Sim.localPlayer<Sim.Players.Players.AnzEntries() && Sim.Players.Players[Sim.localPlayer].GetRoom()!=ROOM_OPTIONS && Sim.Players.Players[Sim.localPlayer].GetRoom()!=0) return (TRUE);

   if (bHandy)
   {
      WasLButtonDown=TRUE;
      WasLButtonDownPoint=point;

      WasLButtonDownMouseClickArea = MouseClickArea;
      WasLButtonDownMouseClickId   = MouseClickId;
      WasLButtonDownMouseClickPar1 = MouseClickPar1;
      WasLButtonDownMouseClickPar2 = MouseClickPar2;

      return (TRUE);
   }

   if (nOptionsOpen==false && qMessages.BeraterWalkState==0 && qMessages.AktuellerBeraterTyp!=-1 && qMessages.IsDialog==0)
   {
      if (XY(point).IfIsWithin (640-80, 440-120, 640, 440))
      {
         qMessages.TalkCountdown=1;
         return (TRUE);
      }
      else if (qMessages.Messages[(SLONG)0].Message.GetLength()>0 &&
               XY(point).IfIsWithin (640-BeraterBms[qMessages.AktuelleBeraterBitmap][0].Size.x+BeraterSprechblasenOffset[qMessages.AktuelleBeraterBitmap].x-qMessages.SprechblaseBm.Size.x, qMessages.BeraterPosY+BeraterSprechblasenOffset[qMessages.AktuelleBeraterBitmap].y,
                                     640-BeraterBms[qMessages.AktuelleBeraterBitmap][0].Size.x+BeraterSprechblasenOffset[qMessages.AktuelleBeraterBitmap].x, qMessages.BeraterPosY+BeraterSprechblasenOffset[qMessages.AktuelleBeraterBitmap].y+qMessages.SprechblaseBm.Size.y))
      {
         qMessages.TalkCountdown=1;
         return (TRUE);
      }
   }
   
   if (MenuIsOpen())
   {
      MenuLeftClick (point);
      return (TRUE);
   }
   else if (DialogPartner!=TALKER_NONE)
   {
      //Klick ignorieren, wenn er erst anfängt zu reden:
      if (pSmackerPartner!=NULL && TextAlign==0 && pSmackerPartner->GetMood()!=SPM_TALKING && timeGetTime()<=(DWORD)SmackerTimeToTalk)
      {
         //Hack, damit leerer Sprach-Text weggeklickt werden kann:
         if (CanCancelEmpty != TRUE)
            return (TRUE);
      }

      //Verhindern, daß ein Doppelklick ein Thema zweimal aufbringt:
      static SLONG LastClickTime, LastClickId=-1;
      if (LastClickId==MouseClickPar1 && timeGetTime()-LastClickTime<1000) return (TRUE);
      LastClickId=MouseClickPar1;
      LastClickTime=timeGetTime();

      if (CurrentMenu == MENU_GAMEOVER && timeGetTime()-TimeAtStart<500) return(TRUE);

      //Wenn gerade die Textblase gemacht wird, dann wird der Klick verschoben
      if (IsPaintingTextBubble)
      {
         LastClickId=-1;
         PleaseCancelTextBubble=TRUE;
         return (TRUE);
      }

      SLONG c, tmp, tmp2, id;
      bool  bJustDeletedTextWindow=false;

      if (MouseClickArea==-102 && MouseClickId==1)
      {
         id=MouseClickPar1;
         if (id-CurrentTextSubIdVon>=0 && id-CurrentTextSubIdVon<10) Answer=OrgOptionen[id-CurrentTextSubIdVon];
         if (OnscreenBitmap.Size.y>0) bJustDeletedTextWindow=true;
         CloseTextWindow ();
      }
      else if (MouseClickArea==-102 && MouseClickId==2)
      {
         id=MouseClickPar1;
         if (id-CurrentTextSubIdVon>=0 && id-CurrentTextSubIdVon<10) Answer=OrgOptionen[id-CurrentTextSubIdVon];
         if (OnscreenBitmap.Size.y>0) bJustDeletedTextWindow=true;
         CloseTextWindow ();
      }
      else
      {
         return (TRUE);  //kein gültiger Klick
      }

      BOOL bIsNetworkDialog = (DialogPartner==TALKER_COMPETITOR && Sim.Players.Players[DialogPar1].Owner==2);

      //Bei Netzwerkdialogen wird ein Klick ignoriert, wenn der andere Spieler gerade etwas auswählt:
      if (bIsNetworkDialog && OnscreenBitmap.Size.y==0 && bJustDeletedTextWindow==false) return (TRUE);
      
      //if (CurrentTextSubIdVon!=CurrentTextSubIdBis && CurrentTextSubIdBis!=0 && TextAlign==1 && (Sim.Options.OptionTalking*Sim.Options.OptionDigiSound || bIsNetworkDialog) && gLanguage==LANGUAGE_D)
      if (CurrentTextSubIdVon!=CurrentTextSubIdBis && CurrentTextSubIdBis!=0 && TextAlign==1 && (Sim.Options.OptionTalking*Sim.Options.OptionDigiSound || bIsNetworkDialog))
      {
         LastClickId=-1;
         MakeSayWindow (TextAlign, id, Answer, pFontNormal);

         if (bIsNetworkDialog)
         {
            TEAKFILE Message;

            Message.Announce(128);

            Message << ATNET_DIALOG_TEXT << TextAlign << id << Answer;

            Sim.SendMemFile (Message, Sim.Players.Players[DialogPar1].NetworkID);
         }

         return (TRUE);
      }

      switch (DialogPartner)
      {
         case TALKER_ARAB:
            switch (id)
            {
               case 100:
               case 101: //Hauptauswahl
               case 102:
               case 650:
                  MakeSayWindow (1, TOKEN_ARAB, 200, 203, FALSE, &FontDialog, &FontDialogLight);
                  break;
               case 250: case 301:
                  MakeSayWindow (1, TOKEN_ARAB, 201, 203, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 200: //Allgem Infos gewünscht: (evtl Verweis zu 251)
                  MakeSayWindow (0, TOKEN_ARAB, 250, pFontPartner);
                  break;

               case 201: //Hauptauswahl Kerosinmarkt:
                  if (DialogMedium==MEDIUM_AIR)
                  {
                     MakeSayWindow (0, TOKEN_ARAB, 300, pFontPartner);
                     MakeNumberWindow (TOKEN_ARAB, 9990300, Sim.Kerosin*2, Sim.Kerosin, Sim.Kerosin/2);
                  }
                  else
                  {
                     MakeSayWindow (0, TOKEN_ARAB, 301, pFontPartner);
                     MakeNumberWindow (TOKEN_ARAB, 9990301, Sim.Kerosin*2, Sim.Kerosin, Sim.Kerosin/2);
                  }
                  break;
               case 300: 
                  MakeSayWindow (1, TOKEN_ARAB, 400, 403, TRUE, &FontDialog, &FontDialogLight, Sim.Kerosin*2, Sim.Kerosin, Sim.Kerosin/2);
                  break;

               case 400: //Spieler kann sich für Kerosinpreis entschieden:
               case 401:
               case 402:  //Arab-Mann fragt Menge:
                  MakeSayWindow (0, TOKEN_ARAB, 500, pFontPartner);
                  qPlayer.KerosinKind=id-400;
                  if (qPlayer.HasBerater(BERATERTYP_KEROSIN))
                     qPlayer.Messages.AddMessage (BERATERTYP_KEROSIN, StandardTexte.GetS (TOKEN_ADVICE, 3020+qPlayer.KerosinKind));
                  break;

               case 202: //etwas über die Tanks erzählen
                  MakeSayWindow (0, TOKEN_ARAB, 800, pFontPartner);
                  break; 

               case 403: //Bail Out
                  MakeSayWindow (1, TOKEN_ARAB, 201, 203, FALSE, &FontDialog, &FontDialogLight);
                  break; 

               case 500: //Spieler muß Menge auswählen
               case 620:
                  MenuDialogReEntryB=650;
                  MenuStart (MENU_BUYKEROSIN, qPlayer.KerosinKind);
                  break;

               case 604:
                     MakeSayWindow (0, TOKEN_ARAB, 620, pFontPartner);
                     break;

               case 700:
                  MakeSayWindow (1, TOKEN_ARAB, 201, 203, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 800:
                  MakeSayWindow (1, TOKEN_ARAB, 900, 904, TRUE, &FontDialog, &FontDialogLight);
                  break;

               case 900: //Spieler kauft Tank
               case 901:
               case 902:
               case 903:
                  DialogPar1=id;
                  MakeSayWindow (0, TOKEN_ARAB, 680, pFontPartner);
                  break;

               case 904:
                  MakeSayWindow (1, TOKEN_ARAB, 201, 203, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 680:
                  {
                     SLONG tmp=TankInfo[(DialogPar1-900)*2+1];

                     MakeSayWindow (1, TOKEN_ARAB, 690, 695, TRUE, &FontDialog, &FontDialogLight, "", (LPCTSTR)Insert1000erDots64 (tmp), (LPCTSTR)Insert1000erDots (tmp*2), (LPCTSTR)Insert1000erDots (tmp*3), (LPCTSTR)Insert1000erDots (tmp*5), (LPCTSTR)Insert1000erDots (tmp*10));
                  }
                  break;

               case 690:
                  MakeSayWindow (0, TOKEN_ARAB, 650, pFontPartner);
                  break;

               case 691:
               case 692:
               case 693:
               case 694:
               case 695:
                  {
                     SLONG Anzahl="\x1\x2\x3\x5\xa"[id-691];

                     if (qPlayer.Money-TankInfo[(DialogPar1-900)*2+1]*Anzahl<DEBT_LIMIT)
                        MakeSayWindow (0, TOKEN_ARAB, 6000, pFontPartner);
                     else
                     {
                        qPlayer.Tank+= TankInfo[(DialogPar1-900)*2]/1000*Anzahl;
                        qPlayer.NetUpdateKerosin();

                        qPlayer.ChangeMoney (-TankInfo[(DialogPar1-900)*2+1]*Anzahl, 2091, (CString)bitoa(TankInfo[(DialogPar1-900)*2]), (char*)(LPCTSTR)(CString)bitoa(Anzahl));
                        Sim.SendSimpleMessage (ATNET_CHANGEMONEY, NULL, Sim.localPlayer, -TankInfo[(DialogPar1-900)*2+1]*Anzahl, -1);

                        qPlayer.DoBodyguardRabatt (TankInfo[(DialogPar1-900)*2+1]*Anzahl);
                        MakeSayWindow (0, TOKEN_ARAB, 700, pFontPartner);
                     }
                  }
                  break;

               case 1010:
                  MakeSayWindow (0, TOKEN_ARAB, 1020, pFontPartner);
                  break;

               default:
                  StopDialog ();
                  break;
            }
            break;

         case TALKER_SABOTAGE:
            switch (id)
            {
               case 1020: case 1022:
               case 1045:
               case 1500:
                  MakeSayWindow (1, TOKEN_SABOTAGE, 1030, 1033, TRUE, &FontDialog, &FontDialogLight, 
                     CString(bprintf ("%s (%s)", (LPCTSTR)Sim.Players.Players[0+(PlayerNum<=0)].AirlineX, (LPCTSTR)Sim.Players.Players[0+(PlayerNum<=0)].NameX)).c_str(),
                     CString(bprintf ("%s (%s)", (LPCTSTR)Sim.Players.Players[1+(PlayerNum<=1)].AirlineX, (LPCTSTR)Sim.Players.Players[1+(PlayerNum<=1)].NameX)).c_str(),
                     CString(bprintf ("%s (%s)", (LPCTSTR)Sim.Players.Players[2+(PlayerNum<=2)].AirlineX, (LPCTSTR)Sim.Players.Players[2+(PlayerNum<=2)].NameX)).c_str(),
                     0);
                  break;

               case 3000:
                  qPlayer.SpiderTrust=TRUE;
                  StopDialog ();
                  break;
               case 3002:
                  if (qPlayer.HasSpaceForItem())
                     qPlayer.BuyItem (ITEM_DART);
                  StopDialog ();
                  break;

               case 1030:
               case 1031:
               case 1032:
                  qPlayer.ArabOpfer=id-1030+(id-1030>=PlayerNum);
                  qPlayer.ArabOpfer2=id-1030+(id-1030>=PlayerNum);
                  qPlayer.ArabOpfer3=id-1030+(id-1030>=PlayerNum);

                  if (Sim.Players.Players[qPlayer.ArabOpfer].IsOut)
                     MakeSayWindow (0, TOKEN_SABOTAGE, 1045, pFontPartner);
                  else
                     MakeSayWindow (0, TOKEN_SABOTAGE, 1040, pFontPartner);
                  break;

               case 1040: //Sabotage an Flugzeug oder Person? ==>+<==
//#ifdef _DEBUG
                  MakeSayWindow (1, TOKEN_SABOTAGE, 1047, 1048+1, TRUE, &FontDialog, &FontDialogLight);
//#else
//                  MakeSayWindow (1, TOKEN_SABOTAGE, 1047, 1048, TRUE, &FontDialog, &FontDialogLight);
//#endif
                  break;
               case 1047:
                  if (qPlayer.ArabMode || qPlayer.ArabMode2 || qPlayer.ArabMode3)
                     MakeSayWindow (0, TOKEN_SABOTAGE, 1100, pFontPartner);
                  else
                     MakeSayWindow (1, TOKEN_SABOTAGE, 1050, 1050+min(5, qPlayer.ArabTrust), FALSE, &FontDialog, &FontDialogLight);
                  break;
               case 1048:
                  if (qPlayer.ArabMode || qPlayer.ArabMode2 || qPlayer.ArabMode3)
                     MakeSayWindow (0, TOKEN_SABOTAGE, 1100, pFontPartner);
                  else
                     MakeSayWindow (1, TOKEN_SABOTAGE, 1250, 1250+min(4, qPlayer.ArabTrust), FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 1049:
                  if (qPlayer.ArabMode || qPlayer.ArabMode2 || qPlayer.ArabMode3)
                     MakeSayWindow (0, TOKEN_SABOTAGE, 1100, pFontPartner);
                  else
                     MakeSayWindow (1, TOKEN_SABOTAGE, 1080, 1080+min(6, qPlayer.ArabTrust), FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 1050:
               case 1250:
                  MakeSayWindow (0, TOKEN_SABOTAGE, 1071, pFontPartner);
                  break;

               case 1051:
               case 1052:
               case 1053:
               case 1054:
               case 1055:
                       if (id==1051 && (Sim.Players.Players[qPlayer.ArabOpfer].SecurityFlags&(1<<6))) MakeSayWindow (0, TOKEN_SABOTAGE, 2096, pFontPartner, Sim.Players.Players[qPlayer.ArabOpfer].AirlineX);
                  else if (id==1052 && (Sim.Players.Players[qPlayer.ArabOpfer].SecurityFlags&(1<<6))) MakeSayWindow (0, TOKEN_SABOTAGE, 2096, pFontPartner, Sim.Players.Players[qPlayer.ArabOpfer].AirlineX);
                  else if (id==1053 && (Sim.Players.Players[qPlayer.ArabOpfer].SecurityFlags&(1<<7))) MakeSayWindow (0, TOKEN_SABOTAGE, 2097, pFontPartner, Sim.Players.Players[qPlayer.ArabOpfer].AirlineX);
                  else if (id==1054 && (Sim.Players.Players[qPlayer.ArabOpfer].SecurityFlags&(1<<7))) MakeSayWindow (0, TOKEN_SABOTAGE, 2097, pFontPartner, Sim.Players.Players[qPlayer.ArabOpfer].AirlineX);
                  else if (id==1055 && (Sim.Players.Players[qPlayer.ArabOpfer].SecurityFlags&(1<<6))) MakeSayWindow (0, TOKEN_SABOTAGE, 2096, pFontPartner, Sim.Players.Players[qPlayer.ArabOpfer].AirlineX);
                  else if (qPlayer.Money-SabotagePrice[id-1051]<DEBT_LIMIT)
                     MakeSayWindow (0, TOKEN_SABOTAGE, 6000, pFontPartner);
                  else
                  {
                     if (id==1051) Sim.Players.Players[qPlayer.ArabOpfer].SecurityNeeded|=(1<<6);
                     if (id==1052) Sim.Players.Players[qPlayer.ArabOpfer].SecurityNeeded|=(1<<6);
                     if (id==1053) Sim.Players.Players[qPlayer.ArabOpfer].SecurityNeeded|=(1<<7);
                     if (id==1054) Sim.Players.Players[qPlayer.ArabOpfer].SecurityNeeded|=(1<<7);
                     if (id==1055) Sim.Players.Players[qPlayer.ArabOpfer].SecurityNeeded|=(1<<6);

                     qPlayer.ArabActive=id-1050;
                     MakeSayWindow (0, TOKEN_SABOTAGE, 1060, pFontPartner);
                  }
                  break;

               case 1251:
               case 1252:
               case 1253:
               case 1254:
                       if (id==1251 && (Sim.Players.Players[qPlayer.ArabOpfer2].SecurityFlags&(1<<0))) MakeSayWindow (0, TOKEN_SABOTAGE, 2090, pFontPartner, Sim.Players.Players[qPlayer.ArabOpfer2].AirlineX);
                  else if (id==1252 && (Sim.Players.Players[qPlayer.ArabOpfer2].SecurityFlags&(1<<1))) MakeSayWindow (0, TOKEN_SABOTAGE, 2091, pFontPartner, Sim.Players.Players[qPlayer.ArabOpfer2].AirlineX);
                  else if (id==1253 && (Sim.Players.Players[qPlayer.ArabOpfer2].SecurityFlags&(1<<0))) MakeSayWindow (0, TOKEN_SABOTAGE, 2090, pFontPartner, Sim.Players.Players[qPlayer.ArabOpfer2].AirlineX);
                  else if (id==1254 && (Sim.Players.Players[qPlayer.ArabOpfer2].SecurityFlags&(1<<2))) MakeSayWindow (0, TOKEN_SABOTAGE, 2092, pFontPartner, Sim.Players.Players[qPlayer.ArabOpfer2].AirlineX);
                  else if (qPlayer.Money-SabotagePrice2[id-1251]<DEBT_LIMIT)
                     MakeSayWindow (0, TOKEN_SABOTAGE, 6000, pFontPartner);
                  else
                  {
                     if (id-1250==2 && !Sim.Players.Players[qPlayer.ArabOpfer2].HasItem(ITEM_LAPTOP))
                        MakeSayWindow (0, TOKEN_SABOTAGE, 1300, pFontPartner);
                     else
                     {
                        if (id==1251) Sim.Players.Players[qPlayer.ArabOpfer2].SecurityNeeded|=(1<<0);
                        if (id==1252) Sim.Players.Players[qPlayer.ArabOpfer2].SecurityNeeded|=(1<<1);
                        if (id==1253) Sim.Players.Players[qPlayer.ArabOpfer2].SecurityNeeded|=(1<<0);
                        if (id==1254) Sim.Players.Players[qPlayer.ArabOpfer2].SecurityNeeded|=(1<<2);

                        qPlayer.ArabMode2=id-1250;
                        qPlayer.ArabTrust=max(qPlayer.ArabTrust, id-1250+1);

                        qPlayer.ChangeMoney (-SabotagePrice2[id-1251], 2080, "");
                        Sim.SendSimpleMessage (ATNET_CHANGEMONEY, NULL, Sim.localPlayer, -SabotagePrice2[id-1251], -1);

                        qPlayer.DoBodyguardRabatt (SabotagePrice2[id-1251]);
                        qPlayer.NetSynchronizeSabotage ();
                        MakeSayWindow (0, TOKEN_SABOTAGE, 1160, pFontPartner);
                     }
                  }
                  break;

               case 1081:
               case 1082:
               case 1083:
               case 1084:
                       if (id==1081 && (Sim.Players.Players[qPlayer.ArabOpfer3].SecurityFlags&(1<<8))) MakeSayWindow (0, TOKEN_SABOTAGE, 2098, pFontPartner, Sim.Players.Players[qPlayer.ArabOpfer3].AirlineX);
                  else if (id==1082 && (Sim.Players.Players[qPlayer.ArabOpfer3].SecurityFlags&(1<<5))) MakeSayWindow (0, TOKEN_SABOTAGE, 2095, pFontPartner, Sim.Players.Players[qPlayer.ArabOpfer3].AirlineX);
                  else if (id==1083 && (Sim.Players.Players[qPlayer.ArabOpfer3].SecurityFlags&(1<<5))) MakeSayWindow (0, TOKEN_SABOTAGE, 2095, pFontPartner, Sim.Players.Players[qPlayer.ArabOpfer3].AirlineX);
                  else if (id==1084 && (Sim.Players.Players[qPlayer.ArabOpfer3].SecurityFlags&(1<<3))) MakeSayWindow (0, TOKEN_SABOTAGE, 2093, pFontPartner, Sim.Players.Players[qPlayer.ArabOpfer3].AirlineX);
                  else if (qPlayer.Money-SabotagePrice3[id-1081]<DEBT_LIMIT)
                     MakeSayWindow (0, TOKEN_SABOTAGE, 6000, pFontPartner);
                  else
                  {
                     if (id==1081) Sim.Players.Players[qPlayer.ArabOpfer3].SecurityNeeded|=(1<<8);
                     if (id==1082) Sim.Players.Players[qPlayer.ArabOpfer3].SecurityNeeded|=(1<<5);
                     if (id==1083) Sim.Players.Players[qPlayer.ArabOpfer3].SecurityNeeded|=(1<<5);
                     if (id==1084) Sim.Players.Players[qPlayer.ArabOpfer3].SecurityNeeded|=(1<<3);

                     qPlayer.ArabMode3=id-1080;
                     qPlayer.ArabTrust=max(qPlayer.ArabTrust, id-1080+1);

                     qPlayer.ChangeMoney (-SabotagePrice3[id-1081], 2080, "");
                     Sim.SendSimpleMessage (ATNET_CHANGEMONEY, NULL, Sim.localPlayer, -SabotagePrice2[id-1081], -1);

                     qPlayer.DoBodyguardRabatt (SabotagePrice3[id-1081]);
                     qPlayer.NetSynchronizeSabotage ();

                     MakeSayWindow (0, TOKEN_SABOTAGE, 1160, pFontPartner);
                  }
                  break;
               case 1085:
                  if ((Sim.Players.Players[qPlayer.ArabOpfer3].SecurityFlags&(1<<8))) MakeSayWindow (0, TOKEN_SABOTAGE, 2098, pFontPartner, Sim.Players.Players[qPlayer.ArabOpfer3].AirlineX);
                  else
                  {
                     qPlayer.ArabTrust=max(qPlayer.ArabTrust, id-1280+1);
                     MenuDialogReEntryB=1090;
                     MenuStart (MENU_SABOTAGEPLANE);
                  }
                  break;
               case 1086:
                  if ((Sim.Players.Players[qPlayer.ArabOpfer3].SecurityFlags&(1<<5))) MakeSayWindow (0, TOKEN_SABOTAGE, 2095, pFontPartner, Sim.Players.Players[qPlayer.ArabOpfer3].AirlineX);
                  else
                  {
                     MenuDialogReEntryB=1091;
                     MenuStart (MENU_SABOTAGEROUTE);
                  }
                  break;

               case 1060:
                  MenuDialogReEntryB=1070;
                  MenuStart (MENU_SABOTAGEPLANE);
                  break;

               case 1070:
                  qPlayer.ArabMode=qPlayer.ArabActive;
                  qPlayer.ArabActive=FALSE;
                  qPlayer.ChangeMoney (-SabotagePrice[qPlayer.ArabMode-1], 2080, "");
                  qPlayer.DoBodyguardRabatt (SabotagePrice[qPlayer.ArabMode-1]);
                  qPlayer.ArabTrust = min (5, qPlayer.ArabTrust+1);
                  qPlayer.NetSynchronizeSabotage ();
                  StopDialog ();

                  Sim.SendSimpleMessage (ATNET_CHANGEMONEY, NULL, Sim.localPlayer, -SabotagePrice[qPlayer.ArabMode-1], -1);
                  break;

               case 1090:
                  qPlayer.ArabMode3=5;
                  qPlayer.ArabTrust=max(qPlayer.ArabTrust, id-1280+1);

                  qPlayer.ChangeMoney (-SabotagePrice3[4], 2080, "");
                  Sim.SendSimpleMessage (ATNET_CHANGEMONEY, NULL, Sim.localPlayer, -SabotagePrice3[4], -1);

                  qPlayer.DoBodyguardRabatt (SabotagePrice3[4]);
                  qPlayer.NetSynchronizeSabotage ();

                  MakeSayWindow (0, TOKEN_SABOTAGE, 1160, pFontPartner);
                  break;

               case 1091:
                  qPlayer.ArabMode3=6;
                  qPlayer.ArabTrust=max(qPlayer.ArabTrust, id-1280+1);

                  qPlayer.ChangeMoney (-SabotagePrice3[5], 2080, "");
                  Sim.SendSimpleMessage (ATNET_CHANGEMONEY, NULL, Sim.localPlayer, -SabotagePrice3[5], -1);

                  qPlayer.DoBodyguardRabatt (SabotagePrice3[5]);
                  qPlayer.NetSynchronizeSabotage ();

                  MakeSayWindow (0, TOKEN_SABOTAGE, 1160, pFontPartner);
                  break;

               default:
                  StopDialog ();
                  break;
            }
            break;

         case TALKER_SECURITY:
            switch (id)
            {
               case 1000:
                  MakeSayWindow (1, TOKEN_SECURITY, 2000, 2002, FALSE, &FontDialog, &FontDialogLight);
                  break;
               case 3000:
                  MakeSayWindow (1, TOKEN_SECURITY, 2001, 2002, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 2000:
                  MakeSayWindow (0, TOKEN_SECURITY, 3000, pFontPartner);
                  break;

               case 2001:
                  MenuStart (MENU_SECURITY);
                  break;

               case 2002:
                  MakeSayWindow (0, TOKEN_SECURITY, 4000, pFontPartner);
                  break;

               default:
                  StopDialog ();
                  break;
            }
            break;

         case TALKER_DESIGNER:
            switch (id)
            {
               case 1000:
                  MakeSayWindow (1, TOKEN_DESIGNER, 2000, 2002, FALSE, &FontDialog, &FontDialogLight);
                  break;
               case 3040:
                  MakeSayWindow (1, TOKEN_DESIGNER, 2001, 2002, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 2000:
                  MakeSayWindow (0, TOKEN_DESIGNER, 3000, pFontPartner);
                  break;
               case 3000:
                  MakeSayWindow (1, TOKEN_DESIGNER, 3010, 3010, FALSE, &FontDialog, &FontDialogLight);
                  break;
               case 3010:
                  MakeSayWindow (0, TOKEN_DESIGNER, 3020, pFontPartner);
                  break;
               case 3020:
                  MakeSayWindow (1, TOKEN_DESIGNER, 3030, 3030, FALSE, &FontDialog, &FontDialogLight);
                  break;
               case 3030:
                  MakeSayWindow (0, TOKEN_DESIGNER, 3040, pFontPartner);
                  break;

               case 2001:
                  MakeSayWindow (0, TOKEN_DESIGNER, 4000, pFontPartner);
                  break;
               case 4000:
                  {
                     CDataTable dt;

                     dt.FillWithXPlaneTypes ();

                     //if (CountMatchingFilelist(FullFilename ("*.plane", MyPlanePath))==0)
                     if (dt.AnzRows==0)
                        MakeSayWindow (0, TOKEN_DESIGNER, 4002, pFontPartner);
                     else
                        MakeSayWindow (0, TOKEN_DESIGNER, 4001, pFontPartner);
                  }
                  break;

               case 4001:
                  MenuDialogReEntryB=6000;
                  MenuStart (MENU_BUYXPLANE);
                  break;

               case 2002:
                  MakeSayWindow (0, TOKEN_DESIGNER, 5000, pFontPartner);
                  break;

               case 6000: //Spieler sagt jetzt Zahl der Flugzeuge:
                  {
                     //BUFFER<CString> Array;
                     //GetMatchingFilelist (FullFilename ("*.plane", MyPlanePath), Array);

                     if (MenuPar4!="")
                     {
                        CXPlane plane;
                        CString fn = MenuPar4; //Array[MenuPar1*3+2];
                        if (fn!="") plane.Load (fn);

                        tmp = plane.CalcCost();

                        if (qPlayer.Money-tmp<DEBT_LIMIT)
                        {
                           MakeSayWindow (0, TOKEN_DESIGNER, 6020, pFontPartner);
                           break;
                        }

                        for (c=4; c>=0; c--)
                           if (qPlayer.Money-tmp*"\x1\x2\x3\x5\xa"[c]>=DEBT_LIMIT)
                           {
                              MakeSayWindow (1, TOKEN_DESIGNER, 6010, 6010+c+1, TRUE, &FontDialog, &FontDialogLight, "", (LPCTSTR)Insert1000erDots (tmp), (LPCTSTR)Insert1000erDots (tmp*2), (LPCTSTR)Insert1000erDots (tmp*3), (LPCTSTR)Insert1000erDots (tmp*5), (LPCTSTR)Insert1000erDots (tmp*10));
                              break;
                           }
                     }
                  }
                  break;

               case 6011:
               case 6012:
               case 6013:
               case 6014:
               case 6015:
                  {
                     BUFFER<CString> maybeArray;
                     BUFFER<CString> Array;
                     GetMatchingFilelist (FullFilename ("*.plane", MyPlanePath), maybeArray);

                     Array.ReSize (maybeArray.AnzEntries());

                     long c, d;
                     for (c=0, d=0; c<(SLONG)maybeArray.AnzEntries(); c++)
                     {
                        CXPlane plane;

                        CString fn = FullFilename (maybeArray[c], MyPlanePath);
                        if (fn!="") plane.Load (fn);

                        if (plane.IsBuildable()) 
                           Array[d++]=maybeArray[c];
                     }

                     Array.ReSize (d);

                     if (MenuPar1>=0 && MenuPar1<Array.AnzEntries())
                     {
                        CXPlane plane;
                        CString fn = FullFilename (Array[MenuPar1], MyPlanePath);
                        if (fn!="") plane.Load (fn);

                        if (qPlayer.Money-plane.CalcCost()*("\x1\x2\x3\x5\xa"[id-6011])<DEBT_LIMIT)
                           MakeSayWindow (0, TOKEN_DESIGNER, 6020, pFontPartner);
                        else
                        {
                           TEAKRAND rnd;
                           SLONG    Anzahl = "\x1\x2\x3\x5\xa"[id-6011];
                           SLONG    Type   = MenuPar1-0x10000000;

                           rnd.SRand (Sim.Date);

                           MakeSayWindow (0, TOKEN_DESIGNER, 6030, pFontPartner);
                           for (c=0; c<Anzahl; c++)
                              qPlayer.BuyPlane (plane, &rnd);

                           qPlayer.NetBuyXPlane (Anzahl, plane);
                           //Sim.SendSimpleMessage (ATNET_BUY_NEW, NULL, PlayerNum, Anzahl, Type);

                           qPlayer.DoBodyguardRabatt (plane.CalcCost()*Anzahl);
                           qPlayer.MapWorkers (FALSE);
                           qPlayer.UpdatePersonalberater (1);
                        }
                     }
                  }
                  break;

               default:
                  StopDialog ();
                  break;
            }
            break;

         case TALKER_BANKER2:
            switch (id)
            {
               case 100: //Wie ist mein Kontostand?
                  if (qPlayer.Credit>0)
                  {
                     MakeSayWindow (0, TOKEN_BANK, 110, pFontPartner, (LPCTSTR)Insert1000erDots64 (qPlayer.Money), (LPCTSTR)Insert1000erDots64 (qPlayer.Credit), qPlayer.SollZins);
                     MakeNumberWindow (TOKEN_BANK, 9990110, (LPCTSTR)Insert1000erDots64 (qPlayer.Money), (LPCTSTR)Insert1000erDots64 (qPlayer.Credit), qPlayer.SollZins);
                  }
                  else
                  {
                     MakeSayWindow (0, TOKEN_BANK, 111, pFontPartner, (LPCTSTR)Insert1000erDots64 (qPlayer.Money));
                     MakeNumberWindow (TOKEN_BANK, 9990111, (LPCTSTR)Insert1000erDots64 (qPlayer.Money));
                  }
                  break;

               case 101: //Will Kredit aufnehmen:
                  MenuDialogReEntryB=-1;
                  tmp = qPlayer.CalcCreditLimit()/1000*1000;
                  if (tmp==0) MakeSayWindow (0, TOKEN_BANK, 120, pFontPartner);
                  else if (DialogMedium==MEDIUM_HANDY) MakeSayWindow (0, TOKEN_BANK, 150, pFontPartner);
                  else MenuStart (MENU_BANK, tmp, 1);
                  break;

               case 102: //Will Kredit zurückzahlen:
                  MenuDialogReEntryB=-1;
                  tmp = long(min(0x7fffffff, qPlayer.Credit/2));
                  if (qPlayer.Credit==0) MakeSayWindow (0, TOKEN_BANK, 140, pFontPartner);
                  else if (DialogMedium==MEDIUM_HANDY) MakeSayWindow (0, TOKEN_BANK, 150, pFontPartner);
                  else MenuStart (MENU_BANK, tmp, 2);
                  break;

               case 109:
                  MakeSayWindow (1, TOKEN_BANK, 101, 103, FALSE, &FontDialog, &FontDialogLight);
                  break;

               default:
                  StopDialog ();
                  break;
            }
            break;

         case TALKER_BANKER1:
            switch (id)
            {
               case 900:  case 901:  case 3000: case 3001:
               case 3015: case 3033: case 3051:
                  MakeSayWindow (1, TOKEN_BANK, 1000, 1005, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 920:
                  MakeSayWindow (1, TOKEN_BANK, 921, 921, TRUE, &FontDialog, &FontDialogLight);
                  break;

               case 1000: //Aktien kaufen
                  if (DialogMedium==MEDIUM_HANDY) MakeSayWindow (0, TOKEN_BANK, 1200, pFontPartner);
                  else MakeSayWindow (0, TOKEN_BANK, 2000, pFontPartner);
                  break;
               case 2000: case 2016:
                  {
                     SLONG NumFree[4];

                     for (SLONG c=0; c<4; c++)
                        if (Sim.Players.Players[c].IsOut) NumFree[c]=0;
                        else
                        {
                           NumFree[c]=Sim.Players.Players[c].AnzAktien;
                           for (SLONG d=0; d<4; d++)
                              if (!Sim.Players.Players[c].IsOut)
                                 NumFree[c]-=Sim.Players.Players[d].OwnsAktien[c];
                        }

                     MakeSayWindow (1, TOKEN_BANK, 2010, 2014, 2, &FontDialog, &FontDialogLight, 
                         (LPCTSTR)Sim.Players.Players[0].AirlineX, (LPCTSTR)Insert1000erDots (NumFree[0]),
                         (LPCTSTR)Sim.Players.Players[1].AirlineX, (LPCTSTR)Insert1000erDots (NumFree[1]),
                         (LPCTSTR)Sim.Players.Players[2].AirlineX, (LPCTSTR)Insert1000erDots (NumFree[2]),
                         (LPCTSTR)Sim.Players.Players[3].AirlineX, (LPCTSTR)Insert1000erDots (NumFree[3]), 0);
                  }
                  break;
               case 2030:
               case 2031:
                  {
                     SLONG NumFree[4];

                     for (SLONG c=0; c<4; c++)
                        if (Sim.Players.Players[c].IsOut) NumFree[c]=0;
                        else
                        {
                           NumFree[c]=Sim.Players.Players[c].AnzAktien;
                           for (SLONG d=0; d<4; d++)
                              if (!Sim.Players.Players[c].IsOut)
                                 NumFree[c]-=Sim.Players.Players[d].OwnsAktien[c];
                        }

                     MakeSayWindow (1, TOKEN_BANK, 2020, 2024, 2, &FontDialog, &FontDialogLight, 
                         (LPCTSTR)Sim.Players.Players[0].AirlineX, (LPCTSTR)Insert1000erDots (NumFree[0]),
                         (LPCTSTR)Sim.Players.Players[1].AirlineX, (LPCTSTR)Insert1000erDots (NumFree[1]),
                         (LPCTSTR)Sim.Players.Players[2].AirlineX, (LPCTSTR)Insert1000erDots (NumFree[2]),
                         (LPCTSTR)Sim.Players.Players[3].AirlineX, (LPCTSTR)Insert1000erDots (NumFree[3]), 0);
                  }
                  break;
               case 2010: case 2020:
                  if (Sim.Players.Players[0].IsOut)
                     MakeSayWindow (0, TOKEN_BANK, 2016, pFontPartner);
                  else
                     { MenuDialogReEntryB=2030; MenuStart (MENU_AKTIE, 0, 0);}
                  break;
               case 2011: case 2021:
                  if (Sim.Players.Players[1].IsOut)
                     MakeSayWindow (0, TOKEN_BANK, 2016, pFontPartner);
                  else
                     { MenuDialogReEntryB=2030; MenuStart (MENU_AKTIE, 1, 0);}
                  break;
               case 2012: case 2022:
                  if (Sim.Players.Players[2].IsOut)
                     MakeSayWindow (0, TOKEN_BANK, 2016, pFontPartner);
                  else
                     { MenuDialogReEntryB=2030; MenuStart (MENU_AKTIE, 2, 0);}
                  break;
               case 2013: case 2023:
                  if (Sim.Players.Players[3].IsOut)
                     MakeSayWindow (0, TOKEN_BANK, 2016, pFontPartner);
                  else
                     { MenuDialogReEntryB=2030; MenuStart (MENU_AKTIE, 3, 0);}
                  break;

               case 1001: //Aktien verkaufen:
                  if (DialogMedium==MEDIUM_HANDY) MakeSayWindow (0, TOKEN_BANK, 1200, pFontPartner);
                  else MakeSayWindow (0, TOKEN_BANK, 2100, pFontPartner);
                  break;
               case 2100: case 2116:
                  MakeSayWindow (1, TOKEN_BANK, 2110, 2114, 2, &FontDialog, &FontDialogLight, 
                      (LPCTSTR)Sim.Players.Players[0].AirlineX, (LPCTSTR)Insert1000erDots (qPlayer.OwnsAktien[0]),
                      (LPCTSTR)Sim.Players.Players[1].AirlineX, (LPCTSTR)Insert1000erDots (qPlayer.OwnsAktien[1]),
                      (LPCTSTR)Sim.Players.Players[2].AirlineX, (LPCTSTR)Insert1000erDots (qPlayer.OwnsAktien[2]),
                      (LPCTSTR)Sim.Players.Players[3].AirlineX, (LPCTSTR)Insert1000erDots (qPlayer.OwnsAktien[3]), 0);
                  break;
               case 2130:
               case 2131:
                  MakeSayWindow (1, TOKEN_BANK, 2120, 2124, 2, &FontDialog, &FontDialogLight, 
                      (LPCTSTR)Sim.Players.Players[0].AirlineX, (LPCTSTR)Insert1000erDots (qPlayer.OwnsAktien[0]),
                      (LPCTSTR)Sim.Players.Players[1].AirlineX, (LPCTSTR)Insert1000erDots (qPlayer.OwnsAktien[1]),
                      (LPCTSTR)Sim.Players.Players[2].AirlineX, (LPCTSTR)Insert1000erDots (qPlayer.OwnsAktien[2]),
                      (LPCTSTR)Sim.Players.Players[3].AirlineX, (LPCTSTR)Insert1000erDots (qPlayer.OwnsAktien[3]), 0);
                  break;
               case 2110: case 2120:
                  if (Sim.Players.Players[0].IsOut)
                     MakeSayWindow (0, TOKEN_BANK, 2116, pFontPartner);
                  else
                     { MenuDialogReEntryB=2130; MenuStart (MENU_AKTIE, 0, 1);}
                  break;
               case 2111: case 2121:
                  if (Sim.Players.Players[1].IsOut)
                     MakeSayWindow (0, TOKEN_BANK, 2116, pFontPartner);
                  else
                     { MenuDialogReEntryB=2130; MenuStart (MENU_AKTIE, 1, 1);}
                  break;
               case 2112: case 2122:
                  if (Sim.Players.Players[2].IsOut)
                     MakeSayWindow (0, TOKEN_BANK, 2116, pFontPartner);
                  else
                     { MenuDialogReEntryB=2130; MenuStart (MENU_AKTIE, 2, 1);}
                  break;
               case 2113: case 2123:
                  if (Sim.Players.Players[3].IsOut)
                     MakeSayWindow (0, TOKEN_BANK, 2116, pFontPartner);
                  else
                     { MenuDialogReEntryB=2130; MenuStart (MENU_AKTIE, 3, 1);}
                  break;

               case 2014: case 2024: case 2114: case 2124: case 2514:
                  MakeSayWindow (1, TOKEN_BANK, 1000, 1005, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 1002: //Aktien anwenden:
                  MakeSayWindow (0, TOKEN_BANK, 2500, pFontPartner);
                  break;
               case 2500: case 2520: case 2521: case 2522: case 2523:
                  MakeSayWindow (1, TOKEN_BANK, 2510, 2514, 2, &FontDialog, &FontDialogLight,
                     (LPCTSTR)Sim.Players.Players[0].AirlineX, SLONG(Sim.Players.Players[0].IsOut ? SLONG(0) : qPlayer.OwnsAktien[0]*100/Sim.Players.Players[0].AnzAktien),
                     (LPCTSTR)Sim.Players.Players[1].AirlineX, SLONG(Sim.Players.Players[1].IsOut ? SLONG(0) : qPlayer.OwnsAktien[1]*100/Sim.Players.Players[1].AnzAktien),
                     (LPCTSTR)Sim.Players.Players[2].AirlineX, SLONG(Sim.Players.Players[2].IsOut ? SLONG(0) : qPlayer.OwnsAktien[2]*100/Sim.Players.Players[2].AnzAktien),
                     (LPCTSTR)Sim.Players.Players[3].AirlineX, SLONG(Sim.Players.Players[3].IsOut ? SLONG(0) : qPlayer.OwnsAktien[3]*100/Sim.Players.Players[3].AnzAktien), 0);
                  break;
               case 2510: case 2511: case 2512: case 2513:
                  DialogPar1=id-2510;
                  if (DialogPar1==PlayerNum)
                     MakeSayWindow (0, TOKEN_BANK, 2520, pFontPartner);
                  else if (Sim.Players.Players[DialogPar1].IsOut)
                     MakeSayWindow (0, TOKEN_BANK, 2521, pFontPartner);
                  else if (qPlayer.OwnsAktien[DialogPar1]==0)
                     MakeSayWindow (0, TOKEN_BANK, 2525, pFontPartner);
                  else if (qPlayer.OwnsAktien[DialogPar1]<Sim.Players.Players[DialogPar1].AnzAktien/2)
                  {
                     MakeSayWindow (0, TOKEN_BANK, 2522, pFontPartner);
                     MakeNumberWindow (TOKEN_BANK, 9992522, bitoa (qPlayer.OwnsAktien[DialogPar1]*100/Sim.Players.Players[DialogPar1].AnzAktien));
                  }
                  else if (Sim.Players.Players[DialogPar1].OwnsAktien[PlayerNum]>=qPlayer.AnzAktien*3/10)
                  {
                     MakeSayWindow (0, TOKEN_BANK, 2523, pFontPartner, (LPCTSTR)Sim.Players.Players[DialogPar1].AirlineX);
                     MakeNumberWindow (TOKEN_BANK, 9992523, bitoa (qPlayer.OwnsAktien[DialogPar1]*100/Sim.Players.Players[DialogPar1].AnzAktien));
                  }
                  else
                  {
                     MakeSayWindow (0, TOKEN_BANK, 2524, pFontPartner, (LPCTSTR)Sim.Players.Players[DialogPar1].AirlineX);
                     //MakeNumberWindow (TOKEN_BANK, 9992524, bitoa (qPlayer.OwnsAktien[DialogPar1]*100/Sim.Players.Players[DialogPar1].AnzAktien));
                  }
                  break;
               case 2524:
                  MakeSayWindow (1, TOKEN_BANK, 2530, 2532, FALSE, &FontDialog, &FontDialogLight);
                  break;
               case 2530: case 2531:
                  Sim.Overtake=id-2530+1;
                  Sim.OvertakenAirline=DialogPar1;
                  Sim.OvertakerAirline=PlayerNum;
                  Sim.NetSynchronizeOvertake ();
                  MakeSayWindow (0, TOKEN_BANK, 2540, pFontPartner);
                  break;
               case 2532:
                  MakeSayWindow (1, TOKEN_BANK, 1000, 1005, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 1003: //Aktien ausgeben:
                  tmp = (qPlayer.MaxAktien-qPlayer.AnzAktien)/100*100;
                  if (tmp<10000)
                     MakeSayWindow (0, TOKEN_BANK, 3000, pFontPartner);
                  else if (qPlayer.Kurse[0]<10)
                     MakeSayWindow (0, TOKEN_BANK, 3001, pFontPartner);
                  else
                  {
                     MakeSayWindow (0, TOKEN_BANK, 3002, pFontPartner);
                     MakeNumberWindow (TOKEN_BANK, 9993002, (LPCTSTR)Insert1000erDots (qPlayer.AnzAktien), (LPCTSTR)Insert1000erDots (tmp));
                  }
                  break;
               case 3002:
                  tmp = (qPlayer.MaxAktien-qPlayer.AnzAktien)/100*100;
                  MakeSayWindow (1, TOKEN_BANK, 3010, 3015, TRUE, &FontDialog, &FontDialogLight, tmp/10, tmp/4, tmp/2, tmp*3/4, tmp);
                  break;
               case 3010: //10%
               case 3011: //25%
               case 3012: //50%
               case 3013: //75%
               case 3014: //100%
                  {
                     SLONG Tab[]={ 10, 25, 50, 75, 100 };
                     DialogPar1 = Tab[id-3010]*(qPlayer.MaxAktien-qPlayer.AnzAktien)/100*100/100;
                  }
                  MakeSayWindow (0, TOKEN_BANK, 3020, pFontPartner);
                  break;
               case 3020: MakeSayWindow (0, TOKEN_BANK, 3021, pFontPartner); break;
               case 3021:
                  MakeSayWindow (0, TOKEN_BANK, 3022, pFontPartner, (SLONG)qPlayer.Kurse[0]);
                  MakeNumberWindow (TOKEN_BANK, 9993022, (SLONG)qPlayer.Kurse[0]);
                  break;
               case 3022:
                  tmp=SLONG(qPlayer.Kurse[0]);
                  MakeSayWindow (1, TOKEN_BANK, 3030, 3033, TRUE, &FontDialog, &FontDialogLight, tmp-5, tmp-3, tmp-1);
                  break;
               case 3030: case 3031: case 3032: //Sicherheitsabfrage:
                  {
                     SLONG Tab[] = { 5,3,1 };
                     DialogPar2 = SLONG(qPlayer.Kurse[0] - Tab[id-3030]);
                     DialogPar3 = Tab[id-3030];
                  }
                  MakeSayWindow (0, TOKEN_BANK, 3040, pFontPartner);
                  MakeNumberWindow (TOKEN_BANK, 9993040, (LPCTSTR)Insert1000erDots (DialogPar1), DialogPar2, (SLONG)qPlayer.Kurse[0], (LPCTSTR)Insert1000erDots (DialogPar1*DialogPar2/10/100*100));
                  break;
               case 3040:
                  MakeSayWindow (1, TOKEN_BANK, 3050, 3051, FALSE, &FontDialog, &FontDialogLight);
                  break;
               case 3050: //Wirklich etwas machen: (Aktien ausgeben)
                  {
                     SLONG   MarktAktien;
                     SLONG   AlterKurs=SLONG(qPlayer.Kurse[0]);

                     if (DialogPar3==5) MarktAktien=DialogPar1;
                     else if (DialogPar3==3) MarktAktien=DialogPar1*8/10;
                     else if (DialogPar3==1) MarktAktien=DialogPar1*6/10;

                     qPlayer.ChangeMoney (MarktAktien*DialogPar2, 3162, "");
                     qPlayer.ChangeMoney (-DialogPar1*DialogPar2/10/100*100, 3160, "");

                     long preis = MarktAktien*DialogPar2-DialogPar1*DialogPar2/10/100*100;
                     qPlayer.Statistiken[STAT_E_SONSTIGES].AddAtPastDay (0, preis);
                     if (PlayerNum==Sim.localPlayer) Sim.SendSimpleMessage (ATNET_CHANGEMONEY, NULL, Sim.localPlayer, preis, STAT_E_SONSTIGES);

                     qPlayer.Kurse[0] = (qPlayer.Kurse[0]*__int64(qPlayer.AnzAktien)+__int64(DialogPar2)*MarktAktien) / (qPlayer.AnzAktien+MarktAktien);
                     if (qPlayer.Kurse[0]<0) qPlayer.Kurse[0]=0;

                     //Entschädigung +/-
                     qPlayer.ChangeMoney (-SLONG((qPlayer.AnzAktien-qPlayer.OwnsAktien[PlayerNum])*(AlterKurs-qPlayer.Kurse[0])), 3161, "");
                     for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (c!=PlayerNum)
                        {
                           SLONG entschaedigung = SLONG(Sim.Players.Players[c].OwnsAktien[PlayerNum]*(AlterKurs-qPlayer.Kurse[0]));

                           Sim.Players.Players[c].ChangeMoney (entschaedigung, 3161, "");
                           Sim.Players.Players[c].Statistiken[STAT_E_SONSTIGES].AddAtPastDay (0, entschaedigung);
                           Sim.SendSimpleMessage (ATNET_CHANGEMONEY, NULL, c, entschaedigung, STAT_E_SONSTIGES);
                        }

                     if (Sim.bNetwork)
                        Sim.SendSimpleMessage (ATNET_ADVISOR, NULL, 3, PlayerNum, DialogPar1);

                     qPlayer.AnzAktien+=DialogPar1;
                     qPlayer.OwnsAktien[PlayerNum]+=(DialogPar1-MarktAktien);
                     qPlayer.NetSynchronizeMoney();
                  }
                  MakeSayWindow (0, TOKEN_BANK, 3060, pFontPartner, DialogPar1);
                  break;

               case 1004: //Rendite festlegen:
                  if (DialogMedium==MEDIUM_HANDY) MakeSayWindow (0, TOKEN_BANK, 1200, pFontPartner);
                  else
                  {
                     MenuDialogReEntryB=-1;
                     MenuStart (MENU_SETRENDITE);
                  }
                  break;

               case 102: //Will Kredit zurückzahlen:
                  break;

               default:
                  StopDialog ();
                  break;
            }
            break;

         case TALKER_BOSS:
            switch (id)
            {
               //Ende der Demo-Version
               case 30:
                  MenuStart (MENU_GAMEOVER, 1);
                  break;

               //Eröffnungsdialog
               case 700: MakeSayWindow (0, TOKEN_BOSS, 701, pFontPartner); break;
               case 701:
                  if (Sim.Difficulty>=DIFF_ATFS)
                     MakeSayWindow (0, TOKEN_BOSS, 1800+(Sim.Difficulty-DIFF_ATFS)*10, pFontPartner);
                  else if (Sim.Difficulty>=DIFF_ADDON)
                     MakeSayWindow (0, TOKEN_BOSS, 1600+(Sim.Difficulty-DIFF_ADDON)*10, pFontPartner);
                  else
                     MakeSayWindow (0, TOKEN_BOSS, 1000+Sim.Difficulty*100, pFontPartner);
                  break;

               case 1000: MakeSayWindow (0, TOKEN_BOSS, 1001, pFontPartner); break;
               case 1100: MakeSayWindow (0, TOKEN_BOSS, 1101, pFontPartner); break;
               case 1200: MakeSayWindow (0, TOKEN_BOSS, 1201, pFontPartner); break;
               case 1201: MakeSayWindow (0, TOKEN_BOSS, 1202, pFontPartner); break;
               case 1300:
                  MakeSayWindow (0, 1301, bprintf (DialogTexte.GetS (TOKEN_BOSS, 1301), LPCTSTR(Cities[Sim.MissionCities[0]].Name), LPCTSTR(Cities[Sim.MissionCities[1]].Name), LPCTSTR(Cities[Sim.MissionCities[2]].Name), LPCTSTR(Cities[Sim.MissionCities[3]].Name), LPCTSTR(Cities[Sim.MissionCities[4]].Name), LPCTSTR(Cities[Sim.MissionCities[5]].Name), LPCTSTR(Cities[Sim.HomeAirportId].Name)), pFontPartner);
                  break;

               case 1610: MakeSayWindow (0, TOKEN_BOSS, 1611, pFontPartner); break;
               case 1620: MakeSayWindow (0, TOKEN_BOSS, 1621, pFontPartner); break;
               case 1630: MakeSayWindow (0, TOKEN_BOSS, 1631, pFontPartner); break;
               case 1640: MakeSayWindow (0, TOKEN_BOSS, 1641, pFontPartner); break;
               case 1650: MakeSayWindow (0, TOKEN_BOSS, 1651, pFontPartner); break;
               case 1660: MakeSayWindow (0, TOKEN_BOSS, 1661, pFontPartner); break;
               case 1670: MakeSayWindow (0, TOKEN_BOSS, 1671, pFontPartner); break;
               case 1680: MakeSayWindow (0, TOKEN_BOSS, 1681, pFontPartner); break;
               case 1690: MakeSayWindow (0, TOKEN_BOSS, 1691, pFontPartner); break;
               case 1700: MakeSayWindow (0, TOKEN_BOSS, 1701, pFontPartner); break;

               case 1810: MakeSayWindow (0, TOKEN_BOSS, 1811, pFontPartner); break;
               case 1820: MakeSayWindow (0, TOKEN_BOSS, 1821, pFontPartner); break;
               case 1830: MakeSayWindow (0, TOKEN_BOSS, 1831, pFontPartner); break;
               case 1840: MakeSayWindow (0, TOKEN_BOSS, 1841, pFontPartner); break;
               case 1850: MakeSayWindow (0, TOKEN_BOSS, 1851, pFontPartner); break;
               case 1860: MakeSayWindow (0, TOKEN_BOSS, 1861, pFontPartner); break;
               case 1870: MakeSayWindow (0, TOKEN_BOSS, 1871, pFontPartner); break;
               case 1880: MakeSayWindow (0, TOKEN_BOSS, 1881, pFontPartner); break;
               case 1890: MakeSayWindow (0, TOKEN_BOSS, 1891, pFontPartner); break;
               case 1900: MakeSayWindow (0, TOKEN_BOSS, 1901, pFontPartner); break;

               case 1301: MakeSayWindow (0, TOKEN_BOSS, 1302, pFontPartner); break;
               case 1302: MakeSayWindow (0, TOKEN_BOSS, 1303, pFontPartner); break;
               case 1400: MakeSayWindow (0, TOKEN_BOSS, 1401, pFontPartner); break;
               case 1500: MakeSayWindow (0, TOKEN_BOSS, 1501, pFontPartner); break;
               case 1501: MakeSayWindow (0, TOKEN_BOSS, 1502, pFontPartner); break;

               case 1001: case 1101: case 1202: case 1303: case 1401: case 1502:
                  MakeSayWindow (0, TOKEN_BOSS, 1999, pFontPartner); break;
               case 1999:
                  StopDialog ();
                  //MenuStart (MENU_GAMEOVER);
                  Sim.bNoTime   = FALSE;
                  Sim.DayState  = 2;
                  //qPlayer.LeaveRoom();
                  ((CAufsicht*)this)->TryLeaveAufsicht ();
                  break;

               //Guten Morgen & globale Bewertung:
               case 2000: case 2001: case 2002: case 2003:
               case 5041:
               case 2081:
                  DialogPar1=0;   //Spieler, der jetzt besprochen wird
                  DialogPar2=0;
                  DialogPar3=0;   //Der wievielte Spieler ist das? (Für Eröffnungssatz) Wenn Spieler 0 rausfliegt, bekommt Spieler 1 den Eröffnungssatz.

                  while (DialogPar1<4 && Sim.Players.Players[DialogPar1].IsOut)
                     DialogPar1++;

                  if (Sim.Difficulty==DIFF_FREEGAME)
                  {
                     #ifdef DEMO
                        TeakLibW_Exception (FNL, ExcImpossible, "");
                     #endif
                     goto _und_jetzt_weiter_mit_etc;
                  }
                  else if (Sim.Difficulty>=DIFF_ATFS)
                     MakeSayWindow (0, TOKEN_BOSS, 8500+(Sim.Difficulty-DIFF_ATFS01)*100, pFontPartner);
                  else
                     MakeSayWindow (0, TOKEN_BOSS, 6000+Sim.Difficulty*100, pFontPartner);
                  break;

               //Missionsbericht #0
               case 6000:
                  {
                     CString TmpStr;
                     CString TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           //TmpStr+="[[*]] ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=bitoa (Sim.Players.Players[c].NumAuftraege);

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=bitoa (Sim.Players.Players[c].NumAuftraege);
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 6001, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 6001:
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].NumAuftraege>Sim.Players.Players[d].NumAuftraege)
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (!Sim.Players.Players[d].HasWon())
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 6002, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              //if (Sim.Players.Players[c].NumAuftraege>=10)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty<DIFF_FIRST)
                                 {
                                    Sim.Options.OptionLastMission = DIFF_FIRST;
                                    Sim.MaxDifficulty             = DIFF_FIRST;
                                    Sim.Options.WriteOptions ();
                                 }
                                 hprintf ("Event: Player %li reached mission target.", c);
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 6003, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 //if (Sim.Players.Players[c].NumAuftraege>=10)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 6004, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Mission zu Ende (so oder so)
               case 6003: case 6004:
               case 6103: case 6104:
               case 6203: case 6204:
               case 6303: case 6304:
               case 6403: case 6404:
               case 6503: case 6504:
               case 7103: case 7104:
               case 7203: case 7204:
               case 7303: case 7304:
               case 7403: case 7404:
               case 7503: case 7504:
               case 7603: case 7604:
               case 7703: case 7704:
               case 7803: case 7804:
               case 7903: case 7904:
               case 8003: case 8004:
               case 8503: case 8504:
               case 8603: case 8604:
               case 8703: case 8704:
               case 8803: case 8804:
               case 8903: case 8904:
               case 9003: case 9004:
               case 9103: case 9104:
               case 9203: case 9204:
               case 9303: case 9304:
               case 9403: case 9404:
                  MenuStart (MENU_GAMEOVER, !qPlayer.HasWon());
                  hprintf ("Event: Mission abgeschlossen, Spiel wird beendet.");
                  break;

               //Missionsbericht #1
               case 6100:
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           //TmpStr+="[[*]] ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=bitoa (Sim.Players.Players[c].NumPassengers);

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=bitoa (Sim.Players.Players[c].NumPassengers);
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 6101, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 6101:
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].NumPassengers>Sim.Players.Players[d].NumPassengers)
                              d=c;
                        }

                     //Missionsziel erreicht?
                     //if (Sim.Players.Players[d].NumPassengers<TARGET_PASSENGERS)
                     if (!Sim.Players.Players[d].HasWon())
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 6102, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              //if (Sim.Players.Players[c].NumPassengers>=TARGET_PASSENGERS)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty<DIFF_EASY)
                                 {
                                    Sim.Options.OptionLastMission = DIFF_EASY;
                                    Sim.MaxDifficulty             = DIFF_EASY;
                                    Sim.Options.WriteOptions ();
                                 }
                                 hprintf ("Event: Player %li reached mission target.", c);
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 6103, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 //if (Sim.Players.Players[c].NumPassengers>=TARGET_PASSENGERS)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 6104, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Missionsbericht #2
               case 6200:
                  #ifdef DEMO
                     TeakLibW_Exception (FNL, ExcImpossible, "");
                  #endif
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";

                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Einheiten[EINH_DM].bString64 (Sim.Players.Players[c].Gewinn);

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Einheiten[EINH_DM].bString64 (Sim.Players.Players[c].Gewinn);
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 6201, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 6201:
                  #ifndef DEMO
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].Gewinn>Sim.Players.Players[d].Gewinn)
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (!Sim.Players.Players[d].HasWon())
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 6202, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty<DIFF_NORMAL)
                                 {
                                    Sim.Options.OptionLastMission = DIFF_NORMAL;
                                    Sim.MaxDifficulty             = DIFF_NORMAL;
                                    Sim.Options.WriteOptions ();
                                 }
                                 hprintf ("Event: Player %li reached mission target.", c);
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 6203, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 6204, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  #endif
                  break;

               //Missionsbericht #3:
               case 6300:
                  #ifdef DEMO
                     TeakLibW_Exception (FNL, ExcImpossible, "");
                  #endif
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";

                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=bitoa (Sim.Players.Players[c].ConnectFlags);
                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=bitoa (Sim.Players.Players[c].ConnectFlags);
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 6301, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 6301:
                  #ifndef DEMO
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].ConnectFlags>Sim.Players.Players[d].ConnectFlags)
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (!Sim.Players.Players[d].HasWon())
                     {
                        //Nein:
                        if (Sim.Players.Players[d].ConnectFlags==0) goto _und_jetzt_weiter_mit_etc;

                        MakeSayWindow (0, TOKEN_BOSS, 6302, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty<DIFF_HARD)
                                 {
                                    Sim.Options.OptionLastMission = DIFF_HARD;
                                    Sim.MaxDifficulty             = DIFF_HARD;
                                    Sim.Options.WriteOptions ();
                                 }
                                 hprintf ("Event: Player %li reached mission target.", c);
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 6303, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 6304, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  #endif
                  break;

               //Missionsbericht #4
               case 6400:
                  #ifdef DEMO
                     TeakLibW_Exception (FNL, ExcImpossible, "");
                  #endif
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";

                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=bitoa (Sim.Players.Players[c].Image/10);
                           TmpStr+="%";

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=bitoa (Sim.Players.Players[c].Image/10);
                           TmpStr2+="%";
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 6401, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 6401:
                  #ifndef DEMO
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].Image>Sim.Players.Players[d].Image)
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (!Sim.Players.Players[d].HasWon())
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 6402, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty<DIFF_FINAL)
                                 {
                                    Sim.Options.OptionLastMission = DIFF_FINAL;
                                    Sim.MaxDifficulty             = DIFF_FINAL;
                                    Sim.Options.WriteOptions ();
                                 }
                                 hprintf ("Event: Player %li reached mission target.", c);
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 6403, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 6404, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  #endif
                  break;

               //Missionsbericht #5 (Final-Mission)
               case 6500:
                  #ifdef DEMO
                     TeakLibW_Exception (FNL, ExcImpossible, "");
                  #endif
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";

                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=bitoa (GetAnzBits (Sim.Players.Players[c].RocketFlags));
                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=bitoa (GetAnzBits (Sim.Players.Players[c].RocketFlags));
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 6501, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 6501:
                  #ifndef DEMO
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || GetAnzBits (Sim.Players.Players[c].RocketFlags)>GetAnzBits (Sim.Players.Players[d].RocketFlags))
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (!Sim.Players.Players[d].HasWon())
                     {
                        //Nein:
                        if (GetAnzBits (Sim.Players.Players[d].RocketFlags)==0) goto _und_jetzt_weiter_mit_etc;

                        MakeSayWindow (0, TOKEN_BOSS, 6502, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 hprintf ("Event: Player %li reached mission target.", c);
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 6503, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 6504, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  #endif
                  break;

               //Add-On Missionsbericht #1
               case 7100:
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 7101, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 7101:
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].GetMissionRating()<Sim.Players.Players[d].GetMissionRating())
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (!Sim.Players.Players[d].HasWon())
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 7102, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              //if (Sim.Players.Players[c].Credit==0)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty2<DIFF_ADDON02)
                                 {
                                    Sim.Options.OptionLastMission2 = DIFF_ADDON02;
                                    Sim.MaxDifficulty2             = DIFF_ADDON02;
                                    Sim.Options.WriteOptions ();
                                 }
                                 hprintf ("Event: Player %li reached mission target.", c);
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 7103, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 7104, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Add-On Missionsbericht #2
               case 7200:
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].NumFracht);

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].NumFracht);
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 7201, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 7201:
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].NumFracht>Sim.Players.Players[d].NumFracht)
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (!Sim.Players.Players[d].HasWon())
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 7202, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty2<DIFF_ADDON03)
                                 {
                                    Sim.Options.OptionLastMission2 = DIFF_ADDON03;
                                    Sim.MaxDifficulty2             = DIFF_ADDON03;
                                    Sim.Options.WriteOptions ();
                                 }
                                 hprintf ("Event: Player %li reached mission target.", c);
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 7203, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 7204, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Add-On Missionsbericht #3
               case 7300:
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].NumFrachtFree);

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].NumFrachtFree);
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 7301, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 7301:
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].NumFrachtFree>Sim.Players.Players[d].NumFrachtFree)
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (Sim.Date<TARGET_DAYS)
                     {
                        //Nein;
                        MakeSayWindow (0, TOKEN_BOSS, 7302, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty2<DIFF_ADDON04)
                                 {
                                    Sim.Options.OptionLastMission2 = DIFF_ADDON04;
                                    Sim.MaxDifficulty2             = DIFF_ADDON04;
                                    Sim.Options.WriteOptions ();
                                 }
                                 hprintf ("Event: Player %li reached mission target.", c);
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 7303, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 7304, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Add-On Missionsbericht #4
               case 7400:
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].NumMiles);

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].NumMiles);
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 7401, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 7401:
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].NumMiles>Sim.Players.Players[d].NumMiles)
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (Sim.Date<TARGET_MILESDAYS)
                     {
                        //Nein;
                        MakeSayWindow (0, TOKEN_BOSS, 7402, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty2<DIFF_ADDON05)
                                 {
                                    Sim.Options.OptionLastMission2 = DIFF_ADDON05;
                                    Sim.MaxDifficulty2             = DIFF_ADDON05;
                                    Sim.Options.WriteOptions ();
                                 }
                                 hprintf ("Event: Player %li reached mission target.", c);
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 7403, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 7404, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Add-On Missionsbericht #5
               case 7500:
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 7501, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 7501:
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].GetMissionRating()>Sim.Players.Players[d].GetMissionRating())
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (!Sim.Players.Players[d].HasWon())
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 7502, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty2<DIFF_ADDON06)
                                 {
                                    Sim.Options.OptionLastMission2 = DIFF_ADDON06;
                                    Sim.MaxDifficulty2             = DIFF_ADDON06;
                                    Sim.Options.WriteOptions ();
                                 }
                                 hprintf ("Event: Player %li reached mission target.", c);
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 7503, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 7504, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Add-On Missionsbericht #6
               case 7600:
                  {
                     CString TmpStr, TmpStr2;

                     Sim.Players.UpdateStatistics ();

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 7601, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 7601:
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].GetMissionRating()>Sim.Players.Players[d].GetMissionRating())
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (Sim.Date<TARGET_VALUEDAYS)
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 7602, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty2<DIFF_ADDON07)
                                 {
                                    Sim.Options.OptionLastMission2 = DIFF_ADDON07;
                                    Sim.MaxDifficulty2             = DIFF_ADDON07;
                                    Sim.Options.WriteOptions ();
                                 }
                                 hprintf ("Event: Player %li reached mission target.", c);
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 7603, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 7604, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Add-On Missionsbericht #7
               case 7700:
                  {
                     CString TmpStr, TmpStr2;

                     Sim.Players.UpdateStatistics ();

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 7701, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 7701:
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].GetMissionRating()>Sim.Players.Players[d].GetMissionRating())
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (!Sim.Players.Players[d].HasWon())
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 7702, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty2<DIFF_ADDON08)
                                 {
                                    Sim.Options.OptionLastMission2 = DIFF_ADDON08;
                                    Sim.MaxDifficulty2             = DIFF_ADDON08;
                                    Sim.Options.WriteOptions ();
                                 }
                                 hprintf ("Event: Player %li reached mission target.", c);
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 7703, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 7704, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Add-On Missionsbericht #8
               case 7800:
                  {
                     CString TmpStr, TmpStr2;

                     Sim.Players.UpdateStatistics ();

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 7801, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 7801:
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].GetMissionRating()>Sim.Players.Players[d].GetMissionRating())
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (!Sim.Players.Players[d].HasWon())
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 7802, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty2<DIFF_ADDON09)
                                 {
                                    Sim.Options.OptionLastMission2 = DIFF_ADDON09;
                                    Sim.MaxDifficulty2             = DIFF_ADDON09;
                                    Sim.Options.WriteOptions ();
                                 }
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 7803, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 7804, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Add-On Missionsbericht #9
               case 7900:
                  {
                     CString TmpStr, TmpStr2;

                     Sim.Players.UpdateStatistics ();

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 7901, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 7901:
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].GetMissionRating()>Sim.Players.Players[d].GetMissionRating())
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (!Sim.Players.Players[d].HasWon())
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 7902, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty2<DIFF_ADDON10)
                                 {
                                    Sim.Options.OptionLastMission2 = DIFF_ADDON10;
                                    Sim.MaxDifficulty2             = DIFF_ADDON10;
                                    Sim.Options.WriteOptions ();
                                 }
                                 hprintf ("Event: Player %li reached mission target.", c);
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 7903, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 7904, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;


               //Add-On Missionsbericht #10
               case 8000:
                  {
                     CString TmpStr, TmpStr2;

                     Sim.Players.UpdateStatistics ();

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 8001, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 8001:
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].GetMissionRating()>Sim.Players.Players[d].GetMissionRating())
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (!Sim.Players.Players[d].HasWon())
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 8002, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                                 Anz++;

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 8003, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 8004, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Add-On Flight Security Missionsbericht #1
               case 8500:
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 8501, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 8501:
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].GetMissionRating()>Sim.Players.Players[d].GetMissionRating())
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (!Sim.Players.Players[d].HasWon())
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 8502, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty3<DIFF_ATFS02)
                                 {
                                    Sim.Options.OptionLastMission3 = DIFF_ATFS02;
                                    Sim.MaxDifficulty3             = DIFF_ATFS02;
                                    Sim.Options.WriteOptions ();
                                 }
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 8503, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 8504, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Add-On Flight Security Missionsbericht #2
               case 8600:
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 8601, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 8601:
                  {
                     bool  bAnywon=false;
                     long  won=-1;
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (Sim.Players.Players[c].HasWon()) { bAnywon=true; won=c; }
                           if (d==-1 || Sim.Players.Players[c].GetMissionRating()>Sim.Players.Players[d].GetMissionRating())
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (bAnywon==false)
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 8602, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty3<DIFF_ATFS03)
                                 {
                                    Sim.Options.OptionLastMission3 = DIFF_ATFS03;
                                    Sim.MaxDifficulty3             = DIFF_ATFS03;
                                    Sim.Options.WriteOptions ();
                                 }
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 8603, pFontPartner, (LPCTSTR)Sim.Players.Players[won].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 8604, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Add-On Flight Security Missionsbericht #3
               case 8700:
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 8701, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 8701:
                  {
                     bool  bAnywon=false;
                     long  won=-1;
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (Sim.Players.Players[c].HasWon()) { bAnywon=true; won=c; }
                           if (d==-1 || Sim.Players.Players[c].GetMissionRating()>Sim.Players.Players[d].GetMissionRating())
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (bAnywon==false)
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 8702, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty3<DIFF_ATFS04)
                                 {
                                    Sim.Options.OptionLastMission3 = DIFF_ATFS04;
                                    Sim.MaxDifficulty3             = DIFF_ATFS04;
                                    Sim.Options.WriteOptions ();
                                 }
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 8703, pFontPartner, (LPCTSTR)Sim.Players.Players[won].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 8704, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Add-On Flight Security Missionsbericht #4
               case 8800:
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 8801, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 8801:
                  {
                     bool  bAnywon=false;
                     long  won=-1;
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (Sim.Players.Players[c].HasWon()) { bAnywon=true; won=c; }
                           if (d==-1 || Sim.Players.Players[c].GetMissionRating()>Sim.Players.Players[d].GetMissionRating())
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (bAnywon==false)
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 8802, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty3<DIFF_ATFS05)
                                 {
                                    Sim.Options.OptionLastMission3 = DIFF_ATFS05;
                                    Sim.MaxDifficulty3             = DIFF_ATFS05;
                                    Sim.Options.WriteOptions ();
                                 }
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 8803, pFontPartner, (LPCTSTR)Sim.Players.Players[won].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 8804, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Add-On Flight Security Missionsbericht #5
               case 8900:
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 8901, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 8901:
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].GetMissionRating()>Sim.Players.Players[d].GetMissionRating())
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (!Sim.Players.Players[d].HasWon())
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 8902, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty3<DIFF_ATFS06)
                                 {
                                    Sim.Options.OptionLastMission3 = DIFF_ATFS06;
                                    Sim.MaxDifficulty3             = DIFF_ATFS06;
                                    Sim.Options.WriteOptions ();
                                 }
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 8903, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 8904, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Add-On Flight Security Missionsbericht #6
               case 9000:
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 9001, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 9001:
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].GetMissionRating()>Sim.Players.Players[d].GetMissionRating())
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (!Sim.Players.Players[d].HasWon())
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 9002, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty3<DIFF_ATFS07)
                                 {
                                    Sim.Options.OptionLastMission3 = DIFF_ATFS07;
                                    Sim.MaxDifficulty3             = DIFF_ATFS07;
                                    Sim.Options.WriteOptions ();
                                 }
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 9003, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 9004, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Add-On Flight Security Missionsbericht #7
               case 9100:
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating())+"/"+Insert1000erDots (Sim.Players.Players[c].GetMissionRating(true));

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating())+"/"+Insert1000erDots (Sim.Players.Players[c].GetMissionRating(true));
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 9101, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 9101:
                  {
                     bool  bAnywon=false;
                     long  won=-1;
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (Sim.Players.Players[c].HasWon()) { bAnywon=true; won=c; }
                           if (d==-1 || Sim.Players.Players[c].GetMissionRating()>Sim.Players.Players[d].GetMissionRating())
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (bAnywon==false)
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 9102, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty3<DIFF_ATFS08)
                                 {
                                    Sim.Options.OptionLastMission3 = DIFF_ATFS08;
                                    Sim.MaxDifficulty3             = DIFF_ATFS08;
                                    Sim.Options.WriteOptions ();
                                 }
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 9103, pFontPartner, (LPCTSTR)Sim.Players.Players[won].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 9104, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Add-On Flight Security Missionsbericht #8
               case 9200:
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 9201, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 9201:
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].GetMissionRating()>Sim.Players.Players[d].GetMissionRating())
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (!Sim.Players.Players[d].HasWon())
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 9202, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty3<DIFF_ATFS09)
                                 {
                                    Sim.Options.OptionLastMission3 = DIFF_ATFS09;
                                    Sim.MaxDifficulty3             = DIFF_ATFS09;
                                    Sim.Options.WriteOptions ();
                                 }
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 9203, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 9204, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Add-On Flight Security Missionsbericht #9
               case 9300:
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 9301, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 9301:
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].GetMissionRating()>Sim.Players.Players[d].GetMissionRating())
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (Sim.Date<BTARGET_NDAYS9)
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 9302, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                              {
                                 Anz++;
                                 if (c==PlayerNum && Sim.MaxDifficulty3<DIFF_ATFS10)
                                 {
                                    Sim.Options.OptionLastMission3 = DIFF_ATFS10;
                                    Sim.MaxDifficulty3             = DIFF_ATFS10;
                                    Sim.Options.WriteOptions ();
                                 }
                              }

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 9303, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 9304, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Add-On Flight Security Missionsbericht #10
               case 9400:
                  {
                     CString TmpStr, TmpStr2;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (TmpStr.GetLength()>0) TmpStr+=", "; else TmpStr+="[[BO\\6001]]";
                           if (TmpStr2.GetLength()>0) TmpStr2+="µ";
                           TmpStr+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());

                           TmpStr2+=Sim.Players.Players[c].AirlineX+": ";
                           TmpStr2+=Insert1000erDots (Sim.Players.Players[c].GetMissionRating());
                        }

                     TmpStr+=".";
                     MakeSayWindow (0, 9401, TmpStr, pFontPartner);
                     MakeNumberWindow (TmpStr2);
                     CanCancelEmpty=TRUE;
                  }
                  break;
               case 9401:
                  {
                     SLONG c,d=-1;

                     for (c=d=0; c<Sim.Players.Players.AnzEntries(); c++)
                        if (!Sim.Players.Players[c].IsOut)
                        {
                           if (d==-1 || Sim.Players.Players[c].GetMissionRating()>Sim.Players.Players[d].GetMissionRating())
                              d=c;
                        }

                     //Missionsziel erreicht?
                     if (Sim.Date<BTARGET_NDAYS10)
                     {
                        //Nein:
                        MakeSayWindow (0, TOKEN_BOSS, 9402, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                     }
                     else
                     {
                        //Ja: Hat es einer erreicht, oder mehrere?
                        SLONG c, Anz=0;

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                              if (Sim.Players.Players[c].HasWon())
                                 Anz++;

                        if (Anz==1)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 9403, pFontPartner, (LPCTSTR)Sim.Players.Players[d].AirlineX);
                        }
                        else
                        {
                           CString tmp;

                           for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                              if (!Sim.Players.Players[c].IsOut)
                                 if (Sim.Players.Players[c].HasWon())
                                 {
                                    if (tmp.GetLength()>0) tmp+=", ";
                                    tmp+="[[*]] ";
                                    tmp+=Sim.Players.Players[c].AirlineX;
                                 }

                           MakeSayWindow (0, TOKEN_BOSS, 9404, pFontPartner, (LPCTSTR)tmp);
                        }
                     }
                  }
                  break;

               //Spieler werden jetzt besprochen:
               case 6002: case 6102: case 6202: case 6302: case 6402: case 6502: 
               case 7102: case 7202: case 7302: case 7402: case 7502: case 7602: case 7702: case 7802: case 7902: case 8002:
               case 8502: case 8602: case 8702: case 8802: case 8902: case 9002: case 9102: case 9202: case 9302: case 9402:
               case 2010: case 2011: case 2012: case 2013:
               case 20110: case 20111:
                 {
_und_jetzt_weiter_mit_etc:
                  CString TmpStr;

                  //Override um in eine andere Richtung zu schauen:
                  if (DialogPar1==0 || DialogPar1==1) ((CAufsicht*)this)->ExitFromMiddle=4;
                  if (DialogPar1==2 || DialogPar1==3)
                  {
                     ((CAufsicht*)this)->ExitFromLeft=10;
                     ((CAufsicht*)this)->ExitFromMiddle=7;
                  }

                  //"Beginnen wir mit ..." / "Und weiter mit ..."
                  TmpStr += bprintf (DialogTexte.GetS (TOKEN_BOSS, 2010+DialogPar3), (LPCSTR)Sim.Players.Players[DialogPar1].AirlineX) + Space;

                  if (Sim.Players.Players[DialogPar1].ImageGotWorse)
                  {
                     Sim.Players.Players[DialogPar1].ImageGotWorse=FALSE;
                     TmpStr += bprintf (DialogTexte.GetS (TOKEN_BOSS, 2020*10+rand()%3)) + Space;
                  }
                  else
                  {
                     if (Sim.Players.Players[DialogPar1].Image<-100)
                        TmpStr += bprintf (DialogTexte.GetS (TOKEN_BOSS, 2021*10+rand()%3)) + Space;
                     else if (Sim.Players.Players[DialogPar1].Image<-400)
                        TmpStr += bprintf (DialogTexte.GetS (TOKEN_BOSS, 2022*10+rand()%2)) + Space;
                     else if (Sim.Players.Players[DialogPar1].Image<-990)
                     {
                        TmpStr += bprintf (DialogTexte.GetS (TOKEN_BOSS, 2023*10+rand()%2)) + Space;
                     }
                     else if (Sim.Players.Players[DialogPar1].Image>500)
                        TmpStr += bprintf (DialogTexte.GetS (TOKEN_BOSS, 2026*10+rand()%5)) + Space;
                     else if (Sim.Players.Players[DialogPar1].Image>100)
                        TmpStr += bprintf (DialogTexte.GetS (TOKEN_BOSS, 2025*10+rand()%8)) + Space;
                     else
                        TmpStr += bprintf (DialogTexte.GetS (TOKEN_BOSS, 2024*10+rand()%10)) + Space;
                  }

                  MakeSayWindow (0, 12040, TmpStr, pFontPartner);
                  break;
               }

               case 12040:
               case 20100: case 20101: case 20102:
               {
                  CString TmpStr;

                  if (Sim.Players.Players[DialogPar1].Image<-990)
                  {
                     MakeSayWindow (0, TOKEN_BOSS, 2023*10+rand()%2, pFontPartner);
                  }
                  else if (Sim.Players.Players[DialogPar1].Money<DEBT_GAMEOVER)
                     MakeSayWindow (0, TOKEN_BOSS, 3000, pFontPartner, (LPCTSTR)Sim.Players.Players[DialogPar1].AirlineX);
                  else
                  {
                     //Äußerung zu den Flugzeugen:
                     for (c=tmp=tmp2=0; c<(SLONG)Sim.Players.Players[DialogPar1].Planes.AnzEntries(); c++)
                        if (Sim.Players.Players[DialogPar1].Planes.IsInAlbum(c))
                        {
                           tmp+=Sim.Players.Players[DialogPar1].Planes[c].Zustand;
                           tmp2++;
                        }
                     if (tmp2)
                     {
                        tmp/=tmp2;
                        if (tmp<40) TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2030))+Space;
                        else if (tmp<60) TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2031))+Space;
                        else if (tmp<80) TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2032))+Space;
                        else if (tmp<90) TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2033))+Space;
                        else
                        {
                           switch (rand()%3)
                           {
                              case 0:
                                 TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2034))+Space;
                                 break;

                              case 1:
                                 TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2037))+Space;
                                 break;

                              case 2:
                                 TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2038))+Space;
                                 break;
                           }
                        }

                        if (Sim.Players.Players[DialogPar1].NumFlights==0)
                           TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2036))+Space;
                     }
                     else
                        TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2035))+Space;

                     //Äußerung zum Personal:
                     /*for (c=tmp=tmp2=0; c<Workers.Workers.AnzEntries(); c++)
                        if (Workers.Workers[c].Employer==DialogPar1)
                        {
                           tmp+=Workers.Workers[c].Talent;
                           tmp2++;
                        }*/
                     for (c=tmp=tmp2=0; c<(SLONG)Sim.Players.Players[DialogPar1].Planes.AnzEntries(); c++)
                        if (Sim.Players.Players[DialogPar1].Planes.IsInAlbum(c))
                        {
                           tmp+=Sim.Players.Players[DialogPar1].Planes[c].PersonalQuality;
                           tmp2++;
                        }
                     if (tmp2)
                     {
                        tmp/=tmp2;
                        if (tmp<40) TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2040))+Space;
                        else if (tmp<60) TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2041))+Space;
                        else if (tmp<70) TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2042))+Space;
                        else if (tmp<80)
                        {
                           switch (rand()%3)
                           {
                              case 0:
                                 TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2043))+Space;
                                 break;

                              case 1:
                                 TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2046))+Space;
                                 break;

                              case 2:
                                 TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2047))+Space;
                                 break;
                           }
                        }
                        else TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2044))+Space;
                     }
                     else
                     {
                        if (DialogPar1==3)
                           TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2040+rand()%2))+Space;
                        else
                           switch (rand()%3)
                           {
                              case 0:
                                 TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2043))+Space;
                                 break;

                              case 1:
                                 TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2046))+Space;
                                 break;

                              case 2:
                                 TmpStr+=bprintf (DialogTexte.GetS (TOKEN_BOSS, 2047))+Space;
                                 break;
                           }
                     }

                     MakeSayWindow (0, 2040, TmpStr, pFontPartner);
                  }
                 }
                  break;
               case 20230: case 20231: //Image zu schlecht
                  if (DialogPar1==Sim.localPlayer)
                  {
                     MenuStart (MENU_GAMEOVER, 1);
                     /*Sim.bNoTime   = FALSE;
                     Sim.DayState  = 2;
                     StopDialog ();
                     Sim.Gamestate = GAMESTATE_BOOT; */
                     break;
                  }
                  else
                  {
                     Sim.Players.Players[DialogPar1].IsOut=TRUE;

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                     {
                        Sim.Players.Players[c].OwnsAktien[DialogPar1]=0;
                        Sim.Players.Players[DialogPar1].OwnsAktien[c]=0;
                     }
                  }
                  //absichtlich kein break, sondern nächster Spieler:

               //Nächster Spieler:
               case 2040: case 2041: case 2042: case 2043: case 2044:
               case 3002:
                  do
                  {
                     DialogPar1++;  //Nächster Spieler
                     DialogPar3++;  //Nächster Eröffnungssatz
                  }
                  while (DialogPar1<4 && Sim.Players.Players[DialogPar1].IsOut);

                  if (DialogPar1<4)
                  {
                     goto _und_jetzt_weiter_mit_etc;
                     //MakeSayWindow (0, TOKEN_BOSS, 2010+DialogPar1, pFontPartner, Sim.Players.Players[DialogPar1].AirlineX);
                  }
                  else
                  {
                     MakeSayWindow (0, TOKEN_BOSS, 2102, pFontPartner);

                     ((CAufsicht*)this)->ExitFromLeft=6;
                     ((CAufsicht*)this)->ExitFromMiddle=-1;
                     ((CAufsicht*)this)->ExitFromRight=9;
                  }
                  break;

               //Sabotage-Bericht
               case 5040:
               case 2004: DialogPar1=DialogPar2=0;
               case 2080:
_ehemals_2080:
                  for (c=0; c<DialogPar1; c++)
                     ((CAufsicht*)this)->SP_Player[c].SetDesiredMood (SPM_IDLE);

                  if (DialogPar1<Sim.SabotageActs.AnzEntries())
                  {
                     SLONG c;

                     for (c=0; c<DialogPar1; c++)
                        if (Sim.SabotageActs[c].Opfer==Sim.SabotageActs[DialogPar1].Opfer)
                        {
                           MakeSayWindow (0, TOKEN_BOSS, 2055, pFontPartner, (LPCTSTR)Sim.Players.Players[Sim.SabotageActs[DialogPar1].Opfer].AirlineX);
                           break;
                        }

                     if (c==DialogPar1)
                     {
                        if (Sim.SabotageActs[DialogPar1].Player!=-2)
                           ((CAufsicht*)this)->SP_Player[Sim.SabotageActs[DialogPar1].Player].SetDesiredMood (SPM_HOLY);
                        MakeSayWindow (0, TOKEN_BOSS, 2050+DialogPar2, pFontPartner, (LPCTSTR)Sim.Players.Players[Sim.SabotageActs[DialogPar1].Opfer].AirlineX);
                     }

                     DialogPar2++;
                     DialogPar2=min(DialogPar2,3);
                  }
                  else MakeSayWindow (0, TOKEN_BOSS, 2081, pFontPartner);

                  break;
               case 2050: case 2051: case 2052: case 2053: case 2055:
                  if (Sim.SabotageActs[DialogPar1].ArabMode<100)
                     MakeSayWindow (0, TOKEN_BOSS, 2060+Sim.SabotageActs[DialogPar1].ArabMode, pFontPartner);
                  else
                     MakeSayWindow (0, TOKEN_BOSS, Sim.SabotageActs[DialogPar1].ArabMode, pFontPartner);
                  break;
               case 2061: case 2062: case 2063: case 2064: case 2065:
               case 2075: case 2076: case 2077: case 2078:
               case 2091: case 2092: case 2093: case 2094: case 2095: case 2096:
                  ((CAufsicht*)this)->ExitFromMiddle=2; //Erhobener Zeigefinger:

                  if (Sim.SabotageActs[DialogPar1].Player==-2 || Sim.Players.Players[Sim.SabotageActs[DialogPar1].Player].ArabHints<15)
                     MakeSayWindow (0, TOKEN_BOSS, 2070, pFontPartner);
                  else if (Sim.Players.Players[Sim.SabotageActs[DialogPar1].Player].ArabHints<40)
                     MakeSayWindow (0, TOKEN_BOSS, 2071, pFontPartner);
                  else if (Sim.Players.Players[Sim.SabotageActs[DialogPar1].Player].ArabHints<100)
                     MakeSayWindow (0, TOKEN_BOSS, 2072, pFontPartner);
                  else
                     MakeSayWindow (0, TOKEN_BOSS, 2073, pFontPartner, (LPCTSTR)Sim.Players.Players[Sim.SabotageActs[DialogPar1].Player].AirlineX);
                  break;
               case 2070:
               case 2071:
               case 2072:
               case 2074:
                  Sim.SabotageActs[DialogPar1].Player=-1;
                  DialogPar1++;

                  if (DialogPar1>=Sim.SabotageActs.AnzEntries())
                  {
                     SLONG count=1000;

                     ((CAufsicht*)this)->ExitFromMiddle=12; //Alle einmal böse anblicken
                     ((CAufsicht*)this)->SP_Boss.ForceNextClip ();
                     while (((CAufsicht*)this)->SP_Boss.GetMood()!=SPM_IDLE && count>0)
                     {
                        ((CAufsicht*)this)->SP_Boss.Pump ();
                        count--;
                     }
                  }

                  goto _ehemals_2080;
                  break;
               case 2073:
                  if (DialogPar1<Sim.SabotageActs.AnzEntries())
                  {
                     MakeSayWindow (0, TOKEN_BOSS, 2074, pFontPartner, (LPCTSTR)Sim.Players.Players[Sim.SabotageActs[DialogPar1].Opfer].AirlineX);
                     MakeNumberWindow (TOKEN_BOSS, 9992074, bitoa (Sim.Players.Players[Sim.SabotageActs[DialogPar1].Player].ArabHints*10000), (LPCTSTR)Sim.Players.Players[Sim.SabotageActs[DialogPar1].Opfer].AirlineX);
                     Sim.Players.Players[Sim.SabotageActs[DialogPar1].Player].Statistiken[STAT_A_STRAFEN].AddAtPastDay (0, -Sim.Players.Players[Sim.SabotageActs[DialogPar1].Player].ArabHints*10000);
                     Sim.Players.Players[Sim.SabotageActs[DialogPar1].Player].ChangeMoney (-Sim.Players.Players[Sim.SabotageActs[DialogPar1].Player].ArabHints*10000, 2200, "");
                     Sim.Players.Players[Sim.SabotageActs[DialogPar1].Opfer].ChangeMoney (Sim.Players.Players[Sim.SabotageActs[DialogPar1].Player].ArabHints*10000, 2201, "");
                  }
                  else
                  {
                     goto _ehemals_2080;
                  }
                  break;

               //Zu wenig Geld:
               case 3000:
                  MakeSayWindow (0, TOKEN_BOSS, 3001, pFontPartner, (LPCTSTR)Sim.Players.Players[DialogPar1].AirlineX);
                  break;
               case 3001:
                  if (DialogPar1==Sim.localPlayer)
                  {
                     MenuStart (MENU_GAMEOVER, 1);
                     break;
                  }
                  else
                  {
                     Sim.Players.Players[DialogPar1].IsOut=TRUE;
                     MakeSayWindow (0, TOKEN_BOSS, 3002, pFontPartner);

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                     {
                        Sim.Players.Players[c].OwnsAktien[DialogPar1]=0;
                        Sim.Players.Players[DialogPar1].OwnsAktien[c]=0;
                     }
                  }
                  break;

               //Spieler spricht den Makker an:
               case 4000: case 4001:
                  MakeSayWindow (1, TOKEN_BOSS, 4010+(Sim.Difficulty==DIFF_FREEGAME), 4012, FALSE, &FontDialog, &FontDialogLight);
                  break;

               //Spieler stellt Antrag oder auch nicht:
               case 4011:
                  if (Airport.GetNumberOfFreeGates()!=0)
                  {
                     MakeSayWindow (0, TOKEN_BOSS, 4110, pFontPartner);
                  }
                  else
                  {
                     if (Sim.CheckIn>=5 || (Sim.CheckIn>=2 && Sim.Difficulty<=DIFF_NORMAL && Sim.Difficulty!=DIFF_FREEGAME))
                        MakeSayWindow (0, TOKEN_BOSS, 4111, pFontPartner);
                     else
                     {
                        if (Sim.Date<8)
                           MakeSayWindow (0, TOKEN_BOSS, 4114, pFontPartner); //abwarten, weil Spielbeginn
                        else if (Sim.Date-Sim.LastExpansionDate<3)
                           MakeSayWindow (0, TOKEN_BOSS, 4115, pFontPartner); //abwarten, weil gerade erst erweitert
                        else if (Sim.ExpandAirport)
                           MakeSayWindow (0, TOKEN_BOSS, 4112, pFontPartner);
                        else
                           MakeSayWindow (0, TOKEN_BOSS, 4113, pFontPartner);
                     }
                  }
                  break;
               case 4010:
                  MakeSayWindow (0, TOKEN_BOSS, 4050, pFontPartner);
                  break;
               case 4050:
                  if (Sim.Difficulty>=DIFF_ATFS)
                  {
                     MakeSayWindow (0, TOKEN_BOSS, 4084+Sim.Difficulty-DIFF_ATFS, pFontPartner);
                  }
                  else if (Sim.Difficulty>=DIFF_ADDON)
                  {
                     if (Sim.Difficulty==DIFF_ADDON03)
                        MakeSayWindow (0, TOKEN_BOSS, 4070+Sim.Difficulty-DIFF_ADDON, pFontPartner, (LPCTSTR)Cities[Sim.KrisenCity].Name);
                     else
                        MakeSayWindow (0, TOKEN_BOSS, 4070+Sim.Difficulty-DIFF_ADDON, pFontPartner);

                     if (Sim.Difficulty==DIFF_ADDON09)
                     {
                        MakeNumberWindow (TOKEN_BOSS, 4090+Sim.Difficulty-DIFF_ADDON);
                     }
                  }
                  else if (Sim.Difficulty!=DIFF_NORMAL)
                     MakeSayWindow (0, TOKEN_BOSS, 4060+Sim.Difficulty, pFontPartner);
                  else
                     MakeSayWindow (0, 4060+Sim.Difficulty, bprintf (DialogTexte.GetS (TOKEN_BOSS, 4060+Sim.Difficulty), LPCTSTR(Cities[Sim.HomeAirportId].Name), LPCTSTR(Cities[Sim.MissionCities[0]].Name), LPCTSTR(Cities[Sim.MissionCities[1]].Name), LPCTSTR(Cities[Sim.MissionCities[2]].Name), LPCTSTR(Cities[Sim.MissionCities[3]].Name), LPCTSTR(Cities[Sim.MissionCities[4]].Name), LPCTSTR(Cities[Sim.MissionCities[5]].Name)), pFontPartner);
                  break;
               case 4113:
                  MakeSayWindow (1, TOKEN_BOSS, 4120, 4121, FALSE, &FontDialog, &FontDialogLight);
                  break;
               case 4120:
                  Sim.ExpandAirport=TRUE;
                  Sim.SendSimpleMessage (ATNET_EXPAND_AIRPORT);
                  qPlayer.ChangeMoney (-1000000, 3170, "");
                  MakeSayWindow (0, TOKEN_BOSS, 4130, pFontPartner);
                  break;
               case 4121:
                  MakeSayWindow (0, TOKEN_BOSS, 4131, pFontPartner);
                  break;

               case 2102:
                  Sim.bNoTime   = FALSE;
                  Sim.DayState  = 2;
                  StopDialog ();
                  //qPlayer.LeaveRoom();
                  ((CAufsicht*)this)->TryLeaveAufsicht ();
                  break;

               case 4012: case 4130: case 4131: case 4110:
               case 4111: case 4112: case 4114: case 4115:
               case 4060: case 4061: case 4062: case 4063: case 4064:
                  if (DialogMedium==MEDIUM_HANDY)
                  {
                     MakeSayWindow (0, TOKEN_BOSS, 4999, pFontPartner);
                     break;
                  }
               case 4999:
                  Sim.bNoTime   = FALSE;
                  Sim.DayState  = 2;
                  StopDialog ();
                  break;

               //Übernamedialog:
               case 5000:
                  MakeSayWindow (0, TOKEN_BOSS, 5001, pFontPartner, (LPCTSTR)Sim.Players.Players[Sim.OvertakerAirline].AirlineX, (LPCTSTR)(Sim.Players.Players[Sim.OvertakenAirline].AirlineX));
                  break;
               case 5001:
                  MakeSayWindow (0, TOKEN_BOSS, 5000+Sim.Overtake*10, pFontPartner, (LPCTSTR)(Sim.Players.Players[Sim.OvertakerAirline].AirlineX), (LPCTSTR)(Sim.Players.Players[Sim.OvertakenAirline].AirlineX), (LPCTSTR)(Sim.Players.Players[Sim.OvertakerAirline].AirlineX));
                  break;
               case 5010: case 5020:
                  {
                     SLONG   c, d;
                     PLAYER &Overtaker=Sim.Players.Players[Sim.OvertakerAirline];
                     PLAYER &Overtaken=Sim.Players.Players[Sim.OvertakenAirline];

                     Overtaken.ArabMode  = 0;
                     Overtaken.ArabMode2 = 0;
                     Overtaken.ArabMode3 = 0;

                     Sim.ShowExtrablatt = Sim.OvertakerAirline*4 + Sim.OvertakenAirline;

                     for (c=(SLONG)Sim.Persons.AnzEntries()-1; c>=0; c--)
                        if (Sim.Persons.IsInAlbum (c))
                           if (Clans[(SLONG)Sim.Persons[c].ClanId].Type<CLAN_PLAYER1 &&
                               Sim.Persons[c].Reason==REASON_FLYING &&
                               Sim.Persons[c].FlightAirline==Sim.OvertakenAirline)
                              Sim.Persons-=c;

                     if (Sim.Overtake==1) //Schlucken
                     {
                        SLONG Piloten=0, Begleiter=0;

                        //Arbeiter übernehmen:
                        for (c=0; c<Workers.Workers.AnzEntries(); c++)
                           if (Workers.Workers[c].Employer==Sim.OvertakenAirline)
                           {
                              Workers.Workers[c].Employer=Sim.OvertakerAirline;
                              Workers.Workers[c].PlaneId=-1;
                           }

                        //Flugzeuge übernehmen, Flugpläne löschen, alle nach Berlin setzen, ggf. Leute dafür einstellen
                        for (c=0; c<(SLONG)Overtaken.Planes.AnzEntries(); c++)
                           if (Overtaken.Planes.IsInAlbum(c))
                           {
                              Overtaken.Planes[c].ClearSaldo();

                              for (d=0; d<Overtaken.Planes[c].Flugplan.Flug.AnzEntries(); d++)
                                 Overtaken.Planes[c].Flugplan.Flug[d].ObjectType=0;

                              Overtaken.Planes[c].Ort      = Sim.HomeAirportId;
                              Overtaken.Planes[c].Position = Cities[Sim.HomeAirportId].GlobusPosition;
                              Overtaken.Planes[c].Flugplan.StartCity  = Sim.HomeAirportId;
                              Overtaken.Planes[c].Flugplan.NextFlight = -1;
                              Overtaken.Planes[c].Flugplan.NextStart  = -1;

                              for (d=0; d<(SLONG)Sim.Persons.AnzEntries(); d++)
                                 if (Sim.Persons.IsInAlbum(d))
                                    if (Sim.Persons[d].FlightAirline==Sim.OvertakenAirline)
                                       Sim.Persons[d].State=PERSON_LEAVING;

                              //Piloten+=PlaneTypes[Overtaken.Planes[c].TypeId].AnzPiloten;
                              Piloten+=Overtaken.Planes[c].ptAnzPiloten;
                              Begleiter+=Overtaken.Planes[c].ptAnzBegleiter;
                              //Begleiter+=PlaneTypes[Overtaken.Planes[c].TypeId].AnzBegleiter;

                              if (Overtaker.Planes.GetNumFree()<=0)
                              {
                                 Overtaker.Planes.Planes.ReSize (Overtaker.Planes.AnzEntries()+5);
                                 Overtaker.Planes.RepairReferences();
                              }

                              d=(Overtaker.Planes+=CPlane());
                              Overtaker.Planes[d]=Overtaken.Planes[c];
                           }
                        Overtaken.Planes.Planes.ReSize (0);
                        Overtaken.Planes.RepairReferences();

                        //Ggf. virtuelle Arbeiter erzeugen:
                        if (Overtaken.Owner!=0)
                        {
                           for (c=0; c<Workers.Workers.AnzEntries(); c++)
                           {
                              if (Workers.Workers[c].Employer==WORKER_RESERVE && Workers.Workers[c].Typ==WORKER_PILOT && Piloten>0)
                              {
                                 Workers.Workers[c].Employer=Sim.OvertakerAirline;
                                 Piloten--;
                              }
                              else if (Workers.Workers[c].Employer==WORKER_RESERVE && Workers.Workers[c].Typ==WORKER_STEWARDESS && Begleiter>0)
                              {
                                 Workers.Workers[c].Employer=Sim.OvertakerAirline;
                                 Begleiter--;
                              }
                           }
                        }

                        //Das nicht Broadcasten. Das bekommen die anderen Spieler auch selber mit:
                        BOOL bOldNetwork = Sim.bNetwork;  Sim.bNetwork=false;
                        Overtaker.MapWorkers (FALSE);
                        Sim.bNetwork=bOldNetwork;

                        //Geld und Aktien übernehmen:
                        Overtaker.ChangeMoney (Overtaken.Money, 3180, "");
                        Overtaker.Credit+=Overtaken.Credit;
                        Overtaker.AnzAktien+=Overtaken.AnzAktien;
                        for (c=0; c<4; c++)
                        {
                           Overtaker.OwnsAktien[c]+=Overtaken.OwnsAktien[c];
                           Overtaker.AktienWert[c]+=Overtaken.AktienWert[c];
                        }

                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           if (!Sim.Players.Players[c].IsOut)
                           {
                              if (c!=Sim.OvertakenAirline && c!=Sim.OvertakerAirline)
                              {
                                 Sim.Players.Players[c].OwnsAktien[Sim.OvertakerAirline]+=Sim.Players.Players[c].OwnsAktien[Sim.OvertakenAirline];
                                 Sim.Players.Players[c].AktienWert[Sim.OvertakerAirline]+=Sim.Players.Players[c].AktienWert[Sim.OvertakenAirline];
                              }

                              Sim.Players.Players[c].OwnsAktien[Sim.OvertakenAirline]=0;
                              Sim.Players.Players[c].AktienWert[Sim.OvertakenAirline]=0;
                           }

                        //Von dem Übernommenen hat keiner mehr Aktien:
                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           Sim.Players.Players[c].OwnsAktien[Sim.OvertakenAirline]=0;

                        //Gates übernehmen:
                        for (c=0; c<Overtaken.Gates.Gates.AnzEntries(); c++)
                           if (Overtaken.Gates.Gates[c].Miete!=-1)
                           {
                              for (d=0; d<Overtaker.Gates.Gates.AnzEntries(); d++)
                                 if (Overtaker.Gates.Gates[d].Miete==-1)
                                 {
                                    Overtaker.Gates.Gates[d].Miete  = Overtaken.Gates.Gates[c].Miete;
                                    Overtaker.Gates.Gates[d].Nummer = Overtaken.Gates.Gates[c].Nummer;
                                    Overtaker.Gates.NumRented++;
                                    break;
                                 }

                              Overtaken.Gates.Gates[c].Miete=-1;
                           }

                        //Routen übernehmen:
                        for (c=0; c<Overtaker.RentRouten.RentRouten.AnzEntries(); c++)
                        {
                           if (Overtaker.RentRouten.RentRouten[c].Rang==0 && Overtaken.RentRouten.RentRouten[c].Rang!=0)
                           {
                              Overtaker.RentRouten.RentRouten[c]=Overtaken.RentRouten.RentRouten[c];
                              Overtaken.RentRouten.RentRouten[c].Rang=0;
                           }
                           else if (Overtaker.RentRouten.RentRouten[c].Rang!=0 && Overtaken.RentRouten.RentRouten[c].Rang!=0 && Overtaker.RentRouten.RentRouten[c].Rang>Overtaken.RentRouten.RentRouten[c].Rang)
                           {
                              for (d=0; d<Sim.Players.Players.AnzEntries(); d++)
                                 if (!Sim.Players.Players[d].IsOut && d!=Sim.OvertakerAirline && d!=Sim.OvertakenAirline &&
                                      Sim.Players.Players[d].RentRouten.RentRouten[c].Rang>Overtaken.RentRouten.RentRouten[c].Rang)
                                    Sim.Players.Players[d].RentRouten.RentRouten[c].Rang--;

                              Overtaker.RentRouten.RentRouten[c].Rang=Overtaken.RentRouten.RentRouten[c].Rang;
                              Overtaken.RentRouten.RentRouten[c].Rang=0;
                           }
                        }

                        //Städte übernehmen:
                        for (c=0; c<Overtaker.RentCities.RentCities.AnzEntries(); c++)
                        {
                           if (Overtaker.RentCities.RentCities[c].Rang==0 && Overtaken.RentCities.RentCities[c].Rang!=0)
                           {
                              Overtaker.RentCities.RentCities[c]=Overtaken.RentCities.RentCities[c];
                              Overtaken.RentCities.RentCities[c].Rang=0;
                           }
                           else if (Overtaker.RentCities.RentCities[c].Rang!=0 && Overtaken.RentCities.RentCities[c].Rang!=0 && Overtaker.RentCities.RentCities[c].Rang>Overtaken.RentCities.RentCities[c].Rang)
                           {
                              for (d=0; d<Sim.Players.Players.AnzEntries(); d++)
                                 if (!Sim.Players.Players[d].IsOut && d!=Sim.OvertakerAirline && d!=Sim.OvertakenAirline &&
                                      Sim.Players.Players[d].RentCities.RentCities[c].Rang>Overtaken.RentCities.RentCities[c].Rang)
                                    Sim.Players.Players[d].RentCities.RentCities[c].Rang--;

                              Overtaker.RentCities.RentCities[c].Rang=Overtaken.RentCities.RentCities[c].Rang;
                              Overtaken.RentCities.RentCities[c].Rang=0;
                           }
                        }
                     }
                     else if (Sim.Overtake==2) //Liquidieren
                     {
                        //Leute rauswerfen:
                        Sim.Players.Players[Sim.OvertakenAirline].SackWorkers ();

                        //Flugzeuge verkaufen:
                        for (c=0; c<(SLONG)Overtaken.Planes.AnzEntries(); c++)
                           if (Overtaken.Planes.IsInAlbum(c))
                              Overtaken.Money+=Overtaken.Planes[c].CalculatePrice();
                        Overtaken.Planes.Planes.ReSize (0);
                        Overtaken.Planes.RepairReferences();

                        //Gates freigeben:
                        for (c=0; c<Overtaken.Gates.Gates.AnzEntries(); c++)
                           if (Overtaken.Gates.Gates[c].Miete!=-1)
                              Overtaken.Gates.Gates[c].Miete=-1;

                        //Routen freigeben:
                        for (c=0; c<Overtaken.RentRouten.RentRouten.AnzEntries(); c++)
                        {
                           if (Overtaken.RentRouten.RentRouten[c].Rang!=0)
                           {
                              for (d=0; d<Sim.Players.Players.AnzEntries(); d++)
                                 if (!Sim.Players.Players[d].IsOut && d!=Sim.OvertakenAirline &&
                                      Sim.Players.Players[d].RentRouten.RentRouten[c].Rang>Overtaken.RentRouten.RentRouten[c].Rang)
                                    Sim.Players.Players[d].RentRouten.RentRouten[c].Rang--;

                              Overtaken.RentRouten.RentRouten[c].Rang=0;
                           }
                        }

                        //Cities freigeben
                        for (c=0; c<Overtaken.RentCities.RentCities.AnzEntries(); c++)
                        {
                           if (Overtaken.RentCities.RentCities[c].Rang!=0)
                           {
                              for (d=0; d<Sim.Players.Players.AnzEntries(); d++)
                                 if (!Sim.Players.Players[d].IsOut && d!=Sim.OvertakenAirline &&
                                      Sim.Players.Players[d].RentCities.RentCities[c].Rang>Overtaken.RentCities.RentCities[c].Rang)
                                    Sim.Players.Players[d].RentCities.RentCities[c].Rang--;

                              Overtaken.RentCities.RentCities[c].Rang=0;
                           }
                        }

                        //Aktien verkaufen:
                        for (c=0; c<4; c++)
                           if (Overtaken.OwnsAktien[c])
                           {
                              Overtaken.Money+=SLONG(Overtaken.OwnsAktien[c]*Sim.Players.Players[c].Kurse[0]);
                              Overtaken.OwnsAktien[c]=0;
                           }

                        //Geld verteilen
                        for (c=d=0; c<4; c++)
                           if (!Sim.Players.Players[c].IsOut && Sim.Players.Players[c].OwnsAktien[Sim.OvertakenAirline])
                              d+=Sim.Players.Players[c].OwnsAktien[Sim.OvertakenAirline];

                        for (c=0; c<4; c++)
                           if (!Sim.Players.Players[c].IsOut && Sim.Players.Players[c].OwnsAktien[Sim.OvertakenAirline])
                              Sim.Players.Players[c].ChangeMoney (__int64((Overtaken.Money-Overtaken.Credit)*(__int64)Sim.Players.Players[c].OwnsAktien[Sim.OvertakenAirline]/d), 3181, (LPCTSTR)Overtaken.AirlineX);
                              //                            Changed: ^ war SLONG und damit vermutlich für einen Bug verantwortlich

                        //Von dem Übernommenen hat keiner mehr Aktien:
                        for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
                           Sim.Players.Players[c].OwnsAktien[Sim.OvertakenAirline]=0;
                     }

                     Airport.CreateGateMapper();

                     Overtaker.UpdateStatistics();
                     Overtaken.UpdateStatistics();
                  }
                  MakeSayWindow (0, TOKEN_BOSS, 5030, pFontPartner, (LPCTSTR)(Sim.Players.Players[Sim.OvertakenAirline].AirlineX));
                  break;
               case 5030:
                  if (Sim.OvertakenAirline==Sim.localPlayer)
                  {
                     MenuStart (MENU_GAMEOVER, 1);
                     /*Sim.bNoTime   = FALSE;
                     Sim.DayState  = 2;
                     StopDialog ();
                     Sim.Gamestate = GAMESTATE_BOOT;*/
                     break;
                  }
                  else
                  {
                     Sim.Players.Players[Sim.OvertakenAirline].IsOut=TRUE;
                     //hprintf ("Event: Player %li (%s, %s) is out (overtaken)!!", DialogPar1, (LPCTSTR)Sim.Players.Players[Sim.OvertakenAirline].NameX, (LPCTSTR)Sim.Players.Players[Sim.OvertakenAirline].AirlineX);

                     for (SLONG c=0; c<Sim.Players.Players.AnzEntries(); c++)
                     {
                        Sim.Players.Players[c].OwnsAktien[Sim.OvertakenAirline]=0;
                        Sim.Players.Players[Sim.OvertakenAirline].OwnsAktien[c]=0;
                     }
                  }
                  Sim.Overtake=0;
                  if (Sim.SabotageActs.AnzEntries()>0)
                      MakeSayWindow (0, TOKEN_BOSS, 5040, pFontPartner);
                  else
                     MakeSayWindow (0, TOKEN_BOSS, 5041, pFontPartner);
                  break;

               case 10003:
               case 10004: //Items anbieten
                  StopDialog ();
                  break;

               default: //Standard: Dialog abbrechen
                  Sim.bNoTime   = FALSE;
                  Sim.DayState  = 2;
                  StopDialog ();

                  if (Sim.GetHour()==9 && Sim.GetMinute()==0)
                     ((CAufsicht*)this)->TryLeaveAufsicht ();
                  break;
            }
            break;

         case TALKER_MECHANIKER:
            switch (id)
            {
               case 1000: case 1001: case 1002:
                  if (DialogMedium) MakeSayWindow (1, TOKEN_MECH, 2100, 2102, FALSE, &FontDialog, &FontDialogLight);
                               else MakeSayWindow (1, TOKEN_MECH, 2000, 2006, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 2000: //Monolog, was er tun kann:
               case 2100: MakeSayWindow (0, TOKEN_MECH, 3000, pFontPartner); break;
               case 3000: MakeSayWindow (0, TOKEN_MECH, 3001, pFontPartner); break;
               case 3001: MakeSayWindow (0, TOKEN_MECH, 3002, pFontPartner); break;
               case 3002: MakeSayWindow (0, TOKEN_MECH, 3003, pFontPartner); break;
               case 3003: MakeSayWindow (0, TOKEN_MECH, 3004+(qPlayer.MechTrust!=2), pFontPartner); break;
               case 3004: MakeSayWindow (0, TOKEN_MECH, 3005, pFontPartner); break;
               case 3005: case 2500:
                  if (DialogMedium) MakeSayWindow (1, TOKEN_MECH, 2100, 2102, FALSE, &FontDialog, &FontDialogLight);
                               else MakeSayWindow (1, TOKEN_MECH, 2000, 2006, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 2001:
               case 2101: //Modus auswählen:
                  MakeSayWindow (0, TOKEN_MECH, 4000+qPlayer.MechMode, pFontPartner); break;
                  break;
               case 4000: case 4001: case 4002: case 4003:
               case 4020: case 4021:
                  MakeSayWindow (1, TOKEN_MECH, 4010+(qPlayer.MechTrust!=2), 4013+(qPlayer.MechTrust!=2), FALSE, &FontDialog, &FontDialogLight);
                  break;
               case 4010: case 4011: case 4012: case 4013:
                  qPlayer.MechMode=id-4010;
                  qPlayer.NetUpdatePlaneProps();
                  MakeSayWindow (0, TOKEN_MECH, 4060, pFontPartner);
                  MakeNumberWindow (TOKEN_MECH, 9994060, gRepairPrice[qPlayer.MechMode]*qPlayer.Planes.GetNumUsed()/30);
                  break;
               case 4014:
                  if (qPlayer.MechTrust==1) qPlayer.MechTrust=2;
                  if (qPlayer.MechTrust==2)
                     MakeSayWindow (0, TOKEN_MECH, 4021, pFontPartner);
                  else
                     MakeSayWindow (0, TOKEN_MECH, 4020, pFontPartner);
                  break;

               case 2002: //Kosten-Frage:
                  if (Sim.Date==0) MakeSayWindow (0, TOKEN_MECH, 2500, pFontPartner);
                              else MakeSayWindow (0, TOKEN_MECH, 2501, pFontPartner);
                  break;
               case 2003: //Kosten-Frage:
                  MenuDialogReEntryB=2502;
                  MenuStart (MENU_PLANEREPAIRS);
                  break;

               case 2501:
                  MenuDialogReEntryB=2502;
                  MenuStart (MENU_PLANECOSTS);
                  break;

               case 2004:
                  MenuDialogReEntryB=2502;
                  MenuStart (MENU_PANNEN);
                  break;

               case 2005: //Raucher-Ermahnung
                  if (qPlayer.MechAngry==0)
                     MakeSayWindow (0, TOKEN_MECH, 5000, pFontPartner);
                  else
                     MakeSayWindow (0, TOKEN_MECH, 5001, pFontPartner);
                  break;
               case 5000: case 2502:
                  qPlayer.MechAngry=1;
                  if (DialogMedium) MakeSayWindow (1, TOKEN_MECH, 2100, 2102, FALSE, &FontDialog, &FontDialogLight);
                               else MakeSayWindow (1, TOKEN_MECH, 2000, 2006, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 5001: case 5002: case 5003:
                  qPlayer.MechAngry=2;
                  if (DialogMedium==MEDIUM_AIR) qPlayer.LeaveRoom();
                  StopDialog ();
                  break;

               default: //Standard: Dialog abbrechen
                  StopDialog ();
                  break;
            }
            break;

         case TALKER_MAKLER:
            switch (id)
            {
               case 80: case 81: case 83:
               case 110:
               case 150:
               //case 160:
               case 170:
                  if (DialogMedium==MEDIUM_HANDY)
                  {
                     StopDialog ();
                  }
                  else MakeSayWindow (1, TOKEN_MAKLER, 100, 103, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 82:
                  MakeSayWindow (1, TOKEN_MAKLER, 1100, 1103, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 1100: //Will Flugzeug per Handy kaufen:
                  MakeSayWindow (0, TOKEN_MAKLER, 1200, pFontPartner);
                  break;

               case 100: //Will Flugzeug kaufen:
                  MakeSayWindow (0, TOKEN_MAKLER, 120+DialogMedium, pFontPartner);
                  break;
               case 120:
                  MenuDialogReEntryB=-1;
                  MenuStart (MENU_BUYPLANE);
                  break;

               case 101: //Will Flugzeug verkaufen:
               case 1101:
                  MakeSayWindow (0, TOKEN_MAKLER, 110, pFontPartner);
                  break;

               case 102: //Wann wird geliefert:
               case 1102:
                  MakeSayWindow (0, TOKEN_MAKLER, 170, pFontPartner);
                  break;

               case 130: //Spieler sagt jetzt Zahl der Flugzeuge:
               case 131:
               case 132:
               case 133:
                  tmp = PlaneTypes [DialogPar2].Preis;

                  if (qPlayer.Money-tmp<DEBT_LIMIT)
                  {
                     MakeSayWindow (0, TOKEN_MAKLER, 6000, pFontPartner);
                     break;
                  }

                  for (c=4; c>=0; c--)
                     if (qPlayer.Money-tmp*"\x1\x2\x3\x5\xa"[c]>=DEBT_LIMIT)
                     {
                        MakeSayWindow (1, TOKEN_MAKLER, 140, 141+c, TRUE, &FontDialog, &FontDialogLight, "", (LPCTSTR)Insert1000erDots (tmp), (LPCTSTR)Insert1000erDots (tmp*2), (LPCTSTR)Insert1000erDots (tmp*3), (LPCTSTR)Insert1000erDots (tmp*5), (LPCTSTR)Insert1000erDots (tmp*10));
                        break;
                     }
                  break;

               case 140: //Spieler kauft doch nicht
                  MakeSayWindow (0, TOKEN_MAKLER, 160, pFontPartner);
                  break;

               case 141:
               case 142:
               case 143:
               case 144:
               case 145:
                  if (qPlayer.Money-PlaneTypes [DialogPar2].Preis*("\x1\x2\x3\x5\xa"[id-141])<DEBT_LIMIT)
                     MakeSayWindow (0, TOKEN_MAKLER, 6000, pFontPartner);
                  else
                  {
                     TEAKRAND rnd;
                     SLONG    Anzahl = "\x1\x2\x3\x5\xa"[id-141];
                     SLONG    Type   = DialogPar2-0x10000000;

                     rnd.SRand (Sim.Date);

                     MakeSayWindow (0, TOKEN_MAKLER, 150, pFontPartner);
                     for (c=0; c<Anzahl; c++)
                        qPlayer.BuyPlane (Type, &rnd);

                     Sim.SendSimpleMessage (ATNET_BUY_NEW, NULL, PlayerNum, Anzahl, Type);

                     qPlayer.DoBodyguardRabatt (PlaneTypes [DialogPar2].Preis*("\x1\x2\x3\x5\xa"[id-141]));
                     qPlayer.MapWorkers (FALSE);
                     qPlayer.UpdatePersonalberater (1);
                  }
                  break;

               case (SLONG)0xffffffff:
                  break;

               default: //Standard: Dialog abbrechen
                  StopDialog ();
                  break;
            }
            break;

         case TALKER_MUSEUM:
            switch (id)
            {
               case 102:
               case 100: //Begrüssung-Spruch
                  MakeSayWindow (1, TOKEN_MUSEUM, 201+((Sim.DialogOvertureFlags&DIALOG_MUSEUM2)!=0)*10, 203+((Sim.DialogOvertureFlags&DIALOG_MUSEUM2)!=0)*10, FALSE, &FontDialog, &FontDialogLight);
                  break;
               case 101: //Begrüssung-Spruch
                  MakeSayWindow (1, TOKEN_MUSEUM, 201+((Sim.DialogOvertureFlags&DIALOG_MUSEUM2)!=0)*10, 203+((Sim.DialogOvertureFlags&DIALOG_MUSEUM2)!=0)*10, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 200: //Info zu Doppeldecker:
                  MakeSayWindow (0, TOKEN_MUSEUM, 300, pFontPartner);
                  break;

               case 201: //Flugzeuge zu verkaufen?
                  if (DialogMedium==MEDIUM_HANDY)
                     MakeSayWindow (0, TOKEN_MUSEUM, 401, pFontPartner);
                  else
                     MakeSayWindow (0, TOKEN_MUSEUM, 400, pFontPartner);
                  break;

               case 211: //Flugzeuge zu verkaufen?
                  if (DialogMedium==MEDIUM_HANDY)
                     MakeSayWindow (0, TOKEN_MUSEUM, 403, pFontPartner);
                  else
                     MakeSayWindow (0, TOKEN_MUSEUM, 402, pFontPartner);
                  break;


               case 202: //Flugzeuge kaufen?
                  MakeSayWindow (0, TOKEN_MUSEUM, 700, pFontPartner);
                  break;

               case 212: //Flugzeuge kaufen?
                  MakeSayWindow (0, TOKEN_MUSEUM, 701, pFontPartner);
                  break;

               case 500: //Nehmen wir das Flugzeug jetzt mit oder nicht?
                  MakeSayWindow (1, TOKEN_MUSEUM, 600, 601, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 600:
                  if (qPlayer.Money-Sim.UsedPlanes[0x1000000+DialogPar1].CalculatePrice()<DEBT_LIMIT)
                     MakeSayWindow (0, TOKEN_MUSEUM, 6000, pFontPartner);
                  else
                  {
                     if (qPlayer.Planes.GetNumFree()==0)
                     {
                        qPlayer.Planes.Planes.ReSize (qPlayer.Planes.AnzEntries()+10);
                        qPlayer.Planes.RepairReferences();
                     }
                     Sim.UsedPlanes[0x1000000+DialogPar1].WorstZustand = UBYTE(Sim.UsedPlanes[0x1000000+DialogPar1].Zustand-20);
                     Sim.UsedPlanes[0x1000000+DialogPar1].GlobeAngle   = 0;
                     qPlayer.Planes += Sim.UsedPlanes[0x1000000+DialogPar1];

                     long Kosten = -Sim.UsedPlanes[0x1000000+DialogPar1].CalculatePrice();
                     qPlayer.ChangeMoney (Kosten, 2010, Sim.UsedPlanes[0x1000000+DialogPar1].Name);
                     Sim.SendSimpleMessage (ATNET_CHANGEMONEY, NULL, Sim.localPlayer, Kosten, STAT_A_SONSTIGES);
                     qPlayer.Statistiken[STAT_A_SONSTIGES].AddAtPastDay (0, Kosten);

                     qPlayer.DoBodyguardRabatt (Sim.UsedPlanes[0x1000000+DialogPar1].CalculatePrice());

                     if (Sim.bNetwork)
                     {
                        Sim.SendSimpleMessage (ATNET_ADVISOR, NULL, 1, PlayerNum, DialogPar1);
                        Sim.SendSimpleMessage (ATNET_BUY_USED, NULL, PlayerNum, DialogPar1, Sim.Time);
                     }

                     Sim.UsedPlanes[0x1000000+DialogPar1].Name.Empty();
                     Sim.TickMuseumRefill=0;

                     StopDialog ();
                     qPlayer.MapWorkers (FALSE);
                     qPlayer.UpdatePersonalberater (1);
                  }
                  break;

               //In den Verkaufsmodus schalten:
               case 700: case 701:
                  if (DialogMedium==MEDIUM_HANDY)
                     MakeSayWindow (1, TOKEN_MUSEUM, 201+((Sim.DialogOvertureFlags&DIALOG_MUSEUM2)!=0)*10, 203+((Sim.DialogOvertureFlags&DIALOG_MUSEUM2)!=0)*10, FALSE, &FontDialog, &FontDialogLight);
                  else
                  {
                     MenuDialogReEntryB=699;
                     MenuStart (MENU_SELLPLANE);
                  }
                  break;

               //Willigt der Spieler beim Verkauf ein?
               case 710: case 712:
                  MakeSayWindow (1, TOKEN_MUSEUM, 720, 721, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 720:
                  {
                     SLONG preis = qPlayer.Planes[DialogPar2].CalculatePrice()*9/10;

                     qPlayer.Statistiken[STAT_E_SONSTIGES].AddAtPastDay (0, preis);
                     if (PlayerNum==Sim.localPlayer) Sim.SendSimpleMessage (ATNET_CHANGEMONEY, NULL, Sim.localPlayer, preis, STAT_E_SONSTIGES);

                     qPlayer.ChangeMoney (
                        preis,
                        2011,                //Verkauf des Flugzeuges
                        qPlayer.Planes[DialogPar2].Name);

                     qPlayer.Planes -= DialogPar2;
                     Sim.SendSimpleMessage (ATNET_SELL_USED, NULL, PlayerNum, DialogPar2);
                     qPlayer.NetSynchronizeMoney ();

                     StopDialog ();
                     qPlayer.MapWorkers (FALSE);

                     if (!qPlayer.Planes.IsInAlbum (qPlayer.ReferencePlane))
                     {
                        qPlayer.ReferencePlane=-1;
                     }
                     qPlayer.UpdatePersonalberater (1);
                  }
                  break;

               case 711: //Flugzeug wird verwendet und kann gar nicht verkauft werden.
               default: //Standard: Dialog abbrechen
                  StopDialog ();
                  break;
            }
            break;

         case TALKER_PERSONAL1a:
         case TALKER_PERSONAL2a:
         case TALKER_PERSONAL3a:
         case TALKER_PERSONAL4a:
         case TALKER_PERSONAL1b:
         case TALKER_PERSONAL2b:
         case TALKER_PERSONAL3b:
         case TALKER_PERSONAL4b:
            switch (id)
            {
               case 99:
                  MakeSayWindow (0, TOKEN_JOBS, 100, pFontPartner);
                  break;

               case 100: case 102:
                  {
                     SLONG c, n1=0, n2=0, n3=0;

                     for (c=0; c<Workers.Workers.AnzEntries(); c++)
                        if (Workers.Workers[c].Employer==WORKER_JOBLESS)
                        {
                           if (Workers.Workers[c].Typ==WORKER_PILOT) n2++;
                           else if (Workers.Workers[c].Typ==WORKER_STEWARDESS) n3++;
                           else n1++;
                        }

                     MakeSayWindow (1, TOKEN_JOBS, 200, 203, TRUE, &FontDialog, &FontDialogLight, n1, n2, n3);
                  }
                  break;

               case 101: case 203:
                  StopDialog ();
                  break;

               case 200:
                  MenuStart (MENU_PERSONAL, BERATERTYP_GIRL, 1);
                  MenuSetZoomStuff (XY(291,279), 0.1, FALSE);
                  break;
               case 201:
                  MenuStart (MENU_PERSONAL, WORKER_PILOT, 1);
                  MenuSetZoomStuff (XY(291,279), 0.1, FALSE);
                  break;
               case 202:
                  MenuStart (MENU_PERSONAL, WORKER_STEWARDESS, 1);
                  MenuSetZoomStuff (XY(291,279), 0.1, FALSE);
                  break;

               case 250:
                  {
                     SLONG c, n1=0, n2=0, n3=0;

                     for (c=0; c<Workers.Workers.AnzEntries(); c++)
                        if (Workers.Workers[c].Employer==WORKER_JOBLESS)
                        {
                           if (Workers.Workers[c].Typ==WORKER_PILOT) n2++;
                           else if (Workers.Workers[c].Typ==WORKER_STEWARDESS) n3++;
                           else n1++;
                        }

                     MakeSayWindow (1, TOKEN_JOBS, 200, 203, TRUE, &FontDialog, &FontDialogLight, LPCTSTR(CString(bitoa(n1))), LPCTSTR(CString(bitoa(n2))), LPCTSTR(CString(bitoa(n3))));
                  }
                  break;

               case 300:
                  qPlayer.SeligTrust=TRUE;
                  StopDialog ();
                  break;
               case 301:
                  if (qPlayer.HasSpaceForItem())
                     qPlayer.BuyItem (ITEM_TABLETTEN);
                  StopDialog ();
                  break;

               case 499:
                  MakeSayWindow (0, TOKEN_JOBS, 500, pFontPartner);
                  break;

               case 500: case 501: case 502: case 503:
                  MakeSayWindow (1, TOKEN_JOBS, 600, 605, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 600:
                  MakeSayWindow (0, TOKEN_JOBS, 700, pFontPartner);
                  Workers.Gehaltsaenderung (1, PlayerNum);
                  qPlayer.StrikePlanned = FALSE;

                  while ((Workers.GetAverageHappyness(Sim.localPlayer)-(Workers.GetMinHappyness(Sim.localPlayer)<0)*10<20) || (Workers.GetAverageHappyness(Sim.localPlayer)-(Workers.GetMinHappyness(Sim.localPlayer)<0)*10<0))
                     Workers.AddHappiness (PlayerNum, 10);
                  break;

               case 601:
                  MakeSayWindow (0, TOKEN_JOBS, 800, pFontPartner);
                  break;

               case 602:
                  if (DialogMedium==MEDIUM_HANDY) MakeSayWindow (0, TOKEN_JOBS, 900, pFontPartner);
                  else
                  {
                     //MenuDialogReEntryB = 502;
                     MenuStart (MENU_PERSONAL, BERATERTYP_GIRL, 2);
                     MenuSetZoomStuff (XY(405,261), 0.1, FALSE);
                  }
                  break;

               case 603:
                  if (DialogMedium==MEDIUM_HANDY) MakeSayWindow (0, TOKEN_JOBS, 900, pFontPartner);
                  else
                  {
                     //MenuDialogReEntryB = 502;
                     MenuStart (MENU_PERSONAL, WORKER_PILOT, 2);
                     MenuSetZoomStuff (XY(405,261), 0.1, FALSE);
                  }
                  break;

               case 604:
                  if (DialogMedium==MEDIUM_HANDY) MakeSayWindow (0, TOKEN_JOBS, 900, pFontPartner);
                  else
                  {
                     //MenuDialogReEntryB = 502;
                     MenuStart (MENU_PERSONAL, WORKER_STEWARDESS, 2);
                     MenuSetZoomStuff (XY(405,261), 0.1, FALSE);
                  }
                  break;

               case 700:
                  MakeSayWindow (0, TOKEN_JOBS, 820, pFontPartner);
                  break;

               case 800:
                  MakeSayWindow (1, TOKEN_JOBS, 810, 811, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 810:
                  Workers.Gehaltsaenderung (0, PlayerNum);
                  MakeSayWindow (0, TOKEN_JOBS, 820, pFontPartner);
                  break;

               case 811:
                  MakeSayWindow (0, TOKEN_JOBS, 820, pFontPartner);
                  break;

               case 820:
                  MakeSayWindow (1, TOKEN_JOBS, 600, 605, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 950:
                  MakeSayWindow (1, TOKEN_JOBS, 960, 962, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 960:
                  qPlayer.StrikeNotified = FALSE;   //Dem Spieler bei nächster Gelegenheit bescheid sagen
                  qPlayer.StrikeEndType  = 2;       //Streik beendet durch Gehaltserhöhunh
                  qPlayer.StrikeEndCountdown = 2;
                  Workers.Gehaltsaenderung (1, PlayerNum);
                  MakeSayWindow (0, TOKEN_JOBS, 970, pFontPartner);

                  while ((Workers.GetAverageHappyness(Sim.localPlayer)-(Workers.GetMinHappyness(Sim.localPlayer)<0)*10<20) || (Workers.GetAverageHappyness(Sim.localPlayer)-(Workers.GetMinHappyness(Sim.localPlayer)<0)*10<0))
                     Workers.AddHappiness (PlayerNum, 10);
                  break;

               case 961:
                  qPlayer.StrikeNotified = FALSE;   //Dem Spieler bei nächster Gelegenheit bescheid sagen
                  qPlayer.StrikeEndType  = 1;       //Streik beendet durch Drohung
                  qPlayer.StrikeEndCountdown = 4;
                  Workers.AddHappiness (PlayerNum, -20);
                  MakeSayWindow (0, TOKEN_JOBS, 971, pFontPartner);
                  break;

               case 962:
                  MakeSayWindow (1, TOKEN_JOBS, 600, 605, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 970: case 971:
                  MakeSayWindow (0, TOKEN_JOBS, 502, pFontPartner);
                  break;

               default:
                  StopDialog ();
                  break;
            }
            break;

#ifndef DEMO
         case TALKER_WERBUNG:
            switch (id)
            {
               case 1000: case 1001: case 1002: case 1003:
                  MakeSayWindow (1, TOKEN_WERBUNG, 2000, 2004, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 8001:
                  qPlayer.WerbungTrust=TRUE;
                  StopDialog ();
                  break;
               case 8002:
                  if (qPlayer.HasSpaceForItem())
                     qPlayer.BuyItem (ITEM_DISKETTE);
                  StopDialog ();
                  break;

               case 2000: //Spieler will Image wissen:
                  Limit (SLONG(-1000), qPlayer.Image, SLONG(1000));

                  if (qPlayer.Image==0)
                     MakeSayWindow (0, TOKEN_WERBUNG, 2520, pFontPartner, (LPCTSTR)qPlayer.AirlineX);
                  else if (qPlayer.Image<0)
                  {
                     MakeSayWindow (0, TOKEN_WERBUNG, 2503+qPlayer.Image/251, pFontPartner, (LPCTSTR)qPlayer.AirlineX);
                     MakeNumberWindow (TOKEN_WERBUNG, 9992050, -qPlayer.Image);
                  }
                  else
                  {
                     MakeSayWindow (0, TOKEN_WERBUNG, 2504+qPlayer.Image/167, pFontPartner, (LPCTSTR)qPlayer.AirlineX);
                     MakeNumberWindow (TOKEN_WERBUNG, 9992050, qPlayer.Image);
                  }
                  break;
               case 2500: case 2501: case 2502: case 2503: case 2504:
               case 2505: case 2506: case 2507: case 2508: case 2509: case 2520:
                  MakeSayWindow (1, TOKEN_WERBUNG, 2001, 2004, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 2001: //Firmenimage:
                  DialogPar1=0;
                  MakeSayWindow (0, TOKEN_WERBUNG, 3000, pFontPartner);
                  break;
               case 2002: //Routen-Image:
                  DialogPar1=1;
                  MakeSayWindow (0, TOKEN_WERBUNG, 3500, pFontPartner);
                  break;
               case 2003: //Alles:
                  DialogPar1=2;
                  MakeSayWindow (0, TOKEN_WERBUNG, 3000, pFontPartner);
                  break;

               case 3400: case 3401:
                  MakeSayWindow (1, TOKEN_WERBUNG, 2000, 2004, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 3500:
                  MenuDialogReEntryB=3400;
                  MenuStart (MENU_ADROUTE);
                  break;
               case 3501: //Alles:
                  DialogPar1=1;
                  MakeSayWindow (0, TOKEN_WERBUNG, 3001, pFontPartner);
                  break;

               case 3000:
                  MakeSayWindow (0, TOKEN_WERBUNG, 3001, pFontPartner);
                  break;
               case 3001:
                  MakeSayWindow (1, TOKEN_WERBUNG, 4000, 4006, TRUE, &FontDialog, &FontDialogLight, (LPCTSTR)Insert1000erDots (gWerbePrice[DialogPar1*6+0]), (LPCTSTR)Insert1000erDots (gWerbePrice[DialogPar1*6+1]), (LPCTSTR)Insert1000erDots (gWerbePrice[DialogPar1*6+2]), (LPCTSTR)Insert1000erDots (gWerbePrice[DialogPar1*6+3]), (LPCTSTR)Insert1000erDots (gWerbePrice[DialogPar1*6+4]), (LPCTSTR)Insert1000erDots (gWerbePrice[DialogPar1*6+5]));
                  break;

               case 4000: case 4001: case 4002: case 4003: case 4004: case 4005:
                  MakeSayWindow (0, TOKEN_WERBUNG, id-4000+5000, pFontPartner);
                  break;

               case 5000: case 5001: case 5002: case 5003: case 5004: case 5005:
                  if (qPlayer.Money-gWerbePrice[DialogPar1*6+id-5000]<DEBT_LIMIT)
                     MakeSayWindow (0, TOKEN_WERBUNG, 6000, pFontPartner);
                  else
                  {
                     if (DialogPar1==0)
                     {
                        qPlayer.Image+=gWerbePrice[DialogPar1*6+id-5000]/10000*(id-5000+6)/55;
                        Limit (SLONG(-1000), qPlayer.Image, SLONG(1000));

                        if (id==5000)
                           for (c=0; c<Sim.Players.AnzPlayers; c++)
                              if (!Sim.Players.Players[c].Owner && !Sim.Players.Players[c].IsOut)
                                 Sim.Players.Players[c].Letters.AddLetter (
                                    TRUE,
                                    (CString)bprintf (StandardTexte.GetS (TOKEN_LETTER, 9900), (LPCTSTR)qPlayer.AirlineX),
                                    (CString)bprintf (StandardTexte.GetS (TOKEN_LETTER, 9901), (LPCTSTR)qPlayer.AirlineX),
                                    (CString)bprintf (StandardTexte.GetS (TOKEN_LETTER, 9902), (LPCTSTR)qPlayer.NameX, (LPCTSTR)qPlayer.AirlineX),
                                    -1);
                     }
                     else if (DialogPar1==1)
                     {
                        qPlayer.RentRouten.RentRouten[DialogPar2].Image+=UBYTE(gWerbePrice[DialogPar1*6+id-5000]/30000);
                        Limit ((UBYTE)0, qPlayer.RentRouten.RentRouten[DialogPar2].Image, (UBYTE)100);

                        for (SLONG c=qPlayer.RentRouten.RentRouten.AnzEntries()-1; c>=0; c--)
                           if (Routen.IsInAlbum(c))
                              if (Routen[c].VonCity==Routen[DialogPar2].NachCity && Routen[c].NachCity==Routen[DialogPar2].VonCity)
                              {
                                 qPlayer.RentRouten.RentRouten[c].Image+=UBYTE(gWerbePrice[DialogPar1*6+id-5000]/30000);
                                 Limit ((UBYTE)0, qPlayer.RentRouten.RentRouten[c].Image, (UBYTE)100);
                                 break;
                              }

                        if (id==5000)
                           for (c=0; c<Sim.Players.AnzPlayers; c++)
                              if (!Sim.Players.Players[c].Owner && !Sim.Players.Players[c].IsOut)
                                 Sim.Players.Players[c].Letters.AddLetter (
                                    TRUE,
                                    (CString)bprintf (StandardTexte.GetS (TOKEN_LETTER, 9910), (LPCTSTR)Cities[Routen[DialogPar2].VonCity].Name, (LPCTSTR)Cities[Routen[DialogPar2].NachCity].Name, (LPCTSTR)qPlayer.AirlineX),
                                    (CString)bprintf (StandardTexte.GetS (TOKEN_LETTER, 9911), (LPCTSTR)Cities[Routen[DialogPar2].VonCity].Name, (LPCTSTR)Cities[Routen[DialogPar2].NachCity].Name, (LPCTSTR)qPlayer.AirlineX),
                                    (CString)bprintf (StandardTexte.GetS (TOKEN_LETTER, 9912), (LPCTSTR)qPlayer.NameX, (LPCTSTR)qPlayer.AirlineX),
                                    -1);
                     }
                     else if (DialogPar1==2)
                     {
                        qPlayer.Image+=gWerbePrice[DialogPar1*6+id-5000]/15000*(id-5000+6)/55;
                        Limit (SLONG(-1000), qPlayer.Image, SLONG(1000));

                        for (c=0; c<qPlayer.RentRouten.RentRouten.AnzEntries(); c++)
                           if (qPlayer.RentRouten.RentRouten[c].Rang)
                           {
                              qPlayer.RentRouten.RentRouten[c].Image+=UBYTE(gWerbePrice[DialogPar1*6+id-5000]*(id-5000+6)/6/120000);
                              Limit ((UBYTE)0, qPlayer.RentRouten.RentRouten[c].Image, (UBYTE)100);
                           }

                        if (id==5000)
                           for (c=0; c<Sim.Players.AnzPlayers; c++)
                              if (!Sim.Players.Players[c].Owner && !Sim.Players.Players[c].IsOut)
                                 Sim.Players.Players[c].Letters.AddLetter (
                                    TRUE,
                                    (CString)bprintf (StandardTexte.GetS (TOKEN_LETTER, 9920), (LPCTSTR)qPlayer.AirlineX),
                                    (CString)bprintf (StandardTexte.GetS (TOKEN_LETTER, 9921), (LPCTSTR)qPlayer.AirlineX),
                                    (CString)bprintf (StandardTexte.GetS (TOKEN_LETTER, 9922), (LPCTSTR)qPlayer.NameX, (LPCTSTR)qPlayer.AirlineX),
                                    -1);
                     }

                     long preis = -gWerbePrice[DialogPar1*6+id-5000];
                     qPlayer.Statistiken[STAT_A_SONSTIGES].AddAtPastDay (0, preis);
                     qPlayer.ChangeMoney (preis, id-5000+3120, "");
                     if (PlayerNum==Sim.localPlayer) Sim.SendSimpleMessage (ATNET_CHANGEMONEY, NULL, Sim.localPlayer, preis, STAT_A_SONSTIGES);

                     qPlayer.NetSynchronizeImage();
                     qPlayer.DoBodyguardRabatt (gWerbePrice[DialogPar1*6+id-5000]);
                     MakeSayWindow (0, TOKEN_WERBUNG, 3401, pFontPartner);
                  }
                  break;

               default: //Standard: Dialog abbrechen
                  StopDialog ();
                  break;
            }
            break;
#endif

         case TALKER_DUTYFREE:
            switch (id)
            {
               case 1000:
                  MakeSayWindow (1, TOKEN_DUTYFREE, 1001, 1001, TRUE, &FontDialog, &FontDialogLight);
                  break;

               case 801:
                  qPlayer.DutyTrust=TRUE;
                  StopDialog ();
                  break;

               case 802:
                  if (qPlayer.HasSpaceForItem())
                     qPlayer.BuyItem (ITEM_HUFEISEN);
                  StopDialog ();
                  break;

               case 1001:
                  {
                     CString tmp;

                     tmp=DialogTexte.GetS (TOKEN_DUTYFREE, 1002);
                     if (Sim.LaptopSoldTo!=-1) tmp+=CString(" ")+DialogTexte.GetS (TOKEN_DUTYFREE, 1003);
                     tmp+=CString(" ")+DialogTexte.GetS (TOKEN_DUTYFREE, 1004);

                     MakeSayWindow (0, 100, tmp, pFontPartner);
                  }
                  break;

               case 2000:
                  MakeSayWindow (0, TOKEN_DUTYFREE, 2001, pFontPartner);
                  break;

               case 3000:
                  MakeSayWindow (1, TOKEN_DUTYFREE, 3001, 3003, TRUE, &FontDialog, &FontDialogLight);
                  break;

               case 3001:
                  StopDialog ();
                  if (qPlayer.HasSpaceForItem())
                  {
                     long preis = -atoi(StandardTexte.GetS (TOKEN_ITEM, 2801));

                     qPlayer.BuyItem(ITEM_PRALINEN_A);

                     qPlayer.ChangeMoney (
                           preis,
                           9999,                //Leerstring
                           StandardTexte.GetS (TOKEN_ITEM, 1801));

                     qPlayer.Statistiken[STAT_A_SONSTIGES].AddAtPastDay (0, preis);
                     if (PlayerNum==Sim.localPlayer) Sim.SendSimpleMessage (ATNET_CHANGEMONEY, NULL, Sim.localPlayer, preis, STAT_A_SONSTIGES);

                     qPlayer.DoBodyguardRabatt (-preis);
                  }
                  break;

               case 3002:
                  StopDialog ();
                  if (qPlayer.HasSpaceForItem())
                  {
                     long preis = -atoi(StandardTexte.GetS (TOKEN_ITEM, 2801));

                     qPlayer.BuyItem(ITEM_PRALINEN);

                     qPlayer.ChangeMoney (
                           preis,
                           9999,                //Leerstring
                           StandardTexte.GetS (TOKEN_ITEM, 1801));
                     qPlayer.Statistiken[STAT_A_SONSTIGES].AddAtPastDay (0, preis);
                     if (PlayerNum==Sim.localPlayer) Sim.SendSimpleMessage (ATNET_CHANGEMONEY, NULL, Sim.localPlayer, preis, STAT_A_SONSTIGES);

                     qPlayer.DoBodyguardRabatt (-preis);
                  }
                  break;

               case 2001:
                  {
                     qPlayer.LaptopVirus=2;

                     for (SLONG c=0; c<6; c++)
                        if (qPlayer.Items[c]==ITEM_LAPTOP)
                           qPlayer.Items[c]=0xff;

                     qPlayer.ReformIcons();
                  }
                  StopDialog ();
                  break;

               case 2020:
                  qPlayer.LaptopVirus=0;
                  qPlayer.BuyItem(ITEM_LAPTOP);
                  qPlayer.ReformIcons();
                  StopDialog ();
                  break;

               default: //Standard: Dialog abbrechen
                  StopDialog ();
                  break;
            }
            break;

         case TALKER_PASSAGIER:
            switch (id)
            {
               case 1000:
               default: //Standard: Dialog abbrechen
                  StopDialog ();
                  break;
            }
            break;

         case TALKER_WORLD:
            switch (id)
            {
               case 1000:
                  MakeSayWindow (1, TOKEN_WELT, 2000, 2000, FALSE, &FontDialog, &FontDialogLight, (LPCTSTR)qPlayer.AirlineX);
                  break;

               case 2000:
                  MakeSayWindow (1, TOKEN_WELT, 3000, 3002, TRUE, &FontDialog, &FontDialogLight, (LPCTSTR)Cities[DialogPar2].Name, (LPCTSTR)Cities[DialogPar2].Name, NULL);
                  break;

               case 5000:
                  qPlayer.CalledCities[DialogPar2]=1;
                  MakeSayWindow (1, TOKEN_WELT, 3001, 3002, TRUE, &FontDialog, &FontDialogLight, (LPCTSTR)Cities[DialogPar2].Name, (LPCTSTR)Cities[DialogPar2].Name, NULL);
                  break;

               case 3000:
                  MakeSayWindow (0, TOKEN_WELT, 4000+(Sim.Date+DialogPar2)%4, pFontPartner);
                  break;

               case 4000: case 4001: case 4002: case 4003:
                  MakeSayWindow (1, TOKEN_WELT, 3001, 3002, TRUE, &FontDialog, &FontDialogLight, (LPCTSTR)Cities[DialogPar2].Name, (LPCTSTR)Cities[DialogPar2].Name, NULL);
                  break;

               case 3001:
                  //MenuDialogReEntryB=5000;
                  MenuStart (MENU_AUSLANDSAUFTRAG, DialogPar2);
                  MenuSetZoomStuff (XY(480,220), 0.1, FALSE);
                  break;

               case 3002:
                  StopDialog ();
                  MenuStart (MENU_FILOFAX, 2);
                  CurrentMenu = MENU_BRANCHLIST;
                  MenuPage=0;
                  MenuRepaint();
                  break;

               default: //Standard: Dialog abbrechen
                  StopDialog ();
                  break;
            }
            break;

         case TALKER_COMPETITOR:
            {
               if (Sim.Players.Players[DialogPar1].Owner==2)
               {
                  Sim.SendSimpleMessage (ATNET_DIALOG_SAY,
                                         Sim.Players.Players[DialogPar1].NetworkID,
                                         id);
               }

               PLAYER &qOther   = Sim.Players.Players[DialogPar1];
               bool    bIsRobot = (qOther.Owner==1);

               switch (id)
               {
                  case 1000:
                     DialogPar2^=1;
                     if (DialogPar2)
                        MakeSayWindow (1, TOKEN_PLAYER, 1200, 1200, FALSE, &FontDialog, &FontDialogLight, (LPCTSTR)qPlayer.AirlineX);
                     else
                        MakeSayWindow (0, TOKEN_PLAYER, 1200, pFontPartner, (LPCTSTR)qOther.AirlineX);
                     break;

                  case 1200:
                     DialogPar2^=1;
                     if (DialogPar2)
                        MakeSayWindow (1, TOKEN_PLAYER, 1600, 1602, FALSE, &FontDialog, &FontDialogLight);
                     else if (bIsRobot)
                     {
                        if (qOther.Sympathie[Sim.localPlayer]<-400) MakeSayWindow (0, TOKEN_PLAYER, 1602, pFontPartner);
                        else if (qOther.Sympathie[Sim.localPlayer]<-100) MakeSayWindow (0, TOKEN_PLAYER, 1601, pFontPartner);
                        else MakeSayWindow (0, TOKEN_PLAYER, 1600, pFontPartner);
                     }
                     else if (point!=CPoint(0,0))
                        Sim.SendSimpleMessage (ATNET_DIALOG_NEXT, qOther.NetworkID);
                     break;

                  case 2200: case 2201: case 2202:
                     DialogPar2^=1;
                     //kein break;
                  case 1001: case 1600: case 2500: case 2501: case 2502: case 2503:
                     DialogPar2^=1;
                     if (DialogPar2)
                     {
                        //4 Dialogvarianten, je nachdem ob wir Pralinen haben und/oder ein Chat erlaubt ist
                        if (DialogMedium==MEDIUM_AIR && (!bIsRobot) && (qPlayer.HasItem(ITEM_PRALINEN) || qPlayer.HasItem(ITEM_PRALINEN_A)))
                           MakeSayWindow (1, TOKEN_PLAYER, 2030+(id!=1600), 2034, FALSE, &FontDialog, &FontDialogLight);
                        else if ((!bIsRobot))
                           MakeSayWindow (1, TOKEN_PLAYER, 2020+(id!=1600), 2023, FALSE, &FontDialog, &FontDialogLight);
                        else if (DialogMedium==MEDIUM_AIR && (qPlayer.HasItem(ITEM_PRALINEN) || qPlayer.HasItem(ITEM_PRALINEN_A)))
                           MakeSayWindow (1, TOKEN_PLAYER, 2010+(id!=1600), 2013, FALSE, &FontDialog, &FontDialogLight);
                        else
                           MakeSayWindow (1, TOKEN_PLAYER, 2000+(id!=1600), 2002, FALSE, &FontDialog, &FontDialogLight);
                     }
                     else if (bIsRobot)
                     {
                        //Was will der Computerspieler? Erst labern, dann Besprechen.
                        MakeSayWindow (0, TOKEN_PLAYER, 2000+(id!=1600), pFontPartner);
                     }
                     else if (point!=CPoint(0,0))
                        Sim.SendSimpleMessage (ATNET_DIALOG_NEXT, qOther.NetworkID);
                     break;

                  case 2000: case 2010: case 2020: case 2030:
                     DialogPar2^=1;
                     if (DialogPar2)
                        MakeSayWindow (1, TOKEN_PLAYER, 2500, 2503, FALSE, &FontDialog, &FontDialogLight);
                     else if (bIsRobot)
                     {
                        SLONG Summe=qOther.BilanzGestern.GetSumme();

                        if (Summe>10000000)                    MakeSayWindow (0, TOKEN_PLAYER, 2500, pFontPartner);
                        else if (Summe>1000000 || Sim.Date==0) MakeSayWindow (0, TOKEN_PLAYER, 2501, pFontPartner);
                        else if (Summe>=0)                     MakeSayWindow (0, TOKEN_PLAYER, 2502, pFontPartner);
                        else                                   MakeSayWindow (0, TOKEN_PLAYER, 2503, pFontPartner);
                     }
                     else if (point!=CPoint(0,0))
                        Sim.SendSimpleMessage (ATNET_DIALOG_NEXT, qOther.NetworkID);
                     break;

                  case 2001: case 2011: case 2021: case 2031:
                     if (DialogPar2)
                     {
                        if (qPlayer.Kooperation[DialogPar1])
                           MakeSayWindow (1, TOKEN_PLAYER, 20210, 20213, FALSE, &FontDialog, &FontDialogLight);
                        else
                           MakeSayWindow (1, TOKEN_PLAYER, 20200, 20201, FALSE, &FontDialog, &FontDialogLight);
                     }
                     else if (bIsRobot)
                     {
                        //Was will der Computerspieler? Kooperation verändern!
                        if (qOther.Kooperation[Sim.localPlayer] && qOther.Sympathie[Sim.localPlayer]<-20)
                           MakeSayWindow (0, TOKEN_PLAYER, 20211, pFontPartner);
                        else
                           MakeSayWindow (0, TOKEN_PLAYER, 20200, pFontPartner);
                     }
                     else if (point!=CPoint(0,0))
                        Sim.SendSimpleMessage (ATNET_DIALOG_NEXT, qOther.NetworkID);
                     break;

                  case 2012: case 2032: //Pralinen
                     if (bIsRobot)
                     {
                        if (qPlayer.HasItem(ITEM_PRALINEN))
                        {
                           qPlayer.DropItem(ITEM_PRALINEN);

                           if (qOther.Sympathie[Sim.localPlayer]>0)
                              MakeSayWindow (0, TOKEN_PLAYER, 2200, pFontPartner);
                           else
                              MakeSayWindow (0, TOKEN_PLAYER, 2201, pFontPartner);

                           qOther.Sympathie[Sim.localPlayer]+=10;
                           Limit ((SLONG)-1000, qOther.Sympathie[Sim.localPlayer], (SLONG)1000);
                        }
                        else if (qPlayer.HasItem(ITEM_PRALINEN_A))
                        {
                           qPlayer.DropItem(ITEM_PRALINEN_A);
                           MakeSayWindow (0, TOKEN_PLAYER, 2202, pFontPartner);
                           qOther.Sympathie[Sim.localPlayer]-=10;
                           Limit ((SLONG)-1000, qOther.Sympathie[Sim.localPlayer], (SLONG)1000);
                        }
                        break;
                     }
                     else if (DialogPar2)
                     {
                        if (qPlayer.HasItem(ITEM_PRALINEN))
                        {
                           TEAKFILE Message;
                           Message.Announce(128);

                           MakeSayWindow (0, TOKEN_PLAYER, 2200, pFontPartner);
                           Message << ATNET_DIALOG_TEXT << TextAlign << SLONG(2200) << OrgOptionen[0];

                           Sim.SendMemFile (Message, qOther.NetworkID);
                           qPlayer.DropItem (ITEM_PRALINEN);
                        }
                        else if (qPlayer.HasItem(ITEM_PRALINEN_A))
                        {
                           MakeSayWindow (0, TOKEN_PLAYER, 2202, pFontPartner);

                           TEAKFILE Message;
                           Message.Announce(128);
                           Message << ATNET_DIALOG_TEXT << TextAlign << SLONG(2202) << OrgOptionen[0];
                           Sim.SendMemFile (Message, qOther.NetworkID);

                           Sim.SendSimpleMessage (ATNET_DIALOG_DRUNK, qOther.NetworkID);
                           qPlayer.DropItem (ITEM_PRALINEN_A);
                        }
                     }
                     else if (point!=CPoint(0,0))
                     {
                        Sim.SendSimpleMessage (ATNET_DIALOG_NEXT, qOther.NetworkID);
                     }
                     break;

                  case 2022: case 2033: //Chatten
                     StopDialog ();
                     MenuStart (MENU_CHAT, DialogPar1);
                     break;

                  case 20200: //Einer schlägt Kooperation vor:
                     DialogPar2^=1;
                     if (DialogPar2)
                        MakeSayWindow (1, TOKEN_PLAYER, 20400, 20401, FALSE, &FontDialog, &FontDialogLight);
                     else if (bIsRobot)
                     {
                        if (qOther.Sympathie[Sim.localPlayer]>0)
                           MakeSayWindow (0, TOKEN_PLAYER, 20400, pFontPartner);
                        else
                           MakeSayWindow (0, TOKEN_PLAYER, 20401, pFontPartner);
                     }
                     else if (point!=CPoint(0,0))
                        Sim.SendSimpleMessage (ATNET_DIALOG_NEXT, qOther.NetworkID);
                     break;

                  case 20400: //Kooperation beginnen:
                     qOther.Sympathie[Sim.localPlayer]+=30;
                     Limit ((SLONG)-1000, qOther.Sympathie[Sim.localPlayer], (SLONG)1000);

                     qOther.Kooperation[Sim.localPlayer]=TRUE;
                     qPlayer.Kooperation[DialogPar1]=TRUE;

                     qOther.NetSynchronizeKooperation ();
                     qOther.NetSynchronizeImage ();
                     qPlayer.NetSynchronizeKooperation ();
                     qPlayer.NetSynchronizeImage ();

                     DialogPar2^=1;
                     if (DialogPar2)
                     {
                        goto label_maindialog_players_again;
                     }
                     else if (bIsRobot)
                     {
                        //Was will der Computerspieler? Tschüss!
                        MakeSayWindow (0, TOKEN_PLAYER, 2003, pFontPartner);
                     }
                     else if (point!=CPoint(0,0))
                        Sim.SendSimpleMessage (ATNET_DIALOG_NEXT, qOther.NetworkID);
                     break;

                  case 20401: //Kooperation ablehnen:
                     qOther.Sympathie[Sim.localPlayer]-=10;
                     Limit ((SLONG)-1000, qOther.Sympathie[Sim.localPlayer], (SLONG)1000);

                     DialogPar2^=1;
                     if (DialogPar2)
                     {
                        goto label_maindialog_players_again;
                     }
                     else if (bIsRobot)
                     {
                        //Was will der Computerspieler? Beleidigt sein!
                        MakeSayWindow (0, TOKEN_PLAYER, 20402, pFontPartner);
                        qOther.Sympathie[Sim.localPlayer]-=20;
                     }
                     else if (point!=CPoint(0,0))
                        Sim.SendSimpleMessage (ATNET_DIALOG_NEXT, qOther.NetworkID);
                     break;

                  case 20210: //Kooperation kündigen (brutal):
                     qOther.Sympathie[Sim.localPlayer]-=200;
                     Limit ((SLONG)-1000, qOther.Sympathie[Sim.localPlayer], (SLONG)1000);

                     qOther.Kooperation[Sim.localPlayer]=FALSE;
                     qPlayer.Kooperation[DialogPar1]=FALSE;

                     qOther.NetSynchronizeKooperation ();
                     qOther.NetSynchronizeImage ();
                     qPlayer.NetSynchronizeKooperation ();
                     qPlayer.NetSynchronizeImage ();

                     DialogPar2^=1;
                     if (DialogPar2)
                     {
                        MakeSayWindow (1, TOKEN_PLAYER, 20402, 20402, FALSE, &FontDialog, &FontDialogLight);
                     }
                     else if (bIsRobot)
                     {
                        MakeSayWindow (0, TOKEN_PLAYER, 20402, pFontPartner);
                     }
                     else if (point!=CPoint(0,0))
                        Sim.SendSimpleMessage (ATNET_DIALOG_NEXT, qOther.NetworkID);
                     break;

                  case 20211: //Kooperation kündigen (sanft):
                     qOther.Kooperation[Sim.localPlayer]=FALSE;
                     qPlayer.Kooperation[DialogPar1]=FALSE;

                     qOther.NetSynchronizeKooperation ();
                     qOther.NetSynchronizeImage ();
                     qPlayer.NetSynchronizeKooperation ();
                     qPlayer.NetSynchronizeImage ();

                     DialogPar2^=1;
                     if (DialogPar2)
                     {
                        MakeSayWindow (1, TOKEN_PLAYER, 20402, 20402, FALSE, &FontDialog, &FontDialogLight);
                     }
                     else if (bIsRobot)
                     {
                        MakeSayWindow (0, TOKEN_PLAYER, 20402, pFontPartner);
                     }
                     else if (point!=CPoint(0,0))
                        Sim.SendSimpleMessage (ATNET_DIALOG_NEXT, qOther.NetworkID);
                     break;

                  case 20212: //schmeicheln/schleimen:
                     qOther.Sympathie[Sim.localPlayer]+=10;
                     Limit ((SLONG)-1000, qOther.Sympathie[Sim.localPlayer], (SLONG)1000);

                     DialogPar2^=1;
                     if (DialogPar2)
                     {
                        MakeSayWindow (1, TOKEN_PLAYER, 20403, 20403, FALSE, &FontDialog, &FontDialogLight);
                     }
                     else
                     {
                        MakeSayWindow (0, TOKEN_PLAYER, 20403, pFontPartner);
                     }
                     break;

                  //Eine Ebene nach oben:
                  case 20201: case 20213:
label_maindialog_players_again:
                     //4 Dialogvarianten, je nachdem ob wir Pralinen haben und/oder ein Chat erlaubt ist
                     if (DialogMedium==MEDIUM_AIR && (!bIsRobot) && (qPlayer.HasItem(ITEM_PRALINEN) || qPlayer.HasItem(ITEM_PRALINEN_A)))
                        MakeSayWindow (1, TOKEN_PLAYER, 2030+(id!=1600), 2034, FALSE, &FontDialog, &FontDialogLight);
                     else if ((!bIsRobot))
                        MakeSayWindow (1, TOKEN_PLAYER, 2020+(id!=1600), 2023, FALSE, &FontDialog, &FontDialogLight);
                     else if (DialogMedium==MEDIUM_AIR && (qPlayer.HasItem(ITEM_PRALINEN) || qPlayer.HasItem(ITEM_PRALINEN_A)))
                        MakeSayWindow (1, TOKEN_PLAYER, 2010+(id!=1600), 2013, FALSE, &FontDialog, &FontDialogLight);
                     else
                        MakeSayWindow (1, TOKEN_PLAYER, 2000+(id!=1600), 2002, FALSE, &FontDialog, &FontDialogLight);
                     break;

                  //Eine Ebene nach oben:
                  case 20403:
                     DialogPar2^=1;
                     goto label_maindialog_players_again;
                     break;

                  //Dialog beenden:
                  case 1601:
                     if (DialogPar2)
                     {
                        qOther.Sympathie[Sim.localPlayer]-=10;
                        Limit ((SLONG)-1000, qOther.Sympathie[Sim.localPlayer], (SLONG)1000);
                        if (qOther.Sympathie[Sim.localPlayer]<-100)
                        {
                           qOther.Kooperation[Sim.localPlayer]=FALSE;
                           qPlayer.Kooperation[DialogPar1]=FALSE;

                           qOther.NetSynchronizeKooperation ();
                           qOther.NetSynchronizeImage ();
                           qPlayer.NetSynchronizeKooperation ();
                           qPlayer.NetSynchronizeImage ();
                        }
                     }
                     else if (bIsRobot)
                     {
                        qOther.Kooperation[Sim.localPlayer]=FALSE;
                        qPlayer.Kooperation[DialogPar1]=FALSE;

                        qOther.NetSynchronizeKooperation ();
                        qOther.NetSynchronizeImage ();
                        qPlayer.NetSynchronizeKooperation ();
                        qPlayer.NetSynchronizeImage ();
                     }
                     else if (point!=CPoint(0,0))
                        Sim.SendSimpleMessage (ATNET_DIALOG_NEXT, qOther.NetworkID);
                     StopDialog ();
                     break;

                  case 1602:
                     if (DialogPar2)
                     {
                        qOther.Sympathie[Sim.localPlayer]-=100;
                        Limit ((SLONG)-1000, qOther.Sympathie[Sim.localPlayer], (SLONG)1000);
                        qOther.Kooperation[Sim.localPlayer]=FALSE;
                        qPlayer.Kooperation[DialogPar1]=FALSE;

                        qOther.NetSynchronizeKooperation ();
                        qOther.NetSynchronizeImage ();
                        qPlayer.NetSynchronizeKooperation ();
                        qPlayer.NetSynchronizeImage ();
                     }
                     else if (bIsRobot)
                     {
                        qOther.Kooperation[Sim.localPlayer]=FALSE;
                        qPlayer.Kooperation[DialogPar1]=FALSE;

                        qOther.NetSynchronizeKooperation ();
                        qOther.NetSynchronizeImage ();
                        qPlayer.NetSynchronizeKooperation ();
                        qPlayer.NetSynchronizeImage ();
                     }
                     else if (point!=CPoint(0,0))
                        Sim.SendSimpleMessage (ATNET_DIALOG_NEXT, qOther.NetworkID);
                     StopDialog ();
                     break;

                  case 20402:
                  default: //Standard: Dialog abbrechen
                     StopDialog ();
                     break;
               }
            }
            break;

         case TALKER_NASA:
            switch (id)
            {
               case 1000: case 1001: case 1002: case 1003:
                  if (Sim.Difficulty==DIFF_FINAL)
                     MakeSayWindow (1, TOKEN_NASA, 2000+(DialogMedium==MEDIUM_HANDY)*100, 2003+(DialogMedium==MEDIUM_HANDY)*100, FALSE, &FontDialog, &FontDialogLight);
                  else
                     MakeSayWindow (1, TOKEN_NASA, 2200+(DialogMedium==MEDIUM_HANDY)*100, 2203+(DialogMedium==MEDIUM_HANDY)*100, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 2000: case 2100: case 2200: case 2300: //Alles senkrecht?
                  MakeSayWindow (0, TOKEN_NASA, 3000, pFontPartner);
                  break;
               case 3000:
                  MakeSayWindow (1, TOKEN_NASA, 3001, 3001, FALSE, &FontDialog, &FontDialogLight);
                  break;
               case 3001:
                  MakeSayWindow (0, TOKEN_NASA, 3002, pFontPartner);
                  break;
               case 3002:
                  if (Sim.Difficulty==DIFF_FINAL)
                     MakeSayWindow (1, TOKEN_NASA, 2001+(DialogMedium==MEDIUM_HANDY)*100, 2003+(DialogMedium==MEDIUM_HANDY)*100, FALSE, &FontDialog, &FontDialogLight);
                  else
                     MakeSayWindow (1, TOKEN_NASA, 2201+(DialogMedium==MEDIUM_HANDY)*100, 2203+(DialogMedium==MEDIUM_HANDY)*100, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 2001: case 2101: //Anleitung
                  MakeSayWindow (0, TOKEN_NASA, 4000, pFontPartner);
                  break;
               case 4000:
                  MakeSayWindow (0, TOKEN_NASA, 4001, pFontPartner);
                  break;
               case 4001:
                  MakeSayWindow (0, TOKEN_NASA, 4002, pFontPartner);
                  break;
               case 4002:
                  MakeSayWindow (0, TOKEN_NASA, 4003, pFontPartner);
                  break;
               case 4003: case 5003: case 5107:
                  MakeSayWindow (1, TOKEN_NASA, 2002+(DialogMedium==MEDIUM_HANDY)*100, 2003+(DialogMedium==MEDIUM_HANDY)*100, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 2201: case 2301: //Anleitung
                  MakeSayWindow (0, TOKEN_NASA, 4100, pFontPartner);
                  break;
               case 4100:
                  MakeSayWindow (0, TOKEN_NASA, 4101, pFontPartner);
                  break;
               case 4101:
                  MakeSayWindow (0, TOKEN_NASA, 4102, pFontPartner);
                  break;
               case 4102:
                  MakeSayWindow (0, TOKEN_NASA, 4103, pFontPartner);
                  break;
               case 4103:
               case 8003: case 8105: case 8202:
                  if (Sim.Difficulty==DIFF_FINAL)
                     MakeSayWindow (1, TOKEN_NASA, 2102+(DialogMedium==MEDIUM_HANDY)*100, 2103+(DialogMedium==MEDIUM_HANDY)*100, FALSE, &FontDialog, &FontDialogLight);
                  else
                     MakeSayWindow (1, TOKEN_NASA, 2202+(DialogMedium==MEDIUM_HANDY)*100, 2203+(DialogMedium==MEDIUM_HANDY)*100, FALSE, &FontDialog, &FontDialogLight);
                  break;

               case 2002: case 2102: case 7000: //Kaufen:
               case 5096: case 5097: case 5098: case 5099: 
               case 5196: case 5197:
                  if ((qPlayer.RocketFlags & ROCKET_PART_ONE) != ROCKET_PART_ONE)
                     MakeSayWindow (1, TOKEN_NASA, 5000, 5003, FALSE, &FontDialog, &FontDialogLight);
                  else
                     MakeSayWindow (1, TOKEN_NASA, 5100, 5107, FALSE, &FontDialog, &FontDialogLight);
                  break;
               case 5000:
                  if (qPlayer.RocketFlags & ROCKET_BASE)
                     MakeSayWindow (0, TOKEN_NASA, 5098, pFontPartner);
                  else if (qPlayer.Money<RocketPrices[0])
                     MakeSayWindow (0, TOKEN_NASA, 7000, pFontPartner);
                  else
                  {
                     ((CNasa*)((qPlayer.DialogWin) ? qPlayer.DialogWin : this))->KommVarTippNow=12;
                  	 qPlayer.AddRocketPart(ROCKET_BASE, -RocketPrices[0]);
                     MakeSayWindow (0, TOKEN_NASA, 5099, pFontPartner);
                     PlayFanfare();
                  }
                  break;
               case 5001:
                  if (qPlayer.RocketFlags & ROCKET_TOWER)
                     MakeSayWindow (0, TOKEN_NASA, 5098, pFontPartner);
                  else if (!(qPlayer.RocketFlags & ROCKET_BASE))
                     MakeSayWindow (0, TOKEN_NASA, 5096, pFontPartner);
                  else if (qPlayer.Money<RocketPrices[1])
                     MakeSayWindow (0, TOKEN_NASA, 7000, pFontPartner);
                  else
                  {
                     ((CNasa*)((qPlayer.DialogWin) ? qPlayer.DialogWin : this))->KommVarTippNow=12;
                     qPlayer.AddRocketPart(ROCKET_TOWER, -RocketPrices[1]);
                     MakeSayWindow (0, TOKEN_NASA, 5099, pFontPartner);
                     PlayFanfare();
                  }
                  break;
               case 5002:
                  if (!(qPlayer.RocketFlags & ROCKET_TOWER))
                     MakeSayWindow (0, TOKEN_NASA, 5097, pFontPartner);
                  else if (qPlayer.Money<RocketPrices[2])
                     MakeSayWindow (0, TOKEN_NASA, 7000, pFontPartner);
                  else
                  {                            
                     ((CNasa*)((qPlayer.DialogWin) ? qPlayer.DialogWin : this))->KommVarTippNow=12;
                     qPlayer.AddRocketPart(ROCKET_ARM, -RocketPrices[2]);
                     MakeSayWindow (0, TOKEN_NASA, 5099, pFontPartner);
                     PlayFanfare();
                  }
                  break;

               case 5100: //Parts Level II
                  if (qPlayer.RocketFlags & ROCKET_AIRFRAME)
                     MakeSayWindow (0, TOKEN_NASA, 5098, pFontPartner);
                  else if (qPlayer.Money<RocketPrices[3])
                     MakeSayWindow (0, TOKEN_NASA, 7000, pFontPartner);
                  else
                  {
                     ((CNasa*)((qPlayer.DialogWin) ? qPlayer.DialogWin : this))->KommVarTippNow=12;
                     qPlayer.AddRocketPart(ROCKET_AIRFRAME, -RocketPrices[3]);
                     MakeSayWindow (0, TOKEN_NASA, 5099, pFontPartner);
                     PlayFanfare();
                  }
                  break;
               case 5101:
                  if (qPlayer.RocketFlags & ROCKET_WINGS)
                     MakeSayWindow (0, TOKEN_NASA, 5098, pFontPartner);
                  else if (!(qPlayer.RocketFlags & ROCKET_AIRFRAME))
                     MakeSayWindow (0, TOKEN_NASA, 5196, pFontPartner);
                  else if (qPlayer.Money<RocketPrices[4])
                     MakeSayWindow (0, TOKEN_NASA, 7000, pFontPartner);
                  else
                  {
                     ((CNasa*)((qPlayer.DialogWin) ? qPlayer.DialogWin : this))->KommVarTippNow=12;
                     qPlayer.AddRocketPart(ROCKET_WINGS, -RocketPrices[4]);
                     MakeSayWindow (0, TOKEN_NASA, 5099, pFontPartner);
                     PlayFanfare();
                  }
                  break;
               case 5102:
                  if (qPlayer.RocketFlags & ROCKET_CAPSULE)
                     MakeSayWindow (0, TOKEN_NASA, 5098, pFontPartner);
                  else if (!(qPlayer.RocketFlags & ROCKET_AIRFRAME))
                     MakeSayWindow (0, TOKEN_NASA, 5196, pFontPartner);
                  else if (qPlayer.Money<RocketPrices[5])
                     MakeSayWindow (0, TOKEN_NASA, 7000, pFontPartner);
                  else
                  {
                     ((CNasa*)((qPlayer.DialogWin) ? qPlayer.DialogWin : this))->KommVarTippNow=12;
                     qPlayer.AddRocketPart(ROCKET_CAPSULE, -RocketPrices[5]);
                     MakeSayWindow (0, TOKEN_NASA, 5099, pFontPartner);
                     PlayFanfare();
                  }
                  break;
               case 5103:
                  if (qPlayer.RocketFlags & ROCKET_HECK)
                     MakeSayWindow (0, TOKEN_NASA, 5098, pFontPartner);
                  else if (!(qPlayer.RocketFlags & ROCKET_AIRFRAME))
                     MakeSayWindow (0, TOKEN_NASA, 5196, pFontPartner);
                  else if (qPlayer.Money<RocketPrices[6])
                     MakeSayWindow (0, TOKEN_NASA, 7000, pFontPartner);
                  else
                  {
                     ((CNasa*)((qPlayer.DialogWin) ? qPlayer.DialogWin : this))->KommVarTippNow=12;
                     qPlayer.AddRocketPart(ROCKET_HECK, -RocketPrices[6]);
                     MakeSayWindow (0, TOKEN_NASA, 5099, pFontPartner);
                     PlayFanfare();
                  }
                  break;
               case 5104:
                  if (qPlayer.RocketFlags & ROCKET_PROP)
                     MakeSayWindow (0, TOKEN_NASA, 5098, pFontPartner);
                  else if (!(qPlayer.RocketFlags & ROCKET_AIRFRAME))
                     MakeSayWindow (0, TOKEN_NASA, 5196, pFontPartner);
                  else if (!(qPlayer.RocketFlags & ROCKET_WINGS))
                     MakeSayWindow (0, TOKEN_NASA, 5197, pFontPartner);
                  else if (qPlayer.Money<RocketPrices[7])
                     MakeSayWindow (0, TOKEN_NASA, 7000, pFontPartner);
                  else
                  {
                     ((CNasa*)((qPlayer.DialogWin) ? qPlayer.DialogWin : this))->KommVarTippNow=12;
                     qPlayer.AddRocketPart(ROCKET_PROP, -RocketPrices[7]);
                     MakeSayWindow (0, TOKEN_NASA, 5099, pFontPartner);
                     PlayFanfare();
                  }
                  break;
               case 5105:
                  if (qPlayer.RocketFlags & ROCKET_MAINPROP)
                     MakeSayWindow (0, TOKEN_NASA, 5098, pFontPartner);
                  else if (!(qPlayer.RocketFlags & ROCKET_AIRFRAME))
                     MakeSayWindow (0, TOKEN_NASA, 5196, pFontPartner);
                  else if (qPlayer.Money<RocketPrices[8])
                     MakeSayWindow (0, TOKEN_NASA, 7000, pFontPartner);
                  else
                  {
                     ((CNasa*)((qPlayer.DialogWin) ? qPlayer.DialogWin : this))->KommVarTippNow=12;
                     qPlayer.AddRocketPart(ROCKET_MAINPROP, -RocketPrices[8]);
                     MakeSayWindow (0, TOKEN_NASA, 5099, pFontPartner);
                     PlayFanfare();
                  }
                  break;
               case 5106:
                  if (qPlayer.RocketFlags & ROCKET_COCKPIT)
                     MakeSayWindow (0, TOKEN_NASA, 5098, pFontPartner);
                  else if (!(qPlayer.RocketFlags & ROCKET_AIRFRAME))
                     MakeSayWindow (0, TOKEN_NASA, 5196, pFontPartner);
                  else if (qPlayer.Money<RocketPrices[9])
                     MakeSayWindow (0, TOKEN_NASA, 7000, pFontPartner);
                  else
                  {
                     ((CNasa*)((qPlayer.DialogWin) ? qPlayer.DialogWin : this))->KommVarTippNow=12;
                     qPlayer.AddRocketPart(ROCKET_COCKPIT, -RocketPrices[9]);
                     MakeSayWindow (0, TOKEN_NASA, 5099, pFontPartner);
                     PlayFanfare();
                  }
                  break;

               case 2003:
                  MakeSayWindow (0, TOKEN_NASA, 6000, pFontPartner);
                  break;

               case 2202: case 2302: //Kaufen im Add-On:
               case 8300: case 8301: case 8302:
                  if ((qPlayer.RocketFlags & STATION_PART_TWO) == STATION_PART_TWO)
                     MakeSayWindow (1, TOKEN_NASA, 8200, 8202, FALSE, &FontDialog, &FontDialogLight);
                  else if ((qPlayer.RocketFlags & STATION_PART_ONE) == STATION_PART_ONE)
                     MakeSayWindow (1, TOKEN_NASA, 8100, 8105, FALSE, &FontDialog, &FontDialogLight);
                  else
                     MakeSayWindow (1, TOKEN_NASA, 8000, 8003, FALSE, &FontDialog, &FontDialogLight);
                  break;

               //Parts Level I, II, III
               case 8000: case 8001: case 8002:
               case 8100: case 8101: case 8102: case 8103: case 8104:
               case 8200: case 8201:
                  {
                     SLONG Flag=0, Index=0;

                     if (id>=8000 && id<=8002) Index=id-8000;
                     if (id>=8100 && id<=8106) Index=id-8100+3;
                     if (id>=8200 && id<=8201) Index=id-8200+8;
                     Flag=1<<Index;

                     if (qPlayer.RocketFlags & Flag)
                        MakeSayWindow (0, TOKEN_NASA, 8301, pFontPartner);
                     else if (qPlayer.Money<StationPrices[Index])
                        MakeSayWindow (0, TOKEN_NASA, 8300, pFontPartner);
                     else
                     {
                        ((CNasa*)((qPlayer.DialogWin) ? qPlayer.DialogWin : this))->KommVarTippNow=12;
                        qPlayer.AddRocketPart(Flag, -RocketPrices[Index]);
                        MakeSayWindow (0, TOKEN_NASA, 8302, pFontPartner);
                        PlayFanfare();
                     }
                  }
                  break;

               case 9999:
                  qPlayer.LeaveRoom();
                  StopDialog ();
                  break;

               case 500:
               default: //Standard: Dialog abbrechen
                  StopDialog ();
                  break;
            }
            break;

         case TALKER_RICK:
            switch (id)
            {
               case 1000:
                  MakeSayWindow (1, TOKEN_RICK, 1100, 1100, FALSE, &FontDialog, &FontDialogLight);
                  break;
               case 1100:
                  MakeSayWindow (0, 1200, bprintf (DialogTexte.GetS (TOKEN_RICK, 1200), LPCTSTR(qPlayer.AirlineX)), pFontPartner);
                  break;
               case 1200:
                  MakeSayWindow (1, TOKEN_RICK, 1300, 1300, FALSE, &FontDialog, &FontDialogLight);
                  break;
               case 1300:
                  MakeSayWindow (0, TOKEN_RICK, 1400, pFontPartner);
                  break;
               case 1400:
                  MakeSayWindow (0, TOKEN_RICK, 1401, pFontPartner);
                  break;

               case 2000:
                  //Neue Räume:
                  if (Sim.Difficulty==DIFF_FINAL && Sim.Date==1) MakeSayWindow (0, TOKEN_RICK, 2100, pFontPartner);
                  else if (Sim.Difficulty==DIFF_NORMAL && Sim.Date==1) MakeSayWindow (0, TOKEN_RICK, 2101, pFontPartner);
                  else
                  {
                     SLONG c;

                     //Neues Flugzeug:
                     for (c=0; c<SLONG(PlaneTypes.AnzEntries()); c++)
                        if (PlaneTypes.IsInAlbum(c))
                        if (PlaneTypes[c].FirstMissions==Sim.Difficulty || Sim.Difficulty==DIFF_FREEGAME)
                        {
                           if (PlaneTypes[c].FirstDay==Sim.Date)
                           {
                              MakeSayWindow (0, 9999, bprintf (DialogTexte.GetS (TOKEN_RICK, 2102), LPCTSTR(PlaneTypes[c].Name), LPCTSTR(PlaneTypes[c].Hersteller)), pFontPartner);
                              break;
                           }
                        }
                     if (c<SLONG(PlaneTypes.AnzEntries())) break;

                     if (Sim.Date>3 && Sim.MoneyInBankTrash && rand()%5==0) MakeSayWindow (0, TOKEN_RICK, 2104, pFontPartner);
                     else if (Sim.Date>15 && qPlayer.MechTrust==0 && rand()%5==0) MakeSayWindow (0, TOKEN_RICK, 2106, pFontPartner);
                     else if (!qPlayer.HasBerater(BERATERTYP_INFO)       && qPlayer.HasBeraterApplied(BERATERTYP_INFO)       && rand()%5==0) MakeSayWindow (0, TOKEN_RICK, 2107, pFontPartner);
                     else if (!qPlayer.HasBerater(BERATERTYP_FITNESS)    && qPlayer.HasBeraterApplied(BERATERTYP_FITNESS)    && rand()%5==0) MakeSayWindow (0, TOKEN_RICK, 2108, pFontPartner);
                     else if (!qPlayer.HasBerater(BERATERTYP_SICHERHEIT) && qPlayer.HasBeraterApplied(BERATERTYP_SICHERHEIT) && rand()%5==0) MakeSayWindow (0, TOKEN_RICK, 2109, pFontPartner);
                     else if (!qPlayer.HasBerater(BERATERTYP_AUFTRAG)    && qPlayer.HasBeraterApplied(BERATERTYP_AUFTRAG)    && rand()%5==0) MakeSayWindow (0, TOKEN_RICK, 2110, pFontPartner);
                     else if (!qPlayer.HasBerater(BERATERTYP_GELD)       && qPlayer.HasBeraterApplied(BERATERTYP_GELD)       && rand()%5==0) MakeSayWindow (0, TOKEN_RICK, 2111, pFontPartner);
                     else if (!qPlayer.HasBerater(BERATERTYP_PERSONAL)   && qPlayer.HasBeraterApplied(BERATERTYP_PERSONAL)   && rand()%5==0) MakeSayWindow (0, TOKEN_RICK, 2112, pFontPartner);
                     else if (!qPlayer.HasBerater(BERATERTYP_ROUTE)      && qPlayer.HasBeraterApplied(BERATERTYP_ROUTE)      && rand()%5==0) MakeSayWindow (0, TOKEN_RICK, 2113, pFontPartner);
                     else
                     {
                        //Neues Flugzeug in drei Tagen:
                        for (c=0; c<SLONG(PlaneTypes.AnzEntries()); c++)
                           if (PlaneTypes.IsInAlbum(c))
                           if (PlaneTypes[c].FirstMissions==Sim.Difficulty || Sim.Difficulty==DIFF_FREEGAME)
                           {
                              if (PlaneTypes[c].FirstDay==Sim.Date+3)
                              {
                                 MakeSayWindow (0, 9999, bprintf (DialogTexte.GetS (TOKEN_RICK, 2103), LPCTSTR(PlaneTypes[c].Name), LPCTSTR(PlaneTypes[c].Hersteller)), pFontPartner);
                                 break;
                              }
                           }
                        if (c<SLONG(PlaneTypes.AnzEntries())) break;

                        //Kooperieren?
                        if (((Sim.Players.Players[(Sim.localPlayer+1)%4].Sympathie[Sim.localPlayer]>0 && qPlayer.Kooperation[(Sim.localPlayer+1)%4]==0 && !Sim.Players.Players[(Sim.localPlayer+1)%4].IsOut) ||
                             (Sim.Players.Players[(Sim.localPlayer+2)%4].Sympathie[Sim.localPlayer]>0 && qPlayer.Kooperation[(Sim.localPlayer+2)%4]==0 && !Sim.Players.Players[(Sim.localPlayer+2)%4].IsOut) ||
                             (Sim.Players.Players[(Sim.localPlayer+3)%4].Sympathie[Sim.localPlayer]>0 && qPlayer.Kooperation[(Sim.localPlayer+3)%4]==0 && !Sim.Players.Players[(Sim.localPlayer+3)%4].IsOut)) && rand()%5==0)
                           MakeSayWindow (0, TOKEN_RICK, 2114, pFontPartner);
                        else
                        {
                           for (c=0; c<SLONG(qPlayer.Planes.AnzEntries()); c++)
                              if (qPlayer.Planes.IsInAlbum(c) && SLONG(qPlayer.Planes[c].TargetZustand)-SLONG(qPlayer.Planes[c].WorstZustand)>20 && rand()%5==0)
                              {
                                 MakeSayWindow (0, TOKEN_RICK, 2115, pFontPartner);
                                 break;
                              }
                              else if (qPlayer.Planes.IsInAlbum(c) && SLONG(qPlayer.Planes[c].TargetZustand)<60 && rand()%5==0)
                              {
                                 MakeSayWindow (0, TOKEN_RICK, 2116, pFontPartner);
                                 break;
                              }
                              else if (qPlayer.Planes.IsInAlbum(c) && SLONG(qPlayer.Planes[c].Zustand)<=SLONG(qPlayer.Planes[c].WorstZustand+2) && qPlayer.Planes[c].WorstZustand<=80 && rand()%5==0)
                              {
                                 MakeSayWindow (0, TOKEN_RICK, 2121, pFontPartner);
                                 break;
                              }
                           if (c<SLONG(qPlayer.Planes.AnzEntries())) break;

                           //Kaum Aktien von sich selber/Neue Aktien:
                           if (qPlayer.OwnsAktien[Sim.localPlayer]<qPlayer.AnzAktien/2 && rand()%5==0) MakeSayWindow (0, TOKEN_RICK, 2117, pFontPartner);
                           else if (((qPlayer.MaxAktien-qPlayer.AnzAktien)/100*100)>=10000 && rand()%5==0) MakeSayWindow (0, TOKEN_RICK, 2118, pFontPartner);
                           else if (qPlayer.Dividende<5 && rand()%5==0) MakeSayWindow (0, TOKEN_RICK, 2119, pFontPartner);
                           else if (qPlayer.Money>30000000 && rand()%5==0) MakeSayWindow (0, TOKEN_RICK, 2120, pFontPartner);

                           //Tote Hose:
                           else MakeSayWindow (0, TOKEN_RICK, 2199, pFontPartner);
                        }
                     }
                  }
                  break;

               default: //Standard: Dialog abbrechen
                  StopDialog ();
                  break;
            }
            break;

         case TALKER_TRINKER:
            switch (id)
            {
               case 6000:
                  qPlayer.StrikeEndType  = 3;       //Streik beendet durch Trinker
                  qPlayer.StrikeEndCountdown = 4;
                  StopDialog ();
                  break;

               case 0:
               default: //Standard: Dialog abbrechen
                  StopDialog ();
                  break;
            }
            break;

         case TALKER_FRACHT:
            StopDialog ();
            break;

         case TALKER_KIOSK:
            switch (id)
            {
               case 1021:
                  if (qPlayer.HasSpaceForItem()) qPlayer.BuyItem(ITEM_STINKBOMBE);
                  qPlayer.KioskTrust=0;
                  StopDialog ();
                  break;

               case 0:
               default: //Standard: Dialog abbrechen
                  StopDialog ();
                  break;
            }
            break;
      }

      return (TRUE);
   }

   return (FALSE);
}
#pragma optimize("", off)