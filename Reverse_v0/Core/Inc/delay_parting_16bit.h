//a structure for building delays
//initializing info is passed here
//it is expected that the main code will create the storage array
//so it's not user-proof, but it should really clean up the process
//NOW EXPANDED! Features will include independent read and write, read direction and speed, etc
//Build expansion based on the modulated read head version, most stable/clean sounding so far

#ifndef INC_delay_parting_16bit_H_
#define INC_delay_parting_16bit_H_

#include <stdio.h>
#include <stdint.h>

#define VOLUMETICKER16BIT 0.001f //determine the number of samples necessary for smooth crossfade, make this 1/that
#define VOLUMEDROPRANGE16BIT 5

typedef struct delay_parting_16bit {
    float readHead;
    int32_t writeHead;
    float writeHeadF;
    int32_t size;
    float time;
    float timePosition;
    float direction;
    int32_t timeInt;
    int16_t *start;
    float smoothing;
    float multiplier;
    float clockSpeed;
    float oneTimeFlag;
    float distanceTracker;
    float delayTarget;
    float timeFiltered;
    float xfade;
    float lastInput;
    float accumulatedInput;
    float accAverager;
    float writeHeadPartial;
    uint8_t newWriteHeadFlag;
    float volume;
    float volumeDirection;
} delay_parting_16bit;

void delay_parting_16bit_init(delay_parting_16bit *self, int32_t initSize, int16_t *initStart);
void delay_parting_16bit_setSize(delay_parting_16bit *self, int32_t newSize);
void delay_parting_16bit_setTime(delay_parting_16bit *self, int32_t newTime);
void delay_parting_16bit_setQueuedTime(delay_parting_16bit *self, int32_t newTime);
void delay_parting_16bit_setStart(delay_parting_16bit *self, int16_t *newStart);
void delay_parting_16bit_snapTime(delay_parting_16bit *self);
void delay_parting_16bit_setDirection(delay_parting_16bit *self, float newDirection);
void delay_parting_16bit_setSmoothing(delay_parting_16bit *self, float newSmoothing);
void delay_parting_16bit_setMultiplier(delay_parting_16bit *self, float newMultiplier);
void delay_parting_16bit_setClockSpeed(delay_parting_16bit *self, float newClockSpeed);
void delay_parting_16bit_reverseOnce(delay_parting_16bit *self, float distance);
int16_t delay_parting_16bit_inOutModulatedForward(delay_parting_16bit *self, int16_t input, float modulation);
int16_t delay_parting_16bit_inOutModulatedForwardSmooth(delay_parting_16bit *self, int16_t input, float modulation);
int16_t delay_parting_16bit_inOutModulatedReverse(delay_parting_16bit *self, int16_t input, float modulation);
void delay_parting_16bit_in(delay_parting_16bit *self, int16_t input);
void delay_parting_16bit_in_lofi(delay_parting_16bit *self, int16_t input);
void delay_parting_16bit_tick_forward(delay_parting_16bit *self);
void delay_parting_16bit_tick_forward_smooth(delay_parting_16bit *self);
void delay_parting_16bit_tick_reverse(delay_parting_16bit *self);
int16_t delay_parting_16bit_out_modulated_forward(delay_parting_16bit *self, float modulation);
int16_t delay_parting_16bit_out_modulated_reverse(delay_parting_16bit *self, float modulation);

#endif /* INC_delay_H_ */
