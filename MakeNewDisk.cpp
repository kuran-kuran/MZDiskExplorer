// MakeNewDisk.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "MZDiskExplorer.h"
#include "MakeNewDisk.h"
#include "MzDisk/Format.hpp"

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
	DDX_Control(pDX, IDC_VOLUME_NUMBER, m_VolumeNumber);
}


BEGIN_MESSAGE_MAP(cMakeNewDisk, CDialog)
	//{{AFX_MSG_MAP(cMakeNewDisk)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_DISKTYPE, &cMakeNewDisk::OnCbnSelchangeDisktype)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cMakeNewDisk メッセージ ハンドラ

BOOL cMakeNewDisk::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	m_DiskType.SetCurSel( DiskType );

	m_VolumeNumber.SetSel( 0, -1, FALSE );
	m_VolumeNumber.Clear();
	m_VolumeNumber.ReplaceSel( "1" );
	VolumeNumber = 1;
	OnCbnSelchangeDisktype();
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void cMakeNewDisk::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	DiskType = m_DiskType.GetCurSel();
	AdjustVolumeNumber();
	CDialog::OnOK();
}


void cMakeNewDisk::OnCbnSelchangeDisktype()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	DiskType = m_DiskType.GetCurSel();
	if(DiskType <= 2)
	{
		m_VolumeNumber.EnableWindow( FALSE );
	}
	else
	{
		m_VolumeNumber.EnableWindow( TRUE );
	}
	AdjustVolumeNumber();
}

void cMakeNewDisk::AdjustVolumeNumber()
{
	char temp[ 261 ];
	m_VolumeNumber.SetSel( 0, -1, FALSE );
	ZeroMemory( temp, sizeof( temp ) );
	int size = m_VolumeNumber.GetLine( 0, temp, 260 );
	temp[size] = '\0';
	VolumeNumber = atoi( temp );
	if(VolumeNumber < 0)
	{
		VolumeNumber = 0;
	}
	if(DiskType >= 5)
	{
		// MZ-80Kは0～127
		if(VolumeNumber > 127)
		{
			VolumeNumber = 127;
		}
	}
	else if(DiskType <= 2)
	{
		// MZ-2500は1固定
		VolumeNumber = 1;
	}
	else
	{
		// MZ-80B/2000は0～255
		if(VolumeNumber > 255)
		{
			VolumeNumber = 255;
		}
	}
	m_VolumeNumber.SetSel( 0, -1, FALSE );
	m_VolumeNumber.Clear();
	m_VolumeNumber.ReplaceSel( dms::Format("%d", VolumeNumber).c_str() );
}
