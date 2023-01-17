#pragma once
#include "MzDisk/Disk.hpp"

// ChangeType ダイアログ

class ChangeType : public CDialog
{
	DECLARE_DYNAMIC(ChangeType)

public:
	int DiskType; // 0: 2D. 1: 2DD
	Disk *MzDiskClass;
	ChangeType(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~ChangeType();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHANGETYPE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
private:
	CComboBox m_DiskType;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
