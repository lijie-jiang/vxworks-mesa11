#include <stdio.h>
#include <windows.h>
#include <winioctl.h>

/* Dos FS definitions taken from h/private/dosFsLibP.h of Tornado 2.2.1
 *
 * Because the MS-DOS boot sector format has word data items
 * on odd-byte boundaries, boot sector offsets cannot be represented as a standard C
 * structure.  Instead, the following symbolic offsets are used to
 * isolate data items.  Non-string data values longer than 1 byte are
 * in "Intel 8086" order.
 */
#define DOS_BOOT_BYTES_PER_SEC  0x0b    /* bytes per sector          (2 bytes)*/
#define DOS_BOOT_SEC_PER_CLUST  0x0d    /* sectors per cluster       (1 byte) */
#define DOS_BOOT_NRESRVD_SECS   0x0e    /* # of reserved sectors     (2 bytes)*/
#define DOS_BOOT_NFATS          0x10    /* # of FAT copies           (1 byte) */
#define DOS_BOOT_MAX_ROOT_ENTS  0x11    /* max # of root dir entries (2 bytes)*/
#define DOS_BOOT_NSECTORS       0x13    /* total # of sectors on vol (2 bytes)*/
#define DOS_BOOT_SEC_PER_FAT    0x16    /* # of sectors per FAT copy (2 bytes)*/
#define DOS_BOOT_NHIDDEN_SECS   0x1c    /* # of hidden sectors       (4 bytes)*/
#define DOS_BOOT_LONG_NSECTORS  0x20    /* total # of sectors on vol (4 bytes)*/
#define DOS32_BOOT_SEC_PER_FAT	0x24    /* sectors per FAT           (4 bytes)*/

/* endian data conversion macros, disk always little Endian
 * #define VX_TO_DISK_16(src,pDst) (*(BYTE*)(pDst)=(BYTE)(src),*((BYTE*)(pDst)+1)=(BYTE)((src)>>8))
 * #define VX_TO_DISK_8(src,pDst) (*(BYTE *)(pDst)=(BYTE)(src))
 */

#define DISK_TO_VX_16( pSrc ) (WORD)( \
	( *((BYTE *)(pSrc)+1) << 8 ) |    \
	( *(BYTE *)(pSrc)          ) )

#define DISK_TO_VX_32( pSrc ) (DWORD)( \
	( *((BYTE *)(pSrc)+3)<<24 ) |	   \
    ( *((BYTE *)(pSrc)+2)<<16 ) |	   \
    ( *((BYTE *)(pSrc)+1)<< 8 ) |	   \
    ( *(BYTE *)(pSrc)         ) )

#define VX_TO_DISK_32( src, pDst )				   \
	( * (BYTE *)(pDst)      = (BYTE)(src)      ,   \
	  *((BYTE *)(pDst) + 1) = (BYTE)((src)>> 8),   \
	  *((BYTE *)(pDst) + 2) = (BYTE)((src)>> 16),  \
	  *((BYTE *)(pDst) + 3) = (BYTE)((src)>> 24) ) \

#define DISK_TO_VX_8( pSrc ) (BYTE)( *(BYTE *)(pSrc) )

/*  #define DOS_FAT_12BIT_MAX	4085    min clusters for 16-bit FAT entries*/
#define DOS_FAT_16BIT_MAX	65525    /* min clusters for 32-bit FAT entries*/

typedef struct CLEANUPtag {
	UCHAR *bufm;
    HANDLE hDrive;
	HANDLE hFile;
	BOOL locked;
	BOOL written;
	const char *fname;
	const char *DriveName;
	const char *sectorfilename;
} CLEANUP;

/* Always cleanup resources
 * Pass through retcode, except when new error occures during cleanup
 */
static int cleanup( int retcode, CLEANUP *pCleanup )
{
	DWORD ReturnedByteCount;

	if ( pCleanup->bufm != NULL )
	{
		free( pCleanup->bufm );
		pCleanup->bufm = NULL;
	}
	if ( pCleanup->hFile != INVALID_HANDLE_VALUE )
	{
		if ( CloseHandle( pCleanup->hFile ) == FALSE )
		{
			printf("%s Failed: Close File %s\n", pCleanup->fname, pCleanup->sectorfilename );
			retcode = 1;
		}
		pCleanup->hFile = INVALID_HANDLE_VALUE;
	}
	if ( pCleanup->hDrive != INVALID_HANDLE_VALUE )
	{
		/*
		 * Dismounting forces the filesystem to re-evaluate the media id
		 * and geometry. This is the same as popping the floppy in and out
		 * of the disk drive
		 */
		if ( pCleanup->written )
		{
			if ( DeviceIoControl(
						pCleanup->hDrive,
						FSCTL_DISMOUNT_VOLUME,
						NULL,
						0,
						NULL,
						0,
						&ReturnedByteCount,
						NULL
						) == FALSE )
			{
				printf("%s Failed: Dismount volume %s\n", pCleanup->fname, pCleanup->DriveName );
				retcode = 1;
			}
			pCleanup->written = FALSE;
		}
		if ( pCleanup->locked )
		{
			if ( DeviceIoControl(
					pCleanup->hDrive,
					FSCTL_UNLOCK_VOLUME,
					NULL,
					0,
					NULL,
					0,
					&ReturnedByteCount,
					NULL
					) == FALSE )
			{
				printf("%s Failed: Unlock volume %s\n", pCleanup->fname, pCleanup->DriveName );
				retcode = 1;
			}
			pCleanup->locked = FALSE;
		}
		if ( CloseHandle( pCleanup->hDrive ) == FALSE )
		{
			printf("%s Failed: Close volume %s\n", pCleanup->fname, pCleanup->DriveName );
			retcode = 1;
		}
		pCleanup->hDrive = INVALID_HANDLE_VALUE;
	}

	return retcode;
}

static int usage( char *fname )
{
    printf( "Make a formatted drive bootable for VxWorks\n"
		    "usage:\n"
			"%s -h\n"
		    "%s DRIVELETTER: -ownrisk [-systemdrive] [-ignore] [-f|-r|-b [filename]]\n"
			"%s -ownrisk -ignore -b [filename]\n"
	    	"    -h - display this message\n"
			"    -ownrisk\n"
			"        Indicate that you know, that working with direct disk access can make\n"
			"        the host computer completely unusable, and that using this program is\n"
			"        entirely your own risk, and authors and providers of this program do\n"
			"        not take any liability for whatever direct or indirect damages caused\n"
			"        by this program in any expected or unexpected situation, and do not\n"
			"        provide any guarantee or warrantee for any behaviour of the program.\n"
			"    -systemdrive\n"
			"        Allow writing to a system drive.\n"
			"        Attention! This will most likely make your computer completely unusable\n"
			"        and is only provided as method of last ressort. Not needed for making\n"
			"        data partitions bootable!\n"
			"    -ignore\n"
			"        Ignore errors. This will most likely make the affected drive unusable\n"
			"        or write unusable data into a file!\n"
			"    -f - Save the original bootsector of the drive to a file.\n"
			"    -r - Restore the bootsector for the drive from a file.\n"
			"    -b - Make bootsector bootable, but write to file instead of to bootsector.\n"
			"        VXLD.BIN is the default name for the above files\n"
			, fname, fname, fname );
	return 1;
}

int
main(
    int argc,
    char *argv[]
    )
{
	static const unsigned char bootSectDat[] =
	{
	0xeb, 0x3c, 0x90,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0,    0,    0,
	   0,    0,    0,    0,    0,    0, 0xfa, 0x88,
	0x16, 0x24, 0x7c, 0xb4, 0x08, 0xcd, 0x13, 0x72,
	0x11, 0x83, 0xe1, 0x3f, 0x89, 0x0e, 0x18, 0x7c,
	0xfe, 0xc6, 0x32, 0xd2, 0x86, 0xf2, 0x89, 0x16,
	0x1a, 0x7c, 0xfa, 0x8c, 0xc8, 0x8e, 0xd8, 0x8e,
	0xc0, 0xfc, 0x8e, 0xd0, 0xbc, 0xfe, 0x7b, 0xfb,
	0xbe, 0xe1, 0x7d, 0xe8, 0x14, 0x01, 0x33, 0xdb,
	0x8b, 0xc3, 0x8b, 0xd3, 0xa0, 0x10, 0x7c, 0xf7,
	0x26, 0x16, 0x7c, 0x8b, 0x0e, 0x0e, 0x7c, 0x03,
	0x0e, 0x1c, 0x7c, 0x13, 0x16, 0x1e, 0x7c, 0x03,
	0xc8, 0x13, 0xd3, 0x89, 0x0e, 0x00, 0x7b, 0x89,
	0x16, 0x02, 0x7b, 0xbe, 0x03, 0x7c, 0xbf, 0xf9,
	0x7d, 0xc7, 0x06, 0x36, 0x7c, 0x20, 0x00, 0xb9,
	0x05, 0x00, 0xf3, 0xa6, 0x75, 0x06, 0xc7, 0x06,
	0x36, 0x7c, 0x40, 0x00, 0xa1, 0x36, 0x7c, 0xf7,
	0x26, 0x11, 0x7c, 0x8b, 0x36, 0x0b, 0x7c, 0x03,
	0xc6, 0x48, 0xf7, 0xf6, 0x8b, 0xc8, 0x51, 0xa1,
	0x00, 0x7b, 0x8b, 0x16, 0x02, 0x7b, 0xbb, 0x00,
	0x7e, 0xe8, 0xd0, 0x00, 0x73, 0x03, 0xe9, 0xa5,
	0x00, 0x8b, 0x16, 0x0b, 0x7c, 0xbf, 0x00, 0x7e,
	0xbe, 0xeb, 0x7d, 0x57, 0xb9, 0x0b, 0x00, 0xf3,
	0xa6, 0x5f, 0x74, 0x1c, 0x03, 0x3e, 0x36, 0x7c,
	0x2b, 0x16, 0x36, 0x7c, 0x75, 0xea, 0x83, 0x06,
	0x00, 0x7b, 0x01, 0x83, 0x16, 0x02, 0x7b, 0x00,
	0x59, 0xe2, 0xc3, 0xbe, 0xea, 0x7d, 0xeb, 0x79,
	0x59, 0x01, 0x0e, 0x00, 0x7b, 0x83, 0x16, 0x02,
	0x7b, 0x00, 0x33, 0xc9, 0xbb, 0x1c, 0x00, 0x83,
	0x3e, 0x36, 0x7c, 0x40, 0x75, 0x03, 0x83, 0xc3,
	0x20, 0x8b, 0x01, 0x43, 0x43, 0x8b, 0x11, 0xf7,
	0x36, 0x0b, 0x7c, 0x40, 0xa3, 0x04, 0x7b, 0x83,
	0xeb, 0x04, 0x8b, 0x01, 0x48, 0x48, 0x8a, 0x0e,
	0x0d, 0x7c, 0xf7, 0xe1, 0x03, 0x06, 0x00, 0x7b,
	0x13, 0x16, 0x02, 0x7b, 0xbb, 0x00, 0x08, 0x8e,
	0xc3, 0x33, 0xdb, 0x50, 0x52, 0xe8, 0x54, 0x00,
	0x5a, 0x58, 0x72, 0x2a, 0xbe, 0xf7, 0x7d, 0xe8,
	0x30, 0x00, 0xff, 0x0e, 0x04, 0x7b, 0x74, 0x0d,
	0x83, 0xc0, 0x01, 0x83, 0xd2, 0x00, 0x8c, 0xc3,
	0x83, 0xc3, 0x20, 0xeb, 0xda, 0xa0, 0x24, 0x7c,
	0x24, 0x80, 0x75, 0x06, 0xba, 0xf2, 0x03, 0x32,
	0xc0, 0xee, 0xff, 0x2e, 0x7e, 0x7d, 0xbe, 0xe6,
	0x7d, 0xe8, 0x06, 0x00, 0xeb, 0xfe, 0x00, 0x00,
	0x00, 0x08, 0x53, 0x50, 0x80, 0x3e, 0xe0, 0x7d,
	0x00, 0x75, 0x0e, 0xac, 0x0a, 0xc0, 0x74, 0x09,
	0xb4, 0x0e, 0xbb, 0x07, 0x00, 0xcd, 0x10, 0xeb,
	0xf2, 0x58, 0x5b, 0xc3, 0x89, 0x1e, 0x06, 0x7b,
	0x8b, 0x1e, 0x18, 0x7c, 0xf7, 0xf3, 0x42, 0x8b,
	0xda, 0x33, 0xd2, 0xf7, 0x36, 0x1a, 0x7c, 0x86,
	0xe0, 0xb1, 0x06, 0xd2, 0xe0, 0x91, 0x0a, 0xcb,
	0x8a, 0xf2, 0xbb, 0x05, 0x00, 0x53, 0x8b, 0x1e,
	0x06, 0x7b, 0x8a, 0x16, 0x24, 0x7c, 0xb8, 0x01,
	0x02, 0x51, 0x52, 0xcd, 0x13, 0x5a, 0x59, 0x72,
	0x03, 0x5b, 0xf8, 0xc3, 0x33, 0xc0, 0xcd, 0x13,
	0x5b, 0xfe, 0xcf, 0x75, 0xe0, 0xf9, 0xeb, 0xf3,
	0x00, 0x56, 0x31, 0x2e, 0x36, 0x00, 0x21, 0x52,
	0x64, 0x00, 0x21, 0x42, 0x4f, 0x4f, 0x54, 0x52,
	0x4f, 0x4d, 0x20, 0x53, 0x59, 0x53, 0x00, 0x2b,
	0x00, 0x56, 0x58, 0x45, 0x58, 0x54,    0,    0
	};

	static const char mediatype_floppy[] =
	{
		F5_1Pt2_512,            // 5.25", 1.2MB,  512 bytes/sector
		F3_1Pt44_512,           // 3.5",  1.44MB, 512 bytes/sector
		F3_2Pt88_512,           // 3.5",  2.88MB, 512 bytes/sector
		F3_20Pt8_512,           // 3.5",  20.8MB, 512 bytes/sector
		F3_720_512,             // 3.5",  720KB,  512 bytes/sector
		F5_360_512,             // 5.25", 360KB,  512 bytes/sector
		F5_320_512,             // 5.25", 320KB,  512 bytes/sector
		F5_320_1024,            // 5.25", 320KB,  1024 bytes/sector
		F5_180_512,             // 5.25", 180KB,  512 bytes/sector
		F5_160_512,             // 5.25", 160KB,  512 bytes/sector
		F3_120M_512,            // 3.5", 120M Floppy
		F3_640_512,             // 3.5" ,  640KB,  512 bytes/sector
		F5_640_512,             // 5.25",  640KB,  512 bytes/sector
		F5_720_512,             // 5.25",  720KB,  512 bytes/sector
		F3_1Pt2_512,            // 3.5" ,  1.2Mb,  512 bytes/sector
		F3_1Pt23_1024,          // 3.5" ,  1.23Mb, 1024 bytes/sector
		F5_1Pt23_1024,          // 5.25",  1.23MB, 1024 bytes/sector
		F3_128Mb_512,           // 3.5" MO 128Mb   512 bytes/sector
		F3_230Mb_512,           // 3.5" MO 230Mb   512 bytes/sector
		F8_256_128,              // 8",     256KB,  128 bytes/sector
		0
	};

	static const char mediatype_harddisk[] =
	{
		RemovableMedia,         // Removable media other than floppy
		FixedMedia,             // Fixed hard disk media
		0
	};

	static const char parttype[] =
	{
		PARTITION_FAT_12, // 12-bit FAT entries
		PARTITION_FAT_16, // 16-bit FAT entries
		PARTITION_HUGE,   // Huge partition MS-DOS V4
		PARTITION_FAT32,  // FAT32
		PARTITION_FAT32_XINT13, // FAT32 using extended int13 services
		PARTITION_XINT13, // Win95 partition using extended int13 services
		0
	};

	static const char ownrisk[] = "ownrisk";
	static const char systemdrive[] = "systemdrive";
	static const char ignore[] = "ignore";
	static const char vxldbin[] = "VXLD.BIN";
	static const char emptystring[] = "";

	CLEANUP Cleanup = { 0 };
    const char *DriveName = emptystring;
	const char *sectorfilename = emptystring;
	BOOL ownriskflag = FALSE;
	BOOL systemdriveflag = FALSE;
	BOOL ignoreflag = FALSE;
	BOOL saveflag = FALSE;
	BOOL restoreflag = FALSE;
	BOOL bootfileflag = FALSE;

	char ownriskbuf[sizeof (ownrisk)+1];
	char systemdrivebuf[sizeof (systemdrive)+1];
	char ignorebuf[sizeof (ignore)+1];

	char fname[_MAX_FNAME];
    char Drive[MAX_PATH];
    HANDLE hDrive;
    HANDLE hFile;
	DWORD BytesPerSector;
	UCHAR *pBoot;
	LARGE_INTEGER newfilepos;
	DWORD BytesRW;
    DISK_GEOMETRY Geometry;
    PARTITION_INFORMATION PartInfo;
    DWORD ReturnedByteCount;
	LARGE_INTEGER SectorsPerDisk;
	DWORD hiddensectors;
    char c;
	char *p;
	UCHAR *bufm;

	_splitpath( argv[0], NULL, NULL, fname, NULL );

    while (--argc > 0 )
	{
        p = *++argv;
		if ( !*p )
		{
			break;
		}
        if (*p == '/' || *p == '-')
		{
            while (c = *++p)
			{
				switch (toupper( c ))
				{
				case '?':
				case 'H':
					return usage( fname );
					break;
				case 'O':
					strncpy(ownriskbuf,p,sizeof (ownrisk) );
					ownriskbuf[sizeof (ownrisk)] = '\0';
					_strlwr(ownriskbuf);
					if (strcmp(ownriskbuf,ownrisk))
					{
						return usage( fname );
					}
					p += sizeof (ownrisk) - 2;
					ownriskflag = TRUE;
					break;
				case 'S':
					strncpy(systemdrivebuf,p,sizeof (systemdrive) );
					systemdrivebuf[sizeof (systemdrive)] = '\0';
					_strlwr(systemdrivebuf);
					if (strcmp(systemdrivebuf,systemdrive))
					{
						return usage( fname );
					}
					p += sizeof (systemdrive) - 2;
					systemdriveflag = TRUE;
					break;
				case 'I':
					strncpy(ignorebuf,p,sizeof (ignore) );
					ignorebuf[sizeof (ignore)] = '\0';
					_strlwr(ignorebuf);
					if (strcmp(ignorebuf,ignore))
					{
						return usage( fname );
					}
					p += sizeof (ignore) - 2;
					ignoreflag = TRUE;
					break;
				case 'F':
					if ( argc < 2 || p[1] || **(argv+1) == '-' || **(argv+1) == '/' )
					{
						sectorfilename = vxldbin;
					}
					else
					{
						--argc;
						sectorfilename = *++argv;
					}
					saveflag = TRUE;
					break;
				case 'R':
					if ( argc < 2 || p[1] || **(argv+1) == '-' || **(argv+1) == '/' )
					{
						sectorfilename = vxldbin;
					}
					else
					{
						--argc;
						sectorfilename = *++argv;
					}
					restoreflag = TRUE;
					break;
				case 'B':
					if ( argc < 2 || p[1] || **(argv+1) == '-' || **(argv+1) == '/' )
					{
						sectorfilename = vxldbin;
					}
					else
					{
						--argc;
						sectorfilename = *++argv;
					}
					bootfileflag = TRUE;
					break;
				default:
					printf("%s: Invalid switch - /%c\n", fname, c );
					return usage( fname );
				}
			}
        }
		else
		{
            if ( !isalpha(*p) || !p[1] || p[1] != ':' || p[2])
			{
				/* relative or absolute path of mounted drive here not supported */
                printf( "%s: Drivename expected\n", fname );
                return usage( fname );
			}
			else
			{
                DriveName = p;
			}
		}
    }

	if ( !ownriskflag || 
		 ((!bootfileflag || !ignoreflag) && *DriveName=='\0') ||
		 (saveflag + restoreflag + bootfileflag > 1 ) )
	{
		return usage( fname );
	}

	Cleanup.DriveName = DriveName;
	Cleanup.fname = fname;
	Cleanup.sectorfilename = sectorfilename;
	Cleanup.hFile = INVALID_HANDLE_VALUE;
	Cleanup.hDrive = INVALID_HANDLE_VALUE;

	sprintf(Drive,"\\\\.\\%s",DriveName);

	/* saveflag: ReadBootSector, Write original boot sector to file
	 * restoreflag: ReadBootSector (for check), WriteBootSector from file
	 * bootfileflag: ReadBootSector (for disk data and check) write new boot data to file
	 * -: ReadBootSector (for check), WriteBootSector new
	 *
	 * For bootfileflag the drivename is optional:
	 * If not given, just write the plain boot sector to a file.
	 */

	if ( bootfileflag && *DriveName=='\0' )
	{
		BytesPerSector = 512;
		SectorsPerDisk.QuadPart = 1;
		hiddensectors = 0;
	}
	else
	{
		if ( saveflag || bootfileflag )
		{
			/* Reading the bootsector only and writing it to a file */
			hDrive = CreateFile(
							Drive,
							GENERIC_READ,
							FILE_SHARE_READ,
							NULL,
							OPEN_EXISTING,
							0,
							NULL
							);
		}
		else
		{
			/* Writing a new bootsector */
			hDrive = CreateFile(
							Drive,
							GENERIC_READ | GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							0,
							NULL
							);
		}
		if ( hDrive == INVALID_HANDLE_VALUE )
		{
			printf( "%s Failed: Open %s\n",fname, DriveName );
			return 1;
		}
		Cleanup.hDrive = hDrive;

		if ( !saveflag && !bootfileflag && !systemdriveflag )
		{
			/* On writing try to lock device for exclusive use.
			 * Even though that writing itself will work without lock,
			 * and writing a single sector only behaves same as exclusive anyway
			 * even without lock, we lock it anyway to prevent accidental damage
			 * of Windows System or Swap file drive.
			 * For these we cannot get an exclusive lock.
			 */
			if ( !DeviceIoControl(
					hDrive,
					FSCTL_LOCK_VOLUME,
					NULL,
					0,
					NULL,
					0,
					&ReturnedByteCount,
					NULL
					) )
			{
				printf("%s Failed: Exclusive access %s\n", fname, DriveName);
				return cleanup( 1, &Cleanup );
			}
			Cleanup.locked = TRUE;
		}

		if ( !DeviceIoControl(
					hDrive,
					IOCTL_DISK_GET_DRIVE_GEOMETRY,
					NULL,
					0,
					&Geometry,
					sizeof(Geometry),
					&ReturnedByteCount,
					NULL
					) )
		{
			printf("%s Failed: Get Disk Geometry %s\n",fname, DriveName);
			return cleanup( 1, &Cleanup );
		}

		BytesPerSector = Geometry.BytesPerSector;

		if ( strchr( mediatype_harddisk, Geometry.MediaType) != NULL )
		{
			if ( !DeviceIoControl(
						hDrive,
						IOCTL_DISK_GET_PARTITION_INFO,
						NULL,
						0,
						&PartInfo,
						sizeof(PartInfo),
						&ReturnedByteCount,
						NULL
						) )
			{
				printf("%s Failed: Get Partition Info Removable or Fixed Hard Disk %s\n",fname, DriveName);
				return cleanup( 1, &Cleanup );
			}
			/* We allow here any FATx partition type, since these partition sector flags are not always
			 * accurately matching the actually formatted volume inside of the partition.
			 * We check in detail later
			 * For vxsys booting we allow only FAT12 and FAT16
			 * (vxsys also supports hardisks without MBR and boot sector directly at the beginning
			 * of the drive, and also vxlongname FAT variant.
			 * But because we are running on Windows here, and those do not making sense for exchanging
			 * disks between VxWorks and Windows, we ignore them here)
			 */
			else if ( strchr( parttype, PartInfo.PartitionType) == NULL )
			{
				printf("%s Failed: Unknown Partition Type %d on Removable or Fixed Hard Disk %s\n",fname, PartInfo.PartitionType, DriveName);
				return cleanup( 1, &Cleanup );
			}
			SectorsPerDisk.QuadPart = PartInfo.PartitionLength.QuadPart / BytesPerSector;
			hiddensectors = (DWORD)(PartInfo.StartingOffset.QuadPart / BytesPerSector);

		}
		else if ( strchr( mediatype_floppy, Geometry.MediaType) == NULL )
		{
			printf("%s Failed: Unknown Floppy Disk Media Type %d on %s\n",fname, Geometry.MediaType, DriveName);
			return cleanup( 1, &Cleanup );
		}
		else
		{
			SectorsPerDisk.QuadPart = Geometry.Cylinders.QuadPart * Geometry.TracksPerCylinder * Geometry.SectorsPerTrack;
			hiddensectors = 0;
		}
	}

	/* Although experimentally not reproducable on WinXP and Win2000, some literature claims
	 * that the buffer needs to be sector aligned in memory for being suitable for direct disc access.
	 * Maybe this is the case on earlier WinNT versions or special situation.
	 * For being on the safe side we allocate the buffer aligned anyway
	 */
	bufm = malloc (BytesPerSector+BytesPerSector-1);
	if ( bufm == NULL )
	{
		printf( "%s Failed: No buffer memory\n",fname );
		return cleanup( 1, &Cleanup );
	}
	Cleanup.bufm = bufm;

	pBoot = (UCHAR *)(((DWORD)bufm + BytesPerSector-1)&(~(BytesPerSector-1)));

	if ( bootfileflag && *DriveName=='\0' )
	{
		/* Instead of reading from the disk we create an empty template boot sector */
		memset( pBoot, 0, BytesPerSector );
		printf("%s Progress: Skipped original Bootloader and Disk Parameters\n",fname);
	}
	else
	{
		newfilepos.u.HighPart = 0;
		newfilepos.u.LowPart = SetFilePointer( hDrive, 0, &(newfilepos.u.HighPart), FILE_BEGIN );
		if ( newfilepos.u.LowPart == 0xFFFFFFFF )
		{
			printf("%s Failed: SetFilePointer on %s\n",fname, DriveName);
			return cleanup( 1, &Cleanup );
		}
		if ( !ReadFile( hDrive, pBoot, BytesPerSector, &BytesRW, NULL ) )
		{
			printf("%s Failed: ReadFile on %s\n",fname, DriveName);
			return cleanup( 1, &Cleanup );
		}
		if ( BytesRW != BytesPerSector )
		{
			printf("%s Failed: Partial ReadFile on %s\n",fname, DriveName);
			return cleanup( 1, &Cleanup );
		}
		printf("%s Progress: Original Bootloader and Disk Parameters read from %s\n",fname, DriveName);

		if ( !ignoreflag )
		{
			DWORD RootDirSectors;
			DWORD FatSz;
			DWORD TotSec;
			DWORD DataSec;
			DWORD CountofClusters;

			/* 0 on FAT32 */
			RootDirSectors = ((DISK_TO_VX_16(pBoot+DOS_BOOT_MAX_ROOT_ENTS) * 32) + (BytesPerSector - 1)) / BytesPerSector;
			FatSz = DISK_TO_VX_16(pBoot+DOS_BOOT_SEC_PER_FAT);
			if (FatSz == 0 )
			{
				FatSz = DISK_TO_VX_32(pBoot+DOS32_BOOT_SEC_PER_FAT);
			}
			TotSec = DISK_TO_VX_16(pBoot+DOS_BOOT_NSECTORS);
			if(TotSec == 0)
			{
				TotSec = DISK_TO_VX_32(pBoot+DOS_BOOT_LONG_NSECTORS);
			}
			DataSec = TotSec -
					  ( DISK_TO_VX_16(pBoot+DOS_BOOT_NRESRVD_SECS) +
						(DISK_TO_VX_8(pBoot+DOS_BOOT_NFATS) * FatSz) + 
						RootDirSectors
					  );
			/* Rounded down; Not cluster index count, since first cluster has index2.
			 * Zero is not allowed because of division by zero.
			 * We will check this later, therefore so far we assign a pseudo value to
			 * CountofClusters in this case
			 */
			if ( DISK_TO_VX_8(pBoot+DOS_BOOT_SEC_PER_CLUST) == 0)
			{
				CountofClusters = 0;
			}
			else
			{
				CountofClusters = DataSec / DISK_TO_VX_8(pBoot+DOS_BOOT_SEC_PER_CLUST);
			}

			/* Now checking confidence and consistency whether this is really a boot parameter block
			 * in the boot sector, or looks only partially as such
			 *
			 * Not checked since Filesystem works without this value:
			 * - 0xF8 is standard for fixed disk, 0xF0 for floppy disk, other 0xF9..0xFF also valid.
			 *   Is here only for MS-DOS 1.x Media Check compatibility
			 *	 (DISK_TO_VX_8(pBoot+DOS_BOOT_MEDIA_BYTE) != 0xF0 &&
			 *	   (DISK_TO_VX_8(pBoot+DOS_BOOT_MEDIA_BYTE) < 0xF8 ) ||
			 * - DOS_BOOT_SIG_REC to indicate that serial, label, and typestring are present
			 * - Unused space in 				 /* Check if
				 RootDirSectors*BytesPerSector > DISK_TO_VX_16(pBoot+DOS_BOOT_MAX_ROOT_ENTS) * 32 ||
			 * - in FAT table:
			 *   Strictly, first Byte of FAT need to be identical to this, but we don't check this.
			 *   For FAT entry 1 and FAT16 or FAT32: Highest Bit=0 shutdown error, SecondHighest Bit=0
			 *   DiskError on last run;
			 *   Other Bits of FAT entry 0 and 1 always 1.
			 *   Special FAT entrys: -9 for Bad Block, -1..-8 for End of file (only -1 used).
			 *   FAT32 has 28Bits. Therefore Bit28..31 always zero, even for negative values as above.
			 */

			if ( /* Boot Sector Signature is expected at offset 510 of Boot Sector.
 				  * Some Software is known to place it into the last two Bytes of the
				  * sector which might be different.
				  * We are tolerant.
				  */
				 (DISK_TO_VX_16(pBoot+510) != 0xAA55 &&
					 DISK_TO_VX_16(pBoot+BytesPerSector-2) != 0xAA55 ) ||
  				 /* FAT12 has less than 4085 Clusters (not less or equal)
				  * FAT16 has less than 65525 Clusters (not less or equal)
				  * This is the only valid way to decide between FAT12/16/32 types!!!
				  * Here we allow only FAT12 and FAT16
				  */
				 CountofClusters >= DOS_FAT_16BIT_MAX ||
				 DISK_TO_VX_16(pBoot+DOS_BOOT_BYTES_PER_SEC) != BytesPerSector ||
				 /* For FAT12 or FAT16 only, RootDirSectors non 0 */
				 RootDirSectors == 0 ||
				 DISK_TO_VX_8(pBoot+DOS_BOOT_NFATS) == 0 ||
				 /* Although more as 2 FAT copies are legal, this is so weired that we do not accept this */
				 DISK_TO_VX_8(pBoot+DOS_BOOT_NFATS) > 2 ||
				 /* The dosFS volume must fit into the partition.
				  * It is allowed, if there is space left over.
				  */
				 TotSec > SectorsPerDisk.QuadPart ||
				 /* If 0 we cannot compute the count of clusters and we have a division by zero
				  * needs to be a power of 2 between 1 and 128
				  * Cluster size max 32KB!
				  */
				 DISK_TO_VX_8(pBoot+DOS_BOOT_SEC_PER_CLUST) * BytesPerSector > 0x8000 ||
				 strchr("\x01\x02\x04\x08\x10\x20\x40\x80",DISK_TO_VX_8(pBoot+DOS_BOOT_SEC_PER_CLUST)) == NULL ||
				 /* If DataSec >= TotSec we had an overflow on subtracting special sectors from TotSec */
				 DataSec >= TotSec ||
				 /* We need space for the boot parameter block itself */
				 DISK_TO_VX_16(pBoot+DOS_BOOT_NRESRVD_SECS) == 0 ||
				 /* FatType is Bits per FAT entry. Since we do not treat FAT32 here we can ignore
				  * integer overflows
				  * FAT need to fit in FAT table
				  */
				 ((CountofClusters+2) * (CountofClusters < 4085 ? 3 : 4) + 2 * BytesPerSector - 1) / (2 * BytesPerSector) > FatSz )
			{
				printf("%s Failed: No or illegal FAT12 or FAT16 Boot Sector for booting found on %s\n",fname, DriveName);
				return cleanup( 1, &Cleanup );
			}
			else
			{
				printf("%s Progress: Original Disk parameters checked\n",fname);
			}
		}
	}
		
	if ( restoreflag )
	{
		/* Restore original sector from file */
		hFile = CreateFile(
						sectorfilename,
						GENERIC_READ,
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,
						0,
						NULL
						);
		if ( hFile == INVALID_HANDLE_VALUE )
		{
			printf("%s Failed: Open Read on %s\n",fname, sectorfilename);
			return cleanup( 1, &Cleanup );
		}
		Cleanup.hFile = hFile;

		if ( !ReadFile( hFile, pBoot, BytesPerSector, &BytesRW, NULL ) )
		{
			printf("%s Failed: ReadFile on %s\n",fname, sectorfilename);
			return cleanup( 1, &Cleanup );
		}
		if ( BytesRW != BytesPerSector )
		{
			printf("%s Failed: Partial ReadFile on %s\n",fname, sectorfilename);
			return cleanup( 1, &Cleanup );
		}

		/* We either read the file or write it, therefore we don't need to close the
		 * handle for making it free for a later write
		 */
		printf("%s Progress: Bootloader and Disk Parameters read from File %s\n",fname, sectorfilename);
	}
	else if ( !saveflag )
	{
		/* Create new boot code in the boot sector with boot parameter block */
		memcpy( pBoot, bootSectDat, 3 );
		memcpy( pBoot+0x3e, bootSectDat+0x3e, sizeof (bootSectDat)-0x3e-2 );

		/* Also fix hidden sectors for bootable
		 * The meaning is operating system specific.
		 * For non bootable data partitions this is not relevant, because they are loaded by an external OS.
		 * For the bootcode to find itself hidden sectors must be equal to LBA of bootsector
		 * For extended partitions, Microsoft formatters are using the distance
		 * to the preceding Ext Part Sectors as hidden sectors, which is usually 0x3f and which
		 * prevents that extended partitions are bootable
		 */
		VX_TO_DISK_32( hiddensectors, pBoot+DOS_BOOT_NHIDDEN_SECS);

		/* Maybe replace the OEM signature string at Byte 3 of Boot sector with length 8 here.
		 * To indicate that there is VxWorks boot code in it.
		 */

		printf("%s Progress: Bootloader created\n",fname);
	}

	if ( saveflag || bootfileflag )
	{
		/* Save original or new sector to a file */
		hFile = CreateFile(
						sectorfilename,
						GENERIC_WRITE,
						0,
						NULL,
						CREATE_ALWAYS,
						0,
						NULL
						);
		if ( hFile == INVALID_HANDLE_VALUE )
		{
			printf("%s Failed: Open Write on %s\n",fname, sectorfilename);
			return cleanup( 1, &Cleanup );
		}
		Cleanup.hFile = hFile;

		if ( !WriteFile( hFile, pBoot, BytesPerSector, &BytesRW, NULL ) )
		{
			printf("%s Failed: WriteFile on %s\n",fname, sectorfilename);
			return cleanup( 1, &Cleanup );
		}
		if ( BytesRW != BytesPerSector )
		{
			printf("%s Failed: Partial WriteFile on %s\n",fname, sectorfilename);
			return cleanup( 1, &Cleanup );
		}
		printf("%s Progress: Bootloader written to File %s\n",fname, sectorfilename);
	}
	else
	{
		/* Write new or restored sector to disk */

		newfilepos.u.HighPart = 0;
		newfilepos.u.LowPart = SetFilePointer( hDrive, 0, &(newfilepos.u.HighPart), FILE_BEGIN );
		if ( newfilepos.u.LowPart == 0xFFFFFFFF )
		{
			printf("%s Failed: SetFilePointer on %s\n",fname, DriveName);
			return cleanup( 1, &Cleanup );
		}
		if ( !WriteFile( hDrive, pBoot, BytesPerSector, &BytesRW, NULL ) )
		{
			printf("%s Failed: WriteFile on %s\n",fname, DriveName);
			return cleanup( 1, &Cleanup );
		}
		if ( BytesRW != BytesPerSector )
		{
			printf("%s Failed: Partial WriteFile on %s\n",fname, DriveName);
			return cleanup( 1, &Cleanup );
		}
		printf("%s Progress: Bootloader written to Disk %s\n",fname, DriveName);
	}

	printf("%s Sucess: Done.\n",fname );

	return cleanup( 0, &Cleanup );
}

/***** END *****/
