// CharTrainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LPR.h"
#include "CharTrainDlg.h"
#include "CharModelManage.h"

#include <fstream>
#include <fstream>
#include <iostream>
using namespace std;
// CCharTrainDlg 对话框


IplImage **imgChar=NULL;    //保存从文件读取出来字符图片
IplImage **ReImgChar=NULL;   

int   NumChar;             //保存图片数

IMPLEMENT_DYNAMIC(CCharTrainDlg, CDialog)

CCharTrainDlg::CCharTrainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCharTrainDlg::IDD, pParent)
	, m_ChTrain(_T(""))
	, m_cityName(_T(""))
{

}

CCharTrainDlg::~CCharTrainDlg()
{
}

void CCharTrainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_EDIT_CHAR, m_CharTrain);
}


BEGIN_MESSAGE_MAP(CCharTrainDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_OK, &CCharTrainDlg::OnBtnOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CCharTrainDlg::OnBtnCancel)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CCharTrainDlg 消息处理程序

void CCharTrainDlg::OnBtnOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
//	m_CharTrain.GetWindowText(m_ChTrain);

	//CCharModelManage *pWnd=(CCharModelManage*)AfxGetMainWnd();
if (m_cityName=="")
{
	m_CharTrain.GetWindowText(m_ChTrain);
	if (m_ChTrain=="")
	{
		AfxMessageBox("请输入字符！");
	}
	else OnOK();
}
else
{
	//MessageBox("JIng");
	OnOK();
	
}
}

BOOL CCharTrainDlg::OnInitDialog()
{
	

	// TODO:  在此添加额外的初始化
CDialog::OnInitDialog();
  
	
   if (m_cityName=="")
   {
	  
	   CRect rectStc(10,60,100,90);
	   CRect rectEdit(110,60,300,90);
	   CRect rectButton1(230,150,280,180);
       CRect rectButton2(300,150,350,180);

	   CString str="字符训练为：";

      CStatic *stc=new CStatic;
	  stc->Create(str,WS_CHILD|WS_VISIBLE,rectStc,this);
	   m_CharTrain.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_UPPERCASE,rectEdit,this,IDC_EDIT_CHAR);
 
	   m_btnOK.Create("确定",WS_CHILD|WS_VISIBLE,rectButton1,this,IDC_BTN_OK);
	   m_btnCancel.Create("取消",WS_CHILD|WS_VISIBLE,rectButton2,this,IDC_BUTTON1);

	   m_EditFont.CreatePointFont(190,"宋体");
	   m_CharTrain.SetFont(&m_EditFont);

	   CString strCaption="字符训练";
	   this->SetWindowText(strCaption);
   }
 
   if (m_cityName!="")
   {
      ::MoveWindow(m_hWnd,0,0,800,600,TRUE);     //重新设定窗口大小

	   CRect rectButton2(650,510,720,550);
	   m_btnCancel.Create("退出",WS_CHILD|WS_VISIBLE,rectButton2,this,IDC_BUTTON1);


	   int index= ReturnCharIndex(m_cityName);     //获取省市简称的索引
	   	   
	   CString s;
	   s.Format("%d",index);
	   CString fpath="./Data/"+s+".Model";
	  
	   NumChar=ScanCharNumFrmFile(fpath,CHAR_HEIGHT);   //获取文件中读取保存图片数
	 

	   rectStc=new CRect[NumChar];
	   m_stc=new CStatic[NumChar];

	   
	   for (int i=0;i<NumChar;i++)
	   {
		   
		   rectStc[i].left=10+i%15*50;         //每行15个,i/15=行X高；i%15=个X宽
		   rectStc[i].right=50+i%15*50;
		   rectStc[i].top=10+i/15*60;
		   rectStc[i].bottom=60+i/15*60;

		  m_stc[i].Create(m_cityName,WS_VISIBLE|WS_CHILD|SS_BLACKFRAME,rectStc[i],this,IDC_STC_IMG+i);
	   }


	double **data=ReadFromFile(fpath,NumChar);  //从文件读取数据

	   CvMat *mat64=new CvMat[NumChar];        //64FC1矩阵
       CvMat *mat32=new CvMat[NumChar];       //32FC1矩阵

       LoadMatConvert1(mat64,mat32,data,cvSize(24,48),NumChar);    // 将数据导入矩阵

	   imgChar=new IplImage*[NumChar];
	   ReImgChar=new IplImage*[NumChar];
	   for(int i=0;i<NumChar;i++)
	   {
		   imgChar[i]=cvCreateImage(cvGetSize(mat32),8,1);      //创建图像
	   }

	   for(int i=0;i<NumChar;i++)
	   {
		   cvGetImage(&mat32[i],imgChar[i]);     //将矩阵转换为图像
	   }
	  
	   CString strNumchar;
	   strNumchar.Format("%d",NumChar);
	   CString str="字符库："+m_cityName+"  总数："+strNumchar;
	   this->SetWindowText(str);
   }

	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CCharTrainDlg::OnBtnCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

CCharTrainDlg::CCharTrainDlg(CString strCityName)
{
 //   m_cityName=strCityName;
}

void CCharTrainDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (::IsWindow(m_btnOK.GetSafeHwnd()))
	{
		m_btnOK.MoveWindow(0,0,cx,cy);
	}
	
	// TODO: 在此处添加消息处理程序代码
}

void CCharTrainDlg::DrawPicToHDC(IplImage* img, UINT ID)
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);

	if (img!=NULL)
	{
		CvvImage cimg;

	cimg.CopyOf(img,3);
	cimg.DrawToHDC(hDC,&rect);

	}
	ReleaseDC(pDC);
}

double** CCharTrainDlg::ReadFromFile(CString filename, int NumChar)
{
	char* fName=(LPSTR)(LPCTSTR)filename;
	ifstream fp(fName,std::ios::in);
	
	char lin[1024]={0};

	double **data=new double*[NumChar]; 

	for (int i=0;i<NumChar;i++)   //创建二维
	{
		data[i]=new double[24*48];
	}
	int m=0;
	int k=0;

	if (fp)
	{
		while (fp.getline(lin,sizeof(lin)))
		{
			std::stringstream word(lin);


			for (int i=0;i<24;i++)
			{
				word>>data[k][m];
				m++;
			}

			if (m==24*48)   //如果取到最后一个元素
			{
				k++;       //指向下一个字符
				m=0;       //重置m
			}

		}
		
		fp.clear();
		fp.close();
	}

		
	return data;
	
}

void CCharTrainDlg::OnPaint()
{
	
	ShowImage();
		CDialog::OnPaint();

}

void CCharTrainDlg::ShowImage(void)
{
	for(int i=0;i<NumChar;i++)
	{
		if(imgChar[i] != NULL)
			DrawPicToHDC(imgChar[i],m_stc[i].GetDlgCtrlID());
	}
}
