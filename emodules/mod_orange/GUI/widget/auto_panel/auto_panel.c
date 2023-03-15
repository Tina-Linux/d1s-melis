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


#ifndef __GUI_AUTO_PANEL_C__
#define __GUI_AUTO_PANEL_C__

#include "auto_panel_i.h"
#include <kconfig.h>
#define PI      3.1415926535
#define sqrt2   1.4142135623
#define RADIAN(angle) ((angle)*PI/180.0)

#define printf __log

#define fopen(name, mode)               eLIBs_fopen((name),(mode))
#define fclose(fptr)                    eLIBs_fclose((fptr))
#define fread(buf,unit,size,fptr)       eLIBs_fread((buf),(unit),(size),(fptr))
#define fseek(fptr,offset,origin)       eLIBs_fllseek((fptr),(offset),(origin))
#define ftell(fptr)                     eLIBs_flltell((fptr))
#define fwrite(buf,unit,size,fptr)      eLIBs_fwrite((buf),(unit),(size),(fptr))
#define FILE                ES_FILE

#define malloc(x)                       esMEMS_Malloc(0, x)
#define free(x)                       esMEMS_Mfree(0, x)


#if 1


//角度到弧度转化

static int InitDesBmp(AutoPanel *this, BmpInfo *bmp)
{
    int desBufSize;
    char *desBuf = NULL;

    memset(bmp, 0, sizeof(BmpInfo));
    desBufSize = this->desBmpW * this->desBmpH * 4;
    desBuf = (char *)malloc(desBufSize);
    if (desBuf == NULL)
    {
        printf("malloc err\n");
        return -1;
    }
    memset(desBuf, 0x00, desBufSize);

    bmp->data = desBuf;
    bmp->head.bfType[0] = 0x42;
    bmp->head.bfType[1] = 0x4D;
    bmp->head.bfSize = sizeof(BmpHeadInfo) + this->desBmpW * this->desBmpH * 4;
    bmp->head.bfOffBits = sizeof(BmpHeadInfo);

    bmp->head.biSize = 40;
    bmp->head.biWidth = this->desBmpW;
    bmp->head.biHeight = this->desBmpH;
    bmp->head.biPlanes = 1;
    bmp->head.biBitCount = 32;
    return 0;
}
static int UninitDesBmp(BmpInfo *bmp)
{
    free(bmp->data);
    bmp->data = NULL;
    return 0;
}

static int InitTmpBmp(BmpInfo *bmp, BmpInfo *srcBmp)
{
    int tmpW, tmpH;
    int tmpMax;
    int desBufSize;
    char *desBuf = NULL;

    tmpW = srcBmp->head.biWidth;
    tmpH = srcBmp->head.biHeight;
    tmpMax = (tmpW > tmpH) ? (tmpW) : (tmpH);
    tmpMax = tmpMax * sqrt2 + 1;
    memset(bmp, 0, sizeof(BmpInfo));

    desBufSize = tmpMax * tmpMax * 4;
    desBuf = (char *)malloc(desBufSize);
    if (desBuf == NULL)
    {
        printf("malloc err\n");
        return -1;
    }
    memset(desBuf, 0x00, desBufSize);

    bmp->data = desBuf;
    bmp->head.bfType[0] = 0x42;
    bmp->head.bfType[1] = 0x4D;
    bmp->head.bfSize = sizeof(BmpHeadInfo) + tmpMax * tmpMax * 4;
    bmp->head.bfOffBits = sizeof(BmpHeadInfo);

    bmp->head.biSize = 40;
    bmp->head.biWidth = tmpMax;
    bmp->head.biHeight = tmpMax;
    bmp->head.biPlanes = 1;
    bmp->head.biBitCount = 32;
    return 0;
}

static int UninitTmpBmp(BmpInfo *bmp)
{
    if (bmp->data != NULL)
    {
        free(bmp->data);
        bmp->data = NULL;
    }
    return 0;
}
static int CalcDesBmpSize(AutoPanel *this)
{
    int tmpW, tmpH;
    int max;
    int tmpMax;

    tmpW = this->bmBmp.head.biWidth;
    tmpH = this->bmBmp.head.biHeight;
    max = (tmpW > tmpH) ? (tmpW) : (tmpH);

    if (this->hr_enable)
    {
        tmpW = this->hrBmp.head.biWidth;
        tmpH = this->hrBmp.head.biHeight;
        tmpMax = (tmpW > tmpH) ? (tmpW) : (tmpH);
        tmpMax = tmpMax * sqrt2 + 1;
        if (tmpMax > max)
        {
            max = tmpMax;
        }
    }

    if (this->min_enable)
    {
        tmpW = this->minBmp.head.biWidth;
        tmpH = this->minBmp.head.biHeight;
        tmpMax = (tmpW > tmpH) ? (tmpW) : (tmpH);
        tmpMax = tmpMax * sqrt2 + 1;
        if (tmpMax > max)
        {
            max = tmpMax;
        }
    }

    if (this->sec_enable)
    {
        tmpW = this->secBmp.head.biWidth;
        tmpH = this->secBmp.head.biHeight;
        tmpMax = (tmpW > tmpH) ? (tmpW) : (tmpH);
        tmpMax = tmpMax * sqrt2 + 1;
        if (tmpMax > max)
        {
            max = tmpMax;
        }
    }

    this->desBmpW = max;
    this->desBmpH = max;
    printf("desBmpW=%d, desBmpH=%d\n", this->desBmpW, this->desBmpH);
    return 0;
}

static int IsEnable(AutoPanel *this)
{
    if (this->hrBmp.data != NULL)
    {
        this->hr_enable = 1;
    }
    else
    {
        this->hr_enable = 0;
    }

    if (this->minBmp.data != NULL)
    {
        this->min_enable = 1;
    }
    else
    {
        this->min_enable = 0;
    }

    if (this->secBmp.data != NULL)
    {
        this->sec_enable = 1;
    }
    else
    {
        this->sec_enable = 0;
    }

    return 0;
}

static int BmpAlphaBlend(BmpInfo *desBmp, BmpInfo *srcBmp)
{
    int i, j;
    int sx, sy;
    int sOffset, dOffset;
    int desWidth, desHeight;
    int srcWidth, srcHeight;
    int lineSize, desLineSize;
    char *srcBuf = NULL;
    char *desBuf = NULL;
    char *sp = NULL;
    char *dp = NULL;

    srcWidth = srcBmp->head.biWidth;
    srcHeight = srcBmp->head.biHeight;
    lineSize = srcWidth * 4;
    srcBuf = srcBmp->data;

    desWidth = desBmp->head.biWidth;
    desHeight = desBmp->head.biHeight;
    desLineSize = desWidth * 4;
    desBuf = desBmp->data;

    if (srcWidth > desWidth || srcHeight > desHeight)
    {
        printf("err srcWidth > desWidth\n");
    }

    sx = (desWidth - srcWidth) * 4 / 2;
    sy = (desHeight - srcHeight) / 2;
    for (i = 0; i < srcHeight; i++)
    {
        dOffset = (sy + i) * desLineSize + sx;
        sOffset = i * lineSize;
        dp = desBuf + dOffset;
        sp = srcBuf + sOffset;
        for (j = 0; j < lineSize; j = j + 4)
        {
            /*
            dp[j+1] = sp[j+1]*sp[j+0] + dp[j+1] * (0xFF - sp[j+0]);     // r
            dp[j+2] = sp[j+2]*sp[j+0] + dp[j+2] * (0xFF - sp[j+0]);     // g
            dp[j+3] = sp[j+3]*sp[j+0] + dp[j+3] * (0xFF - sp[j+0]);     // b
            */

            /*
            dp[j+0] = sp[j+0];      // a
            dp[j+1] = sp[j+1];      // r
            dp[j+2] = sp[j+2];      // g
            dp[j+3] = sp[j+3];      // b
            */
            //if(sp[j+0] != 0)
            if (sp[j + 0] > 0x10)
            {
                dp[j + 0] = sp[j + 0];  // a
                dp[j + 1] = sp[j + 1];  // r
                dp[j + 2] = sp[j + 2];  // g
                dp[j + 3] = sp[j + 3];  // b
            }
        }
    }
    return 0;
}

static int RotateBmp(BmpInfo *dstBmp, BmpInfo *srcBmp, int angle)
{
    int i, j;
    char *desBuf = NULL;
    char *srcBuf = NULL;
    int bitCount, byteCount;
    int srcW, srcH, desWidth, desHeight;
    int lineSize, desLineSize, desBufSize;
    int tmpDesW, tmpDesH, tmpSrcW, tmpSrcH;
    int tmpX, tmpY, tXD, tYD, fx, fy;
    double sina, cosa, tX, tY, tXN, tYN, tmp;
    char *dB, *sB00, *sB10, *sB01, *sB11;

    bitCount = srcBmp->head.biBitCount;
    if (bitCount != 32)
    {
        printf("err not support\n");
        return -1;
    }

    srcW = srcBmp->head.biWidth;
    srcH = srcBmp->head.biHeight;
    lineSize = srcW * 4;

    desWidth = dstBmp->head.biWidth;
    desHeight = dstBmp->head.biHeight;
    desLineSize = desWidth * 4;
    desBufSize = desWidth * desHeight * 4;

    srcBuf = srcBmp->data;
    desBuf = dstBmp->data;
    memset(desBuf, 0x00, desBufSize);

    sina = sin(RADIAN(360 - angle));
    cosa = cos(RADIAN(360 - angle));
    tmpDesW = desWidth / 2;
    tmpDesH = desHeight / 2;
    tmpSrcW = srcW / 2;
    tmpSrcH = srcH / 2;
    byteCount = bitCount / 8;

    if (srcW > desWidth || srcH > desHeight)
    {
        printf("err srcW > desWidth\n");
    }

#if 1
    for (i = 0; i < desHeight; i++)
    {
        for (j = 0; j < desWidth; j++)
        {
            //转换到以图像为中心的坐标系，并进行逆旋转
            tX = (j - tmpDesW) * cosa + (-i + tmpDesH) * sina;
            tY = -(j - tmpDesW) * sina + (-i + tmpDesH) * cosa;
            //printf("i=%d, j=%d\n", i, j);

            //如果这个坐标不在原图像内，则不赋值
            if (tX > (tmpSrcW - 1) || tX < -(tmpSrcW - 1) || tY > (tmpSrcH - 1) || tY < -(tmpSrcH - 1))
            {
                continue;
            }

            //再转换到原坐标系下
            tXN = tX + tmpSrcW;
            tmp = (tY - tmpSrcH);
            tYN = (tmp < 0) ? (-tmp) : tmp;

            // 为了解决旋转引起的锯齿，引入临近像素加权平均值算法
            tmpX = tXN * 100;
            tmpY = tYN * 100;
            tXD = tmpX / 100;
            tYD = tmpY / 100;
            fx = tmpX % 100;
            fy = tmpY % 100;
            //printf("%d, %d, %d, %d\n", j, i, tXD, tYD, fx, fy);

            dB = &desBuf[i * desLineSize + j * byteCount];
            sB00 = &srcBuf[tYD * lineSize + tXD * byteCount];
            sB10 = &srcBuf[tYD * lineSize + (tXD + 1) * byteCount];
            sB01 = &srcBuf[(tYD + 1) * lineSize + tXD * byteCount];
            sB11 = &srcBuf[(tYD + 1) * lineSize + (tXD + 1) * byteCount];
            // a r g b加权
            dB[0] = (sB00[0] * (100 - fx) * (100 - fy) + sB10[0] * (fx) * (100 - fy) + sB01[0] * (100 - fx) * (fy) + sB11[0] * (fx) * (fy)) / 10000;
            dB[1] = (sB00[1] * (100 - fx) * (100 - fy) + sB10[1] * (fx) * (100 - fy) + sB01[1] * (100 - fx) * (fy) + sB11[1] * (fx) * (fy)) / 10000;
            dB[2] = (sB00[2] * (100 - fx) * (100 - fy) + sB10[2] * (fx) * (100 - fy) + sB01[2] * (100 - fx) * (fy) + sB11[2] * (fx) * (fy)) / 10000;
            dB[3] = (sB00[3] * (100 - fx) * (100 - fy) + sB10[3] * (fx) * (100 - fy) + sB01[3] * (100 - fx) * (fy) + sB11[3] * (fx) * (fy)) / 10000;
        }
    }
#endif

#if 0
    for (i = 0; i < desHeight; i++)
    {
        for (j = 0; j < desWidth; j++)
        {
            //转换到以图像为中心的坐标系，并进行逆旋转
            tX = (j - desWidth / 2) * cos(RADIAN(360 - angle)) + (-i + desHeight / 2) * sin(RADIAN(360 - angle));
            tY = -(j - desWidth / 2) * sin(RADIAN(360 - angle)) + (-i + desHeight / 2) * cos(RADIAN(360 - angle));
            //如果这个坐标不在原图像内，则不赋值
            if (tX > srcW / 2 || tX < -srcW / 2 || tY > srcH / 2 || tY < -srcH / 2)
            {
                continue;
            }
            //再转换到原坐标系下
            //tXN = tX + srcW / 2;
            //tYN = abs(tY - srcH / 2);
            tXN = tX + tmpSrcW;
            tmp = (tY - tmpSrcH);
            tYN = (tmp < 0) ? (-tmp) : tmp;
            tXD = tXN;
            tYD = tYN;

            //printf("%d, %d, %d, %d\n", j, i, tXN, tYN);
            //值拷贝
            memcpy(&desBuf[i * desLineSize + j * bitCount / 8], &srcBuf[tYD * lineSize + tXD * bitCount / 8], 4);
        }
    }
#endif
    return 0;
}

static int MiddleCopyBmp(BmpInfo *desBmp, BmpInfo *srcBmp)
{
    int i;
    int sx, sy;
    int sOffset, dOffset;
    int desBufSize, lineSize, desLineSize;
    char *srcBuf = NULL;
    char *desBuf = NULL;
    int desWidth, desHeight;
    int srcWidth, srcHeight;

    desBuf = desBmp->data;
    desWidth = desBmp->head.biWidth;
    desHeight = desBmp->head.biHeight;
    desBufSize = desWidth * desHeight * 4;
    memset(desBuf, 0x00, desBufSize);

    srcBuf = srcBmp->data;
    srcWidth = srcBmp->head.biWidth;
    srcHeight = srcBmp->head.biHeight;

    if (srcWidth > desWidth || srcHeight > desHeight)
    {
        printf("err srcWidth > desWidth\n");
    }

    lineSize = srcWidth * 4;
    desLineSize = desWidth * 4;
    sx = (desWidth - srcWidth) * 4 / 2;
    sy = (desHeight - srcHeight) / 2;
    for (i = 0; i < srcHeight; i++)
    {
        dOffset = (sy + i) * desLineSize + sx;
        sOffset = i * lineSize;
        memcpy((desBuf + dOffset), (srcBuf + sOffset), lineSize);
    }

    return 0;
}

int AutoPanelStart(void *hdle, BmpInfo *desBmp, int hrAngle, int minAngle, int secAngle)
{

    AutoPanel *this = (AutoPanel *)hdle;
    MiddleCopyBmp(desBmp, &this->bmBmp);

    if (this->hr_enable)
    {
        if (hrAngle != this->hrAngle)
        {
            this->hrAngle = hrAngle;
            RotateBmp(&this->tmpHrBmp, &this->hrBmp, hrAngle);
        }
    }
    if (this->min_enable)
    {
        if (minAngle != this->min_enable)
        {
            this->minAngle = minAngle;
            RotateBmp(&this->tmpMinBmp, &this->minBmp, minAngle);
        }
    }
    if (this->sec_enable)
    {
        if (secAngle != this->sec_enable)
        {
            this->secAngle = secAngle;
            RotateBmp(&this->tmpSecBmp, &this->secBmp, secAngle);
        }
    }
    if (this->hr_enable)
    {
        BmpAlphaBlend(desBmp, &this->tmpHrBmp);
    }
    if (this->min_enable)
    {
        BmpAlphaBlend(desBmp, &this->tmpMinBmp);
    }
    if (this->sec_enable)
    {
        BmpAlphaBlend(desBmp, &this->tmpSecBmp);
    }

    return 0;
}

int UnloadBmp(void *hdle, BmpInfo *bmp)
{
    if (bmp->data != NULL)
    {
        free(bmp->data);
        bmp->data = NULL;
    }
    return 0;
}

int LoadBmpFromFile(void *hdle, BmpInfo *bmp, const char *path)
{
#if 1
    FILE *fp = NULL;
    char *tmpBuf = NULL;
    unsigned int fileLen;
    unsigned int realLen;
    BmpHeadInfo bmpHead;

    fp = fopen(path, "rb");
    if (fp == NULL)
    {
        printf("open file err\n");
        goto err;
    }

    fseek(fp, 0, SEEK_END);
    fileLen = ftell(fp);
    printf("fileLen=%d\n", fileLen);

    tmpBuf = malloc(fileLen);
    if (tmpBuf == NULL)
    {
        printf("malloc err\n");
        goto err;
    }

    fseek(fp, 0, SEEK_SET);
    realLen = fread(tmpBuf, 1, fileLen, fp);
    if (realLen != fileLen)
    {
        printf("fread fail...\n");
        goto err;
    }

    memcpy(&bmpHead, tmpBuf, sizeof(bmpHead));
    if (bmpHead.bfType[0] != 'B' || bmpHead.bfType[1] != 'M')
    {
        printf("not bmp picture\n");
        goto err;
    }
    if (bmpHead.biBitCount != 32)
    {
        printf("bmp not 32bit\n");
        goto err;
    }
    if (bmpHead.biHeight < 0)
    {
        bmpHead.biHeight = -bmpHead.biHeight;
    }
    memcpy(&bmp->head, &bmpHead, sizeof(bmpHead));

#if 0
    printf("bmp head:\n bfSize=%d\n bfReserved=%d\n bfOffBits=%d\n biSize=%d\n\
	biWidth=%d\n biHeight=%d\n biPlanes=%d\n biBitCount=%d\n\
	biCompression=%d\n biSizeImage=%d\n biXPelsPerMeter=%d\n biYPelsPerMeter=%d\n\
	biClrUsed=%d\n biClrImportant=%d\n\n", \
           bmpHead.bfSize, bmpHead.bfReserved, bmpHead.bfOffBits, bmpHead.biSize, \
           bmpHead.biWidth, bmpHead.biHeight, bmpHead.biPlanes, bmpHead.biBitCount, \
           bmpHead.biCompression, bmpHead.biSizeImage, bmpHead.biXPelsPerMeter, bmpHead.biYPelsPerMeter, \
           bmpHead.biClrUsed, bmpHead.biClrImportant);
#endif

    bmp->data = malloc(bmpHead.biSizeImage);
    if (bmp->data == NULL)
    {
        printf("malloc err\n");
        goto err;
    }
    memcpy(bmp->data, tmpBuf + bmpHead.bfOffBits, bmpHead.biSizeImage);
    //printf("data:(0x%x, 0x%x, 0x%x, 0x%x)\n", bmp->data[0], bmp->data[1], bmp->data[2], bmp->data[3]);
    //printf("data:(0x%x, 0x%x, 0x%x, 0x%x)\n", bmp->data[4], bmp->data[5], bmp->data[6], bmp->data[7]);

    if (tmpBuf)
    {
        free(tmpBuf);
    }

    if (fp)
    {
        fclose(fp);
    }
    return 0;

err:
    if (bmp->data)
    {
        free(bmp->data);
    }

    if (tmpBuf)
    {
        free(tmpBuf);
    }

    if (fp)
    {
        fclose(fp);
    }
#endif
    return -1;
}

int AutoPanelPrepare(void *hdle)
{
    AutoPanel *this = (AutoPanel *)hdle;

    if (this->bmBmp.data == NULL)
    {
        printf("not bottom bmp\n");
        goto err;
    }

    IsEnable(this);
    CalcDesBmpSize(this);
    InitDesBmp(this, &this->dstBmp);

    if (this->hr_enable)
    {
        InitTmpBmp(&this->tmpHrBmp, &this->hrBmp);
    }
    if (this->min_enable)
    {
        InitTmpBmp(&this->tmpMinBmp, &this->minBmp);
    }
    if (this->sec_enable)
    {
        InitTmpBmp(&this->tmpSecBmp, &this->secBmp);
    }
    return 0;

err:
    return -1;
}

int AutoPanelUnprepare(void *hdle)
{
    AutoPanel *this = (AutoPanel *)hdle;

    if (this->sec_enable)
    {
        UninitTmpBmp(&this->tmpSecBmp);
    }
    if (this->min_enable)
    {
        UninitTmpBmp(&this->tmpMinBmp);
    }
    if (this->hr_enable)
    {
        UninitTmpBmp(&this->tmpHrBmp);
    }
    UninitDesBmp(&this->dstBmp);

    return 0;
}

void *CreateAutoPanel(int id)
{
    AutoPanel *this = (AutoPanel *)malloc(sizeof(AutoPanel));
    if (this == NULL)
    {
        return NULL;
    }
    memset(this, 0, sizeof(AutoPanel));
    this->id = id;
    this->hr_enable = 0;
    this->min_enable = 0;
    this->sec_enable = 0;
    this->hrAngle = -1;
    this->minAngle = -1;
    this->secAngle = -1;

    this->AtpLoadBmp = LoadBmpFromFile;
    this->AtpUnloadBmp = UnloadBmp;
    this->AtpPrepare = AutoPanelPrepare;
    this->AtpUnprepare = AutoPanelUnprepare;
    this->AtpStart = AutoPanelStart;
    return (void *)this;
}

int DestroyAutoPanel(void *hdle)
{
    AutoPanel *this = (AutoPanel *)hdle;
    if (this != NULL)
    {
        free(this);
    }
    return 0;

}
/*
应用调用示例
init:

    autoPanel = (AutoPanel *)GUI_CreateAutoPanel(0);
    autoPanel->AtpLoadBmp(autoPanel, &autoPanel->bmBmp, "d:\\b_200X200.bmp");
    autoPanel->AtpLoadBmp(autoPanel, &autoPanel->hrBmp, "d:\\t_100X100.bmp");
    autoPanel->AtpLoadBmp(autoPanel, &autoPanel->minBmp, "d:\\t_100X100.bmp");
    autoPanel->AtpLoadBmp(autoPanel, &autoPanel->secBmp, "d:\\t_100X100.bmp");
    autoPanel->AtpPrepare(autoPanel);
rotate:
    autoPanel->AtpStart(autoPanel, &autoPanel->dstBmp, angle, 0, 0);
    GUI_BMP_Draw_Rotate(autoPanel->dstBmp.data, 220, 100, autoPanel->dstBmp.head.biWidth, autoPanel->dstBmp.head.biHeight);
destory:
    autoPanel->AtpUnprepare(autoPanel);
    autoPanel->AtpUnloadBmp(autoPanel, &autoPanel->secBmp);
    autoPanel->AtpUnloadBmp(autoPanel, &autoPanel->minBmp);
    autoPanel->AtpUnloadBmp(autoPanel, &autoPanel->hrBmp);
    autoPanel->AtpUnloadBmp(autoPanel, &autoPanel->bmBmp);
    GUI_DestroyAutoPanel(autoPanel);

1、底图及时分秒图要是32bit的.bmp格式的图片。
2、底图、时分秒图最好是正方形的图片。
3、时分秒图指针部分透明度（a值）不为0，其他部分透明度（a值）为0。
4、时分秒图旋转的中心要是图片的中心。
5、支持1~3个指针的仪表盘，如果不要秒针，只要不加载卸载秒针图片即可。

*/
#endif
#endif

