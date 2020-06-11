#include "imx6ull.h"
#include "gpio.h"
/*******************************************************************
 * 函数名称：gpio_set_pin_mode
 * 功能描述：设置GPIOx的输入输出功能
 * 输入参数：GPIOx 是GPIO端口结构体指针；pin是GPIO在所在端口的编号；
 *          mode表示输入还是输出和中断
 * 返回参数：返回-1表示设置失败返回0表示设置成功
 *******************************************************************/
int gpio_set_pin_mode(GPIO_Group_t *GPIOx, unsigned int pin , gpio_mode_t mode)
{
     if((GPIOx == NULL) || (pin > 31))
          return -1;
     /* 设置GPIO输入或输出 */
     if(mode == OUTPUT)
     {
          GPIOx->GPIO_GDIR |= (1 << pin);
     }
     else{
          GPIOx->GPIO_GDIR &= ~(1 << pin);
     }
     if(mode == INTERRUPT)
     {
          GPIOx->GPIO_IMR |= (1 << pin);     /* 使能GPIO5_1中断功能 */  
          GPIOx->GPIO_ISR  = 0xfffffff;     /* 清中断,防止误触发,不然的话就一直中断！ */
     }
     return 0;
}
/* 设置GPIO中断触发方式 */
void gpio_set_intMode(GPIO_Group_t *GPIOx,unsigned int pin, gpio_intMode_t intMode)
{
     unsigned int pinShift = pin - 16;
     if(pin < 16)
     {
          GPIOx->GPIO_ICR1 &= ~ (0x3 << 2*pin);
          GPIOx->GPIO_ICR1 |= (intMode << 2*pin);  /* 设置触发方式 */
     } else {
          GPIOx->GPIO_ICR2 &= ~ (0x3 << 2*pinShift);
          GPIOx->GPIO_ICR2 |= (intMode << 2*pinShift);  /* 设置触发方式 */
     }   
}
/* 设置GPIO的高低电平值 */
void gpio_set_pin_value(GPIO_Group_t *GPIOx, unsigned int pin, unsigned int data)
{
     GPIOx->GPIO_DR   &= ~(1 << pin);
     GPIOx->GPIO_DR   |= (data << pin);
}
/* 获取GPIO的高低电平值 */
unsigned int gpio_get_pin_value(GPIO_Group_t *GPIOx, unsigned int pin)
{
     return ((GPIOx->GPIO_DR >> pin) & 1);
}