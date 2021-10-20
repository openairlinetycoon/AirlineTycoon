//============================================================================================
// Outro.cpp : Der Render-Outro
//============================================================================================
#include "stdafx.h"
#include "Outro.h"
#include <smacker.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//--------------------------------------------------------------------------------------------
//ULONG PlayerNum
//--------------------------------------------------------------------------------------------
COutro::COutro(BOOL bHandy, SLONG PlayerNum, CString SmackName) : CStdRaum(bHandy, PlayerNum, "", NULL)
{
    RoomBm.ReSize(640, 480);
    RoomBm.FillWith(0);
    PrimaryBm.BlitFrom(RoomBm);

    FrameNum = 0;
    FrameNext = 0;

    StopMidi();
    gpSSE->DisableDS();

    gMouseStartup = TRUE;

    pSmack = smk_open_file(FullFilename(SmackName, IntroPath), SMK_MODE_MEMORY);
    smk_enable_video(pSmack, true);
    smk_info_video(pSmack, &Width, &Height, &Scale);
    if (Scale != SMK_FLAG_Y_NONE)
        Height *= 2;

    unsigned char tracks, channels[7], depth[7];
    unsigned long rate[7];
    smk_enable_audio(pSmack, 0, true);
    smk_info_audio(pSmack, &tracks, channels, depth, rate);

    SDL_AudioSpec desired;
    desired.freq = rate[0];
    desired.format = SDL_AUDIO_MASK_SIGNED | (depth[0] & SDL_AUDIO_MASK_BITSIZE);
    desired.channels = channels[0];
    desired.samples = 2048;
    desired.callback = NULL;
    desired.userdata = NULL;
    audioDevice = SDL_OpenAudioDevice(NULL, 0, &desired, NULL, 0);
    if (!audioDevice) Hdu.HercPrintf(SDL_GetError());

    State = smk_first(pSmack);
    Bitmap.ReSize(XY(Width, Height), CREATE_SYSMEM | CREATE_INDEXED);
    {
        // Copy video frame with line-doubling if needed
        SB_CBitmapKey Key(*Bitmap.pBitmap);
        const unsigned char* pVideo = smk_get_video(pSmack);
        int scale_mode = Scale == SMK_FLAG_Y_NONE ? 1 : 2;
        for (unsigned long y = 0; y < Height; y++)
            memcpy((BYTE*)Key.Bitmap + (y * Key.lPitch), pVideo + ((y / scale_mode) * Key.lPitch), Key.lPitch);
    }
    CalculatePalettemapper(smk_get_palette(pSmack), Bitmap.pBitmap->GetPixelFormat()->palette);
    SDL_QueueAudio(audioDevice, smk_get_audio(pSmack, 0), smk_get_audio_size(pSmack, 0));
    State = smk_next(pSmack);

    SDL_ShowWindow(FrameWnd->m_hWnd);
    SDL_UpdateWindowSurface(FrameWnd->m_hWnd);
}

//--------------------------------------------------------------------------------------------
//COutro-Fenster zerstören:
//--------------------------------------------------------------------------------------------
COutro::~COutro()
{
    if (audioDevice) SDL_CloseAudioDevice(audioDevice);
    audioDevice = 0;

    if (pSmack) smk_close(pSmack);
    pSmack = NULL;

    gMouseStartup = FALSE;
    pCursor->SetImage(gCursorBm.pBitmap);

    if (Sim.Options.OptionEnableDigi) gpSSE->EnableDS();
    if (Sim.Options.OptionMusicType != 0) NextMidi();
    SetMidiVolume(Sim.Options.OptionMusik);
}

/////////////////////////////////////////////////////////////////////////////
// COutro message handlers

//--------------------------------------------------------------------------------------------
// void COutro::OnPaint():
//--------------------------------------------------------------------------------------------
void COutro::OnPaint()
{
    //Die Standard Paint-Sachen kann der Basisraum erledigen
    CStdRaum::OnPaint();

    SDL_PauseAudioDevice(audioDevice, 0);

    if (FrameNum++ < 2) PrimaryBm.BlitFrom(RoomBm);

    if (timeGetTime() >= FrameNext && State == SMK_MORE)
    {
        //Take the next frame:
        Bitmap.ReSize(XY(Width, Height), CREATE_SYSMEM | CREATE_INDEXED);
        {
            // Copy video frame with line-doubling if needed
            SB_CBitmapKey Key(*Bitmap.pBitmap);
            const unsigned char* pVideo = smk_get_video(pSmack);
            int scale_mode = Scale == SMK_FLAG_Y_NONE ? 1 : 2;
            for (unsigned long y = 0; y < Height; y++)
                memcpy((BYTE*)Key.Bitmap + (y * Key.lPitch), pVideo + ((y / scale_mode) * Key.lPitch), Key.lPitch);
        }
        CalculatePalettemapper(smk_get_palette(pSmack), Bitmap.pBitmap->GetPixelFormat()->palette);
        SDL_QueueAudio(audioDevice, smk_get_audio(pSmack, 0), smk_get_audio_size(pSmack, 0));
        State = smk_next(pSmack);

        double usf;
        smk_info_all(pSmack, NULL, NULL, &usf);
        FrameNext = timeGetTime() + (usf / 1000.0);
    }

    PrimaryBm.BlitFrom(Bitmap, 320 - Width / 2, 240 - Height / 2);

    if (State != SMK_MORE)
        Sim.Gamestate = GAMESTATE_BOOT;
}

//--------------------------------------------------------------------------------------------
//void COutro::OnLButtonDown(UINT nFlags, CPoint point)
//--------------------------------------------------------------------------------------------
void COutro::OnLButtonDown(UINT, CPoint)
{
    Sim.Gamestate = GAMESTATE_BOOT;
}

//--------------------------------------------------------------------------------------------
// void COutro::OnRButtonDown(UINT nFlags, CPoint point):
//--------------------------------------------------------------------------------------------
void COutro::OnRButtonDown(UINT, CPoint)
{
    DefaultOnRButtonDown();
    Sim.Gamestate = GAMESTATE_BOOT;
}

//--------------------------------------------------------------------------------------------
//BOOL CStdRaum::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) : AG:
//--------------------------------------------------------------------------------------------
BOOL COutro::OnSetCursor(void* pWnd, UINT nHitTest, UINT message)
{
    return (FrameWnd->OnSetCursor(pWnd, nHitTest, message));
}

//--------------------------------------------------------------------------------------------
//void CStdRaum::OnMouseMove(UINT nFlags, CPoint point): AG:
//--------------------------------------------------------------------------------------------
void COutro::OnMouseMove(UINT nFlags, CPoint point)
{
    FrameWnd->OnMouseMove(nFlags, point);
}

//--------------------------------------------------------------------------------------------
//void COutro::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//--------------------------------------------------------------------------------------------
void COutro::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_ESCAPE)
    {
        Sim.Gamestate = GAMESTATE_BOOT;
    }
}