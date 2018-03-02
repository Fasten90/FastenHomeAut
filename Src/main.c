/** \mainpage
 * FastenHomeAut project
 * ========================
 *
 * Description
 * ------------------------
 * Welcome in FastenHomeAut project main page.
 *
 * This document created by [Doxygen](http://www.stack.nl/~dimitri/doxygen/index.html)
 *
 * FastenHomeAut is a simple embedded project with "Hardware tester" and "CommandHandler" software, which written in C by Vizi Gábor.
 *
 * This project has these important software modules:
 * - "TaskHandler" for task scheduling (it is not RTOS!)
 * - "CommandHandler" for command executing from anywhere
 * - "EventHandler" for easy event handling and task schedule requiring
 * - "GlobalVarHandler" for global variable get-set function (it help for debug)
 * - Hardware tester modules:
 *   + CommonIO   - Use IO (Input-Output) pins
 *   + CommonADC  - Use ADC (Analog-Digital Converter), read analog value (How many voltage it?)
 *   + CommonDAC  - Use DAC (Digital-Analog Converter), generate adjustable voltage
 *   + CommonPWM  - Use PWM (Pulse Width Modulation)
 *   + CommonUART - Use UART (Universal Asynchronous Receiver-Transmitter), use other UART with Debug UART. (E.g. test an external UART device)
 * - "ConvertTable" for integer - float conversions (e.g. analog <-> voltage, or analog <-> capacity)
 * - "CircularBuffer" for communication
 * - Display:
 *   + Display module: 0.96 OLED
 *   + Write texts (Fonts: 8x5, 12x8, 32x20 (this is only with numbers))
 *   + Functions:
 *     - Show clock
 *     - Snake game
 *     - Show animation: Car with rotary wheels
 *     - Display input: you can type a command and execute it
 *
 *
 * Requirements
 * ------------------------
 *
 * Recommended IDE is the Atollic TrueSTUDIO 6.0 or newer, but you can use newer or an other IDE.
 * - Install [Atollic TrueSTUDIO](http://atollic.com/)
 *   + Alternative IDEs: Eclipse GNU Arm, SW4STM32, etc.
 * - Install a terminal (serial port reader) software
 *   + PuTTY
 *   + HyperTerminal
 *   + ZOC
 *   + Termite
 *   + or my terminal: [FastenTerminal](http://fasten.e5tv.hu/)
 *
 *
 * How to use it?
 * ------------------------
 *
 * - Download binaries to your Device
 *   + If you have source code (project) and an IDE, you can compile and Run/Debug your code on the device
 * - Start run / Reset device.
 * - Connect Device to UART-USB converter or other serial converter
 *   + Connect device UART pins to USB-UART converter (Do not forget the swap: TX-RX):
 *     - In STM32F4Discovery, the UART3 used for Debug UART:
 *       + PC6 (TX)
 *       + PC7 (RX)
 *       + GND
 *     - In Nucleo-F767ZI, the UART3 is available on USB cable (ST-LINK). It will add a virtual COM port.
 *     - In FastenNode (STM32F0) there is Debug UART connector
 *     - Check the actual pin configurations in Inc\board.h file!
 *   + Plug in the UART-USB converter / USB cable
 * - Start serial terminal
 * - Connect Terminal to Serial COM port, with 115200 baudrate, and 8N1 configs
 * - Type "help" and press enter (send with '\\r' or '\\n' or together '\\r\\n')
 *   + Now, you set "help" command and available commands printed out on terminal
 * - Enjoy it
 *
 *
 * Common HW functions (~Hardware tester)
 * --------------------------------------
 *
 * First:
 * - If you need help, type on terminal: \code help \endcode
 * - If you can't use a command, type this: \code help <CommandName> \endcode
 *   + Example: \code help adcread \endcode
 *
 * For common IO-s:
 * - Send IO pin initialization command:
 *    \code	ioinit <port><pin> <input/output> \endcode
 *   + Example: \code ioinit a1 input  \endcode or \code ioinit b13 output \endcode
 * - Send read / write command:
 *   + Read pin: \code ioin <port><pin> \endcode
 *     - Example: \code ioin a1 \endcode
 *   + Write pin: \code ioout <port><pin> <set/reset> \endcode
 *     - Example: \code ioout b13 set \endcode
 *
 * For ADC-s (Analog-Digital Converter):
 * - Send command, and you received the last Analog states (voltages): \code adc \endcode
 * - If you want periodical ADC reading: \code adcread <milliSec> (pin) \endcode
 *   + milliSec: is the time of period (in millisecond)
 *   + pin: is the num of pin (1-2-3)
 *   + Example: \code adcread 1000 \endcode
 *     - Read adc values time of 1 second (1000 millisecond)
 *
 * For DAC-s (Digital-Analog Converter):
 * - \note Not available in all STM32F4xx!
 * - Send: \code dac <1/2> <voltage with decimal point> \endcode
 *   + Example: \code dac 1 3.0 \endcode
 *   + 1. = A4 pin
 *   + 2. = A5 pin
 *
 * LED functions:
 * - Command: \code led <1/2/3> <on/off/toggle/status> \endcode
 *   + Example: \code led 1 on \endcode
 *     - 1. LED will set on
 *   + Example: \code led status \endcode
 *     - LED statuses will printed out on Terminal
 *   + Example: \code led green off \endcode
 *     - Green LED will turn off
 *
 *
 * Other functions:
 * ------------------------
 *
 * - Try the "GlobalVarHandler"
 *   + Type: \code list \endcode
 *   + Type: \code listvalues \endcode
 *   + These will print the available variables
 *   + Type: \code get tick \endcode
 *     - The tick value should print
 *   + Test: \code set <varname> <value> \encode
 *   + Test: \code trace <varname> "enable" \encode
 *
 *
 * Target platforms:
 * ------------------------
 *
 * - STM32xx
 *   - STM32F030x (tested)
 *   - STM32F4 (tested)
 * - Win32 (tested)
 *
 *
 * How development?
 * ------------------------
 *
 * - First, find Vizi Gábor
 * - Check C language(embedded) tutorials
 * - Read and check Atollic tutorials
 * - Download or request HomeAut embedded codes / project
 * - Develop it, write codes
 *   + Use Doxygen document comment style
 *   + Use cppcheck static code analyzer
 *   + Read "Configs.txt" --> Docs\Configs\Configs.txt for project configuration
 * - Compile
 * - Debug / Program your device
 * - Enjoy it
 *
 *
 * \author	Vizi Gábor
 *
 * <fasten90@gmail.com>
 * <vizi.gabor90@gmail.com>
 *
 * <http://fasten.e5tv.hu>
*/



/*------------------------------------------------------------------------------
 *  Includes
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "compiler.h"
#include "Button.h"
#include "LED.h"
#include "CommandHandler.h"
#include "Display_SSD1306.h"
#include "TaskHandler.h"
#include "UART.h"
#include "DebugUart.h"
#include "Terminal.h"
#include "EventLog.h"
#include "Motor.h"
#include "ESP8266.h"
#include "Display.h"
#include "Logic.h"
#include "CommonIO.h"
#include "CommonADC.h"
#include "CommonDAC.h"
#include "CommonUART.h"
#include "ADC.h"
#include "ErrorHandler.h"
#include "Bluetooth_HC05.h"
#include "GlobalVarHandler.h"
#include "Watchdog.h"

#ifdef CONFIG_MODULE_MEASUREMENTTIMER_ENABLE
#include "MeasurementTimer.h"
#endif

#ifdef CONFIG_MODULE_SELFTEST_ENABLE
#include "SelfTest_Flag.h"
#include "SelfTest_Ram.h"
#endif

#ifdef CONFIG_PLATFORM_PC_WINDOWS
#include "windows_hal.h"
#endif

#if defined(CONFIG_MODULE_ESCAPEBROWSER_ENABLE)
#include "EscapeBrowser.h"
#endif


/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

#if defined(CONFIG_MODULE_ESCAPEBROWSER_ENABLE)
	static const char * const strList[] = { "1", "2", "3", NULL };
	static char strBuffer[300] = { 0 };
#endif


/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

void SystemClock_Config(void);

#ifdef CONFIG_MODULE_SELFTEST_ENABLE
void * SelfTest_StackStartAddress = NULL;
bool SelfTest_FailedRamTest = false;
#endif



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

#if defined(CONFIG_MODULE_ESCAPEBROWSER_ENABLE)
static const char * GetStrListElement(uint8_t i)
{
	const char *str = NULL;

	if (i < NUM_OF(strList))
	{
		str =  strList[i];
	}

	return str;
}
#endif


/**
 * \brief	Main function
 */
int main(void)
{
	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();


#ifdef CONFIG_MODULE_SELFTEST_ENABLE
	// Stack use
	volatile uint8_t selfTestStackStartVariable = 0;
	SelfTest_StackStartAddress = (void *)&selfTestStackStartVariable;

	// RAM test
	if (!SelfTest_Ram_Test())
	{
		// Failed RAM test
		SelfTest_FailedRamTest = true;
	}

	// Flag test
	SelfTest_Flag_Test();
#endif


#ifdef CONFIG_PLATFORM_PC_WINDOWS
	// Windows functions - Run unittest immediately

	#ifdef CONFIG_MODULE_UNITTEST_ENABLE
	char responseBuffer[2048];
	CmdH_ExecuteCommand("unittest", responseBuffer, 2048);
	printf(responseBuffer);
	#endif

	// STDIN
	UART_HandleTypeDef Debug_UartHandle;
	HAL_UART_Init(&Debug_UartHandle);
#endif

#ifdef CONFIG_MODULE_MEASUREMENTTIMER_ENABLE
	// MeasurementTimer
	MeasurementTimer_Init();
	MeasurementTimer_StartMeasurement();
#endif


#ifdef CONFIG_MODULE_EVENTLOG_ENABLE
	// EventLog
	EventLog_Init();
#endif


#ifdef CONFIG_MODULE_EVENTHANDLER_ENABLE
	// EventHandler
	EventHandler_Init();
#endif


#ifdef CONFIG_MODULE_IO_ENABLE
	// LED
	IO_Init();
#endif


#ifdef CONFIG_MODULE_BUTTON_ENABLE
	// Button
	BUTTON_Init();
#endif


#ifdef CONFIG_MODULE_COMMON_UART_ENABLE
	CommonUART_Init();
#endif


#ifdef CONFIG_MODULE_COMMON_DAC_ENABLE
	// Common DAC
	CommonDAC_Init();
#endif


#ifdef CONFIG_MODULE_COMMON_ADC_ENABLE
	// Common ADC
	CommonADC_Init();
	// Test, blocking
	//CommonADC_Test();
#endif


#ifdef CONFIG_MODULE_ADC_ENABLE
	// ADC
	ADC_Init();
#endif


#ifdef CONFIG_MODULE_COMMON_PWM_ENABLE
	// Common PWM Initialize
	CommonPWM_Init();
#endif


#ifdef CONFIG_MODULE_MOTOR_ENABLE
	// Remote car motors
	Motor_Init();
#endif


#ifdef CONFIG_MODULE_DISPLAY_ENABLE
	// Display
	Display_SSD1306_Init();
	#ifdef CONFIG_FUNCTION_DISPLAY_MENU
	Logic_Display_Init();
	#endif
#endif


#ifdef CONFIG_MODULE_DEBUGUART_ENABLE
	// DebugUart initializing
	// \note	Be careful, Terminal need to initializing after this
	DebugUart_Init();
#endif


#ifdef CONFIG_MODULE_BLUETOOTH_ENABLE
	// Bluetooth module initializing
	Bluetooth_HC05_Init();
#endif


#ifdef CONFIG_MODULE_COMMANDHANDLER_ENABLE
	// CommandHandler initialization
	CmdH_Init();
#endif


#ifdef CONFIG_MODULE_GLOBALVARHANDLER_ENABLE
	GlobVarH_Init();
#endif


#ifdef CONFIG_MODULE_TERMINAL_ENABLE
	// Terminal init
	Terminal_Init();
#endif


#ifdef CONFIG_MODULE_ESP8266_ENABLE
	// ESP8266
	ESP8266_Init();
#endif


#ifdef CONFIG_MODULE_RTC_ENABLE
	// RTC
	RTC_Init();
#endif


#ifdef CONFIG_MODULE_WATCHDOG_ENABLE
	// Watchdog
	Watchdog_Init();
#endif


#if defined(CONFIG_MODULE_TERMINAL_ENABLE) && !defined(CONFIG_MODULE_TASKHANDLER_ENABLE) && !defined(CONFIG_USE_FREERTOS)
	// Terminal infinite loop, if not used TaskHandler and FreeRTOS
	Terminal_CheckCommand();
#endif


#ifdef CONFIG_MODULE_MEASUREMENTTIMER_ENABLE
	// MeasurementTimer
	uint32_t initTimeInUs = MeasurementTimer_GetTime();
	uprintf("Init time: %u[us], %u[ms]", initTimeInUs, initTimeInUs/1000);
#endif


#if defined(CONFIG_MODULE_ESCAPEBROWSER_ENABLE)
	EscapeBrowser_Init(GetStrListElement);

	EscapeBrowser_PrintNewLineList(strBuffer);
	DebugUart_SendLineBlocked(strBuffer);
#endif


#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
	// Task handler
	TaskHandler_Init();


	// Run task scheduler
	TaskHandler_Scheduler();

	// TaskHandler never return here!!
#endif


#ifdef CONFIG_USE_FREERTOS
	// FreeRTOS scheduler
	vTaskStartScheduler();
#endif


	// !! Never reach this code !!
	Error_Handler();

}	// End of main



#if defined(CONFIG_USE_PANEL_NODESMALL) || defined(CONFIG_USE_PANEL_FASTENNODE)
/**
 * System Clock Configuration
 */
void SystemClock_Config(void)
{

  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

  __SYSCFG_CLK_ENABLE();

}
#endif //#ifdef CONFIG_USE_PANEL_NODESMALL



#ifdef CONFIG_USE_PANEL_NODEMEDIUM
/**
 * System Clock Configuration
 */
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	__PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);


	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);


	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

}
#endif	// #ifdef CONFIG_USE_PANEL_NODEMEDIUM



#ifdef CONFIG_PLATFORM_PC_WINDOWS
/**
 * System Clock Configuration
 */
void SystemClock_Config(void)
{
	return;
}
#endif



#if defined(CONFIG_USE_PANEL_HOMEAUTCENTERPANEL) \
	|| defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) \
	|| defined(CONFIG_USE_PANEL_NUCLEOF401RE)
/**
 * System Clock Configuration
 */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

}
#endif	// #ifdef CONFIG_USE_PANEL_...



#ifdef CONFIG_USE_FREERTOS

/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	// Not used
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	//for( ;; );
	Error_Handler();
}

/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}

/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	//for( ;; );
	Error_Handler();
}

/*-----------------------------------------------------------*/

#endif	// #ifdef CONFIG_USE_FREERTOS
