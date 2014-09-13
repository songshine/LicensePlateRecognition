// LPRDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "resource.h"
#include "CharLibDlg.h"
// CLPRDlg 对话框
class CLPRDlg : public CDialog
{
	// 构造
public:
	CLPRDlg(CWnd* pParent = NULL);	// 标准构造函数

	// 对话框数据
	enum { IDD = IDD_LPR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


	// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void DrawPicToHDC(IplImage *img, UINT ID);	



protected:
	IplImage* m_imgSrc;                           // 原始图片

	
	

	//车牌
	IplImage *m_pImgLPR;
	CFont m_editFont;//编辑框字体

public:
	afx_msg void OnBnClickedLoadimage();
	afx_msg void OnBnClickedBtnLpr();
	CEdit m_editResult;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

//分割出的字符数
	int m_nCharNum;
	IplImage **m_pImgChar;

public:
	afx_msg void OnBnClickedBtnReset();




public:
	afx_msg void OnBnClickedBtnThresholdDes();
	void OnChangeReset(void);

	CString m_ch[7];






private:
	//字符识别
	CString CharReco(IplImage** charImg,int charnum);
	void ShowImage();
public:
	afx_msg void OnBtnCharLib();
	afx_msg void OnBtnChLib();
	afx_msg void OnBtnRecogError();
	afx_msg void OnBtnSegmentFail();
	CString m_DesFilePath;
	CString m_filePath;
	CString m_filename;
};






/*
// LPRDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "resource.h"
//#include "charmodelmanage.h"
#include "CharModelManage.h"


// CLPRDlg 对话框
class CLPRDlg : public CDialog
{
// 构造
public:
	CLPRDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_LPR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int CodeRecognize(IplImage *imgTest,int num,int char_num);
	int SegmentPlate();
	
	int PlateAreaSearch(IplImage *pImg_Image); //车牌定位算法一
	void Threshold(IplImage *Image,IplImage *Image_O);
	int AdaptiveThreshold(int t,IplImage *Image);
	void DrawPicToHDC(IplImage *img, UINT ID);	
public:
	void OnBinaryimg(); //图像二值化
	void OnCarlocate(); //车牌定位
	void OnSplitcar(); //字符分割
	void OnShibiecar(); //字符识别
	
    //分割出的字符数
	int m_nCharNum;
	IplImage **m_pImgChar;
    CString m_ch[7];



protected:
	IplImage *src ;                           // 原始图片
	IplImage *pImgCanny;                      //二值化的图
	IplImage *pImgResize;                     //归一化的车牌区域灰度图
	IplImage *pImgCharOne;                // 字符图片
	IplImage *pImgCharTwo;
	IplImage *pImgCharThree;
	IplImage *pImgCharFour;
	IplImage *pImgCharFive;
	IplImage *pImgCharSix;
	IplImage *pImgCharSeven;
	

	//车牌
	IplImage *m_pImgLPR;
	CFont m_editFont;//编辑框字体

public:
	afx_msg void OnBnClickedLoadimage();
	afx_msg void OnBnClickedBtnLpr();
	CEdit m_editResult;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

private:
	int m_nSlide;
public:
	CSliderCtrl m_slideCtrl;
	afx_msg void OnBnClickedBtnReset();
	CSliderCtrl m_sliderCtrlReco;

	
	int m_nSliderReco;
private:
	
public:
	afx_msg void OnBnClickedBtnThresholdDes();
	void OnChangeReset(void);
private:
	//字符识别
	CString CharReco(IplImage** charImg,int charnum);
	void ShowImage();
public:
	afx_msg void OnBtnRecogRight();
	afx_msg void OnBtnRecogError();
	afx_msg void OnBtnSegmentFail();
	CString m_filePath;
	CString m_filename;
	CString m_DesFilePath;
	//CCharModelManage m_chManageDlg;
	afx_msg void OnBtnChManage();
};
*/
