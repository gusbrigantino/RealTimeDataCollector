//
// COMP/GENG 422 - Gus Brigantino
//
// Communications module header
//

#ifndef COMM_H_
#define COMM_H_

//struct for the data that will eventually come from the cloud
typedef struct _CommData
{
	char 	 Name[10];			//change to define name max
	uint16_t DeviceStatus;		//prox but based on the state of the screen
	uint16_t Temp;
	uint16_t Hum;
	uint32_t CurrDwell;
	uint32_t MaxDwell;
	uint16_t NumVisits;
} CommDataType;


/*a struct that contains an array of CommDataType
I realize this is not the optimal way to handle data coming in from the
cloud because the devices will not send their data all at the exact same time
but for now this provides a very simple way of transfering large amounts of
data via queue from the comm task to the ctrl task and then from the ctrl task to the screen task.
I will change it so that a single queue accepts the commData type and can hold a larger amount of
items than normal, so that the task can dynamically receive and display data of each device when available.
*/
typedef struct _CommDataArray
{
	CommDataType CD[8];			//change to define device max
} CommDataTypeArray;


extern QueueHandle_t	CommQueue;


void CommTask(void * pvParameters);

#endif // COMM_H_
