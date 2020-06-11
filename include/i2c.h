#ifndef  _I2C_H
#define  _I2C_H





void i2c_init(void);


typedef struct _i2c_transfer{
    unsigned int slavAddr;
    enum{
        I2C_WRITE = 0,
        I2C_READ  = 1,
    }dir;
    unsigned char * data;
    unsigned int len;
    unsigned int regAddr;
    unsigned int regLen;
}i2c_transfer_t;

#define IIC_OK                   0
#define BUS_BUSY                 (1<<0)
#define TRANSFER_UNCOMPLETE      (1<<1)
#define NOACK                    (1<<2)    
#define ARBITRATION_LOST         (1<<3)
#define IIC_TIMEOUT              (1<<4)


unsigned int i2c_write_oneByte(I2C_Type * base, unsigned char slavAddr, unsigned int reg, unsigned int reglen, unsigned char data);
unsigned int i2c_read_oneByte(I2C_Type * base, unsigned char slavAddr, unsigned int reg, unsigned int reglen, unsigned char *data);

#endif // ! _I2C_H