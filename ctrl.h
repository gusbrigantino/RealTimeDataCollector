//
// COMP/GENG 422 - Gus Brigantino
//
// Control module header
//

#ifndef CTRL_H_
#define CTRL_H_

#define COUNT_BTN_DEBOUNCE		3

#define DLY_BTN_TASK			25
#define DLY_CTRL_TASK			50
#define DLY_SENSOR_TASK			500
#define DLY_COMM_TASK			500

#define PRI_CTRL_TASK			5
#define PRI_BTN_TASK			4
#define PRI_SCREEN_TASK			2
#define PRI_SENSOR_TASK			3
#define PRI_COMM_TASK			3

#define QUEUE_LEN_BTN				10
#define QUEUE_LEN_SCREEN			10
#define QUEUE_LEN_SENSOR			10
#define QUEUE_LEN_COMM				10
#define QUEUE_LEN_GLBL_SCRN			10

#define QUEUE_RCV_WAIT_BTN			0
#define QUEUE_RCV_WAIT_SCREEN		portMAX_DELAY
#define QUEUE_RCV_WAIT_SENSOR		0
#define QUEUE_RCV_WAIT_COMM			0
#define QUEUE_RCV_WAIT_GLBL_SCRN	0

#define QUEUE_SEND_WAIT_BTN			0
#define QUEUE_SEND_WAIT_SCREEN		0
#define QUEUE_SEND_WAIT_SENSOR		0
#define QUEUE_SEND_WAIT_COMM		0
#define QUEUE_SEND_WAIT_GLBL_SCRN	0

#define STACK_CTRL_TASK				1000
#define STACK_BTN_TASK				800
#define STACK_SCREEN_TASK			800
#define STACK_SENSOR_TASK			800
#define STACK_COMM_TASK				800

#define DISP_SCREEN_INIT			0
#define DISP_SCREEN_IDLE			1
#define DISP_SCREEN_WLCM			2
#define DISP_SCREEN_PRES			3
#define DISP_SCREEN_GLOBAL			4

#define SELF_NAME_STR				"Gus"

#define WLCM_SCREEN_DELAY			2000	//2s
#define PRES_SCREEN_DELAY			3000	//3s
#define	IDLE_SCREEN_DELAY			2000	//2s
#define SCREEN_DELAY_END			0		//0s

#define PROX_PRESENT_MAX			500		//.5m

#define MS_TO_SECS					1000	//conversion rate


extern QueueHandle_t	BtnQueue;
extern QueueHandle_t	ScreenQueue;
extern QueueHandle_t	GlobalScreenQueue;
extern QueueHandle_t	SensorQueue;
extern QueueHandle_t	CommQueue;


void CtrlTask(void * pvParameters);

#endif // CTRL_H_
