
#ifndef INC_pot_H_
#define INC_pot_H_

#include <stdio.h>
#include <stdint.h>

typedef struct pot {
	int16_t old; //last stored value
	int16_t new; //new value
	int16_t cutoff; //movement cutoff
	uint8_t pauseReading; //if something else affected the setting, flag as 1 and mark current new value in old, don't update until new is far enough from old then clear this flag
	uint8_t queuePause; //use this to trigger a pause on the next cycle - potentially helpful with order-of-operations blocking the unpausing
}pot;

void initPot(pot *self, uint16_t cutoffAmount);
void updatePot(pot *self, uint16_t reading);

#endif /* INC_pot_H_ */
