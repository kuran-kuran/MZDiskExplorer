// MZDiskExplorerView.cpp : CMZDiskExplorerView クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "MZDiskExplorer.h"

#include "MZDiskExplorerDoc.h"
#include "MZDiskExplorerView.h"
#include "GetFile.h"
#include "EditFile.h"
#include "MzDisk/Disk.hpp"
#include "MzDisk/MzDisk.hpp"
#include "MzDisk/Mz80Disk.hpp"
#include "MzDisk/Mz80SP6110Disk.hpp"
#include "path.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerView

IMPLEMENT_DYNCREATE(CMZDiskExplorerView, CListView)

BEGIN_MESSAGE_MAP(CMZDiskExplorerView, CListView)
	//{{AFX_MSG_MAP(CMZDiskExplorerView)
	ON_COMMAND(ID_EDIT_GETFILE, OnEditGetfile)
	ON_COMMAND(ID_EDIT_GETBOOT, OnEditGetboot)
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
	// 標準印刷コマンド
	ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CMZDiskExplorerView::OnNMDblclk)
	ON_COMMAND(ID_EDIT_EDIT, &CMZDiskExplorerView::OnEditEdit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerView クラスの構築/消滅

CMZDiskExplorerView::CMZDiskExplorerView()
{
	// TODO: この場所に構築用のコードを追加してください。
	ColumnFlag = 0;
}

CMZDiskExplorerView::~CMZDiskExplorerView()
{
}

BOOL CMZDiskExplorerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

	cs.style |= LVS_REPORT;
	cs.dwExStyle |= WS_EX_ACCEPTFILES;
	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerView クラスの描画

void CMZDiskExplorerView::OnDraw(CDC* pDC)
{
	CMZDiskExplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CListCtrl& refCtrl = GetListCtrl();
	refCtrl.InsertItem(0, "Item!");
	// TODO: この場所にネイティブ データ用の描画コードを追加します。
}

void CMZDiskExplorerView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	// TODO: GetListCtrl() メンバ関数の呼び出しを通して直接そのリスト コントロールに
	//  アクセスすることによって ListView をアイテムで固定できます。
	if ( 1 == ColumnFlag )
	{
		return;
	}
	CListCtrl &list = GetListCtrl();
	// 項目
	LV_COLUMN column;
	column.mask = LVCF_TEXT | LVCF_WIDTH;
	column.pszText = "ファイル名";
	column.cx = 160;
	list.InsertColumn( 0, &column );
	column.pszText = "モード";
	column.cx = 60;
	list.InsertColumn( 1, &column );
	column.pszText = "属性";
	column.cx = 60;
	list.InsertColumn( 2, &column );
	column.pszText = "ファイルサイズ";
	column.cx = 120;
	list.InsertColumn( 3, &column );
	column.pszText = "ロードアドレス";
	column.cx = 120;
	list.InsertColumn( 4, &column );
	column.pszText = "実行アドレス";
	column.cx = 120;
	list.InsertColumn( 5, &column );
	column.pszText = "日付";
	column.cx = 120;
	list.InsertColumn( 6, &column );
	column.pszText = "開始セクタ";
	column.cx = 120;
	list.InsertColumn( 7, &column );
	column.pszText = "開始トラック";
	column.cx = 120;
	list.InsertColumn( 8, &column );
	ColumnFlag = 1;
}

/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerView クラスの印刷

BOOL CMZDiskExplorerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// デフォルトの印刷準備
	return DoPreparePrinting(pInfo);
}

void CMZDiskExplorerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷前の特別な初期化処理を追加してください。
}

void CMZDiskExplorerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷後の後処理を追加してください。
}

/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerView クラスの診断

#ifdef _DEBUG
void CMZDiskExplorerView::AssertValid() const
{
	CListView::AssertValid();
}

void CMZDiskExplorerView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CMZDiskExplorerDoc* CMZDiskExplorerView::GetDocument() // 非デバッグ バージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMZDiskExplorerDoc)));
	return (CMZDiskExplorerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerView クラスのメッセージ ハンドラ
void CMZDiskExplorerView::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	// TODO: ユーザーによるウインドウのビュー スタイル変更に対応するコードを追加してください
}

void CMZDiskExplorerView::OnEditGetfile() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	CMZDiskExplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if(pDoc->MzDiskClass->DiskType() == Disk::MZ2000)
	{
		MzDisk::DIRECTORY dir;
		char savepath[ MAX_PATH ];
		char filename[ 18 ];
		char extname[ 18 ];
		CString DataPath;
		int i;
		ZeroMemory( savepath, MAX_PATH );
		// ファイル取り出し
		CListCtrl &list = GetListCtrl();
		ASSERT_VALID(pDoc);
		int select = -1;
		int AllOk = 0;
		char pathname[ 260 ];
		CString cpathname;
		cpathname = pDoc->GetPathName();
		strcpy_s( pathname, sizeof(pathname), cpathname.GetBuffer( 260 ) );
		while ( 1 )
		{
			select = list.GetNextItem( select, LVNI_ALL | LVNI_SELECTED );
			if ( -1 == select )
			{
				break;
			}
			pDoc->MzDiskClass->GetDir( &dir, pDoc->ItemToDirIndex[ select ] );
			ZeroMemory( filename, sizeof( filename ) );
			ZeroMemory( extname, sizeof( extname ) );
			strncpy_s( filename, sizeof(filename), dir.filename, 17 );
			for ( i=0; i<17; i++ ) {
				if ( 0x0D == filename[ i ] ) {
					filename[ i ] = 0;
				}
/*
				if ( '.' == filename[ i ] ) {
					int j;
					filename[ i ] = 0;
					if ( i < 17 )
					{
						int k = 0;
						for ( j = i + 1; j < 17; j ++ )
						{
							if ( 0x0D == filename[ j ] ) {
								filename[ j ] = 0;
							}
							extname[ k ] = filename[ j ];
							k ++;
						}
					}
				}
*/
			}
			// ダイアログ表示
			cGetFile getfiledialog;
			cPath path;
			int total = pDoc->MzDiskClass->GetAllBlockSize() * pDoc->MzDiskClass->GetClusterSize();
			std::string name = filename;
			if(total < 655360 || !pDoc->MzDiskClass->IsRom())
			{
				name = pDoc->MzDiskClass->ConvertText(filename);
			}
			path.SetPath( pathname );
			path.SetName( name.c_str() );
			path.SetExtName( extname );
			strcpy_s( getfiledialog.FileName, sizeof(getfiledialog.FileName), path.GetPath() );
			getfiledialog.MzDiskClass = pDoc->MzDiskClass;
			getfiledialog.DirIndex = pDoc->ItemToDirIndex[ select ];
			getfiledialog.SaveType = pDoc->SaveType;
			getfiledialog.AllOk = AllOk;
			getfiledialog.DoModal();
			pDoc->SaveType = getfiledialog.SaveType;
			AllOk = getfiledialog.AllOk;
			strcpy_s( pathname, sizeof(pathname), getfiledialog.FileName );
		}
	}
	else if(pDoc->MzDiskClass->DiskType() == Disk::MZ80K_SP6010)
	{
		Mz80Disk::DIRECTORY dir;
		char savepath[ MAX_PATH ];
		char filename[ 18 ];
		char extname[ 18 ];
		CString DataPath;
		int i;
		ZeroMemory( savepath, MAX_PATH );
		// ファイル取り出し
		CListCtrl &list = GetListCtrl();
		int select = -1;
		int AllOk = 0;
		char pathname[ 260 ];
		CString cpathname;
		cpathname = pDoc->GetPathName();
		strcpy_s( pathname, sizeof(pathname), cpathname.GetBuffer( 260 ) );
		while ( 1 )
		{
			select = list.GetNextItem( select, LVNI_ALL | LVNI_SELECTED );
			if ( -1 == select )
			{
				break;
			}
			pDoc->MzDiskClass->GetDir( &dir, pDoc->ItemToDirIndex[ select ] );
			ZeroMemory( filename, sizeof( filename ) );
			ZeroMemory( extname, sizeof( extname ) );
			strncpy_s( filename, sizeof(filename), dir.filename, 16 );
			for ( i=0; i<17; i++ ) {
				if ( 0x0D == filename[ i ] ) {
					filename[ i ] = 0;
				}
/*
				if ( '.' == filename[ i ] ) {
					int j;
					filename[ i ] = 0;
					if ( i < 17 )
					{
						int k = 0;
						for ( j = i + 1; j < 17; j ++ )
						{
							if ( 0x0D == filename[ j ] ) {
								filename[ j ] = 0;
							}
							extname[ k ] = filename[ j ];
							k ++;
						}
					}
				}
*/
			}
			// ダイアログ表示
			cGetFile getfiledialog;
			cPath path;
			std::string name = pDoc->MzDiskClass->ConvertText(filename);
			path.SetPath( pathname );
			path.SetName( name.c_str() );
			path.SetExtName( extname );
			strcpy_s( getfiledialog.FileName, sizeof(getfiledialog.FileName), path.GetPath() );
			getfiledialog.MzDiskClass = pDoc->MzDiskClass;
			getfiledialog.DirIndex = pDoc->ItemToDirIndex[ select ];
			getfiledialog.SaveType = pDoc->SaveType;
			getfiledialog.AllOk = AllOk;
			getfiledialog.DoModal();
			pDoc->SaveType = getfiledialog.SaveType;
			AllOk = getfiledialog.AllOk;
			strcpy_s( pathname, sizeof(pathname), getfiledialog.FileName );
		}
	}
	else if(pDoc->MzDiskClass->DiskType() == Disk::MZ80K_SP6110)
	{
		MzDisk::DIRECTORY dir;
		char savepath[ MAX_PATH ];
		char filename[ 18 ];
		char extname[ 18 ];
		CString DataPath;
		int i;
		ZeroMemory( savepath, MAX_PATH );
		// ファイル取り出し
		CListCtrl &list = GetListCtrl();
		ASSERT_VALID(pDoc);
		int select = -1;
		int AllOk = 0;
		char pathname[ 260 ];
		CString cpathname;
		cpathname = pDoc->GetPathName();
		strcpy_s( pathname, sizeof(pathname), cpathname.GetBuffer( 260 ) );
		while ( 1 )
		{
			select = list.GetNextItem( select, LVNI_ALL | LVNI_SELECTED );
			if ( -1 == select )
			{
				break;
			}
			pDoc->MzDiskClass->GetDir( &dir, pDoc->ItemToDirIndex[ select ] );
			ZeroMemory( filename, sizeof( filename ) );
			ZeroMemory( extname, sizeof( extname ) );
			strncpy_s( filename, sizeof(filename), dir.filename, 17 );
			for ( i=0; i<17; i++ ) {
				if ( 0x0D == filename[ i ] ) {
					filename[ i ] = 0;
				}
/*
				if ( '.' == filename[ i ] ) {
					int j;
					filename[ i ] = 0;
					if ( i < 17 )
					{
						int k = 0;
						for ( j = i + 1; j < 17; j ++ )
						{
							if ( 0x0D == filename[ j ] ) {
								filename[ j ] = 0;
							}
							extname[ k ] = filename[ j ];
							k ++;
						}
					}
				}
*/
			}
			// ダイアログ表示
			cGetFile getfiledialog;
			cPath path;
			int total = pDoc->MzDiskClass->GetAllBlockSize() * pDoc->MzDiskClass->GetClusterSize();
			std::string name = filename;
			name = pDoc->MzDiskClass->ConvertText(filename);
			path.SetPath( pathname );
			path.SetName( name.c_str() );
			path.SetExtName( extname );
			strcpy_s( getfiledialog.FileName, sizeof(getfiledialog.FileName), path.GetPath() );
			getfiledialog.MzDiskClass = pDoc->MzDiskClass;
			getfiledialog.DirIndex = pDoc->ItemToDirIndex[ select ];
			getfiledialog.SaveType = pDoc->SaveType;
			getfiledialog.AllOk = AllOk;
			getfiledialog.DoModal();
			pDoc->SaveType = getfiledialog.SaveType;
			AllOk = getfiledialog.AllOk;
			strcpy_s( pathname, sizeof(pathname), getfiledialog.FileName );
		}
	}
}

void CMZDiskExplorerView::OnEditGetboot() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	CMZDiskExplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->GetBootExec();
}

void CMZDiskExplorerView::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	UINT uiCount = DragQueryFile(hDropInfo, ~0lu, NULL, 0);
	for(UINT i = 0; i < uiCount; ++ i)
	{
		UINT uiLen = DragQueryFile(hDropInfo, i, NULL, 0);
		CString cpathname;
		DragQueryFile(hDropInfo, i, cpathname.GetBuffer(uiLen + 1), uiLen + 1);
		cpathname.ReleaseBuffer();
		char pathname[260];
		strcpy_s(pathname, sizeof(pathname), cpathname.GetBuffer(260));
		cPath path;
		path.SetPath(pathname);
		if((_stricmp(path.GetExtName(), "D88") == 0) && (_stricmp(path.GetExtName(), "D88") == 0))
		{
			CView::OnDropFiles(hDropInfo);
			return;
		}
		else
		{
			CMZDiskExplorerDoc* pDoc = GetDocument();
			ASSERT_VALID(pDoc);
			// ファイルをドロップした
			pDoc->OnEditPutfile(cpathname);
		}
	}
}

// ファイルをダブルクリックした
void CMZDiskExplorerView::OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	int index = pNMItemActivate->iItem;
	if(index != -1)
	{
		CMZDiskExplorerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		// ダブルクリックで選択したのでファイル編集へ
		EditFile editfileDialog;
		editfileDialog.MzDiskClass = pDoc->MzDiskClass;
		editfileDialog.dirIndex = pDoc->ItemToDirIndex[ index ];
		editfileDialog.DoModal();
		// ファイル画面作成
		pDoc->MakeFileList(pDoc->MzDiskClass->GetDirSector());
		pDoc->isUpdated = true;
	}
	*pResult = 0;
}


void CMZDiskExplorerView::OnEditEdit()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
	CMZDiskExplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// ファイル取り出し
	CListCtrl &list = GetListCtrl();
	ASSERT_VALID(pDoc);
	int select = -1;
	int AllOk = 0;
	char pathname[ 260 ];
	CString cpathname;
	cpathname = pDoc->GetPathName();
	strcpy_s( pathname, sizeof(pathname), cpathname.GetBuffer( 260 ) );
	while ( 1 )
	{
		select = list.GetNextItem( select, LVNI_ALL | LVNI_SELECTED );
		if ( -1 == select )
		{
			break;
		}
		EditFile editfileDialog;
		editfileDialog.MzDiskClass = pDoc->MzDiskClass;
		editfileDialog.dirIndex = pDoc->ItemToDirIndex[ select ];
		editfileDialog.DoModal();
	}
	// ファイル画面作成
	pDoc->MakeFileList(pDoc->MzDiskClass->GetDirSector());
	pDoc->isUpdated = true;
}
