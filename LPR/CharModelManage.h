#pragma once
#include "stdafx.h"
#include "LPR.h"
//#include "LPRDlg.h"
#include "LPHelper.h"
#include "LPLocation.h"
#include "LPRotate.h"
#include "CharSegmentation.h"
#include "CharRecognition.h"
#include "CharLibDlg.h"

#include "resource.h"
// CCharModelManage 对话框

class CCharModelManage : public CDialog
{
	DECLARE_DYNAMIC(CCharModelManage)

public:
	CCharModelManage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCharModelManage();

// 对话框数据
	enum { IDD =IDD_DLG_CHMANAGE  };//IDD_DIALOG1

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	
	void ShowImage(void);
private:
	void DrawPicToHDC(IplImage* img, UINT ID);

	CString m_ch[7];
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedOk();
private:
	void ShowChar(void);
public:
	CFont m_StaticFon;
	afx_msg void OnStnDblClkImgChar1();
	afx_msg void OnBtnBtnChar1();
	CString m_CharTrain;
	afx_msg void OnBtnChar2();
	afx_msg void OnBtnChar3();
	afx_msg void OnBtnChar4();
	afx_msg void OnBtnChar5();
	afx_msg void OnBtnChar6();
	afx_msg void OnBtnChar7();
	afx_msg void OnBtnB_Jing();
	CString m_ChoisChar;
	//afx_msg void OnNMClkB_Jing(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBtnJ_Su();
	afx_msg void OnBtnT_Jin();
	afx_msg void OnBtnH_Ji();
	afx_msg void OnBtnH_Jin();
	afx_msg void OnBtnN_Meng();
	afx_msg void OnBtnS_Liao();
	afx_msg void OnBtnJ_Ji();
	afx_msg void OnBtnH_Hei();
	afx_msg void OnBtnS_Hu();
	afx_msg void OnBtnZ_Zhe();
	afx_msg void OnBtnAn_Wan();
	afx_msg void OnBtnF_Min();
	afx_msg void OnBtnJ_Gan();
	afx_msg void OnBtnS_Lu();
	afx_msg void OnBtnH_Yu();
	afx_msg void OnBtnH_E();
	afx_msg void OnBtnH_Xiang();
	afx_msg void OnBtnG_Yue();
	afx_msg void OnBtnGX_Gui();
	afx_msg void OnBtnH_Qiong();
	afx_msg void OnBtnC_Yu();
	afx_msg void OnBtnS_Chuan();
	afx_msg void OnBtnGZ_Gui();
	afx_msg void OnBtnY_Yun();
	afx_msg void OnBtnX_Zang();
	afx_msg void OnBtnS_Shan();
	afx_msg void OnBtnG_Gan();
	afx_msg void OnBtnQ_Qing();
	afx_msg void OnBtnN_Ning();
	afx_msg void OnBtnX_Xin();
	afx_msg void OnBtnGang();
	afx_msg void OnBtnAo();
	afx_msg void OnBtnXue();
	afx_msg void OnBtnJing();
	afx_msg void OnBtnA();
	afx_msg void OnBtnB();
	afx_msg void OnBtnC();
	afx_msg void OnBtnD();
	afx_msg void OnBtnE();
	afx_msg void OnBtnF();
	afx_msg void OnBtnG();
	afx_msg void OnBtnH();
	afx_msg void OnBtnI();
	afx_msg void OnBtnJ();
	afx_msg void OnBtnK();
	afx_msg void OnBtnL();
	afx_msg void OnBtnM();
	afx_msg void OnBtnN();
	afx_msg void OnBtnO();
	afx_msg void OnBtnP();
	afx_msg void OnBtnQ();
	afx_msg void OnBtnR();
	afx_msg void OnBtnS();
	afx_msg void OnBtnT();
	afx_msg void OnBtnU();
	afx_msg void OnBtnV();
	afx_msg void OnBtnW();
	afx_msg void OnBtnX();
	afx_msg void OnBtnY();
	afx_msg void OnBtnZ();
	afx_msg void OnBtnOne();
	afx_msg void OnBtnTwo();
	afx_msg void OnBtnThree();
	afx_msg void OnBtnFour();
	afx_msg void OnBtnFive();
	afx_msg void OnBtnSix();
	afx_msg void OnBtnSeven();
	afx_msg void OnBtnEight();
	afx_msg void OnBtnNine();
	afx_msg void OnBtnZero();
};
