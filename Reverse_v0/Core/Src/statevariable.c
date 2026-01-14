/*NOTE: THis relies heavy on the book found here:
 * http://sites.music.columbia.edu/cmc/courses/g6610/fall2016/week8/Musical_Applications_of_Microprocessors-Charmberlin.pdf
 * Musical Applications of Microprocessors by Chamberlin
 * Some notes:
 * 	Q is defined as 1/q, with the useful range being 2 to 0
 * 	(although, isn't that undefined? but it says this can be used to create sin waves and the like, infinite resonance)
 * From P. 492:
 * 	F1=2sin(1TF/Fs), where F is the
	center frequency and F1 is the exact value of the frequency parameter for that
	center frequency
	The book gives example code in BASIC, as follows:
	1000 LET L=D2+Fl*D1
	1001 LET H=I-L-Q1*D1
	1002 LET B=F1*H+D1
	1003 LET N=H+L
	1004 LET Dl=B
	1005 LET D2 =L
 */

#include "statevariable.h"
#include <math.h>

void statevariable_init(statevariable *self, float samplefrequency, float cutoff)
{
	self->input = 0.0;
	self->hp = 0.0;
	self->bp = 0.0;
	self->lastbp = 0.0;
	self->lp = 0.0;
	self->lastlp = 0.0;
	self->br = 0.0;
	self->fs = samplefrequency;
	self->hz = cutoff;
	self->fc = 2 * sin((M_PI * self->hz) / self->fs);
	self->q = 1.8f;
	self->overclock = 1.0f;
}
void statevariable_input(statevariable *self, float newinput)
{
	/*do all filter processing
	1000 LET L=D2+Fl*D1
	1001 LET H=I-L-Q1*D1
	1002 LET B=F1*H+D1
	1003 LET N=H+L
	1004 LET Dl=B
	1005 LET D2 =L*/
	for(float n = 0.0f; n < self->overclock; n++) //run the input as many times as overclock says to - once if not overclocking, twice if 2xclock, etc
	{
		self->lp = self->lastlp + (self->fc * self->lastbp);
		self->hp = newinput - self->lp - (self->q * self->lastbp);
		self->bp = (self->fc * self->hp) + self->lastbp;
		self->br = self->hp + self->lp;
		self->lastbp=self->bp;
		self->lastlp=self->lp;
	}
}
float statevariable_getHP(statevariable *self)
{
	return self->hp;
}
float statevariable_getBP(statevariable *self)
{
	return self->bp;
}
float statevariable_getLP(statevariable *self)
{
	return self->lp;
}
float statevariable_getBR(statevariable *self)
{
	return self->br;
}
void statevariable_setFC(statevariable *self, float frequency)
{
	self->hz = frequency;
	self->fc = 2 * sin((M_PI * self->hz) / (self->fs * self->overclock));
}
void statevariable_setQ(statevariable *self, float resonance)
{
	self->q = resonance;
}
void statevariable_setOverclock(statevariable *self, float overclockAmount)
{
	self->overclock = overclockAmount; //set overclock amount
	statevariable_setFC(self, self->hz); //and set new fc
}


