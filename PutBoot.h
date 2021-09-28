#if !defined(AFX_PUTBOOT_H__B061910F_E971_428A_B1A1_E0C3C8627B0A__INCLUDED_)
#define AFX_PUTBOOT_H__B061910F_E971_428A_B1A1_E0C3C8627B0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PutBoot.h : ヘッダー ファイル
//
#include "mzdisk.h"

/////////////////////////////////////////////////////////////////////////////
// cPutBoot ダイアログ

class cPutBoot : public CDialog
{
// コンストラクション
public:
	unsigned short RunAdr;
	int Machine;
	int FileType;
	CString BootName;
	CString DataPath;
	cMzDisk *MzDiskClass;
	cPutBoot(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(cPutBoot)
	enum { IDD = IDD_PUTBOOT };
	CEdit	m_RunAdr;
	CComboBox	m_Machine;
	CEdit	m_BootName;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(cPutBoot)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(cPutBoot)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_PUTBOOT_H__B061910F_E971_428A_B1A1_E0C3C8627B0A__INCLUDED_)
