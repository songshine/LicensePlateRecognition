#pragma once
#include "afxwin.h"


// CCharTrainDlg �Ի���

class CCharTrainDlg : public CDialog
{
	DECLARE_DYNAMIC(CCharTrainDlg)

public:
	CCharTrainDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCharTrainDlg();

// �Ի�������
	enum { IDD = IDD_EDIT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
	
afx_msg void OnBtnOk();
	
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnCancel();
	CCharTrainDlg(CString strCityName);
	
    
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void DrawPicToHDC(IplImage* img, UINT ID);
	double** ReadFromFile(CString filename, int NumChar);
	afx_msg void OnPaint();
	void ShowImage(void);



  

};
