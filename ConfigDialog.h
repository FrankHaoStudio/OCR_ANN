#pragma once
#include "afxwin.h"


// ConfigDialog 对话框

class ConfigDialog : public CDialogEx
{
	DECLARE_DYNAMIC(ConfigDialog)

public:
	ConfigDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ConfigDialog();

// 对话框数据
	enum { IDD = IDD_CONFIG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	CString TestFolderValue;
	CString TrainFolderValue;
	CString ErrorFolderValue;
	afx_msg CString ConfigDialog::SelectFolder(CString parameter);
	afx_msg void OnBnClickedOkconfigButton();
	afx_msg void OnBnClickedTrainfolderButton();
	CString ModelFolderValue;
	int NeuronNumberValue;
	afx_msg void OnBnClickedTestfolderButton();
	afx_msg void OnBnClickedErrorfolderButton();
	afx_msg void OnBnClickedModelfolderButton();
	int HidenLayerSelectValue;
	int CharTypeValue;
	afx_msg void OnSelchangeHidenlayernumCombo();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};
