//
// COMP/GENG 422 - Gus Brigantino
//
// Communication module
//

#include "main.h"
#include "ctrl.h"
#include "disp.h"
#include "comm.h"



void CommTask(void * pvParameters)
{
	CommDataTypeArray CommDataArray;
	CommDataType CommData;


	while (TRUE)
	{
		//for now statically fills out data for CommDataType and adds that to the array CommDataTypeArray
		//Then the entire array is sent via a queue to the ctrl task

		strcpy(CommData.Name, "Anas");
		CommData.DeviceStatus = DISP_SCREEN_IDLE;
		CommData.Temp = 17;
		CommData.Hum = 66;
		CommData.CurrDwell = 0;
		CommData.MaxDwell = 0;
		CommData.NumVisits = 0;

		CommDataArray.CD[0] = CommData;

		strcpy(CommData.Name, "Cal");
		CommData.DeviceStatus = DISP_SCREEN_WLCM;
		CommData.Temp = 17;
		CommData.Hum = 66;
		CommData.CurrDwell = 0;
		CommData.MaxDwell = 0;
		CommData.NumVisits = 0;

		CommDataArray.CD[1] = CommData;


		strcpy(CommData.Name, "Cecilia");
		CommData.DeviceStatus = DISP_SCREEN_PRES;
		CommData.Temp = 17;
		CommData.Hum = 66;
		CommData.CurrDwell = 0;
		CommData.MaxDwell = 0;
		CommData.NumVisits = 0;

		CommDataArray.CD[2] = CommData;


		strcpy(CommData.Name, "");
		CommData.DeviceStatus = DISP_SCREEN_INIT;
		CommData.Temp = 0;
		CommData.Hum = 0;
		CommData.CurrDwell = 0;
		CommData.MaxDwell = 0;
		CommData.NumVisits = 0;

		CommDataArray.CD[3] = CommData;	//this is array index for the local machine all info is handled by the screen data struct and queue

		strcpy(CommData.Name, "Jobe");
		CommData.DeviceStatus = DISP_SCREEN_IDLE;
		CommData.Temp = 17;
		CommData.Hum = 66;
		CommData.CurrDwell = 0;
		CommData.MaxDwell = 0;
		CommData.NumVisits = 0;

		CommDataArray.CD[4] = CommData;

		strcpy(CommData.Name, "Maddie");
		CommData.DeviceStatus = DISP_SCREEN_IDLE;
		CommData.Temp = 17;
		CommData.Hum = 66;
		CommData.CurrDwell = 0;
		CommData.MaxDwell = 0;
		CommData.NumVisits = 0;

		CommDataArray.CD[5] = CommData;

		strcpy(CommData.Name, "Tom");
		CommData.DeviceStatus = DISP_SCREEN_IDLE;
		CommData.Temp = 17;
		CommData.Hum = 66;
		CommData.CurrDwell = 0;
		CommData.MaxDwell = 0;
		CommData.NumVisits = 0;

		CommDataArray.CD[6] = CommData;

		strcpy(CommData.Name, "Trey");
		CommData.DeviceStatus = DISP_SCREEN_IDLE;
		CommData.Temp = 17;
		CommData.Hum = 66;
		CommData.CurrDwell = 0;
		CommData.MaxDwell = 0;
		CommData.NumVisits = 0;

		CommDataArray.CD[7] = CommData;

		xQueueSendToBack(CommQueue, &CommDataArray, QUEUE_SEND_WAIT_COMM);

		vTaskDelay(DLY_COMM_TASK);
	}
}
