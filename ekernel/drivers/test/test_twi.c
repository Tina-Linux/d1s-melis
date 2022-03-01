#include <getopt.h>
#include <log.h>
#include <rtthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sunxi_hal_twi.h>
#include <sunxi_drv_twi.h>

static int cmd_drv_twi(int argc, const char **argv)
{
    rt_device_t fd;
    twi_msg_t msg;
    char buf[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
    uint16_t addr = 0x50;
    uint32_t port;
    char dev_name[8];

    port = strtol(argv[1], NULL, 0);
    sprintf(dev_name, "twi%d", port);

    msg.flags = 0;
    msg.addr =  0x50;
    msg.flags &= ~(TWI_M_RD);
    msg.len = 6;
    msg.buf = buf;

    fd = rt_device_find(dev_name);
    if (fd == RT_NULL)
    {
        return -1;
    }

    rt_device_open(fd, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_STREAM);
    rt_device_control(fd, I2C_SLAVE, &addr);
    rt_device_write(fd, 1, buf, 6);  //twi write
    rt_device_read(fd, 1, buf, 6); //twi read
    rt_device_control(fd, I2C_RDWR, &msg); //twi control
    rt_device_close(fd);
    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_drv_twi, cmd_drv_twi, rtthread twi test code);

#define TEST_READ 0
#define TEST_WRITE 1

static int cmd_hal_twi(int argc, const char **argv)
{
    twi_msg_t msg;
    twi_port_t port;
    uint16_t addr;
    char reg_addr, reg_val = 0, rw = TEST_READ;
    int c;

    if (argc < 5)
    {
        printk("Usage:\n");
        printk("\ttwi [port] [slave_addr] [reg] -r\n");
        printk("\t                              -w [val]\n");
        return -1;
    }

    port = strtol(argv[1], NULL, 0);
    addr = strtol(argv[2], NULL, 0);
    reg_addr = strtol(argv[3], NULL, 0);

    while ((c = getopt(argc, (char *const *)argv, "r:w")) != -1)
    {
        switch (c)
        {
            case 'r':
                rw = TEST_READ;
                break;
            case 'w':
                rw = TEST_WRITE;
                reg_val = strtol(argv[5], NULL, 0);
                break;
        }
    }

    hal_twi_init(port);
    hal_twi_control(port, I2C_SLAVE, &addr);
    if (rw == TEST_READ)
    {
        hal_twi_read(port, reg_addr, &reg_val, 1);
        printk("reg_val: 0x%x\n", reg_val);
    }
    else if (rw == TEST_WRITE)
    {
        /*
         * hal_twi_write bug workaround
         */
        uint8_t buf[2];

        buf[0] = reg_addr;
        buf[1] = reg_val;
        msg.flags = 0;
        msg.addr =  addr;
        msg.len = 2;
        msg.buf = buf;

        hal_twi_control(port, I2C_RDWR, &msg);
        //hal_twi_write(port, reg_addr, &reg_val, 1);
    }
    hal_twi_uninit(port);

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_hal_twi, cmd_hal_twi, hal twi test code);
