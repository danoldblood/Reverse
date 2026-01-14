//a structure for building delays
//initializing info is passed here
//it is expected that the main code will create the storage array
//so it's not user-proof, but it should really clean up the process
//NOW EXPANDED! Features will include independent read and write, read direction and speed, etc
//Build expansion based on the modulated read head version, most stable/clean sounding so far

#ifndef INC_delay_clocked_v2_H_
#define INC_delay_clocked_v2_H_

#include <stdio.h>
#include <stdint.h>

typedef struct delay_clocked_v2 {
    int32_t writeHead;
    float writeHeadF;
    float writeHeadPartial;
    int32_t readHead;
    float readHeadPartial;
    int32_t size;
    float sizeF;
    float time;
    float timePosition;
    float direction;
    int32_t timeInt;
    float *start;
    float smoothing;
    float clockSpeed;
    float delayTarget;
    float timeFiltered;
    float xfade;
    float lastSample;
    float accumulatedInput;
    float accAverager;
    uint8_t newWriteHeadFlag;
    uint8_t lockInSampleFlag;
    uint8_t externalFlag1;
    uint8_t externalFlag2;
    uint8_t externalFlag3;
    uint8_t externalFlag4;
} delay_clocked_v2;

void delay_clocked_v2_init(delay_clocked_v2 *self, int32_t initSize, float *initStart);
void delay_clocked_v2_setSize(delay_clocked_v2 *self, int32_t newSize);
void delay_clocked_v2_setTime(delay_clocked_v2 *self, int32_t newTime);
void delay_clocked_v2_setStart(delay_clocked_v2 *self, float *newStart);
void delay_clocked_v2_snapTime(delay_clocked_v2 *self);
void delay_clocked_v2_setDirection(delay_clocked_v2 *self, float newDirection);
void delay_clocked_v2_setSmoothing(delay_clocked_v2 *self, float newSmoothing);
void delay_clocked_v2_setClockSpeed(delay_clocked_v2 *self, float newClockSpeed);
float delay_clocked_v2_inOutModulatedLoFiVariable(delay_clocked_v2 *self, float input, float modulation);
void delay_clocked_v2_in(delay_clocked_v2 *self, float input);
void delay_clocked_v2_in_lofi(delay_clocked_v2 *self, float input);
float delay_clocked_v2_out(delay_clocked_v2 *self);
float delay_clocked_v2_out_modulated(delay_clocked_v2 *self, float modulation);
void delay_clocked_v2_tick_lofi_variable(delay_clocked_v2 *self);
float delay_clocked_v2_outModulated(delay_clocked_v2 *self, float modulation);

#endif /* INC_delay_H_ */
