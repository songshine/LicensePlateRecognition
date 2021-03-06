#pragma once
#include "afxwin.h"


// CCharTrainDlg 对话框

class CCharTrainDlg : public CDialog
{
	DECLARE_DYNAMIC(CCharTrainDlg)

public:
	CCharTrainDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCharTrainDlg();

// 对话框数据
	enum { IDD = IDD_EDIT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_CharTrain;
	CString m_ChTrain;
	CFont m_EditFont;
CString m_cityName;
CButton m_btnOK;
CButton m_btnCancel;
CRect *rectStc;
CStatic *m_stc;
IplImage **imgChar;    //保存从文件读取出来字符图片

int   NumChar;             //保存图片数
	
afx_msg void OnBtnOk();
	
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnCancel();
	CCharTrainDlg(CString strCityName);
	
    
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void DrawPicToHDC(IplImage* img, UINT ID);
	double** ReadFromFile(CString filename, int NumChar);
	void DeleteRows(CString filename, int startRowIndex, int endRowIndex);
	virtual afx_msg BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	void ShowImage(void);



  

};
