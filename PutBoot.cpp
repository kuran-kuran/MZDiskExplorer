// PutBoot.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "MZDiskExplorer.h"
#include "MzDisk/MzDisk.hpp"
#include "MzDisk/Mz80Disk.hpp"
#include "PutBoot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cPutBoot �_�C�A���O


cPutBoot::cPutBoot(CWnd* pParent /*=NULL*/)
	: CDialog(cPutBoot::IDD, pParent)
{
	//{{AFX_DATA_INIT(cPutBoot)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
	//}}AFX_DATA_INIT
}


void cPutBoot::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cPutBoot)
	DDX_Control(pDX, IDC_RUNADR, m_RunAdr);
	DDX_Control(pDX, IDC_MACHINE, m_Machine);
	DDX_Control(pDX, IDC_BOOTNAME, m_BootName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(cPutBoot, CDialog)
	//{{AFX_MSG_MAP(cPutBoot)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// cPutBoot ���b�Z�[�W �n���h��

BOOL cPutBoot::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: ���̈ʒu�ɏ������̕⑫������ǉ����Ă�������
	char temp[ 16 ];
	m_BootName.SetSel( 0, -1, FALSE );
	m_BootName.Clear();
	m_BootName.ReplaceSel( BootName.GetBuffer( 260 ) );
	m_Machine.SetCurSel( Machine );
	m_RunAdr.SetSel( 0, -1, FALSE );
	m_RunAdr.Clear();
	sprintf_s( temp, sizeof(temp), "%04X", RunAdr );
	m_RunAdr.ReplaceSel( temp );
	if(MzDiskClass->DiskType() == Disk::MZ80K_SP6010)
	{
		m_Machine.EnableWindow(FALSE);
		m_RunAdr.EnableWindow(FALSE);
	}
	else
	{
		m_Machine.EnableWindow(TRUE);
		m_RunAdr.EnableWindow(TRUE);
	}

	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}

void cPutBoot::OnOK() 
{
	// TODO: ���̈ʒu�ɂ��̑��̌��ؗp�̃R�[�h��ǉ����Ă�������
	int result;
	if(MzDiskClass->DiskType() == Disk::MZ2000)
	{
		char temp[ 261 ];
		char *temp2;
		int i;
		m_BootName.SetSel( 0, -1, FALSE );
		ZeroMemory( temp, sizeof( temp ) );
		int size = m_BootName.GetLine( 0, temp, 10 );
		temp[size] = '\0';
		BootName = temp;
		Machine = m_Machine.GetCurSel();
		m_RunAdr.SetSel( 0, -1, FALSE );
		ZeroMemory( temp, sizeof( temp ) );
		size = m_RunAdr.GetLine( 0, temp, 260 );
		temp[size] = '\0';
		RunAdr = (unsigned short)strtol( temp, &temp2, 16 );
		MzDisk::IPL ipl;
		ZeroMemory( &ipl, sizeof( ipl ) );
		ipl.machine = Machine;
		strncpy_s( ipl.bootname, sizeof(ipl.bootname), BootName.GetBuffer( 10 ), 10 );
		for ( i = 0; i < 11; i ++ )
		{
			if ( '\0' == ipl.bootname[ i ] )
			{
				ipl.bootname[ i ] = '\xD';
				break;
			}
		}
		ipl.runAdr = RunAdr;
		if ( 0 == FileType )
		{
			result = MzDiskClass->PutBoot( DataPath.GetBuffer( 260 ), &ipl, Disk::FILEMODE_BIN );
		}
		else if ( 1 == FileType )
		{
			result = MzDiskClass->PutBoot( DataPath.GetBuffer( 260 ), &ipl, Disk::FILEMODE_MZT );
		}
	}
	else if(MzDiskClass->DiskType() == Disk::MZ80K_SP6010)
	{
		if ( 0 == FileType )
		{
			result = MzDiskClass->PutBoot( DataPath.GetBuffer( 260 ), NULL, Disk::FILEMODE_BIN );
		}
		else if ( 1 == FileType )
		{
			result = MzDiskClass->PutBoot( DataPath.GetBuffer( 260 ), NULL, Disk::FILEMODE_MZT );
		}
	}
	if( 0 != result )
	{
		MessageBox( "�u�[�g�v���O�����̏������݂Ɏ��s���܂���. ", "�G���[", MB_OK );
	}
	CDialog::OnOK();
}
