#if !defined(AFX_MAKENEWDISK_H__8C8AFFBB_A2A1_4975_BAFE_DBAD0A4354FB__INCLUDED_)
#define AFX_MAKENEWDISK_H__8C8AFFBB_A2A1_4975_BAFE_DBAD0A4354FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MakeNewDisk.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// cMakeNewDisk ダイアログ

class cMakeNewDisk : public CDialog
{
// コンストラクション
public:
	int DiskType;
	cMakeNewDisk(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(cMakeNewDisk)
	enum { IDD = IDD_NEWDISK };
	CComboBox	m_DiskType;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(cMakeNewDisk)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(cMakeNewDisk)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_MAKENEWDISK_H__8C8AFFBB_A2A1_4975_BAFE_DBAD0A4354FB__INCLUDED_)
