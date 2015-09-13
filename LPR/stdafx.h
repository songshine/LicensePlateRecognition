// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// 从 Windows 头中排除极少使用的资料
#endif

// 如果您必须使用下列所指定的平台之前的平台，则修改下面的定义。
// 有关不同平台的相应值的最新信息，请参考 MSDN。
#ifndef WINVER				// 允许使用特定于 Windows XP 或更高版本的功能。
#define WINVER 0x0501		// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT		// 允许使用特定于 Windows XP 或更高版本的功能。
#define _WIN32_WINNT 0x0501	// 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif						

#ifndef _WIN32_WINDOWS		// 允许使用特定于 Windows 98 或更高版本的功能。
#define _WIN32_WINDOWS 0x0410 // 将它更改为适合 Windows Me 或更高版本的相应值。
#endif

#ifndef _WIN32_IE			// 允许使用特定于 IE 6.0 或更高版本的功能。
#define _WIN32_IE 0x0600	// 将此值更改为相应的值，以适用于 IE 的其他版本。值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类
#include <cv.h>
#include <highgui.h>
#include <cvaux.h>


#define  NEW_ROW    42//60     //改变图像的尺寸（内容不变）
#define  NEW_COL    28//36

//#define  MAT_ROW    48
//#define  MAT_COLS   24
#define  HEIGHT    30// 48
#define  WIDTH     16// 24


#define  CHINESES_CHAR 1           //仅仅选择汉字字符库
#define  LETTER_CHAR   2           //仅仅选择字母字母库
#define  LETTER_NUM    3           //字母数字
#define  LETTER_NUM_CHINESE    4     //字母、数字、汉字
#define  FEATURE_BY_PRJ        0     //根据投影取特征值
#define  FEATURE_BY_GRID       1     //根据网格法提取特征值
#define TOTAL_CHAR 7

struct Char_Letter 
{
	char*  char_name;      //记录字符
	/*
	int mark_max0; //记录字符标记 ,0为最大，1次之，依此类推。
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

struct charVector              //投影向量
{
	int method;                   //0代表投影法，1为网格法
	double featureGrid[16];      //当使用网格法去特征值的时候使用

	int row_count[HEIGHT];         //当使用投影法去特征的时候使用
	int col_count[WIDTH];

	double momentFeature[7];
};

struct Similary
{
	double simi[73];
	
};



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC 对 Windows 公共控件的支持
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


