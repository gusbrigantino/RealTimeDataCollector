//
// COMP/GENG 422 - Gus Brigantino
//
// Screen module
//

#include "main.h"
#include "ctrl.h"
#include "disp.h"
#include "screen.h"
#include "comm.h"


static 	ScreenDataType		ScreenData;
static 	CommDataTypeArray	GlobalScreenDataArray;



static void ScreenIdleRender(void);

static void ScreenWelcomeRender(void);
static void ScreenWelcomeUpdate(void);

static void ScreenPresentRender(void);
static void ScreenPresentUpdate(void);

static void ScreenGlobalRender(void);
static void ScreenGlobalUpdate(void);

static void DeviceStatusColorSwitch(uint16_t status);


//for now all disppritf params are ints that I have decided on based on trail and error
//I would like to make this somewhat more parameterized in the future

void ScreenTask(void * pvParameters)
{
	ScreenIdleRender();

	uint16_t prevScreenState = DISP_SCREEN_IDLE;


	while(TRUE)
	{
		if (xQueueReceive(ScreenQueue, &ScreenData, QUEUE_RCV_WAIT_SCREEN) == pdTRUE)
		{
			if (ScreenData.screenState == prevScreenState)
			{
				//update screen state machine
				switch(ScreenData.screenState)
				{
				case DISP_SCREEN_IDLE:
					//no update for idle
					break;
				case DISP_SCREEN_WLCM:
					ScreenWelcomeUpdate();
					break;
				case DISP_SCREEN_PRES:
					ScreenPresentUpdate();
					break;
				case DISP_SCREEN_GLOBAL:
					if (xQueueReceive(GlobalScreenQueue, &GlobalScreenDataArray, QUEUE_RCV_WAIT_GLBL_SCRN) == pdTRUE)
					{
						ScreenGlobalUpdate();
					}
					break;
				}
			}
			else
			{
				//render new screen state machine
				switch(ScreenData.screenState)
				{
				case DISP_SCREEN_IDLE:
					DispForeColorSet(DISP_BLK);
					DispFilledRectRender(DISP_COL_MIN, DISP_ROW_MIN, DISP_RES_HOR, DISP_RES_VER);

					ScreenIdleRender();
					break;
				case DISP_SCREEN_WLCM:
					DispForeColorSet(DISP_BLK);
					DispFilledRectRender(DISP_COL_MIN, DISP_ROW_MIN, DISP_RES_HOR, DISP_RES_VER);

					ScreenWelcomeRender();
					break;
				case DISP_SCREEN_PRES:
					if(prevScreenState == DISP_SCREEN_GLOBAL)		//intentional to only render a small portion of text when going from welcome to present
					{
						DispForeColorSet(DISP_BLK);
						DispFilledRectRender(DISP_COL_MIN, DISP_ROW_MIN, DISP_RES_HOR, DISP_RES_VER);

						ScreenWelcomeRender();
					}
					ScreenPresentRender();
					break;
				case DISP_SCREEN_GLOBAL:
					DispForeColorSet(DISP_BLK);
					DispFilledRectRender(DISP_COL_MIN, DISP_ROW_MIN, DISP_RES_HOR, DISP_RES_VER);

					ScreenGlobalRender();
					break;
				}

				prevScreenState = ScreenData.screenState;
			}
		}
	}
}


static void ScreenIdleRender(void)
{
	// Demo code
	DispForeColorSet(DISP_RED_MAX);
	DispFontSet(DISP_FONT_24);
	DispPrintf(3, 10, "Big Brother is Watching You!");	// 28 x 17 = 476 pixels

	DispForeColorSet(DISP_RED_MAX);
	DispPrintf(230, 80, "Big");
	DispForeColorSet(DISP_WHT_MAX);
	DispPrintf(200, 120, "Brother");
	DispForeColorSet(DISP_BLU_MAX);
	DispPrintf(225, 160, "Logo");

	//American Flag
	DispFontSet(DISP_FONT_20);
	DispForeColorSet(DISP_BLU_MAX);
	DispPrintf(40, 220, "\n\n\n");
	DispPrintf(40, 240, "\n\n\n");
	DispForeColorSet(DISP_RED_MAX);
	DispPrintf(82, 220, "\n\n\n\n\n\n");
	DispForeColorSet(DISP_WHT_MAX);
	DispPrintf(82, 240, "\n\n\n\n\n\n");
	DispForeColorSet(DISP_RED_MAX);
	DispPrintf(40, 260, "\n\n\n\n\n\n\n\n\n");
	DispForeColorSet(DISP_WHT_MAX);
	DispPrintf(40, 280, "\n\n\n\n\n\n\n\n\n");


	DispForeColorSet(DISP_WHT_MAX);
	DispFontSet(DISP_FONT_20);
	DispPrintf(435, 300, "GUS");
}


static void ScreenWelcomeRender(void)
{
	// Demo code
	DispForeColorSet(DISP_YEL_MAX);
	DispFontSet(DISP_FONT_24);
	DispPrintf(2, 10, "Big Brother is Watching You!");	// 28 x 17 = 476 pixels
	DispForeColorSet(DISP_WHT_MAX);
	DispFontSet(DISP_FONT_20);
	DispPrintf(0, 45, "__________________________________"); // 34 chars x 14 pixels = 476

	DispForeColorSet(DISP_WHT_MAX);
	DispFontSet(DISP_FONT_20);
	DispPrintf(80, 40, "Welcome to Gus' Kiosk!");


	DispFontSet(DISP_FONT_20);

	DispForeColorSet(DISP_RED_MAX);
	DispPrintf(140, 80, "Prox:       mm");

	DispForeColorSet(DISP_WHT_MAX);
	DispPrintf(140, 120, "Temp:       C");

	DispForeColorSet(DISP_BLU_MAX);
	DispPrintf(154, 160, "Hum:       %%");

	DispForeColorSet(DISP_RED_MAX);
	DispPrintf(42, 200, "Dwell Time:       secs");

	DispForeColorSet(DISP_WHT_MAX);
	DispPrintf(56, 240, "Max Dwell:       secs");

	DispForeColorSet(DISP_BLU_MAX);
	if(ScreenData.numVisits == 0)
	{
		DispPrintf(28, 280, "# of Visits:       visit");
	}
	else
	{
		DispPrintf(28, 280, "# of Visits:       visits");
	}


	DispForeColorSet(DISP_WHT_MAX);
	DispFontSet(DISP_FONT_20);
	DispPrintf(435, 300, "GUS");
}

static void ScreenWelcomeUpdate(void)
{
	DispFontSet(DISP_FONT_20);
	DispForeColorSet(DISP_RED_MAX);
	DispPrintf(220, 80, "%4d", ScreenData.Prox);
	DispForeColorSet(DISP_WHT_MAX);
	DispPrintf(220, 120, "%4d", ScreenData.Temp);
	DispForeColorSet(DISP_BLU_MAX);
	DispPrintf(220, 160, "%4d", ScreenData.Hum);
	DispForeColorSet(DISP_RED_MAX);
	DispPrintf(220, 200, "%4d", (ScreenData.dwellTime / MS_TO_SECS));
	DispForeColorSet(DISP_WHT_MAX);
	DispPrintf(220, 240, "%4d", (ScreenData.maxDwell / MS_TO_SECS));
	DispForeColorSet(DISP_BLU_MAX);
	DispPrintf(220, 280, "%4d", ScreenData.numVisits);
}

static void ScreenPresentRender(void)
{
	DispForeColorSet(DISP_GRN_MAX);
	DispFontSet(DISP_FONT_24);
	DispPrintf(2, 10, "Big Brother is Watching You!");	// 28 x 17 = 476 pixels

	DispForeColorSet(DISP_WHT_MAX);
	DispFontSet(DISP_FONT_16);
	DispPrintf(10, 70, "<---");
	DispPrintf(10, 90, "Hold");
	DispPrintf(10, 110, "blue");
	DispPrintf(10, 130, "button");
	DispPrintf(10, 150, "for info");
}

static void ScreenPresentUpdate(void)
{
	DispFontSet(DISP_FONT_20);
	DispForeColorSet(DISP_RED_MAX);
	DispPrintf(220, 80, "%4d", ScreenData.Prox);
	DispForeColorSet(DISP_WHT_MAX);
	DispPrintf(220, 120, "%4d", ScreenData.Temp);
	DispForeColorSet(DISP_BLU_MAX);
	DispPrintf(220, 160, "%4d", ScreenData.Hum);
	DispForeColorSet(DISP_RED_MAX);
	DispPrintf(220, 200, "%4d", (ScreenData.dwellTime / MS_TO_SECS));
	DispForeColorSet(DISP_WHT_MAX);
	DispPrintf(220, 240, "%4d", (ScreenData.maxDwell / MS_TO_SECS));
	DispForeColorSet(DISP_BLU_MAX);
	DispPrintf(220, 280, "%4d", ScreenData.numVisits);
}

static void ScreenGlobalRender(void)
{
	DispForeColorSet(DISP_BLU_MAX);
	DispFontSet(DISP_FONT_24);
	DispPrintf(2, 10, "Big Brother is Watching You!");

	DispFontSet(DISP_FONT_16);
	DispForeColorSet(DISP_WHT_MAX);
	DispPrintf(2, 40, "Name  Status  Temp  Hum  CurDw  MaxDw  #Vis");


	//American Flag
	DispFontSet(DISP_FONT_20);
	DispForeColorSet(DISP_BLU_MAX);
	DispPrintf(167, 230, "\n\n\n");
	DispPrintf(167, 250, "\n\n\n");
	DispForeColorSet(DISP_RED_MAX);
	DispPrintf(209, 230, "\n\n\n\n\n\n");
	DispForeColorSet(DISP_WHT_MAX);
	DispPrintf(209, 250, "\n\n\n\n\n\n");
	DispForeColorSet(DISP_RED_MAX);
	DispPrintf(167, 270, "\n\n\n\n\n\n\n\n\n");
	DispForeColorSet(DISP_WHT_MAX);
	DispPrintf(167, 290, "\n\n\n\n\n\n\n\n\n");

	DispForeColorSet(DISP_WHT_MAX);
	DispFontSet(DISP_FONT_20);
	DispPrintf(435, 300, "GUS");
}


//for now only local machine data is fully functional
//current dwell time for the first three devices is being inc by a sec in the ctrl task
//it is never reset
static void ScreenGlobalUpdate(void)
{
	//name update
	DispFontSet(DISP_FONT_16);
	DispPrintf(2, 60, "%s", GlobalScreenDataArray.CD[0].Name);
	DispPrintf(2, 80, "%s", GlobalScreenDataArray.CD[1].Name);
	DispPrintf(2, 100, "%s", GlobalScreenDataArray.CD[2].Name);
	DispPrintf(2, 120, "%s", SELF_NAME_STR);
	DispPrintf(2, 140, "%s", GlobalScreenDataArray.CD[4].Name);
	DispPrintf(2, 160, "%s", GlobalScreenDataArray.CD[5].Name);
	DispPrintf(2, 180, "%s", GlobalScreenDataArray.CD[6].Name);
	DispPrintf(2, 200, "%s", GlobalScreenDataArray.CD[7].Name);

	//device status update
	DispFontSet(DISP_FONT_16);
	DeviceStatusColorSwitch(GlobalScreenDataArray.CD[0].DeviceStatus);
	DispPrintf(89, 60, "\n\n");				//new line to get block

	DeviceStatusColorSwitch(GlobalScreenDataArray.CD[1].DeviceStatus);
	DispPrintf(89, 80, "\n\n");

	DeviceStatusColorSwitch(GlobalScreenDataArray.CD[2].DeviceStatus);
	DispPrintf(89, 100, "\n\n");

	DeviceStatusColorSwitch(ScreenData.screenState);
	DispPrintf(89, 120, "\n\n");

	DeviceStatusColorSwitch(GlobalScreenDataArray.CD[4].DeviceStatus);
	DispPrintf(89, 140, "\n\n");

	DeviceStatusColorSwitch(GlobalScreenDataArray.CD[5].DeviceStatus);
	DispPrintf(89, 160, "\n\n");

	DeviceStatusColorSwitch(GlobalScreenDataArray.CD[6].DeviceStatus);
	DispPrintf(89, 180, "\n\n");

	DeviceStatusColorSwitch(GlobalScreenDataArray.CD[7].DeviceStatus);
	DispPrintf(89, 200, "\n\n");

	//device temp update
	DispForeColorSet(DISP_WHT_MAX);
	DispFontSet(DISP_FONT_16);
	DispPrintf(140, 60, "%4dC", GlobalScreenDataArray.CD[0].Temp);
	DispPrintf(140, 80, "%4dC", GlobalScreenDataArray.CD[1].Temp);
	DispPrintf(140, 100, "%4dC", GlobalScreenDataArray.CD[2].Temp);
	DispPrintf(140, 120, "%4dC", ScreenData.Temp);
	DispPrintf(140, 140, "%4dC", GlobalScreenDataArray.CD[4].Temp);
	DispPrintf(140, 160, "%4dC", GlobalScreenDataArray.CD[5].Temp);
	DispPrintf(140, 180, "%4dC", GlobalScreenDataArray.CD[6].Temp);
	DispPrintf(140, 200, "%4dC", GlobalScreenDataArray.CD[7].Temp);

	//device hum update
	DispForeColorSet(DISP_WHT_MAX);
	DispFontSet(DISP_FONT_16);
	DispPrintf(200, 60, "%4d%%", GlobalScreenDataArray.CD[0].Hum);
	DispPrintf(200, 80, "%4d%%", GlobalScreenDataArray.CD[1].Hum);
	DispPrintf(200, 100, "%4d%%", GlobalScreenDataArray.CD[2].Hum);
	DispPrintf(200, 120, "%4d%%", ScreenData.Hum);
	DispPrintf(200, 140, "%4d%%", GlobalScreenDataArray.CD[4].Hum);
	DispPrintf(200, 160, "%4d%%", GlobalScreenDataArray.CD[5].Hum);
	DispPrintf(200, 180, "%4d%%", GlobalScreenDataArray.CD[6].Hum);
	DispPrintf(200, 200, "%4d%%", GlobalScreenDataArray.CD[7].Hum);

	//device current dwell update
	DispForeColorSet(DISP_WHT_MAX);
	DispFontSet(DISP_FONT_16);
	DispPrintf(275, 60, "%4ds", (GlobalScreenDataArray.CD[0].CurrDwell / MS_TO_SECS));		//updating
	DispPrintf(275, 80, "%4ds", (GlobalScreenDataArray.CD[1].CurrDwell / MS_TO_SECS));		//updating
	DispPrintf(275, 100, "%4ds", (GlobalScreenDataArray.CD[2].CurrDwell / MS_TO_SECS));		//updating
	DispPrintf(275, 120, "%4ds", (ScreenData.dwellTime / MS_TO_SECS));						//updating
	DispPrintf(275, 140, "%4ds", (GlobalScreenDataArray.CD[4].CurrDwell / MS_TO_SECS));
	DispPrintf(275, 160, "%4ds", (GlobalScreenDataArray.CD[5].CurrDwell / MS_TO_SECS));
	DispPrintf(275, 180, "%4ds", (GlobalScreenDataArray.CD[6].CurrDwell / MS_TO_SECS));
	DispPrintf(275, 200, "%4ds", (GlobalScreenDataArray.CD[7].CurrDwell / MS_TO_SECS));

	//device max dwell update
	DispForeColorSet(DISP_WHT_MAX);
	DispFontSet(DISP_FONT_16);
	DispPrintf(353, 60, "%4ds", (GlobalScreenDataArray.CD[0].MaxDwell / MS_TO_SECS));
	DispPrintf(353, 80, "%4ds", (GlobalScreenDataArray.CD[1].MaxDwell / MS_TO_SECS));
	DispPrintf(353, 100, "%4ds", (GlobalScreenDataArray.CD[2].MaxDwell / MS_TO_SECS));
	DispPrintf(353, 120, "%4ds", (ScreenData.maxDwell / MS_TO_SECS));
	DispPrintf(353, 140, "%4ds", (GlobalScreenDataArray.CD[4].MaxDwell / MS_TO_SECS));
	DispPrintf(353, 160, "%4ds", (GlobalScreenDataArray.CD[5].MaxDwell / MS_TO_SECS));
	DispPrintf(353, 180, "%4ds", (GlobalScreenDataArray.CD[6].MaxDwell / MS_TO_SECS));
	DispPrintf(353, 200, "%4ds", (GlobalScreenDataArray.CD[7].MaxDwell / MS_TO_SECS));

	//device max dwell update
	DispForeColorSet(DISP_WHT_MAX);
	DispFontSet(DISP_FONT_16);
	DispPrintf(419, 60, "%4d", GlobalScreenDataArray.CD[0].NumVisits);
	DispPrintf(419, 80, "%4d", GlobalScreenDataArray.CD[1].NumVisits);
	DispPrintf(419, 100, "%4d", GlobalScreenDataArray.CD[2].NumVisits);
	DispPrintf(419, 120, "%4d", ScreenData.numVisits);
	DispPrintf(419, 140, "%4d", GlobalScreenDataArray.CD[4].NumVisits);
	DispPrintf(419, 160, "%4d", GlobalScreenDataArray.CD[5].NumVisits);
	DispPrintf(419, 180, "%4d", GlobalScreenDataArray.CD[6].NumVisits);
	DispPrintf(419, 200, "%4d", GlobalScreenDataArray.CD[7].NumVisits);
}

static void DeviceStatusColorSwitch(uint16_t status)
{
	switch(status)
	{
	case DISP_SCREEN_IDLE:
		DispForeColorSet(DISP_RED_MAX);
		break;
	case DISP_SCREEN_WLCM:
		DispForeColorSet(DISP_YEL_MAX);
		break;
	case DISP_SCREEN_PRES:
		DispForeColorSet(DISP_GRN_MAX);
		break;
	case DISP_SCREEN_GLOBAL:
		DispForeColorSet(DISP_BLU_MAX);
		break;
	default:
		DispForeColorSet(DISP_WHT_MAX);
		break;
	}
}

