// MainFrm.h : CMainFrame �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂��B
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__676DBE57_051D_47AA_B2C3_EA0DF56CE810__INCLUDED_)
#define AFX_MAINFRM_H__676DBE57_051D_47AA_B2C3_EA0DF56CE810__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMZDiskExplorerView;

class CMainFrame : public CFrameWnd
{
	
protected: // �V���A���C�Y�@�\�݂̂���쐬���܂��B
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// �A�g���r���[�g
protected:
	CSplitterWnd m_wndSplitter;
public:

// �I�y���[�V����
public:

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
	void PutStatusBarSize( LPCTSTR str );
	virtual ~CMainFrame();
	CMZDiskExplorerView* GetRightPane();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


protected:  // �R���g���[�� �o�[�p�����o
	CToolBar    m_wndToolBar;
	CStatusBar  m_wndStatusBar;

// �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		// ���� - ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂��B
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG
	afx_msg void OnUpdateViewStyles(CCmdUI* pCmdUI);
	afx_msg void OnViewStyle(UINT nCommandID);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_MAINFRM_H__676DBE57_051D_47AA_B2C3_EA0DF56CE810__INCLUDED_)
