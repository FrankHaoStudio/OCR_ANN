
// OCR_ANNDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "OCR_ANN.h"
#include "OCR_ANNDlg.h"
#include "afxdialogex.h"
#include "ConfigDialog.h"
#include "Config.h"
#include "AnnTrain.h"
#include "Feature.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COCR_ANNDlg 对话框



COCR_ANNDlg::COCR_ANNDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COCR_ANNDlg::IDD, pParent)
	, ResultValue(_T("> 信息窗口\r\n"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COCR_ANNDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RESULT_EDIT, ResultValue);
	DDX_Control(pDX, IDC_PROGRESS, ProgressValue);
}

BEGIN_MESSAGE_MAP(COCR_ANNDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONFIG_BUTTON, &COCR_ANNDlg::OnBnClickedConfigButton)
	ON_BN_CLICKED(IDC_ANNTRAIN_BUTTON, &COCR_ANNDlg::OnBnClickedAnntrainButton)
	ON_BN_CLICKED(IDC_ANNTEST_BUTTON, &COCR_ANNDlg::OnBnClickedAnntestButton)
	ON_BN_CLICKED(IDCANCEL, &COCR_ANNDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// COCR_ANNDlg 消息处理程序

BOOL COCR_ANNDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	Feature fea;
	CString HidenLayer_Cstr;
	CString Neuron_Cstr;
	CString CharType_Cstr;
	CString keyarray[7] = { _T("ANNTrainDataPath"), _T("ANNTestDataPath"), _T("ANNErrorDataPath"), _T("ANNModelDataPath"),
		                    _T("HidenLayerNumber"), _T("HidenLayerNeuronNumber"), _T("CharsType") };
	CString valuearray[7] = { TrainDataFolder, TestDataFolder, ErrorDataFolder, ModelDataFolder, HidenLayer_Cstr, Neuron_Cstr, CharType_Cstr };
	CString devaluearray[7] = { _T("Data/AnnTrain"), _T("Data/AnnTest"), _T("Data/AnnError"), _T("Model/ann.xml"), _T("0"), _T("40"), _T("0") };
	for (int i = 0; i < 7; ++i)
	{
		valuearray[i] = LoadConfig(keyarray[i], valuearray[i], devaluearray[i]);
	}
	TrainDataFolder = valuearray[0];
	TestDataFolder = valuearray[1];
	ErrorDataFolder = valuearray[2];
	ModelDataFolder = valuearray[3];
	HidenLayerNumber = fea.CStringToInt(valuearray[4]);
	NeuronsNumber = fea.CStringToInt(valuearray[5]);
	CharsType = fea.CStringToInt(valuearray[6]);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void COCR_ANNDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR COCR_ANNDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void COCR_ANNDlg::OnBnClickedConfigButton()
{
	// TODO:  在此添加控件通知处理程序代码
	INT_PTR ConfigResult;   
	ConfigDialog configDialog;   
	ConfigResult = configDialog.DoModal();
	if (IDCANCEL == ConfigResult)
		return;
}

UINT ProgressControl(LPVOID pParam)
{
	CProgressCtrl *Speed = (CProgressCtrl*)pParam;

	Speed->SetRange(0, 1000);   
	for (int i = 0; i < 1000; i++)
	{
		Speed->SetPos(i);   
		Sleep(10);
	}
	return 0;
}

UINT CallAnnTrain(LPVOID PointParam)
{
	//DEBUG
	int a = HidenLayerNumber;
	int b = NeuronsNumber;
	int c = CharsType;
	//DEBUG
	Feature fea;
	const char* pramgma_a = fea.CStringToCharpoint(TrainDataFolder);
	const char* pramgma_b = fea.CStringToCharpoint(TestDataFolder);
	const char* pramgma_c = fea.CStringToCharpoint(ErrorDataFolder);
	const char* pramgma_d = fea.CStringToCharpoint(ModelDataFolder);
	AnnTrain Ann(pramgma_a, pramgma_b, pramgma_c, pramgma_d);
	Ann.Train(HidenLayerNumber, NeuronsNumber, CharsType);
	return 0;
}

UINT CallAnnTest(LPVOID PointParam)
{

	Feature fea;
	const char* pramgma_a = fea.CStringToCharpoint(TrainDataFolder);
	const char* pramgma_b = fea.CStringToCharpoint(TestDataFolder);
	const char* pramgma_c = fea.CStringToCharpoint(ErrorDataFolder);
	const char* pramgma_d = fea.CStringToCharpoint(ModelDataFolder);
	AnnTrain Ann(pramgma_a, pramgma_b, pramgma_c, pramgma_d);
	Ann.Test(CharsType);
	return 0;
}

void COCR_ANNDlg::OnBnClickedAnntrainButton()
{
	// TODO:  在此添加控件通知处理程序代码
	ResultValue += _T("> 正在训练数据......\r\n");
	UpdateData(FALSE);
	//CButton *testbtn = (CButton*)GetDlgItem(IDC_ANNTEST_BUTTON);
	//CButton *exitbtn = (CButton*)GetDlgItem(IDCANCEL);
	//testbtn->EnableWindow(FALSE);
	//exitbtn->EnableWindow(FALSE);
	AfxBeginThread(CallAnnTrain, NULL);
	//AfxBeginThread(ProgressControl,&ProgressValue);
}

void COCR_ANNDlg::OnBnClickedAnntestButton()
{
	// TODO:  在此添加控件通知处理程序代码
	ResultValue += _T("> 正在测试数据......\r\n");
	UpdateData(FALSE);
	//CButton *trainbtn = (CButton*)GetDlgItem(IDC_ANNTRAIN_BUTTON);
	//CButton *exitbtn = (CButton*)GetDlgItem(IDCANCEL);
	//trainbtn->EnableWindow(FALSE);
	//exitbtn->EnableWindow(FALSE);
	AfxBeginThread(CallAnnTest, NULL);
	//AfxBeginThread(ProgressControl, &ProgressValue);
}

void COCR_ANNDlg::OnBnClickedCancel()
{
	// TODO:  在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}

