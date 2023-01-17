// ChangeType.cpp : 実装ファイル
//
#include "stdafx.h"
#include "MZDiskExplorer.h"
#include "ChangeType.h"
#include "afxdialogex.h"


// ChangeType ダイアログ
IMPLEMENT_DYNAMIC(ChangeType, CDialog)

ChangeType::ChangeType(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CHANGETYPE, pParent)
	,DiskType(0)
	,MzDiskClass(NULL)
{

}

ChangeType::~ChangeType()
{
}

void ChangeType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DISKTYPE, m_DiskType);
}


BEGIN_MESSAGE_MAP(ChangeType, CDialog)
	ON_BN_CLICKED(IDOK, &ChangeType::OnBnClickedOk)
END_MESSAGE_MAP()


// ChangeType メッセージ ハンドラー


BOOL ChangeType::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: ここに初期化を追加してください
	m_DiskType.SetCurSel(DiskType);
	m_DiskType.EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void ChangeType::OnBnClickedOk()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	MzDiskClass->ChangeType(DiskType);
	CDialog::OnOK();
}
