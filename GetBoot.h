#if !defined(AFX_GETBOOT_H__EAB24344_9F91_4D62_8F1C_6DBFC4DABF1E__INCLUDED_)
#define AFX_GETBOOT_H__EAB24344_9F91_4D62_8F1C_6DBFC4DABF1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetBoot.h : �w�b�_�[ �t�@�C��
//
#include "MzDisk/Disk.hpp"

/////////////////////////////////////////////////////////////////////////////
// cGetBoot �_�C�A���O

class cGetBoot : public CDialog
{
// �R���X�g���N�V����
public:
	char FileName[ MAX_PATH + 1 ];
	Disk *MzDiskClass;
	int SaveType;
	void SetFile( char *filename );
	cGetBoot(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(cGetBoot)
	enum { IDD = IDD_GETBOOT };
	CEdit	m_Path;
	CComboBox	m_FileType;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(cGetBoot)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(cGetBoot)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeFiletype();
	afx_msg void OnRef();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_GETBOOT_H__EAB24344_9F91_4D62_8F1C_6DBFC4DABF1E__INCLUDED_)
