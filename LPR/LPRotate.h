#ifndef _LPROTATE_H_
#define _LPROTATE_H_
/************************************************************************/
/*						车牌矫正		                                */
/************************************************************************/

void LPRotate(const IplImage* src,IplImage* dst);		//车牌矫正
IplImage* LPRotateFromRect(const IplImage* src_lpr,CvRect rect,int color);

//根据角度进行旋转，角度为水平正方向,angle为弧度
void rotate_img(const IplImage* src,IplImage* dst,double angle,CvScalar fillVal = cvScalarAll(0));
#endif