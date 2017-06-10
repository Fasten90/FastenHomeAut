/*
 *		TaskList.c
 *
 *		Created on:		2017. febr. 2.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. febr. 2.
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"

#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
#include "Logic.h"
#include "GlobalVarHandler.h"
#include "Button.h"
#include "LED.h"
#include "DateTime.h"
#include "CommandHandler.h"
#include "Terminal.h"
#include "Display.h"
#include "Motor.h"
#include "ESP8266.h"

#include "TaskList.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/


#ifdef CONFIG_MODULE_LED_ENABLE
static TaskResult_t TaskLedFunction(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_WATCHDOG_ENABLE
static TaskResult_t TaskWatchdogClear(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
static TaskResult_t TaskEsp8266(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_MOTOR_ENABLE
static TaskResult_t TaskMotor(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
static TaskResult_t Task_ProcessDebugUartCommandReceived(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_BUTTON_ENABLE
static TaskResult_t Task_ProcessButtonPressed(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_DISPLAY_ENABLE
static TaskResult_t Task_DisplayChangeImage(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE
static TaskResult_t Task_SystemTimeSecondStep(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_TASK_SOFTWARE_WATCHDOG_ENABLE
static TaskResult_t Task_SoftwareWatchDog(ScheduleSource_t source);
#endif
#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
static TaskResult_t Task_GlobalVarTrace(ScheduleSource_t source);
#endif

/// Tasks list
Task_t TaskList[] =
{
#ifdef CONFIG_MODULE_LED_ENABLE
	{
		.taskName = "LedTask",
		.taskFunction = TaskLedFunction,
	#ifdef LED_OLD_STYLE
		.taskScheduleRate = 200
	#else
		.taskScheduleRate = 2
	#endif
	},
#endif
#ifdef CONFIG_MODULE_WATCHDOG_ENABLE
	{
		.taskName = "WdtClr",
		.taskFunction = TaskWatchdogClear,
		.taskScheduleRate = 1000,
	},
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
	{
		.taskName = "Esp8266",
		.taskFunction = TaskEsp8266,
		.taskScheduleRate = 1000,
	},
#endif
#ifdef CONFIG_MODULE_MOTOR_ENABLE
	{
		.taskName = "MotorTask",
		.taskFunction = TaskMotor,
		.taskScheduleRate = 100
	},
#endif
#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
	{
		.taskName = "DebugUartReceivedACommand",
		.taskFunction = Task_ProcessDebugUartCommandReceived,
		//.isPeriodisScheduleDisabled = true,
		.taskScheduleRate = 50
	},
#endif
#ifdef CONFIG_MODULE_BUTTON_ENABLE
	{
		.taskName = "ButtonPressed",
		.taskFunction = Task_ProcessButtonPressed,
		.isPeriodisScheduleDisabled = true,
	},
#endif
#ifdef CONFIG_MODULE_DISPLAY_ENABLE
	{
		.taskName = "DisplayRefresh",
		.taskFunction = Task_DisplayChangeImage,
		.taskScheduleRate = 300
	},
#endif
#ifdef CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE
	{
		.taskName = "SystemTime",
		.taskFunction = Task_SystemTimeSecondStep,
		.taskScheduleRate = 1000
	},
#endif
#ifdef CONFIG_MODULE_TASK_SOFTWARE_WATCHDOG_ENABLE
	{
		.taskName = "SwWatchDog",
		.taskFunction = Task_SoftwareWatchDog,
		.taskScheduleRate = 1000,
		.isTimeOutTask = true
	},
#endif
#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
	{
		.taskName = "GlVarTrace",
		.taskFunction = Task_GlobalVarTrace,
		.taskScheduleRate = 1000,
	},
#endif


	// \note Be careful, taskList order need to be equal with TaskName_t

	// XXX: Add here new tasks

};



/// Tasks num
const TaskID_t TasksNum = (sizeof(TaskList)/sizeof(TaskList[0]));



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


// XXX: Function form:
//TaskResult_t (*TaskFunctionPointer)(TaskID_t id);



#ifdef CONFIG_MODULE_LED_ENABLE
/**
 * \brief
 */
static TaskResult_t TaskLedFunction(ScheduleSource_t source)
{
#ifdef CONFIG_TASKHANDLER_DEBUG_ENABLE
	uprintf("Run %s %d\r\n", TaskList[id].taskName, id);
#else
	(void)source;
#endif


#ifdef LED_OLD_STYLE
	static uint8_t Task_LedCnt = 0;

	LED_SetLed(LED_Green, LED_Set_Toggle);
	if ((Task_LedCnt % 5) == 0)
	{
		// *5
		LED_SetLed(LED_Blue, LED_Set_Toggle);
	}
	if ((Task_LedCnt % 25) == 0)
	{
		// * 5 * 5 (=*25)
		LED_SetLed(LED_Red, LED_Set_Toggle);
	}

	Task_LedCnt++;
	if (Task_LedCnt == 25)
	{
		Task_LedCnt = 0;
	}
#else

	// 50 Hz --> 20ms

	static uint8_t LED_PwmCnt = 0;
	static uint8_t LED_PwmLimit = 0;
	static const uint8_t LED_PwmMaxLimit = 10;
	static bool LED_PwmLimitDir = false;
	static uint8_t LED_100ms = 0;
	static uint8_t LED_2ms = 0;

	LED_2ms++;
	if (LED_2ms >= 100/2)
	{
		// Run every 100. ms

		LED_2ms = 0;
		LED_100ms++;

		// Change PWM percent

		if (LED_PwmLimitDir)
		{
			LED_PwmLimit--;
		}
		else
		{
			LED_PwmLimit++;
		}

		if (LED_100ms >= 10)
		{
			// Run every 1000. msec
			LED_100ms = 0;

			// Change dir after 1 sec
			if (LED_PwmLimit == 0)
			{
				LED_PwmLimitDir = false;
			}
			else
			{
				LED_PwmLimitDir = true;
			}
		}
	}

	// PWM limit: 0-10

	// Check, need LED blinking?
	if (LED_PwmCnt < LED_PwmLimit)
	{
		LED_SetLed(LED_GREEN_NUM, LED_SET_ON);
	}
	else
	{
		LED_SetLed(LED_GREEN_NUM, LED_SET_OFF);
	}

	// PWM counter
	LED_PwmCnt++;
	if (LED_PwmCnt >= LED_PwmMaxLimit)	// Max limit
	{
		LED_PwmCnt = 0;
	}

#endif

	return TASK_RESULT_OK;
}
#endif	// #ifdef CONFIG_MODULE_LED_ENABLE



#ifdef CONFIG_MODULE_WATCHDOG_ENABLE
/**
 * \brief	Watchdog clear task
 */
static TaskResult_t TaskWatchdogClear(ScheduleSource_t source)
{
	(void)source;

	Watchdog_Clear();

	return TASK_RESULT_OK;
}
#endif



#ifdef CONFIG_MODULE_ESP8266_ENABLE
static TaskResult_t TaskEsp8266(ScheduleSource_t source)
{
	(void)source;

	ESP8266_StatusMachine();

	return TASK_RESULT_OK;
}
#endif



#ifdef CONFIG_MODULE_MOTOR_ENABLE
static TaskResult_t TaskMotor(ScheduleSource_t source)
{
	(void)source;

	Motor_StatusMachine();

	return TASK_RESULT_OK;
}
#endif



#ifdef CONFIG_MODULE_DEBUGUSART_ENABLE
static TaskResult_t Task_ProcessDebugUartCommandReceived(ScheduleSource_t source)
{
	(void)source;

	Terminal_CheckCommand();

	return TASK_RESULT_OK;
}
#endif



#ifdef CONFIG_MODULE_BUTTON_ENABLE
static TaskResult_t Task_ProcessButtonPressed(ScheduleSource_t source)
{
	(void)source;
	bool canSleep = true;

	// Toggle LED
#ifdef CONFIG_MODULE_LED_ENABLE
	LED_SetLed(LED_Blue, LED_Set_Toggle);
	LED_SetLed(LED_Blue, LED_Set_Toggle);
#endif


#ifdef CONFIG_MODULE_BUTTON_LONG_PRESS
	static uint32_t BUTTON_PressTimeTick[BUTTON_NUM];
	static uint8_t BUTTON_PressedButtons;

	uint8_t i;
	for (i = 0; i < PressedButton_Count; i++)
	{
		if (BUTTON_Clicked & (1 << i))
		{
			// Button pressed "first"
			// Save: this button is pressed
			if (!(BUTTON_PressedButtons & (1 << i)))
			{
				BUTTON_PressedButtons |= (1 << i);
				BUTTON_PressTimeTick[i] = HAL_GetTick();
			}
			// Check the next time
			TaskHandler_SetTaskOnceRun(Task_ButtonPressed, 10);
			canSleep = false;
		}
		else if (BUTTON_PressedButtons & (1 << i))
		{
			// This button pressed at previous time
			// Check, this button is in pressing state?
			if (!BUTTON_GetButtonState(i))
			{
				// Not pressed = End of press
				BUTTON_PressedButtons &= ~(1 << i);

				// Handle button press
				uint32_t buttonPressTime = HAL_GetTick() - BUTTON_PressTimeTick[i];

				if (buttonPressTime > CONFIG_BUTTON_LONG_PRESS_TIME_TICK_LIMIT)
				{
					Logic_ButtonEventHandler(i, ButtonPress_Long);
				}
				else
				{
					Logic_ButtonEventHandler(i, ButtonPress_Short);
				}
				// Not need schedule ButtonTask at next time
				// canSleep = true; // Stay true
			}
			else
			{
				// Button in pressing, Check button at next time
				TaskHandler_SetTaskOnceRun(Task_ButtonPressed, 10);
				canSleep = false;
			}
		}
		else
		{
			// Not pressed at now and previous time!
			// canSleep = true; // Stay true
		}
	}

#else
	// Button long press not used
	Logic_ButtonEventHandler(i, ButtonPress_Short);
#endif

	// Clear flag
	BUTTON_Clicked = 0;

	if (canSleep)
	{
		// Button task can be sleep
		TaskHandler_DisableTask(Task_ButtonPressed);
	}

#ifdef CONFIG_MODULE_RASPBERRYPI_ENABLE
	// Test for Raspberry Pi
	RASPBERRYPI_SendMessage(1, Function_Alarm, Alarm_PressedButton , 17);
#endif

	return TASK_RESULT_OK;
}
#endif



#ifdef CONFIG_MODULE_DISPLAY_ENABLE
static TaskResult_t Task_DisplayChangeImage(ScheduleSource_t source)
{
	(void)source;

	#ifdef CONFIG_MODULE_DISPLAY_SHOW_SCREEN
	// Car image
	if (!Display_CarAnimationDisable_flag)
	{
		Display_ChangeCarImage();
	}


	// Loading image
	static uint8_t loadPercent = 0;

	Display_TestLoading(loadPercent++);

	if (loadPercent >= 100)
	{
		loadPercent = 0;
	}
	#endif

	return TASK_RESULT_OK;
}
#endif



#ifdef CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE
static TaskResult_t Task_SystemTimeSecondStep(ScheduleSource_t source)
{
	(void)source;

	// Step SystemTime +1 second
	DateTime_Step(&DateTime_SystemTime);

#ifdef CONFIG_MODULE_DISPLAY_SHOW_CLOCK
	// Display refresh
	Display_ShowClock(&DateTime_SystemTime.time);
#endif

#ifdef CONFIG_MODULE_TASK_SOFTWARE_WATCHDOG_ENABLE
	TaskHandler_ClearTimeoutTask(Task_SwWDT);
#endif

	return TASK_RESULT_OK;
}
#endif



#ifdef CONFIG_MODULE_TASK_SOFTWARE_WATCHDOG_ENABLE
static TaskResult_t Task_SoftwareWatchDog(ScheduleSource_t source)
{
	(void)source;

	// Software WatchDog - Timeout task
	// If This task is running, the system is lagging
	DebugUart_SendLine("Software WatchdDog Timeout! System is lagging!\r\n");

	return TASK_RESULT_OK;
}
#endif



#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
static TaskResult_t Task_GlobalVarTrace(ScheduleSource_t source)
{
	(void)source;

	// Software WatchDog - Timeout task
	// If This task is running, the system is lagging
	GlobalVarHandler_RunTrace();

	return TASK_RESULT_OK;
}
#endif



#endif //#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
