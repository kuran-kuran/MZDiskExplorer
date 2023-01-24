// EditFile.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZDiskExplorer.h"
#include "EditFile.h"
#include "afxdialogex.h"
#include "MzDisk/MzDisk.hpp"
#include "MzDisk/Mz80Disk.hpp"
#include "MzDisk/Mz80SP6110Disk.hpp"


// EditFile ダイアログ

IMPLEMENT_DYNAMIC(EditFile, CDialog)

EditFile::EditFile(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_EDITFILE, pParent)
,DataPath()
,FileName()
,Mode(0)
,Attr(0)
,Reserve(0)
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
,dirIndex(0)
{
}

EditFile::~EditFile()
{
}

void EditFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
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
	DDX_Control(pDX, IDC_RESERVE, m_Reserve);
}


BEGIN_MESSAGE_MAP(EditFile, CDialog)
END_MESSAGE_MAP()


// EditFile メッセージ ハンドラー


BOOL EditFile::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: ここに初期化を追加してください
	char work[17];
	if(MzDiskClass->DiskType() == Disk::MZ2000)
	{
		MzDisk::DIRECTORY dir;
		MzDiskClass->GetDir(&dir, dirIndex);
		if((0 == dir.mode) || (0x80 == dir.mode) || (0x82 == dir.mode) || (0xF == dir.mode))
		{
			return FALSE;
		}
		memcpy(work, dir.filename, 17);
		for (int j = 0; j < 17; j ++)
		{
			if(0x0D == work[j])
			{
				work[j] = '\0';
			}
		}
		FileName = work;
		Mode = dir.mode;
		if(Mode == 7)
		{
			Mode = 6;
		}
		else if(Mode == 0xA)
		{
			Mode = 7;
		}
		else if(Mode == 0xB)
		{
			Mode = 8;
		}
		Attr = dir.attr;
		Reserve = dir.reserve;
		FileSize = dir.size;
		LoadAdr = dir.loadAdr;
		RunAdr = dir.runAdr;
		Year = ( dir.date & 0xF ) + ( ( dir.date >> 4 ) & 0xF ) * 10;
		Month = ( ( dir.date >> 11 ) & 0xF ) + ( ( dir.date >> 15 ) & 0x1 ) * 10;
		Day = ( ( dir.date >> 5 ) & 0x8 ) + ( ( dir.date >> 21 ) & 0x7 ) + ( ( dir.date >> 9 ) & 0x3 ) * 10;
		Hour = ( ( dir.date >> 31 ) & 0x1 ) + ( ( dir.date >> 15 ) & 0xE ) + ( ( dir.date >> 19 ) & 0x3 ) * 10;
		Minute = ( ( dir.date >> 24 ) & 0xF ) + ( ( dir.date >> 28 ) & 7 ) * 10;
	}
	else if(MzDiskClass->DiskType() == Disk::MZ80K_SP6010)
	{
		Mz80Disk::DIRECTORY dir;
		MzDiskClass->GetDir(&dir, dirIndex);
		if((0 == dir.mode) || (0x80 == dir.mode) || (0x82 == dir.mode) || (0xF == dir.mode))
		{
			return FALSE;
		}
		memcpy(work, dir.filename, 17);
		for (int j = 0; j < 17; j ++)
		{
			if(0x0D == work[j])
			{
				work[j] = '\0';
			}
		}
		std::string filename = MzDiskClass->ConvertText(work);
		FileName = &filename[0];
		Mode = dir.mode;
		Attr = dir.attr;
		Reserve = 0;
		FileSize = dir.size;
		LoadAdr = dir.loadAdr;
		m_Reserve.EnableWindow(TRUE);
	}
	else if(MzDiskClass->DiskType() == Disk::MZ80K_SP6110)
	{
		MzDisk::DIRECTORY dir;
		MzDiskClass->GetDir(&dir, dirIndex);
		if((0 == dir.mode) || (0x80 == dir.mode) || (0x82 == dir.mode) || (0xF == dir.mode))
		{
			return FALSE;
		}
		memcpy(work, dir.filename, 17);
		for (int j = 0; j < 17; j ++)
		{
			if(0x0D == work[j])
			{
				work[j] = '\0';
			}
		}
		std::string filename = MzDiskClass->ConvertText(work);
		FileName = &filename[0];
		Mode = dir.mode;
		if(Mode == 7)
		{
			Mode = 6;
		}
		else if(Mode == 0xA)
		{
			Mode = 7;
		}
		else if(Mode == 0xB)
		{
			Mode = 8;
		}
		Attr = dir.attr;
		Reserve = 0;
		FileSize = dir.size;
		LoadAdr = dir.loadAdr;
		RunAdr = dir.runAdr;
	}
	char temp[ 20 ];
	m_FileName.SetSel( 0, -1, FALSE );
	m_FileName.Clear();
	m_FileName.ReplaceSel( FileName.GetBuffer( 260 ) );
	m_Mode.SetCurSel( Mode - 1 );
	m_Attr.SetCurSel( Attr & 1 );
	m_Reserve.SetSel( 0, -1, FALSE );
	m_Reserve.Clear();
	sprintf_s( temp, sizeof(temp), "%02X", Reserve );
	m_Reserve.ReplaceSel( temp );
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
	if(MzDiskClass->DiskType() == Disk::MZ80K_SP6010 || MzDiskClass->DiskType() == Disk::MZ80K_SP6110)
	{
		m_Reserve.EnableWindow(FALSE);
		m_Year.EnableWindow(FALSE);
		m_Month.EnableWindow(FALSE);
		m_Day.EnableWindow(FALSE);
		m_Hour.EnableWindow(FALSE);
		m_Minute.EnableWindow(FALSE);
	}
	else
	{
		m_Reserve.EnableWindow(TRUE);
		m_Year.EnableWindow(TRUE);
		m_Month.EnableWindow(TRUE);
		m_Day.EnableWindow(TRUE);
		m_Hour.EnableWindow(TRUE);
		m_Minute.EnableWindow(TRUE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void EditFile::OnOK()
{
	// TODO: ここに特定なコードを追加するか、もしくは基底クラスを呼び出してください。
	if(MzDiskClass->DiskType() == Disk::MZ2000)
	{
		MzDisk::DIRECTORY dir;
		MzDiskClass->GetDir(&dir, dirIndex);
		int i;
		char temp[ 261 ];
		char *temp2;
		int mode = 0;
		m_FileName.SetSel( 0, -1, FALSE );
		ZeroMemory( temp, sizeof( temp ) );
		int size = m_FileName.GetLine( 0, temp, 16 );
		temp[size] = '\0';
		std::string mzFileName = MzDiskClass->ConvertMzText(temp);
		FileName = &mzFileName[0];
		Mode = m_Mode.GetCurSel() + 1;
		if( 1 == m_Attr.GetCurSel() )
		{
			Attr |= 0x1;
		}
		ZeroMemory( temp, sizeof( temp ) );
		size = m_Reserve.GetLine( 0, temp, 260 );
		temp[size] = '\0';
		Reserve = (unsigned char)strtol( temp, &temp2, 16 );
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
		// 日付
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
		unsigned short startSector = dir.startSector;

		ZeroMemory( &dir, sizeof( dir ) );
		dir.mode = Mode;
		if( 6 == dir.mode )
		{
			dir.mode = 7;
		}
		else if( 7 == dir.mode )
		{
			dir.mode = 0xA;
		}
		else if( 8 == dir.mode )
		{
			dir.mode = 0xB;
		}
		strncpy_s( dir.filename, sizeof(dir.filename), FileName.GetBuffer( 16 ), 16 );
		if(MzDiskClass->FindFile(dir.filename, dirIndex) != -1)
		{
			int result = MessageBox("同じ名前のファイルが存在しますが実行しますか?", "警告", MB_ICONWARNING | MB_OKCANCEL);
			if(result == IDCANCEL)
			{
				return;
			}
		}
		for ( i = 0; i < 17; i ++ )
		{
			if ( '\0' == dir.filename[ i ] )
			{
				dir.filename[ i ] = '\xD';
				break;
			}
		}
		dir.attr = Attr;
		dir.reserve = Reserve;
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
		dir.startSector = startSector;
		MzDiskClass->SetDir(&dir, dirIndex);
	}
	else if(MzDiskClass->DiskType() == Disk::MZ80K_SP6010)
	{
		Mz80Disk::DIRECTORY dir;
		MzDiskClass->GetDir(&dir, dirIndex);
		int i;
		char temp[ 261 ];
		char *temp2;
		int mode = 0;
		m_FileName.SetSel( 0, -1, FALSE );
		ZeroMemory( temp, sizeof( temp ) );
		int size = m_FileName.GetLine( 0, temp, 16 );
		temp[size] = '\0';
		std::string mzFileName = MzDiskClass->ConvertMzText(temp);
		FileName = &mzFileName[0];
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
		unsigned char startSector = dir.startSector;
		unsigned char startTrack = dir.startTrack;

		ZeroMemory( &dir, sizeof( dir ) );
		dir.mode = Mode;
		if( dir.mode >= 2 )
		{
			dir.mode = 0;
		}
		strncpy_s( dir.filename, sizeof(dir.filename), FileName.GetBuffer( 16 ), 16 );
		if(MzDiskClass->FindFile(dir.filename, dirIndex) != -1)
		{
			int result = MessageBox("同じ名前のファイルが存在しますが実行しますか?", "警告", MB_ICONWARNING | MB_OKCANCEL);
			if(result == IDCANCEL)
			{
				return;
			}
		}
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
		dir.startSector = startSector;
		dir.startTrack = startTrack;

		MzDiskClass->SetDir(&dir, dirIndex);
	}
	else if(MzDiskClass->DiskType() == Disk::MZ80K_SP6110)
	{
		Mz80SP6110Disk::DIRECTORY dir;
		MzDiskClass->GetDir(&dir, dirIndex);
		int i;
		char temp[ 261 ];
		char *temp2;
		int mode = 0;
		m_FileName.SetSel( 0, -1, FALSE );
		ZeroMemory( temp, sizeof( temp ) );
		int size = m_FileName.GetLine( 0, temp, 16 );
		temp[size] = '\0';
		std::string mzFileName = MzDiskClass->ConvertMzText(temp);
		FileName = &mzFileName[0];
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
		// 日付
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
		unsigned short startSector = dir.startSector;

		ZeroMemory( &dir, sizeof( dir ) );
		dir.mode = Mode;
		if( 6 == dir.mode )
		{
			dir.mode = 7;
		}
		else if( 7 == dir.mode )
		{
			dir.mode = 0xA;
		}
		else if( 8 == dir.mode )
		{
			dir.mode = 0xB;
		}
		strncpy_s( dir.filename, sizeof(dir.filename), FileName.GetBuffer( 16 ), 16 );
		if(MzDiskClass->FindFile(dir.filename, dirIndex) != -1)
		{
			int result = MessageBox("同じ名前のファイルが存在しますが実行しますか?", "警告", MB_ICONWARNING | MB_OKCANCEL);
			if(result == IDCANCEL)
			{
				return;
			}
		}
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
		dir.startSector = startSector;
		MzDiskClass->SetDir(&dir, dirIndex);
	}
	CDialog::OnOK();
}
