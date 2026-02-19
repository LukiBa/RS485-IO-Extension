/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _APP_H_
#define _APP_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*${macro:start}*/
#define DEMO_LPUART            LPUART0
#define DEMO_LPUART_CLK_FREQ   (BOARD_DEBUG_UART_CLK_FREQ)
#define DEMO_LPUART_IRQn       LPUART0_IRQn
#define DEMO_LPUART_IRQHandler LPUART0_IRQHandler
#define DEMO_UART_BAUDRATE 	   115200U



#define RS485_LPUART            LPUART2
#define RS485_LPUART_CLK_FREQ   (BOARD_DEBUG_UART_CLK_FREQ)
#define RS485_LPUART_IRQn       LPUART2_IRQn
#define RS485_LPUART_IRQHandler LPUART2_IRQHandler
#define RS485_UART_BAUDRATE 	115200U

#define LPUART_TX_DMA_CHANNEL       0U
#define LPUART_RX_DMA_CHANNEL       1U

#define RS485_LPUART_TX_EDMA_CHANNEL kDma0RequestLPUART2Tx
#define RS485_LPUART_RX_EDMA_CHANNEL kDma0RequestLPUART2Rx
#define RS485_LPUART_DMA_BASEADDR DMA0

#define ADC_BASE                  ADC0
#define ADC_IRQn                  ADC0_IRQn
#define ADC_IRQ_HANDLER_FUNC      ADC0_IRQHandler
#define ADC_TEMP_SENS_CHANNEL     26U
#define ADC_USER_CMDID            1U /* CMD1 */
#define ADC_SAMPLE_CHANNEL_MODE   kLPADC_SampleChannelSingleEndSideA
#define ADC_VREF_SOURCE           kLPADC_ReferenceVoltageAlt3
#define ADC_DO_OFFSET_CALIBRATION true
#define ADC_OFFSET_VALUE_A        0x10U
#define ADC_OFFSET_VALUE_B        0x10U
#define ADC_USE_HIGH_RESOLUTION   true
#define ADC_TEMP_PARAMETER_A      FSL_FEATURE_LPADC_TEMP_PARAMETER_A
#define ADC_TEMP_PARAMETER_B      FSL_FEATURE_LPADC_TEMP_PARAMETER_B
#define ADC_TEMP_PARAMETER_ALPHA  FSL_FEATURE_LPADC_TEMP_PARAMETER_ALPHA
#define ADC_HARDWARE_AVERAGE      kLPADC_HardwareAverageCount1024

#define BOARD_PWM_BASEADDR     (FLEXPWM0)
#define PWM_SRC_CLK_FREQ       (CLOCK_GetFreq(kCLOCK_MainClk))
#define DEMO_PWM_CLOCK_DEVIDER (kPWM_Prescale_Divide_4)
#define DEMO_PWM_FAULT_LEVEL   true
#ifndef DEMO_PWM_DISABLE_MAP_OP
#define DEMO_PWM_DISABLE_MAP_OP
#endif
#define APP_DEFAULT_PWM_FREQUENCY (20000UL)
/*${macro:end}*/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*${prototype:start}*/
void BOARD_InitHardware(void);
/*${prototype:end}*/

#endif /* _APP_H_ */
