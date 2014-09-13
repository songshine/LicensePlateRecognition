#ifndef _LPLOCATION_H_
#define _LPLOCATION_H_
#include <cxcore.h>
//���Ƶĳߴ�������Ϣ
typedef struct  _lp_size_info
{
	int min_height;   //������С�߶�
	int max_height;	  //�������߶�
	int min_width;	  //������С���
	int max_width;	  //������С���
}lp_size_info;


//������ͼ���еĴ���λ��
//�����Ͻ�Ϊԭ��,������Ϊ��λ
typedef struct _lp_pos_info
{
	int ld ;  //������߽����
	int rd ;  //�����ұ߽����
	int ad ;  //�����ϱ߽����
	int bd ;  //�����±߽����
}lp_pos_info;
/************************************************************************/
/*							���ƶ�λ                                    */
/************************************************************************/

/*���ƶ�λ,��λ�ɹ����س��ƣ��Լ�����λ�÷���rect��*/
IplImage* LPLocate(const IplImage* src,CvRect* rect,int *color);

/*������ֵ����ΧΪ��1,100����Ĭ��Ϊ15*/
void AdjustThreshold(int val);

void  LpLocateInit();

#endif