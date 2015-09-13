// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// �� Windows ͷ���ų�����ʹ�õ�����
#endif

// ���������ʹ��������ָ����ƽ̨֮ǰ��ƽ̨�����޸�����Ķ��塣
// �йز�ͬƽ̨����Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define WINVER 0x0501		// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0501	// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ���ض��� Windows 98 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINDOWS 0x0410 // ��������Ϊ�ʺ� Windows Me ����߰汾����Ӧֵ��
#endif

#ifndef _WIN32_IE			// ����ʹ���ض��� IE 6.0 ����߰汾�Ĺ��ܡ�
#define _WIN32_IE 0x0600	// ����ֵ����Ϊ��Ӧ��ֵ���������� IE �������汾��ֵ��
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����
#include <cv.h>
#include <highgui.h>
#include <cvaux.h>


#define  NEW_ROW    42//60     //�ı�ͼ��ĳߴ磨���ݲ��䣩
#define  NEW_COL    28//36

//#define  MAT_ROW    48
//#define  MAT_COLS   24
#define  HEIGHT    30// 48
#define  WIDTH     16// 24


#define  CHINESES_CHAR 1           //����ѡ�����ַ���
#define  LETTER_CHAR   2           //����ѡ����ĸ��ĸ��
#define  LETTER_NUM    3           //��ĸ����
#define  LETTER_NUM_CHINESE    4     //��ĸ�����֡�����
#define  FEATURE_BY_PRJ        0     //����ͶӰȡ����ֵ
#define  FEATURE_BY_GRID       1     //����������ȡ����ֵ
#define TOTAL_CHAR 7

struct Char_Letter 
{
	char*  char_name;      //��¼�ַ�
	/*
	int mark_max0; //��¼�ַ���� ,0Ϊ���1��֮���������ơ�
		int mark_max1;
		int mark_max2;
		int mark_max3;
		int mark_max4;
		int mark_max5;
		int mark_max6;*/
	int mark_max[10];
 
	/*
	double max_val0;
		double max_val1;
		double max_val2;
		double max_val3;
		double max_val4;
		double max_val5;
		double max_val6;*/
	double max_val[10];

};

struct charVector              //ͶӰ����
{
	int method;                   //0����ͶӰ����1Ϊ����
	double featureGrid[16];      //��ʹ������ȥ����ֵ��ʱ��ʹ��

	int row_count[HEIGHT];         //��ʹ��ͶӰ��ȥ������ʱ��ʹ��
	int col_count[WIDTH];

	double momentFeature[7];
};

struct Similary
{
	double simi[73];
	
};



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


