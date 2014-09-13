
#include "stdafx.h"
#include "CharRecognition.h"
///////////////////////////////////////////////////***New Version*****

#include "LPRDlg.h"
#include "LPHelper.h"

#include <fstream>
#include <iostream>
using namespace std;


CvMat *Tmat_64fc1=new CvMat[73];        //64FC1型矩阵保存模板数据
CvMat *Tmat_32fc1=new CvMat[73];         //32FC1型矩阵保存模板数据
double **Tdata=new double*;

charVector **chprjVector=new charVector*[73];     //保存字库特征值
charVector **chGridVector=new charVector*[73];     //保存字库特征值
charVector **chMomentVector=new charVector*[73];     //保存字库特征值

charVector **SrcchprjVector=new charVector*[7];     //保存被识别字符特征值
charVector **SrcchGridVector=new charVector*[7];     //保存被识别字符特征值
charVector **SrcchMomentVector=new charVector*[7];     //保存被识别字符特征值


CvMat *S_mat=new CvMat[7];
CvMat *T_mat=new CvMat[73];

//double** dataMultiTem;

//全局函数
void LoadMatConvert(CvMat *mat_64fc1,CvMat *mat_32fc1,double data[][24*48],CvSize szMat,int num)
//void LoadMatConvert(CvMat *mat_64fc1,CvMat *mat_32fc1,double data[][MAT_ROW*MAT_COLS],int num)
{

	for (int i=0;i<num;i++)
	{ 
		cvInitMatHeader(&mat_64fc1[i], szMat.height,szMat.width,CV_64FC1,data[i]);     //使用模板数据数组初始化64FC1矩阵
		cvInitMatHeader(&mat_32fc1[i],  szMat.height,szMat.width,CV_32FC1,data[i]);     //使用模板数据数组初始化32FC1矩阵

	}

	for (int i=0;i<num;i++)
	{
		cvConvert(&mat_64fc1[i],&mat_32fc1[i]);                              //将64FC1矩阵转化为32FC1，此处做转化的原因是，
		//在使用cvMatchTemplate中只支持8或32FC1型矩阵，
		//而将源数据导入64FC1矩阵中能较好的保存图像的真实度（即还原后接近原图）
	}

}


void LoadMatConvert1(CvMat *mat_64fc1,CvMat *mat_32fc1,double **data,CvSize szCharMat,int num)    // 方便从文件中读取数据用
{
	for (int i=0;i<num;i++)
	{ 
		cvInitMatHeader(&mat_64fc1[i], szCharMat.height,szCharMat.width,CV_64FC1,data[i]);     //使用模板数据数组初始化64FC1矩阵
		cvInitMatHeader(&mat_32fc1[i],szCharMat.height,szCharMat.width,CV_32FC1,data[i]);     //使用模板数据数组初始化32FC1矩阵

	}

	for (int i=0;i<num;i++)
	{
		cvConvert(&mat_64fc1[i],&mat_32fc1[i]);                              //将64FC1矩阵转化为32FC1，此处做转化的原因是，
		//在使用cvMatchTemplate中只支持8或32FC1型矩阵，
		//而将源数据导入64FC1矩阵中能较好的保存图像的真实度（即还原后接近原图）
	}
}

IplImage * t_Resize(IplImage *img,CvSize size)
{
	IplImage *dst=cvCreateImage(size,img->depth,img->nChannels);
	cvResize(img,dst,CV_INTER_LINEAR );                //将图像归一化为指定大小

	return dst;

}

/*************imgCvt2Mat32fc1*************
*szChar:字符大小
*
*szImg：转换成的图像尺寸，保持字符大小不变
*****************************************/
CvMat* imgCvt2Mat32fc1(IplImage *img,CvSize szChar,CvSize szImg)
{     
    CvMat *pMat=cvCreateMat(szImg.height,szImg.width,CV_32FC1); //创建矩阵头，待转化的图像
	IplImage *dst=PreProcessing(img,szChar,szImg);   //此处将图像预处理，包括图像的归一化

	cvConvert(dst,pMat);      //将二值图转化为矩阵

	cvReleaseImage(&dst);
	return pMat;
}

IplImage* PreProcessing(IplImage *img,CvSize szChar,CvSize szImg)
/*
*szChar:字符大小
*
*szImg：转换成的图像尺寸，保持字符大小不变*/
{
	//IplImage *dst_Resize1=t_Resize(img,cvSize(24,48));//将图像归一化

	//cvShowImage("de1",dst_Resize1);
	IplImage *dst_Resize=ReszNoChangeRotateImg(img,0,szChar,szImg);  //改变图像尺寸，但保持内容不变  

	//cvShowImage("dst_Resize",dst_Resize);
	//IplImage *dst=cvCreateImage(cvSize(12,24),IPL_DEPTH_8U,1);
	//cvResize(dst_Resize,dst);
	//cvSaveImage("2.jpg",dst);

	//CLPRDlg *pDlg=(CLPRDlg*)AfxGetMainWnd();

	//	cvCvtColor(dst_Resize,dst,CV_RGB2GRAY);     //将源图像进行灰度处理

    // cvShowImage("dst_Resize",dst_Resize);
	// IplImage* Tdst=cvCreateImage(cvGetSize(&Tmat_32fc1[13]),8,1);
	// cvGetImage(&Tmat_32fc1[13],Tdst);
	// cvShowImage("T",Tdst);
	//dst_Resize=GrayEqualize(dst_Resize);
 //  cvEqualizeHist(dst_Resize,dst_Resize);
	//dst_Resize=GrayEqualize(dst_Resize);


  
	//ImageStretchByHistogram(dst_Resize,dst_Resize);
	//UnsharpMask(dst_Resize,dst_Resize,255,3.1,0,255);

	//cvSmooth(dst_Resize,dst_Resize,CV_MEDIAN);      //进行四次的中值滤波，取出孤立的像素
	//cvSmooth(dst_Resize,dst_Resize,CV_MEDIAN);
	//cvSmooth(dst_Resize,dst_Resize,CV_MEDIAN);
	//cvSmooth(dst_Resize,dst_Resize,CV_MEDIAN);

	//cvThreshold(dst_Resize,dst_Resize,70,255,CV_THRESH_BINARY);     //二值化

	//cvShowImage("T",dst_Resize);
	
	//	
	//PixelMeanThreshold(dst_Resize,dst_Resize,pDlg->m_nSliderReco,4);
	//GreyValueCollection(dst_Resize,"1.dll");
	//GreyValueCollection(dst_Resize,0);
		//cvShowImage("dst_Resize",dst_Resize);
	//	cvWaitKey(0);
	return dst_Resize;
}


/***********************************************
*
* 函数名称：
*   GradientSharp() 
*
*参数 ：
*  HDIB hDIB    -待处理图像的句柄
*
* 返回值：
*       无
* 
*功能：
*    现图像的梯度锐化

*说明：
*    只能对2值图像进行处理，如果图像本身边缘较细，可能造成信息的损失
**********************************************************************/
IplImage* GradientSharp(IplImage *src)
{
	
	IplImage* dst=cvCreateImage(cvGetSize(src),src->depth,src->nChannels);

	dst=cvCloneImage(src);
	int width=dst->width;//获取图像的宽度
	int height=dst->height;//获取图像的高度

	// 阈值
	uchar	bThre = 5;

	// 调用GradSharp()函数进行梯度板锐化

	// 指向源图像的指针
	uchar*	lpSrc;
	uchar*	lpSrc1;
	uchar*	lpSrc2;

	// 循环变量
	LONG	i;
	LONG	j;

	// 图像每行的字节数
	int	step=dst->widthStep;

	// 中间变量
	//BYTE	bTemp;
	uchar	bTemp;
	// 计算图像每行的字节数
	//	lLineBytes = WIDTHBYTES(lWidth * 8);

	// 每行
	for(i = 0; i < height; i++)
	{

		// 每列
		for(j = 0; j < width; j++)
		{

			// 指向DIB第i行，第j个象素的指针
			lpSrc  = (uchar*)dst->imageData + step * (height - 1 - i) + j;

			// 指向DIB第i+1行，第j个象素的指针
			lpSrc1 = (uchar*)dst->imageData + step * (height - 2 - i) + j;

			// 指向DIB第i行，第j+1个象素的指针
			lpSrc2 = (uchar*)dst->imageData + step * (height - 1 - i) + j + 1;


			//计算梯度值
			bTemp = abs((*lpSrc)-(*lpSrc1)) + abs((*lpSrc)-(*lpSrc2));

			// 判断是否小于阈值
			if (bTemp < 255)
			{  

				// 判断是否大于阈值，对于小于情况，灰度值不变。
				if (bTemp >= bThre)
				{

					// 直接赋值为bTemp
					*lpSrc = bTemp;

				}

			}

			else
			{
				// 直接赋值为255
				*lpSrc = 255;
			}
		}
	}

	//最后还要处理一下图像中最下面那行
	for(j = 0; j < width; j++)
	{   

		//指向最下边一行，第j个象素的指针
		lpSrc  = (uchar*)dst->imageData + step * 0 + j;

		//将此位置的象素设置为255，即白点
		*lpSrc=255;
	}

	return dst;
}

/*************************************************************************
* 函数名称：GrayEqualize(IplImage* src)
* 函数参数:
long	Width，DIB图象的宽度
long	Height，DIB图象的高度
* 函数类型:IplImage*        
* 函数功能:该函数用来进行直方图均衡
************************************************************************/

IplImage*  GrayEqualize(IplImage* src)
{	
	IplImage *dst=cvCloneImage(src);

	long i;                 //行循环变量
	long j;                 //列循环变量
	uchar* data=NULL;

	long height=dst->height;
	long width=dst->width;
	long step=dst->widthStep;

	BYTE	bGrayMap[256];// 灰度映射		
	long	lGrayNum[256];// 灰度映射		
	for (i = 0; i < 256; i ++)// 置0
	{		
		lGrayNum[i] = 0;
	}		
	for (i = 0; i < height; i ++)// 各灰度值计数
	{
		for (j = 0; j < width; j ++)
		{
			//lpSrcUnChr=(unsigned char*)lpSrcStartBits + lLineBytes * i + j;		
			data=(uchar*)dst->imageData+i*step+j;
			lGrayNum[*(data)]++;// 加1
		}
	}		
	for (i = 0; i < 256; i++)// 计算灰度映射表
	{		
		long    varLong;		//临时变量
		varLong = 0;// 初始为0		
		for (j = 0; j <= i ; j++)
		{
			varLong += lGrayNum[j];
		}				
		bGrayMap[i] = (BYTE) (varLong * 255 / height / width);// 计算对应的新灰度值
	}		
	for(i = 0; i < height; i++)// 行
	{		
		for(j = 0; j < width; j++)// 列
		{
			// 指向DIB第i行，第j个像素的指针(unsigned char*)
			data=(uchar*)dst->imageData+step*(height-1-i)+j;
			*data=bGrayMap[*data];// 计算新的灰度值
		}
	}
	return dst;

}


char* ReturnChar(int mark)
{
	char *charac=new char[4];
	switch (mark)
	{
	case 0:charac="京";
		break;
	case 1:charac="津";
		break;
	case 2:charac="冀";
		break;
	case 3:charac="晋";
		break;
	case 4:charac="蒙";
		break;
	case 5:charac="辽";
		break;
	case 6:charac="吉";
		break;
	case 7:charac="黑";
		break;
	case 8:charac="沪";
		break;
	case 9:charac="苏";
		break;
	case 10:charac="浙";
		break;
	case 11:charac="皖";
		break;
	case 12:charac="闽";
		break;
	case 13:charac="赣";
		break;
	case 14:charac="鲁";
		break;
	case 15:charac="豫";
		break;
	case 16:charac="鄂";
		break;
	case 17:charac="湘";
		break;
	case 18:charac="粤";
		break;
	case 19:charac="桂";
		break;
	case 20:charac="琼";
		break;
	case 21:charac="渝";
		break;
	case 22:charac="川";
		break;
	case 23:charac="贵";
		break;
	case 24:charac="云";
		break;
	case 25:charac="藏";
		break;
	case 26:charac="陕";
		break;
	case 27:charac="甘";
		break;
	case 28:charac="青";
		break;
	case 29:charac="宁";
		break;
	case 30:charac="新";
		break;
	case 31:charac="A";
		break;
	case 32:charac="B";
		break;
	case 33:charac="C";
		break;
	case 34:charac="D";
		break;
	case 35:charac="E";
		break;
	case 36:charac="F";
		break;
	case 37:charac="G";
		break;
	case 38:charac="H";
		break;
	case 39:charac="I";
		break;
	case 40:charac="J";
		break;
	case 41:charac="K";
		break;
	case 42:charac="L";
		break;
	case 43:charac="M";
		break;
	case 44:charac="N";
		break;
	case 45:charac="O";
		break;
	case 46:charac="P";
		break;
	case 47:charac="Q";
		break;
	case 48:charac="R";
		break;
	case 49:charac="S";
		break;
	case 50:charac="T";
		break;
	case 51:charac="U";
		break;
	case 52:charac="V";
		break;
	case 53:charac="W";
		break;
	case 54:charac="X";
		break;
	case 55:charac="Y";
		break;
	case 56:charac="Z";
		break;
	case 57:charac="1";
		break;
	case 58:charac="2";
		break;
	case 59:charac="3";
		break;
	case 60:charac="4";
		break;
	case 61:charac="5";
		break;
	case 62:charac="6";
		break;
	case 63:charac="7";
		break;
	case 64:charac="8";
		break;
	case 65:charac="9";
		break;
	case 66:charac="0";
		break;
	case 67:charac="港";
		break;
	case 68:charac="澳";
		break;
	case 69:charac="使";
		break;
	case 70:charac="领";
		break;
	case 71:charac="学";
		break;
	case 72:charac="警";
		break;

	}
	return charac;
}

char* ReturnChar2(int mark)
{
	char *charac=new char[4];
	switch (mark)
	{
	case 0:charac="京";
		break;
	case 1:charac="津";
		break;
	case 2:charac="冀";
		break;
	case 3:charac="晋";
		break;
	case 4:charac="蒙";
		break;
	case 5:charac="辽";
		break;
	case 6:charac="吉";
		break;
	case 7:charac="黑";
		break;
	case 8:charac="沪";
		break;
	case 9:charac="苏";
		break;
	case 10:charac="浙";
		break;
	case 11:charac="皖";
		break;
	case 12:charac="闽";
		break;
	case 13:charac="赣";
		break;
	case 14:charac="鲁";
		break;
	case 15:charac="豫";
		break;
	case 16:charac="鄂";
		break;
	case 17:charac="湘";
		break;
	case 18:charac="粤";
		break;
	case 19:charac="桂";
		break;
	case 20:charac="琼";
		break;
	case 21:charac="渝";
		break;
	case 22:charac="川";
		break;
	case 23:charac="贵";
		break;
	case 24:charac="云";
		break;
	case 25:charac="藏";
		break;
	case 26:charac="陕";
		break;
	case 27:charac="甘";
		break;
	case 28:charac="青";
		break;
	case 29:charac="宁";
		break;
	case 30:charac="新";
		break;
	case 31:charac="A";
		break;
	case 32:charac="B";
		break;
	case 33:charac="C";
		break;
	case 34:charac="D";
		break;
	case 35:charac="E";
		break;
	case 36:charac="F";
		break;
	case 37:charac="G";
		break;
	case 38:charac="H";
		break;
	case 39:charac="1";
		break;
	case 40:charac="J";
		break;
	case 41:charac="K";
		break;
	case 42:charac="L";
		break;
	case 43:charac="M";
		break;
	case 44:charac="N";
		break;
	case 45:charac="0";
		break;
	case 46:charac="P";
		break;
	case 47:charac="Q";
		break;
	case 48:charac="R";
		break;
	case 49:charac="S";
		break;
	case 50:charac="T";
		break;
	case 51:charac="U";
		break;
	case 52:charac="V";
		break;
	case 53:charac="W";
		break;
	case 54:charac="X";
		break;
	case 55:charac="Y";
		break;
	case 56:charac="Z";
		break;
	case 57:charac="1";
		break;
	case 58:charac="2";
		break;
	case 59:charac="3";
		break;
	case 60:charac="4";
		break;
	case 61:charac="5";
		break;
	case 62:charac="6";
		break;
	case 63:charac="7";
		break;
	case 64:charac="8";
		break;
	case 65:charac="9";
		break;
	case 66:charac="0";
		break;
	case 67:charac="港";
		break;
	case 68:charac="澳";
		break;
	case 69:charac="使";
		break;
	case 70:charac="领";
		break;
	case 71:charac="学";
		break;
	case 72:charac="警";
		break;


	}
	return charac;
}


int ReturnCharIndex(CString strName)
{
	int CharIndex;
	if (strName=="京")
	{
       CharIndex=0;
	} 
	else if(strName=="津")
	{
		CharIndex=1;
	}
	else if(strName=="冀")
	{
		CharIndex=2;
	}
	else if(strName=="晋")
	{
		CharIndex=3;
	}
	else if(strName=="蒙")
	{
		CharIndex=4;
	}
	else if(strName=="辽")
	{
		CharIndex=5;
	}
	else if(strName=="吉")
	{
		CharIndex=6;
	}
	else if(strName=="黑")
	{
		CharIndex=7;
	}
	else if(strName=="沪")
	{
		CharIndex=8;
	}
	else if(strName=="苏")
	{
		CharIndex=9;
	}
	else if(strName=="浙")
	{
		CharIndex=10;
	}
	else if(strName=="皖")
	{
		CharIndex=11;
	}
	else if(strName=="闽")
	{
		CharIndex=12;
	}
	else if(strName=="赣")
	{
		CharIndex=13;
	}
	else if(strName=="鲁")
	{
		CharIndex=14;
	}
	else if(strName=="豫")
	{
		CharIndex=15;
	}
	else if(strName=="鄂")
	{
		CharIndex=16;
	}
	else if(strName=="湘")
	{
		CharIndex=17;
	}
	else if(strName=="粤")
	{
		CharIndex=18;
	}
	else if(strName=="桂")
	{
		CharIndex=19;
	}
	else if(strName=="琼")
	{
		CharIndex=20;
	}
	else if(strName=="渝")
	{
		CharIndex=21;
	}
	else if(strName=="川")
	{
		CharIndex=22;
	}
	else if(strName=="贵")
	{
		CharIndex=23;
	}
	else if(strName=="云")
	{
		CharIndex=24;
	}
	else if(strName=="藏")
	{
		CharIndex=25;
	}
	else if(strName=="陕")
	{
		CharIndex=26;
	}
	else if(strName=="甘")
	{
		CharIndex=27;
	}
	else if(strName=="青")
	{
		CharIndex=28;
	}
	else if(strName=="宁")
	{
		CharIndex=29;
	}
	else if(strName=="新")
	{
		CharIndex=30;
	}
	
	else if(strName=="A")
	{
		CharIndex=31;
	}
	else if(strName=="B")
	{
		CharIndex=32;
	}
	else if(strName=="C")
	{
		CharIndex=33;
	}
	else if(strName=="D")
	{
		CharIndex=34;
	}
	else if(strName=="E")
	{
		CharIndex=35;
	}
	else if(strName=="F")
	{
		CharIndex=36;
	}
	else if(strName=="G")
	{
		CharIndex=37;
	}
	else if(strName=="H")
	{
		CharIndex=38;
	}
	else if(strName=="I")
	{
		CharIndex=39;
	}
	else if(strName=="J")
	{
		CharIndex=40;
	}
	else if(strName=="K")
	{
		CharIndex=41;
	}
	else if(strName=="L")
	{
		CharIndex=42;
	}
	else if(strName=="M")
	{
		CharIndex=43;
	}
	else if(strName=="N")
	{
		CharIndex=44;
	}
	else if(strName=="O")
	{
		CharIndex=45;
	}
	else if(strName=="P")
	{
		CharIndex=46;
	}
	else if(strName=="Q")
	{
		CharIndex=47;
	}
	else if(strName=="R")
	{
		CharIndex=48;
	}
	else if(strName=="S")
	{
		CharIndex=49;
	}
	else if(strName=="T")
	{
		CharIndex=50;
	}
	else if(strName=="U")
	{
		CharIndex=51;
	}
	else if(strName=="V")
	{
		CharIndex=52;
	}
	else if(strName=="W")
	{
		CharIndex=53;
	}
	else if(strName=="X")
	{
		CharIndex=54;
	}
	else if(strName=="Y")
	{
		CharIndex=55;
	}
	else if(strName=="Z")
	{
		CharIndex=56;
	}
	else if(strName=="1")
	{
		CharIndex=57;
	}
	else if(strName=="2")
	{
		CharIndex=58;
	}
	else if(strName=="3")
	{
		CharIndex=59;
	}
	else if(strName=="4")
	{
		CharIndex=60;
	}
	else if(strName=="5")
	{
		CharIndex=61;
	}
	else if(strName=="6")
	{
		CharIndex=62;
	}
	else if(strName=="7")
	{
		CharIndex=63;
	}
	else if(strName=="8")
	{
		CharIndex=64;
	}
	else if(strName=="9")
	{
		CharIndex=65;
	}
	else if(strName=="0")
	{
		CharIndex=66;
	}
	else if(strName=="港")
	{
		CharIndex=67;
	}
	else if(strName=="澳")
	{
		CharIndex=68;
	}
	else if(strName=="使")
	{
		CharIndex=69;
	}
	else if(strName=="领")
	{
		CharIndex=70;
	}
	else if(strName=="学")
	{
		CharIndex=71;
	}
	else if(strName=="警")
	{
		CharIndex=72;
	}

   else {
	   AfxMessageBox(_T("输入字符不在字符库内!"));
	   return -1;
   }

return CharIndex;

}




/***************函数名IsInSub*********************
*作用：检测当前车牌的第二个字符是否在相应省市的发牌机关的集合内
*参数说明：
* mark:当前识别出的字符的标记
*data:该数组为相应省市的发牌机关的集合
*num：发牌机关的数目
*************************************************/
BOOL IsInSub(int mark,int *data,int num)
{
	for (int i=0;i<num;i++)
	{
		if (mark==data[i])
		{
	//		break;
			return TRUE;

		}
	}
	return FALSE;
}




/***************函数名CharacterRecognition*********************
*作用：字符识别
*参数说明：
* img_chars:输入要识别的字符灰度图
*type:指明当前输入字符的类型：
*CHINESES_CHAR   汉字字符
*LETTER_CHAR     英文字母
*LETTER_NUM      字母和数字，及汉字 港、澳、使、领、学、警
*************************************************/
Char_Letter* CharacterRecognition(IplImage* img_chars,int type)      //字符识别，返回车牌号字符串
{
	Char_Letter *S_char=new Char_Letter;
	S_char->char_name="";
	for (int i=0;i<7;i++)
	{
		S_char->mark_max[i]=0;
		S_char->max_val[i]=0;
	}

	CvMat *I_Mat=imgCvt2Mat32fc1(img_chars,cvSize(WIDTH,HEIGHT),cvSize(NEW_COL,NEW_ROW));      //将图像转换成CV_32FC1型的矩阵

	//被识别图像尺寸
    int W=NEW_COL;
	int H=NEW_ROW; 
	//模板字库尺寸
	int w=WIDTH;     
	int h=HEIGHT;

	IplImage *img_Re=cvCreateImage(cvSize(W-w+1,H-h+1),IPL_DEPTH_32F,1);    //由于已将源图像归一化为模板大小，故cvSize(W-w+1,H-h+1)=cvSize(1,1)

	double minval,maxval;  //图像比较的最大值、最小值

	switch(type)
	{
	case CHINESES_CHAR:
		{
			double Similarity[31]={0};           //将于保存与模板比较的相似度值       

			for (int i=0;i<30;i++)
			{
				/*CV_TM_SQDIFF 平方差匹配法：该方法采用平方差来进行匹配；最好的匹配值为0；匹配越差，匹配值越大。    
				CV_TM_CCORR 相关匹配法：该方法采用乘法操作；数值越大表明匹配程度越好。    
				CV_TM_CCOEFF 相关系数匹配法：1表示完美的匹配；-1表示最差的匹配。    
				CV_TM_SQDIFF_NORMED 归一化平方差匹配法    
				CV_TM_CCORR_NORMED 归一化相关匹配法    
				CV_TM_CCOEFF_NORMED 归一化相关系数匹配法*/

				cvMatchTemplate(I_Mat,&Tmat_32fc1[i],img_Re,CV_TM_CCOEFF_NORMED);     //使用模板匹配，这里采用归一化相关匹配法
				cvMinMaxLoc(img_Re,&minval,&maxval);       //找出相似的最大、最小值，此处有意义的为最大值

				Similarity[i]=maxval;                      //将最大相似值保存在数组中

				maxval=0.0;
				minval=0.0;
				cvZero(img_Re);                            //重置矩阵
			}

			double *MaxVal=GetMaxValFrmArray(Similarity,31,7);      //取最大值
			int MarkVal[7];
			for (int i=0;i<7;i++)
			{
				MarkVal[i]=GetMarkFrmArray(Similarity,31,MaxVal[i]);      //取最大值对应的下标
			}

		
			for ( int i=0;i<7;i++)                                     //给结构体赋值
			{
				S_char->max_val[i]=MaxVal[i];         
				S_char->mark_max[i]=MarkVal[i];
			}

           S_char->char_name=ReturnChar(MarkVal[0]);                //根据索引值返回识别字符
			

		    delete [] MaxVal;
			cvReleaseImage(&img_Re);
			cvReleaseMat(&I_Mat);
			return S_char;

		}break;
	case LETTER_CHAR:
		{
			double Similarity[26]={0};           //将于保存与模板比较的相似度值       

			for (int i=31;i<57;i++)
			{
				/*CV_TM_SQDIFF 平方差匹配法：该方法采用平方差来进行匹配；最好的匹配值为0；匹配越差，匹配值越大。    
				CV_TM_CCORR 相关匹配法：该方法采用乘法操作；数值越大表明匹配程度越好。    
				CV_TM_CCOEFF 相关系数匹配法：1表示完美的匹配；-1表示最差的匹配。    
				CV_TM_SQDIFF_NORMED 归一化平方差匹配法    
				CV_TM_CCORR_NORMED 归一化相关匹配法    
				CV_TM_CCOEFF_NORMED 归一化相关系数匹配法*/

				cvMatchTemplate(I_Mat,&Tmat_32fc1[i],img_Re,CV_TM_CCORR_NORMED);     //使用模板匹配，这里采用归一化相关系数匹配法
				cvMinMaxLoc(img_Re,&minval,&maxval);       //找出相似的最大、最小值，此处有意义的为最大值

				Similarity[i-31]=maxval;                      //将最大相似值保存在数组中

				maxval=0.0;
				minval=0.0;
				cvZero(img_Re);                            //重置矩阵
			}


			double *MaxVal=GetMaxValFrmArray(Similarity,26,7);
			int MarkVal[7];
			for (int i=0;i<7;i++)
			{
				MarkVal[i]=GetMarkFrmArray(Similarity,26,MaxVal[i]);
			}

			for ( int i=0;i<7;i++)
			{
				S_char->max_val[i]=MaxVal[i];
				S_char->mark_max[i]=MarkVal[i]+31;
			}
            S_char->char_name=ReturnChar(MarkVal[0]+31);
		

			delete [] MaxVal;
			cvReleaseImage(&img_Re);
			cvReleaseMat(&I_Mat);
			return S_char;
		}break;
	case LETTER_NUM:
		{
			double Similarity[36]={0};           //将于保存与模板比较的相似度值       

			for (int i=31;i<67;i++)
			{
				/*CV_TM_SQDIFF 平方差匹配法：该方法采用平方差来进行匹配；最好的匹配值为0；匹配越差，匹配值越大。    
				CV_TM_CCORR 相关匹配法：该方法采用乘法操作；数值越大表明匹配程度越好。    
				CV_TM_CCOEFF 相关系数匹配法：1表示完美的匹配；-1表示最差的匹配。    
				CV_TM_SQDIFF_NORMED 归一化平方差匹配法    
				CV_TM_CCORR_NORMED 归一化相关匹配法    
				CV_TM_CCOEFF_NORMED 归一化相关系数匹配法*/

				cvMatchTemplate(I_Mat,&Tmat_32fc1[i],img_Re,CV_TM_CCORR_NORMED);     //使用模板匹配，这里采用归一化相关匹配法
				cvMinMaxLoc(img_Re,&minval,&maxval);       //找出相似的最大、最小值，此处有意义的为最大值

				Similarity[i-31]=maxval;                      //将最大相似值保存在数组中

				maxval=0.0;
				minval=0.0;
				cvZero(img_Re);                            //重置矩阵
			}

			double *MaxVal=GetMaxValFrmArray(Similarity,36,7);
			int MarkVal[7];
			for (int i=0;i<7;i++)
			{
				MarkVal[i]=GetMarkFrmArray(Similarity,36,MaxVal[i]);
			}

       for ( int i=0;i<7;i++)
       {
          S_char->max_val[i]=MaxVal[i];
		  S_char->mark_max[i]=MarkVal[i]+31;
       }

          S_char->char_name=ReturnChar2(MarkVal[0]+31);
			

            delete [] MaxVal;
			cvReleaseImage(&img_Re);
			cvReleaseMat(&I_Mat);
			return S_char;
		}break;

	case LETTER_NUM_CHINESE:
		{
			double Similarity[42]={0};           //将于保存与模板比较的相似度值       

			for (int i=31;i<73;i++)
			{
				/*CV_TM_SQDIFF 平方差匹配法：该方法采用平方差来进行匹配；最好的匹配值为0；匹配越差，匹配值越大。    
				CV_TM_CCORR 相关匹配法：该方法采用乘法操作；数值越大表明匹配程度越好。    
				CV_TM_CCOEFF 相关系数匹配法：1表示完美的匹配；-1表示最差的匹配。    
				CV_TM_SQDIFF_NORMED 归一化平方差匹配法    
				CV_TM_CCORR_NORMED 归一化相关匹配法    
				CV_TM_CCOEFF_NORMED 归一化相关系数匹配法*/

				cvMatchTemplate(I_Mat,&Tmat_32fc1[i],img_Re,CV_TM_CCORR_NORMED);     //使用模板匹配，这里采用归一化相关匹配法
				cvMinMaxLoc(img_Re,&minval,&maxval);       //找出相似的最大、最小值，此处有意义的为最大值

				Similarity[i-31]=maxval;                      //将最大相似值保存在数组中

				maxval=0.0;
				minval=0.0;
				cvZero(img_Re);                            //重置矩阵
			}

			double *MaxVal=GetMaxValFrmArray(Similarity,42,7);
			int MarkVal[7];
			for (int i=0;i<7;i++)
			{
				MarkVal[i]=GetMarkFrmArray(Similarity,42,MaxVal[i]);
			}

			
			for ( int i=0;i<7;i++)
			{
				S_char->max_val[i]=MaxVal[i];
				S_char->mark_max[i]=MarkVal[i]+31;
			}
			

            S_char->char_name=ReturnChar2(MarkVal[0]+31);
			

			delete [] MaxVal;
			cvReleaseImage(&img_Re);
			cvReleaseMat(&I_Mat);
			return S_char;
		}break;
	}

}


/********************使用多级模板识别*******************/
void ChRecgUseMultiTemplate(IplImage* img_chars,Char_Letter* ch,bool UseReturnChar2)
{
	
		//CvMat *matSrc=imgCvt2Mat32fc1(img_chars);
		
		//获取待选字符路径

	CString *strImgfp=new CString [7];
	int *numChar=new int [7];
	double*** data=new double**[7];
	CvMat **mat_64fc1=new CvMat*[7];
	CvMat **mat_32fc1=new CvMat*[7];
   
	 double maxval[7];




	for (int i=0;i<7;i++)
	{
		strImgfp[i]=GetPathByIndexChar(ch->mark_max[i]);   //获取文件路径

		numChar[i]=ScanCharNumFrmFile(strImgfp[i],CHAR_HEIGHT);//获取文件中图片数目

		numChar[i]>0?numChar[i]:numChar[i]=1;           //如果图像为0则取值为1

		data[i]=ReadFromFile(strImgfp[i],cvSize(WIDTH,HEIGHT),numChar[i]);    //从文件导入数据

		mat_64fc1[i]=new CvMat[numChar[i]];              //创建矩阵
		mat_32fc1[i]=new CvMat[numChar[i]];

		LoadMatConvert1(mat_64fc1[i],mat_32fc1[i],data[i],cvSize(WIDTH,HEIGHT),numChar[i]);     //将64矩阵转换为32矩阵

		maxval[i]= GetMaxSimiByMultiTemplate(img_chars,mat_32fc1[i],numChar[i]);         //取最大相似值对应的字符
	}

	double  *maxV=GetMaxValFrmArray(maxval,7,1);         //取得文件中图像相似度值的最大值，并判断属于ch中的哪一组
	if (maxV[0]!=0)
	{
		for (int i=0;i<7;i++) 
		{
			if (maxV[0]==maxval[i])
			{
				if (UseReturnChar2)
				{
					ch->char_name=ReturnChar2(ch->mark_max[i]);
				} 
				else
				{
					ch->char_name=ReturnChar(ch->mark_max[i]);
				}
			}
		}
	
	}


	delete [] maxV;
	delete [] strImgfp;
    delete [] numChar;

	for (int i=0;i<7;i++)
	{
		delete [] mat_64fc1[i];
		delete [] mat_32fc1[i];
		for (int j=0;j<numChar[i];j++)
		{
			delete [] data[i][j];
		}

		delete [] data[i];
	}

	delete [] data;
		
	
}

double GetMaxSimiByMultiTemplate(IplImage* img_char,CvMat *mat32fc1,int numChar)
{
	//img_char=ReszNoChangeRotateImg(img_char,0,cvSize(NEW_COL,NEW_ROW));
	CvMat *I_Mat=imgCvt2Mat32fc1(img_char,cvSize(WIDTH,HEIGHT),cvSize(NEW_COL,NEW_ROW));      //将图像转换成CV_32FC1型的矩阵

	//被识别图像尺寸
	int W=NEW_COL;
	int H=NEW_ROW; 
	//模板字库尺寸
	int w=WIDTH;     
	int h=HEIGHT;
	

	IplImage *img_Re=cvCreateImage(cvSize(W-w+1,H-h+1),IPL_DEPTH_32F,1);    //由于已将源图像归一化为模板大小，故cvSize(W-w+1,H-h+1)=cvSize(1,1)

	double minval,maxval;  //图像比较的最大值、最小值


	double *Simil=new double[numChar];           //将于保存与模板比较的相似度值       
    double max_simi=0.0;

	for (int i=0;i<numChar;i++)
	{
		/*CV_TM_SQDIFF 平方差匹配法：该方法采用平方差来进行匹配；最好的匹配值为0；匹配越差，匹配值越大。    
		CV_TM_CCORR 相关匹配法：该方法采用乘法操作；数值越大表明匹配程度越好。    
		CV_TM_CCOEFF 相关系数匹配法：1表示完美的匹配；-1表示最差的匹配。    
		CV_TM_SQDIFF_NORMED 归一化平方差匹配法    
		CV_TM_CCORR_NORMED 归一化相关匹配法    
		CV_TM_CCOEFF_NORMED 归一化相关系数匹配法*/

		cvMatchTemplate(I_Mat,&mat32fc1[i],img_Re,CV_TM_CCORR_NORMED);     //使用模板匹配，这里采用归一化相关匹配法
		cvMinMaxLoc(img_Re,&minval,&maxval);       //找出相似的最大、最小值，此处有意义的为最大值

		Simil[i]=maxval;                      //将最大相似值保存在数组中

		maxval=0.0;
		minval=0.0;
		cvZero(img_Re);                            //重置矩
	}

	for (int i=0;i<numChar;i++)
	{
		if (max_simi<Simil[i])              
		{
			max_simi=Simil[i];               //取出Similarity数组中的最大值
		}
	}

	delete [] Simil;
	cvReleaseImage(&img_Re);
	cvReleaseMat(&I_Mat);
	return max_simi;

}

int ImageStretchByHistogram(IplImage *src1,IplImage *dst1)
/*************************************************
Function:      通过直方图变换进行图像增强，将图像灰度的域值拉伸到0-255
src1:               单通道灰度图像                  
dst1:              同样大小的单通道灰度图像 
*************************************************/
{
	assert(src1->width==dst1->width);
	double p[256],p1[256],num[256];

	memset(p,0,sizeof(p));
	memset(p1,0,sizeof(p1));
	memset(num,0,sizeof(num));
	int height=src1->height;
	int width=src1->width;
	long wMulh = height * width;

	//statistics
	for(int x=0;x<src1->width;x++)
	{
		for(int y=0;y<src1-> height;y++){
			uchar v=((uchar*)(src1->imageData + src1->widthStep*y))[x];
			num[v]++;
		}
	}
	//calculate probability
	for(int i=0;i<256;i++)
	{
		p[i]=num[i]/wMulh;
	}

	//p1[i]=sum(p[j]);	j<=i;
	for(int i=0;i<256;i++)
	{
		for(int k=0;k<=i;k++)
			p1[i]+=p[k];
	}

	// histogram transformation
	for(int x=0;x<src1->width;x++)
	{
		for(int y=0;y<src1-> height;y++){
			uchar v=((uchar*)(src1->imageData + src1->widthStep*y))[x];
			((uchar*)(dst1->imageData + dst1->widthStep*y))[x]= p1[v]*255+0.5;            
		}
	}
	return 0;
}


//旋转图像内容不变，尺寸相应变大
IplImage* rotateImage1(IplImage* img,int degree){
	double angle = degree  * CV_PI / 180.; // 弧度  
	double a = sin(angle), b = cos(angle); 
	int width = img->width;  
	int height = img->height;  
	int width_rotate= int(height * fabs(a) + width * fabs(b));  
	int height_rotate=int(width * fabs(a) + height * fabs(b));  
	//旋转数组map
	// [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]
	// [ m3  m4  m5 ] ===>  [ A21  A22   b2 ]
	float map[6];
	CvMat map_matrix = cvMat(2, 3, CV_32F, map);  
	// 旋转中心
	CvPoint2D32f center = cvPoint2D32f(width / 2, height / 2);  
	cv2DRotationMatrix(center, degree, 1.0, &map_matrix);  
	map[2] += (width_rotate - width) / 2;  
	map[5] += (height_rotate - height) / 2;  
	IplImage* img_rotate = cvCreateImage(cvSize(width_rotate, height_rotate), 8, 3); 
	//对图像做仿射变换
	//CV_WARP_FILL_OUTLIERS - 填充所有输出图像的象素。
	//如果部分象素落在输入图像的边界外，那么它们的值设定为 fillval.
	//CV_WARP_INVERSE_MAP - 指定 map_matrix 是输出图像到输入图像的反变换，
	cvWarpAffine( img,img_rotate, &map_matrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, cvScalarAll(0));  
	return img_rotate;
}

void GreyValueCollection(IplImage* pImg,char* fname)
{
	//IplImage* pImg;
	//pImg = cvLoadImage( "F:\\OpenCV\\车牌识别_字符\\binary chars24X48\\38.jpg", CV_LOAD_IMAGE_GRAYSCALE );
	FILE *fp;
	//char temp[4]=NULL;
	//"character_DB.dat"
	fp=fopen(fname,"a");

	char* tem=new char[4];

	if(fp==NULL)
	{
		printf("错误!");
		exit(1);
	}

	int row,col;
	//cvNamedWindow( "Image", CV_WINDOW_AUTOSIZE ); 
	//cvShowImage( "Image", pImg ); 
	int count=0;

	for(row=0;row<pImg->height;row++)
	{
		uchar* ptr=(uchar*)(pImg->imageData+row*pImg->widthStep);
		//printf("the %d row : ",row);
		for(col=0;col<pImg->width;col++)
		{
			
			sprintf(tem,"%4d",ptr[col]);    //%4d 为占位符  4个占位符
			fputs(tem,fp);

		}
		//	printf("\n");
		count++;
		fputs("\n",fp);
	}
	//fputs("\n",fp);
	fclose(fp);
	//cvWaitKey(0); //等待按键
	cvDestroyWindow( "Image" );//销毁窗口

}

CString GetPathByIndexChar(int index)
{
	CString str;
	str.Format("%d",index);

	CString strfp="./Data/"+str+".Model";
	return strfp;
}



void GreyValueCollection(IplImage* pImg,int CharIndex)
{
	
	FILE *fp;
	char* fname=NULL;

	switch (CharIndex)
	{
	case 0:fname="./Data/0.Model";
		break;
	case 1:fname="./Data/1.Model";
		break;
	case 2:fname="./Data/2.Model";
		break;
	case 3:fname="./Data/3.Model";
		break;
	case 4:fname="./Data/4.Model";
		break;
	case 5:fname="./Data/5.Model";
		break;
	case 6:fname="./Data/6.Model";
		break;
	case 7:fname="./Data/7.Model";
		break;
	case 8:fname="./Data/8.Model";
		break;
	case 9:fname="./Data/9.Model";
		break;
	case 10:fname="./Data/10.Model";
		break;
	case 11:fname="./Data/11.Model";
		break;
	case 12:fname="./Data/12.Model";
		break;
	case 13:fname="./Data/13.Model";
		break;
	case 14:fname="./Data/14.Model";
		break;
	case 15:fname="./Data/15.Model";
		break;
	case 16:fname="./Data/16.Model";
		break;
	case 17:fname="./Data/17.Model";
		break;
	case 18:fname="./Data/18.Model";
		break;
	case 19:fname="./Data/19.Model";
		break;
	case 20:fname="./Data/20.Model";
		break;
	case 21:fname="./Data/21.Model";
		break;
	case 22:fname="./Data/22.Model";
		break;
	case 23:fname="./Data/23.Model";
		break;
	case 24:fname="./Data/24.Model";
		break;
	case 25:fname="./Data/25.Model";
		break;
	case 26:fname="./Data/26.Model";
		break;
	case 27:fname="./Data/27.Model";
		break;
	case 28:fname="./Data/28.Model";
		break;
	case 29:fname="./Data/29.Model";
		break;
	case 30:fname="./Data/30.Model";
		break;
	case 31:fname="./Data/31.Model";
		break;
	case 32:fname="./Data/32.Model";
		break;
	case 33:fname="./Data/33.Model";
		break;
	case 34:fname="./Data/34.Model";
		break;
	case 35:fname="./Data/35.Model";
		break;
	case 36:fname="./Data/36.Model";
		break;
	case 37:fname="./Data/37.Model";
		break;
	case 38:fname="./Data/38.Model";
		break;
	case 39:fname="./Data/39.Model";
		break;
	case 40:fname="./Data/40.Model";
		break;
	case 41:fname="./Data/41.Model";
		break;
	case 42:fname="./Data/42.Model";
		break;
	case 43:fname="./Data/43.Model";
		break;
	case 44:fname="./Data/44.Model";
		break;
	case 45:fname="./Data/45.Model";
		break;
	case 46:fname="./Data/46.Model";
		break;
	case 47:fname="./Data/47.Model";
		break;
	case 48:fname="./Data/48.Model";
		break;
	case 49:fname="./Data/49.Model";
		break;
	case 50:fname="./Data/50.Model";
		break;
	case 51:fname="./Data/51.Model";
		break;
	case 52:fname="./Data/52.Model";
		break;
	case 53:fname="./Data/53.Model";
		break;
	case 54:fname="./Data/54.Model";
		break;
	case 55:fname="./Data/55.Model";
		break;
	case 56:fname="./Data/56.Model";
		break;
	case 57:fname="./Data/57.Model";
		break;
	case 58:fname="./Data/58.Model";
		break;
	case 59:fname="./Data/59.Model";
		break;
	case 60:fname="./Data/60.Model";
		break;
	case 61:fname="./Data/61.Model";
		break;
	case 62:fname="./Data/62.Model";
		break;
	case 63:fname="./Data/63.Model";
		break;
	case 64:fname="./Data/64.Model";
		break;
	case 65:fname="./Data/65.Model";
		break;
	case 66:fname="./Data/66.Model";
		break;
	case 67:fname="./Data/67.Model";
		break;
	case 68:fname="./Data/68.Model";
		break;
	case 69:fname="./Data/69.Model";
		break;
	case 70:fname="./Data/70.Model";
		break;
	case 71:fname="./Data/71.Model";
		break;
	case 72:fname="./Data/72.Model";
		break;

	}
	

	fp=fopen(fname,"ab+");

	char* tem=new char[4];

	if(fp==NULL)
	{
		AfxMessageBox("错误！");
	}

	int row,col;

	for(row=0;row<pImg->height;row++)
	{
		uchar* ptr=(uchar*)(pImg->imageData+row*pImg->widthStep);
		//printf("the %d row : ",row);
		for(col=0;col<pImg->width;col++)
		{
			sprintf(tem,"%4d",ptr[col]);    //%4d 为占位符  4个占位符
			fputs(tem,fp);

		}
	
		fputs("\n",fp);
	}
	fclose(fp);

}



/*****************************************获取特征值*****************************************/
charVector* GetFeatureFromImgByGrid(IplImage* img)/*******分割12网格*************/
{
	//此处输入的图像需经过二值化处理

	//  int max_row_count=0,max_cols_count=0;
	int height,width,step,channals;
	height=img->height;
	width=img->width;
	step=img->widthStep;
	//	channals=src->nChannels;

	charVector *Vecto=new charVector;
	//Vecto->featureGrid
	uchar* data=(uchar*)img->imageData;

	int i,j,num=0;
	int PixMax=255;     //设定判断的像素值

	for(i=0;i<16;i++)
		Vecto->featureGrid[i]=0.0;

	//图像的大小为WIDTH*HEIGHT，共分为12块

	//当图像尺寸改变时，使得始终在字符区取值
  // int Diff_H=(NEW_ROW-HEIGHT)/2;       //水平边沿差值
  // int Diff_W=(NEW_COL-WIDTH)/2;       //竖直边沿差值

	//第一块
	for(i=0;i<width/4;i++)
	{
		for (j=0;j<height/4;j++)
		{

			if(data[i*step+j]==PixMax)
				Vecto->featureGrid[num]+=1.0;
			
		}

	}

num++;
	//第二块

	for(i=0;i<width/4;i++)
	{
		for (j=height/4;j<height/2;j++)
		{
			if(data[i*step+j]==PixMax)
				Vecto->featureGrid[num]+=1.0;
		}
	}

num++;

	//第三块
	for(i=0;i<width/4;i++)
	{
		for (j=height/2;j<(3*height)/4;j++)
		{
			if(data[i*step+j]==PixMax)
				Vecto->featureGrid[num]+=1.0;
		}
	}
num++;

	//第四块
	for(i=0;i<width/4;i++)
	{
		for (j=(3*height)/4;j<height;j++)
		{
			if(data[i*step+j]==PixMax)
				Vecto->featureGrid[num]+=1.0;
		}
	}

num++;
	//第五块
	for(i=width/4;i<width/2;i++)
	{
		for (j=0;j<height/4;j++)
		{
			if(data[i*step+j]==PixMax)
				Vecto->featureGrid[num]+=1.0;
		}
	}
num++;
	//第六块
	for(i=width/4;i<width/2;i++)
	{
		for (j=height/4;j<height/2;j++)
		{
			if(data[i*step+j]==PixMax)
				Vecto->featureGrid[num]+=1.0;
		}
	}
num++;
	//第七块
	for(i=width/4;i<width/2;i++)
	{
		for (j=height/2;j<(3*height)/4;j++)
		{
			if(data[i*step+j]==PixMax)
				Vecto->featureGrid[num]+=1.0;
		}
	}
num++;
	//第八块
	for(i=width/4;i<width/2;i++)
	{
		for (j=(3*height)/4;j<height;j++)
		{
			if(data[i*step+j]==PixMax)
				Vecto->featureGrid[num]+=1.0;
		}
	}

num++;
	//第九块
	for(i=width/2;i<(3*width)/4;i++)
	{
		for (j=0;j<height/4;j++)
		{
			if(data[i*step+j]==PixMax)
				Vecto->featureGrid[num]+=1.0;
		}
	}
num++;
	//第十块
	for(i=width/2;i<(3*width)/4;i++)
	{
		for (j=height/4;j<height/2;j++)
		{
			if(data[i*step+j]==PixMax)
				Vecto->featureGrid[num]+=1.0;
		}
	}
num++;
	//第十一块
	for(i=width/2;i<(3*width)/4;i++)
	{
		for (j=height/2;j<(3*height)/4;j++)
		{
			if(data[i*step+j]==PixMax)
				Vecto->featureGrid[num]+=1.0;
		}
	}
num++;
	//第十二块
    for(i=width/2;i<(3*width)/4;i++)
	{
		for (j=(3*height)/4;j<height;j++)
		{
			if(data[i*step+j]==PixMax)
				Vecto->featureGrid[num]+=1.0;
		}
	}
	num++;
	//第十三块
	for(i=(3*width)/4;i<width;i++)
	{
		for (j=0;j<height/4;j++)
		{
			if(data[i*step+j]==PixMax)
				Vecto->featureGrid[num]+=1.0;
		}
	}
num++;
	//第十四块
	for(i=(3*width)/4;i<width;i++)
	{
		for (j=height/4;j<height/2;j++)
		{
			if(data[i*step+j]==PixMax)
				Vecto->featureGrid[num]+=1.0;
		}
	}
num++;
	//第十五块
	for(i=(3*width)/4;i<width;i++)
	{
		for (j=height/2;j<(3*height)/4;j++)
		{
			if(data[i*step+j]==PixMax)
				Vecto->featureGrid[num]+=1.0;
		}
	}
num++;
	//第十六块
	for(i=(3*width)/4;i<width;i++)
	{
		for (j=(3*height)/4;j<height;j++)
		{
			if(data[i*step+j]==PixMax)
				Vecto->featureGrid[num]+=1.0;
		}
	}
num++;
	int totalPix=(width*height)/16;//每一块的总像素

	for (int i=0;i<num;i++)        //区域相素占区域总相素的百分比
	{
		Vecto->featureGrid[i]=Vecto->featureGrid[i]/totalPix;
	}

	return Vecto;
}

charVector* GetFeatureFromMatByGrid(CvMat *mat)
{
	IplImage *img=cvCreateImage(cvGetSize(mat),8,1);
	cvConvert(mat,img);          //将矩阵转换成图像
	//cvShowImage("img",img);

	charVector *Vecto=new charVector;
	Vecto=GetFeatureFromImgByGrid(img);
	cvReleaseImage(&img);
	return Vecto;
}


charVector* GetFeatureFromImgByPrj(IplImage* img)
{
	//参数为图像
	//此处使用二值化图
	int height,width,step,channals;

	height=img->height;
	width=img->width;
	step=img->widthStep/sizeof(int);

	uchar* data=(uchar*)img->imageData;
	int i,j;
	int PixMax=255;   

	charVector *vecto=new charVector;

	int count;

	for(j=0;j<img->height;j++)           //找到上行开始
	{

		count=0;
		for(i=0;i<img->width;i++)
		{
			//printf("%3d",data[j*step+i]);
			// double d=CV_MAT_ELEM(*mat,double,j,i);
			//			cout<<setw(3)<<d;
			if(data[j*img->width+i]==PixMax)  //统计行像素个数
				count++;
		}
		vecto->row_count[j]=count;
	}

	count=0;

	for(i=0;i<img->width;i++)           //找到左列开始
	{
		count=0;
		for(j=0;j<img->height;j++)
		{
			if(data[j*img->width+i]==PixMax)
				count++;
		}
		vecto->col_count[i]=count;
	}

	return vecto;            //返回投影统计
}

charVector* GetFeatureFromMatByPrj(CvMat* mat)//使用投影方法对图像进行像素统计
{
	IplImage *img=cvCreateImage(cvGetSize(mat),8,1);
	cvConvert(mat,img);          //将矩阵转换成图像
	//cvShowImage("img",img);

	charVector *Vecto=new charVector;
	Vecto=GetFeatureFromImgByPrj(img);
	cvReleaseImage(&img);
	return Vecto;
}

double EuclideanDistance(charVector *Sr,charVector *Te,int methodType) 
/*欧式距离 ，methodType的取值:
*为0时采用投影特征值计算，
*为1采用网格特征值计算*/
{

	switch (methodType)
	{
	case 0:
		{
			double dv=0.0;      // 竖直投影相似值  
			double dh=0.0;      //水平投影相似值

			//计算竖直投影的欧式距离
			for(int i=0;i<24;i++)
			{
				dv+=pow((double)Sr->col_count[i]-Te->col_count[i],2);
			}
			dv=sqrt((double)dv);

			//计算水平投影的欧式距离
			for(int i=0;i<48;i++)
			{
				dh+=pow((double)Sr->row_count[i]-Te->row_count[i],2);
			}
			dh=sqrt((double)dh);

			double simi=sqrt((double)(pow(dv,2)+pow(dh,2)));

			return simi;
		}
		break;
	case 1:
		{
			double d=0.0;

			for(int i=0;i<12;i++)
			{
				d+=pow((double)Sr->featureGrid[i]-Te->featureGrid[i],2);
			}
			d=sqrt((double)d);

			//	double simi=sqrt((double)(pow(d,2)+pow(dh,2)));

			return d;
		}

	}

}

Similary* calEuclideanDistance(int NumOfChar,Char_Letter* letter,int type)
{
	Similary *Simila=new Similary;
	switch (NumOfChar)
	{
	case 1:
		{
			switch(type)
			{
			case 0:
				{
					Simila->simi[0]=EuclideanDistance(SrcchprjVector[0],chprjVector[letter->mark_max[0]],0);
					Simila->simi[1]=EuclideanDistance(SrcchprjVector[0],chprjVector[letter->mark_max[1]],0);
					Simila->simi[2]=EuclideanDistance(SrcchprjVector[0],chprjVector[letter->mark_max[2]],0);

					return Simila;
				}
				break;
			case 1:
				{

					Simila->simi[0]=EuclideanDistance(SrcchGridVector[0],chGridVector[letter->mark_max[0]],1);
					Simila->simi[1]=EuclideanDistance(SrcchGridVector[0],chGridVector[letter->mark_max[1]],1);
					Simila->simi[2]=EuclideanDistance(SrcchGridVector[0],chGridVector[letter->mark_max[2]],1);

					return Simila;
				}
				break;
			}
		}
		break;

	case 2:
		{
			switch(type)
			{
			case 0:
				{
					Simila->simi[0]=EuclideanDistance(SrcchprjVector[1],chprjVector[letter->mark_max[0]],0);
					Simila->simi[1]=EuclideanDistance(SrcchprjVector[1],chprjVector[letter->mark_max[1]],0);
					Simila->simi[2]=EuclideanDistance(SrcchprjVector[1],chprjVector[letter->mark_max[2]],0);

					return Simila;
				}
				break;
			case 1:
				{
					Simila->simi[0]=EuclideanDistance(SrcchGridVector[1],chGridVector[letter->mark_max[0]],1);
					Simila->simi[1]=EuclideanDistance(SrcchGridVector[1],chGridVector[letter->mark_max[1]],1);
					Simila->simi[2]=EuclideanDistance(SrcchGridVector[1],chGridVector[letter->mark_max[2]],1);

					return Simila;
				}
				break;
			}
		}
		break;

	case 3:
		{
			switch(type)
			{
			case 0:
				{
					Simila->simi[0]=EuclideanDistance(SrcchprjVector[2],chprjVector[letter->mark_max[0]],0);
					Simila->simi[1]=EuclideanDistance(SrcchprjVector[2],chprjVector[letter->mark_max[1]],0);
					Simila->simi[2]=EuclideanDistance(SrcchprjVector[2],chprjVector[letter->mark_max[2]],0);

					return Simila;
				}
				break;
			case 1:
				{
					Simila->simi[0]=EuclideanDistance(SrcchGridVector[2],chGridVector[letter->mark_max[0]],1);
					Simila->simi[1]=EuclideanDistance(SrcchGridVector[2],chGridVector[letter->mark_max[1]],1);
					Simila->simi[2]=EuclideanDistance(SrcchGridVector[2],chGridVector[letter->mark_max[2]],1);

					return Simila;
				}
				break;
			}
		}
		break;

	case 4:
		{
			switch(type)
			{
			case 0:
				{
					Simila->simi[0]=EuclideanDistance(SrcchprjVector[3],chprjVector[letter->mark_max[0]],0);
					Simila->simi[1]=EuclideanDistance(SrcchprjVector[3],chprjVector[letter->mark_max[1]],0);
					Simila->simi[2]=EuclideanDistance(SrcchprjVector[3],chprjVector[letter->mark_max[2]],0);

					return Simila;
				}
				break;
			case 1:
				{
					Simila->simi[0]=EuclideanDistance(SrcchGridVector[3],chGridVector[letter->mark_max[0]],1);
					Simila->simi[1]=EuclideanDistance(SrcchGridVector[3],chGridVector[letter->mark_max[1]],1);
					Simila->simi[2]=EuclideanDistance(SrcchGridVector[3],chGridVector[letter->mark_max[2]],1);

					return Simila;
				}
				break;
			}
		}
		break;

	case 5:
		{
			switch(type)
			{
			case 0:
				{
					Simila->simi[0]=EuclideanDistance(SrcchprjVector[4],chprjVector[letter->mark_max[0]],0);
					Simila->simi[1]=EuclideanDistance(SrcchprjVector[4],chprjVector[letter->mark_max[1]],0);
					Simila->simi[2]=EuclideanDistance(SrcchprjVector[4],chprjVector[letter->mark_max[2]],0);

					return Simila;
				}
				break;
			case 1:
				{
					Simila->simi[0]=EuclideanDistance(SrcchGridVector[4],chGridVector[letter->mark_max[0]],1);
					Simila->simi[1]=EuclideanDistance(SrcchGridVector[4],chGridVector[letter->mark_max[1]],1);
					Simila->simi[2]=EuclideanDistance(SrcchGridVector[4],chGridVector[letter->mark_max[2]],1);

					return Simila;
				}
				break;
			}
		}
		break;

	case 6:
		{
			switch(type)
			{
			case 0:
				{
					Simila->simi[0]=EuclideanDistance(SrcchprjVector[5],chprjVector[letter->mark_max[0]],0);
					Simila->simi[1]=EuclideanDistance(SrcchprjVector[5],chprjVector[letter->mark_max[1]],0);
					Simila->simi[2]=EuclideanDistance(SrcchprjVector[5],chprjVector[letter->mark_max[2]],0);

					return Simila;
				}
				break;
			case 1:
				{
					Simila->simi[0]=EuclideanDistance(SrcchGridVector[5],chGridVector[letter->mark_max[0]],1);
					Simila->simi[1]=EuclideanDistance(SrcchGridVector[5],chGridVector[letter->mark_max[1]],1);
					Simila->simi[2]=EuclideanDistance(SrcchGridVector[5],chGridVector[letter->mark_max[2]],1);

					return Simila;
				}
				break;
			}
		}
		break;

	case 7:
		{
			switch(type)
			{
			case 0:
				{
					Simila->simi[0]=EuclideanDistance(SrcchprjVector[6],chprjVector[letter->mark_max[0]],0);
					Simila->simi[1]=EuclideanDistance(SrcchprjVector[6],chprjVector[letter->mark_max[1]],0);
					Simila->simi[2]=EuclideanDistance(SrcchprjVector[6],chprjVector[letter->mark_max[2]],0);

					return Simila;
				}
				break;
			case 1:
				{
					Simila->simi[0]=EuclideanDistance(SrcchGridVector[6],chGridVector[letter->mark_max[0]],1);
					Simila->simi[1]=EuclideanDistance(SrcchGridVector[6],chGridVector[letter->mark_max[1]],1);
					Simila->simi[2]=EuclideanDistance(SrcchGridVector[6],chGridVector[letter->mark_max[2]],1);

					return Simila;
				}
				break;
			}
		}
		break;
	}

}

Similary* calEuclideanDistance(charVector *Sr,charVector *Te,int numOfChar,int type)
{
	Similary *Simila=new Similary;

	switch (numOfChar)
	{
	case 1:
		{
			switch(type)
			{
			case 0:
				{
					for (int i=0;i<73;i++)
					{
						Simila->simi[i]=EuclideanDistance(SrcchprjVector[0],chprjVector[i],0);
					}

					return Simila;
				}
				break;
			case 1:
				{

					for (int i=0;i<73;i++)
					{
						Simila->simi[i]=EuclideanDistance(SrcchGridVector[0],chGridVector[i],1);
					}

				 return Simila;
				}
				break;
			}
		}
		break;

	case 2:
		{
			switch(type)
			{
			case 0:
				{
					for (int i=0;i<73;i++)
					{
						Simila->simi[i]=EuclideanDistance(SrcchprjVector[1],chprjVector[i],0);
					}

					return Simila;
				}
				break;
			case 1:
				{

					for (int i=0;i<73;i++)
					{
						Simila->simi[i]=EuclideanDistance(SrcchGridVector[1],chGridVector[i],1);
					}

					return Simila;
				}
				break;
			}
		}
		break;

	case 3:
		{
			switch(type)
			{
			case 0:
				{
					for (int i=0;i<73;i++)
					{
						Simila->simi[i]=EuclideanDistance(SrcchprjVector[2],chprjVector[i],0);
					}

					return Simila;
				}
				break;
			case 1:
				{

					for (int i=0;i<73;i++)
					{
						Simila->simi[i]=EuclideanDistance(SrcchGridVector[2],chGridVector[i],1);
					}

					return Simila;
				}
				break;
			}
		}
		break;

	case 4:
		{
			switch(type)
			{
			case 0:
				{
					for (int i=0;i<73;i++)
					{
						Simila->simi[i]=EuclideanDistance(SrcchprjVector[3],chprjVector[i],0);
					}

					return Simila;
				}
				break;
			case 1:
				{

					for (int i=0;i<73;i++)
					{
						Simila->simi[i]=EuclideanDistance(SrcchGridVector[3],chGridVector[i],1);
					}

					return Simila;
				}
				break;
			}
		}
		break;

	case 5:
		{
			switch(type)
			{
			case 0:
				{
					for (int i=0;i<73;i++)
					{
						Simila->simi[i]=EuclideanDistance(SrcchprjVector[4],chprjVector[i],0);
					}

					return Simila;
				}
				break;
			case 1:
				{

					for (int i=0;i<73;i++)
					{
						Simila->simi[i]=EuclideanDistance(SrcchGridVector[4],chGridVector[i],1);
					}

					return Simila;
				}
				break;
			}
		}
		break;

	case 6:
		{
			switch(type)
			{
			case 0:
				{
					for (int i=0;i<73;i++)
					{
						Simila->simi[i]=EuclideanDistance(SrcchprjVector[5],chprjVector[i],0);
					}

					return Simila;
				}
				break;
			case 1:
				{

					for (int i=0;i<73;i++)
					{
						Simila->simi[i]=EuclideanDistance(SrcchGridVector[5],chGridVector[i],1);
					}

					return Simila;
				}
				break;
			}
		}
		break;

	case 7:
		{
			switch(type)
			{
			case 0:
				{
					for (int i=0;i<73;i++)
					{
						Simila->simi[i]=EuclideanDistance(SrcchprjVector[6],chprjVector[i],0);
					}

					return Simila;
				}
				break;
			case 1:
				{

					for (int i=0;i<73;i++)
					{
						Simila->simi[i]=EuclideanDistance(SrcchGridVector[6],chGridVector[i],1);
					}

					return Simila;
				}
				break;
			}
		}
		break;
	}





	/*
	switch (NumOfChar)
		{
		case 1:
			{
				switch(type)
				{
				case 0:
					{
						Simila->simi[0]=EuclideanDistance(SrcchprjVector[0],chprjVector[letter->mark_max[0]],0);
						Simila->simi[1]=EuclideanDistance(SrcchprjVector[0],chprjVector[letter->mark_max[1]],0);
						Simila->simi[2]=EuclideanDistance(SrcchprjVector[0],chprjVector[letter->mark_max[2]],0);
	
						return Simila;
					}
					break;
				case 1:
					{
	
						Simila->simi[0]=EuclideanDistance(SrcchGridVector[0],chGridVector[letter->mark_max[0]],1);
						Simila->simi[1]=EuclideanDistance(SrcchGridVector[0],chGridVector[letter->mark_max[1]],1);
						Simila->simi[2]=EuclideanDistance(SrcchGridVector[0],chGridVector[letter->mark_max[2]],1);
	
						return Simila;
					}
					break;
				}
			}
			break;
	
		case 2:
			{
				switch(type)
				{
				case 0:
					{
						Simila->simi[0]=EuclideanDistance(SrcchprjVector[1],chprjVector[letter->mark_max[0]],0);
						Simila->simi[1]=EuclideanDistance(SrcchprjVector[1],chprjVector[letter->mark_max[1]],0);
						Simila->simi[2]=EuclideanDistance(SrcchprjVector[1],chprjVector[letter->mark_max[2]],0);
	
						return Simila;
					}
					break;
				case 1:
					{
						Simila->simi[0]=EuclideanDistance(SrcchGridVector[1],chGridVector[letter->mark_max[0]],1);
						Simila->simi[1]=EuclideanDistance(SrcchGridVector[1],chGridVector[letter->mark_max[1]],1);
						Simila->simi[2]=EuclideanDistance(SrcchGridVector[1],chGridVector[letter->mark_max[2]],1);
	
						return Simila;
					}
					break;
				}
			}
			break;
	
		case 3:
			{
				switch(type)
				{
				case 0:
					{
						Simila->simi[0]=EuclideanDistance(SrcchprjVector[2],chprjVector[letter->mark_max[0]],0);
						Simila->simi[1]=EuclideanDistance(SrcchprjVector[2],chprjVector[letter->mark_max[1]],0);
						Simila->simi[2]=EuclideanDistance(SrcchprjVector[2],chprjVector[letter->mark_max[2]],0);
	
						return Simila;
					}
					break;
				case 1:
					{
						Simila->simi[0]=EuclideanDistance(SrcchGridVector[2],chGridVector[letter->mark_max[0]],1);
						Simila->simi[1]=EuclideanDistance(SrcchGridVector[2],chGridVector[letter->mark_max[1]],1);
						Simila->simi[2]=EuclideanDistance(SrcchGridVector[2],chGridVector[letter->mark_max[2]],1);
	
						return Simila;
					}
					break;
				}
			}
			break;
	
		case 4:
			{
				switch(type)
				{
				case 0:
					{
						Simila->simi[0]=EuclideanDistance(SrcchprjVector[3],chprjVector[letter->mark_max[0]],0);
						Simila->simi[1]=EuclideanDistance(SrcchprjVector[3],chprjVector[letter->mark_max[1]],0);
						Simila->simi[2]=EuclideanDistance(SrcchprjVector[3],chprjVector[letter->mark_max[2]],0);
	
						return Simila;
					}
					break;
				case 1:
					{
						Simila->simi[0]=EuclideanDistance(SrcchGridVector[3],chGridVector[letter->mark_max[0]],1);
						Simila->simi[1]=EuclideanDistance(SrcchGridVector[3],chGridVector[letter->mark_max[1]],1);
						Simila->simi[2]=EuclideanDistance(SrcchGridVector[3],chGridVector[letter->mark_max[2]],1);
	
						return Simila;
					}
					break;
				}
			}
			break;
	
		case 5:
			{
				switch(type)
				{
				case 0:
					{
						Simila->simi[0]=EuclideanDistance(SrcchprjVector[4],chprjVector[letter->mark_max[0]],0);
						Simila->simi[1]=EuclideanDistance(SrcchprjVector[4],chprjVector[letter->mark_max[1]],0);
						Simila->simi[2]=EuclideanDistance(SrcchprjVector[4],chprjVector[letter->mark_max[2]],0);
	
						return Simila;
					}
					break;
				case 1:
					{
						Simila->simi[0]=EuclideanDistance(SrcchGridVector[4],chGridVector[letter->mark_max[0]],1);
						Simila->simi[1]=EuclideanDistance(SrcchGridVector[4],chGridVector[letter->mark_max[1]],1);
						Simila->simi[2]=EuclideanDistance(SrcchGridVector[4],chGridVector[letter->mark_max[2]],1);
	
						return Simila;
					}
					break;
				}
			}
			break;
	
		case 6:
			{
				switch(type)
				{
				case 0:
					{
						Simila->simi[0]=EuclideanDistance(SrcchprjVector[5],chprjVector[letter->mark_max[0]],0);
						Simila->simi[1]=EuclideanDistance(SrcchprjVector[5],chprjVector[letter->mark_max[1]],0);
						Simila->simi[2]=EuclideanDistance(SrcchprjVector[5],chprjVector[letter->mark_max[2]],0);
	
						return Simila;
					}
					break;
				case 1:
					{
						Simila->simi[0]=EuclideanDistance(SrcchGridVector[5],chGridVector[letter->mark_max[0]],1);
						Simila->simi[1]=EuclideanDistance(SrcchGridVector[5],chGridVector[letter->mark_max[1]],1);
						Simila->simi[2]=EuclideanDistance(SrcchGridVector[5],chGridVector[letter->mark_max[2]],1);
	
						return Simila;
					}
					break;
				}
			}
			break;
	
		case 7:
			{
				switch(type)
				{
				case 0:
					{
						Simila->simi[0]=EuclideanDistance(SrcchprjVector[6],chprjVector[letter->mark_max[0]],0);
						Simila->simi[1]=EuclideanDistance(SrcchprjVector[6],chprjVector[letter->mark_max[1]],0);
						Simila->simi[2]=EuclideanDistance(SrcchprjVector[6],chprjVector[letter->mark_max[2]],0);
	
						return Simila;
					}
					break;
				case 1:
					{
						Simila->simi[0]=EuclideanDistance(SrcchGridVector[6],chGridVector[letter->mark_max[0]],1);
						Simila->simi[1]=EuclideanDistance(SrcchGridVector[6],chGridVector[letter->mark_max[1]],1);
						Simila->simi[2]=EuclideanDistance(SrcchGridVector[6],chGridVector[letter->mark_max[2]],1);
	
						return Simila;
					}
					break;
				}
			}
			break;
		}*/
	
}


bool IsDistinguishChar(Char_Letter *letter,float LimitNum)
{
	double dif=letter->max_val[0]-letter->max_val[1];

	if (abs(dif)>LimitNum)
	{
		return TRUE;
	}

	else if (abs(dif)<LimitNum)
	{
		return FALSE;
	}
}

/****************获取全部模板字库特征********************************/
void InitTemplateFeature()
{
	//根据投影法取特征值
	for (int i=0;i<73;i++)
	{
       chprjVector[i]=GetFeatureFromMatByPrj(&Tmat_32fc1[i]);
       chprjVector[i]->method=0;
	}
  
	//根据网格法取特征值

	for (int i=0;i<73;i++)
	{
		chGridVector[i]=GetFeatureFromMatByGrid(&Tmat_32fc1[i]);
		chGridVector[i]->method=1;
	}

    //根据中心矩提取特征
   for (int i=0;i<73;i++)
   {
	   chMomentVector[i]=GetFeatureByMomment(&Tmat_32fc1[i]);
	   chMomentVector[i]->method=3;
   }

}

void InitSrcFreature(IplImage **img,CvSize RszImg)
{
	////根据投影法取特征值
   IplImage **dst=new IplImage*[7];
  // CvSize sizes=cvSize(WIDTH,HEIGHT);

 
   for (int i=0;i<7;i++)       
   {
	   dst[i]=cvCloneImage(img[i]);//复制图像
	   dst[i]=t_Resize(dst[i],RszImg);//归一化

	   SrcchprjVector[i]=GetFeatureFromImgByPrj(dst[i]);  //根据投影提取特征

	   SrcchGridVector[i]=GetFeatureFromImgByGrid(dst[i]);//根据网格法取特征值

	   SrcchMomentVector[i]=GetFeatureByMomment(dst[i]);    //根据中心矩提取特征
   }
 
	//释放内存
	for(int i=0;i<7;i++)
	{
		cvReleaseImage(&dst[i]);
	}

}


void ReturnCharByEucDistance(Char_Letter *ch,Similary *Sim,bool UseReturnChar2)
{
	double minSim=min(Sim->simi[0],Sim->simi[1]);                     //取最小值
	minSim=min(minSim,Sim->simi[2]);                                //再取最小值
	if (minSim==Sim->simi[1])
	{
		if (UseReturnChar2)
		{
			ch->char_name=ReturnChar2(ch->mark_max[1]);
		}
		else 
		{
			ch->char_name=ReturnChar(ch->mark_max[1]);
		}
		ch->mark_max[0]=ch->mark_max[1];
	}
	else if (minSim==Sim->simi[2])
	{
		ch->char_name=ReturnChar(ch->mark_max[2]);
		ch->mark_max[0]=ch->mark_max[2];
	}
}



IplImage* ReszNoChangeRotateImg(IplImage* img, int degree,CvSize szChar,CvSize szImg)     
/*改变图片的尺寸内容不变，且可旋转
*degree:需要旋转的角度
*/
{  
	IplImage *img1=cvCloneImage(img);
    img1=t_Resize(img,szChar);
	double angle = degree  * CV_PI / 180.; 
	double a = sin(angle), b = cos(angle); 
	int width=img1->width, height=img1->height;
	//旋转后的新图尺寸
	int width_rotate= szImg.width;//int(height * fabs(a) + width * fabs(b));  
	int height_rotate=szImg.height;//int(width * fabs(a) + height * fabs(b));  
	IplImage* img_rotate = cvCreateImage(cvSize(width_rotate, height_rotate), img1->depth, img1->nChannels);  
	cvZero(img_rotate);  
	//保证原图可以任意角度旋转的最小尺寸
	int tempLength = sqrt((double)width * width + (double)height *height) + 10;  
	int tempX = (tempLength + 1) / 2 - width / 2;  
	int tempY = (tempLength + 1) / 2 - height / 2;  
	IplImage* temp = cvCreateImage(cvSize(tempLength, tempLength), img1->depth, img1->nChannels);  
	cvZero(temp);  
	//将原图复制到临时图像tmp中心
	cvSetImageROI(temp, cvRect(tempX, tempY, width, height));  
	cvCopy(img1, temp, NULL);  
	cvResetImageROI(temp);  
	//旋转数组map
	// [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]
	// [ m3  m4  m5 ] ===>  [ A21  A22   b2 ]
	float m[6];  
	int w = temp->width;  
	int h = temp->height;  
	m[0] = b;  
	m[1] = a;  
	m[3] = -m[1];  
	m[4] = m[0];  
	// 将旋转中心移至图像中间  
	m[2] = w * 0.5f;  
	m[5] = h * 0.5f;  
	CvMat M = cvMat(2, 3, CV_32F, m);  
	cvGetQuadrangleSubPix(temp, img_rotate, &M);  
	cvReleaseImage(&temp);  
	return img_rotate;
}  

double** ReadFromFile(CString filename,CvSize szChar,int NumChar)
{
	char* fName=(LPSTR)(LPCTSTR)filename;
	ifstream fp(fName,std::ios::in);

	char lin[1024]={0};

	int CharWidth=szChar.width;
	int CharHeight=szChar.height;
	double** dataMultiTem=new double*[NumChar]; 

	for (int i=0;i<NumChar;i++)   //创建二维
	{
		dataMultiTem[i]=new double[CharWidth*CharHeight];
	}

   //初始化
	for (int i=0;i<NumChar;i++)
	{
		for (int j=0;j<CharWidth*CharHeight;j++)
		{
          dataMultiTem[i][j]=0.0;
		}
	}

	int m=0;
	int k=0;
	if (fp)
	{
		while (fp.getline(lin,sizeof(lin)))
		{
			std::stringstream word(lin);


			for (int i=0;i<CharWidth;i++)
			{
				word>>dataMultiTem[k][m];
				m++;
			}

			if (m==CharWidth*CharHeight)   //如果取到最后一个元素
			{
				k++;       //指向下一个字符
				m=0;       //重置m
			}

		}
	

	}
	
	fp.clear();
	fp.close();
	return dataMultiTem;
}

int ScanCharNumFrmFile(CString filename,int charHeight)
{
	char* fName=(LPSTR)(LPCTSTR)filename;
	ifstream fp(fName,std::ios::in);
	char lin[1024]={0};

	int m=0;
	int k=0;
	if (fp!=NULL)
	{
		while (fp.getline(lin,sizeof(lin)))
		{
			m++;          //统计行数
		}

		fp.clear();
		fp.close();
	}

	
	return m/charHeight;     //字符大小24*48
}



	int B_Jing[]={31,32,33,34,35,
		36,37,38,39,40,
		41,42,43,44,45,
		46,47,48,49,50,
		51,52,53,54,55,
		56};
	//津　　 num=26
	int T_Jin[]={31,32,33,34,35,
		36,37,38,39,40,
		41,42,43,44,45,
		46,47,48,49,50,
		51,52,53,54,55,
		56};
	//冀     num=11
	int H_Ji[]={31,32,33,34,35,
		36,37,38,40,47,
	49};


	//晋  山西   num=11
	int S_Jin[]={31,32,33,34,35,
		36,38,40,41,42,
		43};


	//蒙     num=12
	int N_Meng[]={31,32,33,34,35,
		36,37,38,40,41,
		42,43};


	//辽   num=14
	int S_Liao[]={31,32,33,34,35,
		36,37,38,40,41,
		42,43,44,46};


	//吉    num=10
	int J_Ji[]={31,32,33,34,35,
		36,37,38,40,41};


	//黑     num=15
	int H_Hei[]={31,32,33,34,35,
		36,37,38,40,41,
		42,43,44,46,47};


	//沪     num=26
	int S_Hu[]={31,32,33,34,35,
		36,37,38,39,40,
		41,42,43,44,45,
		46,47,48,49,50,
		51,52,53,54,55,
		56};

	//苏      num=13
	int J_Su[]={31,32,33,34,35,
		36,37,38,40,41,
		42,43,44};


	//浙    num=11
	int Z_Zhe[]={31,32,33,34,35,
		36,37,38,40,41,
		42};


	//皖     num=17
	int AN_Wan[]={31,32,33,34,35,
		36,37,38,40,41,
		42,43,44,46,47,
		48,49};


	//闽    num=10
	int F_Min[]={31,32,33,34,35,
		36,37,38,40,41};


	//赣    num=12 
	int J_Gan[]={31,32,33,34,35,
		36,37,38,40,41,
		42,43};

	//鲁    num=20
	int S_Lu[]={31,32,33,34,35,
		36,37,38,40,41,
		42,43,44,46,47,
		48,49,51,52,55};


	//豫   num= 18
	int H_Yu[]={31,32,33,34,35,
		36,37,38,40,41,
		42,43,44,46,47,
		48,49,51};


	//鄂    num=17
	int H_E[]={31,32,33,34,35,
		36,37,38,40,41,
		42,43,54,56,57,
		48,49};

	//湘    num=15
	int H_Xiang[]={31,32,33,34,35,
		36,37,38,40,41,
		42,43,44,49,51};
	//粤   num=23
	int G_Yue[]={31,32,33,34,35,
		36,37,38,40,41,
		42,43,44,46,47,
		48,49,50,51,52,
		53,54,55};
	//桂   num=15  
	int GX_Gui[]={31,32,33,34,35,
		36,37,38,40,41,
		42,43,44,46,48};

	//琼   num=5
	int H_Qiong[]={31,32,33,34,35};

	//渝   num=26
	int C_Yu[]={31,32,33,34,35,
		36,37,38,39,40,
		41,42,43,44,45,
		46,47,48,49,50,
		51,52,53,54,55,
		56};
	//川   num=21
	int S_Chuan[]={31,32,33,34,35,
		36,48,40,41,42,
		43,47,48,49,50,
		51,52,53,54,55,
		56};


	//贵    num=9
	int GZ_Gui[]={31,32,33,34,35,
		36,37,38,40};

	//云    num=16
	int Y_Yun[]={31,32,33,34,35,
		37,38,40,41,42,
		43,44,46,47,48,
		49};


	//藏     num=9
	int X_Cang[]={31,32,33,34,35,
		36,37,38,40};


	//陕     num=10
	int S_Shan[]={31,32,33,34,35,
		36,37,38,41,52};


	//甘     num=14
	int G_Gan[]={31,32,33,34,35,
		36,37,38,40,41,
		42,43,44,46};


	//青     num=8
	int Q_Qing[]={31,32,33,34,35,
		36,37,38};


	//宁     num=5
	int N_Ning[]={31,32,33,34,35};


	//新     num=16
	int X_Xin[]={31,32,33,34,35,
		36,37,38,40,41,
		42,43,44,56,47,
		48};

void dataCvt2Mat32fc1()
{
	//ReadFromFile()
	//LoadMatConvert(Tmat_64fc1,Tmat_32fc1,data,cvSize(24,48),73);

	int num=ScanCharNumFrmFile("./Data/Standard.Model",CHAR_HEIGHT);
	Tdata=ReadFromFile("./Data/Standard.Model",cvSize(WIDTH,HEIGHT),num);
	LoadMatConvert1(Tmat_64fc1,Tmat_32fc1,Tdata,cvSize(WIDTH,HEIGHT),num);

}



BOOL IsInCitySub(Char_Letter *S_char1,Char_Letter *S_char2)
{
	switch (S_char1->mark_max[0])
	{
	case 0:         //京
		{
			if(IsInSub(S_char2->mark_max[0],B_Jing,26))
			{
               S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
			   return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],B_Jing,26))
			{
              S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
			  return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],B_Jing,26))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
	//			MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 1:      //津
		{
			if(IsInSub(S_char2->mark_max[0],T_Jin,26))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],T_Jin,26))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],T_Jin,26))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
	//			MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 2:     //冀
		{
			if(IsInSub(S_char2->mark_max[0],H_Ji,12))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],H_Ji,12))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],H_Ji,12))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
	//			MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 3:     //晋
		{
			if(IsInSub(S_char2->mark_max[0],S_Jin,11))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],S_Jin,11))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],S_Jin,11))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 4:    //蒙
		{ 
			if(IsInSub(S_char2->mark_max[0],N_Meng,12))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],N_Meng,12))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],N_Meng,12))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 5:    //辽
		{
			if(IsInSub(S_char2->mark_max[0],S_Liao,14))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],S_Liao,14))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],S_Liao,14))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
			//	MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 6:    //吉
		{
			if(IsInSub(S_char2->mark_max[0],J_Ji,10))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],J_Ji,10))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],J_Ji,10))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
			//	MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 7:      //黑
		{
			if(IsInSub(S_char2->mark_max[0],H_Hei,15))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],H_Hei,15))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],H_Hei,15))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
			//	MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 8:     //沪
		{
			if(IsInSub(S_char2->mark_max[0],S_Hu,26))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],S_Hu,26))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],S_Hu,26))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
			//	MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 9:       //苏
		{
			if(IsInSub(S_char2->mark_max[0],J_Su,13))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],J_Su,13))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],J_Su,13))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
			//	MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 10:     //浙
		{
			if(IsInSub(S_char2->mark_max[0],Z_Zhe,11))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],Z_Zhe,11))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],Z_Zhe,11))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
			//	MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 11:    //皖
		{
			if(IsInSub(S_char2->mark_max[0],AN_Wan,17))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],AN_Wan,17))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],AN_Wan,17))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
			//	MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 12:      //闽
		{
			if(IsInSub(S_char2->mark_max[0],F_Min,10))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],F_Min,10))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],F_Min,10))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
			//	MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 13:      //赣
		{
			if(IsInSub(S_char2->mark_max[0],J_Gan,12))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],J_Gan,12))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],J_Gan,12))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 14:       //鲁
		{
			if(IsInSub(S_char2->mark_max[0],S_Lu,20))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],S_Lu,20))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],S_Lu,20))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 15:     //豫
		{
			if(IsInSub(S_char2->mark_max[0],H_Yu,18))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],H_Yu,18))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],H_Yu,18))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 16:     //鄂
		{
			if(IsInSub(S_char2->mark_max[0],H_E,17))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],H_E,17))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],H_E,17))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 17:    //湘
		{
			if(IsInSub(S_char2->mark_max[0],H_Xiang,15))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],H_Xiang,15))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],H_Xiang,15))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 18:      //粤
		{
			if(IsInSub(S_char2->mark_max[0],G_Yue,23))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],G_Yue,23))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],G_Yue,23))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 19:     //桂
		{
			if(IsInSub(S_char2->mark_max[0],GX_Gui,15))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],GX_Gui,15))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],GX_Gui,15))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 20:      //琼
		{
			if(IsInSub(S_char2->mark_max[0],H_Qiong,5))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],H_Qiong,5))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],H_Qiong,5))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 21:     //渝
		{
			if(IsInSub(S_char2->mark_max[0],C_Yu,26))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],C_Yu,26))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],C_Yu,26))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 22:      //川
		{
			if(IsInSub(S_char2->mark_max[0],S_Chuan,21))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],S_Chuan,21))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],S_Chuan,21))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 23:    //贵
		{
			if(IsInSub(S_char2->mark_max[0],GZ_Gui,9))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],GZ_Gui,9))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],GZ_Gui,9))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 24:    //云
		{
			if(IsInSub(S_char2->mark_max[0],Y_Yun,16))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],Y_Yun,16))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],Y_Yun,16))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 25:     //藏
		{
			if(IsInSub(S_char2->mark_max[0],X_Cang,9))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],X_Cang,9))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],X_Cang,9))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 26:    //陕
		{
			if(IsInSub(S_char2->mark_max[0],S_Shan,10))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],S_Shan,10))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],S_Shan,10))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 27:     //甘
		{
			if(IsInSub(S_char2->mark_max[0],G_Gan,14))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],G_Gan,14))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],G_Gan,14))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 28:    //青
		{
			if(IsInSub(S_char2->mark_max[0],Q_Qing,8))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],Q_Qing,8))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],Q_Qing,8))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 29:     //宁
		{
			if(IsInSub(S_char2->mark_max[0],N_Ning,5))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],N_Ning,5))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],N_Ning,5))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;

	case 30:    //新
		{
			if(IsInSub(S_char2->mark_max[0],X_Xin,16))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[0]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[1],X_Xin,16))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[1]);
				return TRUE;
			}
			else if (IsInSub(S_char2->mark_max[2],X_Xin,16))
			{
				S_char2->char_name=ReturnChar(S_char2->mark_max[2]);
				return TRUE;
			}
			else
		//		MessageBox("ERRO！！拒绝识别！");
				return FALSE;
		}
		break;


	}
}




/******************************************************************************************************/
void FileCopyTo(CString source, CString destination, CString searchStr, BOOL cover = TRUE)
{
	CString strSourcePath = source;
	CString strDesPath = destination;
	CString strFileName = searchStr;
	//CFileFind filefinder;
	//CString strSearchPath = strSourcePath + "\\" + strFileName;
	CString strSearchDesPath = strDesPath + "\\" + strFileName;
	CString filename;
	//BOOL bfind = filefinder.FindFile(strSourcePath);

	CopyFile((LPCSTR)strSourcePath, (LPCSTR)strSearchDesPath, cover);
	/*
	CString SourcePath, DisPath;
	while (bfind)
	{
	bfind = filefinder.FindNextFile();
	filename = filefinder.GetFileName();
	SourcePath = strSourcePath + "\\" + filename;
	DisPath = strDesPath + "\\" + filename;
	CopyFile((LPCSTR)SourcePath, (LPCSTR)DisPath, cover);
	}*/
	//filefinder.Close();
}

/****************************************************************************************************/


/************************从数组中取出前N个最大值*****************************************/
double* GetMaxValFrmArray(double arraySimi[],int numOfArray,int numOfMax)
{
	double *MaxSimi=new double[numOfMax];

	double *arrayS=new double[numOfArray];
	for (int i=0;i<numOfArray;i++)
	{
		arrayS[i]=arraySimi[i];
	}

	double temp;

	for (int i=0;i<numOfArray-1;i++)
	{
		for (int j=i+1;j<numOfArray;j++)
		{
			if (arrayS[j]>arrayS[i])
			{
				temp=arrayS[j];
				arrayS[j]=arrayS[i];
				arrayS[i]=temp;
			}
		}
	}


	for (int i=0;i<numOfMax;i++)
	{
		MaxSimi[i]=arrayS[i];
	}

	delete[] arrayS;

	return MaxSimi;

}

/************************从数组中取出前N个最小值*****************************************/
double* GetMinValFrmArray(double arraySimi[],int numOfArray,int numOfMin)
{
	double *MinSimi=new double[numOfMin];

	double *arrayS=new double[numOfArray];
	for (int i=0;i<numOfArray;i++)
	{
		arrayS[i]=arraySimi[i];
	}

	double temp;

	for (int i=0;i<numOfArray-1;i++)
	{
		for (int j=i+1;j<numOfArray;j++)
		{
			if (arrayS[j]<arrayS[i])
			{
				temp=arrayS[j];
				arrayS[j]=arrayS[i];
				arrayS[i]=temp;
			}
		}
	}


	for (int i=0;i<numOfMin;i++)
	{
		MinSimi[i]=arrayS[i];
	}

	delete[] arrayS;

	return MinSimi;
}



int GetMarkFrmArray(double* arraySimi,int numOfArray,double SimiVal)
{

	for (int i=0;i<numOfArray;i++)
	{
		if (arraySimi[i]==SimiVal)
		{
			return i;
			break;
		}
	}
	return -1;
}

/*************释放全部内存*****************************/
void FreeAll()
{
	delete [] Tmat_64fc1;
    delete [] Tmat_32fc1;
    delete [] Tdata;
	for (int i=0;i<73;i++)
	{
	//	delete [] chprjVector[i];
	//	delete [] chGridVector[i];
	}
	

}


void DirImgWrite2File(CString fpath,CString fpathName,CvSize imgSz)
{
	IplImage **src=new IplImage*[73];


	CString str;
	for (int i=1;i<=73;i++)
	{
		str.Format("%d",i);
		CString strpath=fpath+str+".jpg";
		char* fName=(LPSTR)(LPCTSTR)strpath;
		src[i-1]=cvLoadImage(fName,0);
	}

    IplImage **dst=new IplImage*[73];
	for (int i=0;i<73;i++)
	{
		dst[i]=cvCreateImage(imgSz,src[i]->depth,src[i]->nChannels);
	}

	CString strpName="./Data/"+fpathName;

	for (int i=0;i<73;i++)
	{
		cvResize(src[i],dst[i],CV_INTER_LINEAR);

		cvThreshold(dst[i],dst[i],128,255,CV_THRESH_BINARY_INV);

		char *fpaName=(LPSTR)(LPCTSTR)strpName;
		GreyValueCollection(dst[i],fpaName);

	}

	
	for (int i=0;i<73;i++)
		{
			cvReleaseImage(&src[i]);
			cvReleaseImage(&dst[i]);
		}
	

}

long timeS,timeE;

/****************多模板识别***************************/
CString MultiTemplateRecogn(IplImage** charImg,int charnum)
{
	if(charnum==7)
	{
		timeS=GetTickCount();
		Char_Letter *ch1=CharacterRecognition(charImg[0],CHINESES_CHAR);

		float LimitNum=0.05;       //设定字符的区分度

		if (!IsDistinguishChar(ch1,LimitNum))                                    //判定相似区分度是否大于LimitNum
		{
			/*使用多模板进一步识别*/
			ChRecgUseMultiTemplate(charImg[0],ch1,FALSE);
		}


		Char_Letter *ch2=CharacterRecognition(charImg[1],LETTER_CHAR);

		if(IsInCitySub(ch1,ch2))            //检测ch2是否在省市ch1中，不在则取相似度第2大的值，第3大以此推之。
		{	

			if (!IsDistinguishChar(ch2,LimitNum))
			{
				/*使用多模板进一步识别*/
				ChRecgUseMultiTemplate(charImg[1],ch2,FALSE);

			}

			Char_Letter *ch3=CharacterRecognition(charImg[2],LETTER_NUM);

			if (!IsDistinguishChar(ch3,LimitNum))
			{
					/*使用多模板进一步识别*/
				ChRecgUseMultiTemplate(charImg[2],ch3,TRUE);

			}

			Char_Letter *ch4=CharacterRecognition(charImg[3],LETTER_NUM);

			if (!IsDistinguishChar(ch4,LimitNum))
			{
				/*使用多模板进一步识别*/
				ChRecgUseMultiTemplate(charImg[3],ch4,TRUE);

			}

			Char_Letter *ch5=CharacterRecognition(charImg[4],LETTER_NUM);

			if (!IsDistinguishChar(ch5,LimitNum))
			{
				/*使用多模板进一步识别*/
				ChRecgUseMultiTemplate(charImg[4],ch5,TRUE);

			}

			Char_Letter *ch6=CharacterRecognition(charImg[5],LETTER_NUM);

			if (!IsDistinguishChar(ch6,LimitNum))
			{
				/*使用多模板进一步识别*/
				ChRecgUseMultiTemplate(charImg[5],ch6,TRUE);

			}

			Char_Letter *ch7=CharacterRecognition(charImg[6],LETTER_NUM_CHINESE);

			if (!IsDistinguishChar(ch7,LimitNum))
			{
				/*使用多模板进一步识别*/
				ChRecgUseMultiTemplate(charImg[6],ch7,TRUE);

			}


			CString strChar=_T(" ");
			strChar.Format(_T("%s"),ch1->char_name);
			strChar.AppendChar(*ch2->char_name);
			strChar.AppendChar(*ch3->char_name);
			strChar.AppendChar(*ch4->char_name);
			strChar.AppendChar(*ch5->char_name);
			strChar.AppendChar(*ch6->char_name);

			CString str7;                     //第7个字符可能有汉字，使用char* 型容易出现乱码
			str7.Format("%s",ch7->char_name);
			CString strCh=strChar+str7;

			//释放内存

			delete ch1;
			delete ch2;
			delete ch3;
			delete ch4;
			delete ch5;
			delete ch6;
			delete ch7;


			return strCh;
		}
		else 
		{	
			CString s="拒绝识别！";
			return s;
		}

	}
	else 
	{
		CString s="识别错误！";
		return s;
	}



}

CString TempalteFeatureRecogn(IplImage** charImg,int charnum,int FeatureMethod)
{

	InitSrcFreature(charImg,cvSize(WIDTH,HEIGHT));       //提取识别字符的特征值

	if(charnum==7)
	{
		Char_Letter *ch1=CharacterRecognition(charImg[0],CHINESES_CHAR);

		float LimitNum=0.08;       //设定字符的区分度

		if (!IsDistinguishChar(ch1,LimitNum))                                    //判定相似区分度是否大于0.02
		{
			/*使用特征值进一步识别*/
				Similary *Sim=calEuclideanDistance(1,ch1,FeatureMethod);    //计算相似度最大的三个备选的字符的欧氏距离
				ReturnCharByEucDistance(ch1,Sim,FALSE);       //注意ReturnChar与ReturnChar2的使用区别
				 delete [] Sim;
				
		}


		Char_Letter *ch2=CharacterRecognition(charImg[1],LETTER_CHAR);

		if(IsInCitySub(ch1,ch2))            //检测ch2是否在省市ch1中，不在则取相似度第2大的值，第3大以此推之。
		{	

			if (!IsDistinguishChar(ch2,LimitNum))
			{
				/*使用特征值进一步识别*/
					Similary *Sim=calEuclideanDistance(2,ch2,FeatureMethod);	
					ReturnCharByEucDistance(ch2,Sim,FALSE);
					 delete [] Sim;
	
			}

			Char_Letter *ch3=CharacterRecognition(charImg[2],LETTER_NUM);

			if (!IsDistinguishChar(ch3,LimitNum))
			{
				/*使用特征值进一步识别*/
					Similary *Sim=calEuclideanDistance(3,ch3,FeatureMethod);
					ReturnCharByEucDistance(ch3,Sim,TRUE);
					delete [] Sim;

			}

			Char_Letter *ch4=CharacterRecognition(charImg[3],LETTER_NUM);

			if (!IsDistinguishChar(ch4,LimitNum))
			{
				/*使用特征值进一步识别*/
					Similary *Sim=calEuclideanDistance(4,ch4,FeatureMethod);	
					ReturnCharByEucDistance(ch4,Sim,TRUE);
					delete [] Sim;
					
			}

			Char_Letter *ch5=CharacterRecognition(charImg[4],LETTER_NUM);

			if (!IsDistinguishChar(ch5,LimitNum))
			{
				/*使用特征值进一步识别*/
					Similary *Sim=calEuclideanDistance(5,ch5,FeatureMethod);
					ReturnCharByEucDistance(ch5,Sim,TRUE);
					delete [] Sim;

			}

			Char_Letter *ch6=CharacterRecognition(charImg[5],LETTER_NUM);

			if (!IsDistinguishChar(ch6,LimitNum))
			{
				/*使用特征值进一步识别*/
					Similary *Sim=calEuclideanDistance(6,ch6,FeatureMethod);
					ReturnCharByEucDistance(ch6,Sim,TRUE);
					delete [] Sim;

			}

			Char_Letter *ch7=CharacterRecognition(charImg[6],LETTER_NUM_CHINESE);

			if (!IsDistinguishChar(ch7,LimitNum))
			{
				/*使用特征值进一步识别*/
					Similary *Sim=calEuclideanDistance(7,ch7,FeatureMethod);		
					ReturnCharByEucDistance(ch7,Sim,TRUE);
					delete [] Sim;
			}

			CString strChar=_T(" ");
			strChar.Format(_T("%s"),ch1->char_name);
			strChar.AppendChar(*ch2->char_name);
			strChar.AppendChar(*ch3->char_name);
			strChar.AppendChar(*ch4->char_name);
			strChar.AppendChar(*ch5->char_name);
			strChar.AppendChar(*ch6->char_name);

			CString str7;                     //第7个字符可能有汉字，使用char* 型容易出现乱码
			str7.Format("%s",ch7->char_name);
			CString strCh=strChar+str7;

			//释放内存

			delete ch1;
			delete ch2;
			delete ch3;
			delete ch4;
			delete ch5;
			delete ch6;
			delete ch7;


			return strCh;
		}
		else 
		{	
			CString s="拒绝识别！";
			return s;
		}
	}
	else 
	{
		CString s="识别错误！";
		return s;
	}
	

}

//当X，Y取0时，得到重心
double K_OrderMoment(CvMat *I_mat,int OrderX,int OrderY)
{
	int width,height,step;

	width=I_mat->cols;
	height=I_mat->rows;
	step=I_mat->step/sizeof(int);
	int *data=I_mat->data.i;

	double sum=0;

	for (int i=0;i<height;i++)
	{
		for (int j=0;j<width;j++)
		{
			if (data[i*step+j]>80)
			{
               sum+=pow((double)i,OrderX)*pow((double)j,OrderY)*1;
			}
		}
	}

	return sum;
}

double CentralSecondMoment(CvMat *I_mat,int GravityX,int GravityY)
{
	int width,height,step;

	width=I_mat->cols;
	height=I_mat->rows;
	step=I_mat->step/sizeof(int);
	int *data=I_mat->data.i;

	double sum=0;

	for (int i=0;i<height;i++)
	{
		for (int j=0;j<width;j++)
		{
			if (data[i*step+j]>80)
			{
				sum+=pow((double)(i-GravityX),2)+pow((double)(j-GravityY),2)*1;   //计算中心惯性矩
			}
		}
	}

	return sum;
}

double PQ_OrderMoment(CvMat *I_mat,int OrderP,int OrderQ,int GravityX,int GravityY)
{

	int width,height,step;

	width=I_mat->cols;
	height=I_mat->rows;
	step=I_mat->step/sizeof(int);
	int *data=I_mat->data.i;

	double sum=0;

	for (int i=0;i<height;i++)
	{
		for (int j=0;j<width;j++)
		{
			if (data[i*step+j]>80)
			{
				sum+=pow((double)(i-GravityX),OrderP)*pow((double)(j-GravityY),OrderQ)*1;   //计算（p+q）阶中心矩
			}
		}
	}

	return sum;
}

int  FeatureDirect(CvMat *I_mat,int GravityX,int GravityY,int direction)
{
	int width,height,step;

	width=I_mat->cols;
	height=I_mat->rows;
	step=I_mat->step/sizeof(int);
	int *data=I_mat->data.i;

	int sum=0;
switch (direction)
{
case 1:{
	for (int i=0;i<height;i++)
	{	
			if (data[i*step+GravityY]>80)
			{
				sum+=1;   //计算（p+q）阶中心矩
			}
		
	}

	return sum;
	    }
	break;
case 2:{
		for (int j=0;j<width;j++)
		{
			if (data[GravityX*step+j]>80)
			{
				sum+=1;  
			}
		}
	

	return sum;
	   }
	break;
case 3:{
	for (int i=0;i<height/2;i++)
	{
		for (int j=0;j<width;j++)
		{
			if (data[i*step+j]>80)
			{
			//	sum+=pow((double)(i-GravityX),OrderP)*pow((double)(j-GravityY),OrderQ)*1;   //计算（p+q）阶中心矩
			}
		}
	}

	return sum;
	   }
	break;
case 4:{
	for (int i=0;i<height;i++)
	{
		for (int j=0;j<width;j++)
		{
			if (data[i*step+j]>80)
			{
				//sum+=pow((double)(i-GravityX),OrderP)*pow((double)(j-GravityY),OrderQ)*1;   //计算（p+q）阶中心矩
			}
		}
	}

	return sum;
	   }
	break;
}
	
}

CString FeatureTempalteRecogn(IplImage** charImg,int charnum,int FeatureMethod)
{
	InitSrcFreature(charImg,cvSize(WIDTH,HEIGHT));

	Similary *Simi=new Similary[7];
    double **maxVal=new double*[7];
	int **markVal=new int *[7];
	Char_Letter *ch=new Char_Letter[7];

	for (int i=0;i<7;i++)
	{

        maxVal[i]=new double[7];
		markVal[i]=new int[7];
		//ch[i]=new Char_Letter;
	}



for (int i=0;i<7;i++)
{
		cvInitMatHeader(&S_mat[i],2,8,CV_32FC1,SrcchGridVector[i]->featureGrid);
}

for (int i=0;i<73;i++)
{
	cvInitMatHeader(&T_mat[i],2,8,CV_32FC1,chGridVector[i]->featureGrid);
}


IplImage *img_Re=cvCreateImage(cvSize(1,1),IPL_DEPTH_32F,1);    //由于已将源图像归一化为模板大小，故cvSize(W-w+1,H-h+1)=cvSize(1,1)

double minval,maxval;  //图像比较的最大值、最小值


for (int i=0;i<7;i++)
{
	for (int j=0;j<73;j++)
	{
		    cvMatchTemplate(&S_mat[i],&T_mat[j],img_Re,CV_TM_CCOEFF_NORMED);
			cvMinMaxLoc(img_Re,&minval,&maxval);       //找出相似的最大、最小值，此处有意义的为最大值

			Simi[i].simi[j]=maxval;                      //将最大相似值保存在数组中

			maxval=0.0;
			minval=0.0;
			cvZero(img_Re);
	}
	
}

for (int i=0;i<7;i++)
{
        maxVal[i]=GetMaxValFrmArray(Simi[i].simi,73,7);      //取最大值
		//int MarkVal[7];
		for (int j=0;j<7;j++)
		{
			markVal[i][j]=GetMarkFrmArray(Simi[i].simi,73,maxVal[i][j]);      //取最大值对应的下标
		}
}
		

for (int i=0;i<7;i++)
{
	for (int j=0;j<7;j++)
	{
		ch[i].max_val[j]=maxVal[i][j];
		ch[i].mark_max[j]=markVal[i][j];
	}

}


float LimitNum=1;       //设定字符的区分度


for (int i=0;i<7;i++)
{
		
	ch[i].char_name=ReturnChar(ch[i].mark_max[0]);
		
		

		if (!IsDistinguishChar(&ch[i],LimitNum))                                    //判定相似区分度是否大于LimitNum
		{
			/*使用多模板进一步识别*/
				ChRecgUseMultiTemplate(charImg[i],&ch[i],FALSE);
		}
		
		   
}


	

	/*
	CString strChar=_T(" ");
		strChar.Format(_T("%s"),ch[0]->char_name);
		strChar.AppendChar(*ch[1]->char_name);
		strChar.AppendChar(*ch[2]->char_name);
		strChar.AppendChar(*ch[3]->char_name);
		strChar.AppendChar(*ch[4]->char_name);
		strChar.AppendChar(*ch[5]->char_name);
	
		CString str7;                     //第7个字符可能有汉字，使用char* 型容易出现乱码
		str7.Format("%s",ch[6]->char_name);
		CString strCh=strChar+str7;*/

	CString strChar[7];
	CString strCh;
	for (int i=0;i<7;i++)
	{
		strChar[i].Format("%s",ch[i].char_name);
		strCh+=strChar[i];
	}
	//=strChar[0]+strChar[1]+strChar[2]+strChar[3]+strChar[4]+strChar[5]+strChar[6];


	for (int i=0;i<7;i++)
	{
//		delete [] Simi;
		delete [] maxVal[i];
		delete [] markVal[i];
//		delete [] ch;
	}


return strCh;
}

/*    函数：CalBarycenter   
功能：计算重心矩    
参数：pdata -- 图像数据 in

nwidth -- 宽 in              
nheight -- 高 in              
nwidthstep -- 步长 in             
nBarycenterX -- 重心坐标 out             
nBarycenterY   
*/
void CalBarycenter(IplImage  *img,int &nBarycenterX , int &nBarycenterY)
{        


	//unsigned char * pdata , 
	int nwidth =img->width;
	int nheight=img->height;
	int nwidthstep=img->widthStep/sizeof(int);

	uchar* pdata=(uchar*)img->imageData;


	double m00 , m01 ,m10;         
	m00 = 0 ;        
	m01 = 0 ;        
	m10 = 0 ;        
	for (int i = 0 ; i < nheight ; ++i)        
	{            
		for (int j = 0 ; j < nwidth ; ++ j)	            
		{                
			m00 +=*(pdata + i * nwidthstep + j) ;                 
			m01 +=*(pdata + i * nwidthstep + j) * j ;                
			m10 +=*(pdata + i * nwidthstep + j) * i ;             
		}        
	}        
	nBarycenterX =(int) (m10 / m00+0.5);        
	nBarycenterY = (int)(m01/ m00+0.5);
	//nBarycenterX=0;
	//nBarycenterY=0;

}
/*    
函数：CalCenterMoment    
功能：计算中心矩    
参数：pdata --- 图像数据 in              
nwidth -- 宽 in              
nheight -- 高 in              
nwidthstep -- 步长 in              
nBarycenterX -- 重心矩 in              
nBarycenterY               
ip -- 阶数 in              
jq      
返回值：中心距值*/
double CalCenterMoment(IplImage *img,double nBarycenterX , double nBarycenterY,int ip,int jq)
{    


	//unsigned char * pdata , 
	int nwidth=img->width;
	int nheight=img->height;
	int nwidthstep=img->widthStep/sizeof(int);

	uchar *pdata=(uchar*)img->imageData; 


	double Upq = 0 ;     
	for (int i = 0 ; i < nheight ; ++i)    
	{        
		for (int j=0; j <nwidth ; ++ j)        
		{            
			Upq +=*(pdata + i* nwidthstep + j) * pow(j -nBarycenterX ,ip) * pow(i - nBarycenterY , jq) ;        
		}    
	}    
	return Upq ; 
}


double DirectionCharacteristic(IplImage *img,int nBarycenterX , int nBarycenterY,int Direction)
{
	int width=img->width;
	int height=img->height;
	int step=img->widthStep/sizeof(int);

	uchar* pdata=(uchar*)img->imageData;

	switch (Direction)
	{
	case 1:{
		double m1=0.0,s1=0.0;

		for (int i=0;i<height;i++)
		{
			m1+=*(pdata+i*step+nBarycenterY)*pow((double)i-nBarycenterX,2);
			s1+=*(pdata+i*step+nBarycenterY);
		}
		return m1/s1;

		   }
		   break;
	case 2:{

		double m2=0.0,s2=0.0;
		for (int j=0;j<width;j++)
		{
			m2+=*(pdata+nBarycenterX*step+j)*pow((double)j-nBarycenterY,2);
			s2+=*(pdata+nBarycenterX*step+j);
		}

		return m2/s2;
		   }
		   break;
	case 3:{

		double m3=0.0,s3=0.0;
		for (int k=0;k<height/2;k++)
		{
			m3+=2*pow((double)k,2)*(*(pdata+(nBarycenterX+k)*step+nBarycenterY-k)+*(pdata+(nBarycenterX-k)*step+nBarycenterY+k));
			s3+=*(pdata+(nBarycenterX+k)*step+nBarycenterY-k)+*(pdata+(nBarycenterX-k)*step+nBarycenterY+k);
		}

		return m3/s3;
		   }
		   break;
	case 4:{

		double m4=0.0,s4=0.0;

		for (int k=0;k<height/2;k++)
		{
			m4+=2*pow((double)k,2)*(*(pdata+(nBarycenterX+k)*step+nBarycenterY+k)+*(pdata+(nBarycenterX-k)*step+nBarycenterY+k));
			s4+=*(pdata+(nBarycenterX+k)*step+nBarycenterY+k)+*(pdata+(nBarycenterX-k)*step+nBarycenterY+k);
		}

		return m4/s4;

		   }
		   break;
	}
}

charVector *GetFeatureByMomment(IplImage *img)
{

	charVector *vect=new charVector;

	int nwidth ,nheight , nwidthstep;
	nwidth=img->width;
	nheight=img->height;
	nwidthstep=img->widthStep/sizeof(int);

	//uchar* Sdata=(uchar*)src->imageData;
	//uchar* Tdata=(uchar*)temp->imageData;

	//原图和模板重心矩
	int nBarycenterX , nBarycenterY;
	CalBarycenter(img,nBarycenterX,nBarycenterY);

	//原图和模板二阶三阶规格化中心矩
	double u00 ,u02 ,u20 ,u11,u22,x1,x2,x3,x4; 

	u00 = CalCenterMoment(img ,nBarycenterX,nBarycenterY,0,0); 
	u02 = CalCenterMoment(img ,nBarycenterX,nBarycenterY,0,2);
	u20 = CalCenterMoment(img ,nBarycenterX,nBarycenterY,2,0);
	u11 = CalCenterMoment(img ,nBarycenterX,nBarycenterY,1,1); 
	u22 =CalCenterMoment(img ,nBarycenterX,nBarycenterY,2,2); 
	x1=DirectionCharacteristic(img,nBarycenterX,nBarycenterY,1);
	x2=DirectionCharacteristic(img,nBarycenterX,nBarycenterY,2);
	x3=DirectionCharacteristic(img,nBarycenterX,nBarycenterY,3);
	x4=DirectionCharacteristic(img,nBarycenterX,nBarycenterY,4);

	double Sa[7];
	Sa[0]=x1;
	Sa[1]=x2;
	Sa[2]=x3;
	Sa[3]=x4;
	Sa[4]=u11/u22;
	Sa[5]=u20/u22;
	Sa[6]=u02/u22;

for (int i=0;i<7;i++)
{
	vect->momentFeature[i]=Sa[i];
}
	
	return vect;
}

charVector *GetFeatureByMomment(CvMat *mat)
{
	IplImage *img=cvCreateImage(cvGetSize(mat),8,1);
	cvGetImage(mat,img);
	charVector *v=new charVector;
    v=GetFeatureByMomment(img);
	return v;

}