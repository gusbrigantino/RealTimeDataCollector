//
// COMP/GENG 422 - Gus Brigantino
//
// Screen module header
//

#ifndef SCREEN_H_
#define SCREEN_H_

typedef struct _ScreenData
{
	uint16_t Prox;
	uint16_t Temp;
	uint16_t Hum;
	uint32_t dwellTime;
	uint32_t maxDwell;
	uint16_t numVisits;
	uint16_t screenState;

} ScreenDataType;


extern QueueHandle_t	ScreenQueue;
extern QueueHandle_t	GlobalScreenQueue;


void ScreenTask(void * pvParameters);

#endif // SCREEN_H_
