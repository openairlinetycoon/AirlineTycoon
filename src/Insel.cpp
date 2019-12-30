//============================================================================================
// Insel.cpp : Die Insel mit den Raketen
//============================================================================================
// Link: "Insel.h"
//============================================================================================
#include "stdafx.h"
#include "glinsel.h"
#include "glnoinsl.h"
#include "Insel.h"

#define ROOM_LIMIT DIFF_NORMAL

extern SB_CColorFX ColorFX;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//F¸r die Zeichenreihenfolge
static SLONG PartsRemapper [] = { 0, 1, 2, 3, 9, 4, 6, 5, 8, 7 };

static XY RocketPartOffsets[] = { XY(  0, 50), XY(  3,  2), XY(  4,  8),
                                  XY( 18, 29), XY( 39, 29), XY( 19, 27), XY( 23, 29), XY( 31, 29), XY( 19, 15), XY( 19,  0) };

static XY RocketOffsets    [] = { XY(149-34,126-44), XY(450-44,126-43), XY(779-34-7,129-42), XY(1089-34-7,140-54) };
static XY RocketRefOffsets [] = { XY(149-34,328-44-90-30), XY(450-44,328-43-90-40), XY(779-34-7,325-42-90-30), XY(1089-34-7,316-54-90) };

//Zum debuggen:
static const char FileId[] = "Insl";

class CWaitCursorNow
{
public:
   CWaitCursorNow ()
   {
      MouseWait++;
      pCursor->SetImage (gCursorSandBm.pBitmap);
   };

   ~CWaitCursorNow ()
   {
      MouseWait--;
      if (MouseWait==0) pCursor->SetImage (gCursorBm.pBitmap);
   };
};

//--------------------------------------------------------------------------------------------
//Bereitet eine Reflexionstabelle vor:
//--------------------------------------------------------------------------------------------
void PrepareReflexionTable (SBBM &ReflexionMaskBm, BUFFER<UBYTE> *pReflexionTable)
{
#ifndef DEMO
   BUFFER<UBYTE> &ReflexionTable = *pReflexionTable;

   SLONG x, y, xx, yy;

   ReflexionTable.ReSize (ReflexionMaskBm.Size.x*ReflexionMaskBm.Size.y);

   ReflexionTable.FillWith (15);

   SB_CBitmapKey SrcKey(*ReflexionMaskBm.pBitmap);

   //Weniger Bewegung links und rechts:
   for (y=0; y<ReflexionMaskBm.Size.y; y++)
      for (x=0; x<16; x++)
      {
         ReflexionTable[y*ReflexionMaskBm.Size.x+x]=(UBYTE)x;
         ReflexionTable[y*ReflexionMaskBm.Size.x+ReflexionMaskBm.Size.x-1-x]=(UBYTE)x;
      }

   //Weniger Bewegung bei den Objekten in der Bitmap
   for (y=0; y<ReflexionMaskBm.Size.y; y++)
   {
      UWORD *pSrcAdress=(UWORD*)(((UBYTE*)SrcKey.Bitmap)+y*SrcKey.lPitch);
      UBYTE *pTable=ReflexionTable+y*ReflexionMaskBm.Size.x;

      for (x=0; x<ReflexionMaskBm.Size.x; x++)
      {
         UWORD p=pSrcAdress[x]&31;

         if (p>=28) //weiss
         {
            pTable[x]=0;

            for (yy=1; yy<16; yy++)
               if (y+yy<ReflexionMaskBm.Size.y)
                  for (xx=-yy; xx<=yy; xx++)
                     if (x+xx>=0 && x+xx<ReflexionMaskBm.Size.x)
                        pTable[yy*ReflexionMaskBm.Size.x+x+xx]=(UBYTE)min (yy, pTable[yy*ReflexionMaskBm.Size.x+x+xx]);
         }
         else if (p>=10) //grau
         {
            pTable[x]=0;
         }
      }
   }
#endif
}

//--------------------------------------------------------------------------------------------
//Berechnet den Wassereffekt:
//--------------------------------------------------------------------------------------------
#pragma optimize("agptwy", on)
void WaterBlur (SBBM *pTargetBm, SLONG AnimOffset, XY TargetOffset, SBBM &ReflexionSourceBm, const BUFFER<UBYTE> &pReflexionTable)
{
#ifndef DEMO
   static SLONG  x, y;
   static SLONG  MoveOffset[16];
   static SLONG *pMoveOffset=MoveOffset;
   static UWORD *pTgtPixel;
   static UWORD *pSrcPixel;
   static UBYTE *pRefTable;

   SB_CBitmapKey SrcKey(*ReflexionSourceBm.pBitmap);
   SB_CBitmapKey TgtKey(*pTargetBm->pBitmap);

   if (pTargetBm->Size.x-1>=0)
   for (y=0; y<ReflexionSourceBm.Size.y; y++)
   {
      long tx = SLONG(sin((timeGetTime()+sin((y+AnimOffset)/10.0)*2400)/500.0)*7);
      //long tx = SLONG(sin((timeGetTime()+y*240)/1000.0)*7);

      for (x=0; x<16; x++)
         MoveOffset[x]=tx*x/15;

      pTgtPixel = (UWORD*) (((char*)(TgtKey.Bitmap))+(y+TargetOffset.y)*TgtKey.lPitch);
      pSrcPixel = (UWORD*) (((char*)(((UWORD*)SrcKey.Bitmap)+TargetOffset.x))+y*SrcKey.lPitch);
      pRefTable = pReflexionTable+y*ReflexionSourceBm.Size.x+TargetOffset.x;

      x=pTargetBm->Size.x-1;

      /*__asm
      {
         push  esi
         push  edi
         push  ebp

         mov   ecx, x
         mov   esi, pSrcPixel
         mov   edi, pTgtPixel
         mov   eax, pMoveOffset
         mov   ebp, pRefTable

         inc   ecx
         add   esi, 2
         add   edi, 2
         inc   ebp

         test  ecx, 1
         jz    again

         xor   edx, edx
         mov   dl,  [ebp+ecx]

         test  dl, dl
         jz    a0

         mov   edx, [eax+edx*4]
         add   edx, ecx
         mov    dx, [esi+edx*2]
         mov   [edi+ecx*2], dx

a0:
         dec   ecx
         jz    ende

again:
         xor   edx, edx
         xor   ebx, ebx
         mov   dl,  [ebp+ecx]
         mov   bl,  [ebp+ecx-1]

         test  dl,dl
         jz    a2
         test  bl,bl
         jz    a1

         mov   edx, [eax+edx*4]
         mov   ebx, [eax+ebx*4]
         add   edx, ecx
         lea   ebx, [ebx+ecx-1]
         mov    dx, [esi+edx*2]
         mov    bx, [esi+ebx*2]
         mov   [edi+ecx*2],   dx
         mov   [edi+ecx*2-2], bx

         sub   ecx, 2
         jz    ende

         xor   edx, edx
         xor   ebx, ebx
         mov   dl,  [ebp+ecx]
         mov   bl,  [ebp+ecx-1]

         test  dl,dl
         jz    a2
         test  bl,bl
         jz    a1

         mov   edx, [eax+edx*4]
         mov   ebx, [eax+ebx*4]
         add   edx, ecx
         lea   ebx, [ebx+ecx-1]
         mov    dx, [esi+edx*2]
         mov    bx, [esi+ebx*2]
         mov   [edi+ecx*2],   dx
         mov   [edi+ecx*2-2], bx

         sub   ecx, 2
         jnz   again

         jmp   ende

a2:
         test  bl,bl
         jz    a3

         mov   ebx, [eax+ebx*4]
         lea   ebx, [ebx+ecx-1]
         mov    bx, [esi+ebx*2]
         mov   [edi+ecx*2-2], bx

         sub   ecx, 2
         jnz   again
         jmp   ende

a1:
         mov   edx, [eax+edx*4]
         add   edx, ecx
         mov    dx, [esi+edx*2]
         mov   [edi+ecx*2],   dx

a3:
         sub   ecx, 2
         jnz   again

         jmp   ende

ende:
         pop   ebp
         pop   edi
         pop   esi
      }*/

      for (x=pTargetBm->Size.x-1; x>=0; x--)
         if (pRefTable[x])
            pTgtPixel[x] = pSrcPixel[x+MoveOffset[pRefTable[x]]];
   }
#endif
}
#pragma optimize("", off)

//--------------------------------------------------------------------------------------------
//Die Raketen Inseln
//--------------------------------------------------------------------------------------------
CInsel::CInsel(BOOL bHandy, ULONG PlayerNum) : CStdRaum (bHandy, PlayerNum, "", NULL)
{
   CWaitCursorNow wc; //CD-Cursor anzeigen

#ifndef DEMO
   Sim.FocusPerson=-1;

   if (Sim.Difficulty<ROOM_LIMIT)
   {
      Sim.Players.Players[(SLONG)PlayerNum].ChangeMoney (-1, 3300, "");

      ReSize ("noinsel.gli", GFX_NOINSEL);
   }
   else
   {
      ReSize ("insel.gli", NULL);

      SetRoomVisited (PlayerNum, ROOM_INSEL);
      HandyOffset = 320;

      Sim.Players.Players[(SLONG)PlayerNum].ChangeMoney (-1, 3300, "");

      SBBM TempBm;

      FernglasBms.ReSize (pRoomLib, "O1 O2 ATOP ABOTTOM AL1 AL2 AL3 AR1 AR2 AR3");
      IslandBm.ReSize (pRoomLib, GFX_INSEL, CREATE_SYSMEM);
      ReflexionBm.ReSize (pRoomLib, GFX_REFLEX, CREATE_SYSMEM);
      ReflexionInselBm.ReSize (pRoomLib, GFX_REFINSEL, CREATE_SYSMEM);
      FrontBm.ReSize (pRoomLib, GFX_FRONT, CREATE_SYSMEM);

      TempBm.ReSize (pRoomLib, GFX_REFMASK, CREATE_SYSMEM);
      PrepareReflexionTable (TempBm, &ReflexionMask);

      VogelBms.ReSize (pRoomLib, "VOGEL01", 8);
      VogelSail=0;
      VogelY=22*1000;
      VogelOffset=timeGetTime()+rand()%5000+2000;
      LastVogelTime=timeGetTime();

      ShipBm.ReSize (pRoomLib, "BOAT");
      ShipReflexBm.ReSize (pRoomLib, "BOATM");
      ShipWaveBms.ReSize (pRoomLib, "BOATW01", 3);
   
      TempBm.ReSize (pRoomLib, "BOATMASK");
      PrepareReflexionTable (TempBm, &ShipReflexionMask);

      RoomBm.ReSize (640, 440);

      ShipOffset=timeGetTime();

      RocketPartBms.ReSize (pRoomLib, "ROCK00 ROCK01 ROCK02 ROCK03 ROCK05 ROCK07 ROCK06 ROCK09 ROCK08 ROCK04 "
                                      "ROCK00 ROCK01 ROCK12 ROCK03 ROCK15 ROCK17 ROCK16 ROCK09 ROCK18 ROCK14 "
                                      "ROCK00 ROCK01 ROCK22 ROCK03 ROCK25 ROCK27 ROCK26 ROCK09 ROCK28 ROCK24 "
                                      "ROCK00 ROCK01 ROCK32 ROCK03 ROCK35 ROCK37 ROCK36 ROCK09 ROCK38 ROCK34");

      RocketPartReflexBms.ReSize (pRoomLib, "RROCK00 RROCK01 RROCK02 RROCK03 RROCK05 RROCK07 RROCK06 RROCK09 RROCK08 RROCK04 "
                                            "RROCK00 RROCK01 RROCK12 RROCK03 RROCK15 RROCK17 RROCK16 RROCK09 RROCK18 RROCK14 "
                                            "RROCK00 RROCK01 RROCK22 RROCK03 RROCK25 RROCK27 RROCK16 RROCK09 RROCK28 RROCK24 "
                                            "RROCK00 RROCK01 RROCK32 RROCK03 RROCK35 RROCK37 RROCK16 RROCK09 RROCK38 RROCK34");

      if (!bHandy) AmbientManager.SetGlobalVolume (60);

      //Hintergrundsounds:
      if (Sim.Options.OptionEffekte)
      {
         SetBackgroundFx (0, "moeve.raw",    50000, 25000);

         WellenFx.ReInit ("wellen.raw");
         WellenFx.Play(DSBPLAY_NOSTOP|DSBPLAY_LOOPING, Sim.Options.OptionEffekte*100/7);
      }

      BlinkArrowsTimer=0;
   }

   KonstruktorFinished=TRUE;

   SDL_ShowWindow(FrameWnd->m_hWnd);
   SDL_UpdateWindowSurface(FrameWnd->m_hWnd);
#endif
}

//--------------------------------------------------------------------------------------------
//Sehr destruktiv!
//--------------------------------------------------------------------------------------------
CInsel::~CInsel()
{
   FernglasBms.Destroy();
   IslandBm.Destroy();
   ReflexionBm.Destroy();
   ReflexionInselBm.Destroy();
   FrontBm.Destroy();

   RocketPartBms.Destroy();
   RocketPartReflexBms.Destroy();

   VogelBms.Destroy();

   ShipBm.Destroy();
   ShipReflexBm.Destroy();
   ShipWaveBms.Destroy();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CInsel message handlers
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//void CInsel::OnPaint()
//--------------------------------------------------------------------------------------------
void CInsel::OnPaint()
{
#ifndef DEMO
   SLONG c, d;

   if (!KonstruktorFinished) return;

   static SLONG gMouseScrollSpeed=0;
   XY   &ViewPos = Sim.Players.Players[(SLONG)PlayerNum].IslandViewPos;

   XY   BoatPos = XY(1300-((timeGetTime()-ShipOffset)/80)-ViewPos.x, 264);
   SBBM TempBm;

   BoatPos.y+=((1300-(SLONG(timeGetTime()-ShipOffset)/80))-1280)/60;

   if (!bHandy) SetMouseLook (CURSOR_NORMAL, 0, ROOM_INSEL, 0);

   if (!Sim.UsedTelescope)
   {
      BlinkArrowsTimer=timeGetTime();
      Sim.UsedTelescope=TRUE;
   }

   //Die Standard Paint-Sachen kann der Basisraum erledigen
   CStdRaum::OnPaint ();

   if (Sim.Difficulty>=ROOM_LIMIT)
   {
      //Inseln:
      RoomBm.BlitFrom (IslandBm, -ViewPos.x, 0);

      //Raketen-Reflexionen zeichnen:
      TempBm.ReSize (ReflexionBm.Size.x, ReflexionBm.Size.y);
      TempBm.BlitFrom (ReflexionBm);
      for (c=0; c<4; c++)
         if (!Sim.Players.Players[c].IsOut)
            for (d=0; d<10; d++)
            //for (e=0; e<10; e++) if (PartsRemapper[e]==d)
               if (Sim.Players.Players[c].RocketFlags & (1<<PartsRemapper[d]))
                  TempBm.BlitFromT (RocketPartReflexBms[c*10+PartsRemapper[d]], RocketRefOffsets[c]-XY(0, RocketPartReflexBms[c*10+PartsRemapper[d]].Size.y));
      TempBm.BlitFromT (ReflexionInselBm, 10, 236-232);

      //Wasser & Raketen vermanschen:
      WaterBlur (&RoomBm, 0, XY(ViewPos.x,232), TempBm, ReflexionMask);

      //Reflexion vom Schiff:
      TempBm.ReSize (ShipReflexBm.Size.x, ShipReflexBm.Size.y);
      TempBm.BlitFrom (ShipReflexBm);
      WaterBlur (&TempBm, BoatPos.y+61-232, XY(0,0), ShipReflexBm, ShipReflexionMask);
      RoomBm.BlitFromT (TempBm, BoatPos+XY(-14, 61));

      //Raketen zeichnen:
      for (c=0; c<4; c++)
         if (!Sim.Players.Players[c].IsOut)
            for (d=0; d<10; d++)
            //for (e=0; e<10; e++) if (PartsRemapper[e]==d)
               if (Sim.Players.Players[c].RocketFlags & (1<<PartsRemapper[d]))
                  RoomBm.BlitFromT (RocketPartBms[c*10+PartsRemapper[d]], RocketOffsets[c]-XY(ViewPos.x,0));
                  //RoomBm.BlitFromT (RocketPartBms[c*10+PartsRemapper[d]], RocketOffsets[c]+RocketPartOffsets[PartsRemapper[d]]-XY(ViewPos.x,0));

      //Ship+Gischt:
      RoomBm.BlitFromT (ShipBm, BoatPos);
      ColorFX.BlitTrans (ShipWaveBms[SLONG(timeGetTime()/100%ShipWaveBms.AnzEntries())].pBitmap, RoomBm.pBitmap, BoatPos +XY (0,57), NULL, 4);

      //Front-Layer (Berge)
      RoomBm.BlitFromT (FrontBm, -ViewPos.x, 440-FrontBm.Size.y);

      //Vogel...
      if (VogelSail)
         RoomBm.BlitFromT (VogelBms[SLONG(4)], (timeGetTime()-VogelOffset)/25-ViewPos.x, VogelY/1000);
      else 
         RoomBm.BlitFromT (VogelBms[SLONG((timeGetTime()-VogelOffset)/100%VogelBms.AnzEntries())], (timeGetTime()-VogelOffset)/25-ViewPos.x, VogelY/1000);

      if (!VogelSail && ((timeGetTime()-VogelOffset)/100%VogelBms.AnzEntries())==4 && rand()%10==0)
         VogelSail=1;
      if (VogelSail && ((timeGetTime()-VogelOffset)/100%VogelBms.AnzEntries())==4 && rand()%7==0)
         VogelSail=0;

      if (VogelSail) VogelY+=(timeGetTime()-LastVogelTime)*4;
      LastVogelTime=timeGetTime();

      //Fernglas (Alpha):
      ColorFX.BlitAlpha (FernglasBms[2].pBitmap, RoomBm.pBitmap, XY (108,0));
      ColorFX.BlitAlpha (FernglasBms[3].pBitmap, RoomBm.pBitmap, XY (108,411));
      ColorFX.BlitAlpha (FernglasBms[4].pBitmap, RoomBm.pBitmap, XY ( 39,30));
      ColorFX.BlitAlpha (FernglasBms[5].pBitmap, RoomBm.pBitmap, XY (  0,95));
      ColorFX.BlitAlpha (FernglasBms[6].pBitmap, RoomBm.pBitmap, XY ( 39,346));
      ColorFX.BlitAlpha (FernglasBms[7].pBitmap, RoomBm.pBitmap, XY (513,30));
      ColorFX.BlitAlpha (FernglasBms[8].pBitmap, RoomBm.pBitmap, XY (589,94));
      ColorFX.BlitAlpha (FernglasBms[9].pBitmap, RoomBm.pBitmap, XY (515,346));

      //Fernglas (Non-Alpha):
      RoomBm.BlitFrom (FernglasBms[0], 532, 0);    //O1
      RoomBm.BlitFrom (FernglasBms[0], 532, 410);
      RoomBm.BlitFrom (FernglasBms[0],   0, 0);
      RoomBm.BlitFrom (FernglasBms[0],   0, 410);
      RoomBm.BlitFrom (FernglasBms[1], 601, 30);   //O2
      RoomBm.BlitFrom (FernglasBms[1], 601, 346);
      RoomBm.BlitFrom (FernglasBms[1],   0, 30);
      RoomBm.BlitFrom (FernglasBms[1],   0, 346);

      //Wiederholung f¸r Vogel und Schiff
      if (timeGetTime()>DWORD(VogelOffset) && (timeGetTime()-VogelOffset)/100>1400)
      {
         VogelOffset=timeGetTime()+rand()%5000+2000;
         VogelY=22*1000;
      }

      if (timeGetTime()>DWORD(ShipOffset) && (timeGetTime()-ShipOffset)/100>1400)
         ShipOffset=timeGetTime()+rand()%5000+2000;

      if (BlinkArrowsTimer && timeGetTime()-BlinkArrowsTimer<5000)
      {
         if ((timeGetTime()-BlinkArrowsTimer)%1000<500)
         {
            RoomBm.BlitFromT (gCursorLBm, 0, 220);
            RoomBm.BlitFromT (gCursorRBm, 640-gCursorRBm.Size.x, 220);
         }
      }
   }

   //Ggf. Onscreen-Texte einbauen:
   CStdRaum::InitToolTips ();

   if (Sim.Difficulty>=ROOM_LIMIT)
   {
	   if (!IsDialogOpen() && !MenuIsOpen() && !Sim.bPause)
	   {
		   if (gMousePosition.x<=10  && ViewPos.x>0)   SetMouseLook (CURSOR_LEFT,  0, ROOM_INSEL, 6010);
		   if (gMousePosition.x>=630 && ViewPos.x<640) SetMouseLook (CURSOR_RIGHT, 0, ROOM_INSEL, 6011);
	   }

      //Command&Conquer-Scrolling:
	   if (MouseClickArea==ROOM_INSEL && MouseClickId==6010)
      {
         if (gMouseScrollSpeed>-44-(gMouseLButton-1)*30) gMouseScrollSpeed-=2;
         if (gMouseScrollSpeed<-66) gMouseScrollSpeed=-66;
         gMouseScroll=TRUE;
      }
	   else if (MouseClickArea==ROOM_INSEL && MouseClickId==6011)
	   {
         if (gMouseScrollSpeed<44+(gMouseLButton-1)*30) gMouseScrollSpeed+=2;
         if (gMouseScrollSpeed>66) gMouseScrollSpeed=66;
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
         if (ViewPos.x<0)   ViewPos.x=0;
         if (ViewPos.x>640) ViewPos.x=640;
	   }
      else gMouseScrollSpeed=0;
   }

   if (!IsDialogOpen() && !MenuIsOpen())
   {
      if (gMousePosition.IfIsWithin (0,0,100,50) ||      gMousePosition.IfIsWithin (0,0,50,100) ||
          gMousePosition.IfIsWithin (0,390,100,440) ||   gMousePosition.IfIsWithin (0,340,50,440) ||
          gMousePosition.IfIsWithin (590,0,640,100) ||   gMousePosition.IfIsWithin (540,0,640,50) ||
          gMousePosition.IfIsWithin (590,340,640,440) || gMousePosition.IfIsWithin (540,390,640,440))
         SetMouseLook (CURSOR_EXIT, 0, ROOM_INSEL, 999);
   }

   CStdRaum::PostPaint ();
   CStdRaum::PumpToolTips ();
#endif
}

//--------------------------------------------------------------------------------------------
//void CInsel::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CInsel::OnLButtonDown(UINT nFlags, CPoint point)
{
#ifndef DEMO
   XY RoomPos;

   DefaultOnLButtonDown ();

   if (!ConvertMousePosition (point, &RoomPos))
   {
	   CStdRaum::OnLButtonDown(nFlags, point);
      return;
   }

   if (!PreLButtonDown (point))
   {
      if (MouseClickArea==ROOM_INSEL && MouseClickId==999) Sim.Players.Players[(SLONG)PlayerNum].LeaveRoom();
      //else if (MouseClickArea==ROOM_SABOTAGE && MouseClickId==10) StartDialog (TALKER_SABOTAGE, MEDIUM_AIR, 0);
      else CStdRaum::OnLButtonDown(nFlags, point);
   }
#endif
}

//--------------------------------------------------------------------------------------------
//void CInsel::OnRButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void CInsel::OnRButtonDown(UINT nFlags, CPoint point)
{
#ifndef DEMO
   DefaultOnRButtonDown ();

   //Auﬂerhalb geklickt? Dann Default-Handler!
   if (point.x<WinP1.x || point.y<WinP1.y || point.x>WinP2.x || point.y>WinP2.y)
   {
      //CWnd::OnRButtonDown(nFlags, point);
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
#endif
}
