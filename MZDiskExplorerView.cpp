// MZDiskExplorerView.cpp : CMZDiskExplorerView �N���X�̓���̒�`���s���܂��B
//

#include "stdafx.h"
#include "MZDiskExplorer.h"

#include "MZDiskExplorerDoc.h"
#include "MZDiskExplorerView.h"
#include "GetFile.h"
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
	// �W������R�}���h
	ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerView �N���X�̍\�z/����

CMZDiskExplorerView::CMZDiskExplorerView()
{
	// TODO: ���̏ꏊ�ɍ\�z�p�̃R�[�h��ǉ����Ă��������B
	ColumnFlag = 0;
}

CMZDiskExplorerView::~CMZDiskExplorerView()
{
}

BOOL CMZDiskExplorerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ���̈ʒu�� CREATESTRUCT cs ���C������ Window �N���X�܂��̓X�^�C����
	//  �C�����Ă��������B

	cs.style |= LVS_REPORT;
	cs.dwExStyle |= WS_EX_ACCEPTFILES;
	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerView �N���X�̕`��

void CMZDiskExplorerView::OnDraw(CDC* pDC)
{
	CMZDiskExplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CListCtrl& refCtrl = GetListCtrl();
	refCtrl.InsertItem(0, "Item!");
	// TODO: ���̏ꏊ�Ƀl�C�e�B�u �f�[�^�p�̕`��R�[�h��ǉ����܂��B
}

void CMZDiskExplorerView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	// TODO: GetListCtrl() �����o�֐��̌Ăяo����ʂ��Ē��ڂ��̃��X�g �R���g���[����
	//  �A�N�Z�X���邱�Ƃɂ���� ListView ���A�C�e���ŌŒ�ł��܂��B
	if ( 1 == ColumnFlag )
	{
		return;
	}
	CListCtrl &list = GetListCtrl();
	// ����
	LV_COLUMN column;
	column.mask = LVCF_TEXT | LVCF_WIDTH;
	column.pszText = "�t�@�C����";
	column.cx = 160;
	list.InsertColumn( 0, &column );
	column.pszText = "���[�h";
	column.cx = 60;
	list.InsertColumn( 1, &column );
	column.pszText = "����";
	column.cx = 60;
	list.InsertColumn( 2, &column );
	column.pszText = "�t�@�C���T�C�Y";
	column.cx = 80;
	list.InsertColumn( 3, &column );
	column.pszText = "���[�h�A�h���X";
	column.cx = 80;
	list.InsertColumn( 4, &column );
	column.pszText = "���s�A�h���X";
	column.cx = 80;
	list.InsertColumn( 5, &column );
	column.pszText = "���t";
	column.cx = 80;
	list.InsertColumn( 6, &column );
	column.pszText = "�J�n�Z�N�^";
	column.cx = 80;
	list.InsertColumn( 7, &column );
	ColumnFlag = 1;
}

/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerView �N���X�̈��

BOOL CMZDiskExplorerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �f�t�H���g�̈������
	return DoPreparePrinting(pInfo);
}

void CMZDiskExplorerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ����O�̓��ʂȏ�����������ǉ����Ă��������B
}

void CMZDiskExplorerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �����̌㏈����ǉ����Ă��������B
}

/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerView �N���X�̐f�f

#ifdef _DEBUG
void CMZDiskExplorerView::AssertValid() const
{
	CListView::AssertValid();
}

void CMZDiskExplorerView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CMZDiskExplorerDoc* CMZDiskExplorerView::GetDocument() // ��f�o�b�O �o�[�W�����̓C�����C���ł��B
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMZDiskExplorerDoc)));
	return (CMZDiskExplorerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerView �N���X�̃��b�Z�[�W �n���h��
void CMZDiskExplorerView::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	// TODO: ���[�U�[�ɂ��E�C���h�E�̃r���[ �X�^�C���ύX�ɑΉ�����R�[�h��ǉ����Ă�������
}

void CMZDiskExplorerView::OnEditGetfile() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	DIRECTORY dir;
	char savepath[ MAX_PATH ];
	char filename[ 18 ];
	char extname[ 18 ];
	CString DataPath;
	int i;
	ZeroMemory( savepath, MAX_PATH );
	// �t�@�C�����o��
	CListCtrl &list = GetListCtrl();
	CMZDiskExplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	int select = -1;
	int AllOk = 0;
	char pathname[ 260 ];
	CString cpathname;
	cpathname = pDoc->GetPathName();
	strcpy( pathname, cpathname.GetBuffer( 260 ) );
	while ( 1 )
	{
		select = list.GetNextItem( select, LVNI_ALL | LVNI_SELECTED );
		if ( -1 == select )
		{
			break;
		}
		pDoc->MzDiskClass.GetDir( &dir, pDoc->ItemToDirIndex[ select ] );
		ZeroMemory( filename, sizeof( filename ) );
		ZeroMemory( extname, sizeof( extname ) );
		strncpy( filename, dir.Filename, 17 );
		for ( i=0; i<17; i++ ) {
			if ( 0x0D == filename[ i ] ) {
				filename[ i ] = 0;
			}
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
		}
		// �_�C�A���O�\��
		cGetFile getfiledialog;
		cPath path;
		path.SetPath( pathname );
		path.SetName( filename );
		path.SetExtName( extname );
		strcpy( getfiledialog.FileName, path.GetPath() );
		getfiledialog.MzDiskClass = &pDoc->MzDiskClass;
		getfiledialog.DirIndex = pDoc->ItemToDirIndex[ select ];
		getfiledialog.SaveType = pDoc->SaveType;
		getfiledialog.AllOk = AllOk;
		getfiledialog.DoModal();
		pDoc->SaveType = getfiledialog.SaveType;
		AllOk = getfiledialog.AllOk;
		strcpy( pathname, getfiledialog.FileName );
	}
}

void CMZDiskExplorerView::OnEditGetboot() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	CMZDiskExplorerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->GetBootExec();
}

void CMZDiskExplorerView::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
	UINT uiCount = DragQueryFile(hDropInfo, ~0lu, NULL, 0);
	for(UINT i = 0; i < uiCount; ++ i)
	{
		UINT uiLen = DragQueryFile(hDropInfo, i, NULL, 0);
		CString cpathname;
		DragQueryFile(hDropInfo, i, cpathname.GetBuffer(uiLen + 1), uiLen + 1);
		cpathname.ReleaseBuffer();
		char pathname[260];
		strcpy(pathname, cpathname.GetBuffer(260));
		cPath path;
		path.SetPath(pathname);
		if((stricmp(path.GetExtName(), "D88") == 0) && (stricmp(path.GetExtName(), "D88") == 0))
		{
			CView::OnDropFiles(hDropInfo);
			return;
		}
		else
		{
			CMZDiskExplorerDoc* pDoc = GetDocument();
			ASSERT_VALID(pDoc);
			// �t�@�C�����h���b�v����
			pDoc->OnEditPutfile(cpathname);
		}
	}
}
