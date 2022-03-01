//----------------------------------------------------------------------------------------------------------//
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//    I BBMMBB      BBMMBBI     BBI EEMMFFI BBMMI     BBI EEMMFFI BBMMI       BBMMBBI   EEMMI EEMMLL        //
//    EEI   EEFF  EEI   LLBB  EEMMBBI I BBEE  I EE  EEMMBBI I BBEE  I EE    EEI   LLBB    EEBBI I BB        //
//  I FF        I EE      BBI   EEI     EEI   I EE    EEI     EEI   I EE  I EE      BBI   EEI   I EE        //
//  LLFF        LLEE      EELL  EEI     EEI   I EE    EEI     EEI   I EE  LLEE      EELL  EEI   I EE        //
//  I EE      I I BB      EEI   EEI     EEI   I EE    EEI     EEI   I EE  I BB      EEI   EEI   I EE        //
//    BBLL  I I   BBFF  I EE    EEI     EEI   I BB    EEI     EEI   I BB    BBFF  I EE    BBI   I EE        //
//    I BBMMEE    I BBMMBB    EEMMMMLLBBMMBBLLMMMMEEEEMMMMLLBBMMBBLLMMMMEE  I BBMMBB    EEMMMMLLBBMMBB      //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//----------------------------------------------------------------------------------------------------------//
//                                                                                                          //
//                                              Lion System                                                 //
//                                                                                                          //
//                               (c) Copyright 2007-2008, Scottyu China                                     //
//                                                                                                          //
//                                           All Rights Reserved                                            //
//                                                                                                          //
// File    : sysmemory.h                                                                                    //
// By      : scottyu                                                                                        //
// Version : V1.00                                                                                          //
// Time    : 2008-08-12 11:05:12                                                                            //
//                                                                                                          //
//----------------------------------------------------------------------------------------------------------//
//                                                                                                          //
// HISTORY                                                                                                  //
//                                                                                                          //
// 1 2008-08-12 11:05:16                                                                                    //
//                                                                                                          //
// 2 2008-08-19 10:25:23                                                                                    //
//   add EPDK_OS macro                                                                                      //
//                                                                                                          //
//                                                                                                          //
//                                                                                                          //
//----------------------------------------------------------------------------------------------------------//


#ifndef __SYSTEM_MEMORY__H____
#define __SYSTEM_MEMORY__H____

#include <kapi.h>

#define sysmalloc(size)           esMEMS_Malloc(0,size)
#define sysfree(ptr)              esMEMS_Mfree(0,ptr)
#define MEM_MALLOC(size)          esMEMS_Malloc(0,size)
#define MEM_FREE(p)               esMEMS_Mfree(0,p)
#define MEM_BMALLOC(size)         esMEMS_Balloc(size)
#define MEM_BFREE(ptr,size)       esMEMS_Bfree(ptr,size)
#define MEM_PMALLOC(size)         esMEMS_Palloc(((size + 1023 )/1024), MEMS_PALLOC_MODE_BND_1K);
#define MEM_PFREE(ptr,size)       esMEMS_Pfree(ptr,((size + 1023 )/1024));
#define sysmemcpy                 eLIBs_memcpy
#endif //__SYSTEM_MEMORY__H____

