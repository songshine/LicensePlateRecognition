// LPRDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "resource.h"
#include "CharLibDlg.h"
// CLPRDlg �Ի���
class CLPRDlg : public CDialog
{
	// ����
public:
	CLPRDlg(CWnd* pParent = NULL);	// ��׼���캯��

	// �Ի�������
	enum { IDD = IDD_LPR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


	// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void DrawPicToHDC(IplImage *img, UINT ID);	



protected:
	IplImage* m_imgSrc;                           // ԭʼͼƬ

	
	

	//����
	IplImage *m_pImgLPR;
	CFont m_editFont;//�༭������

public:
	afx_msg void OnBnClickedLoadimage();
	afx_msg void OnBnClickedBtnLpr();
	CEdit m_editResult;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

//�ָ�����ַ���
	int m_nCharNum;
	IplImage **m_pImgChar;

public:
	afx_msg void OnBnClickedBtnReset();




public:
	afx_msg void OnBnClickedBtnThresholdDes();
	void OnChangeReset(void);

	CString m_ch[7];






private:
	//�ַ�ʶ��
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
// LPRDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "resource.h"
//#include "charmodelmanage.h"
#include "CharModelManage.h"


// CLPRDlg �Ի���
class CLPRDlg : public CDialog
{
// ����
public:
	CLPRDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_LPR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int CodeRecognize(IplImage *imgTest,int num,int char_num);
	int SegmentPlate();
	
	int PlateAreaSearch(IplImage *pImg_Image); //���ƶ�λ�㷨һ
	void Threshold(IplImage *Image,IplImage *Image_O);
	int AdaptiveThreshold(int t,IplImage *Image);
	void DrawPicToHDC(IplImage *img, UINT ID);	
public:
	void OnBinaryimg(); //ͼ���ֵ��
	void OnCarlocate(); //���ƶ�λ
	void OnSplitcar(); //�ַ��ָ�
	void OnShibiecar(); //�ַ�ʶ��
	
    //�ָ�����ַ���
	int m_nCharNum;
	IplImage **m_pImgChar;
    CString m_ch[7];



protected:
	IplImage *src ;                           // ԭʼͼƬ
	IplImage *pImgCanny;                      //��ֵ����ͼ
	IplImage *pImgResize;                     //��һ���ĳ�������Ҷ�ͼ
	IplImage *pImgCharOne;                // �ַ�ͼƬ
	IplImage *pImgCharTwo;
	IplImage *pImgCharThree;
	IplImage *pImgCharFour;
	IplImage *pImgCharFive;
	IplImage *pImgCharSix;
	IplImage *pImgCharSeven;
	

	//����
	IplImage *m_pImgLPR;
	CFont m_editFont;//�༭������

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
	//�ַ�ʶ��
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
