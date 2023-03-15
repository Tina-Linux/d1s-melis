/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eLIBs Sub-System
*
*                                   (c) Copyright 2008-2009, Jerry.Wang China
*                                             All Rights Reserved
*
* Moudle  : eLibs board config module
* File    : usb_cfg_def.h
* By      : Jerry
* Version : v1.0
* Date    : 2009-10-19 15:52
**********************************************************************************************************************
*/
#ifndef  _XXXX_BOARD_USB_CFG_DEF_H_
#define  _XXXX_BOARD_USB_CFG_DEF_H_

/*

datax
|--------------------------------------------------------
|  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
|--------------------------------------------------------
|    id_piog      |            id_no            |
|                 |                             |
|--------------------------------------------------------

|--------------------------------------------------------
|  15  |  14  |  13  |  12  |  11  |  10  |  9  |  8  |
|--------------------------------------------------------
|    vbus_piog       |            vbus_no             |
|                    |                                |
|--------------------------------------------------------

|--------------------------------------------------------
|  23  |  22  |  21  |  20  |  19  |  18  |  17  |  16  |
|--------------------------------------------------------
|    drv_vbus_piog   |            drv_vbus_no           |
|                    |                                  |
|--------------------------------------------------------

|--------------------------------------------------------
|  31  |  30  |  29  |  28  |  27  |  26  |  25  |  24  |
|--------------------------------------------------------
|      保留          |  det | port_type   | port        |
|                    |      |             |             |
|--------------------------------------------------------

bit4~0      :   id PIO 号。             注: power(189): pin41 : 0, pin42 : 1, pin42 : 2, pin42 : 3,
bit7~5      :   id PIO 组号。           0: 保留，没有使用PIO;    1: PIOA;    2: PIOB;    3: PIOC;
                                        4: PIOD;    5: PIOE;    6: PIOF;    7:  power(189)
例:
1、USB ID pin 为 PIOB 10， 则配置为: bit0~7 = 0x4a.
2、USB ID pin 为 power(189)的pin42， 则配置为: bit0~7 = 0xe1.

bit15~8     :   和bit7~0方法相同
bit23~16    :   和bit7~0方法相同

bit25~24    :   usb端口号。             0: port无效;    1: port0;       2: port1;   else: port无效
bit27~26    :   usb端口类型 。          0: device only; 1: host only;   2: otg;     else: device only
bit28       :   usb检测方式。           0: vbus/id检测; 1: dp/dm检测
bit31~29    :   保留

*/

#if (EPDK_ARCH == EPDK_ARCH_SUNI)
#define  BOARD_USBC_MAX_CTL_NUM    1
#elif (EPDK_ARCH == EPDK_ARCH_SUNII)
#define  BOARD_USBC_MAX_CTL_NUM    2
#elif (EPDK_ARCH == EPDK_ARCH_SUNIII)
#define  BOARD_USBC_MAX_CTL_NUM    3
#elif (EPDK_ARCH == EPDK_ARCH_SUNIV)
#define  BOARD_USBC_MAX_CTL_NUM    1

#endif

enum _usb_pio_group_type
{
    GROUP_TYPE_PIO = 0,
    GROUP_TYPE_P1
};

enum usb_port_type
{
    USB_PORT_TYPE_DEVICE = 0,
    USB_PORT_TYPE_HOST,
    USB_PORT_TYPE_OTG
};

enum usb_detect_type
{
    USB_DETECT_TYPE_VBUS_ID = 0,
    USB_DETECT_TYPE_DP_DM
};

enum usb_controller
{
    USB_CONTROLLER_NONE = 0,
    USB_CONTROLLER_0,
    USB_CONTROLLER_1,
    USB_CONTROLLER_ALL
};

/* pio信息 */
typedef struct tag_borad_pio
{
    __u32 valid;            /* pio是否可用。 0:无效, !0:有效    */

    __u32 group_type;       /* pio类型                          */
    __u32 group;            /* pio组号                          */
    __u32 no;               /* pio pin号                        */
}
board_pio_t;

/* usb port 信息 */
typedef struct tag_borad_usb_port
{
    __u32 valid;            /* port是否可用。 0:无效, !0:有效       */

    __u32 port;             /* usb端口号                            */
    /* 0: port0;    1: port1;   else: port无效 */
    __u32 port_type;            /* usb端口类型。                        */
    /* 0: device only; 1: host only; 2: otg */
    __u32 detect_type;      /* usb检测方式。                        */
    /* 0: vbus/id检测;  1: dp/dm检测        */

    board_pio_t id;         /* usb id pin信息                       */
    board_pio_t vbus;       /* usb vbus pin信息                     */
    board_pio_t drv_vbus;   /* usb drv_vbus pin信息                 */
} board_usb_port_t;

#endif  /*_XXXX_BOARD_USB_CFG_DEF_H_*/

