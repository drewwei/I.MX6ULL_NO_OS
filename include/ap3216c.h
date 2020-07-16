#ifndef     _AP3216C_H
#define     _AP3216C_H    
#include "MCIMX6Y2.h"

typedef struct _ap3216c_regs
{
    unsigned char SysConfig;
    unsigned char INTStatus;
    unsigned char INTClearManner;
    unsigned char IRDataLow;
    unsigned char IRDataHigh;
    unsigned char ALSDataLow;
    unsigned char ALSDataHigh;
    unsigned char PSDataLow;
    unsigned char PSDataHigh;    
}ap3216c_regs_t;


typedef struct _ap3216c_Value{
    unsigned int IRValue;
    unsigned int ALSValue;
    unsigned int PSValue;
    unsigned int Object_detect;
}ap3216c_Value_t;


void init_ap3216c(void);
int ap3216c_read(ap3216c_Value_t *value);

void GPIO4_16_init(void);
void ap3216c_irqHandler(IRQn_Type irq , void * userParam);



#endif 
