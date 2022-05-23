//============================================================================
//  �t�@�C������
//----------------------------------------------------------------------------
//													Programmed by ����
//============================================================================
#include "stdafx.h"
#include <string.h>
#include <memory.h>
#include "generic.h"
#include "path.h"

#define IsKanji(c) ( (unsigned char)((int)((unsigned char)(c) ^ 0x20) - 0x0A1) < 0x3C )

//============================================================================
//  �R���X�g���N�^
//============================================================================
cPath::cPath()
{
#if PATH_STRING_MAX == 0
	Drive = 0;
	Dir = 0;
	Name = 0;
	ExtName = 0;
	Path = 0;
#else
	memset( Drive, 0, PATH_STRING_MAX );
	memset( Dir, 0, PATH_STRING_MAX );
	memset( Name, 0, PATH_STRING_MAX );
	memset( ExtName, 0, PATH_STRING_MAX );
	memset( Path, 0, PATH_STRING_MAX );
#endif
	Dummy = 0;
}

//============================================================================
//  �f�X�g���N�^
//============================================================================
cPath::~cPath()
{
	Release();
}

//============================================================================
//  �p�X�J��
//----------------------------------------------------------------------------
// In  : drive = �h���C�u
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
	memset( Drive, 0, PATH_STRING_MAX );
	memset( Dir, 0, PATH_STRING_MAX );
	memset( Name, 0, PATH_STRING_MAX );
	memset( ExtName, 0, PATH_STRING_MAX );
	memset( Path, 0, PATH_STRING_MAX );
#endif
}

//============================================================================
//  �h���C�u�ݒ�
//----------------------------------------------------------------------------
// In  : drive = �h���C�u
//============================================================================
void cPath::SetDrive( char *drive )
{
	StrCopy( Drive, drive );
	MakePath();
}

//============================================================================
//  �f�B���N�g���ݒ�
//----------------------------------------------------------------------------
// In  : dir = �f�B���N�g���p�X
//============================================================================
void cPath::SetDir( char *dir )
{
	StrCopy( Dir, dir );
	MakePath();
}

//============================================================================
//  �t�@�C���l�[���ݒ�
//----------------------------------------------------------------------------
// In  : name = �t�@�C���l�[��
//============================================================================
void cPath::SetName( char *name )
{
	StrCopy( Name, name );
	MakePath();
}

//============================================================================
//  �g���q�ݒ�
//----------------------------------------------------------------------------
// In  : extname = �g���q
//============================================================================
void cPath::SetExtName( char *extname )
{
	StrCopy( ExtName, extname );
	MakePath();
}

//============================================================================
//  �p�X�ݒ�
//----------------------------------------------------------------------------
// In  : path = �p�X
//============================================================================
void cPath::SetPath( char *path )
{
	Release();
	StrCopy( Path, path );
	DividePath();
}

//============================================================================
//  �p�X�擾
//----------------------------------------------------------------------------
// Out  : �p�X�ւ̃|�C���^
//============================================================================
char* cPath::GetPath( unsigned int mode )
{
	MakePath( mode );
	return Path;
}

//============================================================================
//  �h���C�u���擾
//----------------------------------------------------------------------------
// Out  : �h���C�u���ւ̃|�C���^
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
//  �f�B���N�g�����擾
//----------------------------------------------------------------------------
// Out  : �f�B���N�g�����ւ̃|�C���^
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
//  �t�@�C�����擾
//----------------------------------------------------------------------------
// Out  : �t�@�C�����ւ̃|�C���^
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
//  �g���q�擾
//----------------------------------------------------------------------------
// Out  : �p�X�ւ̃|�C���^
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
//  �f�B���N�g���A�t�@�C���l�[���A�g���q�ɕ�����
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
		// .������
		StrCopy(ExtName, &ExtName[1]);
	}
	return 0;
}

//============================================================================
//  �p�X���쐬����
//============================================================================
void cPath::MakePath( unsigned int mode )
{
	if ( ( mode & PATH_MODE_DRIVE ) && ( Drive != 0 ) )
	{
		StrCopy( Path, Drive );
	} else {
		StrCopy( Path, "" );
	}
	if ( ( mode & PATH_MODE_DIR ) && ( Dir != 0 ) )
	{
		StrAdd( Path, Dir );
		if ( strlen( Path ) > 0 )
		{
			char dividechar[ 2 ];
			dividechar[ 0 ] = PATH_DIVIDE_CHAR;
			dividechar[ 1 ] = '\0';
			if ( Path[ strlen( Path ) -1 ] != PATH_DIVIDE_CHAR )
			{
				StrAdd( Path, dividechar );
			}
		}
	}
	if ( ( mode & PATH_MODE_NAME ) && ( Name != 0 ) )
	{
		StrAdd( Path, Name );
	}
	if ( ( mode & PATH_MODE_EXTNAME ) && ( ExtName != 0 ) )
	{
		if ( *ExtName != '\0' )
		{
			StrAdd( Path, "." );
			StrAdd( Path, ExtName );
		}
	}
}

//============================================================================
//  ��������R�s�[����
//----------------------------------------------------------------------------
// In  : dest = �R�s�[��
//     : source = �R�s�[��
// Out : dest �̃|�C���^
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
//  ������̕������R�s�[����
//----------------------------------------------------------------------------
// In  : dest = �R�s�[��
//     : source = �R�s�[��
//     : sourcestart = �R�s�[���̍ŏ��̈ʒu
//     : sourceend = �R�s�[���̍Ō�̈ʒu
// Out : dest �̃|�C���^
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
//  �������ǉ�����
//----------------------------------------------------------------------------
// In  : dest = �R�s�[��
//     : source = �R�s�[��
// Out : dest �̃|�C���^
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
//  ������̕�����ǉ�����
//----------------------------------------------------------------------------
// In  : dest = �R�s�[��
//     : source = �R�s�[��
//     : sourcestart = �R�s�[���̍ŏ��̈ʒu
//     : sourceend = �R�s�[���̍Ō�̈ʒu
// Out : dest �̃|�C���^
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
