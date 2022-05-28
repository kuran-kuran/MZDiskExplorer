// MZDiskExplorerDoc.cpp : CMZDiskExplorerDoc クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "MZDiskExplorer.h"

#include "MZDiskExplorerDoc.h"
#include "LeftView.h"
#include "MZDiskExplorerView.h"
#include "GetBoot.h"
#include "PutFile.h"
#include "PutBoot.h"
#include "MakeNewDisk.h"
#include "MzDisk/Disk.hpp"
#include "MzDisk/MzDisk.hpp"
#include "MzDisk/Mz80Disk.hpp"
#include "path.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerDoc

IMPLEMENT_DYNCREATE(CMZDiskExplorerDoc, CDocument)

BEGIN_MESSAGE_MAP(CMZDiskExplorerDoc, CDocument)
	//{{AFX_MSG_MAP(CMZDiskExplorerDoc)
	ON_UPDATE_COMMAND_UI(ID_EDIT_GETFILE, OnUpdateEditGetfile)
	ON_UPDATE_COMMAND_UI(ID_EDIT_GETBOOT, OnUpdateEditGetboot)
	ON_COMMAND(ID_EDIT_GETBOOT, OnEditGetboot)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PUTFILE, OnUpdateEditPutfile)
	ON_COMMAND(ID_EDIT_PUTFILE, OnEditPutfile)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PUTBOOT, OnUpdateEditPutboot)
	ON_COMMAND(ID_EDIT_PUTBOOT, OnEditPutboot)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DEL, OnUpdateEditDel)
	ON_COMMAND(ID_EDIT_DEL, OnEditDel)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_BETA, OnUpdateFileExportBeta)
	ON_COMMAND(ID_FILE_EXPORT_BETA, OnFileExportBeta)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerDoc クラスの構築/消滅

CMZDiskExplorerDoc::CMZDiskExplorerDoc()
{
	// TODO: この位置に１度だけ呼ばれる構築用のコードを追加してください。
	MzDiskClass = NULL;
	FirstInit = 1;
	SaveType = 0;
	ImageInit = 0;
	Machine = 1;
}

CMZDiskExplorerDoc::~CMZDiskExplorerDoc()
{
}

BOOL CMZDiskExplorerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: この位置に再初期化処理を追加してください。
	// (SDI ドキュメントはこのドキュメントを再利用します。)
	POSITION pos;
	CLeftView *LeftView;
	CMZDiskExplorerView *FileView;
	pos = GetFirstViewPosition();
	LeftView = (CLeftView*)GetNextView( pos );
	FileView = (CMZDiskExplorerView*)GetNextView( pos );
	CTreeCtrl* tree;
	tree = &LeftView->GetTreeCtrl();
	tree->DeleteAllItems();
	CListCtrl *list;
	list = &FileView->GetListCtrl();
	list->DeleteAllItems();

	ImageInit = 0;
	if ( 0 == FirstInit )
	{
		cMakeNewDisk newdisk;
		newdisk.DiskType = 0;
		if ( IDOK == newdisk.DoModal() )
		{
			unsigned int disktype = MzDisk::DISKTYPE_MZ2500_2DD;
			int needClassType = 0;
			switch( newdisk.DiskType )
			{
				case 0:
					disktype = MzDisk::DISKTYPE_MZ2500_2DD;
					needClassType = Disk::MZ2000;
					break;
				case 1:
					disktype = MzDisk::DISKTYPE_MZ2500_2DD40;
					needClassType = Disk::MZ2000;
					break;
				case 2:
					disktype = MzDisk::DISKTYPE_MZ2500_2DD35;
					needClassType = Disk::MZ2000;
					break;
				case 3:
					disktype = MzDisk::DISKTYPE_MZ80B_2D35;
					needClassType = Disk::MZ2000;
					break;
				case 4:
					disktype = MzDisk::DISKTYPE_MZ2000_2D40;
					needClassType = Disk::MZ2000;
					break;
				case 5:
					disktype = Mz80Disk::DISKTYPE_MZ80_SP6010_2S;
					needClassType = Disk::MZ80K_SP6010;
					break;
				case 6:
					disktype = Mz80Disk::DISKTYPE_MZ80_SP6110_2S;
					needClassType = Disk::MZ80K_SP6110;
					break;
				default:
					disktype = MzDisk::DISKTYPE_MZ2500_2DD;
					needClassType = Disk::MZ2000;
					break;
			}
			if(MzDiskClass != NULL)
			{
				// クラスが違っていたら一旦NULLにする
				if(MzDiskClass->DiskType() != needClassType)
				{
					delete MzDiskClass;
					MzDiskClass = NULL;
				}
			}
			if(MzDiskClass == NULL)
			{
				if(needClassType == Disk::MZ2000)
				{
					MzDiskClass = new MzDisk;
				}
				else if(needClassType == Disk::MZ80K_SP6010)
				{
					MzDiskClass = new Mz80Disk;
				}
				else if(needClassType == Disk::MZ80K_SP6110)
				{
					// MZ-80K SP6110 未実装
				}
			}
			if(MzDiskClass != NULL)
			{
				MzDiskClass->Format( disktype );
				// ツリー画面作成
				DirHandleCount = 0;
				tree = &LeftView->GetTreeCtrl();
				tree->DeleteAllItems();
				DirHandle[ 0 ] = tree->InsertItem( "root" );
				DirSector[ 0 ] = 0x10;
				DirHandleCount ++;
				MakeTree( 0x10, DirHandle[ 0 ] );
				MzDiskClass->SetDirSector( -1 );
				// ファイル画面作成
				MakeFileList( 0x10 );
				MzDiskClass->SetDirSector( -1 );
				ImageInit = 1;
				FirstInit = 0;
			}
		}
	}
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerDoc シリアライゼーション

void CMZDiskExplorerDoc::Serialize(CArchive& ar)
{
	POSITION pos;
	CLeftView *LeftView;
	CMZDiskExplorerView *FileView;
	pos = GetFirstViewPosition();
	LeftView = (CLeftView*)GetNextView( pos );
	FileView = (CMZDiskExplorerView*)GetNextView( pos );
	if (ar.IsStoring())
	{
		// TODO: この位置に保存用のコードを追加してください。
		if ( 0 == ImageInit ) {
			return;
		}
		if ( MzDiskClass == NULL ) {
			return;
		}
		std::vector<unsigned char> writeBuffer;
		MzDiskClass->Save( writeBuffer );
		// イメージファイル書き込み
		CFile *file;
		file = ar.GetFile();
		ar.Flush();
		ar.Write( &writeBuffer[0], static_cast<UINT>(writeBuffer.size()) );
	}
	else
	{
		// TODO: この位置に読み込み用のコードを追加してください。
		CFile *file;
		cPath path;
		// イメージファイル読み込み
		file = ar.GetFile();
		ar.Flush();
		FilePath = file->GetFilePath();
		path.SetPath( FilePath.GetBuffer( 260 ) );
		if ( ( 0 == _stricmp( path.GetExtName(), "d88" ) ) ||
			 ( 0 == _stricmp( path.GetExtName(), "d20" ) ) )
		{
			UINT length = static_cast<UINT>(file->GetLength());
			std::vector<unsigned char> readBuffer;
			readBuffer.resize(length);
			file->Read( &readBuffer[0], length );
			if(MzDiskClass != NULL)
			{
				delete MzDiskClass;
				MzDiskClass = NULL;
			}
			int diskType = Disk::DiskType(readBuffer);
			if(diskType == Disk::MZ2000)
			{
				MzDiskClass = new MzDisk;
			}
			else if(diskType == Disk::MZ80K_SP6010)
			{
				MzDiskClass = new Mz80Disk;
			}
			else
			{
				MzDiskClass = NULL;
				AfxThrowArchiveException(CArchiveException::generic, "");
			}
			MzDiskClass->Load( readBuffer );
			// ツリー画面作成
			DirHandleCount = 0;
			CTreeCtrl* tree;
			tree = &LeftView->GetTreeCtrl();
			tree->DeleteAllItems();
			DirHandle[ 0 ] = tree->InsertItem( "root" );
			DirSector[ 0 ] = 0x10;
			DirHandleCount ++;
			MakeTree( 0x10, DirHandle[ 0 ] );
			MzDiskClass->SetDirSector( -1 );
			// ファイル画面作成
			MakeFileList( 0x10 );
			MzDiskClass->SetDirSector( -1 );
			ImageInit = 1;
		} else {
			CTreeCtrl* tree;
			tree = &LeftView->GetTreeCtrl();
			tree->DeleteAllItems();
			CListCtrl *list;
			list = &FileView->GetListCtrl();
			list->DeleteAllItems();
			ImageInit = 0;
		}
	}
	if(MzDiskClass != NULL)
	{
		CMainFrame *pMainFrame;
		char str[ 200 ];
		int use = MzDiskClass->GetUseBlockSize() * MzDiskClass->GetClusterSize();
		int total = MzDiskClass->GetAllBlockSize() * MzDiskClass->GetClusterSize();
		int free = total - use;
		sprintf_s( str, sizeof(str), "Type: %s    Size: %d/%d    Free: %d", MzDiskClass->DiskTypeText().c_str(), use, total, free );
		pMainFrame = ( CMainFrame* )AfxGetMainWnd();
		pMainFrame->PutStatusBarSize( str );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerDoc クラスの診断

#ifdef _DEBUG
void CMZDiskExplorerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMZDiskExplorerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerDoc コマンド

int CMZDiskExplorerDoc::MakeTree( int dirsector, HTREEITEM parenthandle )
{
	if(MzDiskClass == NULL)
	{
		return 0;
	}
	if(MzDiskClass->DiskType() == Disk::MZ2000)
	{
		MzDisk::DIRECTORY dir;
		int i;
		POSITION pos;
		CLeftView *LeftView;
		pos = GetFirstViewPosition();
		LeftView = (CLeftView*)GetNextView( pos );
		CTreeCtrl *tree;
		tree = &LeftView->GetTreeCtrl();
		MzDiskClass->SetDirSector( dirsector );
		for ( i = 0; i < 64; i ++ )
		{
			int j;
			MzDiskClass->GetDir( &dir, i );
			if ( 0xF == dir.mode ) {
				char dirname[ 17 ];
				memcpy( dirname, dir.filename, 17 );
				for ( j = 0; j < 17; j ++ )
				{
					if ( 0x0D == dirname[ j ] )
					{
						dirname[ j ] = '\0';
					}
				}
				if ( ( 0 == strcmp( dirname, "."  ) ) || ( 0 == strcmp( dirname, ".." ) ) )
				{
					continue;
				}
				DirHandle[ DirHandleCount ] = tree->InsertItem( dirname, parenthandle );
				DirSector[ DirHandleCount ] = dir.startSector;
				int temp;
				temp = DirHandleCount;
				DirHandleCount ++;
				if ( 1 == MakeTree( dir.startSector, DirHandle[ temp ] ) ) {
					return 1;
				}
				MzDiskClass->SetDirSector( dirsector );
				if ( DirHandleCount >= DIRHANDLE_MAX ) {
					return 1;
				}
			}
		}
	}
	else if(MzDiskClass->DiskType() == Disk::MZ80K_SP6010)
	{
		Mz80Disk::DIRECTORY dir;
		int i;
		POSITION pos;
		CLeftView *LeftView;
		pos = GetFirstViewPosition();
		LeftView = (CLeftView*)GetNextView( pos );
		CTreeCtrl *tree;
		tree = &LeftView->GetTreeCtrl();
		MzDiskClass->SetDirSector( dirsector );
		for ( i = 0; i < 28; i ++ )
		{
			int j;
			MzDiskClass->GetDir( &dir, i );
			if ( 0xF == dir.mode ) {
				char dirname[ 17 ];
				memcpy( dirname, dir.filename, 17 );
				for ( j = 0; j < 17; j ++ )
				{
					if ( 0x0D == dirname[ j ] )
					{
						dirname[ j ] = '\0';
					}
				}
				if ( ( 0 == strcmp( dirname, "."  ) ) || ( 0 == strcmp( dirname, ".." ) ) )
				{
					continue;
				}
				DirHandle[ DirHandleCount ] = tree->InsertItem( dirname, parenthandle );
				DirSector[ DirHandleCount ] = dir.startSector;
				int temp;
				temp = DirHandleCount;
				DirHandleCount ++;
				if ( 1 == MakeTree( dir.startSector, DirHandle[ temp ] ) ) {
					return 1;
				}
				MzDiskClass->SetDirSector( dirsector );
				if ( DirHandleCount >= DIRHANDLE_MAX ) {
					return 1;
				}
			}
		}
	}
	return 0;
}

int CMZDiskExplorerDoc::MakeFileList( int dirsector )
{
	if(MzDiskClass == NULL)
	{
		return 0;
	}
	POSITION pos;
	CMZDiskExplorerView *FileView;
	pos = GetFirstViewPosition();
	GetNextView( pos );
	FileView = (CMZDiskExplorerView*)GetNextView( pos );
	CListCtrl *list;
	list = &FileView->GetListCtrl();
	list->DeleteAllItems();
	// ファイル
	if(MzDiskClass->DiskType() == Disk::MZ2000)
	{
		MzDiskClass->SetDirSector( dirsector );
		LV_ITEM item;
		MzDisk::DIRECTORY dir;
		int i;
		int j;
		int itemindex = 0;
		int year;
		int month;
		int day;
		int hour;
		int minute;
		for ( i = 0; i < 64; i ++ )
		{
			struct {
				unsigned char mode;
				char modestr[ 6 ];
			} modetbl[] = {
				0x01, "OBJ  ",
				0x02, "BTX  ",
				0x03, "BSD  ",
				0x04, "BRD  ",
				0x05, "RB   ",
				0x07, "LIB  ",
				0x0A, "SYS  ",
				0x0B, "GR   ",
				0x0F, "DIR  ",
				0x80, "NSWAP",
				0x81, "SWAP "
			};
			char work[ 17 ];
			ZeroMemory(work, sizeof(work));
			MzDiskClass->GetDir( &dir, i );
			if ( ( 0 == dir.mode ) || ( 0x80 == dir.mode ) || ( 0x82 == dir.mode ) || ( 0xF == dir.mode ) )
			{
				continue;
			}
			memcpy( work, dir.filename, 17 );
			for ( j = 0; j < 17; j ++ )
			{
				if ( 0x0D == work[ j ] )
				{
					work[ j ] = '\0';
				}
			}
			// ファイル名
			item.mask = LVIF_TEXT;
			item.iItem = itemindex;
			item.iSubItem = 0;
			item.pszText = work;
			list->InsertItem( &item );
			// モード
			item.iSubItem = 1;
			item.pszText = "";
			for ( j = 0; j < 10; j ++ )
			{
				if ( modetbl[ j ].mode == dir.mode )
				{
					item.pszText = (char*)&modetbl[ j ].modestr;
					break;
				}
			}
			list->SetItem( &item );
			// 属性
			item.iSubItem = 2;
			sprintf_s( work, sizeof(work), "%02X", dir.attr );
			item.pszText = work;
			list->SetItem( &item );
			// ファイルサイズ
			item.iSubItem = 3;
			if( 4 == dir.mode )
			{
				sprintf_s( work, sizeof(work), "%7d", dir.size * 32 );
			}
			else
			{
				sprintf_s( work, sizeof(work), "%7d", dir.size );
			}
			item.pszText = work;
			list->SetItem( &item );
			// ロードアドレス
			item.iSubItem = 4;
			sprintf_s( work, sizeof(work), "%04X", dir.loadAdr );
			item.pszText = work;
			list->SetItem( &item );
			// 実行アドレス
			item.iSubItem = 5;
			sprintf_s( work, sizeof(work), "%04X", dir.runAdr );
			item.pszText = work;
			list->SetItem( &item );
			// 作成日付
			item.iSubItem = 6;
			year = ( dir.date & 0xF ) + ( ( dir.date >> 4 ) & 0xF ) * 10;
			month = ( ( dir.date >> 11 ) & 0xF ) + ( ( dir.date >> 15 ) & 0x1 ) * 10;
			day = ( ( dir.date >> 5 ) & 0x8 ) + ( ( dir.date >> 21 ) & 0x7 ) + ( ( dir.date >> 9 ) & 0x3 ) * 10;
			hour = ( ( dir.date >> 31 ) & 0x1 ) + ( ( dir.date >> 15 ) & 0xE ) + ( ( dir.date >> 19 ) & 0x3 ) * 10;
			minute = ( ( dir.date >> 24 ) & 0xF ) + ( ( dir.date >> 28 ) & 7 ) * 10;
			sprintf_s( work, sizeof(work), "%02d/%02d/%02d %02d:%02d", year, month, day, hour, minute );
			item.pszText = work;
			list->SetItem( &item );
			// 開始セクタ
			item.iSubItem = 7;
			sprintf_s( work, sizeof(work), "%04X", dir.startSector );
			item.pszText = work;
			list->SetItem( &item );
			ItemToDirIndex[ itemindex ] = i;
			itemindex++;
		}
	}
	else if(MzDiskClass->DiskType() == Disk::MZ80K_SP6010)
	{
		MzDiskClass->SetDirSector( dirsector );
		LV_ITEM item;
		Mz80Disk::DIRECTORY dir;
		int i;
		int j;
		int itemindex = 0;
		int year;
		int month;
		int day;
		int hour;
		int minute;
		for ( i = 0; i < 28; i ++ )
		{
			struct {
				unsigned char mode;
				char modestr[ 6 ];
			} modetbl[] = {
				0x01, "OBJ  ",
				0x02, "BTX  ",
				0x03, "BSD  ",
				0x04, "BRD  ",
				0x05, "RB   ",
				0x07, "LIB  ",
				0x0A, "SYS  ",
				0x0B, "GR   ",
				0x0F, "DIR  ",
				0x80, "NSWAP",
				0x81, "SWAP "
			};
			char work[ 17 ];
			ZeroMemory(work, sizeof(work));
			MzDiskClass->GetDir( &dir, i );
			if ( ( 0 == dir.mode ) || ( 0x80 == dir.mode ) || ( 0x82 == dir.mode ) || ( 0xF == dir.mode ) )
			{
				continue;
			}
			memcpy( work, dir.filename, 16 );
			for ( j = 0; j < 17; j ++ )
			{
				if ( 0x0D == work[ j ] )
				{
					work[ j ] = '\0';
				}
			}
			// ファイル名
			item.mask = LVIF_TEXT;
			item.iItem = itemindex;
			item.iSubItem = 0;
			item.pszText = work;
			list->InsertItem( &item );
			// モード
			item.iSubItem = 1;
			item.pszText = "";
			for ( j = 0; j < 10; j ++ )
			{
				if ( modetbl[ j ].mode == dir.mode )
				{
					item.pszText = (char*)&modetbl[ j ].modestr;
					break;
				}
			}
			list->SetItem( &item );
			// 属性
			item.iSubItem = 2;
			sprintf_s( work, sizeof(work), "%02X", dir.attr );
			item.pszText = work;
			list->SetItem( &item );
			// ファイルサイズ
			item.iSubItem = 3;
			if( 4 == dir.mode )
			{
				sprintf_s( work, sizeof(work), "%7d", dir.size * 32 );
			}
			else
			{
				sprintf_s( work, sizeof(work), "%7d", dir.size );
			}
			item.pszText = work;
			list->SetItem( &item );
			// ロードアドレス
			item.iSubItem = 4;
			sprintf_s( work, sizeof(work), "%04X", dir.loadAdr );
			item.pszText = work;
			list->SetItem( &item );
			// 実行アドレス
			item.iSubItem = 5;
			sprintf_s( work, sizeof(work), "%04X", dir.runAdr );
			item.pszText = work;
			list->SetItem( &item );
			// 作成日付
			item.iSubItem = 6;
			year = 0;
			month = 0;
			day = 0;
			hour = 0;
			minute = 0;
			sprintf_s( work, sizeof(work), "%02d/%02d/%02d %02d:%02d", year, month, day, hour, minute );
			item.pszText = work;
			list->SetItem( &item );
			// 開始セクタ
			item.iSubItem = 7;
			sprintf_s( work, sizeof(work), "%d", dir.startSector );
			item.pszText = work;
			list->SetItem( &item );
			// 開始トラック
			item.iSubItem = 8;
			sprintf_s( work, sizeof(work), "%d", dir.startTrack );
			item.pszText = work;
			list->SetItem( &item );
			ItemToDirIndex[ itemindex ] = i;
			itemindex++;
		}
	}
	return 0;
}

void CMZDiskExplorerDoc::OnUpdateEditGetfile(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	if ( 1 == ImageInit )
	{
		pCmdUI->Enable( TRUE );
	}
	else
	{
		pCmdUI->Enable( FALSE );
	}
}

void CMZDiskExplorerDoc::OnUpdateEditGetboot(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	if ( 1 == ImageInit )
	{
		pCmdUI->Enable( TRUE );
	}
	else
	{
		pCmdUI->Enable( FALSE );
	}
}

void CMZDiskExplorerDoc::OnEditGetboot() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	char bootname[ 12 ];
	if(MzDiskClass->DiskType() == Disk::MZ2000)
	{
		int i;
		std::vector<unsigned char> buffer;
		MzDiskClass->ReadSector(buffer, 0, 1);
		ZeroMemory( bootname, sizeof( bootname ) );
		memcpy( bootname, &buffer[ 7 ], 11 );
		for ( i = 0; i < 12; i ++ )
		{
			if ( 0xD == bootname[ i ] )
			{
				bootname[ i ] = '\0';
			}
		}
	}
	else
	{
		strcpy_s(bootname, 12, "Boot.mzt");
	}
	// ダイアログ表示
	cGetBoot getbootdialog;
	char pathname[ 260 ];
	CString cpathname;
	cpathname = GetPathName();
	strcpy_s( pathname, sizeof(pathname), cpathname.GetBuffer( 260 ) );
	cPath path;
	path.SetPath( pathname );
	path.SetName( bootname );
	path.SetExtName( "" );
	getbootdialog.SetFile( path.GetPath() );
	getbootdialog.MzDiskClass = MzDiskClass;
	getbootdialog.SaveType = SaveType;
	getbootdialog.DoModal();
	SaveType = getbootdialog.SaveType;
}

void CMZDiskExplorerDoc::GetBootExec()
{
	OnEditGetboot();
}

void CMZDiskExplorerDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	if ( 1 == ImageInit )
	{
		pCmdUI->Enable( TRUE );
	}
	else
	{
		pCmdUI->Enable( FALSE );
	}
}

void CMZDiskExplorerDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	if ( 1 == ImageInit )
	{
		pCmdUI->Enable( TRUE );
	}
	else
	{
		pCmdUI->Enable( FALSE );
	}
}

void CMZDiskExplorerDoc::OnUpdateEditPutfile(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	if ( 1 == ImageInit )
	{
		pCmdUI->Enable( TRUE );
	}
	else
	{
		pCmdUI->Enable( FALSE );
	}
}

void CMZDiskExplorerDoc::OnEditPutfile()
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	CString datapath;
	datapath = GetPathName();
	CFileDialog SelFile( TRUE, NULL, NULL, OFN_HIDEREADONLY, "全てのファイル|*.*|MZT file|*.mzt||" );
	if ( IDCANCEL == SelFile.DoModal() )
	{
		return;
	}
	datapath = SelFile.GetPathName();
	OnEditPutfile(datapath);
}

void CMZDiskExplorerDoc::OnEditPutfile(CString datapath)
{
	if ( 0 == ImageInit )
	{
		return;
	}
	if (MzDiskClass == NULL)
	{
		return;
	}
	cPath path;
	path.SetPath( datapath.GetBuffer( 260 ) );
	cPutFile putfiledialog;
	CFile file;
	file.Open( path.GetPath(), CFile::modeRead );
	putfiledialog.FileSize = static_cast<int>(file.GetLength());
	putfiledialog.DataPath = datapath;
	SYSTEMTIME systemtime;
	::GetLocalTime( &systemtime );
	if ( 0 == _stricmp( path.GetExtName(), "MZT" ) )
	{
		MzDisk::MZTHEAD mzthead;
		char filename[ 18 ];
		ZeroMemory(filename, sizeof(filename));
		int i;
		file.Read( &mzthead, 128 );
		strncpy_s( filename, sizeof(filename), mzthead.filename, 17 );
		for ( i = 0; i < 17; i ++ )
		{
			if ( 0xD == filename[ i ] )
			{
				filename[ i ] = '\0';
			}
		}
		putfiledialog.FileName = filename;
		putfiledialog.Mode = mzthead.mode;
		putfiledialog.Attr = 0;
		putfiledialog.FileSize = mzthead.size;
		putfiledialog.LoadAdr = mzthead.loadAdr;
		putfiledialog.RunAdr = mzthead.runAdr;
		if(MzDiskClass->DiskType() == Disk::MZ2000)
		{
			putfiledialog.Year = systemtime.wYear % 100;
			putfiledialog.Month = systemtime.wMonth;
			putfiledialog.Day = systemtime.wDay;
			putfiledialog.Hour = systemtime.wHour;
			putfiledialog.Minute = systemtime.wMinute;
		}
		else
		{
			putfiledialog.Year = 0;
			putfiledialog.Month = 0;
			putfiledialog.Day = 0;
			putfiledialog.Hour = 0;
			putfiledialog.Minute = 0;
		}
		putfiledialog.FileType = 1;	// MZT
	}
	else
	{
		putfiledialog.FileName = path.GetPath( PATH_MODE_NAME | PATH_MODE_EXTNAME );
		if ( putfiledialog.FileSize > 65535 )
		{
			putfiledialog.Mode = 4;
		}
		else
		{
			putfiledialog.Mode = 1;
		}
		putfiledialog.Attr = 0;
		putfiledialog.LoadAdr = 0;
		putfiledialog.RunAdr = 0;
		putfiledialog.Year = systemtime.wYear % 100;
		putfiledialog.Month = systemtime.wMonth;
		putfiledialog.Day = systemtime.wDay;
		putfiledialog.Hour = systemtime.wHour;
		putfiledialog.Minute = systemtime.wMinute;
		putfiledialog.FileType = 0;	// BIN
	}
	file.Close();
	if ( putfiledialog.FileSize > 512 * 1024 )
	{
		MessageBox( NULL, "書き込むファイルのサイズが大きすぎます.\n最大 512 KB までです.", "エラー", MB_OK );
	}
	else
	{
		putfiledialog.MzDiskClass = MzDiskClass;
		putfiledialog.DoModal();
	}
	// ファイル画面作成
	MakeFileList( MzDiskClass->GetDirSector() );
	// ステータスバー描画
	CMainFrame *pMainFrame;
	char str[ 200 ];
	int use = MzDiskClass->GetUseBlockSize() * MzDiskClass->GetClusterSize();
	int total = MzDiskClass->GetAllBlockSize() * MzDiskClass->GetClusterSize();
	int free = total - use;
	sprintf_s( str, sizeof(str), "Type: %s    Size: %d/%d    Free: %d", MzDiskClass->DiskTypeText().c_str(), use, total, free );
	pMainFrame = ( CMainFrame* )AfxGetMainWnd();
	pMainFrame->PutStatusBarSize( str );
}

void CMZDiskExplorerDoc::OnUpdateEditPutboot(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	if ( 1 == ImageInit )
	{
		pCmdUI->Enable( TRUE );
	}
	else
	{
		pCmdUI->Enable( FALSE );
	}
}

void CMZDiskExplorerDoc::OnEditPutboot() 
{
	if(MzDiskClass == NULL)
	{
		return;
	}
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	CString datapath;
	datapath = GetPathName();
	CFileDialog SelFile( TRUE, NULL, NULL, OFN_HIDEREADONLY, "全てのファイル|*.*|MZT file|*.mzt||" );
	if ( IDCANCEL == SelFile.DoModal() )
	{
		return;
	}
	datapath = SelFile.GetPathName();
	cPath path;
	path.SetPath( datapath.GetBuffer( 260 ) );
	cPutBoot putBootdialog;
	CFile file;
	size_t size;
	file.Open( path.GetPath(), CFile::modeRead );
	putBootdialog.DataPath = datapath;
	if ( 0 == _stricmp( path.GetExtName(), "MZT" ) )
	{
		MzDisk::MZTHEAD mzthead;
		char filename[ 17 ];
		int i;
		file.Read( &mzthead, 128 );
		strncpy_s( filename, sizeof(filename), mzthead.filename, 17 );
		for ( i = 0; i < 17; i ++ )
		{
			if ( 0xD == filename[ i ] )
			{
				filename[ i ] = '\0';
			}
		}
		putBootdialog.BootName = filename;
		putBootdialog.FileType = 1;	// MZT
	}
	else
	{
		putBootdialog.BootName = path.GetPath( PATH_MODE_NAME | PATH_MODE_EXTNAME );
		putBootdialog.FileType = 0;	// BIN
	}
	size = file.GetLength();
	file.Close();
	if ( size > 65535 )
	{
		MessageBox( NULL, "書き込むファイルのサイズが大きすぎます.\n最大 65535 バイトまでです.", "エラー", MB_OK );
	}
	else
	{
		putBootdialog.RunAdr = 0;
		putBootdialog.Machine = Machine;
		putBootdialog.MzDiskClass = MzDiskClass;
		putBootdialog.DoModal();
		Machine = putBootdialog.Machine;
		// ステータスバー描画
		CMainFrame *pMainFrame;
		char str[ 200 ];
		int use = MzDiskClass->GetUseBlockSize() * MzDiskClass->GetClusterSize();
		int total = MzDiskClass->GetAllBlockSize() * MzDiskClass->GetClusterSize();
		int free = total - use;
		sprintf_s( str, sizeof(str), "Type: %s    Size: %d/%d    Free: %d", MzDiskClass->DiskTypeText().c_str(), use, total, free );
		pMainFrame = ( CMainFrame* )AfxGetMainWnd();
		pMainFrame->PutStatusBarSize( str );
	}
}

void CMZDiskExplorerDoc::OnUpdateEditDel(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	if ( 1 == ImageInit )
	{
		pCmdUI->Enable( TRUE );
	}
	else
	{
		pCmdUI->Enable( FALSE );
	}
}

void CMZDiskExplorerDoc::OnEditDel() 
{
	if(MzDiskClass == NULL)
	{
		return;
	}
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	POSITION pos;
	CMZDiskExplorerView *FileView;
	pos = GetFirstViewPosition();
	GetNextView( pos );
	FileView = (CMZDiskExplorerView*)GetNextView( pos );
	CListCtrl *list;
	list = &FileView->GetListCtrl();
	if(MzDiskClass->DiskType() == Disk::MZ2000)
	{
		MzDisk::DIRECTORY dir;
		int select = -1;
		select = list->GetNextItem( select, LVNI_ALL | LVNI_SELECTED );
		if ( -1 == select )
		{
			return;
		}
		if ( FileView->MessageBox( "選択したファイルを削除してよろしいですか？\nOK またはキャンセルを選んでください。\n", "ファイル削除", MB_OKCANCEL ) != IDOK )
		{
			return;
		}
		select = -1;
		while ( 1 )
		{
			select = list->GetNextItem( select, LVNI_ALL | LVNI_SELECTED );
			if ( -1 == select )
			{
				break;
			}
			MzDiskClass->GetDir( &dir, ItemToDirIndex[ select ] );
			if ( 0 == dir.mode )
			{
				continue;
			}
			MzDiskClass->DelFile( ItemToDirIndex[ select ] );
		}
		// ファイル画面作成
		MakeFileList( MzDiskClass->GetDirSector() );
		// ステータスバー描画
		CMainFrame *pMainFrame;
		char str[ 200 ];
		int use = MzDiskClass->GetUseBlockSize() * MzDiskClass->GetClusterSize();
		int total = MzDiskClass->GetAllBlockSize() * MzDiskClass->GetClusterSize();
		int free = total - use;
		sprintf_s( str, sizeof(str), "Type: %s    Size: %d/%d    Free: %d Bytes", MzDiskClass->DiskTypeText().c_str(), use, total, free );
		pMainFrame = ( CMainFrame* )AfxGetMainWnd();
		pMainFrame->PutStatusBarSize( str );
	}
	else if(MzDiskClass->DiskType() == Disk::MZ80K_SP6010)
	{
		Mz80Disk::DIRECTORY dir;
		int select = -1;
		select = list->GetNextItem( select, LVNI_ALL | LVNI_SELECTED );
		if ( -1 == select )
		{
			return;
		}
		if ( FileView->MessageBox( "選択したファイルを削除してよろしいですか？\nOK またはキャンセルを選んでください。\n", "ファイル削除", MB_OKCANCEL ) != IDOK )
		{
			return;
		}
		select = -1;
		while ( 1 )
		{
			select = list->GetNextItem( select, LVNI_ALL | LVNI_SELECTED );
			if ( -1 == select )
			{
				break;
			}
			MzDiskClass->GetDir( &dir, ItemToDirIndex[ select ] );
			if ( 0 == dir.mode )
			{
				continue;
			}
			MzDiskClass->DelFile( ItemToDirIndex[ select ] );
		}
		// ファイル画面作成
		MakeFileList( MzDiskClass->GetDirSector() );
		// ステータスバー描画
		CMainFrame *pMainFrame;
		char str[ 200 ];
		int use = MzDiskClass->GetUseBlockSize() * MzDiskClass->GetClusterSize();
		int total = MzDiskClass->GetAllBlockSize() * MzDiskClass->GetClusterSize();
		int free = total - use;
		sprintf_s( str, sizeof(str), "Type: %s    Size: %d/%d    Free: %d Bytes", MzDiskClass->DiskTypeText().c_str(), use, total, free );
		pMainFrame = ( CMainFrame* )AfxGetMainWnd();
		pMainFrame->PutStatusBarSize( str );
	}
}

void CMZDiskExplorerDoc::OnUpdateFileExportBeta(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	if ( 1 == ImageInit )
	{
		pCmdUI->Enable( TRUE );
	}
	else
	{
		pCmdUI->Enable( FALSE );
	}
}

void CMZDiskExplorerDoc::OnFileExportBeta() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	cPath path;
	path.SetPath(FilePath.GetBuffer(260));
	path.SetExtName("beta");
	CFileDialog SelFile( TRUE, "beta", path.GetPath(), OFN_HIDEREADONLY, "BETA file|*.beta|全てのファイル|*.*||" );
	if ( IDCANCEL == SelFile.DoModal() )
	{
		return;
	}
	CString datapath = SelFile.GetPathName();
	MzDiskClass->ExportBeta(datapath.GetBuffer(260));
	MessageBox(NULL, "betaイメージの作成が完了しました", "betaイメージ", MB_OK);
}
