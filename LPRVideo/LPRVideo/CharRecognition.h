#ifndef _CHAR_RECOGNITION_H_
#define _CHAR_RECOGNITION_H_

#include <cxcore.h>
/************************************************************************/
/*							�ַ�ʶ��                                    */
/************************************************************************/
#define CHINESE_RECO 1
#define LETTER_RECO 2
#define DIGIT_RECO 3
#define LETTER_DIGIT_RECO 4

/*��7���ָ�õ�char_imgs����ʶ��*/
char* CharacterRecognize(const IplImage** char_imgs);

char* CharacterRecognition(const IplImage* img_chars,int recoType);


//�ͷ�models
void load_svm_model(const char* modlefile);

void load_svm_model(char* modlefile,int flag,int flag1);

void free_model();

#endif