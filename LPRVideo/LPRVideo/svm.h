#pragma once

#ifndef _LIBSVM_H
#define _LIBSVM_H

#define LIBSVM_VERSION 310    //libsvm������+�汾��

#ifdef __cplusplus           //__cplusplus��ʾ�������жϳ�������c����c++����������ġ�������c++����ʱ�������ʾ���ᱻ���壬����c����ʱ�����ᶨ�塣
extern "C" {
#endif

	extern int libsvm_version;

	struct svm_node             //���ݽڵ㣬ÿ���ڵ�ֻ�ǵ������������е�һ��������
	{
		int index;
		double value;
	};

	struct svm_problem       //���ݼ��������������ʸ�������ݽṹ��
	{
		int l;            //��¼�������� 
		double *y;        //ָ�����������������С��ڶ��������У���Ϊʹ����one-agianst-one����������ԭʼ������y[i]�������� 1.0��2.0��3.0����,������������ʱ���μӷ������������Ӧ�� y[i]������+1,�ͣ�1��
		struct svm_node **x;  //ָ��һ���洢����Ϊ��ά�����У�
	};									

	enum { C_SVC, NU_SVC, ONE_CLASS, EPSILON_SVR, NU_SVR };	/* svm_type */  //���ڵĳ�Ա����  SVC��SVM��������SVR��SVM�ع�
	enum { LINEAR, POLY, RBF, SIGMOID, PRECOMPUTED }; /* kernel_type */     //�˺���		�ṩ���ֺ˺������ֱ������ԣ�����ʽ���������sigmoid�ͺ�����

	struct svm_parameter      //SVM������
	{
		int svm_type;
		int kernel_type;
		int degree;	/* for poly */     // forpoly Ϊ2ʽ�е�d
		double gamma;	/* for poly/rbf/sigmoid *///// forpoly/rbf/sigmoidΪ2,3,4�е�gamma
		double coef0;	/* for poly/sigmoid *///// forpoly/sigmoid Ϊ2,4�е�r

		/* these are for training only */
		double cache_size; // in MB      in MB �ƶ�ѵ������Ҫ�Ĵ洢����Ԥ����40M  
		double eps;	// stopping criteria stopping criteria 
		double C;	// for C_SVC, EPSILON_SVR and NU_SVR  for C-SVC , ��-SVR  and ��-SVC  �ͷ�����
		int nr_weight;		//for C_SVC     for C-SVC Ȩ�ص���Ŀ,Ŀǰ��ʵ��������ֻ������ֵ��һ ����Ĭ��0������һ����svm_binary_svc_probability������ʹ����ֵ2��
		int *weight_label;	//for C_SVC      for C-SVC Ȩ�أ�Ԫ�ظ�����nr_weight����
		double* weight;		// for C_SVC    for C-SVC
		double nu;	//for NU_SVC, ONE_CLASS, and NU_SVR     for ��-SVC, ONE_CLASS, and ��-SVR
		double p;	// for EPSILON_SVR   for ��-SVR
		int shrinking;	//use the shrinking heuristics     use the shrinking heuristics ָ��ѵ�������Ƿ�ʹ��ѹ��
		int probability; //do probability estimates     do probability estimates ָ���Ƿ�Ҫ�����ʹ���
	};

	//
	// svm_model
	// 
	struct svm_model   //ѵ���õ�ѵ��ģ�͡�
	{
		struct svm_parameter param;	/* parameter */// parameter ѵ������,����ʹ�õ���svm_param��ʵ
		//����������ָ�롣����ѵ����ɺ�ԭ����������ȫ��������ȥԤ��ʱ���Ͳ��õ�
		//���´�ѵ����Ѳ������
		int nr_class;		/* number of classes, = 2 in regression/one class svm */// number of classes, = 2 in regression/one class svm �����
		int l;			/* total #SV */// total #SV ֧��������
		struct svm_node **SV;		// SVs (SV[l]) ��������֧��������ָ�룬����֧�����������ݣ�����Ǵ��ļ��ж�ȡ�����ݻ���Ᵽ���� �����ֱ��ѵ��������������ԭ����ѵ�����С����ѵ����ɺ���ҪԤ����ԭ����ѵ�����ڴ治�����ͷš�
		double **sv_coef;	/* coefficients for SVs in decision functions (sv_coef[k-1][l]) */ //�൱���б����е�alpha
		double *rho;		/* constants in decision functions (rho[k*(k-1)/2]) *///�൱���б����е�b
		double *probA;		/* pariwise probability information */
		double *probB;

		/* for classification only */

		int *label;		/* label of each class (label[k]) */
		int *nSV;		/* number of SVs for each class (nSV[k]) */
		/* nSV[0] + nSV[1] + ... + nSV[k-1] = l */
		/* XXX */
		int free_sv;		/* 1 if svm_model is created by svm_load_model*/
		/* 0 if svm_model is created by svm_train */
	};
	//ѵ������
	struct svm_model *svm_train(const struct svm_problem *prob, const struct svm_parameter *param);
	//��SVM��������֤ 
	void svm_cross_validation(const struct svm_problem *prob, const struct svm_parameter *param, int nr_fold, double *target);
	//����ѵ���õ�ģ�͵��� 
	int svm_save_model(const char *model_file_name, const struct svm_model *model);
	//�ӵ��а�ѵ���õ�ģ�Ͷ����洢����
	struct svm_model *svm_load_model(const char *model_file_name);
	// �õ�SVM����̬
	int svm_get_svm_type(const struct svm_model *model);
	//�õ����ϼ�������������뾭��ѵ���õ�ģ�ͺ�ſ����ã�
	int svm_get_nr_class(const struct svm_model *model);
	//�õ����ϼ�������ţ����뾭��ѵ���õ�ģ�ͺ�ſ����ã�
	void svm_get_labels(const struct svm_model *model, int *label);
	//LibSvm2.6�������� 
	double svm_get_svr_probability(const struct svm_model *model);
	//��ѵ���õ�ģ��Ԥ��������ֵ�������������������С������ǽ��溯����
	double svm_predict_values(const struct svm_model *model, const struct svm_node *x, double* dec_values);
	//Ԥ��ĳһ������ֵ
	double svm_predict(const struct svm_model *model, const struct svm_node *x);
	// ��������
	double svm_predict_probability(const struct svm_model *model, const struct svm_node *x, double* prob_estimates);

	void svm_free_model_content(struct svm_model *model_ptr);
	void svm_free_and_destroy_model(struct svm_model **model_ptr_ptr);
	void svm_destroy_param(struct svm_parameter *param);
	//�������Ĳ�������֤�����ѵ������������
	const char *svm_check_parameter(const struct svm_problem *prob, const struct svm_parameter *param);
	// �������� 
	int svm_check_probability_model(const struct svm_model *model);

	void svm_set_print_string_function(void (*print_func)(const char *));

#ifdef __cplusplus
}
#endif

#endif /* _LIBSVM_H */
