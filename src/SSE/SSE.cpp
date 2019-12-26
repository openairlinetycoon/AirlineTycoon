#include "stdafx.h"

SSE::SSE(void* hWnd, dword samplesPerSec, word channels, word bitsPerSample, word maxFX)
    : _hWnd((HWND)hWnd)
    , _samplesPerSec(samplesPerSec)
    , _channels(channels)
    , _bitsPerSample(bitsPerSample)
    , _maxSound(maxFX)
{
    if (Mix_OpenAudioDevice(samplesPerSec, SDL_AUDIO_MASK_SIGNED | (bitsPerSample & SDL_AUDIO_MASK_BITSIZE), channels, 1024, nullptr, 0) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }

    Mix_AllocateChannels(maxFX);
}

SSE::~SSE()
{
}

HRESULT SSE::CreateFX(FX** ppFX, char* file, dword samplesPerSec, word channels, word bitsPerSample)
{
    if (!ppFX)
        return SSE_INVALIDPARAM;

    *ppFX = new FX();
    return (*ppFX)->Create(this, file, samplesPerSec, channels, bitsPerSample);
}

HRESULT SSE::CreateMidi(MIDI** ppMidi, char* file)
{
    if (!ppMidi)
        return SSE_INVALIDPARAM;

    *ppMidi = new MIDI();
    return (*ppMidi)->Create(this, file);
}

HRESULT SSE::EnableDS()
{
    return SSE_OK;
}

HRESULT SSE::DisableDS()
{
    return SSE_OK;
}

FX::FX()
{
    memset(&_fxData, 0, sizeof(_fxData));
}

FX::~FX()
{
}

HRESULT	FX::Create(SSE* pSSE, char* file, dword samplesPerSec, word channels, word bitsPerSample)
{
    _digitalData.pSSE = pSSE;
    SetFormat(samplesPerSec, channels, bitsPerSample);

    if (file)
        Load(file);

    return SSE_OK;
}

bool FX::StopPriority(dword flags)
{
    return false;
}

long FX::Release()
{
    return 0;
}

HRESULT FX::Play(dword dwFlags, long pan)
{
    return Mix_PlayChannel(-1, _fxData.pBuffer[0], 0) < 0 ? SSE_CANNOTPLAY : SSE_OK;
}

HRESULT FX::Stop()
{
    return SSE_OK;
}

HRESULT FX::Resume()
{
    return SSE_OK;
}

HRESULT FX::GetVolume(long* pVolume)
{
    return SSE_OK;
}

HRESULT FX::SetVolume(long volume)
{
    return SSE_OK;
}

HRESULT FX::GetPan(long* pPan)
{
    return SSE_OK;
}

HRESULT FX::SetPan(long pan)
{
    return SSE_OK;
}

HRESULT FX::Load(const char* file)
{
    _digitalData.file = file;

    void* buf = SDL_LoadFile(file, &_fxData.bufferSize);
    _fxData.pBuffer[0] = Mix_QuickLoad_RAW((Uint8*)buf, _fxData.bufferSize);
    return SSE_OK;
}

HRESULT FX::Fusion(const FX** Fx, long NumFx)
{
    return SSE_OK;
}

HRESULT FX::Fusion(const FX* Fx, long* Von, long* Bis, long NumFx)
{
    return SSE_OK;
}

HRESULT FX::Tokenize(__int64 Token, long* Von, long* Bis, long& rcAnzahl)
{
    return SSE_OK;
}

FX** FX::Tokenize(__int64 Token, long& rcAnzahl)
{
    return NULL;
}

HRESULT FX::Free()
{
    return SSE_OK;
}

HRESULT FX::GetStatus(dword* pStatus)
{
    return SSE_OK;
}

HRESULT FX::IsMouthOpen(long PreTime)
{
    return SSE_OK;
}

word FX::CountPlaying()
{
    return Mix_Playing(-1);
}

void FX::SetFormat(dword samplesPerSec, word channels, word bitsPerSample)
{
    dword lastSamplesPerSec = _fxData.samplesPerSec;
    word lastChannels = _fxData.channels;
    word lastBitsPerSample = _fxData.bitsPerSample;

    _fxData.samplesPerSec = samplesPerSec ? samplesPerSec : _digitalData.pSSE->_samplesPerSec;
    _fxData.channels = channels ? channels : 1;
    _fxData.bitsPerSample = bitsPerSample ? bitsPerSample : _digitalData.pSSE->_bitsPerSample;

    if (_fxData.pBuffer[0] && (_fxData.samplesPerSec != lastSamplesPerSec ||
        _fxData.channels != lastChannels || _fxData.bitsPerSample != lastBitsPerSample))
    {
        Free();
        if (!_digitalData.file.empty())
            Load(_digitalData.file.c_str());
    }
}

MIDI::MIDI()
{
    _music = nullptr;
}

MIDI::~MIDI()
{
}

HRESULT MIDI::Create(SSE* pSSE, char* file)
{
    _musicData.pSSE = pSSE;

    if (file)
        Load(file);

    return SSE_OK;
}

bool MIDI::StopPriority(dword flags)
{
    return false;
}

long MIDI::Release()
{
    return 0;
}

HRESULT MIDI::Play(dword dwFlags, long pan)
{
    return Mix_PlayMusic(_music, 0) < 0 ? SSE_CANNOTPLAY : SSE_OK;
}

HRESULT MIDI::Stop()
{
    return SSE_OK;
}

HRESULT MIDI::Resume()
{
    return SSE_OK;
}

HRESULT MIDI::GetVolume(long* pVolume)
{
    return SSE_OK;
}

HRESULT MIDI::SetVolume(long volume)
{
    return SSE_OK;
}

HRESULT MIDI::GetPan(long* pPan)
{
    return SSE_OK;
}

HRESULT MIDI::SetPan(long pan)
{
    return SSE_OK;
}

HRESULT MIDI::Load(const char* file)
{
    if (_music)
        Free();

    _musicData.file = file;
    _music = Mix_LoadMUS(file);

    // Some version ship with ogg music as well, use it as a fall-back
    if (!_music)
        _musicData.file.replace(_musicData.file.size() - 3, 3, "ogg");
    _music = Mix_LoadMUS(file);
    return SSE_OK;
}

HRESULT MIDI::Free()
{
    Mix_FreeMusic(_music);
    return SSE_OK;
}

HRESULT MIDI::GetStatus(dword* pStatus)
{
    return SSE_OK;
}

word MIDI::CountPlaying()
{
    return 0;
}
