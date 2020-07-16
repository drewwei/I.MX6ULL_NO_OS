#include "imx6ull.h"
#include "clock_init.h"
#include "gic.h"
#include "led.h"
#include "uart.h"
#include "i2c.h"
#include "ap3216c.h"
#include "key_interrupt.h"
#include "my_printf.h"
inline void delay(void)
{
    volatile int i;
    for(i = 0; i < 0x220000; i++);
}
 
int main(void)
{
    int_init();             /* 初始化GIC中断控制器 */
    imx6ull_clock_init();   /* 初始化系统时钟 */
    Uart_Init();            /* 串口初始化 */
    i2c_init();             /* i2c控制器初始化 */
    led_init();             /* 初始化LED GPIO */
    key_GPIO5_1_init();     /* 初始化KEY GPIO */
    GPIO4_16_init();
    request_irq(GPIO5_Combined_0_15_IRQn, key_irqHandler, NULL);    /* 注册并使能相应irq */   
    request_irq(GPIO4_Combined_16_31_IRQn, ap3216c_irqHandler, NULL);    /* 注册并使能相应irq */   
    init_ap3216c();

    ap3216c_Value_t value;
    int ret,i = 100;
    while(i--)
    {	
        ret = ap3216c_read(&value);
        if(ret)
        {
            printf("The value is invalid\r\n");
            continue;
        }
        printf("value.detect: %d\r\n", (value.Object_detect >> 7));
        printf("value.ALSValue: %d\r\n", value.ALSValue);
        printf("value.PSValue: %d\r\n", value.PSValue);
        printf("value.IRValue: %d\r\n", value.IRValue);
        delay();
    }


    return 0;
}