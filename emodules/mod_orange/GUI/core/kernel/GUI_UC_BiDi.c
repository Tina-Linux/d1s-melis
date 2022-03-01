/*
************************************************************************************************************************
*                                                    Charset
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : sys_charset.h
*
* Author      : Derek.Lai
*
* Version     : 1.1.0
*
* Date        : 2010.10.30
*
* Description :
*
* Others      : None at present.
*
*
* History     :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.08.22       1.1.0        build the file
*
************************************************************************************************************************
*/
#include <stddef.h>           /* needed for definition of NULL */
#include "gui_core_kernel_private.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "epdk.h"

#define MAX_CCH 8000
#define chLS 0x15
#define MAX_NUM_ARRAY 256
const int MAX_LEVEL = 61; // the real value

static __u16 bidi_types[MAX_CCH];    //__s32
static __u16 levels[MAX_CCH];           //__s32
static __u16 pszInput[MAX_CCH + 1];
static __u16 disp_string[MAX_CCH + 1];
static __bool  break_line[MAX_CCH] = {FALSE};




// === HELPER FUNCTIONS AND DECLARATIONS =================================

#define odd(x) ((x) & 1)

/*------------------------------------------------------------------------
    Bidirectional Character Types

    as defined by the unicode_id Bidirectional Algorithm Table 3-7.

    Note:

      The list of bidirectional character types here is not grouped the
      same way as the table 3-7, since the numberic values for the types
      are chosen to keep the state and action tables compact.
------------------------------------------------------------------------*/
enum bidi_class
{
    // input types
    // ON MUST be zero, code relies on ON = N = 0
    ON = 0,  // Other Neutral
    L,       // Left Letter
    R,       // Right Letter
    AN,      // Arabic Number
    EN,      // European Number
    AL,      // Arabic Letter (Right-to-left)
    NSM,     // Non-spacing Mark
    CS,      // Common Separator
    ES,      // European Separator
    ET,      // European Terminator (post/prefix e.g. $ and %)

    // resolved types
    BN,      // Boundary neutral (type of RLE etc after explicit levels)

    // input types,
    S,       // Segment Separator (TAB)     // used only in L1
    WS,      // White space                 // used only in L1
    B,       // Paragraph Separator (aka as PS)

    // types for explicit controls
    RLO,     // these are used only in X1-X9
    RLE,
    LRO,
    LRE,
    PDF,

    // resolved types, also resolved directions
    N = ON,  // alias, where ON, WS and S are treated the same
    LRT = L,
    RTL = R,
    BS = B,
    SS = S,
};

/**********************************************************************************************************
//                          get_unicode_id_charater_bidi_type()
// Author:  Derek Lai
// Time:2010.10.26
// Arguments:
// Return:
// Descripion:
// Note:
// History:
***********************************************************************************************************/
static __u16 get_unicode_id_charater_bidi_type(__u16 unicode_id)    //__s32
{
    /*WS*/
    if (((0x2000 <= unicode_id) && (0x200A >= unicode_id)) || (0x000C == unicode_id) || (0x0020 == unicode_id) || (0x1680 == unicode_id)
        || (0x180E == unicode_id) || (0x2028 == unicode_id) || (0x205F == unicode_id) || (0x3000 == unicode_id))
    {
        return N;
    }
    /*S or SS*/
    else if ((0x0009 == unicode_id) || (0x000B == unicode_id) || (0x001F == unicode_id))
    {
        return N;
    }
    /*ON*/
    else if (((0x0026 <= unicode_id) && (0x002A >= unicode_id)) || ((0x003B <= unicode_id) && (0x0040 >= unicode_id)) || ((0x005B <= unicode_id) && (0x0060 >= unicode_id))
             || ((0x007B <= unicode_id) && (0x007E >= unicode_id)) || ((0x00A6 <= unicode_id) && (0x00A9 >= unicode_id)) || ((0x00B6 <= unicode_id) && (0x00B8 >= unicode_id))
             || ((0x00BB <= unicode_id) && (0x00BF >= unicode_id)) || ((0x02C2 <= unicode_id) && (0x02CF >= unicode_id)) || ((0x02D2 <= unicode_id) && (0x02DF >= unicode_id))
             || ((0x02E5 <= unicode_id) && (0x02ED >= unicode_id)) || ((0x07F6 <= unicode_id) && (0x07F9 >= unicode_id)) || ((0x0BF3 <= unicode_id) && (0x0BF8 >= unicode_id))
             || ((0x0F3A <= unicode_id) && (0x0F3D >= unicode_id)) || ((0x1390 <= unicode_id) && (0x1399 >= unicode_id)) || ((0x17F0 <= unicode_id) && (0x180A >= unicode_id))
             || ((0x19DE <= unicode_id) && (0x19FF >= unicode_id)) || ((0x1FBF <= unicode_id) && (0x1FC1 >= unicode_id)) || ((0x1FCD <= unicode_id) && (0x1FCF >= unicode_id))
             || ((0x1FDD <= unicode_id) && (0x1FDF >= unicode_id)) || ((0x1FED <= unicode_id) && (0x1FEF >= unicode_id)) || ((0x2010 <= unicode_id) && (0x2027 >= unicode_id))
             || ((0x2035 <= unicode_id) && (0x2043 >= unicode_id)) || ((0x2045 <= unicode_id) && (0x205E >= unicode_id)) || ((0x207C <= unicode_id) && (0x207E >= unicode_id))
             || ((0x208C <= unicode_id) && (0x208E >= unicode_id)) || ((0x2103 <= unicode_id) && (0x2106 >= unicode_id)) || ((0x2116 <= unicode_id) && (0x2118 >= unicode_id))
             || ((0x211E <= unicode_id) && (0x2123 >= unicode_id)) || ((0x2140 <= unicode_id) && (0x2144 >= unicode_id)) || ((0x214A <= unicode_id) && (0x214D >= unicode_id))
             || ((0x2153 <= unicode_id) && (0x215F >= unicode_id)) || ((0x2190 <= unicode_id) && (0x2211 >= unicode_id)) || ((0x2214 <= unicode_id) && (0x2335 >= unicode_id))
             || ((0x237B <= unicode_id) && (0x2394 >= unicode_id)) || ((0x2396 <= unicode_id) && (0x23E7 >= unicode_id)) || ((0x2400 <= unicode_id) && (0x2426 >= unicode_id))
             || ((0x2440 <= unicode_id) && (0x244A >= unicode_id)) || ((0x2460 <= unicode_id) && (0x2487 >= unicode_id)) || ((0x24EA <= unicode_id) && (0x269C >= unicode_id))
             || ((0x26A0 <= unicode_id) && (0x2752 >= unicode_id)) || ((0x2758 <= unicode_id) && (0x275E >= unicode_id)) || ((0x2761 <= unicode_id) && (0x27CA >= unicode_id))
             || ((0x27D0 <= unicode_id) && (0x27EB >= unicode_id)) || ((0x27F0 <= unicode_id) && (0x27FF >= unicode_id)) || ((0x2900 <= unicode_id) && (0x2CFF >= unicode_id))
             || ((0x2E00 <= unicode_id) && (0x2E17 >= unicode_id)) || ((0x2E80 <= unicode_id) && (0x2EF3 >= unicode_id)) || ((0x2F00 <= unicode_id) && (0x2FD5 >= unicode_id))
             || ((0x2FF0 <= unicode_id) && (0x3020 >= unicode_id)) || ((0x303D <= unicode_id) && (0x303F >= unicode_id)) || ((0x31C0 <= unicode_id) && (0x31CF >= unicode_id))
             || ((0x3250 <= unicode_id) && (0x325F >= unicode_id)) || ((0x327C <= unicode_id) && (0x327E >= unicode_id)) || ((0x32B1 <= unicode_id) && (0x32BF >= unicode_id))
             || ((0x32CC <= unicode_id) && (0x32CF >= unicode_id)) || ((0x3377 <= unicode_id) && (0x337A >= unicode_id)) || ((0x4DC0 <= unicode_id) && (0x4DFF >= unicode_id))
             || ((0xA490 <= unicode_id) && (0xA4C6 >= unicode_id)) || ((0xA700 <= unicode_id) && (0xA721 >= unicode_id)) || ((0xA828 <= unicode_id) && (0xA82B >= unicode_id))
             || ((0xA874 <= unicode_id) && (0xA877 >= unicode_id)) || ((0xFE10 <= unicode_id) && (0xFE19 >= unicode_id)) || ((0xFE30 <= unicode_id) && (0xFE51 >= unicode_id))
             || ((0xFE54 <= unicode_id) && (0xFE5E >= unicode_id)) || ((0xFE64 <= unicode_id) && (0xFE66 >= unicode_id)) || ((0xFF06 <= unicode_id) && (0xFF0A >= unicode_id))
             || ((0xFF1B <= unicode_id) && (0xFF20 >= unicode_id)) || ((0xFF3B <= unicode_id) && (0xFF40 >= unicode_id)) || ((0xFF5B <= unicode_id) && (0xFF65 >= unicode_id))
             || ((0xFFE2 <= unicode_id) && (0xFFE4 >= unicode_id)) || ((0xFFE8 <= unicode_id) && (0xFFEE >= unicode_id)) || ((0xFFF9 <= unicode_id) && (0xFFFD >= unicode_id)))
    {
        return N;
    }
    else if ((0x0021 == unicode_id) || (0x0022 == unicode_id) || (0x0060 == unicode_id) || (0x00A1 == unicode_id) || (0x00AB == unicode_id) || (0x00AC == unicode_id)
             || (0x00AE == unicode_id) || (0x00AF == unicode_id) || (0x00B4 == unicode_id) || (0x00D7 == unicode_id) || (0x00F7 == unicode_id) || (0x02B9 == unicode_id)
             || (0x02BA == unicode_id) || (0x02EF == unicode_id) || (0x02FF == unicode_id) || (0x0304 == unicode_id) || (0x0374 == unicode_id) || (0x0375 == unicode_id)
             || (0x037E == unicode_id) || (0x0384 == unicode_id) || (0x0385 == unicode_id) || (0x0387 == unicode_id) || (0x03F6 == unicode_id) || (0x058A == unicode_id)
             || (0x060E == unicode_id) || (0x060F == unicode_id) || (0x06E9 == unicode_id) || (0x0BFA == unicode_id) || (0x0CF1 == unicode_id) || (0x0CF2 == unicode_id)
             || (0x104A == unicode_id) || (0x169B == unicode_id) || (0x169C == unicode_id) || (0x1940 == unicode_id) || (0x1944 == unicode_id) || (0x1945 == unicode_id)
             || (0x1FBD == unicode_id) || (0x1FFD == unicode_id) || (0x1FFE == unicode_id) || (0x2061 == unicode_id) || (0x2100 == unicode_id) || (0x2101 == unicode_id)
             || (0x2108 == unicode_id) || (0x2109 == unicode_id) || (0x2114 == unicode_id) || (0x2125 == unicode_id) || (0x2127 == unicode_id) || (0x2129 == unicode_id)
             || (0x213A == unicode_id) || (0x213B == unicode_id) || (0x2756 == unicode_id) || (0x2D03 == unicode_id) || (0x2D0D == unicode_id) || (0x2E1C == unicode_id)
             || (0x2E1D == unicode_id) || (0x3030 == unicode_id) || (0x3036 == unicode_id) || (0x3037 == unicode_id) || (0x309B == unicode_id) || (0x309C == unicode_id)
             || (0x30A0 == unicode_id) || (0x30FB == unicode_id) || (0x321D == unicode_id) || (0x321E == unicode_id) || (0x0009 == unicode_id) || (0x33DE == unicode_id)
             || (0x33DF == unicode_id) || (0x33FF == unicode_id) || (0xFD3E == unicode_id) || (0xFD3F == unicode_id) || (0xFDFD == unicode_id) || (0xFE60 == unicode_id)
             || (0xFE61 == unicode_id) || (0xFE68 == unicode_id) || (0xFE6B == unicode_id) || (0xFF01 == unicode_id) || (0xFF02 == unicode_id))
    {
        return N;
    }
    /*BS or B*/
    else if (((0x001C <= unicode_id) && (0x001E >= unicode_id)) || (0x000A == unicode_id) || (0x0085 == unicode_id) || (0x6138 == unicode_id))
    {
        return B;
    }
    /*CS*/
    else if ((0x002C == unicode_id) || (0x002E == unicode_id) || (0x002F == unicode_id) || (0x003A == unicode_id) || (0x00A0 == unicode_id) || (0x060C == unicode_id)
             || (0x202F == unicode_id) || (0x2044 == unicode_id) || (0xFE50 == unicode_id) || (0xFE52 == unicode_id) || (0xFE55 == unicode_id) || (0xFF0C == unicode_id)
             || (0xFF0E == unicode_id) || (0xFF0F == unicode_id) || (0xFF1A == unicode_id))
    {
        return CS;
    }
    /*ES*/
    else if ((0x002B == unicode_id) || (0x002D == unicode_id) || (0x207A == unicode_id) || (0x207B == unicode_id) || (0x208A == unicode_id) || (0x208B == unicode_id)
             || (0x2212 == unicode_id) || (0xFB29 == unicode_id) || (0xFE62 == unicode_id) || (0xFE63 == unicode_id) || (0xFF0B == unicode_id) || (0xFF0D == unicode_id))
    {
        return ES;
    }
    /*RLO*/
    else if (0x202E == unicode_id)
    {
        return RLO;
    }
    /*LRO*/
    else if (0x202D == unicode_id)
    {
        return LRO;
    }
    /*RLE*/
    else if (0x202B == unicode_id)
    {
        return RLE;
    }
    /*LRE*/
    else if (0x202A == unicode_id)
    {
        return LRE;
    }
    /*PDF*/
    else if (0x202C == unicode_id)
    {
        return PDF;
    }
    /*NSM*/
    else if (((0x0300 <= unicode_id) && (0x036F >= unicode_id)) || ((0x0483 <= unicode_id) && (0x0489 >= unicode_id)) || ((0x0591 <= unicode_id) && (0x05BD >= unicode_id))
             || ((0x0610 <= unicode_id) && (0x0615 >= unicode_id)) || ((0x064B <= unicode_id) && (0x065E >= unicode_id)) || ((0x06D6 <= unicode_id) && (0x06DC >= unicode_id))
             || ((0x06DE <= unicode_id) && (0x06E4 >= unicode_id)) || ((0x06EA <= unicode_id) && (0x06ED >= unicode_id)) || ((0x0730 <= unicode_id) && (0x074A >= unicode_id))
             || ((0x07A6 <= unicode_id) && (0x07B0 >= unicode_id)) || ((0x07EB <= unicode_id) && (0x07F3 >= unicode_id)) || ((0x0941 <= unicode_id) && (0x0948 >= unicode_id))
             || ((0x0951 <= unicode_id) && (0x0954 >= unicode_id)) || ((0x09C1 <= unicode_id) && (0x09C4 >= unicode_id)) || ((0x0A4B <= unicode_id) && (0x0A4D >= unicode_id))
             || ((0x0AC1 <= unicode_id) && (0x0AC5 >= unicode_id)) || ((0x0B41 <= unicode_id) && (0x0B43 >= unicode_id)) || ((0x0C3E <= unicode_id) && (0x0C40 >= unicode_id))
             || ((0x0C46 <= unicode_id) && (0x0C48 >= unicode_id)) || ((0x0C4A <= unicode_id) && (0x0C4D >= unicode_id)) || ((0x0D41 <= unicode_id) && (0x0D43 >= unicode_id))
             || ((0x0DD2 <= unicode_id) && (0x0DD4 >= unicode_id)) || ((0x0E34 <= unicode_id) && (0x0E3A >= unicode_id)) || ((0x0E47 <= unicode_id) && (0x0E4E >= unicode_id))
             || ((0x0EB4 <= unicode_id) && (0x0EB9 >= unicode_id)) || ((0x0EC8 <= unicode_id) && (0x0ECD >= unicode_id)) || ((0x0F71 <= unicode_id) && (0x0F7E >= unicode_id))
             || ((0x0F80 <= unicode_id) && (0x0F84 >= unicode_id)) || ((0x0F90 <= unicode_id) && (0x0F97 >= unicode_id)) || ((0x0F99 <= unicode_id) && (0x0FBC >= unicode_id))
             || ((0x102D <= unicode_id) && (0x1030 >= unicode_id)) || ((0x1712 <= unicode_id) && (0x1714 >= unicode_id)) || ((0x1732 <= unicode_id) && (0x1734 >= unicode_id))
             || ((0x17B7 <= unicode_id) && (0x17BD >= unicode_id)) || ((0x17C9 <= unicode_id) && (0x17D3 >= unicode_id)) || ((0x180B <= unicode_id) && (0x180D >= unicode_id))
             || ((0x1920 <= unicode_id) && (0x1922 >= unicode_id)) || ((0x1927 <= unicode_id) && (0x192B >= unicode_id)) || ((0x1939 <= unicode_id) && (0x193B >= unicode_id))
             || ((0x1B00 <= unicode_id) && (0x1B03 >= unicode_id)) || ((0x1B36 <= unicode_id) && (0x1B3A >= unicode_id)) || ((0x1B6B <= unicode_id) && (0x1B73 >= unicode_id))
             || ((0x1DC0 <= unicode_id) && (0x1DCA >= unicode_id)) || ((0x20D0 <= unicode_id) && (0x20EF >= unicode_id)) || ((0x302A <= unicode_id) && (0x302F >= unicode_id))
             || ((0xFE00 <= unicode_id) && (0xFE0F >= unicode_id)) || ((0xFE20 <= unicode_id) && (0xFE23 >= unicode_id)))
    {
        return NSM;
    }

    else if ((0x05BF == unicode_id) || (0x05C1 == unicode_id) || (0x05C2 == unicode_id) || (0x05C4 == unicode_id) || (0x05C5 == unicode_id) || (0x05C7 == unicode_id)
             || (0x0670 == unicode_id) || (0x06E7 == unicode_id) || (0x06E8 == unicode_id) || (0x0711 == unicode_id) || (0x0901 == unicode_id) || (0x0902 == unicode_id)
             || (0x093C == unicode_id) || (0x094D == unicode_id) || (0x0962 == unicode_id) || (0x0963 == unicode_id) || (0x0981 == unicode_id) || (0x09BC == unicode_id)
             || (0x09CD == unicode_id) || (0x09E2 == unicode_id) || (0x09E3 == unicode_id) || (0x0A01 == unicode_id) || (0x0A02 == unicode_id) || (0x0A3C == unicode_id)
             || (0x0A41 == unicode_id) || (0x0A42 == unicode_id) || (0x0A47 == unicode_id) || (0x0A48 == unicode_id) || (0x0A70 == unicode_id) || (0x0A71 == unicode_id)
             || (0x0A81 == unicode_id) || (0x0A82 == unicode_id) || (0x0ABC == unicode_id) || (0x0AC7 == unicode_id) || (0x0AC8 == unicode_id) || (0x0ACD == unicode_id)
             || (0x0AE2 == unicode_id) || (0x0AE3 == unicode_id) || (0x0B01 == unicode_id) || (0x0B3C == unicode_id) || (0x0B3F == unicode_id) || (0x0B4D == unicode_id)
             || (0x0B56 == unicode_id) || (0x0B82 == unicode_id) || (0x0BC0 == unicode_id) || (0x0BCD == unicode_id) || (0x0C55 == unicode_id) || (0x0C56 == unicode_id)
             || (0x0CBC == unicode_id) || (0x0CCC == unicode_id) || (0x0CCD == unicode_id) || (0x0CE2 == unicode_id) || (0x0CE3 == unicode_id) || (0x0D4D == unicode_id)
             || (0x0DCA == unicode_id) || (0x0DD6 == unicode_id) || (0x0E31 == unicode_id) || (0x0EB1 == unicode_id) || (0x0EBB == unicode_id) || (0x0EBC == unicode_id)
             || (0x0F18 == unicode_id) || (0x0F19 == unicode_id) || (0x0F35 == unicode_id) || (0x0F37 == unicode_id) || (0x0F39 == unicode_id) || (0x0F86 == unicode_id)
             || (0x0F87 == unicode_id) || (0x0FC6 == unicode_id) || (0x1032 == unicode_id) || (0x1036 == unicode_id) || (0x1037 == unicode_id) || (0x1039 == unicode_id)
             || (0x1058 == unicode_id) || (0x1059 == unicode_id) || (0x135F == unicode_id) || (0x1752 == unicode_id) || (0x1753 == unicode_id) || (0x1772 == unicode_id)
             || (0x1773 == unicode_id) || (0x17C6 == unicode_id) || (0x17DD == unicode_id) || (0x18A9 == unicode_id) || (0x1932 == unicode_id) || (0x1A17 == unicode_id)
             || (0x1A18 == unicode_id) || (0x1B34 == unicode_id) || (0x1B3C == unicode_id) || (0x1B42 == unicode_id) || (0x1DFE == unicode_id) || (0x1DFF == unicode_id)
             || (0x3099 == unicode_id) || (0x309A == unicode_id) || (0xA802 == unicode_id) || (0xA806 == unicode_id) || (0xA808 == unicode_id) || (0xA825 == unicode_id)
             || (0xA826 == unicode_id) || (0xFB1E == unicode_id))
    {
        return NSM;
    }
    /*ET*/
    else if (((0x0023 <= unicode_id) && (0x0025 >= unicode_id)) || ((0x00A2 <= unicode_id) && (0x00A5 >= unicode_id)) || ((0x2030 <= unicode_id) && (0x2034 >= unicode_id))
             || ((0x20A0 <= unicode_id) && (0x20B5 >= unicode_id)) || ((0xFF03 <= unicode_id) && (0xFF05 >= unicode_id)) || (0x00B0 == unicode_id) || (0x00B1 == unicode_id)
             || (0x066A == unicode_id) || (0x09F2 == unicode_id) || (0x09F3 == unicode_id) || (0x0AF1 == unicode_id) || (0x0BF9 == unicode_id) || (0x0E3F == unicode_id)
             || (0x17DB == unicode_id) || (0x212E == unicode_id) || (0x2213 == unicode_id) || (0xFE5F == unicode_id) || (0xFE69 == unicode_id) || (0xFE6A == unicode_id)
             || (0xFFE0 == unicode_id) || (0xFFE1 == unicode_id) || (0xFFE5 == unicode_id) || (0xFFE6 == unicode_id))
    {
        return ET;
    }
    /*EN*/
    else if (((0x0030 <= unicode_id) && (0x0039 >= unicode_id)) || ((0x06F0 <= unicode_id) && (0x06F9 >= unicode_id)) || ((0x2074 <= unicode_id) && (0x2079 >= unicode_id))
             || ((0x2080 <= unicode_id) && (0x2089 >= unicode_id)) || ((0x2488 <= unicode_id) && (0x249B >= unicode_id)) || ((0xFF10 <= unicode_id) && (0xFF19 >= unicode_id))
             || (0x00B2 == unicode_id) || (0x00B3 == unicode_id) || (0x00B9 == unicode_id) || (0x2070 == unicode_id))
    {
        return EN;
    }
    /*AN*/
    else if (((0x0660 <= unicode_id) && (0x0669 >= unicode_id)) || (0x066B == unicode_id) || (0x066C == unicode_id))
    {
        return AN;
    }
    /*BN*/
    else if (((0x0000 <= unicode_id) && (0x0008 >= unicode_id)) || ((0x000E <= unicode_id) && (0x001B >= unicode_id)) || ((0x007F <= unicode_id) && (0x0084 >= unicode_id))
             || ((0x0086 <= unicode_id) && (0x009F >= unicode_id)) || ((0x200B <= unicode_id) && (0x200D >= unicode_id)) || ((0x2060 <= unicode_id) && (0x2063 >= unicode_id))
             || ((0x206A <= unicode_id) && (0x206F >= unicode_id)) || (0x00AD == unicode_id) || (0x070F == unicode_id) || (0xFEFF == unicode_id))
    {
        return BN;
    }
    /*AL*/
    else if (((0x0600 <= unicode_id) && (0x0603 >= unicode_id)) || ((0x061E <= unicode_id) && (0x061F >= unicode_id)) || ((0x0621 <= unicode_id) && (0x063A >= unicode_id))
             || ((0x0640 <= unicode_id) && (0x064A >= unicode_id)) || ((0x066D <= unicode_id) && (0x066F >= unicode_id)) || ((0x0671 <= unicode_id) && (0x06D5 >= unicode_id))
             || ((0x06FA <= unicode_id) && (0x070D >= unicode_id)) || ((0x0712 <= unicode_id) && (0x072F >= unicode_id)) || ((0x074D <= unicode_id) && (0x076D >= unicode_id)) || ((0x0780 <= unicode_id) && (0x07A5 >= unicode_id)) || ((0xFB50 <= unicode_id) && (0xFBB1 >= unicode_id))
             || ((0xFBD3 <= unicode_id) && (0xFD3D >= unicode_id)) || ((0xFD50 <= unicode_id) && (0xFD8F >= unicode_id)) || ((0xFD92 <= unicode_id) && (0xFDC7 >= unicode_id)) || ((0xFDF0 <= unicode_id) && (0xFDFC >= unicode_id))
             || ((0xFE70 <= unicode_id) && (0xFE74 >= unicode_id)) || ((0xFE76 <= unicode_id) && (0xFEFC >= unicode_id)) || (0x060B == unicode_id) || (0x060D == unicode_id)
             || (0x06DD == unicode_id) || (0x06E5 == unicode_id) || (0x06E6 == unicode_id) || (0x06EE == unicode_id) || (0x06EF == unicode_id)
             || (0x07B1 == unicode_id) || (0xFFE8 == unicode_id) || (0x061B == unicode_id) || (0x0710 == unicode_id))
    {
        return AL;
    }
    /*R or RTL*/
    else if (((0x05D0 <= unicode_id) && (0x05EA >= unicode_id)) || ((0x05F0 <= unicode_id) && (0x05F4 >= unicode_id)) || ((0x07C0 <= unicode_id) && (0x07EA >= unicode_id))
             || ((0xFB1F <= unicode_id) && (0xFB36 >= unicode_id)) || ((0xFB38 <= unicode_id) && (0xFB3C >= unicode_id)) || ((0xFB46 <= unicode_id) && (0xFB4F >= unicode_id)) || (0x05BE == unicode_id) || (0x05C0 == unicode_id)
             || (0x05C3 == unicode_id) || (0x05C6 == unicode_id) || (0x07F4 == unicode_id) || (0x07F5 == unicode_id) || (0x07FA == unicode_id) || (0x200F == unicode_id)
             || (0xFB1D == unicode_id) || (0xFB3E == unicode_id) || (0xFB40 == unicode_id) || (0xFB41 == unicode_id) || (0xFB43 == unicode_id) || (0xFB44 == unicode_id))
    {
        return R;
    }
    /*L or LTR*/
    else
    {
        return L;
    }
}

// === HELPER FUNCTIONS ================================================

// reverse cch characters
static void reverse(__u16 *psz, __u32 cch)
{
    __u16 chTemp;
    __u32 ich;

    for (ich = 0; ich < --cch; ich++)
    {
        chTemp = psz[ich];
        psz[ich] = psz[cch];
        psz[cch] = chTemp;
    }
}

// Set a run of cval values at locations all prior to, but not including
// iStart, to the new value nval.
static void SetDeferredRun(__u16 *pval, __u32 cval, __u32 iStart, __u16 nval)
{
    __s32 i, temp;
    temp = iStart - cval;

    if (iStart < 1)
    {
        return;
    }

    for (i = iStart - 1; i >= temp; i--)
    {
        pval[i] = nval;
    }
}

// === THE PARAGRAPH LEVEL ===============================================

/*------------------------------------------------------------------------
    Function: resolveParagraphs

    Resolves the input strings into blocks over which the algorithm
    is then applied.

    Implements Rule P1 of the unicode_id Bidi Algorithm

    Input: Text string
           Character count

    Output: revised character count

    Note:   This is a very simplistic function. In effect it restricts
            the action of the algorithm to the first paragraph in the input
            where a paragraph ends at the end of the first block separator
            or at the end of the input text.

------------------------------------------------------------------------*/

static __u32 resolveParagraphs(__u16 *types, __u32 cch)     //__s32
{
    __u32 ich;
    // skip characters not of type B
    for (ich = 0; (ich < cch) && (types[ich] != B); ich++)
    {
        ;
    }
    // stop after first B, make it a BN for use in the next steps
    if (ich < cch && types[ich] == B)
    {
        types[ich++] = BN;
    }
    return ich;
}

/*------------------------------------------------------------------------
    Function: baseLevel

    Determines the base level

    Implements rule P2 of the unicode_id Bidi Algorithm.

    Input: Array of directional classes
           Character count

    Note: Ignores explicit embeddings
------------------------------------------------------------------------*/
static __u16 baseLevel(const __u16 *pcls,  __u32 cch)
{
    __u32 ich;
    for (ich = 0; ich < cch; ich++)
    {
        switch (pcls[ich])
        {
            // strong left
            case L:
                return 0;
                break;

            // strong right
            case R:
            case AL:
                return 1;
                break;
        }
    }
    return 0;
}

//====== RESOLVE EXPLICIT ================================================

static __u16 GreaterEven(__u16 i)
{
    return odd(i) ? i + 1 : i + 2;
}

static __u16 GreaterOdd(__u16 i)
{
    return odd(i) ? i + 2 : i + 1;
}

static __u16 EmbeddingDirection(__u16 level)
{
    return odd(level) ? R : L;
}

/*------------------------------------------------------------------------
    Function: resolveExplicit

    Recursively resolves explicit embedding levels and overrides.
    Implements rules X1-X9, of the unicode_id Bidirectional Algorithm.

    Input: Base embedding level and direction
           Character count

    Output: Array of embedding levels
          Caller must allocate (one level per input character)

    In/Out: Array of direction classes


    Note: The function uses two simple counters to keep track of
          matching explicit codes and PDF. Use the default argument for
          the outermost call. The nesting counter counts the recursion
          depth and not the embedding level.
------------------------------------------------------------------------*/

static __u32 resolveExplicit(__u16 level, __s32 dir, __u16 *pcls, __u16 *plevel, __u32 cch, __s32 nNest)
{
    // always called with a valid nesting level
    // nesting levels are != embedding levels
    __u32 ich;
    __s32 nLastValid;
    __u16 cls;

    nLastValid = nNest;

    // check input values
    __ASSERT(nNest >= 0 && level >= 0 && level <= MAX_LEVEL);

    // process the text
    for (ich = 0; ich < cch; ich++)
    {
        cls = pcls[ich];
        switch (cls)
        {
            case LRO:
            case LRE:
                nNest++;
                if (GreaterEven(level) <= MAX_LEVEL)
                {
                    plevel[ich] = GreaterEven(level);
                    pcls[ich] = BN;
                    ich += resolveExplicit(plevel[ich], (cls == LRE ? N : L),
                                           &pcls[ich + 1], &plevel[ich + 1],
                                           cch - (ich + 1), nNest);
                    nNest--;
                    continue;
                }
                cls = pcls[ich] = BN;
                break;

            case RLO:
            case RLE:
                nNest++;
                if (GreaterOdd(level) <= MAX_LEVEL)
                {
                    plevel[ich] = GreaterOdd(level);
                    pcls[ich] = BN;
                    ich += resolveExplicit(plevel[ich], (cls == RLE ? N : R),
                                           &pcls[ich + 1], &plevel[ich + 1],
                                           cch - (ich + 1), nNest);
                    nNest--;
                    continue;
                }
                cls = pcls[ich] = BN;
                break;

            case PDF:
                cls = pcls[ich] = BN;
                if (nNest)
                {
                    if (nLastValid < nNest)
                    {
                        nNest--;
                    }
                    else
                    {
                        cch = ich; // break the loop, but complete body
                    }
                }
                break;
        }

        // Apply the override
        if (dir != N)
        {
            cls = dir;
        }
        plevel[ich] = level;
        if (pcls[ich] != BN)
        {
            pcls[ich] = cls;
        }
    }

    return ich;
}

// === RESOLVE WEAK TYPES ================================================

enum bidi_state // possible states
{
    xa,     //  arabic letter
    xr,     //  right leter
    xl,     //  left letter

    ao,     //  arabic lett. foll by ON
    ro,     //  right lett. foll by ON
    lo,     //  left lett. foll by ON

    rt,     //  ET following R
    lt,     //  ET following L

    cn,     //  EN, AN following AL
    ra,     //  arabic number foll R
    re,     //  european number foll R
    la,     //  arabic number foll L
    le,     //  european number foll L

    ac,     //  CS following cn
    rc,     //  CS following ra
    rs,     //  CS,ES following re
    lc,     //  CS following la
    ls,     //  CS,ES following le

    ret,    //  ET following re
    let,    //  ET following le
} ;

__u16 stateWeak[][10] =
{
    //  N,  L,  R,  AN, EN, AL,NSM, CS, ES, ET,
    /*xa*/  ao, xl, xr, cn, cn, xa, xa, ao, ao, ao, /* arabic letter          */
    /*xr*/  ro, xl, xr, ra, re, xa, xr, ro, ro, rt, /* right letter           */
    /*xl*/  lo, xl, xr, la, le, xa, xl, lo, lo, lt, /* left letter            */

    /*ao*/  ao, xl, xr, cn, cn, xa, ao, ao, ao, ao, /* arabic lett. foll by ON*/
    /*ro*/  ro, xl, xr, ra, re, xa, ro, ro, ro, rt, /* right lett. foll by ON */
    /*lo*/  lo, xl, xr, la, le, xa, lo, lo, lo, lt, /* left lett. foll by ON  */

    /*rt*/  ro, xl, xr, ra, re, xa, rt, ro, ro, rt, /* ET following R         */
    /*lt*/  lo, xl, xr, la, le, xa, lt, lo, lo, lt, /* ET following L         */

    /*cn*/  ao, xl, xr, cn, cn, xa, cn, ac, ao, ao, /* EN, AN following AL    */
    /*ra*/  ro, xl, xr, ra, re, xa, ra, rc, ro, rt, /* arabic number foll R   */
    /*re*/  ro, xl, xr, ra, re, xa, re, rs, rs, ret, /* european number foll R */
    /*la*/  lo, xl, xr, la, le, xa, la, lc, lo, lt, /* arabic number foll L   */
    /*le*/  lo, xl, xr, la, le, xa, le, ls, ls, let, /* european number foll L */

    /*ac*/  ao, xl, xr, cn, cn, xa, ao, ao, ao, ao, /* CS following cn        */
    /*rc*/  ro, xl, xr, ra, re, xa, ro, ro, ro, rt, /* CS following ra        */
    /*rs*/  ro, xl, xr, ra, re, xa, ro, ro, ro, rt, /* CS,ES following re     */
    /*lc*/  lo, xl, xr, la, le, xa, lo, lo, lo, lt, /* CS following la        */
    /*ls*/  lo, xl, xr, la, le, xa, lo, lo, lo, lt, /* CS,ES following le     */

    /*ret*/ ro, xl, xr, ra, re, xa, ret, ro, ro, ret, /* ET following re        */
    /*let*/ lo, xl, xr, la, le, xa, let, lo, lo, let, /* ET following le        */


};

enum bidi_action // possible actions
{
    // primitives
    IX = 0x100,                 // increment
    XX = 0xF,                   // no-op

    // actions
    xxx = (XX << 4) + XX,       // no-op
    xIx = IX + xxx,             // increment run
    xxN = (XX << 4) + ON,       // set current to N
    xxE = (XX << 4) + EN,       // set current to EN
    xxA = (XX << 4) + AN,       // set current to AN
    xxR = (XX << 4) + R,        // set current to R
    xxL = (XX << 4) + L,        // set current to L
    Nxx = (ON << 4) + 0xF,      // set run to neutral
    Axx = (AN << 4) + 0xF,      // set run to AN
    ExE = (EN << 4) + EN,       // set run to EN, set current to EN
    NIx = (ON << 4) + 0xF + IX, // set run to N, increment
    NxN = (ON << 4) + ON,       // set run to N, set current to N
    NxR = (ON << 4) + R,        // set run to N, set current to R
    NxE = (ON << 4) + EN,       // set run to N, set current to EN

    AxA = (AN << 4) + AN,       // set run to AN, set current to AN
    NxL = (ON << 4) + L,        // set run to N, set current to L
    LxL = (L << 4) + L,         // set run to L, set current to L
};


__u16 actionWeak[][10] =
{
    //   N,.. L,   R,  AN,  EN,  AL, NSM,  CS,..ES,  ET,
    /*xa*/ xxx, xxx, xxx, xxx, xxA, xxR, xxR, xxN, xxN, xxN, /* arabic letter           */
    /*xr*/ xxx, xxx, xxx, xxx, xxE, xxR, xxR, xxN, xxN, xIx, /* right leter             */
    /*xl*/ xxx, xxx, xxx, xxx, xxL, xxR, xxL, xxN, xxN, xIx, /* left letter             */

    /*ao*/ xxx, xxx, xxx, xxx, xxA, xxR, xxN, xxN, xxN, xxN, /* arabic lett. foll by ON */
    /*ro*/ xxx, xxx, xxx, xxx, xxE, xxR, xxN, xxN, xxN, xIx, /* right lett. foll by ON  */
    /*lo*/ xxx, xxx, xxx, xxx, xxL, xxR, xxN, xxN, xxN, xIx, /* left lett. foll by ON   */

    /*rt*/ Nxx, Nxx, Nxx, Nxx, ExE, NxR, xIx, NxN, NxN, xIx, /* ET following R          */
    /*lt*/ Nxx, Nxx, Nxx, Nxx, LxL, NxR, xIx, NxN, NxN, xIx, /* ET following L          */

    /*cn*/ xxx, xxx, xxx, xxx, xxA, xxR, xxA, xIx, xxN, xxN, /* EN, AN following  AL    */
    /*ra*/ xxx, xxx, xxx, xxx, xxE, xxR, xxA, xIx, xxN, xIx, /* arabic number foll R    */
    /*re*/ xxx, xxx, xxx, xxx, xxE, xxR, xxE, xIx, xIx, xxE, /* european number foll R  */
    /*la*/ xxx, xxx, xxx, xxx, xxL, xxR, xxA, xIx, xxN, xIx, /* arabic number foll L    */
    /*le*/ xxx, xxx, xxx, xxx, xxL, xxR, xxL, xIx, xIx, xxL, /* european number foll L  */

    /*ac*/ Nxx, Nxx, Nxx, Axx, AxA, NxR, NxN, NxN, NxN, NxN, /* CS following cn         */
    /*rc*/ Nxx, Nxx, Nxx, Axx, NxE, NxR, NxN, NxN, NxN, NIx, /* CS following ra         */
    /*rs*/ Nxx, Nxx, Nxx, Nxx, ExE, NxR, NxN, NxN, NxN, NIx, /* CS,ES following re      */
    /*lc*/ Nxx, Nxx, Nxx, Axx, NxL, NxR, NxN, NxN, NxN, NIx, /* CS following la         */
    /*ls*/ Nxx, Nxx, Nxx, Nxx, LxL, NxR, NxN, NxN, NxN, NIx, /* CS,ES following le      */

    /*ret*/xxx, xxx, xxx, xxx, xxE, xxR, xxE, xxN, xxN, xxE, /* ET following re         */
    /*let*/xxx, xxx, xxx, xxx, xxL, xxR, xxL, xxN, xxN, xxL, /* ET following le         */
};

static __u16 GetDeferredType(__u16 action)
{
    return (action >> 4) & 0xF;
}

static __u16 GetResolvedType(__u16 action)
{
    return action & 0xF;
}

/*------------------------------------------------------------------------
    Function: resolveWeak

    Resolves the directionality of numeric and other weak character types

    Implements rules X10 and W1-W6 of the unicode_id Bidirectional Algorithm.

    Input: Array of embedding levels
           Character count

    In/Out: Array of directional classes

    Note: On input only these directional classes are expected
          AL, HL, R, L,  ON, BN, NSM, AN, EN, ES, ET, CS,
------------------------------------------------------------------------*/
static void resolveWeak(__u16 baselevel, __u16 *pcls, __u16 *plevel, __u32 cch)
{
    __u16 action;
    __u16  cls, clsRun, clsNew;

    __u16 level = baselevel;
    __u32 ich, cchRun = 0;

    __u16 state = odd(baselevel) ? xr : xl;

    for (ich = 0; ich < cch; ich++)
    {

        if (pcls[ich] > BN)
        {
            __msg("error: pcls[%d] > BN (%d)\n", ich, pcls[ich]);
        }

        // ignore boundary neutrals
        if (pcls[ich] == BN)
        {
            // must flatten levels unless at a level change;
            plevel[ich] = level;

            // lookahead for level changes
            if (ich + 1 == cch && level != baselevel)
            {
                // have to fixup last BN before end of the loop, since
                // its fix-upped value will be needed below the assert
                pcls[ich] = EmbeddingDirection(level);
            }
            else if (ich + 1 < cch && level != plevel[ich + 1] && pcls[ich + 1] != BN)
            {
                // fixup LAST BN in front / after a level run to make
                // it act like the SOR/EOR in rule X10
                __u16 newlevel = plevel[ich + 1];
                if (level > newlevel)
                {
                    newlevel = level;
                }
                plevel[ich] = newlevel;

                // must match assigned level
                pcls[ich] = EmbeddingDirection(newlevel);
                level = plevel[ich + 1];
            }
            else
            {
                // don't interrupt runs
                if (cchRun)
                {
                    cchRun++;
                }
                continue;
            }
        }

        __ASSERT(pcls[ich] <= BN);
        cls = pcls[ich];

        action = actionWeak[state][cls];

        // resolve the directionality for deferred runs
        clsRun = GetDeferredType(action);
        if (clsRun != XX)
        {
            SetDeferredRun(pcls, cchRun, ich, clsRun);
            cchRun = 0;
        }

        // resolve the directionality class at the current location
        clsNew = GetResolvedType(action);
        if (clsNew != XX)
        {
            pcls[ich] = clsNew;
        }

        // increment a deferred run
        if (IX & action)
        {
            cchRun++;
        }

        state = stateWeak[state][cls];
    }

    // resolve any deferred runs
    // use the direction of the current level to emulate PDF
    cls = EmbeddingDirection(level);

    // resolve the directionality for deferred runs
    clsRun = GetDeferredType(actionWeak[state][cls]);
    if (clsRun != XX)
    {
        SetDeferredRun(pcls, cchRun, ich, clsRun);
    }
}

// === RESOLVE NEUTAL TYPES ==============================================

// action values
enum neutral_action
{
    // action to resolve previous input
    nL = L,         // resolve EN to L
    En = 3 << 4,    // resolve neutrals run to embedding level direction
    Rn = R << 4,    // resolve neutrals run to strong right
    Ln = L << 4,    // resolved neutrals run to strong left
    In = (1 << 8),  // increment count of deferred neutrals
    LnL = (1 << 4) + L, // set run and EN to L
};

static __u16 GetDeferredNeutrals(__u16 action, __u16 level)
{
    action = (action >> 4) & 0xF;
    if (action == (En >> 4))
    {
        return EmbeddingDirection(level);
    }
    else
    {
        return action;
    }
}

static __u16 GetResolvedNeutrals(__u16 action)
{
    action = action & 0xF;
    if (action == In)
    {
        return 0;
    }
    else
    {
        return action;
    }
}

// state values
enum neutral_state
{
    // new temporary class
    r,  // R and characters resolved to R
    l,  // L and characters resolved to L
    rn, // N preceded by right
    ln, // N preceded by left
    a,  // AN preceded by left (the abbrev 'la' is used up above)
    na, // N preceeded by a
} ;


/*------------------------------------------------------------------------
    Notes:

    By rule W7, whenever a EN is 'dominated' by an L (including start of
    run with embedding direction = L) it is resolved to, and further treated
    as L.

    This leads to the need for 'a' and 'na' states.
------------------------------------------------------------------------*/

__u16 actionNeutrals[][5] =
{
    //  N,  L,  R, AN, EN, = cls
    // state =
    In,  0,  0,  0,  0,     // r    right
    In,  0,  0,  0,  L,     // l    left

    In, En, Rn, Rn, Rn,     // rn   N preceded by right
    In, Ln, En, En, LnL,    // ln   N preceded by left

    In,  0,  0,  0,  L,     // a   AN preceded by left
    In, En, Rn, Rn, En,     // na   N  preceded by a
} ;

__u16 stateNeutrals[][5] =
{
    //   N, L,  R,  AN, EN = cls
    // state =
    rn, l,  r,  r,  r,      // r   right
    ln, l,  r,  a,  l,      // l   left

    rn, l,  r,  r,  r,      // rn  N preceded by right
    ln, l,  r,  a,  l,      // ln  N preceded by left

    na, l,  r,  a,  l,      // a  AN preceded by left
    na, l,  r,  a,  l,      // na  N preceded by la
} ;

/*------------------------------------------------------------------------
    Function: resolveNeutrals

    Resolves the directionality of neutral character types.

    Implements rules W7, N1 and N2 of the unicode_id Bidi Algorithm.

    Input: Array of embedding levels
           Character count
           Baselevel

    In/Out: Array of directional classes

    Note: On input only these directional classes are expected
          R,  L,  N, AN, EN and BN

          W8 resolves a number of ENs to L
------------------------------------------------------------------------*/
static void resolveNeutrals(__u16 baselevel, __u16 *pcls, const __u16 *plevel, __u32 cch)
{
    __u16 state, action;
    __u32 cchRun, ich;
    __u16 level, cls, clsRun, clsNew;
    // the state at the start of text depends on the base level
    state = odd(baselevel) ? r : l;

    cchRun = 0;
    level = baselevel;

    for (ich = 0; ich < cch; ich++)
    {
        // ignore boundary neutrals
        if (pcls[ich] == BN)
        {
            // include in the count for a deferred run
            if (cchRun)
            {
                cchRun++;
            }

            // skip any further processing
            continue;
        }

        __ASSERT(pcls[ich] < 5); // "Only N, L, R,  AN, EN are allowed"
        cls = pcls[ich];

        action = actionNeutrals[state][cls];

        // resolve the directionality for deferred runs
        clsRun = GetDeferredNeutrals(action, level);
        if (clsRun != N)
        {
            SetDeferredRun(pcls, cchRun, ich, clsRun);
            cchRun = 0;
        }

        // resolve the directionality class at the current location
        clsNew = GetResolvedNeutrals(action);
        if (clsNew != N)
        {
            pcls[ich] = clsNew;
        }

        if (In & action)
        {
            cchRun++;
        }

        state = stateNeutrals[state][cls];
        level = plevel[ich];
    }

    // resolve any deferred runs
    cls = EmbeddingDirection(level);    // eor has type of current level

    // resolve the directionality for deferred runs
    clsRun = GetDeferredNeutrals(actionNeutrals[state][cls], level);
    if (clsRun != N)
    {
        SetDeferredRun(pcls, cchRun, ich, clsRun);
    }
}

// === RESOLVE IMPLLICIT =================================================

/*------------------------------------------------------------------------
    Function: resolveImplicit

    Recursively resolves implicit embedding levels.
    Implements rules I1 and I2 of the unicode_id Bidirectional Algorithm.

    Input: Array of direction classes
           Character count
           Base level

    In/Out: Array of embedding levels

    Note: levels may exceed 15 on output.
          Accepted subset of direction classes
          R, L, AN, EN
------------------------------------------------------------------------*/
__u16 addLevel[][4] =
{
    // L,  R,   AN, EN = cls
    // level =
    /* even */  0,  1,  2,  2,  // EVEN
    /* odd  */  1,  0,  1,  1,  // ODD

};

static void resolveImplicit(const __u16 *pcls, __u16 *plevel, __u32 cch)
{
    __u32 ich;
    for (ich = 0; ich < cch; ich++)
    {
        // cannot resolve bn here, since some bn were resolved to strong
        // types in resolveWeak. To remove these we need the original
        // types, which are available again in resolveWhiteSpace
        if (pcls[ich] == BN)
        {
            continue;
        }
        __ASSERT(pcls[ich] > 0); // "No Neutrals allowed to survive here."
        __ASSERT(pcls[ich] < 5); // "Out of range."
        plevel[ich] += addLevel[odd(plevel[ich])][pcls[ich] - 1];
    }
}

// === REORDER ===========================================================
/*------------------------------------------------------------------------
    Function: resolveLines

    Breaks a paragraph into lines

    Input:  Character count
            Array of line break flags
    In/Out: Array of characters

    Returns the count of characters on the first line

    Note: This function only breaks lines at hard line breaks. Other
    line breaks can be passed in. If pbrk[n] is true, then a break
    occurs after the character in pszInput[n]. Breaks before the first
    character are not allowed.
------------------------------------------------------------------------*/
static __u32 resolveLines(__u16 *pszInput, __bool *pbrk, __u32 cch)
{
    __u32 ich;
    // skip characters not of type LS
    for (ich = 0; ich < cch; ich++)
    {
        if (pszInput[ich] == chLS || (pbrk && pbrk[ich]))
        {
            ich++;
            break;
        }
    }

    return ich;
}

/*------------------------------------------------------------------------
    Function: resolveWhiteSpace

    Resolves levels for WS and S
    Implements rule L1 of the unicode_id bidi Algorithm.

    Input:  Base embedding level
            Character count
            Array of direction classes (for one line of text)

    In/Out: Array of embedding levels (for one line of text)

    Note: this should be applied a line at a time. The default driver
          code supplied in this file assumes a single line of text; for
          a real implementation, cch and the initial pointer values
          would have to be adjusted.
------------------------------------------------------------------------*/
static void resolveWhitespace(__u16 baselevel, const __u16 *pcls, __u16 *plevel,
                              __u32 cch)
{
    __u32 ich, cchrun = 0;
    __u16 oldlevel = baselevel;

    for (ich = 0; ich < cch; ich++)
    {
        switch (pcls[ich])
        {
            default:
                cchrun = 0; // any other character breaks the run
                break;
            case WS:
                cchrun++;
                break;

            case RLE:
            case LRE:
            case LRO:
            case RLO:
            case PDF:
            case BN:
                plevel[ich] = oldlevel;
                cchrun++;
                break;

            case S:
            case B:
                // reset levels for WS before eot
                SetDeferredRun(plevel, cchrun, ich, baselevel);
                cchrun = 0;
                plevel[ich] = baselevel;
                break;
        }
        oldlevel = plevel[ich];
    }
    // reset level before eot
    SetDeferredRun(plevel, cchrun, ich, baselevel);
}


/*------------------------------------------------------------------------
    Functions: reorder/reorderLevel

    Recursively reorders the display string
    "From the highest level down, reverse all characters at that level and
    higher, down to the lowest odd level"

    Implements rule L2 of the unicode_id bidi Algorithm.

    Input: Array of embedding levels
           Character count
           Flag enabling reversal (set to false by initial caller)

    In/Out: Text to reorder

    Note: levels may exceed 15 resp. 61 on input.

    Rule L3 - reorder combining marks is not implemented here
    Rule L4 - glyph mirroring is implemented as a display option below

    Note: this should be applied a line at a time
-------------------------------------------------------------------------*/
static __u32 reorderLevel(__u16 level, __u16 *pszText, const __u16 *plevel, __u32 cch,
                          __bool fReverse)
{
    __u32 ich;
    // true as soon as first odd level encountered
    fReverse = fReverse || odd(level);

    for (ich = 0; ich < cch; ich++)
    {
        if (plevel[ich] < level)
        {
            break;
        }
        else if (plevel[ich] > level)
        {
            ich += reorderLevel(level + 1, pszText + ich, plevel + ich,
                                cch - ich, fReverse) - 1;
        }
    }
    if (fReverse)
    {
        reverse(pszText, ich);
    }
    return ich;
}

static __u32 reorder(__u16 baselevel, __u16 *pszText, const __u16 *plevel, __u32 cch)
{
    __u32 ich = 0;

    while (ich < cch)
    {
        ich += reorderLevel(baselevel, pszText + ich, plevel + ich,
                            cch - ich, FALSE);
    }
    return ich;
}

// === DISPLAY OPTIONS ================================================
/*-----------------------------------------------------------------------
   Function:    mirror

    Crudely implements rule L4 of the unicode_id Bidirectional Algorithm
    Demonstrate mirrored brackets, braces and parens


    Input:  Array of levels
            Count of characters

    In/Out: Array of characters (should be array of glyph ids)

    Note;
    A full implementation would need to substitute mirrored glyphs even
    for characters that are not paired (e.g. integral sign).
-----------------------------------------------------------------------*/
static void mirror(__u16 *pszInput, const __u16 *plevel, __u32 cch)
{
    __u32 ich;
    for (ich = 0; ich < cch; ich ++)
    {
        if (!odd(plevel[ich]))
        {
            continue;
        }

        if (pszInput[ich] == 0x003C)
        {
            pszInput[ich] = 0x003E;
        }
        else if (pszInput[ich] == 0x003E)
        {
            pszInput[ich] = 0x003C;
        }
        else if (pszInput[ich] == 0x0028)
        {
            pszInput[ich] = 0x0029;
        }
        else if (pszInput[ich] == 0x0029)
        {
            pszInput[ich] = 0x0028;
        }
        else if (pszInput[ich] == 0x005B)
        {
            pszInput[ich] = 0x005D;
        }
        else if (pszInput[ich] == 0x005D)
        {
            pszInput[ich] = 0x005B;
        }
        else if (pszInput[ich] == 0x007B)
        {
            pszInput[ich] = 0x007D;
        }
        else if (pszInput[ich] == 0x007D)
        {
            pszInput[ich] = 0x007B;
        }
    }
}

/*-----------------------------------------------------------------------
    Function: clean

    remove formatting codes

    In/Out: Array of characters
            Count of characters

    Note;

    This function can be used to remove formatting codes so the
    ordering of the string can be compared to implementations that
    remove formatting codes. This implementation is limited to the
    pseudo alphabet used for the demo version.

-----------------------------------------------------------------------*/
static __u32 clean(__u16 *pszInput, __u32 cch)
{
    __u32 ich, cchMove = 0;
    __u16 ch;
    for (ich = 0; ich < cch; ich ++)
    {
        ch = pszInput[ich];
        switch (ch)
        {
            default:
                if (((pszInput[ich] >= 0x000B) && (pszInput[ich] < 0x0020)) || (pszInput[ich] <= 0x0009))
                {
                    cchMove++;
                }
                else
                {
                    pszInput[ich - cchMove] = pszInput[ich];
                }
                break;

            case 0x007F:
                cchMove++;
                break;
        }
    }
    pszInput[ich - cchMove] = 0;

    return ich - cchMove;
}

// === BIDI INTERFACE FUNCTIONS ========================================


/*------------------------------------------------------------------------
    Function: BidiLines

    Implements the Line-by-Line phases of the unicode_id Bidi Algorithm

    Input:   Count of characters
             Flag (Mirror output if true)
             Optional: Array of flags, true for last character on each line

    Inp/Out: Input text
             Array of character directions
             Array of levels

    Note:    See resolveLines for information how this function deals with line breaks
------------------------------------------------------------------------*/
static void BidiLines(__u16 baselevel, __u16 *pszLine, __u16 *pclsLine,
                      __u16 *plevelLine, __u32 cchPara, __bool *pbrk)
{
    __u32 cchLine = 0;

    do
    {
        // break lines at LS
        cchLine = resolveLines(pszLine, pbrk, cchPara);

        // resolve whitespace
        resolveWhitespace(baselevel, pclsLine, plevelLine, cchLine);

        mirror(pszLine, plevelLine, cchLine);

        // reorder each line in place
        reorder(baselevel, pszLine, plevelLine, cchLine);       //cchLine - 1 ;cchLine

        pszLine += cchLine;
        plevelLine += cchLine;
        pbrk += pbrk ? cchLine : 0;
        pclsLine += cchLine;
        cchPara -= cchLine;

    } while (cchPara);
}

/**********************************************************************************************************
//                          get_unicode_id_charater_bidi_type()
// Author:  Derek Lai
// Time:2010.10.26
// Arguments:
// Return:
// Descripion:
// Note:
// History:
***********************************************************************************************************/
static __s32 get_character_bidi_type(__s32 charset, char *inputs, __u16 *pszInput, __u16 *bidi_type)
{
    __s32 charset_len, string_len, i, char_num, temp_string_len;
    __u16 unicode_id_id;
    char *temp_inputs;

    i = 0;
    char_num = 0;
    string_len = strlen(inputs);
    if (string_len <= 0)
    {
        __wrn("the input string length is zero!");
        return EPDK_FALSE;
    }

    /*just for UTF-8 text*/
    //inputs += 3;
    //string_len -= 3;

    temp_inputs = inputs;
    temp_string_len = string_len;

    while (1)
    {
        charset_len = esCHS_Char2Uni(charset, inputs, string_len, &unicode_id_id);
        if (charset_len <= 0)
        {
            break;
        }

        char_num += 1;

        inputs += charset_len;
        string_len -= charset_len;
        if (string_len <= 0)
        {
            break;
        }
    }

    for (i = 0; i < char_num; i++)
    {
        charset_len = esCHS_Char2Uni(charset, temp_inputs, temp_string_len, &unicode_id_id);

        bidi_type[i] = get_unicode_id_charater_bidi_type(unicode_id_id);
        pszInput[i] = unicode_id_id;

        temp_inputs += charset_len;
        temp_string_len -= charset_len;
    }


    /*charset_len = esCHS_Char2Uni(charset, inputs, string_len, &unicode_id_id);
    char_num = string_len / charset_len;
    if(char_num >= MAX_CCH)
    {
        char_num = MAX_CCH;
    }

    for(i = 0; i < char_num; i++)
    {
        charset_len = esCHS_Char2Uni(charset, inputs, string_len, &unicode_id_id);
        if(charset_len <= 0)
        {
            break;
        }

        bidi_type[i] = get_unicode_id_charater_bidi_type(unicode_id_id);
        pszInput[i] = unicode_id_id;

        inputs += charset_len;
        string_len -= charset_len;
    }*/

    pszInput[char_num] = 0;

    return char_num;
}

/**********************************************************************************************************
//                          get_unicode_id_charater_bidi_type()
// Author:  Derek Lai
// Time:2010.10.26
// Arguments:
// Return:
// Descripion:
// Note:
// History:
***********************************************************************************************************/
static __u32 get_paragraph_num(__u16 *bidi_type, __u32 cch)
{
    __u32 ich, parag_num = 1;

    for (ich = 0; ich < cch; ich++)
    {
        if (B == bidi_type[ich])
        {
            if (ich < (cch - 1)) //avoid the last line is null
            {
                parag_num++;
            }
            else
            {
                break;
            }
        }
    }

    return parag_num;
}
/**********************************************************************************************************
//                          get_unicode_id_charater_bidi_type()
// Author:  Derek Lai
// Time:2010.10.26
// Arguments:
// Return:
// Descripion:
// Note:
// History:
***********************************************************************************************************/
static void get_character_bidi_type_ws(__u16 *bidi_type, __u16 *pszInput, __u32 cchPara)
{
    __u32 i;

    for (i = 0; i < cchPara; i++)
    {
        if (((2000 <= pszInput[i]) && (0x200A >= pszInput[i])) || (0x000C == pszInput[i]) || (0x0020 == pszInput[i])
            || (0x1680 == pszInput[i]) || (0x180E == pszInput[i]) || (0x2028 == pszInput[i]) || (0x205F == pszInput[i])
            || (0x3000 == pszInput[i]))
        {
            bidi_type[i] = WS;
        }
        /*S or SS*/
        if ((0x0009 == pszInput[i]) || (0x000B == pszInput[i]) || (0x001F == pszInput[i]))
        {
            bidi_type[i] = S;
        }
    }

}

/*------------------------------------------------------------------------
    Function: BidiParagraph

    Implements the per paragraph phases of the unicode_id Bidi Algorithm

    Input:
             Count of characters

    Inp/Out: Input text
             Base paragraph level (negative value on input means implicit)
             Array of character directions
             Array of levels

    Returns: The number of input characters processed for this paragraph

    Note:    This function is not called by the demo code in this file, but
             provided here for completeness. It has been tested by being
             used in the verfication code.
------------------------------------------------------------------------*/
static __u16 bidi_paragraph(__u16 *types, __u16 *levels, __u32 cchPara, __u16 *pszInput)
{
    __u16 baselevel;
    //__u32 display_cch;
    //__bool  break_line[MAX_CCH] = {FALSE};

    // set baselevel
    baselevel = baseLevel(types, cchPara);

    // change B into BN and return length including
    //int cchPara = resolveParagraphs(types, cch);

    // resolve explicit
    resolveExplicit(baselevel, N, types, levels, cchPara, 0);

    // resolve weak
    resolveWeak(baselevel, types, levels, cchPara);

    // resolve neutrals
    resolveNeutrals(baselevel, types, levels, cchPara);

    // resolveImplicit
    resolveImplicit(types, levels, cchPara);

    // assign directional types again, but for WS, S this time
    get_character_bidi_type_ws(types, pszInput, cchPara);

    BidiLines(baselevel, pszInput, types, levels, cchPara, break_line);

    //display_cch = clean(pszInput, cchPara);

    //GUI_UC_DispString(pszInput);
    //GUI_UC_DispStringInRect(pszInput, p_rect, text_Align);

    // return the part of the input processed so far
    return baselevel;
}

/**********************************************************************************************************
//                          gui_disp_bidistring_in_rect_wrap()
// Author:  Derek Lai
// Time:2010.10.26
// Arguments:
// Return:
// Descripion:
// Note:
// History:
***********************************************************************************************************/
#if 0
static __u32 get_ignore_num(__u16 *pinput_string, __u32 string_num)
{
    __u16 Char, Prevchar = 0, NextChar;
    __u32 i, loop_num, ignore_num = 0;

    loop_num = string_num - 1; //防止数组越界

    for (i = 0; i < loop_num; i++)
    {
        if (!Prevchar)
        {
            Char = pinput_string[i];
            if (Char)
            {
                NextChar = pinput_string[i + 1];
            }
        }
        else
        {
            Char = NextChar;
            if (Char)
            {
                NextChar = pinput_string[i + 1];
            }
        }

        if (is_ignore(Prevchar, Char, NextChar))
        {
            ignore_num++;
        }
        Prevchar = Char;
    }

    if (is_ignore(pinput_string[loop_num - 1], pinput_string[loop_num], 0x0)) //判断输入字符串最后一个字符是否与前一个字符合并
    {
        ignore_num++;
    }

    return ignore_num;
}
#endif
/**********************************************************************************************************
//                          gui_disp_bidistring_in_rect_wrap()
// Author:  Derek Lai
// Time:2010.10.26
// Arguments:
// Return:
// Descripion:
// Note:
// History:
***********************************************************************************************************/
static __s32 get_presentation_form(__u16 *pinput_string, __u16 *pout_string, __u32 input_string_num)
{
#if 0
    __u16 Char, PrevChar = 0, NextChar;
    __u32 i, j = 0, k, loop_num, ignore_num = 0;
    __s32 CharIsArabic, IgnoreNextCharacter = 0;

    loop_num = input_string_num - 1; //防止数组越界
    for (i = 0; i < loop_num; i++)
    {
        if (!PrevChar)
        {
            Char = pinput_string[i];
            if (Char)
            {
                NextChar = pinput_string[i + 1];
            }
        }
        else
        {
            Char = NextChar;
            if (Char)
            {
                NextChar = pinput_string[i + 1];
            }
        }

        if (IgnoreNextCharacter)
        {
            IgnoreNextCharacter = 0;
            j++;
        }
        else
        {
            if (Char != 0x20)
            {
                CharIsArabic = GUI__IsArabicCharacter(Char);
            }

            if (CharIsArabic)
            {
                pout_string[i - j] = GUI__GetPresentationForm(Char, NextChar, PrevChar, &IgnoreNextCharacter);
            }
            else
            {
                pout_string[i - j] = Char;
            }
        }
        PrevChar = Char;
    }
    //pout_string[loop_num - j] = GUI__GetPresentationForm(pinput_string[loop_num], 0x0, pinput_string[loop_num - 1], &IgnoreNextCharacter);
    //pout_string[loop_num - j + 1] = 0;
    if (IgnoreNextCharacter)
    {
        pout_string[loop_num - j] = 0;
    }
    else
    {
        pout_string[loop_num - j] = pinput_string[loop_num];
        pout_string[loop_num - j + 1] = 0;
    }
#endif
    int ignoreNext = 0;
    int vowelshift = 0;
    int countlen    = 0;
    int backcnt     = 0;
    int convertLen = 0;
    U16 c0;
    U16 *s2;

    countlen = input_string_num;
    backcnt = input_string_num - 1;

    countlen--;
    while (countlen >= 0)
    {
        c0 = pinput_string[countlen];
        s2 = &pinput_string[countlen];

        if (GUI__IsArabicCharacter(c0) <= 0)
        {
            pout_string[backcnt] = c0;
            backcnt--;
            convertLen++;
            countlen--;
            continue;
        }

        /* 在字符串反转的情况下，s_ncucs + 1表示前一个字符，
         * s_ucs -1表示后一个字符；
         * *(s_ucs + 1 ) != 0检查是否到了字符串末尾；
         * s_ucs != s 检查是否是字符串起始*/
        if ((pinput_string[countlen + 1] != 0) && (countlen > 0))
        {
            pout_string[backcnt] = (__u16)GUI__GetPresentationForm(c0,
                                   pinput_string[countlen - 1], pinput_string[countlen + 1],
                                   &ignoreNext, s2, pinput_string, &vowelshift);
        }
        if ((pinput_string[countlen + 1] == 0) && (countlen > 0))
        {
            pout_string[backcnt] = (__u16)GUI__GetPresentationForm(c0,
                                   pinput_string[countlen - 1], 0,
                                   &ignoreNext, s2, pinput_string, &vowelshift);
        }
        if ((pinput_string[countlen + 1] != 0) && (countlen <= 0))
        {
            pout_string[backcnt] = (__u16)GUI__GetPresentationForm(c0, 0,
                                   pinput_string[countlen + 1],
                                   &ignoreNext, s2, pinput_string, &vowelshift);
        }
        if ((pinput_string[countlen + 1] == 0) && (countlen <= 0))
        {
            pout_string[backcnt] = (__u16)GUI__GetPresentationForm(c0, 0, 0,
                                   &ignoreNext, s2, pinput_string, &vowelshift);
        }

        if (ignoreNext == 1)
        {
            ignoreNext = 0;
            countlen--;
        }
        if (countlen < 0)
        {
            break;
        }

        if (vowelshift == 1)
        {
            /*遇到上下标时，对调上下标和主字符的存储位置，使得显示时上下标偏移正确*/
            U16 temp;
            vowelshift = 0;
            temp = pout_string[backcnt];
            pout_string[backcnt] = pout_string[backcnt + 1];
            pout_string[backcnt + 1] = temp;
        }

        backcnt--;
        convertLen++;
        countlen--;
    }

#if  0
    eLIBs_printf("%s  %d----input_string_num = %d\n", __FILE__, __LINE__, input_string_num);
    eLIBs_printf("%s  %d----convertLen = %d\n", __FILE__, __LINE__, convertLen);
#endif



    if (convertLen < input_string_num)
    {
        int i;
        int delta = input_string_num - convertLen;

        for (i = 0; i < convertLen; i++)
        {
            pout_string[i] = pout_string[i + delta];
        }
        pout_string[i] = 0;
    }

#if 0
    {
        int i = 0;

        eLIBs_printf("------= %s\n", pinput_string);
        for (i = 0; i < 50; i++)
        {
            eLIBs_printf("pout_string[%d]=%x \n", i, pinput_string[i]);
        }

        eLIBs_printf("------= %s\n", pout_string);

        for (i = 0; i < 50; i++)
        {
            eLIBs_printf("pout_string[%d]=%x \n", i, pout_string[i]);
        }
    }
#endif




}


/**********************************************************************************************************
//                          gui_disp_bidistring_in_rect()
// Author:  Derek Lai
// Time:2010.10.26
// Arguments:
// Return:
// Descripion:
// Note:
// History:
***********************************************************************************************************/
void gui_disp_bidistring_in_rect(__s32 charset_enm, char *input_s, GUI_RECT *pRect, __s32 TextAlign)
{

    //__u16 bidi_types[MAX_CCH];    //__s32
    //__u16 levels[MAX_CCH];            //__s32
    //__u16 pszInput[MAX_CCH + 1];
    __u32 cch, paragraph_num, cch_per_parag, i, temp, temp_cch, disp_cch, temp_disp_cch, RTL_arabic_flags = 0, RTL_hebrew_flags = 0;

    cch_per_parag = 0;
    temp = 0;

    if (!input_s)
    {
        __wrn("the input string is invalid!");
        return;
    }

    //analyes input string and obtain the unicode_id string
    cch = get_character_bidi_type(charset_enm, input_s, pszInput, bidi_types);
    temp_cch = cch;

    if (0 == cch)
    {
        __wrn("the input string is null");
        return;
    }

    /*p1 rule*/
    paragraph_num = get_paragraph_num(bidi_types, cch);

    for (i = 0; i < paragraph_num; i++)
    {
        //calculate the number of per paragraph
        cch_per_parag = resolveParagraphs(&bidi_types[temp], cch);

        //bidi algorithm for one paragraph at a time
        //bidi_paragraph(&bidi_types[temp], &levels[temp], cch_per_parag, &pszInput[temp]);
        if (i < (paragraph_num - 1))
        {
            bidi_paragraph(&bidi_types[temp], &levels[temp], cch_per_parag - 2, &pszInput[temp]);
        }
        else
        {
            bidi_paragraph(&bidi_types[temp], &levels[temp], cch_per_parag, &pszInput[temp]);
        }

        temp    += cch_per_parag;
        cch     -= cch_per_parag;
    }

    disp_cch = clean(pszInput, temp_cch);
    temp_disp_cch = disp_cch;

    /*************/
    while (--temp_disp_cch)
    {
        RTL_arabic_flags = ((pszInput[temp_disp_cch] >= 0x600) && (pszInput[temp_disp_cch] <= 0x6ff)) ? 1 : 0;
        RTL_hebrew_flags = ((pszInput[temp_disp_cch] >= 0x590) && (pszInput[temp_disp_cch] <= 0x5ff)) ? 1 : 0;
        if ((RTL_arabic_flags) || (RTL_hebrew_flags))
        {
            orange_memset(disp_string, 0, MAX_CCH + 1);
            break;
        }
    }

    if (RTL_arabic_flags)
    {
#if 1
        get_presentation_form(pszInput, disp_string, disp_cch);
        GUI_UC_DispStringInRect(disp_string, pRect, TextAlign, disp_cch);

#else
        GUI_UC_DispStringInRect(pszInput, pRect, TextAlign, disp_cch);
#endif

    }
    else if (RTL_hebrew_flags)
    {
        get_presentation_hebrew_form(pszInput, disp_string, disp_cch);
        GUI_UC_DispStringInRect(disp_string, pRect, TextAlign, disp_cch);
    }
    else
    {
        GUI_UC_DispStringInRect(pszInput, pRect, TextAlign, disp_cch);
    }
    /*************/

    //GUI_UC_DispStringInRect(pszInput, pRect, TextAlign, disp_cch);

}

/**********************************************************************************************************
//                          gui_disp_bidistring_in_rect_wrap()
// Author:  Derek Lai
// Time:2010.10.26
// Arguments:
// Return:
// Descripion:
// Note:
// History:
***********************************************************************************************************/
void gui_disp_bidistring_in_rect_wrap(__s32 charset_enm, char *input_s, GUI_RECT *pRect, __s32 TextAlign, GUI_WRAPMODE WrapMode)
{
    __u16 baselevel[1000];
    __u32 cch, paragraph_num, cch_per_parag, i, temp, temp_cch, disp_cch, temp_disp_cch, RTL_arabic_flags = 0, RTL_hebrew_flags = 0;
    __u16 *temp_disp_string;

    cch_per_parag = 0;
    temp = 0;

    if (!input_s)
    {
        __wrn("the input string is invalid!");
        return;
    }

    //analyes input string and obtain the unicode_id string
    cch = get_character_bidi_type(charset_enm, input_s, pszInput, bidi_types);
    temp_cch = cch;

    if (0 == cch)
    {
        __wrn("the input string is null");
        return;
    }

    /*judge charset*/
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    temp_disp_cch = cch;
    while (--temp_disp_cch)
    {
        RTL_arabic_flags = ((pszInput[temp_disp_cch] >= 0x600) && (pszInput[temp_disp_cch] <= 0x6ff)) ? 1 : 0;
        RTL_hebrew_flags = ((pszInput[temp_disp_cch] >= 0x590) && (pszInput[temp_disp_cch] <= 0x5ff)) ? 1 : 0;
        if ((RTL_arabic_flags) || (RTL_hebrew_flags))
        {
            orange_memset(disp_string, 0, MAX_CCH + 1);
            break;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*p1 rule*/
    paragraph_num = get_paragraph_num(bidi_types, cch);

    for (i = 0; i < paragraph_num; i++)
    {
        //calculate the number of per paragraph
        cch_per_parag = resolveParagraphs(&bidi_types[temp], cch);

        //bidi algorithm for one paragraph at a time
        //baselevel[i] = bidi_paragraph(&bidi_types[temp], &levels[temp], cch_per_parag, &pszInput[temp]);
        if (i < (paragraph_num - 1))
        {
            baselevel[i] = bidi_paragraph(&bidi_types[temp], &levels[temp], cch_per_parag - 2, &pszInput[temp]);

            if (RTL_hebrew_flags)
            {
                temp_disp_string = orange_malloc((cch_per_parag - 2) * sizeof(__u16));
                if (temp_disp_string)
                {
                    convert_punc_end_and_start(temp_disp_string, &pszInput[temp], cch_per_parag - 2);
                    orange_memcpy(&pszInput[temp], temp_disp_string, (cch_per_parag - 2) * sizeof(__u16));
                    orange_mfree(temp_disp_string);
                }
                else
                {
                    __wrn("malloc fail!\n");
                }
                //convert_punc_end_and_start(temp_disp_string, &pszInput[temp], cch_per_parag - 2);
                //orange_memcpy(&pszInput[temp], temp_disp_string, (cch_per_parag - 2) * sizeof(__u16));
                //orange_mfree(temp_disp_string);
            }

        }
        else
        {
            baselevel[i] = bidi_paragraph(&bidi_types[temp], &levels[temp], cch_per_parag, &pszInput[temp]);

            if (RTL_hebrew_flags)
            {
                temp_disp_string = orange_malloc(cch_per_parag * sizeof(__u16));
                if (temp_disp_string)
                {
                    convert_punc_end_and_start(temp_disp_string, &pszInput[temp], cch_per_parag);
                    orange_memcpy(&pszInput[temp], temp_disp_string, cch_per_parag * sizeof(__u16));
                    orange_mfree(temp_disp_string);
                }
                else
                {
                    __wrn("malloc fail!\n");
                }
                /*convert_punc_end_and_start(temp_disp_string, &pszInput[temp], cch_per_parag);
                orange_memcpy(&pszInput[temp], temp_disp_string, cch_per_parag * sizeof(__u16));
                orange_mfree(temp_disp_string);*/
            }
        }

        temp    += cch_per_parag;
        cch     -= cch_per_parag;
    }

    disp_cch = clean(pszInput, temp_cch);
    //temp_disp_cch = disp_cch;

    /*************/
    /*while(--temp_disp_cch)
    {
        RTL_arabic_flags = ((pszInput[temp_disp_cch] >= 0x600) && (pszInput[temp_disp_cch] <= 0x6ff)) ? 1 : 0;
        RTL_hebrew_flags = ((pszInput[temp_disp_cch] >= 0x590) && (pszInput[temp_disp_cch] <= 0x5ff)) ? 1 : 0;
        if((RTL_arabic_flags) || (RTL_hebrew_flags))
        {
            orange_memset(disp_string, 0, MAX_CCH + 1);
            break;
        }
    }*/

    if (RTL_arabic_flags)
    {
#if 1
        get_presentation_form(pszInput, disp_string, disp_cch);
        GUI_UC_DispStringInRectWrapEX(disp_string, pRect, TextAlign, WrapMode, baselevel);
#else
        GUI_UC_DispStringInRectWrapEX(pszInput, pRect, TextAlign, WrapMode, baselevel);
#endif
    }
    else if (RTL_hebrew_flags)
    {
        get_presentation_hebrew_form(pszInput, disp_string, disp_cch);
        GUI_UC_DispStringInRectWrapEX(disp_string, pRect, TextAlign, WrapMode, baselevel);
    }
    else
    {
        GUI_UC_DispStringInRectWrapEX(pszInput, pRect, TextAlign, WrapMode, baselevel);
    }
    /*************/

    //GUI_UC_DispStringInRectWrapEX(pszInput, pRect, TextAlign, WrapMode, baselevel);

}

/**********************************************************************************************************
//                          gui_disp_bidistring_in_rect_wrap()
// Author:  Derek Lai
// Time:2010.10.26
// Arguments:
// Return:
// Descripion:
// Note:
// History:
***********************************************************************************************************/
void gui_disp_bidistring_at(__s32 charset_enm, char *input_s, __s32 x, __s32 y)
{
    __u32 cch, paragraph_num, cch_per_parag, i, temp, temp_cch, disp_cch, temp_disp_cch, RTL_arabic_flags = 0, RTL_hebrew_flags = 0;

    cch_per_parag = 0;
    temp = 0;

    if (!input_s)
    {
        __wrn("the input string is invalid!");
        return;
    }

    //analyes input string and obtain the unicode_id string
    cch = get_character_bidi_type(charset_enm, input_s, pszInput, bidi_types);
    temp_cch = cch;

    if (0 == cch)
    {
        __wrn("the input string is null");
        return;
    }

    /*p1 rule*/
    paragraph_num = get_paragraph_num(bidi_types, cch);

    for (i = 0; i < paragraph_num; i++)
    {
        //calculate the number of per paragraph
        cch_per_parag = resolveParagraphs(&bidi_types[temp], cch);

        //bidi algorithm for one paragraph at a time
        //bidi_paragraph(&bidi_types[temp], &levels[temp], cch_per_parag, &pszInput[temp]);
        if (i < (paragraph_num - 1))
        {
            bidi_paragraph(&bidi_types[temp], &levels[temp], cch_per_parag - 2, &pszInput[temp]);
        }
        else
        {
            bidi_paragraph(&bidi_types[temp], &levels[temp], cch_per_parag, &pszInput[temp]);
        }

        temp    += cch_per_parag;
        cch     -= cch_per_parag;


    }

    disp_cch = clean(pszInput, temp_cch);
    temp_disp_cch = disp_cch;

    /*************/
    while (--temp_disp_cch)
    {
        RTL_arabic_flags = ((pszInput[temp_disp_cch] >= 0x600) && (pszInput[temp_disp_cch] <= 0x6ff)) ? 1 : 0;
        RTL_hebrew_flags = ((pszInput[temp_disp_cch] >= 0x590) && (pszInput[temp_disp_cch] <= 0x5ff)) ? 1 : 0;
        if ((RTL_arabic_flags) || (RTL_hebrew_flags))
        {
            orange_memset(disp_string, 0, MAX_CCH + 1);
            break;
        }
    }


    if (RTL_arabic_flags)
    {
#if 1
        get_presentation_form(pszInput, disp_string, disp_cch);
        GUI_UC_DispStringAt(disp_string, x, y);
#else
        GUI_UC_DispStringAt(pszInput, x, y);
#endif

    }
    else if (RTL_hebrew_flags)
    {
        get_presentation_hebrew_form(pszInput, disp_string, disp_cch);
        GUI_UC_DispStringAt(disp_string, x, y);
    }
    else
    {
        GUI_UC_DispStringAt(pszInput, x, y);
    }
    /*************/
    //GUI_UC_DispStringAt(pszInput, x, y);

}

