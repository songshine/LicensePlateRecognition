#include "stdafx.h"
#include "LPLocation.h"
#include "LPHelper.h"

#define MIN_LP_REGION 1000
#define MAX_LP_REGION 20000
#define DEFAULT_IMAGE_WIDTH 720
#define DEFAULT_IMAGE_HEIGHT 480
#define MAX_LP_WIDTH 300
#define MAX_LP_HEIGHT 100
#define MIN_LP_WIDTH 60
#define MIN_LP_HEIGHT 12
#define MAX_PIC_AREA  600000

IplImage* g_img = NULL;   //彩色图
IplImage* g_img_gray = NULL;  //灰度图
IplImage* g_img_foreground = NULL;  //顶帽操作后的前景图
IplImage* g_img_bw = NULL;   //二值化图
IplImage* g_img_canny = NULL;  //canny边界图
IplImage* g_img_sobel = NULL;  //sobel边界图
//IplImage* g_img_color = NULL;




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
};

/**********************************局部函数声明*************************/

/*首先车牌初步定位,返回矩形位置在rect，车牌位置的灰度图放在返回图像中*/
int LPCoarseLocate(CvRect** rect);

/*精确定位*/
struct LPR* LPPreciseLocate(CvRect* rectList,int rectNum); 

//扫描边缘图，返回一个车牌所在的区域
int* ScanEdgeImage(const IplImage* img_edge);

void ResizeRect(struct colorD* horColor,struct colorD* verColor,
				CvRect* pRect,int color);
//根据HSV三个分量值确定颜色
int ReturnColor(float valH,float valS,float valV);
//获取颜色图
IplImage* ReturnColorImage(const IplImage* img);
//统计HSV图中蓝、黄、白、黑四种颜色所占比例 
void StatisticsHSV(const IplImage* img,float* rb,float* ry,float* rw,float* rbk);
//统计统计HSV图中蓝、黄、白、黑四种颜色的分布情况
IplImage* StatisticsColorDistribution(const IplImage* img,struct colorD** horColor,
							struct colorD** verColor);
//返回颜色的边界的像素的个数
int ReturnEdgeWithColor(const IplImage* img_color,const IplImage* img_edge,int color);

//返回边界处的灰度图的差的值
int ReturnEdgeWithGrayLevel(const IplImage* img_edge,const IplImage* img_gray,int color);
//获取每一行颜色变化次数
int ReturnColorDiffers(const IplImage* img_color,int color);

//计算方差
void GetVariance(struct colorD* verColor,int h,int color,float* avg,float* var);

//利用膨胀腐蚀调整蓝色车牌矩形区域大小
void ResizeRectImage(CvRect* rect);

//根据颜色精确定位车牌区域
void ResizeLPR(const IplImage* img_color,CvRect* rect,int color);

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
//计算某一矩形域的权值
float ComputeWeightFromRect(IplImage* img_color,struct colorD* horColor,
struct colorD* verColor,CvRect rect,int* color);

/*水平二次差分
即I(x,y) = I(x,y)-I(x,y-2)*/
void TwiceDiff(const IplImage* src,IplImage* dst,const IplImage* img_gray);

//消除过长或过短的边缘线
void Cnp(const IplImage* src,IplImage* dst);

//在已知车牌颜色的情况下,计算某一区域为车牌的可能性
float GetPossibility(const IplImage* img_color,CvRect rect,int color);

//根据边缘密度确定车牌区域
void FindLprAboutEdgeDensity(const IplImage* img_edge,IplImage* img);
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
	IplImage* imgLPP = NULL;	//精确定位结果
	CvRect* rectLPs = NULL;
	struct LPR* lpr = NULL;
	int rectNum;	
	long timeSart,timeSpendA = 0,timeSpendC=0,timeSpendL = 0;
	timeSart = GetTickCount();
	if(src->width * src->height > MAX_PIC_AREA)
	{
		bflagG = 1;
		g_img = GetRectImage(src,cvRect(0,src->height/2,src->width,src->height/2));
	}
	else
	{
		g_img = cvCloneImage(src);
	}
	
	g_img_gray = cvCreateImage(cvGetSize(g_img),IPL_DEPTH_8U,1);
	g_img_foreground = cvCreateImage(cvGetSize(g_img),IPL_DEPTH_8U,1);
	g_img_canny = cvCreateImage(cvGetSize(g_img_gray),IPL_DEPTH_8U,1);
	g_img_bw = cvCreateImage(cvGetSize(g_img_gray),IPL_DEPTH_8U,1);
	g_img_sobel = cvCreateImage(cvGetSize(g_img_gray),IPL_DEPTH_8U,1);

	
	//g_img_color  = ReturnColorImage(g_img);
	timeSpendA =  GetTickCount() - timeSart;
	//转化为灰度图
	//cvResize(src,img);
	//cvCvtColor(img,img_bw,CV_RGB2GRAY);  
	////平滑高斯滤波  滤波后的图片保存在grayImage
	//cvSmooth(img_bw,img_bw,CV_GAUSSIAN,3,0,0);
	cvCvtColor(g_img,g_img_gray,CV_RGB2GRAY); 
	cvSmooth(g_img_gray,g_img_gray,CV_GAUSSIAN,3,0,0);
	
	//cvEqualizeHist(g_img_gray,g_img_gray);
	//初步定位
	
	rectNum = LPCoarseLocate(&rectLPs);
	timeSpendC = GetTickCount() - timeSart - timeSpendA;
	//long timeSart,timeSpend=0;
	if(rectNum != 0)
	{//精确定位

		//timeSart = GetTickCount();
		lpr = LPPreciseLocate(rectLPs,rectNum);
		//timeSpend = GetTickCount() - timeSart;
		timeSpendL =  GetTickCount() - timeSart - timeSpendC - timeSpendA;


	}
	if(lpr != NULL)
	{
		imgLPP = GetRectImage(g_img_gray,lpr->rect);
		*color = lpr->color;

		*rect = lpr->rect;
		if(bflagG)
			rect->y += g_img->height; 
	}
	free(rectLPs);
	free(lpr);
	lpr = NULL;
	rectLPs = NULL;

	cvReleaseImage(&g_img);
	cvReleaseImage(&g_img_gray);
	cvReleaseImage(&g_img_foreground);
	cvReleaseImage(&g_img_bw);
	cvReleaseImage(&g_img_canny);
	//cvReleaseImage(&g_img_color);
	cvReleaseImage(&g_img_sobel);

	return imgLPP;
}
int LPCoarseLocate(CvRect** rect)
{
	IplImage* img_mo = NULL;  //形态学操作之后的图像

	//寻找LP边缘
	int mode = CV_RETR_EXTERNAL;	//轮廓边缘提取时的参数
	CvSeq * contour = 0;		//轮廓边缘提取时的参数
	CvRect bndRect = cvRect(0,0,0,0);//用cvBoundingRect画出外接矩形时需要的矩形
	CvMemStorage * storage = cvCreateMemStorage(0);//轮廓边缘提取时的参数
	IplConvKernel* element0 = NULL;
	long ts,t1,t2,t3,t4;
	//每一个矩形区域的连通域数量
	int contourNum,rectId = 0;
	img_mo = cvCreateImage(cvGetSize(g_img_canny),IPL_DEPTH_8U,1);
	element0 = cvCreateStructuringElementEx(12,1,5,0,CV_SHAPE_RECT,NULL);	
	ts = GetTickCount();
	/*1.顶帽操作*/
	cvMorphologyEx(g_img_gray,g_img_foreground,NULL,element0,CV_MOP_TOPHAT,1);
	t1 = GetTickCount() - ts;
	/*图像二值化*/
	double me = cvMean(g_img_foreground);
	cvThreshold(g_img_foreground,g_img_bw,/*min(30,8+pow(2,me))*/8+me,255,CV_THRESH_BINARY);
	SmoothBinImg(g_img_bw,24,SMOOTH_WHITE_CC);
	cvNot(g_img_bw,g_img_bw);

	t2 = GetTickCount() - ts - t1;
	TwiceDiff(g_img_bw,g_img_canny,g_img_gray);
	//SmoothBinImg(g_img_canny,16,SMOOTH_WHITE_CC);
	//Cnp(g_img_canny,g_img_canny);
	FindLprAboutEdgeDensity(g_img_canny,img_mo);
	//cvShowImage("img_mo",img_mo);
	t3 = GetTickCount() - ts - t1 -t2;
	//cvShowImage("img_canny",g_img_canny);
	//cvShowImage("img_mo",img_mo);
	contourNum = cvFindContours(img_mo, storage, &contour, sizeof(CvContour),
		mode, CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0));
	(*rect) = (CvRect*)malloc(sizeof(CvRect)*contourNum);
	for(;contour!=0;contour=contour->h_next)
	{	
		float avg,var;
		double area = fabs(cvContourArea(contour));
		//得到一个可以将移动轮廓包围的矩形区域
		bndRect = cvBoundingRect(contour,0);
		if(bndRect.height <= 8 || area/bndRect.width/bndRect.height < 0.4)
			continue;
		//去除面积很小的区域
		if(bndRect.height*bndRect.width < MIN_LP_REGION)
			continue;
		////去除面积太大的区域
		if(bndRect.height*bndRect.width > MAX_LP_REGION)
			continue;
		////去除宽高比不合理的区域
		if((double)bndRect.width/bndRect.height < 2 ||
			(double)bndRect.width/bndRect.height > 10.1)
			continue;
		if(bndRect.width < (5*bndRect.height-(bndRect.height*bndRect.height)/10))
			continue;

		//计算水平方向的边界数量
		GetRectEdgeImageStatistics(g_img_canny,bndRect,&avg,&var);
		if(avg < 10 )
			continue;

		bndRect.x = bndRect.x<10?0:bndRect.x-10;
		bndRect.y = bndRect.y<2?0:bndRect.y-2;
		bndRect.width = bndRect.width+20+bndRect.x > g_img_gray->width?g_img_gray->width-bndRect.x:bndRect.width+20;
		bndRect.height = bndRect.height+2+bndRect.y > g_img_gray->height?g_img_gray->height-bndRect.y:bndRect.height+2;
		(*rect)[rectId++] = bndRect;
	}
	t4 = GetTickCount() - ts - t1 - t2 - t3;
	cvReleaseStructuringElement(&element0);
	cvReleaseImage(&img_mo);

	cvReleaseMemStorage(&storage);

	//返回车牌位置
	return rectId;
}



/*精确定位*/
struct LPR* LPPreciseLocate(CvRect* rectList,int rectNum)
{
	float w,minw = -1;
	int minId = -1;
	int color,colormin;
	struct LPR* lpr = NULL;

	IplImage** img_rects;
	IplImage** color_imgs;
	struct colorD** horColorDs,**verColorDs;
	//开始计算
	{
		//分配内存
		color_imgs = (IplImage**)malloc(sizeof(IplImage*)*rectNum);
		img_rects = (IplImage**)malloc(sizeof(IplImage*)*rectNum);
		horColorDs = (struct colorD**)malloc(sizeof(struct colorD*)*rectNum);
		verColorDs = (struct colorD**)malloc(sizeof(struct colorD*)*rectNum);
		//计算各个候选区域的权值
		for(int i=rectNum-1;i>=0;i--)
		{  
			img_rects[i] = GetRectImage(g_img,rectList[i]);
			color_imgs[i] = StatisticsColorDistribution(img_rects[i],&horColorDs[i],&verColorDs[i]);
			w = ComputeWeightFromRect(color_imgs[i],horColorDs[i],verColorDs[i],rectList[i],&color);
			if(w > minw)
			{
				minw = w;
				minId = i;
				colormin = color;
			}
			/*	cvShowImage("song",img_rects[i]);
			cvWaitKey(0);*/
		}

	}

	if(minId != -1)
	{

		lpr = (struct LPR*)malloc(sizeof(struct LPR));		
		lpr->color = colormin;		
		ResizeLPR(color_imgs[minId],&rectList[minId],colormin);
		lpr->rect = rectList[minId];
		
	}
	
	{
		//释放内存
		int i;
		for(i=0;i<rectNum;i++)
		{
			cvReleaseImage(&img_rects[i]);
			cvReleaseImage(&color_imgs[i]);
			free(horColorDs[i]);
			free(verColorDs[i]);
		}
		free(horColorDs);
		free(verColorDs);
	}
	return lpr;

}

void TwiceDiff(const IplImage* src,IplImage* dst,const IplImage* img_gray)
{
	uchar *p1,*p2,*p3;
	int maxVal;
	int height = src->height;
	int width = src->width;
	//cvShowImage("img_bw",src);
	for(int i=0;i<height-1;i++)
		for(int j=0;j<width-2;j++)
		{
			p1 = (uchar*)(src->imageData + src->widthStep*i+j);
			p2 = (uchar*)(src->imageData + src->widthStep*(i+1)+j);
			p3 = (uchar*)(src->imageData + src->widthStep*i+j+2);
			
			maxVal = max(abs(p1[0] - p2[0]),abs(p1[0]-p3[0]));

			
			CV_IMAGE_ELEM(dst,uchar,i,j) = maxVal/*max(max(val1,val2),val3)*/;
		}
		/*cvShowImage("ig",dst);
		cvWaitKey(0);*/
}
void  FindLprAboutEdgeDensity(const IplImage* img_edge,IplImage* img)
{
	int i,j,h;

	IplImage* img_ed = cvCloneImage(img_edge);
	IplConvKernel *element1 = NULL,*element2 = NULL,
		*element3 = NULL,*element4 = NULL;
	/*long s,s1,s2,s3;
	s = GetTickCount();*/
	int* flags = ScanEdgeImage(img_ed);
	
	
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

	cvReleaseStructuringElement(&element1);
	cvReleaseStructuringElement(&element2);
	cvReleaseStructuringElement(&element3);
	cvReleaseImage(&img_ed);
	free(flags);
	flags = NULL;
	//s2 = GetTickCount() - s;
	//return img;
}
//扫描边缘图，返回一个车牌所在的区域
int* ScanEdgeImage(const IplImage* img_edge)
{
	int i,j,t,s;
	//CvPoint ul[128],dr[128]
	int left=0,right=0,top,below;

	int imgWidth = img_edge->width;
	int imgHeight = img_edge->height;
	int thresWidth = 24;
	int thresHeight = 12;
	int nPixNum = 0,nPixWidth = 0;
	int* flags = (int*)calloc(imgHeight+1,sizeof(int));
	IplImage* img_e = cvCloneImage(img_edge);
	IplImage* img_mask = cvCreateImage(cvGetSize(img_edge),IPL_DEPTH_8U,1);
	cvZero(img_mask);
//	memset(flags,0,imgHeight);

	for(i=1;i<imgHeight-1;i++)
	{
		for(j=1;j<imgWidth-1;j++)
		{
			if(CV_IMAGE_ELEM(img_e,uchar,i,j) == 255
				&& CV_IMAGE_ELEM(img_mask,uchar,i,j) != 1)
			{
				//for(t = j+1;t)
				nPixWidth = 0;
				CV_IMAGE_ELEM(img_mask,uchar,i,j) = 1;
				right = left = j;

				while(left < imgWidth && left - right < thresWidth)
				{
					if(CV_IMAGE_ELEM(img_e,uchar,i,left) == 255
						&& CV_IMAGE_ELEM(img_mask,uchar,i,left) != 1)
					{
						nPixWidth++;
						right = left;
						CV_IMAGE_ELEM(img_mask,uchar,i,left) = 1;
					}
					left++;
				}
				if(right - j < MIN_LP_WIDTH || right - j >MAX_LP_WIDTH || nPixWidth < 5)
				{
					j  = left-1;
					continue;
				}
				else
				{
					t = i+1;
					do
					{//向下
						nPixNum = 0;
						for(s = j;s < left && s < imgWidth;s++)
						{
							if(CV_IMAGE_ELEM(img_e,uchar,t,s) == 255
								&& CV_IMAGE_ELEM(img_mask,uchar,t,s) != 1)
							{
								nPixNum++;
								CV_IMAGE_ELEM(img_mask,uchar,t,s) = 1;
							}
						}
						t++;
					}while(nPixNum >= nPixWidth/3 && t < imgHeight);
					below = t;

					t = i-1;
					do
					{//向上
						nPixNum = 0;
						for(s = j;s < left && s < imgWidth;s++)
						{
							if(CV_IMAGE_ELEM(img_e,uchar,t,s) == 255
								&& CV_IMAGE_ELEM(img_mask,uchar,t,s) != 1)
							{
								CV_IMAGE_ELEM(img_mask,uchar,t,s) = 1;
								nPixNum++;
							}
						}
						t--;
					}while(nPixNum >= nPixWidth/3 && t > 0);
					top = t;


					if(below - top < MIN_LP_HEIGHT || below - top > MAX_LP_HEIGHT)
						continue;
					else
					{
						if(right - j > below - top)
						{
							//cvRectangle(img_e,cvPoint(j,top),cvPoint(right,below),CV_RGB(255,255,255),2);
							for(s = max(0,top-2);s<= below;s++)
								flags[s] = 1;
						}
					}
				}

			}
		}
	}
	//cvShowImage("img_e",img_e);
	//cvWaitKey(0);
	cvReleaseImage(&img_e);
	cvReleaseImage(&img_mask);
	return flags;
}
void Cnp(const IplImage* src,IplImage* dst)
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
					if(val > 80)
						CV_IMAGE_ELEM(dst,uchar,i,j) = 0;
				}

				cvReleaseImage(&M);
				cvReleaseImage(&N);


}




//根据HSV三个分量值确定颜色
int ReturnColor(float valH,float valS,float valV)
{

	if(valH >= 200 && valH <= 248)
	{
		if(valS >= 0.16 && valV >= 0.18)
		{//蓝色
			return COLOR_BLUE;
		}
		//return COLOR_WHITE;

	}
	if(valH >= 20 && valH <= 70)
	{
		if(valS >= 0.16 && valV >= 0.18)
		{//黄色
			return COLOR_YELLOW;
		}
		//return COLOR_BLACK;

	}
	if(valS <= 0.5 && valV >= 0.4)
	{//白色
		return COLOR_WHITE;
	}
	if( valV <= 0.5)
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
			for(i=0;i<width;i++)
			{	
				valH = (float)CV_IMAGE_ELEM(h_plane,uchar,j,i)*2;
				valS = (float)CV_IMAGE_ELEM(s_plane,uchar,j,i)/255;
				valV = (float)CV_IMAGE_ELEM(v_plane,uchar,j,i)/255;
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
IplImage* StatisticsColorDistribution(const IplImage* img,struct colorD** horColor,
struct colorD** verColor)
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
		//分别记录水平和竖直的像素分布
		*horColor = (struct colorD*)malloc(sizeof(struct colorD)*width);
		*verColor = (struct colorD*)malloc(sizeof(struct colorD)*height);

		//初始化水平像素分布
		for(i=0;i<width;i++)
		{
			(*horColor)[i].color[COLOR_BLUE] = 0;
			(*horColor)[i].color[COLOR_YELLOW] = 0;
			(*horColor)[i].color[COLOR_WHITE] = 0;
			(*horColor)[i].color[COLOR_BLACK] = 0;
		}
		for(j=0;j<height;j++) 
		{	

			(*verColor)[j].color[COLOR_BLUE] = 0;
			(*verColor)[j].color[COLOR_BLACK] = 0;
			(*verColor)[j].color[COLOR_WHITE] = 0;
			(*verColor)[j].color[COLOR_YELLOW] = 0;
			for(i=0;i<width;i++)
			{	
				valH = (float)CV_IMAGE_ELEM(h_plane,uchar,j,i)*2;
				valS = (float)CV_IMAGE_ELEM(s_plane,uchar,j,i)/255;
				valV = (float)CV_IMAGE_ELEM(v_plane,uchar,j,i)/255;
				c = ReturnColor(valH,valS,valV);
				CV_IMAGE_ELEM(retImg,uchar,j,i) = c;
				(*verColor)[j].color[c]++;
				(*horColor)[i].color[c]++;		

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

		for(i=0;i<width;i++)
		{	
			valH = (float)CV_IMAGE_ELEM(h_plane,uchar,j,i)*2;
			valS = (float)CV_IMAGE_ELEM(s_plane,uchar,j,i)/255;
			valV = (float)CV_IMAGE_ELEM(v_plane,uchar,j,i)/255;
			CV_IMAGE_ELEM(color_img,uchar,j,i) = ReturnColor(valH,valS,valV);
		}

	}	
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&h_plane);
	cvReleaseImage(&s_plane);
	cvReleaseImage(&v_plane);
	return color_img;
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
int ReturnEdgeWithGrayLevel(const IplImage* img_edge,const IplImage* img_gray,int color)
{
	long sum = 0;
	int num = 0;
	int val1,val2;
	int width = img_edge->width;
	int height = img_edge->height;
	for(int i=1;i<width-1;i++)
		for(int j=1;j<height-1;j++)
		{
			if(CV_IMAGE_ELEM(img_edge,uchar,j,i) == 255)
			{
				val1 = abs(CV_IMAGE_ELEM(img_gray,uchar,j,i)-CV_IMAGE_ELEM(img_gray,uchar,j,i-1));
				val2 = abs(CV_IMAGE_ELEM(img_gray,uchar,j,i)-CV_IMAGE_ELEM(img_gray,uchar,j,i+1));
				sum += max(val1,val2);
				num++;
			}
		}

		return (sum/num);
}
//返回颜色的边界的像素的个数
int ReturnEdgeWithColor(const IplImage* img_color,const IplImage* img_edge,int color)
{
	int ret = 0;
	int width = img_edge->width;
	int height = img_edge->height;
	for(int i=1;i<width-1;i++)
		for(int j=1;j<height-1;j++)
		{
			if(CV_IMAGE_ELEM(img_edge,uchar,j,i) == 255)
			{
				if(CV_IMAGE_ELEM(img_color,uchar,j,i) == color && 
					CV_IMAGE_ELEM(img_color,uchar,j,i+1) == color)
				{
					ret++;
				}	

			}
		}

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
		for(j=0;j<width;j++)
		{
			if(CV_IMAGE_ELEM(img,uchar,i,j) == 255)
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
			for(i=0;i<width;i++)
			{	
				valH = (float)CV_IMAGE_ELEM(h_plane,uchar,j,i)*2;
				valS = (float)CV_IMAGE_ELEM(s_plane,uchar,j,i)/255;
				valV = (float)CV_IMAGE_ELEM(v_plane,uchar,j,i)/255;
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
			for(i=0;i<width-1;i++)
			{
				val1 = CV_IMAGE_ELEM(colorImg,uchar,j,i);
				val2 = CV_IMAGE_ELEM(colorImg,uchar,j,i+1);

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
	/*CString str;
	str.Format("nb_w = %d,ny_b = %d",nb_w,ny_b);
	AfxMessageBox(str);*/
}


/************************************************************************/
/*计算某一矩形域的权值，并调整大小
@src:彩色图*/
/************************************************************************/
float ComputeWeightFromRect(IplImage* img_color,struct colorD* horColor,
struct colorD* verColor,CvRect rect,int* color)
{
	int nColor;
	float ret;
	//IplImage* img_canny = GetRectImage(g_img_canny,rect);
	//ret = 1/abs((float)rect.width/rect.height - 3.14);
	//求四种颜色出现的概率，在这里只考虑了蓝牌和黄牌
	{
		int i;

		float r[4],/*rb_w,ry_b,*/var,avg;
		struct colorD* colord;
		int sum = rect.width*rect.height;
		colord = AnalyseColorDistribution(img_color);
		*color = 0;
		for(i=0;i<4;i++)
		{
			r[i] = (float)colord->color[i]/sum;
		}
		if(r[COLOR_BLUE] < 0.1 && r[COLOR_YELLOW] < 0.1)
			return 0;

		if(r[COLOR_BLUE] >r[COLOR_YELLOW])
		{//蓝牌
			*color = COLOR_BLUE;
			GetVariance(verColor,rect.height,COLOR_BLUE,&avg,&var);
			nColor = colord->nb_w;

		}
		else
		{//黄牌
			*color = COLOR_YELLOW;
			GetVariance(verColor,rect.height,COLOR_YELLOW,&avg,&var);
			nColor = colord->ny_b;
		}
		//ret = (float)nColor/colord->color[*color];
		//ret = r[*color];

		free(colord);
	}


	ret = GetPossibility(img_color,rect,*color);
	//cvReleaseImage(&img_canny);
	return ret;
	//return (nColor + GetPossibility(rect,*color));
}
//在已知车牌颜色的情况下,计算某一区域为车牌的可能性
float GetPossibility(const IplImage* img_color,CvRect rect,int color)
{
	float ret;
	float avg,var/*,avgv,varv*/;		
	IplImage *img_canny = GetRectImage(g_img_canny,rect);

	IplImage *img_gray = GetRectImage(g_img_foreground,rect);
	/*cvShowImage("img_g",img_gray);
	cvWaitKey(0);*/
	//统计水平和竖直方向的边缘数量
	GetHorStatistics(img_canny,&avg,&var);
	
	//GetVerStatistcs(img_canny,&avgv,&varv);
	//DealRectImage(&rect);
	if(avg > rect.width*0.6)
		return 0;
	if(var < 0.01 || var > 0.5)
		return 1;
	/*cvShowImage("img_rect",img_gray);
	cvWaitKey(0);*/
	ret = (float)ReturnEdgeWithColor(img_color,img_canny,color)/rect.width/*/rect.width/rect.height*//*+(rect.y+rect.height)/10*/;		
	//float ret1 = (float)ReturnColorDiffers(img_color,color)/rect.width;
	ret *=ReturnEdgeWithGrayLevel(img_canny,img_gray,color);
	cvReleaseImage(&img_canny);
	cvReleaseImage(&img_gray);

	return ret;
}
//精确车牌位置和大小
void ResizeRectImage(CvRect* rect)
{//该函数仅作
	double thres;
	int offL=0,offR=0,offT=0,offB=0;
	int *hisHor,*hisVer;
	IplConvKernel* element1 = NULL;
	IplImage *img_gray = GetRectImage(g_img_gray,*rect);
	IplImage *img_bw = cvCreateImage(cvGetSize(img_gray),IPL_DEPTH_8U,1);		
	cvThreshold(img_gray,img_bw,255,255,CV_THRESH_OTSU | CV_THRESH_BINARY);
	element1 = cvCreateStructuringElementEx(15,6,7,3,CV_SHAPE_RECT,NULL);

	cvDilate(img_bw,img_bw,element1,1);
	//cvShowImage("img_bw1",img_bw);
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

	if(offB != img_bw->height && offT != img_bw->height
		&& offL != img_bw->width && offR != img_bw->width)
	{

		rect->x += offL;
		rect->y += offT;
		rect->width = rect->width-offL-offR;
		rect->height = rect->height-offT-offB;

	}
	/*cvShowImage("img_bw",img_bw);
	cvWaitKey(0);*/
	cvReleaseImage(&img_gray);
	cvReleaseImage(&img_bw);
	free(hisVer);
	free(hisHor);
	cvReleaseStructuringElement(&element1);

}
//根据颜色精确定位车牌区域
void AdujustRect(CvRect* rect,int offL,int offR,int offT,int offB)
{
	double ration1,ration2;   //调整矩形前后的宽高比
	//调整矩形
	if(offB != rect->height-1 && offT != rect->height-1
		&& offL != rect->width-1 && offR != rect->width-1)
	{
		ration1 = (double)rect->width/rect->height;
		ration2 = (double)(rect->width-offL)/rect->height;
		if(ration2 < ration1 || (ration2 > ration1 && ration2 < 6))
			rect->x += offL;
		rect->y += offT;
		ration2 = (double)(rect->width-offL-offR)/rect->height;
		if(ration2 < ration1 || (ration2 > ration1 && ration2 < 6))
			rect->width = rect->width-offL-offR;
		rect->height = rect->height-offT-offB;
	}

}
//扩展矩形
void ExtendLPR(CvRect * rect,int* offL,int* offR,int color)
{
	int nForPix,nBackPix,i,j;
	IplImage* img_tmp = NULL;
	IplImage* img_color = NULL;
	img_tmp = GetRectImage(g_img,cvRect(0,rect->y,g_img->width,rect->height));
	img_color = ReturnColorImage(img_tmp);

	//进行扩展
	//nMinPix = 2;
	if(*offL <= 1 && (float)rect->width/rect->height < 6)
	{

		for(i = rect->x-1;i>0;i--)
		{
			//tmp = 0;
			nForPix = 0;
			nBackPix = 0;
			for(j=0;j<rect->height;j++)
			{
				if(CV_IMAGE_ELEM(img_color,uchar,j,i) == color)
					nBackPix++;
				else if(CV_IMAGE_ELEM(img_color,uchar,j,i) == color+2)
					nForPix++;
			}
			if(nBackPix != 0 && nBackPix+nForPix > rect->height*0.6)
				(*offL)--;
			else
				break;
		}

	}

	if(*offR <= 1 && (float)(rect->width-*offL)/rect->height < 6)
	{
		for(i = rect->x+rect->width;i<g_img_gray->width;i++)
		{
			nForPix = 0;
			nBackPix = 0;
			for(j=0;j<rect->height;j++)
			{
				if(CV_IMAGE_ELEM(img_color,uchar,j,i) == color)
					nBackPix++;
				else if(CV_IMAGE_ELEM(img_color,uchar,j,i) == color+2)
					nForPix++;

			}
			if(nBackPix != 0 && nBackPix+nForPix > rect->height*0.6)
				(*offR)--;
			else
				break;
		}

	}
	cvReleaseImage(&img_tmp);
	cvReleaseImage(&img_color);
}
void ReduceLPR(const IplImage* img_color,CvRect* rect,int* offL,
			   int* offR,int* offT,int* offB,int color)
{
	int i,j,tmp;
	int nMinPix = 4;
	//开始根据颜色进行判断	
	{
		for(i=0;i<img_color->width-1;i++)
		{
			tmp = 0;
			for(j=0;j<img_color->height;j++)
			{
				if(CV_IMAGE_ELEM(img_color,uchar,j,i) == color)
				{
					tmp++;
				}
			}
			if(tmp < nMinPix)
				(*offL)++;
			else
				break;

		}

		for(i=img_color->width-1;i>0;i--)
		{
			tmp = 0;
			for(j=0;j<img_color->height;j++)
			{
				//val = CV_IMAGE_ELEM(img_color,uchar,j,i) ;
				if(CV_IMAGE_ELEM(img_color,uchar,j,i) == color)
				{
					tmp++;
				}
			}

			if(tmp < nMinPix)
				(*offR)++;
			else
				break;
		}


		for(i=0;i<img_color->height-1;i++)
		{
			tmp = 0;
			for(j=0;j<img_color->width;j++)
			{
				if(CV_IMAGE_ELEM(img_color,uchar,i,j) == color)
				{

					tmp++;
				}
			}
			if(tmp < nMinPix)
				(*offT)++;
			else
				break;

		}
		for(i=img_color->height-1;i>0;i--)
		{
			tmp = 0;
			for(int j=0;j<img_color->width;j++)
			{
				if(CV_IMAGE_ELEM(img_color,uchar,i,j) == color)
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
}
void ResizeLPR(const IplImage* img_color, CvRect* rect,int color)
{

	float r;
	int offL=0,offR=0,offT=0,offB=0;
	CvRect rectOld = *rect;
	colorD* colord = NULL; 
	IplImage* img_col = NULL;
	//当图像较暗时利用灰度图进行调整
	colord = AnalyseColorDistribution(img_color);
	r = (float)colord->color[color]/rect->height/rect->width;

	if(r < 0.15 ) 
		return;   //如果车牌颜色较弱则不进行调整

	ResizeRectImage(rect);   //根据实验,经过形态学定位后再经过颜色精确定位效果会更好

	//从新定位后需要从新获得颜色区域
	img_col = GetRectImage(img_color,cvRect(rect->x-rectOld.x,
		rect->y-rect->y,rect->width,rect->height));
	
	//缩小车牌区域
	ReduceLPR(img_col,rect,&offL,&offR,&offT,&offB,color);
	
	//由于左右边经常残缺，所以向左边和右边扩展一下	
	ExtendLPR(rect,&offL,&offR,color);
	
	//调整矩形大小
	AdujustRect(rect,offL,offR,offT,offB);

	
	free(colord);
	cvReleaseImage(&img_col);

}

