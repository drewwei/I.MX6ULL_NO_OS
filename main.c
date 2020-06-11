#include "imx6ull.h"
#include "clock_init.h"
#include "gic.h"
#include "led.h"
#include "uart.h"
#include "i2c.h"
#include "key_interrupt.h"
#include "my_printf.h"
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
    i2c_init();             /* i2c控制器初始化 */
    led_init();             /* 初始化LED GPIO */
    key_GPIO5_1_init();     /* 初始化KEY GPIO */
    request_irq(GPIO5_Combined_0_15_IRQn, key_irqHandler, NULL);    /* 注册并使能相应irq */
    
   // unsigned char cTestData = 'A'; /*用于测试发送的数据*/
    unsigned char ch = 0x3;
    unsigned char ret[2];
    unsigned int value; 
    //value = i2c_write_one_byte(0x1E, 0x10, ch, _I2C1);
    value = i2c_write_oneByte(I2C1, 0x1E, 0x0, 1, ch);
    
    printf("value: %d \r\n", value);

    //i2c_read_oneByte(I2C1, 0x1E, 0x0, 1, &ret);
    //led_ctl(1);
   
    while(1)
    {
		//PutChar(value);
        i2c_read_oneByte(I2C1, 0x1E, 0x0C, 1, &ret[0]);
        i2c_read_oneByte(I2C1, 0x1E, 0x0D, 1, &ret[1]);
        value = (ret[1] << 8) | ret[0];
        printf("value: %d \r\n", value);
        if(value < 1000)
            led_ctl(1);
        else
            led_ctl(0);
        delay();		
    }
    

    return 0;
}