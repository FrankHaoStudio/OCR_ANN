
// OCR_ANNDlg.h : ͷ�ļ�
//

#pragma once
#include "resource.h"
#include "afxcmn.h"

// COCR_ANNDlg �Ի���
class COCR_ANNDlg : public CDialogEx
{
// ����
public:
	COCR_ANNDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_OCR_ANN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedConfigButton();
	CString ResultValue;
	afx_msg void OnBnClickedAnntrainButton();
	afx_msg void OnBnClickedAnntestButton();
	afx_msg void OnBnClickedCancel();
	CProgressCtrl ProgressValue;
};
