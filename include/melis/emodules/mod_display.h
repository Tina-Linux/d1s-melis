#ifndef _MOD_DISPLAY_H_
#define _MOD_DISPLAY_H_

#include <libc.h>
#include <log.h>
#include <typedef.h>
#include <mod_defs.h>
#include <video/sunxi_display2.h>

#define  MAX_VI_LAYERS   (4)
#define  MAX_UI_LAYERS   (4)


#define DISP_DEV "/dev/disp"
#define DISP_MOD_PATH "/data/mod/display.mod"

#define PIC_W 480
#define PIC_H 640


#define CHN_NUM 2
#define RET_FAIL -1
#define RET_OK 0

#define SCREEN_0 0
#define SCREEN_1 1

#define LAYER_ID_OFFSET 100
#define LYL_NUM 4
#define LAYER_DE 8
#define HLAY(chn, lyl) ((chn*4+lyl) + LAYER_ID_OFFSET)
#define HD2CHN(hlay) ((hlay - LAYER_ID_OFFSET)/4)
#define HD2LYL(hlay) ((hlay - LAYER_ID_OFFSET)%4)

#define UI_LYCHN 2
#define UI_LYLAY 0

#define SCREEN_NUM 2

#define ZORDER_MAX 16
#define ZORDER_MIN 0

typedef struct disp_layer_config disp_layer_config;
typedef enum tag_DISP_CMD __DISP_t;
typedef struct disp_fb_info disp_fb_info;

typedef enum
{
    DISP_FB_TYPE_RGB = 0x0,
    DISP_FB_TYPE_YUV = 0x1,
} __disp_pixel_type_t;


typedef struct{
	unsigned char alpha;
	unsigned char red;
	unsigned char green;
	unsigned char blue;
}disp_color;

enum
{
    HWD_STATUS_REQUESTED    = 1,
    HWD_STATUS_NOTUSED      = 2,
    //HWD_STATUS_OPENED       = 4
};

struct view_info
{
    unsigned int x;
    unsigned int y;
    unsigned int w;
    unsigned int h; 
};

typedef enum
{
    DISP_LCD_BRIGHT_LEVEL0  = 16,
    DISP_LCD_BRIGHT_LEVEL1  = 32,
    DISP_LCD_BRIGHT_LEVEL2  = 48,
    DISP_LCD_BRIGHT_LEVEL3  = 64,
    DISP_LCD_BRIGHT_LEVEL4  = 80,
    DISP_LCD_BRIGHT_LEVEL5  = 96,
    DISP_LCD_BRIGHT_LEVEL6  = 112,
    DISP_LCD_BRIGHT_LEVEL7  = 128,
    DISP_LCD_BRIGHT_LEVEL8  = 144,
    DISP_LCD_BRIGHT_LEVEL9  = 160,
    DISP_LCD_BRIGHT_LEVEL10 = 176,
    DISP_LCD_BRIGHT_LEVEL11 = 192,
    DISP_LCD_BRIGHT_LEVEL12 = 208,
    DISP_LCD_BRIGHT_LEVEL13 = 224,
    DISP_LCD_BRIGHT_LEVEL14 = 240,
    DISP_LCD_BRIGHT_LEVEL15 = 255,
} __disp_lcd_bright_t;

typedef enum
{
    DISP_VGA_H1680_V1050    = 0,
    DISP_VGA_H1440_V900     = 1,
    DISP_VGA_H1360_V768     = 2,
    DISP_VGA_H1280_V1024    = 3,
    DISP_VGA_H1024_V768     = 4,
    DISP_VGA_H800_V600      = 5,
    DISP_VGA_H640_V480      = 6,
    DISP_VGA_H1440_V900_RB  = 7,//not support yet
    DISP_VGA_H1680_V1050_RB = 8,//not support yet
    DISP_VGA_H1920_V1080_RB = 9,
    DISP_VGA_H1920_V1080    = 0xa,
} __disp_vga_mode_t;


/*
typedef enum
{
    DISP_BT601  = 0,
    DISP_BT709  = 1,
    DISP_YCC    = 2,
    DISP_VXYCC  = 3,
    DISP_MONO   = 4,
} __disp_cs_mode_t;
*/

typedef enum
{
    MOD_DISP_LAYER_WORK_MODE_NORMAL             = 0,    //normal work mode
    MOD_DISP_LAYER_WORK_MODE_SCALER             = 1,    //scaler work mode
} __mod_disp_layer_work_mode_t;

typedef enum
{
    MOD_DISP_OUTPUT_TYPE_NONE          = 0,    //output close
    MOD_DISP_OUTPUT_TYPE_LCD           = 1,    //output lcd
    MOD_DISP_OUTPUT_TYPE_TV            = 2,    //output tv
} __mod_disp_output_t;

typedef enum
{
    MOD_DISP_TV_MOD_NULL           = 0,    //output close
    MOD_DISP_TV_MOD_NTSC           = 1,    //output lcd
    MOD_DISP_TV_MOD_PAL            = 2,    //output tv
} __mod_disp_output_mode;

typedef enum
{
    DISP_MOD_INTERLEAVED        = 0x1,  //interleaved,1����ַ
    DISP_MOD_NON_MB_PLANAR      = 0x0,  //�޺��ƽ��ģʽ,3����ַ,RGB/YUVÿ��channel�ֱ���
    DISP_MOD_NON_MB_UV_COMBINED = 0x2,  //�޺��UV���ģʽ,2����ַ,Y��UV�ֱ���
    DISP_MOD_MB_PLANAR          = 0x4,  //���ƽ��ģʽ,3����ַ,RGB/YUVÿ��channel�ֱ���
    DISP_MOD_MB_UV_COMBINED     = 0x6,  //���UV���ģʽ ,2����ַ,Y��UV�ֱ���
} __disp_pixel_mod_t;

typedef enum
{
    //for interleave argb8888
    DISP_SEQ_ARGB   = 0x0, //A�ڸ�λ
    DISP_SEQ_BGRA   = 0x2,

    //for nterleaved yuv422
    DISP_SEQ_UYVY   = 0x3,
    DISP_SEQ_YUYV   = 0x4,
    DISP_SEQ_VYUY   = 0x5,
    DISP_SEQ_YVYU   = 0x6,

    //for interleaved yuv444
    DISP_SEQ_AYUV   = 0x7,
    DISP_SEQ_VUYA   = 0x8,

    //for uv_combined yuv420
    DISP_SEQ_UVUV   = 0x9,
    DISP_SEQ_VUVU   = 0xa,

    //for 16bpp rgb
    DISP_SEQ_P10    = 0xd,//p1�ڸ�λ
    DISP_SEQ_P01    = 0xe,//p0�ڸ�λ

    //for planar format or 8bpp rgb
    DISP_SEQ_P3210  = 0xf,//p3�ڸ�λ
    DISP_SEQ_P0123  = 0x10,//p0�ڸ�λ

    //for 4bpp rgb
    DISP_SEQ_P76543210  = 0x11,
    DISP_SEQ_P67452301  = 0x12,
    DISP_SEQ_P10325476  = 0x13,
    DISP_SEQ_P01234567  = 0x14,

    //for 2bpp rgb
    DISP_SEQ_2BPP_BIG_BIG       = 0x15,//15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
    DISP_SEQ_2BPP_BIG_LITTER    = 0x16,//12,13,14,15,8,9,10,11,4,5,6,7,0,1,2,3
    DISP_SEQ_2BPP_LITTER_BIG    = 0x17,//3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12
    DISP_SEQ_2BPP_LITTER_LITTER = 0x18,//0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15

    //for 1bpp rgb
    DISP_SEQ_1BPP_BIG_BIG       = 0x19,//31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0
    DISP_SEQ_1BPP_BIG_LITTER    = 0x1a,//24,25,26,27,28,29,30,31,16,17,18,19,20,21,22,23,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7
    DISP_SEQ_1BPP_LITTER_BIG    = 0x1b,//7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,23,22,21,20,19,18,17,16,31,30,29,28,27,26,25,24
    DISP_SEQ_1BPP_LITTER_LITTER = 0x1c,//0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
	DISP_SEQ_UNKNOWN_VALUE,

} __disp_pixel_seq_t;



typedef struct
{
    __u32 width;
    __u32 height;
} __disp_rectsz_t;

typedef struct
{
    __s32 x;
    __s32 y;
    __u32 width;
    __u32 height;
} __disp_rect_t;

#define MB_WTH 8
#define CODE_MODE_Y_BITS 3
#define CODE_MODE_C_BITS 2
#define BLK_MODE_BITS    2
#define C_DTS_BITS       4
#define ALIGN 128

typedef struct
{
    unsigned long               addr[3];    // frame buffer�����ݵ�ַ������rgb���ͣ�ֻ��addr[0]��Ч
    __disp_rectsz_t     size;//��λ��pixel
    enum disp_pixel_format  format;
    __disp_pixel_seq_t  seq;
    __disp_pixel_mod_t  mode;
    __bool              br_swap;    // blue red color swap flag, FALSE:RGB; TRUE:BGR,only used in rgb format
    enum disp_color_space    cs_mode;    //color space
    __s32                    is_10bit_flag;
    __s32                    lbc_mode;   //1:1.5x; 2:2x; 3:2.5x;
    __u32                    is_lossy; //lossy compression or not
    __u32                    rc_en;    //compact storage or not
} __disp_fb_t;

typedef struct
{
    __mod_disp_layer_work_mode_t    mode;       //layer work mode
    __u8                        pipe;       //layer pipe,0/1,if in scaler mode, scaler0 must be pipe0, scaler1 must be pipe1
    __u8                        prio;       //layer priority,can get layer prio,but never set layer prio
    __bool                      alpha_en;   //layer global alpha enable
    __u16                       alpha_val;  //layer global alpha value
    __bool                      ck_enable;  //layer color key enable
    __disp_rect_t               src_win;    // framebuffer source window,only care x,y if is not scaler mode
    __disp_rect_t               scn_win;    // screen window
    __disp_fb_t                 fb;         //framebuffer
} __disp_layer_info_t;


enum sunxi_tag_DISP_CMD
{
	MOD_DISP_SET_BKCOLOR,
    MOD_DISP_GET_BKCOLOR,        
	MOD_DISP_GET_OUTPUT_TYPE,
	MOD_DISP_GET_SCN_WIDTH,
	MOD_DISP_GET_SCN_HEIGHT,
	MOD_DISP_VSYNC_EVENT_EN,
	MOD_DISP_SHADOW_PROTECT,
	MOD_DISP_BLANK,
	MOD_DISP_DEVICE_SWITCH,
	
    MOD_DISP_LAYER_SET_CONFIG,
    MOD_DISP_LAYER_GET_CONFIG,
    MOD_DISP_LAYER_REQUEST,
    MOD_DISP_LAYER_RELEASE,
    MOD_DISP_CMD_LAYER_SET_PARA,
    MOD_DISP_CMD_LAYER_GET_PARA,
    MOD_DISP_CMD_LAYER_OPEN,
    MOD_DISP_CMD_LAYER_CLOSE,
    MOD_DISP_CMD_LAYER_SET_FB,
    MOD_DISP_CMD_LAYER_GET_FB,
	MOD_DISP_CMD_LAYER_SET_SCN_WINDOW,    
	MOD_DISP_CMD_LAYER_GET_SCN_WINDOW,
    MOD_DISP_CMD_LAYER_SET_SRC_WINDOW,
    MOD_DISP_CMD_LAYER_GET_SRC_WINDOW,
    MOD_DISP_CMD_START_CMD_CACHE,
    MOD_DISP_CMD_EXECUTE_CMD_AND_STOP_CACHE,
    MOD_DISP_CMD_LAYER_BOTTOM,
    MOD_DISP_CMD_LAYER_TOP,
    
    MOD_DISP_CMD_LAYER_ALPHA_ON,
    MOD_DISP_CMD_LAYER_ALPHA_OFF,
    MOD_DISP_CMD_LAYER_SET_ALPHA_VALUE,
    MOD_DISP_CMD_LAYER_GET_ALPHA_VALUE,
    
    MOD_DISP_CMD_VIDEO_SET_FB,
    MOD_DISP_CMD_VIDEO_GET_FB,
	MOD_DISP_CMD_VIDEO_START,
	MOD_DISP_CMD_VIDEO_STOP,
	
    MOD_DISP_CMD_LCD_OFF,
    MOD_DISP_CMD_LCD_ON,
    MOD_DISP_LCD_BACKLIGHT_ENABLE,
	MOD_DISP_LCD_BACKLIGHT_DISABLE,   
	MOD_DISP_LCD_SET_BRIGHTNESS,
	MOD_DISP_LCD_GET_BRIGHTNESS,
	MOD_DISP_TV_SET_GAMMA_TABLE,
	MOD_DISP_LCD_GAMMA_CORRECTION_ENABLE,
	MOD_DISP_LCD_GAMMA_CORRECTION_DISABLE,
	MOD_DISP_LCD_SET_GAMMA_TABLE,
	MOD_DISP_HDMI_SUPPORT_MODE,
	MOD_DISP_SET_TV_HPD,
	
	MOD_DISP_ENHANCE_ENABLE,
	MOD_DISP_ENHANCE_DISABLE,
	MOD_DISP_ENHANCE_DEMO_ENABLE,
	MOD_DISP_ENHANCE_DEMO_DISABLE,
	MOD_DISP_ENHANCE_SET_MODE,
	MOD_DISP_ENHANCE_GET_MODE,
	MOD_DISP_ENHANCE_SET_BRIGHT,
	MOD_DISP_ENHANCE_GET_BRIGHT,
	MOD_DISP_ENHANCE_SET_CONTRAST,
	MOD_DISP_ENHANCE_GET_CONTRAST,
	MOD_DISP_ENHANCE_SET_SATURATION,
	MOD_DISP_ENHANCE_GET_SATURATION,

	/* ---capture -- */
	MOD_DISP_CAPTURE_START,
	MOD_DISP_CAPTURE_STOP,
	MOD_DISP_CAPTURE_COMMIT,
	
	MOD_DISP_LCD_CHECK_OPEN_FINISH,	
						
	
};
typedef struct
{
    __s32   id;
    unsigned long   addr[3];
    __bool  interlace;
    __bool  top_field_first;
    __bool  discontinous;
    __u32   frame_rate; // *FRAME_RATE_BASE(���ڶ�Ϊ1000)
    unsigned long   flag_addr;//dit maf flag address
    __u32   flag_stride;//dit maf flag line stride
    __bool  maf_valid;
    __bool  pre_frame_valid;
    __s32   lbc_mode;   //1:1.5x; 2:2x; 3:2.5x;
    enum disp_pixel_format  format;
} __disp_video_fb_t;
#endif

