/*
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** ============================================================================
 *  @file       ADCBufMSP432.h
 *
 *  @brief      ADC driver implementation for a MSP432 analog-to-digital converter
 *
 * # Driver include #
 *  The ADC header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/ADCBuf.h>
 *  #include <ti/drivers/adc/ADCBufMSP432.h>
 *  @endcode
 *
 * # Overview #
 * This is a MSP432 specific implementation of the generic TI-RTOS ADC driver.
 * The generic ADC API specified in ti/drivers/ADCBuf.h should be called by the application.
 * The board file will define the device specific configuration, and casting in the general
 * API will ensure that the correct device specific functions are called. It is advised
 * to specify a ADCBufMSP432_ParamsExtension in the custom field of the ADCBuf_Params that suits
 * the application at hand. The default settings will work for many, but not all, usecases.
 *
 * # General Behavior #
 * The application should call ADCBuf_init() once by the application to set the isOpened
 * flag to false, indicating that the driver is ready to be used.
 *
 * The ADC driver is opened by calling ADCBuf_open() which in turn will
 * set up interrupts and configure the internal components of the driver.
 * However the ADC hardware or analog pins are not yet configured until the ADCBuf_convert() is
 * called.
 *
 * In order to perform an ADC conversion, the application should call
 * ADCBuf_convert(). This call will request the ADC resource, configure the ADC,
 * and perform the requested ADC conversion on the selected channel defined by the
 * ADCBuf_Conversion structure.
 *
 * # Error handling #
 * The following errors may occur when opening the ADC:
 * - The ADC handle is already open.
 *
 * The following errors may occur when requesing an ADC conversion:
 * - The ADC is currently already doing a conversion.
 * - When in ::ADCBuf_RETURN_MODE_BLOCKING, the timeout specified in the ADCBuf_Params was insufficient
 *   to make the necessary amount of conversions requested in the ADCBuf_Conversion settings.
 *
 * The following errors may occur when adjusting ADC results by calling ADCBuf_adjustRawValues():
 * - The ADC is currently runnning for the conversion. Usually it happens during the ADC
 *   continuous mode.
 *
 * # Power Management #
 * The power management framework will try to put the device into the most
 * power efficient mode whenever possible. Please see the technical reference
 * manual for further details on each power mode.
 *
 * The ADCBufMSP432 driver sets a power constraint during operation to keep
 * the device out of standby. When the operation has finished, the power
 * constraint is released.
 * The following statements are valid:
 *   - After ADCBuf_open(): the device cannot enter standby.
 *   - After ADCBuf_close(): the device can enter standby again.
 *
 *
 * # Supported Functions #
 * | API function               | Description                                          |
 * |----------------------------|------------------------------------------------------|
 * | ADCBuf_init()              | Initialize ADC driver                                |
 * | ADCBuf_open()              | Open the ADC driver and configure driver             |
 * | ADCBuf_convert()           | Perform ADC conversion                               |
 * | ADCBuf_convertCancel()     | Cancel ongoing ADC conversion                        |
 * | ADCBuf_close()             | Close ADC driver                                     |
 * | ADCBuf_Params_init()       | Initialise ADCBuf_Params structure to default values |
 * | ADCBuf_adjustRawValues()   | Adjust ADC raw values based on gain and offset       |
 * | ADCBuf_convertAdjustedToMicroVolts() | Convert adjusted values to micro volts unit|
 *
 *  # Not Supported Functionality #
 *  TBD
 *
 * # Use Cases #
 * ## Basic one-shot conversion #
 *  Perform one conversion on DIO30 in ::ADCBuf_RETURN_MODE_BLOCKING.
 *  @code
 *  // Code will go here when the API is stable
 *  // One shot and blocking mode
 *  ADCBuf_Params param = {0};
 *  param.returnMode = ADCBuf_RETURN_MODE_BLOCKING;
 *  param.recurrenceMode = ADCBuf_RECURRENCE_MODE_ONE_SHOT;
 *  param.samplingFrequency = 10000; //10K
 *  ADCBufHandle adc = ADCBuf_open(Board_ADCBuf0, &param);
 *
 *  ADCBuf_Conversion conversion = {0};
 *  conversion.samplesRequestedCount = 10;
 *  conversion.sampleBuf = adcBuf;
 *  conversion.adcChannel = Board_ADC0_CHANNEL_A0;
 *  ADCBuf_convert(adc, &conversion, 1);
 *
 *  //Recurrence and callback mode
 *  void myCallback(ADCBuf_Handle handle,
 *                  ADCBuf_Conversion *conversion,
                    void *activeADCBuffer)
 *  {
 *     //Process activeADCBuffer
 *     for (i=0; i<conversion->sampleCount; i++) {
 *        sum += activeADCBuffer[i];
 *     }
 *     avg = sum / conversion->sampleCount;
 *  }
 *  ADCBuf_Params param = {0};
 *  param.returnMode = ADCBuf_RETURN_MODE_CALLBACK;
 *  param.recurrenceMode = ADCBuf_RECURRENCE_MODE_CONTINUOUS;
 *  param.samplingFrequency = 10000;
 *  param.callbackFxn = myCallBack;
 *  ADCBufHandle adc = ADCBuf_open(Board_ADCBuf0, &param);
 *
 *  ADCBuf_Conversion conversion[2] = {0};
 *  conversion[0].samplesRequestedCount = 10;
 *  conversion[0].sampleBuf = adcBuf0;
 *  conversion[0].adcChannel = Board_ADC0_CHANNEL_A0;
 *  conversion[1].samplesRequestedCount = 10;
 *  conversion[1].sampleBuf = adcBuf1;
 *  conversion[1].adcChannel = Board_ADC0_CHANNEL_A1;
 *
 *  ADCBuf_convert(adc, &conversion, 2);
 *  @endcode
 *
 *
 *  # Instrumentation #
 *  The ADC driver interface produces log statements if instrumentation is
 *  enabled.
 *
 *  Diagnostics Mask | Log details                          |
 *  ---------------- | -----------                          |
 *  Diags_USER1      | basic ADC operations performed       |
 *  Diags_USER2      | detailed ADC operations performed    |
 *
 *  ============================================================================
 */

#ifndef ti_drivers_adc_adcbufmsp432__include
#define ti_drivers_adc_adcbufmsp432__include

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>


#include <ti/drivers/ADCBuf.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/dma/UDMAMSP432.h>

#include <ti/devices/msp432p4xx/driverlib/adc14.h>

/*
 *  ADCBuf port/pin defines for pin configuration.  Ports P2, P3, and P7 are
 *  configurable through the port mapping controller.  None of the port
 *  mappings support ADC.
 *  Channel specifies the ADC channel and ranges from 0 to 23.
 *  pin range: 0 - 7, port range: 0 - 15
 *
 *
 *    15 - 10    9   8  7 - 4  3 - 0
 *  ---------------------------------
 *  |  CHANNEL | X | X | PORT | PIN |
 *  ---------------------------------
 *
 *  channel = pinConfig >> 10
 *  port = (pinConfig >> 4) & 0xf
 *  pin = 1 << (pinConfig & 0xf)
 *  moduleFunction = (pinConfig >> 8) & 0x3
 *
 *  For non-configurable ports (bits 20 - 12 will be 0).
 *  Bits 8 and 9 hold the module function (PRIMARY, SECONDARY, or
 *  TERTIARY).  All ADC modes are TERTIARY, so bits 8 and 9
 *  are 1.
 *
 *  MAP_GPIO_setAsPeripheralModuleFunctionInputPin(port,
 *       pin, moduleFunction);
 *  or:
 *  MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(port,
 *       pin, moduleFunction);
 *
 */
/* Port 4 */
#define ADCBufMSP432_P4_0_A13 ((13 << 10) | 0x0340) /* ch 13, port 4, pin 0 */
#define ADCBufMSP432_P4_1_A12 ((12 << 10) | 0x0341) /* ch 12, port 4, pin 1 */
#define ADCBufMSP432_P4_2_A11 ((11 << 10) | 0x0342) /* ch 11, port 4, pin 2 */
#define ADCBufMSP432_P4_3_A10 ((10 << 10) | 0x0343) /* ch 10, port 4, pin 3 */
#define ADCBufMSP432_P4_4_A9  ((9 << 10) | 0x0344)  /* ch 9, port 4, pin 4 */
#define ADCBufMSP432_P4_5_A8  ((8 << 10) | 0x0345)  /* ch 8, port 4, pin 5 */
#define ADCBufMSP432_P4_6_A7  ((7 << 10) | 0x0346)  /* ch 7, port 4, pin 6 */
#define ADCBufMSP432_P4_7_A6  ((6 << 10) | 0x0347)  /* ch 6, port 4, pin 7 */

/* Port 5 */
#define ADCBufMSP432_P5_0_A5  ((5 << 10) | 0x0350)  /* ch 5, port 5, pin 0 */
#define ADCBufMSP432_P5_1_A4  ((4 << 10) | 0x0351)  /* ch 4, port 5, pin 1 */
#define ADCBufMSP432_P5_2_A3  ((3 << 10) | 0x0352)  /* ch 3, port 5, pin 2 */
#define ADCBufMSP432_P5_3_A2  ((2 << 10) | 0x0353)  /* ch 2, port 5, pin 3 */
#define ADCBufMSP432_P5_4_A1  ((1 << 10) | 0x0354)  /* ch 1, port 5, pin 4 */
#define ADCBufMSP432_P5_5_A0  ((0 << 10) | 0x0355)  /* ch 0, port 5, pin 5 */

/* Port 6 */
#define ADCBufMSP432_P6_0_A15 ((15 << 10) | 0x0360)  /* ch 15, port 6, pin 0 */
#define ADCBufMSP432_P6_1_A14 ((14 << 10) | 0x0361)  /* ch 14, port 6, pin 1 */

/* Port 8 */
#define ADCBufMSP432_P8_2_A23 ((23 << 10) | 0x0382)  /* ch 23, port 8, pin 2 */
#define ADCBufMSP432_P8_3_A22 ((22 << 10) | 0x0383)  /* ch 22, port 8, pin 3 */
#define ADCBufMSP432_P8_4_A21 ((21 << 10) | 0x0384)  /* ch 21, port 8, pin 4 */
#define ADCBufMSP432_P8_5_A20 ((20 << 10) | 0x0385)  /* ch 20, port 8, pin 5 */
#define ADCBufMSP432_P8_6_A19 ((19 << 10) | 0x0386)  /* ch 19, port 8, pin 6 */
#define ADCBufMSP432_P8_7_A18 ((18 << 10) | 0x0387)  /* ch 18, port 8, pin 7 */

/* Port 9 */
#define ADCBufMSP432_P9_0_A17 ((17 << 10) | 0x0390)  /* ch 17, port 9, pin 0 */
#define ADCBufMSP432_P9_1_A16 ((16 << 10) | 0x0391)  /* ch 16, port 9, pin 1 */

/*
 * =============================================================================
 * Constants
 * =============================================================================
 */

/* ADC function table pointer */
extern const ADCBuf_FxnTable ADCBufMSP432_fxnTable;

/*!
 *  @brief  ADCMSP432 reference source
 *  These fields are used by ADCBufMSP432_HWAttrs to specify the reference voltage
 *  for each channel.
 *
 */
typedef enum ADCBufMSP432_Reference_Source {
    ADCBufMSP432_VREFPOS_AVCC_VREFNEG_VSS = ADC_VREFPOS_AVCC_VREFNEG_VSS,
    ADCBufMSP432_VREFPOS_INTBUF_VREFNEG_VSS = ADC_VREFPOS_INTBUF_VREFNEG_VSS,
    ADCBufMSP432_VREFPOS_EXTPOS_VREFNEG_EXTNEG = ADC_VREFPOS_EXTPOS_VREFNEG_EXTNEG,
    ADCBufMSP432_VREFPOS_EXTBUF_VREFNEG_EXTNEG = ADC_VREFPOS_EXTBUF_VREFNEG_EXTNEG
}ADCBufMSP432_Reference_Source;

/*!
 *  @brief  ADCMSP432 sampling duration
 *  These fields define the MSP432 ADC sampling duration (sample and hold time)
 *  in the pulse width unit. User can specify the differnt sampling duration
 *  in the ADCBufMSP432_ParamsExtension when opening the ADC.
 *
 */
typedef enum ADCBufMSP432_Sampling_Duration {
    ADCBufMSP432_SAMPLING_DURATION_PULSE_WIDTH_4 = ADC_PULSE_WIDTH_4,
    ADCBufMSP432_SAMPLING_DURATION_PULSE_WIDTH_8 = ADC_PULSE_WIDTH_8,
    ADCBufMSP432_SAMPLING_DURATION_PULSE_WIDTH_16 = ADC_PULSE_WIDTH_16,
    ADCBufMSP432_SAMPLING_DURATION_PULSE_WIDTH_32 = ADC_PULSE_WIDTH_32,
    ADCBufMSP432_SAMPLING_DURATION_PULSE_WIDTH_64 = ADC_PULSE_WIDTH_64,
    ADCBufMSP432_SAMPLING_DURATION_PULSE_WIDTH_96 = ADC_PULSE_WIDTH_96,
    ADCBufMSP432_SAMPLING_DURATION_PULSE_WIDTH_128 = ADC_PULSE_WIDTH_128,
    ADCBufMSP432_SAMPLING_DURATION_PULSE_WIDTH_192 = ADC_PULSE_WIDTH_192
} ADCBufMSP432_Sampling_Duration;

/*!
 *  @brief  ADCMSP432 timer trigger source
 *  These fields are used by ADCBufMSP432_HWAttrs to specify the Timer Capture
 *  for each channel.
 *
 */
typedef enum ADCBufMSP432_Timer_Reference_Source {
    ADCBufMSP432_TIMERA0_CAPTURECOMPARE1 = 0,
    ADCBufMSP432_TIMERA0_CAPTURECOMPARE2 = 1,
    ADCBufMSP432_TIMERA1_CAPTURECOMPARE1 = 2,
    ADCBufMSP432_TIMERA1_CAPTURECOMPARE2 = 3,
    ADCBufMSP432_TIMERA2_CAPTURECOMPARE1 = 4,
    ADCBufMSP432_TIMERA2_CAPTURECOMPARE2 = 5,
    ADCBufMSP432_TIMERA3_CAPTURECOMPARE1 = 6
}ADCBufMSP432_Timer_Reference_Source;


/*!
 *  @brief      MSP432 specfic extension to ADCBuf_Params
 *
 *  To use non-default MSP432 specific parameters when calling ADCBuf_open(), a pointer
 *  to an instance of this struct must be specified in ADCBuf_Params::custom. Alternatively,
 *  these values can be set using the control function after calling ADCBuf_open().
 */
typedef struct ADCBufMSP432_ParamsExtension{
    ADCBufMSP432_Sampling_Duration samplingDuration;   /*! ADC sampling duration (sample&hold time), unit is pulse width */
} ADCBufMSP432_ParamsExtension;

/*!
 *  @brief  ADCMSP432 Channel setting
 *  These fields define channel-specific settings: GPIO and reference voltage. These settings
 *  happen when ADCBuf_convert() is called.
 *
 */
typedef struct ADCBufMSP432_Channels {
    uint_fast16_t  adcPin;     /*! ADC pin, port channel */
    ADCBufMSP432_Reference_Source refSource;           /*! ADC reference source */
    uint32_t refVoltage;                               /*! ADC reference voltage in microVolts (if it is internal, the value should be 1.5/2.5) */
} ADCBufMSP432_Channels;

/*!
 *  @brief  ADCBufMSP432 Hardware attributes
 *  These fields are populated by PinMux tool but user is allowed to change for
 *  different channels setting.
 *
 *  A sample structure is shown below:
 *  @code
 *  ADCBufMSP432_Channels adcBufMSP432Channels[] = {
 *      {
 *       .adcPin = ADCBufMSP432_P5_5_A0,
 *       .refSource = ADCBufMSP432_VREFPOS_INTBUF_VREFNEG_VSS,
 *       .refVoltage = 2500000
 *      }
 *  };
 *  const ADCBufMSP432_HWAttrs adcbufMSP432HWAttrs[] = {
 *  {
 *      .intPriority =  ~0,
 *      .channelSetting = adcBufMSP432Channels,
 *      .adcTimerTriggerSource = ADCBufMSP432_TIMERA0_CAPTURECOMPARE2

 *  }
};
 *  @endcode
 */
typedef struct ADCBufMSP432_HWAttrs {
    uint32_t intPriority;                 /*! ADC interrupt priority */
    ADCBufMSP432_Channels *channelSetting; /*! ADC channel specific setting */
    ADCBufMSP432_Timer_Reference_Source adcTimerTriggerSource;    /*! ADC Timer Trigger Source */

} ADCBufMSP432_HWAttrs;

/*!
 *  @brief  ADCBufMSP432 Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct ADCBufMSP432_Object {
    SemaphoreP_Handle mutex;                /* Grants exclusive access to ADC */
    SemaphoreP_Handle convertComplete;      /* Notify finished ADC convert */
    HwiP_Handle       hwiHandle;            /* Hardware interrupt handle */

    uint_fast8_t        pingpongFlag;          /* PingPong flag indicates which sample buffer is active in the conversion */
    uint_fast8_t        channelCount;          /* Count of sampling channels */
    ADCBuf_Conversion   *conversions;          /* Ptr to ADC conversion struct array passed by user */
    ADCBuf_Callback     callBackFxn;           /* Callback function pointer */
    uint_fast16_t       *intlResultBuf;        /* Internal result buffer */

    uint16_t            *conversionSampleBuf;       /* Ptr to current active sampling buffer */
    uint_fast16_t        conversionSampleIdx;        /* Internal dec. conversion buffer counter */
    uint_fast16_t        conversionSampleCount;      /* Total sampling count per channel */

    uint32_t                        semaphoreTimeout;           /*!< Timeout for read semaphore in ::ADCBuf_RETURN_MODE_BLOCKING */
    uint32_t                        samplingFrequency;          /*!< Frequency in Hz at which the ADC is triggered */
    ADCBuf_Recurrence_Mode          recurrenceMode;             /*!< Should we convert continuously or one-shot */
    ADCBuf_Return_Mode              returnMode;                 /*!< Mode for all conversions */
    ADCBufMSP432_Sampling_Duration  samplingDuration;           /*!< ADC sampling duration */

    uint32_t             timerAddr;            /* Timer base address for trigger source */

    bool                 isOpen;               /* To determine if the ADC is open */
} ADCBufMSP432_Object;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_adc_ADCBufMSP432__include */
