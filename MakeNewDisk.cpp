// MakeNewDisk.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "MZDiskExplorer.h"
#include "MakeNewDisk.h"
#include "MzDisk/Format.hpp"

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
	DDX_Control(pDX, IDC_VOLUME_NUMBER, m_VolumeNumber);
}


BEGIN_MESSAGE_MAP(cMakeNewDisk, CDialog)
	//{{AFX_MSG_MAP(cMakeNewDisk)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_DISKTYPE, &cMakeNewDisk::OnCbnSelchangeDisktype)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cMakeNewDisk ���b�Z�[�W �n���h��

BOOL cMakeNewDisk::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: ���̈ʒu�ɏ������̕⑫������ǉ����Ă�������
	m_DiskType.SetCurSel( DiskType );

	m_VolumeNumber.SetSel( 0, -1, FALSE );
	m_VolumeNumber.Clear();
	m_VolumeNumber.ReplaceSel( "1" );
	VolumeNumber = 1;
	OnCbnSelchangeDisktype();
	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}

void cMakeNewDisk::OnOK() 
{
	// TODO: ���̈ʒu�ɂ��̑��̌��ؗp�̃R�[�h��ǉ����Ă�������
	DiskType = m_DiskType.GetCurSel();
	AdjustVolumeNumber();
	CDialog::OnOK();
}


void cMakeNewDisk::OnCbnSelchangeDisktype()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	DiskType = m_DiskType.GetCurSel();
	if(DiskType <= 2)
	{
		m_VolumeNumber.EnableWindow( FALSE );
	}
	else
	{
		m_VolumeNumber.EnableWindow( TRUE );
	}
	AdjustVolumeNumber();
}

void cMakeNewDisk::AdjustVolumeNumber()
{
	char temp[ 261 ];
	m_VolumeNumber.SetSel( 0, -1, FALSE );
	ZeroMemory( temp, sizeof( temp ) );
	int size = m_VolumeNumber.GetLine( 0, temp, 260 );
	temp[size] = '\0';
	VolumeNumber = atoi( temp );
	if(VolumeNumber < 0)
	{
		VolumeNumber = 0;
	}
	if(DiskType >= 5)
	{
		// MZ-80K��0�`127
		if(VolumeNumber > 127)
		{
			VolumeNumber = 127;
		}
	}
	else if(DiskType <= 2)
	{
		// MZ-2500��1�Œ�
		VolumeNumber = 1;
	}
	else
	{
		// MZ-80B/2000��0�`255
		if(VolumeNumber > 255)
		{
			VolumeNumber = 255;
		}
	}
	m_VolumeNumber.SetSel( 0, -1, FALSE );
	m_VolumeNumber.Clear();
	m_VolumeNumber.ReplaceSel( dms::Format("%d", VolumeNumber).c_str() );
}
