#ifndef _LPHELPER_H_
#define _LPHELPER_H_
#define SMOOTH_WHITE_CC    1    //�����ɫ��ͨ��
#define SMOOTH_BLACK_CC    2	//�����ɫ��ͨ��
#define SMOOTH_ALL_CC    3	//���������ͨ��
//��ɫ
#define COLOR_OTHERS 4
#define COLOR_BLUE 0
#define COLOR_YELLOW 1
#define COLOR_WHITE 2
#define COLOR_BLACK 3

#define CHARACTER_NUM 7		//Ĭ���ַ�����

#define max(a,b)  ((a)>(b)?(a):(b))
#define min(a,b)  ((a)<(b)?(a):(b))

//��һ�ײ��ͼ
void FirstDiff(const IplImage* src1,IplImage* dst);
/*����ͼ��ĶԱȶȣ�contrast�ķ�Χ[-255,255]*/
void AdjustContrast(const IplImage* src, IplImage* dst, int contrast);
/*ȥ��ֵ��ͼ��С����ͨ��,binImgΪ��ֵ��ͼ��
ThresΪ����ֵ��С�ڻ���ڸ�����ֵ����ͨ��ȥ��,
selectionѡ�����������ͨ��*/
void SmoothBinImg(IplImage* binImg,int Thres,int selection);

//ͼƬUSM��������������
void UnsharpMask(const IplImage* src, IplImage* dst, float amount=80, float radius=5, uchar threshold=0, int contrast=100);


/*���ֵ��ͼ,otsu�㷨ѡ��ʹ��䷽�����ĻҶ�ֵΪ��ֵ*/
void ImageBinarization(const IplImage *src,IplImage *dst);

//Nibliack��ֵ��
void NibliackImageBinarization(const IplImage *src,IplImage *dst,int w,double k0);

//ͨ���趨��ֵ��ͼ�ľ�ֵ�����ж�ֵ����ʹ��������ֵ��Χ�ڡ�pixmean-range��pixmean+range��
void PixelMeanThreshold(const IplImage* src,IplImage* dst,int pixmean,int range);

//ͬʱ�ͷŶ���ͼƬ�ڴ�
void ReleaseImages(IplImage** imgs,int n);

IplImage* GetRectImage(const IplImage* src,CvRect rect);

//ͼ����ת
IplImage* ImageRotate(const IplImage* src,double angle);
//����Ӧ���Ե����С�������ֵ
void AdaptiveFindThreshold(CvMat *dx, CvMat *dy, double *low, double *high);

//ֱ���˲���type=1ʱ��ֻ�˳���ֱֱ�ߣ�type=2ʱ
void SmoothLine(IplImage* img_bw,int val,int lineL,int type);


//��ֵ��ͼˮƽ����ֱ����ͶӰ
int* histogramVH(const IplImage* img_bw,int isH);
//�ֲ�������ֵ��
void ThresholdPartIter(IplImage* img_gray,IplImage* img_bw,
					   int color,int iters = 4);
#endif











/*
#ifndef _LPHELPER_H_
#define _LPHELPER_H_
#define SMOOTH_WHITE_CC    1    //�����ɫ��ͨ��
#define SMOOTH_BLACK_CC    2	//�����ɫ��ͨ��
#define SMOOTH_ALL_CC    3	//���������ͨ��
//��ɫ
#define COLOR_OTHERS 4
#define COLOR_BLUE 0
#define COLOR_YELLOW 1
#define COLOR_WHITE 2
#define COLOR_BLACK 3

#define CHARACTER_NUM 7		//Ĭ���ַ�����
#define DEFAULT_WIDTH_AND_HEIGHT_RATIO 4.8		//���ƿ�߱�
#define DEFAULT_HEIGHT 60			//���ƿ��

#define max(a,b)  ((a)>(b)?(a):(b))
#define min(a,b)  ((a)<(b)?(a):(b))
/ *����ͼ��ĶԱȶȣ�contrast�ķ�Χ[-255,255]* /
void AdjustContrast(const IplImage* src, IplImage* dst, int contrast);
/ *ȥ��ֵ��ͼ��С����ͨ��,binImgΪ��ֵ��ͼ��
ThresΪ����ֵ��С�ڻ���ڸ�����ֵ����ͨ��ȥ��,
selectionѡ�����������ͨ��* /
void SmoothBinImg(IplImage* binImg,int Thres,int selection);

//ͼƬUSM��������������
void UnsharpMask(const IplImage* src, IplImage* dst, float amount=80, float radius=5, uchar threshold=0, int contrast=100);


/ *���ֵ��ͼ,otsu�㷨ѡ��ʹ��䷽�����ĻҶ�ֵΪ��ֵ* /
void ImageBinarization(const IplImage *src,IplImage *dst);

//Nibliack��ֵ��
void NibliackImageBinarization(const IplImage *src,IplImage *dst,int w,double k0);

//ͨ���趨��ֵ��ͼ�ľ�ֵ�����ж�ֵ����ʹ��������ֵ��Χ�ڡ�pixmean-range��pixmean+range��
void PixelMeanThreshold(const IplImage* src,IplImage* dst,int pixmean,int range);

//ͬʱ�ͷŶ���ͼƬ�ڴ�
void ReleaseImages(IplImage** imgs,int n);

IplImage* GetRectImage(const IplImage* src,CvRect rect);

//ͼ����ת
IplImage* ImageRotate(const IplImage* src,double angle);
#endif*/
