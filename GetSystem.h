#pragma once
#include "MzDisk/Disk.hpp"

// GetSystem ダイアログ

class GetSystem : public CDialog
{
	DECLARE_DYNAMIC(GetSystem)

public:
	GetSystem(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~GetSystem();
	void SetFile( char *filename );
	char FileName[ MAX_PATH + 1 ];
	Disk *MzDiskClass;
	int SaveType;

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GETSYSTEM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRef();
private:
	CEdit m_Path;
	CComboBox m_FileType;
public:
	afx_msg void OnCbnSelchangeFiletype();
};
