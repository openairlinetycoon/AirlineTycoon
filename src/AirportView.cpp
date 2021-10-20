//============================================================================================
// AirportView.cpp - Das Fenster zum anzeigen & handlen der isometrischen Übersicht
//============================================================================================
// Links: "AirportView.h"
//============================================================================================
#include "stdafx.h"
#include "HLine.h"
#include "AskBrick.h"
#include "HallDiskMenu.h"
#include "Synthese.h"
#include "Atnet.h"

extern CHLPool HLPool;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SB_CColorFX ColorFX;
//extern LPDIRECTDRAWSURFACE FrontSurf;
extern XY  gScrollOffsetA, gScrollOffsetB;

//Die statischen Daten dafür initialisieren:
static const char FileId[] = "AVie";

BUFFER<CBencher> BrickWait (MAX_BRICKS);

BOOL IgnoreNextLButtonUp=FALSE;

//Öfnungszeiten:
SLONG timeDutyOpen  = 10*60000;
SLONG timeDutyClose = 16*60000; //Nur Sa, So
SLONG timeArabOpen  = 11*60000;
SLONG timeLastClose = 16*60000;
SLONG timeMuseOpen  = 11*60000;
SLONG timeReisClose = 17*60000;
SLONG timeMaklClose = 16*60000;
SLONG timeWerbOpen  = 12*60000;

#ifdef CD_PROTECTION
extern BOOL CreditsFilesAreMissing;
#endif

//Daten des aktuellen Savegames beim laden:
extern SLONG SaveVersion;
extern SLONG SaveVersionSub;

//--------------------------------------------------------------------------------------------
//AirportView::AirportView():
//--------------------------------------------------------------------------------------------
AirportView::AirportView (BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "", NULL)
{
   //Per Default kein Objekt für den Editor angeben und keine Personen hinzufügen:
   EditObject    = 0xffffffff;
	PersonsToAdd  = 0;
   bgWarp        = FALSE;

   Editor        = EDITOR_NONE;
   Sim.Players.Players[(SLONG)PlayerNum].CameraSpeed   = XY (0, 0);
   Sim.Players.Players[(SLONG)PlayerNum].ViewPos       = XY (0, 0);

   Sim.Players.Players[Sim.localPlayer].Buttons = 0;

   CenterCameraOnPlayer ();

   if (Sim.FocusPerson==-1)
      FocusCameraOnPos (Sim.Persons[Sim.Persons.GetPlayerIndex(PlayerNum)].ScreenPos, FALSE);
   else
      FocusCameraOnPos (Sim.Persons[Sim.FocusPerson].ScreenPos, TRUE);

   ReloadBitmaps();

   //Zentriert die Kamera vor dem rauskommen aus dem Briefing
   if (Sim.GetMinute()==0 && Sim.GetHour()==9 && Sim.FocusPerson==-1)
      CenterCameraOnPlayer ();

   SDL_ShowWindow(FrameWnd->m_hWnd);
   SDL_UpdateWindowSurface(FrameWnd->m_hWnd);

   if (Sim.ShowExtrablatt!=-1)
   {
      MenuStart (MENU_EXTRABLATT, Sim.ShowExtrablatt);
      MenuSetZoomStuff (XY(320,240), 0.17, FALSE, 10);
      Sim.ShowExtrablatt=-1;

      Sim.Players.Players[Sim.localPlayer].WalkStop ();
   }

   if (!bHandy) AmbientManager.SetGlobalVolume (100);

   LastAnzPeopleOnScreen=-1;

   if (Sim.GetHour()==9 && Sim.GetMinute()==0) bgJustDidLotsOfWork=TRUE;

#ifdef CD_PROTECTION
   if (CreditsFilesAreMissing && Sim.Time>9*60000)
      MenuStart (MENU_CLOSED, MENU_CLOSED_PROTECTION);
#endif

   FlushTalkers ();
}

//--------------------------------------------------------------------------------------------
//AirportView::~AirportView():
//--------------------------------------------------------------------------------------------
AirportView::~AirportView()
{
   bgWarp          = FALSE;
   gShowCursorFeet = -1;
}

//--------------------------------------------------------------------------------------------
//Lädt die Bitmap neu:
//--------------------------------------------------------------------------------------------
void AirportView::ReloadBitmaps (void)
{
}

//--------------------------------------------------------------------------------------------
//Setzt die aktuelle Kameraposition
//--------------------------------------------------------------------------------------------
void AirportView::FocusCameraOnPos (XY Pos, BOOL Speed)
{
   SLONG  Tmp;
   SLONG  SizeX=320;
	static SLONG LastScrollTime;
   static SLONG gMouseScrollSpeed=0;
   XY   &ViewPos = Sim.Players.Players[(SLONG)PlayerNum].ViewPos;
   XY   &CameraSpeed = Sim.Players.Players[(SLONG)PlayerNum].CameraSpeed;

   static SLONG LastPlayerPos;
   static SLONG LastPlayerDeltas[5] = { -1000, -1000, -1000, -1000, -1000 };
   SLONG c, AvgLastPlayerDelta;

   static SLONG LastCameraSpeedX=CameraSpeed.x;
   static SLONG AcceptedCameraSpeedX=CameraSpeed.x;

   if (bgWarp) LastCameraSpeedX=AcceptedCameraSpeedX=CameraSpeed.x=CameraSpeed.y=0;

   memmove (LastPlayerDeltas, LastPlayerDeltas+1, 4*5);
   LastPlayerDeltas[4]=Pos.x-LastPlayerPos;

   AvgLastPlayerDelta=LastPlayerDeltas[0];
   for (c=1; c<5; c++)
   {
      if (abs (AvgLastPlayerDelta/c-LastPlayerDeltas[c])>8)
      {
         AvgLastPlayerDelta=0;
         break;
      }

      AvgLastPlayerDelta+=LastPlayerDeltas[c];
   }

   AvgLastPlayerDelta/=5;

   if (LastPlayerPos!=Pos.x)
      LastPlayerPos=LastPlayerPos*1;

   LastPlayerPos=Pos.x;

   //Command&Conquer-Scrolling:
	if (MouseClickArea==ROOM_AIRPORT && MouseClickId==6010)
   {
      Sim.Players.Players[(SLONG)PlayerNum].CameraSpeed = XY (0, 0);
      if (gMouseScrollSpeed>-44-(gMouseLButton-1)*30) gMouseScrollSpeed-=2;
      if (gMouseScrollSpeed<-44) gMouseScrollSpeed=-44;
      gMouseScroll=TRUE;
   }
	else if (MouseClickArea==ROOM_AIRPORT && MouseClickId==6011)
	{
      Sim.Players.Players[(SLONG)PlayerNum].CameraSpeed = XY (0, 0);
      if (gMouseScrollSpeed<44+(gMouseLButton-1)*30) gMouseScrollSpeed+=2;
      if (gMouseScrollSpeed>44) gMouseScrollSpeed=44;
      gMouseScroll=TRUE;
   }

   //Weiches Scrolling abbremsen
   if ((MouseClickId!=6010 && MouseClickId!=6011) || (gMouseLButton==0 && abs(gMouseScrollSpeed)>8))
   {
      if (gMouseScrollSpeed>0) gMouseScrollSpeed = max (0, gMouseScrollSpeed-4);
      if (gMouseScrollSpeed<0) gMouseScrollSpeed = min (0, gMouseScrollSpeed+4);
   }

   if (gMouseScroll)
   {
      ViewPos.x+=gMouseScrollSpeed;
      if (ViewPos.x<Airport.LeftEnd)      ViewPos.x=Airport.LeftEnd;
      if (ViewPos.x+320>Airport.RightEnd) ViewPos.x=Airport.RightEnd-320;
      if (ViewPos.x<-1000) DebugBreak();
	}
   else gMouseScrollSpeed=0;

   //Automatisches Scrolling
	if (!gMouseScroll)
	{
		if (Sim.Options.OptionFlipping)
		{
			//Spielfigur in X-Richtung zentrieren:
			SizeX=320;

			if (Sim.Players.Players[PlayerNum].DialogWin && 
				 ((CStdRaum*)Sim.Players.Players[PlayerNum].DialogWin)->TempScreenScrollV!=1)
				SizeX/=2;

			CameraSpeed.x=0;

			//X-Flipping:
			if (Pos.x<ViewPos.x+90)       ViewPos.x=Pos.x-540;
			else if (Pos.x>ViewPos.x+550) ViewPos.x=Pos.x-100;

			if (ViewPos.x<Airport.LeftEnd)        ViewPos.x=Airport.LeftEnd;
			if (ViewPos.x+SizeX>Airport.RightEnd) ViewPos.x=Airport.RightEnd-SizeX;

         if (ViewPos.x<-1000) DebugBreak();

			//Spielfigur in Y-Richtung zentrieren:
			Pos.y-=(WinP2.y-WinP1.y)/2;

			if (Pos.y<-10) Pos.y=-10;
			if (Pos.y>440-(WinP2.y-WinP1.y-StatusLineSizeY)) Pos.y=440-(WinP2.y-WinP1.y-StatusLineSizeY);

			if (Speed) ViewPos=XY(Pos.x-SizeX, Pos.y);

			Tmp = CalcInertiaVelocity (Pos.y, ViewPos.y);
			if (Tmp > 0)
			{
				if (Tmp>CameraSpeed.y+3 || (Tmp>CameraSpeed.y && Tmp<4)) CameraSpeed.y++;
				if (Tmp<CameraSpeed.y) CameraSpeed.y=Tmp;
			}
			else if (Tmp < 0)
			{
				if (Tmp<CameraSpeed.y-3 || (Tmp<CameraSpeed.y && Tmp>-4)) CameraSpeed.y--;
				if (Tmp>CameraSpeed.y) CameraSpeed.y=Tmp;
			}
			else CameraSpeed.y=Tmp;

			ViewPos+=CameraSpeed;
         if (ViewPos.x<-1000) DebugBreak();
		}
		else //Scrolling:
		{
			//Spielfigur in X-Richtung zentrieren:
			SizeX=320;

			if (Sim.Players.Players[PlayerNum].DialogWin && 
				 ((CStdRaum*)Sim.Players.Players[PlayerNum].DialogWin)->TempScreenScrollV!=1)
				SizeX/=2;

			Pos.x-=SizeX;

			SLONG UncorrectedPosX=Pos.x;

			//Horizonzale Begrenzung
			if (Pos.x<Airport.LeftEnd)        Pos.x=Airport.LeftEnd;
			if (Pos.x+SizeX>Airport.RightEnd) Pos.x=Airport.RightEnd-SizeX;

			//Spielfigur in Y-Richtung zentrieren:
			Pos.y-=(WinP2.y-WinP1.y)/3;

			if (Pos.y<-10) Pos.y=-10;
			if (Pos.y>440-(WinP2.y-WinP1.y-StatusLineSizeY)) Pos.y=440-(WinP2.y-WinP1.y-StatusLineSizeY);

			if (Speed) ViewPos=Pos;

			//Horizontales Scrolling berechnen:
			if (abs(UncorrectedPosX-ViewPos.x)>SizeX*2/3 || CameraSpeed.x!=0 || timeGetTime()-LastScrollTime<500)
			{
            SLONG CameraSpeedX=CameraSpeed.x;

				Tmp = CalcInertiaVelocity (Pos.x, Pos.x+(ViewPos.x-Pos.x)*2) + AvgLastPlayerDelta;
				if (Tmp>CameraSpeed.x) CameraSpeed.x++;
				if (Tmp<CameraSpeed.x) CameraSpeed.x--;

            //Wenn die Kamera in die falsche Richtung zeigt, oder sich der Spieler entfernt, wird die Geschwindigkeit besonders schnell angepasst
				if (Tmp*CameraSpeed.x<0 || abs (AvgLastPlayerDelta-CameraSpeed.x)>4)
				{
					if (Tmp>CameraSpeed.x) CameraSpeed.x++;
					if (Tmp<CameraSpeed.x) CameraSpeed.x--;
				}

            //Änderungen in der Kamera-Geschwindigkeit nur akzeptieren, wenn sie auch von Dauer sind
            if (AcceptedCameraSpeedX!=CameraSpeed.x)
            {
               if (LastCameraSpeedX==CameraSpeed.x || CameraSpeed.x==0)
               {
                  AcceptedCameraSpeedX=CameraSpeed.x;
               }
               else
               {
                  LastCameraSpeedX=CameraSpeed.x;
                  CameraSpeed.x=AcceptedCameraSpeedX;
               }
            }

				//if (CameraSpeed.x) LastScrollTime=timeGetTime();

				//LastX = Pos.x;
			}
			else if (PlayerDidntMove>5*20)
			{
				Tmp = CalcInertiaVelocity (Pos.x, ViewPos.x);
				if (Tmp>CameraSpeed.x) CameraSpeed.x++;
				if (Tmp<CameraSpeed.x) CameraSpeed.x--;
			}

			//Vertikales Scrolling berechnen:
			Tmp = CalcInertiaVelocity (Pos.y, ViewPos.y);
			if (Tmp > 0)
			{
				if (Tmp>CameraSpeed.y+3 || (Tmp>CameraSpeed.y && Tmp<4)) CameraSpeed.y++;
				if (Tmp<CameraSpeed.y) CameraSpeed.y=Tmp;
			}
			else if (Tmp < 0)
			{
				if (Tmp<CameraSpeed.y-3 || (Tmp<CameraSpeed.y && Tmp>-4)) CameraSpeed.y--;
				if (Tmp>CameraSpeed.y) CameraSpeed.y=Tmp;
			}
			else CameraSpeed.y=Tmp;

         //Verhindern, daß die Kamera am stehenden Spieler vorbeirauscht:
         if (LastPlayerDeltas[4]==0 && abs (CameraSpeed.x)<=2 && (ViewPos.x+CameraSpeed.x-Pos.x)*(ViewPos.x-Pos.x)<=0)
            CameraSpeed.x=0;

         //Scrolling sich auswirken lassen:
         if (CameraSpeed.x) LastScrollTime=timeGetTime();
			ViewPos+=CameraSpeed;
			if (ViewPos.x<Airport.LeftEnd) ViewPos.x=Airport.LeftEnd;

         AcceptedCameraSpeedX=CameraSpeed.x; //new!
		}
	}

   //Wenn man ganz rechts ist, darf man nicht, nach oben gescrollt haben:
   if (ViewPos.y<0 && ViewPos.x>((Airport.LeftEnd+Airport.RightEnd*4)/5))
      ViewPos.y++;
   if (ViewPos.x>Airport.RightEnd-800) ViewPos.y=0;

	//Horizonzale Begrenzung:
	if (ViewPos.x<Airport.LeftEnd)        ViewPos.x=Airport.LeftEnd;
	if (ViewPos.x+SizeX>Airport.RightEnd) ViewPos.x=Airport.RightEnd-SizeX;

   if (ViewPos.x<-1000) DebugBreak();
}

//--------------------------------------------------------------------------------------------
//Bewegt die Kamera ein Stück:
//--------------------------------------------------------------------------------------------
void AirportView::MoveCamera (void)
{
   //Falls keine Editor, dann wird der Focus durch den Spieler bestimmt:
   if (!Editor)
   {
      if (Sim.Players.Players[Sim.localPlayer].LocationWin &&
          (*(CStdRaum*)Sim.Players.Players[Sim.localPlayer].LocationWin).DialogPartner==TALKER_PASSENGER)
          return;

      ULONG PlayerIndex = Sim.Persons.GetPlayerIndex(PlayerNum);

      if (Sim.FocusPerson!=-1)
         if (!Sim.Persons.IsInAlbum (Sim.FocusPerson))
         {
            Sim.FocusPerson=-1;

            XY   &ViewPos = Sim.Players.Players[(SLONG)PlayerNum].ViewPos;

            //Über große Strecken lieber faden als scrollen
            if (abs((Sim.Persons[PlayerIndex].ScreenPos.x-ViewPos.x))>640 && Sim.Options.OptionBlenden)
            {
               ViewPos.x=Sim.Persons[PlayerIndex].ScreenPos.x-320;
               if (FrameWnd) FrameWnd->PrepareFade();
               FrameWnd->Invalidate(); MessagePump();
               gBlendState=-2;
               FrameWnd->Invalidate(); MessagePump();

               if (ViewPos.x<-1000) DebugBreak();
            }
         }

      if (Sim.FocusPerson!=-1)
      {
         PERSON &qPerson = Sim.Persons[Sim.FocusPerson];

         if (qPerson.Mood!=MoodPersonNone && qPerson.StatePar==0 && Sim.Players.Players[SLONG(qPerson.State)].GetRoom()==ROOM_AIRPORT)
            if (Sim.GetHour()>9 || Sim.GetMinute()>4)
               qPerson.MoodCountdown=max (MOODCOUNT_START-16, qPerson.MoodCountdown);

         FocusCameraOnPos (qPerson.ScreenPos, FALSE);
      }
      else if (PlayerIndex!=0xffffffff)
      {
         FocusCameraOnPos (Sim.Persons[PlayerIndex].ScreenPos, FALSE);
         //FocusCameraOnPos (Sim.Persons[PlayerIndex].ScreenPos, FALSE);
      }
   }
   else Sim.TickerTime=0;
}

//--------------------------------------------------------------------------------------------
//Zentriert Kamera auf dem Spieler:
//--------------------------------------------------------------------------------------------
void AirportView::CenterCameraOnPlayer (void)
{
   //Falls keine Editor, dann wird der Focus durch den Spieler bestimmt:
   if (!Editor)
   {
      if (Sim.Players.Players[Sim.localPlayer].LocationWin &&
          (*(CStdRaum*)Sim.Players.Players[Sim.localPlayer].LocationWin).DialogPartner==TALKER_PASSENGER)
          return;

      ULONG PlayerIndex = Sim.Persons.GetPlayerIndex(PlayerNum);

      if (PlayerIndex!=0xffffffff)
      {
         SLONG PosX=Sim.Persons[PlayerIndex].ScreenPos.x;
         SLONG SizeX;
         XY   &ViewPos = Sim.Players.Players[(SLONG)PlayerNum].ViewPos;

         //Spielfigur in X-Richtung zentrieren:
         SizeX=320;

         if (Sim.Players.Players[PlayerNum].DialogWin && 
             ((CStdRaum*)Sim.Players.Players[PlayerNum].DialogWin)->TempScreenScrollV!=1)
            SizeX/=2;

         ViewPos.x=PosX-SizeX;

         if (ViewPos.x<Airport.LeftEnd)        ViewPos.x=Airport.LeftEnd;
         if (ViewPos.x+SizeX>Airport.RightEnd) ViewPos.x=Airport.RightEnd-SizeX;

         if (ViewPos.x<-1000) DebugBreak();

         if (Sim.Persons[PlayerIndex].ScreenPos.y<155)
            ViewPos.y=-10;
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
// AirportView message handlers

//--------------------------------------------------------------------------------------------
//AirportView::OnPaint:
//--------------------------------------------------------------------------------------------
void AirportView::OnPaint() 
{
   ULONG c, d, t1, t2;
   static SLONG Pos=0;
   static UWORD Alpha=0, r=10;
   static UBYTE FlackerCount=0;
   static SLONG ParallaxIndex[5]={-1,-1,-1,-1,-1};      //Die direkten Brick Indices fürs Paralax
   XY   &ViewPos = Sim.Players.Players[(SLONG)PlayerNum].ViewPos;

   if (ViewPos.x<-1000) DebugBreak();

   SLONG  Fl1IndexMin     = Bricks((SLONG)0x10000000+452);
   SLONG  Fl1IndexMax     = Bricks((SLONG)0x10000000+450);
   SLONG  Fl2IndexMin     = Bricks((SLONG)0x10000000+455);
   SLONG  Fl2IndexMax     = Bricks((SLONG)0x10000000+453);
   SLONG  Fl3IndexMin     = Bricks((SLONG)0x10000000+458);
   SLONG  Fl3IndexMax     = Bricks((SLONG)0x10000000+457);
   SLONG  Kamera1Index    = Bricks((SLONG)0x10000000+337);
   SLONG  Kamera2Index    = Bricks((SLONG)0x10000000+338);
   SLONG  GateSmackMin    = bFirstClass ? 0 : Bricks((SLONG)0x10000000+774);
   SLONG  GateSmackMax    = bFirstClass ? 0 : Bricks((SLONG)0x10000000+760);
   SLONG  DoorIndexMin    = Bricks((SLONG)0x10000000+729);
   SLONG  DoorIndexMax    = bFirstClass ? Bricks((SLONG)0x10000000+720) : Bricks((SLONG)0x10000000+718);
   SLONG  LogoBarIndex    = Bricks((SLONG)0x10000000+492);  //Der Logobalken über dem CheckIn
   SLONG  CheckInIndex    = Bricks((SLONG)0x10000000+500);  //Der CheckIn-Schalter
   SLONG  AbflugIndex     = Bricks((SLONG)0x10000000+522);  //Der Abflugschalter
   SLONG  AbflugIndex2    = Bricks((SLONG)0x10000000+521);  //Der Abflugschalter
   SLONG  AbflugWandIndex = Bricks((SLONG)0x10000000+520);  //Die Wand hinter dem Schalter
   SLONG  FloorFIndex     = Bricks((SLONG)0x10000000+5006); //Taxiway
   SLONG  ScannerIndex    = Bricks((SLONG)0x10000000+BRICK_SCANNER_DARKLY);
   SLONG  KioskerIndex    = Bricks((SLONG)0x10000000+841);
   SLONG  KioskerIndexZ   = Bricks((SLONG)0x10000000+852);
   SLONG  RouteBoxIndex   = Bricks((SLONG)0x10000000+421);

   SLONG  RightClip = 640;

   static SLONG LastAnzElements=-2;
   static SLONG LastViewPosX=ViewPos.x;
   SLONG  AnzElements=0;

   long cnt=0;
   for (c=0; c<(ULONG)Sim.AirportSmacks.AnzEntries(); c++)
   {
      if (Sim.AirportSmacks[(SLONG)c].pSmack)
      {
         if (!Sim.AirportSmacks[(SLONG)c].Next (&Sim.AirportSmacks[(SLONG)c].Bitmap))
         {
            Bricks[SLONG(0x10000000+Sim.AirportSmacks[(SLONG)c].BrickId)].Bitmap[0].FillWith(0);
            Sim.AirportSmacks[(SLONG)c].Close();
         }
         else
         {
            cnt++;
            Bricks[SLONG(0x10000000+Sim.AirportSmacks[(SLONG)c].BrickId)].Bitmap[0].BlitFrom (Sim.AirportSmacks[(SLONG)c].Bitmap, Sim.AirportSmacks[(SLONG)c].Offset.x, Sim.AirportSmacks[(SLONG)c].Offset.y);
         }
      }
   }
   if (cnt==0) Sim.AirportSmacks.ReSize(0);

   if (bLeaveGameLoop) return;
   if (Sim.CallItADay) return;
   if (Sim.Date>0 && Sim.GetHour()==9 && Sim.GetMinute()==0 && MouseWait>0) return;

   if (Sim.Players.Players[Sim.localPlayer].DialogWin && ((CStdRaum*)Sim.Players.Players[Sim.localPlayer].DialogWin)->bHandy)
   {
      RightClip = ((CStdRaum*)Sim.Players.Players[Sim.localPlayer].DialogWin)->TempScreenScroll;
   }

   SLONG DoorOpenTab[10];

   //DutyFree:
        if ((Sim.Weekday==5 || Sim.Weekday==6) && SLONG(Sim.Time)>timeDutyClose+2000) DoorOpenTab[0]=0;
   else if ((Sim.Weekday==5 || Sim.Weekday==6) && SLONG(Sim.Time)>timeDutyClose && SLONG(Sim.Time)<=timeDutyClose+2000) DoorOpenTab[0]=256-(Sim.Time-timeDutyClose)*256/2000;
   else if (SLONG(Sim.Time)<timeDutyOpen-2000) DoorOpenTab[0]=0;
   else if (SLONG(Sim.Time)>timeDutyOpen) DoorOpenTab[0]=256;
   else DoorOpenTab[0]=(Sim.Time-(timeDutyOpen-2000))*256/2000;

   //ArabAir
   if (SLONG(Sim.Time)<timeArabOpen-2000 || Sim.Weekday==6) DoorOpenTab[1]=0;
   else if (SLONG(Sim.Time)>timeArabOpen) DoorOpenTab[1]=256;
   else DoorOpenTab[1]=(Sim.Time-(timeArabOpen-2000))*256/2000;

   //Museum
   if (SLONG(Sim.Time)<timeMuseOpen-2000 || Sim.Weekday==5 || Sim.Weekday==6) DoorOpenTab[3]=0;
   else if (SLONG(Sim.Time)>timeMuseOpen) DoorOpenTab[3]=256;
   else DoorOpenTab[3]=(Sim.Time-(timeMuseOpen-2000))*256/2000;

   //Makler
   if (SLONG(Sim.Time)>timeMaklClose) DoorOpenTab[5]=0;
   else DoorOpenTab[5]=256;

   //Werbung
   if (SLONG(Sim.Time)<timeWerbOpen || Sim.Weekday==5 || Sim.Weekday==6) DoorOpenTab[6]=0;
   else DoorOpenTab[6]=256;

   if (RobotUse(ROBOT_USE_TRAVELHOLDING))
   {
      //LastMinute
      if (SLONG(Sim.Time)>timeLastClose+2000 || Sim.Weekday==5) DoorOpenTab[2]=0;
      else if (SLONG(Sim.Time)<timeLastClose) DoorOpenTab[2]=256;
      else DoorOpenTab[2]=256-(Sim.Time-timeLastClose)*256/2000;

      //Reisebüro
      if (SLONG(Sim.Time)>timeReisClose+2000) DoorOpenTab[4]=0;
      else if (SLONG(Sim.Time)<timeReisClose) DoorOpenTab[4]=256;
      else DoorOpenTab[4]=256-(Sim.Time-timeReisClose)*256/2000;
   }
   else
   {
      //Closed
      DoorOpenTab[2]=0;
      DoorOpenTab[4]=0;
   }

   //Wenn ein Raumbild offen (und davor ist), dann nicht neu zeichnen:
   if (TopWin==NULL)
   {
      //Ggf. Parallax Tabelle initialisieren:
      if (ParallaxIndex[0]==-1)
         for (c=0; c<5; c++)
            ParallaxIndex[c]=Bricks(0x10000000+5000+c);

      Alpha+=256;
	   FlackerCount++;

	   if (PersonsToAdd && rand()%4==0 && Sim.Persons.GetNumFree()>20)
	   {
         Sim.Persons*=PERSON (Clans.GetCustomerId(0, 99), Airport.GetRandomBirthplace(0), REASON_SHOPPING, 99, 0, 0);
		   PersonsToAdd--;
	   }

      if (bActive)
      {
         //Sowas darf nur das Hauptfenster, was immer links oben ist:
         if (Editor && WinP1.x==0 && WinP1.y==0)
            PrimaryBm.Clear (0x000000);

         UnderCursor = 0xffffffff;

         //Under-Cursor Berechnung:
         if (Editor==EDITOR_BUILDS && EditObject==0xffffffff)
         {
            for (SLONG c=Airport.Builds.AnzEntries()-1; c>=0; c--)
               if (Airport.Builds.IsInAlbum(c) && Bricks.IsInAlbum(Airport.Builds[c].BrickId))
               if (gMousePosition.x+2+ViewPos.x >= Airport.Builds[c].ScreenPos.x && gMousePosition.x+2+ViewPos.x <= Airport.Builds[c].ScreenPos.x+Bricks[Airport.Builds[c].BrickId].GetBitmapDimension().x)
               if (gMousePosition.y+2+ViewPos.y >= Airport.Builds[c].ScreenPos.y && gMousePosition.y+2+ViewPos.y <= Airport.Builds[c].ScreenPos.y+Bricks[Airport.Builds[c].BrickId].GetBitmapDimension().y)
               if (!Bricks[Airport.Builds[c].BrickId].IsGlasAt (gMousePosition.x+2-Airport.Builds[c].ScreenPos.x+ViewPos.x, gMousePosition.y+2-Airport.Builds[c].ScreenPos.y+ViewPos.y))
               {
                  UnderCursor=c;
                  break;
               }
         }

         //Parallax Horizont mit Clipping einbauen:
         {
            SLONG sizes[5];
            SLONG c;
            SLONG RangeDrawn=0;

            sizes[0]=Bricks[ParallaxIndex[0]].GetBitmapDimension().x;
            sizes[1]=Bricks[ParallaxIndex[1]].GetBitmapDimension().x;
            sizes[2]=Bricks[ParallaxIndex[2]].GetBitmapDimension().x;
            sizes[3]=Bricks[ParallaxIndex[3]].GetBitmapDimension().x;
            sizes[4]=Bricks[ParallaxIndex[4]].GetBitmapDimension().x;

            //Video-Support:
            static XY LastViewPosA, LastViewPosB;

            gScrollOffsetA += ViewPos-LastViewPosA;
            LastViewPosA    = ViewPos;

            if (sizes[4]>0)
            {
               gScrollOffsetB.x += LastViewPosB.x-(100000-(ViewPos.x*56)>>6)%sizes[4];
               gScrollOffsetB.y += 0;
               LastViewPosB.x    = (100000-(ViewPos.x*56)>>6)%sizes[4];
            }

            for (c=1; c<Airport.ClipMarkers.AnzEntries(); c++)
            {
               SLONG MiddleHeight=168;

               if (Airport.ClipMarkers[c-1].Type!=RUNE_CLIPFULL && Airport.ClipMarkers[c].Position>=ViewPos.x)
               {
                  SLONG d;

                  switch (Airport.ClipMarkers[c-1].Type)
                  {
                     case RUNE_CLIPMIDDLE: //Büros verdecken
                        PrimaryBm.PrimaryBm.SetClipRect(&CRect(max(RangeDrawn-ViewPos.x,0),MiddleHeight,min(Airport.ClipMarkers[c].Position-ViewPos.x,RightClip),440));
                        break;
                     case RUNE_CLIPNONE: //Nix verdeckt
                        PrimaryBm.PrimaryBm.SetClipRect(&CRect(max(RangeDrawn-ViewPos.x,0),0,min(Airport.ClipMarkers[c].Position-ViewPos.x,RightClip),440));
                        break;
                     default:
                        DebugBreak();
                  }

                  if (sizes[0]+sizes[1]>0)
                     for (d=640/(sizes[0]+sizes[1])+2; d>=0; d--)
                     {
                        BrickWait[ParallaxIndex[0]].Start();
                        Bricks[ParallaxIndex[0]].BlitAt (PrimaryBm, 0, (100000-(ViewPos.x*16)>>6)%(sizes[0]+sizes[1])-(sizes[0]+sizes[1])+d*(sizes[0]+sizes[1]), WinP1.y-18+18);
                        Bricks[ParallaxIndex[1]].BlitAt (PrimaryBm, 0, (100000-(ViewPos.x*16)>>6)%(sizes[0]+sizes[1])-(sizes[0]+sizes[1])+sizes[0]+d*(sizes[0]+sizes[1]), WinP1.y-18+18);
                        BrickWait[ParallaxIndex[0]].Stop();
                        AnzElements++;
                     }

                  if (sizes[2]>0)
                     for (d=640/sizes[2]+2; d>=0; d--)
                     {
                        BrickWait[ParallaxIndex[2]].Start();
                        Bricks[ParallaxIndex[2]].BlitAt (PrimaryBm, 0, (100000-(ViewPos.x*30)>>6)%sizes[2]-sizes[2]+d*sizes[2], WinP1.y-18+18+34);
                        BrickWait[ParallaxIndex[2]].Stop();
                        AnzElements++;
                     }

                  if (sizes[3]>0)
                     for (d=640/sizes[3]+2; d>=0; d--)
                     {
                        BrickWait[ParallaxIndex[3]].Start();
                        Bricks[ParallaxIndex[3]].BlitAt (PrimaryBm, 0, (100000-(ViewPos.x*42)>>6)%sizes[3]-sizes[3]+d*sizes[3], WinP1.y-18+18+34+44);
                        BrickWait[ParallaxIndex[3]].Stop();
                        AnzElements++;
                     }

                  if (sizes[4]>0)
                     for (d=640/sizes[4]+2; d>=0; d--)
                     {
                        BrickWait[ParallaxIndex[4]].Start();
                        Bricks[ParallaxIndex[4]].BlitAt (PrimaryBm, 0, (100000-(ViewPos.x*56)>>6)%sizes[4]-sizes[4]+d*sizes[4], WinP1.y-18+18+34+44+72);
                        BrickWait[ParallaxIndex[4]].Stop();
                        AnzElements++;
                     }
               }

               RangeDrawn=Airport.ClipMarkers[c].Position;

               if (RangeDrawn>=ViewPos.x+640) break;
            }

            PrimaryBm.PrimaryBm.SetClipRect(&CRect(0,0,RightClip,440));

            //Runway rechts aussen:
            Bricks[FloorFIndex].BlitAt (PrimaryBm, 0, (Airport.RightEnd-ViewPos.x)-20-89, WinP1.y+34+44+72+70);
         }

         PrimaryBm.PrimaryBm.SetClipRect(&CRect(0,0,RightClip,440));

         //Flugzeuge von allen Spielern auf dem Runway zeigen:
         if (Sim.Options.OptionPlanes)
         {
            for (c=0; c<(ULONG)Sim.Players.AnzPlayers; c++)
            {
               if (!Sim.Players.Players[(SLONG)c].IsOut)
               {
                  //Für alle Flugzeuge die er besitzt
                  for (d=0; d<Sim.Players.Players[(SLONG)c].Planes.AnzEntries(); d++)
                  {
                     if (Sim.Players.Players[(SLONG)c].Planes.IsInAlbum (d))
                     {
                        if (Sim.Players.Players[(SLONG)c].Planes[(SLONG)d].Ort==-1 || Sim.Players.Players[(SLONG)c].Planes[(SLONG)d].Ort==-2)
                        {
                           if (Sim.Players.Players[(SLONG)c].Planes[(SLONG)d].TypeId!=-1)
                           {
                              PlaneTypes.BlitPlaneAt (PrimaryBm,
                                 Sim.Players.Players[(SLONG)c].Planes[(SLONG)d].TypeId,
                                 1,
                                 Sim.Players.Players[(SLONG)c].Planes[(SLONG)d].AirportPos-XY(ViewPos.x/2, 0)+XY(0,14),
                                 c);
                           }
                           else
                           {
                              Sim.Players.Players[(SLONG)c].Planes[(SLONG)d].XPlane.BlitPlaneAt (PrimaryBm,
                                 1,
                                 Sim.Players.Players[(SLONG)c].Planes[(SLONG)d].AirportPos-XY(ViewPos.x, ViewPos.y-5),
                                 c);
                           }
                        }
                     }
                  }
               }
            }

            //Flugzeuge von allen Spielern hinter Glas zeigen:
            for (c=0; c<(ULONG)Sim.Players.AnzPlayers; c++)
            {
               if (!Sim.Players.Players[(SLONG)c].IsOut)
               {
                  //Für alle Flugzeuge die er besitzt
                  for (d=0; d<Sim.Players.Players[(SLONG)c].Planes.AnzEntries(); d++)
                  {
                     if (Sim.Players.Players[(SLONG)c].Planes.IsInAlbum (d))
                     {
                        if (Sim.Players.Players[(SLONG)c].Planes[(SLONG)d].Ort==-3)
                        {
                           if (Sim.Players.Players[(SLONG)c].Planes[(SLONG)d].TypeId!=-1)
                           {
                              PlaneTypes.BlitPlaneAt (PrimaryBm,
                                 Sim.Players.Players[(SLONG)c].Planes[(SLONG)d].TypeId,
                                 2,
                                 Sim.Players.Players[(SLONG)c].Planes[(SLONG)d].AirportPos-XY(ViewPos.x, ViewPos.y-5),
                                 c);
                           }
                           else
                           {
                              Sim.Players.Players[(SLONG)c].Planes[(SLONG)d].XPlane.BlitPlaneAt (PrimaryBm,
                                 2,
                                 Sim.Players.Players[(SLONG)c].Planes[(SLONG)d].AirportPos-XY(ViewPos.x, ViewPos.y-5),
                                 c);
                           }
                        }
                     }
                  }
               }
            }
         }

         //Draw Builds & Persons:
         c=d=0;

         BUILDS *pBuilds;

         //Was nehmen wir? Den ganzen Flughafen oder die Hash-Bricks?
         if (Editor!=EDITOR_NONE)
            pBuilds=&Airport.Builds;
         else
         {
            SLONG Index=(ViewPos.x-Airport.LeftEnd)/BUILDHASHSIZE;

            if (Index<0 || Index>=Airport.HashBuilds.AnzEntries()) DebugBreak();

            pBuilds=&Airport.HashBuilds[Index];
         }

         Bench.AdminTime.Start();
         while ((c<pBuilds->AnzEntries() && !pBuilds->IsInAlbum(c)) || (Editor==EDITOR_NONE && pBuilds->IsInAlbum(c) && (*pBuilds)[c].BrickId>=0x10000000+2000))
            c++;

         if (!Sim.Persons.IsInAlbum(d)) d=Sim.Persons.AnzEntries();

         if (c<pBuilds->AnzEntries())
         {
            t1 = (long(Bricks[(*pBuilds)[c].BrickId].Layer)<<16) + (*pBuilds)[c].ScreenPos.y + Bricks[(*pBuilds)[c].BrickId].GetBitmapDimension().y+Bricks[(*pBuilds)[c].BrickId].FloorOffset;
         }
         else t1 = 0x7fffffff;

         if (d<Sim.Persons.AnzEntries())
            if (Sim.Persons[d].ScreenPos.y<170) t2 = (60<<16) + Sim.Persons[d].ScreenPos.y;
            else                                t2 = (20<<16) + Sim.Persons[d].ScreenPos.y;
         else t2 = 0x7fffffff;

         AnzPeopleOnScreen=0;

         do
         {
            while (t2<=t1 && t2!=0x7fffffff)
            {
               //Entscheidung! Person malen:
               if (Sim.Persons.IsInAlbum(d) && Clans.IsInAlbum (Sim.Persons[d].ClanId))
               {
                  if (Sim.Options.OptionPassengers || Clans[(SLONG)Sim.Persons[d].ClanId].Type>=CLAN_PLAYER1)
                  {
                     PERSON &qPerson=Sim.Persons[d];
                     CLAN   &qClan=Clans[(SLONG)qPerson.ClanId];

                     Bench.AdminTime.Stop();

                     if (Sim.DontDisplayPlayer==-1 || qClan.Type!=CLAN_PLAYER1+Sim.DontDisplayPlayer)
                     {
                        if ((qClan.Type<CLAN_PLAYER1 && (qPerson.LookDir==6 || qPerson.LookDir==7)) || qClan.Group>30)
                           qClan.BlitAt (PrimaryBm, qPerson.LookDir, qPerson.Phase, qPerson.ScreenPos-ViewPos+WinP1-XY(8-1,0-1), qPerson.Running);
                        else
                           qClan.BlitAt (PrimaryBm, qPerson.LookDir, qPerson.Phase, qPerson.ScreenPos-ViewPos+WinP1-XY(8-1,0-1)+XY(qPerson.FlightPlaneIndex&3, (qPerson.FlightPlaneIndex>>2)&3), qPerson.Running && (qPerson.Position.y/22+5!=13 || (Airport.iPlate[(qPerson.Position.y/22+5)+((qPerson.Position.x/44)<<4)] & 240)!=80));

                        if (Sim.Options.OptionThinkBubbles && qPerson.MoodCountdown)
                        {
                           SLONG Grade=3;

                           if (qPerson.MoodCountdown<=MOODCOUNT_START && (qPerson.ScreenPos.y>219 || qPerson.ScreenPos.y<150))
                           {
                              if (MOODCOUNT_START-qPerson.MoodCountdown<11)
                                 Grade=8-(MOODCOUNT_START-qPerson.MoodCountdown)/2;
                              else if (qPerson.MoodCountdown<11)
                                 Grade=8-qPerson.MoodCountdown/2;

                              if (Grade>=-1 && Grade<=8)
                              {
                                 if ((qPerson.Mood>=MoodPersonBase2 && qPerson.Mood<=MoodPersonBase3) || qPerson.Mood>=MoodPersonRepairFC && qPerson.Mood<=MoodPerson3StarsFC)
                                    MoodBms[qPerson.Mood].BlitFrom (SmallLogoBms[qPerson.FlightAirline], XY(8,10));

                                 ColorFX.BlitTrans (MoodBms[qPerson.Mood].pBitmap, &PrimaryBm.PrimaryBm, qPerson.ScreenPos-ViewPos+XY(8-1-MoodBms[qPerson.Mood].Size.x*4/5,-MoodBms[qPerson.Mood].Size.y-qClan.Phasen[0][0].Size.y-1), NULL, Grade);
                              }
                           }
                        } 

                        if (qClan.Type>=CLAN_PLAYER1 && qClan.Type<=CLAN_PLAYER4)
                        {
                           PLAYER &qPlayer = Sim.Players.Players[qClan.Type-CLAN_PLAYER1];

                           if (qPlayer.PlayerStinking)
                           {
                              for (SLONG c=0; c<20; c++)
                                 if (qPlayer.Smoke[c].TTL)
                                 {
                                    ColorFX.BlitTrans (gStenchBms[0].pBitmap, &PrimaryBm.PrimaryBm, qPlayer.Smoke[c].Position-ViewPos, NULL, 8-min(qPlayer.Smoke[c].TTL/2,8));

                                    qPlayer.Smoke[c].Position.y--;
                                    qPlayer.Smoke[c].TTL--;
                                 }

                              if (qPlayer.PlayerStinking>15 && rand()%4==0)
                              {
                                 for (SLONG c=0; c<20; c++)
                                    if (!qPlayer.Smoke[c].TTL)
                                    {
                                       qPlayer.Smoke[c].Position=qPerson.ScreenPos;
                                       qPlayer.Smoke[c].Position.x+=rand()%20-23;
                                       qPlayer.Smoke[c].Position.y-=rand()%60+25;
                                       qPlayer.Smoke[c].TTL=10+rand()%10;
                                       break;
                                    }
                              }

                              if (qPlayer.PlayerStinking<9999) qPlayer.PlayerStinking--;
                           }
                           else if (qPlayer.PlayerSmoking)
                           {
                              for (SLONG c=0; c<20; c++)
                                 if (qPlayer.Smoke[c].TTL)
                                 {
                                    ColorFX.BlitTrans (gSmokeBms[0].pBitmap, &PrimaryBm.PrimaryBm, qPlayer.Smoke[c].Position-ViewPos, NULL, 8-min(qPlayer.Smoke[c].TTL/2,8));

                                    qPlayer.Smoke[c].Position.y--;
                                    qPlayer.Smoke[c].TTL--;
                                 }

                              if (qPlayer.PlayerSmoking>15 && rand()%4==0)
                              {
                                 for (SLONG c=0; c<20; c++)
                                    if (!qPlayer.Smoke[c].TTL)
                                    {
                                       qPlayer.Smoke[c].Position=qPerson.ScreenPos;
                                       qPlayer.Smoke[c].Position.x+=rand()%20-23;
                                       qPlayer.Smoke[c].Position.y-=rand()%60+25;
                                       qPlayer.Smoke[c].TTL=10+rand()%10;
                                       break;
                                    }
                              }

                              if (qPlayer.PlayerSmoking<99) qPlayer.PlayerSmoking--;
                           }
                        }

                        if (qClan.Type==CLAN_ANIMATION && qClan.Group==20 && qPerson.State!=255)
                        {
                           CSmoker &qSmoker = Smokers[qPerson.State];

                           if (qSmoker.Smoking && qSmoker.Smoke)
                           {
                              for (SLONG c=0; c<20; c++)
                                 if (qSmoker.Smoke[c].TTL)
                                 {
                                    ColorFX.BlitTrans (gStenchBms[0].pBitmap, &PrimaryBm.PrimaryBm, qSmoker.Smoke[c].Position-ViewPos, NULL, 8-min(qSmoker.Smoke[c].TTL/2,8));

                                    if (rand()%3==0)
                                       qSmoker.Smoke[c].Position.x+=qSmoker.Smoke[c].vx;

                                    qSmoker.Smoke[c].Position.y--;
                                    qSmoker.Smoke[c].TTL--;
                                 }

                              if (qSmoker.Smoking>15 && rand()%4==0)
                              {
                                 for (SLONG c=0; c<qSmoker.Smoke.AnzEntries(); c++)
                                    if (!qSmoker.Smoke[c].TTL)
                                    {
                                       qSmoker.Smoke[c].vx = rand()%3-1;
                                       qSmoker.Smoke[c].Position=qPerson.ScreenPos+XY(-10,0);
                                       qSmoker.Smoke[c].Position.x+=qSmoker.Smoke[c].vx*(rand()%10)+rand()%5;
                                       qSmoker.Smoke[c].Position.y-=rand()%60+10;
                                       qSmoker.Smoke[c].TTL=10+rand()%10;
                                       break;
                                    }
                              }
                           }
                        }
                     }
                     Bench.AdminTime.Start();
                  }
               }

               d++;
               if (!Sim.Persons.IsInAlbum(d))
                  d=Sim.Persons.AnzEntries();
               //do d++; while (d<Sim.Persons.AnzEntries() && !Sim.Persons.IsInAlbum(d));

               if (d<Sim.Persons.AnzEntries())
                  if (Sim.Persons[d].ScreenPos.y<170) t2 = (60<<16) + Sim.Persons[d].ScreenPos.y;
                  else                                t2 = (20<<16) + Sim.Persons[d].ScreenPos.y;
               else t2 = 0x7fffffff;
            }

            while (t1<t2 && t1!=0x7fffffff)
            {
               if (Editor!=EDITOR_BUILDS || UnderCursor!=c || (FlackerCount&1)==0)
                  if (pBuilds->IsInAlbum(c) && Bricks.IsInAlbum ((*pBuilds)[c].BrickId))
                  {
                     BUILD &qBuild=(*pBuilds)[c];
                     SLONG BrickId=qBuild.BrickId;
                     BRICK &qBrick=Bricks[BrickId];

                     //BrickWait[BrickId].Start();

                     //Bench.AdminTime.Stop();
                     if (qBrick.Triggered==1 && !Editor)
                     {
                        if (qBuild.BrickId==KioskerIndex)
                        {
                           if (Sim.Headlines.IsInteresting)
                           {
                              qBrick.Triggered=0;
                              Bricks[BrickId].BlitAt (PrimaryBm, 0, qBuild.ScreenPos-ViewPos+WinP1);
                              qBrick.Triggered=1;
                           }
                        }
                        else if (qBuild.BrickId==RouteBoxIndex)
                        {
                           if (Sim.ItemClips)
                           {
                              qBrick.Triggered=0;
                              Bricks[BrickId].BlitAt (PrimaryBm, 0, qBuild.ScreenPos-ViewPos+WinP1);
                              qBrick.Triggered=1;
                           }
                           else
                           {
                              qBrick.AnimSpeed=1;
                              Bricks[BrickId].BlitAt (PrimaryBm, 0, qBuild.ScreenPos-ViewPos+WinP1, 1);
                              qBrick.AnimSpeed=0;
                           }
                        }
                        else if (qBuild.BrickId==KioskerIndexZ)
                        {
                           if (!Sim.Headlines.IsInteresting)
                           {
                              qBrick.Triggered=0;
                              Bricks[BrickId].BlitAt (PrimaryBm, 0, qBuild.ScreenPos-ViewPos+WinP1);
                              qBrick.Triggered=1;
                           }
                        }
                        else
                        {
                           if (Airport.Triggers[(SLONG)qBuild.Par].Winkel && (Sim.TickerTime-Airport.Triggers[(SLONG)qBuild.Par].Winkel)/qBrick.AnimSpeed>=ULONG(qBrick.Bitmap.AnzEntries()))
                           {
                              Airport.Triggers[(SLONG)qBuild.Par].Winkel=0;

                              if (Sim.DontDisplayPlayer!=-1 && qBuild.BrickId==long(Bricks((SLONG)0x10000000+BRICK_ELECTRO)))
                              {
                                 PLAYER &qPlayer = Sim.Players.Players[Sim.DontDisplayPlayer];

                                 Sim.DontDisplayPlayer = -1;
                                 qPlayer.PlayerSmoking = max (qPlayer.PlayerSmoking, 60);

                                 for (SLONG c=0; c<20; c++)
                                    qPlayer.Smoke[c].TTL=0;

                                 for (c=0; c<4; c++)
                                 {
                                    qPlayer.Smoke[c].Position=Sim.Persons[Sim.Persons.GetPlayerIndex (Sim.localPlayer)].ScreenPos;
                                    qPlayer.Smoke[c].Position.x+=rand()%20-23;
                                    qPlayer.Smoke[c].Position.y-=rand()%60+25;
                                    qPlayer.Smoke[c].TTL=10+rand()%10;
                                 }
                              }
                           }

                           if (Airport.Triggers[(SLONG)qBuild.Par].Winkel)
                              qBrick.BlitAt (PrimaryBm, 0, qBuild.ScreenPos-ViewPos+WinP1, (Sim.TickerTime-Airport.Triggers[(SLONG)qBuild.Par].Winkel)/qBrick.AnimSpeed);
                           else
                              qBrick.BlitAt (PrimaryBm, 0, qBuild.ScreenPos-ViewPos+WinP1, 0);
                        }
                     }
                     else if (qBrick.Triggered==2 && !Editor)
                     {
                        RECT rect;
                        RECT r2;

                        r2.top    = qBuild.ScreenPos.y-ViewPos.y+WinP1.y;
                        r2.left   = qBuild.ScreenPos.x-ViewPos.x+WinP1.x;
                        r2.bottom = r2.top  + qBrick.Bitmap[0].Size.y;
                        r2.right  = r2.left + qBrick.Bitmap[0].Size.x;

                        rect = PrimaryBm.PrimaryBm.GetClipRect();
                        PrimaryBm.PrimaryBm.SetClipRect(&r2);
                        qBrick.BlitAt (PrimaryBm, 0, qBuild.ScreenPos-ViewPos+WinP1-XY(0, (qBrick.Bitmap[0].Size.y*DoorOpenTab[qBrick.BaseOffset.y])>>8));
                        PrimaryBm.PrimaryBm.SetClipRect(&rect);
                     }
                     else if (BrickId>=DoorIndexMin && BrickId<=DoorIndexMax)
                        //Tür bekommt als Parameter die Öffnungsweite
                        qBrick.BlitAt (PrimaryBm, 0, qBuild.ScreenPos-ViewPos+WinP1, Airport.Doors[(SLONG)qBuild.Par].Winkel);
                     //Fließband hat 'n kranken Assisstenten:
                     else if (BrickId>=Fl1IndexMin && BrickId<=Fl1IndexMax)
                        qBrick.BlitAt (PrimaryBm, 0, qBuild.ScreenPos-ViewPos+WinP1, (((qBuild.ScreenPos.x+qBuild.ScreenPos.y/2)/44+qBuild.ScreenPos.y/22)*2)%7+100);
                     else if (BrickId>=Fl2IndexMin && BrickId<=Fl2IndexMax)
                        qBrick.BlitAt (PrimaryBm, 0, qBuild.ScreenPos-ViewPos+WinP1, (((qBuild.ScreenPos.x+qBuild.ScreenPos.y/2)/44+qBuild.ScreenPos.y/22))%7+100);
                     else if (BrickId>=Fl3IndexMin && BrickId<=Fl3IndexMax)
                        qBrick.BlitAt (PrimaryBm, 0, qBuild.ScreenPos-ViewPos+WinP1, 6-((((qBuild.ScreenPos.x+qBuild.ScreenPos.y/2)/44+qBuild.ScreenPos.y/22)*2)%7)+100);
                     else if (Editor!=EDITOR_BUILDS && BrickId==LogoBarIndex)
                        qBrick.BlitAt (PrimaryBm, 0, qBuild.ScreenPos-ViewPos+WinP1, Airport.GateMapper[(SLONG)qBuild.Par]+1);
                     else if (Editor!=EDITOR_BUILDS && (BrickId==Kamera1Index || BrickId==Kamera2Index))
                        qBrick.BlitAt (PrimaryBm, 0, qBuild.ScreenPos-ViewPos+WinP1, Sim.Players.Players[Airport.GateMapper[(SLONG)qBuild.Par]].SecurityFlags&(1<<0));
                     else if (Editor!=EDITOR_BUILDS && BrickId>=GateSmackMin && BrickId<=GateSmackMax)
                     {
                        if (Sim.Players.Players[Airport.GateMapper[(SLONG)qBuild.Par]].SecurityFlags&(1<<8))
                           qBrick.BlitAt (PrimaryBm, 0, qBuild.ScreenPos-ViewPos+WinP1, 0); //Ggf erweitertes Gate
                     }
                     else
                     {
                        if (BrickId==AbflugIndex || BrickId==AbflugIndex2)
                        {
                           long phase = 0;
                           
                           if (Sim.Players.Players[Airport.GateMapper[(SLONG)qBuild.Par]].SecurityFlags&(1<<8))  phase=1;
                           if (Sim.Players.Players[Airport.GateMapper[(SLONG)qBuild.Par]].SecurityFlags&(1<<10)) phase=2;
                           Bricks[BrickId].BlitAt (PrimaryBm, 0, qBuild.ScreenPos-ViewPos+WinP1, phase);
                        }
                        else
                           Bricks[BrickId].BlitAt (PrimaryBm, 0, qBuild.ScreenPos-ViewPos+WinP1);

                        if (Editor!=EDITOR_BUILDS && BrickId==AbflugIndex && Airport.GateMapper[(SLONG)qBuild.Par]!=-1)
                           PrimaryBm.BlitFrom (SmallLogoBms[Airport.GateMapper[(SLONG)qBuild.Par]], qBuild.ScreenPos-ViewPos+XY(17, 38+38));
                        else if (Editor!=EDITOR_BUILDS && BrickId==CheckInIndex)
                           PrimaryBm.BlitFromT (TextBricks[(SLONG)qBuild.Par], qBuild.ScreenPos-ViewPos+XY(6, 2));
                        else if (Editor!=EDITOR_BUILDS && BrickId==AbflugWandIndex)
                        {
                           if (Airport.GateMapper[(SLONG)qBuild.Par]!=-1)
                           {
                              PrimaryBm.BlitFromT (TextBricks[(SLONG)qBuild.Par+TextBricks.AnzEntries()/2], qBuild.ScreenPos-ViewPos+XY(43, 5));
                              PrimaryBm.BlitFrom (TinyLogoBms[Airport.GateMapper[(SLONG)qBuild.Par]], qBuild.ScreenPos-ViewPos+XY(20, 5));
                              PrimaryBm.BlitFrom (TinyLogoBms[Airport.GateMapper[(SLONG)qBuild.Par]], qBuild.ScreenPos-ViewPos+XY(134, 5));
                           }
                        }
                     }

                     AnzElements++;

                     //Durchleuchter-screen blitten?
                     if (Editor!=EDITOR_BUILDS && BrickId==ScannerIndex)
                     {
                        XY p=qBuild.ScreenPos-ViewPos+WinP1;
                        PrimaryBm.PrimaryBm.SetClipRect(&CRect(p.x+4, p.y+7, p.x+74,p.y+74));
                        
                        for (SLONG d=0; d<SLONG(Sim.Persons.AnzEntries()); d++)
                        {
                           if (Sim.Persons.IsInAlbum(d) && Clans.IsInAlbum (Sim.Persons[d].ClanId))
                           {
                              PERSON &qPerson=Sim.Persons[d];
                              CLAN   &qClan=Clans[(SLONG)qPerson.ClanId];
                              XY      pp=qPerson.ScreenPos-ViewPos+WinP1-XY(8-1,0-1)+XY(qPerson.FlightPlaneIndex&3, (qPerson.FlightPlaneIndex>>2)&3);

                              if (pp.x<p.x+154 && pp.x>p.x-70 && abs(pp.y-(p.y+72))<14)
                                 qClan.BlitSkelettAt (PrimaryBm, qPerson.LookDir, qPerson.Phase, pp);
                           }
                           else break;
                        }

                        PrimaryBm.PrimaryBm.SetClipRect(&CRect(0,0,640,440));
                     }

                     if (Editor==EDITOR_BUILDS && ((BrickId >=0x10000000+2030 && BrickId<=0x10000000+2035) || BrickId==0x10000000+2045 || BrickId==0x10000000+2001 || BrickId==0x10000000+2003 || BrickId==0x10000000+RUNE_AREALO || BrickId==0x10000000+RUNE_AREARU || BrickId==0x10000000+500 || BrickId==0x10000000+521 || BrickId==0x10000000+522 || BrickId==0x10000000+520 || BrickId==0x10000000+492 || (BrickId>=0x10000000+760 && BrickId<=0x10000000+774) || BrickId==0x10000000+RUNE_WAITPLANE || BrickId==0x10000000+RUNE_DROPSUITCASE || BrickId==0x10000000+RUNE_WAYPOINT || BrickId==0x10000000+RUNE_WAYPOINT_WAIT || BrickId==0x10000000+RUNE_WAYPOINT_START || BrickId==0x10000000+RUNE_WAYPOINT_G || BrickId==0x10000000+RUNE_CONDBLOCK || BrickId==0x10000000+337 || BrickId==0x10000000+338))
                     if (qBuild.ScreenPos.x-ViewPos.x+WinP1.x>-10 && qBuild.ScreenPos.x-ViewPos.x+WinP1.x<650)
                     {
                        SDL_Surface* Surf=PrimaryBm.PrimaryBm.GetSurface();

                        TTF_Font* Font = TTF_OpenFont("arial.ttf", 9); // Arial
                        if (Font)
                        {
                           SDL_Color bg = { 0, 0, 255 };
                           SDL_Color fg = { 255, 255, 0 };
                           SDL_Surface* Text = TTF_RenderText_Shaded(Font, bprintf ("%3li",(long)qBuild.Par), fg, bg);
                           SDL_Rect Dst = { qBuild.ScreenPos.x-ViewPos.x+WinP1.x+3, qBuild.ScreenPos.y-ViewPos.y+WinP1.y+1, Text->w, Text->h };
                           SDL_BlitSurface(Text, NULL, Surf, &Dst);
                           SDL_FreeSurface(Text);
                           TTF_CloseFont(Font);
                        }
                     }
                  }

               //Das hier kostet einiges an Zeit. Könnte man optimieren: ==>+<==
               do c++; while (c<pBuilds->AnzEntries() && (!pBuilds->IsInAlbum(c) || (Editor==EDITOR_NONE && pBuilds->IsInAlbum(c) && (*pBuilds)[c].BrickId>=0x10000000+2000)));

               if (c<pBuilds->AnzEntries())
               {
                  t1 = (long(Bricks[(*pBuilds)[c].BrickId].Layer)<<16) + (*pBuilds)[c].ScreenPos.y + Bricks[(*pBuilds)[c].BrickId].GetBitmapDimension().y+Bricks[(*pBuilds)[c].BrickId].FloorOffset;
               }
               else t1 = 0x7fffffff;
               //...bis hier
            }
         }
         while (c<pBuilds->AnzEntries() || d<Sim.Persons.AnzEntries());

         Bench.AdminTime.Stop();

         if (!Editor)
         {
            SLONG PlayerIndex = Sim.Persons.GetPlayerIndex(PlayerNum);

            if (PlayerIndex!=-1 && Sim.Persons[PlayerIndex].Dir<8)
            {
               if (Sim.Persons[PlayerIndex].Position.x<Sim.Players.Players[(SLONG)PlayerNum].ViewPos.x-40 || Sim.Persons[PlayerIndex].Position.x>Sim.Players.Players[(SLONG)PlayerNum].ViewPos.x+650)
               {
                  if (Sim.Persons[PlayerIndex].Dir==1 || Sim.Persons[PlayerIndex].Dir==2 || Sim.Persons[PlayerIndex].Dir==3)
                     gShowCursorFeet=0;
                  else if (Sim.Persons[PlayerIndex].Dir==5 || Sim.Persons[PlayerIndex].Dir==6 || Sim.Persons[PlayerIndex].Dir==7)
                     gShowCursorFeet=1;
                  else if (Sim.Persons[PlayerIndex].Dir>=8)
                     gShowCursorFeet=-1;
               }
               else
                  gShowCursorFeet=-1;
            }
            else
               gShowCursorFeet=-1;

            if (!Editor && Sim.Persons[PlayerIndex].Position.y>=5000)
               AnzPeopleOnScreen=AnzPeopleOnScreen*2/3;
         }
         else gShowCursorFeet=-1;

         if (LastAnzPeopleOnScreen==-1)
            LastAnzPeopleOnScreen=AnzPeopleOnScreen;
         else
         {
            if (LastAnzPeopleOnScreen<AnzPeopleOnScreen) LastAnzPeopleOnScreen++;
            if (LastAnzPeopleOnScreen>AnzPeopleOnScreen) LastAnzPeopleOnScreen--;
         }

         AmbientManager.SetVolume (AMBIENT_PEOPLE,    LastAnzPeopleOnScreen*2);
         AmbientManager.SetVolume (AMBIENT_JET_FIELD, 150-(Airport.RightEnd-(ViewPos.x+320))/6);

         //Draw intuitive Walknet:
         if (Editor==EDITOR_LINKS)
         {
            for (c=0; c<(ULONG)Airport.PlateDimension.x; c++)
               for (d=0; d<15; d++)
                  if (d!=3 && d!=4)
                  {
                     XY Pos;

                     if (d<5)
                     {
                        Pos.x = c*44-d*11+Airport.PlateOffset.x-ViewPos.x+WinP1.x+33-9;
                        Pos.y = 220+d*22-127-ViewPos.y+WinP1.y;
                     }
                     else
                     {
                        Pos.x = c*44-(d-5)*11+Airport.PlateOffset.x-ViewPos.x+WinP1.x+33-9;
                        Pos.y = 220+(d-5)*22-ViewPos.y+WinP1.y;
                     }

                     if ((Airport.iPlate[SLONG(d+(c<<4))] & 128) == 0) PrimaryBm.BlitFromT (RuneBms[2], Pos);
                     if ((Airport.iPlate[SLONG(d+(c<<4))] & 64) == 0)  PrimaryBm.BlitFromT (RuneBms[3], Pos);
                     if ((Airport.iPlate[SLONG(d+(c<<4))] & 32) == 0)  PrimaryBm.BlitFromT (RuneBms[4], Pos);
                     if ((Airport.iPlate[SLONG(d+(c<<4))] & 16) == 0)  PrimaryBm.BlitFromT (RuneBms[5], Pos);
                  }
         }

         #ifdef _DEBUG
            //if (Registration.GetMode()==1)
            {
               if (Editor==EDITOR_NONE)
                  PrimaryBm.TextOut (0, 20, RGB( 0, 0, 255), RGB( 255, 255, 0), "Shift + E = Editor");
               else
               {
                  PrimaryBm.TextOut (140, 0, RGB( 0, 0, 255), RGB( 255, 255, 0), Airport.GetHallFilename());

                  if (Editor==EDITOR_BUILDS)
                  {
                     PrimaryBm.TextOut (0, 37, RGB( 0, 0, 255), RGB( 255, 255, 0), "F2-F8          = Get Last Object");
                     PrimaryBm.TextOut (0, 54, RGB( 0, 0, 255), RGB( 255, 255, 0), "Shift + F2-F8  = Select Object");
                     PrimaryBm.TextOut (0, 71, RGB( 0, 0, 255), RGB( 255, 255, 0), "Shift + S      = Save");
                     PrimaryBm.TextOut (0, 88, RGB( 0, 0, 255), RGB( 255, 255, 0), "Shift + L      = Load");
                     PrimaryBm.TextOut (0, 105, RGB( 0, 0, 255), RGB( 255, 255, 0), "Shift + PgUp / Down = Time Warp");
                  }
               }
            }
            //else
            //{
            //      PrimaryBm.TextOut (0, 20, RGB( 0, 0, 255), RGB( 255, 255, 0), Registration.GetDisplayString());
            //}
         #endif

         //Der Komfort-Cursor:
         //if (IsWindowEnabled())
         {
           if (Editor==EDITOR_BUILDS && bCursorCaptured && EditObject!=0xffffffff && Bricks.IsInAlbum (EditObject))
              Bricks[EditObject].BlitAt (PrimaryBm, 0, Bricks[EditObject].GetIntelligentPosition (gMousePosition.x+ViewPos.x, gMousePosition.y+ViewPos.y)-ViewPos, 0);
         }

         PrimaryBm.PrimaryBm.SetClipRect(&CRect(0,0,640,480));

         //Die Statuszeile mit ihren Anzeigen...
         CStdRaum::OnPaint ();
         CStdRaum::InitToolTips ();

         if (!IsDialogOpen() && !MenuIsOpen() && Editor==EDITOR_NONE && !MouseWait)
         {
            //Die Transparenten Maus-Tips:
            c=Airport.IsInMarkedArea (gMousePosition+ViewPos);
            if (c==ROOM_WALL) c=0; //Die Kindersicherung für die Wall-Eigenschaft

            if (c>=ROOM_FLIGHTDISPLAY && c<ROOM_FLIGHTDISPLAY+60)
            {
               //Die Tips für die Flugkürzel:
               if (c>=ROOM_FLIGHTDISPLAY && c<ROOM_FLIGHTDISPLAY+30)
                  SetMouseLook (CURSOR_HOT, 5000+c, TextBrickTexts[SLONG(c-ROOM_FLIGHTDISPLAY)], ROOM_AIRPORT, 10);
               else
                  SetMouseLook (CURSOR_HOT, 5000+c, TextBrickTexts[SLONG(c-ROOM_FLIGHTDISPLAY-30+TextBrickTexts.AnzEntries()/2)], ROOM_AIRPORT, 10);
            }
            else if (c>=ROOM_BURO_A && c<=ROOM_BURO_D && c%10==0 && c/10==ULONG(Sim.localPlayer+1))
            {
               if (gLanguage==LANGUAGE_D) SetMouseLook (CURSOR_HOT, 5000+c, CString (bprintf (StandardTexte.GetS (TOKEN_TOOLTIP, 5000+c), (LPCTSTR)Sim.Players.Players[SLONG(c/10-1)].AirlineX))+" (B)", ROOM_AIRPORT, 10);
                                     else SetMouseLook (CURSOR_HOT, 5000+c, CString (bprintf (StandardTexte.GetS (TOKEN_TOOLTIP, 5000+c), (LPCTSTR)Sim.Players.Players[SLONG(c/10-1)].AirlineX))+" (O)", ROOM_AIRPORT, 10);
            }
            else if (c>=ROOM_PERSONAL_A && c<=ROOM_PERSONAL_D && c%10==1 && c/10==ULONG(Sim.localPlayer+1))
               SetMouseLook (CURSOR_HOT, 5000+c, CString (bprintf (StandardTexte.GetS (TOKEN_TOOLTIP, 5000+c), (LPCTSTR)Sim.Players.Players[SLONG(c/10-1)].AirlineX))+" (P)", ROOM_AIRPORT, 10);
            else if (c>=ROOM_BURO_A && c<=ROOM_PERSONAL_D)
               SetMouseLook (CURSOR_HOT, 5000+c, CString (bprintf (StandardTexte.GetS (TOKEN_TOOLTIP, 5000+c), (LPCTSTR)Sim.Players.Players[SLONG(c/10-1)].AirlineX)), ROOM_AIRPORT, 10);
            else if (c) SetMouseLook (CURSOR_HOT, 5000+c, ROOM_AIRPORT, 10);
         }
			if (!IsDialogOpen() && !MenuIsOpen() && !Sim.bPause && gMousePosition.y<440)
			{
				if (gMousePosition.x<=10  && ViewPos.x>Airport.LeftEnd)      SetMouseLook (CURSOR_LEFT,  0, ROOM_AIRPORT, 6010);
				if (gMousePosition.x>=630 && ViewPos.x+320<Airport.RightEnd) SetMouseLook (CURSOR_RIGHT, 0, ROOM_AIRPORT, 6011);
			}

         CStdRaum::PostPaint ();    if (bLeaveGameLoop) return;
         CStdRaum::PumpToolTips (); if (bLeaveGameLoop) return;

         if (Editor!=EDITOR_NONE)
	         PrimaryBm.BlitFrom (StatusLineBms[Editor], WinP1.x, WinP2.y-StatusLineSizeY);

         Pos++;
      }
   }

   LastAnzElements=AnzElements;
   LastViewPosX=ViewPos.x;
}

//--------------------------------------------------------------------------------------------
//Schickt eine Ankündigung weiter...
//--------------------------------------------------------------------------------------------
void AirportView::AnnouceTipDataUpdate (SLONG TipType)
{
   AnnouceTipDataUpdate (TipType);
}

//--------------------------------------------------------------------------------------------
//AirportView::OnLButtonDown:
//--------------------------------------------------------------------------------------------
void AirportView::OnLButtonDown(UINT nFlags, CPoint point) 
{
   ReferTo (nFlags);

   if (Sim.Time<=9*60000) return;

   DefaultOnLButtonDown ();

   point.x+=2;
   point.y+=2;

	if (MouseClickArea==ROOM_AIRPORT && (MouseClickId==6010 || MouseClickId==6011)) return;

   if (Sim.Players.Players[(SLONG)PlayerNum].RunningToToilet) return;
   if (!Editor && gMousePosition.y<440 && !IsDialogOpen() && !MenuIsOpen()) Sim.Persons[Sim.Persons.GetPlayerIndex(PlayerNum)].Running=FALSE;

   //Gimmick abschalten:
   if (Editor==FALSE)
   {
      PERSON &qPerson = Sim.Persons[Sim.Persons.GetPlayerIndex(PlayerNum)];

      if (qPerson.LookDir==9)
      {
         qPerson.Dir=8;
         qPerson.LookDir=UBYTE(Clans[(SLONG)qPerson.ClanId].GimmickArt2);
         qPerson.Phase=0;
      }
   }

   if (Sim.DontDisplayPlayer==Sim.localPlayer) return;

   if (Sim.FocusPerson!=-1 && gMousePosition.y<440 && !IsDialogOpen() && !MenuIsOpen())
      Sim.FocusPerson=-1;

   //Ist das Fenster hier zuständig? Ist der Klick in diesem Fenster?
   if (point.x>=WinP1.x && point.x<=WinP2.x && point.y>=WinP1.y && point.y<=WinP2.y-StatusLineSizeY*(Editor==0))
   {
      LButtonState = TRUE;

      point.x-=WinP1.x;
      point.y-=WinP1.y;

      //if (IsWindowEnabled())
      {
         //Change Editor Mode:
         if (Editor)
         {
            if (point.y>=WinP2.y-StatusLineSizeY && point.x<64)
            {
               EditObject = 0;

               if (point.y<WinP2.y-StatusLineSizeY/2)
                  if (point.x<32) Editor=1; else Editor=2;
               else
                  if (point.x<32) Editor=3; else Editor=4;

               if (Editor==4)
               {
                  Airport.CalcPlates ();
                  Airport.CalcCoordinates ();
               }

               return;
            }
         }
         else
         {
            if (GetAsyncKeyState (VK_SHIFT)/256)
            {
               //Hat der Spieler auf eine Figur geklickt?
               for (SLONG d=SLONG(Sim.Persons.AnzEntries())-1; d>=0; d--)
               {
                  if (Sim.Persons.IsInAlbum(d) && Clans.IsInAlbum (Sim.Persons[d].ClanId))
                  {
                     PERSON &qPerson = Sim.Persons[d];
                     CLAN   &qClan   = Clans[SLONG(qPerson.ClanId)];
                     SLONG   Type    = qClan.Type;

                     XY p=qPerson.ScreenPos-Sim.Players.Players[(SLONG)PlayerNum].ViewPos;

                     if (gMousePosition.IfIsWithin (p.x-(qClan.Phasen[1])[0].Size.x/2, p.y-(qClan.Phasen[1])[0].Size.y, p.x+(qClan.Phasen[1])[0].Size.x/2, p.y))
                     {
                        if (Type==CLAN_PLAYER1+Sim.localPlayer)
                           Sim.FocusPerson=-1;
                        else
                           Sim.FocusPerson=Sim.Persons.GetIdFromIndex (d);

                        gMouseScroll=FALSE;
                        IgnoreNextLButtonUp=TRUE;
                        break;
                     }
                  }
               }
            }
            else if (Sim.Players.Players[PlayerNum].IsWalking2Player==-1 && !PreLButtonDown (point))
            {
               if (Sim.Tutorial==1310 && Sim.IsTutorial) return;
               if (Sim.Players.Players[PlayerNum].IsStuck) return;

               if (IsDialogOpen()) IgnoreNextLButtonUp=TRUE;

               PERSON &qPlayerPerson = Sim.Persons.Persons[(SLONG)Sim.Persons.GetPlayerIndex(PlayerNum)];

               if (MenuIsOpen() || qPlayerPerson.StatePar==ROOM_STAIRS1UP || qPlayerPerson.StatePar==ROOM_STAIRS2UP || qPlayerPerson.StatePar==ROOM_STAIRS3UP || qPlayerPerson.StatePar==ROOM_STAIRS1DOWN || qPlayerPerson.StatePar==ROOM_STAIRS2DOWN || qPlayerPerson.StatePar==ROOM_STAIRS3DOWN)
               {
                  IgnoreNextLButtonUp=TRUE;
                  MenuLeftClick (point);
                  gMouseLButtonDownTimer=timeGetTime()-10000;
               }
               else
               {
                  if (Sim.bNoTime==FALSE && gMousePosition.y<440 && !MouseWait && Sim.Players.Players[PlayerNum].IsWalking2Player==-1 && qPlayerPerson.StatePar==0)
                  {
                     //Hat der Spieler auf einen anderen Spieler geklickt?
                     for (SLONG d=SLONG(Sim.Persons.AnzEntries())-1; d>=0; d--)
                     {
                        if (Sim.Persons.IsInAlbum(d) && !Sim.Persons[d].StatePar && PlayerNum!=Sim.Persons[d].State && Clans.IsInAlbum (Sim.Persons[d].ClanId))
                        {
                           PERSON &qPerson = Sim.Persons[d];
                           CLAN   &qClan   = Clans[SLONG(qPerson.ClanId)];
                           SLONG   Type    = qClan.Type;

                           if (Type>=CLAN_PLAYER1 && Type<=CLAN_PLAYER4)
                           {
                              PLAYER &qPlayer = Sim.Players.Players[(SLONG)Sim.Persons[d].State];
                              XY p=qPerson.ScreenPos-Sim.Players.Players[(SLONG)PlayerNum].ViewPos;

                              if (gMousePosition.IfIsWithin (p.x-(qClan.Phasen[0])[0].Size.x/2, p.y-(qClan.Phasen[0])[0].Size.y, p.x+(qClan.Phasen[0])[0].Size.x/2, p.y))
                              if (abs (qPerson.Position.x-qPlayerPerson.Position.x)<640)
                              if (abs (qPerson.Position.y-qPlayerPerson.Position.y)<640)
                              if (qPlayer.GetRoom()==ROOM_AIRPORT)
                              if (qPlayer.CallItADay==FALSE)
                              if (!qPlayer.IsStuck && !qPlayer.IsTalking)
                              {
                                 XY    ArrayPos;
                                 XY    Target = XY(-100, -100);
                                 UBYTE Phase  = 0;

                                 Sim.Players.Players[PlayerNum].IsWalking2Player=Sim.Persons[d].State;

                                 ArrayPos.x = qPerson.Position.x/44;

                                 if (qPerson.Position.y<4000) ArrayPos.y = qPerson.Position.y/22+5;
                                                         else ArrayPos.y = (qPerson.Position.y-5000)/22;

                                 qPerson.LookDir = 8;

                                 //In N/S/W/O einen freien Platz zu reden suchen:
                                 if (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&128)
                                 {
                                    Target=XY(ArrayPos.x, ArrayPos.y-1);
                                    Phase=0;
                                 }
                                 if (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&32)
                                 {
                                    Target=XY(ArrayPos.x, ArrayPos.y+1);
                                    Phase=2;
                                 }

                                 if (qPerson.Position.x > qPlayerPerson.Position.x)
                                 {
                                    if (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&16)
                                    {
                                       if (Airport.iPlate[ArrayPos.y+((ArrayPos.x-1)<<4)]&16)
                                          Target=XY(ArrayPos.x-2, ArrayPos.y);
                                       else
                                          Target=XY(ArrayPos.x-1, ArrayPos.y);

                                       Phase=3;
                                    }
                                 }
                                 else
                                 {
                                    if (Airport.iPlate[ArrayPos.y+(ArrayPos.x<<4)]&64)
                                    {
                                       if (Airport.iPlate[ArrayPos.y+((ArrayPos.x+1)<<4)]&64)
                                          Target=XY(ArrayPos.x+2, ArrayPos.y);
                                       else
                                          Target=XY(ArrayPos.x+1, ArrayPos.y);

                                       Phase=1;
                                    }
                                 }

                                 if (qPlayerPerson.LookDir==9) //Warte-Gimmick
                                 {
                                    qPlayerPerson.LookDir=0;
                                    if (qPerson.Position.y>qPlayerPerson.Position.y) qPlayerPerson.LookDir=2;
                                    if (qPerson.Position.x<qPlayerPerson.Position.x) qPlayerPerson.LookDir=3;
                                    if (qPerson.Position.x>qPlayerPerson.Position.x) qPlayerPerson.LookDir=1;

                                    qPlayerPerson.Phase=0;
                                 }

                                 //Haben wir einen guten Platz gefunden ?
                                 if (Target.x!=-100)
                                 {
                                    Sim.Players.Players[PlayerNum].WalkToPlate (Target);
                                    Sim.Players.Players[PlayerNum].bDialogStartSent=false;

                                    //Ja!
                                    if (qPlayer.Owner==1)
                                    {
                                       //Computerspieler:
                                       qPerson.Phase             = UBYTE(Phase);
                                       qPlayer.IsWalking2Player  = PlayerNum;
                                       qPlayer.IsTalking         = TRUE;
                                       qPerson.LookAt (Phase);

                                       if (Sim.bNetwork)
                                       {
                                          Sim.SendSimpleMessage (ATNET_DIALOG_LOCK, NULL, qPlayer.PlayerNum);
                                          Sim.SendSimpleMessage (ATNET_DIALOG_LOCK, NULL, Sim.Players.Players[Sim.localPlayer].PlayerNum);
                                          Sim.SendSimpleMessage (ATNET_PLAYERLOOK, NULL, qPerson.State, Phase);
                                          qPlayer.BroadcastPosition ();
                                          Sim.Players.Players[Sim.localPlayer].BroadcastPosition ();
                                       }
                                    }
                                    else
                                    {
                                       TEAKFILE Message;

                                       Message.Announce(64);

                                       Message << ATNET_DIALOG_REQUEST;

                                       //Netzwerkspieler:
                                       Message << Phase              << PlayerNum
                                               << qPerson.Position.x << qPerson.Position.y;

                                       Sim.SendMemFile (Message, qPlayer.NetworkID);

                                       qPlayer.PlayerDialogState = PlayerNum; //im Aufbau
                                    }
                                 }
                                 else
                                 {
                                    //Nein!
                                    Sim.Players.Players[PlayerNum].IsWalking2Player=0;
                                    Sim.Players.Players[SLONG(Sim.Persons[d].State)].IsWalking2Player=-1;
                                    Sim.Players.Players[SLONG(Sim.Persons[d].State)].IsTalking=-1;
                                 }

                                 d=0; break; //gefunden!
                              }
                           }
                        }
                     }

                     if (Sim.Players.Players[PlayerNum].IsWalking2Player==-1 && !IsDialogOpen())
                     {
                        SLONG p=Sim.Persons[Sim.Persons.GetPlayerIndex(PlayerNum)].Position.x-Sim.Players.Players[(SLONG)PlayerNum].ViewPos.x;

                        //Falls Person sichtbar, dann C&C-Scroll abschalten:
                        if (p>-5 && p<645) gMouseScroll=0;

                        Sim.Players.Players[PlayerNum].WalkToMouseClick (gMousePosition+Sim.Players.Players[PlayerNum].ViewPos);
                     }
                  }
               }
            }
         }

         //BUILDS EDITOR:
         if (Editor==EDITOR_BUILDS)
         {
            if (EditObject==0xffffffff && UnderCursor!=0xffffffff && Airport.Builds.IsInAlbum(UnderCursor))
            {
               EditObject = Airport.Builds[UnderCursor].BrickId;
               Airport.Builds -= UnderCursor;
            }
            else
            {
               XY &ViewPos=Sim.Players.Players[(SLONG)PlayerNum].ViewPos;

               if (Airport.Builds.GetNumFree()<2) Airport.Builds.Builds.ReSize (Airport.Builds.AnzEntries()+10);

               if (Bricks.IsInAlbum(EditObject))
                  Airport.Builds += BUILD (EditObject, Bricks[EditObject].GetIntelligentPosition (point.x+ViewPos.x, point.y+ViewPos.y), 0);
            }
            Airport.Builds.Sort ();
         }
	      //TEST EDITOR:
         else if (Editor==EDITOR_TEST)
         {
            if (point.y>=WinP2.y-StatusLineSizeY)
            {
               //Add one Customer:
               if (point.x>=64 && point.x<128)
					   PersonsToAdd++;
               //Add 20 Customers:
               else if (point.x>=128 && point.x<192)
					   PersonsToAdd+=20;
               //Kill all Customers:
               else if (point.x>=192 && point.x<256)
				   {
					   Sim.Persons.ClearAlbum();
					   PersonsToAdd = 0;
				   }
            }
         }
      }
   }
   else
   {
      if (!PreLButtonDown(point))
      {
         if (point.x>=WinP1.x && point.x<=WinP2.x && point.y>=WinP2.y-StatusLineSizeY*(Editor==0) && point.y<=WinP2.y)
         {
            //Klick in Statuszeile:
            CStdRaum::OnLButtonDown(nFlags, point);
            return;
         }
      }
   }
}

//--------------------------------------------------------------------------------------------
//void AirportView::OnLButtonUp(UINT nFlags, CPoint point):
//--------------------------------------------------------------------------------------------
void AirportView::OnLButtonUp(UINT nFlags, CPoint point)
{
   if (Sim.Time<=9*60000) return;

   DefaultOnLButtonUp();

   ReferTo (nFlags);

   LButtonState = FALSE;

   if (Sim.DontDisplayPlayer==Sim.localPlayer) return;

   if (Sim.Players.Players[PlayerNum].IsWalking2Player!=-1 || IsDialogOpen() || IgnoreNextLButtonUp)
   {
      IgnoreNextLButtonUp=FALSE;
      return;
   }

	if (MouseClickArea==ROOM_AIRPORT && (MouseClickId==6010 || MouseClickId==6011))
   {
      if (timeGetTime()-gMouseLButtonDownTimer>250)
         return;
   }

   //Ist das Fenster hier zuständig? Ist der Klick in diesem Fenster?
   if (point.x>=WinP1.x && point.x<=WinP2.x && point.y>=WinP1.y && point.y<=WinP2.y && !MenuIsOpen())
   {
      if (timeGetTime()-gMouseLButtonDownTimer<500 && gMouseScroll && !Editor && gMousePosition.y<440 && !MouseWait)
      if (Sim.Players.Players[PlayerNum].IsWalking2Player==-1 && !IsDialogOpen())
      {
         //gMouseScroll=0;
         Sim.Players.Players[PlayerNum].WalkToMouseClick (gMousePosition+Sim.Players.Players[PlayerNum].ViewPos);
      }
   }
}

//--------------------------------------------------------------------------------------------
//AirportView::OnLButtonDblClk:
//--------------------------------------------------------------------------------------------
void AirportView::OnLButtonDblClk(UINT, CPoint point) 
{
   if (Sim.Time<=9*60000) return;

   //Ist das Fenster hier zuständig? Ist der Klick in diesem Fenster?
   if (point.x>=WinP1.x && point.x<=WinP2.x && point.y>=WinP1.y && point.y<=WinP2.y && !Editor)
   {
      if (MenuIsOpen())
      {
         if (CalculatorIsOpen)
         {
            CalcClick();
            return;
         }
      }
      if (Sim.bNoTime==FALSE && !IsDialogOpen() && Sim.DontDisplayPlayer!=Sim.localPlayer)
      {
         if (Sim.Players.Players[PlayerNum].IsStuck==0)
         {
            Sim.Persons[Sim.Persons.GetPlayerIndex(PlayerNum)].Running=TRUE;
            Sim.Players.Players[PlayerNum].BroadcastPosition ();
         }
      }

      if (Sim.Options.OptionEffekte)
         if (gpClickFx && Sim.Options.OptionDigiSound) gpClickFx->Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7);
   }
}

//--------------------------------------------------------------------------------------------
//AirportView::OnRButtonDown:
//--------------------------------------------------------------------------------------------
void AirportView::OnRButtonDown(UINT nFlags, CPoint point) 
{
   XY     p;

   if (Sim.Time<=9*60000) return;

   DefaultOnRButtonDown ();

   if (Sim.FocusPerson!=-1 && gMousePosition.y<440 && !IsDialogOpen() && !MenuIsOpen())
   {
      Sim.FocusPerson=-1;
      return;
   }

   if (gMousePosition.y<440 && gMouseScroll && !Editor && !MouseWait)
   {
      XY   &ViewPos = Sim.Players.Players[(SLONG)PlayerNum].ViewPos;
		SLONG SizeX   = 320;
      XY    Pos     = Sim.Persons[Sim.Persons.GetPlayerIndex(PlayerNum)].ScreenPos;

      gMouseScroll=FALSE;

		if (Sim.Players.Players[PlayerNum].DialogWin && ((CStdRaum*)Sim.Players.Players[PlayerNum].DialogWin)->TempScreenScrollV!=1)
			SizeX/=2;

		Pos.x-=SizeX;

		//Horizonzale Begrenzung
		if (Pos.x<Airport.LeftEnd)        Pos.x=Airport.LeftEnd;
		if (Pos.x+SizeX>Airport.RightEnd) Pos.x=Airport.RightEnd-SizeX;

      //Über große Strecken lieber faden als scrollen
      if (abs(Pos.x-ViewPos.x)>640)
      {
         ViewPos.x=Pos.x;

         if (ViewPos.x<-1000) DebugBreak();

         if (Sim.Options.OptionBlenden)
         {
            if (FrameWnd) FrameWnd->PrepareFade();
            FrameWnd->Invalidate(); MessagePump();
            gBlendState=-2;
            FrameWnd->Invalidate(); MessagePump();
         }
      }
   }

   if (!Editor && ConvertMousePosition (point, &p) && p.IfIsWithin (0, 0, 639, 479))
   {
      if (MenuIsOpen())
      {
         MenuRightClick (point);
      }
      else
      {
         CStdRaum::OnRButtonDown(nFlags, point);
      }
   }
   else
   {
      if (/*IsWindowEnabled() &&*/ Sim.Players.Players[(SLONG)PlayerNum].GetRoom()==ROOM_AIRPORT)
      {
         if (Editor==EDITOR_BUILDS) //Bei RMB das Objekt vom Cursor abwerfen:
            EditObject = 0xffffffff;
         else if (Editor==EDITOR_LINKS)
         {
            if (UnderCursor!=0xffffffff)
               EditObject2 = UnderCursor;
         }
         else if (Editor==EDITOR_NONE)
         {
            if (MenuIsOpen())
            {
               MenuRightClick (point);
            }
         }
      }
   }
}

//--------------------------------------------------------------------------------------------
//AirportView::OnKeyDown:
//--------------------------------------------------------------------------------------------
void AirportView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   SLONG c;
   XY   &ViewPos = Sim.Players.Players[(SLONG)PlayerNum].ViewPos;

   //Sowas darf nur das Hauptfenster, was immer links oben ist und nicht verdeckt wird:
   if (WinP1.x==0 && WinP1.y==0 && TopWin==NULL)
   {
      //Editor F-Keys
      if (GetAsyncKeyState (VK_SHIFT) && !GetAsyncKeyState (VK_CONTROL))
      {
         switch (nChar)
         {
            //Neues, autonomes & selbstzerstörendes Fenster erzeuges:
            case VK_F2:  if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum, 550+0x10000000, &EditObject); else CStdRaum::OnKeyDown(nChar, nRepCnt, nFlags); break;
            case VK_F3:  if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum, 618+0x10000000, &EditObject); break;
            case VK_F4:  if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum, 664+0x10000000, &EditObject); break;
            case VK_F5:  if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum, 700+0x10000000, &EditObject); break;
            case VK_F6:  if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum, 740+0x10000000, &EditObject); break;
            case VK_F7:  if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum, 800+0x10000000, &EditObject); break;
            case VK_F8:  if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum,  00+0x10000000, &EditObject); break;
            case VK_F9:  if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum,  00+0x10000000, &EditObject); break;
            case VK_F11: if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum, 2030+0x10000000, &EditObject); break;

            case VK_LEFT:
               if (Editor) ViewPos.x-=50;
               break;

            case VK_RIGHT:
               if (Editor) ViewPos.x+=50;
               break;

            default:
               CStdRaum::OnKeyDown(nChar, nRepCnt, nFlags);
         }
      }
      else
      {
         switch (nChar)
         {
            //Neues, autonomes & selbstzerstörendes Fenster erzeuges:
            case VK_F2:  if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum, 100+0x10000000, &EditObject); break;
            case VK_F3:  if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum, 140+0x10000000, &EditObject); break;
            case VK_F4:  if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum, 250+0x10000000, &EditObject); break;
            case VK_F5:  if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum, 300+0x10000000, &EditObject); break;
            case VK_F6:  if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum, 330+0x10000000, &EditObject); break;
            case VK_F7:  if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum, 400+0x10000000, &EditObject); break;
            case VK_F8:  if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum, 450+0x10000000, &EditObject); break;
            case VK_F9:  if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum, 500+0x10000000, &EditObject); break;
            case VK_F11: if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum, 2000+0x10000000, &EditObject); break;
            case VK_F12: if (Editor==EDITOR_BUILDS) TopWin=new AskBrick (bHandy, PlayerNum, 2050+0x10000000, &EditObject); break;

            case VK_INSERT:
               if (Editor==EDITOR_BUILDS)
                  for (c=Airport.Builds.AnzEntries()-1; c>=0; c--)
                     if (Airport.Builds.IsInAlbum(c) && Airport.Builds[c].ScreenPos.x>=gMousePosition.x+ViewPos.x)
                        Airport.Builds[c].ScreenPos.x+=44;
               break;

            case VK_DELETE: //Remove Objekt under Cursor
               if (Editor==EDITOR_BUILDS)
                  if (EditObject==0xffffffff && UnderCursor!=0xffffffff && Airport.Builds.IsInAlbum (UnderCursor))
                     Airport.Builds -= UnderCursor;
               break;

            case VK_BACK: //Kacheln verschieben
               if (Editor==EDITOR_BUILDS)
                  for (c=Airport.Builds.AnzEntries()-1; c>=0; c--)
                     if (Airport.Builds.IsInAlbum(c) && Airport.Builds[c].ScreenPos.x>=gMousePosition.x+ViewPos.x)
                        Airport.Builds[c].ScreenPos.x-=44;
               break;
         }

         CStdRaum::OnKeyDown(nChar, nRepCnt, nFlags);

         //Cursor-Tasten verschieben Ausschnitt:
         if (Editor)
         {
            if (GetAsyncKeyState (VK_SHIFT) && GetAsyncKeyState (VK_CONTROL) && UnderCursor!=0xffffffff)
            {
               switch (nChar)
               {
                  case VK_LEFT:  Airport.Builds[UnderCursor].ScreenPos.x-=44; break;
                  case VK_RIGHT: Airport.Builds[UnderCursor].ScreenPos.x+=44; break;
                  case VK_UP:    Airport.Builds[UnderCursor].ScreenPos.y-=22; Airport.Builds[UnderCursor].ScreenPos.x+=11; break;
                  case VK_DOWN:  Airport.Builds[UnderCursor].ScreenPos.y+=22; Airport.Builds[UnderCursor].ScreenPos.x-=11; break;
               }
            }
            else if (GetAsyncKeyState (VK_CONTROL) && UnderCursor!=0xffffffff)
            {
               switch (nChar)
               {
                  case VK_LEFT:  Airport.Builds[UnderCursor].ScreenPos.x--; break;
                  case VK_RIGHT: Airport.Builds[UnderCursor].ScreenPos.x++; break;
                  case VK_UP:    Airport.Builds[UnderCursor].ScreenPos.y--; break;
                  case VK_DOWN:  Airport.Builds[UnderCursor].ScreenPos.y++; break;
               }
            }
            else
            {
               switch (nChar)
               {
                  case VK_LEFT:  ViewPos.x-=10; break;
                  case VK_RIGHT: ViewPos.x+=10; break;
                  case VK_UP:    ViewPos.y-=10; break;
                  case VK_DOWN:  ViewPos.y+=10; break;
               }
            }
         }
      }
   }
}

//--------------------------------------------------------------------------------------------
//AirportView::OnToggleEditor:
//--------------------------------------------------------------------------------------------
void AirportView::OnToggleEditor() 
{
   Editor ^= 1;
}

//--------------------------------------------------------------------------------------------
//AirportView::OnChar:
//--------------------------------------------------------------------------------------------
void AirportView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   //Sowas darf nur das Hauptfenster, was immer links oben ist:
   if (WinP1.x==0 && WinP1.y==0)
   {
#ifdef _DEBUG
      //Builds-Editor: Parametereingabe
      if (Editor==EDITOR_BUILDS)
      {
         if (nChar>='0' && nChar<='9' && UnderCursor!=0xffffffff && Airport.Builds.IsInAlbum(UnderCursor))
         {
            long brickid = Airport.Builds[UnderCursor].BrickId-0x10000000;

            if ((brickid>=2030 && brickid<=2039) || brickid<=2001 || brickid<=2045 || brickid==2003 || brickid==RUNE_AREALO || brickid==RUNE_AREARU || brickid==500 || brickid==522 || brickid==520 || brickid==492 || brickid==RUNE_WAITPLANE || brickid==RUNE_DROPSUITCASE || brickid==RUNE_WAYPOINT || brickid==RUNE_WAYPOINT_WAIT || brickid==RUNE_WAYPOINT_START || brickid==RUNE_WAYPOINT_G || brickid==RUNE_CONDBLOCK || brickid==337 || brickid==338)
            {
               while (Airport.Builds[UnderCursor].Par>99) Airport.Builds[UnderCursor].Par-=100;
               while (Airport.Builds[UnderCursor].Par>25) Airport.Builds[UnderCursor].Par-=10;

               Airport.Builds[UnderCursor].Par=UBYTE(Airport.Builds[UnderCursor].Par*10+nChar-'0');
            }
         }
      }

      switch (toupper(nChar))
      {
         case 'E':
            //if (Registration.GetMode()==1)
               if (!Sim.bNetwork)
               {
                  Editor = (Editor == EDITOR_NONE);
                  Airport.UpdateStaticDoorImage ();
               }
            break;

         case 'S':
            //if (Registration.GetMode()==1)
               if (Editor) Airport.Save();
            break;

         case 'W':
            //if (Registration.GetMode()==1)
               Sim.Persons.DoOneStep();
            break;

         case 'L':
            //if (Registration.GetMode()==1)
               if (Editor) TopWin = new HallDiskMenu (bHandy, PlayerNum);
            break;
      }
#endif
   }

   CStdRaum::OnChar(nChar, nRepCnt, nFlags);
}

//--------------------------------------------------------------------------------------------
//AirportView::OnSysChar:
//--------------------------------------------------------------------------------------------
void AirportView::OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   //CStdRaum::OnSysChar(nChar, nRepCnt, nFlags);
}

//--------------------------------------------------------------------------------------------
//Gibt einen zufälligen Geburtsort für einen Kunden (0) oder einen Spieler (1) zurück:
//--------------------------------------------------------------------------------------------
XY AIRPORT::GetRandomBirthplace (BOOL Type, SLONG PlayerNum, TEAKRAND *pRand)
{
   if (Type==0) return (GetRandomTypedRune (RUNE_CREATION, 0, false, pRand));
           else return (GetRandomTypedRune (RUNE_PCREATION, (UBYTE)PlayerNum, false, pRand));
}

//--------------------------------------------------------------------------------------------
//Gibt einen zufälligen Ausgang zurück:
//--------------------------------------------------------------------------------------------
XY AIRPORT::GetRandomExit (TEAKRAND *pRand)
{
   return (GetRandomTypedRune (RUNE_DESTRUCTION, 0, false, pRand));
}

//--------------------------------------------------------------------------------------------
//Gibt eines der Geschäfte zurück:
//--------------------------------------------------------------------------------------------
void AIRPORT::GetRandomShop (XY &ReturnPosition, SLONG &ReturnStatePar, SLONG ClanType, UBYTE *pMood, TEAKRAND *pRand)
{
   ULONG Buffer[20];    //Bis zu 20 Möglichkeiten werden hier zwischengespeichert
   SLONG c;
   SLONG Anz;           //Zahl der Elementer in "Buffer"

   Anz = 0;

   if (ReturnStatePar>=ROOM_LASTMIN_X1 && ReturnStatePar<=ROOM_LASTMIN_X4) ReturnStatePar=ROOM_LAST_MINUTE;
   if (ReturnStatePar>=ROOM_REISE_X1 && ReturnStatePar<=ROOM_REISE_X4) ReturnStatePar=ROOM_REISEBUERO;
   if (ReturnStatePar>=ROOM_MONITOR1 && ReturnStatePar<=ROOM_MONITOR9) ReturnStatePar=ROOM_MONITOR1;

   for (c=0; c<SLONG(Runes.AnzEntries()); c++)
      if (Runes[c].BrickId==(RUNE_2SHOP|0x10000000) && ((Runes[c].Par>=ROOM_SHOP1 && Runes[c].Par<=ROOM_SHOP4) || Runes[c].Par==ROOM_MONITOR1 || Runes[c].Par==ROOM_MUSEUM || Runes[c].Par==ROOM_BANK || Runes[c].Par==ROOM_REISEBUERO || Runes[c].Par==ROOM_LAST_MINUTE || (Runes[c].Par==ROOM_WC_F && ClanType==CLAN_FEMALE) || (Runes[c].Par==ROOM_WC_M && ClanType==CLAN_MALE)))
      {
         Buffer[Anz++]=c;
         if (Anz==20)
         {
            hprintf (0, "Buffer exceded!");
            here (FNL);
            break;
         }
      }

   if (Anz==0) TeakLibW_Exception (FNL, ExcNever);

again:
nofreetravelagents:
   do
   {
      if (pRand) c=pRand->Rand(Anz);
            else c=rand()%Anz;
   }
   while (ReturnStatePar==Runes[(SLONG)Buffer[c]].Par && Anz!=1);

   if (Runes[(SLONG)Buffer[c]].Par==ROOM_SHOP1 && (SLONG(Sim.Time)<timeDutyOpen || ((Sim.Weekday==5 || Sim.Weekday==6) && SLONG(Sim.Time)>timeDutyClose-60000))) goto again;
   if (Runes[(SLONG)Buffer[c]].Par==ROOM_LAST_MINUTE && (SLONG(Sim.Time)>timeLastClose-60000 || Sim.Weekday==5)) goto again;
   if (Runes[(SLONG)Buffer[c]].Par==ROOM_MUSEUM && (SLONG(Sim.Time)<timeMuseOpen || Sim.Weekday==5 || Sim.Weekday==6)) goto again;
   if (Runes[(SLONG)Buffer[c]].Par==ROOM_REISEBUERO && (SLONG(Sim.Time)>timeReisClose-60000 || Sim.Weekday==6)) goto again;

   //Spezialfix für Personen zu Reisebüro / LastMinute
   if (Runes[(SLONG)Buffer[c]].Par==ROOM_REISEBUERO)
   {
      SLONG d, n;
      
      if (pRand) n=pRand->Rand(256);
            else n=rand()%256;

      for (d=0; d<4; d++)
         if (Sim.RoomBusy[ROOM_REISE_X1+((d+n)&3)]==0)
         {
            for (c=0; c<SLONG(Runes.AnzEntries()); c++)
               if (Runes[c].BrickId==(RUNE_2SHOP|0x10000000) && Runes[c].Par==ROOM_REISE_X1+((d+n)&3))
               {
                  ReturnPosition = Runes[c].ScreenPos;
                  ReturnStatePar = Runes[c].Par;

                  Sim.RoomBusy[ROOM_REISE_X1+((d+n)&3)]++;
                  if (pMood) (*pMood)=(UBYTE)MoodPersonReise;
                  return;
               }
         }

      goto nofreetravelagents;
   }
   else if (Runes[(SLONG)Buffer[c]].Par==ROOM_LAST_MINUTE)
   {
      SLONG d, n;

      if (pRand) n=pRand->Rand(256);
            else n=rand()%256;

      for (d=0; d<4; d++)
         if (Sim.RoomBusy[ROOM_LASTMIN_X1+((d+n)&3)]==0)
         {
            for (c=0; c<SLONG(Runes.AnzEntries()); c++)
               if (Runes[c].BrickId==(RUNE_2SHOP|0x10000000) && Runes[c].Par==ROOM_LASTMIN_X1+((d+n)&3))
               {
                  ReturnPosition = Runes[c].ScreenPos;
                  ReturnStatePar = Runes[c].Par;

                  Sim.RoomBusy[ROOM_LASTMIN_X1+((d+n)&3)]++;
                  if (pMood) (*pMood)=(UBYTE)MoodPersonReise;
                  return;
               }
         }

      goto nofreetravelagents;
   }
   else if (Runes[(SLONG)Buffer[c]].Par==ROOM_MONITOR1)
   {
      SLONG d, n;

      if (pRand) n=pRand->Rand(256);
            else n=rand()%256;

      for (d=0; d<4; d++)
         if (Sim.RoomBusy[ROOM_MONITOR1+((d+n)%6)]==0)
         {
            for (c=0; c<SLONG(Runes.AnzEntries()); c++)
               if (Runes[c].BrickId==(RUNE_2SHOP|0x10000000) && Runes[c].Par==ROOM_MONITOR1+((d+n)&3))
               {
                  ReturnPosition = Runes[c].ScreenPos;
                  ReturnStatePar = Runes[c].Par;

                  Sim.RoomBusy[ROOM_MONITOR1+((d+n)%6)]++;
                  if (pMood) (*pMood)=(UBYTE)MoodPersonScreen;
                  return;
               }
         }

      goto nofreetravelagents;
   }

   ReturnPosition = Runes[(SLONG)Buffer[c]].ScreenPos;
   ReturnStatePar = Runes[(SLONG)Buffer[c]].Par;

   if (pMood)
   {
      (*pMood)=(UBYTE)MoodPersonNone;

      if (ReturnStatePar==ROOM_SHOP1) //Duty-Free Laden ist Ziel
         if (rand()%10<5) 
            (*pMood)=(UBYTE)MoodPersonBeverage;
         else
            (*pMood)=(UBYTE)MoodPersonSchokolade;
      else if (ReturnStatePar==ROOM_BANK) //Bank ist das Ziel
         if (rand()%90==0) 
            (*pMood)=(UBYTE)MoodPersonBankRobber;
         else
            (*pMood)=(UBYTE)MoodPersonBank;
      else if (ReturnStatePar==ROOM_MUSEUM) (*pMood)=(UBYTE)MoodPersonMuseum;
      else if (ReturnStatePar==ROOM_WC_F || ReturnStatePar==ROOM_WC_M) (*pMood)=(UBYTE)MoodPersonToilet;
   }
}

//--------------------------------------------------------------------------------------------
//War ein Mausklick in einer markierten Kachel-Area? Gibt 0 oder Raum zurück
//--------------------------------------------------------------------------------------------
SLONG AIRPORT::IsInMarkedArea (const XY &Pos)
{
   SLONG c, d, BestSize, NumFound=0;
   BUFFER<SLONG> Sizes(AreaMarkers.AnzEntries());

   //Alle durchsuchen:
   for (c=AreaMarkers.AnzEntries()-1; c>=0; c--)
   {
      if (Pos.x>=AreaMarkers[c].p1.x && Pos.y>=AreaMarkers[c].p1.y && Pos.x<=AreaMarkers[c].p2.x && Pos.y<=AreaMarkers[c].p2.y)
      {
         Sizes[c]=AreaMarkers[c].p2.x-AreaMarkers[c].p1.x;
         NumFound++;
      }
      else Sizes[c]=99999;
   }

   if (NumFound)
   {
      //Hatten wir da was?
      BestSize=999999; d=0;

      for (c=AreaMarkers.AnzEntries()-1; c>=0; c--)
         if (Sizes[c]>0)
         {
            if (Sizes[c]<BestSize)
            {
               BestSize=Sizes[c];
               d=AreaMarkers[c].Par;
            }
         }

      //Spezielle Marked-Areas für FlightDisplays über CheckIn und Abflug
      if (d>=ROOM_FLIGHTDISPLAY && d<ROOM_FLIGHTDISPLAY+60)
      {
         if (d<ROOM_FLIGHTDISPLAY+30)
         {
            if (TextBrickTexts[d-ROOM_FLIGHTDISPLAY].GetLength()==0) return (0);
         }
         else
         {
            if (TextBrickTexts[d-ROOM_FLIGHTDISPLAY-30+TextBrickTexts.AnzEntries()/2].GetLength()==0) return (0);
         }
      }

      //Anti-Marked Areas z.B. für Postkasten
      if (d==255) return (0);

      return (d);
   }
   else return (0);
}

//--------------------------------------------------------------------------------------------
//Gibt die nächste Station eines Weges zurück:
//--------------------------------------------------------------------------------------------
XY AIRPORT::GetNextWaypointRune (UBYTE StartingWaypoint, UBYTE *CurrentWaypoint, ULONG *Gimmick)
{
   SLONG c;

   //Aktuellen WayPoint wg. Warten raussuchen:
   for (c=SLONG(Runes.AnzEntries())-1; c>=0; c--)
      if (Runes[c].BrickId==(SLONG)(RUNE_WAYPOINT_WAIT|0x10000000) && Runes[c].Par==*CurrentWaypoint)
         *Gimmick=100;

   //Nächsten Waypoint raussuchen:
   for (c=SLONG(Runes.AnzEntries())-1; c>=0; c--)
      if ((Runes[c].BrickId==(SLONG)(RUNE_WAYPOINT|0x10000000) || Runes[c].BrickId==(SLONG)(RUNE_WAYPOINT_WAIT|0x10000000) || Runes[c].BrickId==(SLONG)(RUNE_WAYPOINT_G|0x10000000)) && Runes[c].Par==*CurrentWaypoint+1)
      {
         *CurrentWaypoint = *CurrentWaypoint+1;
         if (*Gimmick<=1) *Gimmick = (Runes[c].BrickId==(SLONG)(RUNE_WAYPOINT_G|0x10000000));
         return (Runes[c].ScreenPos);
      }

   for (c=SLONG(Runes.AnzEntries())-1; c>=0; c--)
      if ((Runes[c].BrickId==(SLONG)(RUNE_WAYPOINT|0x10000000) || Runes[c].BrickId==(SLONG)(RUNE_WAYPOINT_WAIT|0x10000000) || Runes[c].BrickId==(SLONG)(RUNE_WAYPOINT_G|0x10000000)) && Runes[c].Par==StartingWaypoint)
      {
         *CurrentWaypoint = StartingWaypoint;
         if (*Gimmick<=1) *Gimmick = (Runes[c].BrickId==(SLONG)(RUNE_WAYPOINT_G|0x10000000));
         return (Runes[c].ScreenPos);
      }

   TeakLibW_Exception (FNL, ExcNever);
   return XY(0,0);
}

//--------------------------------------------------------------------------------------------
//Der Name sagt es schon:
//--------------------------------------------------------------------------------------------
BOOL AIRPORT::DoesRuneExist (ULONG BrickId, UBYTE Par)
{
   for (SLONG c=SLONG(Runes.AnzEntries())-1; c>=0; c--)
      if (Runes[c].BrickId==(SLONG)(BrickId|0x10000000) && Runes[c].Par==Par)
         return (TRUE);

   return (FALSE);
}

//--------------------------------------------------------------------------------------------
//Gibt eine zufälligen (falls mehrere möglich) Rune des Typs zurück:
//--------------------------------------------------------------------------------------------
XY AIRPORT::GetRandomTypedRune (ULONG BrickId, UBYTE Par, bool AcceptError, TEAKRAND *pRand)
{
   ULONG Buffer[20];    //Bis zu 20 Möglichkeiten werden hier zwischengespeichert
   SLONG c;
   SLONG Anz=0;         //Zahl der Elementer in "Buffer"

   for (c=SLONG(Runes.AnzEntries())-1; c>=0; c--)
      if (Runes[c].BrickId==(SLONG)(BrickId|0x10000000) && Runes[c].Par==Par)
      {
         Buffer[Anz++]=c;
         if (Anz==20)
         {
            hprintf (0, "Buffer exceded!");
            here (FNL);
            break;
         }
      }

   if (Anz==0)
      if (AcceptError) return (XY(-9999, -9999));
      else             TeakLibW_Exception (FNL, ExcNever);

   if (Anz==1)
      return (Runes[(SLONG)Buffer[0]].ScreenPos);

   if (pRand)
      return (Runes[(SLONG)Buffer[pRand->Rand(Anz)]].ScreenPos);
   else
      return (Runes[(SLONG)Buffer[rand()%Anz]].ScreenPos);
}

//--------------------------------------------------------------------------------------------
//Wieviele Shops (oder Gates o.ä.) gibt es von einer Sorte?:
//--------------------------------------------------------------------------------------------
SLONG AIRPORT::GetNumberOfShops (ULONG BrickId)
{
   SLONG c;
   SLONG Anz;           //Zahl der Elemente

   Anz = 0;

   for (c=0; c<SLONG(Runes.AnzEntries()); c++)
      if (Runes[c].BrickId==(SLONG)(BrickId|0x10000000))
         Anz++;

   return (Anz);
}

//--------------------------------------------------------------------------------------------
//Wieviele Gates haben noch keinen Mieter?
//--------------------------------------------------------------------------------------------
SLONG AIRPORT::GetNumberOfFreeGates (void)
{
   SLONG c, d, e, rc, Anz;

   rc = Anz = GetNumberOfShops (RUNE_2WAIT);

   for (c=0; c<Anz; c++)
      for (d=0; d<Sim.Players.Players.AnzEntries(); d++)
         if (!Sim.Players.Players[d].IsOut)
            for (e=0; e<Sim.Players.Players[d].Gates.Gates.AnzEntries(); e++)
               if (Sim.Players.Players[d].Gates.Gates[e].Miete!=-1 &&
                   Sim.Players.Players[d].Gates.Gates[e].Nummer==c)
               {
                  rc--;
                  break;
               }

   return (rc);
}

//--------------------------------------------------------------------------------------------
//Gibt die Treppe zurück, die am nächsten ist:
//--------------------------------------------------------------------------------------------
XY AIRPORT::GetBestStairs (UBYTE Par, SLONG x1, SLONG x2)
{
   SLONG c, bestc, bestdist;

   bestdist=9999999; bestc=-1;

   for (c=0; c<SLONG(Runes.AnzEntries()); c++)
      if (Runes[c].BrickId==SLONG(RUNE_2SHOP|0x10000000) && (Runes[c].Par==Par || Runes[c].Par==Par+2 || Runes[c].Par==Par+4))
      {
         if (abs(Runes[c].ScreenPos.x-x1)+abs(Runes[c].ScreenPos.x-x2)<bestdist)
         {
            bestc=c;
            bestdist=abs(Runes[c].ScreenPos.x-x1)+abs(Runes[c].ScreenPos.x-x2);
         }
      }

   if (c==-1) TeakLibW_Exception (FNL, ExcNever);

   return (Runes[bestc].ScreenPos);
}

//--------------------------------------------------------------------------------------------
//Gibt zurück, welchen Parameter die Rune des Types n in der Umgebung von XY hat:
//--------------------------------------------------------------------------------------------
UBYTE AIRPORT::GetRuneParNear (const XY &Pos, const XY &MaxDist, ULONG RuneType)
{
   SLONG c;

   //Alle durchsuchen:
   for (c=SLONG(Runes.AnzEntries())-1; c>=0; c--)
      if (Runes[c].BrickId==(SLONG)(0x10000000|RuneType) && abs(Runes[c].ScreenPos.x-Pos.x)<=MaxDist.x && abs(Runes[c].ScreenPos.y-Pos.y)<=MaxDist.y)
      {
         //gefunden!
         return (Runes[c].Par);
      }

   //Da war nichts!
   return (0);
}

//--------------------------------------------------------------------------------------------
//Gibt zurück, welcher Brick von Sorte X dort so rumhängt:
//--------------------------------------------------------------------------------------------
BUILD *AIRPORT::GetBuildNear (const XY &Pos, const XY &MaxDist, ULONG BrickId)
{
   SLONG c;

   //Alle durchsuchen:
   for (c=SLONG(Builds.AnzEntries())-1; c>=0; c--)
      if (Builds.IsInAlbum(c))
         if (Builds[c].BrickId==SLONG(BrickId) && abs(Builds[c].ScreenPos.x-Pos.x)<=MaxDist.x && abs(Builds[c].ScreenPos.y-Pos.y)<=MaxDist.y)
         {
            //gefunden!
            return (&Builds[c]);
         }

   //Da war nichts!
   return (NULL);
}

//--------------------------------------------------------------------------------------------
//AIRPORT::Load (SLONG Hall, SLONG Level):
//--------------------------------------------------------------------------------------------
void AIRPORT::Load (SLONG Hall, SLONG Level)
{
   SLONG d, e, BestE, Diletation;

   HallNum		 = Hall;
   HallLevel[0] = Level;

   Builds.Load (Hall, Level);

   //Und dafür sorgen, daß nichts im negativen Bereich liegt:
   for (e=0, BestE = -1; e<SLONG(Builds.AnzEntries()); e++)
      if (Builds.IsInAlbum(e) && Builds[e].BrickId == 100+0x10000000)
         if (BestE == -1 || Builds[e].ScreenPos.x<Builds[BestE].ScreenPos.x)
            BestE = e;

   if (BestE==-1) TeakLibW_Exception (FNL, ExcNever);

   if (Builds[BestE].ScreenPos.x<176)
   {
      Diletation = (44*5-Builds[BestE].ScreenPos.x)/176*176+176;

      //Die Bauteile verschieben:
      for (d=0; d<SLONG(Builds.AnzEntries()); d++)
         if (Builds.IsInAlbum(d)) Builds[d].ScreenPos.x+=Diletation;
   }

   /*CalcPlates ();
   CalcCoordinates (); */
}

//--------------------------------------------------------------------------------------------
//AIRPORT::Save (void) const:
//--------------------------------------------------------------------------------------------
void AIRPORT::Save (void) const
{
   if (HallNum!=0)  //Nicht speichern, wenn alle da sind
   {
      Builds.Save (HallNum, HallLevel[0]);
   }
}

//--------------------------------------------------------------------------------------------
//Lädt einen kompletten Flughafen indem er ihn aus Einzelteilen zusammensetzt
//--------------------------------------------------------------------------------------------
//#pragma optimize("", off)
#pragma optimize("agptwy", on)
void AIRPORT::LoadAirport (SLONG LeftEnd, SLONG CheckIn, SLONG Office, SLONG Entry, SLONG Shops, SLONG Cafe, SLONG Security, SLONG Suitcase, SLONG WaitZone, SLONG RightEnd)
{
   SLONG  c, d, e;
   SLONG  TotalMem;
   SLONG  BestD, BestE;
   BUILDS localBuilds[10];  //Die verschiedenen Abschnitte des Flughafens
   SLONG  Diletation[10];   //Die Verschiebung der Abschnitte; Diletation[0] ist immer 0
   SLONG  Count[10];        //Fünf Counter, da alle fünf Abschnitte zugleich zusammengefügt werden

   CWait  Waiting;
   FrameWnd->Invalidate(); MessagePump();
   FrameWnd->Invalidate(); MessagePump();

   //HallNumber-Array initialisieren:
   HallLevel[0] = LeftEnd;
   HallLevel[1] = CheckIn;
   HallLevel[2] = Office;
   HallLevel[3] = Entry;
   HallLevel[4] = Shops;
   HallLevel[5] = Cafe;
   HallLevel[6] = Security;
   HallLevel[7] = Suitcase;
   HallLevel[8] = WaitZone;
   HallLevel[9] = RightEnd;

   HallNum = 0;

   //alten Airport löschen:
   Builds.Clear ();

   TotalMem      = 0;
   Diletation[0] = 0;

   //Alle Airports laden:
   for (c=d=0; c<10; c++)
   {
      //Load Hall to merge:
      localBuilds[c].Load (c+1, HallLevel[c]);

      TotalMem+=localBuilds[c].GetNumUsed();

      if (c>0)
      {
         //Vom alten Bereich die rechteste Bodenplatte raussuchen:
         for (d=0, BestD = -1; d<SLONG(localBuilds[c-1].AnzEntries()); d++)
            if (localBuilds[c-1].IsInAlbum(d) && localBuilds[c-1][d].BrickId == 100+0x10000000)
               if (BestD == -1 || localBuilds[c-1][d].ScreenPos.x>localBuilds[c-1][BestD].ScreenPos.x)
                  BestD = d;

         //Vom neuen Bereich die linkeste Bodenplatte raussuchen:
         for (e=0, BestE = -1; e<SLONG(localBuilds[c].AnzEntries()); e++)
            if (localBuilds[c].IsInAlbum(e) && localBuilds[c][e].BrickId == 100+0x10000000)
               if (BestE == -1 || localBuilds[c][e].ScreenPos.x<localBuilds[c][BestE].ScreenPos.x)
                  BestE = e;
         if (BestD==-1 || BestE==-1) TeakLibW_Exception (FNL, ExcNever);

         Diletation[c] = Diletation[c-1] + (localBuilds[c-1][BestD].ScreenPos.x+88) - localBuilds[c][BestE].ScreenPos.x;
         //hprintf (0, "Merge Level %li with Dilatation %li (NewSize: %li Bricks / %li new)", c, Diletation[c], localBuilds[c-1].AnzEntries(), localBuilds[c].AnzEntries());
      }
   }

   //Alloc Memory for complete Airport:
   Builds.Builds.ReSize (TotalMem);

   for (c=0; c<TotalMem; c++) Builds+=Builds.GetUniqueId();

   //Und alles zusammensetzen: (Vorbereitung)
   for (c=0; c<10; c++)
   {
      Count[c]=0;
      while (!localBuilds[c].IsInAlbum(Count[c]) && Count[c]<(SLONG)localBuilds[c].AnzEntries())
         Count[c]++;

      for (d=0; d<SLONG(localBuilds[c].AnzEntries()); d++)
         if (localBuilds[c].IsInAlbum(d))
            localBuilds[c][d].ScreenPos.x+=Diletation[c];
   }

   d=0;     //localBuilds (0..4) Index 
   e=0;     //Destination (main Build) index

   //Und alles zusammensetzen: (Jetzt im Ernst)
   while (1)
   {
      BestD=0;
      for (d=1; d<10; d++)
      {
         if (Count[d]>=(SLONG)localBuilds[d].AnzEntries()) continue;

         if (Count[BestD]>=(SLONG)localBuilds[BestD].AnzEntries())
         {
            BestD=d;
         }
         else if (Bricks[localBuilds[d][Count[d]].BrickId].Layer <Bricks[localBuilds[BestD][Count[BestD]].BrickId].Layer)
         {
            BestD=d;
         }
         else if (Bricks[localBuilds[d][Count[d]].BrickId].Layer==Bricks[localBuilds[BestD][Count[BestD]].BrickId].Layer && localBuilds[d][Count[d]].ScreenPos.y+Bricks[localBuilds[d][Count[d]].BrickId].GetBitmapDimension().y+Bricks[localBuilds[d][Count[d]].BrickId].FloorOffset <localBuilds[BestD][Count[BestD]].ScreenPos.y+Bricks[localBuilds[BestD][Count[BestD]].BrickId].GetBitmapDimension().y+Bricks[localBuilds[BestD][Count[BestD]].BrickId].FloorOffset)
         {
            BestD=d;
         }
         else if (Bricks[localBuilds[d][Count[d]].BrickId].Layer==Bricks[localBuilds[BestD][Count[BestD]].BrickId].Layer && localBuilds[d][Count[d]].ScreenPos.y+Bricks[localBuilds[d][Count[d]].BrickId].GetBitmapDimension().y+Bricks[localBuilds[d][Count[d]].BrickId].FloorOffset==localBuilds[BestD][Count[BestD]].ScreenPos.y+Bricks[localBuilds[BestD][Count[BestD]].BrickId].GetBitmapDimension().y+Bricks[localBuilds[BestD][Count[BestD]].BrickId].FloorOffset && localBuilds[d][Count[d]].ScreenPos.x<localBuilds[BestD][Count[BestD]].ScreenPos.x)
         {
            BestD=d;
         }
      }

      if (Count[BestD]>=(SLONG)localBuilds[BestD].AnzEntries()) break;

      Builds[e++]=localBuilds[BestD][Count[BestD]];

      //Advance Thread of the rope:
      Count[BestD]++;

      while (!localBuilds[BestD].IsInAlbum(Count[BestD]) && Count[BestD]<(SLONG)localBuilds[BestD].AnzEntries())
         Count[BestD]++;
   }

   //Die Laufintelligenzen vorbereiten:
   CalcPlates ();

   CalcCoordinates ();

   //SpeedUp! Das Resultat darf nie wieder abgespeichert werden! Assoziierung geht verloren!
   RemoveRunes ();
   UnassociateBuilds ();

   DoHashBuilds ();

   NewDay ();
}
#pragma optimize("", off)
//#pragma optimize("", on)

//--------------------------------------------------------------------------------------------
//Den Flughafen für einen neuen Tag vorbereiten: Personen entfernen & Spieler hinzufügen
//--------------------------------------------------------------------------------------------
void AIRPORT::NewDay (void)
{
   //Alte Leute entfernen:
   Sim.Persons.ClearAlbum();

   //Spieler hinzufügen:
   if (!Sim.Players.Players[0].IsOut) Sim.Persons+=PERSON (Clans.GetPlayerId (CLAN_PLAYER1), Airport.GetRandomBirthplace(TRUE, 1), 0, 99, 0, 0);
   if (!Sim.Players.Players[1].IsOut) Sim.Persons+=PERSON (Clans.GetPlayerId (CLAN_PLAYER2), Airport.GetRandomBirthplace(TRUE, 2), 0, 99, 0, 0);
   if (!Sim.Players.Players[2].IsOut) Sim.Persons+=PERSON (Clans.GetPlayerId (CLAN_PLAYER3), Airport.GetRandomBirthplace(TRUE, 3), 0, 99, 0, 0);
   if (!Sim.Players.Players[3].IsOut) Sim.Persons+=PERSON (Clans.GetPlayerId (CLAN_PLAYER4), Airport.GetRandomBirthplace(TRUE, 4), 0, 99, 0, 0);

   SLONG c, d;

   //WayPoint-Figuren hinzufügen:
   for (c=d=0; c<(SLONG)Builds.AnzEntries(); c++)
   {
      if (Builds.IsInAlbum(c))
         if (Builds[c].BrickId-0x10000000==RUNE_WAYPOINT_START)
            if (Builds[c].Par==240) //Hund-Gedankenblase
            {
               ULONG index = (Sim.Persons+=PERSON (Clans.GetCustomerIdByGroup (Builds[c].Par), GetRandomTypedRune (RUNE_WAYPOINT_START, Builds[c].Par), REASON_WAYPOINT, Builds[c].Par, Builds[c].Par, 0, (UBYTE)MoodPersonBone));
               Sim.Persons[index].Position  = Sim.Persons[index].Target;
               Sim.Persons[index].WaitCount = 170;
            }
            else
               Sim.Persons+=PERSON (Clans.GetCustomerIdByGroup (Builds[c].Par), GetRandomTypedRune (RUNE_WAYPOINT_START, Builds[c].Par), REASON_WAYPOINT, Builds[c].Par, Builds[c].Par, 0, 0);
            //Sim.Persons+=PERSON (Clans.GetCustomerIdByGroup (Builds[c].Par), Builds[c].ScreenPos, REASON_WAYPOINT, Builds[c].Par, Builds[c].Par, 0, (Builds[c].Par==240)?MoodPersonBone:0);
   }
}

//--------------------------------------------------------------------------------------------
//Berechnet den x-Index für das iPlate Array aus einer Brick-Position: (Alignment -1=l 0=c 1=r)
//--------------------------------------------------------------------------------------------
SLONG AIRPORT::CalcPlateXPosition (SLONG BuildIndex, SLONG BrickXOffset, SLONG Alignment)
{
   return CalcPlateXPosition(Builds[BuildIndex], BrickXOffset, Alignment);
}

//--------------------------------------------------------------------------------------------
//Berechnet den x-Index für das iPlate Array aus einer Brick-Position: (Alignment -1=l 0=c 1=r)
//--------------------------------------------------------------------------------------------
SLONG AIRPORT::CalcPlateXPosition (BUILD &qBuild, SLONG BrickXOffset, SLONG Alignment)
{
   SLONG rc;
   SLONG AddX;

   switch (Alignment)
   {
      case -1: AddX=0;  break;                                                           //Linke Seite
      case  0: AddX=Bricks[qBuild.BrickId].GetBitmapDimension().x/2; break;  //Mitte
      case +1: AddX=Bricks[qBuild.BrickId].GetBitmapDimension().x;   break;  //Rechte Seite
      default: TeakLibW_Exception (FNL, ExcNever);
   }

   //Ist der Build in der oberen oder unteren Ebene:
   if (Bricks[qBuild.BrickId].GetBitmapDimension().y-2+qBuild.ScreenPos.y<210)
   {
      //obere Ebene:
      rc = (AddX+qBuild.ScreenPos.x-PlateOffset.x+11+BrickXOffset + 
            (Bricks[qBuild.BrickId].GetBitmapDimension().y+
             qBuild.ScreenPos.y-2-93)/2)/44-1;
   }
   else
   {
      //untere Ebene:
      rc = (AddX+qBuild.ScreenPos.x-PlateOffset.x+11+BrickXOffset +
            (Bricks[qBuild.BrickId].GetBitmapDimension().y+
             qBuild.ScreenPos.y-2-220)/2)/44-1;
   }

   if (rc<0 || rc>=PlateDimension.x) { /*hprintf (0, "(x=%li) in Line %li", rc, __LINE__);*/ return (0);}

   return (rc);
}

//--------------------------------------------------------------------------------------------
//Berechnet den y-Index für das iPlate Array aus einer Brick-Position:
//--------------------------------------------------------------------------------------------
SLONG AIRPORT::CalcPlateYPosition (SLONG BuildIndex, SLONG BrickYOffset)
{
   return CalcPlateYPosition(Builds[BuildIndex], BrickYOffset);
}

//--------------------------------------------------------------------------------------------
//Berechnet den y-Index für das iPlate Array aus einer Brick-Position:
//--------------------------------------------------------------------------------------------
SLONG AIRPORT::CalcPlateYPosition (BUILD &qBuild, SLONG BrickYOffset)
{
   SLONG rc;

   //Ist der Build in der oberen oder unteren Ebene:
   if (Bricks[qBuild.BrickId].GetBitmapDimension().y-2+qBuild.ScreenPos.y<210)
   {
      //obere Ebene:
      rc = (Bricks[qBuild.BrickId].GetBitmapDimension().y-2+qBuild.ScreenPos.y-93+BrickYOffset+2200)/22-100;
   }
   else
   {
      //untere Ebene:
      rc = 5+(Bricks[qBuild.BrickId].GetBitmapDimension().y-2+qBuild.ScreenPos.y-220+BrickYOffset+2200)/22-100;
   }

   //Nicht ungefährlich, denn das passiert noch öfter
   //if (rc<0 || rc>=PlateDimension.y) { return (0); }

   return (rc);
}

//--------------------------------------------------------------------------------------------
//Berechnet das iPlate Array neu:
//--------------------------------------------------------------------------------------------
void AIRPORT::CalcPlates (void)
{
   SLONG c, d, e;          //Indices
   SLONG x, y, x2;         //Koordinaten
   SLONG BestD, BestE;     //Index-Speicherungen
   SLONG AnzDoors=0;
   SLONG AnzTriggers=0;
   SLONG AnzClipMarkers=0;

#define FUCK(a) {if((a)<0 || (a)>=(PlateDimension.x-20)*PlateDimension.y)DebugBreak();}

   LeftEnd = -99999;
   RightEnd = 99999;

   //Floor-Offset (Zahl der Pixel in X-Richtung beim "Floor" Objekt bis wieder eine Platte beginnt)
   const SLONG FO=33;

   //Die Grenzen des ganzen Flughafens ausloten:
   for (d=e=0, BestD=BestE=-1; d<SLONG(Builds.AnzEntries()); d++)
      if (Builds.IsInAlbum(d) /*&& Builds[d].BrickId >= 100+0x10000000 && Builds[d].BrickId <= 109+0x10000000*/)
      {
         if (BestD == -1 || Builds[d].ScreenPos.x>Builds[BestD].ScreenPos.x) BestD = d;
         if (BestE == -1 || Builds[d].ScreenPos.x<Builds[BestE].ScreenPos.x) BestE = d;
      }

   if (BestD==-1 || BestE==-1)
   {
      hprintf (0, "CalcPlates failed harmlessly...");
      return;
   }

   //Offset berechnen:
   PlateOffset.x = FO+9;
   PlateOffset.y = 0;

   //Array anlegen:
   PlateDimension.x = (Builds[BestD].ScreenPos.x /*-Builds[BestE].ScreenPos.x*/)/44+40;
   PlateDimension.y = 16;

   iPlate.ReSize (0);
   iPlate.ReSize (PlateDimension.x*PlateDimension.y);
   iPlateDir.ReSize (0);
   iPlateDir.ReSize (PlateDimension.x*PlateDimension.y);
   memset (iPlate, 0xfc, iPlate.AnzEntries());
   memset (iPlateDir, 0xff, iPlateDir.AnzEntries());
   iPlate+=20*16;
   iPlateDir+=20*16;

   SeatsTaken.ReSize (0);
   SeatsTaken.ReSize (PlateDimension.x);
   SeatsTaken+=20;
   CalcSeats ();

   Doors.ReSize(0);
   Doors.ReSize(30);
   Triggers.ReSize(0);
   Triggers.ReSize(30);

   ClipMarkers.ReSize(0);
   ClipMarkers.ReSize(40);

   //Primitive Hindernisse eintragen:
   for (c=0; c<SLONG(Builds.AnzEntries()); c++)
   {
      if (Builds.IsInAlbum(c))
      {
         switch (Bricks[Builds[c].BrickId].ObstacleType)
         {
            //Kein Hindernis
            case OBST_NONE:
               if (Builds[c].BrickId>=0x10000000+718 && Builds[c].BrickId<=0x10000000+729)
               {
                  x = CalcPlateXPosition (c, 0, 0);
                  y = CalcPlateYPosition (c, Bricks[Builds[c].BrickId].FloorOffset+(Builds[c].BrickId==0x10000000+729)*30);
                  Builds[c].Par=UBYTE(AnzDoors);
                  Doors[AnzDoors].ArrayPos=XY(x,y);
                  Doors[AnzDoors].State=0;
                  Doors[AnzDoors].Dir=0;
                  Doors[AnzDoors].Winkel=0;
                  //Doors[AnzDoors].BuildIndex=c;
                  if (Builds[c].BrickId==0x10000000+727)
                     Doors[AnzDoors].ArabDoor=TRUE;
                  else if (Builds[c].BrickId==0x10000000+719 || Builds[c].BrickId==0x10000000+718)
                     Doors[AnzDoors].ArabDoor=3;
                  else if (Builds[c].BrickId==0x10000000+729)
                     Doors[AnzDoors].ArabDoor=2;
                  else
                     Doors[AnzDoors].ArabDoor=FALSE;
                  AnzDoors++;
                  if (AnzDoors>=Doors.AnzEntries()) DebugBreak();
               }
               else if (Builds[c].BrickId==0x10000000+BRICK_KASTEN || Builds[c].BrickId==0x10000000+BRICK_ELECTRO)
               {
                  Builds[c].Par=UBYTE(AnzTriggers);
                  Triggers[AnzTriggers].ArrayPos=Builds[c].ScreenPos;
                  Triggers[AnzTriggers].State=0;
                  Triggers[AnzTriggers].Dir=0;
                  Triggers[AnzTriggers].Winkel=0;

                  AnzTriggers++;
                  if (AnzTriggers>=Triggers.AnzEntries()) DebugBreak();
               }
               break;

            //Punktförmiges Hindernis:
            case OBST_POINT:
               x = CalcPlateXPosition (c, 0, 0);
               y = CalcPlateYPosition (c, 0);
               if (y>=0 && y<=15 && x>=0)
               {
                  FUCK(y+(x<<4));iPlate[y+(x<<4)] = 0;
                  FUCK(y+1+(x<<4));iPlate[y+1+(x<<4)] &= (~128);
                  FUCK(y-1+(x<<4));iPlate[y-1+(x<<4)] &= (~32);
                  FUCK(y+((x+1)<<4));iPlate[y+((x+1)<<4)] &= (~16);
                  FUCK(y+((x-1)<<4));iPlate[y+((x-1)<<4)] &= (~64);
               }
               //if (y>=0 && y<=15 && x>=0) {FUCK(y+(x<<4));iPlate[y+(x<<4)] &= (~12);}
               break;

            case OBST_SITPOINT:
               x = CalcPlateXPosition (c, 0, 0);
               y = CalcPlateYPosition (c, -10);
               if (y>=0 && y<=15 && x>=0)
               {
                  FUCK(y+(x<<4));iPlate[y+(x<<4)] = 0;
                  FUCK(y+1+(x<<4));iPlate[y+1+(x<<4)] &= (~128);
                  FUCK(y-1+(x<<4));iPlate[y-1+(x<<4)] &= (~32);
                  FUCK(y+((x+1)<<4));iPlate[y+((x+1)<<4)] &= (~16);
                  FUCK(y+((x-1)<<4));iPlate[y+((x-1)<<4)] &= (~64);
               }
               //if (y>=0 && y<=15 && x>=0) {FUCK(y+(x<<4));iPlate[y+(x<<4)] &= (~12);}
               break;

            //Breites Hindernis:
            case OBST_WIDE:
               x = CalcPlateXPosition (c, 10, -1);
               x2 = CalcPlateXPosition (c, -10, 1);
               y = CalcPlateYPosition (c, 0);
               if (y>=0 && y<=15 && x>=0) for (d=x; d<=x2; d++) {FUCK(y+(d<<4));iPlate[y+(d<<4)] &= (~12);}
               break;

            //Schmale Horizontale Abgrenzung: 
            case 4:
               x  = CalcPlateXPosition (c, 10, -1);
               x2 = x;   //12.4.98
               y = CalcPlateYPosition (c, -8);
               if (y>=0 && y<=15 && x>=0) for (d=x; d<=x2; d++) {FUCK(y+(d<<4));iPlate[y+(d<<4)] &= (~32);      }
               if (y>=-1 && y<=14 && x>=0) for (d=x; d<=x2; d++) {FUCK(y+1+(d<<4));iPlate[y+1+(d<<4)] &= (~128);  }
               break;

            //Horizontale Abgrenzung: 
            case OBST_HORIZONTAL:
               x  = CalcPlateXPosition (c, 10, -1);
               x2 = CalcPlateXPosition (c, -10, 1);
               y = CalcPlateYPosition (c, -8);
               if (y>=0 && y<=15 && x>=0) for (d=x; d<=x2; d++) {FUCK(y+(d<<4));iPlate[y+(d<<4)] &= (~32);      }
               if (y>=-1 && y<=14 && x>=0) for (d=x; d<=x2; d++) {FUCK(y+1+(d<<4));iPlate[y+1+(d<<4)] &= (~128);  }
               break;

            //Horizontale Abgrenzung: (verschoben)
            case 14:
               x  = CalcPlateXPosition (c, 10, -1);
               x2 = CalcPlateXPosition (c, -10, 1)-1;
               y = CalcPlateYPosition (c, -8)-1;
               if (y>=0 && y<=15 && x>=0) for (d=x; d<=x2; d++) {FUCK(y+(d<<4));iPlate[y+(d<<4)] &= (~32);      }
               if (y>=-1 && y<=14 && x>=0) for (d=x; d<=x2; d++) {FUCK(y+1+(d<<4));iPlate[y+1+(d<<4)] &= (~128);  }
               break;

            //Große Vertikale Abgrenzung: (verändert nur die Ausgänge einer Plate)
            case OBST_BIGVERTIKAL:
               x = CalcPlateXPosition (c, -10+4, -1);
               y = CalcPlateYPosition (c, -8)-1;
               if (y>=0 && y<=15 && x>=0) { FUCK(y+(x<<4)); iPlate[y+(x<<4)] &= (~64); {FUCK(y+((x+1)<<4));iPlate[y+((x+1)<<4)] &= (~16); }}

            //Vertikale Abgrenzung: (verändert nur die Ausgänge einer Plate)
            case OBST_VERTIKAL:
               x = CalcPlateXPosition (c, -10+4, -1);
               y = CalcPlateYPosition (c, -8);
               if (y>=0 && y<=15 && x>=0) { FUCK(y+(x<<4)); iPlate[y+(x<<4)] &= (~64); {FUCK(y+((x+1)<<4));iPlate[y+((x+1)<<4)] &= (~16); }}
               break;

            //Horizontale Ausgang in der Fensterreihe hinten:
            case OBST_HORIZONTALEXIT:
               x  = CalcPlateXPosition (c, 10, -1);
               x2 = CalcPlateXPosition (c, -10, 1);
               y = CalcPlateYPosition (c, -8);
               if (y>=0 && y<=15 && x>=0) for (d=x; d<=x2; d++) if (d!=x2-2) {FUCK(y+(d<<4));iPlate[y+(d<<4)] &= (~32);}
               y = CalcPlateYPosition (c, 8);
               if (y>=0 && y<=15 && x>=0) for (d=x; d<=x2; d++) if (d!=x2-2) {FUCK(y+(d<<4));iPlate[y+(d<<4)] &= (~128); }
               break;

            //Der Eingang zu einem Geschäft / Museum:
            case OBST_SHOPFRONT:
            case OBST_MUSEUM:
               x  = CalcPlateXPosition (c, 10, -1);
               x2 = CalcPlateXPosition (c, -10, 1);
               y = CalcPlateYPosition (c, -8);
               if (y>=0 && y<=15 && x>=0) for (d=x; d<x2; d++) {FUCK(y+(d<<4));iPlate[y+(d<<4)] &= (~32);       }
               if (y>=-1 && y<=14 && x>=0) for (d=x; d<x2; d++) {FUCK(y+1+(d<<4));iPlate[y+1+(d<<4)] &= (~128);}
               for (y--;y>=0 && y<=14 && y!=4; y--)
               {
                  if (Bricks[Builds[c].BrickId].ObstacleType==OBST_SHOPFRONT && x>=0)
                  {
                     {FUCK(y+(x<<4));}
                     {FUCK(y+((x-1)<<4));}
                     iPlate[y+((x-1)<<4)]  &= (~64);   //linke Wand (außen)
                     iPlate[y+(x<<4)]      &= (~16);   //linke Wand (innen)
                  }
                  if (x2>=0)
                  {
                     {FUCK(y+(x2<<4));     }
                     {FUCK(y+((x2+1)<<4));}
                     iPlate[y+((x2+1)<<4)] &= (~16);   //rechte Wand (außen)
                     iPlate[y+(x2<<4)]     &= (~64);   //rechte Wand (innen)
                  }
               }
               if (y>=0 && y<=15 && x>=0) for (d=x; d<=x2; d++) {FUCK(y+(d<<4));iPlate[y+(d<<4)] &= (~128);}
               break;

            //Der Eingang zur Treppe: 
            case OBST_STAIRFRONT:
               x  = CalcPlateXPosition (c, 10, -1);
               x2 = CalcPlateXPosition (c, -10, 1);
               y = CalcPlateYPosition (c, -8);
               if (y>=0 && y<=15 && x>=0) for (d=x; d<x2; d++) {FUCK(y+(d<<4));iPlate[y+(d<<4)] &= (~32);        }
               if (y>=-1 && y<=14 && x>=0) for (d=x; d<x2; d++) {FUCK(y+1+(d<<4));iPlate[y+1+(d<<4)] &= (~128); }
               for (;y>=0 && y<=15 && y!=4; y--)
               {
                  if (x>=0)
                  {
                     {FUCK(y+((x-1)<<4));}
                     {FUCK(y+(x<<4));}
                     iPlate[y+((x-1)<<4)]  &= (~64);   //linke Wand (außen)
                     iPlate[y+(x<<4)]      &= (~16);   //linke Wand (innen)
                  }
                  if (x2>=0)
                  {
                     {FUCK(y+((x2+1)<<4)); }
                     {FUCK(y+(x2<<4));}
                     iPlate[y+((x2+1)<<4)] &= (~16);   //rechte Wand (außen)
                     iPlate[y+(x2<<4)]     &= (~64);   //rechte Wand (innen)
                  }
               }
               break;

            case OBST_BUROFRONT_L:
               x  = CalcPlateXPosition (c, 10, -1);
               x2 = CalcPlateXPosition (c, -10, 1);
               y = CalcPlateYPosition (c, -8);
               if (y>=0 && y<=14 && x>=0) //ex:15
                  for (d=x; d<=x2; d++) 
                  {
                     if (d!=x+1)
                     {
                        {FUCK(y+(d<<4));   }
                        {FUCK(y+1+(d<<4));}
                        iPlate[y+(d<<4)] &= (~32);
                        iPlate[y+1+(d<<4)] &= (~128);
                     }
                  }

               for (;y>=0 && y<=15 && y!=4; y--)
               {
                  if (x>=0)
                  {
                     {FUCK(y+((x-1)<<4));}
                     {FUCK(y+(x<<4));      }
                     iPlate[y+((x-1)<<4)]  &= (~64);   //linke Wand (außen)
                     iPlate[y+(x<<4)]      &= (~16);   //linke Wand (innen)
                  }
                  if (x2>=0)
                  {
                     {FUCK(y+((x2+1)<<4));}
                     {FUCK(y+(x2<<4));}
                     iPlate[y+((x2+1)<<4)] &= (~16);   //rechte Wand (außen)
                     iPlate[y+(x2<<4)]     &= (~64);   //rechte Wand (innen)
                  }
               }
               break;

            case OBST_BUROFRONT_R:
               x  = CalcPlateXPosition (c, 10, -1);
               x2 = CalcPlateXPosition (c, -10, 1);
               y = CalcPlateYPosition (c, -8);
               if (y>=0 && y<=14 && x>=0) //ex:15
                  for (d=x; d<=x2; d++) 
                  {
                     if (d!=x2-1)
                     {
                        {FUCK(y+(d<<4));   }
                        {FUCK(y+1+(d<<4));}
                        iPlate[y+(d<<4)] &= (~32);
                        iPlate[y+1+(d<<4)] &= (~128);
                     }
                  }

               for (;y>=0 && y<=15 && y!=4; y--)
               {
                  if (x>=0)
                  {
                     {FUCK(y+((x-1)<<4));}
                     {FUCK(y+(x<<4));      }
                     iPlate[y+((x-1)<<4)]  &= (~64);   //linke Wand (außen)
                     iPlate[y+(x<<4)]      &= (~16);   //linke Wand (innen)
                  }
                  if (x2>=0)
                  {
                     {FUCK(y+((x2+1)<<4));}
                     {FUCK(y+(x2<<4));}
                     iPlate[y+((x2+1)<<4)] &= (~16);   //rechte Wand (außen)
                     iPlate[y+(x2<<4)]     &= (~64);   //rechte Wand (innen)
                  }
               }
               break;

            //Kein Eingang: (z.B. wegen Kiosk)
            case OBST_NOENTRY1:
               x  = CalcPlateXPosition (c, 10, -1);
               x2 = CalcPlateXPosition (c, -10, 1);
               y = CalcPlateYPosition (c, -8);
               if (y>=0 && y<=15 && x>=0)
                  for (d=x; d<=x2; d++) 
                  {
                     {FUCK(y+(d<<4));   }
                     {FUCK(y+1+(d<<4));}
                     iPlate[y+(d<<4)] &= (~32);
                     iPlate[y+1+(d<<4)] &= (~128);
                  }

               for (;y>=0 && y<=15 && y!=4; y--)
               {
                  if (x>=0)
                  {
                     {FUCK(y+((x-1)<<4));}
                     {FUCK(y+(x<<4));     }
                     iPlate[y+((x-1)<<4)]  &= (~64);   //linke Wand (außen)
                     iPlate[y+(x<<4)]      &= (~16);   //linke Wand (innen)
                  }
                  if (x2>=0)
                  {
                     {FUCK(y+((x2+1)<<4));}
                     {FUCK(y+(x2<<4));     }
                     iPlate[y+((x2+1)<<4)] &= (~16);   //rechte Wand (außen)
                     iPlate[y+(x2<<4)]     &= (~64);   //rechte Wand (innen)
                  }
               }
               break;

            //Kein Eingang: (z.B. wegen Arab-Air)
            case OBST_NOENTRY2:
               x  = CalcPlateXPosition (c, 10, -1);
               x2 = CalcPlateXPosition (c, -60, 1);     //ex -10
               y = CalcPlateYPosition (c, -8);
               if (y>=0 && y<=14 && x>=0)  //ex: 15
                  for (d=x; d<=x2; d++) 
                  {
                     {FUCK(y+(d<<4));   }
                     {FUCK(y+1+(d<<4));}
                     iPlate[y+(d<<4)] &= (~32);
                     iPlate[y+1+(d<<4)] &= (~128);
                  }

               for (;y>=0 && y<=15 && y!=4; y--)
               {
                  if (x>=0)
                  {
                     {FUCK(y+((x-1)<<4));}
                     {FUCK(y+(x<<4));     }
                     iPlate[y+((x-1)<<4)]  &= (~64);   //linke Wand (außen)
                     iPlate[y+(x<<4)]      &= (~16);   //linke Wand (innen)
                  }
                  if (x2>=0)
                  {
                     {FUCK(y+((x2+1)<<4));}
                     {FUCK(y+(x2<<4));     }
                     iPlate[y+((x2+1)<<4)] &= (~16);   //rechte Wand (außen)
                     iPlate[y+(x2<<4)]     &= (~64);   //rechte Wand (innen)
                  }
               }
               break;

            case OBST_UP:
               x  = CalcPlateXPosition (c, 0, 0);
               y = CalcPlateYPosition (c, 0);
               iPlate[y+(x<<4)] &= (~112);
               break;

            case OBST_RIGHT:
               x  = CalcPlateXPosition (c, 0, 0);
               y = CalcPlateYPosition (c, 0);
               iPlate[y+(x<<4)] &= (~176);
               break;

            case OBST_DOWN:
               x  = CalcPlateXPosition (c, 0, 0);
               y = CalcPlateYPosition (c, 0);
               iPlate[y+(x<<4)] &= (~208);
               break;

            case OBST_LEFT:
               x  = CalcPlateXPosition (c, 0, 0);
               y = CalcPlateYPosition (c, 0);
               iPlate[y+(x<<4)] &= (~224);
               break;

            //Die Runen stellen immer Sonderfälle dar:
            case OBST_RUNE:
               x  = CalcPlateXPosition (c, 0, 0);
               y = CalcPlateYPosition (c, 0);

               if (y>=0 && y<=15 && x>=0) 
                  {FUCK(y+(x<<4));
                  switch (Builds[c].BrickId-0x10000000)
                  {
                     //Direction Runes:
                     case RUNE_UP:    iPlate[y+(x<<4)] &= (~112); break;
                     case RUNE_RIGHT: iPlate[y+(x<<4)] &= (~176); break;
                     case RUNE_SOUTH: iPlate[y+(x<<4)] &= (~208); break;
                     case RUNE_LEFT:  iPlate[y+(x<<4)] &= (~224); break;

                     //Crossroads Direction Runes:
                     case RUNE_UPRIGHT:    iPlate[y+(x<<4)] &= (~48);  break;
                     case RUNE_RIGHTSOUTH: iPlate[y+(x<<4)] &= (~144); break;
                     case RUNE_SOUTHLEFT:  iPlate[y+(x<<4)] &= (~192); break;
                     case RUNE_LEFTUP:     iPlate[y+(x<<4)] &= (~96);  break;

                     //Laufband Direction Runes:
                     case RUNE_BELT_RIGHT: iPlate[y+(x<<4)] = UBYTE(iPlate[y+(x<<4)] & (~176) | 1); break;
                     case RUNE_BELT_LEFT:  iPlate[y+(x<<4)] = UBYTE(iPlate[y+(x<<4)] & (~224) | 1); break;

                     case RUNE_WESTEAST:   iPlate[y+(x<<4)] &= (~160); break;
                     case RUNE_NORTHSOUTH: iPlate[y+(x<<4)] &= (~80);  break;

                     //Sonstige Runen:
                     case RUNE_NOPLAYER: iPlate[y+(x<<4)] &= (~4); break;
                     case RUNE_NOCLIENT: iPlate[y+(x<<4)] &= (~8); break;

                     case RUNE_LEFTEND:  LeftEnd = x*44; break;
                     case RUNE_RIGHTEND: RightEnd = x*44; break;

                     //Künstliche Shop-begrenzungen:
                     case RUNE_NOLEFT: iPlate[y+(x<<4)] &= (~16); iPlate[y+((x-1)<<4)] &= (~64); break;
                     case RUNE_NODOWN: iPlate[y+(x<<4)] &= (~32); iPlate[(y+1)+(x<<4)] &= (~128); break;
                     case RUNE_NOWAY:
                        iPlate[y+(x<<4)] &= (~32);     iPlate[(y+1)+(x<<4)] &= (~128);
                        iPlate[(y-1)+(x<<4)] &= (~32); iPlate[y+(x<<4)] &= (~128);
                        iPlate[y+(x<<4)] &= (~16);     iPlate[y+((x-1)<<4)] &= (~64);
                        iPlate[y+((x+1)<<4)] &= (~16); iPlate[y+(x<<4)] &= (~64);
                        break;

                     case RUNE_CLIPMIDDLE:
                     case RUNE_CLIPFULL:
                     case RUNE_CLIPNONE:
                        ClipMarkers[AnzClipMarkers].Type=Builds[c].BrickId-0x10000000;
                        ClipMarkers[AnzClipMarkers].Position=Builds[c].ScreenPos.x;
                        AnzClipMarkers++;
                        if (AnzClipMarkers>=ClipMarkers.AnzEntries()) DebugBreak();
                        break;

                     //Nicht alle Runen werden an dieser Stelle behandelt:
                     default: break;
               }}
               break;

            default:
               TeakLibW_Exception (FNL, ExcNever);
               break;
         }
      }
   }

   Doors.ReSize(AnzDoors);
   ClipMarkers.ReSize(AnzClipMarkers);

   //Clip-Markierungen sortieren:
   for (x=0; x<AnzClipMarkers-1; x++)
      if (ClipMarkers[x].Position>ClipMarkers[x+1].Position)
      {
         CClipMarker tmp;

         tmp=ClipMarkers[x]; ClipMarkers[x]=ClipMarkers[x+1]; ClipMarkers[x+1]=tmp;

         x-=2;
         if (x<-1) x=-1;
      }

   //hprintf (0, "Airport goes from %li to %li", LeftEnd, RightEnd);

   //Plate Ausgänge abgleichen:
   /*for (x=0; x<PlateDimension.x-20; x++)
      for (y=0; y<PlateDimension.y; y++)
      {
         if (y>0 && (iPlate [(x<<4) + y-1] & 8)==0)                    iPlate[(x<<4)+y] &= (~128);
         if (x<PlateDimension.x-1 && (iPlate [((x+1)<<4) + y] & 8)==0) iPlate[(x<<4)+y] &= (~64);
         if (y<PlateDimension.y-1 && (iPlate [(x<<4) + y+1] & 8)==0)   iPlate[(x<<4)+y] &= (~32);
         if (x>0 && (iPlate [((x-1)<<4) + y] & 8)==0)                  iPlate[(x<<4)+y] &= (~16);
      } */
}

//--------------------------------------------------------------------------------------------
//Schaut, ob die Türen bewegt werden müssen:
//--------------------------------------------------------------------------------------------
void AIRPORT::PumpDoors (void)
{
   SLONG c;
   SLONG ArabIndex = Bricks((SLONG)0x10000000+727);

   for (c=Doors.AnzEntries()-1; c>=0; c--)
   {
      if (Doors[c].ArabDoor==TRUE)
      {
         if (Doors[c].Dir==1) //Feuerlöscher nach innen drehen ("öffnen")
         {
            if (Doors[c].State>=40)
            {
               Doors[c].Dir=0;

               //Tür erneut drehen?
               if (!IsRoomBusy(ROOM_SABOTAGE, -1))
               {
                  Doors[c].State=(10-1)*2;
                  Doors[c].Dir=-1;
               }
            }
            else Doors[c].State+=2;                    //ex:6

            /*if (IsRoomBusy(ROOM_SABOTAGE, -1))
            {
               //Tür resetten, weil Spieler nur halb reingegangen ist
               Doors[c].State=(10-1)*2;
               Doors[c].Dir=-1;
            } */

            Doors[c].Winkel=UBYTE(min ((10-1)*2, Doors[c].State+1)/2);
            //Doors[c].Winkel=UBYTE(min ((Bricks[Builds[Doors[c].BuildIndex].BrickId].Bitmap.AnzEntries()-1)*2, Doors[c].State+1)/2);
         }
         else if (Doors[c].Dir==-1) //Feuerlöscher wieder nach außen drehen ("schließen")
         {
            Doors[c].State--;
            if (Doors[c].State==0 && Doors[c].Dir!=0)
            {
               Doors[c].Dir=0;

               //Tür erneut drehen?
               if (IsRoomBusy(ROOM_SABOTAGE, -1))
               {
                  Doors[c].Dir=1;
               }
            }                                          //ex:6

            Doors[c].Winkel=UBYTE(min ((10-1)*2, Doors[c].State+1)/2);
         }
      }
      else if (Doors[c].ArabDoor==3)
      {
         if (Doors[c].Dir==1)
         {
            if (Doors[c].State>=22) Doors[c].Dir=-1;
            else Doors[c].State+=4;

            Doors[c].Winkel=UBYTE(min ((6-1)*2, Doors[c].State+1)/2);
         }
         else if (Doors[c].Dir==-1)
         {
            Doors[c].State--;
            if (Doors[c].State==0 && Doors[c].Dir!=0)
            {
               Doors[c].Dir=0;
            }

            Doors[c].Winkel=UBYTE(min ((6-1)*2, Doors[c].State+1)/2);
         }
      }
      else
      {
         if (Doors[c].Dir==1)
         {
            if (Doors[c].State>=18) Doors[c].Dir=-1;
            else Doors[c].State+=2;

            Doors[c].Winkel=UBYTE(min ((4-1)*2, Doors[c].State+1)/2);
         }
         else if (Doors[c].Dir==-1)
         {
            Doors[c].State--;
            if (Doors[c].State==0 && Doors[c].Dir!=0)
            {
               Doors[c].Dir=0;

               if (Sim.Players.Players[Sim.localPlayer].GetRoom()==ROOM_AIRPORT && abs(Sim.Players.Players[Sim.localPlayer].ViewPos.x+320-Doors[c].ArrayPos.x*44)<400)
                  gDoorClose.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7);
            }

            Doors[c].Winkel=UBYTE(min ((4-1)*2, Doors[c].State+1)/2);
         }
      }
   }
}

//--------------------------------------------------------------------------------------------
//Schaut, ob die Türen bewegt werden müssen:
//--------------------------------------------------------------------------------------------
void AIRPORT::TryDoor (XY ArrayPos, BOOL Player, SLONG PlayerNum)
{
   SLONG c;

   if (Sim.CallItADay) return;

   for (c=Doors.AnzEntries()-1; c>=0; c--)
   {
      if (Doors[c].ArabDoor==2) //Frachttür
      {
         if (!Player) continue;
         if (Sim.Players.Players[PlayerNum].ExRoom!=ROOM_FRACHT && Sim.Players.Players[PlayerNum].DirectToRoom!=ROOM_FRACHT) continue;
      }

      if (Doors[c].ArabDoor==TRUE)
      {
         //if (Doors[c].ArrayPos.x==ArrayPos.x && (Doors[c].ArrayPos.y==ArrayPos.y-1 || Doors[c].ArrayPos.y==ArrayPos.y || Doors[c].ArrayPos.y==ArrayPos.y+1))
         if (Doors[c].ArrayPos.x==ArrayPos.x && (Doors[c].ArrayPos.y==ArrayPos.y-1 || Doors[c].ArrayPos.y==ArrayPos.y || Doors[c].ArrayPos.y==ArrayPos.y+1))
         {
            if (Doors[c].Dir==0 && Doors[c].State==0)
            {
               Doors[c].Dir=1;
               if (Sim.Players.Players[Sim.localPlayer].GetRoom()==ROOM_AIRPORT && abs(Sim.Players.Players[Sim.localPlayer].ViewPos.x+320-Doors[c].ArrayPos.x*44)<400)
                  gArabDoorFx.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7);
            }
            else if (Doors[c].Dir==0 && Doors[c].State>=40 && ArrayPos.y!=9)
            {
               Doors[c].State=(10-1)*2;
               Doors[c].Dir=-1;
               if (Sim.Players.Players[Sim.localPlayer].GetRoom()==ROOM_AIRPORT && abs(Sim.Players.Players[Sim.localPlayer].ViewPos.x+320-Doors[c].ArrayPos.x*44)<400)
                  gArabDoorFx.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7);
            }
         }
      }
      else 
         if ((Doors[c].ArrayPos.x==ArrayPos.x && ((Doors[c].ArrayPos.y==ArrayPos.y-1 && !Player) || Doors[c].ArrayPos.y==ArrayPos.y)) ||
             (Doors[c].ArrayPos.x==ArrayPos.x && Doors[c].ArrayPos.y==ArrayPos.y+4 && Doors[c].ArabDoor==3) ||
             (Doors[c].ArrayPos.x==ArrayPos.x+1 && Doors[c].ArrayPos.y==ArrayPos.y) ||
             (Doors[c].ArrayPos.x==ArrayPos.x-1 && Doors[c].ArrayPos.y==ArrayPos.y))
         {
            //Ist die Tür vermint?
            if (Player && ArrayPos.y<5 && ArrayPos.y>0)
            {
               for (SLONG c=0; c<SLONG(Runes.AnzEntries()); c++)
                  if (Runes[c].BrickId==0x10000000+RUNE_2SHOP && (Runes[c].Par==ROOM_BURO_A || Runes[c].Par==ROOM_BURO_B || Runes[c].Par==ROOM_BURO_C || Runes[c].Par==ROOM_BURO_D))
                  {
                     if (abs(Runes[c].ScreenPos.x-ArrayPos.x*44)<=88)
                        if (Sim.Players.Players[(Runes[c].Par-ROOM_BURO_A)/10].OfficeState==1)
                        {
                           if (Sim.Players.Players[(Runes[c].Par-ROOM_BURO_A)/10].Owner!=2)
                           {
                              SLONG OpferNum  = PlayerNum; //(Runes[c].Par-ROOM_BURO_A)/10;
                              SLONG OfficeNum = (Runes[c].Par-ROOM_BURO_A)/10;

                              PLAYER &qPlayer = Sim.Players.Players[OfficeNum];

                              qPlayer.OfficeState = 2;
                              qPlayer.pSmack      = new CSmack16;
                              qPlayer.pSmack->Open ("expl.smk");
                              qPlayer.NetSynchronizeFlags();

                              gUniversalFx.Stop();
                              gUniversalFx.ReInit("explode.raw");
                              gUniversalFx.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7);

                              SetConditionBlock (20+OfficeNum, 1);

                              if (Sim.bNetwork && qPlayer.Owner==0)
                                 Sim.SendSimpleMessage (ATNET_ADD_EXPLOSION, NULL, OfficeNum);

                              for (SLONG d=Sim.Persons.AnzEntries()-1; d>=0; d--)
                                 if (Sim.Persons.IsInAlbum (d) && Clans[(SLONG)Sim.Persons[d].ClanId].Type>=CLAN_PLAYER1 && Clans[(SLONG)Sim.Persons[d].ClanId].Type<=CLAN_PLAYER4)
                                    if ((Runes[c].ScreenPos-XY(-100,4900)-Sim.Persons[d].ScreenPos).abs()<100)
                                    {
                                       Sim.Persons[d].State    = Sim.Persons[d].State & ~PERSON_WAITFLAG;
                                       Sim.Persons[d].LookDir  = 4; //Gimmick starten
                                       Sim.Persons[d].Dir      = 8;
                                       Sim.Persons[d].Phase    = 0;
                                       Sim.Persons[d].StatePar = 0;
                                       Sim.Persons[d].Running  = 0;
                                       if (Sim.Persons[d].Position.y<5000) Sim.Persons[d].ScreenPos.x -= 5;

                                       Sim.Persons[d].Position.y  = Sim.Persons[d].Position.y/5000*5000+57;
                                       Sim.Persons[d].ScreenPos.y = 150;

                                       Sim.Persons[d].Target   = Sim.Persons[d].Position;

                                       PLAYER &qPlayer = Sim.Players.Players[(SLONG)Sim.Persons[d].State];
                                       XY      p=Sim.Persons[d].Position;
                                       XY      ArrayPos;

                                       if (p.y>0 && p.y>167)
                                       {
                                          ArrayPos.x = (p.x+4400)/44-100 /*+5*/;
                                          ArrayPos.y = (p.y+2200)/22-100+5;
                                       }
                                       else
                                       {
                                          ArrayPos.x = (p.x+4400)/44-100;
                                          ArrayPos.y = (p.y+2200)/22-100;
                                       }

                                       qPlayer.WalkStop ();
                                       qPlayer.TertiaryTarget=qPlayer.SecondaryTarget=qPlayer.PrimaryTarget=ArrayPos+XY(0,5000);
                                       qPlayer.BroadcastPosition ();
                                    }
                           }
                           else
                              Sim.Players.Players[(Runes[c].Par-ROOM_BURO_A)/10].WalkStop ();

                           return;
                        }
                  }
            }

            if (Doors[c].Dir!=1 && Doors[c].State==0)
            {
               if (bIngnoreNextDoor==FALSE)
                  if (Sim.Players.Players[Sim.localPlayer].GetRoom()==ROOM_AIRPORT && abs(Sim.Players.Players[Sim.localPlayer].ViewPos.x+320-Doors[c].ArrayPos.x*44)<400 && Doors[c].ArabDoor!=3)
                     gDoorOpen.Play(DSBPLAY_NOSTOP, Sim.Options.OptionEffekte*100/7);

               bIngnoreNextDoor=FALSE;
            }

            Doors[c].Dir=1;
         }
   }
}

//--------------------------------------------------------------------------------------------
//Sorgt dafür, daß nach neuem Tag/LoadGame eine Tür richtig (Explodiert oder nicht) angezeigt wird:
//--------------------------------------------------------------------------------------------
void AIRPORT::UpdateStaticDoorImage (void)
{
   SLONG c;

   for (c=0; c<4; c++)
   {
      long door;
      if (c==0) door=700;
      else if (c==1) door=707;
      else if (c==2) door=708;
      else if (c==3) door=709;

      if (Sim.Players.Players[c].OfficeState==2 || Editor)
      {
         Bricks[SLONG(0x10000000+734+c)].Bitmap[0].ReSize (Bricks[SLONG(0x10000000+734+4)].Bitmap[0].Size);
         Bricks[SLONG(0x10000000+734+c)].Bitmap[0].BlitFrom (Bricks[SLONG(0x10000000+734+4)].Bitmap[0]);
      }
      else if (Sim.Players.Players[c].OfficeState==3)
      {
         Bricks[SLONG(0x10000000+door)].Bitmap[0].ReSize (Bricks[SLONG(0x10000000+749)].Bitmap[0].Size);
         Bricks[SLONG(0x10000000+door)].Bitmap[0].BlitFrom (Bricks[SLONG(0x10000000+749)].Bitmap[0]);
      }
      else Bricks[0x10000000+734+c].Bitmap[0].Destroy();
   }
}

//--------------------------------------------------------------------------------------------
//Berechnet die Koordinaten einzelnen Spezialstellen neu:
//--------------------------------------------------------------------------------------------
void AIRPORT::CalcCoordinates (void)
{
   SLONG c, d, e, best, BestE;
   SLONG AnzRunes;
   SLONG AnzAreaMarkers=0;

   AnzRunes = 0;

   //Die Area-Runen automatisch ergänzen:
   for (c=0; c<SLONG(Builds.AnzEntries()); c++)
      if (Builds.IsInAlbum(c))
      {
         if (Builds[c].BrickId==0x10000000+RUNE_AREALO && Builds[c].Par==0)
         {
            for (d=0; d<SLONG(Builds.AnzEntries()); d++)
               if (Builds.IsInAlbum(d) &&
                   Builds[d].BrickId==0x10000000+RUNE_2SHOP &&
                   Builds[d].ScreenPos.x>=Builds[c].ScreenPos.x &&
                   Builds[d].ScreenPos.y>=Builds[c].ScreenPos.y &&
                   Builds[d].ScreenPos.x-Builds[c].ScreenPos.x<=176 &&
                   Builds[d].ScreenPos.y-Builds[c].ScreenPos.y<=88+44)
                   Builds[c].Par = Builds[d].Par;
            if (Builds[c].Par==0)
               for (d=0; d<SLONG(Builds.AnzEntries()); d++)
                  if (Builds.IsInAlbum(d) &&
                      Builds[d].BrickId==0x10000000+RUNE_SHOP &&
                      Builds[d].ScreenPos.x>=Builds[c].ScreenPos.x &&
                      Builds[d].ScreenPos.y>=Builds[c].ScreenPos.y &&
                      Builds[d].ScreenPos.x-Builds[c].ScreenPos.x<=176 &&
                      Builds[d].ScreenPos.y-Builds[c].ScreenPos.y<=88+44)
                      Builds[c].Par = Builds[d].Par;

            if (Builds[c].Par==0) DebugBreak();
         }
         else if (Builds[c].BrickId==0x10000000+RUNE_AREARU && Builds[c].Par==0)
         {
            for (d=0; d<SLONG(Builds.AnzEntries()); d++)
               if (Builds.IsInAlbum(d) &&
                   Builds[d].BrickId==0x10000000+RUNE_2SHOP &&
                   Builds[c].ScreenPos.x>=Builds[d].ScreenPos.x &&
                   Builds[c].ScreenPos.y>=Builds[d].ScreenPos.y &&
                   Builds[c].ScreenPos.x-Builds[d].ScreenPos.x<176 &&
                   Builds[c].ScreenPos.y-Builds[d].ScreenPos.y<88)
                   Builds[c].Par = Builds[d].Par;
            if (Builds[c].Par==0) 
               for (d=0; d<SLONG(Builds.AnzEntries()); d++)
                  if (Builds.IsInAlbum(d) &&
                      Builds[d].BrickId==0x10000000+RUNE_SHOP &&
                      Builds[c].ScreenPos.x>=Builds[d].ScreenPos.x &&
                      Builds[c].ScreenPos.y>=Builds[d].ScreenPos.y &&
                      Builds[c].ScreenPos.x-Builds[d].ScreenPos.x<176 &&
                      Builds[c].ScreenPos.y-Builds[d].ScreenPos.y<88)
                      Builds[c].Par = Builds[d].Par;
            if (Builds[c].Par==0) DebugBreak();
         }
      }

   for (c=d=0; c<SLONG(Builds.AnzEntries()); c++)
      if (Builds.IsInAlbum(c) && Builds[c].BrickId-0x10000000==RUNE_AREALO)
         AnzAreaMarkers++;

   //Area-Markierungen aufzeichnen:
   AreaMarkers.ReSize(0);
   AreaMarkers.ReSize(AnzAreaMarkers);
   for (c=d=0; c<SLONG(Builds.AnzEntries()); c++)
      if (Builds.IsInAlbum(c) && Builds[c].BrickId-0x10000000==RUNE_AREALO)
      { 
         AreaMarkers[d].Par=Builds[c].Par;
         AreaMarkers[d].p1=Builds[c].ScreenPos;

         BestE=0; best=999999;
         for (e=0; e<SLONG(Builds.AnzEntries()); e++)
            if (Builds.IsInAlbum(e) && Builds[e].Par==Builds[c].Par && Builds[e].BrickId-0x10000000==RUNE_AREARU)
            {
               if (abs(Builds[e].ScreenPos.x-Builds[c].ScreenPos.x)<best && Builds[e].ScreenPos.x>Builds[c].ScreenPos.x && Builds[e].ScreenPos.y>Builds[c].ScreenPos.y)
               {
                  BestE=e;
                  best=abs(Builds[e].ScreenPos.x-Builds[c].ScreenPos.x);
               }
            }

         AreaMarkers[d].p2=Builds[BestE].ScreenPos;
         d++;
      }

   //Die Zahl der für uns wichtigen Runen bestimmen:
   for (c=0; c<SLONG(Builds.AnzEntries()); c++)
      if (Builds.IsInAlbum(c) && Bricks[Builds[c].BrickId].ObstacleType==OBST_RUNE)
         if ((Builds[c].BrickId>=0x10000000+RUNE_CREATION && Builds[c].BrickId<=0x10000000+RUNE_PCREATION2) || (Builds[c].BrickId>=0x10000000+RUNE_CHECKIN && Builds[c].BrickId<=0x10000000+RUNE_2WAIT) || Builds[c].BrickId==0x10000000+RUNE_AREALO || Builds[c].BrickId==0x10000000+RUNE_AREARU || Builds[c].BrickId==0x10000000+RUNE_WAITPLANE || Builds[c].BrickId==0x10000000+RUNE_DROPSUITCASE || Builds[c].BrickId==0x10000000+RUNE_EXIT_SUITCASE || Builds[c].BrickId==0x10000000+RUNE_CREATE_SUITCASE || Builds[c].BrickId==0x10000000+RUNE_2CHECKIN_EXIT || Builds[c].BrickId==0x10000000+RUNE_WAYPOINT_START || Builds[c].BrickId==0x10000000+RUNE_WAYPOINT || Builds[c].BrickId==0x10000000+RUNE_WAYPOINT_G || Builds[c].BrickId==0x10000000+RUNE_WAYPOINT_WAIT || Builds[c].BrickId==0x10000000+RUNE_CONDBLOCK || Builds[c].BrickId==0x10000000+RUNE_DURCHLEUCHTER)
            AnzRunes++;

   Runes.ReSize (0);
   Runes.ReSize (AnzRunes);

   AnzRunes     = 0;       
   NumBeltSpots = 0;

   //Die Belt-Spots zählen:
   for (c=0; c<SLONG(Builds.AnzEntries()); c++)
      if (Builds.IsInAlbum(c) && Bricks[Builds[c].BrickId].ObstacleType==OBST_RUNE)
         if (Builds[c].BrickId==0x10000000+RUNE_SHOP && Builds[c].Par>=ROOM_BELT_X1 && Builds[c].Par<=ROOM_BELT_X10)
            NumBeltSpots = max (NumBeltSpots, Builds[c].Par-ROOM_BELT_X1);

   //Die Zahl der für uns wichtigen Runen bestimmen:
   for (c=0; c<SLONG(Builds.AnzEntries()); c++)
      if (Builds.IsInAlbum(c) && Bricks[Builds[c].BrickId].ObstacleType==OBST_RUNE)
         if ((Builds[c].BrickId>=0x10000000+RUNE_CREATION && Builds[c].BrickId<=0x10000000+RUNE_PCREATION2) || (Builds[c].BrickId>=0x10000000+RUNE_CHECKIN && Builds[c].BrickId<=0x10000000+RUNE_2WAIT) || Builds[c].BrickId==0x10000000+RUNE_AREALO || Builds[c].BrickId==0x10000000+RUNE_AREARU || Builds[c].BrickId==0x10000000+RUNE_WAITPLANE || Builds[c].BrickId==0x10000000+RUNE_DROPSUITCASE || Builds[c].BrickId==0x10000000+RUNE_EXIT_SUITCASE || Builds[c].BrickId==0x10000000+RUNE_CREATE_SUITCASE || Builds[c].BrickId==0x10000000+RUNE_2CHECKIN_EXIT || Builds[c].BrickId==0x10000000+RUNE_WAYPOINT_START || Builds[c].BrickId==0x10000000+RUNE_WAYPOINT || Builds[c].BrickId==0x10000000+RUNE_WAYPOINT_G || Builds[c].BrickId==0x10000000+RUNE_WAYPOINT_WAIT || Builds[c].BrickId==0x10000000+RUNE_CONDBLOCK || Builds[c].BrickId==0x10000000+RUNE_DURCHLEUCHTER)
         {
            SLONG x, y;

            Runes[AnzRunes]=Builds[c];
         
            if (Builds[c].BrickId!=0x10000000+RUNE_CONDBLOCK)
            {
               //Liegt die Rune unten oder oben?
               if (Bricks[Runes[AnzRunes].BrickId].GetBitmapDimension().y-2+Runes[AnzRunes].ScreenPos.y<170)
               {
                  //Oben! Dort sind Die Koordinaten ab 5000:
                  x = (Bricks[Runes[AnzRunes].BrickId].GetBitmapDimension().x/2+Runes[AnzRunes].ScreenPos.x-PlateOffset.x+11 + (Bricks[Runes[AnzRunes].BrickId].GetBitmapDimension().y+Runes[AnzRunes].ScreenPos.y-2-93)/2)/44-1;
                  y = (Bricks[Runes[AnzRunes].BrickId].GetBitmapDimension().y-2+Runes[AnzRunes].ScreenPos.y-93+2200)/22-100;
                  Runes[AnzRunes].ScreenPos=XY(x*44+22,y*22+11+5000);
               }
               else
               {
                  //Unten! Dort beginnen die Koordinaten bei 0:
                  x = (Bricks[Runes[AnzRunes].BrickId].GetBitmapDimension().x/2+Runes[AnzRunes].ScreenPos.x-PlateOffset.x+11 + (Bricks[Runes[AnzRunes].BrickId].GetBitmapDimension().y+Runes[AnzRunes].ScreenPos.y-2-220)/2)/44-1;
                  y = (Bricks[Runes[AnzRunes].BrickId].GetBitmapDimension().y-2+Runes[AnzRunes].ScreenPos.y-220+2200)/22-100;
                  Runes[AnzRunes].ScreenPos=XY(x*44+22,y*22+11);
               }
            }  

            AnzRunes++;
         }
}

//--------------------------------------------------------------------------------------------
//Entfernt alle unsichtbaren Laufinformations-Builds (auch Runen genannt):
//--------------------------------------------------------------------------------------------
void AIRPORT::RemoveRunes (void)
{
   SLONG c, d;

   for (c=d=0; c<(SLONG)Builds.AnzEntries(); c++)
   {
      //Einige Runen werden zum Abschuß freigegeben:
      if (Builds.IsInAlbum(c))
         switch (Builds[c].BrickId-0x10000000)
         {
            case RUNE_UP:
            case RUNE_RIGHT:
            case RUNE_SOUTH:
            case RUNE_LEFT:
            case RUNE_BELT_LEFT:
            case RUNE_BELT_RIGHT:
            case RUNE_LEFTEND:
            case RUNE_RIGHTEND:
            case RUNE_NOLEFT:
            case RUNE_NODOWN:
            case RUNE_NOWAY:
            case RUNE_NOPLAYER:
            case RUNE_NOCLIENT:
            case RUNE_CLIPFULL:
            case RUNE_CLIPMIDDLE:
            case RUNE_CLIPNONE:
            case RUNE_AREALO:
            case RUNE_AREARU:
               Builds-=c;
         }
   }
}

//--------------------------------------------------------------------------------------------
//Gibt für die On-Screen Informationen einen STATIC Pointer auf den Filenamen zurück:
//--------------------------------------------------------------------------------------------
char *AIRPORT::GetHallFilename (void)
{
	if (HallNum==0)
		return (bprintf (HallFilenames [HallNum], HallLevel[0], HallLevel[1], HallLevel[2], HallLevel[3], HallLevel[4], HallLevel[5], HallLevel[6], HallLevel[7], HallLevel[8]));
	else
		return (bprintf (HallFilenames [HallNum], HallLevel[0]));
}

//--------------------------------------------------------------------------------------------
//Sorgt dafür, daß die Builds die Bricks nicht-assoziativ adressieren; Nicht mit Airport Parts verwenden!
//--------------------------------------------------------------------------------------------
void AIRPORT::UnassociateBuilds (void)
{
   SLONG c;

   for (c=0; c<(SLONG)Builds.AnzEntries(); c++)
      if (Builds.IsInAlbum(c))
         if (Builds[c].BrickId<0x10000000+2000)
            Builds[c].BrickId = Bricks(Builds[c].BrickId);
}

//--------------------------------------------------------------------------------------------
//Hasht die Builds in diverse Buckets:
//--------------------------------------------------------------------------------------------
void AIRPORT::DoHashBuilds (void)
{
   SLONG c, d;

   HashBuilds.ReSize (0);
   HashBuilds.ReSize ((RightEnd-LeftEnd)/BUILDHASHSIZE);

   for (c=0; c<HashBuilds.AnzEntries(); c++)
   {
      HashBuilds[c].Builds.ReSize (Builds.AnzEntries());

      for (d=0; d<(SLONG)Builds.AnzEntries(); d++)
         if (Builds.IsInAlbum(d))
            if (Builds[d].ScreenPos.x+Bricks[Builds[d].BrickId].Bitmap[0].Size.x>LeftEnd+c*BUILDHASHSIZE &&
                Builds[d].ScreenPos.x<=LeftEnd+(c+3)*BUILDHASHSIZE)
            {
               HashBuilds[c]*=Builds[d];
            }

      HashBuilds[c].Builds.ReSize (HashBuilds[c].GetNumUsed());
   }
}

//--------------------------------------------------------------------------------------------
//Berechnet das Array mit den Sitzen:
//--------------------------------------------------------------------------------------------
void AIRPORT::CalcSeats (void)
{
   ULONG SeatsIndex[6]={0x10000000+370,0x10000000+371,0x10000000+372,0,0,0};
   SLONG c, d, x, y;

   for (c=0; c<3; c++)
      SeatsIndex[c+3]=Bricks(SeatsIndex[c]);

   SeatsTaken.FillWith (0xffff);

   //Primitive Hindernisse eintragen:
   for (c=SLONG(Builds.AnzEntries())-1; c>=0; c--)
   {
      if (Builds.IsInAlbum(c))
      {
         for (d=0; d<6; d++)
         {
            if (SeatsIndex[d]==ULONG(Builds[c].BrickId))
            {
               x = CalcPlateXPosition (c, 0, 0);
               y = CalcPlateYPosition (c, -10)-5;

               if (x<0 || x>=SeatsTaken.AnzEntries()-20) DebugBreak();

               SeatsTaken[x]=UWORD(SeatsTaken[x] & (~(1<<(y*2))));
               SeatsTaken[x]=UWORD(SeatsTaken[x] & (~(1<<(y*2+1))));
            }
         }
      }
   }
}

//--------------------------------------------------------------------------------------------
//Schaltet einen Block an oder aus:
//--------------------------------------------------------------------------------------------
void AIRPORT::SetConditionBlock (SLONG Id, BOOL Blocking)
{
   for (SLONG c=0; c<SLONG(Runes.AnzEntries()); c++)
      if (Bricks[Runes[c].BrickId].ObstacleType==OBST_RUNE)
         if (Runes[c].BrickId==0x10000000+RUNE_CONDBLOCK && Runes[c].Par==Id)
         {
            SLONG x = CalcPlateXPosition (Runes[c], 0, 0);
            SLONG y = CalcPlateYPosition (Runes[c], 0);

            if (Blocking)
            {
               iPlate[y-1+(x<<4)] &= (~32);
               iPlate[y+(x<<4)] &= (~128);
            }
            else
            {
               iPlate[y-1+(x<<4)] |= (32);
               iPlate[y+(x<<4)] |= (128);
            }
         }
}

//--------------------------------------------------------------------------------------------
//Generiert die Mapper-Tabelle für die Schalter-Schildchen
//--------------------------------------------------------------------------------------------
void AIRPORT::CreateGateMapper (void)
{
   SLONG c, d;
   SLONG AnzGates= GetNumberOfShops (RUNE_2WAIT);

   GateMapper.ReSize(AnzGates*2);
   GateMapper.FillWith (-1);

   for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
      if (!Sim.Players.Players[c].IsOut)
      {
         for (d=0; d<Sim.Players.Players[c].Gates.Gates.AnzEntries(); d++)
            if (Sim.Players.Players[c].Gates.Gates[d].Miete!=-1)
            {
               GateMapper[Sim.Players.Players[c].Gates.Gates[d].Nummer]=c;
            }
      }

   TextBricks.ReSize (AnzGates*2);
   TextBrickTexts.ReSize (AnzGates*2);
   RepaintTextBricks ();
}

//--------------------------------------------------------------------------------------------
//Erneuert den Inhalt der Textbricks:
//--------------------------------------------------------------------------------------------
void AIRPORT::RepaintTextBricks (void)
{
   SLONG c, d, e;
   CFlugplan *Plan;

   BUFFER<SLONG> TextPrinted(TextBricks.AnzEntries());

   for (c=TextBricks.AnzEntries()-1; c>=0; c--)
   {
      TextPrinted[c]=999;
      TextBrickTexts[c].Empty();
      TextBricks[c].ReSize (132, 12);
      TextBricks[c].FillWith (0);
   }

   for (c=0; c<Sim.Players.Players.AnzEntries(); c++)
      if (!Sim.Players.Players[c].IsOut)
      {
         for (d=0; d<Sim.Players.Players[c].Gates.Gates.AnzEntries(); d++)
            if (Sim.Players.Players[c].Gates.Gates[d].Miete!=-1)
               GateMapper[Sim.Players.Players[c].Gates.Gates[d].Nummer]=c;
      }

   //Für alle Spieler:
   for (c=0; c<Sim.Players.AnzPlayers; c++)
   {
      //Für alle Flugzeuge des Spielers:
      for (d=0; d<(SLONG)Sim.Players.Players[c].Planes.AnzEntries(); d++)
      {
         if (Sim.Players.Players[c].Planes.IsInAlbum(d))
         {
            Plan = &Sim.Players.Players[c].Planes[d].Flugplan;

            //Für alle Flüge des Flugzeuges:
            e= Sim.Players.Players[c].Planes[d].Flugplan.NextStart;

            if (e!=-1)
            {
               //Dadurch wird's unten kompakter:Eintrag vorhanden?
               if ((Plan->Flug[e].ObjectType==1 || Plan->Flug[e].ObjectType==2) && Plan->Flug[e].Gate>=0)
               {
                  //Check-In:
                  if (Plan->Flug[e].Startdate==Sim.Date && (Plan->Flug[e].Startzeit-1==Sim.GetHour() || Plan->Flug[e].Startzeit==Sim.GetHour()) && Plan->Flug[e].VonCity==(ULONG)Sim.HomeAirportId)
                  {
                     if (Plan->Flug[e].Startzeit+1<TextPrinted[Plan->Flug[e].Gate])
                     {
                        TextBricks[Plan->Flug[e].Gate].FillWith (0);
                        TextBricks[Plan->Flug[e].Gate].PrintAt (
                           bprintf ("%s-%s %li:00", (LPCTSTR)Cities[Plan->Flug[e].VonCity].Kuerzel, (LPCTSTR)Cities[Plan->Flug[e].NachCity].Kuerzel, Plan->Flug[e].Startzeit+1), FontSmallWhiteX, TEC_FONT_LEFT, 0, 0, 132, 12);

                        TextBrickTexts[Plan->Flug[e].Gate]=bprintf ("%s-%s %li:00", (LPCTSTR)Cities[Plan->Flug[e].VonCity].Name, (LPCTSTR)Cities[Plan->Flug[e].NachCity].Name, Plan->Flug[e].Startzeit+1);
                        TextPrinted[Plan->Flug[e].Gate]=Plan->Flug[e].Startzeit+1;
                     }
                  }
                  //Abflug:
                  if (Plan->Flug[e].Startdate==Sim.Date && (Plan->Flug[e].Startzeit==Sim.GetHour() || Plan->Flug[e].Startzeit-1==Sim.GetHour()) && Plan->Flug[e].VonCity==(ULONG)Sim.HomeAirportId)
                  {
                     if (Plan->Flug[e].Startzeit+1<TextPrinted[TextBricks.AnzEntries()/2+Plan->Flug[e].Gate])
                     {
                        TextBricks[TextBricks.AnzEntries()/2+Plan->Flug[e].Gate].FillWith (0);
                        TextBricks[TextBricks.AnzEntries()/2+Plan->Flug[e].Gate].PrintAt (
                           bprintf ("%s-%s %li:00", (LPCTSTR)Cities[Plan->Flug[e].VonCity].Kuerzel, (LPCTSTR)Cities[Plan->Flug[e].NachCity].Kuerzel, Plan->Flug[e].Startzeit+1), FontSmallWhiteX, TEC_FONT_LEFT, 0, 0, 132, 12);

                        TextBrickTexts[TextBricks.AnzEntries()/2+Plan->Flug[e].Gate]=bprintf ("%s-%s %li:00", (LPCTSTR)Cities[Plan->Flug[e].VonCity].Name, (LPCTSTR)Cities[Plan->Flug[e].NachCity].Name, Plan->Flug[e].Startzeit+1);
                        TextPrinted[TextBricks.AnzEntries()/2+Plan->Flug[e].Gate]=Plan->Flug[e].Startzeit+1;
                     }
                  }
               }
            }

            //Für alle Flüge des Flugzeuges:
            e= Sim.Players.Players[c].Planes[d].Flugplan.NextFlight;

            if (e!=-1)
            {
               //Dadurch wird's unten kompakter:Eintrag vorhanden?
               if ((Plan->Flug[e].ObjectType==1 || Plan->Flug[e].ObjectType==2) && Plan->Flug[e].Gate>=0)
               {
                  if (Plan->Flug[e].Landedate==Sim.Date && Plan->Flug[e].Landezeit==Sim.GetHour() && Plan->Flug[e].NachCity==(ULONG)Sim.HomeAirportId)
                  {
                     if (Plan->Flug[e].Landezeit<TextPrinted[TextBricks.AnzEntries()/2+Plan->Flug[e].Gate])
                     {
                        TextBricks[TextBricks.AnzEntries()/2+Plan->Flug[e].Gate].FillWith (0);
                        TextBricks[TextBricks.AnzEntries()/2+Plan->Flug[e].Gate].PrintAt (
                           bprintf ("%s-%s %li:00", (LPCTSTR)Cities[Plan->Flug[e].VonCity].Kuerzel, (LPCTSTR)Cities[Plan->Flug[e].NachCity].Kuerzel, Plan->Flug[e].Landezeit), FontSmallWhiteX, TEC_FONT_LEFT, 0, 0, 132, 12);

                        TextBrickTexts[TextBricks.AnzEntries()/2+Plan->Flug[e].Gate]=bprintf ("%s-%s %li:00", (LPCTSTR)Cities[Plan->Flug[e].VonCity].Name, (LPCTSTR)Cities[Plan->Flug[e].NachCity].Name, Plan->Flug[e].Landezeit);
                        TextPrinted[TextBricks.AnzEntries()/2+Plan->Flug[e].Gate]=Plan->Flug[e].Landezeit;
                     }
                  }
               }
            }
         }
      }
   }
}

//--------------------------------------------------------------------------------------------
//Speichert ein Airport-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const AIRPORT &Airport)
{
   File << Airport.Builds      << Airport.LeftEnd     << Airport.RightEnd;
   File << Airport.GateMapper  << Airport.HashBuilds;
   File << Airport.PlateOffset << Airport.PlateDimension;
   File << Airport.iPlate      << Airport.SeatsTaken;
   File << Airport.Runes       << Airport.Doors       << Airport.Triggers;
   File << Airport.HallNum     << Airport.ClipMarkers << Airport.AreaMarkers;
   File << Airport.NumBeltSpots;

   if (SaveVersion==1 && SaveVersionSub>=100)
      File << Airport.iPlateDir;

   for (SLONG c=0; c<10; c++)
      File << Airport.HallLevel[c];

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt ein Airport-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, AIRPORT &Airport)
{
   File >> Airport.Builds      >> Airport.LeftEnd     >> Airport.RightEnd;
   File >> Airport.GateMapper  >> Airport.HashBuilds;
   File >> Airport.PlateOffset >> Airport.PlateDimension;
   File >> Airport.iPlate      >> Airport.SeatsTaken;
   File >> Airport.Runes       >> Airport.Doors       >> Airport.Triggers;
   File >> Airport.HallNum     >> Airport.ClipMarkers >> Airport.AreaMarkers;
   File >> Airport.NumBeltSpots;

   if (SaveVersion==1 && SaveVersionSub>=100)
      File >> Airport.iPlateDir;
   else
   {
      Airport.iPlateDir.ReSize (0);
      Airport.iPlateDir.ReSize (Airport.iPlate.AnzEntries());
      memset (Airport.iPlateDir, 0xff, Airport.iPlateDir.AnzEntries());
      Airport.iPlateDir+=20*16;
   }

   for (SLONG c=0; c<10; c++)
      File >> Airport.HallLevel[c];

   return (File);
}

//--------------------------------------------------------------------------------------------
//Speichert ein ClipMarker-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CClipMarker &Marker)
{
   File << Marker.Type << Marker.Position;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt ein ClipMarker-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CClipMarker &Marker)
{
   File >> Marker.Type >> Marker.Position;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Speichert ein CAreaMarker-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CAreaMarker &Marker)
{
   File << Marker.Par << Marker.p1 << Marker.p2;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt ein CAreaMarker-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CAreaMarker &Marker)
{
   File >> Marker.Par >> Marker.p1 >> Marker.p2;

   return (File);
}