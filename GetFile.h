#if !defined(AFX_CGETFILE_H__91CAB395_A30D_4022_A956_B2FCA9731DF1__INCLUDED_)
#define AFX_CGETFILE_H__91CAB395_A30D_4022_A956_B2FCA9731DF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetFile.h : ヘッダー ファイル
//
#include "mzdisk.h"

/////////////////////////////////////////////////////////////////////////////
// cGetFile ダイアログ

class cGetFile : public CDialog
{
// コンストラクション
public:
	int AllOk;
	char ExtName[ 260 ];
	int DirIndex;
	char FileName[ MAX_PATH ];
	void SetFile( char *filename );
	cGetFile(CWnd* pParent = NULL);   // 標準のコンストラクタ
	cMzDisk *MzDiskClass;
	int SaveType;

// ダイアログ データ
	//{{AFX_DATA(cGetFile)
	enum { IDD = IDD_GETFILE };
	CEdit	m_Path;
	CComboBox	m_FileType;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(cGetFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(cGetFile)
	virtual BOOL OnInitDialog();
	afx_msg void OnRef();
	afx_msg void OnSelchangeFiletype();
	virtual void OnOK();
	afx_msg void OnAllok();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_CGETFILE_H__91CAB395_A30D_4022_A956_B2FCA9731DF1__INCLUDED_)
