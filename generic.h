//============================================================================
//  generic.h �^��`�w�b�_�t�@�C��  PS2EE/PS2IOP/PS1/GBA/�|�P�X�e/Win32 ��
//----------------------------------------------------------------------------
//													Programmed by ����
//----------------------------------------------------------------------------
//�� ���g���G���f�B�A����p (x86�AEE�AIOP�AARM �Ȃ�)
//   �|�P�X�e�͖�����
//----------------------------------------------------------------------------
// 2003.04.24 Ver.1.04 NULL ���`�����B
// 2003.04.08 Ver.1.03 GBA �ɑΉ��B
// 2003.02.20 Ver.1.02 bool ��`�B
// 2003.02.20 Ver.1.01 IOP �ɑΉ��B�ׂ����œK���B
// 2003.02.14 Ver.1.00 ktDefine.h �̃t�@�C������ύX�BPS2EE �ɑΉ��B
//============================================================================

// -------------------- 2 �d��`�h�~
#ifndef GENERIC_INCLUDE
#define GENERIC_INCLUDE

// -------------------- �@��I��
#define GBA
//#define POCKETSTATION

// -------------------- �w�b�_�t�@�C���o�^
#if defined _WIN32
	#include <windows.h>
#elif defined R5900
	#include <eetypes.h>
#elif defined R3000
	#include <sys/types.h>
#endif

// -------------------- �萔��`
static const char* const GENERIC_VERSION = "1.04";
static const char* const GENERIC_NAME = "Genelic";

// -------------------- �@��ʒ�`
#if defined _WIN32
	// Win32 �^��`
	#define S8   char				// �����t�� 8 �r�b�g
	#define S16  short				// �����t�� 16 �r�b�g
	#define S32  long				// �����t�� 32 �r�b�g
	#define S64  __int64			// �����t�� 64 �r�b�g
#elif defined R5900
	// PS2EE �^��`
	#define S8   char				// �����t�� 8 �r�b�g
	#define S16  short				// �����t�� 16 �r�b�g
	#define S32  int				// �����t�� 32 �r�b�g
	#define S64  long				// �����t�� 64 �r�b�g
	#define S128 long128			// �����t�� 128 �r�b�g
#elif defined R3000
	// PS1/PS2IOP �^��`
	#define S8   char				// �����t�� 8 �r�b�g
	#define S16  short				// �����t�� 16 �r�b�g
	#define S32  int				// �����t�� 32 �r�b�g
	#define S64  long				// �����t�� 64 �r�b�g
#elif defined GBA
	// GBA �^��`
	#define S8   char				// �����t�� 8 �r�b�g
	#define S16  short				// �����t�� 16 �r�b�g
	#define S32  long				// �����t�� 32 �r�b�g
#endif

// ��L�ɏ]���Ē�`
#ifdef S8
#define U8      unsigned S8			// 8 �r�b�g
#define PS8     S8*					// �����t�� 8 �r�b�g
#define PU8     U8*					// 8 �r�b�g
#define BYTE    U8					// 8 �r�b�g
#define LPBYTE  PU8					// 8 �r�b�g
#endif
#ifdef S16
#define U16     unsigned S16		// 16 �r�b�g
#define PS16    S16*				// �����t�� 16 �r�b�g
#define PU16    U16*				// 16 �r�b�g
#define WORD    U16					// 16 �r�b�g
#define LPWORD  PU16				// 16 �r�b�g
#endif
#ifdef S32
#define U32     unsigned S32		// 32 �r�b�g
#define PS32    S32*				// �����t�� 32 �r�b�g
#define PU32    U32*				// 32 �r�b�g
#define DWORD   U32					// 32 �r�b�g
#define LPDWORD PU32				// 32 �r�b�g
#endif
#ifdef S64
#define U64     unsigned S64		// 64 �r�b�g
#define PS64    S64*				// �����t�� 64 �r�b�g
#define PU64    U64*				// 64 �r�b�g
#endif
#ifdef S128
#if defined R5900
#define U128    u_long128			// 128 �r�b�g
#else
#define U128    unsigned S128		// 128 �r�b�g
#endif
#define PS128   S128*				// �����t�� 128 �r�b�g
#define PU128   U128*				// 128 �r�b�g
#endif

// BOOL ��`
#ifndef BOOL
#define BOOL  int
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif
// bool ��`
#ifndef bool
#define bool  int
#endif
#ifndef false
#define false 0
#endif
#ifndef true
#define true  1
#endif

// NULL ��`
#ifndef NULL
#define NULL 0
#endif

// -------------------- �\���́E���p�̒�`
// 128 �r�b�g�f�[�^
typedef union {
#ifdef S128
	U128 u128;
#endif
#ifdef S64
	U64 u64[2];
#endif
#ifdef S32
	U32 u32[4];
#endif
#ifdef S16
	U16 u16[8];
#endif
#ifdef S8
	U8 u8[16];
#endif
} U128DATA, *PU128DATA;

// 64 �r�b�g�f�[�^
typedef union {
#ifdef S64
	U64 u64;
#endif
#ifdef S32
	U32 u32[2];
#endif
#ifdef S16
	U16 u16[4];
#endif
#ifdef S8
	U8 u8[8];
#endif
} U64DATA, *PU64DATA;

// 32 �r�b�g�f�[�^
typedef union {
#ifdef S32
	U32 u32;
#endif
#ifdef S16
	U16 u16[2];
#endif
#ifdef S8
	U8 u8[4];
#endif
} U32DATA, *PU32DATA;

// 16 �r�b�g�f�[�^
typedef union {
#ifdef S16
	U16 u16;
#endif
#ifdef S8
	U8 u8[2];
#endif
} U16DATA, *PU16DATA;

// -------------------- 2 �d��`�h�~
#endif
