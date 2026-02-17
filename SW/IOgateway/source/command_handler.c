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
#include "relay_ctrl.h"

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
    sCMD_SET_RELAY_t *cmdRelay;
    char log[MAX_LOG_LENGTH + 1];
    while (1)
    {
        if (xQueueReceive(command_queue, &command, portMAX_DELAY) != pdTRUE)
        {
        	(void)sprintf(log, "Failed to receive from command queue.\r\n");
        	uartTxQueueAdd(log);
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
				cmdRelay = (sCMD_SET_RELAY_t *) command.commandMsg;
				setRelayState(RELAY1,cmdRelay->newState);
				(void)sprintf(log, "CMD %02x St %02x \n\r",command.commandID,cmdRelay->newState);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY2:
				cmdRelay = (sCMD_SET_RELAY_t *) command.commandMsg;
				setRelayState(RELAY2,cmdRelay->newState);
				(void)sprintf(log, "CMD %02x St %02x \n\r",command.commandID,cmdRelay->newState);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY3:
				cmdRelay = (sCMD_SET_RELAY_t *) command.commandMsg;
				setRelayState(RELAY3,cmdRelay->newState);
				(void)sprintf(log, "CMD %02x St %02x \n\r",command.commandID,cmdRelay->newState);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY4:
				cmdRelay = (sCMD_SET_RELAY_t *) command.commandMsg;
				setRelayState(RELAY4,cmdRelay->newState);
				(void)sprintf(log, "CMD %02x St %02x \n\r",command.commandID,cmdRelay->newState);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY5:
				cmdRelay = (sCMD_SET_RELAY_t *) command.commandMsg;
				setRelayState(RELAY5,cmdRelay->newState);
				(void)sprintf(log, "CMD %02x St %02x \n\r",command.commandID,cmdRelay->newState);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY6:
				cmdRelay = (sCMD_SET_RELAY_t *) command.commandMsg;
				setRelayState(RELAY6,cmdRelay->newState);
				(void)sprintf(log, "CMD %02x St %02x \n\r",command.commandID,cmdRelay->newState);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY7:
				cmdRelay = (sCMD_SET_RELAY_t *) command.commandMsg;
				setRelayState(RELAY7,cmdRelay->newState);
				(void)sprintf(log, "CMD %02x St %02x \n\r",command.commandID,cmdRelay->newState);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY8:
				cmdRelay = (sCMD_SET_RELAY_t *) command.commandMsg;
								setRelayState(RELAY8,cmdRelay->newState);
				(void)sprintf(log, "CMD %02x St %02x \n\r",command.commandID,cmdRelay->newState);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY9:
				cmdRelay = (sCMD_SET_RELAY_t *) command.commandMsg;
								setRelayState(RELAY9,cmdRelay->newState);
				(void)sprintf(log, "CMD %02x St %02x \n\r",command.commandID,cmdRelay->newState);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_RELAY_ALL_FIELD:
				(void)sprintf(log, "CMD_SET_RELAY_ALL_FIELD \n\r");
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER1:
				cmdDimmer = (sCMD_SET_DIMMER_t *) command.commandMsg;
				setDimmerState(DIMMER1,cmdDimmer->state);
				dimmerSetDutyCycle(DIMMER1,cmdDimmer->brightness);

				(void)sprintf(log, "CMD %02x St %02x Br %03u\n\r",command.commandID, cmdDimmer->state, cmdDimmer->brightness);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER2:
				cmdDimmer = (sCMD_SET_DIMMER_t *) command.commandMsg;

				setDimmerState(DIMMER2,cmdDimmer->state);
				dimmerSetDutyCycle(DIMMER2,cmdDimmer->brightness);
				(void)sprintf(log, "CMD %02x St %02x Br %03u\n\r",command.commandID, cmdDimmer->state, cmdDimmer->brightness);

				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER3:
				cmdDimmer = (sCMD_SET_DIMMER_t *) command.commandMsg;

				setDimmerState(DIMMER3,cmdDimmer->state);
				dimmerSetDutyCycle(DIMMER3,cmdDimmer->brightness);
				(void)sprintf(log, "CMD %02x St %02x Br %03u\n\r",command.commandID, cmdDimmer->state, cmdDimmer->brightness);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER4:
				cmdDimmer = (sCMD_SET_DIMMER_t *) command.commandMsg;

				setDimmerState(DIMMER4,cmdDimmer->state);
				dimmerSetDutyCycle(DIMMER4,cmdDimmer->brightness);

				(void)sprintf(log, "CMD %02x St %02x Br %03u\n\r",command.commandID, cmdDimmer->state, cmdDimmer->brightness);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER5:
				cmdDimmer = (sCMD_SET_DIMMER_t *) command.commandMsg;

				setDimmerState(DIMMER5,cmdDimmer->state);
				dimmerSetDutyCycle(DIMMER5,cmdDimmer->brightness);

				(void)sprintf(log, "CMD %02x St %02x Br %03u\n\r",command.commandID, cmdDimmer->state, cmdDimmer->brightness);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER6:
				cmdDimmer = (sCMD_SET_DIMMER_t *) command.commandMsg;

				setDimmerState(DIMMER6,cmdDimmer->state);
				dimmerSetDutyCycle(DIMMER6,cmdDimmer->brightness);

				(void)sprintf(log, "CMD %02x St %02x Br %03u\n\r",command.commandID, cmdDimmer->state, cmdDimmer->brightness);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER7:
				cmdDimmer = (sCMD_SET_DIMMER_t *) command.commandMsg;

				setDimmerState(DIMMER7,cmdDimmer->state);
				dimmerSetDutyCycle(DIMMER7,cmdDimmer->brightness);

				(void)sprintf(log, "CMD %02x St %02x Br %03u\n\r",command.commandID, cmdDimmer->state, cmdDimmer->brightness);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER8:
				cmdDimmer = (sCMD_SET_DIMMER_t *) command.commandMsg;

				setDimmerState(DIMMER8,cmdDimmer->state);
				dimmerSetDutyCycle(DIMMER8,cmdDimmer->brightness);

				(void)sprintf(log, "CMD %02x St %02x Br %03u\n\r",command.commandID, cmdDimmer->state, cmdDimmer->brightness);
				uartTxQueueAdd(log);
				break;
			case CMD_SET_DIMMER_ALL_FIELD:
				for(int i=DIMMER1;i<=DIMMER8;++i)
				{
					cmdDimmer = (sCMD_SET_DIMMER_t *) &command.commandMsg[(i-DIMMER1)*sizeof(sCMD_SET_DIMMER_t)];
					setDimmerState(i,cmdDimmer->state);
					dimmerSetDutyCycle(i,cmdDimmer->brightness);
					(void)sprintf(log, "CMD %02x DIM%d St %02x Br %03u\n\r",i+1,command.commandID, cmdDimmer->state, cmdDimmer->brightness);
					uartTxQueueAdd(log);
				}
				break;
			default:
				(void)sprintf(log, "ERROR Invalid Message\n\r");
				uartTxQueueAdd(log);
				break;
        }
        //taskYIELD();
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
