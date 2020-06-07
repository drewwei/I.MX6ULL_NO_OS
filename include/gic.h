#ifndef  _GIC_H
#define  _GIC_H
#include "MCIMX6Y2.h"

#define Num             160


typedef void(*IRQ_Handler_t)(IRQn_Type irq ,void * userParam);

struct  irq_desc {
    IRQ_Handler_t isr;
    void * data;
};


int request_irq(IRQn_Type irq, IRQ_Handler_t irq_handler, void *data);
void int_init(void);
void gic_enable_irq(IRQn_Type nr);
void gic_disable_irq(IRQn_Type nr);

#endif