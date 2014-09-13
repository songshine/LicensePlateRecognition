#pragma once

#ifndef _LIBSVM_H
#define _LIBSVM_H

#define LIBSVM_VERSION 310    //libsvm函数库+版本号

#ifdef __cplusplus           //__cplusplus标示符用来判断程序是用c还是c++编译程序编译的。当编译c++程序时，这个标示符会被定义，编译c程序时，不会定义。
extern "C" {
#endif

	extern int libsvm_version;

	struct svm_node             //数据节点，每个节点只是单个样本向量中的一个特征。
	{
		int index;
		double value;
	};

	struct svm_problem       //数据集，存放所有样本矢量的数据结构。
	{
		int l;            //记录样本总数 
		double *y;        //指向样本所属类别的阵列。在多类问题中，因为使用了one-agianst-one方法，可能原始样本中y[i]的内容是 1.0，2.0，3.0，…,但参与多类计算时，参加分类的两类所对应的 y[i]内容是+1,和－1。
		struct svm_node **x;  //指向一个存储内容为二维的阵列；
	};									

	enum { C_SVC, NU_SVC, ONE_CLASS, EPSILON_SVR, NU_SVR };	/* svm_type */  //类内的成员变量  SVC是SVM分类器，SVR是SVM回归
	enum { LINEAR, POLY, RBF, SIGMOID, PRECOMPUTED }; /* kernel_type */     //核函数		提供四种核函数，分别是线性，多项式，径向基，sigmoid型函数。

	struct svm_parameter      //SVM参数。
	{
		int svm_type;
		int kernel_type;
		int degree;	/* for poly */     // forpoly 为2式中的d
		double gamma;	/* for poly/rbf/sigmoid *///// forpoly/rbf/sigmoid为2,3,4中的gamma
		double coef0;	/* for poly/sigmoid *///// forpoly/sigmoid 为2,4中的r

		/* these are for training only */
		double cache_size; // in MB      in MB 制定训练所需要的存储器，预设是40M  
		double eps;	// stopping criteria stopping criteria 
		double C;	// for C_SVC, EPSILON_SVR and NU_SVR  for C-SVC , ε-SVR  and ν-SVC  惩罚因子
		int nr_weight;		//for C_SVC     for C-SVC 权重的数目,目前在实例代码中只有两个值，一 个是默认0，另外一个是svm_binary_svc_probability函数中使用数值2。
		int *weight_label;	//for C_SVC      for C-SVC 权重，元素个数由nr_weight决定
		double* weight;		// for C_SVC    for C-SVC
		double nu;	//for NU_SVC, ONE_CLASS, and NU_SVR     for ν-SVC, ONE_CLASS, and ν-SVR
		double p;	// for EPSILON_SVR   for ε-SVR
		int shrinking;	//use the shrinking heuristics     use the shrinking heuristics 指明训练过程是否使用压缩
		int probability; //do probability estimates     do probability estimates 指明是否要做概率估计
	};

	//
	// svm_model
	// 
	struct svm_model   //训练好的训练模型。
	{
		struct svm_parameter param;	/* parameter */// parameter 训练参数,这里使用的是svm_param的实
		//例，而不是指针。这样训练完成后，原来参数被完全保留，再去预报时，就不用担
		//心下次训练会把参数冲掉
		int nr_class;		/* number of classes, = 2 in regression/one class svm */// number of classes, = 2 in regression/one class svm 类别数
		int l;			/* total #SV */// total #SV 支持向量数
		struct svm_node **SV;		// SVs (SV[l]) 　　保存支持向量的指针，至于支持向量的内容，如果是从文件中读取，内容会额外保留； 如果是直接训练得来，则保留在原来的训练集中。如果训练完成后需要预报，原来的训练集内存不可以释放。
		double **sv_coef;	/* coefficients for SVs in decision functions (sv_coef[k-1][l]) */ //相当于判别函数中的alpha
		double *rho;		/* constants in decision functions (rho[k*(k-1)/2]) *///相当于判别函数中的b
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
	//训练数据
	struct svm_model *svm_train(const struct svm_problem *prob, const struct svm_parameter *param);
	//用SVM做交叉验证 
	void svm_cross_validation(const struct svm_problem *prob, const struct svm_parameter *param, int nr_fold, double *target);
	//保存训练好的模型到档 
	int svm_save_model(const char *model_file_name, const struct svm_model *model);
	//从档中把训练好的模型读到存储器中
	struct svm_model *svm_load_model(const char *model_file_name);
	// 得到SVM的型态
	int svm_get_svm_type(const struct svm_model *model);
	//得到资料集的类别数（必须经过训练得到模型后才可以用）
	int svm_get_nr_class(const struct svm_model *model);
	//得到资料集的类别标号（必须经过训练得到模型后才可以用）
	void svm_get_labels(const struct svm_model *model, int *label);
	//LibSvm2.6新增函数 
	double svm_get_svr_probability(const struct svm_model *model);
	//用训练好的模型预报样本的值，输出结果保留到阵列中。（并非界面函数）
	double svm_predict_values(const struct svm_model *model, const struct svm_node *x, double* dec_values);
	//预报某一样本的值
	double svm_predict(const struct svm_model *model, const struct svm_node *x);
	// 新增函数
	double svm_predict_probability(const struct svm_model *model, const struct svm_node *x, double* prob_estimates);

	void svm_free_model_content(struct svm_model *model_ptr);
	void svm_free_and_destroy_model(struct svm_model **model_ptr_ptr);
	void svm_destroy_param(struct svm_parameter *param);
	//检查输入的参数，保证后面的训练能正常进行
	const char *svm_check_parameter(const struct svm_problem *prob, const struct svm_parameter *param);
	// 新增函数 
	int svm_check_probability_model(const struct svm_model *model);

	void svm_set_print_string_function(void (*print_func)(const char *));

#ifdef __cplusplus
}
#endif

#endif /* _LIBSVM_H */
