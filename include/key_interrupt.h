#ifndef    _KEY_INTERRUPT_H
#define    _KEY_INTERRUPT_H 
#include "MCIMX6Y2.h"
void key_irqHandler(IRQn_Type irq , void * userParam);

void key_GPIO5_1_init(void);
#endif