#include "imx6ull.h"

/*******************************************************************
 * 函数名称：GPIO_Set_Pin
 * 功能描述：设置GPIOx的输入输出功能
 * 输入参数：GPIOx 是GPIO端口结构体指针；GPIO_num是GPIO在所在端口的编号；
 *          dir表示输入还是输出，data表示输出高或低电平
 * 返回参数：返回-1表示设置失败返回0表示设置成功
 *******************************************************************/
int GPIO_Set_Pin(GPIO_Group_t *GPIOx, int GPIO_num , unsigned int dir, unsigned int data)
{
     if((GPIOx == NULL) || (data != 0 && data != 1) || (dir != 0 && dir != 1) || (GPIO_num > 31))
          return -1;
     /* 设置GPIO输入或输出 */
     if(dir)
     {
          GPIOx->GPIO_GDIR |= 1<<GPIO_num;
          /* 输出模式 设置GPIO出点电平 */
          GPIOx->GPIO_DR   &= ~(1<<GPIO_num);
          GPIOx->GPIO_DR   |= data<<GPIO_num;
     }
     else{
          GPIOx->GPIO_GDIR &= ~(1<<GPIO_num);
     }
     return 0;
}
