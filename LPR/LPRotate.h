#ifndef _LPROTATE_H_
#define _LPROTATE_H_
/************************************************************************/
/*						���ƽ���		                                */
/************************************************************************/

void LPRotate(const IplImage* src,IplImage* dst);		//���ƽ���
IplImage* LPRotateFromRect(const IplImage* src_lpr,CvRect rect,int color);

//���ݽǶȽ�����ת���Ƕ�Ϊˮƽ������,angleΪ����
void rotate_img(const IplImage* src,IplImage* dst,double angle,CvScalar fillVal = cvScalarAll(0));
#endif