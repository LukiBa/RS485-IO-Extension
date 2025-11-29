/*
 * relay_ctrl.c
 *
 *  Created on: 26.11.2025
 *      Author: baischerl
 */

#include "fsl_gpio.h"

#include "pin_mux.h"

#include "relay_ctrl.h"


sRelay_t g_relayArray[] = {
		[RELAY1].id = RELAY1,
		[RELAY1].type = BLINDS_UP,
		[RELAY1].complementaryRelayId = RELAY2,
		[RELAY1].gpioType = BOARD_INITPINS_Relay_K1_GPIO,
		[RELAY1].gpioPinNbr = BOARD_INITPINS_Relay_K1_GPIO_PIN,
		[RELAY1].state = 0,

		[RELAY2].id = RELAY2,
		[RELAY2].type = BLINDS_DOWN,
		[RELAY2].complementaryRelayId = RELAY1,
		[RELAY2].gpioType = BOARD_INITPINS_Relay_K2_GPIO,
		[RELAY2].gpioPinNbr = BOARD_INITPINS_Relay_K2_GPIO_PIN,
		[RELAY2].state = 0,

		[RELAY3].id = RELAY3,
		[RELAY3].type = BLINDS_UP,
		[RELAY3].complementaryRelayId = RELAY4,
		[RELAY3].gpioType = BOARD_INITPINS_Relay_K3_GPIO,
		[RELAY3].gpioPinNbr = BOARD_INITPINS_Relay_K3_GPIO_PIN,
		[RELAY3].state = 0,

		[RELAY4].id = RELAY4,
		[RELAY4].type = BLINDS_DOWN,
		[RELAY4].complementaryRelayId = RELAY4,
		[RELAY4].gpioType = BOARD_INITPINS_Relay_K4_GPIO,
		[RELAY4].gpioPinNbr = BOARD_INITPINS_Relay_K4_GPIO_PIN,
		[RELAY4].state = 0,

		[RELAY5].id = RELAY5,
		[RELAY5].type = BLINDS_UP,
		[RELAY5].complementaryRelayId = RELAY6,
		[RELAY5].gpioType = BOARD_INITPINS_Relay_K5_GPIO,
		[RELAY5].gpioPinNbr = BOARD_INITPINS_Relay_K5_GPIO_PIN,
		[RELAY5].state = 0,

		[RELAY6].id = RELAY6,
		[RELAY6].type = BLINDS_DOWN,
		[RELAY6].complementaryRelayId = RELAY5,
		[RELAY6].gpioType = BOARD_INITPINS_Relay_K6_GPIO,
		[RELAY6].gpioPinNbr = BOARD_INITPINS_Relay_K6_GPIO_PIN,
		[RELAY6].state = 0,

		[RELAY7].id = RELAY7,
		[RELAY7].type = BLINDS_UP,
		[RELAY7].complementaryRelayId = RELAY8,
		[RELAY7].gpioType = BOARD_INITPINS_Relay_K7_GPIO,
		[RELAY7].gpioPinNbr = BOARD_INITPINS_Relay_K7_GPIO_PIN,
		[RELAY7].state = 0,

		[RELAY8].id = RELAY8,
		[RELAY8].type = BLINDS_DOWN,
		[RELAY8].complementaryRelayId = RELAY7,
		[RELAY8].gpioType = BOARD_INITPINS_Relay_K8_GPIO,
		[RELAY8].gpioPinNbr = BOARD_INITPINS_Relay_K8_GPIO_PIN,
		[RELAY8].state = 0,

		[RELAY9].id = RELAY9,
		[RELAY9].type = SWITCH,
		[RELAY9].complementaryRelayId = RELAY9,
		[RELAY9].gpioType = BOARD_INITPINS_Relay_K9_GPIO,
		[RELAY9].gpioPinNbr = BOARD_INITPINS_Relay_K9_GPIO_PIN,
		[RELAY9].state = 0,
};


void initRelayGPIOs()
{
    	gpio_pin_config_t inh_config = {
        kGPIO_DigitalOutput,
        0u,
    };
    GPIO_PinInit(BOARD_INITPINS_Relay_K1_GPIO, BOARD_INITPINS_Relay_K1_GPIO_PIN, &inh_config);
    GPIO_PinInit(BOARD_INITPINS_Relay_K2_GPIO, BOARD_INITPINS_Relay_K2_GPIO_PIN, &inh_config);
    GPIO_PinInit(BOARD_INITPINS_Relay_K3_GPIO, BOARD_INITPINS_Relay_K3_GPIO_PIN, &inh_config);
    GPIO_PinInit(BOARD_INITPINS_Relay_K4_GPIO, BOARD_INITPINS_Relay_K4_GPIO_PIN, &inh_config);
    GPIO_PinInit(BOARD_INITPINS_Relay_K5_GPIO, BOARD_INITPINS_Relay_K5_GPIO_PIN, &inh_config);
    GPIO_PinInit(BOARD_INITPINS_Relay_K6_GPIO, BOARD_INITPINS_Relay_K6_GPIO_PIN, &inh_config);
    GPIO_PinInit(BOARD_INITPINS_Relay_K7_GPIO, BOARD_INITPINS_Relay_K7_GPIO_PIN, &inh_config);
    GPIO_PinInit(BOARD_INITPINS_Relay_K8_GPIO, BOARD_INITPINS_Relay_K8_GPIO_PIN, &inh_config);
    GPIO_PinInit(BOARD_INITPINS_Relay_K9_GPIO, BOARD_INITPINS_Relay_K9_GPIO_PIN, &inh_config);
}

eSetRelayStates_t setRelayState(eAvailableRelays_t id, eRelayStates_t newState)
{
	if (SWITCH == g_relayArray[id].type)
	{

	}
}
