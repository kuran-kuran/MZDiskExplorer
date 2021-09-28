// MZDiskExplorer.h : MZDISKEXPLORER アプリケーションのメイン ヘッダー ファイル
//

#if !defined(AFX_MZDISKEXPLORER_H__6E689B9C_9CBE_4537_A92E_57A3ACA69322__INCLUDED_)
#define AFX_MZDISKEXPLORER_H__6E689B9C_9CBE_4537_A92E_57A3ACA69322__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // メイン シンボル

/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerApp:
// このクラスの動作の定義に関しては MZDiskExplorer.cpp ファイルを参照してください。
//

class CMZDiskExplorerApp : public CWinApp
{
public:
	CMZDiskExplorerApp();

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CMZDiskExplorerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// インプリメンテーション
	//{{AFX_MSG(CMZDiskExplorerApp)
	afx_msg void OnAppAbout();
	afx_msg void OnEditGetboot();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_MZDISKEXPLORER_H__6E689B9C_9CBE_4537_A92E_57A3ACA69322__INCLUDED_)
