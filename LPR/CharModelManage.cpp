// CharModelManage.cpp : 实现文件
//

#include "stdafx.h"
#include "LPR.h"
#include "CharModelManage.h"
#include "LPRDlg.h"
#include "CharTrainDlg.h"
#include "CharLibDlg.h"

// CCharModelManage 对话框

IplImage** m_pCharImg=NULL;    //保存字符图像
int  nNumChar;


IMPLEMENT_DYNAMIC(CCharModelManage, CDialog)

CCharModelManage::CCharModelManage(CWnd* pParent /*=NULL*/)
	: CDialog(CCharModelManage::IDD, pParent)
	//, m_pCharImg(NULL)
	, m_CharTrain(_T(""))
	, m_ChoisChar(_T(""))
{
}

CCharModelManage::~CCharModelManage()
{
}

void CCharModelManage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCharModelManage, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &CCharModelManage::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_CHAR1, &CCharModelManage::OnBtnBtnChar1)
	ON_BN_CLICKED(IDC_BTN_CHAR2, &CCharModelManage::OnBtnChar2)
	ON_BN_CLICKED(IDC_BTN_CHAR3, &CCharModelManage::OnBtnChar3)
	ON_BN_CLICKED(IDC_BTN_CHAR4, &CCharModelManage::OnBtnChar4)
	ON_BN_CLICKED(IDC_BTN_CHAR5, &CCharModelManage::OnBtnChar5)
	ON_BN_CLICKED(IDC_BTN_CHAR6, &CCharModelManage::OnBtnChar6)
	ON_BN_CLICKED(IDC_BTN_CHAR7, &CCharModelManage::OnBtnChar7)
	ON_BN_CLICKED(IDC_BUTTON1, &CCharModelManage::OnBtnB_Jing)
	//ON_NOTIFY(NM_CLICK, IDC_SYSLINK1, &CCharModelManage::OnNMClkB_Jing)
	ON_BN_CLICKED(IDC_BUTTON10, &CCharModelManage::OnBtnJ_Su)
	ON_BN_CLICKED(IDC_BUTTON2, &CCharModelManage::OnBtnT_Jin)
	ON_BN_CLICKED(IDC_BUTTON3, &CCharModelManage::OnBtnH_Ji)
	ON_BN_CLICKED(IDC_BUTTON4, &CCharModelManage::OnBtnH_Jin)
	ON_BN_CLICKED(IDC_BUTTON5, &CCharModelManage::OnBtnN_Meng)
	ON_BN_CLICKED(IDC_BUTTON6, &CCharModelManage::OnBtnS_Liao)
	ON_BN_CLICKED(IDC_BUTTON7, &CCharModelManage::OnBtnJ_Ji)
	ON_BN_CLICKED(IDC_BUTTON8, &CCharModelManage::OnBtnH_Hei)
	ON_BN_CLICKED(IDC_BUTTON9, &CCharModelManage::OnBtnS_Hu)
	ON_BN_CLICKED(IDC_BUTTON11, &CCharModelManage::OnBtnZ_Zhe)
	ON_BN_CLICKED(IDC_BUTTON12, &CCharModelManage::OnBtnAn_Wan)
	ON_BN_CLICKED(IDC_BUTTON13, &CCharModelManage::OnBtnF_Min)
	ON_BN_CLICKED(IDC_BUTTON14, &CCharModelManage::OnBtnJ_Gan)
	ON_BN_CLICKED(IDC_BUTTON15, &CCharModelManage::OnBtnS_Lu)
	ON_BN_CLICKED(IDC_BUTTON16, &CCharModelManage::OnBtnH_Yu)
	ON_BN_CLICKED(IDC_BUTTON17, &CCharModelManage::OnBtnH_E)
	ON_BN_CLICKED(IDC_BUTTON18, &CCharModelManage::OnBtnH_Xiang)
	ON_BN_CLICKED(IDC_BUTTON19, &CCharModelManage::OnBtnG_Yue)
	ON_BN_CLICKED(IDC_BUTTON20, &CCharModelManage::OnBtnGX_Gui)
	ON_BN_CLICKED(IDC_BUTTON21, &CCharModelManage::OnBtnH_Qiong)
	ON_BN_CLICKED(IDC_BUTTON22, &CCharModelManage::OnBtnC_Yu)
	ON_BN_CLICKED(IDC_BUTTON23, &CCharModelManage::OnBtnS_Chuan)
	ON_BN_CLICKED(IDC_BUTTON24, &CCharModelManage::OnBtnGZ_Gui)
	ON_BN_CLICKED(IDC_BUTTON25, &CCharModelManage::OnBtnY_Yun)
	ON_BN_CLICKED(IDC_BUTTON26, &CCharModelManage::OnBtnX_Zang)
	ON_BN_CLICKED(IDC_BUTTON27, &CCharModelManage::OnBtnS_Shan)
	ON_BN_CLICKED(IDC_BUTTON28, &CCharModelManage::OnBtnG_Gan)
	ON_BN_CLICKED(IDC_BUTTON29, &CCharModelManage::OnBtnQ_Qing)
	ON_BN_CLICKED(IDC_BUTTON30, &CCharModelManage::OnBtnN_Ning)
	ON_BN_CLICKED(IDC_BUTTON31, &CCharModelManage::OnBtnX_Xin)
	ON_BN_CLICKED(IDC_BUTTON32, &CCharModelManage::OnBtnGang)
	ON_BN_CLICKED(IDC_BUTTON33, &CCharModelManage::OnBtnAo)
	ON_BN_CLICKED(IDC_BUTTON34, &CCharModelManage::OnBtnXue)
	ON_BN_CLICKED(IDC_BUTTON35, &CCharModelManage::OnBtnJing)
	ON_BN_CLICKED(IDC_BUTTON36, &CCharModelManage::OnBtnA)
	ON_BN_CLICKED(IDC_BUTTON37, &CCharModelManage::OnBtnB)
	ON_BN_CLICKED(IDC_BUTTON38, &CCharModelManage::OnBtnC)
	ON_BN_CLICKED(IDC_BUTTON39, &CCharModelManage::OnBtnD)
	ON_BN_CLICKED(IDC_BUTTON40, &CCharModelManage::OnBtnE)
	ON_BN_CLICKED(IDC_BUTTON41, &CCharModelManage::OnBtnF)
	ON_BN_CLICKED(IDC_BUTTON42, &CCharModelManage::OnBtnG)
	ON_BN_CLICKED(IDC_BUTTON43, &CCharModelManage::OnBtnH)
	ON_BN_CLICKED(IDC_BUTTON44, &CCharModelManage::OnBtnI)
	ON_BN_CLICKED(IDC_BUTTON45, &CCharModelManage::OnBtnJ)
	ON_BN_CLICKED(IDC_BUTTON46, &CCharModelManage::OnBtnK)
	ON_BN_CLICKED(IDC_BUTTON47, &CCharModelManage::OnBtnL)
	ON_BN_CLICKED(IDC_BUTTON48, &CCharModelManage::OnBtnM)
	ON_BN_CLICKED(IDC_BUTTON49, &CCharModelManage::OnBtnN)
	ON_BN_CLICKED(IDC_BUTTON50, &CCharModelManage::OnBtnO)
	ON_BN_CLICKED(IDC_BUTTON51, &CCharModelManage::OnBtnP)
	ON_BN_CLICKED(IDC_BUTTON52, &CCharModelManage::OnBtnQ)
	ON_BN_CLICKED(IDC_BUTTON53, &CCharModelManage::OnBtnR)
	ON_BN_CLICKED(IDC_BUTTON54, &CCharModelManage::OnBtnS)
	ON_BN_CLICKED(IDC_BUTTON55, &CCharModelManage::OnBtnT)
	ON_BN_CLICKED(IDC_BUTTON56, &CCharModelManage::OnBtnU)
	ON_BN_CLICKED(IDC_BUTTON57, &CCharModelManage::OnBtnV)
	ON_BN_CLICKED(IDC_BUTTON58, &CCharModelManage::OnBtnW)
	ON_BN_CLICKED(IDC_BUTTON59, &CCharModelManage::OnBtnX)
	ON_BN_CLICKED(IDC_BUTTON60, &CCharModelManage::OnBtnY)
	ON_BN_CLICKED(IDC_BUTTON61, &CCharModelManage::OnBtnZ)
	ON_BN_CLICKED(IDC_BUTTON62, &CCharModelManage::OnBtnOne)
	ON_BN_CLICKED(IDC_BUTTON63, &CCharModelManage::OnBtnTwo)
	ON_BN_CLICKED(IDC_BUTTON64, &CCharModelManage::OnBtnThree)
	ON_BN_CLICKED(IDC_BUTTON65, &CCharModelManage::OnBtnFour)
	ON_BN_CLICKED(IDC_BUTTON66, &CCharModelManage::OnBtnFive)
	ON_BN_CLICKED(IDC_BUTTON67, &CCharModelManage::OnBtnSix)
	ON_BN_CLICKED(IDC_BUTTON68, &CCharModelManage::OnBtnSeven)
	ON_BN_CLICKED(IDC_BUTTON69, &CCharModelManage::OnBtnEight)
	ON_BN_CLICKED(IDC_BUTTON70, &CCharModelManage::OnBtnNine)
	ON_BN_CLICKED(IDC_BUTTON71, &CCharModelManage::OnBtnZero)
END_MESSAGE_MAP()


// CCharModelManage 消息处理程序

void CCharModelManage::ShowImage(void)
{
	if(m_pCharImg != NULL)
	{
		for(int i=0;i<7;i++)
		{
			if(m_pCharImg[i] != NULL)
				DrawPicToHDC(m_pCharImg[i],IDC_IMG_CHAR1+i);
		}
	}
	
}

void CCharModelManage::DrawPicToHDC(IplImage* img, UINT ID)
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
   
	if (img != NULL)
	{ 
		CvvImage cimg;
		cimg.CopyOf(img,3);
		cimg.DrawToHDC(hDC,&rect);
		
	}
	
	ReleaseDC(pDC);
}

BOOL CCharModelManage::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

    CLPRDlg*  pWnd=(CLPRDlg*)AfxGetMainWnd();
    nNumChar=pWnd->m_nCharNum;
	
    if (pWnd->m_pImgChar!=NULL&&pWnd->m_nCharNum!=0)
	{
		if(m_pCharImg == NULL)
		{
			m_pCharImg = new IplImage*[TOTAL_CHAR];
		}
		for (int i=0;i<pWnd->m_nCharNum;i++)
		{
			m_pCharImg[i]=pWnd->m_pImgChar[i];
		}
    }

	
	m_StaticFon.CreatePointFont(200,"宋体");
	for (int i=0;i<7;i++)
	{
		((CButton*)GetDlgItem(IDC_BTN_CHAR1+i))->SetFont(&m_StaticFon);
	}
	
 
	//this->OnPaint();                    // 重绘对话框
	                // 更新windows窗口，如果无这步调用，图片显示还会出现问题
	//cvShowImage("te",m_pCharImg[1]);
	//cvWaitKey(0);
	ShowImage();
	ShowChar();
	//this->Invalidate();
	//CDialog::UpdateWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CCharModelManage::OnPaint()
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
		//dc.DrawIcon(x, y, this->m_hIcon);
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

void CCharModelManage::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
   /*CStatic *m_stcImg=new CStatic;
   CRect rect(50,180,90,250);
  m_stcImg->Create("",WS_CHILD|WS_VISIBLE|SS_BLACKFRAME,rect,this);

   DrawPicToHDC(m_pCharImg[2],m_stcImg->GetDlgCtrlID());*/
	//ShowImage();
	//ShowChar();
	OnOK();
}

void CCharModelManage::ShowChar(void)
{
   CLPRDlg *pWnd=(CLPRDlg*)AfxGetMainWnd();
   for (int i=0;i<7;i++)
   {
     m_ch[i]=pWnd->m_ch[i];
   }


   for (int i=0;i<7;i++)
   {
	   if(m_ch[i] != "")
	   {
		   ((CButton*)GetDlgItem(IDC_BTN_CHAR1+i))->SetWindowText(m_ch[i]);
	   }
	   else
	   {
		   ((CButton*)GetDlgItem(IDC_BTN_CHAR1+i))->EnableWindow(FALSE);
	   }
   }
  
   

}


void CCharModelManage::OnBtnBtnChar1()
{
	// TODO: 在此添加控件通知处理程序代码
 
	//CString str="./Data/0.Model";
	//ReadFromFile(str,1);

	CCharTrainDlg dlg;
	dlg.DoModal();
	m_CharTrain=dlg.m_ChTrain;
	if (m_CharTrain!="")
	{
		int CharIndex=ReturnCharIndex(m_CharTrain);
		if (CharIndex!=-1)
		{
			m_pCharImg[0]=t_Resize(m_pCharImg[0],cvSize(WIDTH,HEIGHT));
			GreyValueCollection(m_pCharImg[0],CharIndex);
			((CButton*)GetDlgItem(IDC_BTN_CHAR1+0))->SetWindowText(m_CharTrain);
		}


	}
   
}

void CCharModelManage::OnBtnChar2()
{
	// TODO: 在此添加控件通知处理程序代码
	CCharTrainDlg dlg;
	dlg.DoModal();
	m_CharTrain=dlg.m_ChTrain;
	if (m_CharTrain!="")
	{
		int CharIndex=ReturnCharIndex(m_CharTrain);

		if (CharIndex!=-1)
		{
		    m_pCharImg[1]=t_Resize(m_pCharImg[1],cvSize(WIDTH,HEIGHT));
		    GreyValueCollection(m_pCharImg[1],CharIndex);
			((CButton*)GetDlgItem(IDC_BTN_CHAR1+1))->SetWindowText(m_CharTrain);
		}
	}
}

void CCharModelManage::OnBtnChar3()
{
	// TODO: 在此添加控件通知处理程序代码
	CCharTrainDlg dlg;
	dlg.DoModal();
	m_CharTrain=dlg.m_ChTrain;
	if (m_CharTrain!="")
	{
		int CharIndex=ReturnCharIndex(m_CharTrain);

		if (CharIndex!=-1)
		{
		    m_pCharImg[2]=t_Resize(m_pCharImg[2],cvSize(WIDTH,HEIGHT));
		    GreyValueCollection(m_pCharImg[2],CharIndex);
			((CButton*)GetDlgItem(IDC_BTN_CHAR1+2))->SetWindowText(m_CharTrain);
		}
	}
}

void CCharModelManage::OnBtnChar4()
{
	// TODO: 在此添加控件通知处理程序代码
	CCharTrainDlg dlg;
	dlg.DoModal();
	m_CharTrain=dlg.m_ChTrain;
	if (m_CharTrain!="")
	{
		int CharIndex=ReturnCharIndex(m_CharTrain);

		if (CharIndex!=-1)
		{
		    m_pCharImg[3]=t_Resize(m_pCharImg[3],cvSize(WIDTH,HEIGHT));
		    GreyValueCollection(m_pCharImg[3],CharIndex);
			((CButton*)GetDlgItem(IDC_BTN_CHAR1+3))->SetWindowText(m_CharTrain);
		}
	}
}

void CCharModelManage::OnBtnChar5()
{
	// TODO: 在此添加控件通知处理程序代码
	CCharTrainDlg dlg;
	dlg.DoModal();
	m_CharTrain=dlg.m_ChTrain;
	if (m_CharTrain!="")
	{
		int CharIndex=ReturnCharIndex(m_CharTrain);

		if (CharIndex!=-1)
		{
		     m_pCharImg[4]=t_Resize(m_pCharImg[4],cvSize(WIDTH,HEIGHT));
		    GreyValueCollection(m_pCharImg[4],CharIndex);
			((CButton*)GetDlgItem(IDC_BTN_CHAR1+4))->SetWindowText(m_CharTrain);
		}
	}
}

void CCharModelManage::OnBtnChar6()
{
	// TODO: 在此添加控件通知处理程序代码
	CCharTrainDlg dlg;
	dlg.DoModal();
	m_CharTrain=dlg.m_ChTrain;
	if (m_CharTrain!="")
	{
		int CharIndex=ReturnCharIndex(m_CharTrain);
		if (CharIndex!=-1)
		{
		    m_pCharImg[5]=t_Resize(m_pCharImg[5],cvSize(WIDTH,HEIGHT));
		    GreyValueCollection(m_pCharImg[5],CharIndex);
			((CButton*)GetDlgItem(IDC_BTN_CHAR1+5))->SetWindowText(m_CharTrain);
		}
	}
}

void CCharModelManage::OnBtnChar7()
{
	// TODO: 在此添加控件通知处理程序代码
	CCharTrainDlg dlg;
	dlg.DoModal();
	m_CharTrain=dlg.m_ChTrain;
	if (m_CharTrain!="")
	{
		int CharIndex=ReturnCharIndex(m_CharTrain);
		if (CharIndex!=-1)
		{
		    m_pCharImg[6]=t_Resize(m_pCharImg[6],cvSize(WIDTH,HEIGHT));
		    GreyValueCollection(m_pCharImg[6],CharIndex);
			((CButton*)GetDlgItem(IDC_BTN_CHAR1+6))->SetWindowText(m_CharTrain);
		}
	}
	
}

void CCharModelManage::OnBtnB_Jing()
{
	// TODO: 在此添加控件通知处理程序代码
	
	m_ChoisChar="京";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;
	
	dlg.DoModal();
	
}

/*void CCharModelManage::OnNMClkB_Jing(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	MessageBox("hello");
}*/

void CCharModelManage::OnBtnJ_Su()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="苏";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnT_Jin()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="津";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnH_Ji()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="冀";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnH_Jin()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="晋";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnN_Meng()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="蒙";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnS_Liao()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="辽";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnJ_Ji()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="吉";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnH_Hei()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="黑";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnS_Hu()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="沪";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnZ_Zhe()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="浙";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnAn_Wan()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="皖";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnF_Min()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="闽";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnJ_Gan()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="赣";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnS_Lu()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="鲁";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnH_Yu()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="豫";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnH_E()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="鄂";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnH_Xiang()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="湘";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnG_Yue()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="粤";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnGX_Gui()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="桂";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnH_Qiong()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="琼";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnC_Yu()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="渝";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnS_Chuan()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="川";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnGZ_Gui()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="贵";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnY_Yun()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="云";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnX_Zang()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="藏";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnS_Shan()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="陕";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnG_Gan()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="甘";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnQ_Qing()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="青";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnN_Ning()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="宁";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnX_Xin()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="新";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnGang()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="港";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnAo()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="澳";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnXue()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="学";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnJing()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="警";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnA()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="A";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnB()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="B";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnC()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="C";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnD()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="D";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnE()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="E";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnF()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="F";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnG()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="G";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnH()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="H";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnI()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="I";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnJ()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="J";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnK()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="K";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnL()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="L";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnM()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="M";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnN()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="N";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnO()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="O";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnP()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="P";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnQ()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="Q";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnR()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="R";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnS()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="S";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnT()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="T";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnU()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="U";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnV()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="V";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnW()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="W";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnX()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="X";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnY()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="Y";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnZ()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="Z";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnOne()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="1";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnTwo()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="2";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnThree()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="3";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnFour()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="4";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnFive()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="5";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnSix()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="6";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnSeven()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="7";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnEight()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="8";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnNine()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="9";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}

void CCharModelManage::OnBtnZero()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChoisChar="0";
	CCharTrainDlg dlg;
	dlg.m_cityName=m_ChoisChar;

	dlg.DoModal();
}
