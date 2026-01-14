#include <switchDB.h>

void initSwitch(switchDB *self, uint16_t cutoffAmount)
{
	self->new = self->old = self->value = 1; //start switches at 1 (since switches generally will be pull-up, default 1)
	self->switchRegistered=self->holdFlag=self->longHoldFlag=0; //start flags at zero
	self->cutoff = cutoffAmount; //set cutoff
	self->holdCutoff = self->longHoldCutoff = 60000;
	self->counter = self->longHoldCutoff + 1; //and start counter above greatest flag point
}
void initSwitchMomentary(switchDB *self, uint16_t cutoffAmount, uint16_t holdCutoffAmount, uint16_t longHoldCutoffAmount)
{
	self->new = self->old = self->value = 1; //start switches at 1 (since switches generally will be pull-up, default 1)
	self->switchRegistered=self->holdFlag=self->longHoldFlag=0; //start flags at zero
	self->cutoff = cutoffAmount; //assign all cutoff points
	self->holdCutoff = holdCutoffAmount;
	self->longHoldCutoff = longHoldCutoffAmount;
	self->counter = self->longHoldCutoff + 1; //and start counter above greatest flag point
}
void debounce(switchDB *self, uint8_t reading)
{
	self->new = reading;
	if(self->new != self->old) //if it's different than before
	{
		self->counter = 0; //reset counter
	}
	else if(self->counter < 60000) //if the counter won't wrap around
	{
		self->counter++; //increment
	}
	if(self->counter == self->cutoff) //if we hit the debounce cutoff
	{
		self->value = self->new; //mark the new value
		self->switchRegistered = 1;//and flag a switch
	}
	if(self->counter == self->holdCutoff) //if we hit the hold cutoff
	{
		if(self->value == 0) //and switch is grounded (this is new for Bathing, experimental)
			self->holdFlag = 1;//flag it
	}
	if(self->counter == self->longHoldCutoff) //if we hit the long hold cutoff
	{
		if(self->value == 0) //and switch is grounded (this is new for Bathing, experimental)
			self->longHoldFlag = 1;//flag it
	}
	self->old=self->new; //after everything, store the new value to check against the next iteration
}
void clearHoldFlag(switchDB *self)
{
	self->holdFlag = 0;
}
void clearLongHoldFlag(switchDB *self)
{
	self->longHoldFlag = 0;
}
void clearAllFlags(switchDB *self)
{
	self->longHoldFlag = 0;
	self->holdFlag = 0;
	self->switchRegistered = 0;
}
