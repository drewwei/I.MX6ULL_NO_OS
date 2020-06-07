#include "imx6ull.h"
#include "gpio.h"
#include "MCIMX6Y2.h"
#include "gic.h"
#include "uart.h"
GPIO_Group_t *GPIO5_Group = (GPIO_Group_t *)GPIO5_BASE;      /* GPIO5寄存基地址 */

void led_init(void)
{
     rIOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 &= ~0xf;      
     rIOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 |= 0X5;       /* 设置为GPIO模式 */
     rIOMUXC_SNVS_SW_PAD_CTL_PAD_SNVS_TAMPER3 = 0X10B0;     /* 设置电气属性 */
     GPIO_Set_Pin(GPIO5_Group, 3, OUTPUT, HIGH);            /* 设置为GPIO 输出高电平，LED灭*/
}

void led_ctl(int on)
{
    if(on)
    {
         GPIO5_Group->GPIO_DR &= ~(1<<3);   /* 输出低电平，灯亮 */
    }
    else{
         GPIO5_Group->GPIO_DR |= (1<<3);    /* 输出高电平，灯灭 */
    }

}

void turn_on_led2(void)
{
     GPIO5_Group->GPIO_DR &= ~(1<<3);   /* 输出低电平，灯亮 */
}
void turn_off_led2(void)
{
     GPIO5_Group->GPIO_DR |= (1<<3);   /* 输出高电平，灯灭 */
}

void key_GPIO5_1_init(void)
{
     rIOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER1 &= ~0xf;
     rIOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER1 |= 0X5;       /* 设置为GPIO模式 */
     rIOMUXC_SNVS_SW_PAD_CTL_PAD_SNVS_TAMPER1 = 0;          /* 设置电气属性 */
     GPIO_Set_Pin(GPIO5_Group, 1, INPUT, 0);                   /* 设置为GPIO 输入*/
     GPIO5_Group->GPIO_IMR  |= (1<<1);     /* 使能GPIO5_1中断功能 */  
     GPIO5_Group->GPIO_ISR  = 0xfffffff;     /* 清中断,防止误触发,不然的话就一直中断！ */
     GPIO5_Group->GPIO_ICR1 &= ~ (0x3<<2); 
     GPIO5_Group->GPIO_ICR1 |=   (0x3<<2);   /* 下降沿触发 */

}
void led_irqHandler(IRQn_Type irq , void * userParam)
{
     PutChar('l');
     volatile int i;
     static int count = 0;
     unsigned int status = GPIO5_Group->GPIO_ISR;
     for(i = 0; i < 10000; i++);   /* 按键消抖，但中断里面不应该延迟，用定时器比较好 */
     if((GPIO5_Group->GPIO_DR & (1<<1)) != 0)
     {
          GPIO5_Group->GPIO_ISR = (1<<1);     /* 清中断 */
          return;
     }
     if((status & (1<<1)) == 0)
     {
          return;        /* 说明不是该中断发生 */
     }         
     led_ctl(count);
     count = !count;
     GPIO5_Group->GPIO_ISR = (1<<1);     /* 清中断 */
}