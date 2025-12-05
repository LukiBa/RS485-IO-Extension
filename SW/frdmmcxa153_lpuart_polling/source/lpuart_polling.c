/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#define BOARD_DEBUG_UART_BAUDRATE 9600U
#include "board.h"
#include "app.h"
#include "pin_mux.h"
#include "fsl_lpuart.h"
#include "fsl_gpio.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

uint8_t txbuff[]   = "Lpuart polling example\r\nBoard will send back received characters\r\n";
uint8_t rxbuff[20] = {0};

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
int main(void)
{
    uint8_t ch;
    lpuart_config_t config;
    gpio_pin_config_t inh_config = {
            kGPIO_DigitalOutput,
            0u,
        };

    BOARD_InitHardware();


    GPIO_PinInit(BOARD_INITPINS_UART2_DE_GPIO, BOARD_INITPINS_UART2_DE_GPIO_PIN, &inh_config);

    /*
     * config.baudRate_Bps = 115200U;
     * config.parityMode = kLPUART_ParityDisabled;
     * config.stopBitCount = kLPUART_OneStopBit;
     * config.txFifoWatermark = 0;
     * config.rxFifoWatermark = 0;
     * config.enableTx = false;
     * config.enableRx = false;
     */
    LPUART_GetDefaultConfig(&config);
    config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    config.enableTx     = true;
    config.enableRx     = true;
    config.enableRxRTS  = false;

    LPUART_Init(DEMO_LPUART, &config, DEMO_LPUART_CLK_FREQ);

    //LPUART_WriteBlocking(DEMO_LPUART, txbuff, sizeof(txbuff) - 1);

    while (1)
    {
aaaaaa    	GPIO_PinWrite(BOARD_INITPINS_UART2_DE_GPIO,BOARD_INITPINS_UART2_DE_GPIO_PIN,0u);
    	LPUART_ReadBlocking(DEMO_LPUART, &ch, 1);
        //LPUART_WriteBlocking(DEMO_LPUART, &ch, 1);
    }
}
