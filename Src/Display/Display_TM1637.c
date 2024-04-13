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
#include "compiler.h"
#include "board.h"
#include "StringHelper.h"



/* TODO: Move to board.h */
/* Definition for Display TM1637 I2C Pins */
/*
PB6 SCL
PB7 SDA
*/
#define DISPLAY_TM1637_I2C_SCK_PIN                  ( GPIO_PIN_6 )
#define DISPLAY_TM1637_I2C_SCK_GPIO_PORT            ( GPIOB )
#define DISPLAY_TM1637_I2C_SCK_AF                   ( GPIO_AF1_I2C1 )

#define DISPLAY_TM1637_I2C_SDA_PIN                  ( GPIO_PIN_7 )
#define DISPLAY_TM1637_I2C_SDA_GPIO_PORT            ( GPIOB )
#define DISPLAY_TM1637_I2C_SDA_AF                   ( GPIO_AF1_I2C1 )

#define DISPLAY_TM1637_PINS_CLK_ENABLE()            __HAL_RCC_GPIOB_CLK_ENABLE()


#define TM1637_I2C_COMM1    0x40
#define TM1637_I2C_COMM2    0xC0
#define TM1637_I2C_COMM3    0x80



typedef struct
{
  uint8_t               lock;
  uint8_t               brightness;
  bool                  show_zero;
  GPIO_TypeDef          *gpio_clk;
  GPIO_TypeDef          *gpio_dat;
  uint16_t              pin_clk;
  uint16_t              pin_dat;

}tm1637_t;





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


#define BUFFER_LENGTH   (5)
static volatile uint8_t display_buffer[BUFFER_LENGTH];


/* Arduino example sketch to display DHT11 temperature readings
  on a TM1637 4-digit 7-segment display.
  More info: www.www.makerguides.com */


//static uint8_t m_pinClk;
//static uint8_t m_pinDIO;
static uint8_t m_brightness;
static unsigned int m_bitDelay;


static volatile tm1637_t disp;

const uint8_t seg_LOAD[4] =  {0x38, 0x5c, 0x77, 0x5e}; // display LOAD


void TM1637Display_Init(void)
{
    tm1637_init(&disp, DISPLAY_TM1637_I2C_SCK_GPIO_PORT, DISPLAY_TM1637_I2C_SCK_PIN, DISPLAY_TM1637_I2C_SDA_GPIO_PORT, DISPLAY_TM1637_I2C_SDA_PIN);


#if 0
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
    //HAL_I2CEx_ConfigAnalogFilter(&I2cHandle,I2C_ANALOGFILTER_ENABLE);
#endif

}

void setBrightness(uint8_t brightness, bool on)
{
    m_brightness = (brightness & 0x7) | (on? 0x08 : 0x00);
}

void setSegments(const uint8_t segments[], uint8_t length, uint8_t pos)
{
    // Write COMM1
    //start();
    //writeByte(TM1637_I2C_COMM1);
    //stop();

    // Write COMM2 + first digit address
    //start();
    //writeByte(TM1637_I2C_COMM2 + (pos & 0x03));

    // Write the data bytes
    for (uint8_t k=0; k < length; k++)
    {
      //writeByte(segments[k]);
    }

    //stop();

    // Write COMM3 + brightness
    //start();
    //writeByte(TM1637_I2C_COMM3 + (m_brightness & 0x0f));
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


bool writeByte(uint8_t *b, uint8_t datalength)
{

    HAL_StatusTypeDef result;
    //uint8_t datalength = 1;
    result = HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)TM1637_I2C_COMM1, (uint8_t*)b, datalength, 10000);
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


void Display_TM1637_Test(void)
{

    char * string = "0123456789ABCDF";
    uint8_t index;
    uint8_t digit;

    const int i = 0;
    if (1)
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
        //display_buffer[i] = digit;
        //TM1637_I2C_COMM1); // address
        display_buffer[0] = TM1637_I2C_COMM2 + 1;  // pos
        display_buffer[1] = digit;
        uint8_t m_brightness = 2;
        display_buffer[2] = TM1637_I2C_COMM3 + (m_brightness & 0x0f);
#if 0
        writeByte(display_buffer, 3);
#endif
    }


    tm1637_write_segment(&disp, seg_LOAD, 4, 0);
    HAL_Delay(1000);
    tm1637_write_float(&disp, -1.23, 2, 0);
    HAL_Delay(1000);
    tm1637_write_int(&disp, 56, 0);

}

/**************************************/
/* https://github.com/nimaltd/tm1637/blob/main/README.md */



#define TM1637_COMM1    0x40
#define TM1637_COMM2    0xC0
#define TM1637_COMM3    0x80

#define _TM1637_BIT_DELAY           20



const uint8_t _tm1637_digit[] =
  {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
const uint8_t _tm1637_on[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t _tm1637_off[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
const uint8_t fill_off[4] = {0x00, 0x00, 0x00, 0x00};
const uint8_t _tm1637_minus = 0x40;
const uint8_t _tm1637_dot = 0x80;



void tm1637_delay_us(uint8_t delay)
{
    DelayMs(delay/1000);
}


void tm1637_start(tm1637_t *tm1637)
{
  HAL_GPIO_WritePin(tm1637->gpio_dat, tm1637->pin_dat, GPIO_PIN_RESET);
  tm1637_delay_us(_TM1637_BIT_DELAY);
}
//#######################################################################################################################
void tm1637_stop(tm1637_t *tm1637)
{
  HAL_GPIO_WritePin(tm1637->gpio_dat, tm1637->pin_dat, GPIO_PIN_RESET);
  tm1637_delay_us(_TM1637_BIT_DELAY);
  HAL_GPIO_WritePin(tm1637->gpio_clk, tm1637->pin_clk, GPIO_PIN_SET);
  tm1637_delay_us(_TM1637_BIT_DELAY);
  HAL_GPIO_WritePin(tm1637->gpio_dat, tm1637->pin_dat, GPIO_PIN_SET);
  tm1637_delay_us(_TM1637_BIT_DELAY);
}
//#######################################################################################################################
uint8_t tm1637_write_byte(tm1637_t *tm1637, uint8_t data)
{
  //  write 8 bit data
  for (uint8_t i = 0; i < 8; i++)
  {
    HAL_GPIO_WritePin(tm1637->gpio_clk, tm1637->pin_clk, GPIO_PIN_RESET);
    tm1637_delay_us(_TM1637_BIT_DELAY);
    if (data & 0x01)
      HAL_GPIO_WritePin(tm1637->gpio_dat, tm1637->pin_dat, GPIO_PIN_SET);
    else
      HAL_GPIO_WritePin(tm1637->gpio_dat, tm1637->pin_dat, GPIO_PIN_RESET);
    tm1637_delay_us(_TM1637_BIT_DELAY);
    HAL_GPIO_WritePin(tm1637->gpio_clk, tm1637->pin_clk, GPIO_PIN_SET);
    tm1637_delay_us(_TM1637_BIT_DELAY);
    data = data >> 1;
  }
  // wait for acknowledge
  HAL_GPIO_WritePin(tm1637->gpio_clk, tm1637->pin_clk, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(tm1637->gpio_dat, tm1637->pin_dat, GPIO_PIN_SET);
  tm1637_delay_us(_TM1637_BIT_DELAY);
  HAL_GPIO_WritePin(tm1637->gpio_clk, tm1637->pin_clk, GPIO_PIN_SET);
  tm1637_delay_us(_TM1637_BIT_DELAY);
  uint8_t ack = HAL_GPIO_ReadPin(tm1637->gpio_dat, tm1637->pin_dat);
  if (ack == 0)
    HAL_GPIO_WritePin(tm1637->gpio_dat, tm1637->pin_dat, GPIO_PIN_RESET);
  tm1637_delay_us(_TM1637_BIT_DELAY);
  HAL_GPIO_WritePin(tm1637->gpio_clk, tm1637->pin_clk, GPIO_PIN_RESET);
  tm1637_delay_us(_TM1637_BIT_DELAY);
  return ack;
}
//#######################################################################################################################
void tm1637_lock(tm1637_t *tm1637)
{
  while (tm1637->lock == 1)
      DelayMs(1);
  tm1637->lock = 1;
}
//#######################################################################################################################
void tm1637_unlock(tm1637_t *tm1637)
{
  tm1637->lock = 0;
}
//#######################################################################################################################
void tm1637_init(tm1637_t *tm1637, GPIO_TypeDef *gpio_clk, uint16_t pin_clk, GPIO_TypeDef *gpio_dat, uint16_t pin_dat)
{
  memset(tm1637, 0, sizeof(tm1637_t));
  //  set max brightess
  tm1637_brightness(tm1637, 7);
  tm1637_lock(tm1637);
  //  init gpio
  tm1637->gpio_clk = gpio_clk;
  tm1637->pin_clk = pin_clk;
  tm1637->gpio_dat = gpio_dat;
  tm1637->pin_dat = pin_dat;
  GPIO_InitTypeDef g = {0};
  g.Mode = GPIO_MODE_OUTPUT_OD;
  g.Pull = GPIO_NOPULL;
  g.Speed = GPIO_SPEED_FREQ_HIGH;
  g.Pin = pin_clk;
  HAL_GPIO_Init(gpio_clk, &g);
  g.Pin = pin_dat;
  HAL_GPIO_Init(gpio_dat, &g);
  tm1637_unlock(tm1637);
}
//#######################################################################################################################
void tm1637_brightness(tm1637_t *tm1637, uint8_t brightness_0_to_7)
{
  tm1637_lock(tm1637);
  tm1637->brightness = (brightness_0_to_7 & 0x7) | 0x08;
  tm1637_unlock(tm1637);
}
//#######################################################################################################################
void tm1637_write_raw(tm1637_t *tm1637, const uint8_t *raw, uint8_t length, uint8_t pos)
{
  if (pos > 5)
    return;
  if (length > 6)
    length = 6;
  // write COMM1
  tm1637_start(tm1637);
  tm1637_write_byte(tm1637, TM1637_COMM1);
  tm1637_stop(tm1637);
  // write COMM2 + first digit address
  tm1637_start(tm1637);
  tm1637_write_byte(tm1637, TM1637_COMM2 + (pos & 0x03));
  // write the data bytes
  for (uint8_t k=0; k < length; k++)
    tm1637_write_byte(tm1637, raw[k]);
  tm1637_stop(tm1637);
  // write COMM3 + brightness
  tm1637_start(tm1637);
  tm1637_write_byte(tm1637, TM1637_COMM3 + tm1637->brightness);
  tm1637_stop(tm1637);
}
//#######################################################################################################################
void tm1637_write_segment(tm1637_t *tm1637, const uint8_t *segments, uint8_t length, uint8_t pos)
{
  tm1637_lock(tm1637);
  tm1637_write_raw(tm1637, segments, length, pos);
  tm1637_unlock(tm1637);
}
//#######################################################################################################################
void tm1637_write_int(tm1637_t *tm1637, int32_t digit, uint8_t pos)
{
  tm1637_lock(tm1637);
  char str[7];
  uint8_t buffer[6] = {0};
  snprintf(str, sizeof(str) , "%d", digit);
  for (uint8_t i=0; i < 6; i++)
  {
    if (str[i] == '-')
      buffer[i] = _tm1637_minus;
    else if((str[i] >= '0') && (str[i] <= '9'))
      buffer[i] = _tm1637_digit[str[i] - 48];
    else
    {
      buffer[i] = 0;
      break;
    }
  }
  tm1637_write_raw(tm1637, buffer, 6, pos);
  tm1637_unlock(tm1637);
}
//#######################################################################################################################
void tm1637_write_float(tm1637_t *tm1637, float digit, uint8_t floating_digit, uint8_t pos)
{
  tm1637_lock(tm1637);
  char str[8];
  uint8_t buffer[6] = {0};
  if (floating_digit >6)
    floating_digit = 6;
  switch (floating_digit)
  {
    case 0:
      snprintf(str, sizeof(str) , "%.0f", digit);
    break;
    case 1:
      snprintf(str, sizeof(str) , "%.1f", digit);
    break;
    case 2:
      snprintf(str, sizeof(str) , "%.2f", digit);
    break;
    case 3:
      snprintf(str, sizeof(str) , "%.3f", digit);
    break;
    case 4:
      snprintf(str, sizeof(str) , "%.4f", digit);
    break;
    case 5:
      snprintf(str, sizeof(str) , "%.5f", digit);
    break;
    case 6:
      snprintf(str, sizeof(str) , "%.6f", digit);
    break;
  }
  if (tm1637->show_zero == false)
  {
    for (int8_t i = StringLength(str) - 1; i > 0; i--)
    {
      if (str[i] == '0')
        str[i] = 0;
      else
        break;
    }
  }
  uint8_t index = 0;
  for (uint8_t i=0; i < 7; i++)
  {
    if (str[i] == '-')
    {
      buffer[index] = _tm1637_minus;
      index++;
    }
    else if((str[i] >= '0') && (str[i] <= '9'))
    {
      buffer[index] = _tm1637_digit[str[i] - 48];
      index++;
    }
    else if (str[i] == '.')
    {
      if (index > 0)
        buffer[index - 1] |= _tm1637_dot;
    }
    else
    {
      buffer[index] = 0;
      break;
    }
  }
  tm1637_write_raw(tm1637, buffer, 6, pos);
  tm1637_unlock(tm1637);
}
//#######################################################################################################################
void tm1637_show_zero(tm1637_t *tm1637, bool enable)
{
  tm1637->show_zero = enable;
}
//#######################################################################################################################
void tm1637_fill(tm1637_t *tm1637, bool enable)
{
    if (enable)
        tm1637_write_segment(tm1637, _tm1637_on, 6, 0);
    else
        tm1637_write_segment(tm1637, _tm1637_off, 6, 0);
}


