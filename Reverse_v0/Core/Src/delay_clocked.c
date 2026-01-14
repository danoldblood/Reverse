#include <delay_clocked.h>

void delay_clocked_init(delay_clocked *self, int32_t initSize, float *initStart)
{
	self->size = initSize;
	self->sizeF = (double)initSize;
	self->start = initStart;
	self->time = initSize-1;
	self->timePosition = 0.0f; //use this to track current time position (outside read head, or time setting)
	self->readHead = self->sizeF - 10000.0;
	self->writeHead = 0;
	self->writeHeadF = 0.0;
	self->direction = 0.0; //positive for forward, negative for reverse, multiply for different speeds
	self->smoothing = 0.0001; //slew to smooth the time changes
	self->clockSpeed = 1.0;
	self->timeFiltered = 0.0;
	self->xfade = 0.0;
	self->lastSample = 0.0f;
	self->accumulatedInput = 0.0f;
	self->accAverager = 1.0f;
	self->writeHeadPartial = 0.0;
	self->newWriteHeadFlag = 0;
	self->lockInSampleFlag = 0;
	self->externalFlag1 = 0;
	self->externalFlag2 = 0;
	self->externalFlag3 = 0;
	self->externalFlag4 = 0;
}
//establish new size
void delay_clocked_setSize(delay_clocked *self, int32_t newSize)
{
	self->size = newSize;
}
void delay_clocked_setStart(delay_clocked *self, float *newStart)
{
	self->start = newStart;
}
//USE THIS FUNCTION TO SET A NEW TARGET TIME
void delay_clocked_setTime(delay_clocked *self, int32_t newTime)
{
	self->time = newTime;
	if(self->time < 1.0) //if less than 1 sample
		self->time = 1.0; //stick to 1 sample
	if(self->time >= self->sizeF) //if more than size - 1
		self->time = self->sizeF - 1.0;
}
//USE THIS FUNCTION TO SNAP THE READ HEAD TO THE NEW TARGET TIME
void delay_clocked_snapTime(delay_clocked *self)
{
	self->readHead = self->writeHead - (self->time - 1); //set the read head to the intended delay amount
		while(self->readHead < 0) {self->readHead+=self->size;} //wrap around if needed
}
void delay_clocked_setDirection(delay_clocked *self, double newDirection)
{
	//if(self->direction != newDirection) //if new direction
		//self->timePosition = 0.0; //reset time position
	self->direction = newDirection; //set new direction
}
void delay_clocked_setSmoothing(delay_clocked *self, double newSmoothing)
{
	self->smoothing = newSmoothing;
}
void delay_clocked_setClockSpeed(delay_clocked *self, double newClockSpeed)
{
	self->clockSpeed = newClockSpeed;
}
float delay_clocked_inOutModulatedLoFiVariable(delay_clocked *self, float input, float modulation)
{
	//delay_clocked_in(self, input);
	delay_clocked_tick_lofi_variable(self);
	delay_clocked_in_lofi(self, input);
	float output = delay_clocked_out_modulated(self, modulation);
	return output;
}
//function for placing new input
void delay_clocked_in(delay_clocked *self, float input)
{
	self->start[self->writeHead] = input; //write new input
}
//function for placing new input (lofi version, averaging based on nearest two samples)
/*void delay_clocked_in_lofi(delay_clocked *self, float input)
{
	//get the previous write head
	int32_t previousWriteHead = self->writeHead - 1;
	if(previousWriteHead < 0)
	{
		previousWriteHead+=self->size;
	}

	if(self->newWriteHeadFlag == 1) //if the write head changed
	{
		self->start[self->writeHead] = (self->lastSample + input * self->writeHeadPartial) / 2.0f; //interpolate last sample and this one based on distance from the write head
		self->newWriteHeadFlag = 0; //clear flag
	}
	//mark this current input in the last sample area
	self->lastSample = input * self->writeHeadPartial;
}*/
//function for placing new input (lofi version, averaging based on all samples within the half-sample window on either side)
/*void delay_clocked_in_lofi(delay_clocked *self, float input)
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
		self->accAverager = 0.0f; //reset the averager
		self->newWriteHeadFlag = 0; //and clear flag
	}

	//accumulate the new input
	self->accumulatedInput += input * self->writeHeadPartial; //add input, scaled by distance
	self->accAverager += self->writeHeadPartial; //and add that scaling to averager
}*/
//newest version, interpolating whenever passing new underclocked sample point
void delay_clocked_in_lofi(delay_clocked *self, float input)
{
	if(self->newWriteHeadFlag == 1) //if the write head changed
	{
		double sampleDistance = self->writeHeadPartial / self->clockSpeed; //get the distance expressed as partial divided by clock, up to ~1 away from sample
		self->start[self->writeHead] = input * (1.0 - sampleDistance) + self->lastSample * sampleDistance; //interpolate based on the distance to each sample
		self->newWriteHeadFlag = 0; //and clear flag
	}

	//accumulate the new input
	self->lastSample = input;
}
//function for getting standard output
float delay_clocked_out(delay_clocked *self)
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
float delay_clocked_out_modulated(delay_clocked *self, float modulation)
{
	float output = 0.0f; //establish output variable

	float modulatedReadHead = self->readHead - modulation;
	if(modulatedReadHead >= self->sizeF) {modulatedReadHead-=self->sizeF;} //prevent wraparound
	else if(modulatedReadHead < 0.0) {modulatedReadHead+=self->sizeF;} //(in both directions)

	int32_t timeIndexInt = (int32_t)modulatedReadHead; //get int value of current read head
	int32_t timeIndexIntPlus = timeIndexInt + 1; //and next value
	if(timeIndexIntPlus > self->size - 1) //and if it needs to wrap around
	{
		timeIndexIntPlus-=self->size; //do that
	}
	double timeIndexPartial = modulatedReadHead - (float)timeIndexInt; //subtract the int to get just the fractional bit
	//output = self->start[timeIndexIntMinus] + timeIndexPartial * (self->start[timeIndexInt]  - self->start[timeIndexIntMinus]);
	output = self->start[timeIndexInt] * (1.0 - timeIndexPartial) + self->start[timeIndexIntPlus] * timeIndexPartial;

	return output * self->xfade; //return the output, crossfaded if needed
}
//original version, works with the all-samples-in-a-half-sample-window averaging
/*void delay_clocked_tick_lofi_variable(delay_clocked *self)
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
}*/
//newest version, works with the two-sample interpolation
void delay_clocked_tick_lofi_variable(delay_clocked *self)
{
	//first, get the updated filtered time goal
	self->timeFiltered += self->smoothing * (self->time - self->timeFiltered);
	//then, do the time position stuff
	self->timePosition+=self->direction*self->clockSpeed; 	//increment the time position counter
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
		self->xfade = 1.0; //full volume
		self->timePosition = 0.0; //no time position effect
	}


	//now, do write head
	//move the write head forward
	self->writeHeadF+=self->clockSpeed;
	//wrap around if needed
	if(self->writeHeadF >= self->sizeF)
	{
		self->writeHeadF-=self->sizeF;
		self->externalFlag4 = 1;
	}

	//find out the intended delay amount, wrap around if needed
	self->delayTarget = self->writeHeadF - self->timeFiltered;
		if(self->delayTarget < 0.0) {self->delayTarget+=self->sizeF;}
	//update read head based on delay target and time position
	self->readHead = self->delayTarget - self->timePosition; //update read index for next goaround
	//self->readHead = self->writeHeadF - self->timeFiltered;

	while(self->readHead >= self->sizeF) {self->readHead-=self->sizeF;} //prevent wraparound
	while(self->readHead < 0.0) {self->readHead+=self->sizeF;} //(in both directions)

	int32_t tempWriteHead = self->writeHeadF;
	self->writeHeadPartial = self->writeHeadF - (float)tempWriteHead;//get the current partial
	tempWriteHead = self->writeHead; //mark the most recent write head int
	self->writeHead = self->writeHeadF;// + 0.5f; //set the new write head based on the closest location, by adding 0.5 before truncating
	if(self->writeHead != tempWriteHead) //if the write head changed
		self->newWriteHeadFlag = 1; //flag it
}
