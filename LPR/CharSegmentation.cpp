#include "stdafx.h"
#include "CharSegmentation.h"
#include "LPRotate.h"
#include "LPHelper.h"
#include <math.h>
#define MAX_INI 65535   //正无穷

#define POINT_X  18                   // 水平方向不重叠点                        
#define POINT_Y  5                    // 垂直方向不重叠点   
#define RESIZE_IMAGE_WIDTH 160		  //车牌默认宽度
#define RESIZE_IMAGE_HEIGHT 40		  //车牌默认高度

int g_CS_threshold = 40;
int g_pixmean = 72;  //平均像素值
IplImage* g_img_lpr = NULL;
IplImage* g_img_lpr_gray = NULL;
IplImage* g_img_lpr_bw = NULL;
IplImage* g_img_lpr_thin = NULL;
struct borderParam
{
	float ah;
	float bh;
	float al;
	float bl;
};
/************************************************************************/
/* 字符分割的局部函数                                                    */
/************************************************************************/


/************************************************************************/
void cvThin( IplImage* src, IplImage* dst, int iterations=1) ;    
void autoCorrection(IplImage * src , int elementx , int elementy);
/************************************************************************/
//寻找字符边界,返回bound区域放在boundRect中，区域个数返回值给出
int FindCharacterBound(const IplImage* bwImage,CvRect** boundRect);


//连通域分析，包括合并、拆分、删除连通域，返回处理后的连通域个数,
//其中，grayImg为LP的灰度图，bwImg为二值化图,其中boundNum >= 7
int CCAnalyse(const IplImage* grayImg,IplImage* bwImg,
			  CvRect** boundRect,int color);

//对连通区域进行统计分析，找出其出现频率最高的宽度和高度。
int CCStatistics(CvRect** boundRect,int boundNum,int* maxWidth,int* maxHeight);

//研究图像中的矩形分布
struct borderParam* RectDistribution(CvRect** boundRect,int* boundNum,int rightWidth,int rightHeight);

//单行车牌，根据上下边界找出所有的字符
int FindRectsBasedBound(const IplImage* img_bw,CvRect** boundRect,int boundNum,int rightWidth,
						int rightHeight,struct borderParam bp);
//返回切割参数,包括字符宽度高度，以及上下边框的直线方程信息
struct borderParam* ReturnSegmentationParameter(const IplImage* img_bw,
	int* rightWidth,int* rightHeight);
//根据切割参数获得每个分割好的字符，返回空时分割失败
CvRect* ReturnCharRects(IplImage* img_bw,struct borderParam bp,int rightWidth,int rightHeight);

CvRect* ReturnCharRects2(IplImage* img_bw,struct borderParam bp,int rightWidth,int rightHeight);
//投影法定位
int test_FindRectsBasedBound(const IplImage* img_bw,CvRect** boundRect,int rightWidth, int rightHeight);
//合并相邻的矩形域,返回合并后的矩形个数
int MergeRect(CvRect** boundRect,int boundNum,int width,int height);

//字符分割，返回合并后的矩形个数
int SplitRect(CvRect** boundRect,int boundNum,int width,int height);
//调整当前的所有的矩形区域，或者调整各个矩形区域的大小,返回最后矩形的数量
int AdjustRectRegion(CvRect** boundRect,int boundNum,int width,int height,int imgHeight,int imgWidth);

//根据给定的区域取得每一个字符所在区域图片
IplImage** GetBoundImages(const IplImage* src,CvRect* boundRect,int boundNum,int color);

//获取二值化的字符
IplImage* GetThreshholdChar(IplImage* img_gray,CvRect rect,int color);

//删除多余的矩形区域，返回最终的矩形数量
int DeleteRectRegion(const IplImage* bwImg,CvRect** boundRect,int boundNum,int width,int height);

int DeleteRects(CvRect** boundRect,int boundNum,int width,int height);

//如果字符没有找全，需要添加字符
int AddRects(CvRect** boundRect,int boundNum,int rightWidth,int rightHeight,
											struct borderParam bp);
/*判断两个矩形是否相交，最终返回相交的面积，不相交返回值为0*/
int CrossRect(CvRect rt1,CvRect rt2);

//根据上下边界改变矩形
void ChangeRect(CvRect* pRect,float ah,float bh,float al,float bl,int width,int height);
/*从一个矩形数组中删除一个矩形*/
void DelARect(CvRect** boundRect,int* boundNum,int id);

/*向一个矩形数组中添加一个矩形域*/
void AddARect(CvRect** boundRect,int* boundNum,CvRect rect);

//返回矩形面积
int AreaRect(CvRect rt){return rt.height*rt.width;}
//矫正图像
IplImage* RotateImage(const IplImage* src,float ah,float bh,float al,float bl);
//去除二值化图中残留边框像素
void ClearResidualBorder(IplImage* img_bw);
//清除字符边界以外的区域
void ClearOutBorder(IplImage* img_bw,struct borderParam bp);

//字符分割，返回7个字符
IplImage** CharacterSegmentation(const IplImage* src,int* imgNum,int color) 
{
	double pixMeans = 0;   //像素平均值
	IplImage** img_chars = NULL;
	CvRect* boundRect = NULL;
	IplConvKernel* element0 = NULL;

	g_img_lpr = cvCreateImage(cvSize(RESIZE_IMAGE_WIDTH,RESIZE_IMAGE_HEIGHT),src->depth,src->nChannels);
	g_img_lpr_gray = cvCreateImage(cvSize(RESIZE_IMAGE_WIDTH,RESIZE_IMAGE_HEIGHT),IPL_DEPTH_8U,1);
	g_img_lpr_bw = cvCreateImage(cvSize(RESIZE_IMAGE_WIDTH,RESIZE_IMAGE_HEIGHT),IPL_DEPTH_8U,1);
	g_img_lpr_thin = cvCreateImage(cvSize(RESIZE_IMAGE_WIDTH,RESIZE_IMAGE_HEIGHT),IPL_DEPTH_8U,1);

	element0 = cvCreateStructuringElementEx(20,1,10,0,CV_SHAPE_RECT,NULL);

	cvResize(src,g_img_lpr);
	cvCvtColor(g_img_lpr,g_img_lpr_gray,CV_BGR2GRAY);


	cvSmooth(g_img_lpr_gray,g_img_lpr_gray,CV_MEDIAN);

	AdjustContrast(g_img_lpr_gray,g_img_lpr_gray,100);


	ThresholdPartIter(g_img_lpr_gray,g_img_lpr_bw,color,3);
	
	//防止二值化后字符与边框粘连
	ClearResidualBorder(g_img_lpr_bw);

	
	SmoothBinImg(g_img_lpr_bw,24,SMOOTH_WHITE_CC);
	
	
	//cvThin(g_img_lpr_thin,g_img_lpr_thin);
	//LPRotate(g_img_lpr_bw,g_img_lpr_bw);
	*imgNum  = CCAnalyse(g_img_lpr_gray,g_img_lpr_bw,&boundRect,color);

	//从二值化图中获取图片
	//if(*imgNum == CHARACTER_NUM)
		img_chars = GetBoundImages(g_img_lpr_gray,boundRect,*imgNum ,color);
	//else
		//*imgNum = 0;
	


	cvReleaseImage(&g_img_lpr);
	cvReleaseImage(&g_img_lpr_gray);
	cvReleaseImage(&g_img_lpr_bw);
	cvReleaseImage(&g_img_lpr_thin);

	cvReleaseStructuringElement(&element0);
	free(boundRect);
	return img_chars;

}


//连通域分析，包括合并、拆分、删除连通域，返回处理后的连通域个数,其中boundNum >= 7
int CCAnalyse(const IplImage* grayImg,IplImage* bwImg,
			  CvRect** boundRect,int color)
{
	//
	/*在这里要做的事情包括以下几个方面:
	1.找到出现频率最高的宽度和高度
	2.合并一些重叠或者很近的区域
	3.拆分可以拆分的区域
	4.删除太离谱的区域
	5.调整各个矩形的大小
	3.调整各个矩形大小*/
	int boundNum = 0;
	int rightHeight,rightWidth;		
	struct borderParam* bp = ReturnSegmentationParameter(bwImg,&rightWidth,&rightHeight);

	//判断是不是双行黄牌
	if(rightHeight < bwImg->height*0.3 && color == COLOR_YELLOW)
	{//双行黄牌
		AfxMessageBox("双行黄牌，暂不考虑");
	}
	else if(NULL != bp && abs(bp->bh) < 1.5 && abs(bp->bl)<1.5 
		&& abs(bp->bh-bp->bl) <1 && rightHeight >= rightWidth)
	{//单行牌
		//清除边界意外的像素点
		ClearOutBorder(bwImg,*bp);
		//rotate_img(bwImg,bwImg,-atan(min(bp->bl,bp->bh))*180/CV_PI);
		//RotateImage(bwImg,bp->ah,bp->bh,bp->al,bp->bl);
		//防止二值化后字符与边框粘连
		//ClearResidualBorder(bwImg);
		/*cvShowImage("songddd",bwImg);
		cvWaitKey(0);*/
		//cvSmooth(bwImg,bwImg,CV_MEDIAN);
		//从新定位字符
		//IplImage* img_tmp = cvCloneImage(grayImg);
		SmoothBinImg(bwImg,24,SMOOTH_WHITE_CC);
		boundNum = FindCharacterBound(bwImg,boundRect);
		
		//////一般情况下			
		boundNum = MergeRect(boundRect,boundNum,rightWidth,rightHeight);
	
		boundNum = SplitRect(boundRect,boundNum,rightWidth,rightHeight);
		
		//boundNum = AdjustRectRegion(boundRect,boundNum,rightWidth,rightHeight,bwImg->height,bwImg->width);
		boundNum = DeleteRects(boundRect,boundNum,rightWidth,rightHeight);
		/*IplImage* img_tmp = cvCloneImage(grayImg);
		for(int i=0;i<boundNum;i++)
			cvRectangle(img_tmp,cvPoint((*boundRect)[i].x,(*boundRect)[i].y),
			cvPoint((*boundRect)[i].x+(*boundRect)[i].width,
			(*boundRect)[i].y+(*boundRect)[i].height),
			CV_RGB(255,255,255),1,8,0);
		cvShowImage("img_lpr",img_tmp);*/
		
		for(int i=0;i<boundNum;i++)
		{
			if(abs((*boundRect)[i].height-rightHeight) > 5 || abs((*boundRect)[i].width-rightWidth) > 3)
				ChangeRect(*boundRect+i,bp->ah,bp->bh,bp->al,bp->bl,rightWidth,rightHeight);
		}
	}
	free(bp);
	return boundNum;
}
//返回切割参数,包括字符宽度高度，以及上下边框的直线方程信息
struct borderParam* ReturnSegmentationParameter(const IplImage* img_bw,
	int* rightWidth,int* rightHeight)
{
	CvRect* rects = NULL;
	int boundNum = 0;
	struct borderParam* bp = NULL;


	boundNum = FindCharacterBound(img_bw,&rects);
	//找到出现频率最高的宽度和高度
	if(!CCStatistics(&rects,boundNum,rightWidth,rightHeight))
	{
		boundNum = 0;
		return NULL;
	}
	bp = RectDistribution(&rects,&boundNum,*rightWidth,*rightHeight);
	free(rects);
	return bp;
}


//根据切割参数获得每个分割好的字符，返回空时分割失败
CvRect* ReturnCharRects(IplImage* img_bw,struct borderParam bp,
						int rightWidth,int rightHeight)
{
	int i,j,t;
	int nMeanPix = 0;  //水平投影的平均值
	int* hisHor = NULL;
	CvRect* boundRect = NULL;
	CvRect* rectTmp = NULL;
	int nRectTmpNum,nRightRectsNum = 0;
	IplImage* img_his = cvCreateImage(cvGetSize(g_img_lpr_bw),IPL_DEPTH_8U,1);
	cvZero(img_his);


	//首先清除边框以外的像素
	ClearOutBorder(img_bw,bp);
	nRectTmpNum = FindCharacterBound(img_bw,&rectTmp);
	if(nRectTmpNum == 0)
		return NULL;
	//水平投影
	hisHor = histogramVH(img_bw,1);



	//为最终的字符分配空间
	boundRect = (CvRect*)malloc(sizeof(CvRect)*CHARACTER_NUM);
	//首先不能合并的就合并了
	nRectTmpNum = MergeRect(&rectTmp,nRectTmpNum,rightWidth,rightHeight);
	//把能分割的分割
	nRectTmpNum = SplitRect(&rectTmp,nRectTmpNum,rightWidth,rightWidth);

	//求水平投影的平均值
	for(i=0;i<img_bw->width;i++)
		nMeanPix += hisHor[i];
	nMeanPix /= img_bw->width;

	////下面根据已经找到的以及水平投影图进行分析
	for(i=0;i<nRectTmpNum;i++)
	{
		if(nRightRectsNum == CHARACTER_NUM)
			break;
		if(abs(rectTmp[i].width-rightWidth) <=3 
			&& abs(rectTmp[i].height-rightHeight) <= 5)
		{
			//将其添加到标准矩形列表中
			AddARect(&boundRect,&nRightRectsNum,rectTmp[i]);
			for(j=rectTmp[i].x;j<rectTmp[i].x+rectTmp[i].width;j++)
				hisHor[j] = 0;
			//从临时列表中将其删除
			DelARect(&rectTmp,&nRectTmpNum,i);
			i--;
		}
		else if(abs(rectTmp[i].height-rightHeight) <= 4 
			&& rectTmp[i].x > 5 && rectTmp[i].x < img_bw->width-5
			&& rectTmp[i].width < rightWidth)
		{//将1这种加入，并且不是处在太靠边沿的1
			AddARect(&boundRect,&nRightRectsNum,
				cvRect(max(0,rectTmp[i].x-rightWidth/2),rectTmp[i].y,rightWidth,rightHeight));
			for(j=max(0,rectTmp[i].x-rightWidth/2);j<rectTmp[i].x+rightWidth/2;j++)
				hisHor[j] = 0;
			DelARect(&rectTmp,&nRectTmpNum,i);
			i--;
		}

		//for()
		//for(i=0;i<CHARACTER_NUM-nRightRectsNum;i++)
		//{
		//	for(j=1;j<img_bw->height;j++)
		//	{

		//	}
		//	
		//}
		///*else if(abs(rectTmp[i].width-rightWidth) < 4)
		/*{
		if(hisHor[max(0,rectTmp[i].x - 4)] < 0.5*nMeanPix 
		&& hisHor[min(0,rectTmp[i].x + rectTmp[i].width)] < 0.5*nMeanPix)
		{

		}
		}*/

	}
	for(int i=0;i<img_his->width;i++)
	{
		cvLine(img_his,cvPoint(i,img_his->height-1),cvPoint(i,img_his->height-1-hisHor[i]),CV_RGB(255,255,255),1);
	}
	free(rectTmp);
	rectTmp = NULL;
	nRectTmpNum = 0;
	{
		int cx,cy;
		for(i=1;i<img_his->height;i++)
		{
			if(nRightRectsNum >= CHARACTER_NUM)
				continue;
			for(j=0;j<img_his->width;j++)
				if(hisHor[j] <= i)
					hisHor[j] = 0;
			nRectTmpNum = FindCharacterBound(img_bw,&rectTmp);
			//首先不能合并的就合并了
			nRectTmpNum = MergeRect(&rectTmp,nRectTmpNum,rightWidth,rightHeight);
			//把能分割的分割
			nRectTmpNum = SplitRect(&rectTmp,nRectTmpNum,rightWidth,rightWidth);
			for(t=0;t<nRectTmpNum;t++)
			{
				if(abs(rectTmp[t].width - rightWidth) <=4)
				{
					cx = (int)(rectTmp[t].x+rightWidth/2);
					cy = (int)((bp.ah+bp.al)/2+(bp.bh+bp.bl)/2*cx);
					cx = (int)max(cx-0.5*rightWidth,0);
					cy = (int)max(cy-0.5*rightHeight,0);
					if(nRightRectsNum < CHARACTER_NUM)
						AddARect(&boundRect,&nRightRectsNum,cvRect(cx,cy,rightWidth,rightHeight));
				}

			}
			if(nRectTmpNum != 0)
			{
				free(rectTmp);
				rectTmp = NULL;
				nRectTmpNum = 0;
			}

		}
	}


	/*for(i=0;i<nRectTmpNum;i++)
	{	
	if(nRightRectsNum == CHARACTER_NUM)
	break;
	ChangeRect(rectTmp[i]+i,bp.ah,bp.bh,bp.al,bp.bl,rightWidth,rightHeight);

	}*/
	//cvShowImage("img_his_r",img_his);
	if(nRightRectsNum != CHARACTER_NUM)
	{
		free(boundRect);
		boundRect = NULL;
		nRightRectsNum = 0;
	}
	//free(rectTmp);
	free(hisHor);
	return boundRect;
}
//对连通区域进行统计分析，找出其出现频率最高的宽度和高度。
int CCStatistics(CvRect** boundRect,int boundNum,int* maxWidth,int* maxHeight)
{

	int i,j;
	int minHisId;
	double minVal,temp;
	int hisWidthNum = 0,
		hisHeightNum = 0;
	struct histogram
	{
		double value;
		int times;
	};

	struct histogram *hisWidth; 
	struct histogram *hisHeight;


	if(boundNum < 2)
		return 0;
	hisWidth = (struct histogram*)malloc(sizeof(struct histogram)*boundNum);
	hisHeight = (struct histogram*)malloc(sizeof(struct histogram)*boundNum);
	/*这样做:
	1.通过聚类分别找到宽度和高度出现频率最高的值或者一个x+-2的小范围*/

	for(i=0;i<boundNum;i++)
	{
		//求宽度直方图
		minHisId = 0;
		minVal = MAX_INI;
		for(j=0;j<hisWidthNum;j++)
		{
			temp = abs((*boundRect)[i].width-hisWidth[j].value);
			if(temp < minVal)
			{
				minHisId = j;
				minVal = temp;
			}
		}
		if(minVal > 2)
		{//添加新的直方图
			hisWidth[hisWidthNum].times = 1;
			hisWidth[hisWidthNum].value = (*boundRect)[i].width;
			hisWidthNum++;
		}
		else
		{//将该区域加入一个已经存在的直方图
			hisWidth[minHisId].value = (hisWidth[minHisId].value*hisWidth[minHisId].times
				+(*boundRect)[i].width)/(hisWidth[minHisId].times+1);

			hisWidth[minHisId].times++;
		}

		//求高度直方图
		minHisId = 0;
		minVal = MAX_INI;
		for(j=0;j<hisHeightNum;j++)
		{
			temp = abs((*boundRect)[i].height-hisHeight[j].value);
			if(temp < minVal)
			{
				minHisId = j;
				minVal = temp;
			}
		}
		if(minVal > 2)
		{//添加新的直方图
			hisHeight[hisHeightNum].times = 1;
			hisHeight[hisHeightNum].value = (*boundRect)[i].height;
			hisHeightNum++;
		}
		else
		{//将该区域加入一个已经存在的直方图
			hisHeight[minHisId].value = (hisHeight[minHisId].value*hisHeight[minHisId].times+
				(*boundRect)[i].height)/(hisHeight[minHisId].times+1);
			hisHeight[minHisId].times++;
		}
	}

	{//求出现频率最高的宽度和高度

		//求出出现频率最高的宽度范围
		*maxWidth = (int)ceil(hisWidth[0].value);;
		temp = hisWidth[0].times;
		for(i=1;i<hisWidthNum;i++)
		{
			if(temp < hisWidth[i].times && hisWidth[i].value > RESIZE_IMAGE_WIDTH * 0.09)
			{
				*maxWidth = (int)ceil(hisWidth[i].value);
				temp = hisWidth[i].times;
			}
		}
		if (temp < 2)
			return 0;
		//求出出现频率最高的高度范围
		*maxHeight = (int)ceil(hisHeight[0].value);
		temp = hisHeight[0].times;
		for(i=1;i<hisHeightNum;i++)
		{
			if(temp < hisHeight[i].times)
			{
				*maxHeight = (int)ceil(hisHeight[i].value);
				temp = hisHeight[i].times;
			}
		}
		if (temp < 2)
			return 0;
	}

	/*free(indexHeight);
	indexHeight = NULL;
	free(indexWidth);
	indexWidth = NULL;*/

	free(hisHeight);
	hisHeight = NULL;
	free(hisWidth);
	hisWidth = NULL;
	return 1;

}


//研究图像中的矩形分布
struct borderParam* RectDistribution(CvRect** boundRect,int* boundNum,
	int rightWidth,int rightHeight)
{
	/*具体步骤如下:
	1.统计得到所有标准矩形;
	2.通过最小二乘法确定上下边界;
	3.*/
	int i;
	int* rightRectId;
	int rightRectNum = 0;
	float aH,bH,aL,bL;
	struct borderParam* bp;
	rightRectId = (int*)malloc(sizeof(int)*(*boundNum));

	{//首先找出宽和高都比较中意的矩形
		int subW,subH;

		for(i=0;i<*boundNum;i++)
		{
			subW = abs((*boundRect)[i].width - rightWidth);
			subH = abs((*boundRect)[i].height - rightHeight);
			if(subW <= 3 && subH <= 5)
			{
				rightRectId[rightRectNum++] = i;
			}
		}
	}


	if(rightRectNum < 2)
		return NULL;

	bp = (struct borderParam*)malloc(sizeof(struct borderParam));

	//根据最小二乘法求出上下边界
	{
		int x,y;
		int x00=0,x01=0,x10=0,x11=0,
			y0=0,y1=0;
		//求上边界
		for(i=0;i<rightRectNum;i++)
		{
			x = (*boundRect)[rightRectId[i]].x+(*boundRect)[rightRectId[i]].width/2;
			y = (*boundRect)[rightRectId[i]].y;
			x00 += 1;
			x01 += x;
			x10 += x;
			x11 += x*x;
			y0 += y;
			y1 += x*y;
		}
		bH = (float)(x01*y0-y1*x00)/(x01*x10-x00*x11);
		aH = (float)(y0-bH*x10)/x00;
		//cvLine(img,cvPoint(0,aH),cvPoint(img->width,aH+img->width*bH),CV_RGB(255,255,255),1);

		//求下边界
		x00=0,x01=0,x10=0,x11=0,
			y0=0,y1=0;
		for(i=0;i<rightRectNum;i++)
		{
			x = (*boundRect)[rightRectId[i]].x+(*boundRect)[rightRectId[i]].width/2;
			y = (*boundRect)[rightRectId[i]].y+(*boundRect)[rightRectId[i]].height;
			x00 += 1;
			x01 += x;
			x10 += x;
			x11 += x*x;
			y0 += y;
			y1 += x*y;
		}
		bL = (float)(x01*y0-y1*x00)/(x01*x10-x00*x11);
		aL = (float)(y0-bL*x10)/x00;
		bp->ah = aH;
		bp->bh = bH;
		bp->al = aL;
		bp->bl = bL;
	}
	free(rightRectId);
	return bp;
}

//清除字符边界以外的区域
void ClearOutBorder(IplImage* img_bw,struct borderParam bp)
{
	int imgwidth = img_bw->width;
	int imgheight = img_bw->height;

	for(int i=0;i<imgwidth;i++)
	{
		for(int j=0;j<bp.ah+bp.bh*i-2 && j>0 && j< imgheight;j++)
			CV_IMAGE_ELEM(img_bw,uchar,j,i) = 0;
		for(int g=imgheight-1;g>bp.al+bp.bl*i+2 && g<imgheight && g>0;g--)
			CV_IMAGE_ELEM(img_bw,uchar,g,i) = 0;
	}
}

//投影法定位
//投影法定位
int test_FindRectsBasedBound(const IplImage* img_bw,CvRect** boundRect, int rightWidth,int rightHeight)
{

	CvRect *rightRects = NULL;
	int rightRectNum = 0;  //标准矩形个数
	int i=0,j=0,k=0;//循环变量 12
	int  letter[14]={0,20,23,43,55,75,78,98,101,121,124,127,147,167}; // 默认分割
	bool flag1=0;//    1     2     3     4       5           6    7  
	IplImage* img_b = cvCloneImage(img_bw);
	// 垂直投影
	int* num_h = histogramVH(img_bw,1);
	IplImage* img_his = cvCreateImage(cvGetSize(img_bw),IPL_DEPTH_8U,1);
	cvZero(img_his);

	for(int i=0;i<img_his->width;i++)
	{
		cvLine(img_his,cvPoint(i,img_his->height-1),cvPoint(i,img_his->height-1-num_h[i]),CV_RGB(255,255,255),1);
	}
	// cvShowImage("img_his",img_his);
	// 初步定位，定位点 第二个字符末端，
	int	max_count=0;
	int   flag=0;  
	rightRects = (CvRect *)malloc(sizeof(CvRect)*CHARACTER_NUM*2);
	for(i=30;i<RESIZE_IMAGE_WIDTH-1;i++)
	{
		if(num_h[i]<POINT_Y)
		{
			max_count++;
			if(max_count==11)
			{
				letter[3]=i-11;
				while( (num_h[i]<2)||(num_h[i-1]<6) ) i++;
				letter[4]=i-1; 
				break;
			}
		}
		else
		{
			max_count=0;
		}
	}

	// 精定位
	for(i=0;i<RESIZE_IMAGE_WIDTH-1;i++)//每一列的 
	{
		for(j=17;j<=24;j++)  // 17-24 /40   每一列的17 到 24 行相加 
		{
			num_h[i]+=CV_IMAGE_ELEM(img_bw,uchar,j,i)/45;
		}
	}

	for(j=letter[3];j>0;j--)//从第二个字符的末端开始 往前找第一个和第二个字符起始位置
	{
		if((num_h[j]<6)&&(num_h[j-1]<POINT_Y))//只要有两个列的 17到24 行的值小于 2，
		{                                           //即找到第二个字符的开始位置
			letter[2]=j;      // find letter[2]  第二个字符的开始位置
			letter[1]=(j>=23)?j-3:letter[1];   //第一个字符的结束位置
			letter[0]=(j>=23)?j-23:letter[0];  //第一个字符的起始位置
			break;       //找到就退出循环
		}
	}

	j=2;  flag=0;flag1=0;//两个标记
	for(i=letter[4];i<RESIZE_IMAGE_WIDTH-1;i++)  //从第三个字符的开始位置算起
	{
		if((num_h[i]>5)&&(num_h[i-1]>POINT_Y) && !flag )
		{
			flag=1;
			flag1=0;
			letter[2*j]=i-1; //这里 只记录字符的开始位置
			if(j==6)  //判断 最后一个字符的结束位置 是否越界 超出界限,如果没有,则letter[13]=letter[12]+20
			{
				letter[2*j+1]=((letter[2*j]+20)>RESIZE_IMAGE_WIDTH-1)?RESIZE_IMAGE_WIDTH-1:letter[2*j]+20;
				break;//退出 for循环
			}
		}
		else if((num_h[i]<5)&&(num_h[i-1]<POINT_Y) && !flag1 && flag)//如果是 空白区域  
		{
			flag=0;
			flag1=1;
			letter[2*j+1]=i-1;
			j++; //j自动加 1
		}                        //    1    
	}                                   



	CvRect ROI_rect1;
	ROI_rect1.x=(int)(0.5*(letter[1]+letter[0])-10);
	ROI_rect1.y=0;
	ROI_rect1.width=rightWidth;
	ROI_rect1.height=rightHeight;	
	AddARect(&rightRects,&rightRectNum,ROI_rect1);

	ROI_rect1.x=(int)(0.5*(letter[3]+letter[2])-10);
	ROI_rect1.y=0;
	ROI_rect1.width=rightWidth;
	ROI_rect1.height=rightHeight;	
	AddARect(&rightRects,&rightRectNum,ROI_rect1);

	ROI_rect1.x=(int)(0.5*(letter[5]+letter[4])-10);
	ROI_rect1.y=0;
	ROI_rect1.width=rightWidth;
	ROI_rect1.height=rightHeight;	
	AddARect(&rightRects,&rightRectNum,ROI_rect1);

	ROI_rect1.x=(int)(0.5*(letter[7]+letter[6])-10);
	ROI_rect1.y=0;
	ROI_rect1.width=rightWidth;
	ROI_rect1.height=rightHeight;	
	AddARect(&rightRects,&rightRectNum,ROI_rect1);

	ROI_rect1.x=(int)(0.5*(letter[9]+letter[8])-10);
	ROI_rect1.y=0;
	ROI_rect1.width=rightWidth;
	ROI_rect1.height=rightHeight;	
	AddARect(&rightRects,&rightRectNum,ROI_rect1);


	ROI_rect1.x=(int)(0.5*(letter[11]+letter[10])-10);
	ROI_rect1.y=0;
	ROI_rect1.width=rightWidth;
	ROI_rect1.height=rightHeight;	
	AddARect(&rightRects,&rightRectNum,ROI_rect1);

	ROI_rect1.x=(int)(0.5*(letter[13]+letter[12])-10);
	ROI_rect1.y=0;
	ROI_rect1.width=rightWidth;
	ROI_rect1.height=rightHeight;	
	AddARect(&rightRects,&rightRectNum,ROI_rect1);

	// 释放内存
	free(num_h); 
	num_h=NULL;


	/**boundRect = rightRects;
	for(int i=0;i<rightRectNum;i++)
	cvRectangle(img_b,cvPoint(rightRects[i].x,rightRects[i].y),
	cvPoint(rightRects[i].x+rightRects[i].width,
	rightRects[i].y+rightRects[i].height),
	CV_RGB(255,255,255),1,8,0);	

	cvShowImage("son",img_b);
	cvWaitKey(0);*/
	return rightRectNum;

}
//单行车牌，根据上下边界找出所有的字符
int FindRectsBasedBound(const IplImage* img_bw,CvRect** boundRect,int boundNum,
						int rightWidth,	int rightHeight,struct borderParam bp)
{

	int i;	
	//用该数组记录矩形的状态
	//-1: 表示要删除
	//0:表示要调节
	//1:表示标准矩形
	int* rightRectFlag;
	CvRect *rightRects = NULL;
	int rightRectNum = 0;  //标准矩形个数
	rightRectFlag = (int*)malloc(sizeof(int)*CHARACTER_NUM*2);
	rightRects = (CvRect *)malloc(sizeof(CvRect)*CHARACTER_NUM*2);
	IplImage* img_b = cvCloneImage(img_bw);

	{//首先找出宽和高都比较中意的矩形
		int subW,subH;
		for(i=0;i<boundNum;i++)
		{
			//rightRectFlag[i] = 0;
			subW = abs((*boundRect)[i].width - rightWidth);
			subH = abs((*boundRect)[i].height - rightHeight);
			if(subH <= 5 && subW <= 3)
			{
				rightRectFlag[rightRectNum] = i;
				rightRects[rightRectNum] = (*boundRect)[i];
				rightRectNum++;
			}
		}
	}

	//开始
	if(rightRectNum != CHARACTER_NUM)
	{
		int subX;  
		int subY;
		int dis = -1;  //相邻两个矩形之间的距离
		int secondId = -1;  //第二个字符的编号
		int cx,cy;
		int first = -1,second = -1,third = -1,last = -1;
		if(rightRectFlag[0] == 0)
			first = 0;
		if(rightRectFlag[rightRectNum-1] == boundNum-1)
			last = boundNum-1;
		//寻找第二个和第三个并填补
		for(i=0;i<rightRectNum-1;i++)
		{
			subX = rightRects[i+1].x - rightRects[i].x - rightRects[i].width;
			subY = abs(rightRects[i+1].y+rightRects[i+1].height/2 - rightRects[i].y-rightRects[i].height/2);
			if(subX < rightWidth*0.4 )
			{
				if(dis == -1)
					dis = subX;
				else
					dis = (dis+subX)/2;
			}
			else if(subX > rightWidth*0.4 && subX < rightWidth)
			{
				if(second != -1)
					continue;
				secondId = i;
				second = rightRectFlag[i];
				third = rightRectFlag[i+1];
				if(first == -1 && i!= 0)
					first = rightRectFlag[i-1];
				else if(first == secondId)
					first = -1;

			}
			else if(subX > rightWidth && subX < rightWidth*1.5)
			{
				cx = (int)(rightRects[i+1].x-subX/2);
				cy = (int)((bp.ah+bp.al)/2+(bp.bh+bp.bl)/2*cx);
				cx = (int)max(cx-0.5*rightWidth,0);
				cy = (int)max(cy-0.5*rightHeight,0);
				AddARect(&rightRects,&rightRectNum,cvRect(cx,cy,rightWidth,rightHeight));
				i++;
			}
		}

		if(rightRectNum > CHARACTER_NUM)
		{
			int temp = rightRectNum;
			for(i=secondId-2;i>=0;i--)
				DelARect(&rightRects,&rightRectNum,i);		
			for(i=secondId+6;i<temp;i++)
				DelARect(&rightRects,&rightRectNum,i);
		}

		if(rightRectNum < CHARACTER_NUM)
		{
			if(dis == -1)
				dis = 3; //默认两个矩形之间相差三个像素
			if(rightRectNum != CHARACTER_NUM)
			{
				//考虑第一个缺失或者第二个第三个或者最后一个
				if(second != -1 && first == -1)
				{//添加第一个

					cx = (int)max((*boundRect)[second].x-dis-rightWidth/2,0);
					cy = (int)((bp.ah+bp.al)/2+(bp.bh+bp.bl)/2*cx);
					cx = (int)max(cx-0.5*rightWidth,0);
					cy = (int)max(cy-0.5*rightHeight,0);
					AddARect(&rightRects,&rightRectNum,cvRect(cx,cy-cy,rightWidth,rightHeight));
					first = 0;
				}
				if(first != -1 && second == -1)
				{//添加第二个
					subX = rightRects[1].x -  rightRects[1].x -  rightRects[1].width;
					if(subX > rightWidth)
					{
						cx = (int)((*boundRect)[first].x+(*boundRect)[first].width+dis+rightWidth/2,0);
						cy = (int)((bp.ah+bp.al)/2+(bp.bh+bp.bl)/2*cx);
						cx = (int)max(cx-0.5*rightWidth,0);
						cy = (int)max(cy-0.5*rightHeight,0);
						AddARect(&rightRects,&rightRectNum,cvRect(cx,cy-cy,rightWidth,rightHeight));
						second = 1;
					}
				}
			}
			if(rightRectNum < CHARACTER_NUM)
			{
				for(i=rightRectNum-1;i>0;)
				{
					subX = rightRects[i].x -  rightRects[i-1].x - rightRects[i-1].width;
					if(subX > rightWidth*0.9)
					{
						cx = (int)(rightRects[i].x - dis - rightWidth/2);
						cy = (int)((bp.ah+bp.al)/2+(bp.bh+bp.bl)/2*cx);
						cx = (int)max(cx-0.5*rightWidth,0);
						cy = (int)max(cy-0.5*rightHeight,0);
						AddARect(&rightRects,&rightRectNum,cvRect(cx,cy,rightWidth,rightHeight));
						if(rightRectNum == CHARACTER_NUM)
							break;
						else
							continue;
					}
					else if(subX <= 2 )
					{
						if(rightRects[i-1].x > 3-subX)
							rightRects[i-1].x -= (3-subX);
						else
							rightRects[i-1].x = 0;
					}
					i--;

				}
			}
			if(rightRectNum < CHARACTER_NUM)
			{
				if(first == -1 )
				{
					if(second == -1)
						second = 1;
					ChangeRect(*boundRect+second-1,bp.ah,bp.bh,bp.al,bp.bl,rightWidth,rightHeight);
					if(CrossRect((*boundRect)[second-1],rightRects[0]) < rightHeight*rightWidth*0.1)
						AddARect(&rightRects,&rightRectNum,(*boundRect)[second-1]);
				}
				if(last == -1 && rightRectNum < CHARACTER_NUM)
				{
					ChangeRect(*boundRect+boundNum-1,bp.ah,bp.bh,bp.al,bp.bl,rightWidth,rightHeight);
					if(CrossRect((*boundRect)[boundNum-1],rightRects[rightRectNum-1]) < rightHeight*rightWidth*0.1)
						AddARect(&rightRects,&rightRectNum,(*boundRect)[boundNum-1]);
				}
			}
		}


	}

	if(rightRectNum < CHARACTER_NUM)
	{
		int subx1,subx2;
		subx1 = rightRects[0].x;
		subx2 = img_bw->width - rightRects[rightRectNum-1].x-rightRects[rightRectNum-1].width;
		if(subx2 > subx1)
		{
			AddARect(&rightRects,&rightRectNum,cvRect(rightRects[rightRectNum-1].x, rightRects[rightRectNum-1].y,subx2,rightHeight));
		}
		if(rightRectNum < CHARACTER_NUM)
		{
			AddARect(&rightRects,&rightRectNum,cvRect(0, rightRects[rightRectNum-1].y,subx1,rightHeight));
		}
	}
	//第一个汉字左边部首经常会残缺，所以在此宽度适当放大
	{
		int x;
		x = rightRects[0].x + rightRects[0].width;
		if(rightRects[0].x>3)
			rightRects[0].x -= 3;
		else
			rightRects[0].x = 0;
		rightRects[0].width = x - rightRects[0].x;

	}

	for(i=0;i<rightRectNum;i++)
	{
		if(abs(rightRects[i].height-rightHeight) > 5 || abs(rightRects[i].width-rightWidth) > 3)
			ChangeRect(rightRects+i,bp.ah,bp.bh,bp.al,bp.bl,rightWidth,rightHeight);
	}

	/*for(int i=0;i<rightRectNum;i++)
	cvRectangle(img_b,cvPoint(rightRects[i].x,rightRects[i].y),
	cvPoint(rightRects[i].x+rightRects[i].width,
	rightRects[i].y+rightRects[i].height),
	CV_RGB(255,255,255),1,8,0);	

	cvShowImage("son",img_b);
	cvWaitKey(0);*/
	free(*boundRect);
	free(rightRectFlag);
	rightRectFlag = NULL;
	*boundRect = rightRects;
	return rightRectNum;
}
void ClearResidualBorder(IplImage* img_bw)
{
	int i,j,t;
	int left,right;
	int height = img_bw->height;
	int width = img_bw->width;
	int* hisHor = histogramVH(img_bw,1);

	{
		/*step1:去除左右边框;
		  step2:将上下边框处的两行像素去掉*/
		for(i = 0;i<img_bw->width/2;i++)
		{
			if(hisHor[i] > img_bw->height*0.92)
			{
				for(j=0;j<img_bw->height;j++)
					CV_IMAGE_ELEM(img_bw,uchar,j,i)=0;

			}
			else
				break;
		}
		for(i = img_bw->width-1;i>img_bw->width/2;i--)
		{
			if(hisHor[i] > img_bw->height*0.92)
			{
				for(j=0;j<img_bw->height;j++)
					CV_IMAGE_ELEM(img_bw,uchar,j,i)=0;

			}
			else
				break;
		}


		for(i=0;i<width;i++)
		{
			CV_IMAGE_ELEM(img_bw,uchar,0,i) = 0;
			CV_IMAGE_ELEM(img_bw,uchar,1,i) = 0;

			CV_IMAGE_ELEM(img_bw,uchar,height-1,i) = 0;
			CV_IMAGE_ELEM(img_bw,uchar,height-2,i) = 0;
		}
		for(i=0;i<height;i++)
		{
			CV_IMAGE_ELEM(img_bw,uchar,i,0) = 0;
			CV_IMAGE_ELEM(img_bw,uchar,i,1) = 0;

			CV_IMAGE_ELEM(img_bw,uchar,i,width-1) = 0;
			CV_IMAGE_ELEM(img_bw,uchar,i,width-2) = 0;
		}
	}
	
	//水平滤波
	for(i=0;i<height;i++)
	{
		left = -1;
		for(j=0;j<width;j++)
		{
			if(CV_IMAGE_ELEM(img_bw,uchar,i,j) == 255)
			{
				if(left == -1)
				{
					left = j;
					right = j;
				}
				else
				{
					right++;
				}
			}
			else
			{
				if(left != -1)
				{
					if(right - left > width*0.2)
					{
						for(t=left;t<right;t++)
						{
							CV_IMAGE_ELEM(img_bw,uchar,i,t) = 0;
						}
					}
					left = -1;
				}
			}
		}
	}
	//竖直滤波
	for(i=0;i<width;i++)
	{
		left = -1;
		for(j=1;j<height-1;j++)
		{
			if(CV_IMAGE_ELEM(img_bw,uchar,j,i) == 255
				&& CV_IMAGE_ELEM(img_bw,uchar,j-1,i) == 0
				&& CV_IMAGE_ELEM(img_bw,uchar,j+1,i) == 0)
			{
				CV_IMAGE_ELEM(img_bw,uchar,j,i) = 0;
			}
		}
	}

	
	/*for(i=1;i<height-1;i++)
	{
	for(j=0;j<width;j++)
	{
	if(CV_IMAGE_ELEM(img_bw,uchar,i,j) == 255)
	{
	if(CV_IMAGE_ELEM(img_bw,uchar,i-1,j) == 0 &&
	CV_IMAGE_ELEM(img_bw,uchar,i+1,j) == 0)
	CV_IMAGE_ELEM(img_bw,uchar,i,j) = 0;
	}
	}
	}*/
	free(hisHor);
}

//根据上下边界改变矩形
void ChangeRect(CvRect* pRect,float ah,float bh,float al,float bl,
				int width,int height)
{
	int hY,lY;
	int centerX = pRect->x+pRect->width/2;
	hY = (int)(ah+bh*centerX);
	lY = (int)(al+bl*centerX);

	pRect->x = max(0,centerX-width/2);
	pRect->y = max(0,hY);
	pRect->width =  width;
	pRect->height = height;


}
//合并相邻的矩形域,返回合并后的矩形个数
int MergeRect(CvRect** boundRect,int boundNum,int width,int height)
{
	int i;
	int tempRightBound = 0;  //记录遍历时最Right边界
	int mergetimes = 0;  //记录合并的次数

	if(width != -1 && height != -1)
	{

		for(i=0;i<boundNum-1;i++)
		{
			//上下合并
			if(abs(((*boundRect)[i].height+(*boundRect)[i+1].height)<1.2*height)
				&& (abs((*boundRect)[i].y+(*boundRect)[i].height -(*boundRect)[i+1].y) < 5)
				|| (abs((*boundRect)[i+1].y+(*boundRect)[i+1].height -(*boundRect)[i].y) < 5))
			{

				(*boundRect)[i].y = min((*boundRect)[i].y,(*boundRect)[i+1].y);
				(*boundRect)[i].width = width;
				(*boundRect)[i].height = height;
				tempRightBound = max((*boundRect)[i+1].x+width,(*boundRect)[i].x+width);
				DelARect(boundRect,&boundNum,i+1);
				i--;
				continue;
			}
			//左右合并
			if((*boundRect)[i].width + (*boundRect)[i+1].width <= width*1.2 && 
				abs((*boundRect)[i].x+(*boundRect)[i].width/2-(*boundRect)[i+1].x)<5)
			{

				(*boundRect)[i].y = min((*boundRect)[i].y,(*boundRect)[i+1].y);
				(*boundRect)[i].width = (*boundRect)[i+1].x+(*boundRect)[i+1].width - (*boundRect)[i].x;
				(*boundRect)[i].height = max((*boundRect)[i+1].y+(*boundRect)[i+1].height,
					(*boundRect)[i].y+(*boundRect)[i].height)-(*boundRect)[i].y;

				tempRightBound = (*boundRect)[i+1].x+width;

				DelARect(boundRect,&boundNum,i+1);
				i--;
				continue;
			}
		}

	}

	return boundNum;
}


//字符分割，返回合并后的矩形个数,只考虑两个合并或者三合并的情况
int SplitRect(CvRect** boundRect,int boundNum,int width,int height)
{
	int i;
	CvRect rect;
	//int bn = boundNum;
	for(i=0;i<boundNum;i++)
	{
		if(((*boundRect)[i].width>2*width && (*boundRect)[i].width-2*width<0.5*width) && abs((*boundRect)[i].height-height)<=4)
		{//两个字符被同一个矩形包围，将其拆分成两个
			rect = (*boundRect)[i];
			DelARect(boundRect,&boundNum,i);
			AddARect(boundRect,&boundNum,cvRect(rect.x,rect.y,width,height));
			AddARect(boundRect,&boundNum,cvRect(rect.x+rect.width-width,rect.y,width,height));
			continue;
		}
		if(abs((*boundRect)[i].width>3*width && (*boundRect)[i].width-3*width < width)&& abs((*boundRect)[i].height-height)<=4)
		{//三个字符被同一个矩形包围，将其拆分成三个
			rect = (*boundRect)[i];
			DelARect(boundRect,&boundNum,i);
			AddARect(boundRect,&boundNum,cvRect(rect.x,rect.y,width,height));
			AddARect(boundRect,&boundNum,cvRect(rect.x+width+2,rect.y,width,height));
			AddARect(boundRect,&boundNum,cvRect(rect.x+width*2+4,rect.y,width,height));
		}

	}

	return boundNum;
}
//调整当前的所有的矩形区域，或者调整各个矩形区域的大小,返回最后矩形的数量
int AdjustRectRegion(CvRect** boundRect,int boundNum,int width,int height,int imgHeight,int imgWidth)
{
	int i,t;
	//首先找两个相对较标准的矩形域
	//这里假设肯定可以找到
	int firstId = -1,secondId;
	for(i=0;i<boundNum;i++)
	{
		if(abs((*boundRect)[i].height-height)<3
			&& abs((*boundRect)[i].width-width)<3)
		{
			if(firstId == -1)
				firstId = i;
			else
			{
				secondId = i;
				break;
			}
		}
	}

	for(i=0;i<boundNum;i++)
	{
		if(abs((*boundRect)[i].height - height)<2 && 
			abs((*boundRect)[i].width - width)<2)
			continue;
		if(abs((*boundRect)[i].width - width)>=2)
		{//调节宽度
			(*boundRect)[i].x-=(int)ceil((double)(width-(*boundRect)[i].width)/2);
			if((*boundRect)[i].x < 0)
				(*boundRect)[i].x = 0;
			if((*boundRect)[i].x+width >= imgWidth )
				(*boundRect)[i].width = imgWidth - (*boundRect)[i].x-1;
			else
				(*boundRect)[i].width = width;

		}
		if(abs((*boundRect)[i].height - height)>=2)
		{//调节高度
			//所有的矩形区域左上角点在同一条直线上
			t = (*boundRect)[firstId].x-(*boundRect)[secondId].x;
			if(t == 0)
				(*boundRect)[i].y = (*boundRect)[secondId].y;
			else
				(*boundRect)[i].y = ((*boundRect)[i].x-(*boundRect)[secondId].x)
				*(((*boundRect)[firstId].y-(*boundRect)[secondId].y)/t)
				+(*boundRect)[secondId].y;
			//防止超越边界
			if((*boundRect)[i].y < 0)
				(*boundRect)[i].y = 0;

			if((*boundRect)[i].y+height >= imgHeight )
				(*boundRect)[i].height = imgHeight - (*boundRect)[i].y-1;
			else
				(*boundRect)[i].height = height;
		}

	}


	return boundNum;

}

//删除多余的矩形区域，返回最终的矩形数量
int DeleteRectRegion(const IplImage* bwImg,CvRect** boundRect,int boundNum,int width,int height)
{
	/*删除一下区域:
	1.某个矩形在其他矩形之中则将其删除;
	2.如果最左端和最右端和标准差别较大则删除
	3.删除宽和高不足width和height一半的矩形;*/
	int i,j;
	int t = boundNum;
	double pm;  //像素平均值
	double area;
	IplImage* img_bw = NULL;
	IplImage* img_Roi = NULL;
	//如果当前矩形的数量不多于字符数，则退出
	if(boundNum == CHARACTER_NUM)
		return boundNum;

	/*img_bw = cvCreateImage(cvGetSize(bwImg),bwImg->depth,1);
	cvCopy(bwImg,img_bw);*/
	img_bw = cvCloneImage(bwImg);
	for(i=0;i<boundNum;i++)
	{
		//删除宽度太大的矩形
		if((*boundRect)[i].width > bwImg->width*0.5)
		{
			DelARect(boundRect,&boundNum,i);
			continue;
		}
		cvSetImageROI(img_bw,(*boundRect)[i]);
		pm = cvMean(img_bw);
		cvResetImageROI(img_bw);
		pm /= 255;
		if((pm > 0.80 || pm < 0.12) && 
			(abs((*boundRect)[i].height-height)>5 || 
			abs((*boundRect)[i].width-width)>5))
		{
			DelARect(boundRect,&boundNum,i);
			continue;
		}
		if((*boundRect)[i].height < height*0.3 && (*boundRect)[i].width < width*0.3)
		{
			DelARect(boundRect,&boundNum,i);
			continue;
		}
		if((*boundRect)[i].width > 2*(*boundRect)[i].height
			&& (abs((*boundRect)[i].height)<5 || abs((*boundRect)[i].height-bwImg->height/2)<5))
		{
			DelARect(boundRect,&boundNum,i);
		}

	}

	if(abs((*boundRect)[0].height-height)>height/3 || abs((*boundRect)[0].width-width)>width/3)
	{//看最左边矩形是否和标准差别是否较大
		DelARect(boundRect,&boundNum,0);
	}

	if(abs((*boundRect)[boundNum-1].height-height)>height/3 || abs((*boundRect)[boundNum-1].width-width)>width/3)
	{//看最右边矩形和标准差别是否较大
		DelARect(boundRect,&boundNum,boundNum-1);
	}

	//1.
	for(i=0;i<boundNum;i++)
	{
		for(j = i+1;j<boundNum;j++)
		{
			area = CrossRect((*boundRect)[i],(*boundRect)[j]);
			if(area == 0)
				continue;
			else if(area == (*boundRect)[i].height*(*boundRect)[i].width)
			{//矩形i在矩形j中
				DelARect(boundRect,&boundNum,i);
			}
			else if(area == (*boundRect)[j].height*(*boundRect)[j].width)
			{//矩形j在i中
				DelARect(boundRect,&boundNum,j);
			}
			else if(abs((*boundRect)[i].height-height)>height/4 && abs((*boundRect)[i].width-width)>width/4)
			{//矩形i和标准矩形差别有点大
				DelARect(boundRect,&boundNum,i);
			}

			else if(abs((*boundRect)[j].height-height)>width/4 && abs((*boundRect)[j].width-width)>width/4)
			{//矩形j和标准矩形差别有点大
				DelARect(boundRect,&boundNum,j);
			}

		}
	}

	cvReleaseImage(&img_bw);
	cvReleaseImage(&img_Roi);
	return boundNum;
}

int DeleteRects(CvRect** boundRect,int boundNum,int width,int height)
{
	int i,j,area,nDis,nMaxDis,nMaxId;
	float ration1,ration2;
	for(i=0;i<boundNum;i++)
	{//将重叠很多的删除
		for(j = i+1;j<boundNum;j++)
		{
			area = CrossRect((*boundRect)[i],(*boundRect)[j]);
			if(area == 0)
				continue;
			ration1 = (float)area/(*boundRect)[i].height/(*boundRect)[i].width;
			ration2 = (float)area/(*boundRect)[j].height/(*boundRect)[j].width;
			if(ration1 >= ration2 && ration1 > 0.5)
			{
				DelARect(boundRect,&boundNum,i);
				continue;
			}
			if(ration1 < ration2 && ration2 > 0.5)
			{
				DelARect(boundRect,&boundNum,j);
			}
		}
	}
	if(boundNum > CHARACTER_NUM)
	{
		if(abs((*boundRect)[0].height-height)>height/3 || abs((*boundRect)[0].width-width)>width/3)
		{//看最左边矩形是否和标准差别是否较大
			DelARect(boundRect,&boundNum,0);
		}

		if(abs((*boundRect)[boundNum-1].height-height)>height/3 || abs((*boundRect)[boundNum-1].width-width)>width/3)
		{//看最右边矩形和标准差别是否较大
			DelARect(boundRect,&boundNum,boundNum-1);
		}
	}
	while(boundNum > CHARACTER_NUM)
	{
		nMaxDis = 0;  //任意一个较小的值
		nMaxId = 0;
		for(i=0;i<boundNum;i++)
		{
			nDis = abs((*boundRect)[i].width - width)+abs((*boundRect)[i].height - height);
			if(nDis > nMaxDis)
			{
				nMaxId = nDis;
				nMaxId = i;
			}
		}

		DelARect(boundRect,&boundNum,nMaxId);
	}
	return boundNum;
}


//如果字符没有找全，需要添加字符
int AddRects(CvRect** boundRect,int boundNum,int rightWidth,int rightHeight,
											struct borderParam bp)
{
	return 0;
}
int FindCharacterBound(const IplImage* bwImage,CvRect** boundRect)
{

	int mode = CV_RETR_EXTERNAL;	//轮廓边缘提取时的参数
	CvSeq * contour = 0;		//轮廓边缘提取时的参数
	CvRect bndRect = cvRect(0,0,0,0);//用cvBoundingRect画出外接矩形时需要的矩形
	CvMemStorage * storage; //轮廓边缘提取时的参数
	IplImage* img_bw = NULL;
	int ret,i=0,imgW,imgH,j=0;
	double area,ratio;
	if(bwImage->nChannels != 1) return NULL;

	storage = cvCreateMemStorage(0);
	img_bw = cvCloneImage(bwImage);



	//寻找LP边缘
	ret = cvFindContours(img_bw, storage, &contour, sizeof(CvContour),
		mode, CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0));
	if(ret == 0)
	{
		boundRect = NULL;
		return 0;
	}

	
	if(*boundRect != NULL)
		free(*boundRect);
	//分配超过ret个矩形，防止会分割矩形
	if(ret <= CHARACTER_NUM/3)
		return 0;
	else if(ret <= CHARACTER_NUM/2)
		*boundRect = (CvRect*)malloc(sizeof(CvRect)*ret*3);
	else 
		*boundRect = (CvRect*)malloc(sizeof(CvRect)*ret*2);
	imgW = bwImage->width;
	imgH = bwImage->height;
	for(;contour!=0;contour=contour->h_next)
	{	
		//得到一个可以将移动轮廓包围的矩形区域
		/*bndRect = cvBoundingRect(contour,0);
		cvRectangle(src,cvPoint(bndRect.x,bndRect.y),
		cvPoint(bndRect.x+bndRect.width,
		bndRect.y+bndRect.height),
		CV_RGB(0,0,255),1,8,0);*/

		bndRect	= cvBoundingRect(contour,0);

		area = fabs(cvContourArea(contour));
		if(area < 40)
		{
			ret--;
			continue;
		}
		ratio = area/(bndRect.width*bndRect.height);
		if(ratio < 0.1)
		{
			ret--;
			continue;
		}
		if(ratio > 0.9)
		{
			ret--;
			continue;
		}
		if(bndRect.width > imgW*0.5)
		{
			ret--;
			continue;
		}
		//按左边界排序
		for(j=i-1;j>=0;j--)
		{
			if(bndRect.x < (*boundRect)[j].x)
				(*boundRect)[j+1] = (*boundRect)[j];
			else
				break;
		}

		//对每个矩形上下进行扩展2像素
		bndRect.y=bndRect.y>=2?bndRect.y-2:0;
		bndRect.height+=4;
		(*boundRect)[j+1] = bndRect;
		i++;

	}
	cvReleaseImage(&img_bw);
	cvReleaseMemStorage(&storage);
	return ret;
}

//根据给定的区域取得每一个字符所在区域图片
IplImage** GetBoundImages(const IplImage* src,CvRect* boundRect,int boundNum,int color)
{
	int i = 0;
	IplImage** charImg = NULL;
	IplImage* img = NULL;
	img = cvCloneImage(src);
	charImg = (IplImage**)malloc(sizeof(IplImage*)*boundNum);
	for(int i=0;i<g_img_lpr_thin->width;i++)
		for(int j=0;j<g_img_lpr_thin->height;j++)
		{
			if(CV_IMAGE_ELEM(g_img_lpr_thin,uchar,j,i) == 1)
				CV_IMAGE_ELEM(g_img_lpr_thin,uchar,j,i) = 255;
			else
				CV_IMAGE_ELEM(g_img_lpr_thin,uchar,j,i) = 0;
		}
		for(;i<boundNum;i++)
		{	
			if(boundRect[i].y+boundRect[i].height > src->height-1)
				boundRect[i].height = src->height-1-boundRect[i].y;
			if(boundRect[i].x+boundRect[i].width > src->width-1)
				boundRect[i].width = src->width-1-boundRect[i].x;

			//如果此处不想细化，可以将g_img_lpr_thin改成g_img_lpr_bw
			//charImg[i] = GetRectImage(g_img_lpr_thin,boundRect[i]);
			//charImg[i] = /*GetThreshholdChar(img,boundRect[i],color)*/GetRectImage(g_img_lpr_bw,boundRect[i]);
			charImg[i] = GetThreshholdChar(g_img_lpr_gray,boundRect[i],color);
		
		}

		cvReleaseImage(&img);
		return charImg;
}

IplImage* GetThreshholdChar(IplImage* img_gray,CvRect rect,int color)
{
	IplImage* img = NULL;
	IplImage* img_result = NULL;
	img = GetRectImage(img_gray,rect);
	img_result = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
	double me = cvMean(img);
	if(color == COLOR_BLUE)
		cvThreshold(img,img_result,me+10,255,CV_THRESH_BINARY);
	else
		cvThreshold(img,img_result,me-10,255,CV_THRESH_BINARY_INV);
	//先锐化一下啦
	/*UnsharpMask(img,img,100,3);
	cvShowImage("img",img);
	cvWaitKey(0);*/
	//cvThreshold(img,img_result,255,255,CV_THRESH_BINARY | CV_THRESH_OTSU);
	/*if(color == COLOR_BLUE)
		cvAdaptiveThreshold(img,img_result,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY,rect.height*2-1);
	else
		cvAdaptiveThreshold(img,img_result,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY_INV,rect.height*2-1);*/
	cvReleaseImage(&img);
	return img_result;
}

/*判断矩形rt1和rt2是否相交，返回相交的面积，面积为零时表示不相交*/
int CrossRect(CvRect rt1,CvRect rt2)
{

	int minx,miny;
	int maxx,maxy;
	minx = max(rt1.x,rt2.x);  
	miny = max(rt1.y,rt2.y);  
	maxx = min(rt1.x+rt1.width,rt2.x+rt2.width);  
	maxy = min(rt1.y+rt1.height,rt2.y+rt2.height); 

	if ( minx>maxx ) return 0;
	if ( miny>maxy ) return 0;

	return (maxx-minx)*(maxy-miny);

}
/*从一个矩形数组中删除一个矩形*/
void DelARect(CvRect** boundRect,int* boundNum,int id)
{
	int i;
	for(i=id;i<*boundNum-1;i++)
		(*boundRect)[i] = (*boundRect)[i+1];

	(*boundNum)--;
}
/*向一个矩形数组中添加一个矩形域*/
void AddARect(CvRect** boundRect,int* boundNum,CvRect rect)
{
	int i;
	for(i=*boundNum-1;i>=0;i--)
	{
		if(rect.x < (*boundRect)[i].x)
			(*boundRect)[i+1] = (*boundRect)[i];
		else
		{
			(*boundRect)[i+1] = rect;
			(*boundNum)++;
			return;
		}
	}
	(*boundRect)[0] = rect;
	(*boundNum)++;
}
//矫正图像
IplImage* RotateImage(const IplImage* src,float ah,float bh,float al,float bl)
{
	int width = src->width;
	int height = src->height;
	IplImage* img,*img_rect,*dst;
	CvRect rect;
	img = cvCloneImage(src);
	rect.x = 0;
	rect.y = (int)max(min(ah+bh*width,ah),0);
	rect.width = width;
	rect.height = (int)min(height-rect.y,max(al,al+bl*(width-1))-rect.y);
	img_rect = GetRectImage(img,rect);

	dst = ImageRotate(img_rect,atan(min(bl,bh))*180/CV_PI);
	cvReleaseImage(&img);
	cvReleaseImage(&img_rect);
	return dst;
}
///***///////////////////////////////////////////////////////////////////////
void cvThin( IplImage* src, IplImage* dst, int iterations)
{
	CvSize size = cvGetSize(src);
	cvCopy(src, dst);
	int n = 0,i = 0,j = 0;
	for(n=0; n<iterations; n++)
	{
		IplImage* t_image = cvCloneImage(dst);
		for(i=0; i<size.height; i++)
		{
			for(j=0; j<size.width; j++)
			{
				if(CV_IMAGE_ELEM(t_image,byte,i,j)==1)
				{
					int ap=0;
					int p2 = (i==0)?0:CV_IMAGE_ELEM(t_image,byte, i-1, j);
					int p3 = (i==0 || j==size.width-1)?0:CV_IMAGE_ELEM(t_image,byte, i-1, j+1);
					if (p2==0 && p3==1)
					{
						ap++;
					}
					int p4 = (j==size.width-1)?0:CV_IMAGE_ELEM(t_image,byte,i,j+1);
					if(p3==0 && p4==1)
					{
						ap++;
					}
					int p5 = (i==size.height-1 || j==size.width-1)?0:CV_IMAGE_ELEM(t_image,byte,i+1,j+1);
					if(p4==0 && p5==1)
					{
						ap++;
					}
					int p6 = (i==size.height-1)?0:CV_IMAGE_ELEM(t_image,byte,i+1,j);
					if(p5==0 && p6==1)
					{
						ap++;
					}
					int p7 = (i==size.height-1 || j==0)?0:CV_IMAGE_ELEM(t_image,byte,i+1,j-1);
					if(p6==0 && p7==1)
					{
						ap++;
					}
					int p8 = (j==0)?0:CV_IMAGE_ELEM(t_image,byte,i,j-1);
					if(p7==0 && p8==1)
					{
						ap++;
					}
					int p9 = (i==0 || j==0)?0:CV_IMAGE_ELEM(t_image,byte,i-1,j-1);
					if(p8==0 && p9==1)
					{
						ap++;
					}
					if(p9==0 && p2==1)
					{
						ap++;
					}
					if((p2+p3+p4+p5+p6+p7+p8+p9)>1 && (p2+p3+p4+p5+p6+p7+p8+p9)<7)
					{
						if(ap==1)
						{
							if(!(p2 && p4 && p6))
							{
								if(!(p4 && p6 && p8)) 
								{
									CV_IMAGE_ELEM(dst,byte,i,j)=0;
								}
							}
						}
					}
				}
			}
		}
		cvReleaseImage(&t_image);
		t_image = cvCloneImage(dst);
		for(i=0; i<size.height; i++)
		{
			for(int j=0; j<size.width; j++)
			{
				if(CV_IMAGE_ELEM(t_image,byte,i,j)==1)
				{
					int ap=0;
					int p2 = (i==0)?0:CV_IMAGE_ELEM(t_image,byte, i-1, j);
					int p3 = (i==0 || j==size.width-1)?0:CV_IMAGE_ELEM(t_image,byte, i-1, j+1);
					if (p2==0 && p3==1)
					{
						ap++;
					}
					int p4 = (j==size.width-1)?0:CV_IMAGE_ELEM(t_image,byte,i,j+1);
					if(p3==0 && p4==1)
					{
						ap++;
					}
					int p5 = (i==size.height-1 || j==size.width-1)?0:CV_IMAGE_ELEM(t_image,byte,i+1,j+1);
					if(p4==0 && p5==1)
					{
						ap++;
					}
					int p6 = (i==size.height-1)?0:CV_IMAGE_ELEM(t_image,byte,i+1,j);
					if(p5==0 && p6==1)
					{
						ap++;
					}
					int p7 = (i==size.height-1 || j==0)?0:CV_IMAGE_ELEM(t_image,byte,i+1,j-1);
					if(p6==0 && p7==1)
					{
						ap++;
					}
					int p8 = (j==0)?0:CV_IMAGE_ELEM(t_image,byte,i,j-1);
					if(p7==0 && p8==1)
					{
						ap++;
					}
					int p9 = (i==0 || j==0)?0:CV_IMAGE_ELEM(t_image,byte,i-1,j-1);
					if(p8==0 && p9==1)
					{
						ap++;
					}
					if(p9==0 && p2==1)
					{
						ap++;
					}
					if((p2+p3+p4+p5+p6+p7+p8+p9)>1 && (p2+p3+p4+p5+p6+p7+p8+p9)<7)
					{
						if(ap==1)
						{
							if(p2*p4*p8==0)
							{
								if(p2*p6*p8==0)
								{
									CV_IMAGE_ELEM(dst, byte,i,j)=0;
								}
							}
						}
					} 
				}
			}
		} 
		cvReleaseImage(&t_image);
	}
}















