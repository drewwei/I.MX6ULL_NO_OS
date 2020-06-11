#ifndef         _GPIO_H
#define         _GPIO_H
#include "imx6ull.h"    



typedef enum _gpio_mode {
    INPUT           =0,
    OUTPUT          =1,
    INTERRUPT       =2,
}gpio_mode_t;

typedef enum _gpio_intMode{
    LOW_LEVEL    = 0,
    HIGH_LEVEL   = 1,
    RISING_EDGE  = 2,
    FALLING_EDGE = 3,
}gpio_intMode_t;




int gpio_set_pin_mode(GPIO_Group_t *GPIOx, unsigned int pin , gpio_mode_t mode);
void gpio_set_intMode(GPIO_Group_t *GPIOx,unsigned int pin, gpio_intMode_t intMode);
void gpio_set_pin_value(GPIO_Group_t *GPIOx, unsigned int pin, unsigned int data);
unsigned int gpio_get_pin_value(GPIO_Group_t *GPIOx, unsigned int pin);

#endif