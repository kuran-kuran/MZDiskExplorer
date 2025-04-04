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
	memset( Drive, 0, PATH_STRING_MAX );
	memset( Dir, 0, PATH_STRING_MAX );
	memset( Name, 0, PATH_STRING_MAX );
	memset( ExtName, 0, PATH_STRING_MAX );
	memset( Path, 0, PATH_STRING_MAX );
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
	memset( Drive, 0, PATH_STRING_MAX );
	memset( Dir, 0, PATH_STRING_MAX );
	memset( Name, 0, PATH_STRING_MAX );
	memset( ExtName, 0, PATH_STRING_MAX );
	memset( Path, 0, PATH_STRING_MAX );
}

//============================================================================
//  ドライブ設定
//----------------------------------------------------------------------------
// In  : drive = ドライブ
//============================================================================
void cPath::SetDrive( const char *drive )
{
	StrCopy( Drive, sizeof(Drive), drive );
	MakePath();
}

//============================================================================
//  ディレクトリ設定
//----------------------------------------------------------------------------
// In  : dir = ディレクトリパス
//============================================================================
void cPath::SetDir( const char *dir )
{
	StrCopy( Dir, sizeof(Dir), dir );
	MakePath();
}

//============================================================================
//  ファイルネーム設定
//----------------------------------------------------------------------------
// In  : name = ファイルネーム
//============================================================================
void cPath::SetName( const char *name )
{
	StrCopy( Name, sizeof(Name), name );
	MakePath();
}

//============================================================================
//  拡張子設定
//----------------------------------------------------------------------------
// In  : extname = 拡張子
//============================================================================
void cPath::SetExtName( const char *extname )
{
	StrCopy( ExtName, sizeof(ExtName), extname );
	MakePath();
}

//============================================================================
//  拡張子追加
//----------------------------------------------------------------------------
// In  : extname = 拡張子
// ex)
// extname: bas
// srcpath: name: test,     ext: bin
// dstpath: name: test.bin, ext: bas
//============================================================================
void cPath::AddExtName( const char *extname )
{
	if(strlen(ExtName) > 0)
	{
		StrAdd( Name, sizeof(Name), "." );
		StrAdd( Name, sizeof(Name), ExtName );
		StrCopy( ExtName, strlen(ExtName), extname );
	}
	MakePath();
}

//============================================================================
//  パス設定
//----------------------------------------------------------------------------
// In  : path = パス
//============================================================================
void cPath::SetPath( const char *path )
{
	Release();
	StrCopy( Path, sizeof(Path), path );
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
	_splitpath_s(Path, Drive, PATH_STRING_MAX, Dir, PATH_STRING_MAX, Name, PATH_STRING_MAX, ExtName, PATH_STRING_MAX);
	if(ExtName[0] == '.')
	{
		// .を消す
		StrCopy(ExtName, sizeof(ExtName), &ExtName[1]);
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
		StrCopy( Path, sizeof(Path), Drive );
	} else {
		StrCopy( Path, sizeof(Path), "" );
	}
	if ( ( mode & PATH_MODE_DIR ) && ( Dir != 0 ) )
	{
		StrAdd( Path, sizeof(Path), Dir );
		if ( strlen( Path ) > 0 )
		{
			char dividechar[ 2 ];
			dividechar[ 0 ] = PATH_DIVIDE_CHAR;
			dividechar[ 1 ] = '\0';
			if ( Path[ strlen( Path ) -1 ] != PATH_DIVIDE_CHAR )
			{
				StrAdd( Path, sizeof(Path), dividechar );
			}
		}
	}
	if ( ( mode & PATH_MODE_NAME ) && ( Name != 0 ) )
	{
		StrAdd( Path, sizeof(Path), Name );
	}
	if ( ( mode & PATH_MODE_EXTNAME ) && ( ExtName != 0 ) )
	{
		if ( *ExtName != '\0' )
		{
			StrAdd( Path, sizeof(Path), "." );
			StrAdd( Path, sizeof(Path), ExtName );
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
char* cPath::StrCopy( char *dest, size_t dest_size, const char *source )
{
	strcpy_s( dest, dest_size, source );
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
char* cPath::StrCopyPart( char *dest, const char *source, int sourcestart, int sourceend )
{
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
char* cPath::StrAdd( char *dest, size_t dest_size, const char *source )
{
	strcat_s( dest, dest_size, source );
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
char* cPath::StrAddPart( char *dest, const char *source, int sourcestart, int sourceend )
{
	size_t destindex = strlen( dest );
	for (size_t sourceindex = sourcestart; sourceindex <= sourceend; sourceindex ++ )
	{
		dest[ destindex ] = source[ sourceindex ];
		destindex ++;
	}
	dest[ destindex ] = '\0';
	return dest;
}
