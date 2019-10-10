/*
 *    board.h
 *    Created on:   2013-11-19
 *    Author:       Vizi Gabor
 *    E-mail:       vizi.gabor90@gmail.com
 *    Function:     board
 *    Target:       STM32Fx
 */

#ifndef BOARD_H_
#define BOARD_H_


#include "options.h" /* which panel used */



/*------------------------------------------------------------------------------
 *                            Board names / defines
 *----------------------------------------------------------------------------*/

#ifdef CONFIG_USE_PANEL_NUCLEOF401RE
    #define BOARD_NAME        "NucleoF401RE"
    #define BOARD_MCU         "STM32F401RE"
#elif CONFIG_USE_PANEL_STM32F4DISCOVERY
    #define BOARD_NAME        "STM32F4Discovery"
    #define BOARD_MCU         "STM32F407VGT6"
#elif CONFIG_USE_PANEL_HOMEAUTCENTERPANEL
    #define BOARD_NAME        "HomeAutCenterPanel"
    #define BOARD_MCU         "-"
#elif  CONFIG_USE_PANEL_HOMEAUTNODESMALL
    #define BOARD_NAME        "HomeAutNodeSmall"
    #define BOARD_MCU         "-"
#elif CONFIG_USE_PANEL_FASTENNODE
    #define BOARD_NAME        "FastenNode"
    #define BOARD_MCU         "STM32F030C8"
#elif CONFIG_USE_PANEL_PC
    #define BOARD_NAME        "FastenPC"
    #define BOARD_MCU         "Win32"
#else
    #warning "Missed BOARD_NAME define!"
    #define BOARD_NAME        "UnknownBoard"
    #define BOARD_MCU         "-"
#endif



#ifdef STM32F030x8
#define MEM_FLASH_START        (0x08000000)
#define MEM_FLASH_END          (0x08000000 + 64*1024)
#define MEM_RAM_START          (0x20000000)
#define MEM_RAM_END            (0x20002000)
#elif defined(STM32F40xx)
#warning "Set memory address values"
#elif defined(CONFIG_PLATFORM_X86)
/* Do not use... */
#define MEM_FLASH_START        (0x00000000)
#define MEM_FLASH_END          (0xFFFFFFFF)
#define MEM_RAM_START          (0x00000000)
#define MEM_RAM_END            (0xFFFFFFFF)
#else
#warning "ERROR! memory FLASH - RAM values are missed"
#endif



/*------------------------------------------------------------------------------
 *                                LED defines
 *----------------------------------------------------------------------------*/


#if defined(CONFIG_MODULE_IO_ENABLE) & defined(CONFIG_USE_PANEL_HOMEAUTCENTERPANEL)

/* @note: In the schematic the blue and red LED are inversely */
/* blue: PE5 */
/* green: PE6 */
/* red: PA8 */

#define LED_NUM_MIN                (1)
#define LED_NUM_MAX                (3)

#define BOARD_LED_BLUE_PORT            GPIOE
#define BOARD_LED_BLUE_PIN            GPIO_PIN_5

#define BOARD_LED_GREEN_PORT        GPIOE
#define BOARD_LED_GREEN_PIN            GPIO_PIN_6

#define BOARD_LED_RED_PORT            GPIOA
#define BOARD_LED_RED_PIN            GPIO_PIN_8

#define IO_PORT_CLK_ENABLES()        \
        __GPIOA_CLK_ENABLE();        \
        __GPIOE_CLK_ENABLE()


#define LED_RED_ON()            HAL_GPIO_WritePin(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN, GPIO_PIN_SET)
#define LED_BLUE_ON()            HAL_GPIO_WritePin(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PIN, GPIO_PIN_SET)
#define LED_GREEN_ON()            HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN, GPIO_PIN_SET)

#define LED_RED_OFF()            HAL_GPIO_WritePin(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN, GPIO_PIN_RESET)
#define LED_BLUE_OFF()            HAL_GPIO_WritePin(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PIN, GPIO_PIN_RESET)
#define LED_GREEN_OFF()            HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN, GPIO_PIN_RESET)

#define LED_RED_TOGGLE()        HAL_GPIO_TogglePin(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN)
#define LED_BLUE_TOGGLE()        HAL_GPIO_TogglePin(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PIN)
#define LED_GREEN_TOGGLE()        HAL_GPIO_TogglePin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN)

#define LED_RED_STATUS()        HAL_GPIO_ReadPin(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN)
#define LED_BLUE_STATUS()        HAL_GPIO_ReadPin(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PIN)
#define LED_GREEN_STATUS()        HAL_GPIO_ReadPin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN)

#endif /* ifdef CONFIG_USE_PANEL_CENTERPANEL */



#if defined(CONFIG_MODULE_IO_ENABLE) & defined(CONFIG_USE_PANEL_STM32F4DISCOVERY)

/* blue: PE5 */
/* green: PE6 */
/* red: PA8 */

#define LED_NUM_MIN                (1)
#define LED_NUM_MAX                (3)


#define BOARD_LED_GREEN_PORT    GPIOD
#define BOARD_LED_GREEN_PIN        GPIO_PIN_12
#define BOARD_LED_BLUE_PORT        GPIOD
#define BOARD_LED_BLUE_PIN        GPIO_PIN_15
#define BOARD_LED_RED_PORT        GPIOD
#define BOARD_LED_RED_PIN        GPIO_PIN_14

#define IO_PORT_CLK_ENABLES()    __GPIOD_CLK_ENABLE()


#define LED_GREEN_ON()            HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN, GPIO_PIN_SET)
#define LED_BLUE_ON()            HAL_GPIO_WritePin(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PIN, GPIO_PIN_SET)
#define LED_RED_ON()            HAL_GPIO_WritePin(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN, GPIO_PIN_SET)

#define LED_GREEN_OFF()            HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN, GPIO_PIN_RESET)
#define LED_BLUE_OFF()            HAL_GPIO_WritePin(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PIN, GPIO_PIN_RESET)
#define LED_RED_OFF()            HAL_GPIO_WritePin(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN, GPIO_PIN_RESET)

#define LED_GREEN_TOGGLE()        HAL_GPIO_TogglePin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN)
#define LED_BLUE_TOGGLE()        HAL_GPIO_TogglePin(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PIN)
#define LED_RED_TOGGLE()        HAL_GPIO_TogglePin(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN)

#define LED_GREEN_STATUS()        HAL_GPIO_ReadPin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN)
#define LED_BLUE_STATUS()        HAL_GPIO_ReadPin(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PIN)
#define LED_RED_STATUS()        HAL_GPIO_ReadPin(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN)

#endif /* ifdef CONFIG_USE_PANEL_CENTERPANEL */



#if defined(CONFIG_MODULE_IO_ENABLE) && defined(CONFIG_USE_PANEL_NUCLEOF401RE)

/* Green user LED: PA5 */

#define LED_NUM_MIN                (1)
#define LED_NUM_MAX                (1)

#define BOARD_LED_GREEN_PORT    GPIOA
#define BOARD_LED_GREEN_PIN        GPIO_PIN_5

#define IO_PORT_CLK_ENABLES()    __GPIOA_CLK_ENABLE()

#define LED_GREEN_ON()            HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN, GPIO_PIN_SET)
#define LED_BLUE_ON()
#define LED_RED_ON()

#define LED_GREEN_OFF()            HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN, GPIO_PIN_RESET)
#define LED_BLUE_OFF()
#define LED_RED_OFF()


#define LED_GREEN_TOGGLE()        HAL_GPIO_TogglePin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN)
#define LED_BLUE_TOGGLE()
#define LED_RED_TOGGLE()

#define LED_GREEN_STATUS()        HAL_GPIO_ReadPin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN)
#define LED_BLUE_STATUS()
#define LED_RED_STATUS()

#endif


#if !defined(CONFIG_MODULE_IO_ENABLE)

/* if not defined CONFIG_MODULE_IO_ENABLE */
#define LED_GREEN_ON()
#define LED_BLUE_ON()
#define LED_RED_ON()
#define LED_GREEN_OFF()
#define LED_BLUE_OFF()
#define LED_RED_OFF()
#define LED_GREEN_TOGGLE()
#define LED_BLUE_TOGGLE()
#define LED_RED_TOGGLE()
#define LED_GREEN_STATUS()
#define LED_BLUE_STATUS()
#define LED_RED_STATUS()

#endif



///////////////////////////////////////////////////////////////////////////////
/*             TIMERS for LEDs */
///////////////////////////////////////////////////////////////////////////////


/* TODO: LED timers / PWM, need beautify */


#ifdef CONFIG_USE_PANEL_NODEMEDIUM

#define TIMx_BLUE                           TIM4
#define TIMx_BLUE_CLK_ENABLE()              __TIM4_CLK_ENABLE()
#define TIMx_GREEN                            TIM4
#define TIMx_GREEN_CLK_ENABLE()              __TIM4_CLK_ENABLE()
#define TIMx_RED                               TIM3
#define TIMx_RED_CLK_ENABLE()              __TIM3_CLK_ENABLE()

#define TIMER_CLK_ENABLES()                    __TIM3_CLK_ENABLE();        \
                                            __TIM4_CLK_ENABLE()


#define LED_BLUE_TIMER_CHANNEL                TIM_CHANNEL_2
#define LED_GREEN_TIMER_CHANNEL                TIM_CHANNEL_1
#define LED_RED_TIMER_CHANNEL                TIM_CHANNEL_2


#define TIMx_LED_BLUE_GPIO_AF                  GPIO_AF2_TIM4
#define TIMx_LED_GREEN_GPIO_AF                GPIO_AF2_TIM4
#define TIMx_LED_RED_GPIO_AF                GPIO_AF2_TIM3

#endif



///////////////////////////////////////////////////////////////////////////////
/*         ADC  - ANALOG */
///////////////////////////////////////////////////////////////////////////////



#ifdef CONFIG_USE_PANEL_NODESMALL


#define BOARD_USE_ADC                ADC1


#define ADC_COMMON_PORT                GPIOA
#define ADC_COMMON_CLK_ENABLE()        __GPIOA_CLK_ENABLE()


#define ADC_CHANNEL_NUM                ( 5 )



/* SENSOR_LIGH_ADC */
/* PA1 */
/* ADC1_IN1 */
#define    ADC_SENSOR_LIGHT_PORT            GPIOA
#define    ADC_SENSOR_LIGHT_CLK            RCC_AHB1Periph_GPIOA
#define    ADC_SENSOR_LIGHT_PIN            GPIO_PIN_1
#define    ADC_SENSOR_LIGHT_CHANNEL        ADC_CHANNEL_1



/* SENSOR_MICROPHONE_ADC */
/* PA6 */
/* ADC1_IN6 */
#define    ADC_SENSOR_MICROPHONE_PORT            GPIOA
#define    ADC_SENSOR_MICROPHONE_CLK            RCC_AHB1Periph_GPIOA
#define    ADC_SENSOR_MICROPHONE_PIN            GPIO_PIN_6
#define    ADC_SENSOR_MICROPHONE_CHANNEL        ADC_CHANNEL_6


/* VSOURCE_ADC */
/* PA7 */
/* ADC1_IN7 */
#define    ADC_VSOURCE_PORT            GPIOA
#define    ADC_VSOURCE_CLK                RCC_AHB1Periph_GPIOA
#define    ADC_VSOURCE_PIN                GPIO_PIN_7
#define    ADC_VSOURCE_CHANNEL            ADC_CHANNEL_7


#define ADC_TEMPSENSOR_CHANNEL        ADC_CHANNEL_16
#define ADC_VREFINT_CHANNEL            ADC_CHANNEL_17


#endif    /* CONFIG_USE_PANEL_NODEMSMALL */



#ifdef CONFIG_USE_PANEL_NODEMEDIUM


/* ANALOG */
#define BOARD_USE_ADC                ADC1


#define ADC_COMMON_PORT                GPIOB | GPIOC
#define ADC_COMMON_CLK_ENABLE()        \
        __GPIOB_CLK_ENABLE();        \
        __GPIOC_CLK_ENABLE()


#define ADC_CHANNEL_NUM                ( 5 )
/* now: Vsource, ligh, mic, prox, internal temp */



/* SENSOR_LIGH_ADC */
/* PC0 */
/* ADC_IN10 */
#define    ADC_SENSOR_LIGHT_PORT            GPIOC
#define    ADC_SENSOR_LIGHT_PIN            GPIO_PIN_0
#define    ADC_SENSOR_LIGHT_CHANNEL        ADC_CHANNEL_10



/* SENSOR_MICROPHONE_ADC */
/* PC1 */
/* ADC_IN11 */
#define    ADC_SENSOR_MICROPHONE_PORT            GPIOC
#define    ADC_SENSOR_MICROPHONE_PIN            GPIO_PIN_1
#define    ADC_SENSOR_MICROPHONE_CHANNEL        ADC_CHANNEL_11



/* SENSOR_PROXIMITY_ADC */
/* PC2 */
/* ADC_IN12 */
#define    ADC_SENSOR_PROXIMITY_PORT            GPIOC
#define    ADC_SENSOR_PROXIMITY_PIN            GPIO_PIN_2
#define    ADC_SENSOR_PROXIMITY_CHANNEL        ADC_CHANNEL_12



/* VSOURCE_ADC */
/* PB12 */
/* ADC_IN18 */
#define    ADC_VSOURCE_PORT            GPIOB
#define    ADC_VSOURCE_PIN                GPIO_PIN_12
#define    ADC_VSOURCE_CHANNEL            ADC_CHANNEL_18



/* NOTE: L1 not included internal Tempsensor :( */
#define ADC_TEMPSENSOR_CHANNEL        ADC_CHANNEL_16
#define ADC_VREFINT_CHANNEL            ADC_CHANNEL_17


#endif    /* CONFIG_USE_PANEL_NODEMEDIUM */



#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_NUCLEOF401RE)

/* ANALOG */

#define ADC_CHANNEL_NUM                    (3)


#define ADCx                            ADC1
#define ADCx_CLK_ENABLE()               __HAL_RCC_ADC1_CLK_ENABLE()
#define DMAx_CLK_ENABLE()               __HAL_RCC_DMA2_CLK_ENABLE()
#define ADCx_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()

/* Definition for ADCx Channel Pin */
#define ADCx_CHANNEL_PIN_1                GPIO_PIN_1
#define ADCx_CHANNEL_PIN_2                GPIO_PIN_2
#define ADCx_CHANNEL_PIN_3                GPIO_PIN_3
#define ADCx_CHANNEL_GPIO_PORT            GPIOA

/* Definition for ADCx's Channel */
#define ADCx_CHANNEL_1                    ADC_CHANNEL_1
#define ADCx_CHANNEL_2                    ADC_CHANNEL_2
#define ADCx_CHANNEL_3                    ADC_CHANNEL_3

/* Definition for ADCx's DMA */
#define ADCx_DMA_CHANNEL                DMA_CHANNEL_0
#define ADCx_DMA_STREAM                 DMA2_Stream0

/* Definition for ADCx's NVIC */
#define ADCx_DMA_IRQn                   DMA2_Stream0_IRQn
#define ADCx_DMA_IRQHandler             DMA2_Stream0_IRQHandler

/* ADC DMA Interrupt priorities */
#define ADC_DMA_PREEMT_PRIORITY            15
#define ADC_DMA_SUB_PRIORITY            0


#define COMMON_ADC_VOLTAGE_MAX            (3.0f)
#define COMMON_ADC_RESOLUTION_MAX        (4095U)

#endif



///////////////////////////////////////////////////////////////////////////////
/*            DAC */
///////////////////////////////////////////////////////////////////////////////



#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_NUCLEOF401RE)


#define DACx                            DAC

/* Definition for DAC clock resources */
#define DACx_CHANNEL_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define DACx_CLK_ENABLE()                __HAL_RCC_DAC_CLK_ENABLE()

/* Definition for DACx Channel1 Pin */
#define DACx_CHANNEL1_PIN                GPIO_PIN_4
#define DACx_CHANNEL1_GPIO_PORT            GPIOA

#define DACx_CHANNEL2_PIN                GPIO_PIN_5
#define DACx_CHANNEL2_GPIO_PORT            GPIOA

#define DACx_CHANNEL1                    DAC_CHANNEL_1
#define DACx_CHANNEL2                    DAC_CHANNEL_2


#define COMMON_DAC_MAX_VOLTAGE            (3.0f)
#define COMMON_DAC_DA_MAX_VALUE            (4095)


#endif



///////////////////////////////////////////////////////////////////////////////
/*            BUTTONS */
///////////////////////////////////////////////////////////////////////////////



#ifdef CONFIG_USE_PANEL_NODEMEDIUM

#define BUTTON_NUM                    4

#define BUTTON_RIGHT_GPIO_PORT        GPIOA
#define BUTTON_RIGHT_GPIO_PIN        GPIO_PIN_0

#define BUTTON_UP_GPIO_PORT            GPIOD
#define BUTTON_UP_GPIO_PIN            GPIO_PIN_2

#define BUTTON_DOWN_GPIO_PORT        GPIOC
#define BUTTON_DOWN_GPIO_PIN        GPIO_PIN_13

#define BUTTON_LEFT_GPIO_PORT        GPIOB
#define BUTTON_LEFT_GPIO_PIN        GPIO_PIN_9



#define BUTTON_CLK_ENABLES()        \
        __GPIOA_CLK_ENABLE();        \
        __GPIOB_CLK_ENABLE();        \
        __GPIOC_CLK_ENABLE();        \
        __GPIOD_CLK_ENABLE()        \


#endif    /* #ifdef CONFIG_USE_PANEL_NODEMEDIUM */



#ifdef CONFIG_USE_PANEL_HOMEAUTCENTERPANEL

/* Up        PC7 */
/* Down        PC8 */
/* Right    PA0 */
/* Left        PC9 */

#define BUTTON_NUM                    4

#define BUTTON_RIGHT_GPIO_PORT        GPIOA
#define BUTTON_RIGHT_GPIO_PIN        GPIO_PIN_0

#define BUTTON_UP_GPIO_PORT            GPIOC
#define BUTTON_UP_GPIO_PIN            GPIO_PIN_7

#define BUTTON_DOWN_GPIO_PORT        GPIOC
#define BUTTON_DOWN_GPIO_PIN        GPIO_PIN_8

#define BUTTON_LEFT_GPIO_PORT        GPIOC
#define BUTTON_LEFT_GPIO_PIN        GPIO_PIN_9



#define BUTTON_CLK_ENABLES()        \
        __GPIOA_CLK_ENABLE();        \
        __GPIOC_CLK_ENABLE()


#endif    /* #ifdef CONFIG_USE_PANEL_CENTERPANEL */



#ifdef CONFIG_USE_PANEL_STM32F4DISCOVERY

/* User button: PA0 */

#define BUTTON_NUM                                    1

#define BUTTON_USER_GPIO_PORT                        GPIOA
#define BUTTON_USER_GPIO_PIN                        GPIO_PIN_0

#define BUTTON_CLK_ENABLES()                        __GPIOA_CLK_ENABLE()

/* Button Interrupt priority */
#define BUTTON_USER_EXTI_IRQn                        EXTI0_IRQn
#define BUTTON_USER_INTERRUPT_PREEMT_PRIORITY        10
#define BUTTON_USER_INTERRUPT_SUB_PRIORITY            0

#endif



#ifdef CONFIG_USE_PANEL_NUCLEOF401RE

/* User button: PC13 */

#define BUTTON_NUM                                    1

#define BUTTON_USER_GPIO_PORT                        GPIOC
#define BUTTON_USER_GPIO_PIN                        GPIO_PIN_13

#define BUTTON_CLK_ENABLES()                        __GPIOC_CLK_ENABLE()

/* Button Interrupt priority */
#define BUTTON_USER_EXTI_IRQn                        EXTI15_10_IRQn
#define BUTTON_USER_INTERRUPT_PREEMT_PRIORITY        10
#define BUTTON_USER_INTERRUPT_SUB_PRIORITY            0

#endif



///////////////////////////////////////////////////////////////////////////////
/*            TEMPERATURE - I2C - LM75 */
///////////////////////////////////////////////////////////////////////////////

#ifdef CONFIG_MODULE_ADC_ENABLE
/* I2C_SCL        PB10 */
/* I2C_SDA        PB11 */


/* SB LSB */
/*  0 0 1 A2 A1 A0 */
#define TEMPERATURE_I2C_TEMP_ADDRESS    ((uint16_t)( 0x0000 | (1<<6)  | (1 << 3 ) ) )
/* 0b1001000 */
/* 1,0,0,1,0,0,0 BITs, and not hex !!!!!!!!!!!!!! */
/* Wrong example: 0x1001000 */

/* pointer register: */
#define TEMPERATURE_I2C_REGISTER_TEMP    ((uint8_t)0x00)

#endif    /* CONFIG_MODULE_ADC_ENABLE */

#ifdef CONFIG_USE_PANEL_NODESMALL
#define I2Cx                             I2C1
#define I2Cx_CLK_ENABLE()                __I2C1_CLK_ENABLE()
#define DMAx_CLK_ENABLE()                __DMA1_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()       __GPIOB_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()       __GPIOB_CLK_ENABLE()

#define I2Cx_FORCE_RESET()               __I2C1_FORCE_RESET()
#define I2Cx_RELEASE_RESET()             __I2C1_RELEASE_RESET()

/* I2C TIMING is calculated in case of the I2C Clock source is the SYSCLK = 48 MHz */
/* Set TIMING to 0x00E0D3FF to reach 100 KHz speed (Rise time = 50ns, Fall time = 10ns) */
/* define I2Cx_TIMING                      0x00E0D3FF */
/* define I2Cx_TIMING                      0x00201D2A */
/* 100kHz, 100ns rise, 200ns fall */

#define I2Cx_TIMING                      0x00301D28
/* 100kHz, 250ns rise, 250ns fall */

/* om.l2fprod.common.propertysheet.PropertySheetTableModel$Item@9cec42    0x00301D28 */


/* om.l2fprod.common.propertysheet.PropertySheetTableModel$Item@198731b    0x2000090E */

/* om.l2fprod.common.propertysheet.PropertySheetTableModel$Item@7f591a    0x00201D2A */

/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_10
#define I2Cx_SCL_GPIO_PORT              GPIOB
#define I2Cx_SCL_AF                     GPIO_AF1_I2C1

#define I2Cx_SDA_PIN                    GPIO_PIN_11
#define I2Cx_SDA_GPIO_PORT              GPIOB
#define I2Cx_SDA_AF                     GPIO_AF1_I2C1


/* Definition for I2Cx's DMA */
#define I2Cx_TX_DMA_INSTANCE            DMA1_Channel2
#define I2Cx_RX_DMA_INSTANCE            DMA1_Channel3

/* Definition for I2Cx's NVIC */
#define I2Cx_DMA_TX_IRQn                DMA1_Channel2_3_IRQn
#define I2Cx_DMA_RX_IRQn                DMA1_Channel2_3_IRQn
#define I2Cx_DMA_TX_RX_IRQHandler       DMA1_Channel2_3_IRQHandler

/* Size of Trasmission buffer */
/* define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1) */
/* Size of Reception buffer */
/* define RXBUFFERSIZE                      TXBUFFERSIZE */


#endif /* #ifdef CONFIG_USE_PANEL_NODESMALL */



#ifdef CONFIG_USE_PANEL_NODEMEDIUM

#define I2Cx                             I2C2
#define I2Cx_CLK_ENABLE()                __I2C2_CLK_ENABLE()
#define DMAx_CLK_ENABLE()                __DMA1_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()       __GPIOB_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()       __GPIOB_CLK_ENABLE()

#define I2Cx_FORCE_RESET()               __I2C1_FORCE_RESET()
#define I2Cx_RELEASE_RESET()             __I2C1_RELEASE_RESET()

/* I2C TIMING is calculated in case of the I2C Clock source is the SYSCLK = 48 MHz */
/* Set TIMING to 0x00E0D3FF to reach 100 KHz speed (Rise time = 50ns, Fall time = 10ns) */
/* define I2Cx_TIMING                      0x00E0D3FF */
/* define I2Cx_TIMING                      0x00201D2A */
/* 100kHz, 100ns rise, 200ns fall */

#define I2Cx_TIMING                      0x00301D28
/* 100kHz, 250ns rise, 250ns fall */

/* om.l2fprod.common.propertysheet.PropertySheetTableModel$Item@9cec42    0x00301D28 */


/* om.l2fprod.common.propertysheet.PropertySheetTableModel$Item@198731b    0x2000090E */

/* om.l2fprod.common.propertysheet.PropertySheetTableModel$Item@7f591a    0x00201D2A */

/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_10
#define I2Cx_SCL_GPIO_PORT              GPIOB
#define I2Cx_SCL_AF                     GPIO_AF4_I2C2

#define I2Cx_SDA_PIN                    GPIO_PIN_11
#define I2Cx_SDA_GPIO_PORT              GPIOB
#define I2Cx_SDA_AF                     GPIO_AF4_I2C2


/* Definition for I2Cx's DMA */
#define I2Cx_TX_DMA_INSTANCE            DMA1_Channel2
#define I2Cx_RX_DMA_INSTANCE            DMA1_Channel3

/* Definition for I2Cx's NVIC */
#define I2Cx_DMA_TX_IRQn                DMA1_Channel2_3_IRQn
#define I2Cx_DMA_RX_IRQn                DMA1_Channel2_3_IRQn
#define I2Cx_DMA_TX_RX_IRQHandler       DMA1_Channel2_3_IRQHandler

/* Size of Trasmission buffer */
/* define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1) */
/* Size of Reception buffer */
/* define RXBUFFERSIZE                      TXBUFFERSIZE */

#endif /* #ifdef CONFIG_USE_PANEL_NODEMEDIUM */



///////////////////////////////////////////////////////////////////////////////
/*            FLASH - SPI */
///////////////////////////////////////////////////////////////////////////////



#ifdef CONFIG_USE_PANEL_NODESMALL


#define FLASH_SPIx                             SPI1

#define DISPLAY_SSD1306_SPIx_CLK_ENABLE()                __SPI1_CLK_ENABLE()

#define FLASH_SPIx_PINS_CLK_ENABLES()    \
        __GPIOA_CLK_ENABLE();            \
        __GPIOB_CLK_ENABLE()



#define FLASH_SPI_CS                PB7
#define FLASH_SPI_WP                PB6
#define FLASH_SPI_HOLD                PA15


#define FLASH_SPI_CS_GPIO_PIN            GPIO_PIN_7
#define FLASH_SPI_CS_GPIO_PORT            GPIOB
#define FLASH_SPI_WP_GPIO_PIN            GPIO_PIN_6
#define FLASH_SPI_WP_GPIO_PORT            GPIOB
#define FLASH_SPI_HOLD_GPIO_PIN            GPIO_PIN_15
#define FLASH_SPI_HOLD_GPIO_PORT        GPIOA



#define DISPLAY_SSD1306_SPIx_FORCE_RESET()               __SPI1_FORCE_RESET()
#define DISPLAY_SSD1306_SPIx_RELEASE_RESET()             __SPI1_RELEASE_RESET()

/* Definition for DISPLAY_SSD1306_SPIx Pins */
#define DISPLAY_SSD1306_SPIx_SCK_PIN                     GPIO_PIN_3
#define DISPLAY_SSD1306_SPIx_SCK_GPIO_PORT               GPIOB
#define DISPLAY_SSD1306_SPIx_SCK_AF                      GPIO_AF0_SPI1
#define SPIx_MISO_PIN                    GPIO_PIN_4
#define SPIx_MISO_GPIO_PORT              GPIOB
#define SPIx_MISO_AF                     GPIO_AF0_SPI1
#define DISPLAY_SSD1306_SPIx_MOSI_PIN                    GPIO_PIN_5
#define DISPLAY_SSD1306_SPIx_MOSI_GPIO_PORT              GPIOB
#define DISPLAY_SSD1306_SPIx_MOSI_AF                     GPIO_AF0_SPI1

#endif



#ifdef CONFIG_USE_PANEL_NODEMEDIUM


/* FLASH_SPI_CS                PB0 */
/* FLASH_SPI_WP                PB1 */
/* FLASH_SPI_HOLD            PA4 */
/* FLASH - SPI */
/* CLK                        PA5 */
/* MISO                        PA6 */
/* MOSI                        PA7 */



#define FLASH_SPIx                        SPI1


#define FLASH_SPIx_PINS_CLK_ENABLES()    \
        __GPIOA_CLK_ENABLE();            \
        __GPIOB_CLK_ENABLE()


#define DISPLAY_SSD1306_SPIx_CLK_ENABLE()                __SPI1_CLK_ENABLE()


#define DISPLAY_SSD1306_SPIx_FORCE_RESET()                __SPI1_FORCE_RESET()
#define DISPLAY_SSD1306_SPIx_RELEASE_RESET()            __SPI1_RELEASE_RESET()


/* Definition for DISPLAY_SSD1306_SPIx Pins */
#define DISPLAY_SSD1306_SPIx_SCK_PIN                    GPIO_PIN_5
#define DISPLAY_SSD1306_SPIx_SCK_GPIO_PORT                GPIOA
#define DISPLAY_SSD1306_SPIx_SCK_AF                        GPIO_AF5_SPI1
#define SPIx_MISO_PIN                    GPIO_PIN_6
#define SPIx_MISO_GPIO_PORT                GPIOA
#define SPIx_MISO_AF                    GPIO_AF5_SPI1
#define DISPLAY_SSD1306_SPIx_MOSI_PIN                    GPIO_PIN_7
#define DISPLAY_SSD1306_SPIx_MOSI_GPIO_PORT                GPIOA
#define DISPLAY_SSD1306_SPIx_MOSI_AF                    GPIO_AF5_SPI1


#define FLASH_SPI_CS_GPIO_PIN            GPIO_PIN_0
#define FLASH_SPI_CS_GPIO_PORT            GPIOB
#define FLASH_SPI_WP_GPIO_PIN            GPIO_PIN_1
#define FLASH_SPI_WP_GPIO_PORT            GPIOB
#define FLASH_SPI_HOLD_GPIO_PIN            GPIO_PIN_4
#define FLASH_SPI_HOLD_GPIO_PORT        GPIOA


#endif    /* CONFIG_USE_PANEL_NODEMEDIUM */



///////////////////////////////////////////////////////////////////////////////
/* DEBUG - USART */
///////////////////////////////////////////////////////////////////////////////



#if defined(CONFIG_USE_PANEL_NODEMEDIUM) || defined(CONFIG_USE_PANEL_HOMEAUTCENTERPANEL)

/* USART */
/* DEBUG_USART        USART1 */
/* DEBUG_USART_TX    -    PA9 */
/* DEBUG_USART_RX    -    PA10 */


#define DEBUG_USARTx                    USART1


/* define DEBUG_USART_CLK_ENABLE()        __USART1_CLK_ENABLE() */

#define DEBUG_USART_CLK_ENABLES()        \
        __USART1_CLK_ENABLE();            \
        __GPIOA_CLK_ENABLE()


#define DEBUG_USART_TX_GPIO_PORT        GPIOA
#define DEBUG_USART_TX_GPIO_PIN            GPIO_PIN_9

#define DEBUG_USART_RX_GPIO_PORT        GPIOA
#define DEBUG_USART_RX_GPIO_PIN            GPIO_PIN_10

#define DEBUG_USART_AF                    GPIO_AF7_USART1

#define DEBUG_USART_BAUDRATE            9600

/* Definition for USARTx's NVIC */
#define DEBUG_USARTx_IRQn               USART1_IRQn
#define DEBUG_USARTx_IRQHandler         USART1_IRQHandler

#define DEBUG_USART_PREEMT_PRIORITY        10
#define DEBUG_USART_SUB_PRIORITY        0

#endif



#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_NUCLEOF401RE)

/* USART */
/* DEBUG_USART            USART6 */
/* DEBUG_USART_TX        PC6 */
/* DEBUG_USART_RX        PC7 */

#define DEBUG_USARTx                    USART6

#define DEBUG_USART_CLK_ENABLES()        \
        __USART6_CLK_ENABLE();            \
        __GPIOC_CLK_ENABLE()


#define DEBUG_USART_TX_GPIO_PORT        GPIOC
#define DEBUG_USART_TX_GPIO_PIN            GPIO_PIN_6

#define DEBUG_USART_RX_GPIO_PORT        GPIOC
#define DEBUG_USART_RX_GPIO_PIN            GPIO_PIN_7

#define DEBUG_USART_AF                    GPIO_AF8_USART6

#define DEBUG_USART_BAUDRATE            115200


/* Definition for USARTx's NVIC */
#define DEBUG_USARTx_IRQn               USART6_IRQn
#define DEBUG_USARTx_IRQHandler         USART6_IRQHandler

#define DEBUG_USART_PREEMT_PRIORITY        10
#define DEBUG_USART_SUB_PRIORITY        0


#endif



///////////////////////////////////////////////////////////////////////////////
/*            ESP8266        -    USART */
///////////////////////////////////////////////////////////////////////////////



#ifdef CONFIG_USE_PANEL_NODEMEDIUM

/* USART2 */
/* USART_TX        PA2 */
/* USART_RX        PA3 */

#define ESP8266_USARTx                        USART2


#define ESP8266_USART_CLK_ENABLES()            \
        __USART2_CLK_ENABLE();                \
        __GPIOA_CLK_ENABLE()



#define ESP8266_USART_TX_GPIO_PORT            GPIOA
#define ESP8266_USART_TX_GPIO_PIN            GPIO_PIN_2

#define ESP8266_USART_RX_GPIO_PORT            GPIOA
#define ESP8266_USART_RX_GPIO_PIN            GPIO_PIN_3

#define ESP8266_USART_AF                    GPIO_AF7_USART2

/* Definition for USARTx's NVIC */
#define ESP8266_USARTx_IRQn                  USART2_IRQn
#define ESP8266_USARTx_IRQHandler            USART2_IRQHandler

#define ESP8266_USART_PREEMT_PRIORITY        14
#define ESP8266_USART_SUB_PRIORITY            0

#define ESP8266_USART_BAUDRATE                9600



#endif /* #ifdef CONFIG_USE_PANEL_NODEMEDIUM */



#ifdef CONFIG_USE_PANEL_HOMEAUTCENTERPANEL

/* USART2 */
/* USART_TX        PA2 */
/* USART_RX        PA3 */

#define ESP8266_USARTx                        USART2

#define ESP8266_USART_CLK_ENABLES()            \
        __USART2_CLK_ENABLE();                \
        __GPIOA_CLK_ENABLE()


#define ESP8266_USART_TX_GPIO_PORT            GPIOA
#define ESP8266_USART_TX_GPIO_PIN            GPIO_PIN_2

#define ESP8266_USART_RX_GPIO_PORT            GPIOA
#define ESP8266_USART_RX_GPIO_PIN            GPIO_PIN_3

#define ESP8266_USART_AF                    GPIO_AF7_USART2

/* Definition for USARTx's NVIC */
#define ESP8266_USARTx_IRQn                  USART2_IRQn
#define ESP8266_USARTx_IRQHandler            USART2_IRQHandler

#define ESP8266_USART_PREEMT_PRIORITY        14
#define ESP8266_USART_SUB_PRIORITY            0

#define ESP8266_USART_BAUDRATE                9600


#endif /* #ifdef CONFIG_USE_PANEL_CENTERPANEL */



#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) && defined(CONFIG_MODULE_ESP8266_ENABLE)

/* USART3 */
/* USART_TX        PD8 */
/* USART_RX        PB11 */

#define ESP8266_USARTx                        USART3

#define ESP8266_USART_CLK_ENABLES()            \
        __USART3_CLK_ENABLE();                \
        __GPIOB_CLK_ENABLE();                \
        __GPIOD_CLK_ENABLE()


#define ESP8266_USART_TX_GPIO_PORT            GPIOD
#define ESP8266_USART_TX_GPIO_PIN            GPIO_PIN_8

#define ESP8266_USART_RX_GPIO_PORT            GPIOB
#define ESP8266_USART_RX_GPIO_PIN            GPIO_PIN_11

#define ESP8266_USART_AF                    GPIO_AF7_USART3

/* Definition for USARTx's NVIC */
#define ESP8266_USARTx_IRQn                  USART3_IRQn
#define ESP8266_USARTx_IRQHandler            USART3_IRQHandler

#define ESP8266_USART_PREEMT_PRIORITY        11
#define ESP8266_USART_SUB_PRIORITY            0

#define ESP8266_USART_BAUDRATE                9600


/*
ESP8266    - Other pins
_GPIO0             PA12
_RST            PB15
_CH                PA8
_GPIO2            PA11
*/

#define ESP8266_GPIO0_GPIO_PIN            GPIO_PIN_12
#define ESP8266_GPIO0_GPIO_PORT            GPIOA
#define ESP8266_RST_GPIO_PIN            GPIO_PIN_15
#define ESP8266_RST_GPIO_PORT            GPIOB

#define ESP8266_CH_GPIO_PIN                GPIO_PIN_8
#define ESP8266_CH_GPIO_PORT            GPIOA
#define ESP8266_GPIO2_GPIO_PIN            GPIO_PIN_11
#define ESP8266_GPIO2_GPIO_PORT            GPIOA


#define ESP8266_PINS_CLK_ENABLES()        \
        __GPIOB_CLK_ENABLE();            \
        __GPIOA_CLK_ENABLE()



#endif /* #ifdef CONFIG_USE_PANEL_CENTERPANEL */



///////////////////////////////////////////////////////////////////////////////
/*            ESP8266        -    OTHER PINS */
///////////////////////////////////////////////////////////////////////////////


#ifdef CONFIG_USE_PANEL_NODEMEDIUM

/*
ESP8266
_GPIO0             PA1
_RST            PC3
_CH                PC9
_GPIO2            PA8
*/


#define ESP8266_GPIO0_GPIO_PIN            GPIO_PIN_1
#define ESP8266_GPIO0_GPIO_PORT            GPIOA
#define ESP8266_RST_GPIO_PIN            GPIO_PIN_3
#define ESP8266_RST_GPIO_PORT            GPIOC

#define ESP8266_CH_GPIO_PIN                GPIO_PIN_9
#define ESP8266_CH_GPIO_PORT            GPIOC
#define ESP8266_GPIO2_GPIO_PIN            GPIO_PIN_8
#define ESP8266_GPIO2_GPIO_PORT            GPIOA


#define ESP8266_PINS_CLK_ENABLES()        \
        __GPIOA_CLK_ENABLE();            \
        __GPIOC_CLK_ENABLE()



#endif    /* #ifdef CONFIG_USE_PANEL_NODEMEDIUM */



#ifdef CONFIG_USE_PANEL_HOMEAUTCENTERPANEL

/*
ESP8266
_GPIO0             PC13
_RST            PE2
_CH                PB9
_GPIO2            PB8
*/


#define ESP8266_GPIO0_GPIO_PORT            GPIOC
#define ESP8266_GPIO0_GPIO_PIN            GPIO_PIN_13

#define ESP8266_RST_GPIO_PORT            GPIOE
#define ESP8266_RST_GPIO_PIN            GPIO_PIN_2

#define ESP8266_CH_GPIO_PORT            GPIOB
#define ESP8266_CH_GPIO_PIN                GPIO_PIN_9

#define ESP8266_GPIO2_GPIO_PORT            GPIOB
#define ESP8266_GPIO2_GPIO_PIN            GPIO_PIN_8



#define ESP8266_PINS_CLK_ENABLES()        \
        __GPIOB_CLK_ENABLE();            \
        __GPIOC_CLK_ENABLE();            \
        __GPIOE_CLK_ENABLE()



#endif    /* #ifdef CONFIG_USE_PANEL_CENTERPANEL */



///////////////////////////////////////////////////////////////////////////////
/*            IO */
///////////////////////////////////////////////////////////////////////////////




#ifdef CONFIG_USE_PANEL_NODEMEDIUM
#ifdef CONFIG_MODULE_IO_INPUT_MOTION_ENABLE

/* SENSOR_MOTION        PC12 */
/* SENSOR_MOITON_EN        X */

/* SENSOR_SOUND_IMPACT    PB14 */


#define SENSOR_MOTION_GPIO_PINS_CLK_ENABLE()            __GPIOC_CLK_ENABLE()

/* PIR signaling; HIGH = movement/LOW = no movement */
#define SENSOR_MOTION_GPIO_PORT                            GPIOC
#define SENSOR_MOTION_GPIO_PIN                            GPIO_PIN_12

#endif


#ifdef CONFIG_MODULE_IO_INPUT_SOUNDIMPACT_ENABLE

#define SENSOR_SOUND_GPIO_PINS_CLK_ENABLE()                __GPIOB_CLK_ENABLE()

/* SOUND_IMPACT: high, when sound */
#define SENSOR_SOUND_IMPACT_GPIO_PORT                    GPIOB
#define SENSOR_SOUND_IMPACT_GPIO_PIN                    GPIO_PIN_14


/* Input Interrupt priority */
#define SENSOR_SOUND_MOTION_INT_EXTI_IRQn                EXTI15_10_IRQn
#define SENSOR_SOUND_MOTIOn_INT_PREEMT_PRIORITY            4
#define SENSOR_SOUND_MOTION_INT_SUB_PRIORITY            0

#endif


#if defined(CONFIG_MODULE_IO_INPUT_MOTION_ENABLE) + defined(CONFIG_MODULE_IO_INPUT_SOUNDIMPACT_ENABLE)
    #define IO_INPUTS_NUM    1
#else
    #define IO_INPUTS_NUM    0
#endif

#endif



#ifdef CONFIG_MODULE_IO_OUTPUT_EXAMPLE_ENABLE

#ifdef CONFIG_USE_PANEL_NODEMEDIUM

/* DIGITAL_INPUT_2        PC8 */
/* RELAY_1_EN            PC11 */
/* RELAY_2_EN            PC10 */

/*
DO NOT USE THEM AT NODEMEDIUM:
DIGITAL_INPUT_1        PC7
SENSOR_OPENCLOSE    PC6
*/

#define RELAY_1_GPIO_PORT            GPIOC
#define RELAY_1_GPIO_PIN            GPIO_PIN_11

#define RELAY_2_GPIO_PORT            GPIOC
#define RELAY_2_GPIO_PIN            GPIO_PIN_10



#define RELAY_PINS_CLK_ENABLES()    __GPIOC_CLK_ENABLE()

#endif


#ifdef CONFIG_USE_PANEL_HOMEAUTCENTERPANEL

/* RELAY_1_EN            PD3 */
/* RELAY_2_EN            PD4 */

#define RELAY_1_GPIO_PORT            GPIOD
#define RELAY_1_GPIO_PIN            GPIO_PIN_3

#define RELAY_2_GPIO_PORT            GPIOD
#define RELAY_2_GPIO_PIN            GPIO_PIN_4



#define RELAY_PINS_CLK_ENABLES()    __GPIOD_CLK_ENABLE()

#endif

#endif


#if defined(CONFIG_USE_PANEL_NODEMEDIUM) || defined(CONFIG_USE_PANEL_HOMEAUTCENTERPANEL)
#define    RELAY_1_OFF()        HAL_GPIO_WritePin(RELAY_1_GPIO_PORT, RELAY_1_GPIO_PIN, GPIO_PIN_RESET)
#define RELAY_1_ON()        HAL_GPIO_WritePin(RELAY_1_GPIO_PORT, RELAY_1_GPIO_PIN, GPIO_PIN_SET)
#define RELAY_1_TOGGLE()    HAL_GPIO_TogglePin(RELAY_1_GPIO_PORT, RELAY_1_GPIO_PIN)


#define    RELAY_2_OFF()        HAL_GPIO_WritePin(RELAY_2_GPIO_PORT, RELAY_2_GPIO_PIN, GPIO_PIN_RESET)
#define RELAY_2_ON()        HAL_GPIO_WritePin(RELAY_2_GPIO_PORT, RELAY_2_GPIO_PIN, GPIO_PIN_SET)
#define RELAY_2_TOGGLE()    HAL_GPIO_TogglePin(RELAY_2_GPIO_PORT, RELAY_2_GPIO_PIN)
#endif



///////////////////////////////////////////////////////////////////////////////
/*            ETC */
///////////////////////////////////////////////////////////////////////////////


#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) && defined(CONFIG_MODULE_MOTOR_ENABLE)


/* RobonAUT - DinamoSaurus 2015 board: PB8, Motor control, TIM4-CH3 / TIM10-CH1 */
#define MOTOR_MOTORS_PWM_GPIO_CLK_ENABLES()        __GPIOB_CLK_ENABLE()

#define MOTOR_PWM_DCMOTOR_PORT                    GPIOB
#define MOTOR_DCMOTOR_PWM_GPIO_PIN                GPIO_PIN_8
/* RobonAUT - DinamoSaurus 2015 board: PB7, Servo, TIM4-CH2 */
#define MOTOR_SERVOMOTOR_PWM_GPIO_PORT            GPIOB
#define MOTOR_SERVOMOTOR_PWM_GPIO_PIN            GPIO_PIN_7


#define MOTOR_MOTORS_PWM_TIMER_CLK_ENABLES()    __TIM4_CLK_ENABLE();    \
                                                __TIM10_CLK_ENABLE()
#define MOTOR_DCMOTOR_PWM_TIMx                    TIM10
#define MOTOR_SERVOMOTOR_PWM_TIMx                TIM4

#define MOTOR_DCMOTOR_PWM_TIMER_CHANNEL            TIM_CHANNEL_1
#define MOTOR_SERVOMOTOR_PWM_TIMER_CHANNEL        TIM_CHANNEL_2

#define MOTOR_DCMOTOR_PWM_TIMx_GPIO_AF          GPIO_AF3_TIM10
#define MOTOR_SERVOMOTOR_PWM_TIMx_GPIO_AF        GPIO_AF2_TIM4


#define MOTOR_DCMOTOR_DIRS_CLK_ENABLES()        __GPIOA_CLK_ENABLE();    \
                                                __GPIOE_CLK_ENABLE()

#define MOTOR_DCMOTOR_DIR1_GPIO_PORT            GPIOA
#define MOTOR_DCMOTOR_DIR1_GPIO_PIN                GPIO_PIN_1

#define MOTOR_DCMOTOR_DIR2_GPIO_PORT            GPIOE
#define MOTOR_DCMOTOR_DIR2_GPIO_PIN                GPIO_PIN_6


#endif



#ifdef CONFIG_USE_PANEL_STM32F4DISCOVERY


#ifdef CONFIG_MODULE_DISPLAY_ENABLE

/* Display - SSD1306 - 0.96'OLED */

#define DISPLAY_SSD1306_SPIx                            SPI2
#define DISPLAY_SSD1306_SPIx_CLK_ENABLE()                __HAL_RCC_SPI2_CLK_ENABLE()
#define DISPLAY_SSD1306_SPIx_SCK_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define DISPLAY_SSD1306_SPIx_MOSI_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()

#define DISPLAY_SSD1306_SPIx_FORCE_RESET()                __HAL_RCC_SPI2_FORCE_RESET()
#define DISPLAY_SSD1306_SPIx_RELEASE_RESET()            __HAL_RCC_SPI2_RELEASE_RESET()

/* Definition for DISPLAY_SSD1306_SPIx Pins */
#define DISPLAY_SSD1306_SPIx_SCK_PIN                    GPIO_PIN_13
#define DISPLAY_SSD1306_SPIx_SCK_GPIO_PORT                GPIOB
#define DISPLAY_SSD1306_SPIx_SCK_AF                        GPIO_AF5_SPI2

#define DISPLAY_SSD1306_SPIx_MOSI_PIN                    GPIO_PIN_15
#define DISPLAY_SSD1306_SPIx_MOSI_GPIO_PORT                GPIOB
#define DISPLAY_SSD1306_SPIx_MOSI_AF                    GPIO_AF5_SPI2


#define DISPLAY_SSD1306_PINS_CLK_ENABLE()                __HAL_RCC_GPIOD_CLK_ENABLE()

#define DISPLAY_SSD1306_SPIx_CS_GPIO_PIN                GPIO_PIN_9
#define DISPLAY_SSD1306_SPIx_CS_GPIO_PORT                GPIOD


/* Definition for DISPLAY_SSD1306_SPIx's NVIC */
#define DISPLAY_SSD1306_SPIx_IRQn                        SPI2_IRQn
#define DISPLAY_SSD1306_SPIx_IRQHandler                    SPI2_IRQHandler

#define DISPLAY_SPI_IRQ_PREEMT_PRIORITY                    11
#define DISPLAY_SPI_IRQ_SUB_PRIORITY                    0

#define DISPLAY_SSD1306_RST_GPIO_PIN                    GPIO_PIN_10
#define DISPLAY_SSD1306_RST_GPIO_PORT                    GPIOD

#define DISPLAY_SSD1306_DATACOMMAND_GPIO_PIN            GPIO_PIN_11
#define DISPLAY_SSD1306_DATACOMMAND_GPIO_PORT            GPIOD

#endif    /* CONFIG_MODULE_DISPLAY_ENABLE */



#endif    /* CONFIG_USE_PANEL_STM32F4DISCOVERY */



#ifdef CONFIG_USE_PANEL_HOMEAUTNODESMALL


#ifdef CONFIG_MODULE_ESP8266_ENABLE


/* USART */
/* DEBUG_USART        USART1 */
/* DEBUG_USART_TX    -    PA9 */
/* DEBUG_USART_RX    -    PA10 */


#define ESP8266_USARTx                    USART1



#define ESP8266_USART_CLK_ENABLES()        \
        __USART1_CLK_ENABLE();            \
        __GPIOA_CLK_ENABLE()



#define ESP8266_USART_TX_GPIO_PORT        GPIOA
#define ESP8266_USART_TX_GPIO_PIN        GPIO_PIN_9

#define ESP8266_USART_RX_GPIO_PORT        GPIOA
#define ESP8266_USART_RX_GPIO_PIN        GPIO_PIN_10

#define ESP8266_USART_AF                GPIO_AF1_USART1

/* Definition for USARTx's NVIC */
#define ESP8266_USARTx_IRQn                USART1_IRQn
#define ESP8266_USARTx_IRQHandler        USART1_IRQHandler

/* WITHOUT FREERTOS: If use FreeRTOS, you need higher priority */
#define ESP8266_USART_PREEMT_PRIORITY    2
#define ESP8266_USART_SUB_PRIORITY        0

#define ESP8266_USART_BAUDRATE            9600


/*
ESP8266
_GPIO0             PA12
_RST            PA11
_CH                PA8
_GPIO2            PB15
*/


#define ESP8266_GPIO0_GPIO_PIN            GPIO_PIN_12
#define ESP8266_GPIO0_GPIO_PORT            GPIOA
#define ESP8266_RST_GPIO_PIN            GPIO_PIN_11
#define ESP8266_RST_GPIO_PORT            GPIOA

#define ESP8266_CH_GPIO_PIN                GPIO_PIN_8
#define ESP8266_CH_GPIO_PORT            GPIOA
#define ESP8266_GPIO2_GPIO_PIN            GPIO_PIN_15
#define ESP8266_GPIO2_GPIO_PORT            GPIOB


#define ESP8266_PINS_CLK_ENABLES()        \
        __GPIOB_CLK_ENABLE();            \
        __GPIOA_CLK_ENABLE()

#endif    /* #ifdef CONFIG_MODULE_ESP8266_ENABLE */



#ifdef CONFIG_MODULE_BUTTON_ENABLE

/* UP        S4    BUTTON_0        GPIOB14 */
/* LEFT        S5    BUTTON_3        GPIOB13 */
/* DOWN        S3    BUTTON_2        GPIOB12 */
/* RIGHT    S1    BUTTON_1        GPIOA0 */

#define BUTTON_NUM                    4

#define BUTTON_RIGHT_GPIO_PORT        GPIOA
#define BUTTON_RIGHT_GPIO_PIN        GPIO_PIN_0

#define BUTTON_UP_GPIO_PORT            GPIOB
#define BUTTON_UP_GPIO_PIN            GPIO_PIN_14

#define BUTTON_DOWN_GPIO_PORT        GPIOB
#define BUTTON_DOWN_GPIO_PIN        GPIO_PIN_12

#define BUTTON_LEFT_GPIO_PORT        GPIOB
#define BUTTON_LEFT_GPIO_PIN        GPIO_PIN_13


#define BUTTON_CLK_ENABLES()        \
        __GPIOA_CLK_ENABLE();        \
        __GPIOB_CLK_ENABLE()

#endif



#ifdef CONFIG_MODULE_IO_ENABLE

/* LED_RED            PC13 */
/* LED_BLUE            PB8 */
/* LED_GREEN        PB9 */


#define LED_NUM_MIN                (1)
#define LED_NUM_MAX                (3)

#define BOARD_LED_GREEN_PORT    GPIOB
#define BOARD_LED_GREEN_PIN        GPIO_PIN_9
#define BOARD_LED_BLUE_PORT        GPIOB
#define BOARD_LED_BLUE_PIN        GPIO_PIN_8
#define BOARD_LED_RED_PORT        GPIOC
#define BOARD_LED_RED_PIN        GPIO_PIN_13

#define IO_PORT_CLK_ENABLES()    __GPIOB_CLK_ENABLE(); \
                                __GPIOC_CLK_ENABLE()


#define LED_GREEN_ON()            HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN, GPIO_PIN_SET)
#define LED_BLUE_ON()            HAL_GPIO_WritePin(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PIN, GPIO_PIN_SET)
#define LED_RED_ON()            HAL_GPIO_WritePin(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN, GPIO_PIN_SET)

#define LED_GREEN_OFF()            HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN, GPIO_PIN_RESET)
#define LED_BLUE_OFF()            HAL_GPIO_WritePin(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PIN, GPIO_PIN_RESET)
#define LED_RED_OFF()            HAL_GPIO_WritePin(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN, GPIO_PIN_RESET)

#define LED_GREEN_TOGGLE()        HAL_GPIO_TogglePin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN)
#define LED_BLUE_TOGGLE()        HAL_GPIO_TogglePin(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PIN)
#define LED_RED_TOGGLE()        HAL_GPIO_TogglePin(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN)

#define LED_GREEN_STATUS()        HAL_GPIO_ReadPin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN)
#define LED_BLUE_STATUS()        HAL_GPIO_ReadPin(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PIN)
#define LED_RED_STATUS()        HAL_GPIO_ReadPin(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN)

#endif    /* CONFIG_MODULE_IO_ENABLE */

#endif    /* CONFIG_USE_PANEL_HOMEAUTNODESMALL */



#ifdef CONFIG_USE_PANEL_FASTENNODE


#ifdef CONFIG_MODULE_IO_ENABLE

/* LED_GREEN        PC14 */
/* LED_BLUE            PC15 */
/* LED_RED            PC13 */


#define LED_NUM_MAX                (3)

#define BOARD_LED_GREEN_PORT    GPIOC
#define BOARD_LED_GREEN_PIN        GPIO_PIN_14
#define BOARD_LED_BLUE_PORT        GPIOC
#define BOARD_LED_BLUE_PIN        GPIO_PIN_15
#define BOARD_LED_RED_PORT        GPIOC
#define BOARD_LED_RED_PIN        GPIO_PIN_13

#define IO_PORT_CLK_ENABLES()    __GPIOC_CLK_ENABLE()


#define LED_GREEN_ON()            HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN, GPIO_PIN_RESET)
#define LED_BLUE_ON()            HAL_GPIO_WritePin(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PIN, GPIO_PIN_RESET)
#define LED_RED_ON()            HAL_GPIO_WritePin(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN, GPIO_PIN_RESET)

#define LED_GREEN_OFF()            HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN, GPIO_PIN_SET)
#define LED_BLUE_OFF()            HAL_GPIO_WritePin(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PIN, GPIO_PIN_SET)
#define LED_RED_OFF()            HAL_GPIO_WritePin(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN, GPIO_PIN_SET)

#define LED_GREEN_TOGGLE()        HAL_GPIO_TogglePin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN)
#define LED_BLUE_TOGGLE()        HAL_GPIO_TogglePin(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PIN)
#define LED_RED_TOGGLE()        HAL_GPIO_TogglePin(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN)

#define LED_GREEN_STATUS()        HAL_GPIO_ReadPin(BOARD_LED_GREEN_PORT, BOARD_LED_GREEN_PIN)
#define LED_BLUE_STATUS()        HAL_GPIO_ReadPin(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_PIN)
#define LED_RED_STATUS()        HAL_GPIO_ReadPin(BOARD_LED_RED_PORT, BOARD_LED_RED_PIN)

#endif    /* CONFIG_MODULE_IO_ENABLE */



#ifdef CONFIG_MODULE_DISPLAY_ENABLE

/* Display - SSD1306 - 0.96'OLED */

#define DISPLAY_SSD1306_SPIx                            SPI1
#define DISPLAY_SSD1306_SPIx_CLK_ENABLE()                __HAL_RCC_SPI1_CLK_ENABLE()
#define DISPLAY_SSD1306_SPIx_GPIO_CLK_ENABLES()            __HAL_RCC_GPIOB_CLK_ENABLE()

#define DISPLAY_SSD1306_SPIx_FORCE_RESET()                __HAL_RCC_SPI1_FORCE_RESET()
#define DISPLAY_SSD1306_SPIx_RELEASE_RESET()            __HAL_RCC_SPI1_RELEASE_RESET()

/* Definition for DISPLAY_SSD1306_SPIx Pins */
#define DISPLAY_SSD1306_SPIx_SCK_PIN                    GPIO_PIN_3
#define DISPLAY_SSD1306_SPIx_SCK_GPIO_PORT                GPIOB
#define DISPLAY_SSD1306_SPIx_SCK_AF                        GPIO_AF0_SPI1

#define DISPLAY_SSD1306_SPIx_MOSI_PIN                    GPIO_PIN_5
#define DISPLAY_SSD1306_SPIx_MOSI_GPIO_PORT                GPIOB
#define DISPLAY_SSD1306_SPIx_MOSI_AF                    GPIO_AF0_SPI1


#define DISPLAY_SSD1306_PINS_CLK_ENABLE()                __HAL_RCC_GPIOB_CLK_ENABLE()

#define DISPLAY_SSD1306_SPIx_CS_GPIO_PIN                GPIO_PIN_14
#define DISPLAY_SSD1306_SPIx_CS_GPIO_PORT                GPIOB


/* Definition for DISPLAY_SSD1306_SPIx's NVIC */
#define DISPLAY_SSD1306_SPIx_IRQn                        SPI1_IRQn
#define DISPLAY_SSD1306_SPIx_IRQHandler                    SPI1_IRQHandler

#define DISPLAY_SPI_IRQ_PREEMT_PRIORITY                    3
#define DISPLAY_SPI_IRQ_SUB_PRIORITY                    0

#define DISPLAY_SSD1306_RST_GPIO_PIN                    GPIO_PIN_12
#define DISPLAY_SSD1306_RST_GPIO_PORT                    GPIOB

#define DISPLAY_SSD1306_DATACOMMAND_GPIO_PIN            GPIO_PIN_13
#define DISPLAY_SSD1306_DATACOMMAND_GPIO_PORT            GPIOB

#ifdef CONFIG_DISPLAY_SPI_USE_DMA

#define DMAx_CLK_ENABLE()                                __HAL_RCC_DMA1_CLK_ENABLE()

/* Definition for SPIx's DMA */
#define SPIx_TX_DMA_STREAM                                DMA1_Channel3

/* Definition for SPIx's NVIC */
#define SPIx_DMA_TX_IRQn                                DMA1_Channel2_3_IRQn

#define SPIx_DMA_TX_IRQHandler                            DMA1_Channel2_3_IRQHandler
#endif

#endif    /* CONFIG_MODULE_DISPLAY_ENABLE */



#ifdef CONFIG_MODULE_MOTOR_ENABLE

/* DC: PWM3, DIR, DIR2 */
/* SERVO: PWM2 */

/* DIR1 + DIR2, PA1, PB2 */
/* PWM3: TIM16_CH1 - PA6 */
/* PWM2: TIM17_CH1 - PA7 */

#define MOTOR_MOTORS_PWM_GPIO_CLK_ENABLES()        __GPIOA_CLK_ENABLE()

#define MOTOR_DCMOTOR_PWM_GPIO_PORT                GPIOA
#define MOTOR_DCMOTOR_PWM_GPIO_PIN                GPIO_PIN_6

#define MOTOR_SERVOMOTOR_PWM_GPIO_PORT            GPIOA
#define MOTOR_SERVOMOTOR_PWM_GPIO_PIN            GPIO_PIN_7


#define MOTOR_MOTORS_PWM_TIMER_CLK_ENABLES()    __TIM3_CLK_ENABLE();    \
                                                __TIM3_CLK_ENABLE()

#define MOTOR_DCMOTOR_PWM_TIMx                    TIM3
#define MOTOR_SERVOMOTOR_PWM_TIMx                TIM3

#define MOTOR_DCMOTOR_PWM_TIMER_CHANNEL            TIM_CHANNEL_1
#define MOTOR_SERVOMOTOR_PWM_TIMER_CHANNEL        TIM_CHANNEL_2

#define MOTOR_DCMOTOR_PWM_TIMx_GPIO_AF          GPIO_AF1_TIM3
#define MOTOR_SERVOMOTOR_PWM_TIMx_GPIO_AF        GPIO_AF1_TIM3


#define MOTOR_DCMOTOR_DIRS_CLK_ENABLES()        __GPIOA_CLK_ENABLE();    \
                                                __GPIOB_CLK_ENABLE()

#define MOTOR_DCMOTOR_DIR1_GPIO_PORT            GPIOA
#define MOTOR_DCMOTOR_DIR1_GPIO_PIN                GPIO_PIN_1

#define MOTOR_DCMOTOR_DIR2_GPIO_PORT            GPIOB
#define MOTOR_DCMOTOR_DIR2_GPIO_PIN                GPIO_PIN_2


#define MOTOR_MOTORS_PWM_ON_ONE_TIMER


#endif    /* CONFIG_MODULE_MOTOR_ENABLE */



#ifdef CONFIG_MODULE_MEASUREMENTTIMER_ENABLE

#define MEASUREMENTTIMER_TIMER_CLK_ENABLES()    __TIM3_CLK_ENABLE()

#define MEASUREMENTTIMER_TIMx                    TIM3

#define MEASUREMENTTIMER_TIMx_IRQn                TIM3_IRQn

#define MEASUREMENTTIMER_IT_PRIORITY            (5)

#define MEASUREMENTTIMER_IRQ_HANDLER            TIM3_IRQHandler

#define MEASUREMENT_RESOLUTION_16BIT

#endif    /* CONFIG_MODULE_MEASUREMENTTIMER_ENABLE */



#ifdef CONFIG_MODULE_DEBUGUART_ENABLE

/* USART */
/* DEBUG_USART            USART2 */
/* DEBUG_USART_TX        PA2 */
/* DEBUG_USART_RX        PA3 */

#define DEBUG_USARTx                    USART2

#define DEBUG_USART_CLK_ENABLES()        \
        __USART2_CLK_ENABLE();            \
        __GPIOA_CLK_ENABLE()


#define DEBUG_USART_TX_GPIO_PORT        GPIOA
#define DEBUG_USART_TX_GPIO_PIN            GPIO_PIN_2

#define DEBUG_USART_RX_GPIO_PORT        GPIOA
#define DEBUG_USART_RX_GPIO_PIN            GPIO_PIN_3

#define DEBUG_USART_AF                    GPIO_AF1_USART2

#define DEBUG_USART_BAUDRATE            115200


/* Definition for USARTx's NVIC */
#define DEBUG_USARTx_IRQn               USART2_IRQn
#define DEBUG_USARTx_IRQHandler         USART2_IRQHandler

#define DEBUG_USART_PREEMT_PRIORITY        3
#define DEBUG_USART_SUB_PRIORITY        0


#endif    /* CONFIG_MODULE_DEBUGUART_ENABLE */


#ifdef CONFIG_MODULE_ESP8266_ENABLE

/* USART1 */
/* USART_TX        PA9 */
/* USART_RX        PA10 */

#define ESP8266_USARTx                        USART1

#define ESP8266_USART_CLK_ENABLES()            \
        __USART1_CLK_ENABLE();                \
        __GPIOA_CLK_ENABLE()


#define ESP8266_USART_TX_GPIO_PORT            GPIOA
#define ESP8266_USART_TX_GPIO_PIN            GPIO_PIN_9

#define ESP8266_USART_RX_GPIO_PORT            GPIOA
#define ESP8266_USART_RX_GPIO_PIN            GPIO_PIN_10

#define ESP8266_USART_AF                    GPIO_AF1_USART1

/* Definition for USARTx's NVIC */
#define ESP8266_USARTx_IRQn                  USART1_IRQn
#define ESP8266_USARTx_IRQHandler            USART1_IRQHandler

#define ESP8266_USART_PREEMT_PRIORITY        2
#define ESP8266_USART_SUB_PRIORITY            0

#if (ESP8266_VERSION == 0)
    #define ESP8266_USART_BAUDRATE                9600
#else
    #define ESP8266_USART_BAUDRATE                115200
#endif

/*
ESP8266    - Other pins
_GPIO0             PB7
_GPIO2            PB6
_RST            PA11
_CH                PA12
*/

#define ESP8266_PINS_CLK_ENABLES()        \
        __GPIOB_CLK_ENABLE();            \
        __GPIOA_CLK_ENABLE()

#define ESP8266_GPIO0_GPIO_PIN            GPIO_PIN_7
#define ESP8266_GPIO0_GPIO_PORT            GPIOB

#define ESP8266_GPIO2_GPIO_PIN            GPIO_PIN_6
#define ESP8266_GPIO2_GPIO_PORT            GPIOB

#define ESP8266_RST_GPIO_PIN            GPIO_PIN_11
#define ESP8266_RST_GPIO_PORT            GPIOA

#define ESP8266_CH_GPIO_PIN                GPIO_PIN_12
#define ESP8266_CH_GPIO_PORT            GPIOA


#endif /* CONFIG_MODULE_ESP8266_ENABLE */


#ifdef CONFIG_MODULE_BLUETOOTH_ENABLE

/* USART1 */
/* USART_TX        PA9 */
/* USART_RX        PA10 */

#define BLUETOOTH_USARTx                    USART1

#define BLUETOOTH_USART_CLK_ENABLES()        \
        __USART1_CLK_ENABLE();                \
        __GPIOA_CLK_ENABLE()


#define BLUETOOTH_USART_TX_GPIO_PORT        GPIOA
#define BLUETOOTH_USART_TX_GPIO_PIN            GPIO_PIN_9

#define BLUETOOTH_USART_RX_GPIO_PORT        GPIOA
#define BLUETOOTH_USART_RX_GPIO_PIN            GPIO_PIN_10

#define BLUETOOTH_USART_AF                    GPIO_AF1_USART1

/* Definition for USARTx's NVIC */
#define BLUETOOTH_USARTx_IRQn                  USART1_IRQn
#define BLUETOOTH_USARTx_IRQHandler            USART1_IRQHandler

#define BLUETOOTH_USART_PREEMT_PRIORITY        11
#define BLUETOOTH_USART_SUB_PRIORITY        0

#ifdef CONFIG_FUNCTION_REMOTECONTROLLER_CAR
#define BLUETOOTH_USART_BAUDRATE            38400
#else
#define BLUETOOTH_USART_BAUDRATE            9600
#endif

#endif    /* #ifdef CONFIG_MODULE_BLUETOOTH_ENABLE */



#ifdef CONFIG_MODULE_COMMON_UART_ENABLE

/* USART1 */
/* USART_TX        PA9 */
/* USART_RX        PA10 */

#define COMMONUART_USARTx                    USART1

#define COMMONUART_USART_CLK_ENABLES()        \
        __USART1_CLK_ENABLE();                \
        __GPIOA_CLK_ENABLE()


#define COMMONUART_USART_TX_GPIO_PORT        GPIOA
#define COMMONUART_USART_TX_GPIO_PIN        GPIO_PIN_9

#define COMMONUART_USART_RX_GPIO_PORT        GPIOA
#define COMMONUART_USART_RX_GPIO_PIN        GPIO_PIN_10

#define COMMONUART_USART_AF                    GPIO_AF1_USART1

/* Definition for USARTx's NVIC */
#define COMMONUART_USARTx_IRQn              USART1_IRQn
#define COMMONUART_USARTx_IRQHandler           USART1_IRQHandler

#define COMMONUART_USART_PREEMT_PRIORITY    11
#define COMMONUART_USART_SUB_PRIORITY        0

#define COMMONUART_USART_BAUDRATE            9600


#endif    /* #ifdef CONFIG_MODULE_COMMON_UART_ENABLE */



#ifdef CONFIG_MODULE_BUTTON_ENABLE

/*
BUTTON_RIGHT        GPIOB8
BUTTON_UP            GPIOA0
BUTTON_DOWN            GPIOB9
BUTTON_LEFT            GPIOA15
*/

#define BUTTON_NUM                    4

#define BUTTON_UP_GPIO_PORT            GPIOA
#define BUTTON_UP_GPIO_PIN            GPIO_PIN_0

#define BUTTON_RIGHT_GPIO_PORT        GPIOB
#define BUTTON_RIGHT_GPIO_PIN        GPIO_PIN_8

#define BUTTON_DOWN_GPIO_PORT        GPIOB
#define BUTTON_DOWN_GPIO_PIN        GPIO_PIN_9

#define BUTTON_LEFT_GPIO_PORT        GPIOA
#define BUTTON_LEFT_GPIO_PIN        GPIO_PIN_15


#define BUTTON_CLK_ENABLES()        \
        __GPIOA_CLK_ENABLE();        \
        __GPIOB_CLK_ENABLE()        \

/* NOTE: Button GPIO IRQ in Button.c!! */

#endif    /* CONFIG_MODULE_BUTTON_ENABLE */



#define IO_INPUTS_NUM    0

#ifdef CONFIG_MODULE_IO_BATTERY_CHARGER_ENABLE

#define IO_INPUT_BATTERYCHARGER_CLK_ENABLES()        \
        __GPIOA_CLK_ENABLE()

#define IO_INPUT_BATTERYCHARGER_GPIO_PORT        GPIOA
#define IO_INPUT_BATTERYCHARGER_GPIO_PIN        GPIO_PIN_12

#endif



#if defined(CONFIG_MODULE_ADC_ENABLE) || defined(CONFIG_MODULE_COMMON_ADC_ENABLE)


#define ADC_CHANNEL_NUM                    (1)


/* VSOURCE_ADC */
/* PB1 */
/* ADC_IN9 */
#define    ADC_VSOURCE_GPIO_PORT            GPIOB
#define    ADC_VSOURCE_GPIO_PIN            GPIO_PIN_1
#define    ADC_VSOURCE_CHANNEL                ADC_CHANNEL_9


#define ADC_TEMPSENSOR_CHANNEL            ADC_CHANNEL_16
#define ADC_VREFINT_CHANNEL                ADC_CHANNEL_17


#define ADCx                            ADC1
#define ADCx_CLK_ENABLE()               __HAL_RCC_ADC1_CLK_ENABLE()
#define DMAx_CLK_ENABLE()               __HAL_RCC_DMA1_CLK_ENABLE()
#define ADCx_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()


/* Definition for ADCx's DMA */
#define ADCx_DMA_CHANNEL                DMA1_Channel1

/* Definition for ADCx's NVIC */
#define ADCx_DMA_IRQn                   DMA1_Channel1_IRQn
#define ADCx_DMA_IRQHandler             DMA1_Channel1_IRQHandler

/* ADC DMA Interrupt priorities */
#define ADC_DMA_PREEMT_PRIORITY            15
#define ADC_DMA_SUB_PRIORITY            0

#define ADC_VOLTAGE_MAX                    (3.3f)
#define ADC_RESOLUTION_MAX                (4095U)

#define ADC_SOURCE_VOLTAGE_MUL            10


#define VSOURCE_BATTERY_MAX_VOLTAGE        (3.9f)

#endif    /* #if defined(CONFIG_MODULE_ADC_ENABLE) || defined(CONFIG_MODULE_COMMON_ADC_ENABLE) */


#endif    /* CONFIG_USE_PANEL_FASTENNODE */



#endif /* BOARD_H_ */
