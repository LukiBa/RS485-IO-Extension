/*
 * command_handler.c
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

/* NXP includes */
#include "fsl_debug_console.h"

/* local includes */
#include "command_handler.h"
#include "rs485_handler.h"

/*******************************************************************************
 * Globals
 ******************************************************************************/
static QueueHandle_t command_queue = NULL;

/*!
 * @brief uartTxQueueAdd function
 */
void commandQueueAdd(sCommand_t *command)
{
    xQueueSend(command_queue, command, 0);
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
				uartTxQueueAdd(log);
				break;
			case CMD_GET_ADC1:
				(void)sprintf(log, "CMD_GET_ADC1: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_GET_ADC2:
				(void)sprintf(log, "CMD_GET_ADC2: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY1:
				(void)sprintf(log, "CMD_SET_RELAY1: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY2:
				(void)sprintf(log, "CMD_SET_RELAY2: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY3:
				(void)sprintf(log, "CMD_SET_RELAY3: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY4:
				(void)sprintf(log, "CMD_SET_RELAY4: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY5:
				(void)sprintf(log, "CMD_SET_RELAY5: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY6:
				(void)sprintf(log, "CMD_SET_RELAY6: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY7:
				(void)sprintf(log, "CMD_SET_RELAY7: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY8:
				(void)sprintf(log, "CMD_SET_RELAY8: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY9:
				(void)sprintf(log, "CMD_SET_RELAY9: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY_ALL_FIELD:
				(void)sprintf(log, "CMD_SET_RELAY_ALL_FIELD: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER1:
				(void)sprintf(log, "CMD_SET_DIMMER1: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER2:
				(void)sprintf(log, "CMD_SET_DIMMER2: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER3:
				(void)sprintf(log, "CMD_SET_DIMMER3: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER4:
				(void)sprintf(log, "CMD_SET_DIMMER4: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER5:
				(void)sprintf(log, "CMD_SET_DIMMER5: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER6:
				(void)sprintf(log, "CMD_SET_DIMMER6: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER7:
				(void)sprintf(log, "CMD_SET_DIMMER7: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER8:
				(void)sprintf(log, "CMD_SET_DIMMER8: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER_ALL_FIELD:
				(void)sprintf(log, "CMD_SET_DIMMER_ALL_FIELD: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
			default:
				(void)sprintf(log, "ERROR Invalid Message: %u\n\r", counter);
				uartTxQueueAdd(log);
				break;
        }
        taskYIELD();
    }
}

void commandWorkerInit(uint32_t queue_length, uint32_t commandLength)
{
	command_queue = xQueueCreate(queue_length, commandLength);
    if (command_queue != NULL)
    {
        vQueueAddToRegistry(command_queue, "ComQ");
    }
    configASSERT(xTaskCreate(commandWorker, "commandWorker", CMD_TASK_STACK_SIZE, NULL, CMD_TASK_PRIO, NULL) == pdPASS)
}
