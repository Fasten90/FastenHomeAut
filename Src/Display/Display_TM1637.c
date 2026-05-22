/*
 * Display_TM1637.c
 *
 *  Created on: 2024. márc. 25.
 *      Author: fasten
 */


/**************************************/
/* https://github.com/nimaltd/tm1637/blob/main/README.md */


#include "options.h"
#include "Display_TM1637.h"
#include "compiler.h"
#include "board.h"
#include "StringHelper.h"
#include "DateTime.h"


#ifdef CONFIG_MODULE_DISPLAY_ENABLE


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


#define TM1637_COMM1    ( 0x40 )
#define TM1637_COMM2    ( 0xC0 )
#define TM1637_COMM3    ( 0x80 )

#define _TM1637_BIT_DELAY           20



typedef struct
{
  uint8_t               lock;
  uint8_t               brightness;
  bool                  show_zero;
  GPIO_TypeDef          *gpio_clk;
  GPIO_TypeDef          *gpio_dat;
  uint16_t              pin_clk;
  uint16_t              pin_dat;

} tm1637_t;

#define CONFIG_DISPLAY_7SEGMENT_EXTRA_LETTER 1

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
  0b01110001,    // F
#ifdef CONFIG_DISPLAY_7SEGMENT_EXTRA_LETTER
  0b00111101,  // G
  0b01110110, 	 // H
  0b00000110,    // I
  0b00001110,    // J
  0b01010110,    // K  BCEG) ugly!
  0b00111000,    // L
  0b01010100,    // M like n)
  0b01010100,    // n
  0b01011100,    // o
  0b01110011,    // P
  0b01100111,    // q
  0b01010000,    // r
  0b01101101,    // S
  0b01111000,    // t
  0b00111110,    // U
  0b00011100,    // v
  0b00011100,    // w like v
  0b01110110,    // x
  0b01101110,    // y
  0b01011011,    // Z
#endif
};


const uint8_t _tm1637_digit[] =
  {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
const uint8_t _tm1637_on[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const uint8_t _tm1637_off[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
const uint8_t fill_off[4] = {0x00, 0x00, 0x00, 0x00};
const uint8_t _tm1637_minus = 0x40;
const uint8_t _tm1637_dot = 0x80;
const uint8_t _tm1637_empty = 0x00;


static const uint8_t minusSegments = 0b01000000;

static tm1637_t disp;

static const uint8_t seg_LOAD[4] =  {0x38, 0x5c, 0x77, 0x5e}; // display LOAD


static uint8_t Display_ConvertSegmens(uint8_t digit);


static void tm1637_init(tm1637_t *tm1637, GPIO_TypeDef *gpio_clk, uint16_t pin_clk, GPIO_TypeDef *gpio_dat, uint16_t pin_dat);
static void tm1637_brightness(tm1637_t *tm1637, uint8_t brightness_0_to_7);
static void tm1637_write_segment(tm1637_t *tm1637, const uint8_t *segments, uint8_t length, uint8_t pos);
static void tm1637_write_int(tm1637_t *tm1637, int32_t digit, uint8_t pos);
static void tm1637_write_float(tm1637_t *tm1637, float digit, uint8_t floating_digit, uint8_t pos);
static void tm1637_show_zero(tm1637_t *tm1637, bool enable);
static void tm1637_fill(tm1637_t *tm1637, bool enable);



void TM1637Display_Init(void)
{
    tm1637_init(&disp, DISPLAY_TM1637_I2C_SCK_GPIO_PORT, DISPLAY_TM1637_I2C_SCK_PIN, DISPLAY_TM1637_I2C_SDA_GPIO_PORT, DISPLAY_TM1637_I2C_SDA_PIN);
}



static uint8_t Display_ConvertSegmens(uint8_t digit)
{
    uint8_t index;
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
#ifdef CONFIG_DISPLAY_7SEGMENT_EXTRA_LETTER
    else if (digit >= 'g' || digit <= 'z')
    {
        index = digit - 'a' + 10;
        digit = digitToSegment[index];
    }
    else if (digit >= 'G' && digit <= 'Z')
    {
        index = digit - 'A' + 10;
        digit = digitToSegment[index];
    }
#endif
    else if (digit == '-')
    {
        digit = _tm1637_minus;
    }
    else if (digit == ' ')
    {
        digit = _tm1637_empty;
    }
    else if (digit < NUM_OF(digitToSegment))
    {
		/* Byte number */
    	digit = digitToSegment[digit];
    }
    else
    {

        index = 15; /* hardcoded E - ERROR */
        digit = digitToSegment[index];
    }

    return digit;
}


void Display_TM1637_Test(void)
{

    char * string = "0123456789ABCDF";
    uint8_t segments[4] =  {0, 0, 0, 0};

    for (int i = 0; i < 4; i++)
    {
        segments[i] = Display_ConvertSegmens(string[i]);
    }

    //void tm1637_write_segment(tm1637_t *tm1637, const uint8_t *segments, uint8_t length, uint8_t pos)
    tm1637_write_segment(&disp, seg_LOAD, 4, 0);
    DelayMs(1000);
    tm1637_write_segment(&disp, segments, 4, 0);
    DelayMs(1000);
    //tm1637_write_float(&disp, -1.23, 2, 0);
    //HAL_Delay(1000);
    //tm1637_write_int(&disp, 56, 0);

}



void Display_TM1637_DisplayTime(Time_t *time)
{
    uint8_t segments[4] =  {0, 0, 0, 0};

    if (time == NULL)
    {
        return;
    }

    segments[0] = Display_ConvertSegmens(time->hour / 10) | _tm1637_dot;
    segments[1] = Display_ConvertSegmens(time->hour % 10) | _tm1637_dot;
    segments[2] = Display_ConvertSegmens(time->minute / 10) | _tm1637_dot;
    segments[3] = Display_ConvertSegmens(time->minute % 10) | _tm1637_dot;

    tm1637_write_segment(&disp, segments, 4, 0);
    DelayMs(10);
}



void Display_TM1637_Print(char *str)
{
    uint8_t segments[4] =  {0, 0, 0, 0};

    if (str == NULL)
    {
        return;
    }

    // TODO: Check length
    uint8_t len = StringLength(str);
    if (len > 4) {
        len = 4;
    }
    else if (len < 4)
    {
        for (int i = len; i < 4; i++ )
        {
            segments[i] =_tm1637_empty;
        }
    }
    for (int i = 0; i < len; i++)
    {
        segments[i] = Display_ConvertSegmens(str[i]);
    }
    tm1637_write_segment(&disp, segments, 4, 0);
    DelayMs(10);
}



static void tm1637_delay_us(uint8_t delay)
{
    DelayMs(delay/1000);
}



static void tm1637_start(tm1637_t *tm1637)
{
  HAL_GPIO_WritePin(tm1637->gpio_dat, tm1637->pin_dat, GPIO_PIN_RESET);
  tm1637_delay_us(_TM1637_BIT_DELAY);
}



static void tm1637_stop(tm1637_t *tm1637)
{
  HAL_GPIO_WritePin(tm1637->gpio_dat, tm1637->pin_dat, GPIO_PIN_RESET);
  tm1637_delay_us(_TM1637_BIT_DELAY);
  HAL_GPIO_WritePin(tm1637->gpio_clk, tm1637->pin_clk, GPIO_PIN_SET);
  tm1637_delay_us(_TM1637_BIT_DELAY);
  HAL_GPIO_WritePin(tm1637->gpio_dat, tm1637->pin_dat, GPIO_PIN_SET);
  tm1637_delay_us(_TM1637_BIT_DELAY);
}



static uint8_t tm1637_write_byte(tm1637_t *tm1637, uint8_t data)
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



void tm1637_lock(tm1637_t *tm1637)
{
  while (tm1637->lock == 1)
      DelayMs(1);
  tm1637->lock = 1;
}



void tm1637_unlock(tm1637_t *tm1637)
{
  tm1637->lock = 0;
}



static void tm1637_init(tm1637_t *tm1637, GPIO_TypeDef *gpio_clk, uint16_t pin_clk, GPIO_TypeDef *gpio_dat, uint16_t pin_dat)
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



static void tm1637_brightness(tm1637_t *tm1637, uint8_t brightness_0_to_7)
{
  tm1637_lock(tm1637);
  tm1637->brightness = (brightness_0_to_7 & 0x7) | 0x08;
  tm1637_unlock(tm1637);
}



static void tm1637_write_raw(tm1637_t *tm1637, const uint8_t *raw, uint8_t length, uint8_t pos)
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



static void tm1637_write_segment(tm1637_t *tm1637, const uint8_t *segments, uint8_t length, uint8_t pos)
{
  tm1637_lock(tm1637);
  tm1637_write_raw(tm1637, segments, length, pos);
  tm1637_unlock(tm1637);
}



static void tm1637_write_int(tm1637_t *tm1637, int32_t digit, uint8_t pos)
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



static void tm1637_write_float(tm1637_t *tm1637, float digit, uint8_t floating_digit, uint8_t pos)
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



static void tm1637_show_zero(tm1637_t *tm1637, bool enable)
{
  tm1637->show_zero = enable;
}



static void tm1637_fill(tm1637_t *tm1637, bool enable)
{
    if (enable)
        tm1637_write_segment(tm1637, _tm1637_on, 6, 0);
    else
        tm1637_write_segment(tm1637, _tm1637_off, 6, 0);
}


#endif /* #ifdef CONFIG_MODULE_DISPLAY_ENABLE */
