/*
 * boardObserver.c
 *
 *  Created on: 18.02.2026
 *      Author: lukas
 */

#include "FreeRTOS.h"
#include "task.h"

#include "fsl_lpadc.h"
#include "app.h"

#include "IO_Gateway_Config.h"
#include "rs485_handler.h"

lpadc_conv_command_config_t g_LpadcCommandConfigStruct; /* Structure to configure conversion command. */
volatile bool g_LpadcConversionCompletedFlag = false;
float g_CurrentTemperature                   = 0.0f;
const uint32_t g_LpadcFullRange = 4096U;

float measureOnbaordTemperature(ADC_Type *base, uint32_t commandId, uint32_t index)
{
    lpadc_conv_result_t convResultStruct;
    uint16_t Vbe1            = 0U;
    uint16_t Vbe8            = 0U;
    uint32_t convResultShift = 0U;
    float parameterSlope     = ADC_TEMP_PARAMETER_A;
    float parameterOffset    = ADC_TEMP_PARAMETER_B;
    float parameterAlpha     = ADC_TEMP_PARAMETER_ALPHA;
    float temperature        = -273.15f; /* Absolute zero degree as the incorrect return value. */

    /* Read the 2 temperature sensor result. */
    if (true == LPADC_GetConvResult(base, &convResultStruct))
    {
        Vbe1 = convResultStruct.convValue >> convResultShift;
        if (true == LPADC_GetConvResult(base, &convResultStruct))
        {
            Vbe8 = convResultStruct.convValue >> convResultShift;
            /* Final temperature = A*[alpha*(Vbe8-Vbe1)/(Vbe8 + alpha*(Vbe8-Vbe1))] - B. */
            temperature = parameterSlope * (parameterAlpha * ((float)Vbe8 - (float)Vbe1) /
                                            ((float)Vbe8 + parameterAlpha * ((float)Vbe8 - (float)Vbe1))) -
                          parameterOffset;
        }
    }

    return temperature;
}

void ADC_IRQ_HANDLER_FUNC(void)
{
    g_CurrentTemperature           = measureOnbaordTemperature(ADC_BASE, ADC_USER_CMDID, 0U);
    g_LpadcConversionCompletedFlag = true;
    SDK_ISR_EXIT_BARRIER;
}

static void ADC_Configuration(void)
{
    lpadc_config_t lpadcConfigStruct;
    lpadc_conv_trigger_config_t lpadcTriggerConfigStruct;

    /* Init ADC peripheral. */
    LPADC_GetDefaultConfig(&lpadcConfigStruct);
    lpadcConfigStruct.enableAnalogPreliminary = true;
    LPADC_Init(ADC_BASE, &lpadcConfigStruct);
    LPADC_DoResetFIFO(ADC_BASE);

    /* Set conversion CMD configuration. */
    LPADC_GetDefaultConvCommandConfig(&g_LpadcCommandConfigStruct);
    g_LpadcCommandConfigStruct.channelNumber       = ADC_TEMP_SENS_CHANNEL;
    g_LpadcCommandConfigStruct.sampleChannelMode   = ADC_SAMPLE_CHANNEL_MODE;
    g_LpadcCommandConfigStruct.sampleTimeMode      = kLPADC_SampleTimeADCK131;
    g_LpadcCommandConfigStruct.hardwareAverageMode = ADC_HARDWARE_AVERAGE;

    LPADC_SetConvCommandConfig(ADC_BASE, ADC_USER_CMDID, &g_LpadcCommandConfigStruct);

    /* Set trigger configuration. */
    LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfigStruct);
    lpadcTriggerConfigStruct.targetCommandId = ADC_USER_CMDID;
    LPADC_SetConvTriggerConfig(ADC_BASE, 0U, &lpadcTriggerConfigStruct); /* Configurate the trigger0. */


    LPADC_EnableInterrupts(ADC_BASE, kLPADC_FIFOWatermarkInterruptEnable);
    EnableIRQ(ADC_IRQn);

    /* Eliminate the first two inaccurate results. */
    LPADC_DoSoftwareTrigger(ADC_BASE, 1U); /* 1U is trigger0 mask. */
    while (false == g_LpadcConversionCompletedFlag)
    {
    }
}


static void boardObsreverWorker(void *pvParameters)
{
	while(1)
	{

		g_LpadcConversionCompletedFlag = false;
		LPADC_DoSoftwareTrigger(ADC_BASE, 1U);
		while (false == g_LpadcConversionCompletedFlag)
		{
			taskYIELD();
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void boardObserverInit(uint32_t queue_length, uint32_t commandLength)
{
    configASSERT(xTaskCreate(boardObsreverWorker, "boardObsreverWorker",
    		BOARD_OBSERVER_TASK_STACK_SIZE, NULL,
			BOARD_OBSERVER_TASK_PRIO, NULL) == pdPASS);

}
