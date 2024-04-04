/*
 * Display_TM1637.c
 *
 *  Created on: 2024. márc. 25.
 *      Author: fasten
 */


/* https://www.makerguides.com/tm1637-arduino-tutorial/
 *
 */
#include "Display_TM1637.h"
#include "board.h"



/* TODO: Move to board.h */
/* Definition for Display TM1637 I2C Pins */
/*
PB6 SCL
PB7 SDA
*/
#define DISPLAY_TM1637_I2C_SCK_PIN                    GPIO_PIN_6
#define DISPLAY_TM1637_I2C_SCK_GPIO_PORT             GPIOB
#define DISPLAY_TM1637_I2C_SCK_AF                     GPIO_AF1_I2C1

#define DISPLAY_TM1637_I2C_SDA_PIN                   GPIO_PIN_7
#define DISPLAY_TM1637_I2C_SDA_GPIO_PORT             GPIOB
#define DISPLAY_TM1637_I2C_SDA_AF                    GPIO_AF1_I2C1

#define DISPLAY_TM1637_PINS_CLK_ENABLE()               __HAL_RCC_GPIOB_CLK_ENABLE()


#define I2Cx                            I2C1
/* This example use TIMING to 0x00A51314 to reach 1 MHz speed (Rise time = 100 ns, Fall time = 100 ns) */
#define I2C_TIMING      0x00A51314
/* TODO: This is a random address */
#define I2C_ADDRESS     (TM1637_I2C_COMM1)


I2C_HandleTypeDef I2cHandle;





#define TM1637_I2C_COMM1    0x40
#define TM1637_I2C_COMM2    0xC0
#define TM1637_I2C_COMM3    0x80


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


static const uint8_t minusSegments = 0b01000000;


static volatile uint8_t display_buffer[5];


/* Arduino example sketch to display DHT11 temperature readings
  on a TM1637 4-digit 7-segment display.
  More info: www.www.makerguides.com */


//static uint8_t m_pinClk;
//static uint8_t m_pinDIO;
static uint8_t m_brightness;
static unsigned int m_bitDelay;



void TM1637Display_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    /*##-2- Configure peripheral GPIO ##########################################*/
    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.Pin = DISPLAY_TM1637_I2C_SCK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = DISPLAY_TM1637_I2C_SCK_AF;

    HAL_GPIO_Init(DISPLAY_TM1637_I2C_SCK_GPIO_PORT, &GPIO_InitStruct);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin = DISPLAY_TM1637_I2C_SDA_PIN;

    HAL_GPIO_Init(DISPLAY_TM1637_I2C_SDA_GPIO_PORT, &GPIO_InitStruct);

    /* Initialize other GPIO pins */
    DISPLAY_TM1637_PINS_CLK_ENABLE();

    /*##-1- Configure the I2C peripheral ######################################*/
    I2cHandle.Instance             = I2Cx;
    I2cHandle.Init.Timing          = I2C_TIMING;
    I2cHandle.Init.OwnAddress1     = I2C_ADDRESS;
    I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_10BIT;
    I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    I2cHandle.Init.OwnAddress2     = 0xFF;
    I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

    if(HAL_I2C_Init(&I2cHandle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    /* Enable the Analog I2C Filter */
    HAL_I2CEx_ConfigAnalogFilter(&I2cHandle,I2C_ANALOGFILTER_ENABLE);

}

void setBrightness(uint8_t brightness, bool on)
{
    m_brightness = (brightness & 0x7) | (on? 0x08 : 0x00);
}

void setSegments(const uint8_t segments[], uint8_t length, uint8_t pos)
{
    // Write COMM1
    //start();
    writeByte(TM1637_I2C_COMM1);
    //stop();

    // Write COMM2 + first digit address
    //start();
    writeByte(TM1637_I2C_COMM2 + (pos & 0x03));

    // Write the data bytes
    for (uint8_t k=0; k < length; k++)
    {
      writeByte(segments[k]);
    }

    //stop();

    // Write COMM3 + brightness
    //start();
    writeByte(TM1637_I2C_COMM3 + (m_brightness & 0x0f));
    //stop();
}

void clear()
{
    uint8_t data[] = { 0, 0, 0, 0 };
    setSegments(data, 4, 0);
}

void showNumberDec(int num, bool leading_zero, uint8_t length, uint8_t pos)
{
  showNumberDecEx(num, 0, leading_zero, length, pos);
}

void showNumberDecEx(int num, uint8_t dots, bool leading_zero,
                                    uint8_t length, uint8_t pos)
{
  showNumberBaseEx(num < 0? -10 : 10, num < 0? -num : num, dots, leading_zero, length, pos);
}

void showNumberHexEx(uint16_t num, uint8_t dots, bool leading_zero,
                                    uint8_t length, uint8_t pos)
{
  showNumberBaseEx(16, num, dots, leading_zero, length, pos);
}

void showNumberBaseEx(int8_t base, uint16_t num, uint8_t dots, bool leading_zero,
                                    uint8_t length, uint8_t pos)
{
    bool negative = false;
    if (base < 0)
    {
        base = -base;
        negative = true;
    }


    uint8_t digits[4];

    if (num == 0 && !leading_zero) {
        // Singular case - take care separately
        for(uint8_t i = 0; i < (length-1); i++)
            digits[i] = 0;
        digits[length-1] = encodeDigit(0);
    }
    else {
        //uint8_t i = length-1;
        //if (negative) {
        //  // Negative number, show the minus sign
        //    digits[i] = minusSegments;
        //  i--;
        //}

        for(int i = length-1; i >= 0; --i)
        {
            uint8_t digit = num % base;

            if (digit == 0 && num == 0 && leading_zero == false)
                // Leading zero is blank
                digits[i] = 0;
            else
                digits[i] = encodeDigit(digit);

            if (digit == 0 && num == 0 && negative) {
                digits[i] = minusSegments;
                negative = false;
            }

            num /= base;
        }

        if(dots != 0)
        {
            showDots(dots, digits);
        }
    }
    setSegments(digits, length, pos);
}

void bitDelay()
{
    DelayMs(m_bitDelay);
}

void start()
{
  //pinMode(m_pinDIO, OUTPUT);
  bitDelay();
}

//void stop()
void Oldstop()
{
    //pinMode(m_pinDIO, OUTPUT);
    bitDelay();
    //pinMode(m_pinClk, INPUT);

    bitDelay();
    //pinMode(m_pinDIO, INPUT);
    bitDelay();
}


bool writeByte(uint8_t b)
{

    HAL_StatusTypeDef result;
    uint8_t datalength = 1;
    result = HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)I2C_ADDRESS, (uint8_t*)&b, datalength, 10000);
    /* Error_Handler() function is called when Timeout error occurs.
    When Acknowledge failure occurs (Slave don't acknowledge its address)
    Master restarts communication */
    //if (HAL_I2C_GetError(&I2cHandle) != HAL_I2C_ERROR_AF)
    //{
    //Error_Handler();
    //}
    return (result == HAL_OK) ? true : false;
}


//bool writeByte(uint8_t b)
bool oldWriteBzte(uint8_t b)
{
  uint8_t data = b;

  // 8 Data Bits
  for(uint8_t i = 0; i < 8; i++) {
    // CLK low
    //pinMode(m_pinClk, OUTPUT);
    SET_SCK_PIN(RESET);
    bitDelay();

    // Set data bit
    if (data & 0x01)
    {
      //pinMode(m_pinDIO, INPUT);
      SET_DIO_PIN(SET);
    }
    else
    {
      //pinMode(m_pinDIO, OUTPUT);
      SET_DIO_PIN(RESET);
    }

    bitDelay();

    // CLK high
    //pinMode(m_pinClk, INPUT);
    SET_SCK_PIN(SET);
    bitDelay();
    data = data >> 1;
  }

  // Wait for acknowledge
  // CLK to zero
  //pinMode(m_pinClk, OUTPUT);
  SET_SCK_PIN(RESET);
  //pinMode(m_pinDIO, INPUT);
  SET_DIO_PIN(SET);
  bitDelay();

  // CLK to high
  //pinMode(m_pinClk, INPUT);
  SET_SCK_PIN(SET);
  bitDelay();
  //uint8_t ack = digitalRead(m_pinDIO);
  //if (ack == 0)
  //{
  //  pinMode(m_pinDIO, OUTPUT);
    SET_DIO_PIN(RESET);
  //}

  bitDelay();
  //pinMode(m_pinClk, OUTPUT);
  SET_SCK_PIN(RESET);
  bitDelay();

  //return ack;
  return true;
}

void showDots(uint8_t dots, uint8_t* digits)
{
    for(int i = 0; i < 4; ++i)
    {
        digits[i] |= (dots & 0x80);
        dots <<= 1;
    }
}

uint8_t encodeDigit(uint8_t digit)
{
    return digitToSegment[digit & 0x0f];
}



// Define the connections pins:
#define CLK 2
#define DIO 3
#define DHTPIN 4

// Create variable:
int temperature_celsius;
int temperature_fahrenheit;

// Create degree Celsius symbol:
const uint8_t celsius[] = {
  SEG_A | SEG_B | SEG_F | SEG_G,  // Circle
  SEG_A | SEG_D | SEG_E | SEG_F   // C
};

// Create degree Fahrenheit symbol:
const uint8_t fahrenheit[] = {
  SEG_A | SEG_B | SEG_F | SEG_G,  // Circle
  SEG_A | SEG_E | SEG_F | SEG_G   // F
};


void Display_TM1637_Test(void)
{

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

}

