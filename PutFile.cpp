// PutFile.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "MZDiskExplorer.h"
#include "PutFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cPutFile ダイアログ


cPutFile::cPutFile(CWnd* pParent /*=NULL*/)
	: CDialog(cPutFile::IDD, pParent)
{
	//{{AFX_DATA_INIT(cPutFile)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_INIT
}


void cPutFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cPutFile)
	DDX_Control(pDX, IDC_HOUR, m_Hour);
	DDX_Control(pDX, IDC_MONTH, m_Month);
	DDX_Control(pDX, IDC_MINUTE, m_Minute);
	DDX_Control(pDX, IDC_DAY, m_Day);
	DDX_Control(pDX, IDC_YEAR, m_Year);
	DDX_Control(pDX, IDC_RUNADR, m_RunAdr);
	DDX_Control(pDX, IDC_MODE, m_Mode);
	DDX_Control(pDX, IDC_LOADADR, m_LoadAdr);
	DDX_Control(pDX, IDC_FILESIZE, m_FileSize);
	DDX_Control(pDX, IDC_FILENAME, m_FileName);
	DDX_Control(pDX, IDC_ATTR, m_Attr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(cPutFile, CDialog)
	//{{AFX_MSG_MAP(cPutFile)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL cPutFile::OnInitDialog() 
{
	char temp[ 20 ];
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	m_FileName.SetSel( 0, -1, FALSE );
	m_FileName.Clear();
	m_FileName.ReplaceSel( FileName.GetBuffer( 260 ) );
	m_Mode.SetCurSel( Mode - 1 );
	m_Attr.SetCurSel( Attr & 1 );
	m_FileSize.SetSel( 0, -1, FALSE );
	m_FileSize.Clear();
	sprintf( temp, "%d", FileSize );
	m_FileSize.ReplaceSel( temp );
	m_LoadAdr.SetSel( 0, -1, FALSE );
	m_LoadAdr.Clear();
	sprintf( temp, "%04X", LoadAdr );
	m_LoadAdr.ReplaceSel( temp );
	m_RunAdr.SetSel( 0, -1, FALSE );
	m_RunAdr.Clear();
	sprintf( temp, "%04X", RunAdr );
	m_RunAdr.ReplaceSel( temp );
	m_Year.Clear();
	sprintf( temp, "%02d", Year );
	m_Year.ReplaceSel( temp );
	m_Month.Clear();
	sprintf( temp, "%02d", Month );
	m_Month.ReplaceSel( temp );
	m_Day.Clear();
	sprintf( temp, "%02d", Day );
	m_Day.ReplaceSel( temp );
	m_Hour.Clear();
	sprintf( temp, "%02d", Hour );
	m_Hour.ReplaceSel( temp );
	m_Minute.Clear();
	sprintf( temp, "%02d", Minute );
	m_Minute.ReplaceSel( temp );
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void cPutFile::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	DIRECTORY dir;
	int i;
	char temp[ 260 ];
	char *temp2;
	int mode = 0;
	m_FileName.SetSel( 0, -1, FALSE );
	ZeroMemory( temp, sizeof( temp ) );
	m_FileName.GetLine( 0, temp, 16 );
	FileName = temp;
	Mode = m_Mode.GetCurSel() + 1;
	if( 1 == m_Attr.GetCurSel() )
	{
		Attr |= 0x80;
	}
	m_FileSize.SetSel( 0, -1, FALSE );
	m_FileSize.GetLine( 0, temp, 260 );
	FileSize = atoi( temp );
	m_LoadAdr.SetSel( 0, -1, FALSE );
	m_LoadAdr.GetLine( 0, temp, 260 );
	LoadAdr = (unsigned short)strtol( temp, &temp2, 16 );
	m_RunAdr.SetSel( 0, -1, FALSE );
	m_RunAdr.GetLine( 0, temp, 260 );
	RunAdr = (unsigned short)strtol( temp, &temp2, 16 );

	ZeroMemory( &dir, sizeof( dir ) );
	dir.Mode = Mode;
	if( 5 == dir.Mode )
	{
		dir.Mode = 3;
	}
	strncpy( dir.Filename, FileName.GetBuffer( 16 ), 16 );
	for ( i = 0; i < 17; i ++ )
	{
		if ( '\0' == dir.Filename[ i ] )
		{
			dir.Filename[ i ] = '\xD';
			break;
		}
	}
	dir.Attr = Attr;
	dir.Size = FileSize;
	dir.LoadAdr = LoadAdr;
	dir.RunAdr = RunAdr;
	dir.Date = ( ( ( Year % 100 ) / 10 ) << 28 ) +
				( ( ( Year % 100 ) % 10 ) << 24 ) +
				( ( Month / 10 ) << 23 ) +
				( ( Month % 10 ) << 19 ) +
				( ( Day / 10 ) << 17 ) +
				( ( Day % 10 ) << 13 ) +
				( ( Hour / 10 ) << 11 ) +
				( ( Hour % 10 ) << 7 ) +
				( ( Minute / 10 ) << 4 ) +
				( ( Minute % 10 ) );
	dir.Date = ( dir.Date >> 24 ) + ( ( dir.Date >> 8 ) & 0xFF00 ) + ( ( dir.Date << 8 ) & 0xFF0000 ) + ( ( dir.Date << 24 ) & 0xFF000000 );
	int result;
	if ( 0 == FileType )
	{
		result = MzDiskClass->PutFile( DataPath.GetBuffer( 260 ), &dir, MZDISK_FILEMODE_BIN, Mode );
	}
	else
	{
		result = MzDiskClass->PutFile( DataPath.GetBuffer( 260 ), &dir, MZDISK_FILEMODE_MZT, Mode );
	}
	if( 1 == result )
	{
		MessageBox( "ディスクイメージへのファイル書き込みに失敗しました. \nディレクトリに空きがありません.", "エラー", MB_OK );
	}
	else if( ( 2 == result ) || ( 3 == result ) )
	{
		MessageBox( "ディスクイメージへのファイル書き込みに失敗しました. \nファイルを読み込むことができません.", "エラー", MB_OK );
	}
	else if( 4 == result )
	{
		MessageBox( "ディスクイメージへのファイル書き込みに失敗しました. \nビットマップの空きがありません.", "エラー", MB_OK );
	}
	else if( 5 == result )
	{
		MessageBox( "ディスクイメージへのファイル書き込みに失敗しました. \n同じファイル名がすでに存在します.", "エラー", MB_OK );
	}
	else if( 0 != result )
	{
		MessageBox( "ディスクイメージへのファイル書き込みに失敗しました. \n原因は不明です.", "エラー", MB_OK );
	}
	CDialog::OnOK();
}
