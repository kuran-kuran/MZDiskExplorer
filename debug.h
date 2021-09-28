//============================================================================
//  debug.h �u�f�o�b�O����w�b�_�t�@�C���v PS2EE/Win32 ��
//----------------------------------------------------------------------------
//													Programmed by kuran_kuran
//----------------------------------------------------------------------------
// 2003.05.06 Ver.1.02 PS2EE �Ńt�@�C���������݂ɑΉ�����.
// 2003.02.21 Ver.1.01 �����J�E���^�� 0 �̎������t�@�C�����폜����悤�ɂ���.
//                     �������֌W�̃f�o�b�O�p�Ƀt�@�C�����ƍs�����L������.
//                     new/delete ���I�[�o�[���[�h����.
// 2003.02.20 Ver.1.00 �쐬�J�n.
//============================================================================

// -------------------- 2 �d��`�h�~
#ifndef DEFINE_DEBUG
#define DEFINE_DEBUG

// -------------------- �w�b�_�t�@�C���o�^
#if defined R5900
#include <stdio.h>
#endif
#include "generic.h"

// �f�o�b�O (0=���Ȃ�,1=����,2=�o�͂̂�)
#define DEBUG_MODE 2
// �o�͐� (0=�W���o��(PS2),�f�o�b�O�E�C���h�E(Win32),1=�t�@�C��)
#define DEBUG_OUTPUT 1
// DirectX �̃o�[�W���� (0=�f�o�b�O���Ȃ�,8 �� 9 �̂ݎw��\)
#define DEBUG_DIRECTX 0

// -------------------- �����N���C�u�����̎w��
#if defined _WIN32
#if DEBUG_DIRECTX == 8
#pragma comment( lib, "dxerr8.lib" )
#elif DEBUG_DIRECTX == 9
#pragma comment( lib, "dxerr9.lib" )
#endif
#endif

// -------------------- �萔��`
static const char* const DEBUG_VERSION = "1.02";
static const char* const DEBUG_NAME = "cDebug";
static const int DEBUG_MEMORY_MAX = 65536;					// �������m�ۊĎ��ő吔
#if defined R5900
static const char* const DEBUG_FILE = "host0:DEBUG.TXT";	// PS2EE �f�o�b�O�o�͎��̃t�@�C����
#else
static const char* const DEBUG_FILE = "DEBUG.TXT";			// �f�o�b�O�o�͎��̃t�@�C����
#endif
static const U32 DEBUG_10 = 0;
static const U32 DEBUG_16 = 1;
static const U32 DEBUG_DUMP8  = 0x00000001;
static const U32 DEBUG_DUMP16 = 0x00000002;
static const U32 DEBUG_DUMP32 = 0x00000004;

#if DEBUG_MODE == 1
// -------------------- �I�[�o�[���C�h
void* operator new( size_t size, const char* file, int line );
void* operator new[]( size_t size, const char* file, int line );
void operator delete( void* ptr );
void operator delete[]( void* ptr );
#endif

#if DEBUG_MODE != 0
// -------------------- �N���X��`
class cDebug {
	private:
		// �풓��
		static int StatCount;
		// �o��
		int OutputType;
		// �������Ǘ�
		static int MallocHandle;
		static const void *MallocAddress[ DEBUG_MEMORY_MAX ];
		static const char *MallocFile[ DEBUG_MEMORY_MAX ];
		static int MallocLine[ DEBUG_MEMORY_MAX ];
		static int MallocSize[ DEBUG_MEMORY_MAX ];
		// ����֎~
		cDebug( cDebug& debug );
		cDebug& operator = ( const cDebug &debug );
	public:
		cDebug( void );
		cDebug( int type );
		~cDebug( void );
		// �f�o�b�O�o��
		void DeleteFile( void );
		void SetOutputType( int type );
		void Output( const char* outputstring, ... ) const;
		void OutputDump( const void *address, int width, int height, U32 mode ) const;
		// DirectX
		void OutputDxResult( void *result ) const;
		// �������Ǘ�
		void* Malloc( int size, const char *file, int line );
		void Free( void *memblock );
		void MemoryStatus( void );
};
#endif

// -------------------- �f�o�b�O���Ȃ�
#if DEBUG_MODE == 0
#define DEBUGINIT( x )             ;
#define DEBUGOUTMODE( x )          ;
#define DEBUGOUT( x )              ;
#define DEBUGOUTDUMP( a, w, h )    ;
#define DEBUGDX( x )               ;
#define MEMORYSTATE()              ;
// -------------------- ���ׂ�
#elif DEBUG_MODE == 1
#define DEBUGINIT( x )             cDebug DebugMacro( x )
#define DEBUGOUTTYPE( x )          DebugMacro.OutputType( x )
#define DEBUGOUT                   DebugMacro.Output
#define DEBUGOUTDUMP( a, w, h )    DebugMacro.OutputDump( a, w, h, DEBUG_DUMP8 )
#define DEBUGDX( x )               DebugMacro.OutputDxResult( (void*)&x )
#define malloc( x )                DebugMacro.Malloc( x, __FILE__, __LINE__ )
#define free( x )                  DebugMacro.Free( x )
#define MEMORYSTATE()              DebugMacro.MemoryStatus()
#define new                        new( __FILE__, __LINE__ )

// -------------------- �o�͂̂�
#elif DEBUG_MODE == 2
#define DEBUGINIT( x )             cDebug DebugMacro( x )
#define DEBUGOUTTYPE( x )          DebugMacro.OutputType( x )
#define DEBUGOUT                   DebugMacro.Output
#define DEBUGOUTDUMP( a, w, h )    DebugMacro.OutputDump( a, w, h, DEBUG_DUMP8 )
#define DEBUGDX( x )               DebugMacro.OutputDxResult( (void*)&x )
#define MEMORYSTATE()              ;
#endif

// -------------------- ���̃t�@�C������Q�Ɖ�
extern cDebug DebugMacro;

// -------------------- 2 �d��`�h�~
#endif
