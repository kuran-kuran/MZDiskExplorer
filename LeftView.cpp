// LeftView.cpp : CLeftView クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "MZDiskExplorer.h"

#include "MZDiskExplorerDoc.h"
#include "LeftView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
	//{{AFX_MSG_MAP(CLeftView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	//}}AFX_MSG_MAP
	// 標準印刷コマンド
	ON_COMMAND(ID_FILE_PRINT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CTreeView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CTreeView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeftView クラスの構築/消滅

CLeftView::CLeftView()
{
	// TODO: この場所に構築用のコードを追加してください。

}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。
	cs.style |= ( TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS );	// | TVS_EDITLABELS ); // 編集できる
	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CLeftView クラスの描画

void CLeftView::OnDraw(CDC* pDC)
{
	CMZDiskExplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: この場所にネイティブ データ用の描画コードを追加します。
}


/////////////////////////////////////////////////////////////////////////////
// CLeftView クラスの印刷

BOOL CLeftView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// デフォルトの印刷準備
	return DoPreparePrinting(pInfo);
}

void CLeftView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷前の特別な初期化処理を追加してください。
}

void CLeftView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷後の後処理を追加してください。
}

void CLeftView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	// TODO:  GetTreeCtrl() メンバ関数の呼び出しを通して直接そのリスト コントロールに
	//  アクセスすることによって TreeView をアイテムで固定できます。
}

/////////////////////////////////////////////////////////////////////////////
// CLeftView クラスの診断

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CMZDiskExplorerDoc* CLeftView::GetDocument() // 非デバッグ バージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMZDiskExplorerDoc)));
	return (CMZDiskExplorerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLeftView クラスのメッセージ ハンドラ

void CLeftView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CMZDiskExplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	int i;
	int select = -1;
	for ( i = 0; i < pDoc->DirHandleCount; i ++ )
	{
		if ( pDoc->DirHandle[ i ] == pNMTreeView->itemNew.hItem ) {
			select = i;
			break;
		}
	}
	if ( select != -1 )
	{
		pDoc->MakeFileList( pDoc->DirSector[ select ] );
		pDoc->MzDiskClass.SetDirSector( pDoc->DirSector[ select ] );
	}
	*pResult = 0;
}
