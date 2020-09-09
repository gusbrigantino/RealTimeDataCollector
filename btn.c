//
// COMP/GENG 422 - Gus Brigantino
//
// Button module
//

#include "main.h"
#include "btn.h"
#include "ctrl.h"


#define BTN_ST_RELEASED			0
#define BTN_ST_PRESSED			1
#define BTN_ST_DEBOUNCE			2


void BtnInit(void)
{
	BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);
}


// Task to debounce the pushbutton and signal a press
void BtnTask(void * pvParameters)
{
	static int BtnDebounceCnt;
	static int BtnState = BTN_ST_RELEASED;

	while (TRUE)
	{
		switch (BtnState)
		{
		case BTN_ST_RELEASED:
			if (!BSP_PB_GetState(BUTTON_USER))		// pushbutton is pressed
			{
				BtnDebounceCnt = COUNT_BTN_DEBOUNCE;
				BtnState = BTN_ST_DEBOUNCE;
			}
			break;

		case BTN_ST_PRESSED:
			if (BSP_PB_GetState(BUTTON_USER))		// pushbutton is released
			{
				BtnDebounceCnt = COUNT_BTN_DEBOUNCE;
				BtnState = BTN_ST_DEBOUNCE;
			}
			break;

		case BTN_ST_DEBOUNCE:
			if (--BtnDebounceCnt >= 0)				// wait debounce period
			{
				break;
			}
			if (!BSP_PB_GetState(BUTTON_USER))		// pushbutton is pressed
			{
				// Set pressed button state, put in queue
				BtnState = BTN_ST_PRESSED;
				xQueueSendToBack(BtnQueue, &BtnState, QUEUE_SEND_WAIT_BTN);
			}
			else									// pushbutton is released
			{
				// Set released button state, put in queue
				BtnState = BTN_ST_RELEASED;
				xQueueSendToBack(BtnQueue, &BtnState, QUEUE_SEND_WAIT_BTN);
			}
			break;
		}

		vTaskDelay(DLY_BTN_TASK);
	}
}
