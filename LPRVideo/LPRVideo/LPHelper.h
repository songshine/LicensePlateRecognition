#ifndef _LPHELPER_H_
#define _LPHELPER_H_

//平滑图像的参数
#define SMOOTH_WHITE_CC    1    //清除白色连通域
#define SMOOTH_BLACK_CC    2	//清除黑色连通域
#define SMOOTH_ALL_CC    3	//清除所有连通域

//颜色
#define COLOR_BLUE 0
#define COLOR_YELLOW 1
#define COLOR_WHITE 2
#define COLOR_BLACK 3
#define COLOR_OTHERS 4

//默认字符数量
#define CHARACTER_NUM 7		

#define max(a,b)  ((a)>(b)?(a):(b))
#define min(a,b)  ((a)<(b)?(a):(b))
#define LPRFree(para) {free(para);para = NULL;}
//求一阶差分图
void FirstDiff(const IplImage* src1,IplImage* dst);
/*调整图像的对比度，contrast的范围[-255,255]*/
void AdjustContrast(const IplImage* src, IplImage* dst, int contrast);

//根据直方图增强图像
void ImageStretchByHistogram(IplImage *src,IplImage *dst);
/*去二值化图中小的连通域,binImg为二值化图，
Thres为像素值，小于或等于该像素值的连通域被去除,
selection选择清除哪种连通域*/
void SmoothBinImg(IplImage* binImg,int Thres,int selection);

//图片USM锐化以增加清晰度
void UnsharpMask(const IplImage* src, IplImage* dst, float amount=80, float radius=5, uchar threshold=0, int contrast=100);


/*求二值化图,otsu算法选择使类间方差最大的灰度值为阈值*/
void ImageBinarization(const IplImage *src,IplImage *dst);

//Nibliack二值化
void NibliackImageBinarization(const IplImage *src,IplImage *dst,int w,double k0);

//通过设定二值化图的均值来进行二值化，使得最终阈值范围在【pixmean-range，pixmean+range】
void PixelMeanThreshold(const IplImage* src,IplImage* dst,int pixmean,int range);

//同时释放多张图片内存
void ReleaseImages(IplImage** imgs,int n);

IplImage* GetRectImage(const IplImage* src,CvRect rect);

//图像旋转
IplImage* ImageRotate(const IplImage* src,double angle);
//自适应求边缘的最小和最大阈值
void AdaptiveFindThreshold(CvMat *dx, CvMat *dy, double *low, double *high);

//直线滤波，type=1时，只滤除竖直直线，type=2时
void SmoothLine(IplImage* img_bw,int val,int lineL,int type);


//二值化图水平和竖直方向投影
int* histogramVH(const IplImage* img_bw,int isH);
//局部迭代二值化
void ThresholdPartIter(IplImage* img_gray,IplImage* img_bw,
					   int color,int iters = 4);
//获取图img中像素值为val的直方图，hor为1是求水平方向，为0时竖直方向
void ReturnHist(const IplImage* img,int* hist,int val,int hor);

//标准化灰度图,标准化后的灰度图像素平均值为m，方差为v
void NormalizeGrayImage(const IplImage* src,
						IplImage* dst,
						float m,
						float v
						);
void LengthFilter(const IplImage* src,IplImage* dst,int minThres,int maxThres);

//灰度图拉伸
void GrayStretch(const IplImage* img_src,IplImage* img_dst);
#endif