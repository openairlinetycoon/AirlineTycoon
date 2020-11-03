//============================================================================================
// Init.cpp : Routinen zur initialisierung:
//============================================================================================
#include "stdafx.h"
#include "Checkup.h"
#include "cd_prot.h"
#include "Editor.h"
#include <fstream>
#include <algorithm>

extern SLONG IconsPos[];  //Referenziert globe.cpp

extern BOOL gCDFound;
extern BOOL gSpawnOnly;

extern CJumpingVar<CString> gCDPath;

extern CPlaneBuild        gPlaneBuilds[37];
extern CPlanePartRelation gPlanePartRelations[307];

SB_CFont FontVerySmall;

extern char chRegKeyOld[];

DWORD GetKey();

//--------------------------------------------------------------------------------------------
//Programm läuft nicht direkt von CD:
//--------------------------------------------------------------------------------------------
#ifdef CD_PROTECTION
void DontRunFromCD (void)
{
	char laufwerk[4];

	laufwerk[0] = AppPath[0];
	laufwerk[1] = ':';
	laufwerk[2] = '\\';
	laufwerk[3] = 0;

	if ((GetDriveType (laufwerk)) == DRIVE_CDROM)
	{
      ::MessageBox (NULL, "Please start SETUP.EXE to install Airline Tycoon.\n\n"
                          "Bitte verwenden Sie SETUP.EXE um Airline Tycoon zu installieren.\n\n"
                          "Veuillez s´il vous plait utiliser SETUP.EXE pour installer Airline Tycoon.", "Airline Tycoon : Error!",  MB_OK);

      exit(0);
	}
}
#endif

//--------------------------------------------------------------------------------------------
//Copy the savegame from the basic version?
//--------------------------------------------------------------------------------------------
void UpdateSavegames (void)
{
   /*if (Sim.Options.OptionViewedIntro==FALSE)
   {
      char *Text;

      if (gLanguage==LANGUAGE_D) Text = "Möchten Sie Ihre alten Spielstände aus dem 'Airline Tycoon' Verzeichnis in das Verzeichnis von 'Airline Tycoon First Class' kopieren?";
                            else Text = "Would you like to copy your old savegames from the 'Airline Tycoon' directory into the directory of 'Airline Tycoon First Class' ?";

      char OldAppPath [1000];
      CRegistryAccess reg (chRegKeyOld);

      reg.ReadRegistryKeyEx (OldAppPath, "&AppPath");

      for (SLONG c=0; c<12; c++)
         if (DoesFileExist (CString(OldAppPath)+bprintf ("Savegame\\game%li.dat", c)))
            break;

      if (c<12)
      {
         if (IDYES==MessageBox (NULL, Text, "Airline Tycoon First Class", MB_YESNO))
         {
            DoAppPath ();

            for (SLONG c=0; c<12; c++)
               CopyFile (CString(OldAppPath)+CString(bprintf("Savegame\\game%li.dat", c)), AppPath+CString(bprintf("Savegame\\game%li.dat", c)), true);
         }
      }
   }*/
}

//--------------------------------------------------------------------------------------------
//Initialisierung der Pfade:
//--------------------------------------------------------------------------------------------
void InitPathVars (void)
{
   CString str;

#ifndef DEMO
   //Keine Demo, sondern Vollversion:
   #ifdef CD_PROTECTION_ANY_TYPE
      //Kopierschutz der AddOn-Version ist nett: Wir probieren erst einmal, die CD zu finden.
      //Ist sie nicht da, dann schränken wir das Hauptmenü nur ein.
do_findcd_main:
      if (CDProtection (&str))
      {
         gCDPath    = str;
         gCDFound   = TRUE;
         gSpawnOnly = FALSE;
      }

      //Prüfen, ob es die AddOn-CD ist und nicht die Basis-CD
      /*if (DoesFileExist (gCDPath+"intro\\credits.smk"))
      {
         if (MessageBox (NULL, "Please insert the Deluxe CD.\n\n"
                               "Bitte legen Sie die Deluxe CD ein.\n\n"
                               "Veuillez insérer le CD de Deluxe.\n\n" , "Airline Tycoon : Error!",  MB_RETRYCANCEL)==IDCANCEL)
            exit(-1);

         goto do_findcd_main;
      }*/ 

      if (1==0)
   #else
      if (!DoesFileExist ("C:\\Infos\\identity.txt") /*&& GetKey()!=0x1c4298a0*/)
      if (!DoesFileExist (CString(AppPath)+"data\\cm.txt"))
   #endif
      {
do_findcd:
   #ifdef CD_PROTECTION_ANY_TYPE
         while (!CDProtection (&str))
         {
            if (::MessageBox (NULL, "Please insert the Deluxe CD.\n\n"
                                    "Bitte legen Sie die Deluxe CD ein.\n\n"
                                    "Veuillez insérer le CD de Deluxe.\n\n" , "Airline Tycoon : Error!",  MB_RETRYCANCEL)==IDCANCEL)
               exit(-1);
         } 
   #endif
         gCDPath    = str;
         gCDFound   = TRUE;
         gSpawnOnly = FALSE;
      }
   #ifndef CD_PROTECTION_ANY_TYPE
      else
      {
         gCDPath    = "-:\\data\\";     //CD not neccessary, because this is Spellbound-Internal
         gSpawnOnly = FALSE;
      }
   #endif
#else
   gSpawnOnly = FALSE;

   //Demo Version:
   #if defined(NO_D_VOICES) && defined(NO_E_VOICES) && defined(NO_N_VOICES)
      do_findcd:
        gCDPath="z:\\";
   #else
      //Demo-CD mit Voices. Erst einmal ohne CD probieren. Wenn dann aber Verzeichnisse
      //Fehlen (z.B. Voices), dann müssen wir doch nach der CD suchen:
      goto skip_search_demo_cd;

      do_findcd:

      while (!CDProtection (&str))
      {
         if (::MessageBox (NULL, "CD not found! Please insert the Demo-CD containing 'Airline Tycoon Deluxe' in your CD-Drive.\n\n"
                                 "CD wurde nicht gefunden! Bitte legen Sie eine Demo-CD mit Airline Tycoon Deluxe in das CD-Rom Laufwerk ein.\n\n"
                                 "CD non trouvé ! Inserez s'il vous plait le CD dans votre lecteur de CD-Rom.", "Airline Tycoon : Error!",  MB_RETRYCANCEL)==IDCANCEL)
            exit(-1);
      } 
   
      gCDPath=str;
      gCDFound=TRUE;
skip_search_demo_cd:
   #endif
#endif

#ifdef CD_PROTECTION
   DontRunFromCD ();
#endif

   BitmapPath   = "Bitmaps\\%s";
   BrickPath    = "Brick\\%s";
   CityPath     = "City\\%s";
   ClanPath     = "Clan\\%s";
   ExcelPath    = "Data\\%s";
   GliPath      = "GLI\\%s";
   MiscPath     = "Misc\\%s";
   ScanPath     = "Scans\\%s";
   SoundPath    = "Sound\\%s";
   RoomPath     = "Room\\%s";
   PlanePath    = "Planes\\%s";
   SmackerPath  = "Video\\%s";
   IntroPath    = "Intro\\%s";
   VoicePath    = "Voice\\%s";
   MyPlanePath  = "myplanes\\%s";

   if (SavegamePath.GetLength()==0) SavegamePath = "Savegame\\%s";

   //Sind noch Packages auf der CD?
   if (!DoesFileExist (FullFilename ("dir.txt", GliPath)))
   {
#ifndef CD_PROTECTION_ANY_TYPE
      if (DoesFileExist ("Y:\\Projekte\\Airlin~1\\betaver\\data\\at.exe"))
         GliPath="Y:\\Projekte\\Airlin~1\\betaver\\data\\gli\\%s";
      else
#endif
      {
         if (!gCDFound) goto do_findcd;
         GliPath=gCDPath+GliPath;
      }
   }

   if (!DoesFileExist (FullFilename ("dir.txt", RoomPath)))
   {
#ifndef CD_PROTECTION_ANY_TYPE
      if (DoesFileExist ("Y:\\Projekte\\Airlin~1\\betaver\\data\\at.exe"))
         RoomPath="Y:\\Projekte\\Airlin~1\\betaver\\data\\room\\%s";
      else
#endif
      {
         if (!gCDFound) goto do_findcd;
         RoomPath=gCDPath+RoomPath;
      }
   }

#ifndef NO_INTRO
   if (!DoesFileExist (FullFilename ("dir.txt", IntroPath)))
   {
#ifndef CD_PROTECTION_ANY_TYPE
      if (DoesFileExist ("Y:\\Projekte\\Airlin~1\\betaver\\data\\at.exe"))
         IntroPath="Y:\\Projekte\\Airlin~1\\betaver\\data\\intro\\%s";
      else
#endif
      {
         if (gCDFound)
            IntroPath=gCDPath+IntroPath;
         else
            IntroPath="";                //IntroPath leer ==> Spawn Version
      }
   }
#endif

#if !defined(NO_D_VOICES) || !defined(NO_E_VOICES) || !defined(NO_N_VOICES)
   if (!DoesFileExist (FullFilename ("dir.txt", VoicePath)))
   {
#ifndef CD_PROTECTION_ANY_TYPE
      if (DoesFileExist ("Y:\\Projekte\\Airlin~1\\betaver\\data\\at.exe"))
         VoicePath="Y:\\Projekte\\Airlin~1\\betaver\\data\\voice\\%s";
      else
#endif
      {
#ifdef VOICES_OPTIONAL
         bVoicesNotFound = true;
         Sim.Options.OptionSpeechBubble = TRUE;
         Sim.Options.OptionTalking      = FALSE;
#else
         if (!gCDFound) goto do_findcd;
         VoicePath=gCDPath+VoicePath;
#endif
      }
   }
#endif
}

//--------------------------------------------------------------------------------------------
//Initialisiert die Digital-Sache:
//--------------------------------------------------------------------------------------------
void InitSoundSystem (SDL_Window *AppWnd)
{
   gpSSE = new SSE(AppWnd, 22050, 1, 16);
	
   if (Sim.Options.OptionDigiSound)
   {
      if (Sim.Options.OptionEnableDigi) gpSSE->EnableDS();

      gpSSE->CreateFX(&gpClickFx);
      gpClickFx->Load((char*)(LPCTSTR)FullFilename ("click.raw", SoundPath));
      gDoorOpen.ReInit ("Tuerauf.raw");
      gDoorClose.ReInit ("Tuerzu.raw");
      gMovePaper.ReInit ("paper.raw");
      gArabDoorFx.ReInit ("sabotage.raw");

      gMovePaper.Play();
      gMovePaper.Stop();

      //Die Ambiente-Sounds:
      AmbientManager.AmbientFx.ReSize (3);
      AmbientManager.SetFx (AMBIENT_PEOPLE,      "Raunen.raw");
      AmbientManager.SetFx (AMBIENT_JET_OUTSIDE, "jetfield.raw");
      AmbientManager.SetFx (AMBIENT_JET_FIELD,   "jetout.raw");

      SetWaveVolume(Sim.Options.OptionMasterVolume);
   }

   if (gpSSE==NULL || !gpSSE->IsSoundEnabled())
      Sim.Options.OptionMasterVolume=0;
}

//--------------------------------------------------------------------------------------------
//Sorgt für Screen Refresh und für Ablauf der Simulation:
//--------------------------------------------------------------------------------------------
void InitFonts (void)
{
   FontCash.Load (lpDD, (char*)(LPCTSTR)FullFilename ("status.mcf", MiscPath));
   FontSmallBlack.Load (lpDD, (char*)(LPCTSTR)FullFilename ("norm_bl.mcf", MiscPath));
   FontSmallGrey.Load (lpDD, (char*)(LPCTSTR)FullFilename ("norm_gr.mcf", MiscPath));
   FontSmallRed.Load (lpDD, (char*)(LPCTSTR)FullFilename ("norm_rt.mcf", MiscPath));
   FontSmallWhite.Load (lpDD, (char*)(LPCTSTR)FullFilename ("norm_wh.mcf", MiscPath));
   FontSmallWhiteX.Load (lpDD, (char*)(LPCTSTR)FullFilename ("norm_x.mcf", MiscPath));
   FontSmallPlastic.Load (lpDD, (char*)(LPCTSTR)FullFilename ("norm_pl.mcf", MiscPath));

   FontVerySmall.Load (lpDD, (char*)(LPCTSTR)FullFilename ("small_bl.mcf", MiscPath));

   FontCondensedBlack.Load (lpDD, (char*)(LPCTSTR)FullFilename ("cond_bl.mcf", MiscPath));

   FontDialog.Load (lpDD, (char*)(LPCTSTR)FullFilename ("dlgfont1.mcf", MiscPath));
   FontDialogLight.Load (lpDD, (char*)(LPCTSTR)FullFilename ("dlgfont2.mcf", MiscPath));
   FontDialogPartner.Load (lpDD, (char*)(LPCTSTR)FullFilename ("dlgfont1.mcf", MiscPath));
   FontDialogInk.Load (lpDD, (char*)(LPCTSTR)FullFilename ("inkfont.mcf", MiscPath));
   FontBigGrey.Load (lpDD, (char*)(LPCTSTR)FullFilename ("dlgfont3.mcf", MiscPath));
   FontBigWhite.Load (lpDD, (char*)(LPCTSTR)FullFilename ("dlgfont3.mcf", MiscPath)); //ex:4

   FontNormalGreen.Load (lpDD, (char*)(LPCTSTR)FullFilename ("norm_ve.mcf", MiscPath));
   FontNormalGrey.Load (lpDD, (char*)(LPCTSTR)FullFilename ("norm_drk.mcf", MiscPath));

   FontCash.SetLineSpace (1);
   FontSmallBlack.SetLineSpace (1);
   FontSmallGrey.SetLineSpace (1);
   FontSmallRed.SetLineSpace (1);
   FontSmallWhite.SetLineSpace (1);
   FontSmallWhiteX.SetLineSpace (1);
   FontSmallPlastic.SetLineSpace (1);
   FontDialog.SetLineSpace (1);
   FontDialogLight.SetLineSpace (1);
   FontDialogPartner.SetLineSpace (1);
   FontBigGrey.SetLineSpace (1);
   FontNormalGrey.SetLineSpace (1);

   //Determine Font colors on this video card:
   SBBM  TestBm (20,20);
   SLONG x, y;

   TestBm.Clear();
   TestBm.PrintAt ("X",FontSmallBlack, TEC_FONT_LEFT, 0, 0, 20, 20);
   for (x=0; x<20; x++)
      for (y=0; y<20; y++) if (TestBm.GetPixel(x,y)) ColorOfFontBlack=(UWORD)TestBm.GetPixel(x,y);

   TestBm.Clear();
   TestBm.PrintAt ("X",FontSmallGrey, TEC_FONT_LEFT, 0, 0, 20, 20);
   for (x=0; x<20; x++)
      for (y=0; y<20; y++) if (TestBm.GetPixel(x,y)) ColorOfFontGrey=(UWORD)TestBm.GetPixel(x,y);

   TestBm.Clear();
   TestBm.PrintAt ("X",FontSmallRed, TEC_FONT_LEFT, 0, 0, 20, 20);
   for (x=0; x<20; x++)
      for (y=0; y<20; y++) if (TestBm.GetPixel(x,y)) ColorOfFontRed=(UWORD)TestBm.GetPixel(x,y);
}

//--------------------------------------------------------------------------------------------
//Erzeugt die Statuszeilen:
//--------------------------------------------------------------------------------------------
void InitStatusLines (void)
{
   StatusLineBms.ReSize (pGLibStd, GFX_STATLIN0, GFX_STATLIN1, GFX_STATLIN2,
                                   GFX_STATLIN3, GFX_STATLIN4, GFX_STATLIN5,
                                   GFX_STATLIN6, NULL);

   gClockBms.ReSize (pGLibStd, GFX_CLOCK0,  GFX_CLOCK1,  GFX_CLOCK2,  GFX_CLOCK3,
                               GFX_OCLOCK0, GFX_OCLOCK1, GFX_OCLOCK2, GFX_OCLOCK3,
                               GFX_OCLOCK4, GFX_OCLOCK5, GFX_OCLOCK6, GFX_OCLOCK7,NULL);
}

//--------------------------------------------------------------------------------------------
//Lädt die Inventar-Bilder:
//--------------------------------------------------------------------------------------------
void InitItems (void)
{
   gItemBms.ReSize (pGLibStd, "ITEM00", 26);
   LogoBms.ReSize (pGLibStd, "LOGO0", 7);
   SmallLogoBms.ReSize (pGLibStd, "SLOGO0", 4);
   TinyLogoBms.ReSize (pGLibStd, "TLOGO0", 4);
   MoodBms.ReSize (pGLibStd, GFX_MOOD00, GFX_MOOD01, GFX_MOOD02, GFX_MOOD03, GFX_MOOD04, GFX_MOOD10, GFX_MOOD11, GFX_MOOD12, GFX_MOOD13, GFX_MOOD14, GFX_MOOD15, GFX_MOOD16, GFX_MOOD17, GFX_MOOD18, GFX_MOOD19, GFX_MOOD20, GFX_MOOD21, GFX_MOOD22, GFX_MOOD23, GFX_MOOD24, GFX_MOOD25, GFX_MOOD26, GFX_MOOD27, GFX_MOOD28, GFX_MOOD29, GFX_MOOD30, GFX_MOOD31, GFX_MOOD32, GFX_MOOD33,
                   GFX_MOOD34, GFX_MOOD35, GFX_MOOD36, GFX_MOOD37, GFX_MOOD38, GFX_MOOD39, GFX_MOOD40, GFX_MOOD41, GFX_MOOD42, NULL);
   SmileyBms.ReSize (pGLibStd, "SMILEY0", 4);
   gTutoriumBms.ReSize (pGLibStd, "TUT01", 3);
   gRepeatMessageBms.ReSize (pGLibStd, "REPEAT01", 3);
   gStatisticBms.ReSize (pGLibStd, "STAT01", 3);
   gStatLineHigh.ReSize (pGLibStd, "TIMEHIGH GELDHIGH");

   //Tabellen importieren:
   if (!bFirstClass)
   {
      CString str;

      //Relations:
      {
         std::ifstream file(FullFilename ("relation.csv", ExcelPath), std::ios_base::in);

         file >> str;

         for (long c=0; c<sizeof(gPlanePartRelations)/sizeof(gPlanePartRelations[0]); c++)
         {
            file >> str;
            long id=atol(str);

            if (gPlanePartRelations[c].Id!=id) hprintf (0, "Id mismatch: %li vs %li!", gPlanePartRelations[c].Id, id);
            gPlanePartRelations[c].FromString (str);
         }
      }

      //Planebuilds:
      {
         std::ifstream file(FullFilename ("builds.csv", ExcelPath), std::ios_base::in);

         file >> str;

         for (long c=0; c<sizeof(gPlaneBuilds)/sizeof(gPlaneBuilds[0]); c++)
         {
            file >> str;
            long id=atol(str);

            if (gPlaneBuilds[c].Id!=id) hprintf (0, "Id mismatch: %li vs %li!", gPlaneBuilds[c].Id, id);
            gPlaneBuilds[c].FromString (str);
         }
      }
   }
}

//--------------------------------------------------------------------------------------------
//Lädt die Tip-Bitmaps:
//--------------------------------------------------------------------------------------------
void InitTipBms (void)
{
   SLONG c;

   gCursorFeetBms[0].ReSize (pGLibStd, "CWALKL01", 9, CREATE_VIDMEM);
   gCursorFeetBms[1].ReSize (pGLibStd, "CWALKR01", 9, CREATE_VIDMEM);

   gNotepadButtonL.ReSize (pGLibStd, GFX_NOTEB1);
   gNotepadButtonM.ReSize (pGLibStd, GFX_NOTEB3);
   gNotepadButtonR.ReSize (pGLibStd, GFX_NOTEB2);
   gZettelBms.ReSize (pGLibStd, "ZETTEL01", 3);

   gPostItBms.ReSize (pGLibStd, "POST_SR POST_SG POST_SR2 POST_SG2 POST_SB POST_SB2");

   FlugplanBms.ReSize (pGLibStd, "SCHED01", 52+5+2);
   gInfoBms.ReSize (pGLibStd, "INFO01", 2);

   FlugplanIconBms.ReSize(14);
   for (c=0; c<14; c++)
      FlugplanIconBms[c].ReSize (pGLibStd, bprintf ("I%c_00", "123456789ABCDEF"[c]), 2);

   BeraterBms.ReSize (12+4);
   BeraterBms[0].ReSize (pGLibBerater, "BERA1_01", 4);
   BeraterBms[1].ReSize (pGLibBerater, "BERA2_01", 4);
   BeraterBms[8].ReSize (pGLibBerater, "BERA8_01", 4);
   BeraterBms[9].ReSize (pGLibBerater, "BERA9_01", 4);
   BeraterBms[12].ReSize (pGLibBerater, "BER91_01", 8);
   BeraterBms[13].ReSize (pGLibBerater, "BER92_01", 8);
   BeraterBms[14].ReSize (pGLibBerater, "BER93_01", 8);
   BeraterBms[15].ReSize (pGLibBerater, "BER94_01", 8);

   SprechblasenBms.ReSize (pGLibStd, "BLASE0", 6);
   XBubbleBms.ReSize (pGLibStd, "XBLASE01", 14);

   gCityMarkerBm.ReSize (pGLibStd, GFX_CITYCUR);
   gDialogBarBm.ReSize (pGLibStd, GFX_DIALOG);

   gToolTipBms.ReSize (pGLibStd, "TOOLTIP1", 3);
   gUniversalPlaneBms.ReSize (pGLibPlanes, "999SB 999SV 999SR 999SG "
                                           "FLUG07 FLUG08 FLUG09 FLUG10 FLUG11 "
                                           "FLUGB01 FLUGB02 FLUGB03 FLUGB06 FLUGB07 "
                                           "FLUGV01 FLUGV02 FLUGV03 FLUGV06 FLUGV07 "
                                           "FLUGR01 FLUGR02 FLUGR03 FLUGR06 FLUGR07 "
                                           "FLUGG01 FLUGG02 FLUGG03 FLUGG06 FLUGG07 FLUGB08 FLUGB99 FLUGB98 FLUG12");
   gEditorPlane2dBms.ReSize (pGLibStd, "P2D_B1 P2D_B2 P2D_B3 P2D_B4 P2D_B5 "
                                       "P2D_C1 P2D_C2 P2D_C3 P2D_C4 P2D_C5 "
                                       "P2D_H1 P2D_H2 P2D_H3 P2D_H4 P2D_H5 P2D_H6 P2D_H7 "
                                       "P2D_R1 P2D_R2 P2D_R3 P2D_R4 P2D_R5 P2D_R6 "
                                       "P2D_M1 P2D_M2 P2D_M3 P2D_M4 P2D_M5 P2D_M6 P2D_M7 P2D_M8 "
                                       "P2D_L1 P2D_L2 P2D_L3 P2D_L4 P2D_L5 P2D_L6 ");


   gSmokeBms.ReSize (pGLibStd, "SMOKE01", 1);
   gStenchBms.ReSize (pGLibStd, "STENCH01", 1);
}

//--------------------------------------------------------------------------------------------
//Initialisiert die Erdkugel:
//--------------------------------------------------------------------------------------------
void InitGlobeMapper (void)
{
   int x,y, xs;
   dword Color;

   //Dafault-Positionen der Laptop-Fenster:
   GlobeWindows.ReSize (5);

   for (x=0; x<5; x++)
      GlobeWindows[x] = XY (IconsPos[(x+1)*2+24]+60, IconsPos[(x+1)*2+1+24]-200/6);

   SBBM TmpBm (10,10);

   PALETTE EarthPal;
   EarthPal.RefreshPalFromLbm((char*)(LPCTSTR)FullFilename ("EarthAll.lbm", GliPath));

   TECBM ShadeBm ((char*)(LPCTSTR)FullFilename ("shade.pcx", GliPath), SYSRAMBM);

   GlobeMixTab.ReSize (256*64);

   for (x=0; x<256; x++)
      for (y=0; y<64; y++)
      {
         Color  = std::min(EarthPal.Pal[x].b*(y+5)/40,255);
         Color += std::min(EarthPal.Pal[x].r*(y+5)/40+(std::max(EarthPal.Pal[x].b*(y+5)/40-255,0)),255)<<16;
         Color += std::min(EarthPal.Pal[x].g*(y+5)/40+(std::max(EarthPal.Pal[x].b*(y+5)/40-255,0)),255)<<8;

         GlobeMixTab [x+(y<<8)] = (UWORD)TmpBm.pBitmap->GetHardwarecolor (Color);
         if (GlobeMixTab [x+(y<<8)]==0)
            GlobeMixTab [x+(y<<8)]=(UWORD)TmpBm.pBitmap->GetHardwarecolor (0x000008);
      }

   GlobeLight.ReSize (369);
   GlobeMapper.ReSize (185);
   GlobeMapperY.ReSize (369);

   TECBMKEYC      ShadeKey(ShadeBm);
   const UBYTE *s=ShadeKey.Bitmap;

   for (y=0; y<369; y++)
   {
      xs = (SLONG)(sqrt (184*184-(y-184)*(y-184)));

      GlobeLight[y].ReSize (xs*2+1);
      if (y<=184) GlobeMapper[y].ReSize (xs*2+1);
      GlobeMapperY[y]=SLONG(asin ((y-184)/184.0)/3.14159*2*250+250);

      for (x=-xs; x<=xs; x++)
      {
         (GlobeLight[y])[x+xs]=s[184+x];
         //(GlobeLight[y])[x+xs]=UBYTE(150-sqrt((x+80)*(x+80)+(y-80)*(y-80))/2.8);

         if (y<=184)
            (GlobeMapper[y])[x+xs]=UWORD(asin (x/double(xs))/3.14159*2*128+128);
      }

      s+=ShadeKey.lPitch;
   }
}