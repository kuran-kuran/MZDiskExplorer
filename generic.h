//============================================================================
//  generic.h 型定義ヘッダファイル  PS2EE/PS2IOP/PS1/GBA/ポケステ/Win32 版
//----------------------------------------------------------------------------
//													Programmed by たご
//----------------------------------------------------------------------------
//※ リトルエンディアン専用 (x86、EE、IOP、ARM など)
//   ポケステは未完成
//----------------------------------------------------------------------------
// 2003.04.24 Ver.1.04 NULL を定義した。
// 2003.04.08 Ver.1.03 GBA に対応。
// 2003.02.20 Ver.1.02 bool 定義。
// 2003.02.20 Ver.1.01 IOP に対応。細かい最適化。
// 2003.02.14 Ver.1.00 ktDefine.h のファイル名を変更。PS2EE に対応。
//============================================================================

// -------------------- 2 重定義防止
#ifndef GENERIC_INCLUDE
#define GENERIC_INCLUDE

// -------------------- 機種選択
#define GBA
//#define POCKETSTATION

// -------------------- ヘッダファイル登録
#if defined _WIN32
	#include <windows.h>
#elif defined R5900
	#include <eetypes.h>
#elif defined R3000
	#include <sys/types.h>
#endif

// -------------------- 定数定義
static const char* const GENERIC_VERSION = "1.04";
static const char* const GENERIC_NAME = "Genelic";

// -------------------- 機種別定義
#if defined _WIN32
	// Win32 型定義
	#define S8   char				// 符号付き 8 ビット
	#define S16  short				// 符号付き 16 ビット
	#define S32  long				// 符号付き 32 ビット
	#define S64  __int64			// 符号付き 64 ビット
#elif defined R5900
	// PS2EE 型定義
	#define S8   char				// 符号付き 8 ビット
	#define S16  short				// 符号付き 16 ビット
	#define S32  int				// 符号付き 32 ビット
	#define S64  long				// 符号付き 64 ビット
	#define S128 long128			// 符号付き 128 ビット
#elif defined R3000
	// PS1/PS2IOP 型定義
	#define S8   char				// 符号付き 8 ビット
	#define S16  short				// 符号付き 16 ビット
	#define S32  int				// 符号付き 32 ビット
	#define S64  long				// 符号付き 64 ビット
#elif defined GBA
	// GBA 型定義
	#define S8   char				// 符号付き 8 ビット
	#define S16  short				// 符号付き 16 ビット
	#define S32  long				// 符号付き 32 ビット
#endif

// 上記に従って定義
#ifdef S8
#define U8      unsigned S8			// 8 ビット
#define PS8     S8*					// 符号付き 8 ビット
#define PU8     U8*					// 8 ビット
#define BYTE    U8					// 8 ビット
#define LPBYTE  PU8					// 8 ビット
#endif
#ifdef S16
#define U16     unsigned S16		// 16 ビット
#define PS16    S16*				// 符号付き 16 ビット
#define PU16    U16*				// 16 ビット
#define WORD    U16					// 16 ビット
#define LPWORD  PU16				// 16 ビット
#endif
#ifdef S32
#define U32     unsigned S32		// 32 ビット
#define PS32    S32*				// 符号付き 32 ビット
#define PU32    U32*				// 32 ビット
#define DWORD   U32					// 32 ビット
#define LPDWORD PU32				// 32 ビット
#endif
#ifdef S64
#define U64     unsigned S64		// 64 ビット
#define PS64    S64*				// 符号付き 64 ビット
#define PU64    U64*				// 64 ビット
#endif
#ifdef S128
#if defined R5900
#define U128    u_long128			// 128 ビット
#else
#define U128    unsigned S128		// 128 ビット
#endif
#define PS128   S128*				// 符号付き 128 ビット
#define PU128   U128*				// 128 ビット
#endif

// BOOL 定義
#ifndef BOOL
#define BOOL  int
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif
// bool 定義
#ifndef bool
#define bool  int
#endif
#ifndef false
#define false 0
#endif
#ifndef true
#define true  1
#endif

// NULL 定義
#ifndef NULL
#define NULL 0
#endif

// -------------------- 構造体・共用体定義
// 128 ビットデータ
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

// 64 ビットデータ
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

// 32 ビットデータ
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

// 16 ビットデータ
typedef union {
#ifdef S16
	U16 u16;
#endif
#ifdef S8
	U8 u8[2];
#endif
} U16DATA, *PU16DATA;

// -------------------- 2 重定義防止
#endif
