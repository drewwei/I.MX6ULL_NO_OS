/*  ap3216c是一个整合了环境光传感器，距离传感器和红外LED的i2c外设器件
 *  本文件是ap3216c的设备驱动源代码，调用SOC的i2c控制器驱动的读写函数，即i2c.c
 *  文件下的i2c_write_oneByte和i2c_read_oneByte读写函数API，实现i2c控制器驱动和i2c设备驱动分层
 */
#include "ap3216c.h"
#include "fsl_iomuxc.h"
#include "i2c.h"
#include "my_printf.h"
#include "imx6ull.h"
#include "gpio.h"
#include "MCIMX6Y2.h"
#include "gic.h"
#include "uart.h"
#include "led.h"

/* 初始化ap3216c */
void init_ap3216c(void)
{
    i2c_write_oneByte(I2C1, 0x1E, 0x0, 0x3);    /* 011: ALS and PS+IR functions active */
    i2c_write_oneByte(I2C1, 0x1E, 0x10, 0x21);  /* 1291 lux, 4 conversion time */

    i2c_write_oneByte(I2C1, 0x1E, 0x1A, 150);   /* Lower byte of low interrupt threshold for ALS */
    i2c_write_oneByte(I2C1, 0x1E, 0x1B, 0);     /* Higher byte of low interrupt threshold for ALS */
    


    //i2c_write_oneByte(I2C1, 0x1E, 0x1C, 150);   /* Lower byte of high interrupt threshold for ALS */
    //i2c_write_oneByte(I2C1, 0x1E, 0x1D, 0);     /* Higher byte of high interrupt threshold for ALS */

    i2c_write_oneByte(I2C1, 0x1E, 0x02, 1);     /* 1: Software clear after writing 1 into address 0x01 each bit */
}
/* 读取ap3216c内寄存器的值 */
int read_ap3216c_regs(ap3216c_regs_t * data)
{ 
    if(!data)
    {
        printf("data is invalid \r\n");
        return -1;
    }    
    i2c_read_oneByte(I2C1, 0x1E, 0x0A, &data->IRDataLow);
    i2c_read_oneByte(I2C1, 0x1E, 0x0B, &data->IRDataHigh);
    i2c_read_oneByte(I2C1, 0x1E, 0x0C, &data->ALSDataLow);
    i2c_read_oneByte(I2C1, 0x1E, 0x0D, &data->ALSDataHigh);
    i2c_read_oneByte(I2C1, 0x1E, 0x0E, &data->PSDataLow);   
    i2c_read_oneByte(I2C1, 0x1E, 0x0F, &data->PSDataHigh);
    //printf("read_ap3216c_regs...\r\n");
    return 0;
}

/* 读取ap3216c各个传感器的值 */
 int ap3216c_read(ap3216c_Value_t *value)
{
    ap3216c_regs_t regs;
    unsigned int temp;
    read_ap3216c_regs(&regs);
    printf("app_ap3216c...\r\n");
    if((!value) || (regs.IRDataLow & (1<<7)))
    {
        printf("invalid value or data\r\n");
        return -1;
    }
    //printf("IRDataLow...\r\n");

    value->IRValue = (regs.IRDataHigh << 2) | (regs.IRDataLow & 0x3);
    temp =  (regs.ALSDataHigh << 7);
    value->ALSValue = (temp << 1) | (regs.ALSDataLow & 0xff);  /* 见鬼，左移8位会出问题 */
    value->PSValue = ((regs.PSDataHigh & 0x3f) << 4) | (regs.PSDataLow & 0xf);

    value->Object_detect = regs.PSDataHigh & (1<<7);
    //printf("Object_detect...\r\n");
    return 0;
}

/* GPIO4_16初始化 */
void GPIO4_16_init(void)
{
     GPIO_Group_t *GPIO_Group = (GPIO_Group_t *)GPIO4_BASE;      /* GPIO5寄存基地址 */
     IOMUXC_SetPinMux(IOMUXC_NAND_DQS_GPIO4_IO16, 0);           /* 设置成GPIO模式 */
     //IOMUXC_SetPinConfig(IOMUXC_NAND_DQS_GPIO4_IO16, 0x70B0);   /* 设置电气属性 */           
     gpio_set_pin_mode(GPIO_Group, 16, INTERRUPT);          /* 设置为GPIO 中断模式*/
     gpio_set_intMode(GPIO_Group, 16, FALLING_EDGE);        /* 下降沿触发 */
}

/* GPIO4_16中断服务函数  */
void ap3216c_irqHandler(IRQn_Type irq , void * userParam)
{
    unsigned char intStatus;
    ap3216c_Value_t data;

    if((GPIO4->ISR & (1<<16)) == 0)     /* 通过状态寄存器查看是否发生该中断 */
    {
          return;        /* 说明不是该中断发生 */
    } 

    i2c_read_oneByte(I2C1, 0x1E, 0x01, &intStatus);
    ap3216c_read(&data);
    
    if(intStatus & 1) /* 发生了ALS中断 */
    {
        if(data.ALSValue < 150) /* 当环境光小于150，开灯，并且将low interrupt（低于xx则发生中断）置为0 防止继续中断 ，high interrupt置为150，当环境光强于150时触发中断 */
        {
            led_ctl(1);      /* 打开Led */     
            i2c_write_oneByte(I2C1, 0x1E, 0x1C, 150);   /* Lower byte of high interrupt threshold for ALS，设置高值中断触发的值为150 */
            i2c_write_oneByte(I2C1, 0x1E, 0x1D, 0);     /* Higher byte of high interrupt threshold for ALS ，设置高值中断触发的值为150 */
            i2c_write_oneByte(I2C1, 0x1E, 0x1A, 0x0);     /* Lower byte of low interrupt threshold for ALS ，设置低值中断触发的值为0 */
            i2c_write_oneByte(I2C1, 0x1E, 0x1B, 0x0);     /* Higher byte of low interrupt threshold for ALS， 设置低值中断触发的值为0 */
        }       
        else /* 当环境光大于150，关灯，并且将high interrupt(高于xx则发生中断)置为0ffff 防止继续中断 ，low interrupt置为150，当环境光小于150时触发中断 */
        {   
            led_ctl(0);      /* 关闭LED */
            i2c_write_oneByte(I2C1, 0x1E, 0x1A, 150);    /* Lower byte of low interrupt threshold for ALS ，设置低值中断触发的值为150 */
            i2c_write_oneByte(I2C1, 0x1E, 0x1B, 0);      /* Higher byte of low interrupt threshold for ALS */
            i2c_write_oneByte(I2C1, 0x1E, 0x1C, 0x0ff);      /* Higher byte of low interrupt threshold for ALS， 设置高值中断触发的值为0xffff */
            i2c_write_oneByte(I2C1, 0x1E, 0x1D, 0x0ff);      /* Higher byte of high interrupt threshold for ALS */
        }
        printf("ap3216c_irqHandler ALS INT\r\n");
        i2c_write_oneByte(I2C1, 0x1E, 0x01, 0x01);  /* 清除ALS中断状态 */     
    }
    else    /* 发生PS中断 */
    {
        printf("ap3216c_irqHandler PS INT\r\n");
        i2c_write_oneByte(I2C1, 0x1E, 0x01, 0x02);  /* 清除PS中断状态 */   
    }
          
    GPIO4->ISR = (1<<16);     /* 清GPIO中断标志位 */
    
}