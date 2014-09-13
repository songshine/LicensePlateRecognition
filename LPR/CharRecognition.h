#ifndef _CHAR_RECOGNITION_H_
#define _CHAR_RECOGNITION_H_

/************************************************************************/
/*							�ַ�ʶ��                                    */
/************************************************************************/
#endif

#include "stdafx.h"
#include <cv.h>
#include <highgui.h>
#include "LPHelper.h"





void LoadMatConvert(CvMat *mat_64fc1,CvMat *mat_32fc1,double data[][24*48],CvSize szMat,int num);
void LoadMatConvert1(CvMat *mat_64fc1,CvMat *mat_32fc1,double **data,CvSize szCharMat,int num);
/**����˵�����˺������ܽ�CV_64FC1����ת��ΪCV_32FC1���󣬲���data������г�ʼ������
*mat1����ΪCV_64FC1����
*mat2����ΪCV_32FC1����
*dataΪ����ͷ��ʼ��������
*numΪ����ĸ���
*/

void dataCvt2Mat32fc1();//��ģ������ת��ΪCV_32FC1�ľ���

CvMat* imgCvt2Mat32fc1(IplImage *img,CvSize szChar,CvSize szImg);//��ͼ���ļ�ת��ΪCV_32FC1�ľ���

IplImage * t_Resize(IplImage *img,CvSize size);     //��ͼ���趨Ϊsize�ߴ�

char* ReturnChar(int mark); //���ݱ�Ƿ����ַ���0-72���ֱ�Ϊ���֡���ĸ�����֣�
char* ReturnChar2(int mark); //���ݱ�Ƿ����ַ���0-72���ֱ�Ϊ���֡���ĸ������.���ڳ��ƺ�5������û��I��O�����Դ˴��������ų��������ַ���

IplImage* PreProcessing(IplImage *img,CvSize szChar,CvSize szImg);   //ͼ���Ԥ����

IplImage*  GrayEqualize(IplImage* src);               //�ҶȾ���



BOOL IsInSub(int mark,int &data,int num);       //���ڶ��ַ��Ƿ��ڸ�ʡ�ݵķ��ƻ�����

BOOL IsInCitySub(Char_Letter *S_char1,Char_Letter *S_char2);   //����S_char1�жϣ�ʹ��S_char2����Ӧ�ķ��ƻ��ؼ�����ȡֵ�����ʶ��


/**************************�ַ�ʶ��@@����@@���ýӿ�*****************************/
CString MultiTemplateRecogn(IplImage** charImg,int charnum);     //��ģ��ʶ����ýӿ�
CString TempalteFeatureRecogn(IplImage** charImg,int charnum,int FeatureMethod);   //ģ��ƥ��+����ֵ
CString FeatureTempalteRecogn(IplImage** charImg,int charnum,int FeatureMethod);   //����ֵ+ģ��ƥ����

/**************************�ַ�ʶ����غ���*****************************/
Char_Letter* CharacterRecognition(IplImage* img_chars,int type);//�ַ�ʶ�� 

void ChRecgUseMultiTemplate(IplImage* img_chars,Char_Letter* ch,bool UseReturnChar2);    //�ڵ�һ��ʹ�ñ�׼ģ���󣬽�һ��ʹ���ļ����ж�ģ��ƥ��
double GetMaxSimiByMultiTemplate(IplImage* img_char,CvMat *mat32fc1,int numChar);    //ȡ�ÿ��и����ַ����ƥ��ֵ�����ֵ


/*************************ͼ����*****************************/
IplImage* GradientSharp(IplImage *src);       //ͼ����ݶ���

int ImageStretchByHistogram(IplImage *src1,IplImage *dst1);     //ͨ��ֱ��ͼ�任����ͼ����ǿ����ͼ��Ҷȵ���ֵ���쵽0-255

IplImage* rotateImage1(IplImage* img,int degree);       ////��תͼ�����ݲ��䣬�ߴ���Ӧ���


/**************��ȡ����ֵ********/
charVector* GetFeatureFromImgByGrid(IplImage* img);//��ͼ��ָ�Ϊ12����3*4,ʹ�����񷨻������ֵ
charVector* GetFeatureFromMatByGrid(CvMat *mat);   //ʹ�����񷨶�ͼ���������ͳ�ƣ��Ӿ����л�ȡ����ֵ

charVector* GetFeatureFromImgByPrj(IplImage* img);//ʹ��ͶӰ������ͼ���������ͳ��,��ͼ���л�ȡ����ֵ
charVector* GetFeatureFromMatByPrj(CvMat *mat);//ʹ��ͶӰ������ͼ���������ͳ�ƣ��Ӿ����л�ȡ����ֵ

/****************�������ƶ�************/
double EuclideanDistance(charVector *Sr,charVector *Te,int methodType);//ŷʽ����
Similary* calEuclideanDistance(int NumOfChar,Char_Letter* letter,int type);            //����ŷʽ������ӿ�
Similary* calEuclideanDistance(charVector *Sr,charVector *Te,int numOfChar,int type);            //����ŷʽ������ӿ�



/*******************�ж�ģ��ƥ���ַ��Ƿ��нϴ����ֶ�********************
*letter:������ַ��ṹ��
*LimitNum:�趨�����ֶ�
**/
bool IsDistinguishChar(Char_Letter *letter,float LimitNum);

/****************��ȡ����********************************/
void InitTemplateFeature();        //��ȡȫ��ģ���ֿ�����
void InitSrcFreature(IplImage **img,CvSize RszImg);           //��ȡ��ʶ���ַ�����


/********************����ŷʽ���뷵���ַ�**********************/
void ReturnCharByEucDistance(Char_Letter *ch,Similary *Sim,bool UseReturnChar2);



/*******ReszNoChangeRotateImg    �ı�ͼƬ�ĳߴ����ݲ��䣬�ҿ���ת*****
*szChar:�ַ���С
*
*szImg��ת���ɵ�ͼ��ߴ磬�����ַ���С����*/
IplImage* ReszNoChangeRotateImg(IplImage* img,int degree,CvSize szChar,CvSize szImg); 

/*********************�����ļ���ָ��·��___��Ϊͳ��ʹ��************************************/
void FileCopyTo(CString source, CString destination, CString searchStr, BOOL cover);

 /*********************��ͼ��д���ļ���ز���*************************/
void GreyValueCollection(IplImage* pImg,char* fname);
void GreyValueCollection(IplImage* pImg,int CharIndex);
void DirImgWrite2File(CString fpath,CString fpathName,CvSize imgSz);

int ReturnCharIndex(CString strName);    //�����ַ������ַ�����

CString GetPathByIndexChar(int index);
/*******************��ȡ�ļ�����******/
double** ReadFromFile(CString filename,CvSize szChar,int NumChar);

int ScanCharNumFrmFile(CString filename,int charHeight);


/********************ȡ����ֵ����***************************************/
double* GetMaxValFrmArray(double arraySimi[],int numOfArray,int numOfMax);
double* GetMinValFrmArray(double arraySimi[],int numOfArray,int numOfMin);
int GetMarkFrmArray(double* arraySimi,int numOfArray,double SimiVal);

/*********************�ͷ��ڴ�*******************************/
void FreeAll();



/************************K�׾�ȡ����ֵ***************************************/
void CalBarycenter(IplImage  *img,int &nBarycenterX , int &nBarycenterY);
double CalCenterMoment(IplImage *img,double nBarycenterX , double nBarycenterY,int ip,int jq);
double DirectionCharacteristic(IplImage *img,int nBarycenterX , int nBarycenterY,int Direction);

charVector *GetFeatureByMomment(IplImage *img);
charVector *GetFeatureByMomment(CvMat *mat);