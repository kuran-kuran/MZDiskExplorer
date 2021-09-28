// MZDiskExplorerView.h : CMZDiskExplorerView �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂��B
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MZDISKEXPLORERVIEW_H__D909D4C6_A4B6_43B7_8CBA_2797768673D5__INCLUDED_)
#define AFX_MZDISKEXPLORERVIEW_H__D909D4C6_A4B6_43B7_8CBA_2797768673D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CMZDiskExplorerView : public CListView
{
protected: // �V���A���C�Y�@�\�݂̂���쐬���܂��B
	CMZDiskExplorerView();
	DECLARE_DYNCREATE(CMZDiskExplorerView)

// �A�g���r���[�g
public:
	CMZDiskExplorerDoc* GetDocument();

// �I�y���[�V����
public:

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CMZDiskExplorerView)
	public:
	virtual void OnDraw(CDC* pDC);  // ���̃r���[��`�悷��ۂɃI�[�o�[���C�h����܂��B
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // �\�z��̍ŏ��̂P�x�����Ăяo����܂��B
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
	virtual ~CMZDiskExplorerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
	//{{AFX_MSG(CMZDiskExplorerView)
	afx_msg void OnEditGetfile();
	afx_msg void OnEditGetboot();
	//}}AFX_MSG
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
private:
	int ColumnFlag;
};

#ifndef _DEBUG  // MZDiskExplorerView.cpp �t�@�C�����f�o�b�O���̎��g�p����܂��B
inline CMZDiskExplorerDoc* CMZDiskExplorerView::GetDocument()
   { return (CMZDiskExplorerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_MZDISKEXPLORERVIEW_H__D909D4C6_A4B6_43B7_8CBA_2797768673D5__INCLUDED_)
