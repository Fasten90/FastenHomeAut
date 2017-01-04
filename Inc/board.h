/*
 * board.h
 *
 *  	Created on: 	2013.11.19.
 *      Author: 		Vizi Gábor
 *		E-mail:			vizi.gabor90@gmail.com
 *		University:		BME (BUTE)
 *		Project: 		RadioAlarm - Radio Alarm System Center module v2
 *		Function:		board/panel defines: pins, ports, etc
 *		Target:			STM32F107RCT6
 *		Version:		v1
 *		Last modified:	2014.03.21
 */

#ifndef BOARD_H_
#define BOARD_H_


#include "options.h" // what panel using


/// BOARD NAME


#ifdef CONFIG_USE_PANEL_NUCLEOF401RE
#define BOARD_NAME		"NucleoF401RE"
#endif
#ifdef CONFIG_USE_PANEL_STM32F4DISCOVERY
#define BOARD_NAME		"STM32F4Discovery"
#endif
#ifdef CONFIG_USE_PANEL_HOMEAUTCENTERPANEL
#define BOARD_NAME		"HomeAutCenterPanel"
#endif




///////////////////////////////////////////////////////////////////////////////
// 			LED
///////////////////////////////////////////////////////////////////////////////


#if defined(CONFIG_MODULE_LED_ENABLE) & defined(CONFIG_USE_PANEL_HOMEAUTCENTERPANEL)

// \note: In the schematic the blue and red LED are inversely
// blue: PE5
// green: PE6
// red: PA8

#define LED_NUM_MIN				(1)
#define LED_NUM_MAX				(3)

#define BOARD_LED_BLUE_PORT			GPIOE
#define BOARD_LED_BLUE_PIN			GPIO_PIN_5

#define BOARD_LED_GREEN_PORT		GPIOE
#define BOARD_LED_GREEN_PIN			GPIO_PIN_6

#define BOARD_LED_RED_PORT			GPIOA
#define BOARD_LED_RED_PIN			GPIO_PIN_8

#define LED_PORT_CLK_ENABLES()		\
		__GPIOA_CLK_ENABLE();		\
		__GPIOE_CLK_ENABLE()


#define LED_RED_ON()			HAL_GPIO_WritePin(BOARD_LED_RED_PORT,BOARD_LED_RED_PIN,GPIO_PIN_SET)
#define LED_BLUE_ON()			HAL_GPIO_WritePin(BOARD_LED_BLUE_PORT,BOARD_LED_BLUE_PIN,GPIO_PIN_SET)
#define LED_GREEN_ON()			HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN,GPIO_PIN_SET)

#define LED_RED_OFF()			HAL_GPIO_WritePin(BOARD_LED_RED_PORT,BOARD_LED_RED_PIN,GPIO_PIN_RESET)
#define LED_BLUE_OFF()			HAL_GPIO_WritePin(BOARD_LED_BLUE_PORT,BOARD_LED_BLUE_PIN,GPIO_PIN_RESET)
#define LED_GREEN_OFF()			HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN,GPIO_PIN_RESET)

#define LED_RED_TOGGLE()		HAL_GPIO_TogglePin(BOARD_LED_RED_PORT,BOARD_LED_RED_PIN)
#define LED_BLUE_TOGGLE()		HAL_GPIO_TogglePin(BOARD_LED_BLUE_PORT,BOARD_LED_BLUE_PIN)
#define LED_GREEN_TOGGLE()		HAL_GPIO_TogglePin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN)

#define LED_RED_STATUS()		HAL_GPIO_ReadPin(BOARD_LED_RED_PORT,BOARD_LED_RED_PIN)
#define LED_BLUE_STATUS()		HAL_GPIO_ReadPin(BOARD_LED_BLUE_PORT,BOARD_LED_BLUE_PIN)
#define LED_GREEN_STATUS()		HAL_GPIO_ReadPin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN)

#endif //#ifdef CONFIG_USE_PANEL_CENTERPANEL



#if defined(CONFIG_MODULE_LED_ENABLE) & defined(CONFIG_USE_PANEL_STM32F4DISCOVERY)

// blue: PE5
// green: PE6
// red: PA8

#define LED_NUM_MIN				(1)
#define LED_NUM_MAX				(3)

#define BOARD_LED_BLUE_PORT		GPIOD
#define BOARD_LED_BLUE_PIN		GPIO_PIN_15
#define BOARD_LED_GREEN_PORT	GPIOD
#define BOARD_LED_GREEN_PIN		GPIO_PIN_12
#define BOARD_LED_RED_PORT		GPIOD
#define BOARD_LED_RED_PIN		GPIO_PIN_14

#define LED_PORT_CLK_ENABLES()	__GPIOD_CLK_ENABLE()


#define LED_RED_ON()			HAL_GPIO_WritePin(BOARD_LED_RED_PORT,BOARD_LED_RED_PIN,GPIO_PIN_SET)
#define LED_BLUE_ON()			HAL_GPIO_WritePin(BOARD_LED_BLUE_PORT,BOARD_LED_BLUE_PIN,GPIO_PIN_SET)
#define LED_GREEN_ON()			HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN,GPIO_PIN_SET)

#define LED_RED_OFF()			HAL_GPIO_WritePin(BOARD_LED_RED_PORT,BOARD_LED_RED_PIN,GPIO_PIN_RESET)
#define LED_BLUE_OFF()			HAL_GPIO_WritePin(BOARD_LED_BLUE_PORT,BOARD_LED_BLUE_PIN,GPIO_PIN_RESET)
#define LED_GREEN_OFF()			HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN,GPIO_PIN_RESET)

#define LED_RED_TOGGLE()		HAL_GPIO_TogglePin(BOARD_LED_RED_PORT,BOARD_LED_RED_PIN)
#define LED_BLUE_TOGGLE()		HAL_GPIO_TogglePin(BOARD_LED_BLUE_PORT,BOARD_LED_BLUE_PIN)
#define LED_GREEN_TOGGLE()		HAL_GPIO_TogglePin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN)

#define LED_RED_STATUS()		HAL_GPIO_ReadPin(BOARD_LED_RED_PORT,BOARD_LED_RED_PIN)
#define LED_BLUE_STATUS()		HAL_GPIO_ReadPin(BOARD_LED_BLUE_PORT,BOARD_LED_BLUE_PIN)
#define LED_GREEN_STATUS()		HAL_GPIO_ReadPin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN)

#endif //#ifdef CONFIG_USE_PANEL_CENTERPANEL



#if defined(CONFIG_MODULE_LED_ENABLE) && defined(CONFIG_USE_PANEL_NUCLEOF401RE)

// Green user LED: PA5

#define LED_NUM_MIN				(1)
#define LED_NUM_MAX				(1)

#define BOARD_LED_GREEN_PORT	GPIOA
#define BOARD_LED_GREEN_PIN		GPIO_PIN_5

#define LED_PORT_CLK_ENABLES()	__GPIOA_CLK_ENABLE()

#define LED_RED_ON()
#define LED_BLUE_ON()
#define LED_GREEN_ON()			HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN,GPIO_PIN_SET)

#define LED_RED_OFF()
#define LED_BLUE_OFF()
#define LED_GREEN_OFF()			HAL_GPIO_WritePin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN,GPIO_PIN_RESET)


#define LED_RED_TOGGLE()
#define LED_BLUE_TOGGLE()
#define LED_GREEN_TOGGLE()		HAL_GPIO_TogglePin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN)

#define LED_RED_STATUS()
#define LED_BLUE_STATUS()
#define LED_GREEN_STATUS()		HAL_GPIO_ReadPin(BOARD_LED_GREEN_PORT,BOARD_LED_GREEN_PIN)

#endif


#if !defined(CONFIG_MODULE_LED_ENABLE)

// if not defined CONFIG_MODULE_LED_ENABLE
#define LED_RED_ON()
#define LED_BLUE_ON()
#define LED_GREEN_ON()
#define LED_RED_OFF()
#define LED_BLUE_OFF()
#define LED_GREEN_OFF()
#define LED_RED_TOGGLE()
#define LED_BLUE_TOGGLE()
#define LED_GREEN_TOGGLE()
#define LED_RED_STATUS()
#define LED_BLUE_STATUS()
#define LED_GREEN_STATUS()
#endif



///////////////////////////////////////////////////////////////////////////////
// 			TIMERS for LEDs
///////////////////////////////////////////////////////////////////////////////


// LED_BLUE			PB8	TIM16_CH1
// LED_GREEN		PB9	TIM17_CH1

// TODO: LED timers / PWM, need beautify


/* Definition for TIMx clock resources */
#ifdef CONFIG_USE_PANEL_NODESMALL
#define TIMx_BLUE                           TIM16
#define TIMx_BLUE_CLK_ENABLE()              __TIM16_CLK_ENABLE()
#define TIMx_GREEN                        	TIM17
#define TIMx_GREEN_CLK_ENABLE()              __TIM17_CLK_ENABLE()

#define TIMER_CLK_ENABLES()					__TIM16_CLK_ENABLE();		\
											__TIM17_CLK_ENABLE()
											
											
#define LED_BLUE_TIMER_CHANNEL				TIM_CHANNEL_1
#define LED_GREEN_TIMER_CHANNEL				TIM_CHANNEL_1

#define TIMx_LED_BLUE_GPIO_AF          		GPIO_AF2_TIM16
#define TIMx_LED_GREEN_GPIO_AF         		GPIO_AF2_TIM17

#endif



#ifdef CONFIG_USE_PANEL_NODEMEDIUM

#define TIMx_BLUE                           TIM4
#define TIMx_BLUE_CLK_ENABLE()              __TIM4_CLK_ENABLE()
#define TIMx_GREEN                        	TIM4
#define TIMx_GREEN_CLK_ENABLE()              __TIM4_CLK_ENABLE()
#define TIMx_RED                       		TIM3
#define TIMx_RED_CLK_ENABLE()              __TIM3_CLK_ENABLE()

#define TIMER_CLK_ENABLES()					__TIM3_CLK_ENABLE();		\
											__TIM4_CLK_ENABLE()


#define LED_BLUE_TIMER_CHANNEL				TIM_CHANNEL_2
#define LED_GREEN_TIMER_CHANNEL				TIM_CHANNEL_1
#define LED_RED_TIMER_CHANNEL				TIM_CHANNEL_2


#define TIMx_LED_BLUE_GPIO_AF          		GPIO_AF2_TIM4
#define TIMx_LED_GREEN_GPIO_AF        		GPIO_AF2_TIM4
#define TIMx_LED_RED_GPIO_AF        		GPIO_AF2_TIM3

#endif



///////////////////////////////////////////////////////////////////////////////
// 		ADC  - ANALOG
///////////////////////////////////////////////////////////////////////////////



#ifdef CONFIG_USE_PANEL_NODESMALL


#define BOARD_USE_ADC			ADC1


#define ADC_COMMON_PORT				GPIOA
#define ADC_COMMON_CLK_ENABLE()		__GPIOA_CLK_ENABLE()


#define ADC_BUFFER_SIZE				( 5 )



// SENSOR_LIGH_ADC
// PA1
// ADC1_IN1
#define	ADC_SENSOR_LIGHT_PORT			GPIOA
#define	ADC_SENSOR_LIGHT_CLK			RCC_AHB1Periph_GPIOA
#define	ADC_SENSOR_LIGHT_PIN			GPIO_PIN_1
#define	ADC_SENSOR_LIGHT_CHANNEL		ADC_CHANNEL_1



// SENSOR_MICROPHONE_ADC
// PA6
// ADC1_IN6
#define	ADC_SENSOR_MICROPHONE_PORT			GPIOA
#define	ADC_SENSOR_MICROPHONE_CLK			RCC_AHB1Periph_GPIOA
#define	ADC_SENSOR_MICROPHONE_PIN			GPIO_PIN_6
#define	ADC_SENSOR_MICROPHONE_CHANNEL		ADC_CHANNEL_6


// VSOURCE_ADC
// PA7
// ADC1_IN7
#define	ADC_VSOURCE_PORT			GPIOA
#define	ADC_VSOURCE_CLK				RCC_AHB1Periph_GPIOA
#define	ADC_VSOURCE_PIN				GPIO_PIN_7
#define	ADC_VSOURCE_CHANNEL			ADC_CHANNEL_7


#define ADC_TEMPSENSOR_CHANNEL		ADC_CHANNEL_16
#define ADC_VREFINT_CHANNEL			ADC_CHANNEL_17


#endif	// CONFIG_USE_PANEL_NODEMSMALL



#ifdef CONFIG_USE_PANEL_NODEMEDIUM


// ANALOG
#define BOARD_USE_ADC				ADC1


#define ADC_COMMON_PORT				GPIOB | GPIOC
#define ADC_COMMON_CLK_ENABLE()		\
		__GPIOB_CLK_ENABLE();		\
		__GPIOC_CLK_ENABLE()


#define ADC_BUFFER_SIZE				( 5 )	
// now: Vsource, ligh, mic, prox, internal temp



// SENSOR_LIGH_ADC
// PC0
// ADC_IN10
#define	ADC_SENSOR_LIGHT_PORT			GPIOC
#define	ADC_SENSOR_LIGHT_PIN			GPIO_PIN_0
#define	ADC_SENSOR_LIGHT_CHANNEL		ADC_CHANNEL_10



// SENSOR_MICROPHONE_ADC
// PC1
// ADC_IN11
#define	ADC_SENSOR_MICROPHONE_PORT			GPIOC
#define	ADC_SENSOR_MICROPHONE_PIN			GPIO_PIN_1
#define	ADC_SENSOR_MICROPHONE_CHANNEL		ADC_CHANNEL_11



// SENSOR_PROXIMITY_ADC
// PC2
// ADC_IN12
#define	ADC_SENSOR_PROXIMITY_PORT			GPIOC
#define	ADC_SENSOR_PROXIMITY_PIN			GPIO_PIN_2
#define	ADC_SENSOR_PROXIMITY_CHANNEL		ADC_CHANNEL_12



// VSOURCE_ADC
// PB12
// ADC_IN18
#define	ADC_VSOURCE_PORT			GPIOB
#define	ADC_VSOURCE_PIN				GPIO_PIN_12
#define	ADC_VSOURCE_CHANNEL			ADC_CHANNEL_18



// L1 not included internal Tempsensor :(
// TODO: Sure... ?
#define ADC_TEMPSENSOR_CHANNEL		ADC_CHANNEL_16
#define ADC_VREFINT_CHANNEL			ADC_CHANNEL_17


#endif	// CONFIG_USE_PANEL_NODEMEDIUM



#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_NUCLEOF401RE)

// ANALOG

#define ADC_BUFFER_SIZE					( 3 )


#define ADCx                            ADC1
#define ADCx_CLK_ENABLE()               __HAL_RCC_ADC1_CLK_ENABLE()
#define DMAx_CLK_ENABLE()               __HAL_RCC_DMA2_CLK_ENABLE()
#define ADCx_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()

/* Definition for ADCx Channel Pin */
#define ADCx_CHANNEL_PIN_1				GPIO_PIN_1
#define ADCx_CHANNEL_PIN_2				GPIO_PIN_2
#define ADCx_CHANNEL_PIN_3				GPIO_PIN_3
#define ADCx_CHANNEL_GPIO_PORT			GPIOA

/* Definition for ADCx's Channel */
#define ADCx_CHANNEL_1					ADC_CHANNEL_1
#define ADCx_CHANNEL_2					ADC_CHANNEL_2
#define ADCx_CHANNEL_3					ADC_CHANNEL_3

/* Definition for ADCx's DMA */
#define ADCx_DMA_CHANNEL                DMA_CHANNEL_0
#define ADCx_DMA_STREAM                 DMA2_Stream0

/* Definition for ADCx's NVIC */
#define ADCx_DMA_IRQn                   DMA2_Stream0_IRQn
#define ADCx_DMA_IRQHandler             DMA2_Stream0_IRQHandler

/* ADC DMA Interrupt priorities */
#define ADC_DMA_PREEMT_PRIORITY			13
#define ADC_DMA_SUB_PRIORITY			0


#define COMMON_ADC_VOLTAGE_MAX			(3.3f)
#define COMMON_ADC_RESOLUTION_MAX		(4095U)

#endif



///////////////////////////////////////////////////////////////////////////////
//			DAC
///////////////////////////////////////////////////////////////////////////////



#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_NUCLEOF401RE)


#define DACx                            DAC

/* Definition for DAC clock resources */
#define DACx_CHANNEL_GPIO_CLK_ENABLE()	__HAL_RCC_GPIOA_CLK_ENABLE()
#define DACx_CLK_ENABLE()				__HAL_RCC_DAC_CLK_ENABLE()

/* Definition for DACx Channel1 Pin */
#define DACx_CHANNEL1_PIN				GPIO_PIN_4
#define DACx_CHANNEL1_GPIO_PORT			GPIOA

#define DACx_CHANNEL2_PIN				GPIO_PIN_5
#define DACx_CHANNEL2_GPIO_PORT			GPIOA

#define DACx_CHANNEL1					DAC_CHANNEL_1
#define DACx_CHANNEL2					DAC_CHANNEL_2


#define COMMON_DAC_MAX_VOLTAGE					3.3f
#define COMMON_DAC_DA_MAX_VALUE				4095


#endif



///////////////////////////////////////////////////////////////////////////////
//			BUTTONS
///////////////////////////////////////////////////////////////////////////////

// UP		S4	BUTTON_0		GPIOB14
// LEFT		S5	BUTTON_3		GPIOB13
// DOWN		S3	BUTTON_2		GPIOB12
// RIGHT	S1	BUTTON_1		GPIOA0


#ifdef CONFIG_USE_PANEL_NODESMALL

#define BUTTON_RIGHT_GPIO_PORT		GPIOA
#define BUTTON_RIGHT_GPIO_CLK		RCC_AHB1Periph_GPIOA
#define BUTTON_RIGHT_GPIO_PIN		GPIO_PIN_0
#define BUTTON_UP_GPIO_PORT			GPIOB
#define BUTTON_UP_GPIO_CLK			RCC_AHB1Periph_GPIOB
#define BUTTON_UP_GPIO_PIN			GPIO_PIN_14
#define BUTTON_DOWN_GPIO_PORT		GPIOB
#define BUTTON_DOWN_GPIO_CLK		RCC_AHB1Periph_GPIOB
#define BUTTON_DOWN_GPIO_PIN		GPIO_PIN_12
#define BUTTON_LEFT_GPIO_PORT		GPIOB
#define BUTTON_LEFT_GPIO_CLK		RCC_AHB1Periph_GPIOB
#define BUTTON_LEFT_GPIO_PIN		GPIO_PIN_13



#define BUTTON_CLK_ENABLES()		\
		__GPIOA_CLK_ENABLE();		\
		__GPIOB_CLK_ENABLE()

#endif




#ifdef CONFIG_USE_PANEL_NODEMEDIUM

#define BUTTON_RIGHT_GPIO_PORT		GPIOA
#define BUTTON_RIGHT_GPIO_CLK		RCC_AHB1Periph_GPIOA
#define BUTTON_RIGHT_GPIO_PIN		GPIO_PIN_0

#define BUTTON_UP_GPIO_PORT			GPIOD
#define BUTTON_UP_GPIO_CLK			RCC_AHB1Periph_GPIOD
#define BUTTON_UP_GPIO_PIN			GPIO_PIN_2

#define BUTTON_DOWN_GPIO_PORT		GPIOC
#define BUTTON_DOWN_GPIO_CLK		RCC_AHB1Periph_GPIOC
#define BUTTON_DOWN_GPIO_PIN		GPIO_PIN_13

#define BUTTON_LEFT_GPIO_PORT		GPIOB
#define BUTTON_LEFT_GPIO_CLK		RCC_AHB1Periph_GPIOB
#define BUTTON_LEFT_GPIO_PIN		GPIO_PIN_9



#define BUTTON_CLK_ENABLES()		\
		__GPIOA_CLK_ENABLE();		\
		__GPIOB_CLK_ENABLE();		\
		__GPIOC_CLK_ENABLE();		\
		__GPIOD_CLK_ENABLE()		\


#endif	// #ifdef CONFIG_USE_PANEL_NODEMEDIUM



#ifdef CONFIG_USE_PANEL_HOMEAUTCENTERPANEL

// Up		PC7
// Down		PC8
// Right	PA0
// Left		PC9

#define BUTTON_RIGHT_GPIO_PORT		GPIOA
#define BUTTON_RIGHT_GPIO_CLK		RCC_AHB1Periph_GPIOA
#define BUTTON_RIGHT_GPIO_PIN		GPIO_PIN_0

#define BUTTON_UP_GPIO_PORT			GPIOC
#define BUTTON_UP_GPIO_CLK			RCC_AHB1Periph_GPIOC
#define BUTTON_UP_GPIO_PIN			GPIO_PIN_7

#define BUTTON_DOWN_GPIO_PORT		GPIOC
#define BUTTON_DOWN_GPIO_CLK		RCC_AHB1Periph_GPIOC
#define BUTTON_DOWN_GPIO_PIN		GPIO_PIN_8

#define BUTTON_LEFT_GPIO_PORT		GPIOC
#define BUTTON_LEFT_GPIO_CLK		RCC_AHB1Periph_GPIOC
#define BUTTON_LEFT_GPIO_PIN		GPIO_PIN_9



#define BUTTON_CLK_ENABLES()		\
		__GPIOA_CLK_ENABLE();		\
		__GPIOC_CLK_ENABLE()


#endif	// #ifdef CONFIG_USE_PANEL_CENTERPANEL



#ifdef CONFIG_USE_PANEL_STM32F4DISCOVERY

// User button: PA0

#define BUTTON_USER_GPIO_PORT						GPIOA
#define BUTTON_USER_GPIO_PIN						GPIO_PIN_0

#define BUTTON_CLK_ENABLES()						__GPIOA_CLK_ENABLE()

/* Button Interrupt priority */
#define BUTTON_USER_EXTI_IRQn						EXTI0_IRQn
#define BUTTON_USER_INTERRUPT_PREEMT_PRIORITY		10
#define BUTTON_USER_INTERRUPT_SUB_PRIORITY			0

#endif



#ifdef CONFIG_USE_PANEL_NUCLEOF401RE

// User button: PC13

#define BUTTON_USER_GPIO_PORT						GPIOC
#define BUTTON_USER_GPIO_PIN						GPIO_PIN_13

#define BUTTON_CLK_ENABLES()						__GPIOC_CLK_ENABLE()

/* Button Interrupt priority */
#define BUTTON_USER_EXTI_IRQn						EXTI15_10_IRQn
#define BUTTON_USER_INTERRUPT_PREEMT_PRIORITY		10
#define BUTTON_USER_INTERRUPT_SUB_PRIORITY			0

#endif



///////////////////////////////////////////////////////////////////////////////
//			TEMPERATURE - I2C - LM75
///////////////////////////////////////////////////////////////////////////////

#ifdef CONFIG_MODULE_ADC_ENABLE
// I2C_SCL		PB10
// I2C_SDA		PB11


//MSB LSB
//1 0 0 1 A2 A1 A0
#define TEMPERATURE_I2C_TEMP_ADDRESS	((uint16_t)( 0x0000 | (1<<6)  | (1 << 3 ) ) )
// 0b1001000
// 1,0,0,1,0,0,0 BITs, and not hex !!!!!!!!!!!!!!
// Wrong example: 0x1001000

// pointer register:
#define TEMPERATURE_I2C_REGISTER_TEMP	((uint8_t)0x00)

#endif	// CONFIG_MODULE_ADC_ENABLE

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
//#define I2Cx_TIMING                      0x00E0D3FF
//#define I2Cx_TIMING                      0x00201D2A
// 100kHz, 100ns rise, 200ns fall

#define I2Cx_TIMING                      0x00301D28
// 100kHz, 250ns rise, 250ns fall

//com.l2fprod.common.propertysheet.PropertySheetTableModel$Item@9cec42	0x00301D28


//com.l2fprod.common.propertysheet.PropertySheetTableModel$Item@198731b	0x2000090E

//com.l2fprod.common.propertysheet.PropertySheetTableModel$Item@7f591a	0x00201D2A

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
//#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
//#define RXBUFFERSIZE                      TXBUFFERSIZE


#endif // #ifdef CONFIG_USE_PANEL_NODESMALL



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
//#define I2Cx_TIMING                      0x00E0D3FF
//#define I2Cx_TIMING                      0x00201D2A
// 100kHz, 100ns rise, 200ns fall

#define I2Cx_TIMING                      0x00301D28
// 100kHz, 250ns rise, 250ns fall

//com.l2fprod.common.propertysheet.PropertySheetTableModel$Item@9cec42	0x00301D28


//com.l2fprod.common.propertysheet.PropertySheetTableModel$Item@198731b	0x2000090E

//com.l2fprod.common.propertysheet.PropertySheetTableModel$Item@7f591a	0x00201D2A

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
//#define TXBUFFERSIZE                      (COUNTOF(aTxBuffer) - 1)
/* Size of Reception buffer */
//#define RXBUFFERSIZE                      TXBUFFERSIZE

#endif // #ifdef CONFIG_USE_PANEL_NODEMEDIUM



///////////////////////////////////////////////////////////////////////////////
//			FLASH - SPI
///////////////////////////////////////////////////////////////////////////////



#ifdef CONFIG_USE_PANEL_NODESMALL


#define FLASH_SPIx                             SPI1

#define SPIx_CLK_ENABLE()                __SPI1_CLK_ENABLE()

#define FLASH_SPIx_PINS_CLK_ENABLES()	\
		__GPIOA_CLK_ENABLE();			\
		__GPIOB_CLK_ENABLE()



#define FLASH_SPI_CS				PB7
#define FLASH_SPI_WP				PB6
#define FLASH_SPI_HOLD				PA15


#define FLASH_SPI_CS_GPIO_PIN			GPIO_PIN_7
#define FLASH_SPI_CS_GPIO_PORT			GPIOB
#define FLASH_SPI_WP_GPIO_PIN			GPIO_PIN_6
#define FLASH_SPI_WP_GPIO_PORT			GPIOB
#define FLASH_SPI_HOLD_GPIO_PIN			GPIO_PIN_15
#define FLASH_SPI_HOLD_GPIO_PORT		GPIOA



#define SPIx_FORCE_RESET()               __SPI1_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __SPI1_RELEASE_RESET()

// Definition for SPIx Pins
#define SPIx_SCK_PIN                     GPIO_PIN_3
#define SPIx_SCK_GPIO_PORT               GPIOB
#define SPIx_SCK_AF                      GPIO_AF0_SPI1
#define SPIx_MISO_PIN                    GPIO_PIN_4
#define SPIx_MISO_GPIO_PORT              GPIOB
#define SPIx_MISO_AF                     GPIO_AF0_SPI1
#define SPIx_MOSI_PIN                    GPIO_PIN_5
#define SPIx_MOSI_GPIO_PORT              GPIOB
#define SPIx_MOSI_AF                     GPIO_AF0_SPI1

#endif



#ifdef CONFIG_USE_PANEL_NODEMEDIUM


// FLASH_SPI_CS				PB0
// FLASH_SPI_WP				PB1
// FLASH_SPI_HOLD			PA4
// FLASH - SPI
// CLK						PA5
// MISO						PA6
// MOSI						PA7



#define FLASH_SPIx						SPI1


#define FLASH_SPIx_PINS_CLK_ENABLES()	\
		__GPIOA_CLK_ENABLE();			\
		__GPIOB_CLK_ENABLE()


#define SPIx_CLK_ENABLE()				__SPI1_CLK_ENABLE()


#define SPIx_FORCE_RESET()				__SPI1_FORCE_RESET()
#define SPIx_RELEASE_RESET()			__SPI1_RELEASE_RESET()


// Definition for SPIx Pins
#define SPIx_SCK_PIN					GPIO_PIN_5
#define SPIx_SCK_GPIO_PORT				GPIOA
#define SPIx_SCK_AF						GPIO_AF5_SPI1
#define SPIx_MISO_PIN					GPIO_PIN_6
#define SPIx_MISO_GPIO_PORT				GPIOA
#define SPIx_MISO_AF					GPIO_AF5_SPI1
#define SPIx_MOSI_PIN					GPIO_PIN_7
#define SPIx_MOSI_GPIO_PORT				GPIOA
#define SPIx_MOSI_AF					GPIO_AF5_SPI1


#define FLASH_SPI_CS_GPIO_PIN			GPIO_PIN_0
#define FLASH_SPI_CS_GPIO_PORT			GPIOB
#define FLASH_SPI_WP_GPIO_PIN			GPIO_PIN_1
#define FLASH_SPI_WP_GPIO_PORT			GPIOB
#define FLASH_SPI_HOLD_GPIO_PIN			GPIO_PIN_4
#define FLASH_SPI_HOLD_GPIO_PORT		GPIOA


#endif	// CONFIG_USE_PANEL_NODEMEDIUM



///////////////////////////////////////////////////////////////////////////////
// DEBUG - USART
///////////////////////////////////////////////////////////////////////////////



#if defined(CONFIG_USE_PANEL_NODEMEDIUM) || defined(CONFIG_USE_PANEL_HOMEAUTCENTERPANEL)

// USART
// DEBUG_USART		USART1
// DEBUG_USART_TX	-	PA9
// DEBUG_USART_RX	-	PA10


#define DEBUG_USARTx					USART1


//#define DEBUG_USART_CLK_ENABLE()		__USART1_CLK_ENABLE()

#define DEBUG_USART_CLK_ENABLES()		\
		__USART1_CLK_ENABLE();			\
		__GPIOA_CLK_ENABLE()


#define DEBUG_USART_TX_GPIO_PORT		GPIOA
#define DEBUG_USART_TX_GPIO_PIN			GPIO_PIN_9

#define DEBUG_USART_RX_GPIO_PORT		GPIOA
#define DEBUG_USART_RX_GPIO_PIN			GPIO_PIN_10

#define DEBUG_USART_AF					GPIO_AF7_USART1

#define DEBUG_USART_BAUDRATE			9600

// Definition for USARTx's NVIC
#define DEBUG_USARTx_IRQn               USART1_IRQn
#define DEBUG_USARTx_IRQHandler         USART1_IRQHandler

#define DEBUG_USART_PREEMT_PRIORITY		10
#define DEBUG_USART_SUB_PRIORITY		0

#endif



#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) || defined(CONFIG_USE_PANEL_NUCLEOF401RE)

// USART
// DEBUG_USART			USART6
// DEBUG_USART_TX		PC6
// DEBUG_USART_RX		PC7

#define DEBUG_USARTx					USART6

#define DEBUG_USART_CLK_ENABLES()		\
		__USART6_CLK_ENABLE();			\
		__GPIOC_CLK_ENABLE()


#define DEBUG_USART_TX_GPIO_PORT		GPIOC
#define DEBUG_USART_TX_GPIO_PIN			GPIO_PIN_6

#define DEBUG_USART_RX_GPIO_PORT		GPIOC
#define DEBUG_USART_RX_GPIO_PIN			GPIO_PIN_7

#define DEBUG_USART_AF					GPIO_AF8_USART6

#define DEBUG_USART_BAUDRATE			9600


// Definition for USARTx's NVIC
#define DEBUG_USARTx_IRQn               USART6_IRQn
#define DEBUG_USARTx_IRQHandler         USART6_IRQHandler

#define DEBUG_USART_PREEMT_PRIORITY		10
#define DEBUG_USART_SUB_PRIORITY		0


#endif



///////////////////////////////////////////////////////////////////////////////
//			ESP8266		-	USART
///////////////////////////////////////////////////////////////////////////////



#ifdef CONFIG_USE_PANEL_NODESMALL


// USART
// DEBUG_USART		USART1
// DEBUG_USART_TX	-	PA9
// DEBUG_USART_RX	-	PA10


#define ESP8266_USARTx					USART1



#define ESP8266_USART_CLK_ENABLES()		\
		__USART1_CLK_ENABLE();			\
		__GPIOA_CLK_ENABLE()



#define ESP8266_USART_TX_GPIO_PORT		GPIOA
#define ESP8266_USART_TX_GPIO_PIN		GPIO_PIN_9

#define ESP8266_USART_RX_GPIO_PORT		GPIOA
#define ESP8266_USART_RX_GPIO_PIN		GPIO_PIN_10

#define ESP8266_USART_AF				GPIO_AF1_USART1

/* Definition for USARTx's NVIC */
#define ESP8266_USARTx_IRQn				USART1_IRQn
#define ESP8266_USARTx_IRQHandler		USART1_IRQHandler

// WITHOUT FREERTOS: If use FreeRTOS, you need higher priority
#define ESP8266_USART_PREEMT_PRIORITY	2
#define ESP8266_USART_SUB_PRIORITY		0

#define ESP8266_USART_BAUDRATE			9600


#endif // #ifdef CONFIG_USE_PANEL_NODESMALL



#ifdef CONFIG_USE_PANEL_NODEMEDIUM

// USART2
// USART_TX		PA2
// USART_RX		PA3

#define ESP8266_USARTx						USART2


#define ESP8266_USART_CLK_ENABLES()			\
		__USART2_CLK_ENABLE();				\
		__GPIOA_CLK_ENABLE()	



#define ESP8266_USART_TX_GPIO_PORT			GPIOA
#define ESP8266_USART_TX_GPIO_PIN			GPIO_PIN_2

#define ESP8266_USART_RX_GPIO_PORT			GPIOA
#define ESP8266_USART_RX_GPIO_PIN			GPIO_PIN_3
	
#define ESP8266_USART_AF					GPIO_AF7_USART2

// Definition for USARTx's NVIC
#define ESP8266_USARTx_IRQn              	USART2_IRQn
#define ESP8266_USARTx_IRQHandler        	USART2_IRQHandler

#define ESP8266_USART_PREEMT_PRIORITY		14
#define ESP8266_USART_SUB_PRIORITY			0

#define ESP8266_USART_BAUDRATE				9600



#endif // #ifdef CONFIG_USE_PANEL_NODEMEDIUM



#ifdef CONFIG_USE_PANEL_HOMEAUTCENTERPANEL

// USART2
// USART_TX		PA2
// USART_RX		PA3

#define ESP8266_USARTx						USART2

#define ESP8266_USART_CLK_ENABLES()			\
		__USART2_CLK_ENABLE();				\
		__GPIOA_CLK_ENABLE()	


#define ESP8266_USART_TX_GPIO_PORT			GPIOA
#define ESP8266_USART_TX_GPIO_PIN			GPIO_PIN_2

#define ESP8266_USART_RX_GPIO_PORT			GPIOA
#define ESP8266_USART_RX_GPIO_PIN			GPIO_PIN_3
	
#define ESP8266_USART_AF					GPIO_AF7_USART2

// Definition for USARTx's NVIC
#define ESP8266_USARTx_IRQn              	USART2_IRQn
#define ESP8266_USARTx_IRQHandler        	USART2_IRQHandler

#define ESP8266_USART_PREEMT_PRIORITY		14
#define ESP8266_USART_SUB_PRIORITY			0

#define ESP8266_USART_BAUDRATE				9600


#endif // #ifdef CONFIG_USE_PANEL_CENTERPANEL



#if defined(CONFIG_USE_PANEL_STM32F4DISCOVERY) && defined(CONFIG_MODULE_ESP8266_ENABLE)

// USART3
// USART_TX		PD8
// USART_RX		PB11

#define ESP8266_USARTx						USART3

#define ESP8266_USART_CLK_ENABLES()			\
		__USART3_CLK_ENABLE();				\
		__GPIOB_CLK_ENABLE();				\
		__GPIOD_CLK_ENABLE()


#define ESP8266_USART_TX_GPIO_PORT			GPIOD
#define ESP8266_USART_TX_GPIO_PIN			GPIO_PIN_8

#define ESP8266_USART_RX_GPIO_PORT			GPIOB
#define ESP8266_USART_RX_GPIO_PIN			GPIO_PIN_11

#define ESP8266_USART_AF					GPIO_AF7_USART3

// Definition for USARTx's NVIC
#define ESP8266_USARTx_IRQn              	USART3_IRQn
#define ESP8266_USARTx_IRQHandler        	USART3_IRQHandler

#define ESP8266_USART_PREEMT_PRIORITY		14
#define ESP8266_USART_SUB_PRIORITY			0

#define ESP8266_USART_BAUDRATE				9600




/*
ESP8266	- Other pins
_GPIO0 			PA12
_RST			PB15
_CH				PA8
_GPIO2			PA11
*/

#define ESP8266_GPIO0_GPIO_PIN			GPIO_PIN_12
#define ESP8266_GPIO0_GPIO_PORT			GPIOA
#define ESP8266_RST_GPIO_PIN			GPIO_PIN_15
#define ESP8266_RST_GPIO_PORT			GPIOB

#define ESP8266_CH_GPIO_PIN				GPIO_PIN_8
#define ESP8266_CH_GPIO_PORT			GPIOA
#define ESP8266_GPIO2_GPIO_PIN			GPIO_PIN_11
#define ESP8266_GPIO2_GPIO_PORT			GPIOA


#define ESP8266_PINS_CLK_ENABLES()		\
		__GPIOB_CLK_ENABLE();			\
		__GPIOA_CLK_ENABLE()



#endif // #ifdef CONFIG_USE_PANEL_CENTERPANEL



///////////////////////////////////////////////////////////////////////////////
//			ESP8266		-	OTHER PINS
///////////////////////////////////////////////////////////////////////////////



#ifdef CONFIG_USE_PANEL_NODESMALL
/*
ESP8266
_GPIO0 			PA12
_RST			PA11
_CH				PA8
_GPIO2			PB15
*/


#define ESP8266_GPIO0_GPIO_PIN			GPIO_PIN_12
#define ESP8266_GPIO0_GPIO_PORT			GPIOA
#define ESP8266_RST_GPIO_PIN			GPIO_PIN_11
#define ESP8266_RST_GPIO_PORT			GPIOA

#define ESP8266_CH_GPIO_PIN				GPIO_PIN_8
#define ESP8266_CH_GPIO_PORT			GPIOA
#define ESP8266_GPIO2_GPIO_PIN			GPIO_PIN_15
#define ESP8266_GPIO2_GPIO_PORT			GPIOB


#define ESP8266_PINS_CLK_ENABLES()		\
		__GPIOB_CLK_ENABLE();			\
		__GPIOA_CLK_ENABLE()

#endif	// #ifdef CONFIG_USE_PANEL_NODESMALL



#ifdef CONFIG_USE_PANEL_NODEMEDIUM

/*
ESP8266
_GPIO0 			PA1
_RST			PC3
_CH				PC9
_GPIO2			PA8
*/


#define ESP8266_GPIO0_GPIO_PIN			GPIO_PIN_1
#define ESP8266_GPIO0_GPIO_PORT			GPIOA
#define ESP8266_RST_GPIO_PIN			GPIO_PIN_3
#define ESP8266_RST_GPIO_PORT			GPIOC

#define ESP8266_CH_GPIO_PIN				GPIO_PIN_9
#define ESP8266_CH_GPIO_PORT			GPIOC
#define ESP8266_GPIO2_GPIO_PIN			GPIO_PIN_8
#define ESP8266_GPIO2_GPIO_PORT			GPIOA


#define ESP8266_PINS_CLK_ENABLES()		\
		__GPIOA_CLK_ENABLE();			\
		__GPIOC_CLK_ENABLE()



#endif	// #ifdef CONFIG_USE_PANEL_NODEMEDIUM



#ifdef CONFIG_USE_PANEL_HOMEAUTCENTERPANEL

/*
ESP8266
_GPIO0 			PC13
_RST			PE2
_CH				PB9
_GPIO2			PB8
*/


#define ESP8266_GPIO0_GPIO_PORT			GPIOC
#define ESP8266_GPIO0_GPIO_PIN			GPIO_PIN_13

#define ESP8266_RST_GPIO_PORT			GPIOE
#define ESP8266_RST_GPIO_PIN			GPIO_PIN_2

#define ESP8266_CH_GPIO_PORT			GPIOB
#define ESP8266_CH_GPIO_PIN				GPIO_PIN_9

#define ESP8266_GPIO2_GPIO_PORT			GPIOB
#define ESP8266_GPIO2_GPIO_PIN			GPIO_PIN_8



#define ESP8266_PINS_CLK_ENABLES()		\
		__GPIOB_CLK_ENABLE();			\
		__GPIOC_CLK_ENABLE();			\
		__GPIOE_CLK_ENABLE()



#endif	// #ifdef CONFIG_USE_PANEL_CENTERPANEL



///////////////////////////////////////////////////////////////////////////////
//			IO
///////////////////////////////////////////////////////////////////////////////


/*
DO NOT USE THEM AT NODEMEDIUM:
DIGITAL_INPUT_1		PC7
SENSOR_OPENCLOSE	PC6
*/



// DIGITAL_INPUT_2		PC8
// RELAY_1_EN			PC11
// RELAY_2_EN			PC10


#ifdef CONFIG_USE_PANEL_NODEMEDIUM

#define RELAY_1_GPIO_PORT			GPIOC
#define RELAY_1_GPIO_PIN			GPIO_PIN_11

#define RELAY_2_GPIO_PORT			GPIOC
#define RELAY_2_GPIO_PIN			GPIO_PIN_10



#define RELAY_PINS_CLK_ENABLES()		\
		__GPIOC_CLK_ENABLE()



// SENSOR_MOTION		PC12
// SENSOR_MOITON_EN		X

// SENSOR_SOUND_IMPACT	PB14

#define SENSOR_PINS_CLK_ENABLES()		\
		__GPIOB_CLK_ENABLE();			\
		__GPIOC_CLK_ENABLE()
		

// PIR signaling; HIGH = movement/LOW = no movement
#define SENSOR_MOTION_GPIO_PORT				GPIOC
#define SENSOR_MOTION_GPIO_PIN				GPIO_PIN_12

// SOUND_IMPACT: high, when sound
#define SENSOR_SOUND_IMPACT_GPIO_PORT		GPIOB
#define SENSOR_SOUND_IMPACT_GPIO_PIN		GPIO_PIN_14



#endif



#ifdef CONFIG_USE_PANEL_HOMEAUTCENTERPANEL


// RELAY_1_EN			PD3
// RELAY_2_EN			PD4

// Ideiglenesen felcserélem, hogy a relay_1 az lehessen, amelyik be is van forrasztva (relay_2, alsó)


#define RELAY_1_GPIO_PORT			GPIOD
#define RELAY_1_GPIO_PIN			GPIO_PIN_4

#define RELAY_2_GPIO_PORT			GPIOD
#define RELAY_2_GPIO_PIN			GPIO_PIN_3



#define RELAY_PINS_CLK_ENABLES()		\
		__GPIOD_CLK_ENABLE()



#endif



///////////////////////////////////////////////////////////////////////////////
//			ETC
///////////////////////////////////////////////////////////////////////////////

#ifdef CONFIG_MODULE_RASPBERRYPI_ENABLE

// Raspberry Pi

#define RASPBERRYPI_USARTx				USART1

#define RASPBERRYPI_USART_CLK_ENABLES()		\
		__USART1_CLK_ENABLE();			\
		__GPIOA_CLK_ENABLE()


#define RASPBERRYPI_USART_TX_GPIO_PORT			GPIOA
#define RASPBERRYPI_USART_TX_GPIO_PIN			GPIO_PIN_9

#define RASPBERRYPI_USART_RX_GPIO_PORT			GPIOA
#define RASPBERRYPI_USART_RX_GPIO_PIN			GPIO_PIN_10

#define RASPBERRYPI_USART_AF					GPIO_AF7_USART1

#define RASPBERRYPI_USARTx_IRQn              	USART1_IRQn
#define RASPBERRYPI_USARTx_IRQHandler        	USART1_IRQHandler

#define RASPBERRYPI_USART_PREEMT_PRIORITY		14
#define RASPBERRYPI_USART_SUB_PRIORITY			0

#endif



#endif /* BOARD_H_ */
