//
// COMP/GENG 422 - Gus Brigantino
//
// Control module
//

#include "main.h"
#include "btn.h"
#include "ctrl.h"
#include "disp.h"
#include "led.h"
#include "screen.h"
#include "sensor.h"
#include "comm.h"
#include "eprintf.h"


// Intertask communications handles
QueueHandle_t 	BtnQueue;
QueueHandle_t	ScreenQueue;
QueueHandle_t	GlobalScreenQueue;
QueueHandle_t	SensorQueue;
QueueHandle_t	CommQueue;


// Main control task
void CtrlTask(void *pvParameters)
{
	int				BtnState;
	int 			numVisits = 0;
	int				maxDwell = 0;
	int 			dwellTime;

	ScreenDataType		ScreenData;
	SensorDataType		SensorData;
	CommDataTypeArray	CommDataArray;

	ScreenData.maxDwell = maxDwell;
	ScreenData.numVisits = numVisits;

	int wlcmTime = WLCM_SCREEN_DELAY;
	int presentTime = PRES_SCREEN_DELAY;
	int idleTime = IDLE_SCREEN_DELAY;

	ScreenData.screenState = DISP_SCREEN_INIT;


	// Initialize the peripherals
	BtnInit();
	DispInit();
	LedInit();
	SensorInit();

	// Create intertask communications
	BtnQueue = xQueueCreate(QUEUE_LEN_BTN, sizeof(BtnState));
	ScreenQueue = xQueueCreate(QUEUE_LEN_SCREEN, sizeof(ScreenDataType));
	GlobalScreenQueue = xQueueCreate(QUEUE_LEN_GLBL_SCRN, sizeof(CommDataTypeArray));
	SensorQueue = xQueueCreate(QUEUE_LEN_SENSOR, sizeof(SensorDataType));
	CommQueue = xQueueCreate(QUEUE_LEN_COMM, sizeof(CommDataTypeArray));

	// Create the button press task
	xTaskCreate(BtnTask,				// Task function
				"ButtonPressTask",		// Task name, for debugging
				STACK_BTN_TASK,			// Stack size
				NULL,					// Task parameter
				PRI_BTN_TASK,			// Task priority
				NULL);					// Task handle

	// Create the screen task
	xTaskCreate(ScreenTask,			// Task function
				"ScreenTask",		// Task name, for debugging
				STACK_SCREEN_TASK,	// Stack size
				NULL,				// Task parameter
				PRI_SCREEN_TASK,	// Task priority
				NULL);				// Task handle

	// Create the sensor task
	xTaskCreate(SensorTask,			// Task function
				"SensorTask",		// Task name, for debugging
				STACK_SENSOR_TASK,	// Stack size
				NULL,				// Task parameter
				PRI_SENSOR_TASK,	// Task priority
				NULL);				// Task handle

	// Create the communication task
	xTaskCreate(CommTask,			// Task function
				"CommTask",			// Task name, for debugging
				STACK_COMM_TASK,	// Stack size
				NULL,				// Task parameter
				PRI_COMM_TASK,		// Task priority
				NULL);				// Task handle


	int curr = 0;
	// Main processing loop
	while (TRUE)
	{

		// Check for sensor updates
		if (xQueueReceive(SensorQueue, &SensorData, QUEUE_RCV_WAIT_SENSOR) == pdTRUE)
		{
			ScreenData.Prox = SensorData.Prox;
			ScreenData.Temp = SensorData.Temp;
			ScreenData.Hum = SensorData.Hum;

			xQueueSendToBack(ScreenQueue, &ScreenData, QUEUE_SEND_WAIT_SCREEN);
		}


		switch(ScreenData.screenState)
		{
		case DISP_SCREEN_INIT:

			ScreenData.screenState = DISP_SCREEN_IDLE; 			//begin display at idle screen

			break;
		case DISP_SCREEN_IDLE:									//to get here from any other screen prox > 500mm for ?? unspecified 1s

			dwellTime = 0;										//set dwell time to 0 at idle
			ScreenData.dwellTime = dwellTime;

			if(SensorData.Prox <= PROX_PRESENT_MAX)
			{
				idleTime = IDLE_SCREEN_DELAY;					//resets idle time to decrement when in prox

				BSP_LED_On(LED2);								//turns on led when in prox

				if(wlcmTime == SCREEN_DELAY_END)				//var wlcmTime will decrement by the ctrl task delay until == 0
				{
					wlcmTime = WLCM_SCREEN_DELAY;
					ScreenData.screenState = DISP_SCREEN_WLCM;
				}
				else
				{
					wlcmTime = wlcmTime - DLY_CTRL_TASK;
				}
			}
			else
			{
				BSP_LED_Off(LED2);								//turn off led when not in prox
				wlcmTime = WLCM_SCREEN_DELAY;					//resets idle to welcome delay
			}

			break;
		case DISP_SCREEN_WLCM:									//to get here from idle prox <= 500mm for >= 2s

			if(SensorData.Prox <= PROX_PRESENT_MAX)
			{
				idleTime = IDLE_SCREEN_DELAY;					//resets welcome to idle delay

				BSP_LED_On(LED2);								//turns on led when in prox

				if(presentTime == SCREEN_DELAY_END)
				{
					presentTime = PRES_SCREEN_DELAY;			//resets welcome to present
					idleTime = IDLE_SCREEN_DELAY;				//resets welcome to idle

					numVisits++;								//inc num visits when transition to present state
					ScreenData.numVisits = numVisits;

					xQueueReset(BtnQueue);

					ScreenData.screenState = DISP_SCREEN_PRES;
				}
				else
				{
					presentTime = presentTime - DLY_CTRL_TASK;	//dec welcome to present delay
				}
			}
			else
			{
				BSP_LED_Off(LED2);								//turn off led when not in prox
				if(idleTime == SCREEN_DELAY_END)
				{
					presentTime = PRES_SCREEN_DELAY;			//resets welcome to present
					idleTime = IDLE_SCREEN_DELAY;				//resets welcome to idle
					ScreenData.screenState = DISP_SCREEN_IDLE;
				}
				else
				{
					idleTime = idleTime - DLY_CTRL_TASK;		//dec welcome to idle delay
				}
			}
			break;
		case DISP_SCREEN_PRES:									//to get here from welcome prox <= 500mm for an additional three seconds making it >= 5secs

			if(SensorData.Prox <= PROX_PRESENT_MAX)
			{
				idleTime = IDLE_SCREEN_DELAY;					//resets present to idle delay

				BSP_LED_On(LED2);								//turns on led when in prox

				dwellTime += DLY_CTRL_TASK;						//incs dwell time while in prox by crtl task delay

				if(maxDwell < dwellTime)						//handles new max dwell when curr dwell is > curr max dwell
				{
					maxDwell = dwellTime;
					ScreenData.maxDwell = maxDwell;
				}

				ScreenData.dwellTime = dwellTime;

				if(xQueueReceive(BtnQueue, &BtnState, QUEUE_RCV_WAIT_BTN))
				{
					if (BtnState == BTN_IS_PRESSED)					//checks for button press to transition to global screen
					{
						idleTime = IDLE_SCREEN_DELAY;				//resets present to idle delay
						ScreenData.screenState = DISP_SCREEN_GLOBAL;
					}
				}
			}
			else
			{
				BSP_LED_Off(LED2);								//turns off led when not in prox

				if(idleTime == SCREEN_DELAY_END)
				{
					idleTime = IDLE_SCREEN_DELAY;				//resets present to idle delay
					ScreenData.screenState = DISP_SCREEN_IDLE;
				}
				else
				{
					idleTime = idleTime - DLY_CTRL_TASK;		//dec present to idle delay
				}
			}
			break;
		case DISP_SCREEN_GLOBAL:								//to get here from present push button must be held down

			if(SensorData.Prox <= PROX_PRESENT_MAX)				//prox is checked first then button is checked, meaning if button is held and prox > max -> idle screen
			{
				idleTime = IDLE_SCREEN_DELAY;					//resets global to idle delay

				BSP_LED_On(LED2);								//turns on led when in prox

				dwellTime += DLY_CTRL_TASK;						//incs dwell time while in prox by crtl task delay

				if(maxDwell < dwellTime)						//handles new max dwell when curr dwell is > curr max dwell
				{
					maxDwell = dwellTime;
					ScreenData.maxDwell = maxDwell;
				}

				ScreenData.dwellTime = dwellTime;

				if (xQueueReceive(CommQueue, &CommDataArray, QUEUE_RCV_WAIT_COMM) == pdTRUE)
				{
					curr += 500;

					CommDataArray.CD[0].CurrDwell = curr;		//the current dwell time with continually inc by 500 ms, no reset when change to any state
					CommDataArray.CD[1].CurrDwell = curr;		//just to show the data in global screen is dynamic
					CommDataArray.CD[2].CurrDwell = curr;		//*May not increment correctly

					xQueueSendToBack(GlobalScreenQueue, &CommDataArray, QUEUE_SEND_WAIT_SCREEN);
				}

				if(xQueueReceive(BtnQueue, &BtnState, QUEUE_RCV_WAIT_BTN))
				{
					if (BtnState == BTN_IS_RELEASED)				//checks for button release to transition back to present screen
					{
						idleTime = IDLE_SCREEN_DELAY;
						ScreenData.screenState = DISP_SCREEN_PRES;
					}
				}
			}
			else
			{
				BSP_LED_Off(LED2);								//turns off led when not in prox

				if(idleTime == SCREEN_DELAY_END)
				{
					idleTime = IDLE_SCREEN_DELAY;				//resets global to idle delay
					ScreenData.screenState = DISP_SCREEN_IDLE;
				}
				else
				{
					idleTime = idleTime - DLY_CTRL_TASK;		//dec present to idle delay
				}
			}

			break;
		}
		vTaskDelay(DLY_CTRL_TASK);
	}
}
