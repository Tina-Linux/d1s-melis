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
#ifndef __LANG_FMT_H__
#define __LANG_FMT_H__

#define SEC_NAME        ".langdat"  //8字节长度，节名

//语言资源文件数据格式定义
typedef struct tag_LANG_HEAD
{
    __s8        SecName[8];     //节名
    __u16       version;        //版本：0x0100
    __u16       size;           //LANG_HEAD数据结构大小
    __u32       LangTabOff;     //LANG表起始地址偏移量----------------->
    __u16       LangSize;       //LANG表中每个条目大小
    __u32       LangNum;        //LANG表条目个数,既语言种类个数
    __u16       StringSize;     //字符串size
    __u32       StringNum;      //字符串个数；
    __u32       align;          //数据边界对齐模式；
    __u32       flags;          //标志:SYSRES,LIBRES,APPRES
} __attribute__((__packed__)) LANG_HEAD; //34 byte
typedef struct tag_LANG
{
    __u16       LangID;             //0X400 0X420
    __u32       LangOffSet;         //LANG 条目起始地址偏移量
    __u32       LangSize;           //LANG 条目大小
    __u32       StringTabOff;       //字符表起始地址
} __attribute__((__packed__)) LANG; //

typedef struct tag_STRING
{
    __u16       LangID;         //LangID
    __u16       StringID;       //such as:0x0001
    __u32       offset;         //string data位置
    __u32       size;           //data数据长度
} __attribute__((__packed__)) LANG_STRING;

#endif /* __LANG_FMT_H__  */
