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
#include <typedef.h>
#include <log.h>
#include <mod_defs.h>

/* max module number in stub system  */
#define  SYS_MSTUB_MAX_MOD      16

/* special id is invalid or not */
#define  MSTUB_INVALID_ID(id)   (id > EPOS_smid_max)

typedef struct __MSTUB_FUNC_INFO
{
    uint8_t             id;
    unsigned long       *entry;
} __mstub_func_info_t;

typedef struct __MSTUB_FUNC_TBL
{
    uint32_t            ModCount;
    __mstub_func_info_t FuncTbl[SYS_MSTUB_MAX_MOD];
} __mstub_func_tbl_t;

static __mstub_func_tbl_t MStubFuncTbl;

/*
*********************************************************************************************************
*                                               SYSTEM MSTUB INIT
*
* Description: system stub init;
*
* Arguments  : none
*
* Returns    : result;
*                   EPDK_OK,    system stub init successed;
*                   EPDK_FAIL,  system stub init failed;
*********************************************************************************************************
*/
int32_t MSTUB_Init(void)
{
    int32_t     i;

    /* initialize function table */
    for (i = 0; i < SYS_MSTUB_MAX_MOD; i++)
    {
        MStubFuncTbl.FuncTbl[i].id   =  0;
        MStubFuncTbl.FuncTbl[i].entry = NULL;
    }
    MStubFuncTbl.ModCount = 0;
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                               SYSTEM MSTUB EXIT
*
* Description: system stub exit;
*
* Arguments  : none
*
* Returns    : result;
*                   EPDK_OK,    system stub exit successed;
*                   EPDK_FAIL,  system stub exit failed;
*********************************************************************************************************
*/
int32_t MSTUB_Exit(void)
{
    int32_t i;

    /* destory function table content */
    for (i = 0; i < SYS_MSTUB_MAX_MOD; i++)
    {
        MStubFuncTbl.FuncTbl[i].id   =  0;
        MStubFuncTbl.FuncTbl[i].entry = NULL;
    }
    MStubFuncTbl.ModCount = 0;
    return EPDK_OK;
}

/*
*********************************************************************************************************
*                                               REGISTER MSTUB FUNCTION TABLE
*
* Description: Register stub function table;
*
* Arguments  : id       stub module id, what is allocated by system when insmod mod_lemon;
*              tbl      stub function table, defined in mod_lemon
*
* Returns    : result;
*                   EPDK_OK,    register stub function table successed;
*                   EPDK_FAIL,  register stub function table failed;
*********************************************************************************************************
*/
int32_t esMSTUB_RegFuncTbl(int32_t id, void *tbl)
{
    int32_t i;

    if (MSTUB_INVALID_ID(id) || (NULL == tbl))
    {
        __wrn("invalid parametrs for stub system register");
        return EPDK_FAIL;
    }

    /* check function table have registered alreadly */
    for (i = 0; i < SYS_MSTUB_MAX_MOD; i++)
    {
        if (MStubFuncTbl.FuncTbl[i].id == id)
        {
            __wrn("module[%d] function table have registered already", id);
            /* FIXME : maybe regard as unlawful operation */
            //return EPDK_FAIL;
            break;
        }
    }

    if (SYS_MSTUB_MAX_MOD <= MStubFuncTbl.ModCount)
    {
        __wrn("registered modules number beyond the limit");
        return EPDK_FAIL;
    }

    /* set function table entry point, and inc ModCount */
    MStubFuncTbl.FuncTbl[MStubFuncTbl.ModCount].id  =   id;
    MStubFuncTbl.FuncTbl[MStubFuncTbl.ModCount].entry = tbl;
    MStubFuncTbl.ModCount++;

    return EPDK_OK;
}


/*
*********************************************************************************************************
*                                               UNREGISTER MSTUB FUNCTION TABLE
*
* Description: unregister stub function table;
*
* Arguments  : none;
*
* Returns    : result;
*                   EPDK_OK,    unregister function table successed;
*                   EPDK_FAIL,  unregister function table failed;
*********************************************************************************************************
*/
int32_t esMSTUB_UnRegFuncTbl(int32_t id)
{
    int32_t i;

    /* seach target module with special id  */
    for (i = 0; i < SYS_MSTUB_MAX_MOD; i++)
    {
        if (MStubFuncTbl.FuncTbl[i].id == id)
        {

            /* destory function table record in stub sytem */
            MStubFuncTbl.FuncTbl[i].id   = 0;
            MStubFuncTbl.FuncTbl[i].entry = NULL;
            MStubFuncTbl.ModCount--;
            return EPDK_OK;
        }
    }
    __wrn("module [%d] not register in stub system", id);
    return EPDK_FAIL;
}


/*
*********************************************************************************************************
*                                               GET MSTUB FUNCTION ENTRY
*
* Description: get stub function entry;
*
* Arguments  : funcnum      function index number in the function table;
*
* Returns    : result, function entry;
*********************************************************************************************************
*/
unsigned long esMSTUB_GetFuncEntry(int32_t id, uint32_t funcnum)
{
    unsigned long   *tmpL1Entry, *tmpL2Entry;
    int32_t         tmpIdx1, tmpIdx2;
    int32_t         i;

    /* seach target module with special id  */
    for (i = 0; i < SYS_MSTUB_MAX_MOD; i++)
    {
        if (MStubFuncTbl.FuncTbl[i].id == id)
        {
            break;
        }
    }
    if ((i >= MStubFuncTbl.ModCount) || (i >= SYS_MSTUB_MAX_MOD))
    {
        __wrn("module [%d] has not registered in stub system!", id);
        return 0;
    }

    tmpIdx1 = (funcnum >> 16) & 0xffff;
    tmpIdx2 = (funcnum >> 0) & 0xffff;

    tmpL1Entry = MStubFuncTbl.FuncTbl[i].entry;
    tmpL2Entry = (unsigned long *)tmpL1Entry[tmpIdx1];

    return tmpL2Entry[tmpIdx2];
}