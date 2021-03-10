//============================================================================================
// SmackPrs.cpp : Wrapper-Klasse f�r den Smacker f�r 16Bit Farben bei 8-Bit Videos:
//============================================================================================
#include "stdafx.h"
#include <smacker.h>

//Zum debuggen:
static const char FileId[] = "Smak";

//--------------------------------------------------------------------------------------------
//Berechnet eine Remapper-Tabelle anhand einer 256-Farben Palette
//--------------------------------------------------------------------------------------------
void CalculatePalettemapper (const UBYTE *pPalette, SDL_Palette *pPaletteMapper)
{
   if (!pPalette)
      return;

   SDL_Color colors[256];

   for (SLONG c = 0; c < 256; c++)
   {
      if (pPalette[c*3] + pPalette[c*3+1] + pPalette[c*3+2] == 0)
         colors[c] = SDL_Color{ 4, 4, 4, 0xFF };
      else
         colors[c] = SDL_Color{ pPalette[c*3], pPalette[c*3+1], pPalette[c*3+2], 0xFF };
   }

   colors[0] = SDL_Color{ 0 };
   SDL_SetPaletteColors(pPaletteMapper, colors, 0, 256);
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
   SDL_FreePalette(PaletteMapper);
   if (pSmack) smk_close (pSmack);
   pSmack = NULL;
}

//--------------------------------------------------------------------------------------------
//�ffnet ein Smacker-Filmchen:
//--------------------------------------------------------------------------------------------
void CSmack16::Open (CString Filename)
{
   pSmack = smk_open_file(FullFilename (Filename, SmackerPath), SMK_MODE_MEMORY);
   smk_enable_video(pSmack, true);
   smk_info_video(pSmack, &Width, &Height, NULL);
   FrameNext = 0;
   State = smk_first(pSmack);
   PaletteMapper = SDL_AllocPalette(256);
   CalculatePalettemapper(smk_get_palette(pSmack), PaletteMapper);
}

//--------------------------------------------------------------------------------------------
//N�chster Frame:
//--------------------------------------------------------------------------------------------
BOOL CSmack16::Next (SBBM *pTargetBm)
{
   if (timeGetTime() >= FrameNext && State == SMK_MORE)
   {
      //Take the next frame:
      State = smk_next(pSmack);

      double usf;
      smk_info_all(pSmack, NULL, NULL, &usf);
      FrameNext = timeGetTime() + (usf / 1000.0);

      if (pTargetBm)
      {
         if (SLONG(Width) != pTargetBm->Size.x || SLONG(Height) != pTargetBm->Size.y)
            pTargetBm->ReSize(XY(Width, Height), CREATE_INDEXED);
         SDL_SetPixelFormatPalette(pTargetBm->pBitmap->GetPixelFormat(), PaletteMapper);
         SB_CBitmapKey key(*pTargetBm->pBitmap);
         memcpy(key.Bitmap, smk_get_video(pSmack), key.lPitch * Height);
      }
   }

   return State == SMK_MORE;
}

//--------------------------------------------------------------------------------------------
//Und Feierabend:
//--------------------------------------------------------------------------------------------
void CSmack16::Close (void)
{
   if (pSmack) smk_close (pSmack);
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
   FrameNext = 0;
   LastFrame = 0;
   PaletteMapper = SDL_AllocPalette(256);
}

//--------------------------------------------------------------------------------------------
//Destruktor:
//--------------------------------------------------------------------------------------------
CSmackerClip::~CSmackerClip ()
{
   if (PaletteMapper) SDL_FreePalette(PaletteMapper);
   if (pSmack) smk_close (pSmack);
   pSmack = NULL;
}

//--------------------------------------------------------------------------------------------
//Startet die Animation
//--------------------------------------------------------------------------------------------
void CSmackerClip::Start (void)
{
   if (pSmack) smk_close (pSmack);
   pSmack = NULL;

   if (Filename.GetLength()>0)
   {
      pSmack = smk_open_file(FullFilename (Filename, SmackerPath), SMK_MODE_MEMORY);
      smk_enable_video(pSmack, true);
      smk_info_all(pSmack, &FrameNum, &Frames, NULL);
      smk_info_video(pSmack, &Width, &Height, NULL);
      smk_first(pSmack);
      CalculatePalettemapper(smk_get_palette(pSmack), PaletteMapper);
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
   if (pSmack) smk_close (pSmack);
   pSmack = NULL;
   FrameNext = 0;
   LastFrame = 0;

   //SmackPic.ReSize (0);
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

   if (pSmack) smk_close (pSmack);

   CSmackerClip::Filename      = Filename;
   CSmackerClip::SoundFilename = SoundFilename;
   CSmackerClip::pSmack        = NULL;
   CSmackerClip::LastFrame     = 0;
   CSmackerClip::Width         = 0;
   CSmackerClip::Height        = 0;
   CSmackerClip::FrameNum      = 0;
   CSmackerClip::Frames        = 0;
   CSmackerClip::FrameNext     = 0;

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

   //Hilfskonstruktion f�r beliebige viele Argumente deklarieren:
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
//L��t die Person die n�chste Silbe raussuchen:
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
//L��t die Person die n�chste Silbe sprechen:
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
//Legt die Stimmung (Freizeit, Reden, Zu�hren) fest, in der man jemand haben will
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
      return (Clips[ActiveClip].FrameNum);
   else
      return (NULL);
}

//--------------------------------------------------------------------------------------------
//Beginnt den n�chsten Clip, selbst wenn es verboten ist:
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
         Bitmap.ReSize(XY(Clips[ActiveClip].Width, Clips[ActiveClip].Height), CREATE_INDEXED);
         SDL_SetPixelFormatPalette(Bitmap.pBitmap->GetPixelFormat(), Clips[ActiveClip].PaletteMapper);
         {
            SB_CBitmapKey key(*Bitmap.pBitmap);
            memcpy(key.Bitmap, smk_get_video(Clips[ActiveClip].pSmack), key.lPitch * Clips[ActiveClip].Height);
         }
         BitmapPos = Clips[ActiveClip].ScreenOffset;

         if (Clips[ActiveClip].FrameNum==0 && !Clips[ActiveClip].IsFXPlaying)
            Clips[ActiveClip].PlaySyllable ();

         double usf;
         smk_next(Clips[ActiveClip].pSmack);
         smk_info_all(Clips[ActiveClip].pSmack, &Clips[ActiveClip].FrameNum, &Clips[ActiveClip].Frames, &usf);
         Clips[ActiveClip].FrameNext = timeGetTime() + (usf / 1000.0);
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

      Bitmap.ReSize (XY(Clips[ActiveClip].Width, Clips[ActiveClip].Height), CREATE_INDEXED);
      SDL_SetPixelFormatPalette(Bitmap.pBitmap->GetPixelFormat(), Clips[ActiveClip].PaletteMapper);
      {
         SB_CBitmapKey key(*Bitmap.pBitmap);
         memcpy(key.Bitmap, smk_get_video(Clips[ActiveClip].pSmack), key.lPitch * Clips[ActiveClip].Height);
      }
      BitmapPos = Clips[ActiveClip].ScreenOffset;

      if (Clips[ActiveClip].FrameNum==0 && !Clips[ActiveClip].IsFXPlaying)
         Clips[ActiveClip].PlaySyllable ();

      double usf;
      smk_next(Clips[ActiveClip].pSmack);
      smk_info_all(Clips[ActiveClip].pSmack, &Clips[ActiveClip].FrameNum, &Clips[ActiveClip].Frames, &usf);
      Clips[ActiveClip].FrameNext = timeGetTime() + (usf / 1000.0);
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
      if (timeGetTime() >= Clips[ActiveClip].FrameNext)
      {
         //Take the next frame:
         Bitmap.ReSize (XY(Clips[ActiveClip].Width, Clips[ActiveClip].Height), CREATE_INDEXED);
         SDL_SetPixelFormatPalette(Bitmap.pBitmap->GetPixelFormat(), Clips[ActiveClip].PaletteMapper);
         {
            SB_CBitmapKey key(*Bitmap.pBitmap);
            memcpy(key.Bitmap, smk_get_video(Clips[ActiveClip].pSmack), key.lPitch * Clips[ActiveClip].Height);
         }
         BitmapPos = Clips[ActiveClip].ScreenOffset;

         //Variablenver�nderung, w�hrend der Film l�uft?
         if (Clips[ActiveClip].PostVar && (Clips[ActiveClip].PostOperation&SMACKER_CLIP_FRAME)) //Variablen-Messageing:
         {
            if (Clips[ActiveClip].FrameNum >= ULONG(Clips[ActiveClip].PostOperation>>13) &&
                Clips[ActiveClip].LastFrame < (Clips[ActiveClip].PostOperation>>13))

            switch (Clips[ActiveClip].PostOperation&1023)
            {
               case SMACKER_CLIP_ADD: Clips[ActiveClip].PostVar[0] += Clips[ActiveClip].PostValue; break;
               case SMACKER_CLIP_SUB: Clips[ActiveClip].PostVar[0] -= Clips[ActiveClip].PostValue; break;
               case SMACKER_CLIP_SET: Clips[ActiveClip].PostVar[0]  = Clips[ActiveClip].PostValue; break;
               case SMACKER_CLIP_XOR: Clips[ActiveClip].PostVar[0] ^= Clips[ActiveClip].PostValue; break;
            }
         }

         Clips[ActiveClip].LastFrame = Clips[ActiveClip].FrameNum;

         if (Clips[ActiveClip].FrameNum >= Clips[ActiveClip].Frames-1)
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
            if (Clips[ActiveClip].FrameNum==0 && !Clips[ActiveClip].IsFXPlaying)
               Clips[ActiveClip].PlaySyllable ();

            double usf;
            if (Clips[ActiveClip].FrameNum >= Clips[ActiveClip].Frames - 1)
               smk_first(Clips[ActiveClip].pSmack);
            else
               smk_next(Clips[ActiveClip].pSmack);
            smk_info_all(Clips[ActiveClip].pSmack, &Clips[ActiveClip].FrameNum, &Clips[ActiveClip].Frames, &usf);
            Clips[ActiveClip].FrameNext = timeGetTime() + (usf / 1000.0);
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

   //Erste Priorit�t hat immer eine "DecisionVar"
   if (Clips[ActiveClip].DecisionVar && Clips[ActiveClip].DecisionVar[0]!=-1)
   {
      ActiveClip = Clips[ActiveClip].DecisionVar[0];
      goto found_next_clip;
   }
   else //Ansonsten entscheiden wir anhand von Zufall und Regeln
   {
      if (CurrentMood==DesiredMood)
      {
         //1. Schauen, wie's weitergeht ohen die Stimmung zu �ndern:
         //---------------------------------------------------------------------------
         PropSum=0; //Wahrscheinlichkeiten aufsummieren:
         for (c=0; c<Clips[ActiveClip].SuccessorIds.AnzEntries(); c++)
            if (Clips[Clips[ActiveClip].SuccessorIds[c]].MoodId==DesiredMood && Clips[ActiveClip].SuccessorTokens[c*2]!='E')
               PropSum+=(Clips[ActiveClip].SuccessorTokens[c*2+1]-'0');

         if (PropSum)
         {
            PropSum=rand()%PropSum;

            //Alternative ausw�hlten:
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

         //2. Unm�glich, rettet bei Bugs vor dem Absturz:
         //---------------------------------------------------------------------------
         PropSum=0; //Wahrscheinlichkeiten aufsummieren:
         for (c=0; c<Clips[ActiveClip].SuccessorIds.AnzEntries(); c++)
            PropSum+=(Clips[ActiveClip].SuccessorTokens[c*2+1]-'0');

         PropSum=rand()%PropSum;

         //Alternative ausw�hlten:
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

            //Alternative ausw�hlten:
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

            //Alternative ausw�hlten:
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

            //Alternative ausw�hlten:
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

            //Alternative ausw�hlten:
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

         //5. Unm�glich, rettet bei Bugs vor dem Absturz:
         //---------------------------------------------------------------------------
         PropSum=0; //Wahrscheinlichkeiten aufsummieren:
         for (c=0; c<Clips[ActiveClip].SuccessorIds.AnzEntries(); c++)
            PropSum+=(Clips[ActiveClip].SuccessorTokens[c*2+1]-'0');

         PropSum=rand()%PropSum;

         //Alternative ausw�hlten:
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
