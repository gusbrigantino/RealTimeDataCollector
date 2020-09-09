//
// COMP/GENG 422 - Gus Brigantino
//
// Session 24 - DispPrintf(), Control Task, Demo Code
//

#include "main.h"
#include "clock.h"
#include "ctrl.h"
#include "uart.h"


int main(void)
{
	// Initialize the system hardware
	HAL_Init();
	ClockInit();
	UartInit();

	// Create the control task
	xTaskCreate(CtrlTask,			// Task function
				"ControlTask",		// Task name, for debugging
				STACK_CTRL_TASK,	// Stack size
				NULL,				// Task parameter
				PRI_CTRL_TASK,		// Task priority
				NULL);				// Task handle

	// Start the scheduler, which never exits
	vTaskStartScheduler();
}
