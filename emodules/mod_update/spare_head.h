/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef  __spare_head_h__
#define  __spare_head_h__
//#ifndef normal_gpio_cfg
//typedef struct _normal_gpio_cfg
//{
//    char      port;            /* port : PA/PB/PC ... */
//    char      port_num;        /* internal port num: PA00/PA01 ... */
//    char      mul_sel;         /* function num: input/output/io-disalbe ...*/
//    char      pull;            /* pull-up/pull-down/no-pull */
//    char      drv_level;       /* driver level: level0-3*/
//    char      data;            /* pin state when the port is configured as input or output*/
//    char      reserved[2];
//} normal_gpio_cfg;
//#endif
typedef struct _special_gpio_cfg
{
	unsigned char port;      /* port : PA/PB/PC ... */
	unsigned char port_num;  /* intern port num: PA00/PA01 .. */
	char          mul_sel;   /* function num: input/output/io-disalbe ...*/
	char          data;      /* pin state when the port is configured as input or output*/
} special_gpio_cfg;

typedef struct sdcard_spare_info_t
{
	int card_no[4];         /*controller num*/
	int speed_mode[4];      /*speed mode: 0--low speed, other--high speed */
	int line_sel[4];        /*line mode: 1/4/8*/
	int line_count[4];      /*line count*/
} sdcard_spare_info;

typedef struct _boot_extend_head_t {
	__u8		version[8];		/*version:1.0*/
	__u8		magic[8];		/* ="DRAM.ext" */
	__u8            select_mode;		/*0:不进行自动识别 1:gpio识别模式 2:gpadc识别模式 3:1个IO+gpadc识别模式*/
	__u8            gpadc_channel;		/*select gpadc 通道*/
	__u8            reserve[2];
	normal_gpio_cfg dram_select_gpio[4];	/*select_mode=1|3 时设置的pin*/
	unsigned int    dram_para[15][32];	/*ext dram参数*/
	__u8			reserve1[12];
} boot_extend_head_t;
#endif
