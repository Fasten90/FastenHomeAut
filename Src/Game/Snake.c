/*
 *        Snake.c
 *        Created on:        2017-09-06
 *        Author:            Vizi Gábor
 *        E-mail:            vizi.gabor90@gmail.com
 *        Function:        Snake game
 *        Target:            STM32Fx
 *        Version:        v1
 *        Last modified:    2017-09-06
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "GenericTypeDefs.h"

#ifdef CONFIG_FUNCTION_GAME_SNAKE

#include "Display.h"
#include "Display_SSD1306.h"
#include "DebugList.h"
#include "Debug.h"
#include "String.h"
#include "Logic.h"
#include "Snake.h"
#include "MathHelper.h"
#include "StringHelper.h"



/*------------------------------------------------------------------------------
 *  Type definitions
 *----------------------------------------------------------------------------*/

typedef struct
{
    uint8_t x;
    uint8_t y;
} SnakeCoord_t;



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/

// TODO: Optimizing
#define SNAKE_POINT_PIXEL_SIZE    (4)

#define SNAKE_HEADER_HEIGHT        (10)

#define SNAKE_SIZE_X            (DISPLAY_WIDTH/SNAKE_POINT_PIXEL_SIZE)
#define SNAKE_SIZE_Y            ((DISPLAY_HEIGHT-SNAKE_HEADER_HEIGHT)/SNAKE_POINT_PIXEL_SIZE)


#define PRINT(...)                Debug_Printf(Debug_GameSnake, __VA_ARGS__)


uint8_t Snake_Matrix[SNAKE_SIZE_X][SNAKE_SIZE_Y] = { 0 };

SnakeCoord_t Snake_StartPoint = { .x = 0, .y = 0 };
SnakeStep_t Snake_LastStep = Step_Unknown;

SnakeCoord_t Snake_EndPoint = { .x = 0, .y = 0 };

uint16_t Snake_Score = 0;
bool Snake_CaptureEvent = false;

bool Snake_GameInProgress = false;



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static bool Snake_StepIsValid(SnakeStep_t lastStep, SnakeStep_t actualStep);
static void Snake_AddNewCoord(SnakeStep_t step);
static bool Snake_CheckCollision(SnakeStep_t step);
static SnakeCoord_t Snake_ConvertStepToCoordinate(SnakeStep_t step);
static void Snake_CaptureGift(SnakeCoord_t coord);
static void Snake_PutNewGift(void);
static void Snake_DeleteLastPoint(void);
static void Snake_FinishLose(void);
static uint32_t Snake_CalculateFreePoints(void);
static uint32_t Snake_GenerateRandomPosition(uint32_t max);
static void Snake_PutGiftPosition(uint32_t giftPos);
static void Snake_DrawGiftToPoint(uint16_t x, uint16_t y);
static void Snake_DrawPoints(uint16_t x, uint16_t y);


/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/


/**
 * \brief    Initialize Snake game (start position, etc.)
 */
void Snake_Init(void)
{
    Snake_StartPoint.x = SNAKE_SIZE_X/2 + 1;
    Snake_StartPoint.y = SNAKE_SIZE_Y/2;
    Snake_LastStep = Step_Right;

    Snake_Score = 0;

    Snake_GameInProgress = true;

    memset(Snake_Matrix, 0, sizeof(Snake_Matrix));

    Snake_Matrix[Snake_StartPoint.x][Snake_StartPoint.y] = Step_Right;
    Snake_Matrix[Snake_StartPoint.x-1][Snake_StartPoint.y] = Step_Right;
    Snake_Matrix[Snake_StartPoint.x-2][Snake_StartPoint.y] = Step_Right;

    Snake_EndPoint.x = Snake_StartPoint.x-2;
    Snake_EndPoint.y = Snake_StartPoint.y;

    Snake_PutNewGift();
}



void Snake_Step(SnakeStep_t step)
{
    // There is no game
    if (!Snake_GameInProgress) return;

    if (Snake_StepIsValid(Snake_LastStep, step))
    {
        // it is possible step!
        // Check collision
        if (!Snake_CheckCollision(step))
        {
            // No collision, add new point
            Snake_AddNewCoord(step);

            if (!Snake_CaptureEvent)
            {
                // If not captured a gift, need delete last point
                Snake_DeleteLastPoint();
            }
            else
            {
                // Clear flag
                Snake_CaptureEvent = false;
            }

            Snake_LastStep = step;

            Snake_Draw();
        }
        else
        {
            Snake_AddNewCoord(step);

            PRINT("Collision!\r\n");
            // Finish game!
            Snake_FinishLose();
        }
        Display_Activate();
    }
    else
    {
        PRINT("Wrong step!\r\n");
    }
}



static bool Snake_StepIsValid(SnakeStep_t lastStep, SnakeStep_t actualStep)
{
    bool isOk = false;

    if (actualStep != Step_Count && actualStep != Step_Unknown)
    {
        switch (actualStep)
        {
            case Step_Down:
                if (lastStep != Step_Up) isOk = true;
                break;

            case Step_Up:
                if (lastStep != Step_Down) isOk = true;
                break;

            case Step_Left:
                if (lastStep != Step_Right) isOk = true;
                break;

            case Step_Right:
                if (lastStep != Step_Left) isOk = true;
                break;

            case Step_Gift:
            case Step_Unknown:
            case Step_Count:
            default:
                isOk = false;
                break;
        }
    }

    return isOk;
}



static void Snake_AddNewCoord(SnakeStep_t step)
{
    Snake_Matrix[Snake_StartPoint.x][Snake_StartPoint.y] = step;

    SnakeCoord_t changeCoord = Snake_ConvertStepToCoordinate(step);

    Snake_StartPoint.x += changeCoord.x;
    Snake_StartPoint.y += changeCoord.y;
}



static bool Snake_CheckCollision(SnakeStep_t step)
{
    SnakeCoord_t changeCoord = Snake_ConvertStepToCoordinate(step);
    bool isCollision = false;

    changeCoord.x += Snake_StartPoint.x;
    changeCoord.y += Snake_StartPoint.y;


    // Check border
    // TODO: Now work with "underflow", but it is not beautiful solution
    if (changeCoord.x >= SNAKE_SIZE_X || changeCoord.y >= SNAKE_SIZE_Y)
    {
        isCollision = true;
    }
    // Check, the new point is gift?
    else if (Snake_Matrix[changeCoord.x][changeCoord.y] == Step_Gift)
    {
        Snake_CaptureGift(changeCoord);
        isCollision = false;
    }
    // Check "original points" / Tail
    else if (Snake_Matrix[changeCoord.x][changeCoord.y] != Step_Unknown)
    {
        // There is an old point here
        isCollision = true;
    }

    return isCollision;
}



static SnakeCoord_t Snake_ConvertStepToCoordinate(SnakeStep_t step)
{
    SnakeCoord_t coord = { 0 };

    switch (step)
    {
        case Step_Up:
            coord.y = 1;
            break;

        case Step_Down:
            coord.y = -1;
            break;

        case Step_Left:
            coord.x = -1;
            break;

        case Step_Right:
            coord.x = 1;
            break;

        case Step_Gift:
        case Step_Unknown:
        case Step_Count:
        default:
            break;
    }

    return coord;
}



void Snake_Draw(void)
{
    // Print header (Score)
    char string[20];
    usprintf(string, "Score: %d", Snake_Score);
    Display_PrintString(string, 0, Font_8x5, NO_FORMAT);

    // Header Line
    uint8_t pixel;
    for (pixel = 0; pixel < DISPLAY_WIDTH; pixel++)
    {
        // TODO: Rectangular / Line drawing?
        SSD1306_drawPixel(pixel, SNAKE_HEADER_HEIGHT - 1, WHITE);
    }

    // Step around on the matrix
    for (uint16_t i = 0; i < SNAKE_SIZE_X; i++)
    {
        for (uint16_t j = 0; j < SNAKE_SIZE_Y; j++)
        {
            if (Snake_Matrix[i][j] != Step_Gift)
            {
                Snake_DrawPoints(i, j);
            }
            else
            {
                Snake_DrawGiftToPoint(i, j);
            }
        }
    }
}



static void Snake_DrawPoints(uint16_t x, uint16_t y)
{
    for (uint8_t k = 0; k < SNAKE_POINT_PIXEL_SIZE; k++)
    {
        for (uint8_t l = 0; l < SNAKE_POINT_PIXEL_SIZE; l++)
        {
            // Be careful: the display 0,0 point is the top left, but on matrix 0,0 pont is the bottom, left point
            SSD1306_drawPixel(x*SNAKE_POINT_PIXEL_SIZE + k,
                    DISPLAY_HEIGHT - (y*SNAKE_POINT_PIXEL_SIZE + l),
                    (Snake_Matrix[x][y] != 0)? WHITE : BLACK);
        }
    }
}



SnakeStep_t Snake_GetLastStep(void)
{
    return Snake_LastStep;
}



static void Snake_CaptureGift(SnakeCoord_t coord)
{
    Snake_Score++;
    Snake_CaptureEvent = true;

    Snake_Matrix[coord.x][coord.y] = 0;

    Snake_PutNewGift();
}



static void Snake_PutNewGift(void)
{
    // Put new gift
    uint32_t freeSpaces = Snake_CalculateFreePoints();
    uint32_t giftPos = Snake_GenerateRandomPosition(freeSpaces);
    Snake_PutGiftPosition(giftPos);
}



static void Snake_DeleteLastPoint(void)
{
    // Check oldest point "next point"
    SnakeStep_t oldestStep = Snake_Matrix[Snake_EndPoint.x][Snake_EndPoint.y];
    SnakeCoord_t changeCord = Snake_ConvertStepToCoordinate(oldestStep);

    // Clear oldest point
    Snake_Matrix[Snake_EndPoint.x][Snake_EndPoint.y] = 0;

    // Change new oldest point
    Snake_EndPoint.x += changeCord.x;
    Snake_EndPoint.y += changeCord.y;
}



static void Snake_FinishLose(void)
{
    Display_PrintString("End :(", 2, Font_12x8, NO_FORMAT);

    Snake_GameInProgress = false;

    Logic_Display_ChangeState(Menu_Snake);
    Logic_Display_Snake_ChangeToMenu();
}



static uint32_t Snake_CalculateFreePoints(void)
{
    uint32_t freeSpace = 0;

    for (uint16_t i = 0; i < SNAKE_SIZE_X; i++)
    {
        for (uint16_t j = 0; j < SNAKE_SIZE_Y; j++)
        {
            if (Snake_Matrix[i][j] == 0)
            {
                freeSpace++;
            }
        }
    }

    return freeSpace;
}



static uint32_t Snake_GenerateRandomPosition(uint32_t max)
{
    uint32_t randomValue = 0;

    randomValue = random() % max;

    return randomValue;
}



static void Snake_PutGiftPosition(uint32_t giftPos)
{
    uint32_t freePos = 0;

    for (uint16_t i = 0; i < SNAKE_SIZE_X; i++)
    {
        for (uint16_t j = 0; j < SNAKE_SIZE_Y; j++)
        {
            if (Snake_Matrix[i][j] == 0)
            {
                if (freePos == giftPos)
                {
                    Snake_Matrix[i][j] = Step_Gift;
                    return;
                }
                freePos++;
            }
        }
    }
}



static void Snake_DrawGiftToPoint(uint16_t x, uint16_t y)
{
    // Put to X
    uint8_t putMatrix[SNAKE_POINT_PIXEL_SIZE][SNAKE_POINT_PIXEL_SIZE] = {
            { 1, 0, 0, 1 },
            { 0, 1, 1, 0 },
            { 0, 1, 1, 0 },
            { 1, 0, 0, 1 }
    };

    for (uint8_t i = 0; i < SNAKE_POINT_PIXEL_SIZE; i++)
    {
        for (uint8_t j = 0; j < SNAKE_POINT_PIXEL_SIZE; j++)
        {
            // Be careful: the display 0,0 point is the top left, but on matrix 0,0 pont is the bottom, left point
            SSD1306_drawPixel(x*SNAKE_POINT_PIXEL_SIZE + i,
                    DISPLAY_HEIGHT - (y*SNAKE_POINT_PIXEL_SIZE + j),
                    putMatrix[i][j] ? WHITE : BLACK);
        }
    }

}

#endif    // #ifdef CONFIG_FUNCTION_GAME_SNAKE


