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
#include "dimmer_ctrl.h"

/*******************************************************************************
 * Globals
 ******************************************************************************/
static QueueHandle_t command_queue = NULL;


/*******************************************************************************
 * Definitions
 ******************************************************************************/
static void commandWorker(void *pvParameters);

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
    sCMD_SET_DIMMER_t *cmdDimmer;
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
				cmdDimmer = (sCMD_SET_DIMMER_t *) command.commandMsg;
				dimmerSetDutyCycle(DIMMER1,cmdDimmer->brightness);
				setDimmerState(DIMMER1,cmdDimmer->state);
				(void)sprintf(log, "CMD_SET_DIMMER1: State: %u, Brightness %u\n\r", cmdDimmer->state, cmdDimmer->brightness);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER2:
				cmdDimmer = (sCMD_SET_DIMMER_t *) command.commandMsg;
				dimmerSetDutyCycle(DIMMER2,cmdDimmer->brightness);
				setDimmerState(DIMMER2,cmdDimmer->state);
				(void)sprintf(log, "CMD_SET_DIMMER2: State: %u, Brightness %u\n\r", cmdDimmer->state, cmdDimmer->brightness);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER3:
				cmdDimmer = (sCMD_SET_DIMMER_t *) command.commandMsg;
				dimmerSetDutyCycle(DIMMER3,cmdDimmer->brightness);
				setDimmerState(DIMMER3,cmdDimmer->state);
				(void)sprintf(log, "CMD_SET_DIMMER3: State: %u, Brightness %u\n\r", cmdDimmer->state, cmdDimmer->brightness);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER4:
				cmdDimmer = (sCMD_SET_DIMMER_t *) command.commandMsg;
				dimmerSetDutyCycle(DIMMER4,cmdDimmer->brightness);
				setDimmerState(DIMMER4,cmdDimmer->state);
				(void)sprintf(log, "CMD_SET_DIMMER4: State: %u, Brightness %u\n\r", cmdDimmer->state, cmdDimmer->brightness);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER5:
				cmdDimmer = (sCMD_SET_DIMMER_t *) command.commandMsg;
				dimmerSetDutyCycle(DIMMER5,cmdDimmer->brightness);
				setDimmerState(DIMMER5,cmdDimmer->state);
				(void)sprintf(log, "CMD_SET_DIMMER5: State: %u, Brightness %u\n\r", cmdDimmer->state, cmdDimmer->brightness);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER6:
				cmdDimmer = (sCMD_SET_DIMMER_t *) command.commandMsg;
				dimmerSetDutyCycle(DIMMER6,cmdDimmer->brightness);
				setDimmerState(DIMMER6,cmdDimmer->state);
				(void)sprintf(log, "CMD_SET_DIMMER6: State: %u, Brightness %u\n\r", cmdDimmer->state, cmdDimmer->brightness);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER7:
				cmdDimmer = (sCMD_SET_DIMMER_t *) command.commandMsg;
				dimmerSetDutyCycle(DIMMER7,cmdDimmer->brightness);
				setDimmerState(DIMMER7,cmdDimmer->state);
				(void)sprintf(log, "CMD_SET_DIMMER7: State: %u, Brightness %u\n\r", cmdDimmer->state, cmdDimmer->brightness);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER8:
				cmdDimmer = (sCMD_SET_DIMMER_t *) command.commandMsg;
				dimmerSetDutyCycle(DIMMER8,cmdDimmer->brightness);
				setDimmerState(DIMMER8,cmdDimmer->state);
				(void)sprintf(log, "CMD_SET_DIMMER8: State: %u, Brightness %u\n\r", cmdDimmer->state, cmdDimmer->brightness);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER_ALL_FIELD:
				for(int i=DIMMER1;i<=DIMMER8;++i)
				{
					cmdDimmer = (sCMD_SET_DIMMER_t *) &command.commandMsg[(i-DIMMER1)*sizeof(sCMD_SET_DIMMER_t)];
					dimmerSetDutyCycle(i,cmdDimmer->brightness);
					setDimmerState(i,cmdDimmer->state);
					(void)sprintf(log, "Dimmer %d: State: %u, Brightness %u\n\r",i, cmdDimmer->state, cmdDimmer->brightness);
					uartTxQueueAdd(log);
				}
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
	setup_PWM_dimmers();
	command_queue = xQueueCreate(queue_length, commandLength);
    if (command_queue != NULL)
    {
        vQueueAddToRegistry(command_queue, "ComQ");
    }
    configASSERT(xTaskCreate(commandWorker, "commandWorker", CMD_TASK_STACK_SIZE, NULL, CMD_TASK_PRIO, NULL) == pdPASS)
}
