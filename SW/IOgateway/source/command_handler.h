/*
 * command_handler.h
 *
 *  Created on: 24.11.2025
 *      Author: baischerl
 */

#ifndef COMMAND_HANDLER_H_
#define COMMAND_HANDLER_H_

#include "IO_Gateway_Config.h"

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
	CMD_SET_DIMMER_ALL_FIELD,
	CMD_SET_DIMMER1_BRIGHTNESS = 	0x41,
	CMD_SET_DIMMER2_BRIGHTNESS,
	CMD_SET_DIMMER3_BRIGHTNESS,
	CMD_SET_DIMMER4_BRIGHTNESS,
	CMD_SET_DIMMER5_BRIGHTNESS,
	CMD_SET_DIMMER6_BRIGHTNESS,
	CMD_SET_DIMMER7_BRIGHTNESS,
	CMD_SET_DIMMER8_BRIGHTNESS,

} eCommandList_t;

typedef struct __attribute__((__packed__)) sCommand{
	uint8_t commandLength;
	uint8_t addr;
	uint8_t commandID;
	uint8_t commandMsg[MESSAGE_LENGTH_BYTE-sizeof(uint8_t)-sizeof(uint8_t)];
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
	uint8_t state;
} sCMD_SET_DIMMER_t;

typedef struct __attribute__((__packed__)) sCMD_SET_DIMMER_BRIGHTNESS{
	uint8_t brightness;
} sCMD_SET_DIMMER_BRIGHTNESS_t;

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

void commandQueueAdd(sCommand_t *command);
void commandWorkerInit(uint32_t queue_length, uint32_t commandLength);

#endif /* COMMAND_HANDLER_H_ */
