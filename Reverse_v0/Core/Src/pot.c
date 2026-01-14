#include <pot.h>

void initPot(pot *self, uint16_t cutoffAmount)
{
	self->new=self->old=0; //start everything at zero
	self->cutoff = cutoffAmount; //except cutoff, which is user-assignable
	self->pauseReading = 1; //begin with pot paused
}
void updatePot(pot *self, uint16_t reading)
{
	self->new = reading;
	if(self->queuePause == 1) //if pause was queued
	{
		self->pauseReading = 1; //make it happen
		self->queuePause = 0; //and clear queue flag
	}
	if(self->pauseReading == 0) //if reading isn't paused
		self->old = self->new; //update the value
	else
	{
		int16_t diff = self->new - self->old; //get the value difference
		if(diff > self->cutoff || diff < -self->cutoff) //if it's moved significantly
		{
			self->pauseReading = 0; //unpause reading
			self->old = self->new; //and update the value
		}
	}
}
