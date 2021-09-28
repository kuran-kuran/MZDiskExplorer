#if !defined(AFX_PUTBOOT_H__B061910F_E971_428A_B1A1_E0C3C8627B0A__INCLUDED_)
#define AFX_PUTBOOT_H__B061910F_E971_428A_B1A1_E0C3C8627B0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PutBoot.h : �w�b�_�[ �t�@�C��
//
#include "mzdisk.h"

/////////////////////////////////////////////////////////////////////////////
// cPutBoot �_�C�A���O

class cPutBoot : public CDialog
{
// �R���X�g���N�V����
public:
	unsigned short RunAdr;
	int Machine;
	int FileType;
	CString BootName;
	CString DataPath;
	cMzDisk *MzDiskClass;
	cPutBoot(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(cPutBoot)
	enum { IDD = IDD_PUTBOOT };
	CEdit	m_RunAdr;
	CComboBox	m_Machine;
	CEdit	m_BootName;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(cPutBoot)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(cPutBoot)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_PUTBOOT_H__B061910F_E971_428A_B1A1_E0C3C8627B0A__INCLUDED_)
