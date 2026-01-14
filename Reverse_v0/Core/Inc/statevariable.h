//a structure for building state variable filters
//creating high pass, low pass, band pass, and band reject
//with controllable cutoff frequency and resonance
//initializing info is passed here
//it is expected that the main code will create the storage array
//so it's not user-proof, but it should really clean up the process


#ifndef INC_statevariable_H_
#define INC_statevariable_H_

#include <stdio.h>
#include <stdint.h>

typedef struct statevariable {
	float input;
	float hz; //cutoff value in hertz
	float hp; //high pass
	float bp; //band pass
	float lp; //low pass
	float br; //band reject
	float fc; //multiplier (to be calculated based on a given frequency)
	float fs; //sample frequency (for calculation of fc)
	float q; //resonance
	float lastbp; //previous bandpass
	float lastlp; //previouslp
	float overclock; //set by ints to determine amount of overclocking - 1 for standard clock, 2 for 2xclock, etc
} statevariable;

void statevariable_init(statevariable *self, float samplefrequency, float cutoff);
void statevariable_input(statevariable *self, float newinput);
float statevariable_getHP(statevariable *self);
float statevariable_getBP(statevariable *self);
float statevariable_getLP(statevariable *self);
float statevariable_getBR(statevariable *self);
void statevariable_setFC(statevariable *self, float frequency);
void statevariable_setQ(statevariable *self, float resonance);
void statevariable_setOverclock(statevariable *self, float overclockAmount);

#endif /* INC_allpass_H_ */
