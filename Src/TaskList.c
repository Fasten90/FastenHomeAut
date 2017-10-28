/*
 *		TaskList.c
 *		Created on:		2017-02-02
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Task list
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-02-02
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
#include "Display.h"
#include "CommonADC.h"
#include "ADC.h"
#include "EventHandler.h"
#include "HomeAutMessage.h"
#ifdef CONFIG_FUNCTION_GAME_SNAKE
#include "Snake.h"
#endif

#include "TaskList.h"



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/


#ifdef CONFIG_MODULE_LED_ENABLE
static TaskResult_t Task_LedBlink(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_WATCHDOG_ENABLE
static TaskResult_t TaskWatchdogClear(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_ESP8266_ENABLE
static TaskResult_t TaskEsp8266(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_MOTOR_ENABLE
static TaskResult_t Task_Motor(ScheduleSource_t source);
static TaskResult_t Task_MotorConnStop(ScheduleSource_t source);
#endif
#ifdef CONFIG_FUNCTION_REMOTECONTROLLER
static TaskResult_t Task_RemoteControllerSending(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
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
#ifdef CONFIG_FUNCTION_CHARGER
static TaskResult_t Task_InputOutput(ScheduleSource_t source);
#endif
#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
static TaskResult_t Task_CommonAdcFunction(ScheduleSource_t source);
#endif
#ifdef CONFIG_DEBUG_SELFTEST
static TaskResult_t Task_SelfTestFunction(ScheduleSource_t source);
#endif


///< Tasks list
Task_t TaskList[] =
{
#ifdef CONFIG_MODULE_LED_ENABLE
	{
		.taskName = "LedTask",
		.taskFunction = Task_LedBlink,
	#ifdef LED_OLD_STYLE
		.taskScheduleRate = 200,
	#else
		.taskScheduleRate = 2,
	#endif
	#ifdef CONFIG_MODULE_TASKHANDLER_STATISTICS
		// Do not log LED task, not important and run short
		.isDisableLogToStatistics = true,
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
		.taskFunction = Task_Motor,
		.taskScheduleRate = 50,
	},
	{
		.taskName = "MotorConnStopTimeout",
		.taskFunction = Task_MotorConnStop,
		.taskScheduleRate = 500,
		.isTimeOutTask = true,
	},
#endif
#ifdef CONFIG_FUNCTION_REMOTECONTROLLER
	{
		.taskName = "RemoteController",
		.taskFunction = Task_RemoteControllerSending,
		.taskScheduleRate = 300,
	},
#endif
#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
	{
		.taskName = "DbgUartCommandRecv",
		.taskFunction = Task_ProcessDebugUartCommandReceived,
		//.isPeriodisScheduleDisabled = true,
		.taskScheduleRate = 50,
	},
#endif
#ifdef CONFIG_MODULE_BUTTON_ENABLE
	{
		.taskName = "ButtonPressed",
		.taskFunction = Task_ProcessButtonPressed,
		.isPeriodisScheduleDisabled = true,
		.taskScheduleRate = 10,
	},
#endif
#ifdef CONFIG_MODULE_DISPLAY_ENABLE
	{
		.taskName = "DisplayRefresh",
		.taskFunction = Task_DisplayChangeImage,
		.taskScheduleRate = 300,
	},
#endif
#ifdef CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE
	{
		.taskName = "SystemTime",
		.taskFunction = Task_SystemTimeSecondStep,
		.taskScheduleRate = 1000,
	},
#endif
#ifdef CONFIG_MODULE_TASK_SOFTWARE_WATCHDOG_ENABLE
	{
		.taskName = "SwWatchDog",
		.taskFunction = Task_SoftwareWatchDog,
		.taskScheduleRate = 1000,
		.isTimeOutTask = true,
	},
#endif
#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
	{
		.taskName = "GlVarTrace",
		.taskFunction = Task_GlobalVarTrace,
		.taskScheduleRate = 1000,
	},
#endif
#ifdef CONFIG_FUNCTION_CHARGER
	{
		.taskName = "IOtask",
		.taskFunction = Task_InputOutput,
		.taskScheduleRate = 1000,
	},
#endif
#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
	{
		.taskName = "CommonAdc",
		.taskFunction = Task_CommonAdcFunction,
		.taskScheduleRate = 1000,
		.isDisabled = true,
	},
#endif
#ifdef CONFIG_DEBUG_SELFTEST
	{
		.taskName ="SelfTest",
		.taskFunction = Task_SelfTestFunction,
		.taskScheduleRate = 1000,
		.isRunOnce = true,
	},
#endif


	// XXX: Add here new tasks
	// \note Be careful, taskList order need to be equal with TaskName_t
	/*
	const char *taskName;						///< Task Name - Init
	const TaskFunctionPointer taskFunction;		///< Task function - Init
	TaskTick_t taskScheduleRate;				///< Task scheduling rate [ms] - Init/Runtime
	bool isRequestScheduling;					///< Task scheduling request (true, if request) - Runtime
	bool isPeriodisScheduleDisabled;			///< Task schedule (periodic) disabled - Init/Runtime
	bool isRunOnce;								///< Task scheduling once - Init/Runtime
	bool isTimeOutTask;							///< Task is work in TimeOut mode - Init/Runtime
	bool isDisabled;							///< Task is disabled/enabled - Init/Runtime

	!! Do not use other fields !!
	*/
};



///< Tasks num
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
//typedef TaskResult_t (*TaskFunctionPointer)(ScheduleSource_t source);
// static TaskResult_t Task_Name(ScheduleSource_t source);



#ifdef CONFIG_MODULE_LED_ENABLE
/**
 * \brief	LED blinking task
 */
static TaskResult_t Task_LedBlink(ScheduleSource_t source)
{
#ifdef CONFIG_TASKHANDLER_DEBUG_ENABLE
	uprintf("Run %s %d\r\n", TaskList[id].taskName, id);
#else
	(void)source;
#endif


#ifdef CONFIG_MODULE_ESP8266_ENABLE
	// ESP8266 connection status LED

	static uint8_t Task_LedCnt = 0;
	// Blue led toggle every time
	LED_SetLed(LED_Blue, LED_Set_Toggle);

	if (((Task_LedCnt % 2) == 0) && (ESP8266_ConnectionStatus == ESP8266_WifiConnectionStatus_SuccessfulServerStarted))
	{
		// *2 - fast, if has connection
		LED_SetLed(LED_Green, LED_Set_Toggle);
	}
	if (((Task_LedCnt % 5) == 0) && (ESP8266_ConnectionStatus != ESP8266_WifiConnectionStatus_SuccessfulServerStarted))
	{
		// *5 (slow - if there is no connection
		LED_SetLed(LED_Green, LED_Set_Toggle);
	}

	Task_LedCnt++;
	if (Task_LedCnt == 5)
	{
		Task_LedCnt = 0;
	}
#elif defined(LED_OLD_STYLE)
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
#elif defined(LED_PWM_STYLE)

	// 50 Hz --> 20ms

	static uint8_t LED_PwmCnt = 0;
	static uint8_t LED_PwmLimit = 0;
	static const uint8_t LED_PwmMaxLimit = 10;
	static bool LED_PwmLimitDir = false;
	static uint8_t LED_100ms = 0;
	static uint8_t LED_2ms = 0;
	static const uint8_t LED_PWM_ChangeDir_100ms_limit = 10;

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

		// Change direction
		if (LED_100ms >= LED_PWM_ChangeDir_100ms_limit)
		{
			// Run every 1000. msec = every sec
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
		LED_SetLed(LED_Blue, LED_Set_On);
		//LED_SetLed(LED_Blue, LED_Set_Toggle);
	}
	else
	{
		LED_SetLed(LED_Blue, LED_Set_Off);
	}

	// PWM counter
	LED_PwmCnt++;
	if (LED_PwmCnt >= LED_PwmMaxLimit)	// Max limit
	{
		LED_PwmCnt = 0;
	}

#endif
#ifdef CONFIG_FUNCTION_REMOTECONTROLLER_CAR
	// Turn off LEDs
	LED_SetLed(LED_Green, LED_Set_Off);
#endif

	return TaskResult_Ok;
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

	return TaskResult_Ok;
}
#endif



#ifdef CONFIG_MODULE_ESP8266_ENABLE
static TaskResult_t TaskEsp8266(ScheduleSource_t source)
{
	(void)source;

	ESP8266_StatusMachine();

	return TaskResult_Ok;
}
#endif



#ifdef CONFIG_MODULE_MOTOR_ENABLE
static TaskResult_t Task_Motor(ScheduleSource_t source)
{
	(void)source;

	Motor_StateMachine();

	return TaskResult_Ok;
}



static TaskResult_t Task_MotorConnStop(ScheduleSource_t source)
{
	(void)source;

	Motor_ControlStop();

	return TaskResult_Ok;
}
#endif



#ifdef CONFIG_FUNCTION_REMOTECONTROLLER
static TaskResult_t Task_RemoteControllerSending(ScheduleSource_t source)
{
	(void)source;

	Logic_RemoteController_SendMessage();

	return TaskResult_Ok;
}
#endif



#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
static TaskResult_t Task_ProcessDebugUartCommandReceived(ScheduleSource_t source)
{
	(void)source;

#ifdef CONFIG_MODULE_TERMINAL_ENABLE
	Terminal_CheckCommand();
#else
	DebugUart_ProcessReceivedCharacters();
#endif

	return TaskResult_Ok;
}
#endif



#ifdef CONFIG_MODULE_BUTTON_ENABLE
static TaskResult_t Task_ProcessButtonPressed(ScheduleSource_t source)
{
	(void)source;
	bool canSleep = true;

	// Toggle LED
#ifdef CONFIG_MODULE_LED_ENABLE
	LED_SetLed(LED_Green, LED_Set_Toggle);
	LED_SetLed(LED_Green, LED_Set_Toggle);
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
					// Long button, but need to check, this is an Continuous button pressing?
					if (buttonPressTime > CONFIG_BUTTON_CONTINUOUS_PRESS_TIME_TICK_LIMIT)
					{
						// Continuous button release, do not run ButtonHandler
						Logic_ButtonEventHandler(i, ButtonPress_ReleasedContinuous);
						EventHandler_GenerateEvent(Event_ButtonPressed, (i<<8 | ButtonPress_ReleasedContinuous), Task_ButtonPressed);
					}
					else
					{
						// Released "long" button pressing
						Logic_ButtonEventHandler(i, ButtonPress_Long);
						EventHandler_GenerateEvent(Event_ButtonPressed, (i<<8 | ButtonPress_Long), Task_ButtonPressed);
					}
				}
				else
				{
					Logic_ButtonEventHandler(i, ButtonPress_Short);
					EventHandler_GenerateEvent(Event_ButtonPressed, (i<<8 | ButtonPress_Short), Task_ButtonPressed);
				}
				// Not need schedule ButtonTask at next time
				// canSleep = true; // Stay true
			}
			else
			{
				// Button in pressing,
				uint32_t buttonPressTime = HAL_GetTick() - BUTTON_PressTimeTick[i];

				if (buttonPressTime > CONFIG_BUTTON_CONTINUOUS_PRESS_TIME_TICK_LIMIT)
				{
					// Continuous stepping
					Logic_ButtonEventHandler(i, ButtonPress_Continuous);
					EventHandler_GenerateEvent(Event_ButtonPressed, (i<<8 | ButtonPress_Continuous), Task_ButtonPressed);
					TaskHandler_SetTaskOnceRun(Task_ButtonPressed, 100);
					canSleep = false;
				}
				else
				{
					// Check button at next time, for know: continuous button pressing or long/short?
					TaskHandler_SetTaskOnceRun(Task_ButtonPressed, 10);
					canSleep = false;
				}
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

#ifdef CONFIG_MODULE_HOMEAUTMESSAGE_ENABLE
	// Test for HomeAutMessage
	HomeAutMessage_SendMessage(1, Function_Alarm, Alarm_PressedButton, 17);
#endif

	return TaskResult_Ok;
}
#endif



#ifdef CONFIG_MODULE_DISPLAY_ENABLE
static TaskResult_t Task_DisplayChangeImage(ScheduleSource_t source)
{
	(void)source;

	Logic_DisplayHandler(source);

	return TaskResult_Ok;
}
#endif



#ifdef CONFIG_MODULE_TASK_SYSTEMTIME_ENABLE
static TaskResult_t Task_SystemTimeSecondStep(ScheduleSource_t source)
{
	// TODO: If not called by fix 1000ms, it is not accurate
	// Idea: give scheduling ms by parameter
	(void)source;

	// Step SystemTime +1 second
	DateTime_Step(&DateTime_SystemTime);


	#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_CLOCK
	// Display refresh
		#if defined(CONFIG_DISPLAY_CLOCK_LARGE)
		if (DateTime_SystemTime.time.second == 0)
			TaskHandler_RequestTaskScheduling(Task_Display);
		#endif
		#if defined(CONFIG_DISPLAY_CLOCK_SMALL)
		if (Logic_Display_GetClockIsNeedRefresh())
			TaskHandler_RequestTaskScheduling(Task_Display);
		#endif
	#endif


	#ifdef CONFIG_MODULE_TASK_SOFTWARE_WATCHDOG_ENABLE
	TaskHandler_ClearTimeoutTask(Task_SwWDT);
	#endif

	return TaskResult_Ok;
}
#endif



#ifdef CONFIG_MODULE_TASK_SOFTWARE_WATCHDOG_ENABLE
static TaskResult_t Task_SoftwareWatchDog(ScheduleSource_t source)
{
	(void)source;

	// Software WatchDog - Timeout task
	// If This task is running, the system is lagging
	DebugUart_SendLine("Software WatchdDog Timeout! System is lagging!\r\n");

	return TaskResult_Ok;
}
#endif



#ifdef CONFIG_GLOBALVARHANDLER_TRACE_ENABLE
static TaskResult_t Task_GlobalVarTrace(ScheduleSource_t source)
{
	// UNUSED parameter
	(void)source;

	// Trace GlobalVars
	GlobalVarHandler_RunTrace();

	return TaskResult_Ok;
}
#endif



#ifdef CONFIG_FUNCTION_CHARGER
static TaskResult_t Task_InputOutput(ScheduleSource_t source)
{
	// UNUSED parameter
	(void)source;

	Logic_CheckCharger();

	ADC_ConvertAllMeasuredValues();

	return TaskResult_Ok;
}
#endif



#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
static TaskResult_t Task_CommonAdcFunction(ScheduleSource_t source)
{
	(void)source;

	// Convert ADC values
	CommonADC_ConvertAllMeasuredValues();

	// Print
	CommonADC_PrintAdc();

	// Check it is interrupted (with button)?
	if (BUTTON_Clicked)
	{
		// Pressed button. Stop ADC printing
		CommandHandler_SendLine("ADC read is interrupted");
		TaskHandler_DisableTask(Task_CommonAdc);
	}

	return TaskResult_Ok;
}
#endif



#ifdef CONFIG_DEBUG_SELFTEST
static TaskResult_t Task_SelfTestFunction(ScheduleSource_t source)
{
	(void)source;

	// SelfTest:

	#if defined(CONFIG_MODULE_ADC_ENABLE) || defined(CONFIG_MODULE_COMMON_ADC_ENABLE)
	// Check ADC-DMA
	if (ADC_RunCnt == 0)
	{
		// ADC is not run...
		Error_Handler();
	}
	#endif

	return TaskResult_Ok;
}
#endif



#endif //#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
