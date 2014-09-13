#include "stdafx.h"
#include "CharSegmentation.h"
#include "LPRotate.h"
#include "LPHelper.h"
#include <math.h>
#define MAX_INI 65535   //������

#define POINT_X  18                   // ˮƽ�����ص���                        
#define POINT_Y  5                    // ��ֱ�����ص���   
#define RESIZE_IMAGE_WIDTH 160		  //����Ĭ�Ͽ��
#define RESIZE_IMAGE_HEIGHT 40		  //����Ĭ�ϸ߶�

int g_CS_threshold = 40;
int g_pixmean = 72;  //ƽ������ֵ
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
/* �ַ��ָ�ľֲ�����                                                    */
/************************************************************************/


/************************************************************************/
void cvThin( IplImage* src, IplImage* dst, int iterations=1) ;    
void autoCorrection(IplImage * src , int elementx , int elementy);
/************************************************************************/
//Ѱ���ַ��߽�,����bound�������boundRect�У������������ֵ����
int FindCharacterBound(const IplImage* bwImage,CvRect** boundRect);


//��ͨ������������ϲ�����֡�ɾ����ͨ�򣬷��ش�������ͨ�����,
//���У�grayImgΪLP�ĻҶ�ͼ��bwImgΪ��ֵ��ͼ,����boundNum >= 7
int CCAnalyse(const IplImage* grayImg,IplImage* bwImg,
			  CvRect** boundRect,int color);

//����ͨ�������ͳ�Ʒ������ҳ������Ƶ����ߵĿ�Ⱥ͸߶ȡ�
int CCStatistics(CvRect** boundRect,int boundNum,int* maxWidth,int* maxHeight);

//�о�ͼ���еľ��ηֲ�
struct borderParam* RectDistribution(CvRect** boundRect,int* boundNum,int rightWidth,int rightHeight);

//���г��ƣ��������±߽��ҳ����е��ַ�
int FindRectsBasedBound(const IplImage* img_bw,CvRect** boundRect,int boundNum,int rightWidth,
						int rightHeight,struct borderParam bp);
//�����и����,�����ַ���ȸ߶ȣ��Լ����±߿��ֱ�߷�����Ϣ
struct borderParam* ReturnSegmentationParameter(const IplImage* img_bw,
	int* rightWidth,int* rightHeight);
//�����и�������ÿ���ָ�õ��ַ������ؿ�ʱ�ָ�ʧ��
CvRect* ReturnCharRects(IplImage* img_bw,struct borderParam bp,int rightWidth,int rightHeight);

CvRect* ReturnCharRects2(IplImage* img_bw,struct borderParam bp,int rightWidth,int rightHeight);
//ͶӰ����λ
int test_FindRectsBasedBound(const IplImage* img_bw,CvRect** boundRect,int rightWidth, int rightHeight);
//�ϲ����ڵľ�����,���غϲ���ľ��θ���
int MergeRect(CvRect** boundRect,int boundNum,int width,int height);

//�ַ��ָ���غϲ���ľ��θ���
int SplitRect(CvRect** boundRect,int boundNum,int width,int height);
//������ǰ�����еľ������򣬻��ߵ���������������Ĵ�С,���������ε�����
int AdjustRectRegion(CvRect** boundRect,int boundNum,int width,int height,int imgHeight,int imgWidth);

//���ݸ���������ȡ��ÿһ���ַ���������ͼƬ
IplImage** GetBoundImages(const IplImage* src,CvRect* boundRect,int boundNum,int color);

//��ȡ��ֵ�����ַ�
IplImage* GetThreshholdChar(IplImage* img_gray,CvRect rect,int color);

//ɾ������ľ������򣬷������յľ�������
int DeleteRectRegion(const IplImage* bwImg,CvRect** boundRect,int boundNum,int width,int height);

int DeleteRects(CvRect** boundRect,int boundNum,int width,int height);

//����ַ�û����ȫ����Ҫ����ַ�
int AddRects(CvRect** boundRect,int boundNum,int rightWidth,int rightHeight,
											struct borderParam bp);
/*�ж����������Ƿ��ཻ�����շ����ཻ����������ཻ����ֵΪ0*/
int CrossRect(CvRect rt1,CvRect rt2);

//�������±߽�ı����
void ChangeRect(CvRect* pRect,float ah,float bh,float al,float bl,int width,int height);
/*��һ������������ɾ��һ������*/
void DelARect(CvRect** boundRect,int* boundNum,int id);

/*��һ���������������һ��������*/
void AddARect(CvRect** boundRect,int* boundNum,CvRect rect);

//���ؾ������
int AreaRect(CvRect rt){return rt.height*rt.width;}
//����ͼ��
IplImage* RotateImage(const IplImage* src,float ah,float bh,float al,float bl);
//ȥ����ֵ��ͼ�в����߿�����
void ClearResidualBorder(IplImage* img_bw);
//����ַ��߽����������
void ClearOutBorder(IplImage* img_bw,struct borderParam bp);

//�ַ��ָ����7���ַ�
IplImage** CharacterSegmentation(const IplImage* src,int* imgNum,int color) 
{
	double pixMeans = 0;   //����ƽ��ֵ
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
	
	//��ֹ��ֵ�����ַ���߿�ճ��
	ClearResidualBorder(g_img_lpr_bw);

	
	SmoothBinImg(g_img_lpr_bw,24,SMOOTH_WHITE_CC);
	
	
	//cvThin(g_img_lpr_thin,g_img_lpr_thin);
	//LPRotate(g_img_lpr_bw,g_img_lpr_bw);
	*imgNum  = CCAnalyse(g_img_lpr_gray,g_img_lpr_bw,&boundRect,color);

	//�Ӷ�ֵ��ͼ�л�ȡͼƬ
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


//��ͨ������������ϲ�����֡�ɾ����ͨ�򣬷��ش�������ͨ�����,����boundNum >= 7
int CCAnalyse(const IplImage* grayImg,IplImage* bwImg,
			  CvRect** boundRect,int color)
{
	//
	/*������Ҫ��������������¼�������:
	1.�ҵ�����Ƶ����ߵĿ�Ⱥ͸߶�
	2.�ϲ�һЩ�ص����ߺܽ�������
	3.��ֿ��Բ�ֵ�����
	4.ɾ��̫���׵�����
	5.�����������εĴ�С
	3.�����������δ�С*/
	int boundNum = 0;
	int rightHeight,rightWidth;		
	struct borderParam* bp = ReturnSegmentationParameter(bwImg,&rightWidth,&rightHeight);

	//�ж��ǲ���˫�л���
	if(rightHeight < bwImg->height*0.3 && color == COLOR_YELLOW)
	{//˫�л���
		AfxMessageBox("˫�л��ƣ��ݲ�����");
	}
	else if(NULL != bp && abs(bp->bh) < 1.5 && abs(bp->bl)<1.5 
		&& abs(bp->bh-bp->bl) <1 && rightHeight >= rightWidth)
	{//������
		//����߽���������ص�
		ClearOutBorder(bwImg,*bp);
		//rotate_img(bwImg,bwImg,-atan(min(bp->bl,bp->bh))*180/CV_PI);
		//RotateImage(bwImg,bp->ah,bp->bh,bp->al,bp->bl);
		//��ֹ��ֵ�����ַ���߿�ճ��
		//ClearResidualBorder(bwImg);
		/*cvShowImage("songddd",bwImg);
		cvWaitKey(0);*/
		//cvSmooth(bwImg,bwImg,CV_MEDIAN);
		//���¶�λ�ַ�
		//IplImage* img_tmp = cvCloneImage(grayImg);
		SmoothBinImg(bwImg,24,SMOOTH_WHITE_CC);
		boundNum = FindCharacterBound(bwImg,boundRect);
		
		//////һ�������			
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
//�����и����,�����ַ���ȸ߶ȣ��Լ����±߿��ֱ�߷�����Ϣ
struct borderParam* ReturnSegmentationParameter(const IplImage* img_bw,
	int* rightWidth,int* rightHeight)
{
	CvRect* rects = NULL;
	int boundNum = 0;
	struct borderParam* bp = NULL;


	boundNum = FindCharacterBound(img_bw,&rects);
	//�ҵ�����Ƶ����ߵĿ�Ⱥ͸߶�
	if(!CCStatistics(&rects,boundNum,rightWidth,rightHeight))
	{
		boundNum = 0;
		return NULL;
	}
	bp = RectDistribution(&rects,&boundNum,*rightWidth,*rightHeight);
	free(rects);
	return bp;
}


//�����и�������ÿ���ָ�õ��ַ������ؿ�ʱ�ָ�ʧ��
CvRect* ReturnCharRects(IplImage* img_bw,struct borderParam bp,
						int rightWidth,int rightHeight)
{
	int i,j,t;
	int nMeanPix = 0;  //ˮƽͶӰ��ƽ��ֵ
	int* hisHor = NULL;
	CvRect* boundRect = NULL;
	CvRect* rectTmp = NULL;
	int nRectTmpNum,nRightRectsNum = 0;
	IplImage* img_his = cvCreateImage(cvGetSize(g_img_lpr_bw),IPL_DEPTH_8U,1);
	cvZero(img_his);


	//��������߿����������
	ClearOutBorder(img_bw,bp);
	nRectTmpNum = FindCharacterBound(img_bw,&rectTmp);
	if(nRectTmpNum == 0)
		return NULL;
	//ˮƽͶӰ
	hisHor = histogramVH(img_bw,1);



	//Ϊ���յ��ַ�����ռ�
	boundRect = (CvRect*)malloc(sizeof(CvRect)*CHARACTER_NUM);
	//���Ȳ��ܺϲ��ľͺϲ���
	nRectTmpNum = MergeRect(&rectTmp,nRectTmpNum,rightWidth,rightHeight);
	//���ָܷ�ķָ�
	nRectTmpNum = SplitRect(&rectTmp,nRectTmpNum,rightWidth,rightWidth);

	//��ˮƽͶӰ��ƽ��ֵ
	for(i=0;i<img_bw->width;i++)
		nMeanPix += hisHor[i];
	nMeanPix /= img_bw->width;

	////��������Ѿ��ҵ����Լ�ˮƽͶӰͼ���з���
	for(i=0;i<nRectTmpNum;i++)
	{
		if(nRightRectsNum == CHARACTER_NUM)
			break;
		if(abs(rectTmp[i].width-rightWidth) <=3 
			&& abs(rectTmp[i].height-rightHeight) <= 5)
		{
			//������ӵ���׼�����б���
			AddARect(&boundRect,&nRightRectsNum,rectTmp[i]);
			for(j=rectTmp[i].x;j<rectTmp[i].x+rectTmp[i].width;j++)
				hisHor[j] = 0;
			//����ʱ�б��н���ɾ��
			DelARect(&rectTmp,&nRectTmpNum,i);
			i--;
		}
		else if(abs(rectTmp[i].height-rightHeight) <= 4 
			&& rectTmp[i].x > 5 && rectTmp[i].x < img_bw->width-5
			&& rectTmp[i].width < rightWidth)
		{//��1���ּ��룬���Ҳ��Ǵ���̫�����ص�1
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
			//���Ȳ��ܺϲ��ľͺϲ���
			nRectTmpNum = MergeRect(&rectTmp,nRectTmpNum,rightWidth,rightHeight);
			//���ָܷ�ķָ�
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
//����ͨ�������ͳ�Ʒ������ҳ������Ƶ����ߵĿ�Ⱥ͸߶ȡ�
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
	/*������:
	1.ͨ������ֱ��ҵ���Ⱥ͸߶ȳ���Ƶ����ߵ�ֵ����һ��x+-2��С��Χ*/

	for(i=0;i<boundNum;i++)
	{
		//����ֱ��ͼ
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
		{//����µ�ֱ��ͼ
			hisWidth[hisWidthNum].times = 1;
			hisWidth[hisWidthNum].value = (*boundRect)[i].width;
			hisWidthNum++;
		}
		else
		{//�����������һ���Ѿ����ڵ�ֱ��ͼ
			hisWidth[minHisId].value = (hisWidth[minHisId].value*hisWidth[minHisId].times
				+(*boundRect)[i].width)/(hisWidth[minHisId].times+1);

			hisWidth[minHisId].times++;
		}

		//��߶�ֱ��ͼ
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
		{//����µ�ֱ��ͼ
			hisHeight[hisHeightNum].times = 1;
			hisHeight[hisHeightNum].value = (*boundRect)[i].height;
			hisHeightNum++;
		}
		else
		{//�����������һ���Ѿ����ڵ�ֱ��ͼ
			hisHeight[minHisId].value = (hisHeight[minHisId].value*hisHeight[minHisId].times+
				(*boundRect)[i].height)/(hisHeight[minHisId].times+1);
			hisHeight[minHisId].times++;
		}
	}

	{//�����Ƶ����ߵĿ�Ⱥ͸߶�

		//�������Ƶ����ߵĿ�ȷ�Χ
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
		//�������Ƶ����ߵĸ߶ȷ�Χ
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


//�о�ͼ���еľ��ηֲ�
struct borderParam* RectDistribution(CvRect** boundRect,int* boundNum,
	int rightWidth,int rightHeight)
{
	/*���岽������:
	1.ͳ�Ƶõ����б�׼����;
	2.ͨ����С���˷�ȷ�����±߽�;
	3.*/
	int i;
	int* rightRectId;
	int rightRectNum = 0;
	float aH,bH,aL,bL;
	struct borderParam* bp;
	rightRectId = (int*)malloc(sizeof(int)*(*boundNum));

	{//�����ҳ���͸߶��Ƚ�����ľ���
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

	//������С���˷�������±߽�
	{
		int x,y;
		int x00=0,x01=0,x10=0,x11=0,
			y0=0,y1=0;
		//���ϱ߽�
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

		//���±߽�
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

//����ַ��߽����������
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

//ͶӰ����λ
//ͶӰ����λ
int test_FindRectsBasedBound(const IplImage* img_bw,CvRect** boundRect, int rightWidth,int rightHeight)
{

	CvRect *rightRects = NULL;
	int rightRectNum = 0;  //��׼���θ���
	int i=0,j=0,k=0;//ѭ������ 12
	int  letter[14]={0,20,23,43,55,75,78,98,101,121,124,127,147,167}; // Ĭ�Ϸָ�
	bool flag1=0;//    1     2     3     4       5           6    7  
	IplImage* img_b = cvCloneImage(img_bw);
	// ��ֱͶӰ
	int* num_h = histogramVH(img_bw,1);
	IplImage* img_his = cvCreateImage(cvGetSize(img_bw),IPL_DEPTH_8U,1);
	cvZero(img_his);

	for(int i=0;i<img_his->width;i++)
	{
		cvLine(img_his,cvPoint(i,img_his->height-1),cvPoint(i,img_his->height-1-num_h[i]),CV_RGB(255,255,255),1);
	}
	// cvShowImage("img_his",img_his);
	// ������λ����λ�� �ڶ����ַ�ĩ�ˣ�
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

	// ����λ
	for(i=0;i<RESIZE_IMAGE_WIDTH-1;i++)//ÿһ�е� 
	{
		for(j=17;j<=24;j++)  // 17-24 /40   ÿһ�е�17 �� 24 ����� 
		{
			num_h[i]+=CV_IMAGE_ELEM(img_bw,uchar,j,i)/45;
		}
	}

	for(j=letter[3];j>0;j--)//�ӵڶ����ַ���ĩ�˿�ʼ ��ǰ�ҵ�һ���͵ڶ����ַ���ʼλ��
	{
		if((num_h[j]<6)&&(num_h[j-1]<POINT_Y))//ֻҪ�������е� 17��24 �е�ֵС�� 2��
		{                                           //���ҵ��ڶ����ַ��Ŀ�ʼλ��
			letter[2]=j;      // find letter[2]  �ڶ����ַ��Ŀ�ʼλ��
			letter[1]=(j>=23)?j-3:letter[1];   //��һ���ַ��Ľ���λ��
			letter[0]=(j>=23)?j-23:letter[0];  //��һ���ַ�����ʼλ��
			break;       //�ҵ����˳�ѭ��
		}
	}

	j=2;  flag=0;flag1=0;//�������
	for(i=letter[4];i<RESIZE_IMAGE_WIDTH-1;i++)  //�ӵ������ַ��Ŀ�ʼλ������
	{
		if((num_h[i]>5)&&(num_h[i-1]>POINT_Y) && !flag )
		{
			flag=1;
			flag1=0;
			letter[2*j]=i-1; //���� ֻ��¼�ַ��Ŀ�ʼλ��
			if(j==6)  //�ж� ���һ���ַ��Ľ���λ�� �Ƿ�Խ�� ��������,���û��,��letter[13]=letter[12]+20
			{
				letter[2*j+1]=((letter[2*j]+20)>RESIZE_IMAGE_WIDTH-1)?RESIZE_IMAGE_WIDTH-1:letter[2*j]+20;
				break;//�˳� forѭ��
			}
		}
		else if((num_h[i]<5)&&(num_h[i-1]<POINT_Y) && !flag1 && flag)//����� �հ�����  
		{
			flag=0;
			flag1=1;
			letter[2*j+1]=i-1;
			j++; //j�Զ��� 1
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

	// �ͷ��ڴ�
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
//���г��ƣ��������±߽��ҳ����е��ַ�
int FindRectsBasedBound(const IplImage* img_bw,CvRect** boundRect,int boundNum,
						int rightWidth,	int rightHeight,struct borderParam bp)
{

	int i;	
	//�ø������¼���ε�״̬
	//-1: ��ʾҪɾ��
	//0:��ʾҪ����
	//1:��ʾ��׼����
	int* rightRectFlag;
	CvRect *rightRects = NULL;
	int rightRectNum = 0;  //��׼���θ���
	rightRectFlag = (int*)malloc(sizeof(int)*CHARACTER_NUM*2);
	rightRects = (CvRect *)malloc(sizeof(CvRect)*CHARACTER_NUM*2);
	IplImage* img_b = cvCloneImage(img_bw);

	{//�����ҳ���͸߶��Ƚ�����ľ���
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

	//��ʼ
	if(rightRectNum != CHARACTER_NUM)
	{
		int subX;  
		int subY;
		int dis = -1;  //������������֮��ľ���
		int secondId = -1;  //�ڶ����ַ��ı��
		int cx,cy;
		int first = -1,second = -1,third = -1,last = -1;
		if(rightRectFlag[0] == 0)
			first = 0;
		if(rightRectFlag[rightRectNum-1] == boundNum-1)
			last = boundNum-1;
		//Ѱ�ҵڶ����͵��������
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
				dis = 3; //Ĭ����������֮�������������
			if(rightRectNum != CHARACTER_NUM)
			{
				//���ǵ�һ��ȱʧ���ߵڶ����������������һ��
				if(second != -1 && first == -1)
				{//��ӵ�һ��

					cx = (int)max((*boundRect)[second].x-dis-rightWidth/2,0);
					cy = (int)((bp.ah+bp.al)/2+(bp.bh+bp.bl)/2*cx);
					cx = (int)max(cx-0.5*rightWidth,0);
					cy = (int)max(cy-0.5*rightHeight,0);
					AddARect(&rightRects,&rightRectNum,cvRect(cx,cy-cy,rightWidth,rightHeight));
					first = 0;
				}
				if(first != -1 && second == -1)
				{//��ӵڶ���
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
	//��һ��������߲��׾������ȱ�������ڴ˿���ʵ��Ŵ�
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
		/*step1:ȥ�����ұ߿�;
		  step2:�����±߿򴦵���������ȥ��*/
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
	
	//ˮƽ�˲�
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
	//��ֱ�˲�
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

//�������±߽�ı����
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
//�ϲ����ڵľ�����,���غϲ���ľ��θ���
int MergeRect(CvRect** boundRect,int boundNum,int width,int height)
{
	int i;
	int tempRightBound = 0;  //��¼����ʱ��Right�߽�
	int mergetimes = 0;  //��¼�ϲ��Ĵ���

	if(width != -1 && height != -1)
	{

		for(i=0;i<boundNum-1;i++)
		{
			//���ºϲ�
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
			//���Һϲ�
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


//�ַ��ָ���غϲ���ľ��θ���,ֻ���������ϲ��������ϲ������
int SplitRect(CvRect** boundRect,int boundNum,int width,int height)
{
	int i;
	CvRect rect;
	//int bn = boundNum;
	for(i=0;i<boundNum;i++)
	{
		if(((*boundRect)[i].width>2*width && (*boundRect)[i].width-2*width<0.5*width) && abs((*boundRect)[i].height-height)<=4)
		{//�����ַ���ͬһ�����ΰ�Χ�������ֳ�����
			rect = (*boundRect)[i];
			DelARect(boundRect,&boundNum,i);
			AddARect(boundRect,&boundNum,cvRect(rect.x,rect.y,width,height));
			AddARect(boundRect,&boundNum,cvRect(rect.x+rect.width-width,rect.y,width,height));
			continue;
		}
		if(abs((*boundRect)[i].width>3*width && (*boundRect)[i].width-3*width < width)&& abs((*boundRect)[i].height-height)<=4)
		{//�����ַ���ͬһ�����ΰ�Χ�������ֳ�����
			rect = (*boundRect)[i];
			DelARect(boundRect,&boundNum,i);
			AddARect(boundRect,&boundNum,cvRect(rect.x,rect.y,width,height));
			AddARect(boundRect,&boundNum,cvRect(rect.x+width+2,rect.y,width,height));
			AddARect(boundRect,&boundNum,cvRect(rect.x+width*2+4,rect.y,width,height));
		}

	}

	return boundNum;
}
//������ǰ�����еľ������򣬻��ߵ���������������Ĵ�С,���������ε�����
int AdjustRectRegion(CvRect** boundRect,int boundNum,int width,int height,int imgHeight,int imgWidth)
{
	int i,t;
	//������������Խϱ�׼�ľ�����
	//�������϶������ҵ�
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
		{//���ڿ��
			(*boundRect)[i].x-=(int)ceil((double)(width-(*boundRect)[i].width)/2);
			if((*boundRect)[i].x < 0)
				(*boundRect)[i].x = 0;
			if((*boundRect)[i].x+width >= imgWidth )
				(*boundRect)[i].width = imgWidth - (*boundRect)[i].x-1;
			else
				(*boundRect)[i].width = width;

		}
		if(abs((*boundRect)[i].height - height)>=2)
		{//���ڸ߶�
			//���еľ����������Ͻǵ���ͬһ��ֱ����
			t = (*boundRect)[firstId].x-(*boundRect)[secondId].x;
			if(t == 0)
				(*boundRect)[i].y = (*boundRect)[secondId].y;
			else
				(*boundRect)[i].y = ((*boundRect)[i].x-(*boundRect)[secondId].x)
				*(((*boundRect)[firstId].y-(*boundRect)[secondId].y)/t)
				+(*boundRect)[secondId].y;
			//��ֹ��Խ�߽�
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

//ɾ������ľ������򣬷������յľ�������
int DeleteRectRegion(const IplImage* bwImg,CvRect** boundRect,int boundNum,int width,int height)
{
	/*ɾ��һ������:
	1.ĳ����������������֮������ɾ��;
	2.�������˺����Ҷ˺ͱ�׼���ϴ���ɾ��
	3.ɾ����͸߲���width��heightһ��ľ���;*/
	int i,j;
	int t = boundNum;
	double pm;  //����ƽ��ֵ
	double area;
	IplImage* img_bw = NULL;
	IplImage* img_Roi = NULL;
	//�����ǰ���ε������������ַ��������˳�
	if(boundNum == CHARACTER_NUM)
		return boundNum;

	/*img_bw = cvCreateImage(cvGetSize(bwImg),bwImg->depth,1);
	cvCopy(bwImg,img_bw);*/
	img_bw = cvCloneImage(bwImg);
	for(i=0;i<boundNum;i++)
	{
		//ɾ�����̫��ľ���
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
	{//������߾����Ƿ�ͱ�׼����Ƿ�ϴ�
		DelARect(boundRect,&boundNum,0);
	}

	if(abs((*boundRect)[boundNum-1].height-height)>height/3 || abs((*boundRect)[boundNum-1].width-width)>width/3)
	{//�����ұ߾��κͱ�׼����Ƿ�ϴ�
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
			{//����i�ھ���j��
				DelARect(boundRect,&boundNum,i);
			}
			else if(area == (*boundRect)[j].height*(*boundRect)[j].width)
			{//����j��i��
				DelARect(boundRect,&boundNum,j);
			}
			else if(abs((*boundRect)[i].height-height)>height/4 && abs((*boundRect)[i].width-width)>width/4)
			{//����i�ͱ�׼���β���е��
				DelARect(boundRect,&boundNum,i);
			}

			else if(abs((*boundRect)[j].height-height)>width/4 && abs((*boundRect)[j].width-width)>width/4)
			{//����j�ͱ�׼���β���е��
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
	{//���ص��ܶ��ɾ��
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
		{//������߾����Ƿ�ͱ�׼����Ƿ�ϴ�
			DelARect(boundRect,&boundNum,0);
		}

		if(abs((*boundRect)[boundNum-1].height-height)>height/3 || abs((*boundRect)[boundNum-1].width-width)>width/3)
		{//�����ұ߾��κͱ�׼����Ƿ�ϴ�
			DelARect(boundRect,&boundNum,boundNum-1);
		}
	}
	while(boundNum > CHARACTER_NUM)
	{
		nMaxDis = 0;  //����һ����С��ֵ
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


//����ַ�û����ȫ����Ҫ����ַ�
int AddRects(CvRect** boundRect,int boundNum,int rightWidth,int rightHeight,
											struct borderParam bp)
{
	return 0;
}
int FindCharacterBound(const IplImage* bwImage,CvRect** boundRect)
{

	int mode = CV_RETR_EXTERNAL;	//������Ե��ȡʱ�Ĳ���
	CvSeq * contour = 0;		//������Ե��ȡʱ�Ĳ���
	CvRect bndRect = cvRect(0,0,0,0);//��cvBoundingRect������Ӿ���ʱ��Ҫ�ľ���
	CvMemStorage * storage; //������Ե��ȡʱ�Ĳ���
	IplImage* img_bw = NULL;
	int ret,i=0,imgW,imgH,j=0;
	double area,ratio;
	if(bwImage->nChannels != 1) return NULL;

	storage = cvCreateMemStorage(0);
	img_bw = cvCloneImage(bwImage);



	//Ѱ��LP��Ե
	ret = cvFindContours(img_bw, storage, &contour, sizeof(CvContour),
		mode, CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0));
	if(ret == 0)
	{
		boundRect = NULL;
		return 0;
	}

	
	if(*boundRect != NULL)
		free(*boundRect);
	//���䳬��ret�����Σ���ֹ��ָ����
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
		//�õ�һ�����Խ��ƶ�������Χ�ľ�������
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
		//����߽�����
		for(j=i-1;j>=0;j--)
		{
			if(bndRect.x < (*boundRect)[j].x)
				(*boundRect)[j+1] = (*boundRect)[j];
			else
				break;
		}

		//��ÿ���������½�����չ2����
		bndRect.y=bndRect.y>=2?bndRect.y-2:0;
		bndRect.height+=4;
		(*boundRect)[j+1] = bndRect;
		i++;

	}
	cvReleaseImage(&img_bw);
	cvReleaseMemStorage(&storage);
	return ret;
}

//���ݸ���������ȡ��ÿһ���ַ���������ͼƬ
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

			//����˴�����ϸ�������Խ�g_img_lpr_thin�ĳ�g_img_lpr_bw
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
	//����һ����
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

/*�жϾ���rt1��rt2�Ƿ��ཻ�������ཻ����������Ϊ��ʱ��ʾ���ཻ*/
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
/*��һ������������ɾ��һ������*/
void DelARect(CvRect** boundRect,int* boundNum,int id)
{
	int i;
	for(i=id;i<*boundNum-1;i++)
		(*boundRect)[i] = (*boundRect)[i+1];

	(*boundNum)--;
}
/*��һ���������������һ��������*/
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
//����ͼ��
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















