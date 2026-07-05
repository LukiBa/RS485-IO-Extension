/*
 * rs485_handler.c
 *
 *  Created on: 24.11.2025
 *      Author: baischerl
 */

/*System includes.*/
#include <stdio.h>
#include <stddef.h>
#include <string.h>
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


/* NXP includes */
#include "fsl_lpuart.h"
#include "fsl_crc.h"

#include "app.h"
#include "board.h"
#include "pin_mux.h"
//#include "fsl_debug_console.h"


/* local includes */
#include "IO_Gateway_Config.h"
#include "deviceConfig.h"
#include "rs485_handler.h"
#include "command_handler.h"


#define LOG_MAX_MSG_LEN (MESSAGE_LENGTH_BYTE-3)
/*******************************************************************************
 * Globals
 ******************************************************************************/

static LPUART_Type *g_Uart = NULL;
static QueueHandle_t uartTx_queue = NULL;
static SemaphoreHandle_t xUART_Tx_Semaphore;
static SemaphoreHandle_t xUART_Rx_Semaphore;
static SemaphoreHandle_t xCRC_Semaphore;
static lpuart_handle_t g_lpuartHandle;
static crc_config_t g_crcConfig;


//lpuart_edma_handle_t g_lpuartEdmaHandle;
//edma_handle_t g_lpuartTxEdmaHandle;
//edma_handle_t g_lpuartRxEdmaHandle;

AT_NONCACHEABLE_SECTION_INIT(uint8_t g_rxBuffer[MESSAGE_LENGTH_BYTE]) = {0};
AT_NONCACHEABLE_SECTION_INIT(uint8_t g_txBuffer[MESSAGE_LENGTH_BYTE]) = {0};
static uint8_t g_rxMessageBuf[sizeof(sCommand_t)];


static void uartRxWorker(void *pvParameters);
static void uartTxWorker(void *pvParameters);


void LPUART_Callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData);
//void initEdmaForUart(edma_config_t *config);
void uartTxWorkerInit(uint32_t queue_length, uint32_t max_log_lenght);
void uartRxWorkerInit();
static void rxEnable();
static void txEnable();

void LPUART_Callback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *userData)
{
	static char log[MAX_LOG_LENGTH+1];
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	sCommand_t* comm;
	size_t size = 0;

	userData = userData;
    if (kStatus_LPUART_TxIdle == status)
    {
		xSemaphoreGive(xUART_Tx_Semaphore);//,&xHigherPriorityTaskWoken);
		rxEnable();
    }
    else if (kStatus_LPUART_RxIdle == status)
    {

		xSemaphoreGive(xUART_Rx_Semaphore);//,&xHigherPriorityTaskWoken);
		memcpy(g_rxMessageBuf+offsetof(sCommand_t,addr), g_rxBuffer,MESSAGE_LENGTH_BYTE);
    	comm = (sCommand_t*) g_rxMessageBuf;
    	comm->commandLength = MESSAGE_LENGTH_BYTE;
    	if (DEVICE_ADDRESS == comm->addr)
    	{
    		commandQueueAdd((sCommand_t*) comm);
    	}

    }
    else if (kStatus_LPUART_IdleLineDetected == status)
        {
    		LPUART_ClearStatusFlags(g_Uart,kLPUART_IdleLineFlag);
    		size = handle->rxDataSize;
    		if (MESSAGE_LENGTH_BYTE != size)
    		{
    			xSemaphoreGive(xUART_Rx_Semaphore);
    			memcpy(g_rxMessageBuf+offsetof(sCommand_t,addr), g_rxBuffer,MESSAGE_LENGTH_BYTE);
				comm = (sCommand_t*) g_rxMessageBuf;
				comm->commandLength = MESSAGE_LENGTH_BYTE - size;
				if (DEVICE_ADDRESS == comm->addr)
				{
					commandQueueAdd((sCommand_t*) comm);
				}
				LPUART_TransferAbortReceive(g_Uart, &g_lpuartHandle);
    		}

        }
    else
    {
    	sprintf(log,"ERROR: LPUART Callback: Got Status Code: %d.",status);
    	uartTxQueueAdd(log);
    }
}

/*
void initEdmaForUart(edma_config_t *config)
{
    EDMA_GetDefaultConfig(config);
    EDMA_Init(RS485_LPUART_DMA_BASEADDR, config);
    EDMA_CreateHandle(&g_lpuartTxEdmaHandle,
    		RS485_LPUART_DMA_BASEADDR, LPUART_TX_DMA_CHANNEL);
    EDMA_CreateHandle(&g_lpuartRxEdmaHandle,
    		RS485_LPUART_DMA_BASEADDR, LPUART_RX_DMA_CHANNEL);
    EDMA_SetChannelMux(RS485_LPUART_DMA_BASEADDR,
    		LPUART_TX_DMA_CHANNEL, RS485_LPUART_TX_EDMA_CHANNEL);
    EDMA_SetChannelMux(RS485_LPUART_DMA_BASEADDR,
    		LPUART_RX_DMA_CHANNEL, RS485_LPUART_RX_EDMA_CHANNEL);
    LPUART_TransferCreateHandleEDMA(g_Uart, &g_lpuartEdmaHandle,
    		LPUART_Callback, NULL, &g_lpuartTxEdmaHandle,&g_lpuartRxEdmaHandle);
}
*/

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
		//status = LPUART_ReceiveEDMA(g_Uart, &g_lpuartEdmaHandle, &receiveXfer);
		while(kStatus_Success !=
				LPUART_TransferReceiveNonBlocking(g_Uart, &g_lpuartHandle,
						&receiveXfer, NULL))
		{
			taskYIELD();
		}
	}
}

static void uartTxWorker(void *pvParameters)
{
    char log[MAX_LOG_LENGTH + 1];
    int32_t logLen = 0;
    size_t logPos = 0;
    static lpuart_transfer_t sendXfer;
    sendXfer.data        = g_txBuffer;
	sendXfer.dataSize    = MAX_LOG_LENGTH;
	xSemaphoreGive(xUART_Tx_Semaphore);
    while (1)
    {
        xQueueReceive(uartTx_queue, log, portMAX_DELAY);
        logLen = strlen(log);
        logPos = 0;
        while(logLen > 0)
        {
        	xSemaphoreTake(xUART_Tx_Semaphore, portMAX_DELAY);
        	if (logLen >= LOG_MAX_MSG_LEN)
        	{
        		memcpy(&g_txBuffer[2],&log[logPos],MESSAGE_LENGTH_BYTE-3);
        		g_txBuffer[MESSAGE_LENGTH_BYTE-1] = 0;
        		logLen -= LOG_MAX_MSG_LEN;
        		logPos += LOG_MAX_MSG_LEN;
        	}
        	else
        	{
        		strcpy(&g_txBuffer[2],&log[logPos]);

        		logLen = 0;
        		logPos = 0;
        	}
        	g_txBuffer[0] = LOGGING_ADDR;
        	g_txBuffer[1] = LOGGING_CMD;
        	LOG(g_txBuffer,MESSAGE_LENGTH_BYTE);
			sendXfer.data = g_txBuffer;
			sendXfer.dataSize = MESSAGE_LENGTH_BYTE;
			txEnable();
			//LPUART_SendEDMA(g_Uart, &g_lpuartEdmaHandle, &sendXfer);
			LPUART_TransferSendNonBlocking(g_Uart, &g_lpuartHandle, &sendXfer);
        }
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

void txEnable()
{
	GPIO_PinWrite(BOARD_INITPINS_RS485_DE_GPIO,BOARD_INITPINS_RS485_DE_PIN,1u);
	GPIO_PinWrite(BOARD_INITPINS_RS485_nRE_GPIO,BOARD_INITPINS_RS485_nRE_GPIO_PIN,1u);
}

void rxEnable()
{
	GPIO_PinWrite(BOARD_INITPINS_RS485_DE_GPIO,BOARD_INITPINS_RS485_DE_PIN,0u);
	GPIO_PinWrite(BOARD_INITPINS_RS485_nRE_GPIO,BOARD_INITPINS_RS485_nRE_GPIO_PIN,0u);
}

void initUART(LPUART_Type *uart, uint32_t baudrate, uint32_t clockFrequency)
{
	lpuart_config_t config;
	//edma_config_t edmaConfig = {0};
	lpuart_transfer_t sendXfer;
    gpio_pin_config_t inh_config = {
            kGPIO_DigitalOutput,
            0u,
        };

	configASSERT(NULL != uart);
	g_Uart = uart;

	RESET_PeripheralReset(kLPUART2_RST_SHIFT_RSTn);

	GPIO_PinInit(BOARD_INITPINS_RS485_DE_GPIO, BOARD_INITPINS_RS485_DE_PIN, &inh_config);

	xUART_Rx_Semaphore = xSemaphoreCreateBinary();
	configASSERT(NULL != xUART_Rx_Semaphore);

	xUART_Tx_Semaphore = xSemaphoreCreateBinary();
	configASSERT(NULL != xUART_Tx_Semaphore);

	xCRC_Semaphore = xSemaphoreCreateBinary();
	configASSERT(NULL != xCRC_Semaphore);


	CRC_GetDefaultConfig(&g_crcConfig);
	g_crcConfig.polynomial = 0x8005; // Normal form of 0xA001
	g_crcConfig.seed = 0xFFFF; // Modbus initial value
	g_crcConfig.reflectIn = true; // Reflect input bytes
	g_crcConfig.reflectOut = true; // Reflect output
	g_crcConfig.complementChecksum = false; // No final XOR
	g_crcConfig.crcBits = kCrcBits16; // 16-bit CRC config.crcResult = kCrcFinalChecksum;
	CRC_Init(CRC0, &g_crcConfig);
	unlockCrcModule();

    LPUART_GetDefaultConfig(&config);
    config.baudRate_Bps = baudrate;
    config.enableTx     = true;
    config.enableRx     = true;
    LPUART_Init(g_Uart, &config, clockFrequency);
	LPUART_TransferCreateHandle(g_Uart, &g_lpuartHandle, LPUART_Callback, NULL);
    LPUART_EnableInterrupts(g_Uart, kLPUART_IdleLineInterruptEnable);
	NVIC_SetPriority(RS485_LPUART_IRQn, 3);
	EnableIRQ(RS485_LPUART_IRQn);

	//initEdmaForUart(&edmaConfig);
	uartTxWorkerInit(QUEUE_LENGTH, MAX_LOG_LENGTH);
    uartRxWorkerInit();

    sprintf(&g_txBuffer[2],"V01.05 Addr: 0x%02x\n\r",DEVICE_ADDRESS);
	g_txBuffer[0] = LOGGING_ADDR;
	g_txBuffer[1] = LOGGING_CMD;
	g_txBuffer[MESSAGE_LENGTH_BYTE-1] = 0;
	LOG(g_txBuffer,MESSAGE_LENGTH_BYTE);
	sendXfer.data = g_txBuffer;
	sendXfer.dataSize = MESSAGE_LENGTH_BYTE;
	txEnable();
	//LPUART_SendEDMA(g_Uart, &g_lpuartEdmaHandle, &sendXfer);
	LPUART_TransferSendNonBlocking(g_Uart, &g_lpuartHandle, &sendXfer);


}

void resetCrc()
{
	CRC_Init(CRC0, &g_crcConfig);
}

void lockCrcModule()
{
	xSemaphoreTake(xCRC_Semaphore,portMAX_DELAY);
}
void unlockCrcModule()
{
	xSemaphoreGive(xCRC_Semaphore);
}

