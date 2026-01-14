#include <delay_parting_16bit.h>

void delay_parting_16bit_init(delay_parting_16bit *self, int32_t initSize, int16_t *initStart)
{
	self->size = initSize;
	self->start = initStart;
	self->time = initSize-1;
	self->timePosition = 0.0f; //use this to track current time position (outside read head, or time setting)
	self->readHead = 1.0f;
	self->writeHead = 0;
	self->writeHeadF = 0.0f;
	self->direction = 0.0f; //positive for forward, negative for reverse, multiply for different speeds
	self->smoothing = 0.0001f; //slew to smooth the time changes
	self->multiplier = 1.0f;
	self->clockSpeed = 1.0f;
	self->oneTimeFlag = 0.0f; //flag for doing momentary changes of direction/speed
	self->distanceTracker = 0.0f;
	self->timeFiltered = 0.0f;
	self->xfade = 0.0f;
	self->lastInput = 0.0f;
	self->accumulatedInput = 0.0f;
	self->accAverager = 1.0f;
	self->writeHeadPartial = 0.0f;
	self->newWriteHeadFlag = 0;
	self->volume = 1.0f; //use this to eliminate time change artifacts/
	self->volumeDirection = 0.0f;
}
//establish new size
void delay_parting_16bit_setSize(delay_parting_16bit *self, int32_t newSize)
{
	self->size = newSize;
}
void delay_parting_16bit_setStart(delay_parting_16bit *self, int16_t *newStart)
{
	self->start = newStart;
}
//USE THIS FUNCTION TO SET A NEW TARGET TIME
void delay_parting_16bit_setTime(delay_parting_16bit *self, int32_t newTime)
{
	self->time = newTime;
	if(self->time < 1.0f) //if less than 1 sample
		self->time = 1.0f; //stick to 1 sample
	if(self->time >= self->size) //if more than size - 1
		self->time = self->size - 1.0f;
}
//USE THIS FUNCTION TO SNAP THE READ HEAD TO THE NEW TARGET TIME
void delay_parting_16bit_snapTime(delay_parting_16bit *self)
{
	self->readHead = self->writeHead - (self->time - 1); //set the read head to the intended delay amount
		while(self->readHead < 0) {self->readHead+=self->size;} //wrap around if needed
}
void delay_parting_16bit_setDirection(delay_parting_16bit *self, float newDirection)
{
	if(self->direction != newDirection) //if new direction
		self->timePosition = 0.0f; //reset time position
	self->direction = newDirection; //set new direction
}
void delay_parting_16bit_setSmoothing(delay_parting_16bit *self, float newSmoothing)
{
	self->smoothing = newSmoothing;
}
void delay_parting_16bit_setMultiplier(delay_parting_16bit *self, float newMultiplier)
{
	self->multiplier = newMultiplier;
}
void delay_parting_16bit_setClockSpeed(delay_parting_16bit *self, float newClockSpeed)
{
	self->clockSpeed = newClockSpeed;
}
void delay_parting_16bit_reverseOnce(delay_parting_16bit *self, float distance)
{
	self->direction = -1.0f; //set direction to reverse
	self->oneTimeFlag = 1.0f; //flag this as the reason
	//self->readHead = self->writeHead; //and snap the readhead to the writehead
	self->distanceTracker = distance;
}
int16_t delay_parting_16bit_inOutModulatedForward(delay_parting_16bit *self, int16_t input, float modulation)
{
	delay_parting_16bit_in(self, input);
	float output = delay_parting_16bit_out_modulated_forward(self, modulation);
	delay_parting_16bit_tick_forward(self);
	return output;
}
int16_t delay_parting_16bit_inOutModulatedReverse(delay_parting_16bit *self, int16_t input, float modulation)
{
	delay_parting_16bit_in(self, input);
	int16_t output = delay_parting_16bit_out_modulated_reverse(self, modulation);
	delay_parting_16bit_tick_reverse(self);
	return output;
}

//function for placing new input
void delay_parting_16bit_in(delay_parting_16bit *self, int16_t input)
{
	self->start[self->writeHead] = input; //write new input
}
//function for placing new input (lofi version)
void delay_parting_16bit_in_lofi(delay_parting_16bit *self, int16_t input)
{
	//get the previous write heads
	int32_t previousWriteHead = self->writeHead - 1;
	if(previousWriteHead < 0)
	{
		previousWriteHead+=self->size;
	}

	if(self->newWriteHeadFlag == 1) //if the write head changed
	{
		self->start[previousWriteHead] = self->accumulatedInput / self->accAverager; //finalize the last input
		self->accumulatedInput = 0.0f; //reset the accumulator
		self->accAverager = 0.0f; //reset the averager
		self->newWriteHeadFlag = 0; //and clear flag
	}

	//accumulate the new input
	self->accumulatedInput += input * self->writeHeadPartial; //add input, scaled by distance
	self->accAverager += self->writeHeadPartial; //and add that scaling to averager

	self->lastInput = input;

}
//original version, pitch shift when time changes
void delay_parting_16bit_tick_forward(delay_parting_16bit *self)
{
	//filter the time setting
	self->timeFiltered += self->smoothing * (self->time - self->timeFiltered);
	//move the write head forward, and wrap around if needed
	self->writeHeadF+=self->clockSpeed;
	if(self->writeHeadF >= self->size)// - 1)
	{
		self->writeHeadF-=self->size;
		//self->writeHeadF=0.0f; //for now, hardcode to zero, see if it helps with noise (since our speeds should always hit whole numbers anyway)
	}
	self->writeHead = self->writeHeadF; //set the new write head based on the closest location, by adding 0.5 before truncating

	//find out the intended delay amount, wrap around if needed
	self->delayTarget = self->writeHead - self->timeFiltered;
		if(self->delayTarget < 0) {self->delayTarget+=self->size;}
	//update read head based on delay target and time position
	self->readHead = self->delayTarget - self->timePosition; //update read index for next goaround
	while(self->readHead > self->size - 1) {self->readHead-=self->size;} //prevent wraparound
	while(self->readHead < 0) {self->readHead+=self->size;} //(in both directions)
	self->volume = 1.0f; //hard code the volume, brute force, make it work quickly!
}
//original version, pitch shift when time changes
void delay_parting_16bit_tick_reverse(delay_parting_16bit *self)
{
	//filter the time setting
	self->timeFiltered += self->smoothing * (self->time - self->timeFiltered);
	//move the write head forward, and wrap around if needed
	self->writeHeadF+=self->clockSpeed;
	if(self->writeHeadF >= self->size)// - 1)
	{
		self->writeHeadF-=self->size;
		//self->writeHeadF=0.0f; //for now, hardcode to zero, see if it helps with noise (since our speeds should always hit whole numbers anyway)
	}
	self->writeHead = self->writeHeadF; //set the new write head based on the closest location, by adding 0.5 before truncating

	//decrement the time position counter
	self->timePosition+=self->direction*self->clockSpeed;
	//if(self->direction > 0.0f) //if direction is positive (varispeed reverse, in current scheme)
	//{
		if(self->timePosition >= -(self->timeFiltered * 0.5f)) //if it hits half the delay time (where it could bump into the write head)
			self->timePosition = -self->timeFiltered; //reset to negative delay time, which will put it at the write head
		//get crossfade
		self->xfade = -((self->timePosition + self->timeFiltered * 0.5f)/(self->timeFiltered*0.5f)); //first, get distance as a 0-1 scale of half time
		if(self->xfade > 1.0f || self->xfade < 0.0f) //if otuside expected range
			self->xfade = 0.0f; //zero out
		else //if in expected range
		{
			self->xfade-=0.5f; //scale around zero
			self->xfade*=2.0f; //then make -1 to 1
			if(self->xfade < 0.0f)
				self->xfade=-self->xfade; //absolute value
			self->xfade = 1.0f-self->xfade; //invert, to create 0-1-0 triangle
			self->xfade*=10.0f; //scale way up
			if(self->xfade > 1.0f)
				self->xfade = 1.0f; //truncate, now a trapezoid
		}
	//}
	//find out the intended delay amount, wrap around if needed
	self->delayTarget = self->writeHead - self->timeFiltered;
		if(self->delayTarget < 0) {self->delayTarget+=self->size;}
	//update read head based on delay target and time position
	self->readHead = self->delayTarget - self->timePosition; //update read index for next goaround
	while(self->readHead > self->size - 1) {self->readHead-=self->size;} //prevent wraparound
	while(self->readHead < 0) {self->readHead+=self->size;} //(in both directions)
}
//function for getting modulated output
int16_t delay_parting_16bit_out_modulated_forward(delay_parting_16bit *self, float modulation)
{
	float output = 0.0f; //establish output variable

	float modulatedReadHead = self->readHead - modulation;
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

	return output * self->volume; //return the output, crossfaded if needed
}
//function for getting modulated output
int16_t delay_parting_16bit_out_modulated_reverse(delay_parting_16bit *self, float modulation)
{
	float output = 0.0f; //establish output variable

	float modulatedReadHead = self->readHead - modulation;
	if(modulatedReadHead > self->size - 1) {modulatedReadHead-=self->size;} //prevent wraparound
	if(modulatedReadHead < 0) {modulatedReadHead+=self->size;} //(in both directions)
	int32_t timeIndexInt = (int32_t)modulatedReadHead; //get int value of current read head
	int32_t timeIndexIntMinus = timeIndexInt - 1; //and one value back
	if(timeIndexIntMinus < 0) //and if it needs to wrap around
	{
		timeIndexIntMinus+=self->size; //do that
	}
	float timeIndexPartial = modulatedReadHead - (float)timeIndexInt; //subtract the int to get just the fractional bit
	output = (float)self->start[timeIndexIntMinus] + timeIndexPartial * ((float)self->start[timeIndexInt]  - (float)self->start[timeIndexIntMinus]);

	output = output * self->xfade * self->volume;
	return (int16_t)output;// * self->xfade * self->volume; //return the output, crossfaded if needed
}

