#include <led.h>

void led_init(led *self, GPIO_TypeDef* gpioport, uint32_t gpiopin)
{
	self->port = gpioport;
	self->pin = gpiopin;
	self->state = 0;
	self->paused = 0;
}
void led_flip(led *self)
{
	if(self->paused == 0)
	{
		self->state = !self->state;
		HAL_GPIO_WritePin(self->port, self->pin, self->state);
		self->flag = 0;
	}
}
void led_set(led *self, uint8_t state)
{
	if(self->paused == 0)
	{
		self->state = state;
		HAL_GPIO_WritePin(self->port, self->pin, self->state);
		self->flag = 0;
	}
}
void led_on(led *self)
{
	if(self->paused == 0)
	{
		self->state = 1;
		HAL_GPIO_WritePin(self->port, self->pin, LEDON);
		self->flag = 0;
	}
}
void led_off(led *self)
{
	if(self->paused == 0)
	{
		self->state = 0;
		HAL_GPIO_WritePin(self->port, self->pin, LEDOFF);
		self->flag = 0;
	}
}
void led_tick(led *self)
{
	self->counter++;
	if(self->counter > self->counterTop)
	{
		self->flag = 1;
		self->counter = 0;
	}
}
void led_setTop(led *self, uint16_t counterTop)
{
	self->counterTop = counterTop;
}

void led_clear(led *self)
{
	self->counter = 0;
}
void led_pause(led *self)
{
	self->paused = 1;
}
void led_resume(led *self)
{
	self->paused = 0;
}
