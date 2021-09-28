//============================================================================
//  path.h パス操作
//----------------------------------------------------------------------------
//													Programmed by たご
//----------------------------------------------------------------------------
// 2003.05.20 Ver.0.01 作成開始。
// 2003.05.21 Ver.1.00 完成。
//============================================================================

// -------------------- 2 重定義防止
#ifndef PATH_INCLUDE
#define PATH_INCLUDE

// -------------------- ヘッダファイル登録

// -------------------- 定数定義
static const char* const  PATH_VERSION = "1.00";
static const char* const  PATH_NAME = "cPath";
#if defined _WIN32
static const char         PATH_DIVIDE_CHAR = '\\';
#else
static const char         PATH_DIVIDE_CHAR = '/';
#endif
static const unsigned int PATH_MODE_DRIVE   = 0x00000001;
static const unsigned int PATH_MODE_DIR     = 0x00000002;
static const unsigned int PATH_MODE_NAME    = 0x00000004;
static const unsigned int PATH_MODE_EXTNAME = 0x00000008;
static const unsigned int PATH_MODE_ALL     = 0x0000000F;

// -------------------- 動作指定
#define PATH_STRING_MAX    0    // 0=可変, 0以外=スタティックメモリサイズ (260推奨)

// -------------------- クラス定義
class cPath {
	public:
		cPath();
		~cPath();
		void SetDrive( char *drive );
		void SetDir( char *dir );
		void SetName( char *name );
		void SetExtName( char *extname );
		void SetPath( char *path );
		char* GetPath( unsigned int mode = PATH_MODE_ALL );
		char* GetDrive( void );
		char* GetDir( void );
		char* GetName( void );
		char* GetExtName( void );
		void Release( void );
	private:
		cPath( cPath& path );
		cPath& operator = ( cPath &path );
		int DividePath( void );
		void MakePath( unsigned int mode = PATH_MODE_ALL );
		char* StrCopy( char *dest, char *source );
		char* StrCopyPart( char *dest, char *source, int sourcestart, int sourceend );
		char* StrAdd( char *dest, char *source );
		char* StrAddPart( char *dest, char *source, int sourcestart, int sourceend );
#if PATH_STRING_MAX == 0
		char *Drive;
		char *Dir;
		char *Name;
		char *ExtName;
		char *Path;
#else
		char Drive[ PATH_STRING_MAX ];
		char Dir[ PATH_STRING_MAX ];
		char Name[ PATH_STRING_MAX ];
		char ExtName[ PATH_STRING_MAX ];
		char Path[ PATH_STRING_MAX ];
#endif
		char Dummy;
};

// -------------------- 2 重定義防止
#endif
