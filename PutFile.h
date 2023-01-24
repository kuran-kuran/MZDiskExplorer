#if !defined(AFX_PUTFILE_H__64FC9668_733B_4BE3_845B_C2127A9BB045__INCLUDED_)
#define AFX_PUTFILE_H__64FC9668_733B_4BE3_845B_C2127A9BB045__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PutFile.h : �w�b�_�[ �t�@�C��
//
#include "MzDisk/Disk.hpp"

/////////////////////////////////////////////////////////////////////////////
// cPutFile �_�C�A���O

class cPutFile : public CDialog
{
// �R���X�g���N�V����
public:
	CString DataPath;
	CString FileName;
	std::vector<unsigned char> mztFilename;
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
	bool Size64KBObj;
	Disk *MzDiskClass;
	cPutFile(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(cPutFile)
	enum { IDD = IDD_PUTFILE };
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
	CButton m_IsMztFilename;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(cPutFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(cPutFile)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedMztFilename();
	afx_msg void OnSelchangeMode();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_PUTFILE_H__64FC9668_733B_4BE3_845B_C2127A9BB045__INCLUDED_)
