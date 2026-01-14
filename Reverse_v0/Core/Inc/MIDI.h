/*OBNE MIDI DEFINITIONS
 * Definitions to more easily parse bits and whatnot. To be continually updated and implemented across projects.
 * The beginning of history -DP
 * (JK -DP)
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MIDI_H
#define __MIDI_H

#ifdef __cplusplus
extern "C" {
#endif

//definitions based on MIDI standard, as well as definitions specific to Black Fountain

//status bytes
#define CONTROLCHANGE 0xB//1011
#define PROGRAMCHANGE 0xC

//realtime
#define MIDICLOCKPULSE 0x8
#define MIDISTART 0xA
#define MIDICLOCKPULSEBYTE 0xF8
#define MIDISTARTBYTE 0xFA

#define CHANNEL1 0x0
#define CHANNEL2 0x1
#define CHANNEL3 0x2
#define CHANNEL4 0x3
#define CHANNEL5 0x4
#define CHANNEL6 0x5
#define CHANNEL7 0x6
#define CHANNEL8 0x7
#define CHANNEL9 0x8
#define CHANNEL10 0x9
#define CHANNEL11 0xA
#define CHANNEL12 0xB
#define CHANNEL13 0xC
#define CHANNEL14 0xD
#define CHANNEL15 0xE
#define CHANNEL16 0xF

//#define VOLUMECC 7
#define EXPCC 11

#define RATECC 14
#define DEPTHCC 15
#define SHAPECC 	16
#define DISSOLVECC 		17
#define CHANCECC		18
#define SMEARCC 	19
#define GLITCHCC 	20
#define TIMECC 	21
#define FILTERCC 		22
#define MIXCC 	23
#define OFFSETCC 	24
#define SUBRATECC 	25
#define SUBTIMECC 	26
#define VOLUMECC 	27
#define MODMODECC	28
#define FILTERMODECC 29
#define MIXMODECC	30

#define BYPASSSWITCHCC 85
#define TAPSWITCHCC 86
#define HALFTIMESWITCHCC 87
#define PRESETSAVESWITCH 89

#define MIDICHANNELCC 102
#define TRAILSSETTINGCC 103
#define STEREOSETTINGCC 104
#define CLOCKMODECC		105
#define AUXMODECC		106



/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
