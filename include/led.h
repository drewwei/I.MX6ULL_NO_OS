#ifndef     _LDE_H
#define     _LDE_H

extern void led_init(void);
extern void turn_on_led2(void);
extern void turn_off_led2(void);
extern void led_ctl(int on);


void led_irqHandler(IRQn_Type irq , void * userParam);
void key_GPIO5_1_init(void);

#endif