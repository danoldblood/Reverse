#ifndef INC_led_H_
#define INC_led_H_

#include <stdio.h>
#include "stm32h7xx_hal.h"

#define LEDON	1
#define LEDOFF	0

typedef struct led {
    uint8_t state; //current state
    GPIO_TypeDef* port; //port
    uint16_t pin; //pin
    uint16_t counter; //counter
    uint16_t counterTop; //top of counter
    uint8_t flag; //flag to set when counter wraps
    uint8_t paused; //flag to set for pausing any led action
} led;

void led_init(led *self, GPIO_TypeDef* port, uint32_t pin);
void led_flip(led *self);
void led_set(led *self, uint8_t state);
void led_on(led *self);
void led_off(led *self);
void led_tick(led *self);
void led_setTop(led *self, uint16_t counterTop);
void led_clear(led *self);
void led_pause(led *self);
void led_resume(led *self);

#endif /* INC_led_H_ */
