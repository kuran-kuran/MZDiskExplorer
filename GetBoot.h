#if !defined(AFX_GETBOOT_H__EAB24344_9F91_4D62_8F1C_6DBFC4DABF1E__INCLUDED_)
#define AFX_GETBOOT_H__EAB24344_9F91_4D62_8F1C_6DBFC4DABF1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetBoot.h : ヘッダー ファイル
//
#include "MzDisk/Disk.hpp"

/////////////////////////////////////////////////////////////////////////////
// cGetBoot ダイアログ

class cGetBoot : public CDialog
{
// コンストラクション
public:
	char FileName[ MAX_PATH + 1 ];
	Disk *MzDiskClass;
	int SaveType;
	void SetFile( char *filename );
	cGetBoot(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(cGetBoot)
	enum { IDD = IDD_GETBOOT };
	CEdit	m_Path;
	CComboBox	m_FileType;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(cGetBoot)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(cGetBoot)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeFiletype();
	afx_msg void OnRef();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_GETBOOT_H__EAB24344_9F91_4D62_8F1C_6DBFC4DABF1E__INCLUDED_)
