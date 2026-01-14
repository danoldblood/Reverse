//a structure for building delays
//initializing info is passed here
//it is expected that the main code will create the storage array
//so it's not user-proof, but it should really clean up the process

#ifndef INC_delay_mt_H_
#define INC_delay_mt_H_

#include <stdio.h>
#include <stdint.h>

typedef struct delay_mt {
    float readHead;
    int32_t readHeadInt;
    int32_t writeHead;
    int32_t size;
    float time;
    uint8_t timeChangeDirection; //0 for shorter, 1 for longer
    int32_t timeInt;
    float *start;
} delay_mt;

void delay_mt_init(delay_mt *self, int32_t initSize, float *initStart);
void delay_mt_setSize(delay_mt *self, int32_t newSize);
void delay_mt_setTime(delay_mt *self, int32_t newTime);
void delay_mt_setStart(delay_mt *self, float *newStart);
void delay_mt_snapTime(delay_mt *self);
float delay_mt_inOut(delay_mt *self, float input);
float delay_mt_inOutModulated(delay_mt *self, float input, float modulation);

#endif /* INC_delay_H_ */
