//a structure for building delays
//initializing info is passed here
//it is expected that the main code will create the storage array
//so it's not user-proof, but it should really clean up the process
//NOW EXPANDED! Features will include independent read and write, read direction and speed, etc
//Build expansion based on the modulated read head version, most stable/clean sounding so far

#ifndef INC_delay_parting_H_
#define INC_delay_parting_H_

#include <stdio.h>
#include <stdint.h>

#define VOLUMETICKER 0.01f //determine the number of samples necessary for smooth crossfade, make this 1/that
#define VOLUMEDROPRANGE 5

typedef struct delay_parting {
    float readHead;
    int32_t writeHead;
    float writeHeadF;
    int32_t size;
    float time;
    float timePosition;
    float direction;
    int32_t timeInt;
    float *start;
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
} delay_parting;

void delay_parting_init(delay_parting *self, int32_t initSize, float *initStart);
void delay_parting_setSize(delay_parting *self, int32_t newSize);
void delay_parting_setTime(delay_parting *self, int32_t newTime);
void delay_parting_setStart(delay_parting *self, float *newStart);
void delay_parting_snapTime(delay_parting *self);
void delay_parting_setDirection(delay_parting *self, float newDirection);
void delay_parting_setSmoothing(delay_parting *self, float newSmoothing);
void delay_parting_setMultiplier(delay_parting *self, float newMultiplier);
void delay_parting_setClockSpeed(delay_parting *self, float newClockSpeed);
void delay_parting_reverseOnce(delay_parting *self, float distance);
float delay_parting_inOutModulatedForward(delay_parting *self, float input, float modulation);
float delay_parting_inOutModulatedForwardSmooth(delay_parting *self, float input, float modulation);
float delay_parting_inOutModulatedReverse(delay_parting *self, float input, float modulation);
void delay_parting_in(delay_parting *self, float input);
void delay_parting_in_lofi(delay_parting *self, float input);
void delay_parting_tick_forward(delay_parting *self);
void delay_parting_tick_forward_smooth(delay_parting *self);
void delay_parting_tick_reverse(delay_parting *self);
float delay_parting_out_modulated_forward(delay_parting *self, float modulation);
float delay_parting_out_modulated_reverse(delay_parting *self, float modulation);

#endif /* INC_delay_H_ */
