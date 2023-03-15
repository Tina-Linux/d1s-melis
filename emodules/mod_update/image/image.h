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
#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "typedef.h"

/*
#define ITEM_COMMON           "COMMON  "
#define ITEM_INFO             "INFO    "
#define ITEM_BOOTROM          "BOOTROM "
#define ITEM_FES              "FES     "
#define ITEM_FET              "FET     "
#define ITEM_FED              "FED     "
#define ITEM_FEX              "FEX     "
#define ITEM_BOOT0            "BOOT0   "
#define ITEM_BOOT1            "BOOT1   "
#define ITEM_ROOTFSFAT12      "RFSFAT12"
#define ITEM_ROOTFSFAT16      "RFSFAT16"
#define ITEM_ROOTFSFAT32      "FFSFAT32"
#define ITEM_USERFSFAT12      "UFSFAT12"
#define ITEM_USERFSFAT16      "UFSFAT16"
#define ITEM_USERFSFAT32      "UFSFAT32"
#define ITEM_PHOENIX_SCRIPT   "PXSCRIPT"
#define ITEM_PHOENIX_TOOLS    "PXTOOLS "
#define ITEM_AUDIO_DSP        "AUDIODSP"
#define ITEM_VIDEO_DSP        "VIDEODSP"
#define ITEM_FONT             "FONT    "
#define ITEM_FLASH_DRV        "FLASHDRV"
#define ITEM_OS_CORE          "OS_CORE "
#define ITEM_DRIVER           "DRIVER  "
#define ITEM_PIC              "PICTURE "
#define ITEM_AUDIO            "AUDIO   "
#define ITEM_VIDEO            "VIDEO   "
#define ITEM_APPLICATION      "APP     "
*/

//#define IMAGE_VER 100
//------------------------------------------------------------------------------------------------------------
#define IMAGE_MAGIC         "IMAGEWTY"
#define IMAGE_HEAD_VERSION  0x00000300

// Image文件头数据结构
#pragma pack(push, 1)
typedef struct tag_ImageHead
{
    __u8  magic[8];         // IMAGE_MAGIC
    __u32 version;          // 本结构的版本号，IMAGE_HEAD_VERSION
    __u32 size;             // 本结构的长度
    __u32 attr;             // 本结构的属性，格式按照version来确定，加密，压缩等
    __u32 imagever;         // image的版本，由脚本指定
    __u32 lenLo;            //image文件的总长度 低位
    __u32 lenHi;            //image文件的总长度 高位
    __u32 align;            // 数据的对齐边界，缺省1024
    __u32 pid;              // PID信息
    __u32 vid;              // VID信息
    __u32 hardwareid;       // 硬件平台ID
    __u32 firmwareid;       // 软件平台ID
    __u32 itemattr;         // item表的属性,“加密”
    __u32 itemsize;         // item数据项的大小
    __u32 itemcount;        // item数据项的个数
    __u32 itemoffset;       // item表偏移量
    __u32 imageattr;        // image文件属性
    __u32 appendsize;       // 附加数据的长度
    __u32 appendoffsetLo;   //附加数据的偏移量
    __u32 appendoffsetHi;   //附加数据的偏移量
    __u8  reserve[12];      //预留
} ImageHead_t;
#pragma pack(pop)

#define ALIGN_SIZE              0x400
#define HEAD_ATTR_NO_COMPRESS   0x4d    // 1001101

#pragma pack(push, 1)
typedef struct tag_ImageHeadAttr
{
    __u32 res     : 12;
    __u32 len     : 8;
    __u32 encode  : 7;                  // HEAD_ATTR_NO_COMPRESS
    __u32 compress: 5;
} ImageHeadAttr_t;
#pragma pack(pop)

#define IMAGE_ITEM_VERSION  0x00000100
#define MAINTYPE_LEN        (8)
#define SUBTYPE_LEN         (16)
#define FILE_PATH           256
#define IMAGE_ITEM_RCSIZE   640 // 数据项预留大小
#pragma pack(push, 1)
typedef struct tag_ImageItem
{
    // 考虑是否需要加magic
    __u32 version;                  // 本结构的版本号IMAGE_ITEM_VERSION
    __u32 size;                     // 本结构的长度
    __u8  mainType[MAINTYPE_LEN];   // 描述的文件的类型
    __u8  subType[SUBTYPE_LEN];     // 描述子类型，默认由image配置脚本指定
    __u32 attr;                     // 描述的文件的属性,格式按照version来确定，加密，压缩等
    __u8    name[FILE_PATH];        //文件名称 260
    __u32   datalenLo;              //文件数据在image文件中的长度
    __u32   datalenHi;              //高位 文件数据在image文件中的长度
    __u32 filelenLo;                //文件的实际长度
    __u32 filelenHi;                //高位 文件的实际长度
    __u32 offsetLo;             //文件起始位置偏移量
    __u32 offsetHi;             //高位 文件起始位置偏移量
    __u8    encryptID[64];          //加密插件ID，如果该文件不加密，该字段"\0"表示不加密
    __u32 checksum;             //描述的文件的校验和
    __u8    res[IMAGE_ITEM_RCSIZE];             //保留
} ImageItem_t;
#pragma pack(pop)

// image 解析接口
typedef void       *HIMAGE;
typedef void       *HIMAGEITEM;

/*
*********************************************************************************************************
* Function   : Img_Open
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
extern HIMAGE Img_Open(const char *filename);
extern HIMAGE   Img_Open_with_buffer(const __u8 *Image_buffer);
extern HIMAGE   Img_Open_buf(const __u8 *Image_buffer);

/*
*********************************************************************************************************
* Function   : Img_OpenItem
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
extern HIMAGEITEM Img_OpenItem(HIMAGE hImage, char *MainType, char *subType);

/*
*********************************************************************************************************
* Function   : Img_GetItemSize
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
extern __u64 Img_GetItemSize(HIMAGE hImage, HIMAGEITEM hItem);

/*
*********************************************************************************************************
* Function   : Img_ReadItemData
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
extern __u64 Img_ReadItemData(HIMAGE hImage, HIMAGEITEM hItem, void *buffer, __u64 Length);
/*
*********************************************************************************************************
* Function   : Img_CloseItem
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
extern __s32 Img_CloseItem(HIMAGE hImage, HIMAGEITEM hItem);

/*
*********************************************************************************************************
* Function   : Img_Close
*
* Description:
*
* Arguments  :
*
* Return     :
*
* Notes      :
*
*********************************************************************************************************
*/
extern void Img_Close(HIMAGE hImage);
extern void  Img_Close_buf(HIMAGE hImage);

#endif /* _IMAGE_H_ */
