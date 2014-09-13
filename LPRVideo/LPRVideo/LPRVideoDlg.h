
// LPRVideoDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CLPRVideoDlg 对话框
class CLPRVideoDlg : public CDialog
{
// 构造
public:
	CLPRVideoDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CLPRVideoDlg();
// 对话框数据
	enum { IDD = IDD_LPRVIDEO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
private:
	CFont m_editFont;//编辑框字体
	CWinThread  * m_pThread;//视频线程
	
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

