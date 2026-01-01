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

void initUART(LPUART_Type *uart, uint32_t baudrate, uint32_t clockFrequency, const char *startUpText);

void uartTxQueueAdd(char *log);

#endif /* RS485_HANDLER_H_ */
