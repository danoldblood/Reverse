#include "delay.h"

void delay_init(delay *self, int32_t initSize, float *initStart)
{
	self->size = initSize;
	self->start = initStart;
	self->time = initSize-1;
	self->timeInt = self->time;
	self->readHead = 1.0f;
	self->readHeadInt = 1;
	self->writeHead = 0;
	self->timeChangeDirection = 0;
}
//USE THIS FUNCTION TO SET A NEW TARGET TIME
void delay_setTime(delay *self, int32_t newTime)
{
	self->time = self->timeInt = newTime;
}
//USE THIS FUNCTION TO SNAP THE READ HEAD TO THE NEW TARGET TIME
void delay_snapTime(delay *self)
{
	self->readHead = self->writeHead - (self->time - 1); //set the read head to the intended delay amount
		while(self->readHead < 0) {self->readHead+=self->size;} //wrap around if needed
}
float delay_inOut(delay *self, float input)
{
	float output = 0.0f; //establish output variable
	self->start[self->writeHead] = input; //write new input

	int32_t timeIndexInt = (int32_t)self->readHead; //get int value of current read head
	int32_t timeIndexIntMinus = timeIndexInt - 1; //and one value back
	if(timeIndexIntMinus < 0) //and if it needs to wrap around
	{
		timeIndexIntMinus+=self->size; //do that
	}
	float timeIndexPartial = self->readHead - (float)timeIndexInt; //subtract the int to get just the fractional bit
	//float timeIndexPartialInv = 1.0 - timeIndexPartial; //let's try getting the other side of the fraction
	//output = timeIndexPartial * self->start[timeIndexInt] + timeIndexPartialInv * self->start[timeIndexIntMinus]; //interpolate the delay line
	output = self->start[timeIndexIntMinus] + timeIndexPartial * (self->start[timeIndexInt]  - self->start[timeIndexIntMinus]);
	self->writeHead++; //move the write head forward
	if(self->writeHead > self->size - 1) //and wrap around if needed
	{
		self->writeHead-=self->size;
	}
	float delayTarget = self->writeHead - self->time; //find out the intended delay amount
		if(delayTarget < 0) {delayTarget+=self->size;} //wrap around if needed
	//below logic is based on the Drive spreadsheet "Delay Math Spreadsheet for STM32 Library"
	float difference = delayTarget - self->readHead; //get the difference between current and target
	float readMinusWrite = self->readHead - self->writeHead;
	float writeMinusTarget = self->writeHead - delayTarget;
	if(readMinusWrite < 0.0f && writeMinusTarget < 0.0f)
		difference-=self->size;
	else if(readMinusWrite > 0.0f && writeMinusTarget > 0.0f)
		difference+=self->size;
	float g = difference / self->size; //establish growth parameter by dividing the difference by the buffer size - for some reason, this smooths everything out, vs smaller dividers and other slowdown methods
	self->readHead+=(1.0f+g); //update read index for next goaround
	if(self->readHead > self->size - 1.0f) {self->readHead-=self->size;} //prevent wraparound
	if(self->readHead < 0.0f) {self->readHead+=self->size;} //(in both directions)

	return output; //return the output
}
float delay_inOutModulated(delay *self, float input, float modulation)
{
	float output = 0.0f; //establish output variable
	self->start[self->writeHead] = input; //write new input

	float modulatedReadHead = self->readHead + modulation;
	if(modulatedReadHead > self->size - 1) {modulatedReadHead-=self->size;} //prevent wraparound
	if(modulatedReadHead < 0) {modulatedReadHead+=self->size;} //(in both directions)
	int32_t timeIndexInt = (int32_t)modulatedReadHead; //get int value of current read head
	int32_t timeIndexIntMinus = timeIndexInt - 1; //and one value back
	if(timeIndexIntMinus < 0) //and if it needs to wrap around
	{
		timeIndexIntMinus+=self->size; //do that
	}
	float timeIndexPartial = modulatedReadHead - (float)timeIndexInt; //subtract the int to get just the fractional bit
	output = self->start[timeIndexIntMinus] + timeIndexPartial * (self->start[timeIndexInt]  - self->start[timeIndexIntMinus]);
	self->writeHead++; //move the write head forward
	if(self->writeHead > self->size - 1) //and wrap around if needed
	{
		self->writeHead-=self->size;
	}
	float delayTarget = self->writeHead - self->time; //find out the intended delay amount
		if(delayTarget < 0) {delayTarget+=self->size;} //wrap around if needed
	//below logic is based on the Drive spreadsheet "Delay Math Spreadsheet for STM32 Library"
	float difference = delayTarget - self->readHead; //get the difference between current and target
	float readMinusWrite = self->readHead - self->writeHead;
	float writeMinusTarget = self->writeHead - delayTarget;
	if(readMinusWrite < 0.0f && writeMinusTarget < 0.0f)
		difference-=self->size;
	else if(readMinusWrite > 0.0f && writeMinusTarget > 0.0f)
		difference+=self->size;
	float g = difference * .0001f;// / self->size; //establish growth parameter by dividing the difference by the buffer size - for some reason, this smooths everything out, vs smaller dividers and other slowdown methods
	self->readHead+=(1+g); //update read index for next goaround
	if(self->readHead > self->size - 1) {self->readHead-=self->size;} //prevent wraparound
	if(self->readHead < 0) {self->readHead+=self->size;} //(in both directions)

	return output; //return the output
}
//USE THIS FOR A SIMPLIFIED, NO-INTERPOLATION VERSION
float delay_inOutNoInterp(delay *self, float input)
{
	self->start[self->writeHead] = input; //write new input
	self->writeHead++; //move the write head forward
	if(self->writeHead > self->size - 1) //and wrap around if needed
	{
		self->writeHead-=self->size;
	}
	self->readHeadInt = self->writeHead - (self->timeInt - 1); //set the read head to the intended delay amount
		while(self->readHeadInt < 0) {self->readHeadInt+=self->size;} //wrap around if needed

	return self->start[self->readHeadInt]; //return the output
}
//Use for chorus lines without delay time interpolation
float delay_inOutChorus(delay *self, float input, float modulation)
{
	float output = 0.0f; //establish output variable
	self->start[self->writeHead] = input; //write new input
	//float modulatedReadHead = self->readHead;//+modulation;
	int32_t timeIndexInt = (int32_t)self->readHead; //get int value of current read head
	int32_t timeIndexIntMinus = timeIndexInt - 1; //and one value back
	if(timeIndexIntMinus < 0) //and if it needs to wrap around
	{
		timeIndexIntMinus+=self->size; //do that
	}
	float timeIndexPartial = self->readHead - (float)timeIndexInt; //subtract the int to get just the fractional bit
	//float timeIndexPartialInv = 1.0 - timeIndexPartial; //let's try getting the other side of the fraction
	output = self->start[timeIndexIntMinus] + timeIndexPartial * (self->start[timeIndexInt]  - self->start[timeIndexIntMinus]); //interpolate the delay line
	self->writeHead++; //move the write head forward
	if(self->writeHead > self->size - 1) //and wrap around if needed
	{
		self->writeHead-=self->size;
	}
	self->readHead = self->writeHead - (self->time - 1) + modulation; //set the read head to the intended delay amount, modulated
	if(self->readHead > self->size - 1) {self->readHead-=self->size;} //prevent wraparound
	else if(self->readHead < 0) {self->readHead+=self->size;} //(in both directions)

	return output; //return the output
}

