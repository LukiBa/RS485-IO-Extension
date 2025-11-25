/*
 * rs485_handler.c
 *
 *  Created on: 24.11.2025
 *      Author: baischerl
 */

/*System includes.*/
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* NXP includes */
#include "app.h"
#include "board.h"
#include "fsl_debug_console.h"

/* local includes */
#include "IO_Gateway_Config.h"
#include "rs485_handler.h"
#include "command_handler.h"

/*******************************************************************************
 * Globals
 ******************************************************************************/

static LPUART_Type *g_Uart = NULL;
static QueueHandle_t uartTx_queue = NULL;
static SemaphoreHandle_t xUART_Tx_Semaphore;
static SemaphoreHandle_t xUART_Rx_Semaphore;

lpuart_edma_handle_t g_lpuartEdmaHandle;
edma_handle_t g_lpuartTxEdmaHandle;
edma_handle_t g_lpuartRxEdmaHandle;

AT_NONCACHEABLE_SECTION_INIT(uint8_t g_rxBuffer[MESSAGE_LENGTH_BYTE]) = {0};
AT_NONCACHEABLE_SECTION_INIT(uint8_t g_txBuffer[MAX_LOG_LENGTH]) = {0};

static void uartRxWorker(void *pvParameters);
static void uartTxWorker(void *pvParameters);

void LPUART_Callback(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData);
void initEdmaForUart(edma_config_t *config);
void uartTxWorkerInit(uint32_t queue_length, uint32_t max_log_lenght);
void uartRxWorkerInit();

void LPUART_Callback(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData)
{
	static char log[MAX_LOG_LENGTH+1];
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	userData = userData;
    if (kStatus_LPUART_TxIdle == status)
    {
		xSemaphoreGive(xUART_Tx_Semaphore);//,&xHigherPriorityTaskWoken);
    }
    else if (kStatus_LPUART_RxIdle == status)
    {

		xSemaphoreGive(xUART_Rx_Semaphore);//,&xHigherPriorityTaskWoken);
    	commandQueueAdd((sCommand_t*) g_rxBuffer);
    }else
    {
    	sprintf(log,"ERROR: LPUART Callback: Got Status Code: %d",status);
    	uartTxQueueAdd(log);
    }
}

void initEdmaForUart(edma_config_t *config)
{

    /* Init the EDMA module */
    EDMA_GetDefaultConfig(config);
    EDMA_Init(EXAMPLE_LPUART_DMA_BASEADDR, config);
    EDMA_CreateHandle(&g_lpuartTxEdmaHandle, EXAMPLE_LPUART_DMA_BASEADDR, LPUART_TX_DMA_CHANNEL);
    EDMA_CreateHandle(&g_lpuartRxEdmaHandle, EXAMPLE_LPUART_DMA_BASEADDR, LPUART_RX_DMA_CHANNEL);
    EDMA_SetChannelMux(EXAMPLE_LPUART_DMA_BASEADDR, LPUART_TX_DMA_CHANNEL, DEMO_LPUART_TX_EDMA_CHANNEL);
    EDMA_SetChannelMux(EXAMPLE_LPUART_DMA_BASEADDR, LPUART_RX_DMA_CHANNEL, DEMO_LPUART_RX_EDMA_CHANNEL);
    LPUART_TransferCreateHandleEDMA(g_Uart, &g_lpuartEdmaHandle, LPUART_Callback, NULL, &g_lpuartTxEdmaHandle,
                                        &g_lpuartRxEdmaHandle);
}

void uartRxWorker(void *pvParameters)
{
	static lpuart_transfer_t receiveXfer;
	receiveXfer.data     = g_rxBuffer;
	receiveXfer.dataSize = MESSAGE_LENGTH_BYTE;
	status_t status;
	xSemaphoreGive(xUART_Rx_Semaphore);
	while (1)
	{
		xSemaphoreTake(xUART_Rx_Semaphore,portMAX_DELAY);
		status = LPUART_ReceiveEDMA(g_Uart, &g_lpuartEdmaHandle, &receiveXfer);
		taskYIELD();
	}
}

static void uartTxWorker(void *pvParameters)
{
    char log[MAX_LOG_LENGTH + 1];
    size_t logLen = 0;
    static lpuart_transfer_t sendXfer;
    sendXfer.data        = g_txBuffer;
	sendXfer.dataSize    = MAX_LOG_LENGTH;
	xSemaphoreGive(xUART_Tx_Semaphore);
    while (1)
    {
        xQueueReceive(uartTx_queue, log, portMAX_DELAY);
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

        sendXfer.data = g_txBuffer;
        sendXfer.dataSize = logLen;
        LPUART_SendEDMA(g_Uart, &g_lpuartEdmaHandle, &sendXfer);
        taskYIELD();
    }
}

void uartRxWorkerInit()
{
	configASSERT(pdPASS == xTaskCreate(uartRxWorker, "UART_RX", UART_RX_TASK_STACK_SIZE, NULL, UART_RX_TASK_PRIO, NULL));
}

void uartTxQueueAdd(char *log)
{
    xQueueSend(uartTx_queue, log, 0);
}

void uartTxWorkerInit(uint32_t queue_length, uint32_t max_log_lenght)
{
    uartTx_queue = xQueueCreate(queue_length, max_log_lenght);
    /* Enable queue view in MCUX IDE FreeRTOS TAD plugin. */
    configASSERT(NULL != uartTx_queue);
    vQueueAddToRegistry(uartTx_queue, "LogQ");
    configASSERT(pdPASS == xTaskCreate(uartTxWorker, "uartTxWorker", UART_TX_TASK_STACK_SIZE, NULL, UART_TX_TASK_PRIO, NULL));
}

void initUART(LPUART_Type *uart, const char *startUpText)
{
	lpuart_config_t config;
	edma_config_t edmaConfig = {0};
	lpuart_transfer_t sendXfer;
	configASSERT(NULL != uart);
	g_Uart = uart;

    LPUART_GetDefaultConfig(&config);
    config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    config.enableTx     = true;
    config.enableRx     = true;
    LPUART_Init(g_Uart, &config, DEMO_LPUART_CLK_FREQ);

	xUART_Rx_Semaphore = xSemaphoreCreateBinary();
	configASSERT(NULL != xUART_Rx_Semaphore);

	xUART_Tx_Semaphore = xSemaphoreCreateBinary();
	configASSERT(NULL != xUART_Tx_Semaphore);

	initEdmaForUart(&edmaConfig);
	uartTxWorkerInit(QUEUE_LENGTH, MAX_LOG_LENGTH);
    uartRxWorkerInit();


    if(NULL != startUpText)
	{
        sendXfer.data = (char *) startUpText;
    	sendXfer.dataSize = strlen(startUpText);
    	LPUART_SendEDMA(g_Uart, &g_lpuartEdmaHandle, &sendXfer);
	}

}

