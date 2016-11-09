// ConfigDialog.cpp : 实现文件
//
#pragma once
#include "stdafx.h"
#include "OCR_ANN.h"
#include "ConfigDialog.h"
#include "afxdialogex.h"
#include "config.h"
#include "Feature.h"


// ConfigDialog 对话框

IMPLEMENT_DYNAMIC(ConfigDialog, CDialogEx)

ConfigDialog::ConfigDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(ConfigDialog::IDD, pParent)
	, TrainFolderValue(_T(""))
	, TestFolderValue(_T(""))
	, ErrorFolderValue(_T(""))
	, ModelFolderValue(_T(""))
	, NeuronNumberValue(40)
	, HidenLayerSelectValue(0)
	, CharTypeValue(0)
{
}

ConfigDialog::~ConfigDialog()
{
}

void ConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ANNTESTFOLDER_EDIT, TestFolderValue);
	DDX_Text(pDX, IDC_ANNTRAINFOLDER_EDIT, TrainFolderValue);
	DDX_Text(pDX, IDC_ERRORDATAFOLDER_EDIT, ErrorFolderValue);
	DDX_Text(pDX, IDC_MODELFOLDER_EDIT, ModelFolderValue);
	DDX_Text(pDX, IDC_HIDENLAYERNEURONNUM_EDIT, NeuronNumberValue);
	DDX_CBIndex(pDX, IDC_HIDENLAYERNUM_COMBO, HidenLayerSelectValue);
	DDV_MinMaxInt(pDX, HidenLayerSelectValue, 0, 2);
	DDX_CBIndex(pDX, IDC_CHARTYPE_COMBO, CharTypeValue);
	DDV_MinMaxInt(pDX, CharTypeValue, 0, 2);
}


BEGIN_MESSAGE_MAP(ConfigDialog, CDialogEx)
	ON_BN_CLICKED(IDC_OKCONFIG_BUTTON, &ConfigDialog::OnBnClickedOkconfigButton)
	ON_BN_CLICKED(IDC_TRAINFOLDER_BUTTON, &ConfigDialog::OnBnClickedTrainfolderButton)
	ON_BN_CLICKED(IDC_TESTFOLDER_BUTTON, &ConfigDialog::OnBnClickedTestfolderButton)
	ON_BN_CLICKED(IDC_ERRORFOLDER_BUTTON, &ConfigDialog::OnBnClickedErrorfolderButton)
	ON_BN_CLICKED(IDC_MODELFOLDER_BUTTON, &ConfigDialog::OnBnClickedModelfolderButton)
	ON_CBN_SELCHANGE(IDC_HIDENLAYERNUM_COMBO, &ConfigDialog::OnSelchangeHidenlayernumCombo)
	ON_BN_CLICKED(IDCANCEL, &ConfigDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// ConfigDialog 消息处理程序

CString ConfigDialog::SelectFolder(CString parameter)
{
	HWND hwnd = GetSafeHwnd();
	LPMALLOC pMalloc;
	if (::SHGetMalloc(&pMalloc) == NOERROR)
	{
		BROWSEINFO BI;
		TCHAR pszBuffer[MAX_PATH];
		LPITEMIDLIST PIDL;
		BI.hwndOwner = hwnd;
		BI.pidlRoot = NULL;
		BI.pszDisplayName = pszBuffer;
		BI.lpszTitle = parameter;
		BI.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS;
		BI.lpfn = NULL;
		BI.lParam = 0;
		BI.iImage = 0;
		if ((PIDL = ::SHBrowseForFolder(&BI)) != NULL)
		{
			if (::SHGetPathFromIDList(PIDL, pszBuffer))
			{
				 return pszBuffer;
			}
			pMalloc->Free(PIDL);
		}
		pMalloc->Release();
	}
	else
	{
		return _T("");
	}
}

void ConfigDialog::OnBnClickedOkconfigButton()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	Feature fea;
	bool a = SetANNTrainDataFolder(TrainFolderValue);
	bool b = SetANNTestDataFolder(TestFolderValue);
	bool c = SetANNErrorDataFolder(ErrorFolderValue);
	bool d = SetANNModelDataFolder(ModelFolderValue);
	bool e = SetCharsType(CharTypeValue);
	bool f = SetHidenLayerNumber(HidenLayerSelectValue);
	bool g = SetNeuronsNumber(NeuronNumberValue);
	if (IsSaveConfig(a, b, c, d, e, f, g))
	{
		MessageBox(_T("保存成功!"), _T("提示"), MB_OK | MB_ICONINFORMATION);
	}
	CString HidenLayer_Cstr = fea.IntToCString(HidenLayerSelectValue);
	CString Neuron_Cstr = fea.IntToCString(NeuronNumberValue);
	CString CharType_Cstr = fea.IntToCString(CharTypeValue);
	CString keyarray[7] = { _T("ANNTrainDataPath"), _T("ANNTestDataPath"), _T("ANNErrorDataPath"), _T("ANNModelDataPath"),
		                    _T("HidenLayerNumber"), _T("HidenLayerNeuronNumber"), _T("CharsType") };
	CString valuearray[7] = { TrainFolderValue, TestFolderValue, ErrorFolderValue, ModelFolderValue, HidenLayer_Cstr, Neuron_Cstr, CharType_Cstr };
	for (int i = 0; i < 7; ++i)
	{
		SaveConfig(keyarray[i], valuearray[i]);
	}

}


void ConfigDialog::OnBnClickedTrainfolderButton()
{
	// TODO:  在此添加控件通知处理程序代码
	TrainFolderValue=SelectFolder(_T("选择字符训练集路径"));
	UpdateData(FALSE);
}


void ConfigDialog::OnBnClickedTestfolderButton()
{
	// TODO:  在此添加控件通知处理程序代码
	TestFolderValue=SelectFolder(_T("选择字符测试集路径"));
	UpdateData(FALSE);
}


void ConfigDialog::OnBnClickedErrorfolderButton()
{
	// TODO:  在此添加控件通知处理程序代码
	ErrorFolderValue=SelectFolder(_T("选择错误字符集路径"));
	UpdateData(FALSE);
}


void ConfigDialog::OnBnClickedModelfolderButton()
{
	// TODO:  在此添加控件通知处理程序代码
	ModelFolderValue=SelectFolder(_T("选择ANN训练模型路径"));
	UpdateData(FALSE);
}


void ConfigDialog::OnSelchangeHidenlayernumCombo()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CEdit *edit = (CEdit*)GetDlgItem(IDC_HIDENLAYERNEURONNUM_EDIT);
	CStatic *sta = (CStatic*)GetDlgItem(IDC_HIDENLAYERNEURONNUM_STATIC);
	if (HidenLayerSelectValue == 0)
	{
		edit->ShowWindow(TRUE);
		sta->ShowWindow(TRUE);
	}
	else
	{
		edit->ShowWindow(FALSE);
		sta->ShowWindow(FALSE);
	}
}


void ConfigDialog::OnBnClickedCancel()
{
	// TODO:  在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


BOOL ConfigDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	Feature fea;
	CString HidenLayer_Cstr;
	CString Neuron_Cstr;
	CString CharType_Cstr;
	CString keyarray[7] = { _T("ANNTrainDataPath"), _T("ANNTestDataPath"), _T("ANNErrorDataPath"), _T("ANNModelDataPath"),
		                    _T("HidenLayerNumber"), _T("HidenLayerNeuronNumber"), _T("CharsType") };
	CString valuearray[7] = { TrainFolderValue, TestFolderValue, ErrorFolderValue, ModelFolderValue, HidenLayer_Cstr, Neuron_Cstr, CharType_Cstr };
	CString devaluearray[7] = { _T("Data/AnnTrain"), _T("Data/AnnTest"), _T("Data/AnnError"), _T("Model/ann.xml"), _T("0"), _T("40"), _T("0") };
	for (int i = 0; i < 7; ++i)
	{
		valuearray[i] = LoadConfig(keyarray[i], valuearray[i], devaluearray[i]);
	}
	TrainFolderValue = valuearray[0];
	TestFolderValue = valuearray[1];
	ErrorFolderValue = valuearray[2];
	ModelFolderValue = valuearray[3];
	HidenLayerSelectValue = fea.CStringToInt(valuearray[4]);
	NeuronNumberValue = fea.CStringToInt(valuearray[5]);
	CharTypeValue = NeuronNumberValue = fea.CStringToInt(valuearray[6]);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
