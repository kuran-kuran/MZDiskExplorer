//============================================================================
//  mzdisk.h MZ ディスクイメージ操作
//----------------------------------------------------------------------------
//													Programmed by たご
//----------------------------------------------------------------------------
// 2003.05.16 Ver.0.01 作成開始。
//============================================================================

// -------------------- 2 重定義防止
#ifndef MZDISK_INCLUDE
#define MZDISK_INCLUDE

// -------------------- ヘッダファイル登録
#include "generic.h"

// -------------------- 定数定義
static const char* const GENERICLIB_VERSION = "1.00";
static const char* const GENERICLIB_NAME = "cMzDisk";

// 定数定義
static const U32 MZDISK_TYPE_D88     = 0;
static const U32 MZDISK_TYPE_BETA    = 1;
static const U32 MZDISK_MODE_FILE    = 0;
static const U32 MZDISK_MODE_MEMORY  = 1;
static const U8  MZDISK_TYPE_2D      = 0x00;
static const U8  MZDISK_TYPE_2DD     = 0x10;
static const U8  MZDISK_TYPE_2HD     = 0x20;
static const U32 MZDISK_FILEMODE_MZT          = 0x00000000;
static const U32 MZDISK_FILEMODE_BIN          = 0x00000001;
static const U32 MZDISK_FILEMODE_MASK         = 0x0000000F;
static const U32 MZDISK_FILEMODE_NEWLINE_CONV = 0x00000010;
static const U32 MZDISK_FILETYPE_OBJ          = 0x00000001;
static const U32 MZDISK_FILETYPE_BTX          = 0x00000002;
static const U32 MZDISK_FILETYPE_BSD          = 0x00000003;
static const U32 MZDISK_FILETYPE_BRD          = 0x00000004;
static const U32 MZDISK_FILETYPE_BSD_CONV     = 0x00000005;
static const U32 MZDISK_DISKTYPE_MZ2500_2DD   = 0x00000000;
static const U32 MZDISK_DISKTYPE_MZ2500_2D40  = 0x00000001;
static const U32 MZDISK_DISKTYPE_MZ2500_2D35  = 0x00000002;
static const U32 MZDISK_DISKTYPE_MZ2000_2D35  = 0x00000003;

// --------------------	構造体

// アライメントの設定をバイト単位にする
#pragma pack( push, 1 )

// D88 ヘッダ構造体
typedef struct tagD88HEADER
{
	U8 Name[ 17 ];
	U8 Reserve[ 9 ];
	U8 Protect;
	U8 DiskType;
	U32 DiskSize;
	U32 TrackTable[ 164 ];
} D88HEADER;

// D88 セクタ構造体
typedef struct tagD88SECTOR
{
	U8 C;	// シリンダ (0～)
	U8 H;	// ヘッド (0～1)
	U8 R;	// セクタ番号 (1～)
	U8 N;	// セクタ長 (0=128, 1=256, 2=512 ～)
	U16 SectorCount;
	U8 Density;
	U8 DeleteData;
	U8 Status;
	U8 Reserve[ 5 ];
	U16 SectorSize;
} D88SECTOR;

// ディレクトリ構造
typedef struct tagDIRECTORY
{
	U8 Mode;
	char Filename[ 17 ];
	U8 Attr;
	U8 Reserve;
	U16 Size;
	U16 LoadAdr;
	U16 RunAdr;
	U32 Date;
	U16 StartSector;
} DIRECTORY;

// IPL 構造
typedef struct tagIPL
{
	U8 Machine;
	char Signature[ 6 ];
	char Bootname[ 11 ];
	U8 Reserve[ 2 ];
	U16 Size;
	U16 Reserve2;
	U16 RunAdr;
	U32 Reserve3;
	U16 StartSector;
} IPL;

// テープヘッダ構造
typedef struct tagMZTHEAD
{
	U8 Mode;
	char Filename[ 17 ];
	U16 Size;
	U16 LoadAdr;
	U16 RunAdr;
	U8 Reserve[ 104 ];
} MZTHEAD;

// アライメントの設定を元に戻す
#pragma pack( pop )

// -------------------- クラス定義
class cMzDisk
{
	public:
		cMzDisk();
		~cMzDisk();
		void Format( U32 type );
		int Load( char *image, U32 type );
		int Save( char *image, U32 type );
		int GetFile( int dirindex, char *path, U32 mode );
		int PutFile( char *path, DIRECTORY *dirinfo, U32 mode, U32 type = MZDISK_FILETYPE_OBJ );
		int DelFile( int dirindex );
		int GetBoot( char *path, U32 mode );
		int PutBoot( char *path, IPL *iplinfo, U32 mode );
		int GetBitmapSize( void );
		int GetBitmap( void );
		int GetBitmapSerial( int length );
		void SetBitmap( int start, int length );
		void DelBitmap( int start, int length );
		void SetDirSector( int sector );
		int GetDirSector( void );
		void GetDir( DIRECTORY *dirdata, int dirindex );
		int GetSize( U32 type );
		void GetSector( char *buffer, int sector );
		int GetUseBlockSize( void );
		int GetAllBlockSize( void );
		int GetClusterSize( void );
	private:
		unsigned char *DiskImage;
		int DiskImageSize;
		U8 DiskType;
		U8 *Bitmap;
		int CylCount;
		int FileType;
		int ClusterSize;
		int MaxCluster;
		int DirSector;
		cMzDisk( cMzDisk& mzdisk );
		cMzDisk& operator = ( cMzDisk &mzdisk );
		void GetExtFilename( char *path, char *extfilename );
		void DeleteFileExtname( char *path );
		void WriteUseSize( void );
};

// -------------------- 2 重定義防止
#endif
