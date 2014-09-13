
// LPRVideoDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CLPRVideoDlg �Ի���
class CLPRVideoDlg : public CDialog
{
// ����
public:
	CLPRVideoDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CLPRVideoDlg();
// �Ի�������
	enum { IDD = IDD_LPRVIDEO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
private:
	CFont m_editFont;//�༭������
	CWinThread  * m_pThread;//��Ƶ�߳�
	
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
	void DrawPicToHDC(IplImage *img, UINT ID);
	int BeginReco(const IplImage* src);
	CString CharReco(IplImage** charImg);
	afx_msg void OnOpenImage();

	void ShowVideoFrame(const IplImage* frame);
	void CaptureLP();
	CEdit m_editResult;
	afx_msg void OnBnClickedBtnCapLp();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnOpenVedio();
	afx_msg void OnBnClickedBtnPause();
	afx_msg void OnStnClickedStaticVedio();
};
typedef struct _t_p
{
	CLPRVideoDlg* pDlg;
	CString strVideoPath;
}ThreadPara,*PThreadPara;
typedef struct _lp_t
{
	CLPRVideoDlg* pDlg;
	IplImage* img;
}LPThreadPara,*PLPThreadPara;

