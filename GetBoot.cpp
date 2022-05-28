// GetBoot.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "MZDiskExplorer.h"
#include "GetBoot.h"
#include "path.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cGetBoot ダイアログ


cGetBoot::cGetBoot(CWnd* pParent /*=NULL*/)
	: CDialog(cGetBoot::IDD, pParent)
{
	//{{AFX_DATA_INIT(cGetBoot)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_INIT
}


void cGetBoot::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cGetBoot)
	DDX_Control(pDX, IDC_PATH, m_Path);
	DDX_Control(pDX, IDC_FILETYPE, m_FileType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(cGetBoot, CDialog)
	//{{AFX_MSG_MAP(cGetBoot)
	ON_CBN_SELCHANGE(IDC_FILETYPE, OnSelchangeFiletype)
	ON_BN_CLICKED(IDC_REF, OnRef)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cGetBoot メッセージ ハンドラ

BOOL cGetBoot::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	m_FileType.AddString( "BIN" );
	m_FileType.AddString( "MZT" );
	m_FileType.SetCurSel( SaveType );
	m_Path.SetSel( 0, -1, FALSE );
	m_Path.Clear();
	m_Path.ReplaceSel( FileName, FALSE );

	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void cGetBoot::SetFile(char *filename)
{
	strcpy_s( FileName, sizeof(FileName), filename );
}

void cGetBoot::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	int select;
	int result;
	select = m_FileType.GetCurSel();
	if ( 0 == select )
	{
		result = MzDiskClass->GetBoot( FileName, Disk::FILEMODE_BIN );
	}
	else
	{
		result = MzDiskClass->GetBoot( FileName, Disk::FILEMODE_MZT );
	}
	if( 0 != result )
	{
		MessageBox( "ブートプログラムの取り出しに失敗しました. ", "エラー", MB_OK );
	}
	CDialog::OnOK();
}

void cGetBoot::OnSelchangeFiletype() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	cPath path;
	char temp[ 261 ];
	int select;
	ZeroMemory( temp, sizeof( temp ) );
	int size = m_Path.GetLine( 0, temp, 260 );
	temp[size] = '\0';
	path.SetPath( temp );
	select = m_FileType.GetCurSel();
	if ( 0 == select )
	{
		path.SetExtName( "" );
	}
	else
	{
		path.SetExtName( "mzt" );
	}
	m_Path.SetSel( 0, -1, FALSE );
	m_Path.Clear();
	m_Path.ReplaceSel( path.GetPath(), FALSE );
	ZeroMemory( FileName, sizeof( FileName ) );
	size = m_Path.GetLine( 0, FileName, 260 );
	FileName[size] = '\0';
	SaveType = select;
}

void cGetBoot::OnRef() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CString datapath;
	char savepath[ 260 ];
	int select;
	char temp[ 260 ];
	ZeroMemory( temp, sizeof( temp ) );
	int size = m_Path.GetLine( 0, temp, 260 );
	temp[size] = '\0';
	select = m_FileType.GetCurSel();
	if ( 0 == select )
	{
		CFileDialog SelFile( TRUE, "bin", temp, OFN_HIDEREADONLY, "BIN file|*.*||" );
		if ( IDCANCEL == SelFile.DoModal() )
		{
			return;
		}
		datapath = SelFile.GetPathName();
	}
	else
	{
		cPath path;
		path.SetPath( temp );
		path.SetExtName( "mzt" );
		CFileDialog SelFile( TRUE, "mzt", path.GetPath(), OFN_HIDEREADONLY, "MZT file|*.mzt|全てのファイル|*.*||" );
		if ( IDCANCEL == SelFile.DoModal() )
		{
			return;
		}
		datapath = SelFile.GetPathName();
	}
	memset( savepath, 0, MAX_PATH );
	strcpy_s( savepath, sizeof(savepath), datapath.GetBuffer( MAX_PATH ) );
	m_Path.SetSel( 0, -1, FALSE );
	m_Path.Clear();
	m_Path.ReplaceSel( datapath, FALSE );
	ZeroMemory( FileName, sizeof( FileName ) );
	size = m_Path.GetLine( 0, FileName, 260 );
	FileName[size] = '\0';
}
