// MZDiskExplorer.h : MZDISKEXPLORER �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C��
//

#if !defined(AFX_MZDISKEXPLORER_H__6E689B9C_9CBE_4537_A92E_57A3ACA69322__INCLUDED_)
#define AFX_MZDISKEXPLORER_H__6E689B9C_9CBE_4537_A92E_57A3ACA69322__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // ���C�� �V���{��

/////////////////////////////////////////////////////////////////////////////
// CMZDiskExplorerApp:
// ���̃N���X�̓���̒�`�Ɋւ��Ă� MZDiskExplorer.cpp �t�@�C�����Q�Ƃ��Ă��������B
//

class CMZDiskExplorerApp : public CWinApp
{
public:
	CMZDiskExplorerApp();

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CMZDiskExplorerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
	//{{AFX_MSG(CMZDiskExplorerApp)
	afx_msg void OnAppAbout();
	afx_msg void OnEditGetboot();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_MZDISKEXPLORER_H__6E689B9C_9CBE_4537_A92E_57A3ACA69322__INCLUDED_)
