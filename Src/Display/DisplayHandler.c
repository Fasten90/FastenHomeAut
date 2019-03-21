/*
 *    DisplayHandler.c
 *    Created on:   2019-03-16
 *    Author:       Fasten
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     -
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"
#include "Display_SSD1306.h"
#include "DisplayHandler.h"
#include "DebugUart.h"
#include "StringHelper.h"
#include "MemHandler.h"
#include "EscapeSequence.h"


#ifdef CONFIG_TERMINAL_USE_CONEMU
    #define DISPLAY_CHANGED_LINES_ENABLE
#endif



#if defined(CONFIG_MODULE_DISPLAY_ENABLE) || defined(CONFIG_MODULE_DISPLAY_SIMULATOR_ENABLE)

/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

///< Not static, because shall used from Display_ file
uint8_t display_buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8] = { 0 };

#ifdef DISPLAY_CHANGED_LINES_ENABLE
static bool_t Display_ChangedLines[SSD1306_LCDHEIGHT] = { 0 };
bool DisplayHandler_FirstPrintOk = false;
#endif /* DISPLAY_CHANGED_LINES_ENABLE */



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

#if defined(CONFIG_MODULE_DISPLAY_SIMULATOR_ENABLE)
static void SSD1306_display(void);
#endif /* defined(CONFIG_MODULE_DISPLAY_SIMULATOR_ENABLE) */



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

/**
 * @Note    These drawX functions was copied from SSD display driver source files.
 *          These were optimized for the SSD... 128x64
 */

/**
 * @brief    The most basic function, set a single pixel
 */
/* __attribute__( ( section(".data") ) ) */
void DisplayHandler_DrawPixel(uint8_t x, uint8_t y, Display_Color_t color)
{
    if ((x >= SSD1306_LCDWIDTH) || (y >= SSD1306_LCDHEIGHT))
        return;

#ifdef SSD1306_ROTATION_ENABLE
    /* check rotation, move pixel around if necessary */
    uint8_t rotation = 0;    /* TODO: */
    switch (rotation)
    {
        case 1:
            ssd1306_swap(x, y);
            x = SSD1306_LCDWIDTH - x - 1;
            break;
        case 2:
            x = SSD1306_LCDWIDTH - x - 1;
            y = SSD1306_LCDHEIGHT - y - 1;
            break;
        case 3:
            ssd1306_swap(x, y);
            y = SSD1306_LCDHEIGHT - y - 1;
            break;
        default:
            break;
    }
#endif

    /* x is which column */
    switch (color)
    {
        case WHITE:
            display_buffer[x + (y / 8) * SSD1306_LCDWIDTH] |= (1 << (y & 7));
            break;
        case BLACK:
            display_buffer[x + (y / 8) * SSD1306_LCDWIDTH] &= ~(1 << (y & 7));
            break;
        case INVERSE:
            display_buffer[x + (y / 8) * SSD1306_LCDWIDTH] ^= (1 << (y & 7));
            break;
        default:
            break;
    }

#ifdef DISPLAY_CHANGED_LINES_ENABLE
    Display_ChangedLines[y] = true;
#endif /* DISPLAY_CHANGED_LINES_ENABLE */
}



void DisplayHandler_DrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
#ifdef SSD1306_ROTATION_ENABLE
    bool bSwap = false;

    uint8_t rotation = 0;    /* TODO: */
    switch (rotation)
    {
        case 0:
            break;
        case 1:
            /* 90 degree rotation, swap x & y for rotation, then invert x and adjust x for h (now to become w) */
            bSwap = true;
            ssd1306_swap(x, y);
            x = SSD1306_LCDWIDTH - x - 1;
            x -= (h - 1);
            break;
        case 2:
            /* 180 degree rotation, invert x and y - then shift y around for height. */
            x = SSD1306_LCDWIDTH - x - 1;
            y = SSD1306_LCDHEIGHT - y - 1;
            y -= (h - 1);
            break;
        case 3:
            /* 270 degree rotation, swap x & y for rotation, then invert y */
            bSwap = true;
            ssd1306_swap(x, y);
            y = SSD1306_LCDHEIGHT - y - 1;
            break;
        default:
            break;
    }

    if (bSwap)
    {
        DisplayHandler_DrawFastHLineInternal(x, y, h, color);
    }
    else
    {
        DisplayHandler_DrawFastVLineInternal(x, y, h, color);
    }
#else
    DisplayHandler_DrawFastVLineInternal(x, y, h, color);

    #ifdef DISPLAY_CHANGED_LINES_ENABLE
    Display_ChangedLines[y] = true;
    #endif /* DISPLAY_CHANGED_LINES_ENABLE */
#endif
}



void DisplayHandler_DrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
#ifdef SSD1306_ROTATION_ENABLE
    bool bSwap = false;
    uint8_t rotation = 0;    /* TODO: */
    switch (rotation)
    {
        case 0:
            /* 0 degree rotation, do nothing */
            break;
        case 1:
            /* 90 degree rotation, swap x & y for rotation, then invert x */
            bSwap = true;
            ssd1306_swap(x, y);
            x = SSD1306_LCDWIDTH - x - 1;
            break;
        case 2:
            /* 180 degree rotation, invert x and y - then shift y around for height. */
            x = SSD1306_LCDWIDTH - x - 1;
            y = SSD1306_LCDHEIGHT - y - 1;
            x -= (w - 1);
            break;
        case 3:
            /* 270 degree rotation, swap x & y for rotation, then invert y  and adjust y for w (not to become h) */
            bSwap = true;
            ssd1306_swap(x, y);
            y = SSD1306_LCDHEIGHT - y - 1;
            y -= (w - 1);
            break;
        default:
            break;
    }

    if (bSwap)
    {
        DisplayHandler_DrawFastVLineInternal(x, y, w, color);
    }
    else
    {
        DisplayHandler_DrawFastHLineInternal(x, y, w, color);
    }
#else
    DisplayHandler_DrawFastHLineInternal(x, y, w, color);

    #ifdef DISPLAY_CHANGED_LINES_ENABLE
    Display_ChangedLines[y] = true;
    #endif /* DISPLAY_CHANGED_LINES_ENABLE */
#endif
}



void DisplayHandler_DrawFastVLineInternal(int16_t x, int16_t __y, int16_t __h,
        uint16_t color)
{

    /* do nothing if we're off the left or right side of the screen */
    if (x < 0 || x >= SSD1306_LCDWIDTH)
    {
        return;
    }

    /* make sure we don't try to draw below 0 */
    if (__y < 0)
    {
        /* __y is negative, this will subtract enough from __h to account for __y being 0 */
        __h += __y;
        __y = 0;

    }

    /* make sure we don't go past the height of the display */
    if ((__y + __h) > SSD1306_LCDHEIGHT)
    {
        __h = (SSD1306_LCDHEIGHT - __y);
    }

    /* if our height is now negative, punt */
    if (__h <= 0)
    {
        return;
    }

    /* this display doesn't need ints for coordinates, use local byte registers for faster juggling */
    register uint8_t y = __y;
    register uint8_t h = __h;

    /* set up the pointer for fast movement through the buffer */
    register uint8_t *pBuf = display_buffer;
    /* adjust the buffer pointer for the current row */
    pBuf += ((y / 8) * SSD1306_LCDWIDTH);
    /* and offset x columns in */
    pBuf += x;

    /* do the first partial byte, if necessary - this requires some masking */
    register uint8_t mod = (y & 7);
    if (mod)
    {
        /* mask off the high n bits we want to set */
        mod = 8 - mod;

        /* note - lookup table results in a nearly 10% performance improvement in fill* functions */
        /* register uint8_t mask = ~(0xFF >> (mod)); */
        static uint8_t premask[8] =
        { 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
        register uint8_t mask = premask[mod];

        /* adjust the mask if we're not going to reach the end of this byte */
        if (h < mod)
        {
            mask &= (0XFF >> (mod - h));
        }

        switch (color)
        {
            case WHITE:
                *pBuf |= mask;
                break;
            case BLACK:
                *pBuf &= ~mask;
                break;
            case INVERSE:
                *pBuf ^= mask;
                break;
            default:
                break;
        }

        /* fast exit if we're done here! */
        if (h < mod)
        {
            return;
        }

        h -= mod;

        pBuf += SSD1306_LCDWIDTH;
    }

    /* write solid bytes while we can - effectively doing 8 rows at a time */
    if (h >= 8)
    {
        if (color == INVERSE)
        { /* separate copy of the code so we don't impact performance of the black/white write version with an extra comparison per loop */
            do
            {
                *pBuf = ~(*pBuf);

                /* adjust the buffer forward 8 rows worth of data */
                pBuf += SSD1306_LCDWIDTH;

                /* adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now) */
                h -= 8;
            } while (h >= 8);
        }
        else
        {
            /* store a local value to work with */
            register uint8_t val = (color == WHITE) ? 255 : 0;

            do
            {
                /* write our value in */
                *pBuf = val;

                /* adjust the buffer forward 8 rows worth of data */
                pBuf += SSD1306_LCDWIDTH;

                /* adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now) */
                h -= 8;
            } while (h >= 8);
        }
    }

    /* now do the final partial byte, if necessary */
    if (h)
    {
        mod = h & 7;
        /* this time we want to mask the low bits of the byte, vs the high bits we did above */
        /* register uint8_t mask = (1 << mod) - 1; */
        /* note - lookup table results in a nearly 10% performance improvement in fill* functions */
        static uint8_t postmask[8] =
        { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
        register uint8_t mask = postmask[mod];
        switch (color)
        {
            case WHITE:
                *pBuf |= mask;
                break;
            case BLACK:
                *pBuf &= ~mask;
                break;
            case INVERSE:
                *pBuf ^= mask;
                break;
            default:
                break;
        }
    }
}



void DisplayHandler_DrawFastHLineInternal(int16_t x, int16_t y, int16_t w,
        uint16_t color)
{
    /* Do bounds/limit checks */
    if (y < 0 || y >= SSD1306_LCDHEIGHT)
    {
        return;
    }

    /* make sure we don't try to draw below 0 */
    if (x < 0)
    {
        w += x;
        x = 0;
    }

    /* make sure we don't go off the edge of the display */
    if ((x + w) > SSD1306_LCDWIDTH)
    {
        w = (SSD1306_LCDWIDTH - x);
    }

    /* if our width is now negative, punt */
    if (w <= 0)
    {
        return;
    }

    /* set up the pointer for  movement through the buffer */
    register uint8_t *pBuf = display_buffer;
    /* adjust the buffer pointer for the current row */
    pBuf += ((y / 8) * SSD1306_LCDWIDTH);
    /* and offset x columns in */
    pBuf += x;

    register uint8_t mask = 1 << (y & 7);

    switch (color)
    {
        case WHITE:
            while (w--)
            {
                *pBuf++ |= mask;
            }
            break;
        case BLACK:
            mask = ~mask;
            while (w--)
            {
                *pBuf++ &= mask;
            }
            break;
        case INVERSE:
            while (w--)
            {
                *pBuf++ ^= mask;
            }
            break;
        default:
            break;
    }
}



/**
 * @brief    Draw image (put pixel to buffer image)
 */
void DisplayHandler_DrawImage(uint8_t setx, uint8_t sety, uint8_t sizex, uint8_t sizey, uint8_t *img)
{
    /* TODO: Only 8n size can be used */
    uint8_t i;
    uint8_t j;

    /* Step on a row (left to right) */
    for (i = 0; i < sizex; i++)
    {
        /* Step on a column (a 1x8 column is a byte) */
        for (j = 0; j < sizey/8; j++)
        {
            /* Copy an byte */
            display_buffer[sety/8*SSD1306_LCDWIDTH + setx + i + j*SSD1306_LCDWIDTH] =
                    img[j*sizex + i];
        }
    }
}



/**
 *    @brief    Clear everything
 */
void DisplayHandler_ClearDisplay(void)
{
    memset(display_buffer, 0, (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8));

#if defined(CONFIG_MODULE_DISPLAY_SIMULATOR_ENABLE)
    DebugUart_SendMessage(ESCAPE_ERASE_CLS);
#endif

#ifdef DISPLAY_CHANGED_LINES_ENABLE
    memset(Display_ChangedLines, 1, NUM_OF(Display_ChangedLines) * sizeof(Display_ChangedLines[0]));
#endif /* DISPLAY_CHANGED_LINES_ENABLE */
}



inline void DisplayHandler_ShowDisplay(void)
{
    SSD1306_display();
}



#if defined(CONFIG_MODULE_DISPLAY_SIMULATOR_ENABLE)
/**
 * This is only for Windows display simulator
 */
static void SSD1306_display(void)
{
#if defined(CONFIG_TERMINAL_USE_CMD)
    /* Originally it was for the clean all display */
    DebugUart_SendMessage(ESCAPE_ERASE_CLS);
#elif defined(CONFIG_TERMINAL_USE_CONEMU)
    char_t escape[6] = { 0 };
    uint8_t y_start = 1;
    if (DisplayHandler_FirstPrintOk)
    {
        y_start = 2; /* Shall step 2. line because the top frame */
    }
    /* ESC [ row d     Moves the cursor to line row (absolute, 1-based). */
    usprintf(escape, "\x1B" "[" "%dd", y_start);
    DebugUart_SendMessage(escape);
#else
    #warning "Unknown terminal used"
#endif

    /* There is no HW to show the screen. Instead of that print it */
    DisplayHandler_SendOnTerminal();

    #ifdef DISPLAY_CHANGED_LINES_ENABLE
    memset(Display_ChangedLines, 0, NUM_OF(Display_ChangedLines) * sizeof(Display_ChangedLines[0]));
    #endif /* DISPLAY_CHANGED_LINES_ENABLE */
}
#endif



#if defined(CONFIG_MODULE_DISPLAY_TEST_WITH_TERMINAL) || defined(CONFIG_MODULE_DISPLAY_SIMULATOR_ENABLE)

/**
 * @brief    Print Display (Actual screen) on debug port
 */
void DisplayHandler_SendOnTerminal(void)
{
    uint8_t x;
    uint8_t y;

    /* Print top frame row */
  #ifdef DISPLAY_CHANGED_LINES_ENABLE
    if (!DisplayHandler_FirstPrintOk)
    {
  #endif /* DISPLAY_CHANGED_LINES_ENABLE */
        DebugUart_SendChar('+');
        for (x = 0; x < SSD1306_LCDWIDTH; x++) DebugUart_SendChar('-');
        DebugUart_SendLine("+");
  #ifdef DISPLAY_CHANGED_LINES_ENABLE
    }
  #endif /* DISPLAY_CHANGED_LINES_ENABLE */

    /* Print every row */
    for (y = 0; y < SSD1306_LCDHEIGHT; y++)
    {
        #ifdef DISPLAY_CHANGED_LINES_ENABLE
        if (Display_ChangedLines[y])
        {
        #endif /* DISPLAY_CHANGED_LINES_ENABLE */
        DebugUart_SendChar('|');

        /* Print every pixel on row (=column) */
        for (x = 0; x < SSD1306_LCDWIDTH; x++)
        {
            /* Print pixel */
            char ch = ' ';
            if (display_buffer[x + (y / 8) * SSD1306_LCDWIDTH] & (1 << (y & 7)))
            {
                ch = '#';
            }
            DebugUart_SendChar(ch);
        }

        /* |\r\n */
        DebugUart_SendLine("|");

        #ifdef DISPLAY_CHANGED_LINES_ENABLE
        Display_ChangedLines[y] = false;
        }
        /* else - that line not changed */
        else
        {
            #if defined(CONFIG_TERMINAL_USE_CONEMU)
            /* ESC [ row d     Moves the cursor to line row (absolute, 1-based). */
            char_t escape[6] = { 0 };
            usprintf(escape, "\x1B" "[" "%dd", y + 3);
            DebugUart_SendMessage(escape);
            #else
            #warning "This terminal not supported"
            #endif
        }
        #endif /* DISPLAY_CHANGED_LINES_ENABLE */
    }

    /* Print bottom frame row */
  #ifdef DISPLAY_CHANGED_LINES_ENABLE
    if (!DisplayHandler_FirstPrintOk)
    {
  #endif /* DISPLAY_CHANGED_LINES_ENABLE */
        DebugUart_SendChar('+');
        for (x = 0; x < SSD1306_LCDWIDTH; x++) DebugUart_SendChar('-');
        DebugUart_SendLine("+");
  #ifdef DISPLAY_CHANGED_LINES_ENABLE
        DisplayHandler_FirstPrintOk = true;
    }
  #endif /* DISPLAY_CHANGED_LINES_ENABLE */
}
#endif /* defined(CONFIG_MODULE_DISPLAY_TEST_WITH_TERMINAL) || defined(CONFIG_MODULE_DISPLAY_SIMULATOR_ENABLE) */



#endif /* defined(CONFIG_MODULE_DISPLAY_ENABLE) || defined(CONFIG_MODULE_DISPLAY_SIMULATOR_ENABLE) */
