// LPRDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LPR.h"
#include "LPRDlg.h"
#include "LPHelper.h"
#include "LPLocation.h"
#include "LPRotate.h"
#include "CharSegmentation.h"
#include "LPRotate.h"
#include "CharRecognition.h"
#include "CharModelManage.h"
//#include "CharLibDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define DEBUGTEST
#define POINT_X  18                                           // ˮƽ�����ص���                        
#define POINT_Y  2                                            // ��ֱ�����ص���                            
#define WITH_X 0.1                                            // ˮƽ��������ͼ����     
#define WITH_Y 0.2                                            // ��ֱ��������ͼ����     
#define HIGH_WITH_CAR   4.8   //(440/85)       // С���Ŀ�߱�  440/140    
#define CHARACTER 15
#define TEMPLETENUM  43

#define DEFAULT_IMAGE_WIDTH 1000
#define DEFAULT_IMAGE_HEIGHT 600



// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

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


// CLPRDlg �Ի���

CLPRDlg::CLPRDlg(CWnd* pParent /*=NULL*/)
: CDialog(CLPRDlg::IDD, pParent)

, m_DesFilePath(_T(""))
, m_filePath(_T(""))
, m_filename(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//��ʼ��  ����
	m_imgSrc = NULL;      //ԭʼͼƬ                        


	m_nCharNum = 0;
	m_pImgLPR = NULL;

}

void CLPRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RESULT, m_editResult);

}

BEGIN_MESSAGE_MAP(CLPRDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LOADIMAGE, &CLPRDlg::OnBnClickedLoadimage)
	ON_BN_CLICKED(IDC_BTN_LPR, &CLPRDlg::OnBnClickedBtnLpr)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_RESET, &CLPRDlg::OnBnClickedBtnReset)
	ON_BN_CLICKED(IDC_BTN_THRESHOLD_DES, &CLPRDlg::OnBnClickedBtnThresholdDes)
	ON_BN_CLICKED(IDC_BUTTON1, &CLPRDlg::OnBtnCharLib)
	ON_BN_CLICKED(IDC_BUTTON4, &CLPRDlg::OnBtnChLib)
	ON_BN_CLICKED(IDC_BUTTON2, &CLPRDlg::OnBtnRecogError)
	ON_BN_CLICKED(IDC_BUTTON3, &CLPRDlg::OnBtnSegmentFail)
END_MESSAGE_MAP()


// CLPRDlg ��Ϣ�������

BOOL CLPRDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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


	int num=ScanCharNumFrmFile("./Data/Standard.Model",CHAR_HEIGHT);      //����׼ģ����Ƿ����
	if (num==0)
	{
		DirImgWrite2File("F:/OpenCV/LPR_NEW1.04_TEST/LPR/LPR/��׼ģ��/","Standard.Model",cvSize(WIDTH,HEIGHT));
	}

	dataCvt2Mat32fc1();             //�˴���ģ�����ݵ��������
	// InitTemplateFeature();          //��ȡģ����ַ�����ֵ����������������



	//dataCvt2Mat32fc1();             //�˴���ģ�����ݵ��������


	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_editFont.CreatePointFont(240,"����");
	m_editResult.SetFont(&m_editFont);
	m_editResult.SetWindowText("����ʶ����");

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CLPRDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CLPRDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
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
		/*CRect rect;
		CPaintDC dc(this);
		GetClientRect(&rect);
		dc.FillSolidRect(&rect,RGB(200,200,200));*/
		ShowImage();
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CLPRDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLPRDlg::DrawPicToHDC(IplImage *img, UINT ID)
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;

	cimg.CopyOf(img,3);
	cimg.DrawToHDC(hDC,&rect);

	ReleaseDC(pDC);
}

void CLPRDlg::ShowImage()
{
	if(m_imgSrc != NULL)
		DrawPicToHDC(m_imgSrc,IDC_IMAGESRC);
	if(m_pImgLPR != NULL)
		DrawPicToHDC(m_pImgLPR,IDC_BICAR);

	for(int i=0;i<m_nCharNum;i++)
	{
		if(m_pImgChar[i] != NULL)
			DrawPicToHDC(m_pImgChar[i],IDC_ONE+i);
	}
}


void CLPRDlg::OnBnClickedLoadimage()
{// ��ͼƬ
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_imgSrc = NULL ;
	//IplImage* img = NULL;
	CString filePath;
	CFileDialog dlg(TRUE, _T("*.bmp"),"",OFN_FILEMUSTEXIST|
		OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
		"image files (*.bmp; *.jpg) |*.bmp;*.jpg|All Files (*.*)|*.*||",
		NULL);
	char title[]= {"Open Image"};
	dlg.m_ofn.lpstrTitle= title;
	if(dlg.DoModal() == IDOK)
	{
		filePath= dlg.GetPathName();

///////////////����ͳ��ʹ��//////////////////
		filePath= dlg.GetPathName();

		m_filePath=dlg.GetPathName();
		m_filename=dlg.GetFileName();
////////////////////////////////////////////


		if((m_imgSrc=cvLoadImage(filePath)) == NULL)
		{
			MessageBox("����ͼ��ʧ��");
			return;
		}

		/*if(img->height > DEFAULT_IMAGE_HEIGHT && img->width > DEFAULT_IMAGE_WIDTH)
		{
		m_imgSrc = cvCreateImage(cvSize(DEFAULT_IMAGE_WIDTH,DEFAULT_IMAGE_HEIGHT),img->depth,img->nChannels);
		cvResize(img,m_imgSrc,CV_INTER_LINEAR);
		}
		else
		{
		m_imgSrc = cvCloneImage(img);
		}*/

		//cvShowImage("img",m_imgSrc);
		DrawPicToHDC(m_imgSrc,IDC_IMAGESRC);
		//cvReleaseImage(&img);
		OnBnClickedBtnLpr();
	}


}
//�ַ�ʶ��
CString CLPRDlg::CharReco(IplImage** charImg,int charnum)
{
	/*******************ʹ�ö�ģ��****************************/
	CString s=MultiTemplateRecogn(charImg,charnum);

	/*******************ʹ������ֵ+ģ��****************************/
	// CString s=TempalteFeatureRecogn(charImg,charnum,FEATURE_BY_GRID);

	// CString s=FeatureTempalteRecogn(charImg,charnum,FEATURE_BY_GRID);


	//��CString��һ��һ����ȡ���ַ�
	CString str=s;

	m_ch[0]=str.Left(2);
	str.Delete(0,2);
	for (int i=1;i<7;i++)
	{
		m_ch[i]=str.Left(1);
		str.Delete(0,1);

	}

	return s;

}
void CLPRDlg::OnBnClickedBtnLpr()
{//����ͼ��ʶ��

	//��ȡ��ʼʱ��
	long timeSart,timeSpend=0,timeSpendLoc=0,timeSpendSeg=0,timeSpendRec=0;
	timeSart = GetTickCount(); 

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_imgSrc == NULL)
	{
		MessageBox("��ѡ��ͼ");
		return ;
	}
	//���ƶ�λ
	CvRect rectLPR;
	int color;  //������ɫ
	IplImage* img_lpr_gray = NULL;
	if(m_pImgLPR != NULL)
		cvReleaseImage(&m_pImgLPR);	

	/*IplImage* img_resize = cvCreateImage(cvSize(DEFAULT_IMAGE_WIDTH,DEFAULT_IMAGE_WIDTH*(m_imgSrc->height/m_imgSrc->width)),m_imgSrc->depth,m_imgSrc->nChannels);
	img_resize = cvResize(m_imgSrc,img_resize);*/
	img_lpr_gray = LPLocate(m_imgSrc,&rectLPR,&color);
	timeSpendLoc = GetTickCount()-timeSart;


	if(img_lpr_gray == NULL)
	{
		MessageBox("��λ����ʧ��...\n");
		return;
	}
	//��ò�ɫ����
	//m_pImgLPR = GetRectImage(m_imgSrc,rectLPR);
	m_pImgLPR = LPRotateFromRect(m_imgSrc,rectLPR,color);
	/*cvShowImage("img_lpr",m_pImgLPR);
	cvWaitKey(0);*/
	//cvSaveImage("D:\\d.jpg",m_pImgLPR);
	//cvShowImage("imgdd",m_pImgLPR);
	cvRectangle(m_imgSrc,cvPoint(rectLPR.x,rectLPR.y),cvPoint(rectLPR.x+rectLPR.width,rectLPR.y+rectLPR.height),CV_RGB(0,255,0),3);

	

	//�ַ��ָ�
	m_nCharNum = 0;
	m_pImgChar = CharacterSegmentation(m_pImgLPR,&m_nCharNum,color);
	//for(int i=0;i<m_nCharNum;i++)
	//{
	//	static int a = 0;
	//	char name[128];
	//	sprintf_s(name,128,"D:\\song\\%d.jpg",a++);
	//	cvSaveImage(name,m_pImgChar[i]);
	//	//AdjustContrast(m_pImgChar[i],m_pImgChar[i],100);
	//}
	timeSpendSeg = GetTickCount()-timeSart-timeSpendLoc;

	/*if (m_nCharNum > CHARACTER_NUM)
	{
		MessageBox("�������ָ����");
		ReleaseImages(m_pImgChar,m_nCharNum);
		m_nCharNum = 0;
	}
	else if (m_nCharNum < CHARACTER_NUM)
	{
		MessageBox("�������ָ����");
		ReleaseImages(m_pImgChar,m_nCharNum);
		m_nCharNum = 0;
	}*/
	//else
	//{
	//	////�ַ�ʶ��
	
	CString strResult = CharReco(m_pImgChar,m_nCharNum);
	timeSpendRec = GetTickCount()-timeSart-timeSpendLoc-timeSpendSeg;
	//��ʾʶ��Ľ��
	if(color == COLOR_BLUE)
		GetDlgItem(IDC_RESULT)->SetWindowText(strResult+" ��");	
	else if(color == COLOR_YELLOW)
		GetDlgItem(IDC_RESULT)->SetWindowText(strResult+" ��");

	//}

	//��ʾ����ʱ��
	timeSpend = timeSpendLoc + timeSpendSeg + timeSpendRec;
	CString strMsg;
	strMsg.Format("���ƶ�λʱ��Ϊ:%d����\n�ַ��ָ�ʱ��Ϊ:%d����\n�ַ�ʶ��ʱ��Ϊ:%d����\n������ʱ��:%d����",
		timeSpendLoc,timeSpendSeg,timeSpendRec,timeSpend);
	GetDlgItem(IDC_STATIC_TIME)->SetWindowText(strMsg);

	//��ʾ
	ShowImage();

	cvReleaseImage(&img_lpr_gray);
	//cvReleaseImage(&img_half);
	cvReleaseImage(&m_imgSrc);
	for(int i=0;i<m_nCharNum;i++)
	{
		if(m_pImgChar[i] != NULL)
		{
			cvReleaseImage(&m_pImgChar[i]);
		}
	}

}

HBRUSH CLPRDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

void CLPRDlg::OnBnClickedBtnReset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();


}

void CLPRDlg::OnBnClickedBtnThresholdDes()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strMsg = "���ƶ�λ��ֵ˵��:����ֵ������ǰ���Ƶ�����,������Ⱥܵ�,\n��ô����ֵӦ���ʵ���С;����Ӧ�õ���.\n�ַ�ʶ����ֵ˵��:����ֵ�����ַ�������,����ַ�����������\n����ֵӦ�õ���;����Ӧ�õ�С.";
	MessageBox(strMsg,"��ֵ˵��");
}

void CLPRDlg::OnChangeReset(void)
{

}

void CLPRDlg::OnBtnCharLib()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//CharLibDlg dlg;
    //CharModelManage dlg;
	m_DesFilePath="F:\\OpenCV\\ͼƬ\\ʶ����ȷ";
	FileCopyTo(m_filePath,m_DesFilePath,m_filename,1);
	MessageBox("Copy Done!");
}


void CLPRDlg::OnBtnChLib()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//MessageBox("ok!");
	
	CCharModelManage dlg;
	dlg.DoModal();
}


void CLPRDlg::OnBtnRecogError()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_DesFilePath="F:\\OpenCV\\ͼƬ\\ʶ������";
	FileCopyTo(m_filePath,m_DesFilePath,m_filename,1);
	MessageBox("Copy Done!");
}

void CLPRDlg::OnBtnSegmentFail()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_DesFilePath="F:\\OpenCV\\ͼƬ\\�ָ����";
	FileCopyTo(m_filePath,m_DesFilePath,m_filename,1);
	MessageBox("Copy Done!");
}
