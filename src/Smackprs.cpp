//============================================================================================
// SmackPrs.cpp : Wrapper-Klasse für den Smacker für 16Bit Farben bei 8-Bit Videos:
//============================================================================================
#include "stdafx.h"

//Zum debuggen:
static const char FileId[] = "Smak";

//--------------------------------------------------------------------------------------------
//Berechnet eine Remapper-Tabelle anhand einer 256-Farben Palette
//--------------------------------------------------------------------------------------------
void CalculatePalettemapper (UBYTE *pPalette, UWORD *pPaletteMapper)
{
   SLONG c;

   for (c=0; c<256; c++)
   {
      pPaletteMapper[c] = (UWORD)gCursorBm.pBitmap->GetHardwarecolor ((pPalette[c*3]<<16)+(pPalette[c*3+1]<<8)+(pPalette[c*3+2]));
      if (pPaletteMapper[c]==0) pPaletteMapper[c]=1;
   }

   pPaletteMapper[0]=0;
}

//--------------------------------------------------------------------------------------------
//Konvertiert eine halb beschriebene 8-Bit Bitmap in eine mit 16-Bit:
//--------------------------------------------------------------------------------------------
void ConvertBitmapTo16Bit (UBYTE *SourcePic, SBBM *pBitmap, UWORD *pPaletteMapper, SLONG SmackWidth, SLONG SourceSizeY, XY TargetOffset)
{
   if (NULL==pBitmap || NULL==pBitmap->pBitmap) return;

   SLONG register y;
   SB_CBitmapKey  Key(*pBitmap->pBitmap);

   static _EBP;

   if (Key.Bitmap==NULL) return;

   UWORD *p = ((UWORD*)Key.Bitmap) + TargetOffset.x + TargetOffset.y*Key.lPitch/2;

   if (SmackWidth<=0 || SmackWidth>32000) return;

   for (y=SourceSizeY; y>0; y--)
   {
      _asm
      {
         push  esi
         push  edi

         mov   ecx, SmackWidth

         xor   eax, eax
         xor   edx, edx
         mov   esi, pPaletteMapper
         mov   edi, p
         mov   _EBP, ebp
         mov   ebp, SourcePic
         sub   edi, 2
         dec   ebp

         cmp   ecx, 16
         jl    final_loop_entry

      big_loop:
         ; Pixel 1 & 2
         mov   al, BYTE PTR [ebp+ecx]
         mov   ebx, [esi+eax*2-2]
         mov   dl, BYTE PTR [ebp+ecx-1]
         mov   bx, [esi+edx*2]
         mov   [edi+ecx*2-2], ebx

         ; Pixel 2 & 3
         mov   al, BYTE PTR [ebp+ecx-2]
         mov   ebx, [esi+eax*2-2]
         mov   dl, BYTE PTR [ebp+ecx-3]
         mov   bx, [esi+edx*2]
         mov   [edi+ecx*2-6], ebx

         ; Pixel 4 & 5
         mov   al, BYTE PTR [ebp+ecx-4]
         mov   ebx, [esi+eax*2-2]
         mov   dl, BYTE PTR [ebp+ecx-5]
         mov   bx, [esi+edx*2]
         mov   [edi+ecx*2-10], ebx

         ; Pixel 6 & 7
         mov   al, BYTE PTR [ebp+ecx-6]
         mov   ebx, [esi+eax*2-2]
         mov   dl, BYTE PTR [ebp+ecx-7]
         mov   bx, [esi+edx*2]
         mov   [edi+ecx*2-14], ebx

         ; Pixel 8 & 9
         mov   al, BYTE PTR [ebp+ecx-8]
         mov   ebx, [esi+eax*2-2]
         mov   dl, BYTE PTR [ebp+ecx-9]
         mov   bx, [esi+edx*2]
         mov   [edi+ecx*2-18], ebx

         ; Pixel 10 & 11
         mov   al, BYTE PTR [ebp+ecx-10]
         mov   ebx, [esi+eax*2-2]
         mov   dl, BYTE PTR [ebp+ecx-11]
         mov   bx, [esi+edx*2]
         mov   [edi+ecx*2-22], ebx

         ; Pixel 12 & 13
         mov   al, BYTE PTR [ebp+ecx-12]
         mov   ebx, [esi+eax*2-2]
         mov   dl, BYTE PTR [ebp+ecx-13]
         mov   bx, [esi+edx*2]
         mov   [edi+ecx*2-26], ebx

         ; Pixel 14 & 15
         mov   al, BYTE PTR [ebp+ecx-14]
         mov   ebx, [esi+eax*2-2]
         mov   dl, BYTE PTR [ebp+ecx-15]
         mov   bx, [esi+edx*2]
         mov   [edi+ecx*2-30], ebx

         sub   ecx, 16
         cmp   ecx, 16
         jge   big_loop

      final_loop_entry:
         cmp   ecx, 0
         jle   exit_loop

      final_loop:
         mov   al, BYTE PTR [ebp+ecx]
         mov   bx, [esi+eax*2]
         mov   WORD PTR [edi+ecx*2], bx

         dec   ecx
         jnz   final_loop

      exit_loop:
         mov   ebp, _EBP

         pop   edi
         pop   esi
      }

      /*for (x=SmackWidth-1; x>=0; x--)
         p[x] = pPaletteMapper[SourcePic[x]]; */

      p+=Key.lPitch/2;
      SourcePic+=SmackWidth;
   }
   //for (c=Anz-1; c>=0; c--)
   //   ((UWORD*)p)[c] = pPaletteMapper[SourcePic[c]];
}

//--------------------------------------------------------------------------------------------
//CSmack16::
//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
CSmack16::CSmack16 ()
{
   pSmack = NULL;
}

//--------------------------------------------------------------------------------------------
//Destruktor:
//--------------------------------------------------------------------------------------------
CSmack16::~CSmack16 ()
{
   if (pSmack) SmackClose (pSmack);
   pSmack = NULL;
}

//--------------------------------------------------------------------------------------------
//Öffnet ein Smacker-Filmchen:
//--------------------------------------------------------------------------------------------
void CSmack16::Open (CString Filename)
{
   pSmack = SmackOpen (FullFilename (Filename, SmackerPath), SMACKTRACKS, SMACKAUTOEXTRA);
   SmackPic.ReSize (pSmack->Width*pSmack->Height);
   CalculatePalettemapper (pSmack->Palette, PaletteMapper+1);
}

//--------------------------------------------------------------------------------------------
//Nächster Frame:
//--------------------------------------------------------------------------------------------
BOOL CSmack16::Next (SBBM *pTargetBm)
{
   if (!SmackWait (pSmack))
   {
      //Take the next frame:
      SmackToBuffer (pSmack, 0, 0, pSmack->Width, pSmack->Height, (UBYTE*)SmackPic, FALSE);
      SmackDoFrame (pSmack);
      SmackNextFrame (pSmack);

      if (pTargetBm)
      {
         if (SLONG(pSmack->Width)!=pTargetBm->Size.x || SLONG(pSmack->Height)!=pTargetBm->Size.y)
            pTargetBm->ReSize (pSmack->Width, pSmack->Height);
         ConvertBitmapTo16Bit ((UBYTE*)SmackPic, pTargetBm, PaletteMapper+1, pSmack->Width, pSmack->Height, XY(0,0));
      }
   }

   return (pSmack->FrameNum<pSmack->Frames-1);
}

//--------------------------------------------------------------------------------------------
//Und Feierabend:
//--------------------------------------------------------------------------------------------
void CSmack16::Close (void)
{
   if (pSmack) SmackClose (pSmack);
   pSmack = NULL;
}

//--------------------------------------------------------------------------------------------
//CSmackerClip::
//--------------------------------------------------------------------------------------------
//Konstruktor:
//--------------------------------------------------------------------------------------------
CSmackerClip::CSmackerClip ()
{
   pSmack = NULL;
   LastFrame = 0;
}

//--------------------------------------------------------------------------------------------
//Destruktor:
//--------------------------------------------------------------------------------------------
CSmackerClip::~CSmackerClip ()
{
   if (pSmack) SmackClose (pSmack);
   pSmack = NULL;
}

//--------------------------------------------------------------------------------------------
//Startet die Animation
//--------------------------------------------------------------------------------------------
void CSmackerClip::Start (void)
{
   if (pSmack) SmackClose (pSmack);
   pSmack = NULL;

   if (Filename.GetLength()>0)
   {
      pSmack = SmackOpen (FullFilename (Filename, SmackerPath), SMACKTRACKS, SMACKAUTOEXTRA);
      SmackPic.ReSize (pSmack->Width*pSmack->Height);
      CalculatePalettemapper (pSmack->Palette, PaletteMapper+1);
   }

   if (!IsFXPlaying)
   {
      if (SoundFilename.GetLength()>0)
      {
         if (strchr (SoundFilename, '|'))
         {
            char  *p=(char*)(LPCTSTR)SoundFilename;

            NumSoundFx = 1;
            while (1)
            {
               p=strchr (p+1, '|');

               if (p) NumSoundFx++; else break;
            }

            NextSyllable ();
         }
         else
         {
            NumSoundFx = 1;
            SoundFx.ReInit (SoundFilename, (char*)(LPCTSTR)SmackerPath);
         }
      }
      else NumSoundFx = 0;
   }

   RepeatCount = Repeat.x + rand ()%(Repeat.y-Repeat.x+1);
}

//--------------------------------------------------------------------------------------------
//Beendet sie:
//--------------------------------------------------------------------------------------------
void CSmackerClip::Stop (void)
{
   if (pSmack) SmackClose (pSmack);
   pSmack = NULL;

   LastFrame = 0;

   SmackPic.ReSize (0);
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
void CSmackerClip::ReSize (SLONG          ClipId,
                           const CString &Filename,
                           const CString &SoundFilename,

                           XY             ScreenOffset,
                           SLONG          MoodId,
                           CRepeat        Repeat,                 //Min..Max
                           CPostWait      PostWait,               //Min..Max in 20/stel Sekunden
                           BOOL           CanCancelClip,          //Kann der Clip vorzeitig abgebrochen werden?
                           
                           SLONG         *PostVar,                //NULL oder Variable, die am Animationsende gesetzt werden soll
                           SLONG          PostOperation,
                           SLONG          PostValue,
                           
                           const SLONG   *DecisionVar,            //wenn !=NULL, legt sie fest, welche Folgeanimation gespielt wird...
                           
                           CString        SuccessorTokens,        //z.B. "A2X8"...
                           SLONG          SuccessorIds, ...)
{
   SLONG c;

   if (pSmack) SmackClose (pSmack);

   CSmackerClip::Filename      = Filename;
   CSmackerClip::SoundFilename = SoundFilename;
   CSmackerClip::pSmack    = NULL;
   CSmackerClip::LastFrame = 0;

   //Statische Eigenschaften...
   CSmackerClip::ClipId        = ClipId;
   CSmackerClip::ScreenOffset  = ScreenOffset;
   CSmackerClip::MoodId        = MoodId;
   CSmackerClip::Repeat        = Repeat;
   CSmackerClip::PostWait      = PostWait;
   CSmackerClip::CanCancelClip = CanCancelClip;

   CSmackerClip::PostVar       = PostVar;
   CSmackerClip::PostOperation = PostOperation;
   CSmackerClip::PostValue     = PostValue;
   CSmackerClip::DecisionVar   = DecisionVar;

   CSmackerClip::SuccessorTokens = SuccessorTokens;
   CSmackerClip::SuccessorIds.ReSize (SuccessorTokens.GetLength()/2);

   CSmackerClip::SuccessorIds[0] = SuccessorIds;

   //Hilfskonstruktion für beliebige viele Argumente deklarieren:
   {
      va_list  Vars;

      //Tabelle initialisieren:
      va_start (Vars, SuccessorIds);

      for (c=1; c<CSmackerClip::SuccessorIds.AnzEntries(); c++)
         CSmackerClip::SuccessorIds[c] = va_arg (Vars, SLONG);

      //Daten bereinigen:
      va_end (Vars);
   }

   CSmackerClip::IsFXPlaying = FALSE;

   //Dynamische Eigenschaften...
   CSmackerClip::State       = SMACKER_CLIP_INACTIVE;
   CSmackerClip::RepeatCount = 0;
   CSmackerClip::WaitCount   = 0;
}

//--------------------------------------------------------------------------------------------
//Läßt die Person die nächste Silbe raussuchen:
//--------------------------------------------------------------------------------------------
void CSmackerClip::NextSyllable (void)
{
   if (SoundFilename.GetLength()>0)
   {
      char *p=(char*)(LPCTSTR)SoundFilename;

      if (NumSoundFx<3)
      {
         for (SLONG n=rand()%NumSoundFx; n>0; n--)
            p=strchr (p, '|')+1;
      }
      else
      {
         for (SLONG n=SoundRandom.Rand (0, NumSoundFx-1); n>0; n--)
            p=strchr (p, '|')+1;
      }

      char buffer[200];

      strncpy (buffer, p, 200);
      char *pp=strchr (buffer, '|');

      if (pp) *pp=0;

      SoundFx.ReInit (buffer, (char*)(LPCTSTR)SmackerPath);
   }
}

//--------------------------------------------------------------------------------------------
//Läßt die Person die nächste Silbe sprechen:
//--------------------------------------------------------------------------------------------
void CSmackerClip::PlaySyllable (void)
{
   if (SoundFx.pFX)
   {
      SoundFx.Play (0, Sim.Options.OptionEffekte*100/7);
      TickerNext=timeGetTime()+SoundFx.pFX->GetByteLength()*1000/22000;
      IsFXPlaying = TRUE;
   }
   else
      TickerNext=0;
}
 
//--------------------------------------------------------------------------------------------
//CSmackerPerson::
//--------------------------------------------------------------------------------------------
////Konstruktor:
//--------------------------------------------------------------------------------------------
CSmackerPerson::CSmackerPerson ()
{
   ActiveClip = -1;
   CurrentMood = -1;
   DesiredMood = -1;
   AcceptedMood1 = -1;
   AcceptedMood2 = -1;
}

//--------------------------------------------------------------------------------------------
//Destruktor:
//--------------------------------------------------------------------------------------------
CSmackerPerson::~CSmackerPerson ()
{
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
void CSmackerPerson::ReSize (SLONG NumberOfClips)
{
   Clips.ReSize (NumberOfClips);
}

//--------------------------------------------------------------------------------------------
//Legt fest, wie die Person labert:
//--------------------------------------------------------------------------------------------
void CSmackerPerson::SetSpeakFx (CString Filename)
{
   SpeakFx.ReInit (Filename, (char*)(LPCTSTR)SmackerPath);
}

//--------------------------------------------------------------------------------------------
//Legt die Stimmung (Freizeit, Reden, Zuöhren) fest, in der man jemand haben will
//--------------------------------------------------------------------------------------------
void CSmackerPerson::SetDesiredMood (SLONG DesiredMood, SLONG AcceptedMood1, SLONG AcceptedMood2)
{
   CSmackerPerson::DesiredMood   = DesiredMood;
   CSmackerPerson::AcceptedMood1 = AcceptedMood1;
   CSmackerPerson::AcceptedMood2 = AcceptedMood2;
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
SLONG CSmackerPerson::GetMood (void)
{
   return (CurrentMood);
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
SLONG CSmackerPerson::GetDesiredMood (void)
{
   return (DesiredMood);
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
SLONG CSmackerPerson::GetClip (void)
{
   return (ActiveClip);
}

//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
SLONG CSmackerPerson::GetFrame (void)
{
   if (ActiveClip!=-1 && Clips[ActiveClip].pSmack)
      return (Clips[ActiveClip].pSmack->FrameNum);
   else
      return (NULL);
}

//--------------------------------------------------------------------------------------------
//Beginnt den nächsten Clip, selbst wenn es verboten ist:
//--------------------------------------------------------------------------------------------
void CSmackerPerson::ForceNextClip (void)
{
   Clips[ActiveClip].Stop();
   Clips[ActiveClip].State=SMACKER_CLIP_INACTIVE;
   NextClip();
}

//--------------------------------------------------------------------------------------------
//Aktualisiert eine Animation
//--------------------------------------------------------------------------------------------
void CSmackerPerson::Pump (void)
{
   if (Clips.AnzEntries()==0) return;

   if (ActiveClip!=-1 && Clips[ActiveClip].IsFXPlaying && Clips[ActiveClip].NumSoundFx>1 && timeGetTime()>Clips[ActiveClip].TickerNext)
   {
      Clips[ActiveClip].NextSyllable ();
      Clips[ActiveClip].PlaySyllable ();
   }

   //Wenn gerade ein Leerclip abgespielt wird:
   if (ActiveClip!=-1 && Clips[ActiveClip].pSmack==NULL)
   {
      Clips[ActiveClip].Stop();
      Clips[ActiveClip].State=SMACKER_CLIP_INACTIVE;
      NextClip();

      if (Clips[ActiveClip].pSmack!=NULL && ActiveClip!=-1)
      {
         Bitmap.ReSize (Clips[ActiveClip].pSmack->Width, Clips[ActiveClip].pSmack->Height);
         Clips[ActiveClip].SmackPic.ReSize (Clips[ActiveClip].pSmack->Width*Clips[ActiveClip].pSmack->Height);
         SmackToBuffer (Clips[ActiveClip].pSmack, 0, 0, Bitmap.Size.x, Bitmap.Size.y, (UBYTE*)Clips[ActiveClip].SmackPic, FALSE);
         SmackDoFrame (Clips[ActiveClip].pSmack);
         ConvertBitmapTo16Bit ((UBYTE*)Clips[ActiveClip].SmackPic, &Bitmap, Clips[ActiveClip].PaletteMapper+1, Clips[ActiveClip].pSmack->Width, Clips[ActiveClip].pSmack->Height, XY(0,0));
         BitmapPos = Clips[ActiveClip].ScreenOffset;

         if (Clips[ActiveClip].pSmack->FrameNum==0 && !Clips[ActiveClip].IsFXPlaying)
            Clips[ActiveClip].PlaySyllable ();

         SmackNextFrame (Clips[ActiveClip].pSmack);
      }

      return;
   }

   if (ActiveClip==-1)
   {
      ActiveClip  = 0;
      if (CurrentMood==-1) CurrentMood=Clips[ActiveClip].MoodId;
      if (DesiredMood==-1) DesiredMood=Clips[ActiveClip].MoodId;

      Clips[ActiveClip].State   = SMACKER_CLIP_PLAYING;
      Clips[ActiveClip].Start ();

      if (Clips[ActiveClip].pSmack==NULL)
         return;

      Bitmap.ReSize (Clips[ActiveClip].pSmack->Width, Clips[ActiveClip].pSmack->Height);
      Clips[ActiveClip].SmackPic.ReSize (Clips[ActiveClip].pSmack->Width*Clips[ActiveClip].pSmack->Height);
      SmackToBuffer (Clips[ActiveClip].pSmack, 0, 0, Bitmap.Size.x, Bitmap.Size.y, (UBYTE*)Clips[ActiveClip].SmackPic, FALSE);
      SmackDoFrame (Clips[ActiveClip].pSmack);
      ConvertBitmapTo16Bit ((UBYTE*)Clips[ActiveClip].SmackPic, &Bitmap, Clips[ActiveClip].PaletteMapper+1, Clips[ActiveClip].pSmack->Width, Clips[ActiveClip].pSmack->Height, XY(0,0));
      BitmapPos = Clips[ActiveClip].ScreenOffset;

      if (Clips[ActiveClip].pSmack->FrameNum==0 && !Clips[ActiveClip].IsFXPlaying)
         Clips[ActiveClip].PlaySyllable ();

      SmackNextFrame (Clips[ActiveClip].pSmack);
   }

   if (Clips[ActiveClip].CanCancelClip && (CurrentMood!=DesiredMood || (Clips[ActiveClip].DecisionVar && Clips[ActiveClip].DecisionVar[0]!=-1)))
   {
      Clips[ActiveClip].Stop();
      Clips[ActiveClip].State=SMACKER_CLIP_INACTIVE;
      NextClip();

      return;
   }

   if (Clips[ActiveClip].State==SMACKER_CLIP_PLAYING)
   {
      if (!SmackWait (Clips[ActiveClip].pSmack))
      {
         //Take the next frame:
         Bitmap.ReSize (Clips[ActiveClip].pSmack->Width, Clips[ActiveClip].pSmack->Height);
         Clips[ActiveClip].SmackPic.ReSize (Clips[ActiveClip].pSmack->Width*Clips[ActiveClip].pSmack->Height);
         SmackToBuffer (Clips[ActiveClip].pSmack, 0, 0, Bitmap.Size.x, Bitmap.Size.y, (UBYTE*)Clips[ActiveClip].SmackPic, FALSE);
         SmackDoFrame (Clips[ActiveClip].pSmack);
         ConvertBitmapTo16Bit ((UBYTE*)Clips[ActiveClip].SmackPic, &Bitmap, Clips[ActiveClip].PaletteMapper+1, Clips[ActiveClip].pSmack->Width, Clips[ActiveClip].pSmack->Height, XY(0,0));
         BitmapPos = Clips[ActiveClip].ScreenOffset;

         //Variablenveränderung, während der Film läuft?
         if (Clips[ActiveClip].PostVar && (Clips[ActiveClip].PostOperation&SMACKER_CLIP_FRAME)) //Variablen-Messageing:
         {
            if (Clips[ActiveClip].pSmack->FrameNum >= ULONG(Clips[ActiveClip].PostOperation>>13) &&
                Clips[ActiveClip].LastFrame < (Clips[ActiveClip].PostOperation>>13))

            switch (Clips[ActiveClip].PostOperation&1023)
            {
               case SMACKER_CLIP_ADD: Clips[ActiveClip].PostVar[0] += Clips[ActiveClip].PostValue; break;
               case SMACKER_CLIP_SUB: Clips[ActiveClip].PostVar[0] -= Clips[ActiveClip].PostValue; break;
               case SMACKER_CLIP_SET: Clips[ActiveClip].PostVar[0]  = Clips[ActiveClip].PostValue; break;
               case SMACKER_CLIP_XOR: Clips[ActiveClip].PostVar[0] ^= Clips[ActiveClip].PostValue; break;
            }
         }

         Clips[ActiveClip].LastFrame = Clips[ActiveClip].pSmack->FrameNum;

         if (Clips[ActiveClip].pSmack->FrameNum >= Clips[ActiveClip].pSmack->Frames-1)
         {
            Clips[ActiveClip].RepeatCount--;

            if (CurrentMood!=DesiredMood) Clips[ActiveClip].RepeatCount=0;

            if (Clips[ActiveClip].RepeatCount>0)
               if (CurrentMood!=SPM_TALKING && CurrentMood!=SPM_ANGRY_TALKING) Clips[ActiveClip].PlaySyllable ();
         }

         if (Clips[ActiveClip].RepeatCount==0)
         {
            //End of Animation...
            if (Clips[ActiveClip].PostWait.y>0)
            {
               Clips[ActiveClip].WaitCount = (Clips[ActiveClip].PostWait.x + rand ()%(Clips[ActiveClip].PostWait.y-Clips[ActiveClip].PostWait.x+1))*50 + timeGetTime();
               Clips[ActiveClip].State=SMACKER_CLIP_WAITING;
            }
            else
            {
               if (CurrentMood!=DesiredMood || (CurrentMood!=SPM_TALKING && CurrentMood!=SPM_ANGRY_TALKING))
               {
                  Clips[ActiveClip].SoundFx.Stop();
                  Clips[ActiveClip].IsFXPlaying=FALSE;
               }

               Clips[ActiveClip].Stop();
               Clips[ActiveClip].State=SMACKER_CLIP_INACTIVE;
               NextClip();
            }
         }
         else
         {
            if (Clips[ActiveClip].pSmack->FrameNum==0 && !Clips[ActiveClip].IsFXPlaying)
               Clips[ActiveClip].PlaySyllable ();

            SmackNextFrame (Clips[ActiveClip].pSmack);
         }
      }
   }
   else if (Clips[ActiveClip].State==SMACKER_CLIP_WAITING)
   {
       if (SLONG(timeGetTime())>Clips[ActiveClip].WaitCount)
       {
          Clips[ActiveClip].Stop();
          NextClip();
       }
   }
}

//--------------------------------------------------------------------------------------------
//Sucht den Folgeclip heraus:
//--------------------------------------------------------------------------------------------
void CSmackerPerson::NextClip (void)
{
   SLONG c;
   SLONG PropSum;

   if (Clips.AnzEntries()==0) return;

   if (Clips[ActiveClip].PostVar && (Clips[ActiveClip].PostOperation&SMACKER_CLIP_POST)) //Variablen-Messageing:
   {
      switch (Clips[ActiveClip].PostOperation&1023)
      {
         case SMACKER_CLIP_ADD: Clips[ActiveClip].PostVar[0] += Clips[ActiveClip].PostValue; break;
         case SMACKER_CLIP_SUB: Clips[ActiveClip].PostVar[0] -= Clips[ActiveClip].PostValue; break;
         case SMACKER_CLIP_SET: Clips[ActiveClip].PostVar[0]  = Clips[ActiveClip].PostValue; break;
         case SMACKER_CLIP_XOR: Clips[ActiveClip].PostVar[0] ^= Clips[ActiveClip].PostValue; break;
      }
   }

   //Erste Priorität hat immer eine "DecisionVar"
   if (Clips[ActiveClip].DecisionVar && Clips[ActiveClip].DecisionVar[0]!=-1)
   {
      ActiveClip = Clips[ActiveClip].DecisionVar[0];
      goto found_next_clip;
   }
   else //Ansonsten entscheiden wir anhand von Zufall und Regeln
   {
      if (CurrentMood==DesiredMood)
      {
         //1. Schauen, wie's weitergeht ohen die Stimmung zu ändern:
         //---------------------------------------------------------------------------
         PropSum=0; //Wahrscheinlichkeiten aufsummieren:
         for (c=0; c<Clips[ActiveClip].SuccessorIds.AnzEntries(); c++)
            if (Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==DesiredMood && Clips[ActiveClip].SuccessorTokens[c*2]!='E')
               PropSum+=(Clips[ActiveClip].SuccessorTokens[c*2+1]-'0');

         if (PropSum)
         {
            PropSum=rand()%PropSum;

            //Alternative auswählten:
            for (c=0; c<Clips[ActiveClip].SuccessorIds.AnzEntries(); c++)
               if (Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==DesiredMood && Clips[ActiveClip].SuccessorTokens[c*2]!='E')
               {
                  PropSum-=(Clips[ActiveClip].SuccessorTokens[c*2+1]-'0');
                  if (PropSum<0)
                  {
                     ActiveClip=Clips[ActiveClip].SuccessorIds [c];
                     goto found_next_clip;
                  }
               }
         }

         //2. Unmöglich, rettet bei Bugs vor dem Absturz:
         //---------------------------------------------------------------------------
         PropSum=0; //Wahrscheinlichkeiten aufsummieren:
         for (c=0; c<Clips[ActiveClip].SuccessorIds.AnzEntries(); c++)
            PropSum+=(Clips[ActiveClip].SuccessorTokens[c*2+1]-'0');

         PropSum=rand()%PropSum;

         //Alternative auswählten:
         for (c=0; c<Clips[ActiveClip].SuccessorIds.AnzEntries(); c++)
         {
            PropSum-=(Clips[ActiveClip].SuccessorTokens[c*2+1]-'0');
            if (PropSum<0)
            {
               ActiveClip=Clips[ActiveClip].SuccessorIds [c];
               goto found_next_clip;
            }
         }
      }
      else //Die Stimmung wechseln:
      {
         //1. Probieren, ob es Exits in die richtige Richtung gibt:
         //---------------------------------------------------------------------------
         PropSum=0; //Wahrscheinlichkeiten aufsummieren:
         for (c=0; c<Clips[ActiveClip].SuccessorIds.AnzEntries(); c++)
            if (Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==DesiredMood && Clips[ActiveClip].SuccessorTokens[c*2]=='E')
               PropSum+=(Clips[ActiveClip].SuccessorTokens[c*2+1]-'0');

         if (PropSum)
         {
            PropSum=rand()%PropSum;

            //Alternative auswählten:
            for (c=0; c<Clips[ActiveClip].SuccessorIds.AnzEntries(); c++)
               if (Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==DesiredMood && Clips[ActiveClip].SuccessorTokens[c*2]=='E')
               {
                  PropSum-=(Clips[ActiveClip].SuccessorTokens[c*2+1]-'0');
                  if (PropSum<0)
                  {
                     ActiveClip=Clips[ActiveClip].SuccessorIds[c];
                     goto found_next_clip;
                  }
               }
         }

         //2. Probieren, ob es Exits in andere, akzeptierte Richtungen gibt:
         //---------------------------------------------------------------------------
         PropSum=0; //Wahrscheinlichkeiten aufsummieren:
         for (c=0; c<Clips[ActiveClip].SuccessorIds.AnzEntries(); c++)
            if ((Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==DesiredMood ||
                 Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==CurrentMood ||
                 Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==AcceptedMood1 ||
                 Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==AcceptedMood2) &&
                    Clips[ActiveClip].SuccessorTokens[c*2]=='E')
               PropSum+=(Clips[ActiveClip].SuccessorTokens[c*2+1]-'0');

         if (PropSum)
         {
            PropSum=rand()%PropSum;

            //Alternative auswählten:
            for (c=0; c<Clips[ActiveClip].SuccessorIds.AnzEntries(); c++)
               if ((Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==DesiredMood ||
                    Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==CurrentMood ||
                    Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==AcceptedMood1 ||
                    Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==AcceptedMood2) &&
                       Clips[ActiveClip].SuccessorTokens[c*2]=='E')
               {
                  PropSum-=(Clips[ActiveClip].SuccessorTokens[c*2+1]-'0');
                  if (PropSum<0)
                  {
                     ActiveClip=Clips[ActiveClip].SuccessorIds [c];
                     goto found_next_clip;
                  }
               }
         }

         //3. Probieren, ob es eine direkte Verbindung zur ZielMood gibt:
         //---------------------------------------------------------------------------
         PropSum=0; //Wahrscheinlichkeiten aufsummieren:
         for (c=0; c<Clips[ActiveClip].SuccessorIds.AnzEntries(); c++)
            if (Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==DesiredMood && Clips[ActiveClip].SuccessorTokens[c*2]!='S')
               PropSum+=(Clips[ActiveClip].SuccessorTokens[c*2+1]-'0');

         if (PropSum)
         {
            PropSum=rand()%PropSum;

            //Alternative auswählten:
            for (c=0; c<Clips[ActiveClip].SuccessorIds.AnzEntries(); c++)
               if (Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==DesiredMood && Clips[ActiveClip].SuccessorTokens[c*2]!='S')
               {
                  PropSum-=(Clips[ActiveClip].SuccessorTokens[c*2+1]-'0');
                  if (PropSum<0)
                  {
                     ActiveClip=Clips[ActiveClip].SuccessorIds [c];
                     goto found_next_clip;
                  }
               }
         }

         //4. Der Rest:
         //---------------------------------------------------------------------------
         PropSum=0; //Wahrscheinlichkeiten aufsummieren:
         for (c=0; c<Clips[ActiveClip].SuccessorIds.AnzEntries(); c++)
            if ((Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==DesiredMood ||
                 Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==CurrentMood ||
                 Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==AcceptedMood1 ||
                 Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==AcceptedMood2) &&
                    Clips[ActiveClip].SuccessorTokens[c*2]!='S')
               PropSum+=(Clips[ActiveClip].SuccessorTokens[c*2+1]-'0');

         if (PropSum)
         {
            PropSum=rand()%PropSum;

            //Alternative auswählten:
            for (c=0; c<Clips[ActiveClip].SuccessorIds.AnzEntries(); c++)
               if ((Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==DesiredMood ||
                    Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==CurrentMood ||
                    Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==AcceptedMood1 ||
                    Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==AcceptedMood2) &&
                       Clips[ActiveClip].SuccessorTokens[c*2]!='S')
               {
                  PropSum-=(Clips[ActiveClip].SuccessorTokens[c*2+1]-'0');
                  if (PropSum<0)
                  {
                     ActiveClip=Clips[ActiveClip].SuccessorIds [c];
                     goto found_next_clip;
                  }
               }
         }

         //5. Unmöglich, rettet bei Bugs vor dem Absturz:
         //---------------------------------------------------------------------------
         PropSum=0; //Wahrscheinlichkeiten aufsummieren:
         for (c=0; c<Clips[ActiveClip].SuccessorIds.AnzEntries(); c++)
            PropSum+=(Clips[ActiveClip].SuccessorTokens[c*2+1]-'0');

         PropSum=rand()%PropSum;

         //Alternative auswählten:
         for (c=0; c<Clips[ActiveClip].SuccessorIds.AnzEntries(); c++)
         {
            PropSum-=(Clips[ActiveClip].SuccessorTokens[c*2+1]-'0');
            if (PropSum<0)
            {
               ActiveClip=Clips[ActiveClip].SuccessorIds [c];
               goto found_next_clip;
            }
         }
      }
   }
   DebugBreak ();

found_next_clip:
   BOOL LastTalking = (CurrentMood==SPM_TALKING || CurrentMood==SPM_ANGRY_TALKING);
   BOOL Talking     = (Clips[ActiveClip].MoodId==SPM_TALKING || Clips[ActiveClip].MoodId==SPM_ANGRY_TALKING);

   if (Talking && !LastTalking && Sim.Options.OptionTalking*Sim.Options.OptionDigiSound)
      SpeakFx.Play(DSBPLAY_NOSTOP|DSBPLAY_LOOPING, Sim.Options.OptionTalking*100/7);
   else if (!Talking && LastTalking)
      SpeakFx.Stop ();

   CurrentMood = Clips[ActiveClip].MoodId;
   Clips[ActiveClip].State = SMACKER_CLIP_PLAYING;
   Clips[ActiveClip].Start ();

   if (Clips[ActiveClip].PostVar && (Clips[ActiveClip].PostOperation&SMACKER_CLIP_PRE)) //Variablen-Messageing:
   {
      switch (Clips[ActiveClip].PostOperation&1023)
      {
         case SMACKER_CLIP_ADD: Clips[ActiveClip].PostVar[0] += Clips[ActiveClip].PostValue; break;
         case SMACKER_CLIP_SUB: Clips[ActiveClip].PostVar[0] -= Clips[ActiveClip].PostValue; break;
         case SMACKER_CLIP_SET: Clips[ActiveClip].PostVar[0]  = Clips[ActiveClip].PostValue; break;
         case SMACKER_CLIP_XOR: Clips[ActiveClip].PostVar[0] ^= Clips[ActiveClip].PostValue; break;
      }
   }

   if (Clips[ActiveClip].pSmack==NULL && Clips[ActiveClip].PostWait.x==0 && Clips[ActiveClip].PostWait.y==0) NextClip ();
}

//--------------------------------------------------------------------------------------------
//Blittet eine Person in eine Bitmap:
//--------------------------------------------------------------------------------------------
void CSmackerPerson::BlitAtT (SBBM &RoomBm, XY Offset)
{
    if (ActiveClip!=-1 && Clips[ActiveClip].pSmack)
    {
       RoomBm.BlitFromT (Bitmap, BitmapPos+Offset);
    }
}

//--------------------------------------------------------------------------------------------
//Blittet eine Person in eine Bitmap:
//--------------------------------------------------------------------------------------------
void CSmackerPerson::BlitAtT (SBBM &RoomBm)
{
   BlitAtT (RoomBm, XY(0,0));
}

