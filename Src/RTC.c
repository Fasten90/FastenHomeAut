/*
 *		RTC.c
 *
 *		Created on:		2017. jan. 17.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2017. jan. 17.
 */




/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/
#include "include.h"
#include "RTC.h"



#ifdef CONFIG_MODULE_RTC_ENABLE

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/
RTC_HandleTypeDef RtcHandle;


/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief	Initialize RTC
 */
void RTC_Init(void)
{


	/*##-1- Configure the RTC peripheral #######################################*/
	/* Configure RTC prescaler and RTC data registers */
	/* RTC configured as follows:
	  - Hour Format    = Format 24
	  - Asynch Prediv  = Value according to source clock
	  - Synch Prediv   = Value according to source clock
	  - OutPut         = Output Disable
	  - OutPutPolarity = High Polarity
	  - OutPutType     = Open Drain */
	RtcHandle.Instance = RTC;
	RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
	RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
	RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
	RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
	RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

	if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

}



/**
  * @brief RTC MSP Initialization
  *        This function configures the hardware resources used in this example
  * @param hrtc RTC handle pointer
  *
  * @note  Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select
  *        the RTC clock source; in this case the Backup domain will be reset in
  *        order to modify the RTC Clock source, as consequence RTC registers (including
  *        the backup registers) and RCC_BDCR register are set to their reset values.
  *
  * @retval None
  */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
	(void)hrtc;

	RCC_OscInitTypeDef        RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

	/*##-1- Enables the PWR Clock and Enables access to the backup domain ###################################*/
	/* To change the source clock of the RTC feature (LSE, LSI), You have to:
	 - Enable the power clock using __HAL_RCC_PWR_CLK_ENABLE()
	 - Enable write access using HAL_PWR_EnableBkUpAccess() function before to
	   configure the RTC clock source (to be done once after reset).
	 - Reset the Back up Domain using __HAL_RCC_BACKUPRESET_FORCE() and
	   __HAL_RCC_BACKUPRESET_RELEASE().
	 - Configure the needed RTc clock source */
	__HAL_RCC_PWR_CLK_ENABLE();
	HAL_PWR_EnableBkUpAccess();

	/*##-2- Configure LSI as RTC clock source ###################################*/
	RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/*##-3- Enable RTC peripheral Clocks #######################################*/
	/* Enable RTC Clock */
	__HAL_RCC_RTC_ENABLE();
}



/**
  * @brief  Configure the current time and date.
  * @param  None
  * @retval None
  */
void RTC_CalendarConfig(DateTime_t *dateTime)
{

	RTC_DateConfig(&dateTime->date);
	RTC_TimeConfig(&dateTime->time);


	/*##-3- Writes a data in a RTC Backup data Register1 #######################*/
	//HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
}



/**
 * \brief	Set date
 */
void RTC_DateConfig(Date_t *date)
{
	RTC_DateTypeDef sdatestructure;

	/*##-1- Configure the Date #################################################*/
	/* Set Date: Tuesday February 18th 2014 */
	sdatestructure.Year = date->year;
	sdatestructure.Month = date->month;
	sdatestructure.Date = date->day;
	sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;

	if (HAL_RTC_SetDate(&RtcHandle, &sdatestructure, RTC_FORMAT_BIN) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}
}



/**
 * \brief	Set date
 */
void RTC_TimeConfig(Time_t *time)
{

	RTC_TimeTypeDef stimestructure;

	/*##-2- Configure the Time #################################################*/
	/* Set Time: 02:00:00 */
	stimestructure.Hours = time->hour;
	stimestructure.Minutes = time->minute;
	stimestructure.Seconds = time->second;
	stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
	stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
	stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

	if (HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}
}



/**
  * @brief  Display the current time and date.
  * @param  showtime : pointer to buffer
  * @param  showdate : pointer to buffer
  * @retval None
  */
void RTC_CalendarShow(char *showdate, char *showtime)
{
	RTC_DateTypeDef sdatestructureget;
	RTC_TimeTypeDef stimestructureget;
	// TODO: Átalakítani saját formátumra?

	/* Get the RTC current Date */
	HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
	/* Get the RTC current Time */
	HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
	/* Display date Format : mm-dd-yy */
	usprintf((char *)showdate, "%4d-%02d-%02d", 2000 + sdatestructureget.Year, sdatestructureget.Month, sdatestructureget.Date);
	/* Display time Format : hh:mm:ss */
	usprintf((char *)showtime, "%02d:%02d:%02d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
}


#endif	// #ifdef CONFIG_MODULE_RTC_ENABLE
