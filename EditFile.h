#pragma once


// EditFile ダイアログ
#include "MzDisk/Disk.hpp"

class EditFile : public CDialog
{
	DECLARE_DYNAMIC(EditFile)

public:
	CString DataPath;
	CString FileName;
	unsigned char Mode;
	unsigned char Attr;
	unsigned char Reserve;
	int FileSize;
	unsigned short LoadAdr;
	unsigned short RunAdr;
	//	unsigned int Date;
	int Year;
	int Month;
	int Day;
	int Hour;
	int Minute;
	int FileType;
	Disk *MzDiskClass;
	int dirIndex;
	EditFile(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~EditFile();

	// ダイアログ データ
	enum { IDD = IDD_EDITFILE };
	CEdit	m_Hour;
	CEdit	m_Month;
	CEdit	m_Minute;
	CEdit	m_Day;
	CEdit	m_Year;
	CEdit	m_RunAdr;
	CComboBox	m_Mode;
	CEdit	m_LoadAdr;
	CEdit	m_FileSize;
	CEdit	m_FileName;
	CComboBox	m_Attr;
	CEdit	m_Reserve;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
