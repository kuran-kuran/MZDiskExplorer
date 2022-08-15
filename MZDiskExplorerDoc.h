// MZDiskExplorerDoc.h : CMZDiskExplorerDoc �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂��B
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MZDISKEXPLORERDOC_H__31A6F4BB_C60D_4B88_AE4E_72D55E7A6A9E__INCLUDED_)
#define AFX_MZDISKEXPLORERDOC_H__31A6F4BB_C60D_4B88_AE4E_72D55E7A6A9E__INCLUDED_

#include "LeftView.h"	// ClassView �ɂ���Ēǉ�����܂����B
#include "MzDisk/Disk.hpp"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DIRHANDLE_MAX 1000

class CMZDiskExplorerDoc : public CDocument
{
protected: // �V���A���C�Y�@�\�݂̂���쐬���܂��B
	CMZDiskExplorerDoc();
	DECLARE_DYNCREATE(CMZDiskExplorerDoc)

// �A�g���r���[�g
public:

// �I�y���[�V����
public:

//�I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CMZDiskExplorerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
	void GetBootExec( void );
	int ImageInit;
	int ItemToDirIndex[ 64 ];
	HTREEITEM DirHandle[ DIRHANDLE_MAX ];
	int DirSector[ DIRHANDLE_MAX ];
	int DirHandleCount;
	Disk* MzDiskClass;
	int SaveType;
	int Machine;
	CString FilePath;
	virtual ~CMZDiskExplorerDoc();
	int MakeFileList( int dirsector );
	int MakeTree( int dirsector, HTREEITEM parenthandle );
	void OnEditPutfile(CString datapath);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
	//{{AFX_MSG(CMZDiskExplorerDoc)
	afx_msg void OnUpdateEditGetfile(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditGetboot(CCmdUI* pCmdUI);
	afx_msg void OnEditGetboot();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPutfile(CCmdUI* pCmdUI);
	afx_msg void OnEditPutfile();
	afx_msg void OnUpdateEditPutboot(CCmdUI* pCmdUI);
	afx_msg void OnEditPutboot();
	afx_msg void OnUpdateEditDel(CCmdUI* pCmdUI);
	afx_msg void OnEditDel();
	afx_msg void OnUpdateFileExportBeta(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileImportBeta(CCmdUI* pCmdUI);
	afx_msg void OnFileExportBeta();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void Update();
	int FirstInit;
public:
	afx_msg void OnFileImportBeta();
	afx_msg void OnEditGetsystem();
	afx_msg void OnUpdateEditGetsystem(CCmdUI* pCmdUI);
	afx_msg void OnEditPutsystem();
	afx_msg void OnUpdateEditPutsystem(CCmdUI* pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_MZDISKEXPLORERDOC_H__31A6F4BB_C60D_4B88_AE4E_72D55E7A6A9E__INCLUDED_)
