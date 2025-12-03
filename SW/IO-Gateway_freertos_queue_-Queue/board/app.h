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
#define LPUART_TX_DMA_CHANNEL       0U
#define LPUART_RX_DMA_CHANNEL       1U

#define DEMO_LPUART_TX_EDMA_CHANNEL kDma0RequestLPUART0Tx
#define DEMO_LPUART_RX_EDMA_CHANNEL kDma0RequestLPUART0Rx
#define EXAMPLE_LPUART_DMA_BASEADDR DMA0

#define BOARD_PWM_BASEADDR     (FLEXPWM0)
#define PWM_SRC_CLK_FREQ       (CLOCK_GetFreq(kCLOCK_MainClk))
#define DEMO_PWM_CLOCK_DEVIDER (kPWM_Prescale_Divide_4)
#define DEMO_PWM_FAULT_LEVEL   true
#ifndef DEMO_PWM_DISABLE_MAP_OP
#define DEMO_PWM_DISABLE_MAP_OP
#endif
#define APP_DEFAULT_PWM_FREQUENCY (2000UL)
/*${macro:end}*/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*${prototype:start}*/
void BOARD_InitHardware(void);
/*${prototype:end}*/

#endif /* _APP_H_ */
