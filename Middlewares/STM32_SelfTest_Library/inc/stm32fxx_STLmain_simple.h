/*
 * stm32fxx_STLmain_simple.h
 *
 *  Created on: Dec 12, 2016
 *      Author: vizi.gabor
 */

#ifndef STM32_SELFTEST_LIBRARY_INC_STM32FXX_STLMAIN_SIMPLE_H_
#define STM32_SELFTEST_LIBRARY_INC_STM32FXX_STLMAIN_SIMPLE_H_


// Added to STL
#define FLASH_SIZE						(1 * 1024 * 1024)
#define FLASH_ADDR_START				(0x800000)
#define FLASH_ADDR_FLASH_END_VARIABLE	(FLASH_ADDR_START+FLASH_SIZE - 1024)


extern uint8_t STL_FlashResult;


void STL_InitRunTimeChecks(void);
void STL_DoRunTimeChecks(void);



#endif /* STM32_SELFTEST_LIBRARY_INC_STM32FXX_STLMAIN_SIMPLE_H_ */
