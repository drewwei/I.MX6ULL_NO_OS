#ifndef         _GPIO_H
#define         _GPIO_H
#include "imx6ull.h"    


extern int GPIO_Set_Pin(GPIO_Group_t *GPIOx, int pin , unsigned int dir, unsigned int data);

#endif