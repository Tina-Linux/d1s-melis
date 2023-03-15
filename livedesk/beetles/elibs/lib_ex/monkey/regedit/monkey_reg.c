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
#include "monkey_i.h"
#include "monkey_reg.h"

#define MK_REG_NODE     "\\ebook\\ebook"


__s32   mkr_setColor(MkColor mColor)
{
    __hdle  node;
    char  key_value[ ELIBS_KEY_VALUE_LEN + 1];
    node = eLIBs_RegOpenNode(MK_REG_NODE, ELIBS_REG_READWRITE);

    if (NULL == node)
    {
        __wrn("reg node cannot be opened!\n");
        return EPDK_FAIL;
    }

    switch (mColor)
    {
        case MK_COLOR_BLACK:
            g_strcpy(key_value, "black");
            break;

        case MK_COLOR_BLUE:
            g_strcpy(key_value, "blue");
            break;

        case MK_COLOR_GREEN:
            g_strcpy(key_value, "green");
            break;

        case MK_COLOR_RED:
            g_strcpy(key_value, "red");
            break;

        case MK_COLOR_WHITE:
            g_strcpy(key_value, "white");
            break;

        default:
            g_strcpy(key_value, "black");
            break;
    }

    eLIBs_RegSetKeyValue(node, "config", "color", key_value);
    eLIBs_RegCloseNode(node);
    return EPDK_OK;
}

MkColor mkr_getColor(void)
{
    __hdle  node;
    char  key_value[ ELIBS_KEY_VALUE_LEN + 1];
    MkColor ret;
    node = eLIBs_RegOpenNode(MK_REG_NODE, ELIBS_REG_READWRITE);

    if (NULL == node)
    {
        __wrn("reg node cannot be opened!\n");
        return MK_COLOR_BLACK;
    }

    eLIBs_RegGetKeyValue(node, "config", "color", key_value);

    if (!g_strcmp(key_value, "black"))
    {
        ret = MK_COLOR_BLACK;
    }
    else if (!g_strcmp(key_value, "blue"))
    {
        ret = MK_COLOR_BLUE;
    }
    else if (!g_strcmp(key_value, "green"))
    {
        ret = MK_COLOR_GREEN;
    }
    else if (!g_strcmp(key_value, "red"))
    {
        ret = MK_COLOR_RED;
    }
    else if (!g_strcmp(key_value, "white"))
    {
        ret = MK_COLOR_WHITE;
    }
    else
    {
        ret = MK_COLOR_BLACK;
    }

    eLIBs_RegCloseNode(node);
    return ret;
}

__s32   mkr_setFont(MkFont mFont)
{
    char  key_value[ ELIBS_KEY_VALUE_LEN + 1];
    __hdle  node;
    node = eLIBs_RegOpenNode(MK_REG_NODE, ELIBS_REG_READWRITE);

    if (NULL == node)
    {
        __wrn("reg node cannot be opened!\n");
        return EPDK_FAIL;
    }

    switch (mFont)
    {
        case MK_FONT_0:
            g_strcpy(key_value, "font0");
            break;

        case MK_FONT_1:
            g_strcpy(key_value, "font1");
            break;

        default:
            g_strcpy(key_value, "font0");
            break;
    }

    eLIBs_RegSetKeyValue(node, "config", "font", key_value);
    eLIBs_RegCloseNode(node);
    return EPDK_OK;
}

MkFont  mkr_getFont(void)
{
    char    key_value[ ELIBS_KEY_VALUE_LEN + 1];
    MkFont  ret;
    __hdle  node;
    node = eLIBs_RegOpenNode(MK_REG_NODE, ELIBS_REG_READWRITE);

    if (NULL == node)
    {
        __wrn("reg node cannot be opened!\n");
        return MK_FONT_0;
    }

    eLIBs_RegGetKeyValue(node, "config", "font", key_value);

    if (!g_strcmp(key_value, "font0"))
    {
        ret = MK_FONT_0;
    }
    else if (!g_strcmp(key_value, "font1"))
    {
        ret = MK_FONT_1;
    }
    else
    {
        ret = MK_FONT_0;
    }

    eLIBs_RegCloseNode(node);
    return ret;
}

__s32   mkr_setRotate(MkRotate mRotate)
{
    char  key_value[ ELIBS_KEY_VALUE_LEN + 1];
    __hdle  node;
    node = eLIBs_RegOpenNode(MK_REG_NODE, ELIBS_REG_READWRITE);

    if (NULL == node)
    {
        __wrn("reg node cannot be opened!\n");
        return EPDK_OK;
    }

    switch (mRotate)
    {
        case MK_ROTATE_0:
            g_strcpy(key_value, "0");
            break;

        case MK_ROTATE_90:
            g_strcpy(key_value, "90");
            break;

        case MK_ROTATE_180:
            g_strcpy(key_value, "180");
            break;

        case MK_ROTATE_270:
            g_strcpy(key_value, "270");
            break;

        default:
            g_strcpy(key_value, "0");
            break;
    }

    eLIBs_RegSetKeyValue(node, "config", "rotate", key_value);
    eLIBs_RegCloseNode(node);
    return EPDK_OK;
}

MkRotate mkr_getRotate(void)
{
    char    key_value[ ELIBS_KEY_VALUE_LEN + 1];
    MkRotate ret;
    __hdle  node;
    node = eLIBs_RegOpenNode(MK_REG_NODE, ELIBS_REG_READWRITE);

    if (NULL == node)
    {
        __wrn("reg node cannot be opened!\n");
        return MK_ROTATE_0;
    }

    eLIBs_RegGetKeyValue(node, "config", "rotate", key_value);

    if (!g_strcmp(key_value, "0"))
    {
        ret = MK_ROTATE_0;
    }
    else if (!g_strcmp(key_value, "90"))
    {
        ret = MK_ROTATE_90;
    }
    else if (!g_strcmp(key_value, "180"))
    {
        ret = MK_ROTATE_180;
    }
    else if (!g_strcmp(key_value, "270"))
    {
        ret = MK_ROTATE_270;
    }
    else
    {
        ret = MK_ROTATE_0;
    }

    eLIBs_RegCloseNode(node);
    return ret;
}

__s32   mkr_setSwitch(MkSwitch mMode)
{
    char  key_value[ ELIBS_KEY_VALUE_LEN + 1];
    __hdle  node;
    node = eLIBs_RegOpenNode(MK_REG_NODE, ELIBS_REG_READWRITE);

    if (NULL == node)
    {
        __wrn("reg node cannot be opened!\n");
        return EPDK_FAIL;
    }

    switch (mMode)
    {
        case MK_SWITCH_NO:
            g_strcpy(key_value, "no");
            break;

        case MK_SWITCH_GLIDE:
            g_strcpy(key_value, "slide");
            break;

        default:
            g_strcpy(key_value, "no");
            break;
    }

    eLIBs_RegSetKeyValue(node, "config", "switch", key_value);
    eLIBs_RegCloseNode(node);
    return EPDK_OK;
}

MkSwitch mkr_getSwitch(void)
{
    char    key_value[ ELIBS_KEY_VALUE_LEN + 1];
    MkSwitch ret;
    __hdle  node;
    node = eLIBs_RegOpenNode(MK_REG_NODE, ELIBS_REG_READWRITE);

    if (NULL == node)
    {
        __wrn("reg node cannot be opened!\n");
        return MK_SWITCH_GLIDE;
    }

    eLIBs_RegGetKeyValue(node, "config", "switch", key_value);

    if (!g_strcmp(key_value, "no"))
    {
        ret = MK_SWITCH_NO;
    }
    else if (!g_strcmp(key_value, "slide"))
    {
        ret = MK_SWITCH_GLIDE;
    }
    else
    {
        ret = MK_SWITCH_GLIDE;
    }

    eLIBs_RegCloseNode(node);
    return ret;
}

/*
__s32   mkr_setDefEncode(MkReg *reg, MkEncode mEncode)
{
    char  key_value[ ELIBS_KEY_VALUE_LEN+1];

    switch( mMode )
    {
        case MK_SWITCH_NO:
            g_strcpy( key_value, "no");
            break;
        case MK_SWITCH_GLIDE:
            g_strcpy( key_value, "slide");
            break;
        default:
            g_strcpy( key_value, "no");
            break;
    }

    eLIBs_RegSetKeyValue(node, "config", "switch", key_value);
    return EPDK_OK;
}

MkEncode mkr_getDefEncode(MkReg *reg)
{

}
*/

__s32   mkr_setBackground(MkBack mBack)
{
    char  key_value[ ELIBS_KEY_VALUE_LEN + 1];
    __hdle  node;
    node = eLIBs_RegOpenNode(MK_REG_NODE, ELIBS_REG_READWRITE);

    if (NULL == node)
    {
        __wrn("reg node cannot be opened!\n");
        return EPDK_FAIL;
    }

    switch (mBack)
    {
        case MK_BACK_1:
            g_strcpy(key_value, "background0");
            break;

        case MK_BACK_2:
            g_strcpy(key_value, "background1");
            break;

        case MK_BACK_3:
            g_strcpy(key_value, "background2");
            break;

        case MK_BACK_4:
            g_strcpy(key_value, "background3");
            break;

        default:
            g_strcpy(key_value, "background0");
            break;
    }

    eLIBs_RegSetKeyValue(node, "config", "background", key_value);
    eLIBs_RegCloseNode(node);
    return EPDK_OK;
}

MkBack  mkr_getBackground(void)
{
    char    key_value[ ELIBS_KEY_VALUE_LEN + 1];
    MkBack  ret;
    __hdle  node;
    node = eLIBs_RegOpenNode(MK_REG_NODE, ELIBS_REG_READWRITE);

    if (NULL == node)
    {
        __wrn("reg node cannot be opened!\n");
        return MK_BACK_1;
    }

    eLIBs_RegGetKeyValue(node, "config", "background", key_value);

    if (!g_strcmp(key_value, "background0"))
    {
        ret = MK_BACK_1;
    }
    else if (!g_strcmp(key_value, "background1"))
    {
        ret = MK_BACK_2;
    }
    else if (!g_strcmp(key_value, "background2"))
    {
        ret = MK_BACK_3;
    }
    else if (!g_strcmp(key_value, "background3"))
    {
        ret = MK_BACK_4;
    }
    else
    {
        ret = MK_BACK_1;
    }

    eLIBs_RegCloseNode(node);
    return ret;
}

__s32   mkr_setBright(MkBright mBright)
{
    char  key_value[ ELIBS_KEY_VALUE_LEN + 1];
    __hdle  node;
    node = eLIBs_RegOpenNode(MK_REG_NODE, ELIBS_REG_READWRITE);

    if (NULL == node)
    {
        __wrn("reg node cannot be opened!\n");
        return EPDK_FAIL;
    }

    switch (mBright)
    {
        case MK_BRIGHT_0:
            g_strcpy(key_value, "0");
            break;

        case MK_BRIGHT_1:
            g_strcpy(key_value, "1");
            break;

        case MK_BRIGHT_2:
            g_strcpy(key_value, "2");
            break;

        case MK_BRIGHT_3:
            g_strcpy(key_value, "3");
            break;

        case MK_BRIGHT_4:
            g_strcpy(key_value, "4");
            break;

        default:
            g_strcpy(key_value, "0");
            break;
    }

    eLIBs_RegSetKeyValue(node, "config", "bright", key_value);
    eLIBs_RegCloseNode(node);
    return EPDK_OK;
}

MkBright mkr_getBright(void)
{
    char    key_value[ ELIBS_KEY_VALUE_LEN + 1];
    MkBright ret;
    __hdle  node;
    node = eLIBs_RegOpenNode(MK_REG_NODE, ELIBS_REG_READWRITE);

    if (NULL == node)
    {
        __wrn("reg node cannot be opened!\n");
        return MK_BRIGHT_2;
    }

    eLIBs_RegGetKeyValue(node, "config", "bright", key_value);

    if (!g_strcmp(key_value, "0"))
    {
        ret = MK_BRIGHT_0;
    }
    else if (!g_strcmp(key_value, "1"))
    {
        ret = MK_BRIGHT_1;
    }
    else if (!g_strcmp(key_value, "2"))
    {
        ret = MK_BRIGHT_2;
    }
    else if (!g_strcmp(key_value, "3"))
    {
        ret = MK_BRIGHT_3;
    }
    else if (!g_strcmp(key_value, "4"))
    {
        ret = MK_BRIGHT_4;
    }
    else
    {
        ret = MK_BRIGHT_2;
    }

    eLIBs_RegCloseNode(node);
    return ret;
}

__s32   mkr_setAutoPage(MkAutoP  mAutoP)
{
    char  key_value[ ELIBS_KEY_VALUE_LEN + 1];
    __hdle  node;
    node = eLIBs_RegOpenNode(MK_REG_NODE, ELIBS_REG_READWRITE);

    if (NULL == node)
    {
        __wrn("reg node cannot be opened!\n");
        return EPDK_FAIL;
    }

    switch (mAutoP)
    {
        case MK_AS_0:
            g_strcpy(key_value, "0");
            break;

        case MK_AS_10:
            g_strcpy(key_value, "10");
            break;

        case MK_AS_15:
            g_strcpy(key_value, "15");
            break;

        case MK_AS_20:
            g_strcpy(key_value, "20");
            break;

        case MK_AS_25:
            g_strcpy(key_value, "25");
            break;

        case MK_AS_30:
            g_strcpy(key_value, "30");
            break;

        default:
            g_strcpy(key_value, "0");
            break;
    }

    eLIBs_RegSetKeyValue(node, "config", "autopage", key_value);
    eLIBs_RegCloseNode(node);
    return EPDK_OK;
}

MkAutoP mkr_getAutoPage(void)
{
    char    key_value[ ELIBS_KEY_VALUE_LEN + 1];
    MkAutoP ret;
    __hdle  node;
    node = eLIBs_RegOpenNode(MK_REG_NODE, ELIBS_REG_READWRITE);

    if (NULL == node)
    {
        __wrn("reg node cannot be opened!\n");
        return MK_AS_0;
    }

    eLIBs_RegGetKeyValue(node, "config", "autopage", key_value);

    if (!g_strcmp(key_value, "0"))
    {
        ret = MK_AS_0;
    }
    else if (!g_strcmp(key_value, "10"))
    {
        ret = MK_AS_10;
    }
    else if (!g_strcmp(key_value, "15"))
    {
        ret = MK_AS_15;
    }
    else if (!g_strcmp(key_value, "20"))
    {
        ret = MK_AS_20;
    }
    else if (!g_strcmp(key_value, "25"))
    {
        ret = MK_AS_25;
    }
    else if (!g_strcmp(key_value, "30"))
    {
        ret = MK_AS_30;
    }
    else
    {
        ret = MK_AS_0;
    }

    eLIBs_RegCloseNode(node);
    return ret;
}
