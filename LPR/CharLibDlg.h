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
// CCharLibDlg �Ի���

class CCharLibDlg : public CDialog
{
	DECLARE_DYNAMIC(CCharLibDlg)

public:
	CCharLibDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCharLibDlg();

// �Ի�������
	enum { IDD = IDD_CHARLIB_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:

};
#pragma once


// CCharLibDlg �Ի���
/*
class CCharLibDlg : public CDialog
{
	DECLARE_DYNAMIC(CCharLibDlg)

public:
	CCharLibDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCharLibDlg();

// �Ի�������
	enum { IDD = IDD_CHARLIB_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
*/