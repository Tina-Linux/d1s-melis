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
#ifndef  __ELIBS_LANGUAGE_C__
#define  __ELIBS_LANGUAGE_C__


//#include "elibs_i.h"
#include "util/elibs_language.h"
#include <elibs_string.h>

typedef struct _language_map_t
{
    char                   name[EPDK_LANGUAGE_NAME_LEN_MAX + 1];
    __epdk_language_enm_e  enm;
} language_map_t;


const static language_map_t language_map_tbl[ ] =
{
    { EPDK_LANGUAGE_NAME_ENGLISH,       EPDK_LANGUAGE_ENM_ENGLISH    },
    { EPDK_LANGUAGE_NAME_CHINESES,      EPDK_LANGUAGE_ENM_CHINESES   },
    { EPDK_LANGUAGE_NAME_CHINESET,      EPDK_LANGUAGE_ENM_CHINESET   },
    { EPDK_LANGUAGE_NAME_JAPANESE,      EPDK_LANGUAGE_ENM_JAPANESE   },
    { EPDK_LANGUAGE_NAME_KOREAN,        EPDK_LANGUAGE_ENM_KOREAN     },
    { EPDK_LANGUAGE_NAME_GERMAN,        EPDK_LANGUAGE_ENM_GERMAN     },
    { EPDK_LANGUAGE_NAME_SPANISH,       EPDK_LANGUAGE_ENM_SPANISH    },
    { EPDK_LANGUAGE_NAME_FRENCH,        EPDK_LANGUAGE_ENM_FRENCH     },
    { EPDK_LANGUAGE_NAME_ITALIAN,       EPDK_LANGUAGE_ENM_ITALIAN    },
    { EPDK_LANGUAGE_NAME_PORTUGUESE,    EPDK_LANGUAGE_ENM_PORTUGUESE },
    { EPDK_LANGUAGE_NAME_DUTCH,         EPDK_LANGUAGE_ENM_DUTCH      },
    { EPDK_LANGUAGE_NAME_RUSSIAN,       EPDK_LANGUAGE_ENM_RUSSIAN    },
    { EPDK_LANGUAGE_NAME_POLISH,        EPDK_LANGUAGE_ENM_POLISH     },
    { EPDK_LANGUAGE_NAME_TURKISH,       EPDK_LANGUAGE_ENM_TURKISH    },
    { EPDK_LANGUAGE_NAME_CZECH,         EPDK_LANGUAGE_ENM_CZECH      },
    { EPDK_LANGUAGE_NAME_DANISH,        EPDK_LANGUAGE_ENM_DANISH     },
    { EPDK_LANGUAGE_NAME_HUNGARIAN,     EPDK_LANGUAGE_ENM_HUNGARIAN  },
    { EPDK_LANGUAGE_NAME_SWEDISH,       EPDK_LANGUAGE_ENM_SWEDISH    },
    { EPDK_LANGUAGE_NAME_THAI,          EPDK_LANGUAGE_ENM_THAI       },
    { EPDK_LANGUAGE_NAME_HEBREW,        EPDK_LANGUAGE_ENM_HEBREW     },
    { EPDK_LANGUAGE_NAME_ARABIC,        EPDK_LANGUAGE_ENM_ARABIC     }
};

#define LANGUAGE_NR           ( sizeof(language_map_tbl) / sizeof(language_map_tbl[0]) )



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
__epdk_language_enm_e  eLIBs_language_name2enm(const char *language_name)
{
    __u32  i;

    if (language_name == NULL)
    {
        return EPDK_LANGUAGE_ENM_UNKNOWN;
    }

    /* search in table */
    for (i = 0;  i < LANGUAGE_NR;  i++)
    {
        if (eLIBs_strcmp(language_name, language_map_tbl[i].name) == 0)
        {
            return language_map_tbl[i].enm;
        }
    }

    return EPDK_LANGUAGE_ENM_UNKNOWN;
}



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
__s32  eLIBs_language_enm2name(__epdk_language_enm_e language_enm, char *language_name, __u32 size)
{
    __u32  i;

    if (language_name == NULL || size == 0)
    {
        return -1;
    }

    /* search in table */
    for (i = 0;  i < LANGUAGE_NR;  i++)
    {
        if (language_enm == language_map_tbl[i].enm)
        {
            eLIBs_strncpy(language_name, language_map_tbl[i].name, size);
            language_name[size - 1] = '\0';
            return 0;
        }
    }

    return -1;
}

#endif     //  ifndef __ELIBS_LANGUAGE_C__

/* end of elibs_language.c */
