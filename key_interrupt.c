#include "imx6ull.h"
#include "gpio.h"
#include "MCIMX6Y2.h"
#include "gic.h"
#include "uart.h"
#include "led.h"

/* 按键初始化函数 */
void key_GPIO5_1_init(void)
{
     GPIO_Group_t *GPIO_Group = (GPIO_Group_t *)GPIO5_BASE;      /* GPIO5寄存基地址 */
     rIOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER1 &= ~0xf;
     rIOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER1 |= 0X5;       /* 设置为GPIO模式 */
     rIOMUXC_SNVS_SW_PAD_CTL_PAD_SNVS_TAMPER1 = 0;          /* 设置电气属性 */
     gpio_set_pin_mode(GPIO_Group, 1, INTERRUPT);          /* 设置为GPIO 中断模式*/
     gpio_set_intMode(GPIO_Group, 1, FALLING_EDGE);        /* 下降沿触发 */
}

/* 按键中断服务函数 */
void key_irqHandler(IRQn_Type irq , void * userParam)
{
     PutChar('l');
     GPIO_Group_t *GPIO_Group = (GPIO_Group_t *)GPIO5_BASE;      /* GPIO5寄存基地址 */
     volatile int i;
     static int count = 0;
     for(i = 0; i < 10000; i++);   /* 按键消抖，但中断里面不应该延迟，用定时器比较好 */
     if(gpio_get_pin_value(GPIO_Group, 1) != 0)
     {
          GPIO_Group->GPIO_ISR = (1<<1);     /* 清中断 */
          return;
     }
     if((GPIO_Group->GPIO_ISR & (1<<1)) == 0)     /* 通过状态寄存器查看是否发生该中断 */
     {
          return;        /* 说明不是该中断发生 */
     }         
     led_ctl(count);
     count = !count;
     GPIO_Group->GPIO_ISR = (1<<1);     /* 清中断 */
}