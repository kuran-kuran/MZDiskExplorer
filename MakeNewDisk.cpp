// MakeNewDisk.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "MZDiskExplorer.h"
#include "MakeNewDisk.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cMakeNewDisk ダイアログ


cMakeNewDisk::cMakeNewDisk(CWnd* pParent /*=NULL*/)
	: CDialog(cMakeNewDisk::IDD, pParent)
{
	//{{AFX_DATA_INIT(cMakeNewDisk)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_INIT
}


void cMakeNewDisk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cMakeNewDisk)
	DDX_Control(pDX, IDC_DISKTYPE, m_DiskType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(cMakeNewDisk, CDialog)
	//{{AFX_MSG_MAP(cMakeNewDisk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cMakeNewDisk メッセージ ハンドラ

BOOL cMakeNewDisk::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	m_DiskType.SetCurSel( DiskType );
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void cMakeNewDisk::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	DiskType = m_DiskType.GetCurSel();
	CDialog::OnOK();
}
