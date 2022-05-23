// MZDiskExplorerDoc.cpp : CMZDiskExplorerDoc �N���X�̓���̒�`���s���܂��B
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
#include "mzdisk.h"
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
// CMZDiskExplorerDoc �N���X�̍\�z/����

CMZDiskExplorerDoc::CMZDiskExplorerDoc()
{
	// TODO: ���̈ʒu�ɂP�x�����Ă΂��\�z�p�̃R�[�h��ǉ����Ă��������B
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

	// TODO: ���̈ʒu�ɍď�����������ǉ����Ă��������B
	// (SDI �h�L�������g�͂��̃h�L�������g���ė��p���܂��B)
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
			U32 disktype = MZDISK_DISKTYPE_MZ2500_2DD;
			switch( newdisk.DiskType )
			{
				case 0:
					disktype = MZDISK_DISKTYPE_MZ2500_2DD;
					break;
				case 1:
					disktype = MZDISK_DISKTYPE_MZ2500_2D40;
					break;
				case 2:
					disktype = MZDISK_DISKTYPE_MZ2500_2D35;
					break;
				case 3:
					disktype = MZDISK_DISKTYPE_MZ2000_2D35;
					break;
				default:
					disktype = MZDISK_DISKTYPE_MZ2500_2DD;
					break;
			}
			MzDiskClass.Format( disktype );
			// �c���[��ʍ쐬
			DirHandleCount = 0;
			tree = &LeftView->GetTreeCtrl();
			tree->DeleteAllItems();
			DirHandle[ 0 ] = tree->InsertItem( "root" );
			DirSector[ 0 ] = 0x10;
			DirHandleCount ++;
			MakeTree( 0x10, DirHandle[ 0 ] );
			MzDiskClass.SetDirSector( -1 );
			// �t�@�C����ʍ쐬
			MakeFileList( 0x10 );
			MzDiskClass.SetDirSector( -1 );
			ImageInit = 1;
		}
	}
	FirstInit = 0;
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerDoc �V���A���C�[�[�V����

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
		// TODO: ���̈ʒu�ɕۑ��p�̃R�[�h��ǉ����Ă��������B
		if ( 0 == ImageInit ) {
			return;
		}
		char *temp;
		int size;
		size = MzDiskClass.GetSize( MZDISK_TYPE_D88 );
		temp = new char[ size ];
		MzDiskClass.Save( temp, MZDISK_TYPE_D88 );
		// �C���[�W�t�@�C����������
		CFile *file;
		file = ar.GetFile();
		ar.Flush();
		ar.Write( temp, size );
		delete [] temp;
	}
	else
	{
		// TODO: ���̈ʒu�ɓǂݍ��ݗp�̃R�[�h��ǉ����Ă��������B
		int length;
		CFile *file;
		char *temp;
		CString filepath;
		cPath path;
		// �C���[�W�t�@�C���ǂݍ���
		file = ar.GetFile();
		ar.Flush();
		filepath = file->GetFilePath();
		path.SetPath( filepath.GetBuffer( 260 ) );
		if ( ( 0 == stricmp( path.GetExtName(), "d88" ) ) ||
			 ( 0 == stricmp( path.GetExtName(), "d20" ) ) )
		{
			length = file->GetLength();
			temp = new char[ length ];
			file->Read( temp, length );
			MzDiskClass.Load( temp, MZDISK_TYPE_D88 );
			delete [] temp;
			// �c���[��ʍ쐬
			DirHandleCount = 0;
			CTreeCtrl* tree;
			tree = &LeftView->GetTreeCtrl();
			tree->DeleteAllItems();
			DirHandle[ 0 ] = tree->InsertItem( "root" );
			DirSector[ 0 ] = 0x10;
			DirHandleCount ++;
			MakeTree( 0x10, DirHandle[ 0 ] );
			MzDiskClass.SetDirSector( -1 );
			// �t�@�C����ʍ쐬
			MakeFileList( 0x10 );
			MzDiskClass.SetDirSector( -1 );
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
	CMainFrame *pMainFrame;
	char str[ 100 ];
	sprintf( str, "%d/%d", MzDiskClass.GetUseBlockSize() * MzDiskClass.GetClusterSize(), MzDiskClass.GetAllBlockSize() * MzDiskClass.GetClusterSize() );
	pMainFrame = ( CMainFrame* )AfxGetMainWnd();
	pMainFrame->PutStatusBarSize( str );
}

/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerDoc �N���X�̐f�f

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
// CMZDiskExplorerDoc �R�}���h

int CMZDiskExplorerDoc::MakeTree( int dirsector, HTREEITEM parenthandle )
{
	DIRECTORY dir;
	int i;
	POSITION pos;
	CLeftView *LeftView;
	pos = GetFirstViewPosition();
	LeftView = (CLeftView*)GetNextView( pos );
	CTreeCtrl *tree;
	tree = &LeftView->GetTreeCtrl();
	MzDiskClass.SetDirSector( dirsector );
	for ( i = 0; i < 64; i ++ )
	{
		int j;
		MzDiskClass.GetDir( &dir, i );
		if ( 0xF == dir.Mode ) {
			char dirname[ 17 ];
			memcpy( dirname, dir.Filename, 17 );
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
			DirSector[ DirHandleCount ] = dir.StartSector;
			int temp;
			temp = DirHandleCount;
			DirHandleCount ++;
			if ( 1 == MakeTree( dir.StartSector, DirHandle[ temp ] ) ) {
				return 1;
			}
			MzDiskClass.SetDirSector( dirsector );
			if ( DirHandleCount >= DIRHANDLE_MAX ) {
				return 1;
			}
		}
	}
	return 0;
}

int CMZDiskExplorerDoc::MakeFileList( int dirsector )
{
	POSITION pos;
	CMZDiskExplorerView *FileView;
	pos = GetFirstViewPosition();
	GetNextView( pos );
	FileView = (CMZDiskExplorerView*)GetNextView( pos );
	CListCtrl *list;
	list = &FileView->GetListCtrl();
	list->DeleteAllItems();
	// �t�@�C��
	MzDiskClass.SetDirSector( dirsector );
	LV_ITEM item;
	DIRECTORY dir;
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
			U8 mode;
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
		MzDiskClass.GetDir( &dir, i );
		if ( ( 0 == dir.Mode ) || ( 0x80 == dir.Mode ) || ( 0x82 == dir.Mode ) || ( 0xF == dir.Mode ) )
		{
			continue;
		}
		memcpy( work, dir.Filename, 17 );
		for ( j = 0; j < 17; j ++ )
		{
			if ( 0x0D == work[ j ] )
			{
				work[ j ] = '\0';
			}
		}
		// �t�@�C����
		item.mask = LVIF_TEXT;
		item.iItem = itemindex;
		item.iSubItem = 0;
		item.pszText = work;
		list->InsertItem( &item );
		// ���[�h
		item.iSubItem = 1;
		item.pszText = "";
		for ( j = 0; j < 10; j ++ )
		{
			if ( modetbl[ j ].mode == dir.Mode )
			{
				item.pszText = (char*)&modetbl[ j ].modestr;
				break;
			}
		}
		list->SetItem( &item );
		// ����
		item.iSubItem = 2;
		sprintf( work, "%02X", dir.Attr );
		item.pszText = work;
		list->SetItem( &item );
		// �t�@�C���T�C�Y
		item.iSubItem = 3;
		if( 4 == dir.Mode )
		{
			sprintf( work, "%7d", dir.Size * 32 );
		}
		else
		{
			sprintf( work, "%7d", dir.Size );
		}
		item.pszText = work;
		list->SetItem( &item );
		// ���[�h�A�h���X
		item.iSubItem = 4;
		sprintf( work, "%04X", dir.LoadAdr );
		item.pszText = work;
		list->SetItem( &item );
		// ���s�A�h���X
		item.iSubItem = 5;
		sprintf( work, "%04X", dir.RunAdr );
		item.pszText = work;
		list->SetItem( &item );
		// �쐬���t
		item.iSubItem = 6;
		year = ( dir.Date & 0xF ) + ( ( dir.Date >> 4 ) & 0xF ) * 10;
		month = ( ( dir.Date >> 11 ) & 0xF ) + ( ( dir.Date >> 15 ) & 0x1 ) * 10;
		day = ( ( dir.Date >> 5 ) & 0x8 ) + ( ( dir.Date >> 21 ) & 0x7 ) + ( ( dir.Date >> 9 ) & 0x3 ) * 10;
		hour = ( ( dir.Date >> 31 ) & 0x1 ) + ( ( dir.Date >> 15 ) & 0xE ) + ( ( dir.Date >> 19 ) & 0x3 ) * 10;
		minute = ( ( dir.Date >> 24 ) & 0xF ) + ( ( dir.Date >> 28 ) & 7 ) * 10;
		sprintf( work, "%02d/%02d/%02d %02d:%02d", year, month, day, hour, minute );
		item.pszText = work;
		list->SetItem( &item );
		// �J�n�Z�N�^
		item.iSubItem = 7;
		sprintf( work, "%04X", dir.StartSector );
		item.pszText = work;
		list->SetItem( &item );
		ItemToDirIndex[ itemindex ] = i;
		itemindex++;
	}
	return 0;
}

void CMZDiskExplorerDoc::OnUpdateEditGetfile(CCmdUI* pCmdUI) 
{
	// TODO: ���̈ʒu�� command update UI �n���h���p�̃R�[�h��ǉ����Ă�������
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
	// TODO: ���̈ʒu�� command update UI �n���h���p�̃R�[�h��ǉ����Ă�������
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
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	char buffer[ 256 ];
	char bootname[ 12 ];
	int i;
	MzDiskClass.GetSector( buffer, 0 );
	ZeroMemory( bootname, sizeof( bootname ) );
	memcpy( bootname, &buffer[ 7 ], 11 );
	for ( i = 0; i < 12; i ++ )
	{
		if ( 0xD == bootname[ i ] )
		{
			bootname[ i ] = '\0';
		}
	}
	// �_�C�A���O�\��
	cGetBoot getbootdialog;
	char pathname[ 260 ];
	CString cpathname;
	cpathname = GetPathName();
	strcpy( pathname, cpathname.GetBuffer( 260 ) );
	cPath path;
	path.SetPath( pathname );
	path.SetName( bootname );
	path.SetExtName( "" );
	getbootdialog.SetFile( path.GetPath() );
	getbootdialog.MzDiskClass = &MzDiskClass;
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
	// TODO: ���̈ʒu�� command update UI �n���h���p�̃R�[�h��ǉ����Ă�������
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
	// TODO: ���̈ʒu�� command update UI �n���h���p�̃R�[�h��ǉ����Ă�������
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
	// TODO: ���̈ʒu�� command update UI �n���h���p�̃R�[�h��ǉ����Ă�������
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
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	CString datapath;
	datapath = GetPathName();
	CFileDialog SelFile( TRUE, NULL, NULL, OFN_HIDEREADONLY, "�S�Ẵt�@�C��|*.*|MZT file|*.mzt||" );
	if ( IDCANCEL == SelFile.DoModal() )
	{
		return;
	}
	datapath = SelFile.GetPathName();
	cPath path;
	path.SetPath( datapath.GetBuffer( 260 ) );
	cPutFile putfiledialog;
	CFile file;
	file.Open( path.GetPath(), CFile::modeRead );
	putfiledialog.FileSize = file.GetLength();
	putfiledialog.DataPath = datapath;
	SYSTEMTIME systemtime;
	::GetLocalTime( &systemtime );
	if ( 0 == stricmp( path.GetExtName(), "MZT" ) )
	{
		MZTHEAD mzthead;
		char filename[ 17 ];
		int i;
		file.Read( &mzthead, 128 );
		strncpy( filename, mzthead.Filename, 17 );
		for ( i = 0; i < 17; i ++ )
		{
			if ( 0xD == filename[ i ] )
			{
				filename[ i ] = '\0';
			}
		}
		putfiledialog.FileName = filename;
		putfiledialog.Mode = mzthead.Mode;
		putfiledialog.Attr = 0;
		putfiledialog.FileSize = mzthead.Size;
		putfiledialog.LoadAdr = mzthead.LoadAdr;
		putfiledialog.RunAdr = mzthead.RunAdr;
		putfiledialog.Year = systemtime.wYear % 100;
		putfiledialog.Month = systemtime.wMonth;
		putfiledialog.Day = systemtime.wDay;
		putfiledialog.Hour = systemtime.wHour;
		putfiledialog.Minute = systemtime.wMinute;
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
		MessageBox( NULL, "�������ރt�@�C���̃T�C�Y���傫�����܂�.\n�ő� 512 KB �܂łł�.", "�G���[", MB_OK );
	}
	else
	{
		putfiledialog.MzDiskClass = &MzDiskClass;
		putfiledialog.DoModal();
	}
	// �t�@�C����ʍ쐬
	MakeFileList( MzDiskClass.GetDirSector() );
	// �X�e�[�^�X�o�[�`��
	CMainFrame *pMainFrame;
	char str[ 100 ];
	sprintf( str, "%d/%d", MzDiskClass.GetUseBlockSize() * MzDiskClass.GetClusterSize(), MzDiskClass.GetAllBlockSize() * MzDiskClass.GetClusterSize() );
	pMainFrame = ( CMainFrame* )AfxGetMainWnd();
	pMainFrame->PutStatusBarSize( str );
}

void CMZDiskExplorerDoc::OnUpdateEditPutboot(CCmdUI* pCmdUI) 
{
	// TODO: ���̈ʒu�� command update UI �n���h���p�̃R�[�h��ǉ����Ă�������
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
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	CString datapath;
	datapath = GetPathName();
	CFileDialog SelFile( TRUE, NULL, NULL, OFN_HIDEREADONLY, "�S�Ẵt�@�C��|*.*|MZT file|*.mzt||" );
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
	if ( 0 == stricmp( path.GetExtName(), "MZT" ) )
	{
		MZTHEAD mzthead;
		char filename[ 17 ];
		int i;
		file.Read( &mzthead, 128 );
		strncpy( filename, mzthead.Filename, 17 );
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
		MessageBox( NULL, "�������ރt�@�C���̃T�C�Y���傫�����܂�.\n�ő� 65535 �o�C�g�܂łł�.", "�G���[", MB_OK );
	}
	else
	{
		putBootdialog.RunAdr = 0;
		putBootdialog.Machine = Machine;
		putBootdialog.MzDiskClass = &MzDiskClass;
		putBootdialog.DoModal();
		Machine = putBootdialog.Machine;
		// �X�e�[�^�X�o�[�`��
		CMainFrame *pMainFrame;
		char str[ 100 ];
		sprintf( str, "%d/%d", MzDiskClass.GetUseBlockSize() * MzDiskClass.GetClusterSize(), MzDiskClass.GetAllBlockSize() * MzDiskClass.GetClusterSize() );
		pMainFrame = ( CMainFrame* )AfxGetMainWnd();
		pMainFrame->PutStatusBarSize( str );
	}
}

void CMZDiskExplorerDoc::OnUpdateEditDel(CCmdUI* pCmdUI) 
{
	// TODO: ���̈ʒu�� command update UI �n���h���p�̃R�[�h��ǉ����Ă�������
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
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	POSITION pos;
	CMZDiskExplorerView *FileView;
	pos = GetFirstViewPosition();
	GetNextView( pos );
	FileView = (CMZDiskExplorerView*)GetNextView( pos );
	CListCtrl *list;
	list = &FileView->GetListCtrl();
	DIRECTORY dir;
	int select = -1;
	select = list->GetNextItem( select, LVNI_ALL | LVNI_SELECTED );
	if ( -1 == select )
	{
		return;
	}
	if ( FileView->MessageBox( "�I�������t�@�C�����폜���Ă�낵���ł����H\nOK �܂��̓L�����Z����I��ł��������B\n", "�t�@�C���폜", MB_OKCANCEL ) != IDOK )
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
		MzDiskClass.GetDir( &dir, ItemToDirIndex[ select ] );
		if ( 0 == dir.Mode )
		{
			continue;
		}
		MzDiskClass.DelFile( ItemToDirIndex[ select ] );
	}
	// �t�@�C����ʍ쐬
	MakeFileList( MzDiskClass.GetDirSector() );
	// �X�e�[�^�X�o�[�`��
	CMainFrame *pMainFrame;
	char str[ 100 ];
	sprintf( str, "%d/%d", MzDiskClass.GetUseBlockSize() * MzDiskClass.GetClusterSize(), MzDiskClass.GetAllBlockSize() * MzDiskClass.GetClusterSize() );
	pMainFrame = ( CMainFrame* )AfxGetMainWnd();
	pMainFrame->PutStatusBarSize( str );
}

void CMZDiskExplorerDoc::OnUpdateFileExportBeta(CCmdUI* pCmdUI) 
{
	// TODO: ���̈ʒu�� command update UI �n���h���p�̃R�[�h��ǉ����Ă�������
	if ( 1 == ImageInit )
	{
//		pCmdUI->Enable( TRUE );
		pCmdUI->Enable( FALSE );
	}
	else
	{
		pCmdUI->Enable( FALSE );
	}
}

void CMZDiskExplorerDoc::OnFileExportBeta() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	
}
