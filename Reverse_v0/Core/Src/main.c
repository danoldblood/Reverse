/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "AK4621EF.h"
#include "M24C64.h"
//#include "delay.h"
//#include "pitch.h"
#include "allpass.h"
#include "delay_parting_16bit.h"
#include "delay_clocked.h"
#include "switchDB.h"
#include "pot.h"
#include "MIDI.h"
#include "statevariable.h"
#include "preset.h"
#include "led.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SAMPLERATE 48005.0f

#define SAIBUFFERSIZEDOUBLE 8//256
#define SAIBUFFERSIZE 2//128
#define SAIBUFFERSIZEHALF 1//64
#define ADCBUFFERSIZE 	11
#define INPUTBUFFERSIZE	4
#define MIDISTORAGEBUFFERSIZE 248
#define MIDIINPUTBUFFERSIZE 2
#define TIMESMOOTHINGCOEFF 1.0f
#define GROWTHSMOOTHINGCOEFF .005f
#define LPFCOEFF	.15f
//#define DELAYBUFFERSIZE 72172
//#define DELAYBUFFERSIZEF 72172.0f
//#define MAXDELAYTIMEMILLIS ((DELAYBUFFERSIZEF / SAMPLERATE) * 1000.0f)
#define CHORUSBUFFERSIZE 2000
#define SINSIZE 1024
#define SINSIZEF 1024.0f
#define APSIZE1L 96 //allpass sizes
#define APSIZE2L 529
#define APSIZE3L 1203
#define APSIZECHORUSL 7217
#define APSIZE1R 107 //allpass sizes
#define APSIZE2R 444
#define APSIZE3R 1298
#define APSIZECHORUSR 6666
#define APK .67f //allpass coefficient
#define APKEXTRA .42f

#define FSDBCUTOFF 20
#define FSHOLDCUTOFF 333
#define FSLONGHOLDCUTOFF 4200// //call it five seconds but make it a little shorter
#define TOGGLEUP 2
#define TOGGLEMIDDLE 3
#define TOGGLEDOWN 1

#define POTMOVEMENTCUTOFF 50

#define INT16_TO_FLOAT (1.0f / 32768.0f)
#define INT24_TO_FLOAT (1.0f / 8388607.0f)
#define INT32_TO_FLOAT (1.0f / 2147483648.0f)
#define UINT32_TO_FLOAT (1.0f / 4294967295.0f)
#define FLOAT_TO_INT16 32767.0f
#define FLOAT_TO_INT24 8388607.0f
#define FLOAT_TO_INT32 2147483648.0f
#define FLOAT_TO_UINT32 4294967295.0f
#define ADC_TO_FLOAT 1.0f / (1023.0f) //this is a divider to give 0-1 values across the ADC range
#define MIDI_TO_FLOAT 1.0f/127.0f
#define UINT8_TO_FLOAT 1.0f / 255.0f
#define FLOAT_TO_UINT8 255.0f

#define BITMASK25 0b11111111111111111111111110000000
#define BITMASK24 0b11111111111111111111111100000000
#define BITMASK23 0b11111111111111111111111000000000
#define BITMASK22 0b11111111111111111111110000000000
#define BITMASK21 0b11111111111111111111100000000000
#define BITMASK20 0b11111111111111111111000000000000
#define BITMASK19 0b11111111111111111110000000000000
#define BITMASK18 0b11111111111111111100000000000000
#define BITMASK17 0b11111111111111111000000000000000
#define BITMASK16 0b11111111111111110000000000000000
#define BITMASK15 0b11111111111111100000000000000000
#define BITMASK14 0b11111111111111000000000000000000
#define BITMASK13 0b11111111111110000000000000000000
#define BITMASK12 0b11111111111100000000000000000000
#define BITMASK11 0b11111111111000000000000000000000
#define BITMASK10 0b11111111110000000000000000000000
#define BITMASK09 0b11111111100000000000000000000000
#define BITMASK08 0b11111111000000000000000000000000
#define BITMASK07 0b11111110000000000000000000000000
#define BITMASK06 0b11111100000000000000000000000000
#define BITMASK05 0b11111000000000000000000000000000
#define BITMASK04 0b11110000000000000000000000000000
#define BITMASK03 0b11100000000000000000000000000000
#define BITMASK02 0b11000000000000000000000000000000
#define BITMASK01 0b10000000000000000000000000000000

#define FLOAT_TO_32BIT 2147483648.0f
#define FLOAT_TO_31BIT 1073741824.0f
#define FLOAT_TO_30BIT 536870912.0f
#define FLOAT_TO_29BIT 268435456.0f
#define FLOAT_TO_28BIT 134217728.0f
#define FLOAT_TO_27BIT 67108864.0f
#define FLOAT_TO_26BIT 33554432.0f
#define FLOAT_TO_25BIT 16777216.0f
#define FLOAT_TO_24BIT 8388608.0f
#define FLOAT_TO_23BIT 4194304.0f
#define FLOAT_TO_22BIT 2097152.0f
#define FLOAT_TO_21BIT 1048576.0f
#define FLOAT_TO_20BIT 524288.0f
#define FLOAT_TO_19BIT 262144.0f
#define FLOAT_TO_18BIT 131072.0f
#define FLOAT_TO_17BIT 65536.0f
#define FLOAT_TO_16BIT 32768.0f
#define FLOAT_TO_15BIT 16384.0f
#define FLOAT_TO_14BIT 8192.0f
#define FLOAT_TO_13BIT 4096.0f
#define FLOAT_TO_12BIT 2048.0f
#define FLOAT_TO_11BIT 1024.0f
#define FLOAT_TO_10BIT 512.0f
#define FLOAT_TO_9BIT 256.0f
#define FLOAT_TO_8BIT 128.0f
#define FLOAT_TO_7BIT 64.0f
#define FLOAT_TO_6BIT 32.0f
#define FLOAT_TO_5BIT 16.0f
#define FLOAT_TO_4BIT 8.0f
#define FLOAT_TO_3BIT 4.0f
#define FLOAT_TO_2BIT 2.0f
#define FLOAT_TO_1BIT 1.0f

#define TWOTHIRDS (2.0f/3.0f)

#define CLIPPOINT 0.025f

#define PWMMAX	1023.0F
#define PWMHALF 511.0f

#define AUXASTAP 1
#define AUXASDOUBLE 2
#define AUXASPRESET 3
#define HOLDOFF	0
#define HOLDON	1
#define OFF	0
#define	ON	1
#define TRAILSOFF	0
#define TRAILSON	1

#define MONO 1
#define STEREO 3
#define MISO 2

//#define FORWARD 1 //for forward expression direction
//#define BACKWARD 2 //for backward expression direction
//#define ROUGHLYEQUALF 0.1f //use this for setting expression, as a difference to check against for whether knobs moved
#define ROUGHLYEQUAL 100
#define HEEL 1
#define TOE 2

//sources for setting change
#define EXPRESSION 0
#define KNOB 1
#define MIDI 2
#define AUX 3
#define PRESET 4
#define MIDISPECIAL 5 //currently we'll use this for snapping pitch vs linear shifting; could be useful for other categories as well

//EEPROM addresses for globals
//EEPROM addresses for globals
#define BYPASSINDEX 0
#define STEREOSTATEINDEX 1
#define TRAILSINDEX 2
#define MIDICHANNELINDEX 3
#define MIDICLOCKMODEINDEX 4
#define SCOOCHASTAPINDEX 5
#define AUXSETTINGINDEX 6
#define CURRENTPRESETINDEX 7
//11-14 are subRate, subTime, volume, and modes binary

//knob zones
#define LPFZONE	0
#define HPFZONE 1
#define CRUSHZONE 0
#define DIRTZONE 1

#define SPEEDSEGMENT 0.1428f
#define SUBSEGMENTTIME 0.1666666f //divides the subdivision pot into 6 segments
#define SUBSEGMENTRATE 0.1428f //divides the subdivision pot into 7 segments

#define RELAYRESETTIMERTOP 200 //reset lines after 200 ms
#define EETIMERTOP 10000 //set eeprom value after 10 seconds of no change

#define MINRATE 0.1f
#define MIDRATE 10.0f
#define MAXRATE 10.0f

#define DIMENSIONMINRATE 0.1428f
#define DIMENSIONMAXRATE 0.999f

#define MAXDEPTH 0.95f //use this both to scale depth and limit time range at edges

#define MAXTAP 3000
#define MINTAP 100
#define MAXTAPSAMPLES 144345.0f//72391.54f//72007.5f

#define MONOSINGLE 0
#define MONOPARALLEL 1
#define MONOSERIES 2

#define CLOCKIN 0
#define CLOCKOUT 1
#define CLOCKTHRU 2
#define CLOCKOFF 3

#define MAXSTAGES 12.0f //max number of allpass stages
#define SIZESEGMENT (1.0f/MAXSTAGES) //segment of pot based on that

#define TOTALALLPASSMEMORY 777//481//72172
#define TOTALALLPASSMEMORYF 777.0f//481.0f//72172.0f
#define MAXDELAYTIMEMILLIS ((TOTALALLPASSMEMORY / SAMPLERATE) * 1000.0f)
#define MINIMUMALLPASSTIME 10.0f

#define SURPRISEBUFFERSIZE 48000//72172
#define SURPRISEBUFFERSIZEF 48000.0f//72172.0f

#define APTIMEWASTERSIZE 10

#define DIMENSIONMEMORY 222//166
#define DIMENSIONMEMORYF 222.0f//166.0f
#define DIMENSIONMINIMUMTIME 1.0f//64.0f
//#define DIMENSIONMEMORYLIMIT 719 //limit for depth LFO, just in case
#define DIMENSIONMEMORYLIMITF (DIMENSIONMEMORYF - DIMENSIONMINIMUMTIME - 2.0f) //limit for depth LFO, just in case

#define NUMBEROFSHAPES 7
#define NUMBEROFSHAPESF 7.0f
#define SHAPESEGMENT (1.0f/NUMBEROFSHAPESF)
#define SINE 0
#define SQUARE 1
#define SAW 2 //RAMPUP
#define REVERSESAW 3 //RAMPDOWN
#define RANDOMSINE 5
#define RANDOMSQUARE 6
#define ENVELOPE 7

#define RANDOMTHRESHOLD 0.25f

float compandingValue = 255.0f;

uint8_t vibratoActive = OFF;
uint8_t flangeActive = OFF;
uint8_t tremoloActive = OFF;
uint8_t filterActive = OFF;

//#define ALLPASSBUFFERSIZE 11250//TOTALALLPASSMEMORY/MAXSTAGES
//#define ALLPASSBUFFERSIZEF 11250.0f

#define EEPROMBYTEQUEUESIZE 600

#define UINT32_TMAXF 4294967295.0f //max value of a uint32, in float form


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

SAI_HandleTypeDef hsai_BlockA1;
SAI_HandleTypeDef hsai_BlockB1;
DMA_HandleTypeDef hdma_sai1_a;
DMA_HandleTypeDef hdma_sai1_b;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim14;
TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim17;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */

HAL_StatusTypeDef tester = HAL_OK;

uint8_t MIDIBuffer[MIDIINPUTBUFFERSIZE] __attribute__ ((section (".DATA_RAM_D3"))); //use this to receive messages on DMA
uint8_t storedMIDI[MIDISTORAGEBUFFERSIZE]; //use this to store messages as they move through, to be processed by the main loop
uint8_t storedMIDIOut[MIDISTORAGEBUFFERSIZE];
uint8_t MIDIOut1[1] = {0};
uint8_t MIDIOut2[1] = {0};
uint16_t unprocessedBytes = 0;
uint16_t maxUnprocessedBytes = 0;
uint16_t unprocessedBytesOut = 0;
uint16_t MIDIWriteHead = 0;
uint16_t MIDIWriteHeadOut = 0;
uint16_t MIDIReadHead = 0;
uint16_t MIDIReadHeadOut = 0;
uint8_t MIDITxBusy = 0; //flag this when initiating transfer, clear it when transfer complete
uint8_t lsNybble = 0;
uint8_t msNybble = 0;
uint8_t statuslsNybble = 0;
uint8_t statusmsNybble = 0;
uint8_t dataBytes[2] = {0, 0};
uint8_t dataIndex = 0;
uint8_t midiChannel = CHANNEL1;
uint8_t MIDIOutByte[MIDISTORAGEBUFFERSIZE] __attribute__ ((section (".DATA_RAM_D3")));

uint16_t ADCBuffer[ADCBUFFERSIZE] __attribute__ ((section (".DATA_RAM_D3")));
volatile int32_t SAIIn[SAIBUFFERSIZE] __attribute__ ((section (".DATA_RAM_D3")));
volatile int32_t SAIOut[SAIBUFFERSIZE] __attribute__ ((section (".DATA_RAM_D3")));
int32_t testBuff[SAIBUFFERSIZE];
static volatile int32_t *inPtr = &SAIIn[0]; __attribute__ ((section (".DATA_RAM_D3")))
static volatile int32_t *outPtr = &SAIOut[0]; __attribute__ ((section (".DATA_RAM_D3")))
int32_t SAIInCurrentHalf[SAIBUFFERSIZEHALF];
int32_t SAIOutCurrentHalf[SAIBUFFERSIZEHALF];


uint8_t I2COut[2] = {0, 0};

volatile uint16_t unprocessedSamples = 0;
uint16_t sampleCounter = 0;
uint8_t block = 0;
float volume = 1.0;
float volumeAttenuation;
float modifiedMix = 0.5f;
float modifiedMixInv = 0.5f;
float modifiedMixReverseDryBlend, modifiedMixReverseDryBlendInv;
float trueDryMix = 0.5f;
float delayTime = 0.0;
float delayTimeSingleStage = 0.0f;
uint8_t ADCValsReady = 0;
float testFloat;
int16_t testInt;
float delayTimeNew = 0.0;
//float g = 0.0f; //this will be our delay growth parameter
float delayTarget = 0.0;
//float feedbackAmount, verbFeedbackAmount;
float feedbackL, feedbackR, rightVerbFeedback, leftVerbFeedback;
float surpriseFeedbackAmountL, surpriseFeedbackAmountLTarget, surpriseFeedbackAmountR, surpriseFeedbackAmountRTarget;
float summedFeedbackAmountL, summedFeedbackAmountR;
float maxSurpriseFeedback;
float surpriseFeedbackLCounter, surpriseFeedbackRCounter;
float surpriseFeedbackL, surpriseFeedbackR;
float leftVerbLPF, rightVerbLPF;
float verbVolumeTarget, verbVolume;
float cross; //use this as a cross-feedback control
float overdrive, makeupGain;
float lastOutLeft, lastOutRight, lastlastOutLeft, lastlastOutRight, tempLastOutLeft, tempLastOutRight, leftOutLowpass, rightOutLowpass;
float inputVol, inputVolTarget;
float pwmDry, pwmWet, pwmLFO;
float tempFeedback = 0.0f;
float tempTime = 0.0f;
float diffusion = 0.0f;
float diffusionTarget = 0.0f;
float diffusionMix = 0.0f;
float diffusionMixInv = 1.0f;
float diffusionMixTarget = 0.0f;

//allpass ap1L;
//float apLBuffer[TOTALALLPASSMEMORY];

//allpass ap1R;
//float apRBuffer[TOTALALLPASSMEMORY] __attribute__ ((section (".DATA_RAM_D2")));

//allpass surpriseL;
//float surpriseLBuffer[SURPRISEBUFFERSIZE];

//allpass surpriseR;
//float surpriseRBuffer[SURPRISEBUFFERSIZE] __attribute__ ((section (".DATA_RAM_D2")));

delay_parting dimensionL;
float dimensionLBuffer[DIMENSIONMEMORY];
delay_parting dimensionR;
float dimensionRBuffer[DIMENSIONMEMORY];


statevariable leftOutLPF, rightOutLPF, leftOutHPF, rightOutHPF, leftFeedbackLPF, rightFeedbackLPF, crushHPFL, crushHPFR, flangeHPFL, flangeHPFR, flangeLPFL, flangeLPFR, samplerateOutLPFL, samplerateOutLPFR;

allpass apTimeWaster;
float apTimeWasterBuffer[APTIMEWASTERSIZE];

//delay delayRight;
//float delayBufferR[DELAYBUFFERSIZE] __attribute__ ((section (".DATA_RAM_D2")));

float sinTable[SINSIZE]; //fill this with a -1 to 1 single cycle sin wave and read from it

float tone = 0.0f;
float toneHPF = 0.0f;
float grit1 = 0.0f;
float grit2 = 0.0f;
float gritFeedbackScaler = 0.0f;
float sub1 = 1.0f;
float sub2 = 1.0f;

//use this line to put an array in RAM D2 //__attribute__ ((section (".DATA_RAM_D2")));

//arrays for presets
float currentSetting[PRESETARRAYLENGTH];
float targetSetting[PRESETARRAYLENGTH];
float presetRead[PRESETARRAYLENGTH];
float expBuffer[PRESETARRAYLENGTH]; //use this to pull from presetRead or currentSetting depending on which expression settings should be used


//volume(dry), mix(wet volume), spread, time1, sub1, time2, sub2, feedback, rate, depth, tone, grit
//direction, heel, toe of the above
////////////////////////////////////rate    depth   shape  dissolve chance  smear   glitch  time    filter  mix     offset  subR   subT  vol   modes rate             depth            shape            dissolve         chance           smear            glitch           time            filter           mix
float initPatch[PRESETARRAYLENGTH]={0.500f, 0.500f, 0.000f, 0.500f, 0.500f, 0.500f, 0.250f, 0.500f, 1.000f, 0.500f, 1.000f, 0.25f, 1.0f, 0.5f, 0.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 1.0f, OFF, 0.0f, 1.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 1.0f, OFF, 0.0f, 0.0f, ON, 0.0f, 1.0f};
float preset1[PRESETARRAYLENGTH] = {0.350f, 0.270f, 0.300f, 0.370f, 0.400f, 0.600f, 0.550f, 0.800f, 1.000f, 0.500f, 1.000f, 1.00f, 1.0f, 0.5f, 0.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 1.0f, OFF, 0.0f, 1.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 1.0f, OFF, 0.0f, 0.0f, ON, 0.0f, 1.0f};
float preset2[PRESETARRAYLENGTH] = {0.370f, 0.220f, 0.000f, 0.350f, 0.520f, 0.200f, 0.600f, 0.050f, 0.800f, 0.100f, 1.000f, 0.25f, 1.0f, 0.5f, 5.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 1.0f, OFF, 0.0f, 1.0f, OFF, 0.0f, 0.0f, ON, 0.0f, 1.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 1.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 1.0f};
float preset3[PRESETARRAYLENGTH] = {0.130f, 0.800f, 0.840f, 0.590f, 0.400f, 0.770f, 0.250f, 0.700f, 0.900f, 0.060f, 1.000f, 1.00f, 1.0f, 0.5f, 6.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 1.0f, OFF, 0.0f, 1.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 0.0f, OFF, 0.0f, 1.0f, OFF, 0.0f, 0.0f, ON, 0.0f, 1.0f};

//arrays for snapshotting pot values
int16_t bypassPressSnapshot[11] = {0,0,0,0,0,0,0,0,0,0}; //slots for all 10 knobs, and alt settings (currently just subdivisions)
int16_t bypassReleaseSnapshot[11] = {0,0,0,0,0,0,0,0,0,0};
uint8_t auxPressSnapshot = 0;
int16_t auxPressSnapshotSub1, auxPressSnapshotSub2;
uint8_t expressionChanged = 0; //set this when one of the knobs moved, so we can write the new setting to EEPROM at the end
float expBufferTemp[PRESETARRAYLENGTH];

uint32_t millis = 0; //used as millisecond counter
uint32_t sampleBasedMillis = 0; //use this to mark
float LFOcounter = 0; //counter
float modRate = MINRATE; //rate to check counter against
float modRateAsTime, modRateAsTimeTarget; //rate expressed as a number of samples, potentially useful for triggering random delay moments
float modDepth;
float modDepthRandomL, modDepthRandomLTarget, modDepthRandomR, modDepthRandomRTarget;
float modAmountL, modAmountNewL, modAmountFilteredL, modAmountSmoother;
float modAmountR, modAmountNewR, modAmountFilteredR;
float flangeAmountL, flangeAmountR, flangeAmountL2, flangeAmountR2, filterAmountL, filterAmountR;
float dimensionDepth;
float dimensionAmount, dimensionAmountLFiltered, dimensionAmountLNew, dimensionAmountRFiltered, dimensionAmountRNew;
float dimensionRate = DIMENSIONMINRATE;

float envelopeL, envelopeLNew, envelopeR, envelopeRNew;
float envelopeStatusL, envelopeStatusR;
float envelopeModifierL = 1.0f;
float envelopeModifierR = 1.0f;
float envelopeSmoother = 0.0005f;

float leftOutSpread, rightOutSpread, leftOutMixed, rightOutMixed;

float	volumeSmoother, mixSmoother, timeSmoother, filterSmoother, rateSmoother, depthSmoother, shapeSmoother, dissolveSmoother, chanceSmoother, smearSmoother, glitchSmoother, offsetSmoother, subRateSmoother, subTimeSmoother;

led ledBypass, ledAux, ledP1, ledP2, ledP3, ledTime, ledFilter, ledMix; //led states
uint8_t ledTickerOn, ledTickerState; //state for ticking and for the tick itself
uint16_t ledTicker;
uint16_t ledTickerTop = 500;

//presets
uint8_t preset = 0; //use this to track which preset we're on
uint8_t savingPreset = 0; //use to show we're in saving preset mode
uint8_t inPreset = 0; //use this to mark whether we're on a preset or not (so when a knob moves, exit this and move out of preset mode)

//settings
uint8_t trailsState = TRAILSOFF;
uint8_t auxSetting = AUXASTAP;
uint8_t stereoState = STEREO;
uint8_t onOffState = OFF;
uint8_t holdState = OFF;
uint8_t assigningHold = OFF; //turn on when tactile switch is engaged while switch is pressed, then turn back off when switch is released
uint8_t assigningBypass = OFF; //turn on when tactile switch is engaged while switch is pressed, then turn back off when switch is released
uint32_t relayResetCounter = 0; //use this to turn off the relay pins after sufficient time has passed
uint8_t ledTactile = 0; //use this to tell the system which LED to light under the tactile switch

//tap tempo stuff
float tapMillis, lastTapMillis, diffMillis, lastDiffMillis, tapNum, tapNumReset;
uint8_t lfoResetFlag = 0;
uint16_t tapResetTimer;
float tapResetTimerSamples;
float tapLEDTracker1, tapLEDTracker2, tap1TurnOff, tap2TurnOff, tap1On, tap2On, tapLEDResetCounter; //use these for LED indication of taps
float p1TurnOff, p2TurnOff, p3TurnOff;
uint16_t midiPPQCounter;
float midiMillisStart, midiMillisEnd;
float tapAverage = 0.0f;
float tapAverageDelayScaled = 0.0f;
float tapAverageMIDI = 0.0f;
uint8_t MIDIClockMode = CLOCKTHRU;
float clockPulseTimer; //use this to mark the number of samples in a pulse
float clockPulseTimerCounter; //use this to count samples to send a pulse

uint8_t tactWasHeld = 0; //mark this when tactile switch was held recently, to avoid switching on first release
uint8_t auxWasHeld = 0;

//scalers for different routing modes
float monoParallelFeedbackScaler, monoParallelFeedbackScalerTarget, rightInputLeftDryScaler, rightInputLeftDryScalerTarget, rightInputLeftWetScaler, rightInputLeftWetScalerTarget, monoSeriesModScaler, monoSeriesModScalerTarget, rightFeedbackAmountScaler, rightFeedbackAmountScalerTarget, rightOutParallelBlendScaler, rightOutParallelBlendScalerTarget;

//SWITCHES
switchDB fsBypass, fsAux, tactile, tactMod, tactFilter, tactMix, scooch;

//POTS
pot potMix, potTime, potFilter, potRate, potDepth, potShape, potChance, potSmear, potGlitch, potDissolve, potExp;

//relay timer
uint8_t relayResetTimer = RELAYRESETTIMERTOP;

//eeprom storage timers
uint16_t subRateEETimer, subTimeEETimer, offsetEETimer, volumeEETimer;
uint8_t subRateEEFlag, subTimeEEFlag, offsetEEFlag, volumeEEFlag;

//low pass registers for main algo
float leftLPF1 = 0.0f;
float leftHPF1 = 0.0f;
float leftLPF2 = 0.0f;
float leftHPF2 = 0.0f;
float rightLPF1 = 0.0f;
float rightHPF1 = 0.0f;
float rightLPF2 = 0.0f;
float rightHPF2 = 0.0f;
//gain developing registers for main algo
float leftACC1 = 0.0f;
float leftACC2 = 0.0f;
float leftACC3 = 0.0f;
float rightACC1 = 0.0f;
float rightACC2 = 0.0f;
float rightACC3 = 0.0f;

//link to main algo for FV-1 with notes: https://docs.google.com/document/d/1vvPT8ysgV7m1YTVCPfdeg2lGgUZUZuviTpCusYtsVmY/edit?tab=t.0

float sinSweepL = 0.0f; //start left sweeper at zero
float sinSweepR = SINSIZEF * 0.5f; //and right sweeper at halfway

float dimensionSweep = 0.0f;

uint8_t monoState = MONOSINGLE; //assign this based on spread knob position

uint8_t tempByte = 0; //available for grabbing EEPROM info in a byte to turn into a float for a setting

//use this to flag whether to send a MIDI byte on or not (preventing clock messages from passing through)
uint8_t dontSend = 0;

//use these to turn off subdivisions when turning time knob
uint8_t noSubdivisionsTime = 1;
uint8_t noSubdivisionsRate = 1;

//scooch usage
uint8_t scoochAsTap = 0;
uint8_t auxWasPressed = 0;
uint8_t presetWasPressed = 0;
uint8_t lfoWasPressed = 0;
uint8_t bypassWasPressed = 0;

//set this when the tap LEDs shouldn't be flipping (factory reset, when tap is held, etc)
uint8_t tapLEDPaused = 0;

//storage of most recent tap source, to avoid single footswitch messing with MIDI timing
uint8_t lastTapSource = AUX;

float stages = 1.0f; //make sure stages doesn't start at 0

int32_t initBufferSize = (TOTALALLPASSMEMORY/MAXSTAGES);

uint16_t shape = SINE;
uint8_t randFlagL = 0;
uint8_t randFlagR = 0;


float envGain;
float thresholdRandomInput, thresholdRandomDirection, thresholdRandomSub, thresholdRandomFeedback;
uint8_t onlyInput; //use this to set whether delay buffer should only accept input, not randomly feedback - this will allow a "Chance at noon is very normal" setting


uint16_t adcDelayCounter = 0;
uint16_t ledDelayCounter = 0;

float apTimeL, apTimeLFiltered, apTimeR, apTimeRFiltered;
float dwell, dwellFiltered;

float sampleReducerLeft, sampleReducerRight, sampleReducerInc;
float overdrive, makeupGain;
float lastOutLeft, lastOutRight;

float surpriseInputVolL, surpriseInputVolLTarget, surpriseInputVolR, surpriseInputVolRTarget;

float bitDepth, bitDepthDifference, bitGain;
uint32_t bitDepthInt;
uint32_t bitMasks[25] = {BITMASK01, BITMASK02, BITMASK03, BITMASK04, BITMASK05, BITMASK06, BITMASK07, BITMASK08, BITMASK09, BITMASK10, BITMASK11, BITMASK12, BITMASK13, BITMASK14, BITMASK15, BITMASK16, BITMASK17, BITMASK18, BITMASK19, BITMASK20, BITMASK21, BITMASK22, BITMASK23, BITMASK24, BITMASK25};
float bitConverters[34] = {0, FLOAT_TO_1BIT, FLOAT_TO_2BIT, FLOAT_TO_3BIT, FLOAT_TO_4BIT, FLOAT_TO_5BIT, FLOAT_TO_6BIT, FLOAT_TO_7BIT, FLOAT_TO_8BIT, FLOAT_TO_9BIT, FLOAT_TO_10BIT, FLOAT_TO_11BIT, FLOAT_TO_12BIT, FLOAT_TO_13BIT, FLOAT_TO_14BIT, FLOAT_TO_15BIT, FLOAT_TO_16BIT, FLOAT_TO_17BIT, FLOAT_TO_18BIT, FLOAT_TO_19BIT, FLOAT_TO_20BIT, FLOAT_TO_21BIT, FLOAT_TO_22BIT, FLOAT_TO_23BIT, FLOAT_TO_24BIT, FLOAT_TO_25BIT, FLOAT_TO_26BIT, FLOAT_TO_27BIT, FLOAT_TO_28BIT, FLOAT_TO_29BIT, FLOAT_TO_30BIT, FLOAT_TO_31BIT, FLOAT_TO_32BIT, 0};

float flangeOutL, flangeOutR, flangeFeedbackL, flangeFeedbackR;

float offset = -1.0f;

float modBlend;//, modVoice;

float filterCutoff = 0.0f;

float subRate = 1.0f;
float subTime = 1.0f;
float randomModSub = 1.0f;
float randomTimeSub = 1.0f;
float samplerateSub = 1.0f;
float lastRandomTimeSubMotion = 0.0f;

float footswitchDouble = 1.0f;

float randomTimeSubLinear;

uint8_t thresholdCrossedL = 0;
uint8_t thresholdCrossedR = 0;

//variables previously declared at the top of each processData call
static volatile float leftIn, leftOut;
static volatile float rightIn, rightOut;
uint16_t tableIndexL, tableIndexPlusL;
float tableIndexFL;
float tableIndexPartialL;
uint16_t tableIndexR, tableIndexPlusR;
float tableIndexFR;
float tableIndexPartialR;


uint8_t eepromByteQueue[EEPROMBYTEQUEUESIZE];
uint16_t eepromByteQueueRead = 0;
uint16_t eepromByteQueueWrite = 0;
uint16_t unprocessedEepromBytes = 0;
volatile uint8_t eepromByteQueueTimer = 6;

//some sample rate related things pulled from the FV1 clock experimentation
volatile float thisLoFiSampleL, thisLoFiSampleR, lastLoFiSampleL, lastLoFiSampleR, nextLoFiSampleL, nextLoFiSampleR, thisHiFiSampleL, thisHiFiSampleR, lastHiFiSampleL, lastHiFiSampleR, nextHiFiSampleL, nextHiFiSampleR;
float virtualSampleRate = 16000.0f;
float timeScaler = 1.0f;
float sampleRateRelationship, virtualSampleRateInc, realSampleRateInc, virtualSampleRateAccumulator, realSampleRateAccumulator;
float sampleRateInc, inputLPFL, inputLPFR, inputHPFL, inputHPFR, sampleWriteHeadF, sampleReadHeadF, sampleWriteHeadPartial, currentSampleL, currentSampleR, lastSampleL, lastSampleR, accumulatedInputL, accumulatedInputR, accAverager, dsSampleAccumulator;
float sampleRateBlend, sampleRateBlendTarget;
float virtualSampleRateLimiter = 1.0f; //use this when tap won't fit into the clock setting
int32_t sampleWriteHead;
uint8_t newSampleFlag;
int32_t cleanSampleWriteHead, cleanSampleReadHead;
#define SAMPLEBUFFERSIZE 4
#define SAMPLEBUFFERSIZEF 4.0f
float samplesL[SAMPLEBUFFERSIZE];
float samplesR[SAMPLEBUFFERSIZE];
float cleanSamplesL[SAMPLEBUFFERSIZE];
float cleanSamplesR[SAMPLEBUFFERSIZE];
#define SAMPLEDISTANCEBUFFERSIZE 2000
float sampleDistanceBuffer1[SAMPLEDISTANCEBUFFERSIZE];
float sampleDistanceBuffer2[SAMPLEDISTANCEBUFFERSIZE];
uint8_t bufferSwitcher = 0;

volatile uint16_t timerPeriod = 2000;
volatile float timerPeriodF;
#define MINSAMPLERATE 1000.0f
uint8_t samplesReceivedHiFi = 0; //mark until second sample has been received, then leave for the remainder of the program (see if this helps, probably won't)
uint8_t samplesReceivedLoFi = 0;
uint8_t nextSampleFlag = 0; //flag this when tim14 triggers, for main loop to grab next sample

//reverb variables
float earlyRefLOut, midRefLOut, ap1LOut, ap2LOut, ap3LOut, ap4LOut, ap5LOut, earlyRefROut, midRefROut, ap1ROut, ap2ROut, ap3ROut, ap4ROut, ap5ROut;
float bigFeedbackL, bigFeedbackR;
float reverbFeedbackAmount, reverbMix;
#define K1 .66f//.75f
#define K2 .75f//.66f
/*#define AP1SIZE 5555//1857	//1238*1.5
#define AP2SIZE 3851	//2567*1.5
#define AP3SIZE 4680	//3120*1.5
#define AP4SIZE 6435	//4290*1.5
#define AP5SIZE 8804	//5869*1.5
#define APSIZETOTAL 25627*/
//VALUES BASED ON HALF SECOND OF MEMORY - see https://docs.google.com/spreadsheets/d/1dSbskAkEbyfw0botXIGwBVh6QNhgE3guFdcW7mgdHOs/edit?usp=sharing
#define AP1SIZE 1743
#define AP2SIZE 3615
#define AP3SIZE 4409//4393+16
#define AP4SIZE 6056//6040+16
#define AP5SIZE 8264
#define APSIZETOTAL 24057
#define VERBMODMAX 16.0f
#define VERBMODRATE 0.001f
float verbMod = 0.0f;
float verbModSign = 1.0f; //flip the sign to move up instead of down or vice versa, easy triangle LFO
allpass ap1L;
float ap1LBuffer[AP1SIZE];
allpass ap2L;
float ap2LBuffer[AP2SIZE];
allpass ap3L;
float ap3LBuffer[AP3SIZE];
allpass ap4L;
float ap4LBuffer[AP4SIZE];
allpass ap5L;
float ap5LBuffer[AP5SIZE];

allpass ap1R;
float ap1RBuffer[AP1SIZE];// __attribute__ ((section (".DATA_RAM_D2")));
allpass ap2R;
float ap2RBuffer[AP2SIZE] __attribute__ ((section (".DATA_RAM_D2")));
allpass ap3R;
float ap3RBuffer[AP3SIZE] __attribute__ ((section (".DATA_RAM_D2")));
allpass ap4R;
float ap4RBuffer[AP4SIZE] __attribute__ ((section (".DATA_RAM_D2")));
allpass ap5R;
float ap5RBuffer[AP5SIZE] __attribute__ ((section (".DATA_RAM_D2")));


#define REVERSESIZE 48005
float reverseMix, reverseMixTarget; //use this to blend in the reverse delay, we'll see where it sounds best
float reverseClock = 0.25f;
delay_clocked reverseL;
float reverseLBuffer[REVERSESIZE];
delay_clocked reverseR;
float reverseRBuffer[REVERSESIZE] __attribute__ ((section (".DATA_RAM_D2")));
#define REVERSESEGMENT .083333333333f

#define TRUEMIX 0
#define MODMIX 1
#define TREMOLO 0
#define VIBRATO 1
#define LOWPASS 0
#define HIGHPASS 1
#define MIXMODETRUE 0xFB //11111011 used to turn byte off through &=
#define FILTERMODELOWPASS 0xFD //11111101 used to turn byte off through &=
#define MODMODETREMOLO 0xFE //11111110 used to turn byte off through &=
#define MIXMODEMODIFIED 0x04   //00000100 used to turn byte on through |=
#define FILTERMODEHIGHPASS 0x02 //00000010 used to turn byte on through |=
#define MODMODEVIBRATO 0x01 //00000001 used to turn byte on through |=
uint8_t modMode = TREMOLO;
uint8_t filterMode = LOWPASS;
uint8_t mixMode = TRUEMIX;
uint8_t modeBinary = 0; //use this to store all states together (for eeprom) - time MSB, filter middle, volume LSB
float modeBinaryF = 0.0f; //use this if needing to convert to/from float for efficiency purposes w/presets etc

float sampleRateMix, sampleRateMixTarget;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SAI1_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM17_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_TIM3_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM16_Init(void);
static void MX_TIM14_Init(void);
/* USER CODE BEGIN PFP */
void processSample();
void setModes();
void envelopeOffL();
void envelopeOffR();

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	ADCValsReady = 1;
	//PHYSICAL POT LOCATIONS
	//TOP ROW: 7, 3, 4, 6
	//MIDDLE ROW: 5, 8, 2
	//MINI POTS: 0, 1, 9
	updatePot(&potMix, ADCBuffer[7]); //use this for mix
	updatePot(&potDissolve, ADCBuffer[3]); //use this for clock
	updatePot(&potTime, ADCBuffer[4]); //use this for time
	updatePot(&potDepth, ADCBuffer[6]); //use this for speed


	updatePot(&potFilter, ADCBuffer[1]);

	updatePot(&potRate, ADCBuffer[7]);
	updatePot(&potShape, ADCBuffer[5]);

	updatePot(&potChance, ADCBuffer[4]);
	updatePot(&potSmear, ADCBuffer[6]);
	updatePot(&potGlitch, ADCBuffer[2]);

	updatePot(&potExp, ADCBuffer[10]);

}
void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
	//inPtr = &SAIIn[0];
	//outPtr = &SAIOut[0];
	//processSample();
	//adcDelayCounter++; //increment adc counter, to match previous timing in the loop
}
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
	//inPtr = &SAIIn[SAIBUFFERSIZEHALF];
	//outPtr = &SAIOut[SAIBUFFERSIZEHALF];
	inPtr = &SAIIn[0];
	outPtr = &SAIOut[0];
	processSample();
	adcDelayCounter++; //increment adc counter, to match previous timing in the loop
}
/*void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{

}
void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{

}*/
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	MIDIOut1[0] = storedMIDI[MIDIWriteHead] = MIDIBuffer[0];
	MIDIWriteHead++;
	if(MIDIWriteHead > MIDISTORAGEBUFFERSIZE - 1)
		MIDIWriteHead = 0;
	unprocessedBytes++;
	//HAL_UART_Transmit_DMA(&huart1, (uint8_t *)MIDIOut1, 1);
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	MIDIOut2[0] = storedMIDI[MIDIWriteHead] = MIDIBuffer[1];
	MIDIWriteHead++;
	if(MIDIWriteHead > MIDISTORAGEBUFFERSIZE - 1)
		MIDIWriteHead = 0;
	unprocessedBytes++;
	//HAL_UART_Transmit_DMA(&huart1, (uint8_t *)MIDIOut2, 1);
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	MIDITxBusy = 0; //when transfer is complete, mark that it's no longer busy
}
void mute() //use mainly when writing/reading from EEPROM, to avoid the beeps when the DMA circulates without new audio info
{
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, PWMMAX);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, PWMMAX);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, PWMMAX);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, PWMMAX);
}
//ORIGINAL BYTE WRITE
/*void writeByte(uint8_t data, uint32_t address) //byte to write, address to write to
{
	//mute();
	  uint8_t I2COutwdata[3]; //establish buffer for writing each byte
	  HAL_GPIO_WritePin(EEWC_GPIO_Port, EEWC_Pin, GPIO_PIN_RESET); //drive pin low to write
	  I2COutwdata[0] = address >> 8; //shift upper 8 bits into MSB slot
	  I2COutwdata[1] = address & 0x00FF; //mask upper 8 bits for LSB slot (may not be necessary assuming natural truncation, but we can check later)
	  I2COutwdata[2] = data; //value to send
	  tester = HAL_I2C_Master_Transmit(&hi2c1, M24C64WRITE, (uint8_t *)I2COutwdata, 3, 10000);
	  //HAL_Delay(5); //this is necessary because the EEPROM can take up to 5mS to complete its internal write process
	  	  	  	  	  //however, it causes an audible beep
	  	  	  	  	  //so for byte writing, let the program be careful to never write bytes within 5ms of each other
	  HAL_GPIO_WritePin(EEWC_GPIO_Port, EEWC_Pin, GPIO_PIN_SET); //drive pin high to protect
}*/
//refactoring of writebyte to simply add to a queue
void writeByte(uint8_t data, uint32_t address) //byte to write, address to write to
{
	eepromByteQueue[eepromByteQueueWrite] = address >> 8; //shift upper 8 bits into MSB slot
	eepromByteQueue[eepromByteQueueWrite+1] = address & 0x00FF; //mask upper 8 bits for LSB slot (may not be necessary assuming natural truncation, but we can check later)
	eepromByteQueue[eepromByteQueueWrite+2] = data; //value to send
	eepromByteQueueWrite+=3; //advance by 3 to next location
	if(eepromByteQueueWrite >= EEPROMBYTEQUEUESIZE-4)//if not room enough for three more bytes
		eepromByteQueueWrite = 0; //go back to start
	unprocessedEepromBytes++; //mark that a new byte is ready to be written
}
void actuallyWriteByte()
{
	HAL_GPIO_WritePin(EEWC_GPIO_Port, EEWC_Pin, GPIO_PIN_RESET); //drive pin low to write
	tester = HAL_I2C_Master_Transmit(&hi2c1, M24C64WRITE, (uint8_t *)&eepromByteQueue[eepromByteQueueRead], 3, 10000); //trasnmit the set of three bytes located at the read head (two-byte address followed by data byte)
	HAL_GPIO_WritePin(EEWC_GPIO_Port, EEWC_Pin, GPIO_PIN_SET); //drive pin high to protect
	eepromByteQueueRead+=3; //advance by 3 to next location
	if(eepromByteQueueRead >= EEPROMBYTEQUEUESIZE-4)//if not room enough for three more bytes
		eepromByteQueueRead = 0; //go back to start
	unprocessedEepromBytes--; //mark that we've processed a byte
	eepromByteQueueTimer = 0; //and reset the timer to count up toward the next write
}
void writeGlobalExpression(float* data) //pointer to array with the expression data
{
	//mute();
	  //writing to EEPROM
	  uint16_t tempAddress = 0; //temp address, always slot zero for global
	  //uint8_t I2COutwdata[3]; //establish buffer for writing each byte
	  //HAL_GPIO_WritePin(EEWC_GPIO_Port, EEWC_Pin, GPIO_PIN_RESET); //drive pin low to write
	  for (uint16_t n = EXPRESSIONSTART; n < PRESETARRAYLENGTH; n++) //for each element of the array, starting with expression info
	  {
		  tempAddress = n; //set directly to n
		  //I2COutwdata[0] = tempAddress >> 8; //shift upper 8 bits into MSB slot
		  //I2COutwdata[1] = tempAddress & 0x00FF; //mask upper 8 bits for LSB slot (may not be necessary assuming natural truncation, but we can check later)
		  //I2COutwdata[2] = data[n] * FLOAT_TO_UINT8; //value to send
		  //tester = HAL_I2C_Master_Transmit(&hi2c1, M24C64WRITE, (uint8_t *)I2COutwdata, 3, 10000);
		  //HAL_Delay(5); //this is necessary because the EEPROM can take up to 5mS to complete its internal write process
		  writeByte(data[n] * FLOAT_TO_UINT8, tempAddress); //add byte to write queue
	  }
	  //HAL_GPIO_WritePin(EEWC_GPIO_Port, EEWC_Pin, GPIO_PIN_SET); //drive pin high to protect
}
void writePreset(float* data, uint8_t slot) //pointer to array with the preset data, preset slot
{
	  //writing to EEPROM
	  uint16_t eeaddress = slot * PRESETARRAYLENGTH; //start address at beginning of preset section
	  uint16_t tempAddress = eeaddress;
	  for (uint16_t n = 0; n < PRESETARRAYLENGTH; n++) //for each element of the array
	  {
		  tempAddress = eeaddress + n;
		  if(n == MODESINDEX)
			  writeByte(data[n], tempAddress); //add byte to write queue, already in uint8 form
		  else
			  writeByte(data[n] * FLOAT_TO_UINT8, tempAddress); //add byte to write queue, converted from float
	  }
}
void readByte(uint8_t* data, uint32_t address) //byte to read, address to read from
{
	while(eepromByteQueueTimer < 6) {}//if eeprom byte was recently written, hold until ready
	//uint8_t dataIn[1] = {0};
	I2COut[0] = address >> 8; //shift upper 8 bits into MSB slot
	I2COut[1] = address & 0x00FF; //mask upper 8 bits for LSB slot (may not be necessary assuming natural truncation, but we can check later)
	tester = HAL_I2C_Master_Transmit(&hi2c1, M24C64READ, (uint8_t *)I2COut, 2, 1000);
	tester = HAL_I2C_Master_Receive(&hi2c1, M24C64READ, data, 1, 1000);
	//return dataIn[0];
}
void readPreset(float* data, uint8_t slot) //pointer to array where you want the preset data, preset slot
{
	while(eepromByteQueueTimer < 6) {}//if eeprom byte was recently written, hold until ready
	uint8_t dataIn[PRESETARRAYLENGTH]; //establish array to receive bytes from EEPROM
	uint16_t eeaddress = slot * PRESETARRAYLENGTH; //start address at beginning of preset section
	I2COut[0] = eeaddress >> 8; //shift upper 8 bits into MSB slot
	I2COut[1] = eeaddress & 0x00FF; //mask upper 8 bits for LSB slot (may not be necessary assuming natural truncation, but we can check later)
	tester = HAL_I2C_Master_Transmit(&hi2c1, M24C64READ, (uint8_t *)I2COut, 2, 1000);
	tester = HAL_I2C_Master_Receive(&hi2c1, M24C64READ, (uint8_t *)dataIn, PRESETARRAYLENGTH, 1000);
	for(int n = 0; n < PRESETARRAYLENGTH; n++)
	{
		if(n == MODESINDEX) //if lfo active value
		{
			modeBinary = dataIn[n]; //load in straight
			setModes(); //and assign settings
		}
		else
			data[n] = (float)dataIn[n] * UINT8_TO_FLOAT; //for most values, convert back to float
	}
}
void turnOn()
{
	  onOffState = 1; //set on off state
	  //turn relay on
	  HAL_GPIO_WritePin(LEFTRELAY1_GPIO_Port, LEFTRELAY1_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(LEFTRELAY12_GPIO_Port, LEFTRELAY12_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(RIGHTRELAY1_GPIO_Port, RIGHTRELAY1_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(RIGHTRELAY12_GPIO_Port, RIGHTRELAY12_Pin, GPIO_PIN_RESET);
	  relayResetTimer = RELAYRESETTIMERTOP; //start reset line clearing countdown
	  //set LED
	  HAL_GPIO_WritePin(LEDBYPASS_GPIO_Port, LEDBYPASS_Pin, GPIO_PIN_SET);
	  //set input volume target to full
	  //if(holdState == ON) //if currently holding
		  //inputVolTarget = 0.0f; //keep input zeroed
	  //else //otherwise
		  inputVolTarget = 1.0f; //bring in input
	  //write EEPROM
	  writeByte(onOffState, BYPASSINDEX);
}
void turnOff()
{
	  onOffState = 0; //set on off state
	  if(trailsState == TRAILSOFF) //if no trails, turn relay off
	  {
		  HAL_GPIO_WritePin(LEFTRELAY1_GPIO_Port, LEFTRELAY1_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(LEFTRELAY12_GPIO_Port, LEFTRELAY12_Pin, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(RIGHTRELAY1_GPIO_Port, RIGHTRELAY1_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(RIGHTRELAY12_GPIO_Port, RIGHTRELAY12_Pin, GPIO_PIN_SET);
	  }
	  else //if trails, make sure relay is on (this should only make a difference when setting has been changed)
	  {
		  HAL_GPIO_WritePin(LEFTRELAY1_GPIO_Port, LEFTRELAY1_Pin, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(LEFTRELAY12_GPIO_Port, LEFTRELAY12_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(RIGHTRELAY1_GPIO_Port, RIGHTRELAY1_Pin, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(RIGHTRELAY12_GPIO_Port, RIGHTRELAY12_Pin, GPIO_PIN_RESET);
	  }
	  relayResetTimer = RELAYRESETTIMERTOP; //start reset line clearing countdown
	  //set LED
	  HAL_GPIO_WritePin(LEDBYPASS_GPIO_Port, LEDBYPASS_Pin, GPIO_PIN_RESET);
	  //set input volume target to none
	  inputVolTarget = 0.0f;
	  //write EEPROM
	  writeByte(onOffState, BYPASSINDEX);
}
void misoOn()
{
	  HAL_GPIO_WritePin(MISORELAY1_GPIO_Port, MISORELAY1_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(MISORELAY12_GPIO_Port, MISORELAY12_Pin, GPIO_PIN_RESET);
	  relayResetTimer = RELAYRESETTIMERTOP; //start reset line clearing countdown
}
void misoOff()
{
	  HAL_GPIO_WritePin(MISORELAY1_GPIO_Port, MISORELAY1_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(MISORELAY12_GPIO_Port, MISORELAY12_Pin, GPIO_PIN_SET);
	  relayResetTimer = RELAYRESETTIMERTOP; //start reset line clearing countdown
}
void clearRelayLines() //use to reset all high relay lines after switching to lower constant current consumption
{
	  HAL_GPIO_WritePin(LEFTRELAY1_GPIO_Port, LEFTRELAY1_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(LEFTRELAY12_GPIO_Port, LEFTRELAY12_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(RIGHTRELAY1_GPIO_Port, RIGHTRELAY1_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(RIGHTRELAY12_GPIO_Port, RIGHTRELAY12_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(MISORELAY1_GPIO_Port, MISORELAY1_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(MISORELAY12_GPIO_Port, MISORELAY12_Pin, GPIO_PIN_RESET);
}
void checkSwitches(void)
{
	//debounce the first footswitch
	debounce(&fsBypass, HAL_GPIO_ReadPin(FS1_GPIO_Port, FS1_Pin));
	debounce(&fsAux, HAL_GPIO_ReadPin(FS2_GPIO_Port, FS2_Pin));
	debounce(&tactile, HAL_GPIO_ReadPin(PRESETTACT_GPIO_Port, PRESETTACT_Pin));
	debounce(&tactMod, HAL_GPIO_ReadPin(LFOTACT1_GPIO_Port, LFOTACT1_Pin));
	debounce(&tactFilter, HAL_GPIO_ReadPin(LFOTACT2_GPIO_Port, LFOTACT2_Pin));
	debounce(&tactMix, HAL_GPIO_ReadPin(LFOTACT3_GPIO_Port, LFOTACT3_Pin));
	if(scoochAsTap == 1)
		debounce(&scooch, HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6));
	//check if relay lines need to be turned off
	if(relayResetTimer > 0)
		relayResetTimer--;
	if(relayResetTimer == 1)
		clearRelayLines();
}
void eepromTicker(void)
{
	if(subRateEETimer > 0) //if above zero
		subRateEETimer--; //decrement
	if(subRateEETimer == 1) //at 1, just before bottoming out
		subRateEEFlag = 1; //mark to write when timing allows

	if(subTimeEETimer > 0) //if above zero
		subTimeEETimer--; //decrement
	if(subTimeEETimer == 1) //at 1, just before bottoming out
		subTimeEEFlag = 1; //mark to write when timing allows

	if(offsetEETimer > 0) //if above zero
		offsetEETimer--; //decrement
	if(offsetEETimer == 1) //at 1, just before bottoming out
		offsetEEFlag = 1; //mark to write when timing allows

	if(volumeEETimer > 0) //if above zero
		volumeEETimer--; //decrement
	if(volumeEETimer == 1) //at 1, just before bottoming out
		volumeEEFlag = 1; //mark to write when timing allows
}
float softClip(float in, float gain, float makeup)
{
	float out = 0.0f;
	in*=gain;
	if(in <= -1.0)
	{
		out = -TWOTHIRDS;
	}
	else if(in >= 1.0)
	{
		out = TWOTHIRDS;
	}
	else
	{
		out = in - ((in*in*in)/3);
	}
	return out * makeup;
}
float hardClip(float in)
{
	float out = in;
	if(in < -1.0)
	{
		out = -1.0;
	}
	else if(in > 1.0)
	{
		out = 1.0;
	}
	return out;
}
float hardClipThresh(float in, float threshold)
{
	float out = in;
	if(in < -threshold)
	{
		out = -threshold;
	}
	else if(in > threshold)
	{
		out = threshold;
	}
	return out;
}
/*float waveFoldSin(float in, float gain, float makeup)
{
	float saturatedout = tanhf(in * gain);
	float sinfoldedout = sinf(in * gain);
	float out = sinfoldedout;
	return out * makeup;
}*/
void setPWM()
{
	if(onOffState == ON) //if pedal is on
	{
		//if(mixMode == OFF)
			pwmDry = volume * PWMMAX;
		//else
			//pwmDry = PWMMAX;
	}
	else //otherwise
		pwmDry = 0.06f * PWMMAX; //unity volume (in theory, volume is doubled via circuitry so this removes those 6dB at 33mV/dB

	//if(mixMode == TRUEMIX) //if standard mix
		pwmWet = trueDryMix * PWMMAX; //apply mix value
	//else //otherwise
		//pwmWet = volumeAttenuation * PWMMAX; //apply volume straight
		//pwmWet = 0.06f * PWMMAX;

	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwmDry);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, pwmDry);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, pwmWet);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, pwmWet);
}
void setVolumeTarget(float setting, uint8_t source)
{
	float tempSetting = targetSetting[VOLUMEINDEX]; //store the current setting
	volumeSmoother = 0.1f; //start with minimal smoothing, change in later if statement if needed
	if(source != KNOB) //if source wasn't a knob
	{
		potMix.queuePause = 1; //pasue the knob
	}
	targetSetting[VOLUMEINDEX] = setting; //assign setting at end
	if(targetSetting[VOLUMEINDEX] != tempSetting && source != PRESET) //if setting changed, not from a preset switch
		volumeEETimer = EETIMERTOP; //reset timer
}
void setVolume()
{
	currentSetting[VOLUMEINDEX] += volumeSmoother * (targetSetting[VOLUMEINDEX] - currentSetting[VOLUMEINDEX]);
	volumeAttenuation = 1.0f - currentSetting[VOLUMEINDEX]; //max volume is 0.0f
	volumeAttenuation *= 0.12f; //limit range, nuanced approach
}
void setMixTarget(float setting, uint8_t source)
{
	mixSmoother = 0.1f; //start with minimal smoothing, change in later if statement if needed
	if(source != KNOB) //if source wasn't a knob
	{
		potMix.queuePause = 1; //pause pot
	}
	if(source != EXPRESSION && expBuffer[MIXDIRINDEX] != 0) //if source wasn't expression, and expression is active
	{
		potExp.queuePause = 1;
	}
	targetSetting[MIXINDEX] = setting;
}
void setMix()
{
	currentSetting[MIXINDEX] += mixSmoother * (targetSetting[MIXINDEX] - currentSetting[MIXINDEX]);
	//"Mix" is wet volume
	trueDryMix = 1.0f - currentSetting[MIXINDEX]; //max volume is 0.0f
	if(trueDryMix <= 0.5f)
		trueDryMix = 0.0f; //always full until noon //0.06f; //always full until noon
	else if(trueDryMix <= 0.9f)
		trueDryMix = (trueDryMix * 0.3f) - 0.15f;//0.09f;
	else
		trueDryMix = (trueDryMix * 8.33333f) - 7.33333f;//(trueDryMix * 8.2f) - 7.2f;

	//"Volume" is dry volume
	volume = currentSetting[MIXINDEX];
	//updated scaling for mix control at unity volume ----
	if(volume <= 0.5f)
		volume = 0.0f; //always unity until noon
	else if(volume <= 0.9f)
		volume = (volume * 0.3f) - 0.15f;
	else
		volume = (volume * 8.33333f) - 7.33333f;
	//drop volume according to overall attenuation
	trueDryMix+=volumeAttenuation;
	if(trueDryMix > 1.0f)
		trueDryMix = 1.0f;
	volume+=volumeAttenuation;
	if(volume > 1.0f)
		volume = 1.0f;

	//this sets the mix heading into crush
	modifiedMix = 1.0f - currentSetting[MIXINDEX];
	if(modifiedMix <= 0.5f)
		modifiedMix = 1.0f; //always unity until noon
	else
	{
		if(reverseMix <= 0.1f) //if reverse is out of the picture
			modifiedMix = 1.0f - ((modifiedMix-0.5f) * 2.0f); //then down to zero from noon to min
		else //if reverse is in play
			modifiedMix = 1.0f - ((modifiedMix-0.5f) * 2.8f); //accelerate the drop to zero
		if(modifiedMix < 0.0f) //and make sure it stays zero instead of going negative
			modifiedMix = 0.0f;
	}
	modifiedMixInv = currentSetting[MIXINDEX];
	if(modifiedMixInv <= 0.5f)
		modifiedMixInv = 1.0f; //always unity until noon
	else
		modifiedMixInv = 1.0f - ((modifiedMixInv-0.5f) * 2.0f); //then down to zero
	modifiedMix*=0.9f; //attenuate slightly to match volumes better
	modifiedMixInv*=0.9f;

	//for blending dry signal in modified mix when reverse is happening
	modifiedMixReverseDryBlend = currentSetting[MIXINDEX] * 10.0f; //go to 1 at 10% of the sweep
	if(modifiedMixReverseDryBlend > 1.0f)
		modifiedMixReverseDryBlend = 1.0f; //then cap at 1
	if(modifiedMixReverseDryBlend <= 0.5f)
		modifiedMixReverseDryBlend = 1.0f; //always unity until noon
	else
		modifiedMixReverseDryBlend = 1.0f - ((modifiedMixReverseDryBlend-0.5f) * 2.0f); //then down to zero

	modifiedMixReverseDryBlendInv = 1.0f - (currentSetting[MIXINDEX] * 10.0f); //go down to 0 at 10% of the sweep
	if(modifiedMixReverseDryBlendInv < 0.0f)
		modifiedMixReverseDryBlendInv = 0.0f; //then cap at 1
	if(modifiedMixReverseDryBlendInv <= 0.5f)
		modifiedMixReverseDryBlendInv = 1.0f; //always unity until noon
	else
		modifiedMixReverseDryBlendInv = 1.0f - ((modifiedMixReverseDryBlendInv-0.5f) * 2.0f); //then down to zero


}
void setTimeTarget(float setting, uint8_t source)
{
	timeSmoother = 0.005f; //start with minimal smoothing, change in later if statement if needed
	if(source == KNOB || source == MIDI) //if source was a knob or MIDI
	{
		setting = 0.01 + setting * 0.99f; //limit range
		noSubdivisionsTime = 1; //disallow subdivisions
	}
	if(source == AUX) //if tapped tempo
	{
		noSubdivisionsTime = 0; //allow subdivisions
	}
	if(source != KNOB) //if source wasn't a knob
	{
		potTime.queuePause = 1; //pause the knob
	}
	if(source != EXPRESSION && expBuffer[TIMEDIRINDEX] != 0) //if source wasn't expression, and expression is active
	{
		potExp.queuePause = 1;
	}
	targetSetting[TIMEINDEX] = setting;
}
void setTime()
{
	currentSetting[TIMEINDEX] += timeSmoother * (targetSetting[TIMEINDEX] - currentSetting[TIMEINDEX]);
	//modBlend = currentSetting[TIMEINDEX]; //to start, try this as a simple depth modifier - vibrato goes deeper clockwise, tremolo goes deeper counterclockwise; not actually a blend, but maybe interesting?
	modBlend = 0.0f; //fix blend to tremolo, to allow this control as reverb/delay time setter
	timeScaler = currentSetting[TIMEINDEX];
	if(noSubdivisionsTime == 0) //if subdivisions are allowed
		timeScaler*=subTime; //use them
	//uint8_t timeScalerQuantized = timeScaler * FLOAT_TO_UINT8; //quantize to 256 positions
	//timeScaler = timeScalerQuantized * UINT8_TO_FLOAT;
	allpass_setTime(&ap1L, 1.0f + timeScaler * (AP1SIZE - 2.0f));
	allpass_setTime(&ap1R, 1.0f + timeScaler * (AP1SIZE - 2.0f));
	allpass_setTime(&ap2L, 1.0f + timeScaler * (AP2SIZE - 2.0f));
	allpass_setTime(&ap2R, 1.0f + timeScaler * (AP2SIZE - 2.0f));
	allpass_setTime(&ap3L, VERBMODMAX + timeScaler * (AP3SIZE - 2.0f - VERBMODMAX * 2.0f));
	allpass_setTime(&ap3R, VERBMODMAX + timeScaler * (AP3SIZE - 2.0f - VERBMODMAX * 2.0f));
	allpass_setTime(&ap4L, VERBMODMAX + timeScaler * (AP4SIZE - 2.0f - VERBMODMAX * 2.0f));
	allpass_setTime(&ap4R, VERBMODMAX + timeScaler * (AP4SIZE - 2.0f - VERBMODMAX * 2.0f));
	allpass_setTime(&ap5L, 1.0f + timeScaler * (AP5SIZE - 2.0f));
	allpass_setTime(&ap5R, 1.0f + timeScaler * (AP5SIZE - 2.0f));

	//timeScaler = 0.1f;//hard code time for testing purposes

	delay_clocked_setTime(&reverseL, 1.0f + timeScaler * (REVERSESIZE - 1.0f));
	delay_clocked_setTime(&reverseR, 1.0f + timeScaler * (REVERSESIZE - 1.0f));
}
void setFilterTarget(float setting, uint8_t source)
{
	filterSmoother = 0.1f; //start with minimal smoothing, change in later if statement if needed
	if(source != KNOB) //if source wasn't a knob
	{
		potFilter.queuePause = 1; //pause the knob
	}
	if(source != EXPRESSION && expBuffer[FILTERDIRINDEX] != 0) //if source wasn't expression, and expression is active
	{
		potExp.queuePause = 1;
	}
	targetSetting[FILTERINDEX] = setting;
}
void setFilter()
{
	currentSetting[FILTERINDEX] += filterSmoother * (targetSetting[FILTERINDEX] - currentSetting[FILTERINDEX]);
	filterCutoff = currentSetting[FILTERINDEX]; //max volume is 0.0f
	statevariable_setFC(&leftOutLPF, filterCutoff * filterCutoff * 13000.0f);
	statevariable_setFC(&rightOutLPF, filterCutoff * filterCutoff * 13000.0f);
	statevariable_setFC(&leftOutHPF, (1.0f-filterCutoff) * 2000.0f);
	statevariable_setFC(&rightOutHPF, (1.0f-filterCutoff) * 2000.0f);

}
void setChanceTarget(float setting, uint8_t source)
{
	chanceSmoother = .1f; //start with minimal smoothing, change in later if statement if needed
	if(source != KNOB) //if source wasn't a knob
	{
		potChance.queuePause = 1; //pause the knob
	}
	if(source != EXPRESSION && expBuffer[CHANCEDIRINDEX] != 0) //if source wasn't expression, and expression is active
	{
		potExp.queuePause = 1; //pause expression
	}

	targetSetting[CHANCEINDEX] = setting; //at the end, set the target to the manipulated setting
}
void setChance()
{
	currentSetting[CHANCEINDEX] += chanceSmoother * (targetSetting[CHANCEINDEX] - currentSetting[CHANCEINDEX]);
	if(currentSetting[CHANCEINDEX] > 0.47f && currentSetting[CHANCEINDEX] < 0.53f)
		onlyInput = ON;
	else
		onlyInput = OFF;
	thresholdRandomInput = 1.0f - (currentSetting[CHANCEINDEX] * 2.2f); //bring random input threshold down to zero at noon
	if(thresholdRandomInput < 0.0f)
		thresholdRandomInput = 0.0f; //probably not necessary, but nice to be explicit
	//thresholdRandomFeedback = (1.0f - currentSetting[CHANCEINDEX]) * 2.2222f; //this should make the random feedback threshold >= 1 until just above noon, then bring it down to zero
	thresholdRandomFeedback = (1.0f - currentSetting[CHANCEINDEX]) * 2.2222f;
	if(currentSetting[CHANCEINDEX] <= 0.98f)
		maxSurpriseFeedback = 1.0f; //keep feedback as a looper up until top 2% (ish - this is dependent on where randomThresholdFeedback hits 0
	else
		maxSurpriseFeedback = 1.02f; //then allow self-oscillation

	//if(thresholdRandomInput > 0.95f) //if our threshold has just been set to only a five (or less) percent chance
	//{
		//surpriseInputVolLTarget = 0.0f; //hard stop the input
		//surpriseInputVolRTarget = 0.0f;
	//}

	//envelope gain
	envGain = currentSetting[CHANCEINDEX] * 70.0f;//20.0f;
}
void setSmearTarget(float setting, uint8_t source)
{
	smearSmoother = .1f; //start with minimal smoothing, change in later if statement if needed
	if(source != KNOB) //if source wasn't a knob
	{
		potSmear.queuePause = 1; //pause the knob
	}
	if(source != EXPRESSION && expBuffer[SMEARDIRINDEX] != 0) //if source wasn't expression, and expression is active
	{
		potExp.queuePause = 1; //pause expression
	}
	targetSetting[SMEARINDEX] = setting; //at the end, set the target to the manipulated setting
}
void setSmear()
{
	currentSetting[SMEARINDEX] += smearSmoother * (targetSetting[SMEARINDEX] - currentSetting[SMEARINDEX]);
	reverbMix = currentSetting[SMEARINDEX];
	reverbFeedbackAmount = currentSetting[SMEARINDEX] * 0.98f;
	dwell = currentSetting[SMEARINDEX];
	allpass_setK(&ap1L, dwell*K1);
	allpass_setK(&ap2L, dwell*K1);
	allpass_setK(&ap3L, dwell*K1);
	allpass_setK(&ap4L, dwell*K2);
	allpass_setK(&ap5L, dwell*K2);
	allpass_setK(&ap1R, dwell*K1);
	allpass_setK(&ap2R, dwell*K1);
	allpass_setK(&ap3R, dwell*K1);
	allpass_setK(&ap4R, dwell*K2);
	allpass_setK(&ap5R, dwell*K2);
}
void setGlitchTarget(float setting, uint8_t source)
{
	glitchSmoother = 0.1f; //start with minimal smoothing, change in later if statement if needed
	if(source != KNOB) //if source wasn't a knob
	{
		potGlitch.queuePause = 1; //pause the knob
	}
	if(source != EXPRESSION && expBuffer[GLITCHDIRINDEX] != 0) //if source wasn't expression, and expression is active
	{
		potExp.queuePause = 1; //pause expression
	}
	targetSetting[GLITCHINDEX] = setting;
}
void setGlitch()
{
	currentSetting[GLITCHINDEX] += glitchSmoother * (targetSetting[GLITCHINDEX] - currentSetting[GLITCHINDEX]);
	thresholdRandomSub = (1.0f - currentSetting[GLITCHINDEX]) * 2.0f; //use glitch 3 to potentially trigger random subdivisions
	if(thresholdRandomSub > 1.0f) //if no new randomness is allowed, set to a specific subdivision
	{
		if(thresholdRandomSub > 1.6666f)
		{
			randomModSub = 2.0f;
			randomTimeSub = 1.0f;
		}
		else if(thresholdRandomSub > 1.3333f)
		{
			randomModSub = 1.0f;
			randomTimeSub = 0.5f;
		}
		else
		{
			randomModSub = 0.5f;
			randomTimeSub = 0.25f;
		}
		//float temprandomTimeSubF = (thresholdRandomSub - 1.0f) * 16.0f;
		//int16_t temprandomTimeSub = temprandomTimeSubF;
		//randomTimeSub = tempandomTimeSub / 16.0f;
		randomTimeSubLinear = thresholdRandomSub - 1.0f;
	}

	delay_parting_setClockSpeed(&ap1L.memory, randomTimeSub*footswitchDouble);
	delay_parting_setClockSpeed(&ap2L.memory, randomTimeSub*footswitchDouble);
	delay_parting_setClockSpeed(&ap3L.memory, randomTimeSub*footswitchDouble);
	delay_parting_setClockSpeed(&ap4L.memory, randomTimeSub*footswitchDouble);
	delay_parting_setClockSpeed(&ap5L.memory, randomTimeSub*footswitchDouble);
	delay_parting_setClockSpeed(&ap1R.memory, randomTimeSub*footswitchDouble);
	delay_parting_setClockSpeed(&ap2R.memory, randomTimeSub*footswitchDouble);
	delay_parting_setClockSpeed(&ap3R.memory, randomTimeSub*footswitchDouble);
	delay_parting_setClockSpeed(&ap4R.memory, randomTimeSub*footswitchDouble);
	delay_parting_setClockSpeed(&ap5R.memory, randomTimeSub*footswitchDouble);
}
void setRateTarget(float setting, uint8_t source)
{
	rateSmoother = 0.01f; //start with minimal smoothing, change in later if statement if needed
	if(source == KNOB || MIDI) //if source was a knob or MIDI
	{
		setting = 0.01f + setting * 0.99f;
	}
	if(source != KNOB) //if source wasn't a knob
	{
		potRate.queuePause = 1; //pause the knob
	}
	if(source != EXPRESSION && expBuffer[RATEDIRINDEX] != 0) //if source wasn't expression, and expression is active
	{
		potExp.queuePause = 1; //pause expression
	}
	targetSetting[RATEINDEX] = setting;
}
void setRate()
{
	currentSetting[RATEINDEX] += rateSmoother * (targetSetting[RATEINDEX] - currentSetting[RATEINDEX]);
	modRate = currentSetting[RATEINDEX] * MAXRATE;
	//subdivide by random
	modRate*=randomModSub;
	//and by actual subdivisions
	modRate*=subRate;
	//and by footswitch
	modRate*=footswitchDouble;
}
void setDepthTarget(float setting, uint8_t source)
{
	depthSmoother = .01f; //start with minimal smoothing
	if(source != KNOB) //if source wasn't a knob
	{
		potDepth.queuePause = 1; //pause the knob
	}
	if(source != EXPRESSION && expBuffer[DEPTHDIRINDEX] != 0) //if source wasn't expression, and expression is active
	{
		potExp.queuePause = 1; //pause expression
	}
	targetSetting[DEPTHINDEX] = setting;
}
void setDepth()
{
	currentSetting[DEPTHINDEX] += depthSmoother * (targetSetting[DEPTHINDEX] - currentSetting[DEPTHINDEX]);
	modDepth = currentSetting[DEPTHINDEX];

	if(modDepth < SPEEDSEGMENT) //ocatve down
		modDepth = 1.5f;
	else if (modDepth < SPEEDSEGMENT * 2.0f) //fifth down
		modDepth = 1.6666666f;
	else if (modDepth < SPEEDSEGMENT * 3.0f) //fourth down
		modDepth = 1.75f;
	else if (modDepth < SPEEDSEGMENT * 4.0f) //unison
		modDepth = 2.0f;
	else if (modDepth < SPEEDSEGMENT * 5.0f) //fourth up
		modDepth = 2.3333333f;
	else if (modDepth < SPEEDSEGMENT * 6.0f) //fifth up
		modDepth = 2.5f;
	else //octave up
		modDepth = 3.0f;

	delay_clocked_setDirection(&reverseL, modDepth);
	delay_clocked_setDirection(&reverseR, modDepth);
}

void setSubRateTarget(float setting, uint8_t source)
{
	float tempSetting = targetSetting[SUBRATEINDEX]; //store the current setting
	subRateSmoother = 1.0f; //start with minimal smoothing, change in later if statement if needed
	if(source == KNOB || source == MIDI) //if source was a knob or MIDI
	{
		noSubdivisionsRate = 0; //allow subdivisions
		//note these multiplications by 0.25f - this is to keep everything at or below one, for proper EEPROM storage
		//subRate will then be multiplied by 4.0f to make up for it
		if(setting < SUBSEGMENTRATE)
			setting = 0.25f * 0.25f; //whole note
		else if(setting < SUBSEGMENTRATE * 2.0f)
			setting = 0.3333333f * 0.25f; //triplet of some sort?
		else if(setting < SUBSEGMENTRATE * 3.0f)
			setting = 0.5f * 0.25f; //half note
		else if(setting < SUBSEGMENTRATE * 4.0f)
			setting = 1.0f * 0.25f; //quarter
		else if(setting < SUBSEGMENTRATE * 5.0f)
			setting = 2.0f * 0.25f; //eighth
		else if(setting < SUBSEGMENTRATE * 6.0f)
			setting = 3.0f * 0.25f; //triplet?
		else
			setting = 4.0f * 0.25f; //sixteenth
	}
	if(source != KNOB) //if source wasn't a knob
	{
		potRate.queuePause = 1; //pause the knob
	}
	targetSetting[SUBRATEINDEX] = setting; //then update the current setting
	if(targetSetting[SUBRATEINDEX] != tempSetting && source != PRESET) //if setting changed, not from a preset switch
		subRateEETimer = EETIMERTOP; //reset timer
}
void setSubRate()
{
	currentSetting[SUBRATEINDEX] += subRateSmoother * (targetSetting[SUBRATEINDEX] - currentSetting[SUBRATEINDEX]);
	subRate = 4.0f * currentSetting[SUBRATEINDEX];
}

void setSubTimeTarget(float setting, uint8_t source)
{
	float tempSetting = targetSetting[SUBTIMEINDEX]; //store the current setting
	subTimeSmoother = 1.0f; //start with minimal smoothing, change in later if statement if needed
	if(source == KNOB || source == MIDI) //if source was a knob or MIDI
	{
		noSubdivisionsTime = 0; //allow subdivisions
		if(setting < SUBSEGMENTTIME)
			setting = 0.25f; //sixteenth
		else if(setting < SUBSEGMENTTIME * 2.0f)
			setting = 0.375f;
		else if(setting < SUBSEGMENTTIME * 3.0f)
			setting = 0.5f;
		else if(setting < SUBSEGMENTTIME * 4.0f)
			setting = 0.66f;
		else if(setting < SUBSEGMENTTIME * 5.0f)
			setting = 0.75f;
		else
			setting = 1.0f;
	}
	if(source != KNOB) //if source wasn't a knob
	{
		potTime.queuePause = 1;
	}
	targetSetting[SUBTIMEINDEX] = setting; //then update the current setting
	if(targetSetting[SUBTIMEINDEX] != tempSetting && source != PRESET) //if setting changed, not from a preset switch
		subTimeEETimer = EETIMERTOP; //reset timer
}
void setSubTime()
{
	currentSetting[SUBTIMEINDEX] += subTimeSmoother * (targetSetting[SUBTIMEINDEX] - currentSetting[SUBTIMEINDEX]);
	subTime = currentSetting[SUBTIMEINDEX];
}

void setDissolveTarget(float setting, uint8_t source)
{
	dissolveSmoother = 0.01f; //start with minimal smoothing, change in later if statement if needed
	if(source != KNOB) //if source wasn't a knob
	{
		potDissolve.queuePause = 1; //pause the knob
	}
	else //if it was a knob
	{
		setting*=setting; //shape the tape
	}
	if(source != EXPRESSION && expBuffer[DISSOLVEDIRINDEX] != 0) //if source wasn't expression, and expression is active
	{
		potExp.queuePause = 1; //pause expression
	}
	targetSetting[DISSOLVEINDEX] = setting;
}
void setDissolve()
{
	currentSetting[DISSOLVEINDEX] += dissolveSmoother * (targetSetting[DISSOLVEINDEX] - currentSetting[DISSOLVEINDEX]);
	//sample rate setting for tim14
	/*virtualSampleRate = MINSAMPLERATE + currentSetting[DISSOLVEINDEX] * currentSetting[DISSOLVEINDEX] * SAMPLERATE;// * virtualSampleRateLimiter; //add minimum rate in, then multiply by some maximum, allow the random subdivision, as well as the switching doubler/normaler
	virtualSampleRate = (uint32_t)virtualSampleRate; //remove any fractional part
	timerPeriodF = (194000000.0f / 32.0f) / virtualSampleRate; //divide timer by prescaler, then by sample rate, to get period
	timerPeriod = timerPeriodF; //truncate to uint16_t

	//make sample rate incrementer match the relationship between virtual and real sample rates
	reverseClock = virtualSampleRate / SAMPLERATE;
	if(reverseClock > 1.0f)
		reverseClock = 1.0f;*/

	reverseClock = currentSetting[DISSOLVEINDEX];
	if(reverseClock < 0.1f) //if clock is less than 480 Hz
		reverseClock = 0.1f; //prevent it from going further


	//set clock speeds
		/*if(currentSetting[DISSOLVEINDEX] < (0.5f + REVERSESEGMENT * 1.0f))
		{
			reverseClock = 1.0f;
		}
		else if(currentSetting[DISSOLVEINDEX] < (0.5f + REVERSESEGMENT * 2.0f))
		{
			reverseClock = 0.5f;
		}
		else if(currentSetting[DISSOLVEINDEX] < (0.5f + REVERSESEGMENT * 3.0f))
		{
			reverseClock = 0.25f;
		}
		else if(currentSetting[DISSOLVEINDEX] < (0.5f + REVERSESEGMENT * 4.0f))
		{
			reverseClock = 0.125;
		}
		else if(currentSetting[DISSOLVEINDEX] < (0.5f + REVERSESEGMENT * 5.0f))
		{
			reverseClock = 0.0625;
		}
		else
		{
			reverseClock = 0.03125;
		}*/


	//reverseClock = 0.1111f;

	delay_clocked_setClockSpeed(&reverseL, reverseClock);
	delay_clocked_setClockSpeed(&reverseR, reverseClock);

}
void setShapeTarget(float setting, uint8_t source)
{

	shapeSmoother = 0.005f; //start with minimal smoothing, change in later if statement if needed
	if(source != KNOB) //if source wasn't a knob
	{
		potShape.queuePause = 1; //pause the knob
	}
	if(source != EXPRESSION && expBuffer[SHAPEDIRINDEX] != 0) //if source wasn't expression, and expression is active
	{
		potExp.queuePause = 1; //pause expression
	}
	targetSetting[SHAPEINDEX] = setting;
}
void setShape()
{
	currentSetting[SHAPEINDEX] += shapeSmoother * (targetSetting[SHAPEINDEX] - currentSetting[SHAPEINDEX]);
	if(currentSetting[SHAPEINDEX] < SHAPESEGMENT * 1.0f)
		shape = SINE;
	else if(currentSetting[SHAPEINDEX] < SHAPESEGMENT * 2.0f)
		shape = SQUARE;
	else if(currentSetting[SHAPEINDEX] < SHAPESEGMENT * 3.0f)
		shape = REVERSESAW;
	else if(currentSetting[SHAPEINDEX] < SHAPESEGMENT * 4.0f)
		shape = SAW;
	else if(currentSetting[SHAPEINDEX] < SHAPESEGMENT * 5.0f)
		shape = RANDOMSINE;
	else if(currentSetting[SHAPEINDEX] < SHAPESEGMENT * 6.0f)
		shape = RANDOMSQUARE;
	else
		shape = ENVELOPE;
	if(shape != ENVELOPE)
	{
		//make sure, if moving off envelope mode, envelopes are turned off
		envelopeStatusL = OFF;
		envelopeStatusR = OFF;
	}
}
void setOffsetTarget(float setting, uint8_t source)
{
	float tempSetting = targetSetting[OFFSETINDEX]; //store the current setting
	offsetSmoother = 0.01f; //start with minimal smoothing, change in later if statement if needed
	if(source != KNOB)
	{
		potDepth.queuePause = 1;
	}
	targetSetting[OFFSETINDEX] = setting;
	if(targetSetting[OFFSETINDEX] != tempSetting && source != PRESET) //if setting changed, not from a preset switch
		offsetEETimer = EETIMERTOP; //reset timer
}
void setOffset()
{
	currentSetting[OFFSETINDEX] += offsetSmoother * (targetSetting[OFFSETINDEX] - currentSetting[OFFSETINDEX]);
	//if(currentSetting[OFFSETINDEX] < 0.5f)
		//offset = -1.0f;
	//else
		//offset = 1.0f;
	offset = currentSetting[OFFSETINDEX] * 0.5f;
}
void setExp(float setting, uint8_t source)
{
	if((scoochAsTap == 0 && source == EXPRESSION) || source == MIDI) //if called by MIDI, or by expression port when it isn't assigned to tap tempo
	{
		//rate
		if(expBuffer[RATEDIRINDEX] != OFF)
			setRateTarget((expBuffer[RATEHEELINDEX] + (setting*(expBuffer[RATETOEINDEX]-expBuffer[RATEHEELINDEX]))), EXPRESSION);
		//depth
		if(expBuffer[DEPTHDIRINDEX] != OFF)
			setDepthTarget((expBuffer[DEPTHHEELINDEX] + (setting*(expBuffer[DEPTHTOEINDEX]-expBuffer[DEPTHHEELINDEX]))), EXPRESSION);
		//shape
		if(expBuffer[SHAPEDIRINDEX] != OFF)
			setShapeTarget((expBuffer[SHAPEHEELINDEX] + (setting*(expBuffer[SHAPETOEINDEX]-expBuffer[SHAPEHEELINDEX]))), EXPRESSION);
		//dissolve
		if(expBuffer[DISSOLVEDIRINDEX] != OFF)
			setDissolveTarget((expBuffer[DISSOLVEHEELINDEX] + (setting*(expBuffer[DISSOLVETOEINDEX]-expBuffer[DISSOLVEHEELINDEX]))), EXPRESSION);
		//chance
		if(expBuffer[CHANCEDIRINDEX] != OFF)
			setChanceTarget((expBuffer[CHANCEHEELINDEX] + (setting*(expBuffer[CHANCETOEINDEX]-expBuffer[CHANCEHEELINDEX]))), EXPRESSION);
		//smear
		if(expBuffer[SMEARDIRINDEX] != OFF)
			setSmearTarget((expBuffer[SMEARHEELINDEX] + (setting*(expBuffer[SMEARTOEINDEX]-expBuffer[SMEARHEELINDEX]))), EXPRESSION);
		//glitch
		if(expBuffer[GLITCHDIRINDEX] != OFF)
			setGlitchTarget((expBuffer[GLITCHHEELINDEX] + (setting*(expBuffer[GLITCHTOEINDEX]-expBuffer[GLITCHHEELINDEX]))), EXPRESSION);
		//time
		if(expBuffer[TIMEDIRINDEX] != OFF)
			setTimeTarget((expBuffer[TIMEHEELINDEX] + (setting*(expBuffer[TIMETOEINDEX]-expBuffer[TIMEHEELINDEX]))), EXPRESSION);
		//filter
		if(expBuffer[FILTERDIRINDEX] != OFF)
			setFilterTarget((expBuffer[FILTERHEELINDEX] + (setting*(expBuffer[FILTERTOEINDEX]-expBuffer[FILTERHEELINDEX]))), EXPRESSION);
		//mix
		if(expBuffer[MIXDIRINDEX] != OFF)
			setMixTarget((expBuffer[MIXHEELINDEX] + (setting*(expBuffer[MIXTOEINDEX]-expBuffer[MIXHEELINDEX]))), EXPRESSION);
	}
}

void registerTap(uint8_t source)
{
	potRate.pauseReading = 1; //pause rate pot, at least for now
	tapResetTimer = MAXTAP; //start the reset timer, when this gets down to zero in the millis loop, tap num will be set back to zero
	tapResetTimerSamples = MAXTAPSAMPLES;
	if(tapNum < 2) //this creates essentially an on/off, where tapNum is 0 or 1, useful for now
		tapNum++; //increment number of taps
	tapMillis = (float)sampleBasedMillis / (SAMPLERATE / 1000.0f); //millis; //get current millis
	diffMillis = tapMillis-lastTapMillis; //and difference between that and the last one
	if(diffMillis < 0) //if negative, it wrapped around between taps
		diffMillis += UINT32_TMAXF;
		//diffMillis+=65535.0f;
	if(tapNum > 1 && source == lastTapSource) //if we have two taps from the same source, it's enough to set tempo
	{
		/*if((tapAverage/diffMillis > 0.90f) && (tapAverage/diffMillis < 1.1f)) //if it's within 10%
			tapAverage = (tapAverage + diffMillis) * 0.5f; //average it
		else //otherwise*/
			tapAverage = diffMillis; //set new average
		setRateTarget((1.0f/(tapAverage * 0.001f * MAXRATE)), AUX); //set rate based on millis converted to hertz then applied within the available range
		//tapAverageDelayScaled = (tapAverage * 0.001f * SAMPLERATE) / APSIZETOTAL; //get delay based on seconds times sample rate divided by total size available
		tapAverageDelayScaled = ((tapAverage * 0.001f * reverseClock * SAMPLERATE) / REVERSESIZE) * 4.0f; //multiplied by 4 to make up for the reverse only being quarter time (note: still need to account for speed settings)
		if(tapAverageDelayScaled > 1.0f) //if the tap doesn't fit into the available delay
		{
			setDissolveTarget(1.0f / tapAverageDelayScaled, AUX); //limit the clock to the highest possible sample rate allowing this delay time
			tapAverageDelayScaled = 1.0f; //then set the time to 1
		}
		setTimeTarget(tapAverageDelayScaled, AUX);//set new time target,
	}
	/*else //if it's an individual moment of tap, not enough to set tempo
	{
		sinSweepL = 0; //still reset the lfo sweeper, to trigger new cycle every time footswitch is pressed
	}*/
	if(tapNum == 1) //if first tap
		tapNumReset = 0.0f; //trigger the LFO reset
	else //if subsequent taps
	{
		tapNumReset+=(subRate*randomModSub*footswitchDouble*1.1); //increment by subdivisions, scaled up slightly (since triplet subdivisions wouldn't quite hit 1 otherwise)
		if(tapNumReset >= 1.0f) //if it's hit 1
			{
			if(tapNumReset > 1.3f) //if it's the .666 + .666 scenario
				tapNumReset-=1.0f; //clear the 1
			else //otherwise
				tapNumReset = 0.0f; //trigger the LFO reset
			}
	}
	if(tapNumReset == 0.0f) //if we need to trigger an LFO reset
	{
		sinSweepL = 0.0f; //do that
	}
	lastTapMillis = tapMillis; //store both current millis in last millis setting
	lastTapSource = source; //mark the source of this tap
}

//function to grab all random values (triggered by glitch knob)
void grabNewRandoms()
{
	//grab first random, for left input
	float random = (float)rand()/(float)RAND_MAX;
	if(shape != ENVELOPE)//envelopeStatusL == OFF) //if envelope is off, do normal randomness
	{
		if(random >= thresholdRandomInput) //if we want some input
		{
			if(thresholdRandomFeedback >= 1.1f) //if glitch is set before noon
			{
				random = (float)rand()/(float)RAND_MAX; //grab a new random
				if(random >= 0.5f) //most of the time
				{
					surpriseInputVolLTarget = 1.0f; //send some new info into the buffer
					surpriseFeedbackAmountLTarget = 0.0f; //and stop feedback
				}
				else //the other third
				{
					surpriseInputVolLTarget = 0.0f; //stop new info into the buffer
					surpriseFeedbackAmountLTarget = 1.0f; //and loop what's there
				}
			}
			else //otherwise
			{
				surpriseInputVolLTarget = 1.0f; //always send new info into buffer
				if(random >= thresholdRandomFeedback) //if random value is above feedback threshold
					surpriseFeedbackAmountLTarget = 1.0f; //loop what's there
				else
					surpriseFeedbackAmountLTarget = 0.0f; //don't loop what's there
			}
		}
		else
		{
			surpriseInputVolLTarget = 0.0f; //no new info to the buffer
			surpriseFeedbackAmountLTarget = 0.0f; //and no old info
		}
	}
	else if(envelopeStatusL == ON)//if envelope is on
	{
		if (random > 0.666f) //some of the time, randomly
		{
			surpriseInputVolLTarget = 0.0f; //stop new info
			surpriseFeedbackAmountLTarget = 1.0f; //and loop old info
		}
		else //the other half, randomly, maintain envelope setting
		{
			surpriseInputVolLTarget = 1.0f; //allow new info
			surpriseFeedbackAmountLTarget = 0.0f; //and stop old info
		}
	}
	//grab second random, for right input
	random = (float)rand()/(float)RAND_MAX;
	if(shape != ENVELOPE)//envelopeStatusR == OFF) //if envelope is off, normal randomness
	{
		if(random >= thresholdRandomInput) //if we want some input
		{
			if(thresholdRandomFeedback >= 1.1f) //if glitch is set before noon
			{
				random = (float)rand()/(float)RAND_MAX; //grab a new random
				if(random >= 0.5f) //most of the time
				{
					surpriseInputVolRTarget = 1.0f; //send some new info into the buffer
					surpriseFeedbackAmountRTarget = 0.0f; //and stop feedback
				}
				else //the other third
				{
					surpriseInputVolRTarget = 0.0f; //stop new info into the buffer
					surpriseFeedbackAmountRTarget = 1.0f; //and loop what's there
				}
			}
			else //otherwise
			{
				surpriseInputVolRTarget = 1.0f; //always send new info into buffer
				if(random >= thresholdRandomFeedback) //if random value is above feedback threshold
					surpriseFeedbackAmountRTarget = 1.0f; //loop what's there
				else
					surpriseFeedbackAmountRTarget = 0.0f; //don't loop what's there
			}
		}
		else
		{
			surpriseInputVolRTarget = 0.0f; //no new info to the buffer
			surpriseFeedbackAmountRTarget = 0.0f; //and no old info
		}
	}
	else if(envelopeStatusR == ON) //if envelope is on
	{
		if (random > 0.666f) //some of the time, randomly
		{
			surpriseInputVolRTarget = 0.0f; //stop new info
			surpriseFeedbackAmountRTarget = 1.0f; //and loop old info
		}
		else //the other half, randomly, maintain envelope setting
		{
			surpriseInputVolRTarget = 1.0f; //allow new info
			surpriseFeedbackAmountRTarget = 0.0f; //and stop old info
		}
	}
	random = (float)rand()/(float)RAND_MAX;
	if(random >= thresholdRandomSub) //if subdivision change is triggered
	{
		random = (float)rand()/(float)RAND_MAX; //grab a *new* random value
		if(random < 0.3333f)
		{
			randomModSub = 2.0f;
			if(randomTimeSub != 0.25f && lastRandomTimeSubMotion >= 0.0f) //if it wouldn't be a two octave jump, and it didn't just jump upward
			{
				lastRandomTimeSubMotion = randomTimeSub - 1.0f;
				randomTimeSub = 1.0f; //go to 1
			}
		}
		else if(random > 0.6666f)
		{
			randomModSub = 1.0f;
			lastRandomTimeSubMotion = randomTimeSub - 0.5f;
			randomTimeSub = 0.5f;
		}
		else
		{
			randomModSub = 0.5f;
			lastRandomTimeSubMotion = randomTimeSub - 0.25f;
			randomTimeSub = 0.25f;
		}
	}
	random = (float)rand()/(float)RAND_MAX;
	if(random >= thresholdRandomSub) //if subdivision change is triggered
	{
		random = (float)rand()/(float)RAND_MAX; //grab a *new* random value
		if(random < 0.3333f)
		{
			samplerateSub = 2.0f;
		}
		else if(random > 0.6666f)
		{
			samplerateSub = 1.0f;
		}
		else
		{
			samplerateSub = 0.5f;
		}
	}
	else
		samplerateSub = 1.0f;
}
//function for envelope engagement
void envelopeOnL()
{
	envelopeStatusL = ON; //turn envelope status on
	surpriseInputVolLTarget = 1.0f; //open the buffer
	surpriseFeedbackAmountLTarget = 0.0f; //stop any feedback
	envelopeModifierL = 0.5f; //halve everything affected by the envelope
	led_on(&ledAux); //turn the led on
}
void envelopeOffL()
{
	envelopeStatusL = OFF; //turn envelope status off
	surpriseInputVolLTarget = 0.0f; //close the buffer
	surpriseFeedbackAmountLTarget = 0.0f; //stop any feedback
	envelopeModifierL = 1.0f; //return everything to normal
	led_off(&ledAux); //turn the led off
}
void envelopeOnR()
{
	envelopeStatusR = ON; //turn envelope status on
	surpriseInputVolRTarget = 1.0f; //open the buffer
	surpriseFeedbackAmountRTarget = 0.0f; //stop any feedback
	envelopeModifierR = 0.5f; //halve everything affected by the envelope
	led_on(&ledAux); //turn the led on
}
void envelopeOffR()
{
	envelopeStatusR = OFF; //turn envelope status off
	surpriseInputVolRTarget = 0.0f; //close the buffer
	surpriseFeedbackAmountRTarget = 0.0f; //stop any feedback
	envelopeModifierR = 1.0f; //return everything to normal
	led_off(&ledAux); //turn the led off
}
void setModes()
{
	  if((modeBinary & MIXMODEMODIFIED) == MIXMODEMODIFIED) //if the bit for time is high
		  mixMode = MODMIX;
	  else
		  mixMode = TRUEMIX;
	  if((modeBinary & MODMODEVIBRATO) == MODMODEVIBRATO) //if the bit for time is high
		  modMode = VIBRATO;
	  else
		  modMode = TREMOLO;
	  if((modeBinary & FILTERMODEHIGHPASS) == FILTERMODEHIGHPASS) //if the bit for time is high
		  filterMode = HIGHPASS;
	  else
		  filterMode = LOWPASS;
	  led_set(&ledTime, modMode);
	  led_set(&ledFilter, filterMode);
	  led_set(&ledMix, mixMode);
}
void loadPreset(float* data, uint8_t slot)
{
	  if(slot == 0) //if live mode
	  {
		  //mark sure alt-setting pots aren't queued up to pause from a tap-as-preset switch
		  potDepth.queuePause = 0;
		  potRate.queuePause = 0;
		  potTime.queuePause = 0;
		  potMix.queuePause = 0;
		  //unpause all pots to match their readings
		  potRate.pauseReading = 0;
		  potDepth.pauseReading = 0;
		  potShape.pauseReading = 0;
		  potDissolve.pauseReading = 0;
		  potChance.pauseReading = 0;
		  potSmear.pauseReading = 0;
		  potGlitch.pauseReading = 0;
		  potTime.pauseReading = 0;
		  potFilter.pauseReading = 0;
		  potMix.pauseReading = 0;
		  readByte(&tempByte, SUBRATEINDEX);
		  setSubRateTarget((float)tempByte * UINT8_TO_FLOAT, PRESET);
		  readByte(&tempByte, SUBTIMEINDEX);
		  setSubTimeTarget((float)tempByte * UINT8_TO_FLOAT, PRESET);
		  readByte(&tempByte, OFFSETINDEX);
		  setOffsetTarget((float)tempByte * UINT8_TO_FLOAT, PRESET);
		  readByte(&tempByte, VOLUMEINDEX);
		  setVolumeTarget((float)tempByte * UINT8_TO_FLOAT, PRESET);
		  potRate.queuePause = 0;
		  potRate.pauseReading = 0; //unpause the pot, to reverse the pausing that happens when setting subs from a preset
		  potDepth.queuePause = 0;
		  potDepth.pauseReading = 0;
		  potTime.queuePause = 0;
		  potTime.pauseReading = 0;
		  potMix.queuePause = 0;
		  potMix.pauseReading = 0;
		  //pull expression info from slot 0
		  readPreset(expBuffer, 0);
	  }
	  else //otherwise, load preset
	  {
		  //start by pausing all the pots
		  potRate.pauseReading = 1;
		  potDepth.pauseReading = 1;
		  potShape.pauseReading = 1;
		  potDissolve.pauseReading = 1;
		  potChance.pauseReading = 1;
		  potSmear.pauseReading = 1;
		  potGlitch.pauseReading = 1;
		  potTime.pauseReading = 1;
		  potFilter.pauseReading = 1;
		  potMix.pauseReading = 1;
		  potExp.pauseReading = 1;
		  readPreset(presetRead, slot); //read info from EEPROM at preset location
		  setRateTarget(presetRead[RATEINDEX], PRESET);
		  setDepthTarget(presetRead[DEPTHINDEX], PRESET);
		  setShapeTarget(presetRead[SHAPEINDEX], PRESET);
		  setDissolveTarget(presetRead[DISSOLVEINDEX], PRESET);
		  setChanceTarget(presetRead[CHANCEINDEX], PRESET);
		  setSmearTarget(presetRead[SMEARINDEX], PRESET);
		  setGlitchTarget(presetRead[GLITCHINDEX], PRESET);
		  setTimeTarget(presetRead[TIMEINDEX], PRESET);
		  setFilterTarget(presetRead[FILTERINDEX], PRESET);
		  setMixTarget(presetRead[MIXINDEX], PRESET);
		  setOffsetTarget(presetRead[OFFSETINDEX], PRESET);
		  setSubRateTarget(presetRead[SUBRATEINDEX], PRESET);
		  setSubTimeTarget(presetRead[SUBTIMEINDEX], PRESET);
		  setVolumeTarget(presetRead[VOLUMEINDEX], PRESET);
		  for(int n = EXPRESSIONSTART; n < PRESETARRAYLENGTH; n++)
			  expBuffer[n] = presetRead[n];
	  }
	  writeByte(slot, CURRENTPRESETINDEX); //store which preset we're on, to recall at powerup
}

void updateControls()
{
	//set new targets
	if(potMix.pauseReading == 0 && fsAux.value == 0)
		setVolumeTarget(ADC_TO_FLOAT * potMix.old, KNOB);
	else if(potMix.pauseReading == 0 && fsAux.value == 1)
		setMixTarget(ADC_TO_FLOAT * potMix.old, KNOB);
	if(potTime.pauseReading == 0 && fsAux.value == 1)
		setTimeTarget(ADC_TO_FLOAT * potTime.old, KNOB);
	else if(potTime.pauseReading == 0 && fsAux.value == 0)
		setSubTimeTarget(ADC_TO_FLOAT * potTime.old, KNOB);
	if(potFilter.pauseReading == 0)
		setFilterTarget(ADC_TO_FLOAT * potFilter.old, KNOB);
	if(potRate.pauseReading == 0 && fsAux.value == 1)
		setRateTarget(ADC_TO_FLOAT * potRate.old, KNOB);
	else if(potRate.pauseReading == 0 && fsAux.value == 0)
		setSubRateTarget(ADC_TO_FLOAT * potRate.old, KNOB);
	if(potDepth.pauseReading == 0 && fsAux.value == 1)
		setDepthTarget(ADC_TO_FLOAT * potDepth.old, KNOB);
	else if(potDepth.pauseReading == 0 && fsAux.value == 0)
		setOffsetTarget(ADC_TO_FLOAT * potDepth.old, KNOB);
	if(potShape.pauseReading == 0)
		setShapeTarget(ADC_TO_FLOAT * potShape.old, KNOB);
	if(potDissolve.pauseReading == 0)
		setDissolveTarget(ADC_TO_FLOAT * potDissolve.old, KNOB);
	if(potChance.pauseReading == 0)
		setChanceTarget(ADC_TO_FLOAT * potChance.old, KNOB);
	if(potSmear.pauseReading == 0)
		setSmearTarget(ADC_TO_FLOAT * potSmear.old, KNOB);
	if(potGlitch.pauseReading == 0)
		setGlitchTarget(ADC_TO_FLOAT * potGlitch.old, KNOB);
	if(potExp.pauseReading == 0)
		setExp(ADC_TO_FLOAT * potExp.old, EXPRESSION);
	//then advance actual settings
	setVolume();
	setMix();
	setTime();
	setFilter();
	setRate();
	setSubRate();
	setSubTime();
	setDepth();
	setOffset();
	setShape();
	setDissolve();
	setChance();
	setSmear();
	setGlitch();
	setPWM();
	currentSetting[MODESINDEX] = modeBinary; //because active lfos aren't treated the same way as other settings, continually update their index with the current value (for whenever a preset needs to be written)
}

void processMIDIByte()
{
	  storedMIDIOut[MIDIWriteHeadOut] = storedMIDI[MIDIReadHead]; //transfer byte into output buffer
	  MIDIWriteHeadOut++; //increment output write head
	  if(MIDIWriteHeadOut > MIDISTORAGEBUFFERSIZE - 1)
		  MIDIWriteHeadOut = 0; //wrap around if needed
	  unprocessedBytesOut++; //mark that we have a new byte to send out

	  lsNybble = storedMIDI[MIDIReadHead] & 0x0F; //mask off upper four bits to asses least significant nybble
	  msNybble = storedMIDI[MIDIReadHead] >> 4; //bit shift upper four bits to asses most significant nybble
	  if(msNybble > 0x7) //if the most significant bit is flipped (value 8 or larger)
	  {
		  if(msNybble != 0xF) //if most significant nibble is F, it's a real time message, so ignore those
		  {
		  //update status byte
		  statuslsNybble = lsNybble;
		  statusmsNybble = msNybble;
		  dataIndex = 0; //reset data index on new status byte. This shouldn't be necessary, but who knows!
		  dataBytes[0] = dataBytes[1] = 0; //clear data bytes as well, maybe?
		  }
		  else //if msNyblle is F, it's a realtime message
		  {
			  if(lsNybble == MIDICLOCKPULSE) //if it's a clock pulse
			  {
				  if(MIDIClockMode != CLOCKTHRU) //if we're not in clock through mode
					  dontSend = 1; //don't send this byte to the output buffer
				  midiPPQCounter++; //increment counter
				  if (midiPPQCounter > 23) //if past 24 pulses
				  {
					  if(MIDIClockMode == CLOCKTHRU || MIDIClockMode == CLOCKIN) //if accepting MIDI clock input
						  registerTap(MIDI); //register it as a tap
					  midiPPQCounter = 0; //regardless, reset the counter
				  }
				  /*if(midiPPQCounter == 0) //on first message
				  {
					  if(MIDIClockMode == CLOCKTHRU || MIDIClockMode == CLOCKIN) //if accepting MIDI clock input
						  registerTap(MIDI); //register it as a tap
				  }
				  midiPPQCounter++; //increment counter
				  if(midiPPQCounter > 23) //after 24
					  midiPPQCounter = 0; //reset to count on next message*/
			  }
			  else if(lsNybble == MIDISTART) //if it's a start byte
			  {
				  midiPPQCounter = 0; //reset the PPQ counter, so it can realign the start point of the pulse
				  tapNum = 0; //as well as the tap number
			  }
		  }
	  }
	  else if(statuslsNybble == midiChannel) //if the channel matches this one
	  {
		  if(statusmsNybble == CONTROLCHANGE) //if it's a control change message
		  {
			  dataBytes[dataIndex] = storedMIDI[MIDIReadHead]; //bring our next byte into data
			  dataIndex++;
			  if(dataIndex > 1) //if we've filled the data bytes
			  {
				  dataIndex = 0; //reset data index
				  //rate
				  if(dataBytes[0] == RATECC)
					  setRateTarget(dataBytes[1] * MIDI_TO_FLOAT, MIDI);
				  //rate subdivisions
				  if(dataBytes[0] == SUBRATECC)
					  setSubRateTarget(dataBytes[1] * MIDI_TO_FLOAT, MIDI);
				  //depth
				  if(dataBytes[0] == DEPTHCC)
					  setDepthTarget(dataBytes[1] * MIDI_TO_FLOAT, MIDI);
				  //offset
				  if(dataBytes[0] == OFFSETCC)
					  setOffsetTarget(dataBytes[1] * MIDI_TO_FLOAT, MIDI);
				  //shape
				  if(dataBytes[0] == SHAPECC)
					  setShapeTarget(dataBytes[1] * MIDI_TO_FLOAT, MIDI);
				  //dissolve
				  if(dataBytes[0] == DISSOLVECC)
					  setDissolveTarget(dataBytes[1] * MIDI_TO_FLOAT, MIDI);
				  //chance
				  if(dataBytes[0] == CHANCECC)
					  setChanceTarget(dataBytes[1] * MIDI_TO_FLOAT, MIDI);
				  //smear
				  if(dataBytes[0] == SMEARCC)
					  setSmearTarget((dataBytes[1] * MIDI_TO_FLOAT), MIDI);
				  //glitch
				  if(dataBytes[0] == GLITCHCC)
					  setGlitchTarget(dataBytes[1] * MIDI_TO_FLOAT, MIDI);
				  //time
				  if(dataBytes[0] == TIMECC)
					  setTimeTarget(dataBytes[1] * MIDI_TO_FLOAT, MIDI);
				  //time subdivisions
				  if(dataBytes[0] == SUBTIMECC)
					  setSubTimeTarget(dataBytes[1] * MIDI_TO_FLOAT, MIDI);
				  //filter
				  if(dataBytes[0] == FILTERCC)
					  setFilterTarget(dataBytes[1] * MIDI_TO_FLOAT, MIDI);
				  //mix
				  if(dataBytes[0] == MIXCC)
					  setMixTarget(dataBytes[1] * MIDI_TO_FLOAT, MIDI);
				  //volume
				  if(dataBytes[0] == VOLUMECC)
					  setVolumeTarget(dataBytes[1] * MIDI_TO_FLOAT, MIDI);
				  if(dataBytes[0] == EXPCC)
				  {
					  potExp.queuePause = 1; //if expression, need to pause explicitly
					  setExp(dataBytes[1] * MIDI_TO_FLOAT, MIDI); //then send value
				  }
				  if(dataBytes[0] == BYPASSSWITCHCC)
				  {
					  if(dataBytes[1] < 64 && onOffState != OFF) //check for current state to avoid wearing EEPROM as user scales 1-63
						  turnOff();
					  else if(dataBytes[1] >= 64 && onOffState != ON)
						  turnOn();
				  }
				  if(dataBytes[0] == MODMODECC)
				  {
					  uint8_t temp = modeBinary; //grab current state
					  if(dataBytes[1] < 64)
					  {
						  modMode = OFF;
						  modeBinary &= MODMODETREMOLO;
					  }
					  else
					  {
						  modMode = ON;
						  modeBinary |= MODMODEVIBRATO;
					  }
					  if(temp != modeBinary) //if state changed
						  writeByte(modeBinary, MODESINDEX); //store it
					  led_set(&ledTime, modMode);
				  }
				  if(dataBytes[0] == FILTERMODECC)
				  {
					  uint8_t temp = modeBinary; //grab current state
					  if(dataBytes[1] < 64)
					  {
						  filterMode = OFF;
						  modeBinary &= FILTERMODELOWPASS;
					  }
					  else
					  {
						  filterMode = ON;
						  modeBinary |= FILTERMODEHIGHPASS;
					  }
					  if(temp != modeBinary) //if state changed
						  writeByte(modeBinary, MODESINDEX); //store it
					  led_set(&ledFilter, filterMode);
				  }
				  if(dataBytes[0] == MIXMODECC)
				  {
					  uint8_t temp = modeBinary; //grab current state
					  if(dataBytes[1] < 64)
					  {
						  mixMode = TRUEMIX;
						  modeBinary &= MIXMODETRUE;
					  }
					  else
					  {
						  mixMode = MODMIX;
						  modeBinary |= MIXMODEMODIFIED;
					  }
					  if(temp != modeBinary) //if state changed
						  writeByte(modeBinary, MODESINDEX); //store it
					  led_set(&ledMix, mixMode);
				  }
				  if(dataBytes[0] == HALFTIMESWITCHCC)
				  {
					  if(dataBytes[1] < 64)
						  footswitchDouble = 1.0f;
					  else
						  footswitchDouble = 0.5f;
				  }
				  if(dataBytes[0] == TAPSWITCHCC)
				  {
					  if(dataBytes[1] == 0) //if it's meant to be a tap pulse
						  registerTap(MIDI);
				  }
				  if(dataBytes[0] == STEREOSETTINGCC)
				  {
					  uint8_t temp = stereoState; //grab current state
					  if(dataBytes[1] < 43)
						  stereoState = MONO;
					  else if(dataBytes[1] < 86)
						  stereoState = MISO;
					  else
						  stereoState = STEREO;
					  if(stereoState == MISO) //check for mono in stereo out and switch relay as needed
						  misoOn();
					  else
						  misoOff();
					  if(temp != stereoState) //if state changed
						  writeByte(stereoState, STEREOSTATEINDEX); //store it
				  }
				  if(dataBytes[0] == AUXMODECC)
				  {
					  uint8_t temp = auxSetting; //grab current state
					  if(dataBytes[1] < 43)
						  auxSetting = AUXASTAP;
					  else if(dataBytes[1] < 86)
						  auxSetting = AUXASDOUBLE;
					  else
						  auxSetting = AUXASPRESET;
					  if(temp != auxSetting) //if state changed
						  writeByte(auxSetting, AUXSETTINGINDEX); //store it
				  }
				  if(dataBytes[0] == TRAILSSETTINGCC)
				  {
					  uint8_t temp = trailsState; //grab current state
					  if(dataBytes[1] < 64)
						  trailsState = OFF;
					  else
						  trailsState = ON;
					  if(temp != trailsState) //if state changed
					  {
						  writeByte(trailsState, TRAILSINDEX); //store it
						  if(onOffState == OFF) //if currently off
							  turnOff(); //trigger relay change
					  }
				  }
				  if(dataBytes[0] == CLOCKMODECC)
				  {
					  uint8_t temp = MIDIClockMode; //grab current state
					  if(dataBytes[1] < 32)
						  MIDIClockMode = CLOCKTHRU;
					  else if(dataBytes[1] < 64)
						  MIDIClockMode = CLOCKIN;
					  else if(dataBytes[1] < 96)
						  MIDIClockMode = CLOCKOUT;
					  else
						  MIDIClockMode = CLOCKOFF;
					  if(temp != MIDIClockMode) //if state changed
					  {
						  writeByte(MIDIClockMode, MIDICLOCKMODEINDEX); //store it
					  }
				  }
				  if(dataBytes[0] == MIDICHANNELCC)
				  {
					  if(dataBytes[1] > 0 && dataBytes[1] <= 16)
					  {
						  midiChannel = dataBytes[1] - 1;
						  writeByte(midiChannel, MIDICHANNELINDEX);
					  }
				  }
				  if(dataBytes[0] == PRESETSAVESWITCH)
				  {
					  if(dataBytes[1] < 43) //if canceling save
					  {
						  savingPreset = 0; //revert to not-saving mode
					  }
					  else if(dataBytes[1] < 86) //if initiating save
					  {
						  if(savingPreset != 1) //if not already saving
						  {
							  savingPreset = 1;
							  if(preset == 0) preset = 1; //if in live mode (preset is 0), start at preset 1, to avoid confusion where nothing is blinking
							  ledTactile = preset; //assign the led
							  ledTickerTop = 500;
							  led_setTop(&ledP1, ledTickerTop);
							  led_setTop(&ledP2, ledTickerTop);
							  led_setTop(&ledP3, ledTickerTop);
							  led_clear(&ledP1);
							  led_clear(&ledP2);
							  led_clear(&ledP3);
						  }
					  }
					  else //if confirming save
					  {
						  if(savingPreset == 1) //if we were saving preset
						  {
							  savingPreset = 0; //exit that mode
							  if(preset != 0) //don't save if preset slot is zero
							  {
								  for(int n = EXPRESSIONSTART; n < PRESETARRAYLENGTH; n++)
									  currentSetting[n] = expBuffer[n]; //load currentsettings with info from expbuffer
								  if(noSubdivisionsTime == 0) //if subdivisions are allowed
									  currentSetting[TIMEINDEX] = currentSetting[TIMEINDEX] * currentSetting[SUBTIMEINDEX]; //hardcode the time so it matches on recall
								  if(noSubdivisionsRate == 0) //if subdivisions are allowed
									  currentSetting[RATEINDEX] = currentSetting[RATEINDEX] * currentSetting[SUBRATEINDEX]; //hardcode the rate so it matches on recall
								  writePreset(currentSetting, preset); //store preset
							  }
						  }
					  }
				  }
			  }
		  }
		  else if(statusmsNybble == PROGRAMCHANGE) //if it's a patch change message
		  {
			  if(statuslsNybble == midiChannel) //and if addressed to correct MIDI channel (defualt channel 1, which is 0)
			  {
				  dataBytes[dataIndex] = storedMIDI[MIDIReadHead]; //bring the next byte (the PC value) in
				  //no need to increment dataIndex, since the next byte should be status
				  if(dataBytes[0] == 127) //if maximum value
					  ledTactile = preset = 0; //do live mode
				  else //otherwise
					  ledTactile = preset = dataBytes[0] + 1; //mark the preset
				  if(savingPreset == 0) //if not saving preset
					  loadPreset(presetRead, preset); //load it
				  //if saving preset, no worries, you've marked the preset and can save it officially with the tactile switch
				  }
			  }
	  }
	  MIDIReadHead++; //move read head forward
	  if(MIDIReadHead > MIDISTORAGEBUFFERSIZE - 1) //if we've reached the end
		  MIDIReadHead = 0; //wrap around
}
void processSample()
{

	//input volume scaling
	inputVol += 0.01f * (inputVolTarget - inputVol); //scale input volume here too

	//increment sample-based millis
	sampleBasedMillis++;
	if(tapResetTimerSamples > 0.0f) //if tap reset timer active
	{
		tapResetTimerSamples--; //decrement
		if(tapResetTimerSamples <= 0.0f) //if we then hit zero
		{
			tapNum = 0; //reset tap number
			//midiPPQCounter = 0; //and ppq while we're at it
		}
	}

	//do MIDI clock pulse stuff
	clockPulseTimerCounter += 1.0f; //increment counter
	if(clockPulseTimerCounter >= clockPulseTimer) //if past the pulse timing cutoff
	{
		if(MIDIClockMode == CLOCKOUT && clockPulseTimer > 150.0f) //if sending clock out, and pulse is slower than about 1200BPM
		{
			storedMIDIOut[MIDIWriteHeadOut] = MIDICLOCKPULSEBYTE; //load clock pulse byte into output buffer
			MIDIWriteHeadOut++; //increment output write head
			if(MIDIWriteHeadOut > MIDISTORAGEBUFFERSIZE - 1)
				MIDIWriteHeadOut = 0; //wrap around if needed
			unprocessedBytesOut++; //mark that we have a new byte to send out
		}
		clockPulseTimerCounter = 0; //reset timer
	}


		//Get Signal Inputs
		//left channel input
		leftIn = INT32_TO_FLOAT * inPtr[0];
		//leftIn = INT32_TO_FLOAT * SAIInCurrentHalf[n];
		leftOut = leftIn * 1.584893f;//1.584893f 4dB boost //1.995262f //6dB boost  // 3.162278f 10dB boost
		leftOut *= inputVol; //eliminate input if off but trailing
		//apply DC blocking right at the top, see if this helps with later crushing/other things potentially?
		statevariable_input(&crushHPFL, leftOut);
		leftOut = statevariable_getHP(&crushHPFL);

		//right channel input
		rightIn = INT32_TO_FLOAT * inPtr[1];
		//rightIn = INT32_TO_FLOAT * SAIInCurrentHalf[n+1];
		rightOut = rightIn * 1.584893f;//1.584893f 4dB boost //1.995262f //6dB boost  // 3.162278f 10dB boost
		rightOut *= inputVol; //eliminate input if off but trailing
		//apply DC blocking right at the top, see if this helps with later crushing/other things potentially?
		statevariable_input(&crushHPFR, rightOut);
		rightOut = statevariable_getHP(&crushHPFR);

		//store feedback
		leftOut = hardClip(leftOut);
		rightOut = hardClip(rightOut);



		reverseMix += 0.01f * (reverseMixTarget - reverseMix);

		/*float converter = leftOut * FLOAT_TO_INT16;
		converter = delay_parting_16bit_inOutModulatedReverse(&reverseL, converter, 0.0f);
		converter *= INT16_TO_FLOAT;
		leftOut = converter;
		converter = rightOut * FLOAT_TO_INT16;
		converter = delay_parting_16bit_inOutModulatedReverse(&reverseR, converter, 0.0f);
		converter *= INT16_TO_FLOAT;
		rightOut = converter;*/
		statevariable_setFC(&leftOutLPF, 500.0f);
		statevariable_setFC(&rightOutLPF, 500.0f);
		//statevariable_input(&leftOutLPF, leftOut);
		//leftOut = statevariable_getHP(&leftOutLPF);
		//statevariable_input(&rightOutLPF, rightOut);
		//rightOut = statevariable_getHP(&rightOutLPF);
		leftOut = delay_clocked_inOutModulatedLoFiVariable(&reverseL, leftOut, 0.0f);
		rightOut = delay_clocked_inOutModulatedLoFiVariable(&reverseR, rightOut, 0.0f);


		if(leftOut > 1.0f)
			leftOut = 1.0f;
		else if(leftOut < -1.0f)
			leftOut = -1.0f;
		if(rightOut > 1.0f)
			rightOut = 1.0f;
		else if(rightOut < -1.0f)
			rightOut = -1.0f;

		//some final attenuation
		leftOut *= 0.8f;
		rightOut *= 0.8f;

		outPtr[0] = FLOAT_TO_INT32 * leftOut;
		outPtr[1] = FLOAT_TO_INT32 * rightOut;
		//SAIOutCurrentHalf[n] = FLOAT_TO_INT32 * (leftOut);
		//SAIOutCurrentHalf[n+1] = FLOAT_TO_INT32 * (rightOut);

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) //when timer interrupts
{
	if(htim == &htim17) //if the trigger was timer 17, our millisecond timer
	{
		millis++; //increment millisecond counter
		/*if(tapResetTimer > 0) //if above zero
		{
			tapResetTimer--; //decrement
			if(tapResetTimer == 0) //if we then hit zero
				tapNum = 0; //reset tap number
		}*/
		if(savingPreset == 1 || assigningHold == ON || assigningBypass == ON)
		{
			led_tick(&ledP1);
			led_tick(&ledP2);
			led_tick(&ledP3);
		}
		/*if(auxSetting == AUXASPRESET)
		{
			if(tapLEDPaused == 0)
			{
				led_tick(&ledAux);
			}
		}*/
		//TAP TEMPO LED
		tapLEDTracker1++; //increment tap light counter

		if((tapLEDTracker1 >= (((timeScaler*0.25)*1000.0f)/reverseClock)) && tap1On == 0.0f) //if it hits the first delay time
		{
			if(tapLEDPaused == 0)
				led_on(&ledAux); //turn the led on
			tap1On = 1.0f; //mark that we turned it on
			tap1TurnOff = 30.0f; //and start the off timer
			tapLEDTracker1 = 0; //also reset the tracker
		}
		if(tap1TurnOff > 0.0f) //if the off counter isn't finished
		{
			tap1TurnOff--; //decrement
			if(tap1TurnOff <= 0.0f) //and if it hits bottom
			{
				if(tapLEDPaused == 0)
					led_off(&ledAux); //turn the led off
				tap1On = 0.0f; //and reset the flag
			}
		}
		//clear it in main loop when it goes over LEDTICKERTOP, so LED processing can be determined then
		checkSwitches(); //check switches every millisecond
		eepromTicker(); //and do whatever EEPROM ticker routines exist
		if(eepromByteQueueTimer < 10) //if eepromByteQueueTimer needs to keep counting up and won't overflow
			eepromByteQueueTimer++; //increment it
	}
	if(htim == &htim14) //variable sample rate timer
	{
		nextSampleFlag = 1;
		__HAL_TIM_SET_AUTORELOAD(&htim14, timerPeriod);
	}
}

void factoryReset()
{
	//first, mute everything
	mute();
	//turn on all LEDs
	led_on(&ledP1);
	led_on(&ledP2);
	led_on(&ledP3);
	led_on(&ledAux);
	led_on(&ledBypass);
	tapLEDPaused = 1;
	uint16_t tempTicker = 0;
	while(1) //enter holding pattern
	{
		//just in case, clear all the footswitch related flags
		clearAllFlags(&fsBypass);
		clearAllFlags(&fsAux);
		auxWasHeld = tactWasHeld = auxWasPressed = bypassWasPressed = presetWasPressed = lfoWasPressed = 0;
		HAL_Delay(1); //1ms delay
		tempTicker++; //increment ticker
		if(tempTicker == 250) //if it hits half a second
		{
			//flip the LEDs
			led_flip(&ledP1);
			led_flip(&ledP2);
			led_flip(&ledP3);
			led_flip(&ledAux);
			led_flip(&ledBypass);
			//and reset the ticker
			tempTicker = 0;
		}
		checkSwitches(); //check the switches
		if(fsBypass.switchRegistered == 1) //if bypass switched
		{
			if(fsBypass.value == 0) //and is pressed down
			{
				clearAllFlags(&fsBypass);
				break; //return to normal operation
			}
		}
		if(fsAux.switchRegistered == 1) //if aux switched
		{
			if(fsAux.value == 0) //and is pressed down
			{
				//make sure LEDs are on
				led_on(&ledP1);
				led_on(&ledP2);
				led_on(&ledP3);
				led_on(&ledAux);
				led_on(&ledBypass);
				writeByte(255, FACTORYRESETBYTE); //mark the reset byte (this way if power is lost before it's done, it will try again on power-on)
				actuallyWriteByte();
				HAL_Delay(7);
				//rewrite everything, starting with the presets
				writePreset(preset1, 1); //load Dan's preset
				while(unprocessedEepromBytes > 0) //for all these loaded up bytes
				{
					  while(eepromByteQueueTimer < 6) {}//hold until last byte has processed
					  actuallyWriteByte(); //then do the next write
				}
				writePreset(preset2, 2); //load Brady's preset
				while(unprocessedEepromBytes > 0) //for all these loaded up bytes
				{
					  while(eepromByteQueueTimer < 6) {}//hold until last byte has processed
					  actuallyWriteByte(); //then do the next write
				}
				writePreset(preset3, 3); //load Andy's preset
				while(unprocessedEepromBytes > 0) //for all these loaded up bytes
				{
					  while(eepromByteQueueTimer < 6) {}//hold until last byte has processed
					  actuallyWriteByte(); //then do the next write
				}
				for(uint8_t n = 4; n < 128; n++)
				{
					uint8_t ledState1 = (n >> 2) & 0x01; //mask off all but last bit
					uint8_t ledState2 = (n >> 3) & 0x01;
					uint8_t ledState3 = (n >> 4) & 0x01;
					uint8_t ledState4 = (n >> 5) & 0x01;
					uint8_t ledState5 = (n >> 6) & 0x01;
					led_set(&ledP1, ledState1);
					led_set(&ledP2, ledState2);
					led_set(&ledP3, ledState3);
					led_set(&ledBypass, ledState4);
					led_set(&ledAux, ledState5);
					writePreset(initPatch, n);
					while(unprocessedEepromBytes > 0) //for all these loaded up bytes
					{
						while(eepromByteQueueTimer < 6) {}//hold until last byte has processed
						actuallyWriteByte(); //then do the next write
					}
				}
				//LEDs back to on
				led_on(&ledP1);
				led_on(&ledP2);
				led_on(&ledP3);
				led_on(&ledAux);
				led_on(&ledBypass);
				stereoState = MONO; //initialize in mono
				writeByte(stereoState, STEREOSTATEINDEX);
				trailsState = ON;//with trails
				writeByte(trailsState, TRAILSINDEX);
				onOffState = OFF;
				writeByte(onOffState, BYPASSINDEX);
				midiChannel = CHANNEL1;
				writeByte(midiChannel, MIDICHANNELINDEX);
				MIDIClockMode = CLOCKTHRU;
				writeByte(MIDIClockMode, MIDICLOCKMODEINDEX);
				noSubdivisionsRate = noSubdivisionsTime = 0;
				writeByte(0.5f * FLOAT_TO_UINT8, SUBRATEINDEX); //set subdivisions to 1
				writeByte(1.0f * FLOAT_TO_UINT8, SUBTIMEINDEX); //set subdivisions to 1
				writeByte(1.0f * FLOAT_TO_UINT8, OFFSETINDEX); //set offset to full
				writeByte(0.5f * FLOAT_TO_UINT8, VOLUMEINDEX); //set volume to unity
				auxSetting = AUXASTAP;
				writeByte(auxSetting, AUXSETTINGINDEX); //make aux freeze
				modeBinary = 0;
				writeByte(modeBinary, MODESINDEX); //turn off all LFOs
				//disable scooch as tap
				scoochAsTap = 0;
				GPIO_InitTypeDef GPIO_InitStruct = {0};
				GPIO_InitStruct.Pin = GPIO_PIN_6; //for expression pin
				GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; //make mode analog
				GPIO_InitStruct.Pull = GPIO_NOPULL; //disable the pullup
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //and write these init states
				writeByte(scoochAsTap, SCOOCHASTAPINDEX); //store it
				ledTactile = preset = 0; //set preset to 0
				writeByte(preset, CURRENTPRESETINDEX); //store it
				writeGlobalExpression(initPatch); //use init patch to assign live mode expression
				while(unprocessedEepromBytes > 0) //for all these loaded up bytes
				{
					  while(eepromByteQueueTimer < 6) {}//hold until last byte has processed
					  actuallyWriteByte(); //then do the next write
				}
				//HAL_Delay(5);
				//readPreset(expBuffer, 0); //then pull that expression into the buffer
				loadPreset(presetRead, 0); //load the "live" preset, to pull expression info (set by write global expression) as well as load subdivisions, filter type, etc., set earlier
				if(stereoState == MISO) //check for mono in stereo out and switch relay as needed
					misoOn();
				else
					misoOff();
				if(onOffState == OFF) //assign bypass as read
					turnOff();
				else
					turnOn();
				//HAL_Delay(10);
				writeByte(0, FACTORYRESETBYTE); //mark the reset byte on the way out
				while(unprocessedEepromBytes > 0) //for all these loaded up bytes
				{
					  while(eepromByteQueueTimer < 6) {}//hold until last byte has processed
					  actuallyWriteByte(); //then do the next write
				}
				HAL_Delay(5);
				clearAllFlags(&fsAux);
				tapLEDPaused = 0; //keep tap tempo pulsing
				break; //then return to normal operation
			}
		}
	}
}

void init_AK4621EF()
{
	//CODE FOR AK4621EF
	  //create our SPI data
	  uint16_t SPIOut[1] = {0};
	  //load data to put ADC and DAC into reset
	  SPIOut[0] = WRITE_RESETCONTROL<<8; //load write command to upper 8 bits
	  SPIOut[0] += POWERDOWNADCDAC; //and data to bottom 8 bits
	  //bring chip select low again
	  HAL_GPIO_WritePin(CODECCHIPSELECT_GPIO_Port, CODECCHIPSELECT_Pin, GPIO_PIN_RESET);
	  tester = HAL_SPI_Transmit(&hspi2, (uint8_t*)SPIOut, 1, 1000); //send the message
	  //bring chip select high again
	  HAL_GPIO_WritePin(CODECCHIPSELECT_GPIO_Port, CODECCHIPSELECT_Pin, GPIO_PIN_SET);
	  //load data to establish clock and I2S format
	  SPIOut[0] = WRITE_CLOCKFORMATCONTROL<<8; //write address in MSB
	  SPIOut[0] += I2SNORMALSPEED; //data in LSB
	  //bring chip select low
	  HAL_GPIO_WritePin(CODECCHIPSELECT_GPIO_Port, CODECCHIPSELECT_Pin, GPIO_PIN_RESET);
	  tester = HAL_SPI_Transmit(&hspi2, (uint8_t*)SPIOut, 1, 1000); //send the message
	  //bring chip select high
	  HAL_GPIO_WritePin(CODECCHIPSELECT_GPIO_Port, CODECCHIPSELECT_Pin, GPIO_PIN_SET);
	  HAL_Delay(10); //give it a moment before messaging
	  //load data to pull ADC and DAC out of reset
	  SPIOut[0] = WRITE_RESETCONTROL<<8; //load write command to upper 8 bits
	  SPIOut[0] += POWERUPADCDAC; //and data to bottom 8 bits
	  //bring chip select low again
	  HAL_GPIO_WritePin(CODECCHIPSELECT_GPIO_Port, CODECCHIPSELECT_Pin, GPIO_PIN_RESET);
	  tester = HAL_SPI_Transmit(&hspi2, (uint8_t*)SPIOut, 1, 1000); //send the message
	  //bring chip select high again
	  HAL_GPIO_WritePin(CODECCHIPSELECT_GPIO_Port, CODECCHIPSELECT_Pin, GPIO_PIN_SET);
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

//////>>>>>>>>>SEE NOTE BELOW ABOUT DMA!!!!!

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SAI1_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_TIM17_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_TIM3_Init();
  MX_SPI2_Init();
  MX_TIM16_Init();
  MX_TIM14_Init();
  /* USER CODE BEGIN 2 */
  ///^^^^^^^^^^^^^^^^^^^^^^
///////MAKE SURE DMA IS CALLED
  ////BEFORE THE THINGS THAT USE DMA



  //initialize allpass stages

  delay_clocked_init(&reverseL, REVERSESIZE, reverseLBuffer);
  delay_clocked_init(&reverseR, REVERSESIZE, reverseRBuffer);
  delay_clocked_setDirection(&reverseL, 2.0f);
  delay_clocked_setDirection(&reverseR, 2.0f);
  delay_clocked_setClockSpeed(&reverseL, 0.249f);
  delay_clocked_setClockSpeed(&reverseR, 0.249f);

  allpass_init(&apTimeWaster, APTIMEWASTERSIZE, apTimeWasterBuffer, 0.5f);

  allpass_init(&ap1L, AP1SIZE, ap1LBuffer, K1);
  allpass_init(&ap2L, AP2SIZE, ap2LBuffer, K1);
  allpass_init(&ap3L, AP3SIZE, ap3LBuffer, K1);
  allpass_init(&ap4L, AP4SIZE, ap4LBuffer, K2);
  allpass_init(&ap5L, AP5SIZE, ap5LBuffer, K2);

  allpass_init(&ap1R, AP1SIZE, ap1RBuffer, K1);
  allpass_init(&ap2R, AP2SIZE, ap2RBuffer, K1);
  allpass_init(&ap3R, AP3SIZE, ap3RBuffer, K1);
  allpass_init(&ap4R, AP4SIZE, ap4RBuffer, K2);
  allpass_init(&ap5R, AP5SIZE, ap5RBuffer, K2);
  //set time of allpass filters to allow modulation (originally this used a timeScaler variable, just multiply the last parenthetical if desired)
  allpass_setTime(&ap1L, 1.0f + (AP1SIZE - 2.0f));
  allpass_setTime(&ap1R, 1.0f + (AP1SIZE - 2.0f));
  allpass_setTime(&ap2L, 1.0f + (AP2SIZE - 2.0f));
  allpass_setTime(&ap2R, 1.0f + (AP2SIZE - 2.0f));
  allpass_setTime(&ap3L, 1.0f + (AP3SIZE - 2.0f - VERBMODMAX));
  allpass_setTime(&ap3R, 1.0f + (AP3SIZE - 2.0f - VERBMODMAX));
  allpass_setTime(&ap4L, 1.0f + (AP4SIZE - 2.0f - VERBMODMAX));
  allpass_setTime(&ap4R, 1.0f + (AP4SIZE - 2.0f - VERBMODMAX));
  allpass_setTime(&ap5L, 1.0f + (AP5SIZE - 2.0f));
  allpass_setTime(&ap5R, 1.0f + (AP5SIZE - 2.0f));
  float tempSmoothing = 0.0001f;
  delay_parting_setSmoothing(&ap1L.memory, tempSmoothing);
  delay_parting_setSmoothing(&ap2L.memory, tempSmoothing);
  delay_parting_setSmoothing(&ap3L.memory, tempSmoothing);
  delay_parting_setSmoothing(&ap4L.memory, tempSmoothing);
  delay_parting_setSmoothing(&ap5L.memory, tempSmoothing);
  delay_parting_setSmoothing(&ap1R.memory, tempSmoothing);
  delay_parting_setSmoothing(&ap2R.memory, tempSmoothing);
  delay_parting_setSmoothing(&ap3R.memory, tempSmoothing);
  delay_parting_setSmoothing(&ap4R.memory, tempSmoothing);
  delay_parting_setSmoothing(&ap5R.memory, tempSmoothing);
  delay_clocked_setSmoothing(&reverseL, tempSmoothing);
  delay_clocked_setSmoothing(&reverseR, tempSmoothing);


  //initialize dimension chorus delays
  delay_parting_init(&dimensionL, DIMENSIONMEMORY, dimensionLBuffer);
  delay_parting_init(&dimensionR, DIMENSIONMEMORY, dimensionRBuffer);
  //set time to one sample, to be modulated from nearly zero
  delay_parting_setTime(&dimensionL, DIMENSIONMINIMUMTIME);
  delay_parting_setTime(&dimensionR, DIMENSIONMINIMUMTIME);

  delay_parting_setSmoothing(&dimensionL, 0.001f);
  delay_parting_setSmoothing(&dimensionR, 0.001f);



  //initialize filters
  //controllable lowpass filter (L&R)
  statevariable_init(&leftOutLPF, SAMPLERATE, 1000.0f);
  statevariable_setQ(&leftOutLPF, 1.8f);
  statevariable_init(&rightOutLPF, SAMPLERATE, 1000.0f);
  statevariable_setQ(&rightOutLPF, 1.8f);
  statevariable_setOverclock(&leftOutLPF, 4.0f);
  statevariable_setOverclock(&rightOutLPF, 4.0f);
  //controllable highpass filter (L&R)
  statevariable_init(&leftOutHPF, SAMPLERATE, 1000.0f);
  statevariable_setQ(&leftOutHPF, 1.8f);
  statevariable_init(&rightOutHPF, SAMPLERATE, 1000.0f);
  statevariable_setQ(&rightOutHPF, 1.8f);
  statevariable_setOverclock(&leftOutHPF, 4.0f);
  statevariable_setOverclock(&rightOutHPF, 4.0f);


  statevariable_init(&crushHPFL, SAMPLERATE, 20.0f);
  statevariable_init(&crushHPFR, SAMPLERATE, 20.0f);
  statevariable_setOverclock(&crushHPFL, 4.0f);
  statevariable_setOverclock(&crushHPFR, 4.0f);
  statevariable_init(&flangeHPFL, SAMPLERATE, 2500.0f);
  statevariable_init(&flangeHPFR, SAMPLERATE, 2500.0f);
  statevariable_setOverclock(&flangeHPFL, 4.0f);
  statevariable_setOverclock(&flangeHPFR, 4.0f);
  statevariable_setQ(&flangeHPFL, 0.5f);
  statevariable_setQ(&flangeHPFR, 0.5f);
  statevariable_init(&flangeLPFL, SAMPLERATE, 10000.0f);
  statevariable_init(&flangeLPFR, SAMPLERATE, 10000.0f);
  statevariable_setOverclock(&flangeLPFL, 4.0f);
  statevariable_setOverclock(&flangeLPFR, 4.0f);
  statevariable_init(&samplerateOutLPFL, SAMPLERATE, 8000.0f);
  statevariable_init(&samplerateOutLPFR, SAMPLERATE, 8000.0f);
  statevariable_setOverclock(&samplerateOutLPFL, 4.0f);
  statevariable_setOverclock(&samplerateOutLPFR, 4.0f);

  //initialize switches
  initSwitchMomentary(&fsBypass, FSDBCUTOFF, FSHOLDCUTOFF, FSLONGHOLDCUTOFF);
  initSwitchMomentary(&fsAux, FSDBCUTOFF, FSHOLDCUTOFF, FSLONGHOLDCUTOFF);
  initSwitchMomentary(&tactile, FSDBCUTOFF, FSHOLDCUTOFF, FSLONGHOLDCUTOFF);
  initSwitchMomentary(&tactMod, FSDBCUTOFF, FSHOLDCUTOFF, FSLONGHOLDCUTOFF);
  initSwitchMomentary(&tactFilter, FSDBCUTOFF, FSHOLDCUTOFF, FSLONGHOLDCUTOFF);
  initSwitchMomentary(&tactMix, FSDBCUTOFF, FSHOLDCUTOFF, FSLONGHOLDCUTOFF);
  initSwitchMomentary(&scooch, FSDBCUTOFF, FSHOLDCUTOFF, FSLONGHOLDCUTOFF);

  //initialize pots
  initPot(&potMix, POTMOVEMENTCUTOFF);
  initPot(&potFilter, POTMOVEMENTCUTOFF);
  initPot(&potTime, POTMOVEMENTCUTOFF);
  initPot(&potRate, POTMOVEMENTCUTOFF);
  initPot(&potDepth, POTMOVEMENTCUTOFF);
  initPot(&potShape, POTMOVEMENTCUTOFF);
  initPot(&potDissolve, POTMOVEMENTCUTOFF);
  initPot(&potChance, POTMOVEMENTCUTOFF);
  initPot(&potSmear, POTMOVEMENTCUTOFF);
  initPot(&potGlitch, POTMOVEMENTCUTOFF);
  initPot(&potExp, POTMOVEMENTCUTOFF);

  //initialize leds
  led_init(&ledBypass, LEDBYPASS_GPIO_Port, LEDBYPASS_Pin);
  led_init(&ledAux, LEDEXTRA_GPIO_Port, LEDEXTRA_Pin);
  led_init(&ledP1, LEDP1_GPIO_Port, LEDP1_Pin);
  led_init(&ledP2, LEDP2_GPIO_Port, LEDP2_Pin);
  led_init(&ledP3, LEDP3_GPIO_Port, LEDP3_Pin);
  led_init(&ledTime, LFOLED1_GPIO_Port, LFOLED1_Pin);
  led_init(&ledFilter, LFOLED2_GPIO_Port, LFOLED2_Pin);
  led_init(&ledMix, LFOLED3_GPIO_Port, LFOLED3_Pin);


  //fill all delay buffers (this seems to fix issues with RAM_D2 giving NaN)
  //for(uint32_t n = 0; n < TOTALALLPASSMEMORY; n++)
  //{
	  //apLBuffer[n] = apRBuffer[n] = 0.0f;
  //}

  for(uint32_t n = 0; n < AP1SIZE; n++)
	  ap1LBuffer[n] = ap1RBuffer[n] = 0;
  for(uint32_t n = 0; n < AP2SIZE; n++)
	  ap2LBuffer[n] = ap2RBuffer[n] = 0;
  for(uint32_t n = 0; n < AP3SIZE; n++)
	  ap3LBuffer[n] = ap3RBuffer[n] = 0;
  for(uint32_t n = 0; n < AP4SIZE; n++)
	  ap4LBuffer[n] = ap4RBuffer[n] = 0;
  for(uint32_t n = 0; n < AP5SIZE; n++)
	  ap5LBuffer[n] = ap5RBuffer[n] = 0;

  /*for(uint32_t n = 0; n < SURPRISEBUFFERSIZE; n++)
  {
	  surpriseLBuffer[n] = surpriseRBuffer[n] = 0.0f;
  }*/

  for(uint32_t n = 0; n < REVERSESIZE; n++)
  {
	  reverseLBuffer[n] = reverseRBuffer[n] = 0;
  }

  //fill all delay buffers (this seems to fix issues with RAM_D2 giving NaN)
  for(uint32_t n = 0; n < DIMENSIONMEMORY; n++)
  {
	  dimensionLBuffer[n] = dimensionRBuffer[n] =  0.0f;
  }

  //create sine wave table
  for (int n = 0; n < SINSIZE; n++)
  {
	  float position = (float) n / SINSIZEF;
	  sinTable[n] = 0.5f + (0.5f * (sinf(2.0f * M_PI * (position-0.25f))));
  }

  //clear presets on startup for testing
  for(uint32_t n = 0; n < PRESETARRAYLENGTH; n++)
  {
	  currentSetting[n] = targetSetting[n] = expBuffer[n] = 0.0f;
  }

  //initialize blank SAI arrays
  for(uint16_t n = 0; n < SAIBUFFERSIZE; n++)
  {
	  SAIIn[n] = SAIOut[n] = 0;
  }


  HAL_Delay(1000); //delay by 1 second before doing anything

  //ADC startup
  HAL_ADC_Init(&hadc1);
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED); //calibrate ADC
  HAL_Delay(100); //hold for a moment before initializing
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADCBuffer, ADCBUFFERSIZE);

  //start the millisecond timer
  HAL_TIM_Base_Start_IT(&htim17);

  //start the virtual sample rate timer
  HAL_TIM_Base_Start_IT(&htim14);

  uint8_t factoryResetCheck = 0;
  readByte(&factoryResetCheck, FACTORYRESETBYTE);
  if(factoryResetCheck == 255) //if it's never been written
  {
	  factoryReset();
  }

  HAL_DMA_Init(&hdma_sai1_a);
  HAL_DMA_Init(&hdma_sai1_b);
  HAL_SAI_Init(&hsai_BlockA1);
  HAL_SAI_Init(&hsai_BlockB1);
  HAL_SAI_Receive_DMA(&hsai_BlockB1, (uint8_t *)SAIIn, SAIBUFFERSIZE);
  HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t *)SAIOut, SAIBUFFERSIZE);

  //initialize SPI
  HAL_SPI_Init(&hspi2);

  //Enter reset
  HAL_GPIO_WritePin(CODECRESET_GPIO_Port, CODECRESET_Pin, GPIO_PIN_RESET);
  HAL_Delay(5); //give it a moment
  //bring codec out of reset/power down
  HAL_GPIO_WritePin(CODECRESET_GPIO_Port, CODECRESET_Pin, GPIO_PIN_SET);

  init_AK4621EF();

  //turn SPI back off after using
  //HAL_SPI_DeInit(&hspi2);

  //start receiving MIDI messages
  HAL_UART_Receive_DMA(&huart1, (uint8_t *)MIDIBuffer, MIDIINPUTBUFFERSIZE);

  //get the variety of global settings stored in eeprom
  //starting with routing
  readByte(&stereoState, STEREOSTATEINDEX);
  readByte(&trailsState, TRAILSINDEX);
  readByte(&onOffState, BYPASSINDEX);
  if(stereoState == MISO) //check for mono in stereo out and switch relay as needed
	  misoOn();
  else
	  misoOff();
  HAL_Delay(100); //add a delay so relays switch in-turn rather than almost simultaneously
  if(onOffState == OFF) //assign bypass as read
	  turnOff();
  else
	  turnOn();
  //then MIDI
  readByte(&midiChannel, MIDICHANNELINDEX);
  readByte(&MIDIClockMode, MIDICLOCKMODEINDEX);
  if(midiChannel > CHANNEL16) //if outside range
	  midiChannel = CHANNEL1;
  //then subdivisions
  /*readByte(&tempByte, SUBRATEINDEX);
  HAL_Delay(5);
  setSubRateTarget((float)tempByte * UINT8_TO_FLOAT, PRESET);
  readByte(&tempByte, SUBTIMEINDEX);
  HAL_Delay(5);
  setSubTimeTarget((float)tempByte * UINT8_TO_FLOAT, PRESET);
  readByte(&tempByte, OFFSETINDEX);
  HAL_Delay(5);
  setOffsetTarget((float)tempByte * UINT8_TO_FLOAT, PRESET);
  readByte(&tempByte, VOLUMEINDEX);
  HAL_Delay(5);
  setVolumeTarget((float)tempByte * UINT8_TO_FLOAT, PRESET);*/

  //load aux footswitch setting
  //readByte(&auxSetting, AUXSETTINGINDEX);
  //if(auxSetting == AUXASTAP) //if doing tap tempo
  auxSetting = AUXASTAP;
	  tapLEDPaused = 0; //resume blinking LED

  HAL_Delay(5);
  readByte(&modeBinary, MODESINDEX);
  setModes();

  //then scooch as tap
  readByte(&scoochAsTap, SCOOCHASTAPINDEX);
  HAL_Delay(5);
  if(scoochAsTap == 1) //if using scooch for tap
  {
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	  GPIO_InitStruct.Pin = GPIO_PIN_6; //for expression pin
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT; //make mode input
	  GPIO_InitStruct.Pull = GPIO_PULLUP; //and enable the pullup
	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //and write these init states
  }
  else
  {
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	  GPIO_InitStruct.Pin = GPIO_PIN_6; //for expression pin
	  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; //make mode analog
	  GPIO_InitStruct.Pull = GPIO_NOPULL; //disable the pullup
	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //and write these init states
  }


  //then preset, or live mode
  readByte(&preset, CURRENTPRESETINDEX); //get current preset
  ledTactile = preset;
  loadPreset(presetRead, preset); //load it


  HAL_Delay(100); //brief delay before we start PWMing
  //initialize PWM
  HAL_TIM_Base_Start(&htim3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
  HAL_TIM_Base_Start(&htim16);
  HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);

  //before entering while loop, clear all switch flags - not sure if this will help with early bugs after power-on, but let's see
  clearAllFlags(&fsBypass);
  clearAllFlags(&fsAux);
  clearAllFlags(&tactile);
  clearAllFlags(&tactMod);
  clearAllFlags(&tactFilter);
  clearAllFlags(&tactMix);
  clearAllFlags(&scooch);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(adcDelayCounter >= 64) //if we've processed 64 samples (which was the block size, within which we used to check adc once
	  {
		  adcDelayCounter = 0; //reset to zero
		  updateControls(); //then update
	  }
	  if(unprocessedBytes > 0)
	  {
		  processMIDIByte();
		  if(unprocessedBytes > maxUnprocessedBytes)
			  maxUnprocessedBytes = unprocessedBytes;
	  	  unprocessedBytes--; //and mark that a byte was processed
	  }
	  if(unprocessedBytesOut > 0 && MIDITxBusy == 0) //if we have bytes to send out and DMA isn't busy
	  {
		  //unprocessedBytesOut--; //decrement to show a byte was processed
		  MIDITxBusy = 1; //mark it busy
		  for(int n = 0; n < unprocessedBytesOut; n++)
		  {
			  MIDIOutByte[n] = storedMIDIOut[MIDIReadHeadOut++]; //load bytes into DMA buffer and increment read head as we go
			  if(MIDIReadHeadOut > MIDISTORAGEBUFFERSIZE - 1) //wrap around if needed
				  MIDIReadHeadOut = 0;
		  }
		  HAL_UART_Transmit_DMA(&huart1, (uint8_t *)MIDIOutByte, unprocessedBytesOut); //send it
		  unprocessedBytesOut = 0; //clear the bytes
	  }
	  if(unprocessedEepromBytes > 0 && eepromByteQueueTimer > 5) //if we have unprocessed bytes, and the last write has had 5ms to lock in
	  {
		  actuallyWriteByte(); //do the next write
	  }
	  //when bypass switches
	  if(fsBypass.switchRegistered == 1)
	  {
		  if(fsBypass.value == 0)
		  {
			  auxWasPressed = 0; //clear the tap press marker
			  bypassWasPressed = 1;
			  //grab pot snapshots
			  bypassPressSnapshot[0] = potRate.old;
			  bypassPressSnapshot[1] = potDepth.old;
			  bypassPressSnapshot[2] = potShape.old;
			  bypassPressSnapshot[3] = potDissolve.old;
			  bypassPressSnapshot[4] = potChance.old;
			  bypassPressSnapshot[5] = potSmear.old;
			  bypassPressSnapshot[6] = potGlitch.old;
			  bypassPressSnapshot[7] = potTime.old;
			  bypassPressSnapshot[8] = potFilter.old;
			  bypassPressSnapshot[9] = potMix.old;


			  expBufferTemp[RATEHEELINDEX] = currentSetting[RATEINDEX];
			  expBufferTemp[DEPTHHEELINDEX] = currentSetting[DEPTHINDEX];
			  expBufferTemp[SHAPEHEELINDEX] = currentSetting[SHAPEINDEX];
			  expBufferTemp[DISSOLVEHEELINDEX] = currentSetting[DISSOLVEINDEX];
			  expBufferTemp[CHANCEHEELINDEX] = currentSetting[CHANCEINDEX];
			  expBufferTemp[SMEARHEELINDEX] = currentSetting[SMEARINDEX];
			  expBufferTemp[GLITCHHEELINDEX] = currentSetting[GLITCHINDEX];
			  expBufferTemp[TIMEHEELINDEX] = currentSetting[TIMEINDEX];
			  expBufferTemp[FILTERHEELINDEX] = currentSetting[FILTERINDEX];
			  expBufferTemp[MIXHEELINDEX] = currentSetting[MIXINDEX];
			  //flip bypass state
			  if(onOffState == 0)
				  turnOn();
			  else
				  turnOff();
		  }
		  else if(fsBypass.holdFlag == 1) //if it was held and is being released
		  {
				bypassReleaseSnapshot[0] = potRate.old;
				bypassReleaseSnapshot[1] = potDepth.old;
				bypassReleaseSnapshot[2] = potShape.old;
				bypassReleaseSnapshot[3] = potDissolve.old;
				bypassReleaseSnapshot[4] = potChance.old;
				bypassReleaseSnapshot[5] = potSmear.old;
				bypassReleaseSnapshot[6] = potGlitch.old;
				bypassReleaseSnapshot[7] = potTime.old;
				bypassReleaseSnapshot[8] = potFilter.old;
				bypassReleaseSnapshot[9] = potMix.old;

			//load settings to actual expression buffer
			expBuffer[RATETOEINDEX] = currentSetting[RATEINDEX];
			expBuffer[DEPTHTOEINDEX] = currentSetting[DEPTHINDEX];
			expBuffer[SHAPETOEINDEX] = currentSetting[SHAPEINDEX];
			expBuffer[DISSOLVETOEINDEX] = currentSetting[DISSOLVEINDEX];
			expBuffer[CHANCETOEINDEX] = currentSetting[CHANCEINDEX];
			expBuffer[SMEARTOEINDEX] = currentSetting[SMEARINDEX];
			expBuffer[GLITCHTOEINDEX] = currentSetting[GLITCHINDEX];
			expBuffer[TIMETOEINDEX] = currentSetting[TIMEINDEX];
			expBuffer[FILTERTOEINDEX] = currentSetting[FILTERINDEX];
			expBuffer[MIXTOEINDEX] = currentSetting[MIXINDEX];
			//including the temp settings we grabbed when the switch was pressed
			expBuffer[RATEHEELINDEX] = expBufferTemp[RATEHEELINDEX];
			expBuffer[DEPTHHEELINDEX] = expBufferTemp[DEPTHHEELINDEX];
			expBuffer[SHAPEHEELINDEX] = expBufferTemp[SHAPEHEELINDEX];
			expBuffer[DISSOLVEHEELINDEX] = expBufferTemp[DISSOLVEHEELINDEX];
			expBuffer[CHANCEHEELINDEX] = expBufferTemp[CHANCEHEELINDEX];
			expBuffer[SMEARHEELINDEX] = expBufferTemp[SMEARHEELINDEX];
			expBuffer[GLITCHHEELINDEX] = expBufferTemp[GLITCHHEELINDEX];
			expBuffer[TIMEHEELINDEX] = expBufferTemp[TIMEHEELINDEX];
			expBuffer[FILTERHEELINDEX] = expBufferTemp[FILTERHEELINDEX];
			expBuffer[MIXHEELINDEX] = expBufferTemp[MIXHEELINDEX];
			for(int n = 0; n < 11; n++) //for each knob
			{
				int16_t tempDifference = bypassPressSnapshot[n] - bypassReleaseSnapshot[n]; //get difference between knob postions
				int8_t directionIndex = EXPRESSIONSTART + (n*3); //this should be the right index for direction setting as we move through the knob checking
				if( tempDifference > ROUGHLYEQUAL || tempDifference < -ROUGHLYEQUAL) //if the knob moved
					expressionChanged = expBuffer[directionIndex] = ON; //mark it, and mark that one of our expression elements changed
				else //if not
					expBuffer[directionIndex] = OFF; //mark the direction as off, leave expressionChanged as it was
			}
			if(expressionChanged == ON) //if expression changed
			{
				writeGlobalExpression(expBuffer); //store in preset slot 0 area
				expressionChanged = OFF; //and turn the flag back off
				scoochAsTap = 0;
				GPIO_InitTypeDef GPIO_InitStruct = {0};
				GPIO_InitStruct.Pin = GPIO_PIN_6; //for expression pin
				GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; //make mode analog
				GPIO_InitStruct.Pull = GPIO_NOPULL; //disable the pullup
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //and write these init states
				writeByte(scoochAsTap, SCOOCHASTAPINDEX); //store this
			}
			else //if it didn't
			{
				readPreset(expBuffer, preset); //set the expression buffer back to its last stored state based on preset (or live mode)
			}
			if(assigningBypass == ON) //if we were assigning bypass info
			{
				  assigningBypass = OFF; //stop doing that
				  writeByte(stereoState, STEREOSTATEINDEX); //write stereo state to EEPROM
				  HAL_Delay(5); //make sure it sticks
				  writeByte(trailsState, TRAILSINDEX); //write trails state to EEPROM
				  HAL_Delay(5); //make sure it sticks
				  ledTactile = preset; //and set the LEDs back to preset mode
			}
			if(auxWasPressed == 1) //if the tap switch was pressed while bypass was held
			{
				//assign expression to scooch-as-tap usage
				if(scoochAsTap == 0) //if not using scooch for tap
				{
					scoochAsTap = 1;
					GPIO_InitTypeDef GPIO_InitStruct = {0};
					GPIO_InitStruct.Pin = GPIO_PIN_6; //for expression pin
					GPIO_InitStruct.Mode = GPIO_MODE_INPUT; //make mode input
					GPIO_InitStruct.Pull = GPIO_PULLUP; //and enable the pullup
					HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //and write these init states
				}
				else
				{
					scoochAsTap = 0;
					GPIO_InitTypeDef GPIO_InitStruct = {0};
					GPIO_InitStruct.Pin = GPIO_PIN_6; //for expression pin
					GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; //make mode analog
					GPIO_InitStruct.Pull = GPIO_NOPULL; //disable the pullup
					HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); //and write these init states
				}
				writeByte(scoochAsTap, SCOOCHASTAPINDEX); //store this
			}
			if(onOffState == 0)
				  turnOn();
			else
				  turnOff();
		  }
		  clearAllFlags(&fsBypass);
	  }

	  //when hold switches
	  if(fsAux.switchRegistered == 1)
	  {
		  //pause pots with alt settings, or controlled by aux
		  potDepth.pauseReading = 1;
		  potMix.pauseReading = 1;
		  potRate.pauseReading = 1;
		  potTime.pauseReading = 1;
		  potDissolve.pauseReading = 1;
		  //remember to pause expression if this switch can affect a direct parameter, like with decay on Sunlight
		  if(fsAux.value == 0)
		  {
			  bypassWasPressed = presetWasPressed = lfoWasPressed = 0; //clear the press flags
			  auxWasPressed = 1; //and mark its own press flag
			  //if(auxSetting == AUXASTAP) //if using footswitch for tap
				  registerTap(AUX); //register a tap
			  /*else if(auxSetting == AUXASDOUBLE)
			  {
				  if(footswitchDouble == 1.0f)
				  {
					  footswitchDouble = 0.5f;
					  //led_on(&ledAux);
				  }
				  else
				  {
					  footswitchDouble = 1.0f;
					  //led_off(&ledAux);
				  }
			  }
			  else //otherwise, do preset action
			  {
				  preset++; //advance the preset
				  if(preset > 3) //if past the on-board number
					  preset = 0; //set back to 0
				  ledTactile = preset; //assign the led
				  if(savingPreset == 0) //if we're not currently in the saving mode
				  {
					  //pause all pots to avoid immediate override when loading preset
					  potRate.pauseReading = 1;
					  potDepth.pauseReading = 1;
					  potShape.pauseReading = 1;
					  potDissolve.pauseReading = 1;
					  potChance.pauseReading = 1;
					  potSmear.pauseReading = 1;
					  potGlitch.pauseReading = 1;
					  potTime.pauseReading = 1;
					  potFilter.pauseReading = 1;
					  potMix.pauseReading = 1;
					  potExp.pauseReading = 1;
					  loadPreset(presetRead, preset);
				  }
			  }*/
		  }
		  else if(auxWasHeld == 1 || fsAux.holdFlag == 1) //if it was held and is being let go
		  {

			  if(presetWasPressed == 1) //if tactile was pressed
			  {
				  //auxSetting = AUXASPRESET; //assign aux to presets
				  //writeByte(auxSetting, AUXSETTINGINDEX); //store it
			  }
			  else if(lfoWasPressed == 1) //if one of the lfo switches was pressed
			  {
				  //auxSetting = AUXASDOUBLE; //assign aux to tap tempo
				  //writeByte(auxSetting, AUXSETTINGINDEX); //store it
			  }
			  else if(bypassWasPressed == 1)
			  {
				  //auxSetting = AUXASTAP; //assign aux to freeze/hold
				  //writeByte(auxSetting, AUXSETTINGINDEX); //store it
			  }
			  footswitchDouble = 1.0f; //make sure, regardless of new setting, footswitch isn't manipulating timing
			  led_off(&ledAux); //and show that visually
			  auxWasHeld = 0; //clear the hold marker
		  }
		  clearAllFlags(&fsAux);
	  }
	  //this function might not be needed anymore, but leave the flag while testing
	  if(fsAux.holdFlag == 1 && fsAux.value == 0) //if tap switch has been held down
	  {
		  if(auxSetting != AUXASPRESET) //if not changing presets
			  //runawayOn(); //trigger runaway
		  clearHoldFlag(&fsAux); //clear the flag
		  auxWasHeld = 1; //but mark this flag
	  }



	  //when tactile switches
	  if(tactile.switchRegistered == 1) //if tactile switch was pressed
	  {
		  presetWasPressed = 1;
		  //auxPressSnapshot = 1; //mark the press every time, so if Tap was held then released it will know to assign Tap to preset switching
		  if(tactile.value == 1) //if *releasing* the switch
		  {
			  if (tactWasHeld == 0 && fsAux.value != 0 && fsBypass.value != 0)//if tactile wasn't held, and neither footswitch is engaged
			  {
				  preset++; //advance the preset
				  if(preset > 3) //if past the on-board number
					  preset = 0; //set back to 0
				  ledTactile = preset; //assign the led
				  if(savingPreset == 0) //if we're not currently in the saving mode
				  {
					  //pause all pots to avoid immediate override when loading preset
					  potRate.pauseReading = 1;
					  potDepth.pauseReading = 1;
					  potShape.pauseReading = 1;
					  potDissolve.pauseReading = 1;
					  potChance.pauseReading = 1;
					  potSmear.pauseReading = 1;
					  potGlitch.pauseReading = 1;
					  potTime.pauseReading = 1;
					  potFilter.pauseReading = 1;
					  potMix.pauseReading = 1;
					  potExp.pauseReading = 1;
					  loadPreset(presetRead, preset);
				  }

			  }
			  else if (tactWasHeld == 0 && fsBypass.value == 0)//tactile switch wasn't held and bypass switch is engaged
			  {
				  stereoState++; //move to next stereo setting
				  if(stereoState > 3)
					  stereoState = 1;
				  if(stereoState == MISO) //check for mono in stereo out and switch relay as needed
					  misoOn();
				  else
					  misoOff();
				  ledTactile = stereoState; //mark the LED position
				  //set the LED ticker top
				  if(trailsState == TRAILSON)
				  {
					  ledTickerTop = 125;
					  led_setTop(&ledP1, ledTickerTop);
					  led_setTop(&ledP2, ledTickerTop);
					  led_setTop(&ledP3, ledTickerTop);
				  }
				  else
				  {
					  ledTickerTop = 250;
					  led_setTop(&ledP1, ledTickerTop);
					  led_setTop(&ledP2, ledTickerTop);
					  led_setTop(&ledP3, ledTickerTop);
				  }
				  if(assigningBypass == OFF) //if we weren't already assigning bypass
				  {
					  //start the LEDs up
					  led_setTop(&ledP1, ledTickerTop);
					  led_setTop(&ledP2, ledTickerTop);
					  led_setTop(&ledP3, ledTickerTop);
					  led_clear(&ledP1);
					  led_clear(&ledP2);
					  led_clear(&ledP3);
				  }
				  assigningBypass = ON; //and mark that we're assigning now
			  }
		  }
		  clearAllFlags(&tactile);
		  tactWasHeld = 0; //also clear this hold marker
	  }

	  //Tactile Switch Bank
	  	  if(tactMod.switchRegistered == 1) //if time tactile switched
	  	  {
	  		  lfoWasPressed = 1; //set flag to mark the press
	  		  if(fsAux.value == 1) //if hold switch isn't currently being pressed
	  		  {
	  			  uint8_t temp = modeBinary; //grab current state
	  			  if(tactMod.value == 0) //if pressed down
	  				  modMode = !modMode; //flip the state
	  			  else if(tactMod.holdFlag == 1) //or if released from a press
	  				  modMode = !modMode; //flip the state
	  			  //mark the state within the binary
	  			  if(modMode == TREMOLO)
	  				  modeBinary &= MODMODETREMOLO;
	  			  else
	  				  modeBinary |= MODMODEVIBRATO;
	  			  led_set(&ledTime, modMode);  //set the LED
	  			  if(temp != modeBinary) //if state changed
	  				  writeByte(modeBinary, MODESINDEX); //store it
	  		  }
	  		  clearAllFlags(&tactMod);
	  	  }
	  	  if(tactFilter.switchRegistered == 1) //if filter tactile switched
	  	  {
	  		  lfoWasPressed = 1; //set flag to mark the press
	  		  if(fsAux.value == 1) //if hold switch isn't currently being pressed
	  		  {
	  			  uint8_t temp = modeBinary; //grab current state
	  			  if(tactFilter.value == 0) //if pressed down
	  				  filterMode = !filterMode; //flip the state
	  			  else if(tactFilter.holdFlag == 1) //or if released from a press
	  				  filterMode = !filterMode; //flip the state
	  			  //mark the state within the binary
	  			  if(filterMode == LOWPASS)
	  				  modeBinary &= FILTERMODELOWPASS;
	  			  else
	  				  modeBinary |= FILTERMODEHIGHPASS;
	  			  led_set(&ledFilter, filterMode);  //set the LED
	  			  if(temp != modeBinary) //if state changed
	  				  writeByte(modeBinary, MODESINDEX); //store it
	  		  }
	  		  clearAllFlags(&tactFilter);
	  	  }
	  	  if(tactMix.switchRegistered == 1) //if filter tactile switched
	  	  {
	  		  lfoWasPressed = 1; //set flag to mark the press
	  		  potMix.pauseReading = 1; //pause the pot whose function is changed by this switch
	  		  if(fsAux.value == 1) //if hold switch isn't currently being pressed
	  		  {
	  			  uint8_t temp = modeBinary; //grab current state
	  			  if(tactMix.value == 0) //if pressed down
	  				  mixMode = !mixMode; //flip the state
	  			  else if(tactMix.holdFlag == 1) //or if released from a press
	  				  mixMode = !mixMode; //flip the state
	  			  //mark the state within the binary
	  			  if(mixMode == TRUEMIX)
	  				  modeBinary &= MIXMODETRUE;
	  			  else
	  				  modeBinary |= MIXMODEMODIFIED;
	  			  led_set(&ledMix, mixMode);  //set the LED
	  			  if(temp != modeBinary) //if state changed
	  				  writeByte(modeBinary, MODESINDEX); //store it
	  		  }
	  		  clearAllFlags(&tactMix);
	  	  }

	  //when expression/external switches
	  if(scooch.switchRegistered == 1)
	  {
		  if(scooch.value == 0) //if pressed down
		  {
			  registerTap(AUX); //mark the tap
		  }
		  clearAllFlags(&scooch); //and reset
	  }



	  //PRESET SAVING
	  if(tactile.longHoldFlag == 1 && tactile.value == 0) //if tactile switch has been pressed for a long hold
	  {
		  if(savingPreset == 1) //if we were saving preset
		  {
			  savingPreset = 0; //exit that mode
			  if(preset != 0) //don't save if preset slot is zero
			  {
				  for(int n = EXPRESSIONSTART; n < PRESETARRAYLENGTH; n++)
					  currentSetting[n] = expBuffer[n]; //load currentsettings with info from expbuffer
				  if(noSubdivisionsTime == 0) //if subdivisions are allowed
					  currentSetting[TIMEINDEX] = currentSetting[TIMEINDEX] * currentSetting[SUBTIMEINDEX]; //hardcode the time so it matches on recall
				  if(noSubdivisionsRate == 0) //if subdivisions are allowed
					  currentSetting[RATEINDEX] = currentSetting[RATEINDEX] * currentSetting[SUBRATEINDEX]; //hardcode the rate so it matches on recall
				  writePreset(currentSetting, preset); //store preset
			  }
		  }
		  else if(fsBypass.value == 0) //if bypass is pressed
		  {
			  //flip trails state
			  if(trailsState == TRAILSOFF) trailsState = TRAILSON;
			  else trailsState = TRAILSOFF;
			  if(onOffState == OFF) //if currently off
				  turnOff(); //trigger relay change
			  ledTactile = stereoState; //mark the LED position
			  //set the LED ticker top
			  if(trailsState == TRAILSON)
			  {
				  ledTickerTop = 125;
				  led_setTop(&ledP1, ledTickerTop);
				  led_setTop(&ledP2, ledTickerTop);
				  led_setTop(&ledP3, ledTickerTop);
			  }
			  else
			  {
				  ledTickerTop = 250;
				  led_setTop(&ledP1, ledTickerTop);
				  led_setTop(&ledP2, ledTickerTop);
				  led_setTop(&ledP3, ledTickerTop);
			  }
			  if(assigningBypass == OFF) //if we weren't already assigning bypass
			  {
				  //start the LEDs up
				  led_setTop(&ledP1, ledTickerTop);
				  led_setTop(&ledP2, ledTickerTop);
				  led_setTop(&ledP3, ledTickerTop);
				  led_clear(&ledP1);
				  led_clear(&ledP2);
				  led_clear(&ledP3);
			  }
			  assigningBypass = ON; //and mark that we're assigning now
		  }
		  else //otherwise, we're entering preset save mode
		  {
			  savingPreset = 1;
			  if(preset == 0) preset = 1; //if in live mode (preset is 0), start at preset 1, to avoid confusion where nothing is blinking
			  ledTactile = preset; //assign the led
			  ledTickerTop = 500;
			  led_setTop(&ledP1, ledTickerTop);
			  led_setTop(&ledP2, ledTickerTop);
			  led_setTop(&ledP3, ledTickerTop);
			  led_clear(&ledP1);
			  led_clear(&ledP2);
			  led_clear(&ledP3);
		  }
		  tactile.longHoldFlag = 0; //clear long hold flag
		  tactWasHeld = 1; //but mark that we just cleared it
	  }


	  ledDelayCounter++;
	  if(ledDelayCounter == 30)
		  ledDelayCounter = 0;
	  if(ledDelayCounter == 0)
	  {
		  //LED toggling
		  if(ledTactile == 0)
		  {
			  //led_set(&ledP1, vibratoActive);
			  //led_set(&ledP2, flangeActive);
			  //led_set(&ledP3, tremoloActive);
		  }
		  else if(ledTactile == 1)
		  {
			  led_off(&ledP2);
			  led_off(&ledP3);
			  if(savingPreset == 0 && assigningHold == OFF && assigningBypass == OFF) //if not saving preset or assigning hold or bypass, keep LEDs solid
			  {
				  led_on(&ledP1);
			  }
			  else if(ledP1.flag == 1) //else, and flag was set
				  led_flip(&ledP1); //flip the LED
		  }
		  else if(ledTactile == 2)
		  {
			  led_off(&ledP1);
			  led_off(&ledP3);
			  if(savingPreset == 0 && assigningHold == OFF && assigningBypass == OFF) //if not saving preset, keep LEDs solid
			  {
				  led_on(&ledP2);
			  }
			  else if(ledP2.flag == 1) //else, and flag was set
				  led_flip(&ledP2); //flip the LED
		  }
		  else if(ledTactile == 3)
		  {
			  led_off(&ledP1);
			  led_off(&ledP2);
			  if(savingPreset == 0 && assigningHold == OFF && assigningBypass == OFF) //if not saving preset, keep LEDs solid
			  {
				  led_on(&ledP3);
			  }
			  else if(ledP3.flag == 1) //else, and flag was set
				  led_flip(&ledP3); //flip the LED
		  }
		  else
		  {
			  if(savingPreset == 0 && assigningHold == OFF && assigningBypass == OFF) //if not saving preset, keep LEDs solid
			  {
				  led_on(&ledP1);
				  led_on(&ledP2);
				  led_on(&ledP3);
			  }
			  else //otherwise
			  {
				  if(ledP1.flag == 1) //if flag was set
					  led_flip(&ledP1); //flip the LED
				  if(ledP2.flag == 1) //if flag was set
					  led_flip(&ledP2); //flip the LED
				  if(ledP3.flag == 1) //if flag was set
					  led_flip(&ledP3); //flip the LED
			  }
		  }

		  /*if(auxSetting == AUXASPRESET) //if using tap as preset switcher
		  {
			  if(ledAux.flag == 1)
				  led_flip(&ledAux);
		  }*/
	  }

	  //factory reset
	  if(fsAux.counter > 10000 && fsAux.value == 0 && fsBypass.counter > 10000 && fsBypass.value == 0)
	  {
		  factoryReset();
	  }


	  //eeprom queue
	  if(subRateEEFlag == 1) //if sub1 needs to be written
	  {
		  writeByte(targetSetting[SUBRATEINDEX] * FLOAT_TO_UINT8, SUBRATEINDEX); //write it
		  subRateEEFlag = 0; //clear flag
	  }
	  if(subTimeEEFlag == 1) //if sub1 needs to be written
	  {
		  writeByte(targetSetting[SUBTIMEINDEX] * FLOAT_TO_UINT8, SUBTIMEINDEX); //write it
		  subTimeEEFlag = 0; //clear flag
	  }
	  if(offsetEEFlag == 1) //if sub1 needs to be written
	  {
		  writeByte(targetSetting[OFFSETINDEX] * FLOAT_TO_UINT8, OFFSETINDEX); //write it
		  offsetEEFlag = 0; //clear flag
	  }
	  if(volumeEEFlag == 1) //if sub1 needs to be written
	  {
		  writeByte(targetSetting[VOLUMEINDEX] * FLOAT_TO_UINT8, VOLUMEINDEX); //write it
		  volumeEEFlag = 0; //clear flag
	  }



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 48;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 63;
  RCC_OscInitStruct.PLL.PLLR = 4;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 4096;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_SAI1;
  PeriphClkInitStruct.PLL2.PLL2M = 4;
  PeriphClkInitStruct.PLL2.PLL2N = 58;
  PeriphClkInitStruct.PLL2.PLL2P = 76;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 3072;
  PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL2;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 11;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.Oversampling.Ratio = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV64;
  hadc1.Init.Resolution = ADC_RESOLUTION_10B;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_32CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_17;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  sConfig.Offset = 32;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_15;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_19;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_7;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_REGULAR_RANK_8;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_9;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = ADC_REGULAR_RANK_10;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_11;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10C0E7F5;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SAI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SAI1_Init(void)
{

  /* USER CODE BEGIN SAI1_Init 0 */

  /* USER CODE END SAI1_Init 0 */

  /* USER CODE BEGIN SAI1_Init 1 */

  /* USER CODE END SAI1_Init 1 */
  hsai_BlockA1.Instance = SAI1_Block_A;
  hsai_BlockA1.Init.AudioMode = SAI_MODEMASTER_TX;
  hsai_BlockA1.Init.Synchro = SAI_ASYNCHRONOUS;
  hsai_BlockA1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockA1.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
  hsai_BlockA1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockA1.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_48K;
  hsai_BlockA1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockA1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockA1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockA1.Init.TriState = SAI_OUTPUT_NOTRELEASED;
  if (HAL_SAI_InitProtocol(&hsai_BlockA1, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_32BIT, 2) != HAL_OK)
  {
    Error_Handler();
  }
  hsai_BlockB1.Instance = SAI1_Block_B;
  hsai_BlockB1.Init.AudioMode = SAI_MODESLAVE_RX;
  hsai_BlockB1.Init.Synchro = SAI_SYNCHRONOUS;
  hsai_BlockB1.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
  hsai_BlockB1.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
  hsai_BlockB1.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
  hsai_BlockB1.Init.MonoStereoMode = SAI_STEREOMODE;
  hsai_BlockB1.Init.CompandingMode = SAI_NOCOMPANDING;
  hsai_BlockB1.Init.TriState = SAI_OUTPUT_NOTRELEASED;
  if (HAL_SAI_InitProtocol(&hsai_BlockB1, SAI_I2S_STANDARD, SAI_PROTOCOL_DATASIZE_32BIT, 2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SAI1_Init 2 */

  /* USER CODE END SAI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES_TXONLY;
  hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 0x0;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi2.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi2.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi2.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi2.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi2.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi2.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi2.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi2.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi2.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1023;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 32-1;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 65535;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 0;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 1023;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim16, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */
  HAL_TIM_MspPostInit(&htim16);

}

/**
  * @brief TIM17 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM17_Init(void)
{

  /* USER CODE BEGIN TIM17_Init 0 */

  /* USER CODE END TIM17_Init 0 */

  /* USER CODE BEGIN TIM17_Init 1 */

  /* USER CODE END TIM17_Init 1 */
  htim17.Instance = TIM17;
  htim17.Init.Prescaler = 1940-1;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = 100-1;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM17_Init 2 */

  /* USER CODE END TIM17_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 31250;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 9;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.battery_charging_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, EEWC_Pin|CODECRESET_Pin|CODECCHIPSELECT_Pin|MISORELAY1_Pin
                          |MISORELAY12_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LEDBYPASS_Pin|LEDEXTRA_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, LEDP1_Pin|LEDP2_Pin|LEDP3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, DEEMPHASISCONTROL_Pin|CODECFITLERSELECT_Pin|DOUBLESPEEDMODE_Pin|CODECPARALLELSERIAL_Pin
                          |RIGHTRELAY1_Pin|RIGHTRELAY12_Pin|LEFTRELAY1_Pin|LEFTRELAY12_Pin
                          |LFOLED1_Pin|LFOLED2_Pin|LFOLED3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : EEWC_Pin CODECRESET_Pin CODECCHIPSELECT_Pin MISORELAY1_Pin
                           MISORELAY12_Pin */
  GPIO_InitStruct.Pin = EEWC_Pin|CODECRESET_Pin|CODECCHIPSELECT_Pin|MISORELAY1_Pin
                          |MISORELAY12_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC15 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PH0 PH1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA4 PA8 PA10
                           PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_4|GPIO_PIN_8|GPIO_PIN_10
                          |GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LEDBYPASS_Pin LEDEXTRA_Pin */
  GPIO_InitStruct.Pin = LEDBYPASS_Pin|LEDEXTRA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LFOTACT3_Pin FS2_Pin */
  GPIO_InitStruct.Pin = LFOTACT3_Pin|FS2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LFOTACT2_Pin LFOTACT1_Pin PRESETTACT_Pin FS1_Pin */
  GPIO_InitStruct.Pin = LFOTACT2_Pin|LFOTACT1_Pin|PRESETTACT_Pin|FS1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : LEDP1_Pin LEDP2_Pin LEDP3_Pin */
  GPIO_InitStruct.Pin = LEDP1_Pin|LEDP2_Pin|LEDP3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PE13 PE15 PE0 PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PB11 PB12 PB4 PB5
                           PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PD8 PD9 PD10 PD15
                           PD2 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_15
                          |GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : DEEMPHASISCONTROL_Pin CODECFITLERSELECT_Pin DOUBLESPEEDMODE_Pin CODECPARALLELSERIAL_Pin
                           RIGHTRELAY1_Pin RIGHTRELAY12_Pin LEFTRELAY1_Pin LEFTRELAY12_Pin
                           LFOLED1_Pin LFOLED2_Pin LFOLED3_Pin */
  GPIO_InitStruct.Pin = DEEMPHASISCONTROL_Pin|CODECFITLERSELECT_Pin|DOUBLESPEEDMODE_Pin|CODECPARALLELSERIAL_Pin
                          |RIGHTRELAY1_Pin|RIGHTRELAY12_Pin|LEFTRELAY1_Pin|LEFTRELAY12_Pin
                          |LFOLED1_Pin|LFOLED2_Pin|LFOLED3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x38000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_64KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RW_URO;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
