#ifndef __GUI_AUTO_PANEL_H__
#define __GUI_AUTO_PANEL_H__

#include <kconfig.h>

#if defined(CONFIG_SOC_SUN3IW2) || defined(CONFIG_SOC_SUN8IW19) || defined(CONFIG_SOC_SUN20IW1)
#include "ResAdapter.h"
#endif

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "log.h"

// bmp file head
typedef struct BmpHeadInfo
{
    char    bfType[2];                // "BM"
    unsigned int    bfSize;           // total size of bmp file
    unsigned int   bfReserved;       // reserved field
    unsigned int   bfOffBits;        // pixel matrix offset from file head

    unsigned int   biSize;           // the size of bmp information head
    unsigned int   biWidth;          // the width of bmp, unit: pixel
    int     biHeight;                // the height of bmp, unit:pixel
    unsigned short int   biPlanes;   // always 1
    unsigned short int   biBitCount; // bits per pixel. 1-mono, 4-16color, 8-256color, 24-truecolor
    unsigned int   biCompression;    // copression format. 0: no compression.
    unsigned int   biSizeImage;      // the size of pixel matirx.
    unsigned int   biXPelsPerMeter;  // horizontal dis, unit : pixel/meter
    unsigned int   biYPelsPerMeter;  // vertical dis, unit : pixel/meter
    unsigned int   biClrUsed;        // number of used colors, 0 means 2^biBitCount
    unsigned int   biClrImportant;   // number of important colors, 0 means that all colors are important.
} __attribute__((packed)) BmpHeadInfo;

typedef struct BmpInfo
{
    BmpHeadInfo head;
    char *data;
} BmpInfo;

typedef struct AutoPanel
{
    // public member
    int id;
    BmpInfo hrBmp;      // hour bmp
    BmpInfo minBmp;     // min bmp
    BmpInfo secBmp;     // sec bmp
    BmpInfo bmBmp;      // bottom bmp

    BmpInfo dstBmp;     // output bmp
    int desBmpW;        // output bmp width
    int desBmpH;        // output bmp height

    // private member
    int hr_enable;
    int min_enable;
    int sec_enable;
    int hrAngle;        // 旋转角度，范围0-360度，精度6度
    int minAngle;
    int secAngle;
    BmpInfo tmpHrBmp;       // hour bmp
    BmpInfo tmpMinBmp;      // min bmp
    BmpInfo tmpSecBmp;      // sec bmp

    // interface
    int (*AtpLoadBmp)(void *hdle, BmpInfo *bmp, const char *path);
    int (*AtpUnloadBmp)(void *hdle, BmpInfo *bmp);
    int (*AtpPrepare)(void *hdle);
    int (*AtpUnprepare)(void *hdle);
    int (*AtpStart)(void *hdle, BmpInfo *desBmp, int hrAngle, int minAngle, int secAngle);

} AutoPanel;


void *CreateAutoPanel(int id);
int LoadBmpFromFile(void *hdle, BmpInfo *bmp, const char *path);
int UnloadBmp(void *hdle, BmpInfo *bmp);
int AutoPanelPrepare(void *hdle);
int AutoPanelUnprepare(void *hdle);
int AutoPanelStart(void *hdle, BmpInfo *desBmp, int hrAngle, int minAngle, int secAngle);
int DestroyAutoPanel(void *hdle);

#endif
