
// LPRVideoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LPRVideo.h"
#include "LPRVideoDlg.h"
#include "LPHelper.h"
#include "LPLocation.h"
#include "CharSegmentation.h"
#include "CharRecognition.h"

#include "Windows.h"
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <direct.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL g_bIsAutoCap = TRUE;   //�Զ���׽����
BOOL g_bIsThreadOn = FALSE;  
BOOL g_bIsOverThread = FALSE;//�Ƿ���Ҫ������ǰ�߳�
IplImage* g_pFrame = NULL;  //��Ƶ֡
ThreadPara g_threaPara ;  //�̲߳���
CMutex g_mutexImage;   //ͼ��ʹ�û�����
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���


//���ƶ�λ��ʶ���߳�
UINT LPProcThread(LPVOID pParam)
{
	PLPThreadPara lpThread = (PLPThreadPara)pParam;

	lpThread->pDlg->BeginReco(lpThread->img);

	cvReleaseImage(&(lpThread->img));
	LPRFree(lpThread);
	return 0;
}
//��Ƶ�������ٵ��߳�
UINT VideoProcThread(LPVOID pParam)
{
	PThreadPara tp = (PThreadPara)pParam;
	int nFrmNum = 0;               
	CvCapture* pCapture = NULL;
	if (!(pCapture = cvCreateFileCapture(tp->strVideoPath)))
	{
		AfxMessageBox("��Ƶ��ʧ��");
		return 0;
	}
	//g_mutexImage.Lock();
	g_bIsThreadOn = TRUE;
	while((g_pFrame = cvQueryFrame(pCapture)) != NULL
					&& !g_bIsOverThread)
	{
		//g_mutexImage.Unlock();

		nFrmNum++;	
		CString str;
		str.Format("%d",nFrmNum);
		tp->pDlg->SetDlgItemText(IDC_STATIC_TMP,str);
		//if(nFrmNum == 180)
		//	cvSaveImage("d:\\song.jpg",g_pFrame);
	
		tp->pDlg->ShowVideoFrame(g_pFrame);
		if(g_bIsAutoCap)
		{
			if(nFrmNum % 3 == 0)
			{				
				PLPThreadPara plpt = (PLPThreadPara)malloc(sizeof(LPThreadPara));
				plpt->pDlg = tp->pDlg;
				plpt->img = cvCloneImage(g_pFrame);
				AfxBeginThread(LPProcThread, plpt, 
					THREAD_PRIORITY_NORMAL, 0, 0, NULL);

			}
		}
	//	g_mutexImage.Lock();
	}
	
	//g_mutexImage.Unlock();
	g_bIsThreadOn = FALSE;
	tp->pDlg->Invalidate();
	//�ͷ�ͼ��;���		
	cvReleaseCapture(&pCapture);
	
	return 0;
}

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CLPRVideoDlg �Ի���




CLPRVideoDlg::CLPRVideoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLPRVideoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	char dir[1024];
	_getcwd(dir,1024);  //��ȡ��������Ŀ¼

	char *modelfile="\\Models\\";
	int nember=11;
	char q3[5]="m";
	char q5[5]="\\";
	char q4[20];
	int t=1;
	for(int i=1;i<7;i++)
	{
		char n[1024];
		char Dri[1024];
		strcpy(Dri,dir);
		strcpy(n,modelfile);
		if(i==6)
			nember++;
		itoa(i,q4,10);
		strcat(n,q3);
		strcat(n,q4);
		strcat(n,q5);
		strcat(Dri,n);
		if(i!=1)
		{
			t=t+10;
			nember+=10;
		}
		load_svm_model(Dri,nember,t);
	}

	m_pThread = NULL;

}
CLPRVideoDlg::~CLPRVideoDlg()
{
	g_bIsOverThread = TRUE;
	if(g_bIsThreadOn)
		WaitForSingleObject(m_pThread->m_hThread,500);
	Sleep(200);
	//�ͷ�model�ļ�
	free_model();
}
void CLPRVideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LPN, m_editResult);
}

BEGIN_MESSAGE_MAP(CLPRVideoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_OPEN_IMAGE, &CLPRVideoDlg::OnOpenImage)
	ON_BN_CLICKED(IDC_BTN_CAP_LP, &CLPRVideoDlg::OnBnClickedBtnCapLp)
	ON_BN_CLICKED(IDC_BTN_STOP, &CLPRVideoDlg::OnBnClickedBtnStop)
	ON_COMMAND(ID_OPEN_VEDIO, &CLPRVideoDlg::OnOpenVedio)
	ON_BN_CLICKED(IDC_BTN_PAUSE, &CLPRVideoDlg::OnBnClickedBtnPause)
	ON_STN_CLICKED(IDC_STATIC_VEDIO, &CLPRVideoDlg::OnStnClickedStaticVedio)
END_MESSAGE_MAP()


// CLPRVideoDlg ��Ϣ�������

BOOL CLPRVideoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_editFont.CreatePointFont(150,"����");
	m_editResult.SetFont(&m_editFont);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CLPRVideoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLPRVideoDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CLPRVideoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLPRVideoDlg::DrawPicToHDC(IplImage *img, UINT ID)
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	if(img != NULL)
	{
		cimg.CopyOf(img,3);
		cimg.DrawToHDC(hDC,&rect);
	}
	
	ReleaseDC(pDC);
}

void CLPRVideoDlg::ShowVideoFrame(const IplImage* frame)
{
	IplImage* img = cvCloneImage(frame);
	DrawPicToHDC(img,IDC_STATIC_VEDIO);
	cvReleaseImage(&img);
}
//�ַ�ʶ��
CString CLPRVideoDlg::CharReco(IplImage** charImg)
{
	char *ch1=CharacterRecognition(charImg[0],CHINESE_RECO);
	char *ch2=CharacterRecognition(charImg[1],LETTER_RECO);
	char *ch3=CharacterRecognition(charImg[2],LETTER_DIGIT_RECO);
	char *ch4=CharacterRecognition(charImg[3],LETTER_DIGIT_RECO);
	char *ch5=CharacterRecognition(charImg[4],LETTER_DIGIT_RECO);
	char *ch6=CharacterRecognition(charImg[5],LETTER_DIGIT_RECO);
	char *ch7=CharacterRecognition(charImg[6],LETTER_DIGIT_RECO);

	CString strChar="";
	strChar.Format("%s",ch1);
	strChar.AppendChar(*ch2);
	strChar.AppendChar(*ch3);
	strChar.AppendChar(*ch4);
	strChar.AppendChar(*ch5);
	strChar.AppendChar(*ch6);
	strChar.AppendChar(*ch7);
	free(ch1);
	ch1 = NULL;
	free(ch2);
	ch2 = NULL;
	free(ch3);
	ch3 = NULL;
	free(ch4);
	ch4 = NULL;
	free(ch5);
	ch5 = NULL;
	free(ch6);
	ch6 = NULL;
	free(ch7);
	ch7 = NULL;
	return strChar;
}
int CLPRVideoDlg::BeginReco(const IplImage* src)
{	
	//���ƶ�λ
	CvRect rectLPR;
	int color;  //������ɫ
	IplImage* pImgLpr = NULL;
	IplImage** pImgChar = NULL;
	int charNum= 0;
	CString strResult = "";
	if(src == NULL) return 0;
	pImgLpr = LPLocate(src,&rectLPR,&color);

	//�ַ��ָ�
	if(pImgLpr != NULL)
		pImgChar = CharacterSegmentation(pImgLpr,&charNum,color);
	

	if (charNum > CHARACTER_NUM)
	{
	//	MessageBox("�������ָ����");

	}
	else if (charNum < CHARACTER_NUM)
	{
	//	MessageBox("�������ָ����");
		//GetDlgItem(IDC_RESULT)->SetWindowText("�޷�ʶ��!"); 

	}
	else
	{		
		strResult = CharReco(pImgChar);
		
		//��ʾʶ��Ľ��
		if(color == COLOR_BLUE)
			strResult += "��";
		else if(color == COLOR_YELLOW)
			strResult += "��";
	}
	if(charNum >= 5)
	{
		GetDlgItem(IDC_EDIT_LPN)->SetWindowText(strResult);
		DrawPicToHDC(pImgLpr,IDC_STATIC_LP);
	}	

	cvReleaseImage(&pImgLpr);
	ReleaseImages(pImgChar,charNum);
	LPRFree(pImgChar);

	return charNum;
	
}

void CLPRVideoDlg::OnOpenImage()
{
	// TODO: �ڴ���������������
	CString filePath;
	IplImage* src = NULL;
	CFileDialog dlg(TRUE, _T("*.bmp"),"",OFN_FILEMUSTEXIST|
			OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
			"image files (*.bmp; *.jpg) |*.bmp;*.jpg|All Files (*.*)|*.*||",
				NULL);
	char title[]= {"Open Image"};
	dlg.m_ofn.lpstrTitle= title;
	if(dlg.DoModal() == IDOK)
	{
		filePath= dlg.GetPathName();
		//m_strImgName = dlg.GetFileName();
		if((src = cvLoadImage(filePath)) == NULL)
		{
			MessageBox("����ͼ��ʧ��");
			return;
		}

		BeginReco(src);
		DrawPicToHDC(src,IDC_STATIC_VEDIO);
		cvReleaseImage(&src);
	}
}
void CLPRVideoDlg::CaptureLP()
{
	if(NULL == g_pFrame)
		return;
	IplImage* img = NULL;
	g_mutexImage.Lock(200);
	img = cvCloneImage(g_pFrame);
	g_mutexImage.Unlock();

	BeginReco(img);

	cvReleaseImage(&img);
}
void CLPRVideoDlg::OnBnClickedBtnCapLp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(g_bIsAutoCap)
	{
		SetDlgItemText(IDC_BTN_CAP_LP,"�Զ���׽");
		g_bIsAutoCap = FALSE;
	}
	else
	{
		SetDlgItemText(IDC_BTN_CAP_LP,"�ֶ���׽");
		g_bIsAutoCap = TRUE;
	}
	
}

void CLPRVideoDlg::OnBnClickedBtnStop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g_bIsOverThread = TRUE;
}

void CLPRVideoDlg::OnOpenVedio()
{
	// TODO: �ڴ���������������
	CFileDialog dlg(TRUE, _T("*.avi"),"",OFN_FILEMUSTEXIST|
		OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
		"Video files (*.avi; *.mp4) |*.avi;*.mp4|All Files (*.*)|*.*||",
		NULL);
	char title[]= {"Open Video"};
	dlg.m_ofn.lpstrTitle= title;
	if(dlg.DoModal() == IDOK)
	{		
		g_threaPara.pDlg = this;
		g_threaPara.strVideoPath = dlg.GetPathName();
	
		g_bIsOverThread = TRUE;
		//������߳�����ִ����ֹͣ
		if(g_bIsThreadOn)
			WaitForSingleObject(m_pThread->m_hThread,-1);
		
		m_pThread = AfxBeginThread(VideoProcThread, &g_threaPara, 
			THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		//CloseHandle(m_pThread->m_hThread);
		g_bIsOverThread = FALSE;
	}
	
}

void CLPRVideoDlg::OnBnClickedBtnPause()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	if(g_bIsThreadOn == FALSE) return;
	GetDlgItemText(IDC_BTN_PAUSE,str);
	if(str == "��ͣ")
	{
		m_pThread->SuspendThread();
		SetDlgItemText(IDC_BTN_PAUSE,"����");
	}
	else
	{
		m_pThread->ResumeThread();
		SetDlgItemText(IDC_BTN_PAUSE,"��ͣ");
	}
}

void CLPRVideoDlg::OnStnClickedStaticVedio()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnBnClickedBtnCapLp();
}
