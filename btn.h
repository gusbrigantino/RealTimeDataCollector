//
// COMP/GENG 422 - Gus Brigantino
//
// Button module header
//

#ifndef BTN_H_
#define BTN_H_


#define BTN_IS_RELEASED		0
#define BTN_IS_PRESSED		1


void BtnInit(void);
void BtnTask(void * pvParameters);

#endif // BTN_H_
