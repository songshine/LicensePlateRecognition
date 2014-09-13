#ifndef _LPLOCATION_H_
#define _LPLOCATION_H_
#include <cxcore.h>
//车牌的尺寸限制信息
typedef struct  _lp_size_info
{
	int min_height;   //车牌最小高度
	int max_height;	  //车牌最大高度
	int min_width;	  //车牌最小宽度
	int max_width;	  //车牌最小宽度
}lp_size_info;


//车牌在图像中的大致位置
//以左上角为原点,以像素为单位
typedef struct _lp_pos_info
{
	int ld ;  //距离左边界距离
	int rd ;  //距离右边界距离
	int ad ;  //距离上边界距离
	int bd ;  //距离下边界距离
}lp_pos_info;
/************************************************************************/
/*							车牌定位                                    */
/************************************************************************/

/*车牌定位,定位成功返回车牌，以及车牌位置放在rect中*/
IplImage* LPLocate(const IplImage* src,CvRect* rect,int *color);

/*调节阈值，范围为【1,100】，默认为15*/
void AdjustThreshold(int val);

void  LpLocateInit();

#endif