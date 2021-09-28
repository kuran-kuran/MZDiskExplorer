// PutBoot.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "MZDiskExplorer.h"
#include "PutBoot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cPutBoot ダイアログ


cPutBoot::cPutBoot(CWnd* pParent /*=NULL*/)
	: CDialog(cPutBoot::IDD, pParent)
{
	//{{AFX_DATA_INIT(cPutBoot)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_INIT
}


void cPutBoot::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cPutBoot)
	DDX_Control(pDX, IDC_RUNADR, m_RunAdr);
	DDX_Control(pDX, IDC_MACHINE, m_Machine);
	DDX_Control(pDX, IDC_BOOTNAME, m_BootName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(cPutBoot, CDialog)
	//{{AFX_MSG_MAP(cPutBoot)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cPutBoot メッセージ ハンドラ

BOOL cPutBoot::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	char temp[ 16 ];
	m_BootName.SetSel( 0, -1, FALSE );
	m_BootName.Clear();
	m_BootName.ReplaceSel( BootName.GetBuffer( 260 ) );
	m_Machine.SetCurSel( Machine );
	m_RunAdr.SetSel( 0, -1, FALSE );
	m_RunAdr.Clear();
	sprintf( temp, "%04X", RunAdr );
	m_RunAdr.ReplaceSel( temp );

	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void cPutBoot::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	char temp[ 260 ];
	char *temp2;
	int i;
	m_BootName.SetSel( 0, -1, FALSE );
	ZeroMemory( temp, sizeof( temp ) );
	m_BootName.GetLine( 0, temp, 10 );
	BootName = temp;
	Machine = m_Machine.GetCurSel();
	m_RunAdr.SetSel( 0, -1, FALSE );
	m_RunAdr.GetLine( 0, temp, 260 );
	RunAdr = (unsigned short)strtol( temp, &temp2, 16 );
	IPL ipl;
	ZeroMemory( &ipl, sizeof( ipl ) );
	ipl.Machine = Machine;
	strncpy( ipl.Bootname, BootName.GetBuffer( 10 ), 10 );
	for ( i = 0; i < 11; i ++ )
	{
		if ( '\0' == ipl.Bootname[ i ] )
		{
			ipl.Bootname[ i ] = '\xD';
			break;
		}
	}
	ipl.RunAdr = RunAdr;
	int result;
	if ( 0 == FileType )
	{
		result = MzDiskClass->PutBoot( DataPath.GetBuffer( 260 ), &ipl, MZDISK_FILEMODE_BIN );
	}
	else if ( 1 == FileType )
	{
		result = MzDiskClass->PutBoot( DataPath.GetBuffer( 260 ), &ipl, MZDISK_FILEMODE_MZT );
	}
	if( 0 != result )
	{
		MessageBox( "ブートプログラムの書き込みに失敗しました. ", "エラー", MB_OK );
	}
	CDialog::OnOK();
}
