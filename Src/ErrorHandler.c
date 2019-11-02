/*
 *    ErrorHandler.c
 *    Created on:   2016-09-15
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Global variables
 *    Target:       STM32Fx
 */



#include "ErrorHandler.h"
#include "options.h"
#include "compiler.h"
#include "IO.h"
#include "StringHelper.h"
#include "DebugUart.h"
#include "TaskHandler.h"
#include "Timing.h"

#ifdef CONFIG_PLATFORM_X86
#include "x86_hal.h"
#endif



/*------------------------------------------------------------------------------
 *                                Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *                                    Functions
 *----------------------------------------------------------------------------*/


/**
 * @brief    Error Handler
 */
void Error_Handler(void)
{
#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
    #ifdef CONFIG_TASKHANDLER_DEBUG_RUN_ENABLE
    const uint8_t msgLength = 60;
    char msg[msgLength];
    #endif
#endif

    /* Error LED */
    IO_Output_SetStatus(IO_LED_Red, IO_Output_Cmd_SetOn);
    IO_Output_SetStatus(IO_LED_Green, IO_Output_Cmd_SetOff);
    IO_Output_SetStatus(IO_LED_Blue, IO_Output_Cmd_SetOff);

#ifdef CONFIG_USE_FREERTOS
    /* End task scheduling */
    vTaskEndScheduler();
#endif

    DebugUart_SendMessageBlocked("ErrorHandler...!!!\r\n");

#ifdef CONFIG_MODULE_TASKHANDLER_ENABLE
    #ifdef CONFIG_TASKHANDLER_DEBUG_RUN_ENABLE
    usnprintf(msg, msgLength, "TaskHandler frozen: %s\r\n", TaskHandler_GetActualRunningTaskName());
    DebugUart_SendMessageBlocked(msg);
    #else
    DebugUart_SendMessageBlocked("TaskHandler frozen!\r\n");
    #endif
#endif


    /* Stop debugger */
    DEBUG_BREAKPOINT();

    /* Infinite loop, do not disable interrupts ... */
    /* TODO: But... If there is no interrupt? */
    uint8_t cnt = 8;
    while (cnt--)
    {
        IO_Output_SetStatus(IO_LED_Red, IO_Output_Cmd_SetToggle);
        DelayMs(125);
    }

    /* Reset... */
    /* TODO: It is not the best solution, The user will not detect the reset */
    NVIC_SystemReset();
}



#ifdef USE_FULL_ASSERT
/**
 * @brief Reports the name of the source file and the source line number where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    Assert_Function((char *)file, line, (char *)"assert_failed");
}
#endif



/**
 * @brief    Assert
 */
void Assert_Function(char *file, uint32_t line, char *exp)
{
    const uint8_t msgLength = 255;
    char errorMsg[msgLength];

    /* Error LED */
    IO_Output_SetStatus(IO_LED_Red, IO_Output_Cmd_SetOn);
    IO_Output_SetStatus(IO_LED_Green, IO_Output_Cmd_SetOff);
    IO_Output_SetStatus(IO_LED_Blue, IO_Output_Cmd_SetOff);

    /* Send error message */
    usnprintf(errorMsg, msgLength, "File: %s, %d. line: %s\r\n", file, line, exp);
    DebugUart_SendMessageBlocked(errorMsg);
    /* TODO: Need the wait? Message was sent blocked */
    DelayMs(100);

    DEBUG_BREAKPOINT();
    Error_Handler();
}


