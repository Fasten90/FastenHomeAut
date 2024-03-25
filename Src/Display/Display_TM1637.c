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


/* Arduino example sketch to display DHT11 temperature readings
  on a TM1637 4-digit 7-segment display.
  More info: www.www.makerguides.com */


static uint8_t m_pinClk;
static uint8_t m_pinDIO;
static uint8_t m_brightness;
static unsigned int m_bitDelay;




static const uint8_t minusSegments = 0b01000000;

TM1637Display(uint8_t pinClk, uint8_t pinDIO, unsigned int bitDelay)
{
    // Copy the pin numbers
    m_pinClk = pinClk;
    m_pinDIO = pinDIO;
    m_bitDelay = bitDelay;

    // Set the pin direction and default value.
    // Both pins are set as inputs, allowing the pull-up resistors to pull them up
    pinMode(m_pinClk, INPUT);
    pinMode(m_pinDIO, INPUT);
    digitalWrite(m_pinClk, LOW);
    digitalWrite(m_pinDIO, LOW);
}

void setBrightness(uint8_t brightness, bool on)
{
    m_brightness = (brightness & 0x7) | (on? 0x08 : 0x00);
}

void setSegments(const uint8_t segments[], uint8_t length, uint8_t pos)
{
    // Write COMM1
    start();
    writeByte(TM1637_I2C_COMM1);
    stop();

    // Write COMM2 + first digit address
    start();
    writeByte(TM1637_I2C_COMM2 + (pos & 0x03));

    // Write the data bytes
    for (uint8_t k=0; k < length; k++)
      writeByte(segments[k]);

    stop();

    // Write COMM3 + brightness
    start();
    writeByte(TM1637_I2C_COMM3 + (m_brightness & 0x0f));
    stop();
}

void clear()
{
    uint8_t data[] = { 0, 0, 0, 0 };
    setSegments(data);
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
    if (base < 0) {
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
    delayMicroseconds(m_bitDelay);
}

void start()
{
  pinMode(m_pinDIO, OUTPUT);
  bitDelay();
}

void stop()
{
    pinMode(m_pinDIO, OUTPUT);
    bitDelay();
    pinMode(m_pinClk, INPUT);
    bitDelay();
    pinMode(m_pinDIO, INPUT);
    bitDelay();
}

bool writeByte(uint8_t b)
{
  uint8_t data = b;

  // 8 Data Bits
  for(uint8_t i = 0; i < 8; i++) {
    // CLK low
    pinMode(m_pinClk, OUTPUT);
    bitDelay();

    // Set data bit
    if (data & 0x01)
      pinMode(m_pinDIO, INPUT);
    else
      pinMode(m_pinDIO, OUTPUT);

    bitDelay();

    // CLK high
    pinMode(m_pinClk, INPUT);
    bitDelay();
    data = data >> 1;
  }

  // Wait for acknowledge
  // CLK to zero
  pinMode(m_pinClk, OUTPUT);
  pinMode(m_pinDIO, INPUT);
  bitDelay();

  // CLK to high
  pinMode(m_pinClk, INPUT);
  bitDelay();
  uint8_t ack = digitalRead(m_pinDIO);
  if (ack == 0)
    pinMode(m_pinDIO, OUTPUT);


  bitDelay();
  pinMode(m_pinClk, OUTPUT);
  bitDelay();

  return ack;
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




void setup() {

    // Create display object of type TM1637Display:
    TM1637Display display = TM1637Display(CLK, DIO);

  // Set the display brightness (0-7):
  display.setBrightness(0);
  // Clear the display:
  display.clear();
  // Setup sensor:
  dht.begin();
  // Begin serial communication at a baud rate of 9600:
  Serial.begin(9600);
  // Wait for console opening:
  delay(2000);
}

void loop() {
  // Read the temperature as Celsius and Fahrenheit:
  temperature_celsius = dht.readTemperature();
  temperature_fahrenheit = dht.readTemperature(true);
  // Print the temperature to the Serial Monitor:
  Serial.println(temperature_celsius);
  Serial.println(temperature_fahrenheit);

  // Show the temperature on the TM1637 display:
  display.showNumberDec(temperature_celsius, false, 2, 0);
  display.setSegments(celsius, 2, 2);

  delay(2000);

  display.showNumberDec(temperature_fahrenheit, false, 2, 0);
  display.setSegments(fahrenheit, 2, 2);

  delay(2000);
}



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

