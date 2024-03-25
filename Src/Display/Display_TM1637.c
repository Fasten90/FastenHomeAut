/*
 * Display_TM1637.c
 *
 *  Created on: 2024. márc. 25.
 *      Author: faste
 */


/* https://www.makerguides.com/tm1637-arduino-tutorial/ /*
 *
 */
#include "Display_TM1637.h"
#include "UART.h"


//
//      A
//     ---
//  F |   | B
//     -G-
//  E |   | C
//     ---
//      D
static const uint8_t digitToSegment[] = {
 // XGFEDCBA
  0b00111111,    // 0
  0b00000110,    // 1
  0b01011011,    // 2
  0b01001111,    // 3
  0b01100110,    // 4
  0b01101101,    // 5
  0b01111101,    // 6
  0b00000111,    // 7
  0b01111111,    // 8
  0b01101111,    // 9
  0b01110111,    // A
  0b01111100,    // b
  0b00111001,    // C
  0b01011110,    // d
  0b01111001,    // E
  0b01110001     // F
  };



volatile UART_HandleTypeDef Display_UartHandle;

static volatile uint8_t display_buffer[5];


void Display_TM1637_Test(void)
{

    /* USART TX - RX     */
    UART_Init(&Display_UartHandle);


    char * string = "0123456789ABCDF";
    uint8_t index;
    uint8_t digit;

    for (int i=0; i++; i<strlen(string))
    {
        digit = string[i];
        if (digit >= '0' && digit <= '9')
        {
            index = digit - '0';
            digit = digitToSegment[index];
        }
        else if (digit >= 'A' && digit <= 'F')
        {
            index = digit - 'A' + 10;
            digit = digitToSegment[index];
        }
        else if (digit >= 'a' && digit <= 'f')
        {
            index = digit - 'a' + 10;
            digit = digitToSegment[index];
        }
        else
        {
            index = 15; /* hardcoded E - ERROR */
            digit = digitToSegment[index];
        }
        display_buffer[i] = digit;
    }

    //HAL_StatusTypeDef res = HAL_UART_Transmit(Display_UartHandle, uint8_t *pData, uint16_t Size, uint32_t Timeout);
    HAL_StatusTypeDef res = HAL_UART_Transmit(&Display_UartHandle, display_buffer, 5, 5000);

}

