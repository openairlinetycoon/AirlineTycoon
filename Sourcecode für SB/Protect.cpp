//============================================================================================
// Protect.cpp : Routinen zur Kopierschutz:
//============================================================================================
#include "stdafx.h"
#include "cd_prot.h"
#include "SecurityManager.h"
#include "fillfile.h"

extern SLONG IconsPos[];  //Referenziert globe.cpp

extern BOOL gCDFound;
extern BOOL gSpawnOnly;

extern CJumpingVar<CString> gCDPath;

SLONG GetTotalDiskSpace (char Driveletter);
SLONG ReadLine (BUFFER<UBYTE> &Buffer, SLONG BufferStart, char *Line, SLONG LineLength);

extern FILE *CreditsSmackerFileHandle;

BOOL CreditsFilesAreMissing=FALSE;

//--------------------------------------------------------------------------------------------
//Gibt die Seriennummer eines Volumes zurück: (z.B. 0x0D4719D2 für 0D47-19D2)
//--------------------------------------------------------------------------------------------
ULONG GetVolumeSerial (char Drive)
{
	char laufwerk[4];
	laufwerk[0] = Drive;
	laufwerk[1] = ':';
	laufwerk[2] = '\\';
	laufwerk[3] = 0;

	char	chLabel[255];

   dword componentLength;
   dword	fileSystemFlags = 0;
   dword	Serial=0;

	GetVolumeInformation (laufwerk, chLabel, 255, &Serial, &componentLength, &fileSystemFlags, NULL, 0);

   return (Serial);
}

//--------------------------------------------------------------------------------------------
//Liest aus der Registry eine Liste aller physikalischen Devices und gibt zurück, ob die
//sie vorhanden sind. Bit0=DriveA, Bit1=DriveB, ...
//--------------------------------------------------------------------------------------------
ULONG GetPhysicalCdRomBitlist (void)
{
   SLONG rc=0;
   HKEY  hKey;
   dword dwDisposition;
   char  Line[300];
   ULONG LineLength;

   char  Subkey[10];   Subkey[0]='E'; Subkey[1]='N'; Subkey[2]='U'; Subkey[3]='M'; Subkey[4]=0;
   char  strClass[10]; strClass[0]='C'; strClass[1]='l'; strClass[2]='a'; strClass[3]='s'; strClass[4]='s'; strClass[5]=0;
   char  strCDROM[10]; strCDROM[0]='C'; strCDROM[1]='D'; strCDROM[2]='R'; strCDROM[3]='O'; strCDROM[4]='M'; strCDROM[5]=0;

   //CurrentDriveLetterAssignment:
   char  strCurr[50];
   strCurr[ 0]='C'; strCurr[ 1]='u'; strCurr[ 2]='r'; strCurr[ 3]='r'; strCurr[ 4]='e';
   strCurr[ 5]='n'; strCurr[ 6]='t'; strCurr[ 7]='D'; strCurr[ 8]='r'; strCurr[ 9]='i';
   strCurr[10]='v'; strCurr[11]='e'; strCurr[12]='L'; strCurr[13]='e'; strCurr[14]='t';
   strCurr[15]='t'; strCurr[16]='e'; strCurr[17]='r'; strCurr[18]='A'; strCurr[19]='s';
   strCurr[20]='s'; strCurr[21]='i'; strCurr[22]='g'; strCurr[23]='n'; strCurr[24]='m';
   strCurr[25]='e'; strCurr[26]='n'; strCurr[27]='t'; strCurr[28]=0;

   //Die Device Description:
   char strDeviceDesc[30];
   strDeviceDesc[ 0]='D'; strDeviceDesc[ 1]='e'; strDeviceDesc[ 2]='v'; strDeviceDesc[ 3]='i'; 
   strDeviceDesc[ 4]='c'; strDeviceDesc[ 5]='e'; strDeviceDesc[ 6]='D'; strDeviceDesc[ 7]='e'; 
   strDeviceDesc[ 8]='s'; strDeviceDesc[ 9]='c'; strDeviceDesc[10]=0;

   char strVirtual[30]; //Gefährliche Strings
   strVirtual[ 0]='v'; strVirtual[ 1]='i'; strVirtual[ 2]='r'; strVirtual[ 3]='t'; 
   strVirtual[ 4]='u'; strVirtual[ 5]='a'; strVirtual[ 6]='l'; strVirtual[ 7]=' '; 
   strVirtual[ 8]=0; 
   char strFakeCD[10];
   strFakeCD[ 0]='f'; strFakeCD[ 1]='a'; strFakeCD[ 2]='k'; strFakeCD[ 3]='e'; 
   strFakeCD[ 4]='c'; strFakeCD[ 5]='d'; strFakeCD[ 6]=' '; strFakeCD[ 7]=0; 
   char strFakeCD2[10];
   strFakeCD2[ 0]='f'; strFakeCD2[ 1]='a'; strFakeCD2[ 2]='k'; strFakeCD2[ 3]='e'; 
   strFakeCD2[ 4]=' '; strFakeCD2[ 5]=' '; strFakeCD2[ 6]='c'; strFakeCD2[ 7]='d'; strFakeCD2[ 9]=0;
   char strFastCD[10];
   strFastCD[ 0]='f'; strFastCD[ 1]='a'; strFastCD[ 2]='s'; strFastCD[ 3]='t'; 
   strFastCD[ 4]='c'; strFastCD[ 5]='d'; strFastCD[ 6]=' '; strFastCD[ 7]=0; 
   char strPsuedoCD[10];
   strPsuedoCD[ 0]='p'; strPsuedoCD[ 1]='s'; strPsuedoCD[ 2]='e'; strPsuedoCD[ 3]='u'; strPsuedoCD[ 4]='d'; 
   strPsuedoCD[ 5]='o'; strPsuedoCD[ 6]='c'; strPsuedoCD[ 7]='d'; strPsuedoCD[ 8]=0; 
   char strCDSubst[10];
   strCDSubst[ 0]='c'; strCDSubst[ 1]='d'; strCDSubst[ 2]='s'; strCDSubst[ 3]='u';
   strCDSubst[ 4]='b'; strCDSubst[ 5]='s'; strCDSubst[ 6]='z'; strCDSubst[ 7]=0; 

   int always_true=1;
   always_true++;
   always_true--;

   if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_LOCAL_MACHINE, Subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_QUERY_VALUE, NULL, &hKey, &dwDisposition))
   {
      DWORD    Index=0;
      FILETIME dummy;

      LineLength=290; //HEY_LOCAL_MACHINE\\ENUM (DeviceArten) auslesen
      while (ERROR_SUCCESS==RegEnumKeyEx(hKey, Index, Line, &LineLength, NULL, NULL, NULL, &dummy))
      {
         CString CLine = Line;
         HKEY    hKey2;

         //Alle Geräte (z.B. Mitsumi CD-Rom) für diesen DeviceTyp abfragen: 
         if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_LOCAL_MACHINE, CString (Subkey)+"\\"+CLine, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_QUERY_VALUE, NULL, &hKey2, &dwDisposition))
         {
            DWORD Index2=0;

            LineLength=290;
            while (ERROR_SUCCESS==RegEnumKeyEx(hKey2, Index2, Line, &LineLength, NULL, NULL, NULL, &dummy))
            {
               CString CLine2 = Line;
               HKEY    hKey3;

               //Alle Untereinträge für dieses Gerät abfragen:
               if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_LOCAL_MACHINE, CString (Subkey)+"\\"+CLine+"\\"+CLine2, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_QUERY_VALUE, NULL, &hKey3, &dwDisposition))
               {
                  DWORD Index3=0;

                  LineLength=290;
                  while (ERROR_SUCCESS==RegEnumKeyEx(hKey3, Index3, Line, &LineLength, NULL, NULL, NULL, &dummy))
                  {
                     CString CLine3 = Line;

                     SLONG err;

                     //Und für diesen Untereintrag des Gerätes nachschauen, ob's ein CD-Rom ist:
                     HKEY hKey4;
                     if (ERROR_SUCCESS == (err=RegCreateKeyEx(HKEY_LOCAL_MACHINE, CString (Subkey)+"\\"+CLine+"\\"+CLine2+"\\"+CLine3, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ENUMERATE_SUB_KEYS|KEY_EXECUTE|KEY_QUERY_VALUE, NULL, &hKey4, &dwDisposition)))
                     {
                        char          Temp[500];
                        unsigned long TempSize=500;

                        if (ERROR_SUCCESS == RegQueryValueEx (hKey4, strClass, NULL, NULL, (UBYTE*)Temp, &TempSize))
                        {
                           if (stricmp (Temp, strCDROM)==0)
                           {
                              //Enthält die Device Description sowas wie "virtual"?
                              TempSize=500;
                              if (ERROR_SUCCESS == RegQueryValueEx (hKey4, strDeviceDesc, NULL, NULL, (UBYTE*)Temp, &TempSize))
                              {
                                 strlwr (Temp);
                                 if (strstr(Temp, strVirtual)==0 && strstr(Temp, strFakeCD)==0 && strstr(Temp, strFastCD)==0 && strstr(Temp, strFakeCD2)==0 && strstr(Temp, strPsuedoCD)==0 && strstr(Temp, strCDSubst)==0)
                                 {
                                    TempSize=500;
                                    if (ERROR_SUCCESS == RegQueryValueEx (hKey4, strCurr, NULL, NULL, (UBYTE*)Temp, &TempSize))
                                    {
                                       if (*Temp>='A' && *Temp<='Z') rc|=1<<((*Temp)-'A');
                                       if (*Temp>='a' && *Temp<='z') rc|=1<<((*Temp)-'a');
                                    }
                                 }
                              }
                           }
                        }
                        RegCloseKey (hKey4);
                     }

                     LineLength=290;
                     Index3++;
                  }
                  RegCloseKey (hKey3);
               }

               LineLength=290;
               Index2++;
            }
            RegCloseKey (hKey2);
         }

         LineLength=290;
         Index++;
      }
      RegCloseKey (hKey);
   }

   return (rc);
}

//--------------------------------------------------------------------------------------------
//Sucht nach der CD:
//--------------------------------------------------------------------------------------------
bool CDProtection(CString *pCdDrive)
{
	BOOL	bFound = FALSE;
	char	chLabel[255];

	// Alle Laufwerke durchsuchen
	for (word d = 2 ; d <= 26 ; d++)
	{
		char laufwerk[4];
		laufwerk[0] = 65 + d;
		laufwerk[1] = ':';
		laufwerk[2] = '\\';
		laufwerk[3] = 0;
		if ((GetDriveType (laufwerk)) == DRIVE_CDROM)
		{
			dword componentLength;
			dword	fileSystemFlags = 0;
			GetVolumeInformation (laufwerk, chLabel, 255, NULL, &componentLength, &fileSystemFlags, NULL, 0);

         if (stricmp (chLabel, "ATDE")==0) return (true);

			/*CString file = laufwerk;
			file += "data\\at.exe";
			HANDLE hf = CreateFile (file, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
			if (hf && hf != INVALID_HANDLE_VALUE)
			{
				CloseHandle(hf);

            SLONG DiskSpace=GetTotalDiskSpace (laufwerk[0]);

#ifdef CD_PROTECTION_LARGE
            if (DiskSpace<REQUIRED_CD_SIZE)
            {
               //Ge'fake'te CD; Es fehlen Dateien
               return (false);
            }
#endif

				(*pCdDrive) = CString(laufwerk)+"data\\";
				return (true);
			}  */
		}
	}

	return (false);
}

//--------------------------------------------------------------------------------------------
//Errechnet eine Checksumme für eine Datei:
//--------------------------------------------------------------------------------------------
ULONG GetFileChecksum (const CString &Filename)
{
   TEAKFILE File (Filename, TEAKFILE_READ);
   BUFFER<UBYTE>  FileData (File.GetFileLength());
   ULONG         *Buffer;

   SLONG c;
   ULONG HashId=0;

   File.Read(FileData, File.GetFileLength());
   Buffer = (ULONG*)(UBYTE*)FileData;

   for (c=(FileData.AnzEntries()-1)/8; c>=0; c--)
      HashId ^= Buffer[c];

   return (HashId);
}

//--------------------------------------------------------------------------------------------
//Prüft, ob die EXE-Datei okay ist:
//--------------------------------------------------------------------------------------------
BOOL CheckMainFile (const CString &Filename)
{
   ULONG Hash = GetFileChecksum (Filename);

   ULONG Compare;

   TEAKFILE File (FullFilename ("shops.dat", MiscPath), TEAKFILE_READ);

   File >> Compare;
   File.Close();

   if (Hash!=Compare)
   {
      return (FALSE);
   }

   return (TRUE);
}

//--------------------------------------------------------------------------------------------
//Gibt TRUE zurück, wenn Datei korrekt gefunden wurde:
//--------------------------------------------------------------------------------------------
bool CheckCDFile( int _filepos, int _filesize, int _code )
{
   // Create a new security manager
	/*SecurityManager *manager = new SecurityManager ((char*)(LPCTSTR)FullFilename ("plain_r.mcf", MiscPath), 0, 11771);

	// Decrypt the GetCode Function
	manager->DecryptFunction (CheckCDFile);

   delete manager;*/

// Put a marker
PUTSTARTMARK;

  if (gSpawnOnly) return (TRUE);

  char fname[2048];
  char code = (char)_code;

  fseek(CreditsSmackerFileHandle, _filepos, SEEK_SET );

  char buffer[128];
  fread (buffer, 128, 1, CreditsSmackerFileHandle);

  for (int i=0; i<128; i++)
    buffer[i] ^= code;

  lstrcpy (fname, (LPCTSTR)(CString)gCDPath);  fname[3]=0;
  lstrcat (fname, buffer);

  FILE *f = fopen( fname, "rb" );
  if (!f) return (false);

  fseek (f, 0, SEEK_END);
  int filesize = ftell (f);
  fclose (f);

  if (filesize==_filesize) return true;

  // Put the end marker
  PUTENDMARK;

  return false;
}

//--------------------------------------------------------------------------------------------
//Überprüft nach und nach die Länge einer Datei auf der CD:
//--------------------------------------------------------------------------------------------
void CheckSomeCdFileLength (void)
{
   static SLONG NumNewDay=0;

   //hprintf ("CreditsFilesAreMissing1=%li", CreditsFilesAreMissing);

#ifdef CD_PROTECTION
#ifdef CD_PROTECTION_FILLFILE
   if (Sim.bNetwork && Sim.bIsHost==false) return;

   switch (NumNewDay++)
   {
      case 5:  if (!CHECKCDFILE0001) CreditsFilesAreMissing=TRUE;         break;
      case 8:  if (!CHECKCDFILE0002) CreditsFilesAreMissing=TRUE;         break;
      case 9:  if (!CHECKCDFILE0003) CreditsFilesAreMissing=TRUE;         break;
      case 10: if (!CHECKCDFILE0004) CreditsFilesAreMissing=TRUE;         break;
      case 11: if (!CHECKCDFILE0005) CreditsFilesAreMissing=TRUE;         break;
      case 12: if (!CHECKCDFILE0006) CreditsFilesAreMissing=TRUE;         break;
      case 13: if (!CHECKCDFILE0007) CreditsFilesAreMissing=TRUE;         break;
      case 14: if (!CHECKCDFILE0008) CreditsFilesAreMissing=TRUE;         break;
      case 15: if (!CHECKCDFILE0009) CreditsFilesAreMissing=TRUE;         break;
      case 16: if (!CHECKCDFILE0010) CreditsFilesAreMissing=TRUE;         break;
   }

   //hprintf ("CreditsFilesAreMissing1=%li", CreditsFilesAreMissing);

   if (NumNewDay>=50) NumNewDay=0;
#endif
#endif
}

//--------------------------------------------------------------------------------------------
//Gibt zurück, wieviele Bytes auf einer CD sind:
//--------------------------------------------------------------------------------------------
SLONG GetTotalDiskSpace (char Driveletter)
{
   char Path[]="-:\\";

   DWORD SectorsPerCluster;
   DWORD BytesPerSector;
   DWORD NumberOfFreeClusters;
   DWORD TotalNumberOfClusters;

   Path[0]=Driveletter;

   GetDiskFreeSpace(Path, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters);

   return (TotalNumberOfClusters*SectorsPerCluster*BytesPerSector);
}

/**********************************************************************\
|* Demonstration program to compute the 32-bit CRC used as the frame  *|
|* check sequence in ADCCP (ANSI X3.66, also known as FIPS PUB 71     *|
|* and FED-STD-1003, the U.S. versions of CCITT's X.25 link-level     *|
|* protocol).  The 32-bit FCS was added via the Federal Register,     *|
|* 1 June 1982, p.23798.  I presume but don't know for certain that   *|
|* this polynomial is or will be included in CCITT V.41, which        *|
|* defines the 16-bit CRC (often called CRC-CCITT) polynomial.  FIPS  *|
|* PUB 78 says that the 32-bit FCS reduces otherwise undetected       *|
|* errors by a factor of 10^-5 over 16-bit FCS.                       *|
\**********************************************************************/

/* Copyright (C) 1986 Gary S. Brown.  You may use this program, or
   code or tables extracted from it, as desired without restriction.*/

/* First, the polynomial itself and its table of feedback terms.  The  */
/* polynomial is                                                       */
/* X^32+X^26+X^23+X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X^1+X^0 */
/* Note that we take it "backwards" and put the highest-order term in  */
/* the lowest-order bit.  The X^32 term is "implied"; the LSB is the   */
/* X^31 term, etc.  The X^0 term (usually shown as "+1") results in    */
/* the MSB being 1.                                                    */

/* Note that the usual hardware shift register implementation, which   */
/* is what we're using (we're merely optimizing it by doing eight-bit  */
/* chunks at a time) shifts bits into the lowest-order term.  In our   */
/* implementation, that means shifting towards the right.  Why do we   */
/* do it this way?  Because the calculated CRC must be transmitted in  */
/* order from highest-order term to lowest-order term.  UARTs transmit */
/* characters in order from LSB to MSB.  By storing the CRC this way,  */
/* we hand it to the UART in the order low-byte to high-byte; the UART */
/* sends each low-bit to hight-bit; and the result is transmission bit */
/* by bit from highest- to lowest-order term without requiring any bit */
/* shuffling on our part.  Reception works similarly.                  */

/* The feedback terms table consists of 256, 32-bit entries.  Notes:   */
/*                                                                     */
/*  1. The table can be generated at runtime if desired; code to do so */
/*     is shown later.  It might not be obvious, but the feedback      */
/*     terms simply represent the results of eight shift/xor opera-    */
/*     tions for all combinations of data and CRC register values.     */
/*                                                                     */
/*  2. The CRC accumulation logic is the same for all CRC polynomials, */
/*     be they sixteen or thirty-two bits wide.  You simply choose the */
/*     appropriate table.  Alternatively, because the table can be     */
/*     generated at runtime, you can start by generating the table for */
/*     the polynomial in question and use exactly the same "updcrc",   */
/*     if your application needn't simultaneously handle two CRC       */
/*     polynomials.  (Note, however, that XMODEM is strange.)          */
/*                                                                     */
/*  3. For 16-bit CRCs, the table entries need be only 16 bits wide;   */
/*     of course, 32-bit entries work OK if the high 16 bits are zero. */
/*                                                                     */
/*  4. The values must be right-shifted by eight bits by the "updcrc"  */
/*     logic; the shift must be unsigned (bring in zeroes).  On some   */
/*     hardware you could probably optimize the shift in assembler by  */
/*     using byte-swap instructions.                                   */

static DWORD crc_32_tab[] = { /* CRC polynomial 0xedb88320 */
0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

#define UPDC32(octet, crc) (crc_32_tab[((crc) ^ (octet)) & 0xff] ^ ((crc) >> 8))

DWORD	CRC(unsigned char * pBuffer, DWORD dwBufferSize)
{
	DWORD g_oldcrc32 = 0xFFFFFFFF;

   // Create a new security manager
	/*SecurityManager *manager = new SecurityManager ((char*)(LPCTSTR)FullFilename ("plain_r.mcf", MiscPath), 0, 11771);

	// Decrypt the GetCode Function
	manager->DecryptFunction (CRC);

   delete manager; */

// Put a marker
PUTSTARTMARK;

	for (DWORD counter = 0 ; counter < dwBufferSize ; counter++)
		g_oldcrc32 = UPDC32(pBuffer[counter], g_oldcrc32);

  // Put the end marker
  PUTENDMARK;

	return (g_oldcrc32);
}

// ---------------------------------------------------------------------
// Helper function
// ---------------------------------------------------------------------
bool	GetRegistryString (HKEY hKey, const char* subKey, const char* entry, char* result, DWORD* pBufferSize)
{
	bool	retVal = false;
	HKEY	key1;

   // Create a new security manager
	/*SecurityManager *manager = new SecurityManager ((char*)(LPCTSTR)FullFilename ("plain_r.mcf", MiscPath), 0, 11771);

	// Decrypt the GetCode Function
	manager->DecryptFunction (GetRegistryString);

   delete manager;*/

// Put a marker
PUTSTARTMARK;

	if (ERROR_SUCCESS == RegOpenKeyEx (hKey, subKey, 0, KEY_ALL_ACCESS, &key1))
	{
		retVal = (ERROR_SUCCESS == RegQueryValueEx (key1, entry, NULL, NULL, (BYTE*)result, pBufferSize));
		RegCloseKey (key1);
	}

	if (!retVal) *pBufferSize = 0;
	return (retVal);

  // Put the end marker
  PUTENDMARK;
}

// ---------------------------------------------------------------------
// Build a unique key
// Get several registry entries and build crc for each one,
// than build ONE CRC from the result
// ---------------------------------------------------------------------
DWORD GetKey()
{
   typedef struct uniqueComp
   {
	   DWORD USERNAME;
	   DWORD DIGITALID;
	   DWORD FIRSTINSTALL;
	   DWORD HWID;
	   DWORD PRODUCTID;
	   DWORD PRODUCTKEY;
	   DWORD MEMORY;
	   DWORD	SERIALHD;
   } UCOMP;

   // Create a new security manager
	/*SecurityManager *manager = new SecurityManager ((char*)(LPCTSTR)FullFilename ("plain_r.mcf", MiscPath), 0, 11771);

	// Decrypt the GetCode Function
	manager->DecryptFunction (GetKey);

   delete manager;*/

// Put a marker
PUTSTARTMARK;

	//
	// [HKEY_LOCAL_MACHINE/Software/Microsoft/Windows/CurrentVersion]
	// RegisteredOwner="Spellbound Software"
	// DigitalProductId=...
	// FirstInstallDateTime=60 9c 9e 25
	// HWID="4953f7a0a1c311d28b4100104b16c7cb"
	// ProductId="79878-025-0021191-40429"
	// ProductKey="TFR6W-49TR8-MX68M-X4CC9-XDYRJ"

	UCOMP key;
	ZeroMemory (&key, sizeof (key));

	char	buffer[2048];
	DWORD	bufferSize;

	// Username
	bufferSize = 2048;
	if (GetRegistryString (HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion", "RegisteredOwner", buffer, &bufferSize))
		key.USERNAME = CRC((unsigned char*)buffer, bufferSize);

	// DigitalProductId
	bufferSize = 2048;
	if (GetRegistryString (HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion", "DigitalProductId", buffer, &bufferSize))
		key.DIGITALID = CRC((unsigned char*)buffer, bufferSize);

	// FirstInstallDateTime
	bufferSize = 2048;
	if (GetRegistryString (HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion", "FirstInstallDateTime", buffer, &bufferSize))
		key.FIRSTINSTALL = CRC((unsigned char*)buffer, bufferSize);
	
	// HWID
	bufferSize = 2048;
	if (GetRegistryString (HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion", "HWID", buffer, &bufferSize))
		key.HWID = CRC((unsigned char*)buffer, bufferSize);
	
	// ProductId
	bufferSize = 2048;
	if (GetRegistryString (HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion", "ProductId", buffer, &bufferSize))
		key.PRODUCTID = CRC((unsigned char*)buffer, bufferSize);

	// ProductKey
	bufferSize = 2048;
	if (GetRegistryString (HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion", "ProductKey", buffer, &bufferSize))
		key.PRODUCTKEY = CRC((unsigned char*)buffer, bufferSize);

	// Serial-Number of the System HD
	GetSystemDirectory(buffer, 2048);
	key.SERIALHD = GetVolumeSerial(buffer[0]);

	// RAM-Size
	MEMORYSTATUS ms;
	ZeroMemory (&ms, sizeof (ms));
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);
	key.MEMORY = ms.dwTotalPhys;

	return (CRC((unsigned char*)&key, sizeof(UCOMP)));

// Put a marker
PUTENDMARK;
}

//--------------------------------------------------------------------------------------------
// Checks if this computer was licensed: (TRUE if okay, FALSE if not licensed)
//--------------------------------------------------------------------------------------------
bool CheckComputerLicense (void)
{
   static DWORD Licenses[] =
   {
      //Spellbound:
      0x5a386d00, //Mein Rechner
      0xafce7953, //Mein Zweitrechner für's Netzwerk
      0xa25b21cb, //Armin Gessert
      0x3d7d3458, //Armin Gessert (Laptop)
      0x250bf30b, //Jean-Marc Haessig
      0x7e5cc765, //Serge Mandon
      0x1c4298a0, //Mein privater Rechner

      //Betatester mit Netz:
      0x2ce68e1c, //Christian Dube
      0x57f0ad56, //Christian Dube
      0x28237d75, //Christian Dube
      0x44c10327, //Christian Dube
      0xd1e4cb63, //Christian Dube
      0x98fcfacf, //Christian Dube (neu)
      0xfbfa3e2a, //Christian Dube (neu)
      0x360aaa9b, //Erhard Gayer
      0x0b3600bc, //Erhard Gayer
      0x5223998d, //Erhard Gayer
      0xf2c0f5bf, //Joerg Friedrich
      0xe135f95e, //Joerg Friedrich
      0x145f2460, //Joerg Friedrich
      0xfaba9f73, //Joerg Friedrich
      0xa4a7bfb2, //Sandro Le Foche
      0x28f51903, //Sandro Le Foche
      0xc346d721, //ganymed
      0xa61ed38b, //ganymed
      0x95c28ff7, //Ver Lucas
      0xad6b16b9, //Ver Lucas
      0xca6ab899, //Norbert.Roeth@t-online.de
      0x2ec53155, //Norbert.Roeth@t-online.de
      0xcd6269b2, //Norbert.Roeth@t-online.de
      0x02df502e, //Norbert.Roeth@t-online.de (neu)

      //Betatester ohne Netz bzw. nur Daten von einem Rechner:
      0xc83613c5, //SB
      0xe383a19b, //OZehrer@aol.com
      0xe90ea240, //David Faber
      0x0cea35ef, //David Faber (neu)
      0x416d0712, //David Faber (neu)
      0x9dc26c4d, //Marcel Meier
      0x6b79c07e, //Steffen Brandt
      0x85e74863, //Marco Koch
      0x9bcf0c0e, //Kandis@gmx.de
      0x33aca95c, //Kandis@gmx.de (neu)
      0x6c217edf, //Kandis@gmx.de (neu)
      0x885e30f0, //Kandis@gmx.de (neu)
      0x5c25945b, //Arno Fink
      0xed09cdb6, //MUlzhoefer@aol.com
      0xe2219be3, //XSt3585564@aol.com
      0xd56ef41e, //Philip Beissert
      0x8adc4ed3, //j.tegethoff@01019freenet.de
      0x5b36189f, //Rudi Esser
      0xe4ab1fd7, //M.Stueber@mobilkom.at
      0xd7ebc476, //Ingo Löhrer
      0x08ae5e71, //Klaus H.D. Luber
      0xe030371f, //Klaus H.D. Luber (neu)
      0xf0921f32, //Mario Ruppert
      0xf7d3531c, //Mario Ruppert (neu)
      0x01889940, //Steffen
      0x91a92798, //Christoph Reermann
      0x5b15f988, //Anatol Tobler
      0x3ccb6546, //Anatol Tobler (neu)
      0xc95fec71, //Airline Tycoon
      0x99440948, //Eik Hitscherich
      0xddf59385, //Eik Hitscherich (neu)
      0x0e9586ba, //Eik Hitscherich (neu)
      0xcbd2f7d3, //Stephan Schulz
      0x3a82c936, //Norman Rummler
      0xbbf0dba5, //Andre Schulz
      0xe9fbaec2, //Bastian Scholtysik  
      0x9bcf0c0e, //sw@niederrhein.de
      0xaa60aa4a, //Rudi.Esser@t-online.de (Neues Motherboard)
      0xb6176640, //Oliver Schill (neu)
      0x7fe9baff, //Stefan Hock

      //Ende
      NULL
   };

   // Create a new security manager
	/*SecurityManager *manager = new SecurityManager ((char*)(LPCTSTR)FullFilename ("plain_r.mcf", MiscPath), 0, 11771);

	// Decrypt the GetCode Function
	manager->DecryptFunction (CheckComputerLicense);

   delete manager; */

// Put a marker
PUTSTARTMARK;

   DWORD ThisComputer = GetKey();

   for (SLONG c=0; Licenses[c]; c++)
      if (Licenses[c]==ThisComputer) return (true);

   return (false);

// Put a marker
PUTENDMARK;
}

//--------------------------------------------------------------------------------------------
// Sucht aus, welchen String wir den heute mal so abfagen:
//--------------------------------------------------------------------------------------------
void GetProtectionString (const CString &TabFilename, SLONG *pPageAndWord, CString *pRightWord)
{
   BUFFER<char>  Line(300);
   SLONG         FileP, Num, Count=0;

   //Load Table header:
   BUFFER<UBYTE> FileData (*LoadCompleteFile (FullFilename (TabFilename, ExcelPath)));

   //Die erste Zeile einlesen
   FileP=0; FileP=ReadLine (FileData, FileP, Line, 300);

   while (1)
   {
      if (FileP>=FileData.AnzEntries()) break;
      FileP=ReadLine (FileData, FileP, Line, 300);

      TeakStrRemoveEndingCodes (Line, "\xd\xa\x1a\r");

      Num = atoi (strtok (Line, TabSeparator));

      if (Num>=1000000) Count++;
   }

   TEAKRAND r;

   CTime t    = CTime::GetCurrentTime();
   long  days = t.GetDay()+t.GetMonth()+t.GetYear();
   r.SRandTime();
   Count = days%(Count-(Count/8))+r.Rand(Count/8);

   //Load Table header:
   BUFFER<UBYTE> FileData2 (*LoadCompleteFile (FullFilename (TabFilename, ExcelPath)));

   //Die erste Zeile einlesen
   FileP=0; FileP=ReadLine (FileData2, FileP, Line, 300);

   bool bFirst=true;
   while (1)
   {
      if (FileP>=FileData2.AnzEntries()) break;
      FileP=ReadLine (FileData2, FileP, Line, 300);

      TeakStrRemoveEndingCodes (Line, "\xd\xa\x1a\r");

      Num = atoi (strtok (Line, TabSeparator));

      if (Num>=1000000)
      {
         Count--;
         if (Count==0 || bFirst)
         {
            bFirst=false;
            if (pPageAndWord) *pPageAndWord = Num;
            if (pRightWord)   *pRightWord   = strtok (NULL, TabSeparator);
         }
      }
   }

   FileData.FillWith(0);
   FileData2.FillWith(0);
}
