/*
 * dimmer_ctrl.c
 *
 *  Created on: 25.11.2025
 *      Author: baischerl
 */
#include "fsl_pwm.h"
#include "fsl_gpio.h"


#include "app.h"

#include "dimmer_ctrl.h"

static void setup_PWMs();
static void configurePwmFaults();
static void initDimmerArray();

typedef struct dimmer {
	eDimmers_t id;
	pwm_submodule_t pwmSubmodule;
	pwm_channels_t pwm_channel;
	eDimmerState_t state;
	uint8_t dutyCycle;
} dimmer_t;

dimmer_t g_dimmerArray[DIMMER_COUNT];

static void updateDimmer(eDimmers_t dimmer)
{
	if (g_dimmerArray[dimmer].state == DIM_ON)
	{
		PWM_UpdatePwmDutycycle(BOARD_PWM_BASEADDR, g_dimmerArray[dimmer].pwmSubmodule, g_dimmerArray[dimmer].pwm_channel, kPWM_EdgeAligned, g_dimmerArray[dimmer].dutyCycle);
	}
}

void setDimmerState(eDimmers_t dimmer, eDimmerState_t state)
{
	g_dimmerArray[dimmer].state = state;
	updateDimmer(dimmer);
}

void dimmerSetDutyCycle(eDimmers_t dimmer, uint8_t dutyCyclePer)
{
	g_dimmerArray[dimmer].dutyCycle = dutyCyclePer;
	updateDimmer(dimmer);
}



static void initDimmerArray()
{
	g_dimmerArray[0].id = DIMMER1;
	g_dimmerArray[0].pwmSubmodule = kPWM_Module_2;
	g_dimmerArray[0].pwm_channel = kPWM_PwmX;
	g_dimmerArray[0].state = DIM_OFF;
	g_dimmerArray[0].dutyCycle = 100u;

	g_dimmerArray[1].id = DIMMER2;
	g_dimmerArray[1].pwmSubmodule = kPWM_Module_2;
	g_dimmerArray[1].pwm_channel = kPWM_PwmB;
	g_dimmerArray[1].state = DIM_OFF;
	g_dimmerArray[1].dutyCycle = 100u;

	g_dimmerArray[2].id = DIMMER3; // 5
	g_dimmerArray[2].pwmSubmodule = kPWM_Module_0;
	g_dimmerArray[2].pwm_channel = kPWM_PwmB;
	g_dimmerArray[2].state = DIM_OFF;
	g_dimmerArray[2].dutyCycle = 100u;

	g_dimmerArray[3].id = DIMMER4; // 6
	g_dimmerArray[3].pwmSubmodule = kPWM_Module_1;
	g_dimmerArray[3].pwm_channel = kPWM_PwmX;
	g_dimmerArray[3].state = DIM_OFF;
	g_dimmerArray[3].dutyCycle = 100u;

	g_dimmerArray[4].id = DIMMER5; // 7
	g_dimmerArray[4].pwmSubmodule = kPWM_Module_2;
	g_dimmerArray[4].pwm_channel = kPWM_PwmA;
	g_dimmerArray[4].state = DIM_OFF;
	g_dimmerArray[4].dutyCycle = 100u;

	g_dimmerArray[5].id = DIMMER6; // 8
	g_dimmerArray[5].pwmSubmodule = kPWM_Module_1;
	g_dimmerArray[5].pwm_channel = kPWM_PwmB;
	g_dimmerArray[5].state = DIM_OFF;
	g_dimmerArray[5].dutyCycle = 100u;

	g_dimmerArray[6].id = DIMMER7; // 9
	g_dimmerArray[6].pwmSubmodule = kPWM_Module_0;
	g_dimmerArray[6].pwm_channel = kPWM_PwmA;
	g_dimmerArray[6].state = DIM_OFF;
	g_dimmerArray[6].dutyCycle = 100u;

	g_dimmerArray[7].id = DIMMER8;
	g_dimmerArray[7].pwmSubmodule = kPWM_Module_0;
	g_dimmerArray[7].pwm_channel = kPWM_PwmX;
	g_dimmerArray[7].state = DIM_OFF;
	g_dimmerArray[7].dutyCycle = 100u;
}

status_t setup_PWM_dimmers()
{
    status_t ret = kStatus_Success;
	pwm_config_t pwmConfig;
    gpio_pin_config_t inh_config = {
        kGPIO_DigitalOutput,
        0,
    };
    PWM_GetDefaultConfig(&pwmConfig);
    pwmConfig.prescale = DEMO_PWM_CLOCK_DEVIDER;

    /* Use full cycle reload */
    pwmConfig.reloadLogic = kPWM_ReloadPwmFullCycle;
    /* PWM A & PWM B form a complementary PWM pair */
    pwmConfig.pairOperation   = kPWM_Independent;
    pwmConfig.enableDebugMode = true;
    /* Initialize submodule 0 */
    ret = PWM_Init(BOARD_PWM_BASEADDR, kPWM_Module_0, &pwmConfig);
    if (kStatus_Success != ret)
    {
        return ret;
    }

    ret = PWM_Init(BOARD_PWM_BASEADDR, kPWM_Module_1, &pwmConfig);
    if (kStatus_Success != ret)
    {;
        return ret;
    }

    /* Initialize submodule 2 the same way as submodule 1 */
    ret = PWM_Init(BOARD_PWM_BASEADDR, kPWM_Module_2, &pwmConfig);
	if (kStatus_Success != ret)
	{
		return ret;
	}

	configurePwmFaults();
	setup_PWMs();
    PWM_SetPwmLdok(BOARD_PWM_BASEADDR, kPWM_Control_Module_0 | kPWM_Control_Module_1 | kPWM_Control_Module_2, true);
    /* Start the PWM generation from Submodules 0, 1 and 2 */
    PWM_StartTimer(BOARD_PWM_BASEADDR, kPWM_Control_Module_0 | kPWM_Control_Module_1 | kPWM_Control_Module_2);
    initDimmerArray();

    return ret;
}

static void setup_PWMs()
{
    uint16_t deadTimeVal;
    pwm_signal_param_t pwmSignal[3];
    uint32_t pwmSourceClockInHz;
    uint32_t pwmFrequencyInHz = APP_DEFAULT_PWM_FREQUENCY;

    pwmSourceClockInHz = PWM_SRC_CLK_FREQ;

    /* Set deadtime count, we set this to about 650ns */
    deadTimeVal = ((uint64_t)pwmSourceClockInHz * 650) / 1000000000;

    pwmSignal[0].pwmChannel       = kPWM_PwmA;
    pwmSignal[0].level            = kPWM_HighTrue;
    pwmSignal[0].dutyCyclePercent = 0;
    pwmSignal[0].deadtimeValue    = deadTimeVal;
    pwmSignal[0].faultState       = kPWM_PwmFaultState0;
    pwmSignal[0].pwmchannelenable = true;

    pwmSignal[1].pwmChannel = kPWM_PwmB;
    pwmSignal[1].level      = kPWM_HighTrue;
    pwmSignal[1].dutyCyclePercent = 0;
    pwmSignal[1].deadtimeValue    = deadTimeVal;
    pwmSignal[1].faultState       = kPWM_PwmFaultState0;
    pwmSignal[1].pwmchannelenable = true;


    pwmSignal[2].pwmChannel = kPWM_PwmX;
    pwmSignal[2].level      = kPWM_HighTrue;
    pwmSignal[2].dutyCyclePercent = 0;
    pwmSignal[2].deadtimeValue    = deadTimeVal;
    pwmSignal[2].faultState       = kPWM_PwmFaultState0;
    pwmSignal[2].pwmchannelenable = true;


    /*********** PWMA_SM0 - phase A, configuration, setup 2 channel as an example ************/
    PWM_SetupPwm(BOARD_PWM_BASEADDR, kPWM_Module_0, pwmSignal, 3, kPWM_EdgeAligned, pwmFrequencyInHz,
                 pwmSourceClockInHz);

    /*********** PWMA_SM1 - phase B configuration, setup PWM A channel only ************/
    PWM_SetupPwm(BOARD_PWM_BASEADDR, kPWM_Module_1, pwmSignal, 3, kPWM_EdgeAligned, pwmFrequencyInHz,
                 pwmSourceClockInHz);

    /*********** PWMA_SM2 - phase C configuration, setup PWM A channel only ************/
    PWM_SetupPwm(BOARD_PWM_BASEADDR, kPWM_Module_2, pwmSignal, 3, kPWM_EdgeAligned, pwmFrequencyInHz,
                 pwmSourceClockInHz);
    /* Write compare for X output */
    BOARD_PWM_BASEADDR->OUTEN |=
         (1u << 0) |   /* PWM_A0 */
         (1u << 1) |   /* PWM_B0 */
         (1u << 2);    /* PWM_X0 */
}

static void configurePwmFaults()
{
	pwm_fault_param_t faultConfig;
    PWM_FaultDefaultConfig(&faultConfig);

#ifdef DEMO_PWM_FAULT_LEVEL
    faultConfig.faultLevel = DEMO_PWM_FAULT_LEVEL;
#endif
    /* Sets up the PWM fault protection */
    PWM_SetupFaults(BOARD_PWM_BASEADDR, kPWM_Fault_0, &faultConfig);
    PWM_SetupFaults(BOARD_PWM_BASEADDR, kPWM_Fault_1, &faultConfig);
    PWM_SetupFaults(BOARD_PWM_BASEADDR, kPWM_Fault_2, &faultConfig);
    PWM_SetupFaults(BOARD_PWM_BASEADDR, kPWM_Fault_3, &faultConfig);

    /* Set PWM fault disable mapping for submodule 0/1/2 */
    PWM_SetupFaultDisableMap(BOARD_PWM_BASEADDR, kPWM_Module_0, kPWM_PwmA, kPWM_faultchannel_0,
                             DEMO_PWM_DISABLE_MAP_OP(kPWM_FaultDisable_0 | kPWM_FaultDisable_1 | kPWM_FaultDisable_2 | kPWM_FaultDisable_3));
    PWM_SetupFaultDisableMap(BOARD_PWM_BASEADDR, kPWM_Module_1, kPWM_PwmA, kPWM_faultchannel_0,
                             DEMO_PWM_DISABLE_MAP_OP(kPWM_FaultDisable_0 | kPWM_FaultDisable_1 | kPWM_FaultDisable_2 | kPWM_FaultDisable_3));
    PWM_SetupFaultDisableMap(BOARD_PWM_BASEADDR, kPWM_Module_2, kPWM_PwmA, kPWM_faultchannel_0,
                             DEMO_PWM_DISABLE_MAP_OP(kPWM_FaultDisable_0 | kPWM_FaultDisable_1 | kPWM_FaultDisable_2 | kPWM_FaultDisable_3));

    PWM_SetupFaultDisableMap(BOARD_PWM_BASEADDR, kPWM_Module_0, kPWM_PwmB, kPWM_faultchannel_0,
                             DEMO_PWM_DISABLE_MAP_OP(kPWM_FaultDisable_0 | kPWM_FaultDisable_1 | kPWM_FaultDisable_2 | kPWM_FaultDisable_3));
    PWM_SetupFaultDisableMap(BOARD_PWM_BASEADDR, kPWM_Module_1, kPWM_PwmB, kPWM_faultchannel_0,
                             DEMO_PWM_DISABLE_MAP_OP(kPWM_FaultDisable_0 | kPWM_FaultDisable_1 | kPWM_FaultDisable_2 | kPWM_FaultDisable_3));
    PWM_SetupFaultDisableMap(BOARD_PWM_BASEADDR, kPWM_Module_2, kPWM_PwmB, kPWM_faultchannel_0,
                             DEMO_PWM_DISABLE_MAP_OP(kPWM_FaultDisable_0 | kPWM_FaultDisable_1 | kPWM_FaultDisable_2 | kPWM_FaultDisable_3));

    PWM_SetupFaultDisableMap(BOARD_PWM_BASEADDR, kPWM_Module_0, kPWM_PwmX, kPWM_faultchannel_0,
                             DEMO_PWM_DISABLE_MAP_OP(kPWM_FaultDisable_0 | kPWM_FaultDisable_1 | kPWM_FaultDisable_2 | kPWM_FaultDisable_3));
    PWM_SetupFaultDisableMap(BOARD_PWM_BASEADDR, kPWM_Module_1, kPWM_PwmX, kPWM_faultchannel_0,
                             DEMO_PWM_DISABLE_MAP_OP(kPWM_FaultDisable_0 | kPWM_FaultDisable_1 | kPWM_FaultDisable_2 | kPWM_FaultDisable_3));
    PWM_SetupFaultDisableMap(BOARD_PWM_BASEADDR, kPWM_Module_2, kPWM_PwmX, kPWM_faultchannel_0,
                             DEMO_PWM_DISABLE_MAP_OP(kPWM_FaultDisable_0 | kPWM_FaultDisable_1 | kPWM_FaultDisable_2 | kPWM_FaultDisable_3));
}

void persistDimmerDutyCycles()
{
	return ;
}


