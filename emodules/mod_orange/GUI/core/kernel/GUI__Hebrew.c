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
#include "gui_core_kernel_private.h"

/**********************************************************************************************************
//                          GUI__IsHebrewCharacter()
// Author:      Derek Lai
// Time:        2011-7-11 19:24:34
// Arguments:
//              c: input character
// Return:      1: hebrew character; 0: not hebrew character
// Descripion:  judge a character whether a hebrew character
// Note:
// History:
***********************************************************************************************************/
static __s32 GUI__IsHebrewCharacter(__u16 c)
{
    return ((c >= 0x590) && (c <= 0x5ff)) ? 1 : 0;
}

static __s32 GUI__IsPunctuationCharacter(__u16 c)
{
    if (((c >= 0x21) && (c <= 0x2F)) || ((c >= 0x3A) && (c <= 0x40)) || ((c >= 0x5B) && (c <= 0x60)) || ((c >= 0x7B) && (c <= 0x7E)))
    {
        return 1;
    }
    return 0;
}

__s32 convert_punc_end_and_start(__u16 *output_string, __u16 *input_string, __u32 disp_cch)
{
    __u32 temp_disp_cch = 0, end_punc_count = 0, start_punc_count = 0, i = 0;
    __u16 Char = 0;
    __u32 temp_count = 0;

    if ((!output_string) || (!input_string))
    {
        ORANGE_WRN("input pointer is null, please check it!\n");
        return ORANGE_FAIL;
    }

    temp_disp_cch = disp_cch - 1;
    Char = input_string[temp_disp_cch];
    while (Char)
    {
        if (GUI__IsPunctuationCharacter(Char))
        {
            end_punc_count++;
            temp_disp_cch--;
            Char = input_string[temp_disp_cch];
        }
        else
        {
            break;
        }
    }

    temp_disp_cch = 0;
    Char = input_string[temp_disp_cch];
    while (Char)
    {
        if (GUI__IsPunctuationCharacter(Char))
        {
            start_punc_count++;
            temp_disp_cch++;
            Char = input_string[temp_disp_cch];
        }
        else
        {
            break;
        }
    }

    if ((end_punc_count) && (start_punc_count))
    {
        for (i = 0; i < end_punc_count; i++)
        {
            output_string[i] = input_string[disp_cch - 1 - i];
        }

        for (i = end_punc_count, temp_count = start_punc_count; i < disp_cch  - start_punc_count; i++, temp_count++)
        {
            output_string[i] = input_string[temp_count];
        }

        for (i = disp_cch - start_punc_count, temp_count = start_punc_count - 1; i < disp_cch; i++, temp_count--)
        {
            output_string[i] = input_string[temp_count];
        }
    }
    else if ((end_punc_count) && (!start_punc_count))
    {
        for (i = 0; i < end_punc_count; i++)
        {
            output_string[i] = input_string[disp_cch - 1 - i];
        }

        for (i = end_punc_count, temp_count = 0; i < disp_cch; i++, temp_count++)
        {
            output_string[i] = input_string[temp_count];
        }
    }
    else if ((start_punc_count) && (!end_punc_count))
    {
        for (i = 0, temp_count = start_punc_count; i < disp_cch - start_punc_count; i++, temp_count++)
        {
            output_string[i] = input_string[temp_count];
        }

        for (i = disp_cch - start_punc_count, temp_count = start_punc_count - 1; i < disp_cch; i++, temp_count--)
        {
            output_string[i] = input_string[temp_count];
        }
    }
    else
    {
        orange_memcpy(output_string, input_string, disp_cch * sizeof(__u16));
    }

    return ORANGE_OK;

}

/**********************************************************************************************************
//                          get__presentation_hebrew_form()
// Author:      Derek Lai
// Time:        2011-7-11 19:24:34
// Arguments:
//              Char: input character
//              pignore_char: combination flag
// Return:      hebrew presentation form after combination
// Descripion:  get Alphabetic presentation forms(Hebrew presentation, see The Unicode Standed5.0 FB30-FB4A)
// Note:
// History:
***********************************************************************************************************/
static __u16 get__presentation_hebrew_form(__u16 Char, __s32 *pignore_char)
{
    if (!pignore_char)
    {
        ORANGE_WRN("pignore_char is null!\n");
        return Char;
    }
    switch (Char)
    {
        case 0x05D0:
            *pignore_char = 1;
            return 0xFB30;
        case 0x05D1:
            *pignore_char = 1;
            return 0xFB31;
        case 0x05D2:
            *pignore_char = 1;
            return 0xFB32;
        case 0x05D3:
            *pignore_char = 1;
            return 0xFB33;
        case 0x05D4:
            *pignore_char = 1;
            return 0xFB34;
        case 0x05D5:
            *pignore_char = 1;
            return 0xFB35;
        case 0x05D6:
            *pignore_char = 1;
            return 0xFB36;
        case 0x05D8:
            *pignore_char = 1;
            return 0xFB38;
        case 0x05D9:
            *pignore_char = 1;
            return 0xFB39;
        case 0x05DA:
            *pignore_char = 1;
            return 0xFB3A;
        case 0x05DB:
            *pignore_char = 1;
            return 0xFB3B;
        case 0x05DC:
            *pignore_char = 1;
            return 0xFB3C;
        case 0x05DE:
            *pignore_char = 1;
            return 0xFB3E;
        case 0x05E0:
            *pignore_char = 1;
            return 0xFB40;
        case 0x05E1:
            *pignore_char = 1;
            return 0xFB41;
        case 0x05E3:
            *pignore_char = 1;
            return 0xFB43;
        case 0x05E4:
            *pignore_char = 1;
            return 0xFB44;
        case 0x05E6:
            *pignore_char = 1;
            return 0xFB46;
        case 0x05E7:
            *pignore_char = 1;
            return 0xFB47;
        case 0x05E8:
            *pignore_char = 1;
            return 0xFB48;
        case 0x05E9:
            *pignore_char = 1;
            return 0xFB49;
        case 0x05EA:
            *pignore_char = 1;
            return 0xFB4A;
        default:
            return Char;
    }
}

/**********************************************************************************************************
//                          get_presentation_hebrew_form()
// Author:  Derek Lai
// Time:2011-7-11 19:24:34
// Arguments:
//              pszInput: input string
//              disp_string: output string
//              disp_cch: the number of characters
// Return:      ORANGE_OK   success
//              ORANGE_FAIL fail
// Descripion:  get Alphabetic presentation forms(Hebrew presentation, see The Unicode Standed5.0 FB00-FB4B)
// Note:
// History:
***********************************************************************************************************/
__s32 get_presentation_hebrew_form(__u16 *pszInput, __u16 *disp_string, __u32 disp_cch)
{
    __s32 i = 0, j = 0, ignore_char = 0, char_is_hebrew = 0;
    __u16 Char = 0, temp_form = 0, prev_char = 0;
    //__u16 *temp_disp_string;

    if ((!pszInput) || (!disp_string))
    {
        ORANGE_WRN("input argument invalid!\n");
        return ORANGE_FAIL;
    }

    for (i = 0; i < disp_cch; i++)
    {
        Char = pszInput[i];
        char_is_hebrew = GUI__IsHebrewCharacter(Char);

        if (char_is_hebrew)
        {
            switch (Char)
            {
                case 0x05D0:
                    disp_string[i - j] = 0xFB21;
                    break;
                case 0x05D3:
                    disp_string[i - j] = 0xFB22;
                    break;
                case 0x05D4:
                    disp_string[i - j] = 0xFB23;
                    break;
                case 0x05DB:
                    disp_string[i - j] = 0xFB24;
                    break;
                case 0x05DD:
                    disp_string[i - j] = 0xFB26;
                    break;
                case 0x05E8:
                    disp_string[i - j] = 0xFB27;
                    break;
                case 0x05EA:
                    disp_string[i - j] = 0xFB28;
                    break;
                case 0x05BC:
                    prev_char = pszInput[i + 1];

                    char_is_hebrew = GUI__IsHebrewCharacter(prev_char);
                    if (char_is_hebrew)
                    {
                        temp_form = get__presentation_hebrew_form(prev_char, &ignore_char);
                        if (ignore_char)
                        {
                            disp_string[i - j] = temp_form;
                            i++;
                            j++;
                            ignore_char = 0;
                        }
                        else
                        {
                            disp_string[i - j] = Char;
                        }
                    }
                    else
                    {
                        disp_string[i - j] = Char;
                    }

                    break;
                case 0x05B4:
                    if (pszInput[i + 1] == 0x05D9)
                    {
                        disp_string[i - j] = 0xFB1D;
                        i++;
                        j++;
                    }
                    else
                    {
                        disp_string[i - j] = Char;
                    }
                    break;
                case 0x05B7:
                    if (pszInput[i + 1] == 0x05F2)
                    {
                        disp_string[i - j] = 0xFB1F;
                        i++;
                        j++;
                    }
                    else if (pszInput[i + 1] == 0x05D0)
                    {
                        disp_string[i - j] = 0xFB2E;
                        i++;
                        j++;
                    }
                    else
                    {
                        disp_string[i - j] = Char;
                    }
                    break;
                case 0x05B8:
                    if (pszInput[i + 1] == 0x05D0)
                    {
                        disp_string[i - j] = 0xFB2F;
                        i++;
                        j++;
                    }
                    else
                    {
                        disp_string[i - j] = Char;
                    }
                    break;
                case 0x05C1:
                    if (pszInput[i + 1] == 0x05E9)
                    {
                        disp_string[i - j] = 0xFB2A;
                        i++;
                        j++;
                    }
                    else if (pszInput[i + 1] == 0xFB49)
                    {
                        disp_string[i - j] = 0xFB2C;
                        i++;
                        j++;
                    }
                    else
                    {
                        disp_string[i - j] = Char;
                    }
                    break;
                case 0x05C2:
                    if (pszInput[i + 1] == 0x05E9)
                    {
                        disp_string[i - j] = 0xFB2B;
                        i++;
                        j++;
                    }
                    else if (pszInput[i + 1] == 0xFB49)
                    {
                        disp_string[i - j] = 0xFB2D;
                        i++;
                        j++;
                    }
                    else
                    {
                        disp_string[i - j] = Char;
                    }
                    break;
                case 0x05B9:
                    if (pszInput[i + 1] == 0x05D5)
                    {
                        disp_string[i - j] = 0xFB4B;
                        i++;
                        j++;
                    }
                    else
                    {
                        disp_string[i - j] = Char;
                    }
                    break;
                case 0x05BF:
                    if (pszInput[i + 1] == 0x05D1)
                    {
                        disp_string[i - j] = 0xFB4C;
                        i++;
                        j++;
                    }
                    else if (pszInput[i + 1] == 0x05DB)
                    {
                        disp_string[i - j] = 0xFB4D;
                        i++;
                        j++;
                    }
                    else if (pszInput[i + 1] == 0x05E4)
                    {
                        disp_string[i - j] = 0xFB4E;
                        i++;
                        j++;
                    }
                    else
                    {
                        disp_string[i - j] = Char;
                    }
                    break;
                case 0x05DC:
                    disp_string[i - j] = 0xFB25;
                    break;
                default:
                    disp_string[i - j] = pszInput[i];

            }
        }
        else
        {
            disp_string[i - j] = Char;
        }

    }
    disp_string[i - j] = 0;
    return ORANGE_OK;
}
