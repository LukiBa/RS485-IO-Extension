/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*System includes.*/
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_inputmux.h"
#include "board.h"
#include "app.h"

/* local includes */
#include "IO_Gateway_Config.h"
#include "rs485_handler.h"
#include "command_handler.h"
#include "boardObserver.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* Application API */

static void BoardObserverTask (void *pvParameters);
static void WatchdogTask (void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */

int main (void) {

  SDK_DelayAtLeastUs (2000000, SystemCoreClock);

  BOARD_InitHardware ();

  CLOCK_SetClockDiv (kCLOCK_DivADC0, 1u);
  CLOCK_AttachClk (kFRO12M_to_ADC0);

  CLOCK_SetClockDiv (kCLOCK_DivADC0, 1u);
  CLOCK_AttachClk (kFRO12M_to_ADC0);
  CLOCK_SetupFRO16KClocking (kCLKE_16K_SYSTEM | kCLKE_16K_COREMAIN);
  CLOCK_EnableClock (kCLOCK_GateINPUTMUX0);
  RESET_ReleasePeripheralReset (kADC0_RST_SHIFT_RSTn);
  RESET_ReleasePeripheralReset (kINPUTMUX0_RST_SHIFT_RSTn);
  INPUTMUX_AttachSignal (INPUTMUX0, 0U, kINPUTMUX_Lptmr0ToAdc0Trigger);

  uint8_t txbuff[] = "IO Gateway\r\n";
  LOG(txbuff, sizeof(txbuff) - 1);
  /* Create LPUART DMA handle. */
  initUART (RS485_LPUART, RS485_UART_BAUDRATE, RS485_LPUART_CLK_FREQ);
  commandWorkerInit (QUEUE_LENGTH, sizeof(sCommand_t));
  boardObserverInit ();
  vTaskStartScheduler ();
  for (;;)
    ;
}

