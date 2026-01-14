//a structure for building delays
//initializing info is passed here
//it is expected that the main code will create the storage array
//so it's not user-proof, but it should really clean up the process
//NOW EXPANDED! Features will include independent read and write, read direction and speed, etc
//Build expansion based on the modulated read head version, most stable/clean sounding so far

#ifndef INC_delay_clocked_H_
#define INC_delay_clocked_H_

#include <stdio.h>
#include <stdint.h>

typedef struct delay_clocked {
    double readHead;
    int32_t writeHead;
    double writeHeadF;
    int32_t size;
    double sizeF;
    double time;
    double timePosition;
    double direction;
    int32_t timeInt;
    float *start;
    double smoothing;
    double clockSpeed;
    double delayTarget;
    double timeFiltered;
    double xfade;
    float lastSample;
    float accumulatedInput;
    float accAverager;
    double writeHeadPartial;
    uint8_t newWriteHeadFlag;
    uint8_t lockInSampleFlag;
    uint8_t externalFlag1;
    uint8_t externalFlag2;
    uint8_t externalFlag3;
    uint8_t externalFlag4;
} delay_clocked;

void delay_clocked_init(delay_clocked *self, int32_t initSize, float *initStart);
void delay_clocked_setSize(delay_clocked *self, int32_t newSize);
void delay_clocked_setTime(delay_clocked *self, int32_t newTime);
void delay_clocked_setStart(delay_clocked *self, float *newStart);
void delay_clocked_snapTime(delay_clocked *self);
void delay_clocked_setDirection(delay_clocked *self, double newDirection);
void delay_clocked_setSmoothing(delay_clocked *self, double newSmoothing);
void delay_clocked_setClockSpeed(delay_clocked *self, double newClockSpeed);
float delay_clocked_inOutModulatedLoFiVariable(delay_clocked *self, float input, float modulation);
void delay_clocked_in(delay_clocked *self, float input);
void delay_clocked_in_lofi(delay_clocked *self, float input);
float delay_clocked_out(delay_clocked *self);
float delay_clocked_out_modulated(delay_clocked *self, float modulation);
void delay_clocked_tick_lofi_variable(delay_clocked *self);
float delay_clocked_outModulated(delay_clocked *self, float modulation);

#endif /* INC_delay_H_ */
