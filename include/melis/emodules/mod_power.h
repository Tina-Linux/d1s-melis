#ifndef _DRV_POWER_H_
#define _DRV_POWER_H_

#define POWERDEV "/dev/power"

#define POWER_BOARD1_DEV_ADDR_19    (0x68>>1)
#define POWER_BOARD1_DEV_ADDR_18    (0x2C>>1)

#define DRV_POWER_BAT_CAP           2200
#define POWER_RDC_COUNT             10
#define DISCHARGE_CURRENT_RATE      35
#define EXT_PMOS                    115
#define CHG_RDC_RATE                100

#define MUSIC_CURRENT_LCD_OFF       50

#define  POWER_ADC_REGS_BASE                0xF1C24400
#define  POWER_ADC_REG_o_CTRL               0x00
#define  POWER_ADC_REG_o_CTRL1              0x04
#define  POWER_ADC_REG_o_FITER              0x0C
#define  POWER_ADC_REG_o_INT                0x10
#define  POWER_ADC_REG_o_FIFOS              0x14
#define  POWER_ADC_REG_o_TPR                0x18
#define  POWER_ADC_REG_o_CDAT               0x1c
#define  POWER_ADC_REG_o_TEMP_DATA          0x20
#define  POWER_ADC_REG_o_DATA               0x24
#define  POWER_ADC_REG_o_IO_CONFIG          0x28
#define  POWER_ADC_REG_o_PORT_DATA          0x2c
#define  TP_Y1                              (0x2<<8)
#define  TP_Y2                              (0x2<<12)
#define  TP_X1                              (0x2<<0)
#define  TP_X2                              (0x2<<4)
#define  POWER_ADC_DATA_IRQ                 (1 << 16)
#define  POWER_ADC_READ_REG(Nb)             (*(volatile unsigned int *)(POWER_ADC_REGS_BASE + (Nb)))
#define  POWER_ADC_WRITE_REG(Nb, value)     ((*(volatile unsigned int *)(POWER_ADC_REGS_BASE + (Nb))) = (value))

typedef struct __DRV_POWER_PARA_T
{
    __u8    byte_addr;
    __u8    reserved1;
    __u16   reserved2;
    __u8    *data;
} __drv_power_para_t;

typedef struct __DRV_POWER_BATTERY_STATUS1
{
    __u16   battery_exist       : 2;
    __u16   bat_voladc_enable   : 2;
    __u16   bat_crntadc_enable  : 2;
    __u16   bat_status          : 2;
    __u16   reserved            : 8;
    __u16   adc_freq;
    __u16   bat_vol;
    __u16   bat_crnt;

} __drv_power_battery_status_t1;

typedef struct __DRV_POWER_BATTERY_STATUS2
{
    __u8    dcin_valid;
    __u8    charge_status;
    __u8    battery_exist;
    __u8    battery_status;
    __u16   rest_vol;
    __u16   rest_time;
    __u16   bat_vol;
    __u16   bat_crnt;
    __u16   bat_cap;
    __u16   bat_rdc;
    __u8    dcin_not_enough;
    __u16   bat_vol_correct;

} __drv_power_battery_status_t2;

typedef struct _DRV_POWER_CHARGE_STATUS
{
    __u8    acin_valid;
    __u8    vbus_valid;
    __u8    ext_power_valid;
    __u8    bat_current_direction;
    __u8    in_Short;
    __u8    int_over_temp;
    __u8    charge_indication;
    __u8    battery_exist;
    __u8    battery_active;
    __u8    low_charge_current;
    __u16   battery_current;
    __u16   battery_vol;
    __u16   acin_vol;
    __u16   vbus_vol;
    __u8    dcin_not_enough;
    __u16   Ichgset;
    __u16   Iendchg;

} __drv_power_charge_stat;

typedef struct __DRV_POWER_BATTERY_STATUS3
{
    __u8    dcin_valid;
    __s8    charge_status;
    __u8    acin_en;
} __drv_power_battery_status_t3;

typedef struct __DRV_POWER_FUELGUAGE
{
    __s32   charge_status;
    __s32   rest_vol;
    __u32   rest_time;
    __u32   bat_cap;
    __u32   bat_power;
    __u32   rest_time_music;
    __u32   rest_time_video;
    __u32   rest_time_pic;
    __u32   rest_time_book;
    __u32   rest_time_standby;
    __s32   is_low_power;
    __s32   is_full_charge;

} __drv_power_fuelguage_t;

typedef struct __DRV_POWER_DCIN_STATUS
{
    __u16   dcin_exist          : 2;
    __u16   dcin_voladc_enable  : 2;
    __u16   dcin_crntadc_enable : 2;
    __u16   reserved            : 10;
    __u16   adc_freq;
    __u16   dcin_vol;
    __u16   dcin_crnt;
} __drv_power_dcin_status_t;

typedef struct __DRV_POWER_CHARGE_STATUS
{
    __u8    battery_status;
    __u8    charge_end;
    __u8    charge_autolimit;
    __u8    charge_enable;
    __u16   charge_vtarget;
    __u16   charge_current;
    __u16   charge_timer1;
    __u16   charge_timer2;
} __drv_power_charge_status_t;

typedef struct __DRV_POWER_BASIC_STATUS
{
    __u32   acin_input          : 1;
    __u32   charge_source       : 2;
    __u32   vout_exist          : 1;
    __u32   vout_enable         : 1;
    __u32   vout_choose         : 1;
    __u32   bat_exist           : 1;
    __u32   bat_work_status     : 1;
    __u32   bat_charge_status   : 1;
} __drv_power_basic_status_t;

typedef struct __DRV_POWER_IPS_STATUS
{
    __u16   dcin_choose     : 2;
    __u16   vhold_mode      : 2;
    __u16   reserved1       : 4;
    __u16   vhold;
    __u16   vwarning;
    __u16   voff;
} __drv_power_ips_status_t;

typedef struct  __DRV_POWER_ONOFFTIME
{
    __u16   on_time;
    __s16   on_status;
    __u16   off_time;
    __s16   off_status;
} __drv_power_onofftime_t;

#define POWER_EPT_1                 0x1f00
#define POWER_EPT_2                 0x1f01
#define POWER_EPT_3                 0x1f02
#define POWER_EPT_4                 0x1f03
#define POWER_EPT_5                 0x1f04
#define POWER_EPT_6                 0x1f05
#define POWER_EPT_7                 0x1f06
#define POWER_EPT_8                 0x1f07

#define POWER_EPT_MOS_N             (0x00 << 2)
#define POWER_EPT_MOS_P             (0x01 << 2)
#define POWER_EPT_MOS_NOTCARE       (0x02 << 2)

#define POWER_EPT_GATE_OFF          (0x00 << 0)
#define POWER_EPT_GATE_ON           (0x01 << 0)
#define POWER_EPT_GATE_NOTCARE      (0x02 << 0)

#define POWER_VOL_DCDC1             0x0f00
#define POWER_VOL_DCDC2             0x0f01
#define POWER_VOL_DCDC3             0x0f02
#define POWER_VOL_LDO1              0x0f03
#define POWER_VOL_LDO2              0x0f04
#define POWER_VOL_LDO3              0x0f05
#define POWER_VOL_LDO4              0x0f06
#define POWER_VOL_GPIO0             0x0f0a
#define POWER_VOL_EXTEN             0x0f0b

#define POWER_VOL_LDO5              0x0f07
#define POWER_VOL_SW1               0x0f08
#define POWER_VOL_SW2               0x0f09

#define POWER_VOL_ON                0x01
#define POWER_VOL_OFF               0x00

#define POWER_INT_VOUT_OP           (1<<7)
#define POWER_INT_VOUT_PLUGIN       (1<<6)
#define POWER_INT_VOUT_PLUGOUT      (1<<5)
#define POWER_INT_VOOUT_L_VHOLD     (1<<4)
#define POWER_INT_BAT_TMP_HIGH      (1<<2)
#define POWER_INT_BAT_TMP_LOW       (1<<1)
#define POWER_INT_VDCDC1_LOW        (1<<16)
#define POWER_INT_VDCDC2_LOW        (1<<15)
#define POWER_INT_VDCDC3_LOW        (1<<14)
#define POWER_INT_VLDO3_LOW         (1<<13)
#define POWER_INT_PEK_SHORT         (1<<12)
#define POWER_INT_PEK_LONG          (1<<11)
#define POWER_INT_BAT_IN            (1<<24)
#define POWER_INT_BAT_OUT           (1<<23)
#define POWER_INT_BAT_ACTIVE_IN     (1<<22)
#define POWER_INT_BAT_ACTIVE_OUT    (1<<21)
#define POWER_INT_CHARGE_START      (1<<20)
#define POWER_INT_CHARGE_END        (1<<19)
#define POWER_INT_LOWPWR_WARN       (1<<18)

typedef enum __DRV_POWER_CMD_GROUP
{
    DRV_POWER_CMD_READ_REG     =      0xff00,
    DRV_POWER_CMD_WRITE_REG,
    DRV_POWER_CMD_BASIC_STATUS,
    DRV_POWER_CMD_POWER_ON,
    DRV_POWER_CMD_POWER_OFF,
    DRV_POWER_CMD_INT_ENABLE,
    DRV_POWER_CMD_INT_DISABLE,
    DRV_POWER_CMD_INT_QUERY,
    DRV_POWER_CMD_INT_CLEAR,
    DRV_POWER_CMD_VOUT_GET,
    DRV_POWER_CMD_VOUT_SET,
    DRV_POWER_CMD_VSTS_GET,
    DRV_POWER_CMD_VSTS_SET,
    DRV_POWER_CMD_PWREPT_GET,
    DRV_POWER_CMD_PWREPT_SET,
    DRV_POWER_CMD_BATADC_GET,
    DRV_POWER_CMD_BATADC_SET,
    DRV_POWER_CMD_DCINADC_GET,
    DRV_POWER_CMD_DCINADC_SET,
    DRV_POWER_CMD_CHARGE_GET,
    DRV_POWER_CMD_CHARGE_SET,
    DRV_POWER_CMD_IPS_GET,
    DRV_POWER_CMD_IPS_SET,
    DRV_POWER_CMD_BATSTATUS_TABLE_GET,
    DRV_POWER_CMD_GET_BAT_RDC,
    DRV_POWER_CMD_GET_DCIN,
    DRV_POWER_CMD_GET_FUELGUAGE,
    DRV_POWER_CMD_SET_USB_CHARGE,
    DRV_POWER_CMD_SET_STANDBY_FLAG,
    DRV_POWER_CMD_ONOFFTIME_GET,
    DRV_POWER_CMD_ONOFFTIME_SET,
    DRV_POWER_CMD_CHGSTAT_GET,
    DRV_POWER_CMD_GET_COULOMB,
    DRV_POWER_CMD_GET_PMU_TYPE,
    DRV_POWER_CMD_RELEASE_DEV
} __drv_power_cmd_group_t;

#endif /*_DRV_POWER_H_*/
