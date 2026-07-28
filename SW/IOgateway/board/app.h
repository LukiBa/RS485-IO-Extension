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

#define LOG_DEBUG

#define DEMO_LPUART                   LPUART0
#define DEMO_LPUART_CLK_FREQ          (BOARD_DEBUG_UART_CLK_FREQ)
#define DEMO_LPUART_IRQn              LPUART0_IRQn
#define DEMO_LPUART_IRQHandler        LPUART0_IRQHandler
#define DEMO_UART_BAUDRATE            115200U

#define RS485_LPUART                  LPUART2
#define RS485_LPUART_CLK_FREQ         (BOARD_DEBUG_UART_CLK_FREQ)
#define RS485_LPUART_IRQn             LPUART2_IRQn
#define RS485_LPUART_IRQHandler       LPUART2_IRQHandler
#define RS485_UART_BAUDRATE           115200U

#define LPUART_TX_DMA_CHANNEL         0U
#define LPUART_RX_DMA_CHANNEL         1U

#define RS485_LPUART_TX_EDMA_CHANNEL  kDma0RequestLPUART2Tx
#define RS485_LPUART_RX_EDMA_CHANNEL  kDma0RequestLPUART2Rx
#define RS485_LPUART_DMA_BASEADDR     DMA0

#define LPADC_BASE                    ADC0
#define LPADC_IRQn                    ADC0_IRQn
#define LPADC_IRQ_HANDLER_FUNC        ADC0_IRQHandler
#define LPADC_USER_CHANNEL            0U
#define LPADC_USER_CMDID              1U /* CMD1 */
#define LPADC_VREF_SOURCE             kLPADC_ReferenceVoltageAlt3
#define LPADC_DO_OFFSET_CALIBRATION   true
#define LPADC_USE_HIGH_RESOLUTION     true
#define DMA_BASEADDR                  DMA0
#define DMA_CHANNEL_0                 0U
#define DMA_IRQ                       DMA_CH0_IRQn
#define DMA_IRQ_HANDLER               DMA_CH0_IRQHandler
#define ADC_DMA_BUFFER_LENGTH         32U
#define ADC_DMA_BUFFER_SHIFT          5U
#define ADC_DMA_REQUEST               kDma0RequestMuxAdc0FifoRequest
#define MCP9700_TEMP_COEFF            0.01f
#define MCP9700_TEMP_COEFF_FACTOR     (1/MCP9700_TEMP_COEFF)
#define MCP9700_0degC_OFFSET          0.4f
/* Low power timer for ADC Trigger */
#define LPTMR_TRIG_BASE               LPTMR0
#define LPTMR_TRIG_USEC_COUNT         1000U
#define LPTMR_TRIG_IRQn               LPTMR0_IRQn
#define LPTMR_TRIG_HANDLER            LPTMR0_IRQHandler
#define ADC_LPTMR_TRIG_CLOCK          kLPTMR_PrescalerClock_1
#define LPTMR_TRIG_SOURCE_CLOCK       (16000U)

#define LPADC_RESULT_MASK             ADC_RESFIFO_D_MASK
#define LPADC_FULLRANGE               65536U
#define LPADC_RESULTSHIFT             0U

#define BOARD_PWM_BASEADDR            (FLEXPWM0)
#define PWM_SRC_CLK_FREQ              (CLOCK_GetFreq(kCLOCK_MainClk))
#define DEMO_PWM_CLOCK_DEVIDER        (kPWM_Prescale_Divide_4)
#define DEMO_PWM_FAULT_LEVEL          true
#define DEMO_PWM_DISABLE_MAP_OP
#define APP_DEFAULT_PWM_FREQUENCY     (200UL)
#define FAN_TEMP_THRESHOLD            40.0f
#define FAN_TEMP_HYSTERESIS           5.0f
/*${macro:end}*/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*${prototype:start}*/
void BOARD_InitHardware (void);
/*${prototype:end}*/

#endif /* _APP_H_ */
