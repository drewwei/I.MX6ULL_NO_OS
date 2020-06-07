#include "imx6ull.h"
#include "clock_init.h"
#include "gic.h"
#include "led.h"
#include "uart.h"

inline void delay(void)
{
    volatile int i;
    for(i = 0; i < 0x320000; i++);
}

 
int main(void)
{
    int_init();             /* 初始化GIC中断控制器 */
    imx6ull_clock_init();   /* 初始化系统时钟 */
    Uart_Init();            /* 串口初始化 */
    led_init();             /* 初始化LED GPIO */
    key_GPIO5_1_init();     /* 初始化KEY GPIO */
    request_irq(GPIO5_Combined_0_15_IRQn, led_irqHandler, NULL);    /* 注册并使能相应irq */
    led_ctl(1);
    unsigned char cTestData = 'A'; /*用于测试发送的数据*/
 	
    
    while(1)
    {
		PutChar(cTestData);
        delay();		
    }
    

    return 0;
}