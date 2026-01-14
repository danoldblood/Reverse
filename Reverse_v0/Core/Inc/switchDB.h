
#ifndef INC_switchDB_H_
#define INC_switchDB_H_

#include <stdio.h>
#include <stdint.h>

typedef struct switchDB {
    uint8_t old; //stored state
    uint8_t new; //new state
    uint16_t counter; //counter
    uint16_t cutoff; //debounce cutoff
    uint16_t holdCutoff;
    uint16_t longHoldCutoff;
    uint8_t value; //debounced value, to be updated when cutoff is met
    uint8_t holdFlag;
    uint8_t longHoldFlag;
    uint8_t switchRegistered; //flag to check to see if a switch has been marked - if it has, do whatever processing desired and clear
} switchDB;

void initSwitch(switchDB *self, uint16_t cutoffAmount);
void initSwitchMomentary(switchDB *self, uint16_t cutoffAmount, uint16_t holdCutoffAmount, uint16_t longHoldCutoffAmount);
void debounce(switchDB *self, uint8_t reading);
void clearHoldFlag(switchDB *self);
void clearLongHoldFlag(switchDB *self);
void clearAllFlags(switchDB *self);

#endif /* INC_switchDB_H_ */
