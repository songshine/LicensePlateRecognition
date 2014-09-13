#ifndef _LPLOCATION_H_
#define _LPLOCATION_H_

/************************************************************************/
/*							车牌定位                                    */
/************************************************************************/

/*车牌定位,定位成功返回车牌，以及车牌位置放在rect中*/
IplImage* LPLocate(const IplImage* src,CvRect* rect,int *color);


#endif







/*
#ifndef _LPLOCATION_H_
#define _LPLOCATION_H_

/ ************************************************************************ /
/ *							车牌定位                                    * /
/ ************************************************************************ /

/ *车牌定位,定位成功返回车牌，以及车牌位置放在rect中* /
IplImage* LPLocate(IplImage* src,CvRect* rect,int *color);

/ *调节阈值，范围为【1,100】，默认为15* /
void AdjustThreshold(int val);
#endif*/
