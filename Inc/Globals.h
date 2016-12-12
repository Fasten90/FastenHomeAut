/*
 *		Globals.h
 *
 *		Created on:		2016.09.15
 *		Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Globals
 *		Target:			STM32Fx
 *		Version:		v4
 *		Last modified:	2016.09.28
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_


extern const char Global_BoardName[];
extern const char Global_Version[];


void DelayMs(uint32_t ms);
void Error_Handler(void);


#endif /* GLOBALS_H_ */
