#include "pitch.h"

void pitchFV1_init(pitchFV1 *self, int16_t initSize, float *initStart)
{
	self->size = initSize;
	self->start = initStart;
	self->shift = 0;
	self->writeHead = initSize;
	self->readHead1 = 1;
	self->readHead2 = 1 + initSize/2;
	self->rampPos = 0;
}
//USE THIS FUNCTION TO SET THE SHIFT DIRECTLY AS A DECIMAL
void pitchFV1_setShiftDecimal(pitchFV1 *self, float shiftDecimal)
{
	self->shift = shiftDecimal;
}
//USE THIS FUNCTION TO SET THE SHIFT IN TERMS OF SEMITONES
void pitchFV1_setShiftSemitone(pitchFV1 *self, float shiftSemitone)
{
	if(shiftSemitone == -24.0f)
		self->shift = -0.75f;
	else if(shiftSemitone == -12.0f)
		self->shift = -0.5f;
	else if(shiftSemitone == -11.0f)
		self->shift = -0.4702684528f;
	else if(shiftSemitone == -10.0f)
		self->shift = -0.4387689758f;
	else if(shiftSemitone == -9.0f)
		self->shift = -0.4053964425f;
	else if(shiftSemitone == -8.0f)
		self->shift = -0.3700394751f;
	else if(shiftSemitone == -7.0f)
		self->shift = -0.3325800729;
	else if(shiftSemitone == -6.0f)
		self->shift = -0.2928932188f;
	else if(shiftSemitone == -5.0f)
		self->shift = -0.2508464616f;
	else if(shiftSemitone == -4.0f)
		self->shift = -0.206299474f;
	else if(shiftSemitone == -3.0f)
		self->shift = -0.1591035847f;
	else if(shiftSemitone == -2.0f)
		self->shift = -0.1091012819f;
	else if(shiftSemitone == -1.0f)
		self->shift = -0.05612568732f;
	else if(shiftSemitone == 1.0f)
		self->shift = 0.05946309436f;
	else if(shiftSemitone == 2.0f)
		self->shift = 0.1224620483f;
	else if(shiftSemitone == 3.0f)
		self->shift = 0.189207115f;
	else if(shiftSemitone == 3.0f)
		self->shift = 0.2599210499f;
	else if(shiftSemitone == 5.0f)
		self->shift = 0.3348398542f;
	else if(shiftSemitone == 6.0f)
		self->shift = 0.4142135624f;
	else if(shiftSemitone == 7.0f)
		self->shift = 0.4983070769f;
	else if(shiftSemitone == 8.0f)
		self->shift = 0.587401052f;
	else if(shiftSemitone == 9.0f)
		self->shift = 0.6817928305f;
	else if(shiftSemitone == 10.0f)
		self->shift = 0.7817974363f;
	else if(shiftSemitone == 11.0f)
		self->shift = 0.8877486254f;
	else if(shiftSemitone == 12.0f)
		self->shift = 1.0f;
	else if(shiftSemitone == 24.0f)
		self->shift = 3.0f;
}
float pitchFV1_inOut(pitchFV1 *self, float input)
{
	//NEW INPUT
	self->start[self->writeHead] = input; //write new input

	//advance read head 1
	self->readHead1 = self->writeHead + self->rampPos; //update read index for next goaround
	if(self->readHead1 >= self->size) {self->readHead1-=self->size;} //prevent wraparound
	if(self->readHead1 < 0.0f) {self->readHead1+=self->size;} //(in both directions)
	//advance read head 2
	self->readHead2 = self->readHead1 + self->size/2.0f; //update read index for next goaround
	if(self->readHead2 >= self->size) {self->readHead2-=self->size;} //prevent wraparound
	if(self->readHead2 < 0.0f) {self->readHead2+=self->size;} //(in both directions)

	//NEW OUTPUT PT1
	float output1 = 0.0f; //establish first half output variable
	int16_t timeIndexInt = (int16_t)self->readHead1; //get int value of current read head
	int16_t timeIndexIntMinus = timeIndexInt - 1; //and one value back
	if(timeIndexIntMinus < 0) //and if it needs to wrap around
	{
		timeIndexIntMinus+=(self->size); //do that
	}
	float timeIndexPartial = self->readHead1 - (float)timeIndexInt; //subtract the int to get just the fractional bit
	output1 = self->start[timeIndexIntMinus] + timeIndexPartial * (self->start[timeIndexInt]  - self->start[timeIndexIntMinus]); //interpolate the delay line

	//NEW OUTPUT PT2
	float output2 = 0.0f; //establish second half output variable
	timeIndexInt = (int16_t)self->readHead2; //get int value of current read head
	timeIndexIntMinus = timeIndexInt - 1; //and one value back
	if(timeIndexIntMinus < 0) //and if it needs to wrap around
	{
		timeIndexIntMinus+=(self->size); //do that
	}
	timeIndexPartial = self->readHead2 - (float)timeIndexInt; //subtract the int to get just the fractional bit
	output2 = self->start[timeIndexIntMinus] + timeIndexPartial * (self->start[timeIndexInt]  - self->start[timeIndexIntMinus]); //interpolate the delay line


	//ESTABLISH CROSSFADE BASED ON CURRENT READ AND WRITE VALUES
	float xfade = (self->readHead1 - self->writeHead) / (self->size / 2.0f);
	if(xfade < 0.0f) xfade = -xfade;
	if(xfade > 1.0f) xfade = 2.0f-xfade; //from here, we have 0-1-0 in a triangle, but we'll scale below to spend more time at 0 and 1
	xfade*=2.0f;
	xfade-=0.5f;
	if(xfade < 0.0f) xfade=0.0f;
	else if(xfade > 1.0f) xfade=1.0f;
	/*float xfade = self->rampPos / self->size;
	xfade*=2.0f;
	if(xfade > 1.0f)
		xfade = 2.0f - xfade;
	xfade*=2.0f;
	xfade-=0.5f;
	if(xfade < 0.0f) xfade=0.0f;
	else if(xfade > 1.0f) xfade=1.0f;*/

	//ADVANCE WRITE HEAD FOR NEXT PASS
	self->writeHead--; //move the write head forward
	if(self->writeHead < 0) //and wrap around if needed
	{
		self->writeHead+=self->size;
	}
	//ADVANCE RAMP POSITION FOR NEXT PASS
	self->rampPos-=self->shift; //add shift amount to ramp
	if(self->rampPos>(self->size)) self->rampPos-=(self->size); //if over delay size, subtract size
	if(self->rampPos<0.0f) self->rampPos+=(self->size); //if under 0, add size

	//FINAL OUTPUT
	float output = output1 * (xfade) + output2 * (1.0f-xfade);
	//output = output1 * xfade;
	//output = output2 * (1.0-xfade);
	return output; //return the output
}
