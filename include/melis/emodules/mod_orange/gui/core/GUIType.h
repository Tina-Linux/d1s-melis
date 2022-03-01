/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Miracle.MCL.China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : GUIType.h
* Purpose : Include file define the types used for GUI
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-1-6 11:55:42
**********************************************************************************************************************
*/

#ifndef  GUITYPE_H_INCLUDED
#define  GUITYPE_H_INCLUDED

#include "LCD.h"
#include "GUIConf.h"

typedef enum { GUI_WRAPMODE_NONE, GUI_WRAPMODE_WORD, GUI_WRAPMODE_CHAR } GUI_WRAPMODE;  //mcl
/*********************************************************************
*
*       Common types
*/
typedef const char   *GUI_ConstString;

typedef LCD_COLOR       GUI_COLOR;
typedef LCD_LOGPALETTE  GUI_LOGPALETTE;
typedef LCD_DRAWMODE    GUI_DRAWMODE;
typedef LCD_FONTMODE     GUI_FONTMODE;
typedef LCD_RECT        GUI_RECT;

typedef struct
{
    void (* pfDraw)(int x0,
                    int y0,
                    int xsize,
                    int ysize,
                    const U8 GUI_UNI_PTR *pPixel,
                    const LCD_LOGPALETTE GUI_UNI_PTR *pLogPal,
                    int xMag,
                    int yMag);
    GUI_COLOR(* pfIndex2Color)(int Index);
} GUI_BITMAP_METHODS;

typedef struct
{
    U16P XSize;
    U16P YSize;
    U16P BytesPerLine;
    U16P BitsPerPixel;
    const U8 GUI_UNI_PTR *pData;
    const GUI_LOGPALETTE GUI_UNI_PTR *pPal;
    const GUI_BITMAP_METHODS *pMethods;
} GUI_BITMAP;

/* This structure may not be changed because the data that it
   expects is read in binary form (via any kind of interface,
   at runtime).
   This structure should therefor not be changed.
*/
typedef struct
{
    U16 ID;           /* Version 1.00 => 100*/
    U16 Version;
    U16 XSize;
    U16 YSize;
    U16 BytesPerLine;
    U16 BitsPerPixel;
    U16 NumColors;
    U16 HasTrans;
} GUI_BITMAP_STREAM;

typedef struct
{
    int x, y;
    U8  Pressed;
} GUI_PID_STATE;


/*********************************************************************
*
*       FONT structures
*/

/* Translation list. Translates a character code into up to 2
   indices of images to display on top of each other;
   '? -> index('a'), index('?) */
typedef struct
{
    I16P c0;
    I16P c1;
} GUI_FONT_TRANSLIST;

typedef struct
{
    U16P FirstChar;
    U16P LastChar;
    const GUI_FONT_TRANSLIST GUI_UNI_PTR *pList;
} GUI_FONT_TRANSINFO;

typedef struct
{
    U8 XSize;
    U8 XDist;
    U8 BytesPerLine;
    const unsigned char GUI_UNI_PTR *pData;
} GUI_CHARINFO;

typedef struct
{
    U8 XSize;
    U8 YSize;
    I8 XPos;
    I8 YPos;
    U8 XDist;
    const unsigned char GUI_UNI_PTR *pData;
} GUI_CHARINFO_EXT;

typedef struct GUI_FONT_PROP
{
    U16P First;                                              /* First character               */
    U16P Last;                                               /* Last character                */
    const GUI_CHARINFO         GUI_UNI_PTR *paCharInfo;      /* Address of first character    */
    const struct GUI_FONT_PROP GUI_UNI_PTR *pNext;           /* Pointer to next               */
} GUI_FONT_PROP;

typedef struct GUI_FONT_PROP_EXT
{
    U16P First;                                              /* First character               */
    U16P Last;                                               /* Last character                */
    const GUI_CHARINFO_EXT         GUI_UNI_PTR *paCharInfo;  /* Address of first character    */
    const struct GUI_FONT_PROP_EXT GUI_UNI_PTR *pNext;       /* Pointer to next               */
} GUI_FONT_PROP_EXT;

typedef struct
{
    const unsigned char GUI_UNI_PTR *pData;
    const U8 GUI_UNI_PTR *pTransData;
    const GUI_FONT_TRANSINFO GUI_UNI_PTR *pTrans;
    U16P FirstChar;
    U16P LastChar;
    U8 XSize;
    U8 XDist;
    U8 BytesPerLine;
} GUI_FONT_MONO;

/*********************************************************************
*
*       FONT structures
*
* This structure is used when retrieving information about a font.
* It is designed for future expansion without incompatibilities.
*/
typedef struct
{
    U16 Flags;
    U8 Baseline;
    U8 LHeight;     /* height of a small lower case character (a,x) */
    U8 CHeight;     /* height of a small upper case character (A,X) */
} GUI_FONTINFO;

#define GUI_FONTINFO_FLAG_PROP (1<<0)    /* Is proportional */
#define GUI_FONTINFO_FLAG_MONO (1<<1)    /* Is monospaced */
#define GUI_FONTINFO_FLAG_AA   (1<<2)    /* Is an antialiased font */
#define GUI_FONTINFO_FLAG_AA2  (1<<3)    /* Is an antialiased font, 2bpp */
#define GUI_FONTINFO_FLAG_AA4  (1<<4)    /* Is an antialiased font, 4bpp */

/*********************************************************************
*
*       UNICODE Encoding
*/
typedef U16  tGUI_GetCharCode(const char GUI_UNI_PTR *s);
typedef int  tGUI_GetCharSize(const char GUI_UNI_PTR *s);
typedef int  tGUI_CalcSizeOfChar(U16 Char);
typedef int  tGUI_Encode(char *s, U16 Char);

typedef struct
{
    tGUI_GetCharCode            *pfGetCharCode;
    tGUI_GetCharSize            *pfGetCharSize;
    tGUI_CalcSizeOfChar         *pfCalcSizeOfChar;
    tGUI_Encode                 *pfEncode;
} GUI_UC_ENC_APILIST;

/*********************************************************************
*
*       FONT Encoding
*/
typedef int  tGUI_GetLineDistX(const char GUI_UNI_PTR *s, int Len);
typedef int  tGUI_GetLineLen(const char GUI_UNI_PTR *s, int MaxLen);
typedef void tGL_DispLine(const char GUI_UNI_PTR *s, int Len);

typedef struct
{
    tGUI_GetLineDistX          *pfGetLineDistX;
    tGUI_GetLineLen            *pfGetLineLen;
    tGL_DispLine               *pfDispLine;
} tGUI_ENC_APIList;

extern const tGUI_ENC_APIList GUI_ENC_APIList_SJIS;
extern const tGUI_ENC_APIList GUI_ENC_APIList_EXT;

/*********************************************************************
*
*       FONT methods
*/
typedef struct GUI_FONT GUI_FONT;

typedef void GUI_DISPCHAR(U16 c);
typedef int  GUI_GETCHARDISTX(U16P c);
typedef void GUI_GETFONTINFO(const GUI_FONT GUI_UNI_PTR *pFont, GUI_FONTINFO *pfi);
typedef char GUI_ISINFONT(const GUI_FONT GUI_UNI_PTR *pFont, U16 c);
typedef int  GUI_GETCHARINFO(U16P c, GUI_CHARINFO_EXT *pInfo);

#define DECLARE_FONT(Type)                                     \
    void GUI##Type##_DispChar    (U16P c);                         \
    int  GUI##Type##_GetCharDistX(U16P c);                         \
    void GUI##Type##_GetFontInfo (const GUI_FONT GUI_UNI_PTR * pFont, GUI_FONTINFO * pfi); \
    char GUI##Type##_IsInFont    (const GUI_FONT GUI_UNI_PTR * pFont, U16 c); \
    int  GUI##Type##_GetCharInfo (U16P c, GUI_CHARINFO_EXT * pInfo)

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/* MONO: Monospaced fonts */
DECLARE_FONT(MONO);
#define GUI_FONTTYPE_MONO       \
    GUIMONO_DispChar,             \
    GUIMONO_GetCharDistX,         \
    GUIMONO_GetFontInfo,          \
    GUIMONO_IsInFont,             \
    (GUI_GETCHARINFO *)0,         \
    (tGUI_ENC_APIList*)0

/* PROP: Proportional fonts */
DECLARE_FONT(PROP);
#define GUI_FONTTYPE_PROP       \
    GUIPROP_DispChar,             \
    GUIPROP_GetCharDistX,         \
    GUIPROP_GetFontInfo,          \
    GUIPROP_IsInFont,             \
    (GUI_GETCHARINFO *)0,         \
    (tGUI_ENC_APIList*)0

/* PROP_EXT: Extended proportional fonts */
DECLARE_FONT(PROP_EXT);
#define GUI_FONTTYPE_PROP_EXT       \
    GUIPROP_EXT_DispChar,             \
    GUIPROP_EXT_GetCharDistX,         \
    GUIPROP_EXT_GetFontInfo,          \
    GUIPROP_EXT_IsInFont,             \
    GUIPROP_EXT_GetCharInfo,          \
    &GUI_ENC_APIList_EXT

/* PROP_FRM: Extended proportional fonts, framed */
DECLARE_FONT(PROP_FRM);
#define GUI_FONTTYPE_PROP_FRM       \
    GUIPROP_FRM_DispChar,             \
    GUIPROP_FRM_GetCharDistX,         \
    GUIPROP_FRM_GetFontInfo,          \
    GUIPROP_FRM_IsInFont,             \
    (GUI_GETCHARINFO *)0,             \
    (tGUI_ENC_APIList*)0

/* PROP: Proportional fonts SJIS */
DECLARE_FONT(PROP);
#define GUI_FONTTYPE_PROP_SJIS  \
    GUIPROP_DispChar,             \
    GUIPROP_GetCharDistX,         \
    GUIPROP_GetFontInfo,          \
    GUIPROP_IsInFont,             \
    (GUI_GETCHARINFO *)0,         \
    &GUI_ENC_APIList_SJIS

/* PROPAA: Proportional, antialiased fonts */
DECLARE_FONT(PROPAA);
#define GUI_FONTTYPE_PROPAA       \
    GUIPROPAA_DispChar,             \
    GUIPROPAA_GetCharDistX,         \
    GUIPROPAA_GetFontInfo,          \
    GUIPROPAA_IsInFont,             \
    (GUI_GETCHARINFO *)0,           \
    (tGUI_ENC_APIList*)0

/* PROPAA: Proportional, antialiased fonts, 2bpp */
DECLARE_FONT(PROP_AA2);
#define GUI_FONTTYPE_PROP_AA2       \
    GUIPROP_AA2_DispChar,             \
    GUIPROP_AA2_GetCharDistX,         \
    GUIPROP_AA2_GetFontInfo,          \
    GUIPROP_AA2_IsInFont,             \
    (GUI_GETCHARINFO *)0,             \
    (tGUI_ENC_APIList*)0

/* PROPAA: Proportional, antialiased fonts, 2bpp, SJIS encoding */
DECLARE_FONT(PROP_AA2);
#define GUI_FONTTYPE_PROP_AA2_SJIS  \
    GUIPROP_AA2_DispChar,             \
    GUIPROP_AA2_GetCharDistX,         \
    GUIPROP_AA2_GetFontInfo,          \
    GUIPROP_AA2_IsInFont,             \
    (GUI_GETCHARINFO *)0,             \
    GUI_ENCODE_SJIS

/* PROPAA: Proportional, antialiased fonts, 4bpp */
DECLARE_FONT(PROP_AA4);
#define GUI_FONTTYPE_PROP_AA4       \
    GUIPROP_AA4_DispChar,             \
    GUIPROP_AA4_GetCharDistX,         \
    GUIPROP_AA4_GetFontInfo,          \
    GUIPROP_AA4_IsInFont,             \
    (GUI_GETCHARINFO *)0,             \
    (tGUI_ENC_APIList*)0

/* PROPAA: Proportional, antialiased fonts, 4bpp, SJIS encoding */
DECLARE_FONT(PROP_AA4);
#define GUI_FONTTYPE_PROP_AA4_SJIS  \
    GUIPROP_AA4_DispChar,             \
    GUIPROP_AA4_GetCharDistX,         \
    GUIPROP_AA4_GetFontInfo,          \
    GUIPROP_AA4_IsInFont,             \
    (GUI_GETCHARINFO *)0,             \
    GUI_ENCODE_SJIS

#if defined(__cplusplus)
}
#endif

typedef struct
{
    int magic;
    unsigned long faceHandle;
    U8 *pFrame8bpp32; //  pointer to font frame buffer
} FONT_EXT;

struct GUI_FONT
{
    GUI_DISPCHAR      *pfDispChar;
    GUI_GETCHARDISTX *pfGetCharDistX;
    GUI_GETFONTINFO   *pfGetFontInfo;
    GUI_ISINFONT      *pfIsInFont;
    GUI_GETCHARINFO   *pfGetCharInfo;
    const tGUI_ENC_APIList *pafEncode;
    U8 YSize;
    U8 YDist;
    U8 XMag;
    U8 YMag;
    union
    {
        const void              GUI_UNI_PTR *pFontData;
        const GUI_FONT_MONO     GUI_UNI_PTR *pMono;
        const GUI_FONT_PROP     GUI_UNI_PTR *pProp;
        const GUI_FONT_PROP_EXT GUI_UNI_PTR *pPropExt;
    } p;
    U8 Baseline;
    U8 LHeight;     /* Height of a small lower case character (a,x) */
    U8 CHeight;     /* Height of a small upper case character (A,X) */
    FONT_EXT  Font_EXT;
};

/*********************************************************************
*
*       System independent font structures (SIF)
*/
typedef struct
{
    U32 ID;           /* Font file ID */
    U16 YSize;        /* Height of font */
    U16 YDist;        /* Space of font Y */
    U16 Baseline;     /* Index of baseline */
    U16 LHeight;      /* Height of a small lower case character (a) */
    U16 CHeight;      /* Height of a upper case character (A) */
    U16 NumAreas;     /* Number of character areas */
} GUI_SI_FONT;

typedef struct
{
    U16 First;        /* Index of first character */
    U16 Last;         /* Index of last character */
} GUI_SIF_CHAR_AREA;

typedef struct
{
    U16 XSize;        /* Size of bitmap data in X */
    U16 XDist;        /* Number of pixels for increment cursor in X */
    U16 BytesPerLine; /* Number of bytes per line */
    U16 Dummy;
    U32 OffData;      /* Offset of pixel data */
} GUI_SIF_CHARINFO;

typedef struct
{
    U16 XSize;        /* Size of bitmap data in X */
    U16 YSize;        /* Size of bitmap data in X */
    I16 XOff;         /* Display offset of bitmap data in X */
    I16 YOff;         /* Display offset of bitmap data in Y */
    U16 XDist;        /* Number of pixels for increment cursor in X */
    U16 Dummy;
    U32 OffData;      /* Offset of pixel data */
} GUI_SIF_CHARINFO_EXT;

typedef struct tGUI_SIF_APIList_struct
{
    GUI_DISPCHAR           *pfDispChar;
    GUI_GETCHARDISTX       *pfGetCharDistX;
    GUI_GETFONTINFO        *pfGetFontInfo;
    GUI_ISINFONT           *pfIsInFont;
    GUI_GETCHARINFO        *pfGetCharInfo;
    const tGUI_ENC_APIList *pafEncode;
} tGUI_SIF_APIList;

#define GUI_SIF_TYPE          tGUI_SIF_APIList
#define GUI_SIF_TYPE_PROP     &GUI_SIF_APIList_Prop
#define GUI_SIF_TYPE_PROP_EXT &GUI_SIF_APIList_Prop_Ext
#define GUI_SIF_TYPE_PROP_FRM &GUI_SIF_APIList_Prop_Frm
#define GUI_SIF_TYPE_PROP_AA2 &GUI_SIF_APIList_Prop_AA2
#define GUI_SIF_TYPE_PROP_AA4 &GUI_SIF_APIList_Prop_AA4

/*********************************************************************
*
*       External binary font structures (XBF)
*/
#if GUI_COMPILER_SUPPORTS_FP

typedef int GUI_XBF_GET_DATA_FUNC(U32 Off, U16 NumBytes, void *pVoid, void *pBuffer);

typedef struct
{
    U16 First;                         /* First character of font */
    U16 Last;                          /* Last character of font */
    void *pVoid;                       /* Void pointer passed to GetData-function */
    GUI_XBF_GET_DATA_FUNC *pfGetData;  /* Pointer to callback function */
} GUI_XBF_DATA;

typedef struct tGUI_XBF_APIList_struct
{
    GUI_DISPCHAR           *pfDispChar;
    GUI_GETCHARDISTX       *pfGetCharDistX;
    GUI_GETFONTINFO        *pfGetFontInfo;
    GUI_ISINFONT           *pfIsInFont;
    GUI_GETCHARINFO        *pfGetCharInfo;
    const tGUI_ENC_APIList *pafEncode;
} tGUI_XBF_APIList;

#define GUI_XBF_TYPE          tGUI_XBF_APIList
#define GUI_XBF_TYPE_PROP     &GUI_XBF_APIList_Prop
#define GUI_XBF_TYPE_PROP_EXT &GUI_XBF_APIList_Prop_Ext
#define GUI_XBF_TYPE_PROP_FRM &GUI_XBF_APIList_Prop_Frm

#endif

/*********************************************************************
*
*       TrueType support (TTF)
*/
#if GUI_COMPILER_SUPPORTS_FP

typedef struct
{
    const char   *filePath;        /* mcl */
    U32 NumBytes;            /* Size of file in bytes */
} GUI_TTF_DATA;



typedef struct
{
    GUI_TTF_DATA *pTTF;      /* Pointer to GUI_TTF_DATA structure which contains location and size of font file */
    U32 aImageTypeBuffer[4]; /* Buffer for image type structure */
    int PixelHeight;         /* Pixel height of new font. It means the height of the surrounding rectangle
                            * between the glyphs 'g' anf 'f'. Please notice that it is not the distance
                            * between two lines of text. With other words the value returned byGUI_GetFontSizeY()
                            * is not identically with this value. */
    int FaceIndex;           /* Some font files can contain more than one font face. In case of more than one face
                            * this index specifies the zero based face index to be used to create the font.
                            * Usually 0. */
} GUI_TTF_CS;



typedef struct  tag_GUI_FONT_PAL
{
    int start_index;             // start index of palette(256 color)
    int num;                     // size of pal index used for font
} GUI_FONT_PAL;


typedef struct tag_GUI_TTF_ATTR
{
    int height;                             // size of font matrix
    char filepath[60];                      //  file path of *.ttf file
    GUI_FONT_PAL font_pal;
} GUI_TTF_ATTR;
#endif

/*********************************************************************
*
*       Memory management
*/
/*
#ifndef     GUI_HMEM
  #if GUI_MAXBLOCKS < 32768
    #define     GUI_HMEM        I16P
  #else
    #define     GUI_HMEM        I32
  #endif
#endif
*/


#ifndef     GUI_HMEM
#define     GUI_HMEM        I16P
#endif
#define     GUI_HMEM_NULL     (0)
typedef     GUI_HMEM      GUI_HWIN;
typedef     GUI_HMEM      GUI_HSPRITE;


#endif  /* GUITYPE_H_INCLUDED */

/*************************** End of file ****************************/
