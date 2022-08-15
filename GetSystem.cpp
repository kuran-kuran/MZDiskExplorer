// GetSystem.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZDiskExplorer.h"
#include "GetSystem.h"
#include "path.h"
#include "afxdialogex.h"

// GetSystem ダイアログ

IMPLEMENT_DYNAMIC(GetSystem, CDialog)

GetSystem::GetSystem(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_GETSYSTEM, pParent)
	,FileName()
	,MzDiskClass(NULL)
	,SaveType(0)
{

}

GetSystem::~GetSystem()
{
}

void GetSystem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PATH, m_Path);
	DDX_Control(pDX, IDC_FILETYPE, m_FileType);
}


BEGIN_MESSAGE_MAP(GetSystem, CDialog)
	ON_EN_CHANGE(IDC_PATH, &GetSystem::OnEnChangePath)
	ON_BN_CLICKED(IDC_REF, &GetSystem::OnRef)
	ON_CBN_SELCHANGE(IDC_FILETYPE, &GetSystem::OnCbnSelchangeFiletype)
END_MESSAGE_MAP()


// GetSystem メッセージ ハンドラー
void GetSystem::SetFile(char *filename)
{
	strcpy_s( FileName, sizeof(FileName), filename );
}


BOOL GetSystem::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: ここに初期化を追加してください
	m_FileType.AddString( "BIN" );
	m_FileType.AddString( "MZT" );
	m_FileType.SetCurSel( SaveType );
	m_Path.SetSel( 0, -1, FALSE );
	m_Path.Clear();
	m_Path.ReplaceSel( FileName, FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void GetSystem::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	int select;
	int result;
	select = m_FileType.GetCurSel();
	if ( 0 == select )
	{
		result = MzDiskClass->GetSystem( FileName, Disk::FILEMODE_BIN );
	}
	else
	{
		result = MzDiskClass->GetSystem( FileName, Disk::FILEMODE_MZT );
	}
	if( 0 != result )
	{
		MessageBox( "ブートプログラムの取り出しに失敗しました. ", "エラー", MB_OK );
	}
	CDialog::OnOK();
}


void GetSystem::OnEnChangePath()
{
	// TODO: これが RICHEDIT コントロールの場合、このコントロールが
	// この通知を送信するには、CDialog::OnInitDialog() 関数をオーバーライドし、
	// CRichEditCtrl().SetEventMask() を関数し呼び出します。
	// OR 状態の ENM_CHANGE フラグをマスクに入れて呼び出す必要があります。

	// TODO: ここにコントロール通知ハンドラー コードを追加してください。
}


void GetSystem::OnRef()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	CString datapath;
	char savepath[ 261 ];
	int select;
	char temp[ 261 ];
	ZeroMemory( temp, sizeof( temp ) );
	int size = m_Path.GetLine( 0, temp, 260 );
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


void GetSystem::OnCbnSelchangeFiletype()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
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
	SaveType = select;
}
