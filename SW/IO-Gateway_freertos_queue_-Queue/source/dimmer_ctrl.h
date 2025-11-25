/*
 * dimmer_ctrl.h
 *
 *  Created on: 25.11.2025
 *      Author: baischerl
 */

#ifndef DIMMER_CTRL_H_
#define DIMMER_CTRL_H_

#include "fsl_common.h"

#define DIMMER_COUNT 8

typedef enum eDimmers {
	DIMMER1 = 0,
	DIMMER2,
	DIMMER3,
	DIMMER4,
	DIMMER5,
	DIMMER6,
	DIMMER7,
	DIMMER8
} eDimmers_t;

typedef enum eDimmerState {
	DIM_OFF,
	DIM_ON
} eDimmerState_t;

status_t setup_PWM_dimmers();

void dimmerSetDutyCycle(eDimmers_t dimmer, uint8_t dutyCyclePer);
void setDimmmerState(eDimmers_t dimmer, eDimmerState_t state);
void persistDimmerDutyCycles();

#endif /* DIMMER_CTRL_H_ */
