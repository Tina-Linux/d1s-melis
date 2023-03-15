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
/*  ftrfork.h                                                              */
/*                                                                         */
/*    Embedded resource forks accessor (specification).                    */
/*                                                                         */
/*  Copyright 2004, 2006 by                                                */
/*  Masatake YAMATO and Redhat K.K.                                        */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/* Development of the code in this file is support of                      */
/* Information-technology Promotion Agency, Japan.                         */
/***************************************************************************/
#ifndef __FTRFORK_H__
#define __FTRFORK_H__


#include "ft2build.h"
#include FT_INTERNAL_OBJECTS_H


FT_BEGIN_HEADER


/* Number of guessing rules supported in `FT_Raccess_Guess'.            */
/* Don't forget to increment the number if you add a new guessing rule. */
#define FT_RACCESS_N_RULES  8


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    FT_Raccess_Guess                                                   */
/*                                                                       */
/* <Description>                                                         */
/*    Guess a file name and offset where the actual resource fork is     */
/*    stored.  The macro FT_RACCESS_N_RULES holds the number of          */
/*    guessing rules;  the guessed result for the Nth rule is            */
/*    represented as a triplet: a new file name (new_names[N]), a file   */
/*    offset (offsets[N]), and an error code (errors[N]).                */
/*                                                                       */
/* <Input>                                                               */
/*    library ::                                                         */
/*      A FreeType library instance.                                     */
/*                                                                       */
/*    stream ::                                                          */
/*      A file stream containing the resource fork.                      */
/*                                                                       */
/*    base_name ::                                                       */
/*      The (base) file name of the resource fork used for some          */
/*      guessing rules.                                                  */
/*                                                                       */
/* <Output>                                                              */
/*    new_names ::                                                       */
/*      An array of guessed file names in which the resource forks may   */
/*      exist.  If `new_names[N]' is NULL, the guessed file name is      */
/*      equal to `base_name'.                                            */
/*                                                                       */
/*    offsets ::                                                         */
/*      An array of guessed file offsets.  `offsets[N]' holds the file   */
/*      offset of the possible start of the resource fork in file        */
/*      `new_names[N]'.                                                  */
/*                                                                       */
/*    errors ::                                                          */
/*      An array of FreeType error codes.  `errors[N]' is the error      */
/*      code of Nth guessing rule function.  If `errors[N]' is not       */
/*      FT_Err_Ok, `new_names[N]' and `offsets[N]' are meaningless.      */
/*                                                                       */
FT_BASE(void)
FT_Raccess_Guess(FT_Library  library,
                 FT_Stream   stream,
                 char       *base_name,
                 char      **new_names,
                 FT_Long    *offsets,
                 FT_Error   *errors);


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    FT_Raccess_Get_HeaderInfo                                          */
/*                                                                       */
/* <Description>                                                         */
/*    Get the information from the header of resource fork.  The         */
/*    information includes the file offset where the resource map        */
/*    starts, and the file offset where the resource data starts.        */
/*    `FT_Raccess_Get_DataOffsets' requires these two data.              */
/*                                                                       */
/* <Input>                                                               */
/*    library ::                                                         */
/*      A FreeType library instance.                                     */
/*                                                                       */
/*    stream ::                                                          */
/*      A file stream containing the resource fork.                      */
/*                                                                       */
/*    rfork_offset ::                                                    */
/*      The file offset where the resource fork starts.                  */
/*                                                                       */
/* <Output>                                                              */
/*    map_offset ::                                                      */
/*      The file offset where the resource map starts.                   */
/*                                                                       */
/*    rdata_pos ::                                                       */
/*      The file offset where the resource data starts.                  */
/*                                                                       */
/* <Return>                                                              */
/*    FreeType error code.  FT_Err_Ok means success.                     */
/*                                                                       */
FT_BASE(FT_Error)
FT_Raccess_Get_HeaderInfo(FT_Library  library,
                          FT_Stream   stream,
                          FT_Long     rfork_offset,
                          FT_Long    *map_offset,
                          FT_Long    *rdata_pos);


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    FT_Raccess_Get_DataOffsets                                         */
/*                                                                       */
/* <Description>                                                         */
/*    Get the data offsets for a tag in a resource fork.  Offsets are    */
/*    stored in an array because, in some cases, resources in a resource */
/*    fork have the same tag.                                            */
/*                                                                       */
/* <Input>                                                               */
/*    library ::                                                         */
/*      A FreeType library instance.                                     */
/*                                                                       */
/*    stream ::                                                          */
/*      A file stream containing the resource fork.                      */
/*                                                                       */
/*    map_offset ::                                                      */
/*      The file offset where the resource map starts.                   */
/*                                                                       */
/*    rdata_pos ::                                                       */
/*      The file offset where the resource data starts.                  */
/*                                                                       */
/*    tag ::                                                             */
/*      The resource tag.                                                */
/*                                                                       */
/* <Output>                                                              */
/*    offsets ::                                                         */
/*      The stream offsets for the resource data specified by `tag'.     */
/*      This array is allocated by the function, so you have to call     */
/*      @ft_mem_free after use.                                          */
/*                                                                       */
/*    count ::                                                           */
/*      The length of offsets array.                                     */
/*                                                                       */
/* <Return>                                                              */
/*    FreeType error code.  FT_Err_Ok means success.                     */
/*                                                                       */
/* <Note>                                                                */
/*    Normally you should use `FT_Raccess_Get_HeaderInfo' to get the     */
/*    value for `map_offset' and `rdata_pos'.                            */
/*                                                                       */
FT_BASE(FT_Error)
FT_Raccess_Get_DataOffsets(FT_Library  library,
                           FT_Stream   stream,
                           FT_Long     map_offset,
                           FT_Long     rdata_pos,
                           FT_Long     tag,
                           FT_Long   **offsets,
                           FT_Long    *count);


FT_END_HEADER

#endif /* __FTRFORK_H__ */


/* END */
