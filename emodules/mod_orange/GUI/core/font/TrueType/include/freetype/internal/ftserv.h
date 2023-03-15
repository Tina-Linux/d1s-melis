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
/*  ftserv.h                                                               */
/*                                                                         */
/*    The FreeType services (specification only).                          */
/*                                                                         */
/*  Copyright 2003, 2004, 2005, 2006, 2007 by                              */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
/*************************************************************************/
/*                                                                       */
/*  Each module can export one or more `services'.  Each service is      */
/*  identified by a constant string and modeled by a pointer; the latter */
/*  generally corresponds to a structure containing function pointers.   */
/*                                                                       */
/*  Note that a service's data cannot be a mere function pointer because */
/*  in C it is possible that function pointers might be implemented      */
/*  differently than data pointers (e.g. 48 bits instead of 32).         */
/*                                                                       */
/*************************************************************************/


#ifndef __FTSERV_H__
#define __FTSERV_H__


FT_BEGIN_HEADER

#if defined( _MSC_VER )      /* Visual C++ (and Intel C++) */

/* we disable the warning `conditional expression is constant' here */
/* in order to compile cleanly with the maximum level of warnings   */
#pragma warning( disable : 4127 )

#endif /* _MSC_VER */

/*
 * @macro:
 *   FT_FACE_FIND_SERVICE
 *
 * @description:
 *   This macro is used to look up a service from a face's driver module.
 *
 * @input:
 *   face ::
 *     The source face handle.
 *
 *   id ::
 *     A string describing the service as defined in the service's
 *     header files (e.g. FT_SERVICE_ID_MULTI_MASTERS which expands to
 *     `multi-masters').  It is automatically prefixed with
 *     `FT_SERVICE_ID_'.
 *
 * @output:
 *   ptr ::
 *     A variable that receives the service pointer.  Will be NULL
 *     if not found.
 */
#ifdef __cplusplus

#define FT_FACE_FIND_SERVICE( face, ptr, id )                               \
    FT_BEGIN_STMNT                                                            \
    FT_Module    module = FT_MODULE( FT_FACE( face )->driver );             \
    FT_Pointer   _tmp_  = NULL;                                             \
    FT_Pointer*  _pptr_ = (FT_Pointer*)&(ptr);                              \
    \
    \
    if ( module->clazz->get_interface )                                     \
        _tmp_ = module->clazz->get_interface( module, FT_SERVICE_ID_ ## id ); \
    *_pptr_ = _tmp_;                                                        \
    FT_END_STMNT

#else /* !C++ */

#define FT_FACE_FIND_SERVICE( face, ptr, id )                               \
    FT_BEGIN_STMNT                                                            \
    FT_Module   module = FT_MODULE( FT_FACE( face )->driver );              \
    FT_Pointer  _tmp_  = NULL;                                              \
    \
    if ( module->clazz->get_interface )                                     \
        _tmp_ = module->clazz->get_interface( module, FT_SERVICE_ID_ ## id ); \
    ptr = _tmp_;                                                            \
    FT_END_STMNT

#endif /* !C++ */

/*
 * @macro:
 *   FT_FACE_FIND_GLOBAL_SERVICE
 *
 * @description:
 *   This macro is used to look up a service from all modules.
 *
 * @input:
 *   face ::
 *     The source face handle.
 *
 *   id ::
 *     A string describing the service as defined in the service's
 *     header files (e.g. FT_SERVICE_ID_MULTI_MASTERS which expands to
 *     `multi-masters').  It is automatically prefixed with
 *     `FT_SERVICE_ID_'.
 *
 * @output:
 *   ptr ::
 *     A variable that receives the service pointer.  Will be NULL
 *     if not found.
 */
#ifdef __cplusplus

#define FT_FACE_FIND_GLOBAL_SERVICE( face, ptr, id )               \
    FT_BEGIN_STMNT                                                   \
    FT_Module    module = FT_MODULE( FT_FACE( face )->driver );    \
    FT_Pointer   _tmp_;                                            \
    FT_Pointer*  _pptr_ = (FT_Pointer*)&(ptr);                     \
    \
    \
    _tmp_ = ft_module_get_service( module, FT_SERVICE_ID_ ## id ); \
    *_pptr_ = _tmp_;                                               \
    FT_END_STMNT

#else /* !C++ */

#define FT_FACE_FIND_GLOBAL_SERVICE( face, ptr, id )               \
    FT_BEGIN_STMNT                                                   \
    FT_Module   module = FT_MODULE( FT_FACE( face )->driver );     \
    FT_Pointer  _tmp_;                                             \
    \
    \
    _tmp_ = ft_module_get_service( module, FT_SERVICE_ID_ ## id ); \
    ptr   = _tmp_;                                                 \
    FT_END_STMNT

#endif /* !C++ */


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****         S E R V I C E   D E S C R I P T O R S                 *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

/*
 *  The following structure is used to _describe_ a given service
 *  to the library.  This is useful to build simple static service lists.
 */
typedef struct  FT_ServiceDescRec_
{
    const char  *serv_id;     /* service name         */
    const void  *serv_data;   /* service pointer/data */

} FT_ServiceDescRec;

typedef const FT_ServiceDescRec  *FT_ServiceDesc;


/*
 *  Parse a list of FT_ServiceDescRec descriptors and look for
 *  a specific service by ID.  Note that the last element in the
 *  array must be { NULL, NULL }, and that the function should
 *  return NULL if the service isn't available.
 *
 *  This function can be used by modules to implement their
 *  `get_service' method.
 */
FT_BASE(FT_Pointer)
ft_service_list_lookup(FT_ServiceDesc  service_descriptors,
                       const char     *service_id);


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****             S E R V I C E S   C A C H E                       *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

/*
 *  This structure is used to store a cache for several frequently used
 *  services.  It is the type of `face->internal->services'.  You
 *  should only use FT_FACE_LOOKUP_SERVICE to access it.
 *
 *  All fields should have the type FT_Pointer to relax compilation
 *  dependencies.  We assume the developer isn't completely stupid.
 *
 *  Each field must be named `service_XXXX' where `XXX' corresponds to
 *  the correct FT_SERVICE_ID_XXXX macro.  See the definition of
 *  FT_FACE_LOOKUP_SERVICE below how this is implemented.
 *
 */
typedef struct  FT_ServiceCacheRec_
{
    FT_Pointer  service_POSTSCRIPT_FONT_NAME;
    FT_Pointer  service_MULTI_MASTERS;
    FT_Pointer  service_GLYPH_DICT;
    FT_Pointer  service_PFR_METRICS;
    FT_Pointer  service_WINFNT;

} FT_ServiceCacheRec, *FT_ServiceCache;


/*
 *  A magic number used within the services cache.
 */
#define FT_SERVICE_UNAVAILABLE  ((FT_Pointer)-2)  /* magic number */


/*
 * @macro:
 *   FT_FACE_LOOKUP_SERVICE
 *
 * @description:
 *   This macro is used to lookup a service from a face's driver module
 *   using its cache.
 *
 * @input:
 *   face::
 *     The source face handle containing the cache.
 *
 *   field ::
 *     The field name in the cache.
 *
 *   id ::
 *     The service ID.
 *
 * @output:
 *   ptr ::
 *     A variable receiving the service data.  NULL if not available.
 */
#ifdef __cplusplus

#define FT_FACE_LOOKUP_SERVICE( face, ptr, id )                \
    FT_BEGIN_STMNT                                               \
    FT_Pointer   svc;                                          \
    FT_Pointer*  Pptr = (FT_Pointer*)&(ptr);                   \
    \
    \
    svc = FT_FACE( face )->internal->services. service_ ## id; \
    if ( svc == FT_SERVICE_UNAVAILABLE )                       \
        svc = NULL;                                              \
    else if ( svc == NULL )                                    \
    {                                                          \
        FT_FACE_FIND_SERVICE( face, svc, id );                   \
        \
        FT_FACE( face )->internal->services. service_ ## id =    \
                (FT_Pointer)( svc != NULL ? svc                        \
                              : FT_SERVICE_UNAVAILABLE );  \
    }                                                          \
    *Pptr = svc;                                               \
    FT_END_STMNT

#else /* !C++ */

#define FT_FACE_LOOKUP_SERVICE( face, ptr, id )                \
    FT_BEGIN_STMNT                                               \
    FT_Pointer  svc;                                           \
    \
    \
    svc = FT_FACE( face )->internal->services. service_ ## id; \
    if ( svc == FT_SERVICE_UNAVAILABLE )                       \
        svc = NULL;                                              \
    else if ( svc == NULL )                                    \
    {                                                          \
        FT_FACE_FIND_SERVICE( face, svc, id );                   \
        \
        FT_FACE( face )->internal->services. service_ ## id =    \
                (FT_Pointer)( svc != NULL ? svc                        \
                              : FT_SERVICE_UNAVAILABLE );  \
    }                                                          \
    ptr = svc;                                                 \
    FT_END_STMNT

#endif /* !C++ */

/*
 *  A macro used to define new service structure types.
 */

#define FT_DEFINE_SERVICE( name )            \
    typedef struct FT_Service_ ## name ## Rec_ \
        FT_Service_ ## name ## Rec ;             \
    typedef struct FT_Service_ ## name ## Rec_ \
        const * FT_Service_ ## name ;            \
    struct FT_Service_ ## name ## Rec_

/* */

/*
 *  The header files containing the services.
 */

#define FT_SERVICE_BDF_H                "freetype/internal/services/svbdf.h"
#define FT_SERVICE_GLYPH_DICT_H         "freetype/internal/services/svgldict.h"
#define FT_SERVICE_GX_VALIDATE_H        "freetype/internal/services/svgxval.h"
#define FT_SERVICE_KERNING_H            "freetype/internal/services/svkern.h"
#define FT_SERVICE_MULTIPLE_MASTERS_H   "freetype/internal/services/svmm.h"
#define FT_SERVICE_OPENTYPE_VALIDATE_H  "freetype/internal/services/svotval.h"
#define FT_SERVICE_PFR_H                "freetype/internal/services/svpfr.h"
#define FT_SERVICE_POSTSCRIPT_CMAPS_H   "freetype/internal/services/svpscmap.h"
#define FT_SERVICE_POSTSCRIPT_INFO_H    "freetype/internal/services/svpsinfo.h"
#define FT_SERVICE_POSTSCRIPT_NAME_H    "freetype/internal/services/svpostnm.h"
#define FT_SERVICE_SFNT_H               "freetype/internal/services/svsfnt.h"
#define FT_SERVICE_TRUETYPE_ENGINE_H    "freetype/internal/services/svtteng.h"
#define FT_SERVICE_TT_CMAP_H            "freetype/internal/services/svttcmap.h"
#define FT_SERVICE_WINFNT_H             "freetype/internal/services/svwinfnt.h"
#define FT_SERVICE_XFREE86_NAME_H       "freetype/internal/services/svxf86nm.h"
#define FT_SERVICE_TRUETYPE_GLYF_H      "freetype/internal/services/svttglyf.h"

/* */

FT_END_HEADER

#endif /* __FTSERV_H__ */


/* END */
