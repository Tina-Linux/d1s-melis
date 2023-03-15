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
#ifndef  __ELIBS_LANGUAGE_H__
#define  __ELIBS_LANGUAGE_H__

#include <typedef.h>

typedef  enum
{
    EPDK_LANGUAGE_ENM_ENGLISH    = 0x400,      // 英语
    EPDK_LANGUAGE_ENM_CHINESES   = 0x410,      // 简体中文
    EPDK_LANGUAGE_ENM_CHINESET   = 0x420,      // 繁体中文
    EPDK_LANGUAGE_ENM_JAPANESE   = 0x430,      // 日语
    EPDK_LANGUAGE_ENM_KOREAN     = 0x440,      // 韩语
    EPDK_LANGUAGE_ENM_GERMAN     = 0x450,      // 德语
    EPDK_LANGUAGE_ENM_SPANISH    = 0x460,      // 西班牙语
    EPDK_LANGUAGE_ENM_FRENCH     = 0x470,      // 法语
    EPDK_LANGUAGE_ENM_ITALIAN    = 0x480,      // 意大利语
    EPDK_LANGUAGE_ENM_PORTUGUESE = 0x490,      // 葡萄牙语
    EPDK_LANGUAGE_ENM_DUTCH      = 0x4a0,      // 荷兰语
    EPDK_LANGUAGE_ENM_RUSSIAN    = 0x4b0,      // 俄语
    EPDK_LANGUAGE_ENM_POLISH     = 0x4c0,      // 波兰语
    EPDK_LANGUAGE_ENM_TURKISH    = 0x4d0,      // 土耳其语
    EPDK_LANGUAGE_ENM_CZECH      = 0x4e0,      // 捷克语
    EPDK_LANGUAGE_ENM_DANISH     = 0x4f0,      // 丹麦语
    EPDK_LANGUAGE_ENM_HUNGARIAN  = 0x500,      // 匈牙利语
    EPDK_LANGUAGE_ENM_SWEDISH    = 0x510,      // 瑞典语
    EPDK_LANGUAGE_ENM_THAI       = 0x520,      // 泰语
    EPDK_LANGUAGE_ENM_HEBREW     = 0x530,      // 希伯来语
    EPDK_LANGUAGE_ENM_ARABIC     = 0x540,      // 阿拉伯语

    EPDK_LANGUAGE_ENM_UNKNOWN    = -1          // always the last one
} __epdk_language_enm_e;


#define EPDK_LANGUAGE_NAME_ENGLISH       "ENGLISH"      // 英语    
#define EPDK_LANGUAGE_NAME_CHINESES      "CHINESES"     // 简体中文
#define EPDK_LANGUAGE_NAME_CHINESET      "CHINESET"     // 繁体中文
#define EPDK_LANGUAGE_NAME_JAPANESE      "JAPANESE"     // 日语    
#define EPDK_LANGUAGE_NAME_KOREAN        "KOREAN"       // 韩语    
#define EPDK_LANGUAGE_NAME_GERMAN        "GERMAN"       // 德语    
#define EPDK_LANGUAGE_NAME_SPANISH       "SPANISH"      // 西班牙语
#define EPDK_LANGUAGE_NAME_FRENCH        "FRENCH"       // 法语    
#define EPDK_LANGUAGE_NAME_ITALIAN       "ITALIAN"      // 意大利语
#define EPDK_LANGUAGE_NAME_PORTUGUESE    "PORTUGUESE"   // 葡萄牙语
#define EPDK_LANGUAGE_NAME_DUTCH         "DUTCH"        // 荷兰语  
#define EPDK_LANGUAGE_NAME_RUSSIAN       "RUSSIAN"      // 俄语    
#define EPDK_LANGUAGE_NAME_POLISH        "POLISH"       // 波兰语  
#define EPDK_LANGUAGE_NAME_TURKISH       "TURKISH"      // 土耳其语
#define EPDK_LANGUAGE_NAME_CZECH         "CZECH"        // 捷克语  
#define EPDK_LANGUAGE_NAME_DANISH        "DANISH"       // 丹麦语  
#define EPDK_LANGUAGE_NAME_HUNGARIAN     "HUNGARIAN"    // 匈牙利语
#define EPDK_LANGUAGE_NAME_SWEDISH       "SWEDISH"      // 瑞典语  
#define EPDK_LANGUAGE_NAME_THAI          "THAI"         // 泰语    
#define EPDK_LANGUAGE_NAME_HEBREW        "HEBREW"       // 希伯来语
#define EPDK_LANGUAGE_NAME_ARABIC        "ARABIC"       // 阿拉伯语


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
