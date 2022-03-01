#ifndef __GDI_H__
#define __GDI_H__

/*gdi»æÖÆÄ£Ê½*/
#define GDI_DRAWMODE_NORMAL             (0)
#define GDI_DRAWMODE_XOR                (1<<0)
#define GDI_DRAWMODE_TRANS              (1<<1)
#define GDI_DRAWMODE_REV                (1<<2)
#define GDI_DRAWMODE_SHADOW             (1<<3)
#define GDI_DRAWMODE_BLEND              (1<<4)  /* uses alpha from color */
#define GDI_DRAWMODE_SRC_PREMULTIPLY    (1<<5)
#define GDI_DRAWMODE_DST_PREMULTIPLY    (1<<6)

#define GDI_FONTMODE_8BPP32             (0)
#define GDI_FONTMODE_8BPP256            (1<<0)
#define GDI_FONTMODE_8BPP128_1          (1<<1)    // font color 1
#define GDI_FONTMODE_8BPP128_2          (1<<2)    // font color 2
#define GDI_FONTMODE_32BPPSHADOW        (1<<3)   // 32bpp font shadow
#define GDI_FONTMODE_8BPP32_FRAME       (1<<4)  // 8bpp32 font frame mode 

#define GDI_TEXTMODE_NORMAL             GDI_DRAWMODE_NORMAL
#define GDI_TEXTMODE_XOR                GDI_DRAWMODE_XOR
#define GDI_TEXTMODE_TRANS              GDI_DRAWMODE_TRANS
#define GDI_TEXTMODE_REV                GDI_DRAWMODE_REV
#define GDI_TM_NORMAL                   GDI_DRAWMODE_NORMAL
#define GDI_TM_XOR                      GDI_DRAWMODE_XOR
#define GDI_TM_TRANS                    GDI_DRAWMODE_TRANS
#define GDI_TM_REV                      GDI_DRAWMODE_REV

/*Text Style*/
#define GDI_TS_NORMAL           0x00000000
#define GDI_TS_UNDERLINE        0x00000001
#define GDI_TS_STRIKETHRU       0x00000002
#define GDI_TS_OVERLINE         0x00000004

/* Pen Styles */
#define GDI_PS_NULL             0x00000010
#define GDI_PS_SOLID            0x00000011
#define GDI_PS_DASH             0x00000012  /* -------  */
#define GDI_PS_DOT              0x00000013  /* .......  */
#define GDI_PS_DASHDOT          0x00000014  /* _._._._  */
#define GDI_PS_DASHDOTDOT       0x00000015  /* _.._.._  */

/*Pen shapes*/
#define GDI_PS_ROUND            0x00000000
#define GDI_PS_FLAT             0x00000001
#define GDI_PS_SQUARE           0x00000002

/* Brush Styles */
#define GDI_BS_NULL             0x00000020
#define GDI_BS_SOLID            0x00000021
#define GDI_BS_HATCHED          0x00000022
#define GDI_BS_AGAINST          0x00000023


/* Hatch Styles */
#define GDI_HS_HORIZONTAL       0   /* ----- */
#define GDI_HS_VERTICAL         1   /* ||||| */
#define GDI_HS_FDIAGONAL        2   /* \\\\\ */
#define GDI_HS_BDIAGONAL        3   /* ///// */
#define GDI_HS_CROSS            4   /* +++++ */
#define GDI_HS_DIAGCROSS        5   /* xxxxx */

//System Pre-defined GDI

#define WHITE_PEN           0x00000000
#define LTGRAY_PEN          0x00000001
#define GRAY_PEN            0x00000002
#define DKGRAY_PEN          0x00000003
#define BLACK_PEN           0x00000004
#define NULL_PEN            0x00000005

#define WHITE_BRUSH         0x00000006
#define LTGRAY_BRUSH        0x00000007
#define GRAY_BRUSH          0x00000008
#define DKGRAY_BRUSH        0x00000009
#define BLACK_BRUSH         0x0000000A
#define NULL_BRUSH          0x0000000B

#endif

