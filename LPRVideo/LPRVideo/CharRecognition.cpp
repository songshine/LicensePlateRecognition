#include "stdafx.h"
#include "CharRecognition.h"
#include "svm.h"



//����ģ��Ŀ�͸�
#define CHINESE_MODEL_WIDTH 37
#define CHINESE_MODEL_HEIGHT 43

//��ͨ�ַ������ֿ��
#define MODEL_WIDHT 29
#define MODEL_HEIGHT 29

//�������ַ���������
#define TOTAL_CHARACTERS 74
//��ǰmodel����
#define CUR_MODELS 62
/**************************ģ��˵��***************************************/
/*                                                                   */
/************************************************************************/

/******************************�ֲ�������������**************************/
struct svm_model* models[CUR_MODELS];///////////�洢models


char *typeName[TOTAL_CHARACTERS] = {"��","��","��","��","��","³","��",
									"��","��","ԥ","A","B","C","D","E",
									"F","G","H","J","K","L","M","N","P","Q",
									"R","S","T","U","V","W","X","Y","Z","0","1",
									"2","3","4","5","6","7","8","9","!"};

//�ַ�ʶ�𣬷��س��ƺ��ַ���
char* CharacterRecognition1(const IplImage* img_chars,int Min,int Maxx,int flag,int recoType); 

/*������arr�������±�*/

int Max(double *arr,int *m,int n);

/*��ȡ�ָ���ַ�����ֵ*/
void load_svm_txtfile(const IplImage *img,unsigned a[],int flag);

///*��ȡ��һ��ͼƬ*//*
//IplImage* GetResize(IplImage* img_gray);
//�����ַ����ͽ���ʶ��
char* CharacterRecognition(const IplImage* img_chars,int recoType)
{
		switch(recoType)
	{
	case CHINESE_RECO:
		return CharacterRecognition1(img_chars,1,11,1,recoType);
		break;
	case LETTER_RECO:
		return CharacterRecognition1(img_chars,11,35,0,recoType);
		break;
	case DIGIT_RECO:
	case LETTER_DIGIT_RECO:
	default:
		return CharacterRecognition1(img_chars,11,45,0,recoType);
	}
}
//***************��ȡ�ָ���ַ�����ֵ*******************//
void load_svm_txtfile(const IplImage *img,unsigned a[],int flag)
{
	int j=0;
	int row,col;

	IplImage* dst = NULL;
	//��һ��
	CvSize dst_cvsize;

	if(flag==1)
	{
		dst_cvsize.width = CHINESE_MODEL_WIDTH;  
		dst_cvsize.height = CHINESE_MODEL_HEIGHT; 
	}
	else
	{
		dst_cvsize.width = MODEL_WIDHT; 
		dst_cvsize.height =MODEL_HEIGHT; 
	}
	//����Ŀ��ͼ��
	dst = cvCreateImage( dst_cvsize, img->depth, img->nChannels); 
	//����Դͼ��Ŀ��ͼ��
	cvResize(img, dst, CV_INTER_LINEAR);	

	cvThreshold(dst, dst, 60, 255, CV_THRESH_BINARY|CV_THRESH_OTSU);


	for(row=0;row<dst->height;row++)
	{
		uchar* ptr=(uchar*)(dst->imageData+row*dst->widthStep);
		for(col=0;col<dst->width;col++)
		{ 
			a[j++]=ptr[col];
		}	 
	}
	//�ͷ�Ŀ��ͼ��ռ�õ��ڴ�
	cvReleaseImage(&dst); 
}

//***************�������з�����*******************//
void load_svm_model(char* modlefile,int flag,int flag1)
{
//	models=(struct svm_model **)malloc(74*sizeof(struct svm_model *));
	//int k=1;
	char n[1024];
	char s1[]=".model";
	char q[5];
	for(flag1;flag1<flag;flag1++)
	{
		strcpy(n,modlefile);		
		itoa(flag1,q,10);
		strcat(n,q);		
		strcat(n,s1);
	//	AfxMessageBox(n);
		models[flag1]=svm_load_model(n);//////����model
		if(models[flag1] == NULL)
			exit(1);
		//ASSERT(models[k] != NULL);
	}
}
void free_model()
{
	for(int i=1;i<CUR_MODELS;i++)
		svm_free_and_destroy_model(&models[i]);
}
//***************������arr�������±�*******************//
int Max(double *arr,int *m,int n)
{
	int i;
	int max=0;
	for(i=0;i<n;i++)
	{	
		if(arr[m[i]] > arr[m[max]])
        max=i;
	}
	return max;
}

char* CharacterRecognition1(const IplImage* img_chars,int Min,int Maxx,int flag,int recoType)
{
//	char *typeName[74] = {"��","��","��","��","A","B","C","D","0","1","2","3","4","5","6","7","8","9","A","B","C","D","!"};
	int m = 0,k,i,sumPix = 0; 
	char* result =(char *)malloc(8*sizeof(char));
	
	int* j=(int *)malloc(CUR_MODELS*sizeof(int));    ///////���ڴ洢model�±�

//	IplImage *cpimg_chars=cvCloneImage(img_chars);

	struct svm_node *x;
	double predict_type;
	double *prob_estimates=NULL;
	//�洢ͼ���е�����
	unsigned* data = NULL;  
	
	if(flag==1)
	//����ʶ��
		sumPix = CHINESE_MODEL_HEIGHT*CHINESE_MODEL_HEIGHT;		
	else
		sumPix = MODEL_HEIGHT*MODEL_HEIGHT;

	//�����ڴ��������
	x=(struct svm_node *)malloc((sumPix+1)*sizeof(struct svm_node));
	data = (unsigned*)malloc(sizeof(unsigned)*sumPix);
	load_svm_txtfile(img_chars,data,flag);

	

	for(i=0;i<sumPix;i++)
	{
		x[i].index=i+1;
		x[i].value=data[i];

	}
	x[i].index=-1;

	for(k=Min;k<Maxx;k++)
	{
		predict_type=svm_predict(models[k],x);//////Ԥ������
		if(predict_type==1)
		{
			j[m++]=k;
			strcpy(result,typeName[k-1]);
		}
	}
	if(m==0)
	{
		strcpy(result,"*");
	}
	//if(m==1)
	//{
	//	if(j[0]==27)
	//	{
	//		//	//	predict_type1=svm_predict(models[45],x);//////Ԥ������
	//		if((svm_predict(models[45],x))==-1)
	//			k=40;
	//		strcpy(result,typeName[k-1]);
	//	}
	//}
	if(m>1)
	{
		double p[CUR_MODELS];
		for(int d=0;d<m;d++)
		{
			double v,f;
			int nr_class=svm_get_nr_class(models[j[d]]);
			prob_estimates = (double *) malloc(nr_class*sizeof(double));
			v = svm_predict_probability(models[j[d]],x,prob_estimates);
			f=prob_estimates[0];
			p[j[d]]=f;
		}
		k=j[Max(p,j,m)];

	    switch (k)
		{
		case 27:
		//	//	predict_type1=svm_predict(models[45],x);//////Ԥ������
			if((svm_predict(models[45],x))==-1)
			{k=40;}
		    break;
		case 28:
		{
			if((svm_predict(models[47],x))==-1)
				k=36;
			if((svm_predict(models[46],x))==-1)
				k=42;
		}
		break;
		case 19:
		{
			if((svm_predict(models[48],x))==-1)
				k=36;
		}
		break;
		case 35:
		{
		//	if((svm_predict(models[49],x))==-1)
		//		k=25;
			/*if((svm_predict(models[50],x))==-1)
				k=20;*/
			if((svm_predict(models[60],x))==-1)
				k=14;
		}
		break;
		case 42:
		{
			if((svm_predict(models[50],x))==-1)
				k=36;
			if((svm_predict(models[51],x))==-1)
				k=34;
		}	
		break;
		case 44:
		{
			if((svm_predict(models[52],x))==-1)
				k=31;
		}
		break;
		case 11:
		{
			if((svm_predict(models[53],x))==-1)
				k=39;
			if((svm_predict(models[54],x))==-1)
				k=32;
		}
		break;
		case 15:
		{
			if((svm_predict(models[55],x))==-1)
				k=12;
		}
		break;
		case 12:
		{
			if((svm_predict(models[56],x))==-1)
				k=43;
		}
		break;
		case 33:
		{
			if((svm_predict(models[57],x))==-1)
				k=44;
		}
		break;
		case 34:
		{
			if((svm_predict(models[58],x))==-1)
				k=37;
		}
		break;
		case 29:
		{
			if((svm_predict(models[59],x))==-1)
				k=35;
		}
		break;
		case 16:
		{
			if((svm_predict(models[61],x))==-1)
				k=24;
		}
		break;
		}
		//if(k==27)
		//{
		////	//	predict_type1=svm_predict(models[45],x);//////Ԥ������
		//	if((svm_predict(models[45],x))==-1)
		//		k=40;
		//}
		//if(k==28)
		//{
		//	if((svm_predict(models[47],x))==-1)
		//		k=36;
		//}
		//if(k==28)
		//{
		//	if((svm_predict(models[46],x))==-1)
		//		k=42;
		//}
		//if(k==19)
		//{
		//	if((svm_predict(models[48],x))==-1)
		//		k=36;
		//}
		//if(k==35)
		//{
		//	if((svm_predict(models[49],x))==-1)
		//		k=25;
		//	/*if((svm_predict(models[50],x))==-1)
		//		k=20;*/
		//}
		//if(k==42)
		//{
		//	if((svm_predict(models[50],x))==-1)
		//		k=36;
		//	if((svm_predict(models[51],x))==-1)
		//		k=34;
		//}	
		//if(k==44)
		//{
		//	if((svm_predict(models[52],x))==-1)
		//		k=31;
		//}
		//if(k==11)
		//{
		//	if((svm_predict(models[53],x))==-1)
		//		k=39;
		//	if((svm_predict(models[54],x))==-1)
		//		k=32;
		//}
		//if(k==15)
		//{
		//	if((svm_predict(models[55],x))==-1)
		//		k=12;
		//}
		//if(k==12)
		//{
		//	if((svm_predict(models[56],x))==-1)
		//		k=43;
		//}
		//if(k==33)
		//{
		//	if((svm_predict(models[57],x))==-1)
		//		k=44;
		//}
		//if(k==34)
		//{
		//	if((svm_predict(models[58],x))==-1)
		//		k=37;
		//}
		//if(k==29)
		//{
		//	if((svm_predict(models[59],x))==-1)
		//		k=35;
		//}
		//if(k==35)
		//{
		//	if((svm_predict(models[60],x))==-1)
		//		k=14;
		//}
		//if(k==16)
		//{
		//	if((svm_predict(models[61],x))==-1)
		//		k=24;
		//}
		strcpy(result,typeName[k-1]);
	}
	
	free(x);
	x = NULL;
	free(j);
	j=NULL;
	free(data);
	data = NULL;
	free(prob_estimates);
	prob_estimates=NULL;
//	cvReleaseImage(&cpimg_chars);
//	AfxMessageBox(timespend);
	return result;
}

