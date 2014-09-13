#ifndef _CHAR_RECOGNITION_H_
#define _CHAR_RECOGNITION_H_

#include <cxcore.h>
/************************************************************************/
/*							字符识别                                    */
/************************************************************************/
#define CHINESE_RECO 1
#define LETTER_RECO 2
#define DIGIT_RECO 3
#define LETTER_DIGIT_RECO 4

/*对7个分割好的char_imgs进行识别*/
char* CharacterRecognize(const IplImage** char_imgs);

char* CharacterRecognition(const IplImage* img_chars,int recoType);


//释放models
void load_svm_model(const char* modlefile);

void load_svm_model(char* modlefile,int flag,int flag1);

void free_model();

#endif