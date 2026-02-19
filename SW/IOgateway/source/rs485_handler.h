/*
 * rs_485_hanlder.h
 *
 *  Created on: 24.11.2025
 *      Author: baischerl
 */

#ifndef RS485_HANDLER_H_
#define RS485_HANDLER_H_

/* NXP includes */
#include "fsl_lpuart.h"

//#define LOG_DEBUG
#ifdef LOG_DEBUG
#define LOG(x,y) LPUART_WriteBlocking(DEMO_LPUART, x, y)
#else
#define LOG(X,y) ((void)0)
#endif

#define LOGGING_ADDR 0x5F
#define LOGGING_CMD 0x5F

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void initUART(LPUART_Type *uart, uint32_t baudrate, uint32_t clockFrequency);

void uartTxQueueAdd(char *log);

void resetCrc();
void lockCrcModule();
void unlockCrcModule();


#endif /* RS485_HANDLER_H_ */
