/*
 * Display_TM1637.h
 *
 *  Created on: 2024. márc. 25.
 *      Author: faste
 */

#ifndef INC_DISPLAY_DISPLAY_TM1637_H_
#define INC_DISPLAY_DISPLAY_TM1637_H_


#include "compiler.h"
#include "DateTime.h"



#define SEG_A   0b00000001
#define SEG_B   0b00000010
#define SEG_C   0b00000100
#define SEG_D   0b00001000
#define SEG_E   0b00010000
#define SEG_F   0b00100000
#define SEG_G   0b01000000
#define SEG_DP  0b10000000


extern void TM1637Display_Init(void);

extern void Display_TM1637_Test(void);

extern void Display_TM1637_DisplayTime(Time_t *time);

extern void Display_TM1637_Print(char *str);



#endif /* INC_DISPLAY_DISPLAY_TM1637_H_ */
