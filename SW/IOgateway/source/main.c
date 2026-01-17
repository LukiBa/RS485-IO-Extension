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
#include "board.h"
#include "app.h"

/* local includes */
#include "IO_Gateway_Config.h"
#include "rs485_handler.h"
#include "command_handler.h"

#include "fsl_lpuart_edma.h"
#if defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT
#include "fsl_dmamux.h"
#endif


AT_NONCACHEABLE_SECTION_INIT(uint8_t g_tipString[]) =
    "IO Gateway Command V1.01 interface initialized\n\rAddress: 0x01\n\r";


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* Application API */

static void BoardObserverTask(void *pvParameters);
static void WatchdogTask(void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
uint8_t txbuff[]   = "Lpuart polling example\r\nBoard will send back received characters\r\n";

int main(void)
{
	BOARD_InitHardware();



    /* Create LPUART DMA handle. */
	initUART(RS485_LPUART,RS485_UART_BAUDRATE,RS485_LPUART_CLK_FREQ,g_tipString);
	commandWorkerInit(QUEUE_LENGTH, sizeof(sCommand_t));

    vTaskStartScheduler();
    PRINTF("FATAL ERROR. VTaskStartScheduler() returned.\r\n");
    for (;;)
        ;
}


