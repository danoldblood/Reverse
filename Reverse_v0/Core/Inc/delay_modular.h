//a structure for building delays
//initializing info is passed here
//it is expected that the main code will create the storage array
//so it's not user-proof, but it should really clean up the process
//NOW EXPANDED! Features will include independent read and write, read direction and speed, etc
//Build expansion based on the modulated read head version, most stable/clean sounding so far

#ifndef INC_delay_modular_H_
#define INC_delay_modular_H_

#include <stdio.h>
#include <stdint.h>

typedef struct delay_modular {
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
    uint8_t externalFlag;
} delay_modular;

void delay_modular_init(delay_modular *self, int32_t initSize, float *initStart);
void delay_modular_setSize(delay_modular *self, int32_t newSize);
void delay_modular_setTime(delay_modular *self, int32_t newTime);
void delay_modular_setStart(delay_modular *self, float *newStart);
void delay_modular_snapTime(delay_modular *self);
void delay_modular_setDirection(delay_modular *self, float newDirection);
void delay_modular_setSmoothing(delay_modular *self, float newSmoothing);
void delay_modular_setMultiplier(delay_modular *self, float newMultiplier);
void delay_modular_setClockSpeed(delay_modular *self, float newClockSpeed);
void delay_modular_reverseOnce(delay_modular *self, float distance);
float delay_modular_inOut(delay_modular *self, float input);
float delay_modular_inOutModulated(delay_modular *self, float input, float modulation);
float delay_modular_inOutModulatedLoFi(delay_modular *self, float input, float modulation);
float delay_modular_inOutModulatedLoFiVariable(delay_modular *self, float input, float modulation);
void delay_modular_in(delay_modular *self, float input);
void delay_modular_in_lofi(delay_modular *self, float input);
float delay_modular_out(delay_modular *self);
float delay_modular_out_modulated(delay_modular *self, float modulation);
void delay_modular_tick(delay_modular *self);
void delay_modular_tick2(delay_modular *self);
void delay_modular_tick3(delay_modular *self);
void delay_modular_tick_lofi(delay_modular *self);
void delay_modular_tick_lofi_experimental(delay_modular *self);
void delay_modular_tick_lofi_variable(delay_modular *self);
float delay_modular_outModulated(delay_modular *self, float modulation);

#endif /* INC_delay_H_ */
