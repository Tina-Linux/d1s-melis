/*
 * ===========================================================================================
 *
 *       Filename:  bmp.h
 *
 *    Description:  none
 *
 *        Version:  Melis3.0
 *         Create:  2018-08-24 19:16:03
 *       Revision:  none
 *       Compiler:  GCC:version 7.2.1 20170904 (release),ARM/embedded-7-branch revision 255204
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2018-08-24 19:16:37
 *
 * ===========================================================================================
 */
#ifndef  __bmp_h
#define  __bmp_h
#include <typedef.h>
#include <rtthread.h>

#define PALETTE_COLOR_BYTE_COUNT        4

enum _origin_pos_e
{
    ORIGIN_POS_UPPER_LEFT,
    ORIGIN_POS_LOWER_LEFT
};

typedef struct _full_color_t
{
    uint8_t     B;
    uint8_t     G;
    uint8_t     R;
    uint8_t     A;
} __attribute__((packed)) full_color_t;

typedef struct _bmp_file_head_t
{
    char        bfType[2];        // "BM"
    uint32_t    bfSize;           // total size of bmp file
    uint32_t    bfReserved;       // reserved field
    uint32_t    bfOffBits;        // pixel matrix offset from file head
} __attribute__((packed)) bmp_file_head_t;

typedef struct _bmp_info_head_t
{
    uint32_t    biSize;           // the size of bmp information head
    uint32_t    biWidth;          // the width of bmp, unit: pixel
    int32_t     biHeight;         // the height of bmp, unit:pixel
    uint16_t    biPlanes;         // always 1
    uint16_t    biBitCount;       // bits per pixel. 1-mono, 4-16color, 8-256color, 24-truecolor
    uint32_t    biCompression;    // copression format. 0: no compression.
    uint32_t    biSizeImage;      // the size of pixel matirx.
    uint32_t    biXPelsPerMeter;  // horizontal dis, unit : pixel/meter
    uint32_t    biYPelsPerMeter;  // vertical dis, unit : pixel/meter
    uint32_t    biClrUsed;        // number of used colors, 0 means 2^biBitCount
    uint32_t    biClrImportant;   // number of important colors, 0 means that all colors are important.
} __attribute__((packed)) bmp_info_head_t;

typedef struct _HBMP_i_t
{
    uint32_t    byte_count;
    uint16_t    bitcount;
    uint32_t    width;
    int32_t     height;
    uint32_t    row_size;
    uint32_t    matrix_off;
} HBMP_i_t, *HBMP_i;


typedef void  *HBMP;
extern HBMP    open_bmp(const char *bmp_file, HBMP_i_t *hbmp_hdl_buf);
extern HBMP    open_ram_bmp(const unsigned long base, const __u32 size, HBMP_i_t *hbmp_hdl_buf);
extern int32_t   close_bmp(HBMP hbmp);
extern uint32_t   get_bitcount(HBMP hbmp);
extern uint32_t   get_width(HBMP hbmp);
extern int32_t   get_height(HBMP hbmp);
extern uint32_t   get_rowsize(HBMP hbmp);
extern int32_t   get_matrix(HBMP hbmp, void *buf);
extern int32_t   read_pixel(HBMP hbmp, uint32_t x, uint32_t y, full_color_t *color_p, int32_t origin_pos);
extern int32_t   read_index(HBMP hbmp, uint32_t x, uint32_t y, uint8_t *index_p, int32_t origin_pos);

#endif     //  ifndef __bmp_h
