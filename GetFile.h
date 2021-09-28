#if !defined(AFX_CGETFILE_H__91CAB395_A30D_4022_A956_B2FCA9731DF1__INCLUDED_)
#define AFX_CGETFILE_H__91CAB395_A30D_4022_A956_B2FCA9731DF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetFile.h : �w�b�_�[ �t�@�C��
//
#include "mzdisk.h"

/////////////////////////////////////////////////////////////////////////////
// cGetFile �_�C�A���O

class cGetFile : public CDialog
{
// �R���X�g���N�V����
public:
	int AllOk;
	char ExtName[ 260 ];
	int DirIndex;
	char FileName[ MAX_PATH ];
	void SetFile( char *filename );
	cGetFile(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^
	cMzDisk *MzDiskClass;
	int SaveType;

// �_�C�A���O �f�[�^
	//{{AFX_DATA(cGetFile)
	enum { IDD = IDD_GETFILE };
	CEdit	m_Path;
	CComboBox	m_FileType;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(cGetFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(cGetFile)
	virtual BOOL OnInitDialog();
	afx_msg void OnRef();
	afx_msg void OnSelchangeFiletype();
	virtual void OnOK();
	afx_msg void OnAllok();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_CGETFILE_H__91CAB395_A30D_4022_A956_B2FCA9731DF1__INCLUDED_)
