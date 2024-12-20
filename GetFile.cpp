// GetFile.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "MZDiskExplorer.h"
#include "GetFile.h"
#include "path.h"
#include "MzDisk/Disk.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cGetFile ダイアログ


cGetFile::cGetFile(CWnd* pParent /*=NULL*/)
	: CDialog(cGetFile::IDD, pParent)
	,AllOk(0)
	,DirIndex(0)
	,ExtName()
	,FileName()
	,MzDiskClass(NULL)
	,SaveType(0)
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
	path.AddExtName("");
	ZeroMemory( ExtName, sizeof( ExtName ) );
	if ( path.GetExtName() != 0 )
	{
		strcpy_s( ExtName, sizeof(ExtName), path.GetExtName() );
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
	strcpy_s( FileName, sizeof(FileName), path.GetPath() );
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
	char savepath[ 261 ];
	int select;
	char temp[ 261 ];
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
	ZeroMemory(FileName, sizeof(FileName));
	size = m_Path.GetLine( 0, FileName, 260 );
	FileName[size] = '\0';
}

void cGetFile::SetFile(char *filename)
{
	strcpy_s( FileName, sizeof(FileName), filename );
}

void cGetFile::OnSelchangeFiletype() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	cPath path;
	char temp[ 261 ];
	int select;
	ZeroMemory( temp, sizeof( temp ) );
	int size = m_Path.GetLine( 0, temp, 260 );
	temp[size] = '\0';
	path.SetPath( temp );
	if ( 0 == SaveType )
	{
		// binの場合は拡張子はMZ側の名前なので無しにする
		path.AddExtName("");
	}
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
	size = m_Path.GetLine( 0, FileName, 260 );
	FileName[size] = '\0';
	SaveType = select;
}

void cGetFile::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	cPath path;
	char temp[ 261 ];
	ZeroMemory( temp, sizeof( temp ) );
	int size = m_Path.GetLine( 0, temp, 260 );
	temp[size] = '\0';
	path.SetPath( temp );
	m_Path.SetSel( 0, -1, FALSE );
	m_Path.Clear();
	m_Path.ReplaceSel( path.GetPath(), FALSE );
	ZeroMemory( FileName, sizeof( FileName ) );
	size = m_Path.GetLine( 0, FileName, 260 );
	FileName[size] = '\0';
	m_Path.SetSel( 0, -1, FALSE );
	ZeroMemory( FileName, sizeof( FileName ) );
	size = m_Path.GetLine( 0, FileName, 260 );
	FileName[size] = '\0';
	int select;
	select = m_FileType.GetCurSel();
	if ( 0 == select )
	{
		MzDiskClass->GetFile( DirIndex, FileName, Disk::FILEMODE_BIN );
	}
	else
	{
		MzDiskClass->GetFile( DirIndex, FileName, Disk::FILEMODE_MZT );
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
