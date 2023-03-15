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
#ifndef _GUI_CONFIG_H_
#define _GUI_CONFIG_H_

#define GUI_CHECK_ARG_EN            1

#define GUI_NAME_MAX                (32)
#define GUI_CLASSNAME_MAX           (15)
#define GUI_TITLE_MAX               (64)

#define GUI_SCR_WIDTH               (800)
#define GUI_SCR_HEIGTH              (480)

#define GUI_MAX_LYRZORDER_NUM       (4)

#define GUI_USEGDI                  (0)

#if 1
#ifndef I8
#define I8      signed char
#endif
#ifndef U8
#define U8      unsigned char     /* unsigned 8  bits. */
#endif
#ifndef I16
#define I16     signed short    /*   signed 16 bits. */
#endif
#ifndef U16
#define U16     unsigned short    /* unsigned 16 bits. */
#endif
#ifndef I32
#define I32     signed int   /*   signed 32 bits. */
#endif
#ifndef U32
#define U32     unsigned int   /* unsigned 32 bits. */
#endif
#define I16P    I16              /*   signed 16 bits OR MORE ! */
#define U16P    U16              /* unsigned 16 bits OR MORE ! */

typedef __u32           GUI_PIXELINDEX;
typedef __u32           HDC;
typedef char            Byte;

/*通用GUI kernel定义*/
#define ORANGE_ALLOC_SUPPORT           (1)
#define ORANGE_SUPPORT_CURSOR          (0)
//#define ORANGE_ALLOC_SIZE                (800*480*4*2)     /* Size of dynamic memory ... For WM and memory devices         */
#define ORANGE_ALLOC_SIZE              (200)     /* Size of dynamic memory ... For WM and memory devices         */
#define ORANGE_ALLOC_DATATYPE          I32
#define ORANGE_ALLOC_DATATYPE_U        I32
#define ORANGE_HMEM                    I32
#define ORANGE_HMEM_NULL               (0)
typedef ORANGE_HMEM                     ORANGE_HWIN;
typedef ORANGE_HMEM                     ORANGE_HSPRITE;

/*GDI 相关配置信息*/
#define GDI_SUPPORT_UNICODE         (1)             /* Support mixed ASCII/UNICODE strings                          */
#define GDI_SUPPORT_MEMDEV          (1)     /* Memory devices available */
#define GDI_NUM_LAYERS              (16)
#define GDI_DEFAULT_FONT            &GUI_Font6x8
#define GDI_SUPPORT_CHINESE         0   /*added by mcl*/
#define GDI_AA_SUPPORT              0   /* Anti aliasing available */
#define GDI_SKIN_SUPPORT            0   /* skin switch */
#endif

#define GUI_SCNDIR_NORMAL           (0)
#define GUI_SCNDIR_ROTATE90         (1)
#define GUI_SCNDIR_ROTATE180        (2)
#define GUI_SCNDIR_ROTATE270        (3)

#define GUI_INITSCNDIR              GUI_SCNDIR_NORMAL

/*操作系统定义*/
#define GUI_OS_LINUX        0x10
#define GUI_OS_EPOS         0x11
#define GUI_OS_WINCE        0x12

#define GUI_WIN_OS      GUI_OS_EPOS

#if(GUI_WIN_OS == GUI_OS_EPOS)

#if 0
#if(EPDK_ARCH == EPDK_ARCH_SUNIII)
#define ORANGE_SUPPORT_SPRITE          (1)

#if ORANGE_SUPPORT_SPRITE
#define ORANGE_SPRITE_NUM              (32)
#endif
#elif(EPDK_ARCH == EPDK_ARCH_SUNII)
#define ORANGE_SUPPORT_SPRITE          (1)

#if ORANGE_SUPPORT_SPRITE
#define ORANGE_SPRITE_NUM              (32)
#endif
#elif(EPDK_ARCH == EPDK_ARCH_SUNI)
#define ORANGE_SUPPORT_SPRITE          (0)

#if ORANGE_SUPPORT_SPRITE
#define ORANGE_SPRITE_NUM              (0)
#endif
#endif
#endif
#endif

#define _WIDGET_STATIC 1
#define _WIDGET_BUTTON 1
#define _WIDGET_SIMEDIT 1
#define _WIDGET_SLEDIT 1
#define _WIDGET_MLEDIT 1
#define _WIDGET_LISTBOX 1
#define _WIDGET_PROGRESSBAR 1
#define _WIDGET_TOOLBAR 1
#define _WIDGET_NEWTOOLBAR 1
#define _WIDGET_MENUBUTTON 1
#define _WIDGET_TRACKBAR 1
#define _WIDGET_COMBOBOX 1
#define _WIDGET_PROPSHEET 1
#define _EXT_WIDGET_MONTHCAL 1
#define _EXT_WIDGET_TREEVIEW 1
#define _EXT_WIDGET_SPINBOX 1
#define _EXT_WIDGET_COOLBAR 1
#define _EXT_WIDGET_LISTVIEW 1
/* #undef _EXT_CTRL_GRID */
#define _EXT_FULLGIF 1
#define _EXT_SKIN 1

#define  GUI_MAXSCEEN_WIDTH             1280
#define  GUI_MAXSCEEN_HEIGHT            720
#define  GUI_OVERWIDTH                  0xFFFF
#define  GUI_OVERHEIGHT                 0xFFFF

#endif  /* _GUI_PRIVATE_H_ */
