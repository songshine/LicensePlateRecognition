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
// CCharLibDlg 对话框

class CCharLibDlg : public CDialog
{
	DECLARE_DYNAMIC(CCharLibDlg)

public:
	CCharLibDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCharLibDlg();

// 对话框数据
	enum { IDD = IDD_CHARLIB_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:

};
#pragma once


// CCharLibDlg 对话框
/*
class CCharLibDlg : public CDialog
{
	DECLARE_DYNAMIC(CCharLibDlg)

public:
	CCharLibDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCharLibDlg();

// 对话框数据
	enum { IDD = IDD_CHARLIB_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
*/