//
// COMP/GENG 422 - Gus Brigantino
//
// Sensor module header
//

#ifndef SENSOR_H_
#define SENSOR_H_

typedef struct _SensorData
{
	uint16_t Prox;
	uint16_t Temp;
	uint16_t Hum;
} SensorDataType;


extern QueueHandle_t	SensorQueue;


void SensorInit(void);
void SensorTask(void * pvParameters);

#endif // SENSOR_H_
