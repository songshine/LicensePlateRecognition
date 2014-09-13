// LPRDlg.cpp : 实现文件
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
#define POINT_X  18                                           // 水平方向不重叠点                        
#define POINT_Y  2                                            // 垂直方向不重叠点                            
#define WITH_X 0.1                                            // 水平方向车牌在图区域     
#define WITH_Y 0.2                                            // 垂直方向车牌在图区域     
#define HIGH_WITH_CAR   4.8   //(440/85)       // 小车的宽高比  440/140    
#define CHARACTER 15
#define TEMPLETENUM  43

#define DEFAULT_IMAGE_WIDTH 1000
#define DEFAULT_IMAGE_HEIGHT 600



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
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


// CLPRDlg 对话框

CLPRDlg::CLPRDlg(CWnd* pParent /*=NULL*/)
: CDialog(CLPRDlg::IDD, pParent)

, m_DesFilePath(_T(""))
, m_filePath(_T(""))
, m_filename(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//初始化  变量
	m_imgSrc = NULL;      //原始图片                        


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


// CLPRDlg 消息处理程序

BOOL CLPRDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


	int num=ScanCharNumFrmFile("./Data/Standard.Model",CHAR_HEIGHT);      //检测标准模板库是否存在
	if (num==0)
	{
		DirImgWrite2File("F:/OpenCV/LPR_NEW1.04_TEST/LPR/LPR/标准模板/","Standard.Model",cvSize(WIDTH,HEIGHT));
	}

	dataCvt2Mat32fc1();             //此处将模板数据导入矩阵中
	// InitTemplateFeature();          //读取模板库字符特征值，并保存入数组中



	//dataCvt2Mat32fc1();             //此处将模板数据导入矩阵中


	// TODO: 在此添加额外的初始化代码
	m_editFont.CreatePointFont(240,"宋体");
	m_editResult.SetFont(&m_editFont);
	m_editResult.SetWindowText("车牌识别结果");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLPRDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
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
		/*CRect rect;
		CPaintDC dc(this);
		GetClientRect(&rect);
		dc.FillSolidRect(&rect,RGB(200,200,200));*/
		ShowImage();
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
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
{// 打开图片
	// TODO: 在此添加控件通知处理程序代码
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

///////////////测试统计使用//////////////////
		filePath= dlg.GetPathName();

		m_filePath=dlg.GetPathName();
		m_filename=dlg.GetFileName();
////////////////////////////////////////////


		if((m_imgSrc=cvLoadImage(filePath)) == NULL)
		{
			MessageBox("加载图像失败");
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
//字符识别
CString CLPRDlg::CharReco(IplImage** charImg,int charnum)
{
	/*******************使用多模板****************************/
	CString s=MultiTemplateRecogn(charImg,charnum);

	/*******************使用特征值+模板****************************/
	// CString s=TempalteFeatureRecogn(charImg,charnum,FEATURE_BY_GRID);

	// CString s=FeatureTempalteRecogn(charImg,charnum,FEATURE_BY_GRID);


	//从CString中一个一个的取出字符
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
{//进行图像识别

	//获取初始时间
	long timeSart,timeSpend=0,timeSpendLoc=0,timeSpendSeg=0,timeSpendRec=0;
	timeSart = GetTickCount(); 

	// TODO: 在此添加控件通知处理程序代码
	if(m_imgSrc == NULL)
	{
		MessageBox("请选择图");
		return ;
	}
	//车牌定位
	CvRect rectLPR;
	int color;  //车牌颜色
	IplImage* img_lpr_gray = NULL;
	if(m_pImgLPR != NULL)
		cvReleaseImage(&m_pImgLPR);	

	/*IplImage* img_resize = cvCreateImage(cvSize(DEFAULT_IMAGE_WIDTH,DEFAULT_IMAGE_WIDTH*(m_imgSrc->height/m_imgSrc->width)),m_imgSrc->depth,m_imgSrc->nChannels);
	img_resize = cvResize(m_imgSrc,img_resize);*/
	img_lpr_gray = LPLocate(m_imgSrc,&rectLPR,&color);
	timeSpendLoc = GetTickCount()-timeSart;


	if(img_lpr_gray == NULL)
	{
		MessageBox("定位车牌失败...\n");
		return;
	}
	//获得彩色车牌
	//m_pImgLPR = GetRectImage(m_imgSrc,rectLPR);
	m_pImgLPR = LPRotateFromRect(m_imgSrc,rectLPR,color);
	/*cvShowImage("img_lpr",m_pImgLPR);
	cvWaitKey(0);*/
	//cvSaveImage("D:\\d.jpg",m_pImgLPR);
	//cvShowImage("imgdd",m_pImgLPR);
	cvRectangle(m_imgSrc,cvPoint(rectLPR.x,rectLPR.y),cvPoint(rectLPR.x+rectLPR.width,rectLPR.y+rectLPR.height),CV_RGB(0,255,0),3);

	

	//字符分割
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
		MessageBox("哈哈，分割错了");
		ReleaseImages(m_pImgChar,m_nCharNum);
		m_nCharNum = 0;
	}
	else if (m_nCharNum < CHARACTER_NUM)
	{
		MessageBox("哈哈，分割错了");
		ReleaseImages(m_pImgChar,m_nCharNum);
		m_nCharNum = 0;
	}*/
	//else
	//{
	//	////字符识别
	
	CString strResult = CharReco(m_pImgChar,m_nCharNum);
	timeSpendRec = GetTickCount()-timeSart-timeSpendLoc-timeSpendSeg;
	//显示识别的结果
	if(color == COLOR_BLUE)
		GetDlgItem(IDC_RESULT)->SetWindowText(strResult+" 蓝");	
	else if(color == COLOR_YELLOW)
		GetDlgItem(IDC_RESULT)->SetWindowText(strResult+" 黄");

	//}

	//显示所用时间
	timeSpend = timeSpendLoc + timeSpendSeg + timeSpendRec;
	CString strMsg;
	strMsg.Format("车牌定位时间为:%d毫秒\n字符分割时间为:%d毫秒\n字符识别时间为:%d毫秒\n所用总时间:%d毫秒",
		timeSpendLoc,timeSpendSeg,timeSpendRec,timeSpend);
	GetDlgItem(IDC_STATIC_TIME)->SetWindowText(strMsg);

	//显示
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

	// TODO:  在此更改 DC 的任何属性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CLPRDlg::OnBnClickedBtnReset()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();


}

void CLPRDlg::OnBnClickedBtnThresholdDes()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strMsg = "车牌定位阈值说明:该阈值关联当前车牌的亮度,如果亮度很低,\n那么该阈值应该适当调小;否则应该调大.\n字符识别阈值说明:该阈值关联字符清晰度,如果字符不清晰，则\n该阈值应该调大;否则应该调小.";
	MessageBox(strMsg,"阈值说明");
}

void CLPRDlg::OnChangeReset(void)
{

}

void CLPRDlg::OnBtnCharLib()
{
	// TODO: 在此添加控件通知处理程序代码
	//CharLibDlg dlg;
    //CharModelManage dlg;
	m_DesFilePath="F:\\OpenCV\\图片\\识别正确";
	FileCopyTo(m_filePath,m_DesFilePath,m_filename,1);
	MessageBox("Copy Done!");
}


void CLPRDlg::OnBtnChLib()
{
	// TODO: 在此添加控件通知处理程序代码
	//MessageBox("ok!");
	
	CCharModelManage dlg;
	dlg.DoModal();
}


void CLPRDlg::OnBtnRecogError()
{
	// TODO: 在此添加控件通知处理程序代码
	m_DesFilePath="F:\\OpenCV\\图片\\识别有误";
	FileCopyTo(m_filePath,m_DesFilePath,m_filename,1);
	MessageBox("Copy Done!");
}

void CLPRDlg::OnBtnSegmentFail()
{
	// TODO: 在此添加控件通知处理程序代码
	m_DesFilePath="F:\\OpenCV\\图片\\分割错误";
	FileCopyTo(m_filePath,m_DesFilePath,m_filename,1);
	MessageBox("Copy Done!");
}
