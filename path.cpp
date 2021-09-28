//============================================================================
//  ファイル操作
//----------------------------------------------------------------------------
//													Programmed by たご
//============================================================================
#include "stdafx.h"
#include <string.h>
#include <memory.h>
#include "generic.h"
#include "path.h"

#define IsKanji(c) ( (unsigned char)((int)((unsigned char)(c) ^ 0x20) - 0x0A1) < 0x3C )

//============================================================================
//  コンストラクタ
//============================================================================
cPath::cPath()
{
#if FILEMANAGER_PATH_MAX == 0
	Drive = 0;
	Dir = 0;
	Name = 0;
	ExtName = 0;
	Path = 0;
#else
	memset( Drive, 0, FILEMANAGER_PATH_MAX );
	memset( Dir, 0, FILEMANAGER_PATH_MAX );
	memset( Name, 0, FILEMANAGER_PATH_MAX );
	memset( ExtName, 0, FILEMANAGER_PATH_MAX );
	memset( Path, 0, FILEMANAGER_PATH_MAX );
#endif
	Dummy = 0;
}

//============================================================================
//  デストラクタ
//============================================================================
cPath::~cPath()
{
	Release();
}

//============================================================================
//  パス開放
//----------------------------------------------------------------------------
// In  : drive = ドライブ
//============================================================================
void cPath::Release( void )
{
#if PATH_STRING_MAX == 0
	if ( Drive != 0 )
	{
		free( Drive );
		Drive = 0;
	}
	if ( Dir != 0 )
	{
		free( Dir );
		Dir = 0;
	}
	if ( Name != 0 )
	{
		free( Name );
		Name = 0;
	}
	if ( ExtName != 0 )
	{
		free( ExtName );
		ExtName = 0;
	}
	if ( Path != 0 )
	{
		free( Path );
		Path = 0;
	}
#else
	memset( Drive, 0, FILEMANAGER_PATH_MAX );
	memset( Dir, 0, FILEMANAGER_PATH_MAX );
	memset( Name, 0, FILEMANAGER_PATH_MAX );
	memset( ExtName, 0, FILEMANAGER_PATH_MAX );
	memset( Path, 0, FILEMANAGER_PATH_MAX );
#endif
}

//============================================================================
//  ドライブ設定
//----------------------------------------------------------------------------
// In  : drive = ドライブ
//============================================================================
void cPath::SetDrive( char *drive )
{
	Drive = StrCopy( Drive, drive );
	MakePath();
}

//============================================================================
//  ディレクトリ設定
//----------------------------------------------------------------------------
// In  : dir = ディレクトリパス
//============================================================================
void cPath::SetDir( char *dir )
{
	Dir = StrCopy( Dir, dir );
	MakePath();
}

//============================================================================
//  ファイルネーム設定
//----------------------------------------------------------------------------
// In  : name = ファイルネーム
//============================================================================
void cPath::SetName( char *name )
{
	Name = StrCopy( Name, name );
	MakePath();
}

//============================================================================
//  拡張子設定
//----------------------------------------------------------------------------
// In  : extname = 拡張子
//============================================================================
void cPath::SetExtName( char *extname )
{
	ExtName = StrCopy( ExtName, extname );
	MakePath();
}

//============================================================================
//  パス設定
//----------------------------------------------------------------------------
// In  : path = パス
//============================================================================
void cPath::SetPath( char *path )
{
	Release();
	Path = StrCopy( Path, path );
	DividePath();
}

//============================================================================
//  パス取得
//----------------------------------------------------------------------------
// Out  : パスへのポインタ
//============================================================================
char* cPath::GetPath( unsigned int mode )
{
	MakePath( mode );
	return Path;
}

//============================================================================
//  ドライブ名取得
//----------------------------------------------------------------------------
// Out  : ドライブ名へのポインタ
//============================================================================
char* cPath::GetDrive( void )
{
	if ( Drive )
	{
		return Drive;
	}
	return &Dummy;
}

//============================================================================
//  ディレクトリ名取得
//----------------------------------------------------------------------------
// Out  : ディレクトリ名へのポインタ
//============================================================================
char* cPath::GetDir( void )
{
	if ( Dir )
	{
		return Dir;
	}
	return &Dummy;
}

//============================================================================
//  ファイル名取得
//----------------------------------------------------------------------------
// Out  : ファイル名へのポインタ
//============================================================================
char* cPath::GetName( void )
{
	if ( Name )
	{
		return Name;
	}
	return &Dummy;
}

//============================================================================
//  拡張子取得
//----------------------------------------------------------------------------
// Out  : パスへのポインタ
//============================================================================
char* cPath::GetExtName( void )
{
	if ( ExtName )
	{
		return ExtName;
	}
	return &Dummy;
}

//============================================================================
//  ディレクトリ、ファイルネーム、拡張子に分ける
//============================================================================
int cPath::DividePath( void )
{
	if ( 0 == Path )
	{
		return 1;
	}
	if ( '\0' == *Path )
	{
		return 1;
	}
	int start = 0;
	int i = 0;
	int driveflag = 0;
	int nameflag = 0;
	bool kanji = false;
	while( 1 )
	{
		if ( IsKanji( Path[ i ] ) )
		{
			// 漢字の 1 バイト目
			i ++;
			kanji = true;
			continue;
		}
		if( ':' == Path[ i ] )
		{
			// ドライブ名
			if ( ( 0 == i ) || ( 1 == driveflag ) )
			{
				// 0 文字目、パスの途中に「:」は無効
				return 1;
			}
			Drive = StrCopyPart( Drive, Path, start, i );
			start = i + 1;
			driveflag = 1;
		}
		if( ( '\\' == Path[ i ] ) || ( '/' == Path[ i ] ) )
		{
			if( kanji )
			{
				i ++;
				kanji = false;
				continue;
			}
			// ディレクトリ名
			Dir = StrAddPart( Dir, Path, start, i );
			start = i + 1;
		}
		if( '.' == Path[ i ] )
		{
			// ファイル名
			if ( 1 == nameflag )
			{
				Name = StrAdd( Name, "." );
			}
			Name = StrAddPart( Name, Path, start, i - 1 );
			start = i + 1;
			nameflag = 1;
		}
		if( '\0' == Path[ i ] )
		{
			// ファイル名または拡張子
			if ( 1 == nameflag )
			{
				// 拡張子
				ExtName = StrAddPart( ExtName, Path, start, i - 1 );
			}
			else
			{
				// ファイル名
				Name = StrAddPart( Name, Path, start, i - 1 );
			}
			break;
		}
		kanji = false;
		i ++;
	}
	return 0;
}

//============================================================================
//  パスを作成する
//============================================================================
void cPath::MakePath( unsigned int mode )
{
	if ( ( mode & PATH_MODE_DRIVE ) && ( Drive != 0 ) )
	{
		Path = StrCopy( Path, Drive );
	} else {
		Path = StrCopy( Path, "" );
	}
	if ( ( mode & PATH_MODE_DIR ) && ( Dir != 0 ) )
	{
		Path = StrAdd( Path, Dir );
		if ( strlen( Path ) > 0 )
		{
			char dividechar[ 2 ];
			dividechar[ 0 ] = PATH_DIVIDE_CHAR;
			dividechar[ 1 ] = '\0';
			if ( Path[ strlen( Path ) -1 ] != PATH_DIVIDE_CHAR )
			{
				Path = StrAdd( Path, dividechar );
			}
		}
	}
	if ( ( mode & PATH_MODE_NAME ) && ( Name != 0 ) )
	{
		Path = StrAdd( Path, Name );
	}
	if ( ( mode & PATH_MODE_EXTNAME ) && ( ExtName != 0 ) )
	{
		if ( *ExtName != '\0' )
		{
			Path = StrAdd( Path, "." );
			Path = StrAdd( Path, ExtName );
		}
	}
}

//============================================================================
//  文字列をコピーする
//----------------------------------------------------------------------------
// In  : dest = コピー先
//     : source = コピー元
// Out : dest のポインタ
//============================================================================
char* cPath::StrCopy( char *dest, char *source )
{
#if PATH_STRING_MAX == 0
	if ( dest != 0 )
	{
		free( dest );
	}
	dest = (char*)malloc( strlen( source ) + 1 );
#endif
	strcpy( dest, source );
	return dest;
}

//============================================================================
//  文字列の部分をコピーする
//----------------------------------------------------------------------------
// In  : dest = コピー先
//     : source = コピー元
//     : sourcestart = コピー元の最初の位置
//     : sourceend = コピー元の最後の位置
// Out : dest のポインタ
//============================================================================
char* cPath::StrCopyPart( char *dest, char *source, int sourcestart, int sourceend )
{
#if PATH_STRING_MAX == 0
	if ( dest != 0 )
	{
		free( dest );
	}
	dest = (char*)malloc( sourceend - sourcestart + 2 );
#endif
	int sourceindex;
	int destindex = 0;
	for ( sourceindex = sourcestart; sourceindex <= sourceend; sourceindex ++ )
	{
		dest[ destindex ] = source[ sourceindex ];
		destindex ++;
	}
	dest[ destindex ] = '\0';
	return dest;
}

//============================================================================
//  文字列を追加する
//----------------------------------------------------------------------------
// In  : dest = コピー先
//     : source = コピー元
// Out : dest のポインタ
//============================================================================
char* cPath::StrAdd( char *dest, char *source )
{
#if PATH_STRING_MAX == 0
	if ( dest != 0 )
	{
		char *temp;
		int size;
		size = strlen( dest );
		temp = (char*)malloc( size + 1 );
		strcpy( temp, dest );
		free( dest );
		dest = (char*)malloc( size + strlen( source ) + 1 );
		strcpy( dest, temp );
		free( temp );
	}
	else
	{
		dest = (char*)malloc( strlen( source ) + 1 );
		memset( dest, 0, strlen( source ) + 1 );
	}
#endif
	strcat( dest, source );
	return dest;
}

//============================================================================
//  文字列の部分を追加する
//----------------------------------------------------------------------------
// In  : dest = コピー先
//     : source = コピー元
//     : sourcestart = コピー元の最初の位置
//     : sourceend = コピー元の最後の位置
// Out : dest のポインタ
//============================================================================
char* cPath::StrAddPart( char *dest, char *source, int sourcestart, int sourceend )
{
#if PATH_STRING_MAX == 0
	if ( dest != 0 )
	{
		char *temp;
		int size;
		size = strlen( dest );
		temp = (char*)malloc( size + 1 );
		strcpy( temp, dest );
		free( dest );
		dest = (char*)malloc( size + strlen( source ) + 1 );
		strcpy( dest, temp );
		free( temp );
	}
	else
	{
		dest = (char*)malloc( sourceend - sourcestart + 2 );
		memset( dest, 0, sourceend - sourcestart + 2 );
	}
#endif
	int sourceindex;
	int destindex;
	destindex = strlen( dest );
	for ( sourceindex = sourcestart; sourceindex <= sourceend; sourceindex ++ )
	{
		dest[ destindex ] = source[ sourceindex ];
		destindex ++;
	}
	dest[ destindex ] = '\0';
	return dest;
}
