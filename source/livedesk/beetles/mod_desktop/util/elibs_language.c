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
