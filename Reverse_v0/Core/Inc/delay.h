//a structure for building delays
//initializing info is passed here
//it is expected that the main code will create the storage array
//so it's not user-proof, but it should really clean up the process

#ifndef INC_delay_H_
#define INC_delay_H_

#include <stdio.h>
#include <stdint.h>

typedef struct delay {
    float readHead;
    int32_t readHeadInt;
    int32_t writeHead;
    int32_t size;
    float time;
    uint8_t timeChangeDirection; //0 for shorter, 1 for longer
    int32_t timeInt;
    float *start;
} delay;

void delay_init(delay *self, int32_t initSize, float *initStart);
void delay_setTime(delay *self, int32_t newTime);
void delay_snapTime(delay *self);
float delay_inOut(delay *self, float input);
float delay_inOutNoInterp(delay *self, float input);
float delay_inOutChorus(delay *self, float input, float modulation);
float delay_inOutModulated(delay *self, float input, float modulation);

#endif /* INC_delay_H_ */
