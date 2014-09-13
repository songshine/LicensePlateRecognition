#include "stdafx.h"
#include "LPLocation.h"
#include "LPHelper.h"
#include <malloc.h>

#define MIN_LP_REGION 1000
#define MAX_LP_REGION 30000
#define DEFAULT_IMAGE_WIDTH 720
#define DEFAULT_IMAGE_HEIGHT 480
#define MAX_LP_WIDTH 324
#define MAX_LP_HEIGHT 124
#define MIN_LP_WIDTH 88
#define MIN_LP_HEIGHT 12
#define MAX_PIC_AREA  20000000

#define  SPACEING 40
#define BELLOW_DETECT_DIS 28    //车牌距离底部的最小距离
#define TOP_DETECT_DIS 20   //车牌距离顶部的最小距离

//车牌的相关信息
int g_lpMinHeight = MIN_LP_HEIGHT;   //车牌最小高度
int g_lpMinWidth = MIN_LP_HEIGHT;    //车牌小宽度

//车牌尺寸和位置信息
int g_bInit = 0;
lp_size_info g_lsi;    //车牌大小限制信息
lp_pos_info  g_lpi;		//车牌位置限制信息


//调试开关
#define SHOW_FLAG  0
#define SHOW_FLAG_OTHER 0
#define SHOW_BORDER 0
/*车牌定位步骤:
1.图像预处理,主要利用高斯滤波去除噪声
2.图像TOP-HAT操作,从而去除光照不均匀的影响
3.图像二值化,采用全局二值化方法,阈值的选择根据当前像素均值动态确定
4.从二值化图中检测出边缘，边缘主要是水平方向边缘
5.从边缘图中寻找边缘比较密集的区域，
6.对边缘密集的区域进行形态学操作，从而得到多个连通区域
7.从连通域图中检测出车牌,检测车牌规则rect应该满足的条件为:
	1).rect对应的边缘图存在蓝色或者黄色的边缘
	2).rect中存在较大的光照对比度
8.经过以上操作可以精确得到车牌的上下边界，但是左右边界还有待确定
我们做如下操作：
	1)对包含车牌的一个小区域进行车牌旋转
	2)对上下边界确定左右不确定的车牌区域进行处理，膨胀得到连通域，接着利用步骤7进行车牌再次检测
	3)利用颜色信息再次精确定位车牌
*/

//用于统计颜色分布情况
struct colorD
{
	int color[4];//记录四种颜色
	int nb_w;
	int ny_b;
};

//车牌区域
struct LPR
{
	CvRect rect;
	int color;
	IplImage* img_color;
};

//输出调试信息
#ifdef _DEBUG 
void XdPrint(char* format, ...) 
{ 
	va_list args; 
	va_start(args, format); 
	_TCHAR buf[256]; 
	_vstprintf(buf, format, args); 
	OutputDebugString(buf); 
	va_end(args); 
} 
#else 
void XdPrint(_TCHAR* format, ...){} 
#endif 
/**********************************局部函数声明*************************/

/*首先车牌初步定位,返回矩形位置在rect，车牌位置的灰度图放在返回图像中*/
int LPCoarseLocate(const IplImage* src,CvRect** lp_list);
//获取所有的车牌候选区域
int GetLPCandidate(const IplImage* img_gray,CvRect** lp_list);
/*精确定位*/
IplImage* LPPreciseLocate(const IplImage* pic,struct LPR lpr); 
//从img中获取所有可能的车牌位置
int DetectLPAreas(const IplImage* img,CvRect** lp_list);

//从img中获取所有可能车牌位置版本2
int DetectLPAreasVer2(const IplImage* img,CvRect** lp_list);
//获得所有候选连通域
int GetLpcList(const IplImage* pic,CvRect** lpc_list);
//从多个候选车牌区域确定车牌
//返回车牌对应id
struct LPR* DetermineLPArea(const IplImage* img,CvRect* lp_list,int lp_num);

//灰度图方差投影
void ImageVPF(const IplImage* img_gray);
//进一步根据边界确定车牌区域的左右边界和上下边界,，返回0出错
int ImproveLPAreaByEdge(const IplImage* lp_img_edge,CvRect* rect);
//根据二值化图进一步确定边界图
int ImproveLPAreaByBinaryImg(const IplImage* lp_img_binary,CvRect* rect);

//根据车牌颜色确定左右边界
IplImage* ImproveLPAreaByColor(const IplImage* lp_img,CvRect in_rect,int color);

//精确定位车牌位置
int RectifyLPR(const IplImage* lp_gray_img,CvRect* out_rect);
//根据边缘图找到上下边界
void FindEdgeImageBorderUD(const IplImage* img_edge,int *u,int* d);
//根据边缘图找到左右边界
void FindEdgeImageBorderLR(const IplImage* img_edge, int* l,int* r);
//扫描边缘图，返回一个车牌所在的区域,返回车牌可能在的行，对应flag值为1
void ScanEdgeImage(const IplImage* img_edge,unsigned* flag);
//
void ResizeRect(struct colorD* horColor,struct colorD* verColor,
				CvRect* pRect,int color);

//寻找RGB图的倾斜角,threshold表明最小线段长度阈值
//hor=1表示求水平方向倾斜角,hor=0表示求竖直方向倾斜角
float FindAngel(const IplImage* rgb_img,int threshold = 10,int hor=1);
//根据角度进行旋转，角度为水平正方向,angle为弧度
void ImageRotate(const IplImage* src,IplImage* dst,
				double angle,CvScalar fillVal = cvScalarAll(0),
				int hor = 1);
//将src_lpr中的rect区域的图片进行旋转操作
IplImage* LPRotateFromRect(const IplImage* src_lpr,
						   CvRect in_rect,CvRect* out_rect);

//返回边缘图
void ReturnEdgeImage(const IplImage* rgb_src,IplImage* edge_dst);

//求微分图
void ReturnSobelImage(const IplImage* gray_src,IplImage* sobel_dst,int dx,int dy);
//从RGB转换成HSV
void Rgb2Hsv(float r,float g,float b,float* h,float* s,float* v);
//根据HSV三个分量值确定颜色
int ReturnColor(float valH,float valS,float valV);
//获取颜色图中某一颜色的
void ReturnMeanColorHSV(const IplImage* img,const IplImage* img_color,
					
					float* h,float* s,float* v,int color);
//返回连通域的个数
int ReturnCRNum(const IplImage* bwImage);
//求两个hsv像素点之间的距离
float ReturnHsvDist(float h0,float s0,float v0,
					float h1,float s1,float v1);
//从bgr中获取一个像素的hsv值
void GetHsvValFromBgr(const IplImage* img_rgb,int row,int col,float* h,float* s,float* v);
//获取颜色图
IplImage* ReturnColorImage(const IplImage* img);
//统计HSV图中蓝、黄、白、黑四种颜色所占比例 
void StatisticsHSV(const IplImage* img,float* rb,float* ry,float* rw,float* rbk);
//统计统计HSV图中蓝、黄、白、黑四种颜色的分布情况
IplImage* StatisticsColorDistribution(const IplImage* img,struct colorD* horColor,
							struct colorD* verColor);
//返回颜色的边界的像素的个数
int ReturnEdgeWithColor(const IplImage* img_color,const IplImage* img_edge,int color);
//返回车牌颜色
int ReturnLPColor(const IplImage* img_color);
//计算候选车牌候选区域权值,color为-1时需要通过rgb_img,当颜色color不为空时返回颜色值
float ReturnLPCWeight(const IplImage* rgb_img,const IplImage* img_color = NULL,int* color = NULL);
//返回边界处的灰度图的差的值
float ReturnEdgeWithGrayLevel(const IplImage* img_edge,const IplImage* img_gray,int color);
//获取每一行颜色变化次数
int ReturnColorDiffers(const IplImage* img_color,int color);

//计算方差
void GetVariance(struct colorD* verColor,int h,int color,float* avg,float* var);

//利用膨胀调整蓝色车牌矩形区域大小
IplImage* ResizeImageByDilate(const IplImage* img);

//根据颜色精确定位车牌区域
IplImage* ResizeLPR(const IplImage* pic,CvRect* rect,int color);
void ResizeLPREx(const IplImage* img,CvRect* rect,int color);
void ThresholdAccordColor(const IplImage* src,IplImage* dst,int color);
//获取每一行非零像素的个数
void GetHorStatistics(const IplImage* img,float* avg,float* var);

//获取列中含像素的个数
void GetVerStatistcs(const IplImage* img,float* avg,float* var);
//获取图像中某一矩形内的边缘像素的个数
void GetRectEdgeImageStatistics(const IplImage* img_edge,CvRect rect,float* avg,float* var);

//获取灰度图中像素为val的个数
long GetNumFromGrayImage(const IplImage* img_gray,int val);

//获得颜色分布图
IplImage* ColorDistribution(const IplImage* img);
//根据统计的颜色分布情况进行分析
struct colorD*  AnalyseColorDistribution(const IplImage* colorImg);

/*水平二次差分
即I(x,y) = I(x,y)-I(x,y-2)*/
void TwiceDiff(const IplImage* src,IplImage* dst);
//对灰度图求差分图
void ImageDiff(const IplImage* src,IplImage* dst,int threshold=10);
//消除过长或过短的边缘线
void ImageCnp(const IplImage* src,IplImage* dst);
//图像增强
void ImageStretch(const IplImage* src,IplImage* dst);

int RectArea(const CvRect rt) {return rt.width*rt.height;}
//根据边缘密度确定车牌区域
IplImage* FindLprAboutEdgeDensity(const IplImage* img_edge);

void FreeLPR(struct LPR** lpr)
{
	if((*lpr) != NULL)
	{
		if((*lpr)->img_color != NULL)
			cvReleaseImage(&(*lpr)->img_color);
		LPRFree(*lpr);
	}
}
void ShowColorImage(const IplImage* img_color)
{
	IplImage* img = cvCloneImage(img_color);
	IplImage* tmp = cvCloneImage(img_color);
	cvZero(tmp);
	cvAddS(tmp,cvScalar(50),tmp);
	cvMul(img_color,tmp,img);
	//cvOrS(img,cvScalar(32),img);

	cvShowImage("img_color",img);


	cvReleaseImage(&img);
	cvReleaseImage(&tmp);

}
/************************************************************************/
/*                         函数定义                                     */
/************************************************************************/

/*车牌定位,定位成功返回车牌，以及车牌位置放在rect中*/
IplImage* LPLocate(const IplImage* src,CvRect* rect,int* color)
{
	int bflagG = 0;
	/*车牌的定位分成一下两个步骤:
	1.车牌的初步定位，即先定位车牌在整个图像中的大致位置和区域,可能有多个不确定区域;
	2.然后对1中初步得到的区域进行分析，得到车牌的精确位置*/
	IplImage* img = NULL;
	IplImage* imgLPP = NULL;	//精确定位结果	
	struct LPR* lpr = NULL;
	CvRect* lpc_list = NULL ;
	int lp_num;	
	
	if(src->width * src->height > MAX_PIC_AREA)
	{
		bflagG = 1;
		img = GetRectImage(
			src,cvRect(0,src->height/2,
			src->width,src->height/2)
			);
	}
	else
	{
		img = cvCloneImage(src);
	}
	

	//获取所有车牌候选区域
	lp_num = LPCoarseLocate(img,&lpc_list);

	//从车牌候选区域中找出车牌所在的区域
	if(0 != lp_num)
		lpr = DetermineLPArea(img,lpc_list,lp_num);
	
	//精确定位
	if(lpr != NULL)
	{
		IplImage* small_img = NULL;
		IplImage* lpr_pic = NULL;
		CvRect new_rect;
		
		//对图像进行旋转
		small_img = LPRotateFromRect(img,lpr->rect,&new_rect);		

		//精确定位
		{
			struct LPR lpr_tmp;
			lpr_tmp.color = lpr->color;
			lpr_tmp.rect = new_rect;
			imgLPP = LPPreciseLocate(small_img,lpr_tmp);	
		}
			
		*color = lpr->color;
		*rect = lpr->rect;
		if(bflagG)
			rect->y += img->height; 
	
		cvReleaseImage(&small_img);
		cvReleaseImage(&lpr_pic);
	}
	
	cvReleaseImage(&img);
	FreeLPR(&lpr);
	LPRFree(lpc_list);

	return imgLPP;
}
int LPCoarseLocate(const IplImage* src,CvRect** lp_list)
{
	int ret = 0;  //最终返回值
	
	//CvRect* lp_list = NULL;
	//寻找LP边缘
	IplConvKernel* element0 = NULL;	

	IplImage* img_gray = NULL;  //灰度图
	IplImage* img_foreground = NULL;  //顶帽操作后的前景图
	IplImage* img_bw = NULL;   //二值化图
	IplImage* img_canny = NULL;  //canny边界图
	IplImage* img_mo = NULL;  //形态学操作之后的图像
	//每一个矩形区域的连通域数量
	int rect_num = 0,max_id = -1;
	int szElem = /*min(20,g_img->height/20)*/14;
	element0 = cvCreateStructuringElementEx
				(
				szElem,1,szElem/2,0,
				CV_SHAPE_RECT,NULL
				);
	//为了进行图片处理分配必要的空间
	img_gray = cvCreateImage(cvGetSize(src),IPL_DEPTH_8U,1);
	img_foreground = cvCreateImage(cvGetSize(src),IPL_DEPTH_8U,1);
	img_canny = cvCreateImage(cvGetSize(src),IPL_DEPTH_8U,1);
	img_bw = cvCreateImage(cvGetSize(src),IPL_DEPTH_8U,1);
	//转换成灰度图
	cvCvtColor(src,img_gray,CV_BGR2GRAY); 

	
	//中值滤波
	cvSmooth(img_gray,img_gray,CV_MEDIAN,3);

	
	/*1.顶帽操作*/
	cvMorphologyEx(
		img_gray,img_foreground,
		NULL,element0,CV_MOP_TOPHAT,1
			);

	
	{//求边缘图
		double meanPix = cvMean(img_foreground);
		cvThreshold(img_foreground,img_bw,10+meanPix,255,CV_THRESH_BINARY);
		SmoothBinImg(img_bw,22,SMOOTH_WHITE_CC);
		TwiceDiff(img_bw,img_canny);
		ImageCnp(img_canny,img_canny);
		//ReturnSobelImage(img_bw,img_canny,1,0);

		
	}
	//对边界密集的区域进行处理
	img_mo = FindLprAboutEdgeDensity(img_canny);
	//LengthFilter(img_mo,img_mo,MIN_LP_WIDTH-20,MAX_LP_WIDTH);
	
	//找出所有可能的区域
	rect_num = DetectLPAreas(img_mo,lp_list);

	if(SHOW_FLAG)
	{
		cvNamedWindow("show_img",0);
		cvResizeWindow("show_img",720,480);

		cvShowImage("show_img",img_gray);
		cvWaitKey(0);

		cvShowImage("show_img",img_foreground);
		cvWaitKey(0);

		cvShowImage("show_img",img_bw);
		cvWaitKey(0);

		cvShowImage("show_img",img_canny);
		cvWaitKey(0);

		cvShowImage("show_img",img_mo);
		cvWaitKey(0);
	}

	cvReleaseStructuringElement(&element0);
	cvReleaseImage(&img_mo);
	cvReleaseImage(&img_gray);
	cvReleaseImage(&img_foreground);
	cvReleaseImage(&img_canny);
	cvReleaseImage(&img_bw);
	//返回车牌候选区域
	return rect_num;
}


//返回相交矩形
CvRect CrossRect(CvRect rt1,CvRect rt2)
{

	double minx,miny;
	double maxx,maxy;
	minx = max(rt1.x,rt2.x);  
	miny = max(rt1.y,rt2.y);  
	maxx = min(rt1.x+rt1.width,rt2.x+rt2.width);  
	maxy = min(rt1.y+rt1.height,rt2.y+rt2.height); 

	if ( minx>maxx ) return cvRect(0,0,0,0);
	if ( miny>maxy ) return cvRect(0,0,0,0);

	return cvRect(minx,miny,maxx-minx,maxy-miny);

}
/*精确定位*/
IplImage* LPPreciseLocate(const IplImage* pic,struct LPR lpr)
{
	//精确定位
	IplImage* imgLPP = NULL;   //最终返回的车牌精确位置
	IplImage* img_gray = NULL;
	IplImage* img_bw = NULL;
	IplImage* img_edge = NULL;
	IplImage* lpr_pic = NULL;
	IplImage* img_back = NULL;

	
	//根据颜色进行定位
	lpr_pic = ImproveLPAreaByColor(pic,lpr.rect,lpr.color);
	
	//////////////////
	{
		CvSize size= cvGetSize(lpr_pic);
		img_gray = cvCreateImage(size,IPL_DEPTH_8U,1);
		img_bw = cvCreateImage(size,IPL_DEPTH_8U,1);
		img_edge = cvCreateImage(size,IPL_DEPTH_8U,1);
		img_back = cvCreateImage(size,IPL_DEPTH_8U,1);


		/////
		{
			//double meanPix ;

			cvCvtColor(lpr_pic,img_gray,CV_BGR2GRAY);
			cvSmooth(img_gray,img_gray,CV_MEDIAN);
			
			/*ReturnSobelImage(img_gray,img_back,1,0);
			cvShowImage("img_sobel",img_back);
			cvWaitKey(0);
			ReturnSobelImage(img_gray,img_back,0,1);
			cvShowImage("img_sobel",img_back);
			cvWaitKey(0);*/
			//灰度图拉伸
			GrayStretch(img_gray,img_gray);

			/*if(lpr.color == COLOR_BLUE || lpr.color == COLOR_BLACK)
				cvThreshold(img_gray,img_bw,255,255,CV_THRESH_BINARY|CV_THRESH_OTSU);
			else
				cvThreshold(img_gray,img_bw,255,255,CV_THRESH_BINARY_INV |CV_THRESH_OTSU);
			
		

			cvCanny(img_bw,img_edge,50,150);*/
			

		}
	}
	
	{//根据边缘进行再次精确定位
		CvRect rect_ret,rect_ret1;
		/*ImproveLPAreaByBinaryImg(img_bw,&rect_ret1);
		ImproveLPAreaByEdge(img_edge,&rect_ret);*/
		//rect_ret = CrossRect(rect_ret,rect_ret1);		
		RectifyLPR(img_gray,&rect_ret);
		//ImproveLPAreaByEdge(img_edge,&rect_ret);
		if(rect_ret.width*rect_ret.height != 0)
			imgLPP = GetRectImage(lpr_pic,rect_ret);
		else
			imgLPP = GetRectImage(pic,lpr.rect);
	}
	

	cvReleaseImage(&lpr_pic);
	cvReleaseImage(&img_gray);
	cvReleaseImage(&img_bw);
	cvReleaseImage(&img_edge);
	cvReleaseImage(&img_back);

	return imgLPP;

}

//从多个候选车牌区域确定车牌
//返回车牌对应id
struct LPR* DetermineLPArea(const IplImage* img,CvRect* lp_list,int lp_num)
{
	float w,max_w = -1;
	int max_id = -1;
	int color = 0,color_min;
	struct LPR* lpr = NULL;
	IplImage** img_rects ;
	IplImage** img_color;
	if(lp_num == 0) return NULL;
	
	//开始计算
	{
		//分配内存
		img_rects = (IplImage**)malloc(sizeof(IplImage*)*lp_num);
		img_color = (IplImage**)malloc(sizeof(IplImage*)*lp_num);
		if(1 == lp_num)
		{
			max_id = 0;
			img_rects[0] = GetRectImage(img,lp_list[0]);
			img_color[0] = ReturnColorImage(img_rects[0]);
			color_min = ReturnLPColor(img_color[0]);
		}
		else
		{
			//计算各个候选区域的权值
			for(int i=0;i<lp_num;i++)
			{  

			
				img_rects[i] = GetRectImage(img,lp_list[i]);
				img_color[i] = ReturnColorImage(img_rects[i]);
				/*ShowColorImage(img_color[i] );
				cvShowImage("img",img_rects[i]);
				cvWaitKey(0);*/
				//分别记录水平和竖直的像素分布
				w = ReturnLPCWeight(img_rects[i],img_color[i],&color)/**((float)lp_list[i].y/img->height)*/;
				if(w > max_w || (w > max_w* 0.6 && lp_list[i].y>lp_list[max_id].y))
				{
					max_w = w;
					max_id = i;
					color_min = color;
				}			
			}
		}
		

	}

	if(max_id != -1)
	{
		
		lpr = (struct LPR*)malloc(sizeof(struct LPR));		
		lpr->color = color_min;	
		lpr->rect = lp_list[max_id];
		lpr->img_color = cvCloneImage(img_color[max_id]);
	}
	
	{
		//释放内存
		int i;
		for(i=0;i<lp_num;i++)
		{
			cvReleaseImage(&img_rects[i]);
			cvReleaseImage(&img_color[i]);
		}

		LPRFree(img_rects);
		LPRFree(img_color);
	}
	
	return lpr;
}
//从img中获取所有可能的车牌位置
int DetectLPAreas(const IplImage* img,CvRect** lp_list)
{
	int i;
	//根据连通域定位车牌区域
	int mode = CV_RETR_EXTERNAL;	//轮廓边缘提取时的参数
	CvSeq * contour = 0;		//轮廓边缘提取时的参数
	CvRect bndRect = cvRect(0,0,0,0);//用cvBoundingRect画出外接矩形时需要的矩形
	CvMemStorage * storage = cvCreateMemStorage(0);//轮廓边缘提取时的参数
	IplImage* img_mo = cvCloneImage(img);
	int contourNum = cvFindContours(img_mo, storage, &contour, sizeof(CvContour),
		mode, CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0));
	int rect_num = 0;  //最终返回的矩形个数
	(*lp_list) = (CvRect*)malloc(sizeof(CvRect)*contourNum);
	for(;contour!=0;contour=contour->h_next)
	{	
		//float avg,var;
		double area = fabs(cvContourArea(contour));
		//得到一个可以将移动轮廓包围的矩形区域
		bndRect = cvBoundingRect(contour,0);
		if(bndRect.height <= 8 || area/bndRect.width/bndRect.height < 0.4)
			continue;
		//去除面积很小的区域
		if(bndRect.height*bndRect.width < MIN_LP_REGION)
			continue;
		////去除面积太大的区域
		if(bndRect.height*bndRect.width > MAX_LP_REGION )
			continue;
		if(bndRect.width > MAX_LP_WIDTH)
			continue;
		if(bndRect.height > MAX_LP_HEIGHT)
			continue;
		if(bndRect.y <  TOP_DETECT_DIS|| 
			abs(bndRect.y+bndRect.height-img->height) < BELLOW_DETECT_DIS)
			continue;
		////去除宽高比不合理的区域
		if((double)bndRect.width/bndRect.height < 2 ||
			(double)bndRect.width/bndRect.height > 10.4)
			continue;
		if(bndRect.width < (5*bndRect.height-(bndRect.height*bndRect.height)/10))
			continue;
		//bndRect.x = bndRect.x<8?0:bndRect.x-8;
		bndRect.y = bndRect.y<2?0:bndRect.y-2;
		/*bndRect.width = bndRect.width+8+bndRect.x > 
						img->width?
						img->width-bndRect.x:
						bndRect.width+8;*/
		bndRect.height = bndRect.height+4+bndRect.y > 
						img->height?
						img->height-bndRect.y:
						bndRect.height+4;

		//按照面积大小降序排列
		for(i=rect_num-1;i>=0;i--)
		{
			if(RectArea(bndRect) > RectArea((*lp_list)[i]))
				(*lp_list)[i+1] = (*lp_list)[i];
			else 
				break;

		}
		(*lp_list)[i+1] = bndRect;
		rect_num++;
	}
	{
		//删除面积比最大面积的三分之一还小的矩形
		
		for(i=rect_num-1;i>0;i--)
		{
			if(RectArea((*lp_list)[i]) < RectArea((*lp_list)[0])/6.4
				|| (*lp_list)[i].width < (*lp_list)[i].width*0.38)
				rect_num -- ;
				
		}
		

	}
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&img_mo);
	if(rect_num == 0)
	{
		LPRFree(*lp_list);
	}
	return rect_num;
}
void DetectLPBorder(const IplImage* cnn_img,CvRect rect,int* l,int* r)
{
	int i,j,t;
	int ll,rr;
	float thres = 4.2;
	IplImage* img = GetRectImage(cnn_img,rect);
	int iWidth = img->width;
	int iHeight = img->height;
	/*cvShowImage("img",img);
	cvWaitKey(0);*/
	ll = 0;
	for(i=0;i<iWidth/2;i++)
	{
		t = 0;
		for(j=0;j<iHeight;j++)
			t += CV_IMAGE_ELEM(img,uchar,j,i)/255;
		if( t < iHeight/2)
			ll++;
	}
	rr = 0;
	for(i=iWidth;i>iWidth/2;i--)
	{
		t = 0;
		for(j=0;j<iHeight;j++)
			t += CV_IMAGE_ELEM(img,uchar,j,i)/255;
		if( t < iHeight/2)
			rr++;
	}

	if((iWidth - ll - rr)/(float)iHeight < thres)
	{
		*l = 0;
		*r = 0;

	}
	else
	{
		*l = ll;
		*r = rr;
	}
	cvReleaseImage(&img);
}
//从img中获取所有可能车牌位置版本2
int DetectLPAreasVer2(const IplImage* img,CvRect** lp_list)
{
	int i;
	//根据连通域定位车牌区域
	int mode = CV_RETR_EXTERNAL;	//轮廓边缘提取时的参数
	CvSeq * contour = 0;		//轮廓边缘提取时的参数
	CvRect bndRect = cvRect(0,0,0,0);//用cvBoundingRect画出外接矩形时需要的矩形
	CvMemStorage * storage = cvCreateMemStorage(0);//轮廓边缘提取时的参数
	IplImage* img_mo = cvCloneImage(img);
	int contourNum = cvFindContours(img_mo, storage, &contour, sizeof(CvContour),
		mode, CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0));
	int rect_num = 0;  //最终返回的矩形个数
	(*lp_list) = (CvRect*)malloc(sizeof(CvRect)*contourNum);
	for(;contour!=0;contour=contour->h_next)
	{	
		//float avg,var;
		int l,r;
		double area = fabs(cvContourArea(contour));
		//得到一个可以将移动轮廓包围的矩形区域
		bndRect = cvBoundingRect(contour,0);
		if(bndRect.height <= 8 || area/bndRect.width/bndRect.height < 0.4)
			continue;
		//去除面积很小的区域
		if(bndRect.height*bndRect.width < MIN_LP_REGION)
			continue;
		////去除面积太大的区域
		if(bndRect.height*bndRect.width > MAX_LP_REGION )
			continue;
		if(bndRect.width > MAX_LP_WIDTH)
			continue;
		if(bndRect.height > MAX_LP_HEIGHT)
			continue;
		////去除宽高比不合理的区域
		if((double)bndRect.width/bndRect.height < 2 ||
			(double)bndRect.width/bndRect.height > 10.4)
			continue;
		if(bndRect.width < (5*bndRect.height-(bndRect.height*bndRect.height)/10))
			continue;
		bndRect.x = bndRect.x<8?0:bndRect.x-8;
		bndRect.y = bndRect.y<2?0:bndRect.y-2;
		bndRect.width = bndRect.width+8+bndRect.x > 
			img->width?
			img->width-bndRect.x:
		bndRect.width+8;
		bndRect.height = bndRect.height+2+bndRect.y > 
			img->height?
			img->height-bndRect.y:
		bndRect.height+2;
		DetectLPBorder(img,bndRect,&l,&r);
		bndRect.x += l;
		bndRect.width = bndRect.width - l - r;
		//按照面积大小降序排列
		for(i=rect_num-1;i>=0;i--)
		{
			if(RectArea(bndRect) > RectArea((*lp_list)[i]))
				(*lp_list)[i+1] = (*lp_list)[i];
			else 
				break;

		}
		(*lp_list)[i+1] = bndRect;
		rect_num++;
	}
	{
		//删除面积比最大面积的三分之一还小的矩形

		for(i=rect_num-1;i>0;i--)
		{
			if(RectArea((*lp_list)[i]) < RectArea((*lp_list)[0])/3.5)
				rect_num -- ;

		}


	}
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&img_mo);
	if(rect_num == 0)
	{
		LPRFree(*lp_list);
	}
	return rect_num;
}
//获得所有候选连通域
int GetLpcList(const IplImage* pic,CvRect** lpc_list)
{
	int ret = 0;  //最终返回值
	//每一个矩形区域的连通域数量
	int rect_num = 0,max_id = -1;
	IplImage* img_mo = NULL;  //形态学操作之后的图像
	IplImage* img_back = NULL;
	IplImage* img_gray =  NULL;
	IplImage* img_bw = NULL;
	IplImage* img_edge = NULL;
	IplConvKernel * element0 = NULL,*element1 = NULL,*element2 = NULL,
		*element3 = NULL;
	

	element1 = cvCreateStructuringElementEx(28,6,14,3,CV_SHAPE_RECT,NULL);
	element2 = cvCreateStructuringElementEx(20,6,10,3,CV_SHAPE_RECT,NULL);
	element3 = cvCreateStructuringElementEx(20,1,10,0,CV_SHAPE_RECT,NULL);

	img_gray = cvCreateImage(cvGetSize(pic),IPL_DEPTH_8U,1);
	img_back = cvCreateImage(cvGetSize(pic),IPL_DEPTH_8U,1);
	img_bw = cvCreateImage(cvGetSize(pic),IPL_DEPTH_8U,1);
	img_edge = cvCreateImage(cvGetSize(pic),IPL_DEPTH_8U,1);
	img_mo = cvCreateImage(cvGetSize(pic),IPL_DEPTH_8U,1);
/*	int szElem = max(6,pic->height/50)*/;
	element0 = cvCreateStructuringElementEx
		(
		16,1,8,0,
		CV_SHAPE_RECT,NULL
		);	
	//转换成灰度图
	cvCvtColor(pic,img_gray,CV_BGR2GRAY); 
	//中值滤波
	cvSmooth(img_gray,img_gray,CV_MEDIAN);
	GrayStretch(img_gray,img_gray);
	/*1.顶帽操作*/
	cvMorphologyEx(
		img_gray,img_back,
		NULL,element0,CV_MOP_TOPHAT,1
		);

	{//求边缘图
		double meanPix = cvMean(img_back);
		cvThreshold(img_back,img_bw,11+meanPix,255,CV_THRESH_BINARY);
		SmoothBinImg(img_bw,21,SMOOTH_WHITE_CC);
		TwiceDiff(img_bw,img_edge);
		ImageCnp(img_edge,img_edge);
		//ReturnSobelImage(img_back,img_edge,1,0);
	
	}
	//对边界密集的区域进行处理
	cvMorphologyEx(img_edge,img_mo,NULL,element1,CV_MOP_CLOSE,1);

	cvMorphologyEx(img_mo,img_mo,NULL,element2,CV_MOP_OPEN,1);

	cvMorphologyEx(img_mo,img_mo,NULL,element3,CV_MOP_CLOSE,1);

	LengthFilter(img_mo,img_mo,MIN_LP_WIDTH,MAX_LP_WIDTH);
	/*cvShowImage("img_mo",img_mo);
	cvWaitKey(0);*/
	//cvShowImage("img_mo",img_mo);
	//找出所有可能的区域
	rect_num = DetectLPAreasVer2(img_mo,lpc_list);

	if(SHOW_FLAG_OTHER)
	{
		//cvNamedWindow("show_img",0);
		//cvResizeWindow("show_img",720,480);

		cvShowImage("show_img_",pic);
		cvWaitKey(0);

		cvShowImage("show_img_",img_gray);
		cvWaitKey(0);

		cvShowImage("show_img_",img_back);
		cvWaitKey(0);

		cvShowImage("show_img_",img_bw);
		cvWaitKey(0);

		cvShowImage("show_img_",img_edge);
		cvWaitKey(0);

		cvShowImage("show_img_",img_mo);
		cvWaitKey(0);

		cvDestroyAllWindows();
	}

	cvReleaseStructuringElement(&element0);
	cvReleaseStructuringElement(&element1);
	cvReleaseStructuringElement(&element2);
	cvReleaseStructuringElement(&element3);

	cvReleaseImage(&img_mo);
	cvReleaseImage(&img_gray);
	cvReleaseImage(&img_back);
	cvReleaseImage(&img_bw);
	cvReleaseImage(&img_edge);
	cvReleaseStructuringElement(&element1);
	//返回车牌候选区域
	return rect_num;
}
void TwiceDiff(const IplImage* src,IplImage* dst)
{
	int val1,val2,pixVal;
	int height = src->height;
	int width = src->width;
	uchar *ptr,*ptr1,*ptrDst;
	cvZero(dst);
	for(int i=1;i<height-1;i++)
	{
		ptr = (uchar*)(src->imageData+i*src->widthStep);
		ptr1 = (uchar*)(src->imageData+(i-1)*src->widthStep);
		ptrDst = (uchar*)(dst->imageData+i*dst->widthStep);
		for(int j=2;j<width;j++)
		{
			pixVal = ptr[j];
			val1 = abs(pixVal -  ptr[j-2]);
			val2 = abs(pixVal -  ptr1[j]);
		
			ptrDst[j] = max(val1,val2);
		}
	}
}
//对灰度图求差分图
void ImageDiff(const IplImage* src,IplImage* dst,int threshold)
{
	ASSERT(threshold > 0);
	ASSERT(src->imageData != dst->imageData);
	int pixVal;
	int val1=0,val2=0,maxVal;
	int height = src->height;
	int width = src->width;
	uchar *ptr,*ptr1,*ptrDst;
	cvZero(dst);
	for(int i=1;i<height;i++)
	{
		ptr = (uchar*)(src->imageData+i*src->widthStep);
		ptr1 = (uchar*)(src->imageData+(i-1)*src->widthStep);
		ptrDst = (uchar*)(dst->imageData+i*dst->widthStep);
		for(int j=2;j<width-1;j++)
		{
			pixVal = ptr[j];
			val1 = pixVal -  ptr[j-2];
			val2 = pixVal -  ptr1[j];
			maxVal = max(abs(val1),abs(val2));
			ptrDst[j] = (maxVal > threshold &&abs(val1 - val2) > threshold)*255;
			
		}
	}
	
}

//求微分图
void ReturnSobelImage(const IplImage* gray_src,IplImage* sobel_dst,int dx,int dy)
{
	CvSize size = cvGetSize(gray_src);
	IplImage* img_sobel = cvCreateImage(size,IPL_DEPTH_16S,1);
	IplImage* img_sobel_8u = cvCreateImage(size,IPL_DEPTH_8U,1);



	cvSobel(gray_src,img_sobel,dx,dy);
	cvCvtScaleAbs(img_sobel,img_sobel_8u);

	cvThreshold(img_sobel_8u,sobel_dst,255,255,
		CV_THRESH_BINARY|CV_THRESH_OTSU);
	SmoothBinImg(sobel_dst,24,SMOOTH_WHITE_CC);

	cvReleaseImage(&img_sobel);
	cvReleaseImage(&img_sobel_8u);

}
//返回边缘图
void ReturnEdgeImage(const IplImage* rgb_src,IplImage* edge_dst)
{

	double meanPixVal = 0;
	IplImage* img_gray = cvCreateImage(cvGetSize(rgb_src),IPL_DEPTH_8U,1);
	//IplImage* img_bw = cvCreateImage(cvGetSize(rgb_src),IPL_DEPTH_8U,1);
	
	cvCvtColor(rgb_src,img_gray,CV_BGR2GRAY);
	cvSmooth(img_gray,img_gray,CV_MEDIAN);
	
	
	GrayStretch(img_gray,img_gray);

	//求边缘图
	cvCanny(img_gray,edge_dst,50,140);
	/*for(int i=0;i<edge_dst->width-1;i++)
	{
		for(int j=0;j<edge_dst->height;j++)
		{
			if(CV_IMAGE_ELEM(edge_dst,uchar,j,i) == 255)
			{
				if(CV_IMAGE_ELEM(edge_dst,uchar,j,i+1) == 255 )
					CV_IMAGE_ELEM(edge_dst,uchar,j,i) = 0;

			}
		}
	}*/

	//SmoothBinImg(edge_dst,2,SMOOTH_WHITE_CC);
	//SmoothBinImg(edge_dst,8,SMOOTH_WHITE_CC);
	//cvShowImage("img_gray",img_gray);
	/*cvShowImage("img_bw",img_bw);
	cvWaitKey(0);*/


	cvReleaseImage(&img_gray);
	//cvReleaseImage(&img_bw);
	
}
IplImage* FindLprAboutEdgeDensity(const IplImage* img_edge)
{
	int i,j,h;
	IplImage* img = cvCloneImage(img_edge);
	IplImage* img_ed = cvCloneImage(img_edge);
	IplConvKernel *element1 = NULL,*element2 = NULL,
		*element3 = NULL,*element4 = NULL;
	//AfxMessageBox("Wait...1");
	unsigned* flags = (unsigned*)
		calloc(img_ed->height+1,sizeof(unsigned));
	ScanEdgeImage(img_ed,flags);
	//AfxMessageBox("Wait...2");
	
	element1 = cvCreateStructuringElementEx(20,6,10,3,CV_SHAPE_RECT,NULL);
	element2 = cvCreateStructuringElementEx(20,6,10,3,CV_SHAPE_RECT,NULL);
	element3 = cvCreateStructuringElementEx(20,1,10,0,CV_SHAPE_RECT,NULL);
	cvZero(img);
	for(i=0;i<img_ed->height;i++)
	{
		h = 0;
		if(flags[i] == 1)
		{
			for(j=i+1;j<img_ed->height;j++)
			{
				if(flags[j] == 1)
					h++;
				else
					break;
			}

			cvSetImageROI(img_ed,cvRect(0,i,img_ed->width,h));
			cvSetImageROI(img,cvRect(0,i,img_ed->width,h));

			cvMorphologyEx(img_ed,img_ed,NULL,element1,CV_MOP_CLOSE,1);

			cvMorphologyEx(img_ed,img_ed,NULL,element2,CV_MOP_OPEN,1);

			cvMorphologyEx(img_ed,img,NULL,element3,CV_MOP_CLOSE,1);

			cvResetImageROI(img_ed);
			cvResetImageROI(img);
			i = j;

		}
	}
	free(flags);flags = NULL;
	cvReleaseImage(&img_ed);
	
	cvReleaseStructuringElement(&element1);
	cvReleaseStructuringElement(&element2);
	cvReleaseStructuringElement(&element3);
	
	
	
	return img;
}
//扫描边缘图，返回一个车牌所在的区域
void ScanEdgeImage(const IplImage* img_edge,unsigned* flags)
{
	int i,j,t,s;
	//CvPoint ul[128],dr[128]
	int left=0,right=0,top,below;

	int imgWidth = img_edge->width;
	int imgHeight = img_edge->height;
	int thresWidth = 24;
	int thresHeight = 12;
	int nPixNum = 0;
	uchar* ptrE,*ptrMask;
	//int* flags = (int*)malloc(sizeof(int)*imgHeight);
	IplImage* img_e = cvCloneImage(img_edge);
	IplImage* img_mask = cvCreateImage(cvGetSize(img_edge),IPL_DEPTH_8U,1);
	cvZero(img_mask);

	for(i=1;i<imgHeight-1;i++)
	{
		ptrE = (uchar*)(img_e->imageData+img_e->widthStep*i);
		ptrMask = (uchar*)(img_mask->imageData+img_mask->widthStep*i);
		for(j=1;j<imgWidth-1;j++)
		{
			if(ptrE[j] == 255
				&& ptrMask[j] != 1)
			{
				//for(t = j+1;t)
				ptrMask[j] = 1;
				right = left = j;

				while(left < imgWidth && left - right < thresWidth)
				{
					if(ptrE[left] == 255
						&& ptrMask[left] != 1)
					{
						right = left;
						ptrMask[left] = 1;
					}
					left++;
				}
				if(right - j < MIN_LP_WIDTH || right - j >MAX_LP_WIDTH)
				{
					j  = left-1;
					continue;
				}
				else
				{
					t = i+1;
					do
					{//向下
						uchar* ptrEt = (uchar*)(img_e->imageData+img_e->widthStep*t);
						uchar* ptrMaskt = (uchar*)(img_mask->imageData+img_mask->widthStep*t);
						nPixNum = 0;
						for(s = j;s < left && s < imgWidth;s++)
						{
							if(ptrEt[s] == 255
								&& ptrMaskt[s] != 1)
							{
								nPixNum++;
								ptrMaskt[s] = 1;
							}
						}
						t++;
					}while(nPixNum != 0 && t < imgHeight);
					below = t;

					t = i-1;
					do
					{//向上
						uchar* ptrEt = (uchar*)(img_e->imageData+img_e->widthStep*t);
						uchar* ptrMaskt = (uchar*)(img_mask->imageData+img_mask->widthStep*t);
						nPixNum = 0;
						for(s = j;s < left && s < imgWidth;s++)
						{
							if(ptrEt[s] == 255
								&& ptrMaskt[s] != 1)
							{
								ptrMaskt[s] = 1;
								nPixNum++;
							}
						}
						t--;
					}while(nPixNum != 0 && t > 0);
					top = t;


					if(below - top < MIN_LP_HEIGHT || below - top > MAX_LP_HEIGHT)
						continue;
					else
					{
						if(right - j > below - top)
						{
							//if(SHOW_FLAG)
								//cvRectangle(img_e,cvPoint(j,top),cvPoint(right,below),CV_RGB(255,255,255),2);
							for(s = max(0,top-2);s<= below;s++)
								flags[s] = 1;
						}
					}
				}

			}
		}
	}
	//if(SHOW_FLAG)
	/*{
		cvShowImage("img_e",img_e);
		cvWaitKey(0);
	}*/

	cvReleaseImage(&img_e);
	cvReleaseImage(&img_mask);
}
//图像增强
void ImageStretch(const IplImage* src,IplImage* dst)
{
	ASSERT(src->width == dst->width);
	ASSERT(src->height == dst->height);
	int i,j;
	int offD = 3;  //偏移量
	int width = src->width;
	int height = src->height;
	IplImage* dst1 = cvCloneImage(dst);
	cvZero(dst1);
	for(i=offD;i<height - offD;i++)
	{
		uchar* ptrL = (uchar*)(src->imageData + src->widthStep*(i-offD));
		uchar* ptr = (uchar*)(src->imageData + src->widthStep*i);
		uchar* ptrH = (uchar*)(src->imageData + src->widthStep*(i+offD));
		uchar* ptrDst = (uchar*)(dst1->imageData + dst1->widthStep*i);
		for(j=0;j<width;j++)
		{
			if(2*ptr[j] - ptrL[j] - ptrH[j] > 0)
				ptrDst[j] = 255;
			else
				ptrDst[j] = 0;
		}
	}
	cvCopy(dst1,dst);
	cvReleaseImage(&dst1);
	/*for(i=0;i<offD;i++)
	{
		uchar* ptrDst = (uchar*)(dst->imageData + dst->widthStep*i);
		for(j=0;j<width;j++)
			ptrDst[j] = 0;
	}
*/

}
void ImageCnp(const IplImage* src,IplImage* dst)
{
	int i,j,val;
	IplImage* M = NULL;
	IplImage* N = NULL;
	int height = src->height;
	int width = src->width;
	M = cvCloneImage(src);
	N = cvCloneImage(src);
	cvZero(M);
	cvZero(N);

	for(i=2;i<height-3;i++)
		for(j=2;j<width-3;j++)
		{
			val = CV_IMAGE_ELEM(src,uchar,i,j);
			if( val == 255)
			{
				if(CV_IMAGE_ELEM(src,uchar,i-1,j-1) == 255 ||
					CV_IMAGE_ELEM(src,uchar,i-1,j) == 255 ||
					CV_IMAGE_ELEM(src,uchar,i-1,j+1) == 255 ||
					CV_IMAGE_ELEM(src,uchar,i,j-1) == 255)
					CV_IMAGE_ELEM(M,uchar,i,j) = max(
					max(CV_IMAGE_ELEM(M,uchar,i-1,j-1),CV_IMAGE_ELEM(M,uchar,i-1,j)),
					max(CV_IMAGE_ELEM(M,uchar,i-1,j+1),CV_IMAGE_ELEM(M,uchar,i,j-1)))+1;
				else
					CV_IMAGE_ELEM(M,uchar,i,j) = max(max(
					max(CV_IMAGE_ELEM(M,uchar,i-2,j-1),CV_IMAGE_ELEM(M,uchar,i-2,j)),
					max(CV_IMAGE_ELEM(M,uchar,i-2,j+1),CV_IMAGE_ELEM(M,uchar,i-1,j-2))),
					max(CV_IMAGE_ELEM(M,uchar,i-1,j+2),CV_IMAGE_ELEM(M,uchar,i,j-2)))+1;
			}
		}

		for(i=height-3;i>=2;i--)
			for(j=width-3;j>=2;j--)
			{
				if(CV_IMAGE_ELEM(src,uchar,i,j) == 255)
				{
					if(CV_IMAGE_ELEM(src,uchar,i+1,j-1) == 255 ||
						CV_IMAGE_ELEM(src,uchar,i+1,j) == 255 ||
						CV_IMAGE_ELEM(src,uchar,i+1,j+1) == 255 ||
						CV_IMAGE_ELEM(src,uchar,i,j+1) == 255)
						CV_IMAGE_ELEM(N,uchar,i,j) = max(
						max(CV_IMAGE_ELEM(N,uchar,i+1,j-1),CV_IMAGE_ELEM(N,uchar,i+1,j)),
						max(CV_IMAGE_ELEM(N,uchar,i+1,j+1),CV_IMAGE_ELEM(N,uchar,i,j+1)))+1;
					else
						CV_IMAGE_ELEM(N,uchar,i,j) = max(max(
						max(CV_IMAGE_ELEM(N,uchar,i+2,j-1),CV_IMAGE_ELEM(N,uchar,i+2,j)),
						max(CV_IMAGE_ELEM(N,uchar,i+2,j+1),CV_IMAGE_ELEM(N,uchar,i+1,j-2))),
						max(CV_IMAGE_ELEM(N,uchar,i+1,j+2),CV_IMAGE_ELEM(N,uchar,i,j+2)))+1;
				}
			}

			for(i=2;i<height-2;i++)
				for(j=2;j<width-2;j++)
				{
					val = CV_IMAGE_ELEM(M,uchar,i,j)+CV_IMAGE_ELEM(N,uchar,i,j);
					if(val > MAX_LP_WIDTH*0.8 || val < 6)
						CV_IMAGE_ELEM(dst,uchar,i,j) = 0;
				}

				cvReleaseImage(&M);
				cvReleaseImage(&N);


}


//从RGB转换成HSV
void Rgb2Hsv(float r,float g,float b,float* h,float* s,float* v)
{
	float maxPix,minPix;
	maxPix = max(r,max(g,b));
	minPix = min(r,min(g,b));
	if(maxPix == minPix)
		*h = 0;
	else if(maxPix == r && g >= b)
		*h = 60*(float)(g-b)/(maxPix-minPix);
	else if(maxPix == r && g < b)
		*h = 60*(float)(g-b)/(maxPix-minPix)+360;
	else if(maxPix == g)
		*h = 60*(float)(b-r)/(maxPix-minPix)+120;
	else if(maxPix == b)
		*h = 60*(float)(r-g)/(maxPix-minPix)+240;

	if(maxPix == 0)
		*s = 0;
	else
		*s = (float)(maxPix-minPix)/maxPix;
	*v = maxPix;


}

//根据HSV三个分量值确定颜色
int ReturnColor(float valH,float valS,float valV)
{

	if(valH >= 200 && valH <= 250)
	{
		if(valS >= 0.16 && valV >= 0.18)
		{//蓝色
			return COLOR_BLUE;
		}
		//return COLOR_WHITE;

	}
	if(valH >= 36 && valH <= 70)
	{
		if(valS >= 0.16 && valV >= 0.18)
		{//黄色
			return COLOR_YELLOW;
		}
		//return COLOR_BLACK;

	}
	if(valS <= 0.3 && valV >= 0.6)
	{//白色
		return COLOR_WHITE;
	}
	if( valV <= 0.4)
	{//黑色
		return COLOR_BLACK;
	}
	return COLOR_OTHERS;

}
/************************************************************************/
/*        统计HSV图中蓝、黄、白、黑四种颜色所占比例          */
/************************************************************************/
void StatisticsHSV(const IplImage* img,float* rb,float* ry,float* rw,float* rbk)
{
	int width = img->width;
	int height = img->height;
	IplImage* src = cvCloneImage(img);
	IplImage* imgHSV = cvCreateImage(cvGetSize(img),8,img->nChannels);
	IplImage* h_plane = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );
	IplImage* s_plane = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );
	IplImage* v_plane = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );

	//RGB图转换成HSV图
	cvCvtColor(img,imgHSV,CV_BGR2HSV);
	//获取各个空间分量
	cvCvtPixToPlane(imgHSV, h_plane, s_plane, v_plane,0);

	//统计蓝、黄、白、黑四种像素的数量
	{
		int i,j;
		int sumY = 0,sumB=0,sumW = 0,sumBk = 0;
		float valH,valS,valV;
		for(j=0;j<height;j++) 
		{	
			uchar* ptrH = (uchar*)(h_plane->imageData+h_plane->widthStep*j);
			uchar* ptrS = (uchar*)(s_plane->imageData+s_plane->widthStep*j);
			uchar* ptrV = (uchar*)(v_plane->imageData+v_plane->widthStep*j);
			for(i=0;i<width;i++)
			{	
				valH = (float)ptrH[i]*2;
				valS = (float)ptrS[i]/255;
				valV = (float)ptrV[i]/255;
				switch(ReturnColor(valH,valS,valV))
				{
				case COLOR_BLUE:
					sumB++;
					break;
				case COLOR_YELLOW:
					sumY++;
					break;
				case COLOR_WHITE:
					sumW++;
					break;
				case COLOR_BLACK:
					sumBk++;
					break;
				default:
					break;
				}				

			}

		}	
		//计算四种色素分别所占的比例
		*rb = (float)sumB/(i*j);
		*ry = (float)sumY/(i*j);
		*rw = (float)sumW/(i*j);
		*rbk = (float)sumBk/(i*j);

	}

	cvReleaseImage(&src);
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&h_plane);
	cvReleaseImage(&s_plane);
	cvReleaseImage(&v_plane);

}
/************************************************************************/
/* 统计统计HSV图中蓝、黄、白、黑四种颜色的分布情况                       */
/************************************************************************/
IplImage* StatisticsColorDistribution(const IplImage* img,struct colorD* horColor,
										struct colorD* verColor)
{
	int width = img->width;
	int height = img->height;
	IplImage* imgHSV = cvCreateImage(cvGetSize(img),8,img->nChannels);
	IplImage* h_plane = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );
	IplImage* s_plane = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );
	IplImage* v_plane = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );

	IplImage* retImg = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );
	//RGB图转换成HSV图
	cvCvtColor(img,imgHSV,CV_BGR2HSV);
	//获取各个空间分量
	cvCvtPixToPlane(imgHSV, h_plane, s_plane, v_plane,0);

	{
		int i,j,c;
		float valH,valS,valV;
		

		//初始化水平像素分布
		for(i=0;i<width;i++)
		{
			horColor[i].color[COLOR_BLUE] = 0;
			horColor[i].color[COLOR_YELLOW] = 0;
			horColor[i].color[COLOR_WHITE] = 0;
			horColor[i].color[COLOR_BLACK] = 0;
		}
		for(j=0;j<height;j++) 
		{	
			uchar* ptrH = (uchar*)(h_plane->imageData+h_plane->widthStep*j);
			uchar* ptrS = (uchar*)(s_plane->imageData+s_plane->widthStep*j);
			uchar* ptrV = (uchar*)(v_plane->imageData+v_plane->widthStep*j);

			verColor[j].color[COLOR_BLUE] = 0;
			verColor[j].color[COLOR_BLACK] = 0;
			verColor[j].color[COLOR_WHITE] = 0;
			verColor[j].color[COLOR_YELLOW] = 0;
			for(i=0;i<width;i++)
			{	
				valH = (float)ptrH[i]*2;
				valS = (float)ptrS[i]/255;
				valV = (float)ptrV[i]/255;
				c = ReturnColor(valH,valS,valV);
				CV_IMAGE_ELEM(retImg,uchar,j,i) = c;
				verColor[j].color[c]++;
				horColor[i].color[c]++;		

			}

		}	
	}

	//cvShowImage("color",retImg);
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&h_plane);
	cvReleaseImage(&s_plane);
	cvReleaseImage(&v_plane);
	return retImg;
}
//获取颜色图
IplImage* ReturnColorImage(const IplImage* img)
{
	int i,j;
	int width = img->width;
	int height = img->height;
	IplImage* imgHSV = cvCreateImage(cvGetSize(img),8,img->nChannels);
	IplImage* h_plane = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );
	IplImage* s_plane = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );
	IplImage* v_plane = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );

	IplImage* color_img = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );

	float valH,valS,valV;
	//RGB图转换成HSV图
	cvCvtColor(img,imgHSV,CV_BGR2HSV);
	//获取各个空间分量
	cvCvtPixToPlane(imgHSV, h_plane, s_plane, v_plane,0);

	for(j=0;j<height;j++) 
	{	
		uchar* ptrH = (uchar*)(h_plane->imageData+h_plane->widthStep*j);
		uchar* ptrS = (uchar*)(s_plane->imageData+s_plane->widthStep*j);
		uchar* ptrV = (uchar*)(v_plane->imageData+v_plane->widthStep*j);
		uchar* ptrD = (uchar*)(color_img->imageData+color_img->widthStep*j);
		for(i=0;i<width;i++)
		{	
			valH = (float)ptrH[i]*2;
			valS = (float)ptrS[i]/255;
			valV = (float)ptrV[i]/255;
			ptrD[i] = ReturnColor(valH,valS,valV);
		}

	}
	//去除多余意外的颜色
	for(j=1;j<height-1;j++)
	{
		uchar* ptrT = (uchar*)(color_img->imageData+color_img->widthStep*j);
		uchar* ptr = (uchar*)(color_img->imageData+color_img->widthStep*j);
		uchar* ptrB = (uchar*)(color_img->imageData+color_img->widthStep*j);
		for(i=1;i<width-1;i++)
		{
			if(ptr[i] != COLOR_OTHERS
				&& ptr[i]  != ptr[i-1]
				&& ptr[i] != ptr[i+1]
				&& ptr[i] != ptrT[i]
				&& ptr[i] != ptrB[i])
					ptr[i] = COLOR_OTHERS;

		}
	}
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&h_plane);
	cvReleaseImage(&s_plane);
	cvReleaseImage(&v_plane);
	return color_img;
}


//求两个hsv像素点之间的距离
float ReturnHsvDist(float h0,float s0,float v0,
					float h1,float s1,float v1)
{
	float s;
	float rad0 = h0/180;
	float rad1 = h1/180;
	s = sqrt(
			pow(s0*cos(rad0)-s1*cos(rad1),2)+
			pow(s0*sin(rad0)-s1*sin(rad1),2)+
			pow(v0-v1,2)
			);

	return s;
}
//从bgr中获取一个像素的hsv值
void GetHsvValFromBgr(const IplImage* img_rgb,int row,int col,
					  float* h,float* s,float* v)
{
	float r,g,b;
	uchar* ptr = (uchar*)(img_rgb->imageData+img_rgb->widthStep*row);
	b = (float)ptr[col*3];
	g = (float)ptr[col*3+1];
	r = (float)ptr[col*3+2];
	Rgb2Hsv(r,g,b,h,s,v);
}
//获取颜色图中某一颜色的
void ReturnMeanColorHSV(const IplImage* img,const IplImage* img_color,
						float* h,float* s,float* v,int color)
{
	int i,j,n = 0;
	int width = img->width;
	int height = img->height;
	IplImage* imgHSV = cvCreateImage(cvGetSize(img),8,img->nChannels);
	IplImage* h_plane = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );
	IplImage* s_plane = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );
	IplImage* v_plane = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );

	IplImage* color_img = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );

	double valH=0.0,valS=0.0,valV=0.0;
	//RGB图转换成HSV图
	cvCvtColor(img,imgHSV,CV_BGR2HSV);
	//获取各个空间分量
	cvCvtPixToPlane(imgHSV, h_plane, s_plane, v_plane,0);
	for(j=0;j<height;j++) 
	{	
		uchar* ptrH = (uchar*)(h_plane->imageData+h_plane->widthStep*j);
		uchar* ptrS = (uchar*)(s_plane->imageData+s_plane->widthStep*j);
		uchar* ptrV = (uchar*)(v_plane->imageData+v_plane->widthStep*j);
		uchar* ptrColor = (uchar*)(img_color->imageData+img_color->widthStep*j);
		for(i=0;i<width;i++)
		{
			if(ptrColor[i] == color)
			{
				valH += (float)ptrH[i]*2;
				valS += (float)ptrS[i]/255;
				valV += (float)ptrV[i]/255;
				n++;
			}
		}

	}	
	*h = (float)(valH/n);
	*s = (float)(valS/n);
	*v = (float)(valV/n);
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&h_plane);
	cvReleaseImage(&s_plane);
	cvReleaseImage(&v_plane);
}
//获取每一行颜色变化次数
int ReturnColorDiffers(const IplImage* img_color,int color)
{
	int ret = 0;
	int flag = 0;
	int width = img_color->width;
	int height = img_color->height;
	for(int i=1;i<width-1;i++)
		for(int j=1;j<height-1;j++)
		{
			if(CV_IMAGE_ELEM(img_color,uchar,j,i) == color && flag == 0
				&& CV_IMAGE_ELEM(img_color,uchar,j,i+1) == color+2)
			{
				flag = 1;
				ret++;
			}
			else if(CV_IMAGE_ELEM(img_color,uchar,j,i) == color+2 && flag == 1
				&& CV_IMAGE_ELEM(img_color,uchar,j,i+1) == color)
			{
				flag = 0;
				ret++;
			}
		}
		return ret;
}
//返回颜色的边界的像素的个数
float ReturnEdgeWithGrayLevel(const IplImage* img_edge,const IplImage* img_gray,int color)
{
	long sum = 0;
	int num = 0;
	int val1,val2;
	int width = img_edge->width;
	int height = img_edge->height;
	for(int i=1;i<height-1;i++)
	{
		uchar* ptr = (uchar*)(img_gray->imageData+img_gray->widthStep*i);
		uchar* ptr1 = (uchar*)(img_edge->imageData+img_edge->widthStep*i);
		uchar* ptrEdgeU = (uchar*)(img_edge->imageData+img_edge->widthStep*(i-1));
		uchar* ptrEdgeD = (uchar*)(img_edge->imageData+img_edge->widthStep*(i+1));
		for(int j=1;j<width-1;j++)
		{
			if( ptr1[j] == 255  &&
				ptrEdgeU[j] == 255 &&
				ptrEdgeD[j] == 255)
			{
				val1 = abs(ptr[j]-ptr[j-1]);
				val2 = abs(ptr[j]-ptr[j+1]);
				sum += max(val1,val2);
				num++;
			}
		}
	}

	//return (sum/num);
	return sum/(float)width;
}
//返回颜色的边界的像素的个数
int ReturnEdgeWithColor(const IplImage* img_color,const IplImage* img_edge,int color)
{
	int ret = 0;
	int width = img_edge->width;
	int height = img_edge->height;
	for(int i=1;i<height-1;i++)
	{
		uchar* ptrEdge = (uchar*)(img_edge->imageData+img_edge->widthStep*i);
		uchar* ptrColorU = (uchar*)(img_color->imageData+img_color->widthStep*(i-1));
		uchar* ptrColorD = (uchar*)(img_color->imageData+img_color->widthStep*(i+1));
		uchar* ptrColor = (uchar*)(img_color->imageData+img_color->widthStep*i);
		for(int j=1;j<width-1;j++)
		{
			if(ptrEdge[j] == 255)
			{
				if(ptrColor[j-1] == color || 
					ptrColor[j+1] == color ||
					ptrColorD[j] == color ||
					ptrColorU[j] == color)
				{
					ret++;
				}	
			}
		}
	}

	//{
	//	///用匹配法试一下
	//	IplImage* img_temp = cvLoadImage("d:\\1.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	//	IplImage* img_resize = cvCreateImage(cvSize(img_temp->width,img_edge->height),IPL_DEPTH_8U,1);
	//	cvResize(img_temp,img_resize);
	//	/*cvShowImage("1",img_temp);
	//	cvShowImage("2",img_resize);
	//	cvWaitKey(0);*/
	//	int width = abs(img_edge->width-img_resize->width)+1;
	//	int height = 1;
	//	IplImage* result = cvCreateImage(cvSize(width,height),32,1);
	//	if(img_edge->width > img_resize->width)
	//		cvMatchTemplate(img_edge,img_resize,result,CV_TM_SQDIFF);
	//	else
	//		cvMatchTemplate(img_resize,img_edge,result,CV_TM_SQDIFF);

	//	cvNormalize(result,result,CV_MINMAX);

	//	double maxVal = 0.0;
	//	cvMinMaxLoc(result,NULL,&maxVal);
	//	ret /= maxVal;

	//	cvReleaseImage(&img_temp);
	//	cvReleaseImage(&img_resize);
	//	
	//}
	return ret;
}
//计算平均值和方差
void GetVariance(struct colorD* verColor,int h,int color,float* avg,float* var)
{
	int i;
	int tal = h;
	float temp;
	*avg = 0,*var = 0;
	for(i=0;i<h;i++)
	{
		temp = (float)verColor[i].color[color];
		if(temp <= 8)
			tal--;
		else
			*avg+=temp;
	}
	if(tal < 0.4*h)
	{
		*avg = 0;
		*var = 65535;
		return;
	}
	*avg /= tal;
	for(i=0;i<h;i++)
	{

		temp = (float)verColor[i].color[color];
		if(temp > 8)
		{
			temp /= *avg;
			*var+=(temp-1)*(temp-1);
		}


	}
	*var /= tal;
}
//获取边缘图每一行非零的统计
void GetHorStatistics(const IplImage* img,float* avg,float* var)
{
	float val;
	int i,j,s = 0;
	int width = img->width;
	int height = img->height;
	int *num = (int*)malloc(sizeof(int)*height);
	*avg = 0;
	*var = 0;
	for(i=0;i<height;i++)
	{
		num[i] = 0;
		for(j=1;j<width-1;j++)
		{
			if(CV_IMAGE_ELEM(img,uchar,i,j) == 255
				&& CV_IMAGE_ELEM(img,uchar,i,j-1) == 0
				&& CV_IMAGE_ELEM(img,uchar,i,j+1) == 0)
				num[i]++;
		}
		s += num[i];
	}
	*avg = (float)s/height;

	for(i=0;i<height;i++)
		val = ((float)num[i]/(*avg) - 1)*((float)num[i]/(*avg) - 1);
	//*avg = s;
	*var = sqrt(val/height);
	free(num);
}

//获取列中含像素的个数
void GetVerStatistcs(const IplImage* img,float* avg,float* var)
{
	float val;
	int i,j,t = 0,s = 0;
	int width = img->width;
	int height = img->height;
	int *num = (int*)malloc(sizeof(int)*width);
	*avg = 0;
	*var = 0;
	for(i=0;i<width;i++)
	{
		num[i] = 0;
		for(j=0;j<height;j++)
		{
			if(CV_IMAGE_ELEM(img,uchar,j,i) == 255)
				num[i]++;
		}
		if(num[i] != 0)
		{
			s += num[i];
			t++;
		}

	}
	*avg = (float)s/t;

	for(i=0;i<width;i++)
	{
		if(num[i] != 0)
			val = ((float)num[i]/(*avg) - 1)*((float)num[i]/(*avg) - 1);
	}
	//*avg = s;
	*var = sqrt(val/t);
	free(num);
}
void GetRectEdgeImageStatistics(const IplImage* img_edge,CvRect rect,float* avg,float* var)
{
	IplImage* img = GetRectImage(img_edge,rect);
	GetHorStatistics(img,avg,var);
	cvReleaseImage(&img);
}

//获取灰度图中像素为val的个数
long GetNumFromGrayImage(const IplImage* img_gray,int val)
{
	long ret = 0;
	int height = img_gray->height;
	int width = img_gray->width;
	for(int i=0;i<height;i++)
		for(int j=0;j<width;j++)
		{
			if(CV_IMAGE_ELEM(img_gray,uchar,i,j) == val)
				ret++;
		}

		return ret;
}
//获得颜色分布图
IplImage* ColorDistribution(const IplImage* img)
{
	int width = img->width;
	int height = img->height;
	IplImage* imgHSV = cvCreateImage(cvGetSize(img),8,img->nChannels);
	IplImage* h_plane = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );
	IplImage* s_plane = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );
	IplImage* v_plane = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );

	IplImage* retImg = cvCreateImage( cvGetSize(imgHSV), IPL_DEPTH_8U, 1 );
	//RGB图转换成HSV图
	cvCvtColor(img,imgHSV,CV_BGR2HSV);
	//获取各个空间分量
	cvCvtPixToPlane(imgHSV, h_plane, s_plane, v_plane,0);

	{
		int i,j;
		float valH,valS,valV;	
		for(j=0;j<height;j++) 
		{	
			uchar* ptrH = (uchar*)(h_plane->imageData+h_plane->widthStep*j);
			uchar* ptrS = (uchar*)(s_plane->imageData+s_plane->widthStep*j);
			uchar* ptrV = (uchar*)(v_plane->imageData+v_plane->widthStep*j);
			for(i=0;i<width;i++)
			{	
				valH = (float)ptrH[i]*2;
				valS = (float)ptrS[i]/255;
				valV = (float)ptrV[i]/255;
				CV_IMAGE_ELEM(retImg,uchar,j,i) = ReturnColor(valH,valS,valV);
			}

		}	
	}
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&h_plane);
	cvReleaseImage(&s_plane);
	cvReleaseImage(&v_plane);
	return retImg;
}
//根据统计的颜色分布情况进行分析
struct colorD* AnalyseColorDistribution(const IplImage* colorImg)
{
	int nb_w = 0,ny_b = 0;
	int height = colorImg->height;
	int width = colorImg->width;
	struct colorD* colordis = (struct colorD*)malloc(sizeof(struct colorD));
	{//colordis初始化
		colordis->color[COLOR_BLUE] = 0;
		colordis->color[COLOR_BLACK] = 0;
		colordis->color[COLOR_WHITE] = 0;
		colordis->color[COLOR_YELLOW] = 0;
		colordis->nb_w = 0;
		colordis->ny_b = 0;
	}
	//统计蓝、黄、白、黑四种像素的数量
	{
		int val1,val2;
		int i,j;
		for(j=0;j<height;j++) 
		{	
			uchar* ptr= (uchar*)(colorImg->imageData+colorImg->widthStep*j);
			for(i=0;i<width-1;i++)
			{
				val1 = ptr[i];
				val2 = ptr[i+1];

				switch(val1)
				{
				case COLOR_BLUE:
					colordis->color[COLOR_BLUE]++;
					if(COLOR_WHITE == val2)
						colordis->nb_w++;
					break;
				case COLOR_YELLOW:
					colordis->color[COLOR_YELLOW]++;
					if(COLOR_BLACK == val2)
						colordis->ny_b++;
					break;
				case COLOR_WHITE:
					colordis->color[COLOR_WHITE]++;
					if(COLOR_BLUE == val2)
						colordis->nb_w++;
					break;
				case COLOR_BLACK:
					colordis->color[COLOR_BLACK]++;
					if(COLOR_YELLOW == val2)
						colordis->ny_b++;
					break;
				default:
					break;
				}	

			}
		}
	}

	return colordis;
}
//返回车牌颜色
int ReturnLPColor(const IplImage* img_color)
{
	int color;
	struct colorD* colord;


	colord = AnalyseColorDistribution(img_color);
	if(colord->color[COLOR_BLUE] >colord->color[COLOR_YELLOW])
	{//蓝牌
		color = COLOR_BLUE;
	}
	else
	{//黄牌
		color = COLOR_YELLOW;
	}

	//其他颜色牌


	LPRFree(colord);

	return color;
}

//返回连通域的个数
int ReturnCRNum(const IplImage* bwImage)
{
	int ret;
	int mode = CV_RETR_EXTERNAL;	//轮廓边缘提取时的参数
	CvSeq * contour = 0;		//轮廓边缘提取时的参数
	CvRect bndRect = cvRect(0,0,0,0);//用cvBoundingRect画出外接矩形时需要的矩形
	CvMemStorage * storage; //轮廓边缘提取时的参数
	IplImage* img_bw = NULL;
	

	storage = cvCreateMemStorage(0);
	img_bw = cvCloneImage(bwImage);



	//寻找LP边缘
	ret = cvFindContours(img_bw, storage, &contour, sizeof(CvContour),
		mode, CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0));
	

	
	for(;contour!=0;contour=contour->h_next)
	{	
		bndRect	= cvBoundingRect(contour,0);
		if(bndRect.width*bndRect.height < 40)
			ret--;
	}
	cvReleaseImage(&img_bw);
	cvReleaseMemStorage(&storage);
	return ret;
}
//计算候选车牌候选区域权值
float ReturnLPCWeight(const IplImage* rgb_img,const IplImage* img_color,int* color)
{
	float ret;
	int colorv;
	float avg,var/*,avgv,varv*/;		
	IplImage* img_edge = NULL;
	IplImage* img_gray = NULL;
	IplImage* img_bw = NULL;
	IplImage* img_back = NULL;
	IplImage* img_col = NULL;
	IplConvKernel* element = NULL;

	img_bw = cvCreateImage(cvGetSize(rgb_img),IPL_DEPTH_8U,1);
	img_edge = cvCreateImage(cvGetSize(rgb_img),IPL_DEPTH_8U,1);
	img_back = cvCreateImage(cvGetSize(rgb_img),IPL_DEPTH_8U,1);
	img_gray = cvCreateImage(cvGetSize(rgb_img),IPL_DEPTH_8U,1);

	if(img_color == NULL)
		img_col = ReturnColorImage(rgb_img);
	else
		img_col = cvCloneImage(img_color);

	element = cvCreateStructuringElementEx
		(
		20,1,10,0,
		CV_SHAPE_RECT,NULL
		);	
	cvCvtColor(rgb_img,img_gray,CV_BGR2GRAY);
	cvSmooth(img_gray,img_gray,CV_MEDIAN);
	/*1.顶帽操作*/
	cvMorphologyEx(
		img_gray,img_back,
		NULL,element,CV_MOP_TOPHAT,1
		);

	//求边缘图
	cvCanny(img_back,img_edge,40,100);


	/*cvShowImage("rgnb",rgb_img);
	cvWaitKey(0);*/
	{//求颜色

		struct colorD* colord;	
		colord = AnalyseColorDistribution(img_col);
		
		/*if(colord->color[COLOR_WHITE] >)
		else */if(colord->color[COLOR_BLUE] >colord->color[COLOR_YELLOW])
		{//蓝牌
			colorv = COLOR_BLUE;
			cvThreshold(img_back,img_bw,255,255,CV_THRESH_BINARY|CV_THRESH_OTSU);
		}
		else
		{//黄牌
			colorv = COLOR_YELLOW;
			cvThreshold(img_back,img_bw,255,255,CV_THRESH_BINARY_INV|CV_THRESH_OTSU);
		}

		//或许有其他颜色牌

		LPRFree(colord);
	}

	
	//统计水平和竖直方向的边缘数量
	GetHorStatistics(img_edge,&avg,&var);
	if(avg > img_col->width*0.5)
		return 0;
	if(var < 0.001 || var > 0.5)
		return 1;
	ret =(float)ReturnEdgeWithColor(img_col,img_edge,colorv)/rgb_img->height+1.5;
	

	ret *= sqrt(ReturnEdgeWithGrayLevel(img_edge,img_back,colorv));
	if(NULL != color)
		*color = colorv;
	cvReleaseImage(&img_col);
	cvReleaseImage(&img_edge);
	cvReleaseImage(&img_gray);
	cvReleaseImage(&img_bw);
	cvReleaseImage(&img_back);
	cvReleaseStructuringElement(&element);
	return ret;
}
//精确车牌位置和大小
IplImage* ResizeImageByDilate(const IplImage* img)
{//该函数仅作用于蓝牌
	double thres;
	int offL=0,offR=0,offT=0,offB=0;
	int *hisHor,*hisVer;
	IplConvKernel* element1 = NULL;
	IplImage *img_gray = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
	IplImage *img_bw = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);	

	cvCvtColor(img,img_gray,CV_BGR2GRAY); 
	cvThreshold(img_gray,img_bw,255,255,CV_THRESH_OTSU | CV_THRESH_BINARY);
	element1 = cvCreateStructuringElementEx(15,6,7,3,CV_SHAPE_RECT,NULL);
	cvDilate(img_bw,img_bw,element1,1);
	cvShowImage("img_bw",img_bw);
	hisHor = histogramVH(img_bw,1);
	hisVer = histogramVH(img_bw,0);

	thres = img_bw->height*0.5;
	for(int i=0;i<img_bw->width/2;i++)
	{
		if(hisHor[i] < thres)
			offL++;
		/*else
		break;*/
	}
	for(int i=img_bw->width-1;i>=img_bw->width/2;i--)
	{
		if(hisHor[i] < thres )
			offR++;
	}

	thres = 0.5*img_bw->width;
	for(int i=0;i<img_bw->height;i++)
	{
		if(hisVer[i] < thres)
			offT++;
		else
			break;
	}
	for(int i=img_bw->height-1;i>=0;i--)
	{
		if(hisVer[i] < thres)
			offB++;
		else
			break;
	}

	/*if(offB != img_bw->height && offT != img_bw->height
		&& offL != img_bw->width && offR != img_bw->width)
	{

		rect->x += offL;
		rect->y += offT;
		rect->width = rect->width-offL-offR;
		rect->height = rect->height-offT-offB;

	}*/
	/*cvShowImage("img_bw",img_bw);
	cvWaitKey(0);*/
	cvReleaseImage(&img_gray);
	cvReleaseImage(&img_bw);
	free(hisVer);
	free(hisHor);
	cvReleaseStructuringElement(&element1);

	return GetRectImage(img,
		cvRect(
		offL,offT,
		img->width-offL-offR,
		img->height-offT-offB)
		);

}
//根据颜色精确定位车牌区域
void AdujustRect(CvRect* rect,int offL,int offR,int offT,int offB)
{
	/*offR -= 2;
	offL += 2;*/
	//调整矩形
	if(offB != rect->height-1 && offT != rect->height-1
		&& offL != rect->width-1 && offR != rect->width-1)
	{
		if((float)(rect->width-offL-offR)/rect->height < 5 && 
			(float)(rect->width-offL-offR)/rect->height > 2.2)
			rect->x += offL;
		rect->y += offT;
		if((float)(rect->width-offL-offR)/rect->height < 5 &&
			(float)(rect->width-offL-offR)/rect->height > 2.2)
			rect->width = rect->width-offL-offR;
		rect->height = rect->height-offT-offB;
	}

}
//扩展矩形
void ExtendLPR(const IplImage* img_color,CvRect rect,
			   int* offL,int* offR,int color)
{
	int tmp,i,j,nMinPix;	

	//进行扩展
	nMinPix = 2;
	if(*offL <= 1 && (float)rect.width/rect.height < 6)
	{

		for(i = rect.x-1;i>0;i--)
		{
			tmp = 0;
			for(j=0;j<img_color->height;j++)
			{
				if(CV_IMAGE_ELEM(img_color,uchar,j,i) == color)
				{
					tmp++;
				}
			}
			if(tmp > nMinPix)
				(*offL)--;
			else
				break;
		}

	}

	if(*offR <= 1 && (float)(rect.width-*offL)/rect.height < 6)
	{
		for(i = rect.x+rect.width;i<img_color->width;i++)
		{
			tmp = 0;
			for(j=0;j<img_color->height;j++)
			{
				if(CV_IMAGE_ELEM(img_color,uchar,j,i) == color)
				{
					tmp++;
				}

			}
			if(tmp > nMinPix)
				(*offR)--;
			else
				break;
		}

	}
}
void ReduceLPR(const IplImage* img_color,CvRect rect,int* offL,
			   int* offR,int* offT,int* offB,int color)
{
	int i,j,tmp;
	int height = rect.height;
	int width = rect.width;
	int nMinPix = height/8;
	int nMinColorDis = 4;
	int iMeanColorPix = 0;
	int l = 0,r = rect.width;
	int* histTextHor = (int*)malloc(width*sizeof(int));
	int* histBackHor = (int*)malloc(width*sizeof(int));
	IplImage* img_small_color = GetRectImage(img_color,rect);
	//获取车牌字体颜色直方图
	ReturnHist(img_small_color,histTextHor,color+2,1);
	//获取车牌背景颜色直方图
	ReturnHist(img_small_color,histBackHor,color,1);


	for(i=0;i<width;i++)
		iMeanColorPix += histBackHor[i];
	iMeanColorPix /= width;
	//开始根据颜色进行判断	
	{
		//左边界缩小
		tmp = 0;
		for(i=0;i<img_small_color->width/3;i++)
		{
			if(histBackHor[i] >= iMeanColorPix*0.5
				&& histBackHor[i]+histTextHor[i] > 0.5*height)	
				break;
			else
			{
				tmp++;
				while(tmp < nMinColorDis && i <img_small_color->width/3
					&&(histBackHor[i] < nMinPix	|| histBackHor[i]+histTextHor[i] <= 0.5*height))
					//(*offL)++;
				{
					tmp++;
					i++;
				}
				if(tmp == nMinColorDis)
				{
					l = i;
				}
				tmp = 0;
			}
			
		}
		//右边界缩进
		tmp = 0;
		for(i=img_small_color->width-1;i>img_small_color->width/3;i--)
		{
			if(histBackHor[i] >= iMeanColorPix*0.5
				&& histBackHor[i]+histTextHor[i] > 0.5*height)	
				break;
			else
			{
				tmp++;
				while(tmp < nMinColorDis && i > img_small_color->width/3
					&&(histBackHor[i] < nMinPix	|| histBackHor[i]+histTextHor[i] <= 0.5*height))
				{
					tmp++;
					i--;
				}
				if(tmp == nMinColorDis)
				{
					r = i;
				}
				tmp = 0;
			}
		}
	
		//上边界缩进
		for(i=0;i<img_small_color->height-1;i++)
		{
			tmp = 0;
			for(j=0;j<img_small_color->width;j++)
			{
				if(CV_IMAGE_ELEM(img_small_color,uchar,i,j) == color)
				{

					tmp++;
				}
			}
			if(tmp < nMinPix)
				(*offT)++;
			else
				break;

		}
		//下边界缩进
		for(i=img_small_color->height-1;i>0;i--)
		{
			tmp = 0;
			for(int j=0;j<img_small_color->width;j++)
			{
				if(CV_IMAGE_ELEM(img_small_color,uchar,i,j) == color)
				{
					tmp++;
				}

			}
			if(tmp < nMinPix)
				(*offB)++;
			else
				break;
		}
	}
	*offL = l;
	*offR = rect.width-r;
	LPRFree(histTextHor);
	LPRFree(histBackHor);
	cvReleaseImage(&img_small_color);
}

//灰度图方差投影
void ImageVPF(const IplImage* img_gray)
{
	int iWidth = img_gray->width;
	int iHeight = img_gray->height;

	int t,t1,t2;
	float minVal = 0x8fffffff,maxVal=0;
	float* p = (float*)malloc(sizeof(float)*iWidth);
	uchar* ptr = (uchar*)img_gray->imageData;
	int step = img_gray->widthStep;
	for(int i=0;i<iWidth;i++)
	{
		t1 = 0;
		t2 = 0;
		for(int j=0;j<iHeight;j++)
		{
			t = ptr[j*step+i];
			t1 += t;
			t2 += t*t;
		}
		p[i] = t2/(float)iHeight-(t1/(float)iHeight)*(t1/(float)iHeight);
		if(p[i] > maxVal)
			maxVal = p[i];
		if(p[i] < minVal)
			minVal = p[i];
	}
	
	//显示
	{
		IplImage* img_VPF = cvCloneImage(img_gray);
		cvZero(img_VPF);
		for(int i=0;i<iWidth;i++)
		{
			if(p[i] < maxVal/7)
				p[i] = 0;
			else
				p[i] = (p[i] - minVal)/(maxVal-minVal);

			cvLine(img_VPF,cvPoint(i,iHeight),cvPoint(i,iHeight-iHeight*p[i]),cvScalar(255,255,255),1);

		}
		cvShowImage("img_VPF",img_VPF);
		cvShowImage("img_vpf_gray",img_gray);
		cvWaitKey(0);
	}
	



}
//根据车牌颜色确定左右边界
IplImage* ImproveLPAreaByColor(const IplImage* lp_img,CvRect in_rect,int color)
{
	float r;
	int offL=0,offR=0,offT=0,offB=0;
	colorD* colord = NULL; 
	CvRect rect = in_rect;
	IplImage* img_color = ReturnColorImage(lp_img);
	
	IplImage* img_color_rect = GetRectImage(img_color,in_rect);
	if(SHOW_BORDER)
		ShowColorImage(img_color_rect);

	//当图像较暗时利用灰度图进行调整
	colord = AnalyseColorDistribution(img_color_rect);
	r = (float)colord->color[color]/in_rect.height/in_rect.width;
	if(r >= 0.36 ) 
	{//如果车牌颜色较弱则不进行调整

		//缩小车牌区域
		ReduceLPR(img_color,rect,&offL,&offR,&offT,&offB,color);

		//由于左右边经常残缺，所以向左边和右边扩展一下
		if((offL == 0 || offR == 0)
			&& (float)rect.width/rect.height < 3)
			ExtendLPR( img_color,rect,&offL,&offR,color);

		//调整矩形大小
		AdujustRect(&rect,offL,offR,offT,offB);  
	}

	LPRFree(colord);
	cvReleaseImage(&img_color);
	cvReleaseImage(&img_color_rect);
	return GetRectImage(lp_img,rect);
}

//精确定位车牌位置
int RectifyLPR(const IplImage* lp_gray_img,CvRect* out_rect)
{
	
	int iWidth = lp_gray_img->width;
	int iHeight = lp_gray_img->height;
	
	int l = 0,r = iWidth,t = 0,b = iHeight;
	IplImage* img_sobel = cvCreateImage(cvGetSize(lp_gray_img),8,1);
	IplImage* img_dia = cvCreateImage(cvGetSize(lp_gray_img),8,1);
	ReturnSobelImage(lp_gray_img,img_sobel,1,0);
	cvDilate(img_sobel,img_dia,NULL,5);

	/*cvShowImage("ingZ_so_d",img_dia);
	cvShowImage("img_gray",lp_gray_img);
	cvShowImage("ingZ_so",img_sobel);
	cvWaitKey(0);*/
	//

	{//上下边界定位

		int i,j,s;
		int histVerMeanPix = 0,minHorDis = 2;
	
		int *histVer = (int*)malloc(iHeight*sizeof(int));
		//求图像边缘图直方图
		ReturnHist(img_sobel,histVer,255,0);
		for(i=0;i<iHeight;i++)
		{
			histVerMeanPix += histVer[i]; 
		}
		histVerMeanPix /= iHeight;	

		for(i=0;i<iHeight/3;i++)
		{
			if(histVer[i] < histVerMeanPix*0.6
				|| histVer[i] > 0.8*iWidth)
			{
				t = i;
			}
		}
		//下边界定位
		for(i=iHeight;i>iHeight/3*2;i--)
		{

			if(histVer[i] < histVerMeanPix*0.6
				|| histVer[i] > 0.8*iWidth)
			{
				b = i;
			}

		}
		LPRFree(histVer);
	}
	{//确定左右边界
		int i,j,s;
		int thres = 3;
		int histHorMeanPix = 0,
			minHorDis = 2;
		int iHeight2 = b-t;
		int *histHor = NULL;
		IplImage* sobel = NULL;
		histHor = (int*)malloc(iWidth*sizeof(int));
		if((float)iWidth/iHeight2 > 3.4)
			sobel = GetRectImage(img_sobel,cvRect(0,0,iWidth,iHeight2));
		else
			sobel = cvCloneImage(img_sobel);
		
		//求图像边缘图直方图
		ReturnHist(sobel,histHor,255,1);

		for(i=0;i<iWidth;i++)
		{
			histHorMeanPix += histHor[i]; 
		}
		histHorMeanPix /= iWidth;	

		////左边界定位
		s = 0;
		l = 0;
		for(i=0;i<iWidth/2;i++)
		{
			if(histHor[i] < thres )
			{
				l = i;
			}
			else if(histHor[i] >= iHeight2*0.9)
			{
				l = min(i+2,iWidth/3);
				break;
			}
			else			
			{
				s++;
				while(s <= minHorDis 
					&& i < iWidth/3)
				{
					i++;
					if(histHor[i] < thres )
						break;
					else if(histHor[i] > iHeight2*0.9)
					{
						s = minHorDis+1;
						l = min(i+2,iWidth/3);
					}
					else
						s++;
				}
				if(s > minHorDis)
					break;
				else
				{
					s = 0;
					l = i;
				}
			}
		}
		//直接寻找左边界
		/*s = 0;
		for(i=l;i<iWidth/2;i++)
		{
			if(histHor[i] >= 0.96*iHeight2)
				s++;
			else
				s = 0;
			if(s >= 3)
			{
				l = i;
				break;
			}
		}*/
		//右边界定位
		s = 0;
		r=iWidth;
		for(i=iWidth;i>=iWidth/2;i--)
		{
			if(histHor[i] < thres )
			{
				r = i;
			}
			else if(histHor[i] >= iHeight2*0.99)
			{
				r = max(i-2,iWidth/2);
				break;
			}
			else
			{
				s++;
				while(s <= minHorDis 
					&& i >= iWidth/2)
				{
					i--;
					if(histHor[i] < thres )
						break;
					else if(histHor[i] > iHeight2*0.99)
					{
						s = minHorDis+1;
						r = max(i-2,iWidth/2);
					}
					else
						s++;
				}
				if(s > minHorDis)
					break;
				else
				{
					s = 0;
					r = i;
				}
			}
		}
		////直接寻找右边界
		/*s = 0;
		for(i=r;i>=iWidth/2;i--)
		{
			if(histHor[i] > 0.99*iHeight2)
				s++;
			else
				s = 0;
			if(s >= 3)
			{
				r = i;
				break;
			}
		}*/
		LPRFree(histHor);
		cvReleaseImage(&sobel);

	}
	
	{
		//计算定位结果
		int x,y;
		int width ;
		int height;
		if(b-t < MIN_LP_HEIGHT)
		{
			y = 0;
			height = iHeight;
		}
		else
		{
			y = t;
			height = b - t;
		}
		if(r-l < MIN_LP_WIDTH)
		{
			x = 0;
			width = iWidth;
		}
		else
		{
			x = l;
			width = r - l;
		}
		*out_rect = cvRect(x,y, width,height);
		
	}

	
	cvReleaseImage(&img_sobel);
	return 1;
}
//进一步根据边界确定车牌区域的左右边界和上下边界,，返回0出错
int ImproveLPAreaByEdge(const IplImage* lp_img_edge,CvRect* rect)
{

	
	int l=0,r=lp_img_edge->width,
		u=0,d=lp_img_edge->height;

	IplImage* img_canny_tmp = NULL;
	/*cvShowImage("song is here",lp_img_edge);
	cvWaitKey(0);*/
	//寻找上下边界
	FindEdgeImageBorderUD(lp_img_edge,&u,&d);
	
	if(d - u > MIN_LP_HEIGHT)
		img_canny_tmp = GetRectImage(
								lp_img_edge,
								cvRect(0,u, lp_img_edge->width,d-u)
								);
	else
		img_canny_tmp = cvCloneImage(lp_img_edge);
	//寻找左右边界
	FindEdgeImageBorderLR(img_canny_tmp,&l,&r);
	
	//定位
	{
		//返回定位结果
		int x,y;
		int width ;
		int height;
		if(d-u < MIN_LP_HEIGHT)
		{
			y = 0;
			height = lp_img_edge->height;
		}
		else
		{
			y = u;
			height = d - u;
		}
		if(r-l < MIN_LP_WIDTH)
		{
			x = 0;
			width = lp_img_edge->width;
		}
		else
		{
			x = l;
			width = r - l;
		}
		*rect = cvRect(x,y, width,height);
	}
	
	cvReleaseImage(&img_canny_tmp);
	return 1;
}
//根据二值化图进一步确定边界图
int ImproveLPAreaByBinaryImg(const IplImage* lp_img_binary,CvRect* rect)
{
	int i,j,t;
	int l,r,u,d;
	int iWidth = lp_img_binary->width;
	int iHeight = lp_img_binary->height;
	int pixThres,minHorDis = 3;
	int histHorMeanPix = 0;  //水平像素平均值
	
	int *histHor = (int*)malloc(iWidth*sizeof(int));
	
	//求图像边缘图直方图
	ReturnHist(lp_img_binary,histHor,255,1);

	for(i=0;i<iWidth;i++)
	{
		histHorMeanPix += histHor[i]; 
	}
	histHorMeanPix /= iWidth;
	pixThres = histHorMeanPix*0.6;

	//左边界定位
	t = 0;
	l = 0;
	for(i=0;i<iWidth/3;i++)
	{
		if(histHor[i] < histHorMeanPix)
		{
			l = i;
		}
		else if(histHor[i] >= iHeight*0.9)
		{
			l = i;
			break;
		}
		else			
		{
			t++;
			while(t <= minHorDis 
				&& i < iWidth/3)
			{
				i++;
				if(histHor[i] < pixThres ||
					histHor[i] > iHeight*0.92)
					break;
				else
					t++;
			}
			if(t > minHorDis)
				break;
			else
			{
				t = 0;
				l = i;
			}
		}
	}

	//右边界定位
	t = 0;
	r=iWidth;
	for(i=iWidth;i>=iWidth/3;i--)
	{
		if(histHor[i] < pixThres)
		{
			r = i;
		}
		else if(histHor[i] > iHeight*0.9)
		{
			break;
		}
		else

		{
			t++;
			while(t <= minHorDis 
				&& i >= iWidth/3)
			{
				i--;
				if(histHor[i] < pixThres ||
					histHor[i] > iHeight*0.92)
					break;
				else
					t++;
			}
			if(t > minHorDis)
				break;
			else
			{
				t = 0;
				r = i;
			}
		}
	}

	//中间位置的跳变次数
	{

		int mid = iHeight/2;
		for(i=l;i<iWidth/2;i++)
		{
			if(CV_IMAGE_ELEM(lp_img_binary,uchar,mid,i) == 0)
				l = i;
			else
				break;
		}
	}
	
	u = 0;
	d = iHeight;
	{//上下边界定位
		int minSkipThres = 10;
		int maxSkipTimes = -1,minSkipTimes = iWidth;
		int* iSkipTimes = (int*)malloc(sizeof(int)*iHeight);   //跳变次数
		for(i=0;i<iHeight;i++)
		{
			uchar* ptr = (uchar*)
				(lp_img_binary->imageData+
				lp_img_binary->widthStep*i);
			t = 0;
			for(j=0;j<iWidth-1;j++)
				if(ptr[j] != ptr[j+1])
					t++ ;

			if(maxSkipTimes < t)
				maxSkipTimes = t;
			if(minSkipTimes > t)
				minSkipTimes = t;
			iSkipTimes[i] = t;
			//XdPrint("%d\n",t);
		}	
		for(i=0;i<iHeight/4;i++)
			if(iSkipTimes[i] <= minSkipThres)
				u = i;
		for(i = iHeight;i>= iHeight/4*3;i--)
			if(iSkipTimes[i] <= minSkipThres)
				d = i;
		LPRFree(iSkipTimes);
	}
	{
		
		/*CvMat* matSkip = cvCreateMat(iHeight,iWidth,CV_32SC1);
		cvZero(matSkip);
		for(i=1;i<iHeight;i++)
		{
			for(j=1;j<iWidth;j++)
			{
				CV_MAT_ELEM(*matSkip,unsigned,i,j) = 
					CV_MAT_ELEM(*matSkip,unsigned,i-1,j) + 
					CV_MAT_ELEM(*matSkip,unsigned,i,j-1) -
					CV_MAT_ELEM(*matSkip,unsigned,i-1,j-1) +
					CV_IMAGE_ELEM(lp_img_binary,uchar,i,j)/255;
				XdPrint("%5d ",CV_MAT_ELEM(*matSkip,unsigned,i,j));
			}
			XdPrint("\n");
		}*/
		//竖直方向的跳变次数
		//int minSkipThres = 10;
		//int maxSkipTimes = -1,minSkipTimes = iHeight;
		//int* iSkipTimes = (int*)malloc(sizeof(int)*iWidth);   //跳变次数
		//for(i=0;i<iWidth;i++)
		//{
		//	uchar* ptr = (uchar*)
		//		(lp_img_binary->imageData);
		//	t = 0;
		//	for(j=0;j<iHeight-1;j++)
		//		if(ptr[j*lp_img_binary->widthStep+i] != 
		//			ptr[(j+1)*lp_img_binary->widthStep+i])
		//			t++ ;

		//	if(maxSkipTimes < t)
		//		maxSkipTimes = t;
		//	if(minSkipTimes > t)
		//		minSkipTimes = t;
		//	iSkipTimes[i] = t;
		//	XdPrint("%d\n",t);
		//}	

		//LPRFree(iSkipTimes);

	}
	//定位
	{
		//返回定位结果
		int x,y;
		int width ;
		int height;
		if(d-u < MIN_LP_HEIGHT)
		{
			y = 0;
			height = iHeight;
		}
		else
		{
			y = u;
			height = d - u;
		}
		if(r-l < MIN_LP_WIDTH)
		{
			x = 0;
			width = iWidth;
		}
		else
		{
			x = l;
			width = r - l;
		}
		*rect = cvRect(x,y, width,height);
	}


	//if(1)
	//{
	//	//查看车牌直方图

	//	IplImage* img_show = cvCreateImage(cvGetSize(lp_img_binary),8,3);

	//	cvZero(img_show);


	//	for(int i=0;i<img_show->width;i++)
	//		cvLine(img_show,cvPoint(i,img_show->height),
	//		cvPoint(i,img_show->height-histHor[i]),cvScalar(255,255,255),1);
	//	cvLine(img_show,cvPoint(0,img_show->height-histHorMeanPix),
	//		cvPoint(img_show->width,img_show->height-histHorMeanPix),cvScalar(0,0,255),1);

	//	cvShowImage("img_slhow",img_show);

	//	cvReleaseImage(&img_show);

	//	//cvDestroyAllWindows();
	//}
	LPRFree(histHor);
	return 1;

}
//根据边缘图找到左右边界
void FindEdgeImageBorderLR(const IplImage* img_edge, int* l,int* r)
{
	int i,t;
	int iWidth = img_edge->width;
	int iHeight = img_edge->height;
	int pixThres,minHorDis = 3;
	int histHorMeanPix = 0;  //水平像素平均值

	int *histHor = (int*)malloc(iWidth*sizeof(int));

	//求图像边缘图直方图
	ReturnHist(img_edge,histHor,255,1);

	for(i=0;i<iWidth;i++)
	{
		histHorMeanPix += histHor[i]; 
	}
	histHorMeanPix /= iWidth;
	pixThres = max(histHorMeanPix*0.6,2);

	//左边界定位
	t = 0;
	*l = 0;
	for(i=0;i<iWidth/3;i++)
	{
		if(histHor[i] < histHorMeanPix*0.7)
		{
			*l = i;
		}
		else if(histHor[i] >= iHeight*0.9)
		{
			*l = i;
			break;
		}
		else
			break;
		/*else			
		{
			t++;
			while(t <= minHorDis 
				&& ++i < iWidth/3)
			{
				if(histHor[i] < pixThres)
					break;
				else if(histHor[i] > 2*histHorMeanPix)
					t += 2;
				else
					t++;
			}
			if(t >= minHorDis)
				break;
			else
			{
				t = 0;
				*l = i;
			}
		}*/
	}

	//右边界定位,右边界定位不同于左边界的定位
	t = 0;
	*r=iWidth;
	for(i=iWidth;i>=iWidth/3;i--)
	{
		if(histHor[i] < pixThres)
		{
			*r = i;
		}
		else if(histHor[i] > iHeight*0.9)
		{
			*r = i;
			break;
		}
		else
			break;
		/*else

		{
			t++;
			while(t <= minHorDis 
				&& --i >= iWidth/3)
			{
				if(histHor[i] < pixThres ||
					histHor[i] > iHeight*0.92)
					break;
				else
					t++;
			}
			if(t > minHorDis)
				break;
			else
			{
				t = 0;
				*r = i;
			}
		}*/
	}
	if(SHOW_BORDER)
	{
		//查看车牌直方图

		IplImage* img_show = cvCreateImage(cvGetSize(img_edge),8,3);

		cvZero(img_show);


		for(int i=0;i<img_show->width;i++)
			cvLine(img_show,cvPoint(i,img_show->height),
			cvPoint(i,img_show->height-histHor[i]),cvScalar(255,255,255),1);
		cvLine(img_show,cvPoint(0,img_show->height-histHorMeanPix),
			cvPoint(img_show->width,img_show->height-histHorMeanPix),cvScalar(0,0,255),1);

		cvShowImage("img_show",img_show);
		cvShowImage("img_show_edge",img_edge);
		cvReleaseImage(&img_show);

		cvWaitKey(0);

		cvDestroyAllWindows();
		
	}
	LPRFree(histHor);

}
//根据边缘图找到上下边界
void FindEdgeImageBorderUD(const IplImage* img_edge,int *u,int* d)
{

	int i;
	*u=0,*d=img_edge->height;
	
	int minVerPix = img_edge->width/20;  //竖直像素最小值
	int minHorDis = img_edge->width/5;   //水平方向上相邻两个比较小的最小距离


	int *histVer = (int*)malloc(img_edge->height*sizeof(int));
	int histVerMeanPix = 0;  //竖直像素平均值
	/*int iMinPix = img_edge->width;
	int iMaxPix = 0;*/
	//求图像边缘图直方图

	ReturnHist(img_edge,histVer,255,0);

	{
		//求竖直像素平均值
		for(i=0;i<img_edge->height;i++)
			histVerMeanPix += histVer[i];
		histVerMeanPix /= img_edge->height;
	}
	//上边界定位
	for(i=0;i<img_edge->height/3;i++)
	{
		if((histVer[i] < histVerMeanPix*0.5
			|| histVer[i] > histVerMeanPix * 2.4))
		{
			*u = i;
		}
		/*if(histVer[i] <= minVerPix ||
			histVer[i] > 0.8 * img_edge->width)
			*u = i;
		else
			break;*/
	}
	//下边界定位
	for(i=img_edge->height;i>img_edge->height/3*2;i--)
	{

		if(histVer[i] < histVerMeanPix*0.5
			|| histVer[i] > histVerMeanPix * 2.2)
		{
			*d = i;
		}
	
	}
	if(SHOW_BORDER)
	{
		//查看车牌直方图
		IplImage* img_draw = cvCreateImage(cvGetSize(img_edge),8,3);
	
		cvZero(img_draw);
	


		for(int i=0;i<img_draw->height;i++)
			cvLine(img_draw,cvPoint(0,i),cvPoint(histVer[i],i),cvScalar(255,255,255),1);

		cvLine(img_draw,cvPoint(histVerMeanPix,0),
			cvPoint(histVerMeanPix,img_draw->height),cvScalar(0,0,255),2);

	
		cvShowImage("img_draw",img_draw);

		cvReleaseImage(&img_draw);


		//cvDestroyAllWindows();
	}
	LPRFree(histVer);
}


IplImage* ResizeLPR(const IplImage* pic, CvRect* rect,int color)
{

	float r;
	int offL=0,offR=0,offT=0,offB=0;
	CvRect rectOld = *rect;
	colorD* colord = NULL; 
	IplImage* img_lpr = GetRectImage(pic,*rect);
	IplImage* img_color = ReturnColorImage(img_lpr);
	//当图像较暗时利用灰度图进行调整
	colord = AnalyseColorDistribution(img_color);
	r = (float)colord->color[color]/rect->height/rect->width;
	
	if(r >= 0.1 ) 
	{//如果车牌颜色较弱则不进行调整
		
		//缩小车牌区域
		//ReduceLPR(img_color,rect,&offL,&offR,&offT,&offB,color);

		////由于左右边经常残缺，所以向左边和右边扩展一下	
		//if(offL == 0 && offR == 0)
		//	ExtendLPR(rect,&offL,&offR,color);

		////调整矩形大小
		//AdujustRect(rect,offL,offR,offT,offB);  
	}
	
	free(colord);
	cvReleaseImage(&img_color);
	cvReleaseImage(&img_lpr);
	return GetRectImage(pic,*rect);

}


void ThresholdAccordColor(const IplImage* src,IplImage* dst,int color)
{

}
//根据角度进行旋转，角度为水平正方向,angle为弧度
void ImageRotate(const IplImage* src,IplImage* dst,
				double angle,CvScalar fillVal,int hor)
{
	double scale = 1.0;
	double a = 0.0;	
	CvMat* rot_mat = cvCreateMat(2,3,CV_32FC1);
	
	CvPoint2D32f center = cvPoint2D32f(
		0,
		0
		);

	if(hor)
	{
		a= -(angle)*180/CV_PI;
		cv2DRotationMatrix(center,a,scale,rot_mat);
	}
	else
	{
		cvSet2D(rot_mat,0,0,cvScalar(1));
		cvSet2D(rot_mat,0,1,cvScalar(0));
		cvSet2D(rot_mat,0,2,cvScalar(0));
		cvSet2D(rot_mat,1,0,cvScalar(-1/tan(angle)));
		cvSet2D(rot_mat,1,1,cvScalar(1));
		cvSet2D(rot_mat,1,2,cvScalar(0));
	}
	/*cvSet2D(rot_mat,0,0,cvScalar(1/cos(angle)));
	cvSet2D(rot_mat,0,1,cvScalar(-tan(angle)));
	cvSet2D(rot_mat,0,2,cvScalar(0));
	cvSet2D(rot_mat,0,2,cvScalar(1));
	cvSet2D(rot_mat,0,2,cvScalar(0));*/

	cvWarpAffine(src,dst,rot_mat,CV_WARP_FILL_OUTLIERS,fillVal);

	
	//对超出的边界的部分用原图中的像素点进行填充
	/*for(int i=0;i<src->height;i++)
	{
		uchar* ptr = (uchar*)(dst->imageData+i*dst->widthStep);
		uchar* ptr1 = (uchar*)(src->imageData+i*src->widthStep);
		for(int j=0;j<src->width;j++)
		{
			if(ptr[j*3] == 0 && ptr[j*3+1] == 0 && ptr[j*3+2] == 0)
			{
				ptr[j*3] = ptr1[j*3];
				ptr[j*3+1] = ptr1[j*3+1];
				ptr[j*3+2] = ptr1[j*3+2];
			}
		}
	}*/

	cvReleaseMat(&rot_mat);

}
float FindAngel(const IplImage* rgb_img,int threshold,int hor)
{
	float ret = -1.0; 
	CvSize size = cvGetSize(rgb_img);
	CvMemStorage* storage = cvCreateMemStorage(0);
	
	IplImage* img_gray = cvCreateImage(size,IPL_DEPTH_8U,1);
	IplImage* img_sobel_bw = cvCreateImage(size,IPL_DEPTH_8U,1);


	cvCvtColor(rgb_img,img_gray,CV_BGR2GRAY);
	cvSmooth(img_gray,img_gray,CV_MEDIAN);
	GrayStretch(img_gray,img_gray);
	//求边缘图
	if(hor)
		ReturnSobelImage(img_gray,img_sobel_bw,0,1);
	else
		ReturnSobelImage(img_gray,img_sobel_bw,1,0);
	


	/*cvShowImage("img_sobel",img_sobel_bw);
	cvWaitKey(0);*/
	
	//利用哈弗变换求图中的直线
	CvSeq* result = cvHoughLines2(
		img_sobel_bw,
		storage,
		CV_HOUGH_STANDARD,
		1,
		CV_PI/180,
		threshold);

	if(result->total != 0)
	{
		float* line = (float*)cvGetSeqElem(result,0);
		ret = (float)CV_PI/2 - line[1];
		//IplImage* img_draw = cvCloneImage(img_gray);
		//cvZero(img_draw);
		//for(int i=0;i<result->total;i++)
		//{
		//	float* l = (float*)cvGetSeqElem(result,i);
		//	cvLine(img_draw,cvPoint(0,l[0]/sin(l[1])),cvPoint(l[0]/cos(l[1]),0),cvScalar(255,255,255),1);
		//	/*CvPoint* line = (CvPoint*)cvGetSeqElem(result,i);
		//	cvLine(img_draw,line[0],line[1],cvScalar(255,255,255),1);*/
		//}
		//
		//cvShowImage("img_draw",img_draw);
		//cvWaitKey(0);
	}
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&img_gray);
	cvReleaseImage(&img_sobel_bw);
	

	return ret;
	
}
//寻找车牌轮廓从而定位车牌
int FindLpContour(const IplImage* rgb_img,CvRect* out_rect)
{
	int ret = 1;
	int rect_num = 0;
	CvRect* lp_list = NULL;

	//找出所有可能的候选区域
	rect_num = GetLpcList(rgb_img,&lp_list);
	if(rect_num == 0)
		ret = 0;
	//找出车牌
	{		
		struct LPR* lpr = NULL;
		lpr = DetermineLPArea(rgb_img,lp_list,rect_num);
		
		if(lpr != NULL)
			*out_rect = lpr->rect;
		else
			ret = 0;
		FreeLPR(&lpr);
	}
	LPRFree(lp_list);
	return ret;
}
IplImage* LPRotateFromRect(const IplImage* src_lpr,CvRect in_rect,CvRect* out_rect)
{
	double angleH,angleV;
	IplImage* img_result = NULL;
	IplImage* img_extend = NULL;
	IplImage* img_extend_dst = NULL;
	IplImage* img_lpr = NULL;
	CvRect rect_extend;
	//对车牌区域进行扩展后进行旋转
	rect_extend.x = max(0,in_rect.x - SPACEING*1.5);
	rect_extend.width = min(in_rect.width+2*SPACEING,src_lpr->width-rect_extend.x);
	rect_extend.y = max(0,in_rect.y - SPACEING);
	rect_extend.height = min(in_rect.height+2*SPACEING,src_lpr->height-rect_extend.y);

	img_lpr = GetRectImage(src_lpr,in_rect);
	img_extend = GetRectImage(src_lpr,rect_extend);
	
	
	angleH = FindAngel(img_lpr,img_lpr->height);

	{//旋转
		img_extend_dst = cvCloneImage(img_extend);

		ImageRotate(img_extend,img_extend_dst,angleH);


		if(FindLpContour(img_extend_dst,out_rect))
		{
			img_result = cvCloneImage(img_extend_dst);
		}
		else
		{
			
			img_result = GetRectImage(src_lpr,in_rect);	
			*out_rect = cvRect(0,0,img_result->width,img_result->height);
		}

		//显示
		//cvShowImage("img_extend",img_extend);
		//cvShowImage("img_extend_dst",img_extend_dst);
		/*cvShowImage("img_lpr",img_lpr);
		cvWaitKey(0);*/

		cvReleaseImage(&img_extend_dst);
	}
	
	cvReleaseImage(&img_lpr);
	cvReleaseImage(&img_extend);	
	
	return img_result;
}
