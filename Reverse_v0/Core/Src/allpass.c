#include "allpass.h"

void allpass_init(allpass *self, int32_t initSize, float *initStart, float initK)
{
	delay_parting_init(&self->memory, initSize, initStart); //initialize the delay memory
	delay_parting_setTime(&self->memory, (initSize-1)); //and set the time to match the size (most of the time, this will be left alone, but for things like dweller we'll need to create shorter times within the existing buffer)
	self->K = initK;
	self->Kneg = -initK;
	self->Kout = 1.0f - (initK * initK);
	self->previousOut = 0.0f;
	self->dwell = 1.0f;
}
void allpass_setTime(allpass *self, int32_t newTime)
{
	//delay_parting_setQueuedTime(&self->memory, newTime);
	delay_parting_setTime(&self->memory, newTime);
}
void allpass_setSize(allpass *self, int32_t newSize)
{
	delay_modular_setSize(&self->memory, newSize);
}
void allpass_setStart(allpass *self, float *newStart)
{
	delay_modular_setStart(&self->memory, newStart);
}
void allpass_setK(allpass *self, float newK)
{
	self->K = newK;
	self->Kneg = -newK;
	//self->Kout = 1.0f - (newK * newK);
}
void allpass_setDwell(allpass *self, float newDwell)
{
	self->dwell = newDwell;
}
float allpass_inOutDwell(allpass *self, float newInput)
{
	float input = newInput + (self->K * self->previousOut);
	float output = self->previousOut + input*self->Kneg;
	//input = newInput;
	delay_modular_in(&self->memory, input);
	self->previousOut = delay_modular_out(&self->memory);
	//output = delay_modular_out(&self->memory);
	delay_modular_tick(&self->memory);
	//output = delay_modular_inOutModulated(&self->memory, input, 0.0f);

	return output;
}
float allpass_inOutModulated(allpass *self, float newInput, float modulation)
{
	float input = newInput + (self->K * self->previousOut);
	float output = self->previousOut + input*self->Kneg;
	self->previousOut = delay_modular_inOutModulated(&self->memory, input, modulation);
	return output;
}
float allpass_inOutModulatedLoFi(allpass *self, float newInput, float modulation)
{
	float input = newInput + (self->K * self->previousOut);
	float output = self->previousOut + input*self->Kneg;
	self->previousOut = delay_parting_inOutModulatedForward(&self->memory, input, modulation);
	return output;
}
/*float allpass_inOutModulatedLoFiVariable(allpass *self, float newInput, float modulation)
{
	float input = newInput + (self->K * self->previousOut);
	float output = self->previousOut + input*self->Kneg;
	self->previousOut = delay_parting_inOutModulatedForward(&self->memory, input, modulation);
	return output;
}*/
/*float allpass_inOutFV1(allpass *self, float newInput)
{
	float input = newInput + (self->Kneg * self->previousOut);
	float output = self->previousOut + input*self->K;
	self->previousOut = delay_inOutModulated(&self->memory, input, 0.0f); //in other versions, this uses delay_inOutNoInterp for speed - for Dweller, use interp

	return output;
}
float allpass_inOutFV1Modulated(allpass *self, float newInput, float modulation)
{
	float input = newInput + (self->Kneg * self->previousOut);
	float output = self->previousOut + input*self->K;
	//self->previousOut = delay_inOutChorus(&self->memory, input, modulation);
	self->previousOut = delay_inOutModulated(&self->memory, input, modulation);
	return output;
}
float allpass_inOutSchroeder(allpass *self, float newInput)
{
	float input = newInput + (self->K * self->previousOut);
	float output = self->previousOut*self->Kout + input*self->Kneg;
	self->previousOut = delay_inOutNoInterp(&self->memory, input);

	return output;
}
float allpass_inOutSchroederModulated(allpass *self, float newInput, float modulation)
{
	float input = newInput + (self->K * self->previousOut);
	float output = self->previousOut*self->Kout + input*self->Kneg;
	//self->previousOut = delay_inOutChorus(&self->memory, input, modulation);
	self->previousOut = delay_inOutModulated(&self->memory, input, modulation);
	return output;
}*/
