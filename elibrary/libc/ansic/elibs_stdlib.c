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
#include <kapi.h>
#include <mod_slib.h>
#include <log.h>
#include <stdio.h>

/*
******************************************************************************************************************
*
*Function Name : int_to_string_dec
*
*Description : This function is to convert an 'int' data 'input' to a string in decimalism, and the string
*              converted is in 'str'.
*
*Input : int input : 'int' data to be converted.
*        char * str : the start address of the string storing the converted result.
*
*Output : void
*
*call for :
*
*Others : None at present.
*
*******************************************************************************************************************
*/
void eLIBs_int2str_dec(int input, char *str)
{
    SLIB_int2str_dec(input, str);
}

/*
******************************************************************************************************************
*
*Function Name : Uint_to_string_dec
*
*Description : This function is to convert an 'unsigned int' data 'input' to a string in decimalism, and the
*              string converted is in 'str'.
*
*Input : int input : 'unsigned int' data to be converted.
*        char * str : the start address of the string storing the converted result.
*
*Output : void
*
*call for :
*
*Others : None at present.
*
*******************************************************************************************************************
*/
void eLIBs_uint2str_dec(unsigned int input, char *str)
{
    SLIB_uint2str_dec(input, str);
}

/*
******************************************************************************************************************
*
*Function Name : int_to_string_hex
*
*Description : This function is to convert an 'int' data 'input' to a string in hex, and the string
*              converted is in 'str'.
*
*Input : int input : 'int' data to be converted.
*        char * str : the start address of the string storing the converted result.
*        int hex_flag : hex_flag is just a option to distinguish whether hex format is '0f' or '0F'.
*
*Output : void
*
*call for :
*
*Others : None at present.
*
*******************************************************************************************************************
*/
void eLIBs_int2str_hex(int input, char *str, int hex_flag)
{
    SLIB_int2str_hex(input, str, hex_flag);
}

/*******************************************************************************
*
*             eLIBs_atoi
*
  Description:
  FS internal function. Convert string to int. The function stops with
  the first character it cannot convert. It expects decimal numbers only.
  It can handle +/- at the beginning and leading 0. It cannot handle
  HEX or any other numbers.

  Parameters:
  s           - Pointer to a zero terminated string.

  Return value:
  0           - In case of any problem or if the converted value is zero.
  !=0         - Integer value of the converted string.
*/
int eLIBs_atoi(const char *s)
{
    return SLIB_atoi(s);
}

/*
**********************************************************************************************************************
*                                               eLIBC_toupper
*
* Description: FS internal function. Convert a lowecase letter to a corresponding
*              uppercase letter.
*
* Arguments  : c           - Letter to convert.
*
* Returns    : Corresponding uppercase character.
*
* Notes      :
*
**********************************************************************************************************************
*/
int eLIBs_toupper(int c)
{
    return SLIB_toupper(c);
}

/*
**********************************************************************************************************************
*                                               eLIBs_isspace
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int eLIBs_isspace(int ch)
{
    return SLIB_isspace(ch);
}

/*
**********************************************************************************************************************
*                                               eLIBs_strtol
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
long eLIBs_strtol(const char *str, const char **err, int base)
{
    return SLIB_strtol(str, err, base);
}

/*
**********************************************************************************************************************
*                                               eLIBs_ramdom
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int eLIBs_ramdom(int max)
{
    return esKSRV_Random(max);
}


/*
**********************************************************************************************************************
*                                               eLIBs_malloc
*
* Description: request memory from system heap.
*
* Arguments  : size     size of memory need be requested.
*
* Returns    : the pointer of the memory requested. NULL means request failed.
*
**********************************************************************************************************************
*/
void *eLIBs_malloc(size_t size)
{
    return esMEMS_Malloc(0, size);
}

/*
**********************************************************************************************************************
*                                               eLIBs_free
*
* Description: free memory.
*
* Arguments  : ptr  memory address need be free.
*
* Returns    : none.
*
**********************************************************************************************************************
*/
void eLIBs_free(void *ptr)
{
    /* if ptr is NULL, the pointer is ignored and immediately returns. */
    if (NULL == ptr)
    {
        return ;
    }

    esMEMS_Mfree(0, ptr);
}

/*
**********************************************************************************************************************
*                                               eLIBs_realloc
*
* Description: reallocate memory from system heap.
*
* Arguments  : size     size of memory need be requested.
*
* Returns    : the pointer of the memory requested. NULL means request failed.
*
**********************************************************************************************************************
*/
void *eLIBs_realloc(void *ptr, size_t size)
{
    return esMEMS_Realloc(0, ptr, size);
}

/*******************************************************************************
*
*             eLIBs_atof
*
  Description:
  FS internal function. Convert string to float. The function stops with
  the first character it cannot convert. It expects decimal numbers only.
  It can handle +/- at the beginning and leading 0. It cannot handle
  HEX or any other numbers.

  Parameters:
  s           - Pointer to a zero terminated string.

  Return value:
  0           - In case of any problem or if the converted value is zero.
  !=0         - Float value of the converted string.
*/
double eLIBs_atof(const char *s)
{
    return SLIB_atoi(s);
}

__s8 eLIBs_sscanf(const char *buf, const char *fmt, ...)
{
    va_list args;
    __s8 rc;
    va_start(args, fmt);
    rc = SLIB_sscanf(buf, fmt, args);
    va_end(args);
    return rc;
}

void _exit(int status)
{
    __err("error, status = %d.", status);
    while (1);
}

void _write(void)
{
    __err("error, status");
    while (1);
}

void _kill(void)
{
    __err("error, status");
    while (1);
}

void _getpid(void)
{
    __err("error, status");
    while (1);
}

void _sbrk(void)
{
    __err("error, status");
    while (1);
}
