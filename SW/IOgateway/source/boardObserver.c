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
#include "command_handler.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static void AdcConfiguration (void);
static void LowPowerTimerADCTrigger_Init (void);
static void EDMA_Configuration (void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

volatile uint32_t DMACounter0 = 0U;
volatile uint32_t DMACounter1 = 0U;
volatile uint32_t DMACounter2 = 0U;
AT_NONCACHEABLE_SECTION_ALIGN_INIT(uint32_t destAddr[ADC_DMA_BUFFER_LENGTH][3], 32U) = {0x00U};
//AT_NONCACHEABLE_SECTION_ALIGN(static edma_tcd_t tcd[2], 32);
edma_handle_t g_edmaHandle;
//AT_QUICKACCESS_SECTION_DATA_ALIGN(edma_tcd_t tcdMemoryPoolPtr[TCD_QUEUE_SIZE], sizeof(edma_tcd_t));

static char gLogBuf[MAX_LOG_LENGTH + 1];
static float gBoardTemperatur = 0.0f;
static float gAdc1Value_V = 0.0f;
static float gAdc2Value_V = 0.0f;

float getBoardTemperatur () {
  return gBoardTemperatur;
}

float getAdc1ValueInV () {
  return gAdc1Value_V;
}

float getAdc2ValueInV () {
  return gAdc2Value_V;
}

/*
 void DMA_IRQ_HANDLER_CH0 (void) {
 if((EDMA_GetChannelStatusFlags(DMA_BASEADDR, DMA_CHANNEL_0)
 & kEDMA_InterruptFlag) != 0U) {
 DMACounter0++;
 EDMA_ClearChannelStatusFlags(DMA_BASEADDR, DMA_CHANNEL_0,
 kEDMA_InterruptFlag);
 EDMA_EnableChannelRequest(DMA_BASEADDR, DMA_CHANNEL_0);
 }
 }
 */

static void boardObsreverWorker (void *pvParameters) {
  int logCnt = 0;
  while(1) {
    int32_t tempMeanValue = 0;
    int32_t adc1MeanValue = 0;
    int32_t adc2MeanValue = 0;
#if ADC_DMA_BUFFER_LENGTH > 1
    for(int i = 0; i < ADC_DMA_BUFFER_LENGTH; ++i) {
      tempMeanValue += ((destAddr[i][LPADC_TEMP_CMDID - 1] & LPADC_RESULT_MASK)
          >> LPADC_RESULTSHIFT);
      adc1MeanValue += ((destAddr[i][LPADC_ADC1_CMDID - 1] & LPADC_RESULT_MASK)
                >> LPADC_RESULTSHIFT);
      adc2MeanValue += ((destAddr[i][LPADC_ADC2_CMDID - 1] & LPADC_RESULT_MASK)
                >> LPADC_RESULTSHIFT);
    }
    tempMeanValue >>= ADC_DMA_BUFFER_SHIFT;
    adc1MeanValue >>= ADC_DMA_BUFFER_SHIFT;
    adc2MeanValue >>= ADC_DMA_BUFFER_SHIFT;
#else
    tempMeanValue = ((destAddr[0][LPADC_TEMP_CMDID - 1] & LPADC_RESULT_MASK) >> LPADC_RESULTSHIFT);
    adc2MeanValue = ((destAddr[0][LPADC_ADC1_CMDID - 1] & LPADC_RESULT_MASK) >> LPADC_RESULTSHIFT);
    adc1MeanValue = ((destAddr[0][LPADC_ADC2_CMDID - 1] & LPADC_RESULT_MASK) >> LPADC_RESULTSHIFT);
#endif
    float boardTemperature = (((float)tempMeanValue) * 3.3f) / LPADC_FULLRANGE;
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

    float adc1_voltage = (((float)adc1MeanValue) * 3.3f) / LPADC_FULLRANGE;
    gAdc1Value_V = adc1_voltage;

    float adc2_voltage = (((float)adc2MeanValue) * 3.3f) / LPADC_FULLRANGE;
    gAdc2Value_V = adc2_voltage;

    if(++logCnt >= 20) {
      logCnt = 0;
      int len = snprintf(gLogBuf, MAX_LOG_LENGTH,
                         "ADC CH0: Temp: %3ld.%02ld C\n\r",
                         (int32_t)boardTemperature,
                         (((int32_t)(boardTemperature * 100.0f)) % 100));
      LOG(gLogBuf, len);
      len = snprintf(gLogBuf, MAX_LOG_LENGTH, "ADC1: %3ld.%02ld V\n\r",
                     (int32_t)adc1_voltage,
                     (((int32_t)(adc1_voltage * 100.0f)) % 100));
      LOG(gLogBuf, len);
      len = snprintf(gLogBuf, MAX_LOG_LENGTH, "ADC2: %3ld.%02ld V\n\r",
                     (int32_t)adc2_voltage,
                     (((int32_t)(adc2_voltage * 100.0f)) % 100));
      LOG(gLogBuf, len);
    }

#ifdef ADC_TYPE_A1_IS_SCHMITT_TRIGGER
    static float adc1_voltage_R = 0, adc1_voltage_RR;
    // 2 Values are continuously higher than threshold and was lower than threshold before
    if(adc1_voltage_RR < ADC_A1_UPPER_THRESHOLD_V
        && adc1_voltage_R > ADC_A1_UPPER_THRESHOLD_V
        && adc1_voltage > ADC_A1_UPPER_THRESHOLD_V) {
      gLogBuf[0] = CMD_ADC1_SCHMITT_TRIGGER;
      (void) snprintf(gLogBuf + 1, MAX_LOG_LENGTH - 1, "ADC1_TRG: 1\n\r");
      uartTxQueueAdd(gLogBuf);
    }
    // 2 Values are continuously lower than threshold and was higher than threshold before
    if(adc1_voltage_RR > ADC_A1_LOWER_THRESHOLD_V
        && adc1_voltage_R < ADC_A1_LOWER_THRESHOLD_V
        && adc1_voltage < ADC_A1_LOWER_THRESHOLD_V) {
      gLogBuf[0] = CMD_ADC1_SCHMITT_TRIGGER;
      (void) snprintf(gLogBuf + 1, MAX_LOG_LENGTH - 1, "ADC1_TRG: 0\n\r");
      uartTxQueueAdd(gLogBuf);
    }
    adc1_voltage_RR = adc1_voltage_R;
    adc1_voltage_R = adc1_voltage;
#endif

#ifdef ADC_TYPE_A2_IS_SCHMITT_TRIGGER
    static float adc2_voltage_R = 0, adc2_voltage_RR;
    // 2 Values are continuously higher than threshold and was lower than threshold before
    if(adc2_voltage_RR < ADC_A2_UPPER_THRESHOLD_V
        && adc2_voltage_R > ADC_A2_UPPER_THRESHOLD_V
        && adc2_voltage > ADC_A2_UPPER_THRESHOLD_V) {
      gLogBuf[0] = CMD_ADC2_SCHMITT_TRIGGER;
      (void) snprintf(gLogBuf + 1, MAX_LOG_LENGTH - 1, "ADC2_TRG: 1\n\r");
      uartTxQueueAdd(gLogBuf);
    }
    // 2 Values are continuously lower than threshold and was higher than threshold before
    if(adc2_voltage_RR > ADC_A2_LOWER_THRESHOLD_V
        && adc2_voltage_R < ADC_A2_LOWER_THRESHOLD_V
        && adc2_voltage < ADC_A2_LOWER_THRESHOLD_V) {
      gLogBuf[0] = CMD_ADC2_SCHMITT_TRIGGER;
      (void) snprintf(gLogBuf + 1, MAX_LOG_LENGTH - 1, "ADC2_TRG: 0\n\r");
      uartTxQueueAdd(gLogBuf);
    }
    adc2_voltage_RR = adc2_voltage_R;
    adc2_voltage_R = adc2_voltage;
#endif

    vTaskDelay(pdMS_TO_TICKS(100));
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

void EDMA_Callback (edma_handle_t *handle, void *param, bool transferDone,
                    uint32_t tcds) {
  DMACounter0++;
  EDMA_ClearChannelStatusFlags(DMA_BASEADDR, DMA_CHANNEL_0,
                               kEDMA_InterruptFlag);
  EDMA_EnableChannelRequest(DMA_BASEADDR, DMA_CHANNEL_0);
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
  EDMA_CreateHandle(&g_edmaHandle, DMA_BASEADDR, DMA_CHANNEL_0);
  EDMA_SetCallback(&g_edmaHandle, EDMA_Callback, NULL);
  EDMA_ResetChannel(g_edmaHandle.base, g_edmaHandle.channel);

  //EDMA_InstallTCDMemory(&g_EDMA_Handle, tcdMemoryPoolPtr, TCD_QUEUE_SIZE);

  void *srcAddr = (uint32_t*)&(LPADC_BASE->RESFIFO);
  EDMA_PrepareTransfer(&transferConfig,
                       srcAddr, sizeof(uint32_t),
                       destAddr, sizeof(uint32_t),
                       sizeof(uint32_t), // Bytes Each Request
                       sizeof(destAddr),
                       kEDMA_PeripheralToMemory);


  transferConfig.dstMajorLoopOffset = (-1) * ((int32_t)sizeof(destAddr));
  EDMA_SetTransferConfig(DMA_BASEADDR, DMA_CHANNEL_0, &transferConfig, NULL);
  EDMA_InitChannel(DMA_BASEADDR, DMA_CHANNEL_0, &lpadcDmaChnlConfig);
  EDMA_EnableAutoStopRequest(DMA_BASEADDR, DMA_CHANNEL_0, false); // DMA is automatically reenabled
  EDMA_EnableChannelRequest(DMA_BASEADDR, DMA_CHANNEL_0);

  /*
   EDMA_PrepareTransferTCD(&g_edmaHandle,
   &tcd[0],
   srcAddr,
   sizeof(uint32_t),
   0,
   destAddr,
   sizeof(uint32_t),
   sizeof(uint32_t),
   sizeof(uint32_t),
   ((int32_t)sizeof(destAddr)),
   &tcd[0]);

   EDMA_SubmitTransferTCD(&g_edmaHandle, &tcd[0]);
   EDMA_StartTransfer(&g_edmaHandle);
   */
}

/*!
 * @brief ADC configuration
 */
static void AdcConfiguration (void) {
  lpadc_config_t lpadcConfigStruct;
  lpadc_conv_trigger_config_t lpadcTriggerConfigStruct;
  lpadc_conv_command_config_t lpadcCommandConfigStruct;

  lpadc_config_t ADC0_config = {
    .enableInDozeMode = true,
    .conversionAverageMode = kLPADC_ConversionAverage128,
    .enableAnalogPreliminary = true,
    .powerUpDelay = 0x80UL,
    .referenceVoltageSource = kLPADC_ReferenceVoltageAlt3,
    .powerLevelMode = kLPADC_PowerLevelAlt4,
    .triggerPriorityPolicy = kLPADC_ConvPreemptImmediatelyNotAutoResumed,
    .enableConvPause = false,
    .convPauseDelay = 0UL,
    .FIFOWatermark = 0UL,
  };
  lpadc_conv_command_config_t ADC0_commandsConfig[3] = {
    {
      .sampleChannelMode = kLPADC_SampleChannelSingleEndSideA,
      .channelNumber = LPADC_TEMP_CHANNEL,
      .chainedNextCommandNumber = LPADC_ADC1_CMDID,
      .enableAutoChannelIncrement = false,
      .loopCount = 0UL,
      .hardwareAverageMode = kLPADC_HardwareAverageCount128,
      .sampleTimeMode = kLPADC_SampleTimeADCK3,
      .hardwareCompareMode = kLPADC_HardwareCompareDisabled,
      .hardwareCompareValueHigh = 0UL,
      .hardwareCompareValueLow = 0UL,
      .conversionResolutionMode = kLPADC_ConversionResolutionHigh,
      .enableWaitTrigger = false
    },
    {
      .sampleChannelMode = kLPADC_SampleChannelSingleEndSideA,
      .channelNumber = LPADC_ADC1_CHANNEL,
      .chainedNextCommandNumber = LPADC_ADC2_CMDID,
      .enableAutoChannelIncrement = false,
      .loopCount = 0UL,
      .hardwareAverageMode = kLPADC_HardwareAverageCount128,
      .sampleTimeMode = kLPADC_SampleTimeADCK3,
      .hardwareCompareMode = kLPADC_HardwareCompareDisabled,
      .hardwareCompareValueHigh = 0UL,
      .hardwareCompareValueLow = 0UL,
      .conversionResolutionMode = kLPADC_ConversionResolutionHigh,
      .enableWaitTrigger = false
    },
    {
      .sampleChannelMode = kLPADC_SampleChannelSingleEndSideA,
      .channelNumber = LPADC_ADC2_CHANNEL,
      .chainedNextCommandNumber = 0,
      .enableAutoChannelIncrement = false,
      .loopCount = 0UL,
      .hardwareAverageMode = kLPADC_HardwareAverageCount128,
      .sampleTimeMode = kLPADC_SampleTimeADCK3,
      .hardwareCompareMode = kLPADC_HardwareCompareDisabled,
      .hardwareCompareValueHigh = 0UL,
      .hardwareCompareValueLow = 0UL,
      .conversionResolutionMode = kLPADC_ConversionResolutionHigh,
      .enableWaitTrigger = false
    }
  };
  lpadc_conv_trigger_config_t ADC0_triggersConfig[1] = {
    {
      .targetCommandId = LPADC_TEMP_CMDID,
      .delayPower = 0UL,
      .priority = 0,
      .enableHardwareTrigger = true
    }
  };

  LPADC_Init(LPADC_BASE, &lpadcConfigStruct);
  LPADC_DoOffsetCalibration(LPADC_BASE);
  LPADC_DoAutoCalibration(LPADC_BASE);
  LPADC_EnableFIFOWatermarkDMA(LPADC_BASE, true);
  LPADC_SetConvCommandConfig(LPADC_BASE, LPADC_TEMP_CMDID, &ADC0_commandsConfig[0]);
  LPADC_SetConvCommandConfig(LPADC_BASE, LPADC_ADC1_CMDID, &ADC0_commandsConfig[1]);
  LPADC_SetConvCommandConfig(LPADC_BASE, LPADC_ADC2_CMDID, &ADC0_commandsConfig[2]);
  LPADC_SetConvTriggerConfig(LPADC_BASE, ADC0_ADC_TIMERTRIG, &ADC0_triggersConfig[0]);

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
