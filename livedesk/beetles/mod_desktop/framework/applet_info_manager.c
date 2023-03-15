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
#include "mod_desktop_i.h"

#include "mini_xml_builder.h"
#include "mini_xml_parser.h"

struct _AppletInfoManager
{
    __s32 nr;           // 实际节点数目
    __s32 alloc_nr;     // 分配内存数目
    AppletInfo *info;   // 动态数组
};

typedef struct _BuilderInfo
{
    AppletInfoManager *info_manager;
} BuilderInfo;

static __s32  applet_info_manager_add(AppletInfoManager *thiz);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//MiniXmlBuilder 实现
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void applet_info_init(AppletInfo *info)
{
    memset(info, 0, sizeof(AppletInfo));
    info->permission = PERMISSON_;
    info->standby    = EPDK_FALSE;
}

static void applet_info_get_from_tag_application(AppletInfo *info, const char **attrs)
{
    const char  *name;
    const char  *value;
    int i;

    for (i = 0; attrs[i] != NULL; i += 2)
    {
        name = attrs[i];
        value = attrs[i + 1];

        switch (name[0])
        {
            case 'n': //name
                strncpy(info->name, value, sizeof(info->name) - 1);
                break;

            case 'e': //exec path
                strncpy(info->exec, value, sizeof(info->exec) - 1);
                break;

            case 'p': //permission
                if (!strcmp(value, "audio"))
                {
                    info->permission = PERMISSON_AUDIO;
                }
                else
                {
                    info->permission = PERMISSON_;
                }

                break;

            case 's': //standby
                if (!strcmp(value, "yes"))
                {
                    info->standby = EPDK_TRUE;
                }
                else if (!strcmp(value, "no"))
                {
                    info->standby = EPDK_FALSE;
                }
                else
                {
                    __msg("standby type invalid!");
                }

                break;

            default:
                break;
        }
    }
}

static void applet_info_get_from_tag_loading(AppletInfo *info, const char **attrs)
{
    const char  *name;
    const char  *value;
    Suffix *su  = info->loading;
    int i;

    if (info->loading == NULL)
    {
        su = info->loading = (Suffix *)ZALLOC(sizeof(Suffix));
        return_if_fail(su != NULL);
        su->nr = 0;
        su->alloc_nr = 5;
        su->suffix = ZALLOC(32 * su->alloc_nr);
    }

    for (i = 0; attrs[i] != NULL; i += 2)
    {
        name = attrs[i];
        value = attrs[i + 1];

        if (!strcmp(name, "suffix"))
        {
            if (su->nr >= su->alloc_nr)
            {
                su->alloc_nr += 5;
                su->suffix = REALLOC(su->suffix, 32 * su->alloc_nr);
                return_if_fail(su->suffix != NULL);
            }

            memset(su->suffix[su->nr], 0, 32);
            strncpy(su->suffix[su->nr], value, 32);
            su->nr++;
        }
    }
}

static void applet_info_builder_on_start(MiniXmlBuilder *thiz, const char *tag, const char **attrs)
{
    BuilderInfo        *info         = (BuilderInfo *)thiz->priv;
    AppletInfoManager  *info_manager = info->info_manager;
    AppletInfo         *app_info     = info_manager->info + info_manager->nr - 1;

    if (!strcmp(tag, "application"))
    {
        applet_info_get_from_tag_application(app_info, attrs);
    }
    else if (!strcmp(tag, "loading"))
    {
        applet_info_get_from_tag_loading(app_info, attrs);
    }
    else
    {
        return;
    }
}

static void applet_info_builder_on_end(MiniXmlBuilder *thiz, const char *tag)
{
    return;
}

static void applet_info_builder_on_text(MiniXmlBuilder *thiz, const char *text, size_t length)
{
    return;
}

static void applet_info_builder_on_comment(MiniXmlBuilder *thiz, const char *text, size_t length)
{
    return;
}

static void applet_info_builder_on_pi(MiniXmlBuilder *thiz, const char *tag, const char **attrs)
{
    return;
}

static void applet_info_builder_on_err(MiniXmlBuilder *thiz, int line, int row, const char *message)
{
    return;
}

static void applet_info_builder_on_destroy(MiniXmlBuilder *thiz)
{
    if (thiz != NULL)
    {
        ZFREE(thiz, sizeof(MiniXmlBuilder) + sizeof(BuilderInfo));
    }
}

static MiniXmlBuilder *applet_info_builder_create(AppletInfoManager *info_manager)
{
    MiniXmlBuilder *thiz;
    BuilderInfo *priv;
    thiz = ZALLOC(sizeof(MiniXmlBuilder) + sizeof(BuilderInfo));

    if (thiz != NULL)
    {
        priv = (BuilderInfo *)thiz->priv;
        thiz->on_start_element  = applet_info_builder_on_start;
        thiz->on_end_element    = applet_info_builder_on_end;
        thiz->on_text           = applet_info_builder_on_text;
        thiz->on_comment        = applet_info_builder_on_comment;
        thiz->on_pi_element     = applet_info_builder_on_pi;
        thiz->on_error          = applet_info_builder_on_err;
        thiz->destroy           = applet_info_builder_on_destroy;
        priv->info_manager      = info_manager;
    }

    return thiz;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//AppletInfoManager Interface
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AppletInfoManager *applet_info_manager_create(void)
{
    AppletInfoManager   *thiz = (AppletInfoManager *)ZALLOC(sizeof(AppletInfoManager));

    return_val_if_fail(thiz != NULL, NULL);
    thiz->nr        = 0;
    thiz->alloc_nr  = 5;
    thiz->info      = ZALLOC(sizeof(AppletInfo) * (thiz->alloc_nr));
    return_val_if_fail(thiz->info != NULL, NULL);
    return thiz;
}

int32_t  applet_info_manager_parse(AppletInfoManager *thiz, const char *xml, size_t length)
{
    MiniXmlParser   *parser;
    MiniXmlBuilder  *builder;

    parser  = mini_xml_parser_create();
    return_val_if_fail(parser, -1);

    applet_info_manager_add(thiz);

    builder = applet_info_builder_create(thiz);

    if (builder != NULL)
    {
        mini_xml_parser_set_builder(parser, builder);
        mini_xml_parser_parse(parser, xml, length);
    }

    mini_xml_builder_destroy(builder);
    mini_xml_parser_destroy(parser);
    return EPDK_OK;
}

int32_t  applet_info_manager_load_file(AppletInfoManager *thiz, const char *filename)
{
    ES_FILE *fp = NULL;
    char    *data;
    int      len  = 0;

    fp  = esFSYS_fopen(filename, "rb");
    return_val_if_fail(fp != NULL, -1);

    len = eLIBs_GetFileSize(filename);
    __log("filename = %s len=%d",filename, len);

    //return_val_if_fail(len >= 0, -1);
    if (len < 0)
    {
        esFSYS_fclose(fp);
        return -1;
    }

    data = ZALLOC(len + 1);
    return_val_if_fail(data != NULL, -1);
    esFSYS_fseek(fp, 0, SEEK_SET);
    esFSYS_fread(data, 1, len, fp);
    applet_info_manager_parse(thiz, data, len);
    FREE(data);
    esFSYS_fclose(fp);
    return EPDK_OK;
}

int32_t  applet_info_manager_load_dir(AppletInfoManager *thiz, const char *path)
{
    ES_DIR      *dir = NULL;
    ES_DIRENT   *iter = NULL;
    char        filename[260] = {0};

    return_val_if_fail(thiz != NULL && path != NULL, -1);

    dir     = esFSYS_opendir(path);
    return_val_if_fail(dir != NULL, -1);

    while ((iter = esFSYS_readdir(dir)) != NULL)
    {
        if (iter->d_name[0] == '.')
        {
            continue;
        }
        if (strstr(iter->d_name, ".desktop") == NULL)
        {
            continue;
        }
        __log("find dir file %s\\%s!", path, iter->d_name);
        memset(filename, 0, 260);
        strcpy(filename, path);
        strcat(filename, "\\");
        strcat(filename, (const char *)iter->d_name);
        applet_info_manager_load_file(thiz, filename);
    }

    esFSYS_closedir(dir);
    return EPDK_OK;
}

int32_t  applet_info_manager_get_count(AppletInfoManager *thiz)
{
    return_val_if_fail(thiz != NULL, 0);
    return thiz->nr;
}

int32_t  applet_info_manager_get(AppletInfoManager *thiz, __u8 index, AppletInfo **info)
{
    return_val_if_fail(thiz != NULL && index < thiz->nr && info != NULL, -1);
    *info = thiz->info + index;
    return EPDK_OK;
}

int32_t applet_info_manager_foreach(AppletInfoManager *thiz, AppletInfoIter iter, void *ctx1, void *ctx2)
{
    AppletInfo *info = NULL;
    int count, i;
    int ret;
    count = applet_info_manager_get_count(thiz);

    for (i = 0; i < count; i++)
    {
        applet_info_manager_get(thiz, i, &info);
        ret = (*iter)(info, ctx1, ctx2);

        if (ret < 0)
        {
            break;
        }
    }

    return EPDK_OK;
}

static int32_t applet_info_manager_add(AppletInfoManager *thiz)
{
    return_val_if_fail(thiz != NULL, -1);

    if (thiz->nr == thiz->alloc_nr)
    {
        thiz->alloc_nr += 5;
        thiz->info = (AppletInfo *)REALLOC(thiz->info, sizeof(AppletInfo) * (thiz->alloc_nr));
    }

    applet_info_init(thiz->info + thiz->nr);
    thiz->nr++;
    return EPDK_OK;
}

void applet_info_manager_destroy(AppletInfoManager *thiz)
{
    if (thiz != NULL)
    {
        FREE(thiz->info);
        FREE(thiz);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// applet_info_manager 接口测试
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t applet_info_manager_iter_printf(AppletInfo *info, void *ctx1, void *ctx2)
{
    Suffix  *loading = info->loading;
    int     j;
    __inf("name=%s ,exec=%s ,permission=%d ,standby =%d\n", info->name, info->exec, info->permission, info->standby);

    if ((loading != NULL) && (loading->nr > 0))
    {
        for (j = 0; j < loading->nr; j++)
        {
            __inf("surfix=%s\n", loading->suffix[j]);
        }
    }

    return 0;
}

void applet_info_manager_printf(AppletInfoManager *thiz)
{
    applet_info_manager_foreach(thiz, applet_info_manager_iter_printf, NULL, NULL);
}
