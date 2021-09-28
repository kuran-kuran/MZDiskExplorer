// MZDiskExplorerView.h : CMZDiskExplorerView クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MZDISKEXPLORERVIEW_H__D909D4C6_A4B6_43B7_8CBA_2797768673D5__INCLUDED_)
#define AFX_MZDISKEXPLORERVIEW_H__D909D4C6_A4B6_43B7_8CBA_2797768673D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMZDiskExplorerView : public CListView
{
protected: // シリアライズ機能のみから作成します。
	CMZDiskExplorerView();
	DECLARE_DYNCREATE(CMZDiskExplorerView)

// アトリビュート
public:
	CMZDiskExplorerDoc* GetDocument();

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CMZDiskExplorerView)
	public:
	virtual void OnDraw(CDC* pDC);  // このビューを描画する際にオーバーライドされます。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // 構築後の最初の１度だけ呼び出されます。
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~CMZDiskExplorerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG(CMZDiskExplorerView)
	afx_msg void OnEditGetfile();
	afx_msg void OnEditGetboot();
	//}}AFX_MSG
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
private:
	int ColumnFlag;
};

#ifndef _DEBUG  // MZDiskExplorerView.cpp ファイルがデバッグ環境の時使用されます。
inline CMZDiskExplorerDoc* CMZDiskExplorerView::GetDocument()
   { return (CMZDiskExplorerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_MZDISKEXPLORERVIEW_H__D909D4C6_A4B6_43B7_8CBA_2797768673D5__INCLUDED_)
