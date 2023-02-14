#if !defined(AFX_PUTFILE_H__64FC9668_733B_4BE3_845B_C2127A9BB045__INCLUDED_)
#define AFX_PUTFILE_H__64FC9668_733B_4BE3_845B_C2127A9BB045__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PutFile.h : ヘッダー ファイル
//
#include "MzDisk/Disk.hpp"

/////////////////////////////////////////////////////////////////////////////
// cPutFile ダイアログ

class cPutFile : public CDialog
{
// コンストラクション
public:
	CString DataPath;
	CString FileName;
	std::vector<unsigned char> mztFilename;
	unsigned char Mode;
	unsigned char Attr;
	unsigned char Reserve;
	int FileSize;
	unsigned short LoadAdr;
	unsigned short RunAdr;
//	unsigned int Date;
	int Year;
	int Month;
	int Day;
	int Hour;
	int Minute;
	int FileType;
	bool Size64KBObj;
	Disk *MzDiskClass;
	cPutFile(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(cPutFile)
	enum { IDD = IDD_PUTFILE };
	CEdit	m_Hour;
	CEdit	m_Month;
	CEdit	m_Minute;
	CEdit	m_Day;
	CEdit	m_Year;
	CEdit	m_RunAdr;
	CComboBox	m_Mode;
	CEdit	m_LoadAdr;
	CEdit	m_FileSize;
	CEdit	m_FileName;
	CComboBox	m_Attr;
	CEdit	m_Reserve;
	CButton m_IsMztFilename;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(cPutFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(cPutFile)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedMztFilename();
	afx_msg void OnSelchangeMode();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_PUTFILE_H__64FC9668_733B_4BE3_845B_C2127A9BB045__INCLUDED_)
