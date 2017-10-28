/*
 *		Display_SSD1306.c
 *		Created on:		2017-03-16
 *		Author:			Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		Function:		Display - SSD1306 driver
 *		Target:			STM32Fx
 *		Version:		v1
 *		Last modified:	2017-03-16
 *		Help: An arduino (C++) code:
 *			https://github.com/adafruit/Adafruit_SSD1306/blob/master/Adafruit_SSD1306.cpp
 *		Modified:
 *			- C++ --> C
 *			- Renames, refactors
 *			- Added comments
 *			- porting (from Atmel to ST - HAL driver)
 *			- Added string printing functions
 *			- Use my font: 8x5
 */



/*------------------------------------------------------------------------------
 *  Header files
 *----------------------------------------------------------------------------*/

#include "options.h"
#include "include.h"
#include "MEM.h"
#include "Globals.h"
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
static uint8_t buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8] = { 0 };

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
 * \brief	Initialize Display - SSD1306
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


	// Initialize other GPIO pins
	DISPLAY_SSD1306_PINS_CLK_ENABLE();

	// CS
	GPIO_InitStruct.Pin = DISPLAY_SSD1306_SPIx_CS_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(DISPLAY_SSD1306_SPIx_CS_GPIO_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(DISPLAY_SSD1306_SPIx_CS_GPIO_PORT, DISPLAY_SSD1306_SPIx_CS_GPIO_PIN, SET);

	// RST
	GPIO_InitStruct.Pin = DISPLAY_SSD1306_RST_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(DISPLAY_SSD1306_RST_GPIO_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(DISPLAY_SSD1306_RST_GPIO_PORT,
			DISPLAY_SSD1306_RST_GPIO_PIN, SET);

	// Data/Command
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


// #ifdef CONFIG_DISPLAY_SPI_USE_DMA
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

	// Reset --> low --> high
	HAL_GPIO_WritePin(DISPLAY_SSD1306_RST_GPIO_PORT,
			DISPLAY_SSD1306_RST_GPIO_PIN, RESET);

	DelayMs(10);

	HAL_GPIO_WritePin(DISPLAY_SSD1306_RST_GPIO_PORT,
			DISPLAY_SSD1306_RST_GPIO_PIN, SET);

	// Wait after reset
	DelayMs(100);

	// Init sequence
	SSD1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
	SSD1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
	SSD1306_command(0x80);                           // the suggested ratio 0x80

	SSD1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
	SSD1306_command(SSD1306_LCDHEIGHT - 1);

	SSD1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
	SSD1306_command(0x0);                                   // no offset
	SSD1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
	SSD1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
	//if (vccstate == SSD1306_EXTERNALVCC)	// TODO:
	//SSD1306_command(0x10);
	//else
	SSD1306_command(0x14);
	SSD1306_command(SSD1306_MEMORYMODE);                    // 0x20
	SSD1306_command(0x00);                                // 0x0 act like ks0108
	SSD1306_command(SSD1306_SEGREMAP | 0x1);
	SSD1306_command(SSD1306_COMSCANDEC);

#if defined(SSD1306_128_32)
	SSD1306_command(SSD1306_SETCOMPINS);                    // 0xDA
	SSD1306_command(0x02);
	SSD1306_command(SSD1306_SETCONTRAST);// 0x81
	SSD1306_command(0x8F);

#elif defined(SSD1306_128_64)
	SSD1306_command(SSD1306_SETCOMPINS);                    // 0xDA
	SSD1306_command(0x12);
	SSD1306_command(SSD1306_SETCONTRAST);                   // 0x81
	//if (vccstate == SSD1306_EXTERNALVCC)	// TODO:
	//SSD1306_command(0x9F);
	//  else
	SSD1306_command(0xCF);

#elif defined(SSD1306_96_16)
	SSD1306_command(SSD1306_SETCOMPINS);                    // 0xDA
	SSD1306_command(0x2);//ada x12
	SSD1306_command(SSD1306_SETCONTRAST);// 0x81
	if (vccstate == SSD1306_EXTERNALVCC)
	{   SSD1306_command(0x10);}
	else
	{   SSD1306_command(0xAF);}

#endif

	SSD1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
	//if (vccstate == SSD1306_EXTERNALVCC)	// TODO:
	//SSD1306_command(0x22);
	//else
	SSD1306_command(0xF1);

	SSD1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
	SSD1306_command(0x40);
	SSD1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
	SSD1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6

	SSD1306_command(SSD1306_DEACTIVATE_SCROLL);

	SSD1306_command(SSD1306_DISPLAYON);                 //--turn on oled panel
}



/**
 * \brief	Send command on SPI
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
 * \brief	Send command on SPI to Display
 */
static void SSD1306_fastSPIwrite(uint8_t d)
{
	HAL_SPI_Transmit_IT(&SpiHandle, &d, 1);
}



/**
 * \brief	The most basic function, set a single pixel
 */
void SSD1306_drawPixel(int16_t x, int16_t y, uint16_t color)
{
	if ((x < 0) || (x >= SSD1306_LCDWIDTH) || (y < 0) || (y >= SSD1306_LCDHEIGHT))
		return;

#ifdef SSD1306_ROTATION_ENABLE
	// check rotation, move pixel around if necessary
	uint8_t rotation = 0;	// TODO:
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

	// x is which column
	switch (color)
	{
		case WHITE:
			buffer[x + (y / 8) * SSD1306_LCDWIDTH] |= (1 << (y & 7));
			break;
		case BLACK:
			buffer[x + (y / 8) * SSD1306_LCDWIDTH] &= ~(1 << (y & 7));
			break;
		case INVERSE:
			buffer[x + (y / 8) * SSD1306_LCDWIDTH] ^= (1 << (y & 7));
			break;
		default:
			break;
	}
}



#if 0
#warning "Not tested"
void SSD1306_drawFixVerticalLine(int16_t x, int16_t y, uint8_t row)
{
	// TODO: Check parameters
	buffer[x + (y / 8) * SSD1306_LCDWIDTH] = row;
}
#endif



/**
 * \brief	Draw image (put pixel to buffer image)
 */
void SSD1306_drawImage(uint8_t setx, uint8_t sety, uint8_t sizex, uint8_t sizey, uint8_t *img)
{
	// TODO: Only 8n size can be used
	uint8_t i;
	uint8_t j;

	// Step on a row (left to right)
	for (i = 0; i < sizex; i++)
	{
		// Step on a column (a 1x8 column is a byte)
		for (j = 0; j < sizey/8; j++)
		{
			// Copy an byte
			buffer[sety/8*SSD1306_LCDWIDTH + setx + i + j*SSD1306_LCDWIDTH] =
					img[j*sizex + i];
		}
	}
}



/**
 * \brief	Invert display
 * TODO: Delete?
 */
void SSD1306_invertDisplay(uint8_t i)
{
	if (i)
	{
		SSD1306_command(SSD1306_INVERTDISPLAY);
	}
	else
	{
		SSD1306_command(SSD1306_NORMALDISPLAY);
	}
}



// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
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



// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
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



// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
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



// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
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



// Dim the display
// dim = true: display is dimmed
// dim = false: display is normal
void SSD1306_dim(bool dim)
{
	uint8_t contrast;

	if (dim)
	{
		contrast = 0; // Dimmed display
	}
	else
	{
		// TODO:
		//if (_vccstate == SSD1306_EXTERNALVCC) {
		//contrast = 0x9F;
		//} else {
		contrast = 0xCF;
		//}
	}
	// the range of contrast to too small to be really useful
	// it is useful to dim the display
	SSD1306_command(SSD1306_SETCONTRAST);
	SSD1306_command(contrast);
}



void SSD1306_display(void)
{
	// TODO: Check Display_TransferInProgress
#ifdef CONFIG_EVENTLOG_DISPLAY_LOG_ENABLE
	// Save event
	EventHandler_GenerateEvent(Event_Display_SpiEvent, 3, Task_Display);
#endif

	SSD1306_command(SSD1306_COLUMNADDR);
	SSD1306_command(0);   // Column start address (0 = reset)
	SSD1306_command(SSD1306_LCDWIDTH - 1); // Column end address (127 = reset)

	SSD1306_command(SSD1306_PAGEADDR);
	SSD1306_command(0); // Page start address (0 = reset)
#if SSD1306_LCDHEIGHT == 64
	SSD1306_command(7); // Page end address
#endif
#if SSD1306_LCDHEIGHT == 32
	SSD1306_command(3); // Page end address
#endif
#if SSD1306_LCDHEIGHT == 16
	SSD1306_command(1); // Page end address
#endif

#ifdef CONFIG_EVENTLOG_DISPLAY_LOG_ENABLE
	// Save event
	//EventHandler_GenerateEvent(Event_Display_SpiEvent, 1, Task_Display);
#endif

	// SPI
	HAL_GPIO_WritePin(DISPLAY_SSD1306_SPIx_CS_GPIO_PORT, DISPLAY_SSD1306_SPIx_CS_GPIO_PIN, SET);

	// Data
	HAL_GPIO_WritePin(DISPLAY_SSD1306_DATACOMMAND_GPIO_PORT,
			DISPLAY_SSD1306_DATACOMMAND_GPIO_PIN, SET);

	HAL_GPIO_WritePin(DISPLAY_SSD1306_SPIx_CS_GPIO_PORT, DISPLAY_SSD1306_SPIx_CS_GPIO_PIN, RESET);

	/*
	// Blocking mode
	for (uint16_t i = 0; i < (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8); i++)
	{
		SSD1306_fastSPIwrite(buffer[i]);
	}
	*/

	Display_TransferInProgress = true;

	
#ifdef CONFIG_DISPLAY_SPI_USE_DMA
	HAL_SPI_Transmit_DMA(&SpiHandle, buffer, (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8));
#else
	// TODO: This Sending need 20ms, optimize!! (HAL_SPI_Transmit_IT)
	HAL_SPI_Transmit_IT(&SpiHandle, buffer, (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8));
#endif

#ifdef CONFIG_EVENTLOG_DISPLAY_LOG_ENABLE
	// Save event
	EventHandler_GenerateEvent(Event_Display_SpiEvent, 4, Task_Display);
#endif

	// CS pin set in IT function
	//HAL_GPIO_WritePin(DISPLAY_SSD1306_SPIx_CS_GPIO_PORT, DISPLAY_SSD1306_SPIx_CS_GPIO_PIN, SET);
}



/**
 *	\brief	Clear everything
 */
void SSD1306_clearDisplay(void)
{
	memset(buffer, 0, (SSD1306_LCDWIDTH * SSD1306_LCDHEIGHT / 8));
}



void SSD1306_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
#ifdef SSD1306_ROTATION_ENABLE
	bool bSwap = false;
	uint8_t rotation = 0;	// TODO:
	switch (rotation)
	{
		case 0:
			// 0 degree rotation, do nothing
			break;
		case 1:
			// 90 degree rotation, swap x & y for rotation, then invert x
			bSwap = true;
			ssd1306_swap(x, y);
			x = SSD1306_LCDWIDTH - x - 1;
			break;
		case 2:
			// 180 degree rotation, invert x and y - then shift y around for height.
			x = SSD1306_LCDWIDTH - x - 1;
			y = SSD1306_LCDHEIGHT - y - 1;
			x -= (w - 1);
			break;
		case 3:
			// 270 degree rotation, swap x & y for rotation, then invert y  and adjust y for w (not to become h)
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
		SSD1306_drawFastVLineInternal(x, y, w, color);
	}
	else
	{
		SSD1306_drawFastHLineInternal(x, y, w, color);
	}
#else
	SSD1306_drawFastHLineInternal(x, y, w, color);
#endif
}



void SSD1306_drawFastHLineInternal(int16_t x, int16_t y, int16_t w,
		uint16_t color)
{
	// Do bounds/limit checks
	if (y < 0 || y >= SSD1306_LCDHEIGHT)
	{
		return;
	}

	// make sure we don't try to draw below 0
	if (x < 0)
	{
		w += x;
		x = 0;
	}

	// make sure we don't go off the edge of the display
	if ((x + w) > SSD1306_LCDWIDTH)
	{
		w = (SSD1306_LCDWIDTH - x);
	}

	// if our width is now negative, punt
	if (w <= 0)
	{
		return;
	}

	// set up the pointer for  movement through the buffer
	register uint8_t *pBuf = buffer;
	// adjust the buffer pointer for the current row
	pBuf += ((y / 8) * SSD1306_LCDWIDTH);
	// and offset x columns in
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



void SSD1306_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
#ifdef SSD1306_ROTATION_ENABLE
	bool bSwap = false;

	uint8_t rotation = 0;	// TODO:
	switch (rotation)
	{
		case 0:
			break;
		case 1:
			// 90 degree rotation, swap x & y for rotation, then invert x and adjust x for h (now to become w)
			bSwap = true;
			ssd1306_swap(x, y);
			x = SSD1306_LCDWIDTH - x - 1;
			x -= (h - 1);
			break;
		case 2:
			// 180 degree rotation, invert x and y - then shift y around for height.
			x = SSD1306_LCDWIDTH - x - 1;
			y = SSD1306_LCDHEIGHT - y - 1;
			y -= (h - 1);
			break;
		case 3:
			// 270 degree rotation, swap x & y for rotation, then invert y
			bSwap = true;
			ssd1306_swap(x, y);
			y = SSD1306_LCDHEIGHT - y - 1;
			break;
		default:
			break;
	}

	if (bSwap)
	{
		SSD1306_drawFastHLineInternal(x, y, h, color);
	}
	else
	{
		SSD1306_drawFastVLineInternal(x, y, h, color);
	}
#else
	SSD1306_drawFastVLineInternal(x, y, h, color);
#endif
}



void SSD1306_drawFastVLineInternal(int16_t x, int16_t __y, int16_t __h,
		uint16_t color)
{

	// do nothing if we're off the left or right side of the screen
	if (x < 0 || x >= SSD1306_LCDWIDTH)
	{
		return;
	}

	// make sure we don't try to draw below 0
	if (__y < 0)
	{
		// __y is negative, this will subtract enough from __h to account for __y being 0
		__h += __y;
		__y = 0;

	}

	// make sure we don't go past the height of the display
	if ((__y + __h) > SSD1306_LCDHEIGHT)
	{
		__h = (SSD1306_LCDHEIGHT - __y);
	}

	// if our height is now negative, punt
	if (__h <= 0)
	{
		return;
	}

	// this display doesn't need ints for coordinates, use local byte registers for faster juggling
	register uint8_t y = __y;
	register uint8_t h = __h;

	// set up the pointer for fast movement through the buffer
	register uint8_t *pBuf = buffer;
	// adjust the buffer pointer for the current row
	pBuf += ((y / 8) * SSD1306_LCDWIDTH);
	// and offset x columns in
	pBuf += x;

	// do the first partial byte, if necessary - this requires some masking
	register uint8_t mod = (y & 7);
	if (mod)
	{
		// mask off the high n bits we want to set
		mod = 8 - mod;

		// note - lookup table results in a nearly 10% performance improvement in fill* functions
		// register uint8_t mask = ~(0xFF >> (mod));
		static uint8_t premask[8] =
		{ 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
		register uint8_t mask = premask[mod];

		// adjust the mask if we're not going to reach the end of this byte
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

		// fast exit if we're done here!
		if (h < mod)
		{
			return;
		}

		h -= mod;

		pBuf += SSD1306_LCDWIDTH;
	}

	// write solid bytes while we can - effectively doing 8 rows at a time
	if (h >= 8)
	{
		if (color == INVERSE)
		{ // separate copy of the code so we don't impact performance of the black/white write version with an extra comparison per loop
			do
			{
				*pBuf = ~(*pBuf);

				// adjust the buffer forward 8 rows worth of data
				pBuf += SSD1306_LCDWIDTH;

				// adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
				h -= 8;
			} while (h >= 8);
		}
		else
		{
			// store a local value to work with
			register uint8_t val = (color == WHITE) ? 255 : 0;

			do
			{
				// write our value in
				*pBuf = val;

				// adjust the buffer forward 8 rows worth of data
				pBuf += SSD1306_LCDWIDTH;

				// adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
				h -= 8;
			} while (h >= 8);
		}
	}

	// now do the final partial byte, if necessary
	if (h)
	{
		mod = h & 7;
		// this time we want to mask the low bits of the byte, vs the high bits we did above
		// register uint8_t mask = (1 << mod) - 1;
		// note - lookup table results in a nearly 10% performance improvement in fill* functions
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



/**
 * \brief	Display SPI communication IRQ handler
 */
void DISPLAY_SSD1306_SPIx_IRQHandler(void)
{
	HAL_SPI_IRQHandler(&SpiHandle);
}



/**
  * @brief Tx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	// Suppress warning
	(void)hspi;

	HAL_GPIO_WritePin(DISPLAY_SSD1306_SPIx_CS_GPIO_PORT, DISPLAY_SSD1306_SPIx_CS_GPIO_PIN, SET);

	if (Display_TransferInProgress)
	{
		Display_TransferInProgress = false;
#ifdef CONFIG_EVENTLOG_DISPLAY_LOG_ENABLE
		// Save event
		EventHandler_GenerateEvent(Event_Display_SpiEvent, 5, Task_Display);
#endif
	}
}



/**
  * @brief  SPI error callbacks
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	// Suppress warning
	(void)hspi;

	HAL_GPIO_WritePin(DISPLAY_SSD1306_SPIx_CS_GPIO_PORT, DISPLAY_SSD1306_SPIx_CS_GPIO_PIN, SET);
	Display_TransferInProgress = false;

#ifdef CONFIG_EVENTLOG_DISPLAY_LOG_ENABLE
	// Save event
	EventHandler_GenerateEvent(Event_Display_SpiEvent, 4, Task_Display);
#endif
}



#if defined(CONFIG_DISPLAY_SPI_USE_DMA)
/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void SPIx_DMA_TX_IRQHandler(void)
{
	HAL_DMA_IRQHandler(SpiHandle.hdmatx);
}
#endif



#ifdef CONFIG_MODULE_DISPLAY_TEST_WITH_TERMINAL
/**
 * \brief	Print Display (Actual screen) on debug port
 */
void Display_SendOnTerminal(void)
{
	uint8_t x;
	uint8_t y;

	// Print top frame row
	DebugUart_SendChar('+');
	for (x = 0; x < SSD1306_LCDWIDTH; x++) DebugUart_SendChar('-');
	DebugUart_SendLine("+");

	// Print every row
	for (y = 0; y < SSD1306_LCDHEIGHT; y++)
	{
		DebugUart_SendChar('|');

		// Print every pixel on row (=column)
		for (x = 0; x < SSD1306_LCDWIDTH; x++)
		{
			// Print pixel
			char ch = ' ';
			if (buffer[x + (y / 8) * SSD1306_LCDWIDTH] & (1 << (y & 7)))
			{
				ch = '#';
			}
			DebugUart_SendChar(ch);
		}

		// |\r\n
		DebugUart_SendLine("|");
	}

	// Print bottom frame row
	DebugUart_SendChar('+');
	for (x = 0; x < SSD1306_LCDWIDTH; x++) DebugUart_SendChar('-');
	DebugUart_SendLine("+");
}
#endif



#endif	// #ifdef CONFIG_MODULE_DISPLAY_ENABLE
