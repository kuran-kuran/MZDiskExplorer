// MakeNewDisk.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "MZDiskExplorer.h"
#include "MakeNewDisk.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cMakeNewDisk �_�C�A���O


cMakeNewDisk::cMakeNewDisk(CWnd* pParent /*=NULL*/)
	: CDialog(cMakeNewDisk::IDD, pParent)
{
	//{{AFX_DATA_INIT(cMakeNewDisk)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
	//}}AFX_DATA_INIT
}


void cMakeNewDisk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cMakeNewDisk)
	DDX_Control(pDX, IDC_DISKTYPE, m_DiskType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(cMakeNewDisk, CDialog)
	//{{AFX_MSG_MAP(cMakeNewDisk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cMakeNewDisk ���b�Z�[�W �n���h��

BOOL cMakeNewDisk::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: ���̈ʒu�ɏ������̕⑫������ǉ����Ă�������
	m_DiskType.SetCurSel( DiskType );
	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}

void cMakeNewDisk::OnOK() 
{
	// TODO: ���̈ʒu�ɂ��̑��̌��ؗp�̃R�[�h��ǉ����Ă�������
	DiskType = m_DiskType.GetCurSel();
	CDialog::OnOK();
}
