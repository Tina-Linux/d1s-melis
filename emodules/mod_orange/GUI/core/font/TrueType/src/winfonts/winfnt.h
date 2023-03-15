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
/*                                                                         */
/*  winfnt.h                                                               */
/*                                                                         */
/*    FreeType font driver for Windows FNT/FON files                       */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2003, 2004, 2007 by                         */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*  Copyright 2007 Dmitry Timoshkov for Codeweavers                        */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
#ifndef __WINFNT_H__
#define __WINFNT_H__


#include <ft2build.h>
#include FT_WINFONTS_H
#include FT_INTERNAL_DRIVER_H


FT_BEGIN_HEADER

typedef struct  WinMZ_HeaderRec_
{
    FT_UShort  magic;
    /* skipped content */
    FT_UShort  lfanew;

} WinMZ_HeaderRec;


typedef struct  WinNE_HeaderRec_
{
    FT_UShort  magic;
    /* skipped content */
    FT_UShort  resource_tab_offset;
    FT_UShort  rname_tab_offset;

} WinNE_HeaderRec;


typedef struct  WinPE32_HeaderRec_
{
    FT_ULong   magic;
    FT_UShort  machine;
    FT_UShort  number_of_sections;
    /* skipped content */
    FT_UShort  size_of_optional_header;
    /* skipped content */
    FT_UShort  magic32;
    /* skipped content */
    FT_ULong   rsrc_virtual_address;
    FT_ULong   rsrc_size;
    /* skipped content */

} WinPE32_HeaderRec;


typedef struct  WinPE32_SectionRec_
{
    FT_Byte   name[8];
    /* skipped content */
    FT_ULong  virtual_address;
    FT_ULong  size_of_raw_data;
    FT_ULong  pointer_to_raw_data;
    /* skipped content */

} WinPE32_SectionRec;


typedef struct  WinPE_RsrcDirRec_
{
    FT_ULong   characteristics;
    FT_ULong   time_date_stamp;
    FT_UShort  major_version;
    FT_UShort  minor_version;
    FT_UShort  number_of_named_entries;
    FT_UShort  number_of_id_entries;

} WinPE_RsrcDirRec;


typedef struct  WinPE_RsrcDirEntryRec_
{
    FT_ULong  name;
    FT_ULong  offset;

} WinPE_RsrcDirEntryRec;


typedef struct  WinPE_RsrcDataEntryRec_
{
    FT_ULong  offset_to_data;
    FT_ULong  size;
    FT_ULong  code_page;
    FT_ULong  reserved;

} WinPE_RsrcDataEntryRec;


typedef struct  WinNameInfoRec_
{
    FT_UShort  offset;
    FT_UShort  length;
    FT_UShort  flags;
    FT_UShort  id;
    FT_UShort  handle;
    FT_UShort  usage;

} WinNameInfoRec;


typedef struct  WinResourceInfoRec_
{
    FT_UShort  type_id;
    FT_UShort  count;

} WinResourceInfoRec;


#define WINFNT_MZ_MAGIC  0x5A4D
#define WINFNT_NE_MAGIC  0x454E
#define WINFNT_PE_MAGIC  0x4550


typedef struct  FNT_FontRec_
{
    FT_ULong             offset;

    FT_WinFNT_HeaderRec  header;

    FT_Byte             *fnt_frame;
    FT_ULong             fnt_size;
    FT_String           *family_name;

} FNT_FontRec, *FNT_Font;


typedef struct  FNT_FaceRec_
{
    FT_FaceRec     root;
    FNT_Font       font;

    FT_CharMap     charmap_handle;
    FT_CharMapRec  charmap;  /* a single charmap per face */

} FNT_FaceRec, *FNT_Face;


FT_EXPORT_VAR(const FT_Driver_ClassRec)  winfnt_driver_class;


FT_END_HEADER


#endif /* __WINFNT_H__ */


/* END */
