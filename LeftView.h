// LeftView.h : CLeftView クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEFTVIEW_H__322DF586_960F_4B5F_8072_4821AC98EBBB__INCLUDED_)
#define AFX_LEFTVIEW_H__322DF586_960F_4B5F_8072_4821AC98EBBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMZDiskExplorerDoc;

class CLeftView : public CTreeView
{
protected: // シリアライズ機能のみから作成します。
	CLeftView();
	DECLARE_DYNCREATE(CLeftView)

// アトリビュート
public:
	CMZDiskExplorerDoc* GetDocument();

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CLeftView)
	public:
	virtual void OnDraw(CDC* pDC);  // このビューを描画する際にオーバーライドされます。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnInitialUpdate(); // 構築後の最初の１度だけ呼び出されます。
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~CLeftView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG(CLeftView)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // LeftView.cpp ファイルがデバッグ環境の時使用されます。
inline CMZDiskExplorerDoc* CLeftView::GetDocument()
   { return (CMZDiskExplorerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_LEFTVIEW_H__322DF586_960F_4B5F_8072_4821AC98EBBB__INCLUDED_)
