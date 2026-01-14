#include <delay_modular.h>

void delay_modular_init(delay_modular *self, int32_t initSize, float *initStart)
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
	self->externalFlag = 0;
}
//establish new size
void delay_modular_setSize(delay_modular *self, int32_t newSize)
{
	self->size = newSize;
}
void delay_modular_setStart(delay_modular *self, float *newStart)
{
	self->start = newStart;
}
//USE THIS FUNCTION TO SET A NEW TARGET TIME
void delay_modular_setTime(delay_modular *self, int32_t newTime)
{
	self->time = newTime;
	if(self->time < 1.0f) //if less than 1 sample
		self->time = 1.0f; //stick to 1 sample
	if(self->time >= self->size) //if more than size - 1
		self->time = self->size - 1.0f;
}
//USE THIS FUNCTION TO SNAP THE READ HEAD TO THE NEW TARGET TIME
void delay_modular_snapTime(delay_modular *self)
{
	self->readHead = self->writeHead - (self->time - 1); //set the read head to the intended delay amount
		while(self->readHead < 0) {self->readHead+=self->size;} //wrap around if needed
}
void delay_modular_setDirection(delay_modular *self, float newDirection)
{
	if(self->direction != newDirection) //if new direction
		self->timePosition = 0.0f; //reset time position
	self->direction = newDirection; //set new direction
}
void delay_modular_setSmoothing(delay_modular *self, float newSmoothing)
{
	self->smoothing = newSmoothing;
}
void delay_modular_setMultiplier(delay_modular *self, float newMultiplier)
{
	self->multiplier = newMultiplier;
}
void delay_modular_setClockSpeed(delay_modular *self, float newClockSpeed)
{
	self->clockSpeed = newClockSpeed;
}
void delay_modular_reverseOnce(delay_modular *self, float distance)
{
	self->direction = -1.0f; //set direction to reverse
	self->oneTimeFlag = 1.0f; //flag this as the reason
	//self->readHead = self->writeHead; //and snap the readhead to the writehead
	self->distanceTracker = distance;
}
float delay_modular_inOutModulated(delay_modular *self, float input, float modulation)
{
	delay_modular_in(self, input);
	float output = delay_modular_out_modulated(self, modulation);
	delay_modular_tick3(self);
	return output;
}
float delay_modular_inOutModulatedLoFi(delay_modular *self, float input, float modulation)
{
	delay_modular_in(self, input);
	//delay_modular_in_lofi(self, input);
	float output = delay_modular_out_modulated(self, modulation);
	delay_modular_tick_lofi(self);
	//delay_modular_tick_lofi_experimental(self);
	return output;
}
float delay_modular_inOutModulatedLoFiVariable(delay_modular *self, float input, float modulation)
{
	//delay_modular_in(self, input);
	delay_modular_in_lofi(self, input);
	float output = delay_modular_out_modulated(self, modulation);
	//delay_modular_tick_lofi(self);
	delay_modular_tick_lofi_variable(self);
	return output;
}
//function for placing new input
void delay_modular_in(delay_modular *self, float input)
{
	self->start[self->writeHead] = input; //write new input
}
//function for placing new input (lofi version)
void delay_modular_in_lofi(delay_modular *self, float input)
{
	//get the next and previous write heads
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


	//self->start[self->writeHead] = input;
}
//function for placing new input (lofi version)
/*void delay_modular_in_lofi(delay_modular *self, float input)
{
	//get the previous write head
	int32_t previousWriteHead = self->writeHead - 1;
	if(previousWriteHead < 0)
	{
		previousWriteHead+=self->size;
	}

	if(self->newWriteHeadFlag == 1) //if the write head changed
	{
		self->start[previousWriteHead] = self->accumulatedInput / self->accAverager; //finalize the last input
		self->accumulatedInput = 0.0f; //reset the accumulator
		self->accAverager = 0.0f; //reset the averager/counter
		self->newWriteHeadFlag = 0; //and clear flag
	}

	//accumulate the new input
	self->accumulatedInput += input;//add input
	self->accAverager++;//mark another input (counting upward)

}*/
//function for getting standard output
float delay_modular_out(delay_modular *self)
{
	float output = 0.0f; //establish output variable

	float modulatedReadHead = self->readHead;
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

	if(self->direction == -1.0f) //if reverse
	{
		//get crossfade
		float distance = self->writeHead - self->readHead; //find the distance between the read and write heads
			if(distance < 0) {distance+=self->size;} //wrap around if negative, to represent actual distance
		float xfade = distance/(self->time*0.5f); //first, get distance as a 0-1 scale of half time
		xfade-=0.5f; //scale around zero
		xfade*=2.0f; //then make -1 to 1
		if(xfade < 0.0f)
			xfade=-xfade; //absolute value
		xfade = 1.0f-xfade; //invert, to create 0-1-0 triangle
		xfade*=10.0f; //scale way up
		if(xfade > 1.0f)
			xfade = 1.0f; //truncate, now a trapezoid
		output*=xfade;
	}

	return output; //return the output
}
//function for getting modulated output
/*float delay_modular_out_modulated(delay_modular *self, float modulation)
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

	return output * self->xfade; //return the output, crossfaded if needed
}*/
float delay_modular_out_modulated(delay_modular *self, float modulation)
{
	float output = 0.0f; //establish output variable

	float modulatedReadHead = self->readHead - modulation;
	if(modulatedReadHead >= self->size) {modulatedReadHead-=self->size;} //prevent wraparound
	if(modulatedReadHead < 0) {modulatedReadHead+=self->size;} //(in both directions)

	//////////testing
	modulatedReadHead = self->readHead;
	///////////
	int32_t timeIndexInt = (int32_t)modulatedReadHead; //get int value of current read head
	int32_t timeIndexIntPlus = timeIndexInt + 1; //and next value
	if(timeIndexIntPlus > self->size - 1) //and if it needs to wrap around
	{
		timeIndexIntPlus-=self->size; //do that
	}
	float timeIndexPartial = modulatedReadHead - (float)timeIndexInt; //subtract the int to get just the fractional bit
	//output = self->start[timeIndexIntMinus] + timeIndexPartial * (self->start[timeIndexInt]  - self->start[timeIndexIntMinus]);
	output = self->start[timeIndexInt] * (1.0f - timeIndexPartial) + self->start[timeIndexIntPlus] * timeIndexPartial;

	return output * self->xfade; //return the output, crossfaded if needed
}
//function to tick read and write heads forward
void delay_modular_tick(delay_modular *self)
{
	self->writeHead++; //move the write head forward
	if(self->writeHead > self->size - 1) //and wrap around if needed
	{
		self->writeHead-=self->size;
	}
	if(self->direction == 1.0f) //if forward-moving, do traditional head advancement
	{
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
		float g = difference * self->smoothing;// / self->size; //establish growth parameter by dividing the difference by the buffer size - for some reason, this smooths everything out, vs smaller dividers and other slowdown methods
		self->readHead+=(1+g); //update read index for next goaround
		if(self->readHead > self->size - 1) {self->readHead-=self->size;} //prevent wraparound
		if(self->readHead < 0) {self->readHead+=self->size;} //(in both directions)
	}
	else// if (self->direction == -1.0f) //if reverse
	{
		//self->readHead--; //decrement read head
		self->readHead+=(self->direction); //decrement or increment read head according to direction/speed
		if(self->readHead > self->size - 1) {self->readHead-=self->size;} //prevent wraparound
		if(self->readHead < 0) {self->readHead+=self->size;} //(in both directions)
		if(self->oneTimeFlag == 1.0f) //if this was a one-time reverse request
		{
			if(self->distanceTracker > 0.0f)
				self->distanceTracker--; //decrement distance tracker
			if(self->distanceTracker <= 0.0f) //if it hits zero
			{
				self->direction = 1.0f; //set direction back to forward
				self->oneTimeFlag = 0.0f; //and unflag
				delay_modular_snapTime(self); //then snap read head to time setting
			}
		}
		else //otherwise
		{
			float distance = self->writeHead - self->readHead; //find the distance between the read and write heads
				if(distance < 0) {distance+=self->size;} //wrap around if negative, to represent actual distance
			if(distance >= (self->time * 0.5f)) //if distance is more than half the current time setting (because half is when it wraps around if they're moving opposite each other)
			{
				self->readHead = self->writeHead; //snap back to writehead
			}
		}
	}
}
//function to tick read and write heads forward - experimental
void delay_modular_tick2(delay_modular *self)
{
	//filter the time setting
	self->timeFiltered += self->smoothing * (self->time - self->timeFiltered);
	//move the write head forward, and wrap around if needed
	self->writeHead++;
	if(self->writeHead > self->size - 1)
	{
		self->writeHead-=self->size;
	}
	//increment the time position counter
	self->timePosition+=self->direction;
	if(self->timePosition >= self->timeFiltered || self->timePosition <= -self->timeFiltered) //if past the time amount
		self->timePosition = 0.0f; //reset to zero
	//find out the intended delay amount, wrap around if needed
	self->delayTarget = self->writeHead - self->timeFiltered;
		if(self->delayTarget < 0) {self->delayTarget+=self->size;}
	//update read head based on delay target and time position
	self->readHead = self->delayTarget - self->timePosition; //update read index for next goaround
	while(self->readHead > self->size - 1) {self->readHead-=self->size;} //prevent wraparound
	while(self->readHead < 0) {self->readHead+=self->size;} //(in both directions)
	if(self->direction != 0.0f) //if not in standard delay mode
	{
		if((uint32_t)self->readHead == (uint32_t)self->writeHead - 1) //if read head bumps into the last write position
		{
			self->timePosition = 0.0f; //reset time position
		}
	}
}
//function to tick read and write heads forward - experimental
void delay_modular_tick3(delay_modular *self)
{
	//filter the time setting
	self->timeFiltered += self->smoothing * (self->time - self->timeFiltered);
	//move the write head forward, and wrap around if needed
	self->writeHead++;
	if(self->writeHead > self->size - 1)
	{
		self->writeHead-=self->size;
	}
	//decrement the time position counter
	self->timePosition+=self->direction;
	if(self->direction > 0.0f) //if direction is positive (varispeed reverse, in current scheme)
	{
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
	}
	else if(self->direction < 0.0f)//if direction is negative (varispeed forward, in current scheme)
	{
		if(self->timePosition <= -self->timeFiltered) //if past the time amount
			self->timePosition = 0.0f; //reset to zero
		//get crossfade
		self->xfade = -self->timePosition/(self->timeFiltered); //first, get distance as a 0-1 scale of time
		if(self->xfade > 1.0f || self->xfade < 0.0f) //if outside expected range
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
	}
	else //direction == 0.0f
	{
		self->xfade = 1.0f; //full volume
	}
	//find out the intended delay amount, wrap around if needed
	self->delayTarget = self->writeHead - self->timeFiltered;
		if(self->delayTarget < 0) {self->delayTarget+=self->size;}
	//update read head based on delay target and time position
	self->readHead = self->delayTarget - self->timePosition; //update read index for next goaround
	while(self->readHead > self->size - 1) {self->readHead-=self->size;} //prevent wraparound
	while(self->readHead < 0) {self->readHead+=self->size;} //(in both directions)
	if(self->direction != 0.0f) //if not in standard delay mode
	{
		if((uint32_t)self->readHead == (uint32_t)self->writeHead - 1) //if read head bumps into the last write position
		{
			self->timePosition = 0.0f; //reset time position
		}
	}
}
//function to tick read and write heads forward - experimental, lofi version
void delay_modular_tick_lofi(delay_modular *self)
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
	if(self->direction > 0.0f) //if direction is positive (varispeed reverse, in current scheme)
	{
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
	}
	else if(self->direction < 0.0f)//if direction is negative (varispeed forward, in current scheme)
	{
		if(self->timePosition <= -self->timeFiltered) //if past the time amount
			self->timePosition = 0.0f; //reset to zero
		//get crossfade
		self->xfade = -self->timePosition/(self->timeFiltered); //first, get distance as a 0-1 scale of time
		if(self->xfade > 1.0f || self->xfade < 0.0f) //if outside expected range
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
	}
	else //direction == 0.0f
	{
		self->xfade = 1.0f; //full volume
	}
	//find out the intended delay amount, wrap around if needed
	self->delayTarget = self->writeHead - self->timeFiltered;
		if(self->delayTarget < 0) {self->delayTarget+=self->size;}
	//update read head based on delay target and time position
	self->readHead = self->delayTarget - self->timePosition; //update read index for next goaround
	while(self->readHead > self->size - 1) {self->readHead-=self->size;} //prevent wraparound
	while(self->readHead < 0) {self->readHead+=self->size;} //(in both directions)
	/*if(self->direction != 0.0f) //if not in standard delay mode
	{
		if((uint32_t)self->readHead == (uint32_t)self->writeHead - 1) //if read head bumps into the last write position
		{
			self->timePosition = 0.0f; //reset time position
		}
	}*/
}
//function to tick read and write heads forward - experimental, lofi version, with variable speed beyond half/double
void delay_modular_tick_lofi_experimental(delay_modular *self)
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
	int32_t tempWriteHead = self->writeHeadF;
	self->writeHeadPartial = 2.0f * ((self->writeHeadF - tempWriteHead) - 0.5f); //get the current partial, scaled from -1 to 1 around a given int
	if(self->writeHeadPartial < 0.0f)
		self->writeHeadPartial = -self->writeHeadPartial; //absolute value
	tempWriteHead = self->writeHead; //mark the most recent write head int
	self->writeHead = self->writeHeadF + 0.5f; //set the new write head based on the closest location, by adding 0.5 before truncating
	if(self->writeHead >= self->size) //and kick back to zero if it's off the edge
		self->writeHead-=self->size;
	if(self->writeHead != tempWriteHead) //if the write head changed
		self->newWriteHeadFlag = 1; //flag it

	//decrement the time position counter
	self->timePosition+=self->direction*self->clockSpeed;
	if(self->direction > 0.0f) //if direction is positive (varispeed reverse, in current scheme)
	{
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
	}
	else if(self->direction < 0.0f)//if direction is negative (varispeed forward, in current scheme)
	{
		if(self->timePosition <= -self->timeFiltered) //if past the time amount
			self->timePosition = 0.0f; //reset to zero
		//get crossfade
		self->xfade = -self->timePosition/(self->timeFiltered); //first, get distance as a 0-1 scale of time
		if(self->xfade > 1.0f || self->xfade < 0.0f) //if outside expected range
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
	}
	else //direction == 0.0f
	{
		self->xfade = 1.0f; //full volume
	}
	//find out the intended delay amount, wrap around if needed
	self->delayTarget = self->writeHeadF - self->timeFiltered;
		if(self->delayTarget < 0) {self->delayTarget+=self->size;}
	//update read head based on delay target and time position
	self->readHead = self->delayTarget - self->timePosition; //update read index for next goaround
	while(self->readHead > self->size - 1) {self->readHead-=self->size;} //prevent wraparound
	while(self->readHead < 0) {self->readHead+=self->size;} //(in both directions)
	if(self->direction != 0.0f) //if not in standard delay mode
	{
		if((uint32_t)self->readHead == (uint32_t)self->writeHead - 1) //if read head bumps into the last write position
		{
			self->timePosition = 0.0f; //reset time position
		}
	}
}
//function to tick read and write heads forward - lofi, with linearly variable clock - based on experimental version above
void delay_modular_tick_lofi_variable(delay_modular *self)
{
	//filter the time setting
	self->timeFiltered += self->smoothing * (self->time - self->timeFiltered);
	//move the write head forward, and wrap around if needed
	self->writeHeadF+=self->clockSpeed;
	if(self->writeHeadF >= self->size)// - 1)
	{
		self->writeHeadF-=self->size;
		self->externalFlag = 1;
	}
	int32_t tempWriteHead = self->writeHeadF;
	self->writeHeadPartial = 2.0f * ((self->writeHeadF - tempWriteHead) - 0.5f); //get the current partial, scaled from -1 to 1 around a given int
	if(self->writeHeadPartial < 0.0f)
		self->writeHeadPartial = -self->writeHeadPartial; //absolute value
	tempWriteHead = self->writeHead; //mark the most recent write head int
	self->writeHead = self->writeHeadF + 0.5f; //set the new write head based on the closest location, by adding 0.5 before truncating
	if(self->writeHead >= self->size) //and kick back to zero if it's off the edge
		self->writeHead-=self->size;
	if(self->writeHead != tempWriteHead) //if the write head changed
		self->newWriteHeadFlag = 1; //flag it


	//decrement the time position counter
	self->timePosition+=self->direction*self->clockSpeed;
	if(self->direction > 0.0f) //if direction is positive (varispeed reverse, in current scheme)
	{
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
	}
	else if(self->direction < 0.0f)//if direction is negative (varispeed forward, in current scheme)
	{
		if(self->timePosition <= -self->timeFiltered) //if past the time amount
			self->timePosition = 0.0f; //reset to zero
		//get crossfade
		self->xfade = -self->timePosition/(self->timeFiltered); //first, get distance as a 0-1 scale of time
		if(self->xfade > 1.0f || self->xfade < 0.0f) //if outside expected range
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
	}
	else //direction == 0.0f
	{
		self->xfade = 1.0f; //full volume
		self->timePosition = 0.0f; //no time position effect
	}
	//find out the intended delay amount, wrap around if needed
	self->delayTarget = self->writeHeadF - self->timeFiltered;
		if(self->delayTarget < 0.0f) {self->delayTarget+=self->size;}
	//update read head based on delay target and time position
	self->readHead = self->delayTarget - self->timePosition; //update read index for next goaround
	//self->readHead = self->writeHeadF - 10000.0f;// - self->timeFiltered;
	while(self->readHead >= self->size) {self->readHead-=self->size;} //prevent wraparound
	while(self->readHead < 0.0f) {self->readHead+=self->size;} //(in both directions)
}

