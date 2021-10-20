
/******************************************************************************\
*
* Projekt:	SSE (Spellbound SoundEngine)
* Author:	Armin Gessert
* Begin:		16.12.1996
* Purpose:  Include-Datei
*
\******************************************************************************/

#ifndef _SSE_
#define _SSE_

#include <list>
#include <string>

#define DllExport

#define	MAX_FX_BUFFER			(4)
#define	EVENTS					(2)

#define	SSE_OK					0
#define	SSE_DSOUND_NOINIT		(-1000)
#define	SSE_NOTCREATED			(-1010)
#define	SSE_ALREADYCREATED		(-1020)
#define	SSE_NOFILENAME			(-1030)
#define	SSE_CANNOTLOAD			(-1040)
#define	SSE_CANNOTREAD			(-1050)
#define	SSE_NOTSUPPORTED		(-1060)
#define	SSE_SOUNDDISABLED		(-1070)
#define	SSE_MUSICDISABLED		(-1080)
#define	SSE_MAXFXREACHED		(-1090)
#define	SSE_CANNOTPLAY			(-1100)
#define	SSE_NOMUSICLOADED		(-1110)
#define	SSE_NOSOUNDLOADED		(-1120)
#define SSE_INVALIDPARAM		(-1130)
#define SSE_CANNOTTEST			(-1140)
#define SSE_NORMALMODE			(-1150)
#define SSE_COMPATIBLEMODE		(-1160)

#define	SSE_MCIERROR			(-3000)
#define	SSE_MIDIERR_STOP		(-3010)

// Interne Flags
#define	STATE_PAUSED			(0x0001)
#define	STATE_PLAYING			(0x0002)

// Zusätzliche Flags für ::Play()
#define	DSBPLAY_RESUME			(0x10000000)
#define	DSBPLAY_SETPAN			(0x20000000)
#define	DSBPLAY_FIRE			(0x40000000)
#define	DSBPLAY_PRIORITY		(0x01000000)
#define	DSBPLAY_HIGHPRIORITY	(0x02000000)
#define	DSBPLAY_NOSTOP			(0x04000000)
#define	DSBPLAY_LOOPING		(0x08000000)

#define DSBSTATUS_PLAYING		(0x00000001)
#define DSBSTATUS_BUFFERLOST	(0x00000002)
#define DSBSTATUS_LOOPING		(0x00000004)
#define DSBSTATUS_LOCHARDWARE	(0x00000008)
#define DSBSTATUS_LOCSOFTWARE	(0x00000010)
#define DSBSTATUS_TERMINATED	(0x00000020)

class FX;
class MIDI;

typedef struct _mixerBounds
{
	SLONG	lMinimum;
	SLONG	lMaximum;
	DWORD	dwSteps;

} MIXERBOUNDS;

/******************************************************************************\
*
* Notwendige Forward-Deklarationen
*
\******************************************************************************/

class SSE;

/******************************************************************************\
*
* Typen für die Objektdaten
*
\******************************************************************************/

typedef struct _DigitalData
{
	std::string	file;
	SSE*			pSSE;
	word			state;
	bool			fNoStop;
	dword			time;
} DigitalData;	

typedef struct _FXData
{
	Mix_Chunk*	pBuffer;
	word			ref;
	size_t		bufferSize;					// Die aktuelle Größe des DSBuffers

	dword		samplesPerSec;	// Primary buffer frequency
	word		channels;			// Kanäle
	word		bitsPerSample;	// Bits per Sample of mono data

	SLONG		pan;
	SLONG		volume;
} FXData;
	
typedef struct _MusicData
{
	std::string		file;
	SSE*			pSSE;
	word			state;
	bool			fNoStop;
	dword			time;
	char			alias[9];
} MusicData;
	
/*typedef struct _DigiMusicData
{
	//IDirectSoundBuffer *		pBuffer;			// Der Sound-Puffer
	//IDirectSoundNotify *		pNotify;			// Das Notify-Objekt
	dword						bufferSize;			// Die Größe des Buffers
	dword						bufferSecs;			// Wieviele Sekunden werden im Puffer gehalten
	dword						triggerSecs;		// Nach wieviel gespielten Sekunden wird neu geladen

	HANDLE						hFile;				// Das Handle der zu spielenden Datei
	dword						fileSizeRemaining;	// Die noch zu lesende Dateigröße
	dword						initialFileSize;	// Die Gesamtgröße der Datei
	dword						initialFilePos;		// (Nach Pause) Startposition im File

	dword						cNotifyStructs;		// Anzahl der zu setzenden Notifies
	//DSBPOSITIONNOTIFY	*		pNotifyStructs;		// Die Notify-Strukturen
	HANDLE						hEvent[EVENTS];		// Die Events, die bei den Notifies generiert werden

	unsigned SLONG				hThread;			// Das Handle des Hilfsthreads
	unsigned int				threadId;			// Die ID des Threads
	bool						threadStop;			// Wird der Thread aufgefordet zu stoppen
	bool						threadWaiting;		// Der Thread wartet

	bool						compatibleMode;		// Soll der kompatible (Terratec-freundliche)
													// Modus verwendet werden?

	dword						samplesPerSec;		// Die Samplefrequenz
	word						channels;			// Kanäle
	word						bitsPerSample;		// Bittiefe

	dword						state;
	bool						looped;				// Soll der Sound geloopt werden?
	bool						critical;			// Ist der Thread gerade im kritischen Teil

	SLONG						pan;				// Pan-Wert
	SLONG						volume;				// gemerkte Lautstärke

} DigiMusicData;*/

/******************************************************************************\
*
* Class:		SSE (Spellbound SoundEngine)
*
\******************************************************************************/
class SSE
{
	friend class DIGITAL;
	friend class FX;

	friend class MUSIC;
	friend class MIDI;

	friend class DIGIMUSIC;

	public:
		DllExport SSE(void *hWnd, dword samplesPerSec = 22050, word channels = 2, word bitsPerSample = 16, word maxFX = 8);
		DllExport ~SSE();

		DllExport int	EnableDS ();
		DllExport int	DisableDS ();
		DllExport int	CreateFX (FX** ppFX, char* file = 0, dword samplesPerSec = 0, word channels = 0, word bitsPerSample = 0);
		DllExport int	CreateMidi (MIDI** ppMidi, char* file = 0);
		//DllExport int	CreateDigimusic (DIGIMUSIC** ppDigimusic, char* file = 0, dword bufferSecs = 4, dword samplesPerSec = 0, word channels = 0, word bitsPerSample = 0);

		//DllExport void		Activate(bool fActivate, bool fPlayAgain = true);
		//DllExport bool		IsDSInit() { return (_pDS != 0); }
		//DllExport IDirectSound* GetDD() { return _pDS; }
		//DllExport IDSB*	GetPrimaryBuffer() { return _pBuffer; }

		DllExport dword	GetFrequency() { return _samplesPerSec; }

		//DllExport int GetCaps(DSCAPS* pDSCaps);

		DllExport int	EnableSound (bool fSoundEnabled);
		DllExport bool		IsSoundEnabled() { return _fSoundEnabled; }
		DllExport void		SetMaxSound(word fx) { _maxSound = Mix_AllocateChannels(fx - _maxSound); }
		DllExport word		GetMaxSound()		{ return _maxSound; }
		DllExport word		GetSoundPlaying();
		DllExport void		StopSound();

		DllExport int	EnableMusic(bool fMusicEnabled	);
		DllExport bool		IsMusicEnabled() { return _fMusicEnabled; }
		DllExport void		StopMusic();

		// Lautstärkefunktionen

		DllExport int	SetMusicVolume(SLONG volume);
		DllExport int	GetMusicVolume(SLONG* pVolume);

		DllExport int	SetSoundVolume(SLONG volume);
		DllExport int	GetSoundVolume(SLONG* pVolume);

		//DllExport int	SetMixerVolume (char * device, SLONG volume);
		//DllExport int	GetMixerVolume(char * device, SLONG* pVolume, MIXERBOUNDS* pMB = NULL);


		DllExport void		SetMusicCallback(void (*callback)());

		//DllExport bool		SetMusicList(char* path, char* files);
		//DllExport void		ClearMusicList();

		DllExport void		SwapChannels(bool fSwap)	{ Mix_SetReverseStereo(-1, _swapChannels = fSwap); }
		DllExport bool		IsSwapChannels()				{ return _swapChannels; }

		DllExport SDL_Window* GetWindow()						{ return _hWnd; }

	protected:
		//std::string	GetNextFileFromPlaylist();
		//int	   CreateSoundBuffer (IDSB** ppBuffer, dword size, dword samplesPerSecond, word channels, word bitsPerSample, dword flags);
		//int	   DuplicateSoundBuffer (IDSB* lpDsbOriginal, IDSB** lplpDsbDuplicate);

	protected:
		SDL_Window* _hWnd;
		dword		_samplesPerSec;	// Primary buffer frequency
		word		_channels;			// Kanäle
		word		_bitsPerSample;	// Bits per Sample of mono data
		word		_maxSound;			// Anz. der Samples, die gleichzeitig gespielt werden dürfen
		bool		_swapChannels;

		//IDirectSound* _pDS;			// DirectSound-Object
		//IDSB*		_pBuffer;			// Primary buffer
	
		std::list<FX>	_soundObjList;		// Liste der FX-Objekte
		std::list<MIDI>	_musicObjList;		// Liste der Midi-Objekte

		bool		_fSoundEnabled;
		bool		_fMusicEnabled;

		class MUSIC*	_playingMusicObj;	// Das aktuell gespielte Music-Objekt

		//std::string	_musicListPath;	// Pfad auf Musik-Dateien
		//char*		   _pMusicListFiles;	// Liste der Musik-Dateien (00-terminiert)
		//char*		   _pMusicListPos;	// Ptr. auf den aktuellen Eintrag in der Musik-Liste
};

/******************************************************************************\
*
* Class:	DIGITAL
* Desc:	Einheitliche Schnittstelle für Samples u.s.w.
*
\******************************************************************************/
class DIGITAL
{
	friend class SSE;

	protected:
		DIGITAL() { ZeroMemory(&_digitalData,sizeof(_digitalData)); }
		virtual word GetInternalState() { return _digitalData.state; }
		virtual	bool	StopPriority (dword flags) = 0;

	public:
		virtual	~DIGITAL() {};
		virtual  SLONG Release() = 0;
		virtual	int Play(dword dwFlags = 0, SLONG pan = 0) = 0;
		virtual	int Stop() = 0;
		virtual	int Pause() { return Stop(); }
		virtual	int Resume() = 0;
		virtual	int GetVolume (SLONG* pVolume) = 0;
		virtual	int SetVolume (SLONG volume) = 0;
		virtual	int GetPan (SLONG* pPan) = 0;
		virtual	int SetPan (SLONG pan) = 0;
		virtual	int Load (const char* file = NULL) = 0;
		virtual	int Free () = 0;

		virtual	int GetStatus(dword* pStatus) = 0;
		virtual	word	CountPlaying() = 0;
		virtual	void	SetFormat (dword samplesPerSec, word channels, word bitsPerSample) = 0;

	protected:
		DigitalData _digitalData;
};

/******************************************************************************\
*
* Class:		FX (Soundeffekte)
*
\******************************************************************************/
class FX : public DIGITAL
{
	friend class SSE;

	protected:
		FX();
		int	Create(SSE* pSSE, char* file, dword samplesPerSec, word channels, word bitsPerSample);
		virtual	bool	StopPriority (dword flags);

	public:
		virtual	~FX();
		virtual  SLONG Release();
		virtual	int Play(dword dwFlags = 0, SLONG pan = 0);
		virtual	int Stop();
		virtual	int Pause();
		virtual	int Resume();
		virtual	int GetVolume (SLONG* pVolume);
		virtual	int SetVolume (SLONG volume);
		virtual	int GetPan (SLONG* pPan);
		virtual	int SetPan (SLONG pan);
		virtual	int Load (const char* file = NULL);
		virtual	int Fusion (const FX **Fx, SLONG NumFx);
		virtual	int Fusion (const FX *Fx, SLONG *Von, SLONG *Bis, SLONG NumFx);
		virtual	int Tokenize (__int64 Token, SLONG *Von, SLONG *Bis, SLONG &rcAnzahl);
		virtual	FX    **Tokenize (__int64 Token, SLONG &rcAnzahl);
		virtual	int Free ();

		virtual	int GetStatus(dword* pStatus);
		virtual	bool	IsMouthOpen(SLONG PreTime);
		virtual	word	CountPlaying();
		virtual	void	SetFormat (dword samplesPerSec = 0, word channels = 0, word bitsPerSample = 0);
      SLONG  GetByteLength (void) { return (_fxData.bufferSize); }

	protected:
		FXData	_fxData;
};

/******************************************************************************\
*
* Class:	MUSIC
* Desc:	Einheitliche Schnittstelle für Midi, CD u.s.w.
*
\******************************************************************************/
class MUSIC
{
	friend class SSE;

	protected:
		MUSIC() { ZeroMemory(&_musicData,sizeof(_musicData)); }
		virtual word GetInternalState() { return _musicData.state; }
		virtual	bool	StopPriority (dword flags) = 0;

	public:
		virtual	~MUSIC() {};
		virtual  SLONG Release() = 0;
		DllExport virtual	int Play(dword dwFlags = 0, SLONG pan = 0) = 0;
		virtual	int Stop() = 0;
		virtual	int Pause() { return Stop(); }
		virtual	int Resume() = 0;
		virtual	int GetVolume (SLONG* pVolume) = 0;
		virtual	int SetVolume (SLONG volume) = 0;
		virtual	int GetPan (SLONG* pPan) = 0;
		virtual	int SetPan (SLONG pan) = 0;
		virtual	int Load (const char* file = NULL) = 0;
		virtual	int Free () = 0;

		virtual	int GetStatus(dword* pStatus) = 0;
		virtual	word	CountPlaying() = 0;

	protected:
		MusicData _musicData;
};

/******************************************************************************\
*
* Class:		MIDI (Hintergrundmusik)
*
\******************************************************************************/
class MIDI : public MUSIC
{
	friend class SSE;

	protected:
		MIDI();
		int	Create(SSE* pSSE, char* file);
		virtual	bool StopPriority (dword flags);

	public:
		virtual	~MIDI();
		virtual  SLONG Release();
		DllExport virtual	int Play(dword dwFlags = 0, SLONG pan = 0);
		virtual	int Stop();
		virtual	int Pause();
		virtual	int Resume();
		virtual	int GetVolume (SLONG* pVolume);
		virtual	int SetVolume (SLONG volume);
		virtual	int GetPan (SLONG* pPan);
		virtual	int SetPan (SLONG pan);
		virtual	int Load (const char* file = NULL);
		virtual	int Free ();
		virtual void SetMode(int mode);

		virtual	int GetStatus(dword* pStatus);
		virtual	word	CountPlaying();

	protected:
		Mix_Music*		_music;
		int				_mode;
};
/******************************************************************************\
*
* Class:		DigiMusic (Digitale Hintergrundmusik)
*
\******************************************************************************/

/*class DIGIMUSIC : public DIGITAL,public MUSIC
{
	friend SSE;
	friend unsigned int WINAPI HandleNotifications(void * lParam); 
	friend unsigned int WINAPI CompatiblePlay(LPVOID ptr);
	friend unsigned int WINAPI CompatibilityCheck(LPVOID ptr);

	protected:
		DIGIMUSIC() { ZeroMemory(&_dmData,sizeof(_dmData)); }

		int	Create(SSE* pSSE, char* file,dword buffersSecs,dword samplesPerSec, word channels, word bitsPerSample);
		int DIGIMUSIC::CompatibleModeRequired();

		virtual	word	GetInternalState() { return word(_dmData.state); }
		virtual	bool	StopPriority (dword flags);

	public:
		virtual	~DIGIMUSIC();
		virtual  SLONG Release();
		DllExport virtual	int Play(dword dwFlags = 0, SLONG pan = 0);
		virtual	int Stop();
		virtual	int Pause();
		virtual	int Resume();
		virtual	int GetVolume (SLONG* pVolume);
		virtual	int SetVolume (SLONG volume);
		virtual	int GetPan (SLONG* pPan);
		virtual	int SetPan (SLONG pan);
		virtual	int Load (const char* file = NULL);
		virtual	int Free ();

		virtual	int GetStatus(dword* pStatus);
		virtual	word	CountPlaying();
		virtual	void	SetFormat (dword samplesPerSec = 0, word channels = 0, word bitsPerSample = 0);

		virtual void SetCompatibleMode() { _dmData.compatibleMode = true; }
		virtual void SetNormalMode() { _dmData.compatibleMode = false; }

	protected:
		virtual	int SetupNotifications();
//		dword		HandleNotifications(void * lParam);

	protected:
		DigiMusicData	_dmData;
};*/

#endif
