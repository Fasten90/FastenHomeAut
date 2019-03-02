/*
 *    Display.c
 *    Created on:   2017-04-02
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Display handling
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"

#include "Display_SSD1306.h"
#include "DisplayImages.h"
#include "ErrorHandler.h"
#include "Timing.h"
#include "StringHelper.h"
#include "Display.h"

#ifdef CONFIG_MODULE_DISPLAY_ENABLE

#ifdef CONFIG_DISPLAY_FONT8X5_ENABLE
#include "Font8x5.h"
#endif
#ifdef CONFIG_DISPLAY_FONT12X8_ENABLE
#include "Font12x8.h"
#endif
#ifdef CONFIG_DISPLAY_FONT32X20_ENABLE
#include "Font32x20.h"
#endif

#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_CLOCK
#include "DateTime.h"
#endif
#ifdef CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK
#include "Logic.h"
#endif


#define DISPLAY_FONT32X20_CLOCK_START_POSITION_X        ( ( 128 - ( 5*20 ) ) / 2 )
#define DISPLAY_FONT32X20_CLOCK_START_POSITION_Y        ( ( 64 - ( 1*32 ) ) / 2 )



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

const FontFormat_t Display_NoFormat = { 0 };



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static void Display_FillRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/



/**
 * @brief    Print ASCII text string to display
 */
void Display_PrintString(const char *str, uint8_t line, FontType_t font, FontFormat_t format)
{

    uint8_t i;
    uint8_t index_offset = 0;

    // Handle "centering" text
    if (format.Format_Center)
    {
        format.Format_Center = 0;
        uint8_t length = StringLength(str);
        uint8_t fontWidth = 0;
        switch (font)
        {
#ifdef CONFIG_DISPLAY_FONT8X5_ENABLE
            case Font_8x5:
                fontWidth = 5;
                break;
#endif
#ifdef CONFIG_DISPLAY_FONT12X8_ENABLE
            case Font_12x8:
                fontWidth = 8;
                break;
#endif
#ifdef CONFIG_DISPLAY_FONT32X20_ENABLE
            case Font_32x20:
                fontWidth = 20;
                break;
#endif
            case Font_Unknown:
            case Font_Count:
            default:
                break;
        }
        // Calculate empty/2 space (index, not pixel!)
        index_offset = (DISPLAY_WIDTH - length * fontWidth)/2/fontWidth;
    }

    // Print text
    for (i = 0; str[i]; i++)
    {
        switch (font)
        {
#ifdef CONFIG_DISPLAY_FONT8X5_ENABLE
            case Font_8x5:
                // Print ASCII character (0-127)
                Display_PrintFont8x5(str[i], i+index_offset, line, format);
                break;
#endif
#ifdef CONFIG_DISPLAY_FONT12X8_ENABLE
            case Font_12x8:
                // Print ASCII character (0-127)
                Display_PrintFont12x8(str[i], i+index_offset, line, format);
                break;
#endif
#ifdef CONFIG_DISPLAY_FONT32X20_ENABLE
            case Font_32x20:
                // Print numbers (for Clock)
                Display_PrintFont32x20(str[i], i+index_offset,
                    DISPLAY_FONT32X20_CLOCK_START_POSITION_X,
                    DISPLAY_FONT32X20_CLOCK_START_POSITION_Y,
                    format);
                break;
#endif
            case Font_Unknown:
            case Font_Count:
            default:
                break;
        }
    }
}



#ifdef CONFIG_DISPLAY_FONT8X5_ENABLE
/**
 * @brief    Print a 8x5 pixel (very small) character to display
 * @param    chr        - which character (ASCII - 0-127)
 * @param    index    - column
 * @param    line    - line / row
 * @param    format    - format of text
 */
//__attribute__( ( section(".data") ) )
void Display_PrintFont8x5(uint8_t chr, uint8_t index, uint8_t line, FontFormat_t format)
{
    // 8x5 pixel font
    register const uint8_t indexStart = index * (FONT_8X5_WIDTH + 1);
    register const uint8_t lineStart = (line * (FONT_8X5_HEIGHT + 1));
    register uint8_t i;
    register uint8_t j;

    // TODO: Handle "underline"
    // Step on columns
    for (i = 0; i < FONT_8X5_WIDTH; i++)
    {
        // Step on rows from top to bottom
        register uint8_t storedChar = Font8x5[chr][i];
        register uint8_t x = indexStart + i;
        for (j = 0; j < FONT_8X5_HEIGHT; j++)
        {
            // Draw pixel to "screen"
            register uint8_t y = lineStart + j;
            if (storedChar & (1 << (7-j)))
            {
                SSD1306_drawPixel(x, y,
                        format.Format_Inverse ? BLACK : WHITE);
            }
            else
            {
                SSD1306_drawPixel(x, y,
                        format.Format_Inverse ? WHITE : BLACK);
            }
        }
    }
}
#endif



#ifdef CONFIG_DISPLAY_FONT12X8_ENABLE
/**
 * @brief    Print a 12x8 pixel character to display
 * @param    chr        - which character (ASCII - 0-127)
 * @param    index    - column
 * @param    line    - line / row
 * @param    format    - format of text
 */
//__attribute__( ( section(".data") ) )
void Display_PrintFont12x8(uint8_t chr, uint8_t index, uint8_t line, FontFormat_t format)
{
    // 12x8 pixel font
    register const uint8_t lineStart = line * (FONT_12X8_HEIGHT + 1);
    register const uint8_t indexStart = index * (FONT_12X8_WIDTH + 1);
    register uint8_t i;

    // Step on rows from top to bottom
    for (i = 0; i < FONT_12X8_HEIGHT; i++)
    {
        // Step on column from left to right
        register uint8_t storedChar = Font12x8[chr][i];
        register uint8_t y = lineStart + i;
        register uint8_t j;

        for (j = 0; j < FONT_12X8_WIDTH; j++)
        {
            // Draw pixel to "screen"
            register uint8_t x = indexStart + j;
            if (storedChar & (1 << (7-j)))
            {
                SSD1306_drawPixel(x, y,
                        format.Format_Inverse ? BLACK : WHITE);
            }
            else
            {
                SSD1306_drawPixel(x, y,
                        format.Format_Inverse ? WHITE : BLACK);
            }
        }
    }
}
#endif



#ifdef CONFIG_DISPLAY_FONT32X20_ENABLE
/**
 * @brief    Print 32x20 font
 * @param    chr            - which character (ASCII - '0'-'9', ':')
 * @param    index        - index of character
 * @param    startposx    - line / row
 * @param    startposy    - line / row
 * @param    format        - format of text
 * @note    Now it is only usable for Hour:Minute displaying
 */
void Display_PrintFont32x20(uint8_t chr, uint8_t index, uint8_t startposx, uint8_t startposy, FontFormat_t format)
{
    // 32x20 pixel font
    uint8_t i;
    uint8_t j;

    // Character selecting
    if (chr >= '0' && chr <= '9')
    {
        // 0-9 - Dependent with Font store (32x20)
        chr -= '0';
    }
    else if (chr == ':')
    {
        // ':'    - Dependent with Font store (32x20)
        chr = 10;
    }

    // Step on columns
    for (i = 0; i < FONT_32X20_WIDTH; i++)
    {
        // Step on rows from top to bottom
        uint8_t x = startposx + index * (FONT_32X20_WIDTH + 1) + i;
        for (j = 0; j < FONT_32X20_HEIGHT; j++)
        {
            // Draw pixel to "screen"
            uint8_t y = startposy + j;
            if (Font32x20[chr][i] & (1 << (31-j)))
            {
                SSD1306_drawPixel(x, y,
                        format.Format_Inverse ? BLACK : WHITE);
            }
            else
            {
                SSD1306_drawPixel(x, y,
                        format.Format_Inverse ? WHITE : BLACK);
            }
        }
    }
}
#endif



/**
 * @brief    Clear display - make empty screen
 */
inline void Display_Clear(void)
{
    SSD1306_clearDisplay();
}



/**
 * @brief    Refresh Display
 */
inline void Display_Activate(void)
{
    SSD1306_display();
/*
#ifdef CONFIG_MODULE_DISPLAY_TEST_WITH_TERMINAL
    Display_SendOnTerminal();
#endif
*/
}



/**
 * @brief    Fill rectangle with color
 */
static void Display_FillRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color)
{
    uint8_t i;
    uint8_t j;

    // Step on columns
    for (i = x; i <= x + width; i++)
    {
        // Step on rows
        for (j = y; j <= y + height; j++)
        {
            SSD1306_drawPixel(i, j, color);
        }
    }

}



/*------------------------------------------------------------------------------
 *                              Loading screen
 *----------------------------------------------------------------------------*/



void Display_LoadingInit(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    // Clear rectangle
    Display_FillRectangle(x, y, width, height, BLACK);

    // Init empty rectangle
    SSD1306_drawFastVLine(x, y, height, WHITE);
    SSD1306_drawFastVLine(x+width, y, height, WHITE);
    SSD1306_drawFastHLine(x, y, width, WHITE);
    SSD1306_drawFastHLine(x, y+height, width, WHITE);

    // 0 percent
}



void Display_LoadingPercent(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t percent)
{
    // Fill rectangle
    uint8_t percent_real = (float)(width-4)*percent/100;
    Display_FillRectangle(x+2, y+2, percent_real, height-4, WHITE);
}



void Display_TestLoading(uint8_t percent)
{
    static uint8_t x = 16;
    static uint8_t y = 56;
    static uint8_t width = 96;
    static uint8_t height = 8;

    if (percent == 0)
    {
        //Display_Clear();
        Display_LoadingInit(x,  y,  width, height);
    }
    Display_LoadingPercent(x, y, width, height, percent);

    Display_Activate();
}



void Display_ChargeLoading(uint8_t percent)
{
    static const uint8_t x = 100;
    static const uint8_t y = 4;
    static uint8_t width = 24;
    static uint8_t height = 8;

    Display_LoadingInit(x,  y,  width, height);

    Display_LoadingPercent(x, y, width, height, percent);

    //Display_Activate();
}



/*------------------------------------------------------------------------------
 *                              Car images
 *----------------------------------------------------------------------------*/



#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_SCREEN
/**
 * @brief    Load "Car" image to screen
 */
void Display_LoadCarImage(void)
{
    Display_Clear();

    SSD1306_drawImage(8, 8, 120, 48, (uint8_t *)Display_Image_Car);

    Display_Activate();
}



/**
 * @brief    Change "Car's wheels"
 */
void Display_ChangeCarImage(void)
{
    static uint8_t CarActualStateCnt = 0;
    uint8_t *img = NULL;

    if ((CarActualStateCnt % 3) == 0)
    {
        // 1.
        img = (uint8_t *)Display_Image_CarWheel_1_16x16;
    }
    else if ((CarActualStateCnt % 3) == 1)
    {
        // 2.
        img = (uint8_t *)Display_Image_CarWheel_2_16x16;
    }
    else
    {
        // 3.
        img = (uint8_t *)Display_Image_CarWheel_3_16x16;
    }

    // Draw 2 wheel
    SSD1306_drawImage(24, 40, 16, 16, img);
    SSD1306_drawImage(96, 40, 16, 16, img);

    CarActualStateCnt++;

    Display_Activate();
}
#endif



#if defined(CONFIG_FUNCTION_DISPLAY_SHOW_CLOCK) && defined(CONFIG_DISPLAY_CLOCK_LARGE)
/**
 * @brief    Display time (HH:MM) (large)
 */
void Display_ShowLargeClock(Time_t *time)
{

    // Show clock: large version
    // Set hour
    Display_PrintFont32x20(time->hour/10, 0,
            DISPLAY_FONT32X20_CLOCK_START_POSITION_X,
            DISPLAY_FONT32X20_CLOCK_START_POSITION_Y,
            Display_NoFormat);

    Display_PrintFont32x20(time->hour%10, 1,
            DISPLAY_FONT32X20_CLOCK_START_POSITION_X,
            DISPLAY_FONT32X20_CLOCK_START_POSITION_Y,
            Display_NoFormat);

    // ':'
    Display_PrintFont32x20(':', 2,
            DISPLAY_FONT32X20_CLOCK_START_POSITION_X,
            DISPLAY_FONT32X20_CLOCK_START_POSITION_Y,
            Display_NoFormat);

    // Set minute
    Display_PrintFont32x20(time->minute/10, 3,
            DISPLAY_FONT32X20_CLOCK_START_POSITION_X,
            DISPLAY_FONT32X20_CLOCK_START_POSITION_Y,
            Display_NoFormat);

    Display_PrintFont32x20(time->minute%10, 4,
            DISPLAY_FONT32X20_CLOCK_START_POSITION_X,
            DISPLAY_FONT32X20_CLOCK_START_POSITION_Y,
            Display_NoFormat);

#ifndef CONFIG_FUNCTION_DISPLAY_MENU
    // Refresh display
    Display_Activate();
#endif
}
#endif



#ifdef CONFIG_FUNCTION_DISPLAY_CHANGE_CLOCK
/**
 * @brief    Display time (HH:MM)
 */
void Display_ShowLargeClockHalf(Time_t *time, DisplayClock_ChangeState_t displayState)
{
    // Clear
    Display_Clear();

    if (displayState == DisplayClock_Hour)
    {
        // Set hour
        Display_PrintFont32x20(time->hour/10, 0,
                DISPLAY_FONT32X20_CLOCK_START_POSITION_X,
                DISPLAY_FONT32X20_CLOCK_START_POSITION_Y,
                Display_NoFormat);

        Display_PrintFont32x20(time->hour%10, 1,
                DISPLAY_FONT32X20_CLOCK_START_POSITION_X,
                DISPLAY_FONT32X20_CLOCK_START_POSITION_Y,
                Display_NoFormat);
    }

    // ':'
    Display_PrintFont32x20(':', 2,
            DISPLAY_FONT32X20_CLOCK_START_POSITION_X,
            DISPLAY_FONT32X20_CLOCK_START_POSITION_Y,
            Display_NoFormat);

    if (displayState == DisplayClock_Minute)
    {
        // Set minute
        Display_PrintFont32x20(time->minute/10, 3,
                DISPLAY_FONT32X20_CLOCK_START_POSITION_X,
                DISPLAY_FONT32X20_CLOCK_START_POSITION_Y,
                Display_NoFormat);

        Display_PrintFont32x20(time->minute%10, 4,
                DISPLAY_FONT32X20_CLOCK_START_POSITION_X,
                DISPLAY_FONT32X20_CLOCK_START_POSITION_Y,
                Display_NoFormat);
    }

#ifndef CONFIG_FUNCTION_DISPLAY_MENU
    // Refresh display
    Display_Activate();
#endif
}
#endif



#if defined(CONFIG_FUNCTION_DISPLAY_SHOW_CLOCK) &&  defined(CONFIG_DISPLAY_CLOCK_SMALL)
/**
 * @brief    Display time (HH:MM:SS) (small)
 */
void Display_ShowSmallClock(Time_t *time)
{
    char clock[10];
    usprintf(clock, "%02d:%02d:%02d", time->hour, time->minute, time->second);
    Display_PrintString(clock, 0, Font_12x8, NO_FORMAT);

#ifndef CONFIG_FUNCTION_DISPLAY_MENU
    // Refresh display
    Display_Activate();
#endif
}
#endif



#ifdef CONFIG_MODULE_DISPLAY_TEST

#ifdef CONFIG_DISPLAY_FONT8X5_ENABLE
/**
 * @brief    Test 8x5 fonts
 */
void Display_Test8x5Font(void)
{
    Display_Clear();

    Display_PrintString("Text example", 0, Font_8x5, NO_FORMAT);
    Display_PrintString("Sari <3", 1, Font_8x5, NO_FORMAT);
    Display_PrintString("0123456789", 2, Font_8x5, NO_FORMAT);
    Display_PrintString("abcdefghijklm", 3, Font_8x5, NO_FORMAT);
    Display_PrintString("nopqrstuvwxyz", 4, Font_8x5, NO_FORMAT);
    Display_PrintString(",.;?-*_()[]{}&", 5, Font_8x5, NO_FORMAT);
    Display_PrintString("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 6, Font_8x5, NO_FORMAT);

    Display_Activate();
}
#endif



#ifdef CONFIG_DISPLAY_FONT12X8_ENABLE
/**
 * @brief    Test 12x8 fonts
 */
void Display_Test12x8Font(void)
{
    Display_Clear();

    Display_PrintString("0123456789", 0, Font_12x8, NO_FORMAT);
    Display_PrintString("abcdefghijklm", 1, Font_12x8, NO_FORMAT);
    Display_PrintString("nopqrtsuvwxyz", 2, Font_12x8, NO_FORMAT);
    Display_PrintString(",.;?-*_()[]{}&", 3, Font_12x8, NO_FORMAT);
    Display_PrintString("ABCDEFGHIJKLMNOPQ", 4, Font_12x8, NO_FORMAT);

    Display_Activate();
}
#endif



#ifdef CONFIG_DISPLAY_FONT32X20_ENABLE
/**
 * @brief    Test 32x20 fonts
 */
void Display_Test32x20Font(void)
{
    Display_Clear();

    Display_PrintString("12:34", 0, Font_32x20, NO_FORMAT);

    Display_Activate();
}
#endif



#ifdef CONFIG_FUNCTION_DISPLAY_SHOW_CLOCK
/**
 * @brief    Test Clock show
 */
void Display_TestClock(void)
{
    uint16_t i;
    DateTime_t dateTime = { { 17, 5, 14 }, { 0, 0, 0 } };

    Display_Clear();


    for (i = 0; i < 24*60; i++)
    {
        Display_ShowLargeClock(&dateTime.time);
        DelayMs(300);
        // Step 1 minute
        DateTime_StepMoreSecond(&dateTime, 60);
    }

}
#endif

#endif    // #ifdef CONFIG_MODULE_DISPLAY_TEST



#endif    // #ifdef CONFIG_MODULE_DISPLAY_ENABLE
