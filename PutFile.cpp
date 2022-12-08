// PutFile.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "MZDiskExplorer.h"
#include "PutFile.h"
#include "MzDisk/MzDisk.hpp"
#include "MzDisk/Mz80Disk.hpp"
#include "MzDisk/Mz80SP6110Disk.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// cPutFile �_�C�A���O


cPutFile::cPutFile(CWnd* pParent /*=NULL*/)
	: CDialog(cPutFile::IDD, pParent)
,DataPath()
,FileName()
,Mode(0)
,Attr(0)
,FileSize(0)
,LoadAdr(0)
,RunAdr(0)
,Year(0)
,Month(0)
,Day(0)
,Hour(0)
,Minute(0)
,FileType(0)
,MzDiskClass(NULL)
,m_Hour()
,m_Month()
,m_Minute()
,m_Day()
,m_Year()
,m_RunAdr()
,m_Mode()
,m_LoadAdr()
,m_FileSize()
,m_FileName()
,m_Attr()
{
	//{{AFX_DATA_INIT(cPutFile)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
	//}}AFX_DATA_INIT
}


void cPutFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(cPutFile)
	DDX_Control(pDX, IDC_HOUR, m_Hour);
	DDX_Control(pDX, IDC_MONTH, m_Month);
	DDX_Control(pDX, IDC_MINUTE, m_Minute);
	DDX_Control(pDX, IDC_DAY, m_Day);
	DDX_Control(pDX, IDC_YEAR, m_Year);
	DDX_Control(pDX, IDC_RUNADR, m_RunAdr);
	DDX_Control(pDX, IDC_MODE, m_Mode);
	DDX_Control(pDX, IDC_LOADADR, m_LoadAdr);
	DDX_Control(pDX, IDC_FILESIZE, m_FileSize);
	DDX_Control(pDX, IDC_FILENAME, m_FileName);
	DDX_Control(pDX, IDC_ATTR, m_Attr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(cPutFile, CDialog)
	//{{AFX_MSG_MAP(cPutFile)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL cPutFile::OnInitDialog() 
{
	char temp[ 20 ];
	CDialog::OnInitDialog();
	
	// TODO: ���̈ʒu�ɏ������̕⑫������ǉ����Ă�������
	m_FileName.SetSel( 0, -1, FALSE );
	m_FileName.Clear();
	m_FileName.ReplaceSel( FileName.GetBuffer( 260 ) );
	m_Mode.SetCurSel( Mode - 1 );
	m_Attr.SetCurSel( Attr & 1 );
	m_FileSize.SetSel( 0, -1, FALSE );
	m_FileSize.Clear();
	sprintf_s( temp, sizeof(temp), "%d", FileSize );
	m_FileSize.ReplaceSel( temp );
	m_LoadAdr.SetSel( 0, -1, FALSE );
	m_LoadAdr.Clear();
	sprintf_s( temp, sizeof(temp), "%04X", LoadAdr );
	m_LoadAdr.ReplaceSel( temp );
	m_RunAdr.SetSel( 0, -1, FALSE );
	m_RunAdr.Clear();
	sprintf_s( temp, sizeof(temp), "%04X", RunAdr );
	m_RunAdr.ReplaceSel( temp );
	m_Year.Clear();
	sprintf_s( temp, sizeof(temp), "%02d", Year );
	m_Year.ReplaceSel( temp );
	m_Month.Clear();
	sprintf_s( temp, sizeof(temp), "%02d", Month );
	m_Month.ReplaceSel( temp );
	m_Day.Clear();
	sprintf_s( temp, sizeof(temp), "%02d", Day );
	m_Day.ReplaceSel( temp );
	m_Hour.Clear();
	sprintf_s( temp, sizeof(temp), "%02d", Hour );
	m_Hour.ReplaceSel( temp );
	m_Minute.Clear();
	sprintf_s( temp, sizeof(temp), "%02d", Minute );
	m_Minute.ReplaceSel( temp );
	if((MzDiskClass->DiskType() == Disk::MZ80K_SP6010) || (MzDiskClass->DiskType() == Disk::MZ80K_SP6110))
	{
		m_Year.EnableWindow(FALSE);
		m_Month.EnableWindow(FALSE);
		m_Day.EnableWindow(FALSE);
		m_Hour.EnableWindow(FALSE);
		m_Minute.EnableWindow(FALSE);
	}
	else
	{
		m_Year.EnableWindow(TRUE);
		m_Month.EnableWindow(TRUE);
		m_Day.EnableWindow(TRUE);
		m_Hour.EnableWindow(TRUE);
		m_Minute.EnableWindow(TRUE);
	}
	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}

void cPutFile::OnOK() 
{
	// TODO: ���̈ʒu�ɂ��̑��̌��ؗp�̃R�[�h��ǉ����Ă�������
	if(MzDiskClass->DiskType() == Disk::MZ2000)
	{
		MzDisk::DIRECTORY dir;
		int i;
		char temp[ 261 ];
		char *temp2;
		int mode = 0;
		m_FileName.SetSel( 0, -1, FALSE );
		ZeroMemory( temp, sizeof( temp ) );
		int size = m_FileName.GetLine( 0, temp, 16 );
		temp[size] = '\0';
		FileName = temp;
		Mode = m_Mode.GetCurSel() + 1;
		if( 1 == m_Attr.GetCurSel() )
		{
			Attr |= 0x1;
		}
		m_FileSize.SetSel( 0, -1, FALSE );
		ZeroMemory( temp, sizeof( temp ) );
		size = m_FileSize.GetLine( 0, temp, 260 );
		temp[size] = '\0';
		FileSize = atoi( temp );
		m_LoadAdr.SetSel( 0, -1, FALSE );
		temp[size] = '\0';
		ZeroMemory( temp, sizeof( temp ) );
		size = m_LoadAdr.GetLine( 0, temp, 260 );
		temp[size] = '\0';
		LoadAdr = (unsigned short)strtol( temp, &temp2, 16 );
		m_RunAdr.SetSel( 0, -1, FALSE );
		ZeroMemory( temp, sizeof( temp ) );
		size = m_RunAdr.GetLine( 0, temp, 260 );
		temp[size] = '\0';
		RunAdr = (unsigned short)strtol( temp, &temp2, 16 );
		// ���t
		size = m_Year.GetLine(0, temp, 260);
		temp[size] = '\0';
		Year = (int)strtol(temp, &temp2, 10);
		size = m_Month.GetLine(0, temp, 260);
		temp[size] = '\0';
		Month = (int)strtol(temp, &temp2, 10);
		size = m_Day.GetLine(0, temp, 260);
		temp[size] = '\0';
		Day = (int)strtol(temp, &temp2, 10);
		size = m_Hour.GetLine(0, temp, 260);
		temp[size] = '\0';
		Hour = (int)strtol(temp, &temp2, 10);
		size = m_Minute.GetLine(0, temp, 260);
		temp[size] = '\0';
		Minute = (int)strtol(temp, &temp2, 10);

		ZeroMemory( &dir, sizeof( dir ) );
		dir.mode = Mode;
		if( 5 == dir.mode )
		{
			dir.mode = 3;
		}
		strncpy_s( dir.filename, sizeof(dir.filename), FileName.GetBuffer( 16 ), 16 );
		for ( i = 0; i < 17; i ++ )
		{
			if ( '\0' == dir.filename[ i ] )
			{
				dir.filename[ i ] = '\xD';
				break;
			}
		}
		dir.attr = Attr;
		if(dir.mode == 4)
		{
			dir.size = FileSize / 32;
		}
		else
		{
			dir.size = FileSize;
		}
		dir.loadAdr = LoadAdr;
		dir.runAdr = RunAdr;
		dir.date = ( ( ( Year % 100 ) / 10 ) << 28 ) +
					( ( ( Year % 100 ) % 10 ) << 24 ) +
					( ( Month / 10 ) << 23 ) +
					( ( Month % 10 ) << 19 ) +
					( ( Day / 10 ) << 17 ) +
					( ( Day % 10 ) << 13 ) +
					( ( Hour / 10 ) << 11 ) +
					( ( Hour % 10 ) << 7 ) +
					( ( Minute / 10 ) << 4 ) +
					( ( Minute % 10 ) );
		dir.date = ( dir.date >> 24 ) + ( ( dir.date >> 8 ) & 0xFF00 ) + ( ( dir.date << 8 ) & 0xFF0000 ) + ( ( dir.date << 24 ) & 0xFF000000 );
		int result;
		if ( 0 == FileType )
		{
			result = MzDiskClass->PutFile( DataPath.GetBuffer( 260 ), &dir, Disk::FILEMODE_BIN, Mode );
		}
		else
		{
			result = MzDiskClass->PutFile( DataPath.GetBuffer( 260 ), &dir, Disk::FILEMODE_MZT, Mode );
		}
		if( 1 == result )
		{
			MessageBox( "�f�B�X�N�C���[�W�ւ̃t�@�C���������݂Ɏ��s���܂���. \n�f�B���N�g���ɋ󂫂�����܂���.", "�G���[", MB_OK );
		}
		else if( ( 2 == result ) || ( 3 == result ) )
		{
			MessageBox( "�f�B�X�N�C���[�W�ւ̃t�@�C���������݂Ɏ��s���܂���. \n�t�@�C����ǂݍ��ނ��Ƃ��ł��܂���.", "�G���[", MB_OK );
		}
		else if( 4 == result )
		{
			MessageBox( "�f�B�X�N�C���[�W�ւ̃t�@�C���������݂Ɏ��s���܂���. \n�r�b�g�}�b�v�̋󂫂�����܂���.", "�G���[", MB_OK );
		}
		else if( 5 == result )
		{
			MessageBox( "�f�B�X�N�C���[�W�ւ̃t�@�C���������݂Ɏ��s���܂���. \n�����t�@�C���������łɑ��݂��܂�.", "�G���[", MB_OK );
		}
		else if( 0 != result )
		{
			MessageBox( "�f�B�X�N�C���[�W�ւ̃t�@�C���������݂Ɏ��s���܂���. \n�����͕s���ł�.", "�G���[", MB_OK );
		}
	}
	else if(MzDiskClass->DiskType() == Disk::MZ80K_SP6010)
	{
		Mz80Disk::DIRECTORY dir;
		int i;
		char temp[ 261 ];
		char *temp2;
		int mode = 0;
		m_FileName.SetSel( 0, -1, FALSE );
		ZeroMemory( temp, sizeof( temp ) );
		int size = m_FileName.GetLine( 0, temp, 16 );
		temp[size] = '\0';
		FileName = temp;
		Mode = m_Mode.GetCurSel() + 1;
		if( 1 == m_Attr.GetCurSel() )
		{
			Attr |= 0x1;
		}
		m_FileSize.SetSel( 0, -1, FALSE );
		ZeroMemory( temp, sizeof( temp ) );
		size = m_FileSize.GetLine( 0, temp, 260 );
		temp[size] = '\0';
		FileSize = atoi( temp );
		m_LoadAdr.SetSel( 0, -1, FALSE );
		temp[size] = '\0';
		ZeroMemory( temp, sizeof( temp ) );
		size = m_LoadAdr.GetLine( 0, temp, 260 );
		temp[size] = '\0';
		LoadAdr = (unsigned short)strtol( temp, &temp2, 16 );
		m_RunAdr.SetSel( 0, -1, FALSE );
		ZeroMemory( temp, sizeof( temp ) );
		size = m_RunAdr.GetLine( 0, temp, 260 );
		temp[size] = '\0';
		size = RunAdr = (unsigned short)strtol( temp, &temp2, 16 );

		ZeroMemory( &dir, sizeof( dir ) );
		dir.mode = Mode;
		if( dir.mode >= 2 )
		{
			dir.mode = 0;
		}
		strncpy_s( dir.filename, sizeof(dir.filename), FileName.GetBuffer( 16 ), 16 );
		for ( i = 0; i < 17; i ++ )
		{
			if ( '\0' == dir.filename[ i ] )
			{
				dir.filename[ i ] = '\xD';
				break;
			}
		}
		dir.attr = Attr;
		if(dir.mode == 4)
		{
			dir.size = FileSize / 32;
		}
		else
		{
			dir.size = FileSize;
		}
		dir.loadAdr = LoadAdr;
		dir.runAdr = RunAdr;
		int result;
		if ( 0 == FileType )
		{
			result = MzDiskClass->PutFile( DataPath.GetBuffer( 260 ), &dir, Disk::FILEMODE_BIN, Mode );
		}
		else
		{
			result = MzDiskClass->PutFile( DataPath.GetBuffer( 260 ), &dir, Disk::FILEMODE_MZT, Mode );
		}
		if( 1 == result )
		{
			MessageBox( "�f�B�X�N�C���[�W�ւ̃t�@�C���������݂Ɏ��s���܂���. \n�f�B���N�g���ɋ󂫂�����܂���.", "�G���[", MB_OK );
		}
		else if( ( 2 == result ) || ( 3 == result ) )
		{
			MessageBox( "�f�B�X�N�C���[�W�ւ̃t�@�C���������݂Ɏ��s���܂���. \n�t�@�C����ǂݍ��ނ��Ƃ��ł��܂���.", "�G���[", MB_OK );
		}
		else if( 4 == result )
		{
			MessageBox( "�f�B�X�N�C���[�W�ւ̃t�@�C���������݂Ɏ��s���܂���. \n�r�b�g�}�b�v�̋󂫂�����܂���.", "�G���[", MB_OK );
		}
		else if( 5 == result )
		{
			MessageBox( "�f�B�X�N�C���[�W�ւ̃t�@�C���������݂Ɏ��s���܂���. \n�����t�@�C���������łɑ��݂��܂�.", "�G���[", MB_OK );
		}
		else if( 0 != result )
		{
			MessageBox( "�f�B�X�N�C���[�W�ւ̃t�@�C���������݂Ɏ��s���܂���. \n�����͕s���ł�.", "�G���[", MB_OK );
		}
	}
	else if(MzDiskClass->DiskType() == Disk::MZ80K_SP6110)
	{
		MzDisk::DIRECTORY dir;
		int i;
		char temp[ 261 ];
		char *temp2;
		int mode = 0;
		m_FileName.SetSel( 0, -1, FALSE );
		ZeroMemory( temp, sizeof( temp ) );
		int size = m_FileName.GetLine( 0, temp, 16 );
		temp[size] = '\0';
		FileName = temp;
		Mode = m_Mode.GetCurSel() + 1;
		if( 1 == m_Attr.GetCurSel() )
		{
			Attr |= 0x1;
		}
		m_FileSize.SetSel( 0, -1, FALSE );
		ZeroMemory( temp, sizeof( temp ) );
		size = m_FileSize.GetLine( 0, temp, 260 );
		temp[size] = '\0';
		FileSize = atoi( temp );
		m_LoadAdr.SetSel( 0, -1, FALSE );
		temp[size] = '\0';
		ZeroMemory( temp, sizeof( temp ) );
		size = m_LoadAdr.GetLine( 0, temp, 260 );
		temp[size] = '\0';
		LoadAdr = (unsigned short)strtol( temp, &temp2, 16 );
		m_RunAdr.SetSel( 0, -1, FALSE );
		ZeroMemory( temp, sizeof( temp ) );
		size = m_RunAdr.GetLine( 0, temp, 260 );
		temp[size] = '\0';
		RunAdr = (unsigned short)strtol( temp, &temp2, 16 );
		// ���t
		size = m_Year.GetLine(0, temp, 260);
		temp[size] = '\0';
		Year = (int)strtol(temp, &temp2, 10);
		size = m_Month.GetLine(0, temp, 260);
		temp[size] = '\0';
		Month = (int)strtol(temp, &temp2, 10);
		size = m_Day.GetLine(0, temp, 260);
		temp[size] = '\0';
		Day = (int)strtol(temp, &temp2, 10);
		size = m_Hour.GetLine(0, temp, 260);
		temp[size] = '\0';
		Hour = (int)strtol(temp, &temp2, 10);
		size = m_Minute.GetLine(0, temp, 260);
		temp[size] = '\0';
		Minute = (int)strtol(temp, &temp2, 10);

		ZeroMemory( &dir, sizeof( dir ) );
		dir.mode = Mode;
		if( 5 == dir.mode )
		{
			dir.mode = 3;
		}
		strncpy_s( dir.filename, sizeof(dir.filename), FileName.GetBuffer( 16 ), 16 );
		for ( i = 0; i < 17; i ++ )
		{
			if ( '\0' == dir.filename[ i ] )
			{
				dir.filename[ i ] = '\xD';
				break;
			}
		}
		dir.attr = Attr;
		if(dir.mode == 4)
		{
			dir.size = FileSize / 32;
		}
		else
		{
			dir.size = FileSize;
		}
		dir.loadAdr = LoadAdr;
		dir.runAdr = RunAdr;
		dir.date = 0;
		int result;
		if ( 0 == FileType )
		{
			result = MzDiskClass->PutFile( DataPath.GetBuffer( 260 ), &dir, Disk::FILEMODE_BIN, Mode );
		}
		else
		{
			result = MzDiskClass->PutFile( DataPath.GetBuffer( 260 ), &dir, Disk::FILEMODE_MZT, Mode );
		}
		if( 1 == result )
		{
			MessageBox( "�f�B�X�N�C���[�W�ւ̃t�@�C���������݂Ɏ��s���܂���. \n�f�B���N�g���ɋ󂫂�����܂���.", "�G���[", MB_OK );
		}
		else if( ( 2 == result ) || ( 3 == result ) )
		{
			MessageBox( "�f�B�X�N�C���[�W�ւ̃t�@�C���������݂Ɏ��s���܂���. \n�t�@�C����ǂݍ��ނ��Ƃ��ł��܂���.", "�G���[", MB_OK );
		}
		else if( 4 == result )
		{
			MessageBox( "�f�B�X�N�C���[�W�ւ̃t�@�C���������݂Ɏ��s���܂���. \n�r�b�g�}�b�v�̋󂫂�����܂���.", "�G���[", MB_OK );
		}
		else if( 5 == result )
		{
			MessageBox( "�f�B�X�N�C���[�W�ւ̃t�@�C���������݂Ɏ��s���܂���. \n�����t�@�C���������łɑ��݂��܂�.", "�G���[", MB_OK );
		}
		else if( 0 != result )
		{
			MessageBox( "�f�B�X�N�C���[�W�ւ̃t�@�C���������݂Ɏ��s���܂���. \n�����͕s���ł�.", "�G���[", MB_OK );
		}
	}
	CDialog::OnOK();
}
