#ifndef  _DRV_TWI_H_
#define  _DRV_TWI_H_
#include <typedef.h>
#include <kconfig.h>


#define      TWI_SLAVE_ADDRESS_07BIT           (0)
#define      TWI_SLAVE_ADDRESS_10BIT           (1)

typedef  enum __TWI_CMD_SET
{
    TWI_WRITE_SPEC_RS  = 0,  //写规范，支持标准的twi写，带restart
    TWI_READ_SPEC_RS,        //读规范，支持标准的twi读，带restart
    TWI_READ_EX_NO_RS,        //新的twi命令，读，不带restart
    TWI_READ_EX_STP_RS,      //新的twi命令，读，stop之后再restart
    TWI_SET_SCL_CLOCK,       //设置twi的scl时钟，典型的�?100khz�?400khz，即100000 �?400000 单位为HZ
} __twi_cmd_set_t;

#ifdef CONFIG_SOC_SUN3IW1
typedef struct __IIC_DEV_PARA
{
    __u32    byte_addr;                             //�?要读写的数据在设备中的地�?
    __u32    byte_count;                            //�?次读写要完成的字节数
    __u32    baddr_count;                            //byte_addr中的字节个数
    __u8    *data;                                  //数据的地�?
} __iic_dev_para_t;
#endif

typedef struct __TWI_DEV_PARA_EX
{
    __u16    slave_addr;             //从设备地�?
    __u16    slave_addr_flag;        //从设置地�?宽度  0�?7bit�? 1�?10bit
    __u8    *byte_addr;              //�?要读写的数据在从设备中的地址,低字节存放低地址，高字节存放高地�?
    __u16    byte_addr_width;        //从设置地�?宽度  0�?1�?8 bit; 其它：数字代表字节数
    __u16    byte_count;             //�?次读写要完成的字节数
    __u32    reserved;               //保留�?
    __u8    *data;                   //数据的地�?
} __twi_dev_para_ex_t;

#endif