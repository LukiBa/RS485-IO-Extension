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

#include "fsl_lpuart_edma.h"
#if defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT
#include "fsl_dmamux.h"
#endif
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MESSAGE_LENGTH_BYTE 32
#define QUEUE_LENGTH 32
#define DEVICE_ADDRESS 0x01
#define MAX_LOG_LENGTH 64


#define UART_RX_TASK_PRIO tskIDLE_PRIORITY + 1
#define UART_TX_TASK_PRIO tskIDLE_PRIORITY + 1
#define CMD_TASK_PRIO tskIDLE_PRIORITY + 2
#define UART_RX_TASK_STACK_SIZE configMINIMAL_STACK_SIZE + 1958
#define UART_TX_TASK_STACK_SIZE configMINIMAL_STACK_SIZE + 1958
#define CMD_TASK_STACK_SIZE configMINIMAL_STACK_SIZE + 5030

typedef enum eCommandList{
	CMD_GET_STATE	=	0x01,
	CMD_GET_ADC1	=	0x11,
	CMD_GET_ADC2,
	CMD_SET_RELAY1	=	0x21,
	CMD_SET_RELAY2,
	CMD_SET_RELAY3,
	CMD_SET_RELAY4,
	CMD_SET_RELAY5,
	CMD_SET_RELAY6,
	CMD_SET_RELAY7,
	CMD_SET_RELAY8,
	CMD_SET_RELAY9,
	CMD_SET_RELAY_ALL_FIELD,
	CMD_SET_DIMMER1 = 	0x31,
	CMD_SET_DIMMER2,
	CMD_SET_DIMMER3,
	CMD_SET_DIMMER4,
	CMD_SET_DIMMER5,
	CMD_SET_DIMMER6,
	CMD_SET_DIMMER7,
	CMD_SET_DIMMER8,
	CMD_SET_DIMMER_ALL_FIELD
} eCommandList_t;

typedef struct __attribute__((__packed__)) sCommand{
	uint8_t addr;
	uint8_t commandID;
	uint8_t commandMsg[MESSAGE_LENGTH_BYTE-2];
} sCommand_t;

typedef struct __attribute__((__packed__)) sCMD_SET_RELAY{
	uint8_t newState;
} sCMD_SET_RELAY_t;

typedef struct __attribute__((__packed__)) sCMD_SET_RELAY_ALL_FIELD{
	uint8_t newStateRelay1;
	uint8_t newStateRelay2;
	uint8_t newStateRelay3;
	uint8_t newStateRelay4;
	uint8_t newStateRelay5;
	uint8_t newStateRelay6;
	uint8_t newStateRelay7;
	uint8_t newStateRelay8;
	uint8_t newStateRelay9;
} sCMD_SET_RELAY_ALL_FIELD_t;

typedef struct __attribute__((__packed__)) sCMD_SET_DIMMER{
	uint8_t brightness;
	uint16_t fadeTimeMs;
} sCMD_SET_DIMMER_t;

typedef struct __attribute__((__packed__)) sCMD_SET_DIMMER_ALL_FIELD{
	sCMD_SET_DIMMER_t dimmer1;
	sCMD_SET_DIMMER_t dimmer2;
	sCMD_SET_DIMMER_t dimmer3;
	sCMD_SET_DIMMER_t dimmer4;
	sCMD_SET_DIMMER_t dimmer5;
	sCMD_SET_DIMMER_t dimmer6;
	sCMD_SET_DIMMER_t dimmer7;
	sCMD_SET_DIMMER_t dimmer8;
} sCMD_SET_DIMMER_ALL_FIELD_t;

/*******************************************************************************
 * Globals
 ******************************************************************************/
/* Logger queue handle */
static QueueHandle_t log_queue = NULL;
static QueueHandle_t command_queue = NULL;
static SemaphoreHandle_t xUART_Tx_Semaphore;
static SemaphoreHandle_t xUART_Rx_Semaphore;

lpuart_edma_handle_t g_lpuartEdmaHandle;
edma_handle_t g_lpuartTxEdmaHandle;
edma_handle_t g_lpuartRxEdmaHandle;
AT_NONCACHEABLE_SECTION_INIT(uint8_t g_tipString[]) =
    "IO Gateway Command interface initialized\n\rAddress: 0x01\n\r";
AT_NONCACHEABLE_SECTION_INIT(uint8_t g_rxBuffer[sizeof(sCommand_t)]) = {0};
AT_NONCACHEABLE_SECTION_INIT(uint8_t g_txBuffer[MAX_LOG_LENGTH]) = {0};


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* Application API */
static void commandWorker(void *pvParameters);
static void BoardObserverTask(void *pvParameters);
static void WatchdogTask(void *pvParameters);
static void log_task(void *pvParameters);
static void uart_RX_task(void *pvParameters);

void LPUART_UserCallback(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData);

void commandQueueAdd(sCommand_t *command);
void commandWorkerInit(uint32_t queue_length, uint32_t max_log_lenght);

void initEdmaFromDemo(edma_config_t *config);

/* Logger API */
void log_add(char *log);
void log_init(uint32_t queue_length, uint32_t max_log_lenght);



/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
int main(void)
{
	lpuart_config_t config;
	lpuart_transfer_t xfer;
	lpuart_transfer_t sendXfer;
	lpuart_transfer_t receiveXfer;
	edma_config_t userConfig = {0};
	xUART_Tx_Semaphore = xSemaphoreCreateBinary();
	xUART_Rx_Semaphore = xSemaphoreCreateBinary();

	BOARD_InitHardware();

	log_init(QUEUE_LENGTH, MAX_LOG_LENGTH);
	commandWorkerInit(QUEUE_LENGTH, sizeof(sCommand_t));

    LPUART_GetDefaultConfig(&config);
    config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    config.enableTx     = true;
    config.enableRx     = true;

    LPUART_Init(DEMO_LPUART, &config, DEMO_LPUART_CLK_FREQ);


    /* Create LPUART DMA handle. */


    /* Send g_tipString out. */
    xfer.data     = g_tipString;
    xfer.dataSize = sizeof(g_tipString) - 1;
    if (xSemaphoreTake(xUART_Tx_Semaphore, portMAX_DELAY) != pdTRUE)
	{
		PRINTF("Failed to take semaphore.\r\n");
	}
    LPUART_SendEDMA(DEMO_LPUART, &g_lpuartEdmaHandle, &xfer);

    if (xTaskCreate(uart_RX_task, "UART_RX", UART_RX_TASK_STACK_SIZE, NULL, UART_RX_TASK_PRIO, NULL) != pdPASS)
    {
        PRINTF("UART RX Task creation failed!.\r\n");
        while (1)
            ;
    }

    vTaskStartScheduler();
    PRINTF("FATAL ERROR. VTaskStartScheduler() returned.\r\n");
    for (;;)
        ;
}

void LPUART_UserCallback(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData)
{
	static char log[MAX_LOG_LENGTH+1];
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	userData = userData;
    if (kStatus_LPUART_TxIdle == status)
    {
    	xSemaphoreGiveFromISR(xUART_Tx_Semaphore,&xHigherPriorityTaskWoken);
    }
    else if (kStatus_LPUART_RxIdle == status)
    {
    	xSemaphoreGiveFromISR(xUART_Rx_Semaphore,&xHigherPriorityTaskWoken);
    	commandQueueAdd((sCommand_t*) g_rxBuffer);
    }else
    {
    	sprintf(log,"ERROR: LPUART Callback: Got Status Code: %d",status);
    	log_add(log);
    }
}

/*******************************************************************************
 * Application functions
 ******************************************************************************/



/*******************************************************************************
 * Logger functions
 ******************************************************************************/
/*!
 * @brief log_add function
 */
void commandQueueAdd(sCommand_t *command)
{
    xQueueSend(command_queue, command, 0);
}

/*!
 * @brief log_init function
 */
void commandWorkerInit(uint32_t queue_length, uint32_t max_log_lenght)
{
	command_queue = xQueueCreate(queue_length, max_log_lenght);
    if (command_queue != NULL)
    {
        vQueueAddToRegistry(command_queue, "ComQ");
    }
    if (xTaskCreate(commandWorker, "commandWorker", CMD_TASK_STACK_SIZE, NULL, CMD_TASK_PRIO, NULL) != pdPASS)
    {
        PRINTF("commandWorker creation failed!.\r\n");
        while (1)
            ;
    }
}

/*!
 * @brief log_print_task function
 */
static void commandWorker(void *pvParameters)
{
    uint32_t counter = 0;
    sCommand_t command;
    char log[MAX_LOG_LENGTH + 1];
    while (1)
    {
        if (xQueueReceive(command_queue, &command, portMAX_DELAY) != pdTRUE)
        {
            PRINTF("Failed to receive from command queue.\r\n");
        }
        switch(command.commandID)
        {
			case CMD_GET_STATE:
				(void)sprintf(log, "CMD_GET_STATE: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_GET_ADC1:
				(void)sprintf(log, "CMD_GET_ADC1: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_GET_ADC2:
				(void)sprintf(log, "CMD_GET_ADC2: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_RELAY1:
				(void)sprintf(log, "CMD_SET_RELAY1: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_RELAY2:
				(void)sprintf(log, "CMD_SET_RELAY2: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_RELAY3:
				(void)sprintf(log, "CMD_SET_RELAY3: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_RELAY4:
				(void)sprintf(log, "CMD_SET_RELAY4: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_RELAY5:
				(void)sprintf(log, "CMD_SET_RELAY5: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_RELAY6:
				(void)sprintf(log, "CMD_SET_RELAY6: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_RELAY7:
				(void)sprintf(log, "CMD_SET_RELAY7: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_RELAY8:
				(void)sprintf(log, "CMD_SET_RELAY8: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_RELAY9:
				(void)sprintf(log, "CMD_SET_RELAY9: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_RELAY_ALL_FIELD:
				(void)sprintf(log, "CMD_SET_RELAY_ALL_FIELD: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_DIMMER1:
				(void)sprintf(log, "CMD_SET_DIMMER1: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_DIMMER2:
				(void)sprintf(log, "CMD_SET_DIMMER2: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_DIMMER3:
				(void)sprintf(log, "CMD_SET_DIMMER3: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_DIMMER4:
				(void)sprintf(log, "CMD_SET_DIMMER4: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_DIMMER5:
				(void)sprintf(log, "CMD_SET_DIMMER5: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_DIMMER6:
				(void)sprintf(log, "CMD_SET_DIMMER6: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_DIMMER7:
				(void)sprintf(log, "CMD_SET_DIMMER7: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_DIMMER8:
				(void)sprintf(log, "CMD_SET_DIMMER8: %u\n\r", counter);
				log_add(log);
				break;
			case CMD_SET_DIMMER_ALL_FIELD:
				(void)sprintf(log, "CMD_SET_DIMMER_ALL_FIELD: %u\n\r", counter);
				log_add(log);
				break;
			default:
				(void)sprintf(log, "ERROR Invalid Message: %u\n\r", counter);
				log_add(log);
				break;
        }
        taskYIELD();
    }
}

void uart_RX_task(void *pvParameters)
{
	static lpuart_transfer_t receiveXfer;
	receiveXfer.data     = g_rxBuffer;
	receiveXfer.dataSize = MESSAGE_LENGTH_BYTE;
	while (1)
	{
		LPUART_ReceiveEDMA(DEMO_LPUART, &g_lpuartEdmaHandle, &receiveXfer);
	}
}

/*!
 * @brief log_add function
 */
void log_add(char *log)
{
    xQueueSend(log_queue, log, 0);
}

/*!
 * @brief log_init function
 */
void log_init(uint32_t queue_length, uint32_t max_log_lenght)
{
    log_queue = xQueueCreate(queue_length, max_log_lenght);
    /* Enable queue view in MCUX IDE FreeRTOS TAD plugin. */
    if (log_queue != NULL)
    {
        vQueueAddToRegistry(log_queue, "LogQ");
    }
    if (xTaskCreate(log_task, "log_task", UART_TX_TASK_STACK_SIZE, NULL, UART_TX_TASK_PRIO, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
}

/*!
 * @brief log_print_task function
 */
static void log_task(void *pvParameters)
{
    char log[MAX_LOG_LENGTH + 1];
    size_t logLen = 0;
    static lpuart_transfer_t sendXfer;
    sendXfer.data        = g_txBuffer;
	sendXfer.dataSize    = MAX_LOG_LENGTH;
    while (1)
    {
        xQueueReceive(log_queue, log, portMAX_DELAY);
        xSemaphoreTake(xUART_Tx_Semaphore, portMAX_DELAY);
        logLen = strlen(log);
        if (logLen == 0 || logLen >= MAX_LOG_LENGTH)
        {
        	sprintf(g_txBuffer,"ERROR: Invalid Logger Message");
        }
        else
        {
        	strcpy(g_txBuffer,log);
        }

        sendXfer.data        = g_txBuffer;
        sendXfer.dataSize = logLen;
        LPUART_SendEDMA(DEMO_LPUART, &g_lpuartEdmaHandle, &sendXfer);
        taskYIELD();
    }
}


void initEdmaFromDemo(edma_config_t *config)
{
#if defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT
#if defined(LPUART_TX_DMAMUX_CHANNEL) && defined(LPUART_RX_DMAMUX_CHANNEL)
    /* Init DMAMUX */
    DMAMUX_Init(EXAMPLE_LPUART_TX_DMAMUX_BASEADDR);
    DMAMUX_Init(EXAMPLE_LPUART_RX_DMAMUX_BASEADDR);
    /* Set channel for LPUART */
    DMAMUX_SetSource(EXAMPLE_LPUART_TX_DMAMUX_BASEADDR, LPUART_TX_DMAMUX_CHANNEL, LPUART_TX_DMA_REQUEST);
    DMAMUX_SetSource(EXAMPLE_LPUART_RX_DMAMUX_BASEADDR, LPUART_RX_DMAMUX_CHANNEL, LPUART_RX_DMA_REQUEST);
    DMAMUX_EnableChannel(EXAMPLE_LPUART_TX_DMAMUX_BASEADDR, LPUART_TX_DMAMUX_CHANNEL);
    DMAMUX_EnableChannel(EXAMPLE_LPUART_RX_DMAMUX_BASEADDR, LPUART_RX_DMAMUX_CHANNEL);
#else
    /* Init DMAMUX */
    DMAMUX_Init(EXAMPLE_LPUART_DMAMUX_BASEADDR);
    /* Set channel for LPUART */
    DMAMUX_SetSource(EXAMPLE_LPUART_DMAMUX_BASEADDR, LPUART_TX_DMA_CHANNEL, LPUART_TX_DMA_REQUEST);
    DMAMUX_SetSource(EXAMPLE_LPUART_DMAMUX_BASEADDR, LPUART_RX_DMA_CHANNEL, LPUART_RX_DMA_REQUEST);
    DMAMUX_EnableChannel(EXAMPLE_LPUART_DMAMUX_BASEADDR, LPUART_TX_DMA_CHANNEL);
    DMAMUX_EnableChannel(EXAMPLE_LPUART_DMAMUX_BASEADDR, LPUART_RX_DMA_CHANNEL);
#endif
#endif

    /* Init the EDMA module */
    EDMA_GetDefaultConfig(config);
#if defined(BOARD_GetEDMAConfig)
    BOARD_GetEDMAConfig(config);
#endif
    EDMA_Init(EXAMPLE_LPUART_DMA_BASEADDR, config);
    EDMA_CreateHandle(&g_lpuartTxEdmaHandle, EXAMPLE_LPUART_DMA_BASEADDR, LPUART_TX_DMA_CHANNEL);
    EDMA_CreateHandle(&g_lpuartRxEdmaHandle, EXAMPLE_LPUART_DMA_BASEADDR, LPUART_RX_DMA_CHANNEL);
#if defined(FSL_FEATURE_EDMA_HAS_CHANNEL_MUX) && FSL_FEATURE_EDMA_HAS_CHANNEL_MUX
    EDMA_SetChannelMux(EXAMPLE_LPUART_DMA_BASEADDR, LPUART_TX_DMA_CHANNEL, DEMO_LPUART_TX_EDMA_CHANNEL);
    EDMA_SetChannelMux(EXAMPLE_LPUART_DMA_BASEADDR, LPUART_RX_DMA_CHANNEL, DEMO_LPUART_RX_EDMA_CHANNEL);
#endif
    LPUART_TransferCreateHandleEDMA(DEMO_LPUART, &g_lpuartEdmaHandle, LPUART_UserCallback, NULL, &g_lpuartTxEdmaHandle,
                                        &g_lpuartRxEdmaHandle);
}
