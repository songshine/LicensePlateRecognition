#include "stdafx.h"
#include "LPRotate.h"
#include "LPHelper.h"
#define MAXX(x,y) ((x) > (y) ? (x) : (y))
#define MINX(x,y) ((x) > (y) ? (y):(x))
/************************************************************************/
/*						车牌矫正		                                */
/************************************************************************/
//求一幅图像的(p,q)阶中心矩
long knMe(const IplImage* img,int p,int q);
//求一幅图的p+q中心矩
long kmpq(const IplImage* img,int p,int q);

//求一幅图片的倾斜角
double img_rotate_angle(const IplImage* img);

CvScalar GetMaxColor(const IplImage* img);
/********/
IplImage* LPRotateFromRect(const IplImage* src_lpr,CvRect rect,int color)
{
	double angle;
	IplImage* src,*gray,*bw,*dst;
	gray = cvCreateImage(cvSize(rect.width,rect.height),IPL_DEPTH_8U,1);
	bw = cvCreateImage(cvSize(rect.width,rect.height),IPL_DEPTH_8U,1);
	dst = cvCreateImage(cvSize(rect.width,rect.height),src_lpr->depth,src_lpr->nChannels);

	src = GetRectImage(src_lpr,rect);
	cvConvertImage(src,gray,CV_BGR2GRAY);
	AdjustContrast(gray,gray,100);
	ThresholdPartIter(gray,bw,color,3);
	// cvShowImage("img_bw",bw);
	cvWaitKey(0);
	angle = img_rotate_angle(src);
	rotate_img(src,dst,angle);
	cvReleaseImage(&src);
	cvReleaseImage(&gray);
	cvReleaseImage(&bw);
	return dst;
}
//车牌矫正	
void LPRotate(const IplImage* src,IplImage* dst)		
{
	double a = img_rotate_angle(src);
	return;
}

//求一幅图像的(p,q)阶中心矩
long knMe(const IplImage* img,int p,int q)
{
	long mpq = 0;
	int N = img->height;
	int M = img->width;
	int i,j;
	for(i=0;i<N;i++)
	{
		for(j=0;j<M;j++)
		{
			mpq += pow((double)i,p)*pow((double)j,q)*CV_IMAGE_ELEM(img,uchar,i,j)/255;
		}
	}

	return mpq;
}
CvScalar GetMaxColor(const IplImage* img)
{
	CvScalar scalar;
	IplImage* gray = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
	cvConvertImage(img,gray,CV_BGR2GRAY);
	return scalar;
	//cvGetSi
}
//求一幅图的p+q中心矩
long kmpq(const IplImage* img,int p,int q)
{
	int i,j;
	int ig,jg;
	long mpq=0;
	ig = knMe(img,1,0)/knMe(img,0,0);
	jg = knMe(img,0,1)/knMe(img,0,0);

	for(i=0;i<img->height;i++)
	{
		for(j=0;j<img->width;j++)
			mpq += pow((double)(i-ig),p)*pow((double)(j-jg),q)*CV_IMAGE_ELEM(img,uchar,i,j)/255;

	}
	return mpq;
}

//求一幅图片的倾斜角
double img_rotate_angle(const IplImage* img)
{
	double a = 0.0;
	long m11,m20,m02;
	m11 = knMe(img,1,1);
	m20 = knMe(img,2,0);
	m02 = knMe(img,0,2);
	a = (double)(2*m11)/(m20-m02);

	return (-0.5*atan(a));
}
//根据角度进行旋转，角度为水平正方向,angle为弧度
void rotate_img(const IplImage* src,IplImage* dst,
				double angle,CvScalar fillVal)
{
	double scale = 1.0;
	double a = 0.0;	
	CvMat* rot_mat = cvCreateMat(2,3,CV_32FC1);
	CvPoint2D32f center = cvPoint2D32f(
		0,
		src->height
		);
	if(abs(angle) >= 0.2)
		a= -angle*180/CV_PI/3;
	cv2DRotationMatrix(center,a,scale,rot_mat);
	cvWarpAffine(src,dst,rot_mat,CV_WARP_FILL_OUTLIERS,fillVal);
	
	//copyMakeBorder(dst,dst,3,3,3,3,BORDER_REPLICATE);
	//cvResize(dst,dst,CV_INTER_LINEAR);
	/*cvShowImage("src",src);
	cvShowImage("dst",dst);
	cvWaitKey(0);*/
}
