
// LPRVideo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLPRVideoApp:
// �йش����ʵ�֣������ LPRVideo.cpp
//

class CLPRVideoApp : public CWinAppEx
{
public:
	CLPRVideoApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CLPRVideoApp theApp;