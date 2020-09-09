//
// COMP/GENG 422 - Tom Lupfer
//
// LED module
//

#include "main.h"
#include "led.h"


void LedInit(void)
{
	BSP_LED_Init(LED2);
}


void LedSetOff(void)
{
	BSP_LED_Off(LED2);
}


void LedSetOn(void)
{
	BSP_LED_On(LED2);
}
