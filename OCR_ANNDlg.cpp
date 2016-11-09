
// OCR_ANNDlg.cpp : ʵ���ļ�
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


// COCR_ANNDlg �Ի���



COCR_ANNDlg::COCR_ANNDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COCR_ANNDlg::IDD, pParent)
	, ResultValue(_T("> ��Ϣ����\r\n"))
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


// COCR_ANNDlg ��Ϣ�������

BOOL COCR_ANNDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void COCR_ANNDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR COCR_ANNDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void COCR_ANNDlg::OnBnClickedConfigButton()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	ResultValue += _T("> ����ѵ������......\r\n");
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	ResultValue += _T("> ���ڲ�������......\r\n");
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}

