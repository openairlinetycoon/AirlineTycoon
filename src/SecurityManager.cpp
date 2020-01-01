// SecurityManager.cpp: implementation of the SecurityManager class.
//
// v1.01:		the class was not closing all opened files.
// v1.00:		initial release
//////////////////////////////////////////////////////////////////////

#include <stdafx.h>

#ifdef CD_PROTECTION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include	"SecurityManager.h"

#define MK_LONG(h,l)		( (((DWORD)h) << 16) | ((DWORD)l) )

#define squareRound(text, temp, T0, T1, T2, T3, roundkey) \
{ \
	temp[0] = T0[MSB (text[0])] \
			^ T1[MSB (text[1])] \
			^ T2[MSB (text[2])] \
			^ T3[MSB (text[3])] \
			^ roundkey[0]; \
  temp[1] = T0[SSB (text[0])] \
			^ T1[SSB (text[1])] \
			^ T2[SSB (text[2])] \
			^ T3[SSB (text[3])] \
			^ roundkey[1]; \
	temp[2] = T0[TSB (text[0])] \
			^ T1[TSB (text[1])] \
			^ T2[TSB (text[2])] \
			^ T3[TSB (text[3])] \
			^ roundkey[2]; \
	temp[3] = T0[LSB (text[0])] \
			^ T1[LSB (text[1])] \
			^ T2[LSB (text[2])] \
			^ T3[LSB (text[3])] \
			^ roundkey[3]; \
} /* squareRound */

#define squareFinal(text, temp, S, roundkey) \
{ \
	text[0] = ((word32) (S[MSB (temp[0])]) << 24) \
			^ ((word32) (S[MSB (temp[1])]) << 16) \
			^ ((word32) (S[MSB (temp[2])]) <<  8) \
			^  (word32) (S[MSB (temp[3])]) \
			^ roundkey[0]; \
	text[1] = ((word32) (S[SSB (temp[0])]) << 24) \
			^ ((word32) (S[SSB (temp[1])]) << 16) \
			^ ((word32) (S[SSB (temp[2])]) <<  8) \
			^  (word32) (S[SSB (temp[3])]) \
			^ roundkey[1]; \
	text[2] = ((word32) (S[TSB (temp[0])]) << 24) \
			^ ((word32) (S[TSB (temp[1])]) << 16) \
			^ ((word32) (S[TSB (temp[2])]) <<  8) \
			^  (word32) (S[TSB (temp[3])]) \
			^ roundkey[2]; \
	text[3] = ((word32) (S[LSB (temp[0])]) << 24) \
			^ ((word32) (S[LSB (temp[1])]) << 16) \
			^ ((word32) (S[LSB (temp[2])]) <<  8) \
			^  (word32) (S[LSB (temp[3])]) \
			^ roundkey[3]; \
} /* squareFinal */


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SecurityManager::SecurityManager( char *file, long offset, long size )
{
    unsigned long i;
	unsigned long j;
    unsigned long c;

	// reset code to zero
	code = 0;

	// We will build a standard CRC table using the poly
    for (i = 0; i < 256; ++i) {
            for (c = i << 24, j = 8; j > 0; --j)
                    c = c & 0x80000000 ? (c << 1) ^ CRC32_POLY : (c << 1);
            crc32_table[i] = c;
    }

	// Initialize the key used by the decryption algorithm
	dKey[ 0 ][0] = KEY00;
	dKey[ 1 ][0] = KEY01;
	dKey[ 2 ][0] = KEY02;
	dKey[ 3 ][0] = KEY03;
	dKey[ 4 ][0] = KEY04;
	dKey[ 5 ][0] = KEY05;
	dKey[ 6 ][0] = KEY06;
	dKey[ 7 ][0] = KEY07;
	dKey[ 8 ][0] = KEY08;
	dKey[ 0 ][1] = KEY10;
	dKey[ 1 ][1] = KEY11;
	dKey[ 2 ][1] = KEY12;
	dKey[ 3 ][1] = KEY13;
	dKey[ 4 ][1] = KEY14;
	dKey[ 5 ][1] = KEY15;
	dKey[ 6 ][1] = KEY16;
	dKey[ 7 ][1] = KEY17;
	dKey[ 8 ][1] = KEY18;
	dKey[ 0 ][2] = KEY20;
	dKey[ 1 ][2] = KEY21;
	dKey[ 2 ][2] = KEY22;
	dKey[ 3 ][2] = KEY23;
	dKey[ 4 ][2] = KEY24;
	dKey[ 5 ][2] = KEY25;
	dKey[ 6 ][2] = KEY26;
	dKey[ 7 ][2] = KEY27;
	dKey[ 8 ][2] = KEY28;
	dKey[ 0 ][3] = KEY30;
	dKey[ 1 ][3] = KEY31;
	dKey[ 2 ][3] = KEY32;
	dKey[ 3 ][3] = KEY33;
	dKey[ 4 ][3] = KEY34;
	dKey[ 5 ][3] = KEY35;
	dKey[ 6 ][3] = KEY36;
	dKey[ 7 ][3] = KEY37;
	dKey[ 8 ][3] = KEY38;

	// Load and  Init the vxd
	LoadInitVxD( file, offset, size );
}

// Standard destructor that will erase the VxD on the disc
SecurityManager::~SecurityManager()
{
	char	filename[ 128 ];
	unsigned int		pos;

	// release the vxd
	CloseHandle( VxDHandle );


	// Run Time Creation of the VxD Name
	GetTempPath( 128, filename );
	//strcat( filename, ".\\" );
	strcat( filename, MYVXD );
	strcat( filename, "." );
	strcat( filename, EXTENSION );
	
	for( pos = strlen( filename ) - 3; pos < strlen( filename ); pos ++ )
		filename[ pos ]--;

	// erase the VxD files we have created
	DeleteFile( filename );

	// Erase the buffer
	delete writebuffer;
}


bool SecurityManager::LoadInitVxD( char *datafile, long offset, long size)
{
	FILE	*file;
	FILE	*out;
	char	filename[128];
	char	add[128];

	unsigned long		pos;
	unsigned char		*nbuf;

	// Open the dummy file which contain our VxD

	if( ( file = fopen( datafile, "rb" ))  == NULL )
	{
		code += 2;
		return false;
	}

	// Seek to the right pos
	fseek( file, offset, SEEK_SET );

	// Allocate a buffer
	VxDBuffer = new unsigned char[ size ];

	// Read the file inside the buffer correct the buffer by the way...
	code += size - fread( VxDBuffer, sizeof( unsigned char ), size, file );
	fclose(file);

	// Decrypt the VxD using local uncrypt
	DecryptData( (unsigned char *)VxDBuffer, size );

	// Decompress the data
	nbuf = DecompressData( VxDBuffer, (unsigned long &) size );
	delete VxDBuffer;
	VxDBuffer = nbuf;

	// Write the VxD on the disc
	GetTempPath( 128, add );
	strcpy( filename, "\\\\.\\" );
	strcat( filename, add );
	strcat( filename, MYVXD );
	strcat( filename, "." );
	strcat( filename, EXTENSION );
	
	for( pos = strlen( filename ) - 3; pos < strlen( filename ); pos ++ )
		filename[ pos ]--;

	// Write out the VxD
	out = fopen( &filename[4], "wb" );
	code += size - fwrite( VxDBuffer, sizeof( unsigned char ), size, out );
	fclose( out );

	// Initialize the VxD
	VxDHandle = CreateFile( filename, 0, 0, NULL, 0,
                 FILE_FLAG_DELETE_ON_CLOSE, NULL );
    
	// Erase extension
	for( pos = strlen( filename ) - 3; pos < strlen( filename ); pos ++ )
		filename[ pos ] = 0;

	return true;
}

// This function will calculate the checksum of the given area
// This will mainly serves to check if the VxD was not patched
long SecurityManager::CalculateChecksum( unsigned char *lpMem, long size )
{
        unsigned char *p;
        unsigned long  crc;

		// preload shift register, per CRC-32 spec
        crc = 0xffffffff;       

		// Compute
        for (p = lpMem; size > 0; ++p, --size)
                crc = (crc << 8) ^ crc32_table[(crc >> 24) ^ *p];

		// transmit complement, per CRC-32 spec
        return ~crc;            
}

// This function will perform the decryption of a function using the VXD
bool SecurityManager::DecryptFunction( void *lpFunc )
{
	//VxdRet = DeviceIoControl( VxDHandle, 3, caster.ptr, (unsigned long) crc32_table,
	//					dKey, HARDCODED_CHECKSUM, NULL, NULL );
	return DeviceIoControl( VxDHandle, VXD_DECRYPT, lpFunc, 0, dKey, NULL, NULL, NULL ) == 1;
}

// This function will perform the decryption of a function using the VXD
unsigned long SecurityManager::CheckFunction( void *lpFunc )
{
	return DeviceIoControl( VxDHandle, VXD_CHECK, lpFunc, 0, crc32_table, NULL, NULL, NULL );
}

// This function will check for the CD Rom
// Mainly a dummy target for cracker
bool SecurityManager::StupidCDCheck( char driveLetter, char *expectedCDName )
{
	char				root[4];
	unsigned int		value;

	// Create root name structure
	sprintf( root, "%c:\\", driveLetter );

	// Get drive characteristic to see 
	value = GetDriveType( root );

	// Try to guess my dear cracker
	_asm {
		test	eax,eax;
		nop;
		nop;
		nop;
		nop;
		nop;
	}

	if( value != DRIVE_CDROM ) return false;

	return true;
}

// Start of internal function


void SecurityManager::squareDecrypt(word32 text[4], word32 roundkeys[R+1][4])
{
	word32 temp[4];
   
	/* initial key addition */
	text[0] ^= roundkeys[0][0];
	text[1] ^= roundkeys[0][1];
	text[2] ^= roundkeys[0][2];
	text[3] ^= roundkeys[0][3];
 
	/* R - 1 full rounds */
	squareRound (text, temp, Td0, Td1, Td2, Td3, roundkeys[1]);
	squareRound (temp, text, Td0, Td1, Td2, Td3, roundkeys[2]);
	squareRound (text, temp, Td0, Td1, Td2, Td3, roundkeys[3]);
	squareRound (temp, text, Td0, Td1, Td2, Td3, roundkeys[4]);
	squareRound (text, temp, Td0, Td1, Td2, Td3, roundkeys[5]);
	squareRound (temp, text, Td0, Td1, Td2, Td3, roundkeys[6]);
	squareRound (text, temp, Td0, Td1, Td2, Td3, roundkeys[7]);

	/* last round (diffusion becomes only transposition) */
	squareFinal (text, temp, Sd, roundkeys[R]);
#ifdef DESTROY_TEMPORARIES
	memset (temp, 0, sizeof (temp));
#endif /* ?DESTROY_TEMPORARIES */
} /* squareDecrypt */

bool SecurityManager::DecryptData(unsigned char *lpData, unsigned long size )
{
	unsigned long	data[4];
	unsigned long	pos;

	// Now encrypt data
	for( pos = 0; (pos+15) < size; pos += 16 )
	{
		// Put data inside our data array to be encrypted
		data[ 0 ] = lpData[ pos + 0 ] << 24 | lpData[ pos + 1 ] << 16 | lpData[ pos + 2 ] << 8 | lpData[ pos + 3 ];
		data[ 3 ] = lpData[ pos + 12 ] << 24 | lpData[ pos + 13 ] << 16 | lpData[ pos + 14 ] << 8 | lpData[ pos + 15 ];
		data[ 2 ] = lpData[ pos + 8 ] << 24 | lpData[ pos + 9 ] << 16 | lpData[ pos + 10 ] << 8 | lpData[ pos + 11 ];
		data[ 1 ] = lpData[ pos + 4 ] << 24 | lpData[ pos + 5 ] << 16 | lpData[ pos + 6 ] << 8 | lpData[ pos + 7 ];

		// Decrypt the array
		squareDecrypt( data, dKey );
		
		// Copy back the decrypted data
		lpData[ pos + 0 ] = (unsigned char)((data[ 0 ] >> 24) & 0xFF);
		lpData[ pos + 1 ] = (unsigned char)(data[ 0 ] >> 16) & 0xFF;
		lpData[ pos + 2 ] = (unsigned char)(data[ 0 ] >> 8) & 0xFF;
		lpData[ pos + 3 ] = (unsigned char)data[ 0 ]  & 0xFF;
		lpData[ pos + 12 ] = (unsigned char)(data[ 3 ] >> 24) & 0xFF;
		lpData[ pos + 13 ] = (unsigned char)(data[ 3 ] >> 16) & 0xFF;
		lpData[ pos + 14 ] = (unsigned char)(data[ 3 ] >> 8) & 0xFF;
		lpData[ pos + 15 ] = (unsigned char)data[ 3 ] & 0xFF;
		lpData[ pos + 8 ] = (unsigned char)(data[ 2 ] >> 24) & 0xFF;
		lpData[ pos + 9 ] = (unsigned char)(data[ 2 ] >> 16) & 0xFF;
		lpData[ pos + 10 ] = (unsigned char)(data[ 2 ] >> 8) & 0xFF;
		lpData[ pos + 11 ] = (unsigned char)data[ 2 ] & 0xFF;
		lpData[ pos + 4 ] = (unsigned char)(data[ 1 ] >> 24) & 0xFF;
		lpData[ pos + 5 ] = (unsigned char)(data[ 1 ] >> 16) & 0xFF;
		lpData[ pos + 6 ] = (unsigned char)(data[ 1 ] >> 8) & 0xFF;
		lpData[ pos + 7 ] = (unsigned char)data[ 1 ] & 0xFF;
	}

	
	return true;
}

unsigned char * SecurityManager::DecompressData(unsigned char *lpData, unsigned long &size)
{
	unsigned long originalsize;

	// We will retrieve the size of the uncompressed file from the file
	originalsize = *((unsigned long*)lpData);

	// Skip the size
	lpData += sizeof( unsigned long );

	// Initialize our buffer
	readbuffer = lpData;
	readsize = size - 4;
	readpos = 0;

	writebuffer = new unsigned char[ originalsize ];
	writesize = originalsize;
	writepos = 0;

	// Start the decoding engine
	Decode();

	size = writesize;
	return writebuffer;
}

void SecurityManager::Decode(void)	
{
	int  i, j, k, r, c;
	unsigned int  flags;
	
	for (i = 0; i < N - F; i++) text_buf[i] = ' ';
	r = N - F;  flags = 0;
	for ( ; ; ) {
		if (((flags >>= 1) & 256) == 0) {
			if (readpos == readsize) break;
			READCHAR(c);
			flags = c | 0xff00;		/* uses higher byte cleverly */
		}							/* to count eight */
		if (flags & 1) {
			if (readpos == readsize) break;
			READCHAR(c);
			WRITECHAR(c);  text_buf[r++] = c;  r &= (N - 1);
		} else {
			if (readpos == readsize) break;
			READCHAR(i);
			if (readpos == readsize) break;
			READCHAR(j);
			i |= ((j & 0xf0) << 4);  j = (j & 0x0f) + THRESHOLD;
			for (k = 0; k <= j; k++) {
				c = text_buf[(i + k) & (N - 1)];
				WRITECHAR(c);  text_buf[r++] = c;  r &= (N - 1);
			}
		}
	}
}

// This function will evaluate the security code and then return it
// to the application

long GetCode( char *root)
{
	unsigned long	SectorsPerCluster;
	unsigned long	BytesPerSector;
	unsigned long	NumberOfFreeClusters;
	unsigned long	TotalNumberOfClusters;

	// Put a marker
	PUTSTARTMARK;

	GetDiskFreeSpace( root, &SectorsPerCluster, &BytesPerSector,
		&NumberOfFreeClusters, &TotalNumberOfClusters );

	// Put the end marker
	PUTENDMARK;

	return	SectorsPerCluster * BytesPerSector * TotalNumberOfClusters;
}

#endif
