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
/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

/* Huffman code lookup table entry--this entry is four bytes for machines
   that have 16-bit pointers (e.g. PC's in the small or medium model). */

#ifndef _INFTREES_H
#define _INFTREES_H

typedef struct inflate_huft_s FAR inflate_huft;

struct inflate_huft_s
{
    union
    {
        struct
        {
            Byte Exop;        /* number of extra bits or operation */
            Byte Bits;        /* number of bits in this code or subcode */
        } what;
        uInt pad;           /* pad structure to a power of 2 (4 bytes for */
    } word;               /*  16-bit, 8 bytes for 32-bit int's) */
    uInt base;            /* literal, length base, distance base,
                           or table offset */
};

/* Maximum size of dynamic tree.  The maximum found in a long but non-
   exhaustive search was 1004 huft structures (850 for length/literals
   and 154 for distances, the latter actually the result of an
   exhaustive search).  The actual maximum is not known, but the
   value below is more than safe. */
#define MANY 1440

local  int inflate_trees_bits OF((
                                     uIntf *,                    /* 19 code lengths */
                                     uIntf *,                    /* bits tree desired/actual depth */
                                     inflate_huft *FAR *,        /* bits tree result */
                                     inflate_huft *,             /* space for trees */
                                     z_streamp));                /* for messages */

local  int inflate_trees_dynamic OF((
                                        uInt,                       /* number of literal/length codes */
                                        uInt,                       /* number of distance codes */
                                        uIntf *,                    /* that many (total) code lengths */
                                        uIntf *,                    /* literal desired/actual bit depth */
                                        uIntf *,                    /* distance desired/actual bit depth */
                                        inflate_huft *FAR *,        /* literal/length tree result */
                                        inflate_huft *FAR *,        /* distance tree result */
                                        inflate_huft *,             /* space for trees */
                                        z_streamp));                /* for messages */

local  int inflate_trees_fixed OF((
                                      uIntf *,                    /* literal desired/actual bit depth */
                                      uIntf *,                    /* distance desired/actual bit depth */
                                      const inflate_huft *FAR *,  /* literal/length tree result */
                                      const inflate_huft *FAR *,  /* distance tree result */
                                      z_streamp));                /* for memory allocation */

#endif /* _INFTREES_H */
