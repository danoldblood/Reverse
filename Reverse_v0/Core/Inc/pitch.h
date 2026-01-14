//a structure for building FV-1 style pitch shifters
//initializing info is passed here
//it is expected that the main code will create the storage array
//so it's not user-proof, but it should really clean up the process

#ifndef INC_pitch_H_
#define INC_pitch_H_

#include <stdio.h>
#include <stdint.h>

typedef struct pitchFV1 {
    float readHead1;
    float readHead2;
    int16_t writeHead;
    int16_t size;
    float shift;
    float *start;
    float rampPos;
} pitchFV1;

void pitchFV1_init(pitchFV1 *self, int16_t initSize, float *initStart);
void pitchFV1_setShiftDecimal(pitchFV1 *self, float shiftDecimal);
void pitchFV1_setShiftSemitone(pitchFV1 *self, float shiftSemitone);
float pitchFV1_inOut(pitchFV1 *self, float input);

#endif /* INC_delay_H_ */
