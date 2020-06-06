#include "imx6ull.h"
#include "led.h"
#include "clock_init.h"
inline void delay(void)
{
    volatile int i;
    for(i = 0; i < 0x320000; i++);
}
int main(void)
{
    imx6ull_clock_init();
    led_init();

    unsigned char cTestData = 'A'; /*用于测试发送的数据*/
 	Uart_Init()	 ;

    while(1)
    {
        led_ctl(1);
        delay();
        led_ctl(0);
        delay();
      
		PutChar(cTestData);

				
    }
    

    return 0;
}