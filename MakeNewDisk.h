#if !defined(AFX_MAKENEWDISK_H__8C8AFFBB_A2A1_4975_BAFE_DBAD0A4354FB__INCLUDED_)
#define AFX_MAKENEWDISK_H__8C8AFFBB_A2A1_4975_BAFE_DBAD0A4354FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MakeNewDisk.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// cMakeNewDisk �_�C�A���O

class cMakeNewDisk : public CDialog
{
// �R���X�g���N�V����
public:
	int DiskType;
	int VolumeNumber;
	cMakeNewDisk(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(cMakeNewDisk)
	enum { IDD = IDD_NEWDISK };
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(cMakeNewDisk)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(cMakeNewDisk)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CComboBox	m_DiskType;
	CEdit m_VolumeNumber;
	afx_msg void OnCbnSelchangeDisktype();
	void AdjustVolumeNumber();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_MAKENEWDISK_H__8C8AFFBB_A2A1_4975_BAFE_DBAD0A4354FB__INCLUDED_)
