/* 该文件下源代码是i2c控制器驱动源码
 * I.MX6ULL有好几个I2C控制器，通过不同传参可以用同一套代码实现i2c控制器的读写
 */
#include "fsl_iomuxc.h"
#include "i2c.h"
#include "my_printf.h"
/* 设置I2C_clock */
static void i2c_regs_init(I2C_Type * base)
{
    base->I2CR &= ~(1<<7);
    base->IFDR = 0x38;  /* divider:640 i2c_clock=66MHz/640 = 103.125KHz 标准速率*/
    base->I2CR |= (1<<7);   /* 复位IIC */
}

/* 设置IOMUX 为I2C模式 */
static void i2c_io_init(void)
{
    IOMUXC_SetPinMux(IOMUXC_UART4_RX_DATA_I2C1_SDA, 1);
    IOMUXC_SetPinMux(IOMUXC_UART4_TX_DATA_I2C1_SCL, 1);
    IOMUXC_SetPinConfig(IOMUXC_UART4_TX_DATA_I2C1_SCL, 0x70B0);  
    IOMUXC_SetPinConfig(IOMUXC_UART4_RX_DATA_I2C1_SDA, 0x70B0);   
}
/* IIC初始化 */
void i2c_init(void)
{
    i2c_io_init();
    i2c_regs_init((I2C_Type * )I2C1_BASE);
}


unsigned int i2c_start(I2C_Type * base, i2c_transfer_t * msg)
{
    if(base->I2SR & (1<<5))  /* 判断总线是否空闲，忙则返回 */
        return BUS_BUSY;

    base->I2CR &= ~(1<<3);   /* 使能ACK */
    base->I2CR |= (1<<5) | (1<<4);   /* 1 Master Transmit mode. Changing MSTA from 0 to 1 signals a Start on the bus and selects Master mode. */

    if(msg->dir == I2C_WRITE)
    {
        base->I2DR = (msg->slavAddr << 1) | 0;  /* 向i2c写入从设备地址 */
    }else {
        base->I2DR = ((msg->slavAddr << 1) | ((msg->regLen == 0) ? 1 : 0));  /* 向i2c写入从设备地址 */
    }

    return IIC_OK;
}

unsigned int i2c_stop(I2C_Type * base)
{
    int timeout = 0xffff;
    base->I2CR &= ~((1 <<5 ) | (1 << 4 ) | (1 << 3));      /* The MSTA bit is cleared by software to generate a Stop condition; */

   while(base->I2SR & (1 << 5))  /* 判断总线是否空闲，等待stop真实发出 */
   {
       timeout--;
       if(timeout == 0)
            return IIC_TIMEOUT;
   }
     return IIC_OK;
}

unsigned int i2c_repeatStart(I2C_Type * base)
{
    if(((base->I2SR & (1<<4))) && (base->I2SR & (1 << 5)))
    {
        base->I2SR &= ~(1<<4);  /* 清除仲裁丢失标志位 */
        return ARBITRATION_LOST;
    } 
    base->I2CR &= ~(1<<3);   /* 使能ACK */
    base->I2CR |= (1 << 4) | (1 << 2) ;   /* restart without stop */

    return IIC_OK;
}


unsigned int error_check(I2C_Type * base)
{   
    if(base->I2SR & (1<<4))
    {
        base->I2SR &= ~(1<<4);  /* 清除仲裁丢失标志位 */
        base->I2CR &= ~(1 << 7);	/* 复位I2C: 先关闭I2C 				*/
        base->I2CR |= (1 << 7);	/* 再打开I2C 				*/
        return ARBITRATION_LOST;
    }
    else if(base->I2SR & (1<<0))
    {
        return NOACK;
    }
        
    return IIC_OK;
}


unsigned int i2c_write(I2C_Type * base, i2c_transfer_t * msg)
{
    unsigned ret;
   	/* 等待数据寄存器就绪,可以再次发送数据 */
	while(!(base->I2SR & (1 << 7))); 

    base->I2SR &= ~(1 << 1);         /* 清中断标志位 */
    base->I2CR |= 1 << 4;	      /* 发送数据(transmit) */
    
    while(msg->len--)
    {
        base->I2DR = *msg->data++;      /* 向IIC数据寄存器写数据 */

        while((base->I2SR & (1 << 1)) == 0) {};    /* 等待传输完成，中断置为 */ 
        base->I2SR &= ~(1 << 1);         /* 清中断标志位 */
        ret = error_check(base);
        if(ret)    /* 如果没有收到ACK */
        {
            break;
        }
    }

    i2c_stop(base);
    return ret;
}

unsigned int i2c_read(I2C_Type * base, i2c_transfer_t * msg)
{
    unsigned int ret, dummy = 0;
    dummy++;
   /* 等待数据寄存器就绪 */
    while(!(base->I2SR & (1 << 7))); 

    base->I2SR &= ~(1 << 1);         /* 清中断标志位 */  

    if(msg->regLen > 0)
    {
        ret = i2c_repeatStart(base);        /* 发出restart */

        if(ret)    
        {
            base->I2CR &= ~(1 << 7);
            base->I2CR |= (1 << 7);     /* 重启IIC */
            return ARBITRATION_LOST;
        }

        base->I2DR = (msg->slavAddr << 1) | (1);  /* 发送从设备地址和读位 */

        while((base->I2SR & (1 << 1)) == 0) {};    /* 等待传输完成，中断置位 */ 

        //printf("1...\r\n");

        base->I2SR &= ~(1 << 1);         /* 清中断标志位 */

        ret = error_check(base);
        if(ret)    /* 如果没有收到ACK */
        {
            i2c_stop(base);
            return ret;
        }
    }

    base->I2CR &= ~((1 << 4) | (1 << 3));    /* 设置为接收模式,使能ACK */

    if(msg->len == 1)
    {
        base->I2CR |= (1 << 3);  /* 如果只读取一个字节则不发送ACK */
    }
/*
 *  P1468. In Master Receive mode, reading the data register allows a read to occur and initiates the
 *  next byte to be received 
 */
    dummy = base->I2DR;  /* 读一次，假读 */ 

    //printf("2...\r\n");
    do{
        msg->len--;
        while(!(base->I2SR & (1 << 1))) {};    /* 等待传输完成，中断置位 */ 
        //printf("3...\r\n");
        base->I2SR &= ~(1 << 1);         /* 清中断标志位 */
        if(msg->len == 0)
        {
            i2c_stop(base);         /* 如果是最后一个数据，就先发停止，再读数据 */
        }
        if(msg->len == 1)
        {
            base->I2CR |= (1 << 3);  /* 如果只剩下一个字节则下一次读不发送ACK */
        }

        *msg->data++ = base->I2DR;

    }while(msg->len);
    //printf("4...\r\n");
    return IIC_OK;
}


unsigned int i2c_transfer(I2C_Type * base, i2c_transfer_t * msg)
{
    unsigned int ret = 0;
    unsigned int len = msg->regLen;
    /* 先清标志位 */
    base->I2SR &= ~((1 << 4) | (1 << 1));

    /* 等待传输完成 */
	while(!((base->I2SR >> 7) & 0X1));  
    
    /* 发出start 和从设备地址加方向*/
    ret = i2c_start(base, msg);
    if(ret)
    {
        return ret;
    }

    while(!(base->I2SR & (1 << 1)));    /* 等待传输完成 */

    /* 判断是否收到ACK */
    ret = error_check(base);
    if(ret)    /* 如果没有收到ACK */
    {
        i2c_stop(base);
        return ret;
    }
    //base->I2SR &= ~(1 << 1);         /* 清中断标志位 */

    /* iic发出要写或者要读的寄存器地址 */
    while(len--)
    {       
        base->I2SR &= ~(1 << 1);         /* 清中断标志位 */

        base->I2DR = ((msg->regAddr) >> 8*(len)) & 0xff;   

        while((base->I2SR & (1 << 1)) == 0) {};    /* 等待传输完成，中断置为 */ 
        
        /* 判断是否收到ACK */
        ret = error_check(base);
        if(ret)    /* 如果没有收到ACK */
        {
            i2c_stop(base);
            return ret;
        }

        
    }
   

/* 发送数据 */
    if((msg->dir == I2C_WRITE) && (msg->len > 0))
    {
        ret = i2c_write(base, msg);
/* 读取数据 */
    }else if ((msg->dir == I2C_READ) && (msg->len > 0))
    {
        ret = i2c_read(base, msg);
    }
    
    return ret;
}


unsigned int i2c_write_oneByte(I2C_Type * base, unsigned char slavAddr, unsigned int reg, unsigned char data)
{  
    unsigned int ret;
    unsigned char oneByte = data;
    i2c_transfer_t msg;
    msg.data = &oneByte;
    msg.dir  = I2C_WRITE;
    msg.len  = 1;
    msg.regAddr  = reg;
    msg.slavAddr = slavAddr;
    msg.regLen   = 1;      /* 漏了这句导致了大问题 */
    ret = i2c_transfer(base, &msg);
    return ret;
}

unsigned int i2c_read_oneByte(I2C_Type * base, unsigned char slavAddr, unsigned int reg, unsigned char *data)
{
    unsigned int ret;
    i2c_transfer_t msg;
    msg.data = data;
    msg.dir  = I2C_READ;
    msg.len  = 1;
    msg.regAddr  = reg;
    msg.slavAddr = slavAddr;
    msg.regLen   = 1;

    ret = i2c_transfer(base, &msg);
    return ret;
}

