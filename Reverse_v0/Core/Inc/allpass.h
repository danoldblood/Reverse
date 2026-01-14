//a structure for building all pass filters
//initializing info is passed here
//it is expected that the main code will create the storage array
//so it's not user-proof, but it should really clean up the process

//note: this sits on top of the delay structure, so it is reliant on that

#ifndef INC_allpass_H_
#define INC_allpass_H_

#include "delay_parting.h"

typedef struct allpass {
	delay_parting memory;
	float K;
	float Kneg;
	float Kout;
	float dwell;
	float previousOut;
} allpass;

void allpass_init(allpass *self, int32_t initSize, float *initStart, float initK);
void allpass_setTime(allpass *self, int32_t newTime);
void allpass_setSize(allpass *self, int32_t newSize);
void allpass_setStart(allpass *self, float *newStart);
void allpass_setK(allpass *self, float newK);
void allpass_setDwell(allpass *self, float newDwell);
float allpass_inOutDwell(allpass *self, float input);
float allpass_inOutModulated(allpass *self, float newInput, float modulation);
float allpass_inOutModulatedLoFi(allpass *self, float newInput, float modulation);
float allpass_inOutModulatedLoFiVariable(allpass *self, float newInput, float modulation);

/*float allpass_inOutFV1(allpass *self, float input);
float allpass_inOutFV1Modulated(allpass *self, float input, float modulation);
float allpass_inOutSchroeder(allpass *self, float input);
float allpass_inOutSchroederModulated(allpass *self, float input, float modulation);*/

#endif /* INC_allpass_H_ */
