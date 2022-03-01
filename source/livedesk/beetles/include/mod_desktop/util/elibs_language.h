/*
************************************************************************************************************************
*                                                  language definition
*
*                                  Copyright(C), 2006-2010, AllWinner Technology Co., Ltd.
*                                                  All Rights Reserved
*
* File Name   : elibs_language.h
*
* Author      : Gary.Wang
*
* Version     : 1.0
*
* Date        : 2010.08.25
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
* Gary.Wang      2010.08.25        1.0         build the file
*
************************************************************************************************************************
*/
#ifndef  __ELIBS_LANGUAGE_H__
#define  __ELIBS_LANGUAGE_H__

#include <typedef.h>

typedef  enum
{
    EPDK_LANGUAGE_ENM_ENGLISH    = 0x400,      // ”¢”Ô
    EPDK_LANGUAGE_ENM_CHINESES   = 0x410,      // ºÚÃÂ÷–Œƒ
    EPDK_LANGUAGE_ENM_CHINESET   = 0x420,      // ∑±ÃÂ÷–Œƒ
    EPDK_LANGUAGE_ENM_JAPANESE   = 0x430,      // »’”Ô
    EPDK_LANGUAGE_ENM_KOREAN     = 0x440,      // ∫´”Ô
    EPDK_LANGUAGE_ENM_GERMAN     = 0x450,      // µ¬”Ô
    EPDK_LANGUAGE_ENM_SPANISH    = 0x460,      // Œ˜∞‡—¿”Ô
    EPDK_LANGUAGE_ENM_FRENCH     = 0x470,      // ∑®”Ô
    EPDK_LANGUAGE_ENM_ITALIAN    = 0x480,      // “‚¥Û¿˚”Ô
    EPDK_LANGUAGE_ENM_PORTUGUESE = 0x490,      // ∆œÃ——¿”Ô
    EPDK_LANGUAGE_ENM_DUTCH      = 0x4a0,      // ∫…¿º”Ô
    EPDK_LANGUAGE_ENM_RUSSIAN    = 0x4b0,      // ∂Ì”Ô
    EPDK_LANGUAGE_ENM_POLISH     = 0x4c0,      // ≤®¿º”Ô
    EPDK_LANGUAGE_ENM_TURKISH    = 0x4d0,      // Õ¡∂˙∆‰”Ô
    EPDK_LANGUAGE_ENM_CZECH      = 0x4e0,      // Ω›øÀ”Ô
    EPDK_LANGUAGE_ENM_DANISH     = 0x4f0,      // µ§¬Û”Ô
    EPDK_LANGUAGE_ENM_HUNGARIAN  = 0x500,      // –Ÿ—¿¿˚”Ô
    EPDK_LANGUAGE_ENM_SWEDISH    = 0x510,      // »µ‰”Ô
    EPDK_LANGUAGE_ENM_THAI       = 0x520,      // Ã©”Ô
    EPDK_LANGUAGE_ENM_HEBREW     = 0x530,      // œ£≤Æ¿¥”Ô
    EPDK_LANGUAGE_ENM_ARABIC     = 0x540,      // ∞¢¿≠≤Æ”Ô

    EPDK_LANGUAGE_ENM_UNKNOWN    = -1          // always the last one
} __epdk_language_enm_e;


#define EPDK_LANGUAGE_NAME_ENGLISH       "ENGLISH"      // ”¢”Ô    
#define EPDK_LANGUAGE_NAME_CHINESES      "CHINESES"     // ºÚÃÂ÷–Œƒ
#define EPDK_LANGUAGE_NAME_CHINESET      "CHINESET"     // ∑±ÃÂ÷–Œƒ
#define EPDK_LANGUAGE_NAME_JAPANESE      "JAPANESE"     // »’”Ô    
#define EPDK_LANGUAGE_NAME_KOREAN        "KOREAN"       // ∫´”Ô    
#define EPDK_LANGUAGE_NAME_GERMAN        "GERMAN"       // µ¬”Ô    
#define EPDK_LANGUAGE_NAME_SPANISH       "SPANISH"      // Œ˜∞‡—¿”Ô
#define EPDK_LANGUAGE_NAME_FRENCH        "FRENCH"       // ∑®”Ô    
#define EPDK_LANGUAGE_NAME_ITALIAN       "ITALIAN"      // “‚¥Û¿˚”Ô
#define EPDK_LANGUAGE_NAME_PORTUGUESE    "PORTUGUESE"   // ∆œÃ——¿”Ô
#define EPDK_LANGUAGE_NAME_DUTCH         "DUTCH"        // ∫…¿º”Ô  
#define EPDK_LANGUAGE_NAME_RUSSIAN       "RUSSIAN"      // ∂Ì”Ô    
#define EPDK_LANGUAGE_NAME_POLISH        "POLISH"       // ≤®¿º”Ô  
#define EPDK_LANGUAGE_NAME_TURKISH       "TURKISH"      // Õ¡∂˙∆‰”Ô
#define EPDK_LANGUAGE_NAME_CZECH         "CZECH"        // Ω›øÀ”Ô  
#define EPDK_LANGUAGE_NAME_DANISH        "DANISH"       // µ§¬Û”Ô  
#define EPDK_LANGUAGE_NAME_HUNGARIAN     "HUNGARIAN"    // –Ÿ—¿¿˚”Ô
#define EPDK_LANGUAGE_NAME_SWEDISH       "SWEDISH"      // »µ‰”Ô  
#define EPDK_LANGUAGE_NAME_THAI          "THAI"         // Ã©”Ô    
#define EPDK_LANGUAGE_NAME_HEBREW        "HEBREW"       // œ£≤Æ¿¥”Ô
#define EPDK_LANGUAGE_NAME_ARABIC        "ARABIC"       // ∞¢¿≠≤Æ”Ô


#define EPDK_LANGUAGE_NAME_LEN_MAX       31



/*
****************************************************************************************************
*Name        : eLIBs_language_name2enm
*Prototype   : __epdk_language_enm_e  eLIBs_language_name2enm( const char *language_name )
*Arguments   : language_name   input. the name of a language
*Return      : the enum value of the language. return EPDK_LANGUAGE_ENM_UNKNOWN, if fail.
*Description : get the enum value of the language through its name
*Other       :
****************************************************************************************************
*/
extern __epdk_language_enm_e  eLIBs_language_name2enm(const char *language_name);

/*
****************************************************************************************************
*Name        : eLIBs_language_enm2name
*Prototype   : __s32  eLIBs_language_enm2name( __epdk_language_enm_e language_enm,
*                                              char *language_name, __u32 size )
*Arguments   : language_enm    input. the enum value of a language
*              language_name   output. a buffert to store the name of a language
*              size            input. the buffer's size.
*Return      : ==0      succeed
*              !=0      fail
*Description : get the name of the language through its enum value
*Other       :
****************************************************************************************************
*/
extern __s32  eLIBs_language_enm2name(__epdk_language_enm_e language_enm, char *language_name, __u32 size);



#endif     //  ifndef __ELIBS_LANGUAGE_H__

/* end of elibs_language.h */
