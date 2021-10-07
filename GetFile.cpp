// GetFile.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "MZDiskExplorer.h"
#include "GetFile.h"
#include "path.h"
#include "mzdisk.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cGetFile ダイアログ


cGetFile::cGetFile(CWnd* pParent /*=NULL*/)
	: CDialog(cGetFile::IDD, pParent)
{
	//{{AFX_DATA_INIT(cGetFile)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_INIT
}


void cGetFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cGetFile)
	DDX_Control(pDX, IDC_PATH, m_Path);
	DDX_Control(pDX, IDC_FILETYPE, m_FileType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(cGetFile, CDialog)
	//{{AFX_MSG_MAP(cGetFile)
	ON_BN_CLICKED(IDC_REF, OnRef)
	ON_CBN_SELCHANGE(IDC_FILETYPE, OnSelchangeFiletype)
	ON_BN_CLICKED(IDALLOK, OnAllok)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cGetFile メッセージ ハンドラ

BOOL cGetFile::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	m_FileType.AddString( "BIN" );
	m_FileType.AddString( "MZT" );
	m_FileType.SetCurSel( SaveType );
	cPath path;
	path.SetPath( FileName );
	ZeroMemory( ExtName, sizeof( ExtName ) );
	if ( path.GetExtName() != 0 )
	{
		strcpy( ExtName, path.GetExtName() );
	}
	if ( 0 == SaveType )
	{
		path.SetExtName( ExtName );
	}
	else
	{
		path.SetExtName( "mzt" );
	}
	m_Path.SetSel( 0, -1, FALSE );
	m_Path.Clear();
	m_Path.ReplaceSel( path.GetPath(), FALSE );
	strcpy( FileName, path.GetPath() );
	if ( 1 == AllOk )
	{
		OnOK();
	}
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void cGetFile::OnRef() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CString datapath;
	char savepath[ 260 ];
	int select;
	char temp[ 260 ];
	ZeroMemory( temp, sizeof( temp ) );
	m_Path.GetLine( 0, temp, 260 );
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
	strcpy( savepath, datapath.GetBuffer( MAX_PATH ) );
	m_Path.SetSel( 0, -1, FALSE );
	m_Path.Clear();
	m_Path.ReplaceSel( datapath, FALSE );
	m_Path.GetLine( 0, FileName, 260 );
}

void cGetFile::SetFile(char *filename)
{
	strcpy( FileName, filename );
}

void cGetFile::OnSelchangeFiletype() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	cPath path;
	char temp[ 260 ];
	int select;
	ZeroMemory( temp, sizeof( temp ) );
	m_Path.GetLine( 0, temp, 260 );
	path.SetPath( temp );
	select = m_FileType.GetCurSel();
	if ( 0 == select )
	{
		path.SetExtName( ExtName );
	}
	else
	{
		path.SetExtName( "mzt" );
	}
	m_Path.SetSel( 0, -1, FALSE );
	m_Path.Clear();
	m_Path.ReplaceSel( path.GetPath(), FALSE );
	ZeroMemory( FileName, sizeof( FileName ) );
	m_Path.GetLine( 0, FileName, 260 );
	SaveType = select;
}

void cGetFile::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	ZeroMemory( FileName, sizeof( FileName ) );
	m_Path.SetSel( 0, -1, FALSE );
	m_Path.GetLine( 0, FileName, 260 );
	int select;
	select = m_FileType.GetCurSel();
	if ( 0 == select )
	{
		MzDiskClass->GetFile( DirIndex, FileName, MZDISK_FILEMODE_BIN );
	}
	else
	{
		MzDiskClass->GetFile( DirIndex, FileName, MZDISK_FILEMODE_MZT );
	}
	CDialog::OnOK();
}

void cGetFile::OnAllok() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	AllOk = 1;
	OnOK();
	CDialog::OnOK();
}
