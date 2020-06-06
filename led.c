#include "imx6ull.h"
#include "gpio.h"
GPIO_Group_t *GPIO5 = (GPIO_Group_t *)GPIO5_BASE;      /* GPIO5寄存基地址 */

void led_init(void)
{
     rIOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 &= ~0xf;      
     rIOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 |= 0X5;       /* 设置为GPIO模式 */
     rIOMUXC_SNVS_SW_PAD_CTL_PAD_SNVS_TAMPER3 = 0X10B0;     /* 设置电气属性 */
     GPIO_Set_Pin(GPIO5, 3, OUTPUT, LOW);                   /* 设置为GPIO 输入输出*/
}

void led_ctl(int on)
{
    if(on)
    {
         GPIO5->GPIO_DR &= ~(1<<3);   /* 输出低电平，灯亮 */
    }
    else{
         GPIO5->GPIO_DR |= (1<<3);    /* 输出高电平，灯灭 */
    }

}

void turn_on_led2(void)
{
     GPIO5->GPIO_DR &= ~(1<<3);   /* 输出低电平，灯亮 */
}
void turn_off_led2(void)
{
     GPIO5->GPIO_DR |= (1<<3);   /* 输出高电平，灯灭 */
}