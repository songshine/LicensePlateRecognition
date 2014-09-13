// CharTrainDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LPR.h"
#include "CharTrainDlg.h"
#include "CharModelManage.h"

#include <fstream>
#include <fstream>
#include <iostream>
using namespace std;
// CCharTrainDlg �Ի���


IplImage **imgChar=NULL;    //������ļ���ȡ�����ַ�ͼƬ
IplImage **ReImgChar=NULL;   

int   NumChar;             //����ͼƬ��

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


// CCharTrainDlg ��Ϣ�������

void CCharTrainDlg::OnBtnOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
//	m_CharTrain.GetWindowText(m_ChTrain);

	//CCharModelManage *pWnd=(CCharModelManage*)AfxGetMainWnd();
if (m_cityName=="")
{
	m_CharTrain.GetWindowText(m_ChTrain);
	if (m_ChTrain=="")
	{
		AfxMessageBox("�������ַ���");
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
	

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
CDialog::OnInitDialog();
  
	
   if (m_cityName=="")
   {
	  
	   CRect rectStc(10,60,100,90);
	   CRect rectEdit(110,60,300,90);
	   CRect rectButton1(230,150,280,180);
       CRect rectButton2(300,150,350,180);

	   CString str="�ַ�ѵ��Ϊ��";

      CStatic *stc=new CStatic;
	  stc->Create(str,WS_CHILD|WS_VISIBLE,rectStc,this);
	   m_CharTrain.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_UPPERCASE,rectEdit,this,IDC_EDIT_CHAR);
 
	   m_btnOK.Create("ȷ��",WS_CHILD|WS_VISIBLE,rectButton1,this,IDC_BTN_OK);
	   m_btnCancel.Create("ȡ��",WS_CHILD|WS_VISIBLE,rectButton2,this,IDC_BUTTON1);

	   m_EditFont.CreatePointFont(190,"����");
	   m_CharTrain.SetFont(&m_EditFont);

	   CString strCaption="�ַ�ѵ��";
	   this->SetWindowText(strCaption);
   }
 
   if (m_cityName!="")
   {
      ::MoveWindow(m_hWnd,0,0,800,600,TRUE);     //�����趨���ڴ�С

	   CRect rectButton2(650,510,720,550);
	   m_btnCancel.Create("�˳�",WS_CHILD|WS_VISIBLE,rectButton2,this,IDC_BUTTON1);


	   int index= ReturnCharIndex(m_cityName);     //��ȡʡ�м�Ƶ�����
	   	   
	   CString s;
	   s.Format("%d",index);
	   CString fpath="./Data/"+s+".Model";
	  
	   NumChar=ScanCharNumFrmFile(fpath,CHAR_HEIGHT);   //��ȡ�ļ��ж�ȡ����ͼƬ��
	 

	   rectStc=new CRect[NumChar];
	   m_stc=new CStatic[NumChar];

	   
	   for (int i=0;i<NumChar;i++)
	   {
		   
		   rectStc[i].left=10+i%15*50;         //ÿ��15��,i/15=��X�ߣ�i%15=��X��
		   rectStc[i].right=50+i%15*50;
		   rectStc[i].top=10+i/15*60;
		   rectStc[i].bottom=60+i/15*60;

		  m_stc[i].Create(m_cityName,WS_VISIBLE|WS_CHILD|SS_BLACKFRAME,rectStc[i],this,IDC_STC_IMG+i);
	   }


	double **data=ReadFromFile(fpath,NumChar);  //���ļ���ȡ����

	   CvMat *mat64=new CvMat[NumChar];        //64FC1����
       CvMat *mat32=new CvMat[NumChar];       //32FC1����

       LoadMatConvert1(mat64,mat32,data,cvSize(24,48),NumChar);    // �����ݵ������

	   imgChar=new IplImage*[NumChar];
	   ReImgChar=new IplImage*[NumChar];
	   for(int i=0;i<NumChar;i++)
	   {
		   imgChar[i]=cvCreateImage(cvGetSize(mat32),8,1);      //����ͼ��
	   }

	   for(int i=0;i<NumChar;i++)
	   {
		   cvGetImage(&mat32[i],imgChar[i]);     //������ת��Ϊͼ��
	   }
	  
	   CString strNumchar;
	   strNumchar.Format("%d",NumChar);
	   CString str="�ַ��⣺"+m_cityName+"  ������"+strNumchar;
	   this->SetWindowText(str);
   }

	
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CCharTrainDlg::OnBtnCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	
	// TODO: �ڴ˴������Ϣ����������
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

	for (int i=0;i<NumChar;i++)   //������ά
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

			if (m==24*48)   //���ȡ�����һ��Ԫ��
			{
				k++;       //ָ����һ���ַ�
				m=0;       //����m
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
