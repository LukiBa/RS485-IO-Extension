/*
 * boardObserver.c
 *
 *  Created on: 18.02.2026
 *      Author: lukas
 */
#include <stdint.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "fsl_port.h"
#include "fsl_gpio.h"
#include "fsl_lpadc.h"
#include "fsl_edma.h"
#include "fsl_lptmr.h"

#include "pin_mux.h"
#include "app.h"

#include "IO_Gateway_Config.h"
#include "rs485_handler.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void AdcConfiguration (void);
static void LowPowerTimerADCTrigger_Init (void);
static void EDMA_Configuration (void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

volatile uint32_t DMACounter = 0U;
AT_NONCACHEABLE_SECTION_ALIGN_INIT(uint32_t destAddr[ADC_DMA_BUFFER_LENGTH], 32U) = {0x00U};
static char gLogBuf[MAX_LOG_LENGTH + 1];
static float gBoardTemperatur = 0.0f;

float getBoardTemperatur () {
  return gBoardTemperatur;
}

void DMA_IRQ_HANDLER (void) {
  if((EDMA_GetChannelStatusFlags(DMA_BASEADDR, DMA_CHANNEL_0)
      & kEDMA_InterruptFlag) != 0U) {
    DMACounter++;
    EDMA_ClearChannelStatusFlags(DMA_BASEADDR, DMA_CHANNEL_0,
                                 kEDMA_InterruptFlag);
    EDMA_EnableChannelRequest(DMA_BASEADDR, DMA_CHANNEL_0);
  }
}

static void boardObsreverWorker (void *pvParameters) {
  while(1) {
    int32_t adcMeanValue = 0;
    for(int i = 0; i < ADC_DMA_BUFFER_LENGTH; ++i) {
      adcMeanValue += ((destAddr[i] & LPADC_RESULT_MASK) >> LPADC_RESULTSHIFT);
    }
    adcMeanValue >>= ADC_DMA_BUFFER_SHIFT;
    float boardTemperature = (((float)adcMeanValue) * 3.3f) / LPADC_FULLRANGE;
    boardTemperature -= MCP9700_0degC_OFFSET;
    boardTemperature *= MCP9700_TEMP_COEFF_FACTOR;
    gBoardTemperatur = boardTemperature;
    if(boardTemperature >= FAN_TEMP_THRESHOLD) {
      GPIO_PinWrite(BOARD_INITPINS_FAN_Out_GPIO,
      BOARD_INITPINS_FAN_Out_GPIO_PIN,
                    1u);
    } else if(boardTemperature < (FAN_TEMP_THRESHOLD - FAN_TEMP_HYSTERESIS)) {
      GPIO_PinWrite(BOARD_INITPINS_FAN_Out_GPIO,
      BOARD_INITPINS_FAN_Out_GPIO_PIN,
                    0u);
    }
    int len = snprintf(gLogBuf, MAX_LOG_LENGTH,
                       "ADC: %08ld, Temp: %3ld.%02ld C\n\r", adcMeanValue,
                       (int32_t)boardTemperature,
                       (((int32_t)(boardTemperature * 100.0f)) % 100));
    LOG(gLogBuf, len);

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void boardObserverInit () {
  gpio_pin_config_t inh_config = {kGPIO_DigitalOutput, 0u, };
  GPIO_PinInit(BOARD_INITPINS_FAN_Out_GPIO, BOARD_INITPINS_FAN_Out_GPIO_PIN,
               &inh_config);
  LowPowerTimerADCTrigger_Init();
  AdcConfiguration();
  EDMA_Configuration();
  LPTMR_StartTimer(LPTMR_TRIG_BASE);

  configASSERT(
      xTaskCreate(boardObsreverWorker, "boardObsreverWorker", BOARD_OBSERVER_TASK_STACK_SIZE, NULL, BOARD_OBSERVER_TASK_PRIO, NULL) == pdPASS);

}

static void EDMA_Configuration (void) {
  edma_transfer_config_t transferConfig;
  edma_channel_config_t lpadcDmaChnlConfig;
  edma_config_t userConfig;

  lpadcDmaChnlConfig.channelDataSignExtensionBitPosition = 0U;
  lpadcDmaChnlConfig.channelPreemptionConfig.enableChannelPreemption = false;
  lpadcDmaChnlConfig.channelPreemptionConfig.enablePreemptAbility = true;
  lpadcDmaChnlConfig.channelRequestSource = ADC_DMA_REQUEST;
  lpadcDmaChnlConfig.protectionLevel = kEDMA_ChannelProtectionLevelUser;

  /* Configure EDMA channel for one shot transfer */
  EDMA_GetDefaultConfig(&userConfig);
  EDMA_Init(DMA_BASEADDR, &userConfig);

  void *srcAddr = (uint32_t*)&(LPADC_BASE->RESFIFO);
  EDMA_PrepareTransfer(&transferConfig, srcAddr, sizeof(uint32_t), destAddr,
                       sizeof(destAddr[0]), sizeof(destAddr[0]),
                       sizeof(destAddr), kEDMA_PeripheralToMemory);

  /* Used to change the destination address to the original value */
  transferConfig.dstMajorLoopOffset = (int32_t)((-1) * sizeof(destAddr));

  EDMA_SetTransferConfig(DMA_BASEADDR, DMA_CHANNEL_0, &transferConfig, NULL);
  EDMA_InitChannel(DMA_BASEADDR, DMA_CHANNEL_0, &lpadcDmaChnlConfig);

  EnableIRQ(DMA_IRQ);
  EDMA_EnableChannelRequest(DMA_BASEADDR, DMA_CHANNEL_0);
}

/*!
 * @brief ADC configuration
 */
static void AdcConfiguration (void) {
  lpadc_config_t lpadcConfigStruct;
  lpadc_conv_trigger_config_t lpadcTriggerConfigStruct;
  lpadc_conv_command_config_t lpadcCommandConfigStruct;

  /* Sets the converter configuration structure with an available settings.
   * code
   *   config->enableInDozeMode        = true;
   *   config->conversionAverageMode   = kLPADC_ConversionAverage1;
   *   config->enableAnalogPreliminary = true;
   *   config->powerUpDelay            = 0x10;
   *   config->referenceVoltageSource  = kLPADC_ReferenceVoltageAlt3;
   *   config->powerLevelMode          = kLPADC_PowerLevelAlt1;
   *   config->triggerPriorityPolicy   = kLPADC_TriggerPriorityPreemptImmediately;
   *   config->enableConvPause         = false;
   *   config->convPauseDelay          = 0U;
   *   config->FIFO0Watermark          = 0U;
   *   config->FIFO1Watermark          = 0U;
   *   config->FIFOWatermark           = 0U;
   * endcode
   */
  LPADC_GetDefaultConfig(&lpadcConfigStruct);
  /* Set to highest power level here, users need to properly match ADC clock and power level according
   * to application requirements. For specific correspondence, please refer to the data sheet. */
  lpadcConfigStruct.powerLevelMode = kLPADC_PowerLevelAlt4;
  lpadcConfigStruct.referenceVoltageSource = kLPADC_ReferenceVoltageAlt3; // use VDD_ANA supply Pin --> 3.3V
  lpadcConfigStruct.enableAnalogPreliminary = true;
  lpadcConfigStruct.powerUpDelay = 0x10U;
  lpadcConfigStruct.conversionAverageMode = kLPADC_ConversionAverage128;
  lpadcConfigStruct.enableInDozeMode = true;
  LPADC_Init(LPADC_BASE, &lpadcConfigStruct);
  LPADC_DoOffsetCalibration(LPADC_BASE);
  LPADC_DoAutoCalibration(LPADC_BASE);

  /* Sets the conversion command's configuration structure with an available settings.
   * code
   *   config->sampleScaleMode            = kLPADC_SampleFullScale;
   *   config->channelBScaleMode          = kLPADC_SampleFullScale;
   *   config->channelSampleMode          = kLPADC_SampleChannelSingleEndSideA;
   *   config->channelNumber              = 2U;
   *   config->alternateChannelNumber     = 0U;
   *   config->chainedNextCmdNumber       = 0U;
   *   config->enableAutoChannelIncrement = false;
   *   config->loopCount                  = 0U;
   *   config->hardwareAverageMode        = kLPADC_HardwareAverageCount1;
   *   config->sampleTimeMode             = kLPADC_SampleTimeADCK3;
   *   config->hardwareCompareMode        = kLPADC_HardwareCompareDisabled;
   *   config->hardwareCompareValueHigh   = 0U;
   *   config->hardwareCompareValueLow    = 0U;
   *   config->conversionResolutionMode   = kLPADC_ConversionResolutionStandard;
   *   config->enableWaitTrigger          = false;
   *   config->enableChannelB             = false;
   * endcode
   */
  LPADC_GetDefaultConvCommandConfig(&lpadcCommandConfigStruct);
  lpadcCommandConfigStruct.channelNumber = LPADC_USER_CHANNEL;
  lpadcCommandConfigStruct.conversionResolutionMode =
      kLPADC_ConversionResolutionHigh;
  LPADC_SetConvCommandConfig(LPADC_BASE, LPADC_USER_CMDID,
                             &lpadcCommandConfigStruct);

  /* Sets the trigger's configuration structure with an available settings.
   *   config->targetCommandId       = 1U;
   *   config->delayPower            = 0U;
   *   config->priority              = 0U;
   *   config->enableHardwareTrigger = true;
   *   config->channelAFIFOSelect    = 0U;
   *   config->channelBFIFOSelect    = 0U;
   * endcode
   */
  LPADC_GetDefaultConvTriggerConfig(&lpadcTriggerConfigStruct);
  lpadcTriggerConfigStruct.targetCommandId = LPADC_USER_CMDID; /* CMD1 is executed. */

  /* Enable the hardware trigger function in the ADC block */
  lpadcTriggerConfigStruct.enableHardwareTrigger = true;

  /* Configured the trigger0. */
  LPADC_SetConvTriggerConfig(LPADC_BASE, 0U, &lpadcTriggerConfigStruct);

  /* Enable the watermark DMA in the ADC block */
  LPADC_EnableFIFOWatermarkDMA(LPADC_BASE, true);

}

/*!
 * @brief LPTMR configuration
 */
static void LowPowerTimerADCTrigger_Init (void) {
  lptmr_config_t lptmrConfig;

  /* Configure LPTMR */
  /*
   * Default configuration:
   *
   * lptmrConfig.timerMode = kLPTMR_TimerModeTimeCounter;
   * lptmrConfig.pinSelect = kLPTMR_PinSelectInput_0;
   * lptmrConfig.pinPolarity = kLPTMR_PinPolarityActiveHigh;
   * lptmrConfig.enableFreeRunning = false;
   * lptmrConfig.bypassPrescaler = true;
   * lptmrConfig.prescalerClockSource = kLPTMR_PrescalerClock_1;
   * lptmrConfig.value = kLPTMR_Prescale_Glitch_0;
   */
  LPTMR_GetDefaultConfig(&lptmrConfig);
  lptmrConfig.prescalerClockSource = ADC_LPTMR_TRIG_CLOCK;

  /* Initialize the LPTMR */
  LPTMR_Init(LPTMR_TRIG_BASE, &lptmrConfig);

  /*
   * Set timer period.
   * Note : the parameter "ticks" of LPTMR_SetTimerPeriod should be equal or greater than 1.
   */
  LPTMR_SetTimerPeriod(
      LPTMR_TRIG_BASE,
      USEC_TO_COUNT(LPTMR_TRIG_USEC_COUNT, LPTMR_TRIG_SOURCE_CLOCK));
}
