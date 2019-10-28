//============================================================================================
// Personal.Cpp - Das Personalbüro des Spielers
//============================================================================================
// Link: "Personal.h"
//============================================================================================
#include "stdafx.h"
#include "glpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//Zum debuggen:
static const char FileId[] = "Pers";

SLONG ReadLine (BUFFER<UBYTE> &Buffer, SLONG BufferStart, char *Line, SLONG LineLength);

//--------------------------------------------------------------------------------------------
//Konstruktor
//--------------------------------------------------------------------------------------------
CPersonal::CPersonal(BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "personal.gli", GFX_PERSONAL)
{
   SetRoomVisited (PlayerNum, ROOM_PERSONAL_A);

   Sim.FocusPerson=-1;

   if (!bHandy) AmbientManager.SetGlobalVolume (40);

   KugelBm.ReSize (pRoomLib, "KUGEL");
   WildBm.ReSize (pRoomLib, "ZEITSCHR");
   LetterBm.ReSize (pRoomLib, "MITTELB");
   StapelBriefBm.ReSize (pRoomLib, "BRIEF");
   NoSaboBm.ReSize (pRoomLib, "NOSABO");

   FlugbahnType=-1;

   PaperOnTable=-1;
   PaperDropping=-1;
   StapelBrief=TRUE;
   Zeitung=TRUE;
   KommVar=-1;
   KommVar2=-1;

   SP_Frau.ReSize (11);
   //--------------------------------------------------------------------------------------------
   SP_Frau.Clips[0].ReSize (0, "PFBrief.smk", "PFBrief.raw", XY (340, 113), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            &StapelBrief, SMACKER_CLIP_XOR|SMACKER_CLIP_FRAME+1*SMACKER_CLIP_MULT, 1, NULL,  //Warten
                            "A9", 1);
   SP_Frau.Clips[1].ReSize (1, "PFLeseB.smk", "", XY (340, 113), SPM_IDLE,       CRepeat(1,1), CPostWait(30,90), SMACKER_CLIP_CANCANCEL,
                            &PaperOnTable, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, 2, NULL,  //Warten
                            "A9", 2);
   SP_Frau.Clips[2].ReSize (2, "PFWerf.smk", "PFWerf.raw", XY (340, 113), SPM_IDLE,        CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            &FlugbahnType, SMACKER_CLIP_SET|SMACKER_CLIP_FRAME+5*SMACKER_CLIP_MULT, 0, &KommVar,  //Warten
                            "A9A2E1", 0, 3, 6);

   SP_Frau.Clips[3].ReSize (3, "PFWild.smk", "PFWild.raw", XY (340, 113), SPM_IDLE,        CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            &Zeitung, SMACKER_CLIP_SET|SMACKER_CLIP_POST, 0,NULL,  //Warten
                            "A9", 4);
   SP_Frau.Clips[4].ReSize (4, "PFLeseW.smk", "", XY (340, 113), SPM_IDLE,       CRepeat(1,1), CPostWait(70,130), SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, &KommVar2,  //Warten
                            "A9", 5);
   SP_Frau.Clips[5].ReSize (5, "PFWildZ.smk", "PFWildZ.raw", XY (340, 113), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            &Zeitung, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, 1, NULL,  //Warten
                            "A9E1", 0, 6);

   SP_Frau.Clips[6].ReSize (6, "PFTurn.smk", "", XY (340, 113), SPM_IDLE,        CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                            "E1E1E1", 8, 7, 9);
   SP_Frau.Clips[7].ReSize (7, "PFRede.smk", "", XY (340, 113), SPM_TALKING,     CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,  //Labern
                            "A9E1E1", 7, 8, 9);
   SP_Frau.Clips[8].ReSize (8, "PFTurnW.smk", "", XY (340, 113), SPM_LISTENING,  CRepeat(1,1), CPostWait(20,20), SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,  //Zuhören
                            "A9A1E1E1", 8, 10, 7, 9);
   SP_Frau.Clips[9].ReSize (9, "PFTurnZ.smk", "", XY (340, 113), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                            "A9", 0);
   SP_Frau.Clips[10].ReSize (10, "PFTurnK.smk", "", XY (340, 113), SPM_LISTENING, CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                            "A9", 8);

   SP_Mann.ReSize (9);
   //--------------------------------------------------------------------------------------------
   SP_Mann.Clips[0].ReSize (0, "PMLese.smk", "", XY (130, 80), SPM_IDLE,        CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, &PaperOnTable,  //Warten
                            "A1A1E1", 0, 1, 3);
   SP_Mann.Clips[1].ReSize (1, "PMBlat.smk", "", XY (130, 80), SPM_IDLE,        CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, &PaperOnTable,  //Warten
                            "A9", 0);

   SP_Mann.Clips[2].ReSize (2, "PMKorb.smk", "", XY (130, 80), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            &PaperOnTable, SMACKER_CLIP_SET|SMACKER_CLIP_FRAME+4*SMACKER_CLIP_MULT, -1, NULL,
                            "A9", 7);
   SP_Mann.Clips[7].ReSize (7, "PMKorbB.smk", "", XY (130, 80), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            &PaperDropping, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, 1, NULL,
                            "A9E1", 1, 3);

   SP_Mann.Clips[3].ReSize (3, "PMTurn.smk", "", XY (130, 80), SPM_IDLE,        CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                            "E1E1A1E1", 4, 5, 5, 6);
   SP_Mann.Clips[4].ReSize (4, "PMReden.smk", "", XY (130, 80), SPM_TALKING,    CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                            "A9E1E1", 4, 5, 6);
   SP_Mann.Clips[5].ReSize (5, "PMWait.smk", "", XY (130, 80), SPM_LISTENING,   CRepeat(1,1), CPostWait(99,99), SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                            "A9A1E1E1", 5, 8, 4, 6);
   SP_Mann.Clips[6].ReSize (6, "PMTurnZ.smk", "", XY (130, 80), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                            "A9", 0);
   SP_Mann.Clips[8].ReSize (8, "PMWaitK.smk", "", XY (130, 80), SPM_LISTENING,  CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                            "A9", 5);

   SP_DroppingPaper.ReSize (2);
   //--------------------------------------------------------------------------------------------
   SP_DroppingPaper.Clips[0].ReSize (0, "", "", XY (133, 260), SPM_LISTENING,   CRepeat(1,1), CPostWait(2,2), SMACKER_CLIP_CANCANCEL,
                                     NULL, SMACKER_CLIP_SET, 0, &PaperDropping,  //Warten
                                     "A1", 0);
   SP_DroppingPaper.Clips[1].ReSize (1, "Brief.smk", "", XY (133, 260), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                                     &PaperDropping, SMACKER_CLIP_SET|SMACKER_CLIP_POST, -1, NULL,  //Warten
                                     "A9", 0);

   Talkers.Talkers[SLONG(TALKER_PERSONAL1a+PlayerNum*2)].IncreaseReference ();
   Talkers.Talkers[SLONG(TALKER_PERSONAL1b+PlayerNum*2)].IncreaseReference ();

   /*if (gLanguage==LANGUAGE_E)
      DefaultDialogPartner=TALKER_PERSONAL1b+PlayerNum*2; //Fehler bei englischer Sprachaufnahme ausgleichen
   else*/

   DefaultDialogPartner=TALKER_PERSONAL1a+PlayerNum*2;

   Sim.Players.Players[(SLONG)PlayerNum].UpdatePersonalberater (3);

   ShowWindow(SW_SHOW);
   UpdateWindow();
}

//--------------------------------------------------------------------------------------------
//Destruktor
//--------------------------------------------------------------------------------------------
CPersonal::~CPersonal()
{
   NoSaboBm.Destroy();

   Talkers.Talkers[SLONG(TALKER_PERSONAL1a+PlayerNum*2)].DecreaseReference ();
   Talkers.Talkers[SLONG(TALKER_PERSONAL1b+PlayerNum*2)].DecreaseReference ();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Personalbüro message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//BEGIN_MESSAGE_MAP(CMakler, CStdRaum)
//--------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CPersonal, CStdRaum)
	//{{AFX_MSG_MAP(CPersonal)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//--------------------------------------------------------------------------------------------
//void CPersonal::OnPaint()
//--------------------------------------------------------------------------------------------
void CPersonal::OnPaint()
{
   { CPaintDC dc(this); }

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   if (PaperOnTable!=-1)
   {
      if (SP_Frau.GetDesiredMood()==SPM_TALKING || SP_Frau.GetDesiredMood()==SPM_LISTENING)
         KommVar=6; //Drehen
      else
         KommVar=3; //Zeitung lesen

      KommVar2=4;
   }
   else
   {
      KommVar=-1;
      KommVar2=-1;
   }

   if (FlugbahnType==0)
   {
      if (rand()%100<95) FlugbahnType=1;
                    else FlugbahnType=2;

      FlugbahnCounter=timeGetTime();
   }

   if (Sim.Players.Players[(SLONG)PlayerNum].SecurityFlags&4) RoomBm.BlitFrom (NoSaboBm, 276, 0);

   SP_Frau.Pump ();
   SP_Mann.Pump ();
   SP_DroppingPaper.Pump ();
   if (Zeitung) RoomBm.BlitFromT (WildBm, 483, 225);
   if (PaperOnTable!=-1) RoomBm.BlitFromT (LetterBm, 342, 224);
   if (StapelBrief) RoomBm.BlitFromT (StapelBriefBm, 361, 292);

   SP_Frau.BlitAtT (RoomBm);
   SP_Mann.BlitAtT (RoomBm);
   SP_DroppingPaper.BlitAtT (RoomBm);

   //Die Papierkugel blitten:
   if (FlugbahnType>0)
   {
      XY    Pos;
      SLONG Size;

      if (FlugbahnType==1)
      {
         Size=5;
         Pos=XY(491+(timeGetTime()-FlugbahnCounter)/2,143-(timeGetTime()-FlugbahnCounter)/10);
         if (Pos.x>640) FlugbahnType=-1;
      }
      else if (FlugbahnType==2)
      {
         if (timeGetTime()-FlugbahnCounter<=400)
         {
            Size=5+(timeGetTime()-FlugbahnCounter)*95/400;
            Pos=XY(491-166*(timeGetTime()-FlugbahnCounter)/400,143+20*(timeGetTime()-FlugbahnCounter)/400);
         }
         else
         {
            Size=100-(timeGetTime()-FlugbahnCounter-400)*95/400;
            Pos=XY(325-166*(timeGetTime()-FlugbahnCounter-400)/400,163+225*(timeGetTime()-FlugbahnCounter-400)/400);
         }
         if (Pos.x<159) FlugbahnType=-1;
      }

      if (FlugbahnType>0)
      {
         CRect SrcRect (0, 0, KugelBm.Size.x, KugelBm.Size.y);
         CRect DestRect;

         DestRect.left   = Pos.x-(KugelBm.Size.x/2 * Size)/100;
         DestRect.top    = Pos.y-(KugelBm.Size.y/2 * Size)/100;
         DestRect.right  = long(DestRect.left + KugelBm.Size.x*Size/100);
         DestRect.bottom = long(DestRect.top  + KugelBm.Size.y*Size/100);

         /*RoomBm.pBitmap->GetSurface()->Blt (
            &DestRect,
            KugelBm.pBitmap->GetSurface(),
            &SrcRect,
            DDBLT_KEYSRC|DDBLTFAST_WAIT,
            NULL);*/
      }
   }

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   if (!IsDialogOpen() && !MenuIsOpen())
   {
      if (gMousePosition.IfIsWithin (640-620, 0, 640-457, 293)) SetMouseLook (CURSOR_EXIT, 0, ROOM_PERSONAL_A, 999);
      else if (gMousePosition.IfIsWithin (640-255, 145, 640-74, 352)) SetMouseLook (CURSOR_HOT, 0, ROOM_PERSONAL_A, 10);
      else if (gMousePosition.IfIsWithin (640-463, 94, 640-311, 248)) SetMouseLook (CURSOR_HOT, 0, ROOM_PERSONAL_A, 11);
      else if (gMousePosition.IfIsWithin (394, 378, 446, 425) && !Sim.Players.Players[(SLONG)PlayerNum].HasItem(ITEM_TABLETTEN)) SetMouseLook (CURSOR_HOT, 0, ROOM_PERSONAL_A, 20);
   }

   CStdRaum::PostPaint ();
   CStdRaum::PumpToolTips ();
}

//--------------------------------------------------------------------------------------------
//void CPersonal::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CPersonal::OnLButtonDown(UINT nFlags, CPoint point)
{
   XY    RoomPos;

   DefaultOnLButtonDown ();

   if (!ConvertMousePosition (point, &RoomPos))
      return;

   if (!PreLButtonDown (point))
   {
      if (MouseClickArea==ROOM_PERSONAL_A && MouseClickId==999) Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
      else if (MouseClickArea==ROOM_PERSONAL_A && MouseClickId==10) StartDialog (TALKER_PERSONAL1a+PlayerNum*2, MEDIUM_AIR);
      else if (MouseClickArea==ROOM_PERSONAL_A && MouseClickId==11) StartDialog (TALKER_PERSONAL1b+PlayerNum*2, MEDIUM_AIR);
      else if (MouseClickArea==ROOM_PERSONAL_A && MouseClickId==20)
         StartDialog (TALKER_PERSONAL1a+PlayerNum*2, MEDIUM_AIR, 20);
      else CStdRaum::OnLButtonDown(nFlags, point);
   }
}

//--------------------------------------------------------------------------------------------
//void CPersonal::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CPersonal::OnRButtonDown(UINT nFlags, CPoint point)
{
   DefaultOnRButtonDown ();

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
         MenuRightClick (point);
      }
      else
      {
         if (!IsDialogOpen() && point.y<440)
            Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();

         CStdRaum::OnRButtonDown(nFlags, point);
      }
   }
}

//============================================================================================
//CWorker::
//============================================================================================
//Konstruktor:
//============================================================================================
CWorkers::CWorkers (const CString &TabFilename, const CString &TabFilename2)
{
   ReInit (TabFilename, TabFilename2);
}

//--------------------------------------------------------------------------------------------
//Lädt die Tabelle von der Platte
//--------------------------------------------------------------------------------------------
void CWorkers::ReInit (const CString &TabFilename, const CString &TabFilename2)
{
   //CStdioFile    Tab;
   BUFFER<char>  Line(300);
   SLONG         Num;
   CString       TmpStr;

   //Load Table header:
   BUFFER<UBYTE> FileData (*LoadCompleteFile (FullFilename (TabFilename, ExcelPath)));
   SLONG         FileP=0;

   //Die erste Zeile einlesen
   FileP=ReadLine (FileData, FileP, Line, 300);

   Workers.ReSize (0);
   Workers.ReSize (MAX_WORKERS);
   Num=0;

   while (1)
   {
      if (FileP>=FileData.AnzEntries()) break;
      FileP=ReadLine (FileData, FileP, Line, 300);

      TeakStrRemoveEndingCodes (Line, "\xd\xa\x1a\r");

      if (Num>=Workers.AnzEntries())
      {
         TeakLibW_Exception (FNL, ExcNever);
         return;
      }

      Workers[Num].Name        = strtok (Line, TabSeparator);
      Workers[Num].Geschlecht  = atoi (strtok (NULL, TabSeparator));
      Workers[Num].Typ         = atoi (strtok (NULL, TabSeparator));
      Workers[Num].Gehalt      = atoi (strtok (NULL, TabSeparator));
      Workers[Num].Talent      = atoi (strtok (NULL, TabSeparator));
      Workers[Num].Alter       = atoi (strtok (NULL, TabSeparator));
      Workers[Num].Kommentar   = KorrigiereUmlaute (CString(strtok (NULL, "")));
      Workers[Num].Employer    = WORKER_RESERVE;
      Workers[Num].Happyness   = 100;

      Workers[Num].WarnedToday = 0;
      Workers[Num].OriginalGehalt = Workers[Num].Gehalt;

      if (Workers[Num].Kommentar.GetLength()>0)
      {
         if (Workers[Num].Kommentar[0]=='"') Workers[Num].Kommentar = Workers[Num].Kommentar.Right(Workers[Num].Kommentar.GetLength()-1);
         if (Workers[Num].Kommentar[Workers[Num].Kommentar.GetLength()-1]=='"') Workers[Num].Kommentar = Workers[Num].Kommentar.Left(Workers[Num].Kommentar.GetLength()-1);
      }

      Num++;
   }

   Workers.ReSize (Num);

   //Load Table header:
   BUFFER<UBYTE> FileData2 (*LoadCompleteFile (FullFilename (TabFilename2, ExcelPath)));

   FNames.ReSize (0);
   MNames.ReSize (0);
   LNames.ReSize (0);
   FNames.ReSize (40);
   MNames.ReSize (40);
   LNames.ReSize (80);

   //Die erste Zeile einlesen
   FileP=0;
   FileP=ReadLine (FileData2, FileP, Line, 300);

   SLONG i1=0, i2=0, i3=0;

   while (1)
   {
      if (FileP>=FileData2.AnzEntries()) break;
      FileP=ReadLine (FileData2, FileP, Line, 300);

      TeakStrRemoveEndingCodes (Line, "\xd\xa\x1a\r");

      Num = atoi (strtok (Line, TabSeparator));

      if (Num==0) FNames[i1++] = strtok (NULL, TabSeparator);
      if (Num==1) MNames[i2++] = strtok (NULL, TabSeparator);
      if (Num==2) LNames[i3++] = strtok (NULL, TabSeparator);

      if (i1>=FNames.AnzEntries() || i2>=MNames.AnzEntries() || i3>=LNames.AnzEntries()) DebugBreak();
   }

   FileData2.FillWith(0);

   FNames.ReSize (i1);
   MNames.ReSize (i2);
   LNames.ReSize (i3);
}

//--------------------------------------------------------------------------------------------
//Neuer Tag, die Leute sind etwas unglücklicher und der Arbeitsmarkt ändert sich:
//--------------------------------------------------------------------------------------------
void CWorkers::NewDay (void)
{
   SLONG c, m, n;

   CheckShortage ();

   TEAKRAND LocalRand (Sim.Date+Sim.StartTime);

   //Eingestellte Leute bei Mensch-Spielern unglücklich machen:
   for (c=0; c<Workers.AnzEntries(); c++)
   {
      Workers[c].WarnedToday=FALSE;
      if (Workers[c].Employer>=0 && Workers[c].Employer<=3)
      {
         SLONG Anz=0;

         if (Sim.Players.Players[Workers[c].Employer].Owner==0 && Sim.Players.Players[Workers[c].Employer].Image<500) Anz=1;
         if (Anz && Sim.Players.Players[Workers[c].Employer].Image<0) Anz++;
         if (Anz && Sim.Players.Players[Workers[c].Employer].Image<-500) Anz++;

         //Worker u.U. mehrfach um 1%-Punkt unglücklicher machen
         for (; Anz>0; Anz--)
         {
            Workers[c].Happyness--;

            if (Workers[c].Gehalt<Workers[c].OriginalGehalt)
               Workers[c].Happyness-=20;

            if (Workers[c].Happyness<-100)
            {
               //Ihm reicht's! Er kündigt:
               if (Sim.Players.Players[Workers[c].Employer].Owner==0)
                  Sim.Players.Players[Workers[c].Employer].Messages.AddMessage (BERATERTYP_GIRL, bprintf (StandardTexte.GetS (TOKEN_ADVICE, 2000+Workers[c].Typ+Workers[c].Geschlecht*100), Workers[c].Name));

               SLONG ExEmployer=Workers[c].Employer;
               Workers[c].Employer=WORKER_RESERVE;

               Sim.Players.Players[ExEmployer].MapWorkers (FALSE);
               break;
            }
         }
      }
   }

   //Arbeitsmarkt reorganisieren:
   for (c=0; c<Workers.AnzEntries(); c++)
      if (Workers[c].Employer==WORKER_JOBLESS)
         Workers[c].Employer=WORKER_RESERVE;

   //Rund 15 Bewerber (+/- 5) aussuchen:
   SLONG Anzahl = 10+LocalRand.Rand(10);

   for (c=n=0; c<100 && n<Anzahl; c++)
   {
      m=LocalRand.Rand(Workers.AnzEntries());

      for (SLONG d=0; d<100; d++)
        if (Workers[(m+d)%Workers.AnzEntries()].Employer==WORKER_RESERVE)
        {
           m=(m+d)%Workers.AnzEntries();
           break;
        }

      if (Workers[m].Employer==WORKER_RESERVE)
      {
         Workers[m].Employer= WORKER_JOBLESS;
         Workers[m].Gehalt  = Workers[m].OriginalGehalt;
         n++;
      }
   }

   //Und dazu noch 5 Stewardessen (+/- 2) aussuchen:
   Anzahl = 5+LocalRand.Rand(2);

   for (c=n=0; c<100 && n<Anzahl; c++)
   {
      m=LocalRand.Rand(Workers.AnzEntries());

      for (SLONG d=0; d<100; d++)
        if (Workers[(m+d)%Workers.AnzEntries()].Employer==WORKER_RESERVE && Workers[(m+d)%Workers.AnzEntries()].Typ==WORKER_STEWARDESS)
        {
           m=(m+d)%Workers.AnzEntries();
           break;
        }

      if (Workers[m].Employer==WORKER_RESERVE && Workers[m].Typ==WORKER_STEWARDESS)
      {
         Workers[m].Employer= WORKER_JOBLESS;
         Workers[m].Gehalt  = Workers[m].OriginalGehalt;
         n++;
      }
   }
}

//--------------------------------------------------------------------------------------------
//Erhöht oder erniedrigt einer Personen das Gehalt
//--------------------------------------------------------------------------------------------
void CWorker::Gehaltsaenderung (BOOL Art)
{
   if (Art)
   {
      //Gehaltserhöhung:
      SLONG OldGehalt=Gehalt;

      Gehalt += (Gehalt/100*10);
      if (Gehalt>1000000) Gehalt=1000000;

      if (Gehalt!=OldGehalt) Happyness += 20;
   }
   else
   {
      //Gehaltskürzung:
      Gehalt -= (Gehalt/100*10);
      Happyness -= 25;

      if (Happyness<-100)
      {
         if (Sim.Players.Players[Employer].Owner==0)
            Sim.Players.Players[Employer].Messages.AddMessage (BERATERTYP_GIRL, bprintf (StandardTexte.GetS (TOKEN_ADVICE, 2000+Typ+Geschlecht*100), Name));

         SLONG ExEmployer=Employer;

         Employer  = WORKER_RESERVE;
         Gehalt    = OriginalGehalt;
         Happyness = 100;

         Sim.Players.Players[ExEmployer].UpdateWalkSpeed();
         Sim.Players.Players[ExEmployer].MapWorkers (FALSE);
      }
   }
}     

//--------------------------------------------------------------------------------------------
//Erzeugt einen zufälligen Namen
//--------------------------------------------------------------------------------------------
CString CWorkers::GetRandomName(BOOL Geschlecht)
{
   if (Geschlecht) return (MNames[rand()%MNames.AnzEntries()]+" "+LNames[rand()%LNames.AnzEntries()]);
              else return (FNames[rand()%FNames.AnzEntries()]+" "+LNames[rand()%LNames.AnzEntries()]);
}

//--------------------------------------------------------------------------------------------
//Verhindert, daß es zu wenig Piloten oder Stewardessen gibt:
//--------------------------------------------------------------------------------------------
void CWorkers::CheckShortage (void)
{
   SLONG    anz, c;
   TEAKRAND LocalRand (Sim.Date+Sim.StartTime);

   for (anz=c=0; c<Workers.AnzEntries(); c++)
      if ((Workers[c].Employer==WORKER_RESERVE || Workers[c].Employer==WORKER_JOBLESS) && Workers[c].Typ==WORKER_PILOT && Workers[c].Talent>60)
         anz++;

   if (anz<40)
   {
      Workers.ReSize (Workers.AnzEntries()+10);

      for (c=Workers.AnzEntries()-10; c<Workers.AnzEntries(); c++)
      {
         Workers[c].Geschlecht  = ((LocalRand.Rand(100))>20);
         Workers[c].Name        = GetRandomName(Workers[c].Geschlecht);
         Workers[c].Typ         = WORKER_PILOT;
         Workers[c].Gehalt      = (30+LocalRand.Rand(70))*100;
         Workers[c].Talent      = Workers[c].Gehalt/200+LocalRand.Rand(30)+20;
         Workers[c].Alter       = (19+LocalRand.Rand(50));
         Workers[c].Kommentar   = "";
         Workers[c].Employer    = WORKER_RESERVE;
         Workers[c].Happyness   = 100;
         Workers[c].WarnedToday = 0;

         Workers[c].OriginalGehalt = Workers[c].Gehalt;
      }
   }

   for (anz=c=0; c<Workers.AnzEntries(); c++)
      if ((Workers[c].Employer==WORKER_RESERVE || Workers[c].Employer==WORKER_JOBLESS) && Workers[c].Typ==WORKER_STEWARDESS && Workers[c].Talent>60)
         anz++;

   if (anz<40)
   {
      Workers.ReSize (Workers.AnzEntries()+10);

      for (c=Workers.AnzEntries()-10; c<Workers.AnzEntries(); c++)
      {
         Workers[c].Geschlecht  = ((rand()%100)>80);
         Workers[c].Name        = GetRandomName(Workers[c].Geschlecht);
         Workers[c].Typ         = WORKER_STEWARDESS;
         Workers[c].Gehalt      = (30+LocalRand.Rand(50))*100;
         Workers[c].Talent      = Workers[c].Gehalt*100/80/200+LocalRand.Rand(30)+20;
         Workers[c].Alter       = (19+LocalRand.Rand(40));
         Workers[c].Kommentar   = "";
         Workers[c].Employer    = WORKER_RESERVE;
         Workers[c].Happyness   = 100;
         Workers[c].WarnedToday = 0;

         Workers[c].OriginalGehalt = Workers[c].Gehalt;
      }
   }
}

//--------------------------------------------------------------------------------------------
//Erhöht oder erniedrigt allen Personen das Gehalt
//--------------------------------------------------------------------------------------------
void CWorkers::Gehaltsaenderung (BOOL Art, SLONG PlayerNum)
{
   for (SLONG c=0; c<Workers.AnzEntries(); c++)
      if (Workers[c].Employer==PlayerNum)
         Workers[c].Gehaltsaenderung (Art);
}

//--------------------------------------------------------------------------------------------
//Vergleicht Qualifikation mit Gehaltsforderung. Ist es angemessen?
//--------------------------------------------------------------------------------------------
SLONG CWorkers::GetQualityRatio (SLONG prs)
{
   SLONG c, p1;

   for (c=p1=0; c<Workers.AnzEntries(); c++)
      if (Workers[c].Typ==Workers[prs].Typ)
      if (abs(Workers[c].Talent-Workers[prs].Talent)<15)
      {
         if (Workers[c].Gehalt>Workers[prs].Gehalt) p1++;
         else p1--;
      }

   return (p1);
}

//--------------------------------------------------------------------------------------------
//Stellt sicher, daß der gewünschte Berater heute im Angebot ist:
//--------------------------------------------------------------------------------------------
void CWorkers::EnsureBerater (SLONG Typ)
{
   SLONG c, m;

   //Arbeitsmarkt reorganisieren:
   for (c=0; c<Workers.AnzEntries(); c++)
      if (Workers[c].Employer==WORKER_JOBLESS && Workers[c].Typ==Typ)
         return;

   TEAKRAND LocalRand (Sim.Date+Sim.StartTime);

   for (c=0; c<1000; c++)
   {
      m=LocalRand.Rand(Workers.AnzEntries());

      if (Workers[m].Employer==WORKER_RESERVE && Workers[m].Typ==Typ)
      {
         Workers[m].Employer= WORKER_JOBLESS;
         Workers[m].Gehalt  = Workers[m].OriginalGehalt;
         return;
      }
   }
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
SLONG CWorkers::GetAverageHappyness (SLONG PlayerNum)
{
   SLONG c, Anz, Value;

   for (c=Anz=Value=0; c<Workers.AnzEntries(); c++)
      if (Workers[c].Employer==PlayerNum)
      {
         Anz++;
         Value += Workers[c].Happyness;
      }

   if (Anz)
      return (Value/Anz);
   else
      return (100);
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
SLONG CWorkers::GetMaxHappyness (SLONG PlayerNum)
{
   SLONG c, Max=-1;

   for (c=0; c<Workers.AnzEntries(); c++)
      if (Workers[c].Employer==PlayerNum && Workers[c].Happyness>Max)
         Max = Workers[c].Happyness;

   if (Max!=-1)
      return (Max);
   else
      return (100);
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
SLONG CWorkers::GetMinHappyness (SLONG PlayerNum)
{
   SLONG c, Min=999;

   for (c=0; c<Workers.AnzEntries(); c++)
      if (Workers[c].Employer==PlayerNum && Workers[c].Happyness<Min)
         Min = Workers[c].Happyness;

   if (Min!=999)
      return (Min);
   else
      return (100);
}

//--------------------------------------------------------------------------------------------
//Verändert die Happiness aller Worker um den angegebenen Betrag:
//--------------------------------------------------------------------------------------------
void CWorkers::AddHappiness (SLONG PlayerNum, SLONG Value)
{
   SLONG c;

   for (c=0; c<Workers.AnzEntries(); c++)
      if (Workers[c].Employer==PlayerNum)
      {
         Workers[c].Happyness += Value;
         if (Workers[c].Happyness<-100) Workers[c].Happyness=-100;
         if (Workers[c].Happyness>100)  Workers[c].Happyness=100;
      }
}

//--------------------------------------------------------------------------------------------
//Berechnet die Zahl der Berater-Bewerbungen:
//--------------------------------------------------------------------------------------------
SLONG CWorkers::GetNumJoblessBerater (void)
{
   SLONG c, d;

   for (c=d=0; c<Workers.AnzEntries(); c++)
      if (Workers[c].Employer==WORKER_JOBLESS)
         if (Workers[c].Typ<WORKER_PILOT)
            d++;

   return (d);
}

//--------------------------------------------------------------------------------------------
//Berechnet die Zahl der Piloten-Bewerbungen:
//--------------------------------------------------------------------------------------------
SLONG CWorkers::GetNumJoblessPiloten (void)
{
   SLONG c, d;

   for (c=d=0; c<Workers.AnzEntries(); c++)
      if (Workers[c].Employer==WORKER_JOBLESS)
         if (Workers[c].Typ==WORKER_PILOT)
            d++;

   return (d);
}

//--------------------------------------------------------------------------------------------
//Berechnet die Zahl der Flugbegleiter-Bewerbungen
//--------------------------------------------------------------------------------------------
SLONG CWorkers::GetNumJoblessFlugbegleiter (void)
{
   SLONG c, d;

   for (c=d=0; c<Workers.AnzEntries(); c++)
      if (Workers[c].Employer==WORKER_JOBLESS)
         if (Workers[c].Typ==WORKER_STEWARDESS)
            d++;

   return (d);
}

//--------------------------------------------------------------------------------------------
//Speichert ein Worker-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CWorker &Worker)
{
   File << Worker.Name;
   File << Worker.Geschlecht;
   File << Worker.Typ;
   File << Worker.Gehalt;
   File << Worker.OriginalGehalt;
   File << Worker.Talent;
   File << Worker.Alter;
   File << Worker.Kommentar;
   File << Worker.Employer;
   File << Worker.PlaneId;
   File << Worker.Happyness;
   File << Worker.WarnedToday;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt ein Worker-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CWorker &Worker)
{
   File >> Worker.Name;
   File >> Worker.Geschlecht;
   File >> Worker.Typ;
   File >> Worker.Gehalt;
   File >> Worker.OriginalGehalt;
   File >> Worker.Talent;
   File >> Worker.Alter;
   File >> Worker.Kommentar;
   File >> Worker.Employer;
   File >> Worker.PlaneId;
   File >> Worker.Happyness;
   File >> Worker.WarnedToday;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Speichert ein Workers-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator << (TEAKFILE &File, const CWorkers &Workers)
{
   File << Workers.Workers;

   return (File);
}

//--------------------------------------------------------------------------------------------
//Lädt ein Workers-Objekt:
//--------------------------------------------------------------------------------------------
TEAKFILE &operator >> (TEAKFILE &File, CWorkers &Workers)
{
   File >> Workers.Workers;

   /*SLONG c,NewAnz;
   ULONG Tmp;

   File >> NewAnz;

   Workers.Workers.ReSize (NewAnz);

   File >> Tmp;
   for (c=0; c<NewAnz; c++)
      File >> Workers.Workers[c]; */

   return (File);
}