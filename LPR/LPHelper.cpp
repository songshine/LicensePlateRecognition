#include "stdafx.h"
#include "LPHelper.h"
#define B(image,x,y)((uchar*)(image->imageData+image->widthStep*(y)))[(x*3)]//B
#define G(image,x,y)((uchar*)(image->imageData+image->widthStep*(y)))[(x*3)+1]//G
#define R(image,x,y)((uchar*)(image->imageData+image->widthStep*(y)))[(x*3)+2]//R
#define S(image,x,y)((uchar*)(image->imageData+image->widthStep*(y)))[(x)]		//S


/*����ͼ��ĶԱȶȣ�contrast�ķ�Χ[-255,255]*/
void AdjustContrast(const IplImage* src, IplImage* dst, int contrast)
{
	if (!src)return ;

	int imagewidth = src->width;
	int imageheight = src->height;
	int channel = src->nChannels;

	//��ԭͼ��ֵ
	CvScalar mean = {0,0,0,0};
	for (int y=0; y<imageheight; y++)
	{
		for (int x=0; x<imagewidth; x++)
		{
			for (int k=0; k<channel; k++)
			{
				CvScalar ori = cvGet2D(src, y, x);
				for (int k=0; k<channel; k++)
				{
					mean.val[k] += ori.val[k];
				}
			}
		}
	}
	for (int k=0; k<channel; k++)
	{
		mean.val[k] /= imagewidth * imageheight;
	}

	//�����Աȶ�
	if (contrast <= -255)
	{
		//����������-255ʱ����ͼ��Աȶȵ��¶˼��ޣ���ʱ��ͼ��RGB�����������ڷ�ֵ��ͼ���ȫ��ɫ���Ҷ�ͼ��ֻ��1���ߣ�����ֵ�Ҷȣ�
		for (int y=0; y<imageheight; y++)
		{
			for (int x=0; x<imagewidth; x++)
			{
				cvSet2D(dst, y, x, mean);
			}
		}
	} 
	else if(contrast > -255 &&  contrast <= 0)
	{
		//(1)nRGB = RGB + (RGB - Threshold) * Contrast / 255
		// ����������-255��С��0ʱ��ֱ��������Ĺ�ʽ����ͼ�����ظ�����
		//��ʽ�У�nRGB��ʾ�������R��G��B������RGB��ʾԭͼR��G��B������ThresholdΪ�����ķ�ֵ��ContrastΪ������ĶԱȶ�������
		for (int y=0; y<imageheight; y++)
		{
			for (int x=0; x<imagewidth; x++)
			{
				CvScalar nRGB;
				CvScalar ori = cvGet2D(src, y, x);
				for (int k=0; k<channel; k++)
				{
					nRGB.val[k] = ori.val[k] + (ori.val[k] - mean.val[k]) *contrast /255;
				}
				cvSet2D(dst, y, x, nRGB);
			}
		}
	}
	else if(contrast >0 && contrast <255)
	{
		//����������0��С��255ʱ�����Ȱ����湫ʽ(2)����������Ȼ���ٰ����湫ʽ(1)����Աȶȣ�
		//(2)��nContrast = 255 * 255 / (255 - Contrast) - 255
		//��ʽ�е�nContrastΪ�����ĶԱȶ�������ContrastΪ�����ĶԱȶ�������

		CvScalar nRGB;
		int nContrast = 255 *255 /(255 - contrast) - 255;

		for (int y=0; y<imageheight; y++)
		{
			for (int x=0; x<imagewidth; x++)
			{
				CvScalar ori = cvGet2D(src, y, x);
				for (int k=0; k<channel; k++)
				{
					nRGB.val[k] = ori.val[k] + (ori.val[k] - mean.val[k]) *nContrast /255;
				}
				cvSet2D(dst, y, x, nRGB);
			}
		}
	}
	else
	{
		//���������� 255ʱ����ͼ��Աȶȵ��϶˼��ޣ�ʵ�ʵ�������ͼ��ֵ��ͼ������������ɫ��ɣ��Ҷ�ͼ�����8���ߣ�
		//���졢�ơ��̡��ࡢ�����ϼ�����ף�
		for (int y=0; y<imageheight; y++)
		{
			for (int x=0; x<imagewidth; x++)
			{
				CvScalar rgb;
				CvScalar ori = cvGet2D(src, y, x);
				for (int k=0; k<channel; k++)
				{
					if (ori.val[k] > mean.val[k])
					{
						rgb.val[k] = 255;
					}
					else
					{
						rgb.val[k] = 0;
					}
				}
				cvSet2D(dst, y, x, rgb);
			}
		}
	}
}

//ȥ��ֵ��ͼ��С����ͨ��
void SmoothBinImg(IplImage* binImg,int Thres,int selection)
{ 
	CvSeq* contour = NULL; 
	double minarea = Thres;  
	double tmparea = 0.0;  
	CvMemStorage* storage = cvCreateMemStorage(0);   
	IplImage* img_Clone=cvCloneImage(binImg);  
	//���ʶ�ֵͼ��ÿ�����ֵ  
	uchar *pp;  

	//------------������ֵͼ�е�������������������ɾ�����С��ĳ����ֵminarea������-------------//  
	CvScalar color = cvScalar(255,0,0);//CV_RGB(128,0,0);  
	CvContourScanner scanner = NULL;  
	scanner = cvStartFindContours(img_Clone,storage,sizeof(CvContour),CV_RETR_CCOMP,CV_CHAIN_APPROX_NONE,cvPoint(0,0));  
	//��ʼ����������  
	CvRect rect;  
	while (contour=cvFindNextContour(scanner))  
	{  
		tmparea = fabs(cvContourArea(contour));  
		rect = cvBoundingRect(contour,0);     
		if (tmparea < minarea/*||tmparea>4900*/)  
		{  

			//����ͨ������ĵ�Ϊ��ɫʱ�����������С���ð�ɫ�������  
			pp=(uchar*)(binImg->imageData + binImg->widthStep*(rect.y+rect.height/2)+rect.x+rect.width/2);  

			if (pp[0]==255 && (selection == SMOOTH_WHITE_CC || selection == SMOOTH_ALL_CC)) 
			{
				for(int y = rect.y;y<rect.y+rect.height;y++)  
				{  
					for(int x =rect.x;x<rect.x+rect.width;x++)  
					{  
						pp=(uchar*)(binImg->imageData + binImg->widthStep*y+x);  

						if (pp[0]==255)  
						{  
							pp[0]=0;  
						}  
					}  
				}  
			}
			if(pp[0]==0 && (selection == SMOOTH_BLACK_CC || selection == SMOOTH_ALL_CC))
			{
				for(int y = rect.y;y<rect.y+rect.height;y++)  
				{  
					for(int x =rect.x;x<rect.x+rect.width;x++)  
					{  
						pp=(uchar*)(binImg->imageData + binImg->widthStep*y+x);  

						if (pp[0]==0)  
						{  
							pp[0]=255;  
						}  
					}  
				}  
			}

		}  
	} 

}


void UnsharpMask(const IplImage* src, IplImage* dst, float amount, float radius, uchar threshold, int contrast)
{
	if(!src)return ;

	int imagewidth = src->width;
	int imageheight = src->height;
	int channel = src->nChannels;

	IplImage* blurimage =    cvCreateImage(cvSize(imagewidth,imageheight), src->depth, channel);
	IplImage* DiffImage =    cvCreateImage(cvSize(imagewidth,imageheight), 8, channel);

	//ԭͼ�ĸ߶Աȶ�ͼ��
	IplImage* highcontrast = cvCreateImage(cvSize(imagewidth,imageheight), 8, channel);
	AdjustContrast(src, highcontrast, contrast);

	//ԭͼ��ģ��ͼ��
	cvSmooth(src, blurimage, CV_GAUSSIAN, (int)radius);

	//ԭͼ��ģ��ͼ����
	for (int y=0; y<imageheight; y++)
	{
		for (int x=0; x<imagewidth; x++)
		{
			CvScalar ori = cvGet2D(src, y, x);
			CvScalar blur = cvGet2D(blurimage, y, x);
			CvScalar val;
			val.val[0] = abs(ori.val[0] - blur.val[0]);
			val.val[1] = abs(ori.val[1] - blur.val[1]);
			val.val[2] = abs(ori.val[2] - blur.val[2]);

			cvSet2D(DiffImage, y, x, val);
		}
	}

	//��
	for (int y=0; y<imageheight; y++)
	{
		for (int x=0; x<imagewidth; x++)
		{
			CvScalar hc = cvGet2D(highcontrast, y, x);
			CvScalar diff = cvGet2D(DiffImage, y, x);
			CvScalar ori = cvGet2D(src, y, x);
			CvScalar val;

			for (int k=0; k<channel; k++)
			{
				if (diff.val[k] > threshold)
				{
					//����ͼ�� = ԭʼ*(1-r) + �߶Ա�*r
					val.val[k] = ori.val[k] *(100-amount) + hc.val[k] *amount;
					val.val[k] /= 100;
				}
				else
				{
					val.val[k] = ori.val[k];
				}
			}
			cvSet2D(dst, y, x, val);
		}
	}

	cvReleaseImage(&blurimage);
	cvReleaseImage(&DiffImage);
}
/*otsu�㷨ѡ��ʹ��䷽�����ĻҶ�ֵΪ��ֵ�����кܺõ�Ч����

1������ֱ��ͼ����һ��histogram
2������ͼ��ҶȾ�ֵavgValue.
3������ֱ��ͼ�����w[i]��һ����u[i]
4�����㲢�ҵ�������䷽�between-class variance��
variance[i]=(avgValue*w[i]-u[i])*(avgValue*w[i]-u[i])/(w[i]*(1-w[i]))
��Ӧ����󷽲�ĻҶ�ֵ��ΪҪ�ҵ���ֵ
5�����ҵ�����ֵ��ֵ��ͼ��*/
void ImageBinarization(const IplImage *src,IplImage *dst)
{	/*�ԻҶ�ͼ���ֵ��������Ӧ����threshold*/
	int i,j,width,height,step,chanel,threshold;
	/*size��ͼ��ߴ磬svg�ǻҶ�ֱ��ͼ��ֵ��va�Ƿ���*/
	float size,avg,va,maxVa,p,a,s;
	unsigned char *dataSrc;
	float histogram[256];
	cvCopy(src,dst);
	width = dst->width;
	height = dst->height;
	dataSrc = (unsigned char *)dst->imageData;
	step = dst->widthStep/sizeof(char);
	chanel = dst->nChannels;
	/*����ֱ��ͼ����һ��histogram*/
	for(i=0; i<256; i++)
		histogram[i] = 0;
	for(i=0; i<height; i++)
		for(j=0; j<width*chanel; j++)
		{
			histogram[dataSrc[i*step+j]-'0'+48]++;
		}
		size = (float)width * height;
		for(i=0; i<256; i++)
			histogram[i] /=size;
		/*����Ҷ�ֱ��ͼ��ֵ�ͷ���*/
		avg = 0;
		for(i=0; i<256; i++)
			avg += i*histogram[i];
		va = 0;
		for(i=0; i<256; i++)
			va += fabs(i*i*histogram[i]-avg*avg);
		/*���ü�Ȩ��󷽲�������*/
		threshold = 27;
		maxVa = 0;
		p = a = s = 0;
		for(i=0; i<256; i++)
		{
			p += histogram[i];
			a += i*histogram[i];
			s = (avg*p-a)*(avg*p-a)/p/(1-p);
			if(s > maxVa)
			{
				threshold = i;
				maxVa = s;
			}
		}
		threshold-=7;
		/*��ֵ��*/
		for(i=0; i<height; i++)
			for(j=0; j<width*chanel; j++)
			{
				if(dataSrc[i*step+j] > threshold)
					dataSrc[i*step+j] = 255;
				else
					dataSrc[i*step+j] = 0;
			}
}

//Nibliack��ֵ��
void NibliackImageBinarization(const IplImage *src,IplImage *dst,int w,double k0)
{
	int i,j,k,l,t;
	/*avg�ǻҶ�ֱ��ͼ��ֵ��va�Ƿ���*/
	float avg,va;
	int sum;
	int width,height;
	width = src->width;
	height = src->height;
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
		{
			sum = 0;
			t = 0;
			for(k=-w;k<=w;k++)
				for(l=-w;l<=w;l++)
				{
					if(k >= 0 && l >= 0 && k+i<height && l+j<width)
					{
						sum += CV_IMAGE_ELEM(src,uchar,k+i,l+j);
						t++;
					}
				}
			avg = (float)sum/t;
			
			va = 0;
			for(k=-w;k<=w;k++)
				for(l=-w;l<=w;l++)
				{
					if(k >= 0 && l >= 0 && k+i<height && l+j<width)
					{
						va += (CV_IMAGE_ELEM(src,uchar,k+i,l+j)-avg)*(CV_IMAGE_ELEM(src,uchar,k+i,l+j)-avg);
					}					
				}
			va = sqrt((float)va/t);

			if(avg+k0*va < CV_IMAGE_ELEM(src,uchar,i,j))
				CV_IMAGE_ELEM(dst,uchar,i,j) = 255;
			else
				CV_IMAGE_ELEM(dst,uchar,i,j) = 0;
		}
	
}
//��ֵ��ͼ��ʹ��������ֵ��Χ�ڡ�pixmean-range��pixmean+range��
void PixelMeanThreshold(const IplImage* src,IplImage* dst,int pixmean,int range)
{
	int down = 0,up = 255,mid = 128;   //��ʼ��Χ
	double px = 0;
	int iternum = 32;  //����������
	range = abs(range);
	cvThreshold(src,dst,mid,255,CV_THRESH_BINARY);
	px = cvMean(dst);
	while(abs(px-pixmean) > range && iternum-- > 0)
	{
		if(px > pixmean)
		{
			down = mid;
			mid = (mid+up)/2;
		}
		else
		{
			up = mid;
			mid = (mid+down)/2;
		}
		cvThreshold(src,dst,mid,255,CV_THRESH_BINARY);
		px = cvMean(dst);
	}
}

//ͬʱ�ͷŶ���ͼƬ�ڴ�
void ReleaseImages(IplImage** imgs,int n)
{
	for(int i=0;i<n;i++)
		if(imgs[i] != NULL)
		cvReleaseImage(&imgs[i]);
	imgs = NULL;
}

IplImage* GetRectImage(const IplImage* src,CvRect rect)
{
	//����ROI����
	IplImage* img = cvCloneImage(src);
	IplImage* dst = NULL;
	cvSetImageROI(img,rect);
	dst = cvCreateImage(cvSize(rect.width,rect.height),img->depth,img->nChannels);
	cvCopy(img,dst);
	cvResetImageROI(img);
	cvReleaseImage(&img);
	return dst;
}
//���ݽǶȽ�����ת���Ƕ�Ϊˮƽ������
IplImage* ImageRotate(const IplImage* src,double angle)
{
	/* the first command line parameter must be image file name */
	int delta = 1;
	// 0:  ������ת
	double factor = 1;
	IplImage* dst = NULL;
	dst = cvCloneImage (src);
	/*cvNamedWindow ("src", 1);
	cvShowImage ("src", src);*/

	float m[6];
	// Matrix m looks like:
	//
	// [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]
	// [ m3  m4  m5 ]       [ A21  A22   b2 ]
	//
	CvMat M = cvMat (2, 3, CV_32F, m);
	int w = src->width;
	int h = src->height;
	m[0] = (float) (factor * cos (-angle * 2 * CV_PI / 180.));
	m[1] = (float) (factor * sin (-angle * 2 * CV_PI / 180.));
	m[3] = -m[1];
	m[4] = m[0];
	// ����ת��������ͼ���м�
	m[2] = w * 0.5f;
	m[5] = h * 0.5f;
	//  dst(x,y) = A * src(x,y) + b
	cvZero (dst);
	cvGetQuadrangleSubPix (src, dst, &M);
	/*cvNamedWindow ("dst", 1);
	cvShowImage ("dst", dst);
	cvWaitKey(0);*/
	return dst;

}



//��ֵ��ͼˮƽ����ֱ����ͶӰ
int* histogramVH(const IplImage* img_bw,int isH)
{
	int i,j;
	int height = img_bw->height;
	int width = img_bw->width;
	int* his = NULL;
	//ˮƽ����
	if(isH)
	{
		his = (int*)malloc(sizeof(int)*width);
		for(i=0;i<width;i++)
		{
			his[i] = 0;
			for(j=0;j<height;j++)
			{
				if(CV_IMAGE_ELEM(img_bw,uchar,j,i) == 255)
					his[i]++;
			}
		}
	}
	else//��ֱ����
	{
		his = (int*)malloc(sizeof(int)*height);
		for(i=0;i<height;i++)
		{
			his[i] = 0;
			for(j=0;j<width;j++)
			{
				if(CV_IMAGE_ELEM(img_bw,uchar,i,j) == 255)
					his[i]++;
			}
		}
	}

	return his;
}
//�ֲ�������ֵ��
void ThresholdPartIter(IplImage* img_gray,IplImage* img_bw,int color,int iters)
{
	double meanPix;
	int i;
	int nImgWidth = img_gray->width;
	int nImgHeight = img_gray->height;
	int* beginPix = (int*)malloc(sizeof(int)*(iters+1));
	for(i=0;i<iters;i++)
		beginPix[i] = nImgWidth/iters*i;
	beginPix[iters] = nImgWidth;
	for(i=0;i<iters;i++)
	{
		cvSetImageROI(img_gray,cvRect(beginPix[i],0,beginPix[i+1]-beginPix[i],nImgHeight));
		cvSetImageROI(img_bw,cvRect(beginPix[i],0,beginPix[i+1]-beginPix[i],nImgHeight));
		meanPix = cvMean(img_gray);
		switch(color)
		{
		case COLOR_BLUE:
			cvThreshold(img_gray,img_bw,meanPix+10,255,CV_THRESH_BINARY);
			break;
		case COLOR_YELLOW:
			cvThreshold(img_gray,img_bw,meanPix-20,255,CV_THRESH_BINARY_INV);
			break;
		case COLOR_WHITE:
			break;
		case COLOR_BLACK:
			break;
		}

		cvResetImageROI(img_gray);
		cvResetImageROI(img_bw);
	}

}