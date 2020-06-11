#include "imx6ull.h"
#include "gpio.h"
#include "MCIMX6Y2.h"
#include "uart.h"
GPIO_Group_t *GPIO5_Group = (GPIO_Group_t *)GPIO5_BASE;      /* GPIO5寄存基地址 */
/* led gpio初始化 */
void led_init(void)
{
     rIOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 &= ~0xf;      
     rIOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 |= 0X5;       /* 设置为GPIO模式 */
     rIOMUXC_SNVS_SW_PAD_CTL_PAD_SNVS_TAMPER3 = 0X10B0;     /* 设置电气属性 */
     gpio_set_pin_mode(GPIO5_Group, 3, OUTPUT);             /* 设置为GPIO 输出*/
     gpio_set_pin_value(GPIO5_Group, 3, HIGH);              /* 输出高电平 */
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
/* 开灯 */
void turn_on_led2(void)
{
     GPIO5_Group->GPIO_DR &= ~(1<<3);   /* 输出低电平，灯亮 */
}
/* 熄灯 */
void turn_off_led2(void)
{
     GPIO5_Group->GPIO_DR |= (1<<3);   /* 输出高电平，灯灭 */
}
