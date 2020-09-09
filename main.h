//
// COMP/GENG 422 - Gus Brigantino
//
// General header file
//

#ifndef MAIN_H_
#define MAIN_H_

#include "stm32l4xx_hal.h"
#include "stm32l475e_iot01.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_tsensor.h"

#include "vl53l0x_def.h"
#include "vl53l0x_api.h"
#include "vl53l0x_tof.h"

#include "freertos.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"


#define FALSE				0
#define TRUE				1


typedef int		Bool;	// Boolean

#endif // MAIN_H_
