/*
 *		commonIO.c
 *
 *		Created on:		2016. nov. 28.
 *      Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		-
 *		Target:			STM32Fx
 *		Version:		-
 *		Last modified:	2016. nov. 28.
 */


#include "options.h"
#include "include.h"
#include "CommonIO.h"


#ifdef CONFIG_MODULE_COMMON_IO_ENABLE

/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static bool IO_EnablePeripheralClock(char port);
static GPIO_TypeDef * IO_GetPort(char port);
static uint32_t IO_GetPin(uint8_t pin);
static uint32_t IO_GetMode(IO_Type io);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

/**
 * \brief	Initialize IO pin
 */
bool IO_Init(char port, uint8_t pin, IO_Type io)
{

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_TypeDef *GPIO_port = IO_GetPort(port);

	GPIO_InitStruct.Pin = IO_GetPin(pin);

	bool clk = IO_EnablePeripheralClock(port);

	GPIO_InitStruct.Mode = IO_GetMode(io);

	// Check parameters
	if (GPIO_port == NULL || GPIO_InitStruct.Pin == 0xFFFFFFFF || !clk)
	{
		return false;
	}

	GPIO_InitStruct.Pull = GPIO_NOPULL;
	//GPIO_InitStruct.Alternate = // TODO:
	HAL_GPIO_Init(GPIO_port, &GPIO_InitStruct);

	return true;
}



/**
 * \brief	Set output
 */
bool IO_SetOutput(char port, uint8_t pin, Output_Type output)
{
	bool status = false;

	GPIO_TypeDef *GPIO_port = IO_GetPort(port);

	uint32_t realPin = IO_GetPin(pin);

	// Check parameters
	if (GPIO_port == NULL || realPin == 0xFFFFFFFF || output >= OUTPUT_COUNT)
	{
		// Wrong parameters
		return false;
	}

	// Check it is configurated?

	switch (output)
	{
		case OUTPUT_DONTCARE:
			status = true;
			break;

		case OUTPUT_HIGH:
			HAL_GPIO_WritePin(GPIO_port,realPin,GPIO_PIN_SET);
			status = true;
			break;

		case OUTPUT_LOW:
			HAL_GPIO_WritePin(GPIO_port,realPin,GPIO_PIN_RESET);
			status = true;
			break;

		case OUTPUT_TOGGLE:
			HAL_GPIO_TogglePin(GPIO_port,realPin);
			status = true;
			break;

		case OUTPUT_STATUS:
			status = HAL_GPIO_ReadPin(GPIO_port, realPin);
			break;

		case OUTPUT_COUNT:
			status = false;
			break;

		default:
			status = false;
	}

	return status;
}



/**
 * \brief	Read input
 */
bool IO_ReadPin(char port, uint8_t pin)
{
	bool status = false;

	GPIO_TypeDef *GPIO_port = IO_GetPort(port);

	uint32_t realPin = IO_GetPin(pin);

	// Check parameters
	if (GPIO_port == NULL || realPin == 0xFFFFFFFF)
	{
		// Wrong parameters
		return false;
	}

	status = HAL_GPIO_ReadPin(GPIO_port, realPin);

	return status;
}



/**
 * \brief	Enable peripheral clock
 */
static bool IO_EnablePeripheralClock(char port)
{

	switch (port)
	{
		case 'a':
		case 'A':
			__GPIOA_CLK_ENABLE();
			break;

		case 'b':
		case 'B':
			__GPIOB_CLK_ENABLE();
			break;
		case 'c':
		case 'C':
			__GPIOC_CLK_ENABLE();
			break;

		case 'd':
		case 'D':
			__GPIOD_CLK_ENABLE();
			break;

		case 'e':
		case 'E':
			__GPIOE_CLK_ENABLE();
			break;

		default:
			 return false;
	}

	return true;

}



/**
 * \brief	Convert "port name" to "port"
 */
static GPIO_TypeDef * IO_GetPort(char port)
{
	GPIO_TypeDef *GPIO_port;

	switch (port)
	{
		case 'a':
		case 'A':
			GPIO_port = GPIOA;
			break;

		case 'b':
		case 'B':
			GPIO_port = GPIOB;

			break;
		case 'c':
		case 'C':
			GPIO_port = GPIOC;
			break;

		case 'd':
		case 'D':
			GPIO_port = GPIOD;
			break;

		case 'e':
		case 'E':
			GPIO_port = GPIOE;
			break;

		default:
			GPIO_port = NULL;
	}

	return GPIO_port;
}



/**
 * \brief	Convert "num" to "pin"
 */
static uint32_t IO_GetPin(uint8_t pin)
{

	uint32_t returnPin;

	switch (pin)
	{
		case 0:
			returnPin = GPIO_PIN_0;
			break;
		case 1:
			returnPin = GPIO_PIN_1;
			break;
		case 2:
			returnPin = GPIO_PIN_2;
			break;
		case 3:
			returnPin = GPIO_PIN_3;
			break;
		case 4:
			returnPin = GPIO_PIN_4;
			break;
		case 5:
			returnPin = GPIO_PIN_5;
			break;
		case 6:
			returnPin = GPIO_PIN_6;
			break;
		case 7:
			returnPin = GPIO_PIN_7;
			break;
		case 8:
			returnPin = GPIO_PIN_8;
			break;
		case 9:
			returnPin = GPIO_PIN_9;
			break;
		case 10:
			returnPin = GPIO_PIN_10;
			break;
		case 11:
			returnPin = GPIO_PIN_11;
			break;
		case 12:
			returnPin = GPIO_PIN_12;
			break;
		case 13:
			returnPin = GPIO_PIN_13;
			break;
		case 14:
			returnPin = GPIO_PIN_14;
			break;
		case 15:
			returnPin = GPIO_PIN_15;
			break;
		default:
			returnPin = 0xFFFFFFFFU;
	}

	return returnPin;
}



/**
 * \brief	Get IO mode
 */
static uint32_t IO_GetMode(IO_Type io)
{
	uint32_t iomode;
	switch (io)
	{
		case IO_INPUT:
			iomode = GPIO_MODE_INPUT;
			break;

		case IO_OUTPUT:
			iomode = GPIO_MODE_OUTPUT_PP;
			break;

		case IO_UNKNOWN:
		case IO_COUNT:
			break;

		default:
			break;
	}

	return iomode;
}

#endif	// #ifdef CONFIG_MODULE_COMMON_IO_ENABLE
