/*
 *    Display_SSD1306.c
 *    Created on:   2017-03-16
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     Display - SSD1306 driver
 *    Target:       STM32Fx
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "compiler.h"
#include "MemHandler.h"
#include "ErrorHandler.h"
#include "Timing.h"
#include "DebugUart.h"
#include "Display_SSD1306.h"
#include "TaskList.h"
#include "EventHandler.h"



#ifdef CONFIG_MODULE_DISPLAY_ENABLE

#define ssd1306_swap(a, b) { int16_t t = a; a = b; b = t; }



/*------------------------------------------------------------------------------
 *  Global variables
 *----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 *  Local variables
 *----------------------------------------------------------------------------*/

static SPI_HandleTypeDef SpiHandle;
static bool Display_TransferInProgress = false;

///< Actual image (screen buffer)
extern uint8_t display_buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8];

#ifdef CONFIG_DISPLAY_SPI_USE_DMA
static DMA_HandleTypeDef hdma_tx;
#endif



/*------------------------------------------------------------------------------
 *  Function declarations
 *----------------------------------------------------------------------------*/

static void SSD1306_HardwareInit(void);
static void SSD1306_command(uint8_t c);
static void SSD1306_fastSPIwrite(uint8_t d);



/*------------------------------------------------------------------------------
 *  Functions
 *----------------------------------------------------------------------------*/

/**
 * @brief       Initialize Display - SSD1306
 */
void Display_SSD1306_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    DISPLAY_SSD1306_SPIx_GPIO_CLK_ENABLES();
    /* Enable SPI clock */
    DISPLAY_SSD1306_SPIx_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.Pin = DISPLAY_SSD1306_SPIx_SCK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = DISPLAY_SSD1306_SPIx_SCK_AF;

    HAL_GPIO_Init(DISPLAY_SSD1306_SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin = DISPLAY_SSD1306_SPIx_MOSI_PIN;
    GPIO_InitStruct.Alternate = DISPLAY_SSD1306_SPIx_MOSI_AF;

    HAL_GPIO_Init(DISPLAY_SSD1306_SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);


    /* Initialize other GPIO pins */
    DISPLAY_SSD1306_PINS_CLK_ENABLE();

    /* CS */
    GPIO_InitStruct.Pin = DISPLAY_SSD1306_SPIx_CS_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(DISPLAY_SSD1306_SPIx_CS_GPIO_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(DISPLAY_SSD1306_SPIx_CS_GPIO_PORT, DISPLAY_SSD1306_SPIx_CS_GPIO_PIN, SET);

    /* RST */
    GPIO_InitStruct.Pin = DISPLAY_SSD1306_RST_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(DISPLAY_SSD1306_RST_GPIO_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(DISPLAY_SSD1306_RST_GPIO_PORT,
            DISPLAY_SSD1306_RST_GPIO_PIN, SET);

    /* Data/Command */
    GPIO_InitStruct.Pin = DISPLAY_SSD1306_DATACOMMAND_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(DISPLAY_SSD1306_DATACOMMAND_GPIO_PORT, &GPIO_InitStruct);

    /*##-1- Configure the SPI peripheral #######################################*/
    /* Set the SPI parameters */
    SpiHandle.Instance = DISPLAY_SSD1306_SPIx;
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    SpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
    SpiHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
    SpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    SpiHandle.Init.CRCPolynomial = 7;
    SpiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
    SpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.NSS = SPI_NSS_SOFT;
    SpiHandle.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    SpiHandle.Init.TIMode = SPI_TIMODE_DISABLE;

    SpiHandle.Init.Mode = SPI_MODE_MASTER;


#ifdef CONFIG_DISPLAY_SPI_USE_DMA
    /* Enable DMA clock */
    DMAx_CLK_ENABLE();

    /*##-3- Configure the DMA ##################################################*/
    /* Configure the DMA handler for Transmission process */
    hdma_tx.Instance                 = SPIx_TX_DMA_STREAM;
    hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
    hdma_tx.Init.Mode                = DMA_NORMAL;
    hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&hdma_tx);

    /* Associate the initialized DMA handle to the the SPI handle */
    __HAL_LINKDMA(&SpiHandle, hdmatx, hdma_tx);


    /*##-4- Configure the NVIC for DMA #########################################*/
    /* NVIC configuration for DMA transfer complete interrupt (SPI3_TX) */
    HAL_NVIC_SetPriority(SPIx_DMA_TX_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(SPIx_DMA_TX_IRQn);
#endif

    /*##-3- Configure the NVIC for SPI #########################################*/
    /* NVIC for SPI */
    HAL_NVIC_SetPriority(DISPLAY_SSD1306_SPIx_IRQn,
            DISPLAY_SPI_IRQ_PREEMT_PRIORITY, DISPLAY_SPI_IRQ_SUB_PRIORITY);
    HAL_NVIC_EnableIRQ(DISPLAY_SSD1306_SPIx_IRQn);


    if (HAL_SPI_Init(&SpiHandle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    SSD1306_HardwareInit();
}


/* #ifdef CONFIG_DISPLAY_SPI_USE_DMA */
#if 0
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    if (hspi->Instance == DISPLAY_SSD1306_SPIx)
    {

        /* Enable DMA clock */
        DMAx_CLK_ENABLE();

        /*##-3- Configure the DMA ##################################################*/
        /* Configure the DMA handler for Transmission process */
        hdma_tx.Instance                 = SPIx_TX_DMA_STREAM;
        hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
        hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
        hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
        hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
        hdma_tx.Init.Mode                = DMA_NORMAL;
        hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;

        HAL_DMA_Init(&hdma_tx);

        /* Associate the initialized DMA handle to the the SPI handle */
        __HAL_LINKDMA(&SpiHandle, hdmatx, hdma_tx);


        /*##-4- Configure the NVIC for DMA #########################################*/
        /* NVIC configuration for DMA transfer complete interrupt (SPI3_TX) */
        HAL_NVIC_SetPriority(SPIx_DMA_TX_IRQn, 1, 1);
        HAL_NVIC_EnableIRQ(SPIx_DMA_TX_IRQn);
    }
}
#endif



static void SSD1306_HardwareInit(void)
{

    /* Reset --> low --> high */
    HAL_GPIO_WritePin(DISPLAY_SSD1306_RST_GPIO_PORT,
            DISPLAY_SSD1306_RST_GPIO_PIN, RESET);

    DelayMs(10);

    HAL_GPIO_WritePin(DISPLAY_SSD1306_RST_GPIO_PORT,
            DISPLAY_SSD1306_RST_GPIO_PIN, SET);

    /* Wait after reset */
    DelayMs(100);

    /* Init sequence */
    SSD1306_command(SSD1306_DISPLAYOFF);                    /* 0xAE */
    SSD1306_command(SSD1306_SETDISPLAYCLOCKDIV);            /* 0xD5 */
    SSD1306_command(0x80);                           /* the suggested ratio 0x80 */

    SSD1306_command(SSD1306_SETMULTIPLEX);                  /* 0xA8 */
    SSD1306_command(SSD1306_LCDHEIGHT - 1);

    SSD1306_command(SSD1306_SETDISPLAYOFFSET);              /* 0xD3 */
    SSD1306_command(0x0);                                   /* no offset */
    SSD1306_command(SSD1306_SETSTARTLINE | 0x0);            /* line #0 */
    SSD1306_command(SSD1306_CHARGEPUMP);                    /* 0x8D */
    /* f (vccstate == SSD1306_EXTERNALVCC)    // TODO: */
    /* SD1306_command(0x10); */
    /* lse */
    SSD1306_command(0x14);
    SSD1306_command(SSD1306_MEMORYMODE);                    /* 0x20 */
    SSD1306_command(0x00);                                /* 0x0 act like ks0108 */
    SSD1306_command(SSD1306_SEGREMAP | 0x1);
    SSD1306_command(SSD1306_COMSCANDEC);

#if defined(SSD1306_128_32)
    SSD1306_command(SSD1306_SETCOMPINS);                    /* 0xDA */
    SSD1306_command(0x02);
    SSD1306_command(SSD1306_SETCONTRAST);/* 0x81 */
    SSD1306_command(0x8F);

#elif defined(SSD1306_128_64)
    SSD1306_command(SSD1306_SETCOMPINS);                    /* 0xDA */
    SSD1306_command(0x12);
    SSD1306_command(SSD1306_SETCONTRAST);                   /* 0x81 */
    /* f (vccstate == SSD1306_EXTERNALVCC)    // TODO: */
    /* SD1306_command(0x9F); */
    /*  else */
    SSD1306_command(0xCF);

#elif defined(SSD1306_96_16)
    SSD1306_command(SSD1306_SETCOMPINS);                    /* 0xDA */
    SSD1306_command(0x2);/* da x12 */
    SSD1306_command(SSD1306_SETCONTRAST);/* 0x81 */
    if (vccstate == SSD1306_EXTERNALVCC)
    {   SSD1306_command(0x10);}
    else
    {   SSD1306_command(0xAF);}

#endif

    SSD1306_command(SSD1306_SETPRECHARGE);                  /* 0xd9 */
    /* f (vccstate == SSD1306_EXTERNALVCC)    // TODO: */
    /* SD1306_command(0x22); */
    /* lse */
    SSD1306_command(0xF1);

    SSD1306_command(SSD1306_SETVCOMDETECT);                 /* 0xDB */
    SSD1306_command(0x40);
    SSD1306_command(SSD1306_DISPLAYALLON_RESUME);           /* 0xA4 */
    SSD1306_command(SSD1306_NORMALDISPLAY);                 /* 0xA6 */

    SSD1306_command(SSD1306_DEACTIVATE_SCROLL);

    SSD1306_command(SSD1306_DISPLAYON);                 /* -turn on oled panel */
}



/**
 * @brief       Send command on SPI
 */
static void SSD1306_command(uint8_t c)
{
    HAL_GPIO_WritePin(DISPLAY_SSD1306_SPIx_CS_GPIO_PORT, DISPLAY_SSD1306_SPIx_CS_GPIO_PIN, SET);

    HAL_GPIO_WritePin(DISPLAY_SSD1306_DATACOMMAND_GPIO_PORT,
            DISPLAY_SSD1306_DATACOMMAND_GPIO_PIN, RESET);

    HAL_GPIO_WritePin(DISPLAY_SSD1306_SPIx_CS_GPIO_PORT, DISPLAY_SSD1306_SPIx_CS_GPIO_PIN, RESET);

    SSD1306_fastSPIwrite(c);

    HAL_GPIO_WritePin(DISPLAY_SSD1306_SPIx_CS_GPIO_PORT, DISPLAY_SSD1306_SPIx_CS_GPIO_PIN, SET);
}



/**
 * @brief       Send command on SPI to Display
 */
static void SSD1306_fastSPIwrite(uint8_t d)
{
    HAL_SPI_Transmit_IT(&SpiHandle, &d, 1);
}



#if 0
#warning "Not tested"
void SSD1306_drawFixVerticalLine(int16_t x, int16_t y, uint8_t row)
{
    /* TODO: Check parameters */
    display_buffer[x + (y / 8) * SSD1306_LCDWIDTH] = row;
}
#endif



/**
 * @brief       Invert display
 *              TODO: Delete?
 */
void SSD1306_invertDisplay(bool invert)
{
    if (invert)
    {
        SSD1306_command(SSD1306_INVERTDISPLAY);
    }
    else
    {
        SSD1306_command(SSD1306_NORMALDISPLAY);
    }
}



/* startscrollright */
/* Activate a right handed scroll for rows start through stop */
/* Hint, the display is 16 rows tall. To scroll the whole display, run: */
/* display.scrollright(0x00, 0x0F) */
void SSD1306_startscrollright(uint8_t start, uint8_t stop)
{
    SSD1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
    SSD1306_command(0X00);
    SSD1306_command(start);
    SSD1306_command(0X00);
    SSD1306_command(stop);
    SSD1306_command(0X00);
    SSD1306_command(0XFF);
    SSD1306_command(SSD1306_ACTIVATE_SCROLL);
}



/* startscrollleft */
/* Activate a right handed scroll for rows start through stop */
/* Hint, the display is 16 rows tall. To scroll the whole display, run: */
/* display.scrollright(0x00, 0x0F) */
void SSD1306_startscrollleft(uint8_t start, uint8_t stop)
{
    SSD1306_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
    SSD1306_command(0X00);
    SSD1306_command(start);
    SSD1306_command(0X00);
    SSD1306_command(stop);
    SSD1306_command(0X00);
    SSD1306_command(0XFF);
    SSD1306_command(SSD1306_ACTIVATE_SCROLL);
}



/* startscrolldiagright */
/* Activate a diagonal scroll for rows start through stop */
/* Hint, the display is 16 rows tall. To scroll the whole display, run: */
/* display.scrollright(0x00, 0x0F) */
void SSD1306_startscrolldiagright(uint8_t start, uint8_t stop)
{
    SSD1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
    SSD1306_command(0X00);
    SSD1306_command(SSD1306_LCDHEIGHT);
    SSD1306_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
    SSD1306_command(0X00);
    SSD1306_command(start);
    SSD1306_command(0X00);
    SSD1306_command(stop);
    SSD1306_command(0X01);
    SSD1306_command(SSD1306_ACTIVATE_SCROLL);
}



/* startscrolldiagleft */
/* Activate a diagonal scroll for rows start through stop */
/* Hint, the display is 16 rows tall. To scroll the whole display, run: */
/* display.scrollright(0x00, 0x0F) */
void SSD1306_startscrolldiagleft(uint8_t start, uint8_t stop)
{
    SSD1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);
    SSD1306_command(0X00);
    SSD1306_command(SSD1306_LCDHEIGHT);
    SSD1306_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
    SSD1306_command(0X00);
    SSD1306_command(start);
    SSD1306_command(0X00);
    SSD1306_command(stop);
    SSD1306_command(0X01);
    SSD1306_command(SSD1306_ACTIVATE_SCROLL);
}



void SSD1306_stopscroll(void)
{
    SSD1306_command(SSD1306_DEACTIVATE_SCROLL);
}



/* Dim the display */
/* dim = true: display is dimmed */
/* dim = false: display is normal */
void SSD1306_dim(bool dim)
{
    uint8_t contrast;

    if (dim)
    {
        contrast = 0; /* Dimmed display */
    }
    else
    {
        /* TODO: */
        /* f (_vccstate == SSD1306_EXTERNALVCC) { */
        /* ontrast = 0x9F; */
        /*  else { */
        contrast = 0xCF;
        //}
    }
    /* the range of contrast to too small to be really useful */
    /* it is useful to dim the display */
    SSD1306_command(SSD1306_SETCONTRAST);
    SSD1306_command(contrast);
}



void SSD1306_display(void)
{
    /* TODO: Check Display_TransferInProgress */
#ifdef CONFIG_EVENTLOG_DISPLAY_LOG_ENABLE
    /* Save event */
    EventHandler_GenerateEvent(Event_Display_SpiEvent, 3, Task_Display);
#endif

    SSD1306_command(SSD1306_COLUMNADDR);
    SSD1306_command(0);   /* Column start address (0 = reset) */
    SSD1306_command(SSD1306_LCDWIDTH - 1); /* Column end address (127 = reset) */

    SSD1306_command(SSD1306_PAGEADDR);
    SSD1306_command(0); /* Page start address (0 = reset) */
#if SSD1306_LCDHEIGHT == 64
    SSD1306_command(7); /* Page end address */
#endif
#if SSD1306_LCDHEIGHT == 32
    SSD1306_command(3); /* Page end address */
#endif
#if SSD1306_LCDHEIGHT == 16
    SSD1306_command(1); /* Page end address */
#endif

#ifdef CONFIG_EVENTLOG_DISPLAY_LOG_ENABLE
    /* Save event */
    /* ventHandler_GenerateEvent(Event_Display_SpiEvent, 1, Task_Display); */
#endif

    /* SPI */
    HAL_GPIO_WritePin(DISPLAY_SSD1306_SPIx_CS_GPIO_PORT, DISPLAY_SSD1306_SPIx_CS_GPIO_PIN, SET);

    /* Data */
    HAL_GPIO_WritePin(DISPLAY_SSD1306_DATACOMMAND_GPIO_PORT,
            DISPLAY_SSD1306_DATACOMMAND_GPIO_PIN, SET);

    HAL_GPIO_WritePin(DISPLAY_SSD1306_SPIx_CS_GPIO_PORT, DISPLAY_SSD1306_SPIx_CS_GPIO_PIN, RESET);

    /* Blocking mode */
    /* TODO: Make define for this */
    /*
    for (uint16_t i = 0; i < (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8); i++)
    {
        SSD1306_fastSPIwrite(buffer[i]);
    }
    */

    Display_TransferInProgress = true;


#ifdef CONFIG_DISPLAY_SPI_USE_DMA
    HAL_SPI_Transmit_DMA(&SpiHandle, display_buffer, (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8));
#else
    /* TODO: This Sending need 20ms, optimize!! (HAL_SPI_Transmit_IT) */
    HAL_SPI_Transmit_IT(&SpiHandle, display_buffer, (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8));
#endif

#ifdef CONFIG_EVENTLOG_DISPLAY_LOG_ENABLE
    /* Save event */
    EventHandler_GenerateEvent(Event_Display_SpiEvent, 4, Task_Display);
#endif

    /* CS pin set in IT function */
    /* AL_GPIO_WritePin(DISPLAY_SSD1306_SPIx_CS_GPIO_PORT, DISPLAY_SSD1306_SPIx_CS_GPIO_PIN, SET); */
}



/**
 * @brief       Display SPI communication IRQ handler
 */
void DISPLAY_SSD1306_SPIx_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&SpiHandle);
}



/**
 * @brief       Tx Transfer completed callback.
 * @param       hspi: pointer to a SPI_HandleTypeDef structure that contains
 *              the configuration information for SPI module.
 * @retval      None
 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    /* Suppress warning */
    UNUSED_ARGUMENT(hspi);

    HAL_GPIO_WritePin(DISPLAY_SSD1306_SPIx_CS_GPIO_PORT, DISPLAY_SSD1306_SPIx_CS_GPIO_PIN, SET);

    if (Display_TransferInProgress)
    {
        Display_TransferInProgress = false;
#ifdef CONFIG_EVENTLOG_DISPLAY_LOG_ENABLE
        /* Save event */
        EventHandler_GenerateEvent(Event_Display_SpiEvent, 5, Task_Display);
#endif
    }
}



/**
 * @brief       SPI error callbacks
 * @param       hspi: SPI handle
 * @note        This example shows a simple way to report transfer error, and you can
 *              add your own implementation.
 * @retval      None
 */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    /* Suppress warning */
    UNUSED_ARGUMENT(hspi);

    HAL_GPIO_WritePin(DISPLAY_SSD1306_SPIx_CS_GPIO_PORT, DISPLAY_SSD1306_SPIx_CS_GPIO_PIN, SET);
    Display_TransferInProgress = false;

#ifdef CONFIG_EVENTLOG_DISPLAY_LOG_ENABLE
    /* Save event */
    EventHandler_GenerateEvent(Event_Display_SpiEvent, 4, Task_Display);
#endif
}



#if defined(CONFIG_DISPLAY_SPI_USE_DMA)
/**
 * @brief       This function handles DMA interrupt request.
 * @param       None
 * @retval      None
 */
void SPIx_DMA_TX_IRQHandler(void)
{
    HAL_DMA_IRQHandler(SpiHandle.hdmatx);
}
#endif



#endif    /* #ifdef CONFIG_MODULE_DISPLAY_ENABLE */
