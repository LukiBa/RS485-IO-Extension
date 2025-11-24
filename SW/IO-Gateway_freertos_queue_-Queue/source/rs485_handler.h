/*
 * rs_485_hanlder.h
 *
 *  Created on: 24.11.2025
 *      Author: baischerl
 */

#ifndef RS485_HANDLER_H_
#define RS485_HANDLER_H_

/* NXP includes */
#include "fsl_lpuart_edma.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void LPUART_Callback(LPUART_Type *base, lpuart_edma_handle_t *handle, status_t status, void *userData);
void initEdmaForUart(edma_config_t *config);
void initUART(LPUART_Type *uart, const char *startUpText);

static void uartRxWorker(void *pvParameters);
static void uartTxWorker(void *pvParameters);

void uartTxQueueAdd(char *log);
void uartTxWorkerInit(uint32_t queue_length, uint32_t max_log_lenght);
void uartRxWorkerInit();



#endif /* RS485_HANDLER_H_ */
