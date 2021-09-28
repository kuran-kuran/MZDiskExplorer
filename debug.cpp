//============================================================================
//  debug.cpp �u�f�o�b�O����v PS2EE/Win32 ��
//----------------------------------------------------------------------------
//													Programmed by kuran_kuran
//============================================================================
#include "stdafx.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#if defined _WIN32
#include <windows.h>
#if DEBUG_DIRECTX == 8
#include <dxerr8.h>
#elif DEBUG_DIRECTX == 9
#include <dxerr9.h>
#endif
#elif defined R5900
#include <sifdev.h>
#elif defined R3000
#include <sys/file.h>
#include <kernel.h>
#endif
#include "generic.h"

// -------------------- �f�o�b�O����ꍇ�̂݃R���p�C������
#if DEBUG_MODE != 0

// -------------------- ��`����������
#if DEBUG_MODE == 1
#undef new
#undef malloc
#undef free
#endif

// -------------------- �X�^�e�B�b�N�̈�
cDebug DebugMacro( DEBUG_OUTPUT );

#if DEBUG_MODE == 1
//===========================================================================
//  new
// ��) ��O�͕Ԃ��܂���
//===========================================================================
void* operator new( size_t size, const char* file, int line )
{
	void* ptr = DebugMacro.Malloc( size, file, line );
//	if ( NULL == ptr ) {
//		throw 1;
//	}
	return ptr;
}

//===========================================================================
//  new[]
// ��) ��O�͕Ԃ��܂���
//===========================================================================
void* operator new[]( size_t size, const char* file, int line )
{
	void* ptr = DebugMacro.Malloc( size, file, line );
//	if ( NULL == ptr ) {
//		throw 1;
//	}
	return ptr;
}

//===========================================================================
//  delete
//===========================================================================
void operator delete( void* ptr )
{
	DebugMacro.Free( ptr );
}

//===========================================================================
//  delete[]
//===========================================================================
void operator delete[]( void* ptr )
{
	DebugMacro.Free( ptr );
}
#endif

//===========================================================================
//  cDebug �N���X
//===========================================================================

// -------------------- �X�^�e�B�b�N�ϐ�
int cDebug::StatCount = 0;
int cDebug::MallocHandle = 0;
const void *cDebug::MallocAddress[ DEBUG_MEMORY_MAX ];
const char *cDebug::MallocFile[ DEBUG_MEMORY_MAX ];
int cDebug::MallocLine[ DEBUG_MEMORY_MAX ];
int cDebug::MallocSize[ DEBUG_MEMORY_MAX ];

//===========================================================================
//  �R���X�g���N�^
//===========================================================================
cDebug::cDebug()
:OutputType( 0 )
{
	StatCount++;
}

//===========================================================================
//  �R���X�g���N�^
//---------------------------------------------------------------------------
// In  : type
//     :   0 = �W���o��(PS2), �f�o�b�O�E�C���h�E(Win32)
//     :   1 = �t�@�C��
//===========================================================================
cDebug::cDebug( int type )
:OutputType( type )
{
	if ( 0 == StatCount ) {
		DeleteFile();
	}
	StatCount++;
}

//===========================================================================
//  �f�X�g���N�^
//===========================================================================
cDebug::~cDebug()
{
	StatCount--;
}

//===========================================================================
//  �o�͐��I������
//---------------------------------------------------------------------------
// In  : type
//     :   0 = �W���o��(PS2), �f�o�b�O�E�C���h�E(Win32)
//     :   1 = �t�@�C��
//===========================================================================
void cDebug::SetOutputType( int type )
{
	OutputType = type;
}

//===========================================================================
//  �f�o�b�O�t�@�C�����폜����
//===========================================================================
void cDebug::DeleteFile( void )
{
	#if defined _WIN32
		unlink( DEBUG_FILE );
	#endif
}

//============================================================================
//	�f�o�b�O���b�Z�[�W���o�͂���
//----------------------------------------------------------------------------
//In  : outputstring = �o�͕�����
//============================================================================
void cDebug::Output( const char* outputstring, ... ) const
{
	char str[ 1000 ];
	va_list arglist;
	va_start( arglist, outputstring );
	vsprintf( str, outputstring, arglist );
	va_end( arglist );
	if ( 0 == OutputType ) {
		#if defined _WIN32
			OutputDebugString( (LPCTSTR)str );
		#else
			printf( str );
		#endif
	} else if ( 1 == OutputType ) {
		#if defined R3000
			int fp;
			// �t�@�C���I�[�v��
			fp = open( DEBUG_FILE, O_CREAT | O_WRONLY );
			if ( fp >= 0 ) {
				lseek( fp, 0, SEEK_END );
				write( fp, str, strlen( str ) + 1 );
				close( fp );
			}
		#elif defined R5900
			int fp;
			// �t�@�C���I�[�v��
			fp = sceOpen( DEBUG_FILE, ( SCE_CREAT | SCE_WRONLY ), 0777 );
			if ( fp >= 0 ) {
				sceLseek( fp, 0, SCE_SEEK_END );
				sceWrite( fp, str, strlen( str ) + 1 );
				sceClose( fp );
			}
		#else
			FILE *fp;
			// �t�@�C���I�[�v��
			fp = fopen( DEBUG_FILE, "r+" );
			if ( NULL == fp ) {
				// �t�@�C���쐬
				fp = fopen( DEBUG_FILE, "w+" );
			}
			if ( fp != NULL ) {
				fseek( fp, 0L, SEEK_END );
				fwrite( str, strlen( str ), 1, fp );
				fclose( fp );
			}
		#endif
	}
}

//============================================================================
//	���������_���v�o�͂���
//----------------------------------------------------------------------------
//In  : mode
//    :   DEBUG_DUMP8  1 �o�C�g�\��
//    :   DEBUG_DUMP16 2 �o�C�g�\��
//    :   DEBUG_DUMP32 4 �o�C�g�\��
//============================================================================
void cDebug::OutputDump( const void *address, int width, int height, U32 mode ) const
{
	int x, y;
	const U8 *dump_address = (U8*)address;
	for ( y=0; y<height; y++ ) {
		Output( "%08X", (U32)dump_address );
		Output( " : " );
		for ( x=0; x<width; x++ ) {
			if ( mode & DEBUG_DUMP8 ) {
				Output( "%02X", *(U8*)dump_address );
				dump_address++;
			} else if ( mode & DEBUG_DUMP16 ) {
				Output( "%04X", *(U16*)dump_address );
				dump_address += 2;
			} else {
				Output( "%08X", *(U32*)dump_address );
				dump_address += 4;
			}
			if ( x != width - 1 ) {
				Output( " " );
			}
		}
		Output( "\n" );
	}
}

//============================================================================
//	DirectX �̃G���[���o�͂���
//----------------------------------------------------------------------------
//In  : result = DirectX �̃G���[�R�[�h�ւ̃|�C���^
//============================================================================
void cDebug::OutputDxResult( void *result ) const
{
	#if defined _WIN32
	#if DEBUG_DIRECTX != 0
	char *str;
	char strtemp[ 100 ];
	#if DEBUG_DIRECTX == 8
	str = (char*)DXGetErrorString8( *(HRESULT*)result );
	#elif DEBUG_DIRECTX == 9
	str = (char*)DXGetErrorString9( *(HRESULT*)result );
	#endif
	sprintf( strtemp, "�߂�l = 0x%08X ( %s )\n", (HRESULT*)result, str );
	Output( strtemp );
	#endif
	#endif
}

//============================================================================
//	�������m�� malloc �݊�
//----------------------------------------------------------------------------
//In  : size = �m�ۂ���o�C�g��
//    : file = �������m�ۂ��Ă���t�@�C����
//    : line = �������m�ۂ��Ă���s
//============================================================================
void* cDebug::Malloc( int size, const char *file, int line )
{
	void *memory;
	#if defined R3000
	memory = AllocSysMemory( SMEM_Low, (unsigned long)size, NULL );
	#else
	memory = malloc( size );
	#endif
	MallocAddress[ MallocHandle ] = memory;
	MallocSize[ MallocHandle ] = size;
	MallocFile[ MallocHandle ] = file;
	MallocLine[ MallocHandle ] = line;
	MallocHandle++;
	if ( MallocHandle > DEBUG_MEMORY_MAX ) {
		MallocHandle = 0;
	}
	return( memory );
}

//============================================================================
//	�������J�� free �݊�
//============================================================================
void cDebug::Free( void *memblock )
{
	int i;
	for ( i=0; i<DEBUG_MEMORY_MAX; i++ ) {
		if ( MallocAddress[ i ] == memblock ) {
			MallocAddress[ i ] = NULL;
			break;
		}
	}
	#if defined R3000
	FreeSysMemory( memblock );
	#else
	free( memblock );
	#endif
}

//============================================================================
//	�������Ǘ��̃`�F�b�N
// ��) �t�@�C������p�X���� \ ���܂ފ���������ꍇ�͓��삵�܂���.
//============================================================================
void cDebug::MemoryStatus( void )
{
	char str[ 1024 ];
	int i;
	int sum = 0;
	Output( "���������̊J����\n" );
	for ( i=0; i<DEBUG_MEMORY_MAX; i++ ) {
		if ( MallocAddress[ i ] != NULL ) {
			char file[ 1024 ];
			bool find = false;
			int srcindex = strlen( MallocFile[ i ] ) - 1;
			while( 0 <= srcindex-- ) {
				if ( ( '\\' == MallocFile[ i ][ srcindex ] ) ||
					 ( '/' == MallocFile[ i ][ srcindex ] ) ) {
					find = true;
					srcindex++;
					break;
				}
			}
			if ( find ) {
				strcpy( file, &MallocFile[ i ][ srcindex ] );
			} else {
				strcpy( file, MallocFile[ i ] );
			}
			sprintf( str, "%s �� %d �s�ڂŊm�ۂ��ꂽ %d �o�C�g(%p) �͖��J���ł�.\n",
					 file,
					 MallocLine[ i ],
					 MallocSize[ i ],
					 MallocAddress[ i ] );
			sum += MallocSize[ i ];
			Output( str );
		}
	}
	sprintf( str, "�J�����Ă��Ȃ��������� %d �o�C�g�ł�.\n", sum );
	Output( str );
}

// -------------------- �f�o�b�O����ꍇ�̂݃R���p�C������
#endif
