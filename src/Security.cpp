//============================================================================================
// Security.cpp : Der Security-Raum
//============================================================================================
#include "stdafx.h"
#include "Security.h"
#include "glsecurity.h"
#include "atnet.h"
#include <dsound.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DSBVOLUME_MIN               -10000
#define DSBVOLUME_MAX               0

//Zum debuggen:
static const char FileId[] = "Secu";

//--------------------------------------------------------------------------------------------
//Die Schalter wird erˆffnet:
//--------------------------------------------------------------------------------------------
CSecurity::CSecurity(BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "security.gli", GFX_SECURITY)
{
   SetRoomVisited (PlayerNum, ROOM_SECURITY);
   HandyOffset = 320;
   Sim.FocusPerson=-1;

   if (!bHandy) AmbientManager.SetGlobalVolume (60);

   Talkers.Talkers[TALKER_SECURITY].IncreaseReference ();
   DefaultDialogPartner=TALKER_SECURITY;

   KommVar=-1;

   SP_Secman.ReSize (15);
   SP_Secman.Clips[0].ReSize (0, "SO_normal.smk", "", XY (240, 79), SPM_IDLE,       CRepeat(2,3), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                              "A1A9A1A1E1", 0, 14, 7, 10, 1);
   SP_Secman.Clips[1].ReSize (1, "SO_zum_reden.smk", "", XY (240, 79), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten->Speak
                              "E1E1E1", 2, 4, 6);
   SP_Secman.Clips[2].ReSize (2, "SO_reden_mund_zu.smk", "", XY (240, 79), SPM_LISTENING,  CRepeat(1,1), CPostWait(50,50), SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Listen
                              "A9E5E5", 2, 4, 5);
   SP_Secman.Clips[3].ReSize (3, "ArabP.smk", "", XY (240, 79), SPM_TALKING,    CRepeat(1,1), CPostWait(20,20), SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Psst!, danach Speaking
                              "A9", 4);
   SP_Secman.Clips[4].ReSize (4, "SO_reden.smk", "", XY (240, 79), SPM_TALKING,    CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Speak
                              "A9E1E1", 4, 2, 5);
   SP_Secman.Clips[5].ReSize (5, "SO_reden_zurueck.smk", "", XY (240, 79), SPM_IDLE, CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Speak->Wait
                              "A9", 0);

   SP_Secman.Clips[6].ReSize (6, "SO_erschrecken.smk", "", XY (240, 79), SPM_IDLE, CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Speak->Wait
                              "A9", 0);
   SP_Secman.Clips[7].ReSize (7, "SO_hindrehen.smk", "", XY (240, 79), SPM_IDLE, CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Speak->Wait
                              "A2A1E1", 8, 9, 9);
   SP_Secman.Clips[8].ReSize (8, "SO_druecken.smk", "", XY (256, 63), SPM_IDLE, CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Speak->Wait
                              "A9A9E1", 8, 9, 9);
   SP_Secman.Clips[9].ReSize (9, "SO_herdrehen.smk", "", XY (240, 79), SPM_IDLE, CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Speak->Wait
                              "A1", 0);

   SP_Secman.Clips[10].ReSize (10, "SO_tasse_nehmen.smk", "", XY (240, 79), SPM_IDLE, CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,  //Speak->Wait
                               "A1A1E1", 11, 12, 13);
   SP_Secman.Clips[11].ReSize (11, "SO_tasse_trinken.smk", "", XY (240, 79), SPM_IDLE, CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,  //Speak->Wait
                               "A1A1E1", 12, 13, 13);
   SP_Secman.Clips[12].ReSize (12, "SO_tasse_zittern.smk", "", XY (240, 79), SPM_IDLE, CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,  //Speak->Wait
                               "A1A1A1E1", 11, 12, 13, 13);
   SP_Secman.Clips[13].ReSize (13, "SO_tasse_hinsetzen.smk", "", XY (240, 79), SPM_IDLE, CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_DONTCANCEL,
                               NULL, SMACKER_CLIP_SET, 0, NULL,  //Speak->Wait
                               "A1", 0);

   SP_Secman.Clips[14].ReSize (14, "SO_normal.smk", "", XY (240, 79), SPM_IDLE,       CRepeat(1,1), CPostWait(120,120),   SMACKER_CLIP_CANCANCEL,
                               &KommVar, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, 1, NULL,  //Speak->Wait
                               "A1E1", 6, 1);

   SP_Tuer.ReSize (1);
   SP_Tuer.Clips[0].ReSize (0, "SO_tuer.smk", "", XY (38, 145), SPM_IDLE,       CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                            NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                            "A9", 0);

   SP_Tresor.ReSize (2);
   SP_Tresor.Clips[0].ReSize (0, "SO_tresor_pause.smk", "", XY (528, 198), SPM_IDLE,  CRepeat(1,1), CPostWait(20,20),   SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                              "A9A2", 0, 1);
   SP_Tresor.Clips[1].ReSize (1, "SO_tresor.smk", "", XY (528, 198), SPM_IDLE,  CRepeat(1,3), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                              NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                              "A9A3", 1, 0);

   SP_Spion.ReSize (2);
   SP_Spion.Clips[0].ReSize (0, "SO_spion_pause.smk", "", XY (85, 327), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                             NULL, SMACKER_CLIP_SET, 0, &KommVar,  //Warten
                             "A9A0", 0, 1);
   SP_Spion.Clips[1].ReSize (1, "SO_spion.smk", "", XY (85, 327), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                             &KommVar, SMACKER_CLIP_SET|SMACKER_CLIP_PRE, 0, NULL,  //Warten
                             "A9", 0);

   SP_Gasmaske.ReSize (2);
   SP_Gasmaske.Clips[0].ReSize (0, "SO_gasmask_pause.smk", "", XY (468, 89), SPM_IDLE,  CRepeat(1,1), CPostWait(80,80),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A4", 0, 1);
   SP_Gasmaske.Clips[1].ReSize (1, "SO_gasmask.smk", "", XY (468, 89), SPM_IDLE,  CRepeat(1,1), CPostWait(0,0),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 0);

   SP_Cam1.ReSize (14);
   SP_Cam1.Clips[0].ReSize (0, "KamA12.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 1, 2);
   SP_Cam1.Clips[1].ReSize (1, "KamA2.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 2);
   SP_Cam1.Clips[2].ReSize (2, "KamA23.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 3, 4);
   SP_Cam1.Clips[3].ReSize (3, "KamA3.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 4);
   SP_Cam1.Clips[4].ReSize (4, "KamA34.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 5, 6);
   SP_Cam1.Clips[5].ReSize (5, "KamA4.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 6);
   SP_Cam1.Clips[6].ReSize (6, "KamA45.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 7, 8);
   SP_Cam1.Clips[7].ReSize (7, "KamA5.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 8);
   SP_Cam1.Clips[8].ReSize (8, "KamA56.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 9, 10);
   SP_Cam1.Clips[9].ReSize (9, "KamA6.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 10);
   SP_Cam1.Clips[10].ReSize (10, "KamA67.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 11, 12);
   SP_Cam1.Clips[11].ReSize (11, "KamA7.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 12);
   SP_Cam1.Clips[12].ReSize (12, "KamA71.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 13, 0);
   SP_Cam1.Clips[13].ReSize (13, "KamA1.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 0);

   SP_Cam2.ReSize (14);
   SP_Cam2.Clips[0].ReSize (0, "KamB12.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 1, 2);
   SP_Cam2.Clips[1].ReSize (1, "KamB2.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 2);
   SP_Cam2.Clips[2].ReSize (2, "KamB23.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 3, 4);
   SP_Cam2.Clips[3].ReSize (3, "KamB3.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 4);
   SP_Cam2.Clips[4].ReSize (4, "KamB34.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 5, 6);
   SP_Cam2.Clips[5].ReSize (5, "KamB4.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 6);
   SP_Cam2.Clips[6].ReSize (6, "KamB45.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 7, 8);
   SP_Cam2.Clips[7].ReSize (7, "KamB5.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 8);
   SP_Cam2.Clips[8].ReSize (8, "KamB56.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 9, 10);
   SP_Cam2.Clips[9].ReSize (9, "KamB6.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 10);
   SP_Cam2.Clips[10].ReSize (10, "KamB67.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 11, 12);
   SP_Cam2.Clips[11].ReSize (11, "KamB7.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 12);
   SP_Cam2.Clips[12].ReSize (12, "KamB71.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 13, 0);
   SP_Cam2.Clips[13].ReSize (13, "KamB1.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 0);

   SP_Cam3.ReSize (6);
   SP_Cam3.Clips[0].ReSize (0, "KamC12.smk", "", XY (362, 83), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 1, 2);
   SP_Cam3.Clips[1].ReSize (1, "KamC2.smk", "", XY (362, 83), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 2);
   SP_Cam3.Clips[2].ReSize (2, "KamC23.smk", "", XY (362, 83), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 3, 4);
   SP_Cam3.Clips[3].ReSize (3, "KamC3.smk", "", XY (362, 83), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 4);
   SP_Cam3.Clips[4].ReSize (4, "KamC31.smk", "", XY (362, 83), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 5, 0);
   SP_Cam3.Clips[5].ReSize (5, "KamC1.smk", "", XY (362, 83), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 0);

   SP_Cam4.ReSize (6);
   SP_Cam4.Clips[0].ReSize (0, "KamD12.smk", "", XY (348, 277), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 1, 2);
   SP_Cam4.Clips[1].ReSize (1, "KamD2.smk", "", XY (348, 277), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 2);
   SP_Cam4.Clips[2].ReSize (2, "KamD23.smk", "", XY (348, 277), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 3, 4);
   SP_Cam4.Clips[3].ReSize (3, "KamD3.smk", "", XY (348, 277), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 4);
   SP_Cam4.Clips[4].ReSize (4, "KamD31.smk", "", XY (348, 277), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 5, 0);
   SP_Cam4.Clips[5].ReSize (5, "KamD1.smk", "", XY (348, 277), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 0);

   SP_Cam5.ReSize (6);
   SP_Cam5.Clips[0].ReSize (0, "KamE12.smk", "", XY (509, 47), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 1, 2);
   SP_Cam5.Clips[1].ReSize (1, "KamE2.smk", "", XY (509, 47), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 2);
   SP_Cam5.Clips[2].ReSize (2, "KamE23.smk", "", XY (509, 47), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 3, 4);
   SP_Cam5.Clips[3].ReSize (3, "KamE3.smk", "", XY (509, 47), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 4);
   SP_Cam5.Clips[4].ReSize (4, "KamE31.smk", "", XY (509, 47), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9A9", 5, 0);
   SP_Cam5.Clips[5].ReSize (5, "KamE1.smk", "", XY (509, 47), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 0);

   /*SP_Cam1.ReSize (1);
   SP_Cam1.Clips[0].ReSize (0, "SO_Kam_HiLi.smk", "", XY (125, 85), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 0);

   SP_Cam3.ReSize (1);
   SP_Cam3.Clips[0].ReSize (0, "SO_Kam_HiRe.smk", "", XY (362, 83), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 0);
   SP_Cam4.ReSize (1);
   SP_Cam4.Clips[0].ReSize (0, "SO_Kam_Pult.smk", "", XY (348, 277), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 0);
   SP_Cam5.ReSize (1);
   SP_Cam5.Clips[0].ReSize (0, "SO_Kam_VoRe.smk", "", XY (509, 47), SPM_IDLE,  CRepeat(1,1), CPostWait(0,20),   SMACKER_CLIP_CANCANCEL,
                                NULL, SMACKER_CLIP_SET, 0, NULL,  //Warten
                                "A9", 0);*/

   SDL_ShowWindow(FrameWnd->m_hWnd);
   SDL_UpdateWindowSurface(FrameWnd->m_hWnd);

   //Hintergrundsounds:
   if (Sim.Options.OptionEffekte)
   {
      BackFx.ReInit("secure.raw");
      BackFx.Play(DSBPLAY_NOSTOP|DSBPLAY_LOOPING, Sim.Options.OptionEffekte*100/7);
   }
}

//--------------------------------------------------------------------------------------------
//Sehr destruktiv!
//--------------------------------------------------------------------------------------------
CSecurity::~CSecurity()
{
   BackFx.SetVolume(DSBVOLUME_MIN);
   BackFx.Stop();
   Talkers.Talkers[TALKER_SECURITY].DecreaseReference ();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CSecurity message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//void CSecurity::OnPaint()
//--------------------------------------------------------------------------------------------
void CSecurity::OnPaint()
{
   if (!bHandy) SetMouseLook (CURSOR_NORMAL, 0, ROOM_SECURITY, 0);

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   SP_Tuer.Pump ();
   SP_Gasmaske.Pump ();
   SP_Cam1.Pump ();
   SP_Cam3.Pump ();
   SP_Cam4.Pump ();
   SP_Cam5.Pump ();
   SP_Spion.Pump ();
   SP_Tresor.Pump ();
   SP_Secman.Pump ();

   SP_Tuer.BlitAtT (RoomBm);
   SP_Gasmaske.BlitAtT (RoomBm);
   SP_Cam1.BlitAtT (RoomBm);
   SP_Cam3.BlitAtT (RoomBm);
   SP_Cam4.BlitAtT (RoomBm);
   SP_Cam5.BlitAtT (RoomBm);
   SP_Spion.BlitAtT (RoomBm);
   SP_Tresor.BlitAtT (RoomBm);
   SP_Secman.BlitAtT (RoomBm);

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   if (!IsDialogOpen() && !MenuIsOpen())
   {
      if (gMousePosition.IfIsWithin (4, 70, 112, 412)) SetMouseLook (CURSOR_EXIT, 0, ROOM_SECURITY, 999);
      else if (gMousePosition.IfIsWithin (288,144,401,267)) SetMouseLook (CURSOR_HOT, 0, ROOM_SECURITY, 10);
   }

   CStdRaum::PostPaint ();
   CStdRaum::PumpToolTips ();
}

//--------------------------------------------------------------------------------------------
//void CSecurity::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CSecurity::OnLButtonDown(UINT nFlags, CPoint point)
{
   XY RoomPos;

   DefaultOnLButtonDown ();

   if (!ConvertMousePosition (point, &RoomPos))
   {
	   CStdRaum::OnLButtonDown(nFlags, point);
      return;
   }

   if (!PreLButtonDown (point))
   {
      if (MouseClickArea==ROOM_SECURITY && MouseClickId==999) Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
      else if (MouseClickArea==ROOM_SECURITY && MouseClickId==10) { StartDialog (TALKER_SECURITY, MEDIUM_AIR, 1000); }
      else if (MouseClickArea==ROOM_SECURITY && MouseClickId==12)
      {
         Sim.Players.Players[(SLONG)PlayerNum].BuyItem (ITEM_GLOVE);

         if (Sim.Players.Players[(SLONG)PlayerNum].HasItem (ITEM_GLOVE))
         {
            Sim.ItemGlove=0;
            Sim.SendSimpleMessage (ATNET_TAKETHING, NULL, ITEM_GLOVE);
         }
      }
      else CStdRaum::OnLButtonDown(nFlags, point);
   }
}

//--------------------------------------------------------------------------------------------
//void CSecurity::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CSecurity::OnRButtonDown(UINT nFlags, CPoint point)
{
   DefaultOnRButtonDown ();

   //Auﬂerhalb geklickt? Dann Default-Handler!
   if (point.x<WinP1.x || point.y<WinP1.y || point.x>WinP2.x || point.y>WinP2.y)
   {
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
