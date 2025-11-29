/*
 * relay_ctrl.h
 *
 *  Created on: 26.11.2025
 *      Author: baischerl
 */

#ifndef RELAY_CTRL_H_
#define RELAY_CTRL_H_

#define RELAY_COUNT 9

typedef enum eRelayTypes{
	SWITCH,
	BLINDS_UP,
	BLINDS_DOWN
} eRelayTypes_t;

typedef enum eSetRelayStates{
	SUCCESS,
	FORBIDDEN
} eSetRelayStates_t;

typedef enum eRelayStates{
	RELAY_ON,
	RELAY_OFF
} eRelayStates_t;

typedef enum eAvailableRelays{
	RELAY1 = 0,
	RELAY2,
	RELAY3,
	RELAY4,
	RELAY5,
	RELAY6,
	RELAY7,
	RELAY8,
	RELAY9
} eAvailableRelays_t;

typedef struct sRelay {
	const eAvailableRelays_t id;
	const eRelayTypes_t type;
	const eAvailableRelays_t complementaryRelayId;
	const GPIO_Type* gpioType;
	const uint8_t	gpioPinNbr;
	eRelayStates_t state;
} sRelay_t;


void initRelayGPIOs();
eSetRelayStates_t setRelayState(eAvailableRelays_t id,eRelayStates_t newState);


#endif /* RELAY_CTRL_H_ */
