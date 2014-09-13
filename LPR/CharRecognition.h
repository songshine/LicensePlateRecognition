#ifndef _CHAR_RECOGNITION_H_
#define _CHAR_RECOGNITION_H_

/************************************************************************/
/*							字符识别                                    */
/************************************************************************/
#endif

#include "stdafx.h"
#include <cv.h>
#include <highgui.h>
#include "LPHelper.h"





void LoadMatConvert(CvMat *mat_64fc1,CvMat *mat_32fc1,double data[][24*48],CvSize szMat,int num);
void LoadMatConvert1(CvMat *mat_64fc1,CvMat *mat_32fc1,double **data,CvSize szCharMat,int num);
/**参数说明：此函数功能将CV_64FC1矩阵转换为CV_32FC1矩阵，并用data数组进行初始化矩阵。
*mat1创建为CV_64FC1类型
*mat2创建为CV_32FC1类型
*data为矩阵头初始化的数据
*num为矩阵的个数
*/

void dataCvt2Mat32fc1();//将模板数据转换为CV_32FC1的矩阵

CvMat* imgCvt2Mat32fc1(IplImage *img,CvSize szChar,CvSize szImg);//将图像文件转换为CV_32FC1的矩阵

IplImage * t_Resize(IplImage *img,CvSize size);     //将图像设定为size尺寸

char* ReturnChar(int mark); //根据标记返回字符（0-72，分别为汉字、字母、数字）
char* ReturnChar2(int mark); //根据标记返回字符（0-72，分别为汉字、字母、数字.由于车牌后5个编码没有I和O，所以此处函数将排除这两个字符）

IplImage* PreProcessing(IplImage *img,CvSize szChar,CvSize szImg);   //图像的预处理

IplImage*  GrayEqualize(IplImage* src);               //灰度均衡



BOOL IsInSub(int mark,int &data,int num);       //检测第二字符是否在该省份的发牌机关中

BOOL IsInCitySub(Char_Letter *S_char1,Char_Letter *S_char2);   //根据S_char1判断，使得S_char2在相应的发牌机关集合中取值，提高识别


/**************************字符识别@@方法@@调用接口*****************************/
CString MultiTemplateRecogn(IplImage** charImg,int charnum);     //多模板识别调用接口
CString TempalteFeatureRecogn(IplImage** charImg,int charnum,int FeatureMethod);   //模板匹配+特征值
CString FeatureTempalteRecogn(IplImage** charImg,int charnum,int FeatureMethod);   //特征值+模板匹配结合

/**************************字符识别相关函数*****************************/
Char_Letter* CharacterRecognition(IplImage* img_chars,int type);//字符识别 

void ChRecgUseMultiTemplate(IplImage* img_chars,Char_Letter* ch,bool UseReturnChar2);    //在第一步使用标准模板库后，进一步使用文件库中多模板匹配
double GetMaxSimiByMultiTemplate(IplImage* img_char,CvMat *mat32fc1,int numChar);    //取得库中各个字符类别匹配值的最大值


/*************************图像处理*****************************/
IplImage* GradientSharp(IplImage *src);       //图像的梯度锐化

int ImageStretchByHistogram(IplImage *src1,IplImage *dst1);     //通过直方图变换进行图像增强，将图像灰度的域值拉伸到0-255

IplImage* rotateImage1(IplImage* img,int degree);       ////旋转图像内容不变，尺寸相应变大


/**************获取特征值********/
charVector* GetFeatureFromImgByGrid(IplImage* img);//将图像分割为12网格3*4,使用网格法获得特征值
charVector* GetFeatureFromMatByGrid(CvMat *mat);   //使用网格法对图像进行像素统计，从矩阵中获取特征值

charVector* GetFeatureFromImgByPrj(IplImage* img);//使用投影方法对图像进行像素统计,从图像中获取特征值
charVector* GetFeatureFromMatByPrj(CvMat *mat);//使用投影方法对图像进行像素统计，从矩阵中获取特征值

/****************计算相似度************/
double EuclideanDistance(charVector *Sr,charVector *Te,int methodType);//欧式距离
Similary* calEuclideanDistance(int NumOfChar,Char_Letter* letter,int type);            //计算欧式距离外接口
Similary* calEuclideanDistance(charVector *Sr,charVector *Te,int numOfChar,int type);            //计算欧式距离外接口



/*******************判定模板匹配字符是否有较大区分度********************
*letter:传入的字符结构体
*LimitNum:设定的区分度
**/
bool IsDistinguishChar(Char_Letter *letter,float LimitNum);

/****************获取特征********************************/
void InitTemplateFeature();        //获取全部模板字库特征
void InitSrcFreature(IplImage **img,CvSize RszImg);           //获取被识别字符特征


/********************根据欧式距离返回字符**********************/
void ReturnCharByEucDistance(Char_Letter *ch,Similary *Sim,bool UseReturnChar2);



/*******ReszNoChangeRotateImg    改变图片的尺寸内容不变，且可旋转*****
*szChar:字符大小
*
*szImg：转换成的图像尺寸，保持字符大小不变*/
IplImage* ReszNoChangeRotateImg(IplImage* img,int degree,CvSize szChar,CvSize szImg); 

/*********************拷贝文件到指定路径___作为统计使用************************************/
void FileCopyTo(CString source, CString destination, CString searchStr, BOOL cover);

 /*********************将图像写入文件相关操作*************************/
void GreyValueCollection(IplImage* pImg,char* fname);
void GreyValueCollection(IplImage* pImg,int CharIndex);
void DirImgWrite2File(CString fpath,CString fpathName,CvSize imgSz);

int ReturnCharIndex(CString strName);    //根据字符返回字符索引

CString GetPathByIndexChar(int index);
/*******************读取文件数据******/
double** ReadFromFile(CString filename,CvSize szChar,int NumChar);

int ScanCharNumFrmFile(CString filename,int charHeight);


/********************取相似值操作***************************************/
double* GetMaxValFrmArray(double arraySimi[],int numOfArray,int numOfMax);
double* GetMinValFrmArray(double arraySimi[],int numOfArray,int numOfMin);
int GetMarkFrmArray(double* arraySimi,int numOfArray,double SimiVal);

/*********************释放内存*******************************/
void FreeAll();



/************************K阶矩取特征值***************************************/
void CalBarycenter(IplImage  *img,int &nBarycenterX , int &nBarycenterY);
double CalCenterMoment(IplImage *img,double nBarycenterX , double nBarycenterY,int ip,int jq);
double DirectionCharacteristic(IplImage *img,int nBarycenterX , int nBarycenterY,int Direction);

charVector *GetFeatureByMomment(IplImage *img);
charVector *GetFeatureByMomment(CvMat *mat);