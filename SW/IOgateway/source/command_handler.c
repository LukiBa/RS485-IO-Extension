/*
 * command_handler.c
 *
 *  Created on: 24.11.2025
 *      Author: baischerl
 */

/*System includes.*/
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* NXP includes */
#include "fsl_crc.h"

/* local includes */
#include "command_handler.h"
#include "rs485_handler.h"
#include "dimmer_ctrl.h"
#include "relay_ctrl.h"

/*******************************************************************************
 * Globals
 ******************************************************************************/
static QueueHandle_t command_queue = NULL;
static char gLogBuf[MAX_LOG_LENGTH + 1];
static sCommand_t gCommand;
/*******************************************************************************
 * Definitions
 ******************************************************************************/
static void commandWorker(void *pvParameters);
static int32_t extractBrightness(uint8_t* msg, uint32_t size);
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

    uint32_t crc = 0;
    sCMD_SET_DIMMER_t *cmdDimmer;
    sCMD_SET_RELAY_t *cmdRelay;
    sCMD_SET_DIMMER_BRIGHTNESS_t *cmdDimmerBrightness;
    int32_t dimmerBrightness = 0;
    eDimmerState_t dimmerState = 0;

    while (1)
    {
        if (xQueueReceive(command_queue,&gCommand, portMAX_DELAY) != pdTRUE)
        {
        	(void)sprintf(gLogBuf, "Failed to receive from command queue.\r\n");
        	uartTxQueueAdd(gLogBuf);
        }
        lockCrcModule();
        resetCrc();
        CRC_WriteData(CRC0,(uint8_t*) &gCommand+offsetof(sCommand_t,addr), gCommand.commandLength);
        crc = CRC_Get16bitResult(CRC0);
        unlockCrcModule();
        switch(gCommand.commandID)
        {
			case CMD_GET_STATE:
				(void)sprintf(gLogBuf, "CMD_GET_STATE: %u\n\r", counter);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_GET_ADC1:
				(void)sprintf(gLogBuf, "CMD_GET_ADC1: %u\n\r", counter);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_GET_ADC2:
				(void)sprintf(gLogBuf, "CMD_GET_ADC2: %u\n\r", counter);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_RELAY1:
				cmdRelay = (sCMD_SET_RELAY_t *) gCommand.commandMsg;
				setRelayState(RELAY1,cmdRelay->newState);
				(void)sprintf(gLogBuf, "CMD %02x St %02x \n\r",gCommand.commandID,cmdRelay->newState);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_RELAY2:
				cmdRelay = (sCMD_SET_RELAY_t *) gCommand.commandMsg;
				setRelayState(RELAY2,cmdRelay->newState);
				(void)sprintf(gLogBuf, "CMD %02x St %02x \n\r",gCommand.commandID,cmdRelay->newState);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_RELAY3:
				cmdRelay = (sCMD_SET_RELAY_t *) gCommand.commandMsg;
				setRelayState(RELAY3,cmdRelay->newState);
				(void)sprintf(gLogBuf, "CMD %02x St %02x \n\r",gCommand.commandID,cmdRelay->newState);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_RELAY4:
				cmdRelay = (sCMD_SET_RELAY_t *) gCommand.commandMsg;
				setRelayState(RELAY4,cmdRelay->newState);
				(void)sprintf(gLogBuf, "CMD %02x St %02x \n\r",gCommand.commandID,cmdRelay->newState);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_RELAY5:
				cmdRelay = (sCMD_SET_RELAY_t *) gCommand.commandMsg;
				setRelayState(RELAY5,cmdRelay->newState);
				(void)sprintf(gLogBuf, "CMD %02x St %02x \n\r",gCommand.commandID,cmdRelay->newState);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_RELAY6:
				cmdRelay = (sCMD_SET_RELAY_t *) gCommand.commandMsg;
				setRelayState(RELAY6,cmdRelay->newState);
				(void)sprintf(gLogBuf, "CMD %02x St %02x \n\r",gCommand.commandID,cmdRelay->newState);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_RELAY7:
				cmdRelay = (sCMD_SET_RELAY_t *) gCommand.commandMsg;
				setRelayState(RELAY7,cmdRelay->newState);
				(void)sprintf(gLogBuf, "CMD %02x St %02x \n\r",gCommand.commandID,cmdRelay->newState);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_RELAY8:
				cmdRelay = (sCMD_SET_RELAY_t *) gCommand.commandMsg;
								setRelayState(RELAY8,cmdRelay->newState);
				(void)sprintf(gLogBuf, "CMD %02x St %02x \n\r",gCommand.commandID,cmdRelay->newState);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_RELAY9:
				cmdRelay = (sCMD_SET_RELAY_t *) gCommand.commandMsg;
								setRelayState(RELAY9,cmdRelay->newState);
				(void)sprintf(gLogBuf, "CMD %02x St %02x \n\r",gCommand.commandID,cmdRelay->newState);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_RELAY_ALL_FIELD:
				(void)sprintf(gLogBuf, "CMD_SET_RELAY_ALL_FIELD \n\r");
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_DIMMER1:
				cmdDimmer = (sCMD_SET_DIMMER_t *) gCommand.commandMsg;
				setDimmerState(DIMMER1,cmdDimmer->state);
				dimmerBrightness = getDimmerDutyCycle(DIMMER1);
				(void)sprintf(gLogBuf, "CMD %02x St %02x Br %03ld\n\r",gCommand.commandID, cmdDimmer->state, dimmerBrightness);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_DIMMER2:
				cmdDimmer = (sCMD_SET_DIMMER_t *) gCommand.commandMsg;
				setDimmerState(DIMMER2,cmdDimmer->state);
				dimmerBrightness = getDimmerDutyCycle(DIMMER2);
				(void)sprintf(gLogBuf, "CMD %02x St %02x Br %03ld\n\r",gCommand.commandID, cmdDimmer->state, dimmerBrightness);

				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_DIMMER3:
				cmdDimmer = (sCMD_SET_DIMMER_t *) gCommand.commandMsg;
				setDimmerState(DIMMER3,cmdDimmer->state);
				dimmerBrightness = getDimmerDutyCycle(DIMMER3);
				(void)sprintf(gLogBuf, "CMD %02x St %02x Br %03ld\n\r",gCommand.commandID, cmdDimmer->state, dimmerBrightness);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_DIMMER4:
				cmdDimmer = (sCMD_SET_DIMMER_t *) gCommand.commandMsg;
				setDimmerState(DIMMER4,cmdDimmer->state);
				dimmerBrightness = getDimmerDutyCycle(DIMMER4);
				(void)sprintf(gLogBuf, "CMD %02x St %02x Br %03ld\n\r",gCommand.commandID, cmdDimmer->state, dimmerBrightness);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_DIMMER5:
				cmdDimmer = (sCMD_SET_DIMMER_t *) gCommand.commandMsg;
				setDimmerState(DIMMER5,cmdDimmer->state);
				dimmerBrightness = getDimmerDutyCycle(DIMMER5);
				(void)sprintf(gLogBuf, "CMD %02x St %02x Br %03ld\n\r",gCommand.commandID, cmdDimmer->state, dimmerBrightness);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_DIMMER6:
				cmdDimmer = (sCMD_SET_DIMMER_t *) gCommand.commandMsg;
				setDimmerState(DIMMER6,cmdDimmer->state);
				dimmerBrightness = getDimmerDutyCycle(DIMMER6);
				(void)sprintf(gLogBuf, "CMD %02x St %02x Br %03ld\n\r",gCommand.commandID, cmdDimmer->state, dimmerBrightness);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_DIMMER7:
				cmdDimmer = (sCMD_SET_DIMMER_t *) gCommand.commandMsg;
				setDimmerState(DIMMER7,cmdDimmer->state);
				dimmerBrightness = getDimmerDutyCycle(DIMMER7);
				(void)sprintf(gLogBuf, "CMD %02x St %02x Br %03ld\n\r",gCommand.commandID, cmdDimmer->state, dimmerBrightness);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_DIMMER8:
				cmdDimmer = (sCMD_SET_DIMMER_t *) gCommand.commandMsg;
				setDimmerState(DIMMER8,cmdDimmer->state);
				dimmerBrightness = getDimmerDutyCycle(DIMMER8);
				(void)sprintf(gLogBuf, "CMD %02x St %02x Br %03ld\n\r",gCommand.commandID, cmdDimmer->state, dimmerBrightness);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_DIMMER1_BRIGHTNESS:
				cmdDimmerBrightness = (sCMD_SET_DIMMER_BRIGHTNESS_t *) gCommand.commandMsg;
				dimmerBrightness = extractBrightness(gCommand.commandMsg,gCommand.commandLength-sizeof(int16_t));
				if (dimmerBrightness < 0 || dimmerBrightness>100)
				{
					(void)sprintf(gLogBuf, "CMD %02x St -1 Br %03ld\n\r",gCommand.commandID, dimmerBrightness);
					uartTxQueueAdd(gLogBuf);
					break;
				}
				setDimmerDutyCycle(DIMMER1,dimmerBrightness);
				dimmerState = getDimmerState(DIMMER1);
				(void)sprintf(gLogBuf, "CMD %02x St %02x Br %03ld\n\r",gCommand.commandID, dimmerState, dimmerBrightness);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_DIMMER2_BRIGHTNESS:
				cmdDimmerBrightness = (sCMD_SET_DIMMER_BRIGHTNESS_t *) gCommand.commandMsg;
				dimmerBrightness = extractBrightness(gCommand.commandMsg,gCommand.commandLength-sizeof(int16_t));
				if (dimmerBrightness < 0 || dimmerBrightness>100)
				{
					(void)sprintf(gLogBuf, "CMD %02x St -1 Br %03ld\n\r",gCommand.commandID, dimmerBrightness);
					uartTxQueueAdd(gLogBuf);
					break;
				}
				setDimmerDutyCycle(DIMMER2,dimmerBrightness);
				dimmerState = getDimmerState(DIMMER2);
				(void)sprintf(gLogBuf, "CMD %02x St %02x Br %03ld\n\r",gCommand.commandID, dimmerState, dimmerBrightness);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_DIMMER3_BRIGHTNESS:
				cmdDimmerBrightness = (sCMD_SET_DIMMER_BRIGHTNESS_t *) gCommand.commandMsg;
				dimmerBrightness = extractBrightness(gCommand.commandMsg,gCommand.commandLength-sizeof(int16_t));
				if (dimmerBrightness < 0 || dimmerBrightness>100)
				{
					(void)sprintf(gLogBuf, "CMD %02x St -1 Br %03ld\n\r",gCommand.commandID, dimmerBrightness);
					uartTxQueueAdd(gLogBuf);
					break;
				}
				setDimmerDutyCycle(DIMMER3,dimmerBrightness);
				dimmerState = getDimmerState(DIMMER3);
				(void)sprintf(gLogBuf, "CMD %02x St %02x Br %03ld\n\r",gCommand.commandID, dimmerState, dimmerBrightness);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_DIMMER4_BRIGHTNESS:
				cmdDimmerBrightness = (sCMD_SET_DIMMER_BRIGHTNESS_t *) gCommand.commandMsg;
				dimmerBrightness = extractBrightness(gCommand.commandMsg,gCommand.commandLength-sizeof(int16_t));
				if (dimmerBrightness < 0 || dimmerBrightness>100)
				{
					(void)sprintf(gLogBuf, "CMD %02x St -1 Br %03ld\n\r",gCommand.commandID, dimmerBrightness);
					uartTxQueueAdd(gLogBuf);
					break;
				}
				setDimmerDutyCycle(DIMMER4,dimmerBrightness);
				dimmerState = getDimmerState(DIMMER4);
				(void)sprintf(gLogBuf, "CMD %02x St %02x Br %03ld\n\r",gCommand.commandID, dimmerState, dimmerBrightness);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_DIMMER5_BRIGHTNESS:
				cmdDimmerBrightness = (sCMD_SET_DIMMER_BRIGHTNESS_t *) gCommand.commandMsg;
				dimmerBrightness = extractBrightness(gCommand.commandMsg,gCommand.commandLength-sizeof(int16_t));
				if (dimmerBrightness < 0 || dimmerBrightness>100)
				{
					(void)sprintf(gLogBuf, "CMD %02x St -1 Br %03ld\n\r",gCommand.commandID, dimmerBrightness);
					uartTxQueueAdd(gLogBuf);
					break;
				}
				setDimmerDutyCycle(DIMMER5,dimmerBrightness);
				dimmerState = getDimmerState(DIMMER5);
				(void)sprintf(gLogBuf, "CMD %02x St %02x Br %03ld\n\r",gCommand.commandID, dimmerState, dimmerBrightness);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_DIMMER6_BRIGHTNESS:
				cmdDimmerBrightness = (sCMD_SET_DIMMER_BRIGHTNESS_t *) gCommand.commandMsg;
				dimmerBrightness = extractBrightness(gCommand.commandMsg,gCommand.commandLength-sizeof(int16_t));
				if (dimmerBrightness < 0 || dimmerBrightness>100)
				{
					(void)sprintf(gLogBuf, "CMD %02x St -1 Br %03ld\n\r",gCommand.commandID, dimmerBrightness);
					uartTxQueueAdd(gLogBuf);
					break;
				}
				setDimmerDutyCycle(DIMMER6,dimmerBrightness);
				dimmerState = getDimmerState(DIMMER6);
				(void)sprintf(gLogBuf, "CMD %02x St %02x Br %03ld\n\r",gCommand.commandID, dimmerState, dimmerBrightness);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_DIMMER7_BRIGHTNESS:
				cmdDimmerBrightness = (sCMD_SET_DIMMER_BRIGHTNESS_t *) gCommand.commandMsg;
				dimmerBrightness = extractBrightness(gCommand.commandMsg,gCommand.commandLength-sizeof(int16_t));
				if (dimmerBrightness < 0 || dimmerBrightness>100)
				{
					(void)sprintf(gLogBuf, "CMD %02x St -1 Br %03ld\n\r",gCommand.commandID, dimmerBrightness);
					uartTxQueueAdd(gLogBuf);
					break;
				}
				setDimmerDutyCycle(DIMMER7,dimmerBrightness);
				dimmerState = getDimmerState(DIMMER7);
				(void)sprintf(gLogBuf, "CMD %02x St %02x Br %03ld\n\r",gCommand.commandID, dimmerState, dimmerBrightness);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_DIMMER8_BRIGHTNESS:
				cmdDimmerBrightness = (sCMD_SET_DIMMER_BRIGHTNESS_t *) gCommand.commandMsg;
				dimmerBrightness = extractBrightness(gCommand.commandMsg,gCommand.commandLength-sizeof(int16_t));
				if (dimmerBrightness < 0 || dimmerBrightness>100)
				{
					(void)sprintf(gLogBuf, "CMD %02x St -1 Br %03ld\n\r",gCommand.commandID, dimmerBrightness);
					uartTxQueueAdd(gLogBuf);
					break;
				}
				setDimmerDutyCycle(DIMMER8,dimmerBrightness);
				dimmerState = getDimmerState(DIMMER8);
				(void)sprintf(gLogBuf, "CMD %02x St %02x Br %03ld\n\r",gCommand.commandID, dimmerState, cmdDimmerBrightness->brightness);
				uartTxQueueAdd(gLogBuf);
				break;
			case CMD_SET_DIMMER_ALL_FIELD:
				for(int i=DIMMER1;i<=DIMMER8;++i)
				{
					cmdDimmer = (sCMD_SET_DIMMER_t *) &gCommand.commandMsg[(i-DIMMER1)*sizeof(sCMD_SET_DIMMER_t)];
					setDimmerState(i,cmdDimmer->state);
					dimmerBrightness = getDimmerDutyCycle(i);
					(void)sprintf(gLogBuf, "CMD %02x DIM%d St %02x Br %03ld\n\r",i+1,gCommand.commandID, cmdDimmer->state, dimmerBrightness);
					uartTxQueueAdd(gLogBuf);
				}
				break;
			default:
				(void)sprintf(gLogBuf, "ERROR Invalid Message\n\r");
				uartTxQueueAdd(gLogBuf);
				break;
        }
        //taskYIELD();
    }
}

static int32_t extractBrightness(uint8_t* msg, uint32_t size)
{
	int mPos = 0;
	int brightnessStringStart = 0;
	int brightnessStringEnd = 0;
	char* endptr;
	while (++mPos < size)
	{
		if (msg[mPos-1] == 0x55 && msg[mPos] == 0xaa)
		{
			brightnessStringStart = mPos+1;
		}
		if (msg[mPos-1] == 0xaa && msg[mPos] == 0x55)
		{
			brightnessStringEnd = mPos-2;
			break;
		}
	}
	if (brightnessStringStart == 0 || brightnessStringEnd == 0 || brightnessStringStart>= brightnessStringEnd)
	{
		return -1;
	}
	msg[brightnessStringEnd+1] = 0; // Force end of string. Overwrites 0xaa
	errno = 0;
	int32_t val = strtol(&msg[brightnessStringStart], &endptr, 10);

	if (errno == ERANGE || *endptr != '\0' || ((char* )&msg[brightnessStringStart]) == endptr)
	{
		return -1; //
	}
	return val;
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
