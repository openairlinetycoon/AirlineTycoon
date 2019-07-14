// SecurityManager.h: interface for the SecurityManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SECURITYMANAGER_H__D58FD8C4_164E_11D2_BA6D_00C0DF80FBBE__INCLUDED_)
#define AFX_SECURITYMANAGER_H__D58FD8C4_164E_11D2_BA6D_00C0DF80FBBE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#define R 8

#define SECURITY_ON		1
#define SECURITY_OFF	0

// Important Security Options
#define	ANTIDEBUGGING SECURITY_ON
#define	CRAPWARE	  SECURITY_OFF
#define	AGRESSIVE	  SECURITY_ON
	
#define PUTSTARTMARK __asm {		\
	__asm	nop				\
	__asm	nop				\
	__asm	inc	eax			\
	__asm	dec eax			\
	__asm	nop				\
	__asm	nop				\
	__asm	nop				\
	__asm	nop				\
	}

#define PUTENDMARK __asm {		\
	__asm	inc	eax			\
	__asm	dec eax			\
	__asm	inc	eax			\
	__asm	dec	eax			\
	}

// Constant for the VxD
#define VXD_DECRYPT 1
#define VXD_CHECK	2
#define	VXD_SIGNED	3

#define	HARDCODED_CHECKSUM	0x20202020

// Constant for the Checksum code
#define CRC32_POLY 0x04c11db7     

// Constant for the decrunching code
#define N		 4096	/* size of ring buffer */
#define F		   18	/* upper limit for match_length */
#define THRESHOLD	2 
#define READCHAR(x)		x = readbuffer[ readpos++ ]
#define WRITECHAR(x)	writebuffer[ writepos++ ] = x
#define DRIVE_CDROM 5

// Constant for the decryption code
#ifndef USUAL_TYPES
#define USUAL_TYPES
typedef unsigned char	byte;	/*  8 bit */
typedef unsigned short	word16;	/* 16 bit */
typedef unsigned long	word32;	/* 32 bit */
#endif /* ?USUAL_TYPES */

#define	KEY00 0xcafe3fb
#define	KEY01 0x9f11cc3b
#define	KEY02 0xe751f3af
#define	KEY03 0x7cd5e03f
#define	KEY04 0xed63d864
#define	KEY05 0xb6e263dc
#define	KEY06 0x0eb1ccd3
#define	KEY07 0x010d84d2
#define	KEY08 0xd6de8b55
#define	KEY10 0x5c07d9c7
#define	KEY11 0x50a83a3c
#define	KEY12 0xcfb9f607
#define	KEY13 0x28e805a8
#define	KEY14 0x543de597
#define	KEY15 0xb95e3df3
#define	KEY16 0x0fbc5e2f
#define	KEY17 0x10d92fc
#define	KEY18 0x38145e4a
#define	KEY20 0x82c27d7
#define	KEY21 0x542bfe10
#define	KEY22 0x483c42c
#define	KEY23 0xcb3a322b
#define	KEY24 0xe3d23783
#define	KEY25 0xb7efd214
#define	KEY26 0xeb1efe7
#define	KEY27 0x10db1c8
#define	KEY28 0x17511a4b
#define	KEY30 0xc83f99f8
#define	KEY31 0xc013be2f
#define	KEY32 0x9438403f
#define	KEY33 0x90bb8413
#define	KEY34 0x5b81b638
#define	KEY35 0xb85381bb
#define	KEY36 0xfbc53af
#define	KEY37 0x10dbc48
#define	KEY38 0x8d976ff8

#include "square.tab"			/* substitution boxes */

#ifdef HARDWARE_ROTATIONS
#define ROTL(x, s) (_lrotl ((x), (s)))
#else  /* !HARDWARE_ROTATIONS */
#define ROTL(x, s) (((x) << (s)) | ((x) >> (32 - (s))))
#endif /* ?HARDWARE_ROTATIONS */

#ifdef MASKED_BYTE_EXTRACTION
#define MSB(x) (((x) >> 24) & 0xffU)	/* most  significant byte */
#define SSB(x) (((x) >> 16) & 0xffU)	/* second in significance */
#define TSB(x) (((x) >>  8) & 0xffU)	/* third  in significance */
#define LSB(x) (((x)      ) & 0xffU)	/* least significant byte */
#else  /* !MASKED_BYTE_EXTRACTION */
#define MSB(x) ((byte)  ((x) >> 24))	/* most  significant byte */
#define SSB(x) ((byte)  ((x) >> 16))	/* second in significance */
#define TSB(x) ((byte)  ((x) >>  8))	/* third  in significance */
#define LSB(x) ((byte)  ((x)      ))	/* least significant byte */
#endif /* ?MASKED_BYTE_EXTRACTION */

#define mul(a, b) ((a && b) ? alogtab[logtab[a] + logtab[b]] : 0)

// definition for the VxD
#define		MYVXD		"VXFLAT"
#define		EXTENSION	"WYE"

// Define the getcode function
long	GetCode( char * );

class SecurityManager  
{
public:
	SecurityManager( char *, long, long );
	virtual ~SecurityManager();

	// The Stupid CD Check to lure cracker
	bool StupidCDCheck( char driveLetter, char *expectedCDName );

	// Function modifier
	bool DecryptFunction( void * );
	unsigned long CheckFunction( void * );

protected:
	unsigned long pId;					/* pointer for softice detection */

	unsigned char	*writebuffer;		/* Buffer for writing */
	unsigned char	*readbuffer;		/* Buffer for reading */

	unsigned long	writesize;			/* size of write buffer */
	unsigned long	readsize;			/* size of read buffer */
	
	unsigned long	writepos;			/* pos in the write flow */
	unsigned long	readpos;			/* pos in the read flow */

	unsigned long	crc32_table[256];
	unsigned char	text_buf[N + F - 1];	/* ring buffer of size N */
	unsigned char	*VxDBuffer;
	void			*VxDHandle;
	unsigned long	dKey[R+1][4];

	long			code;

	long CalculateChecksum( unsigned char *, long );
	bool LoadInitVxD( char *,long ,long );

	void			squareTransform (word32 [4], word32 [4]);
	void			squareDecrypt(word32 [4], word32 [R+1][4]);
	bool			DecryptData(unsigned char *, unsigned long  );
	unsigned char *	DecompressData(unsigned char *, unsigned long &);
	void			Decode(void);

	void			mystrcat( char *, char * );
	void			mystrcpy( char *, char * );
	unsigned short	mystrlen( char * );
	
};

#endif // !defined(AFX_SECURITYMANAGER_H__D58FD8C4_164E_11D2_BA6D_00C0DF80FBBE__INCLUDED_)
