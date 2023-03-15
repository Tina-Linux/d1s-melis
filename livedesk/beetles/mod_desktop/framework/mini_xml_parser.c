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
#include "mini_xml_parser.h"

struct _MiniXmlParser
{
    const char  *read_ptr;

    int         attrs_nr;
    char        *attrs[2 * MAX_ATTR_NR + 1];

    char        *buffer;
    int         buffer_used;
    int         buffer_total;

    MiniXmlBuilder  *builder;
};

static const char   *strtrim(char *str);
static void mini_xml_parser_parse_entity(MiniXmlParser *thiz);
static void mini_xml_parser_parse_start_tag(MiniXmlParser *thiz);
static void mini_xml_parser_parse_end_tag(MiniXmlParser *thiz);
static void mini_xml_parser_parse_comment(MiniXmlParser *thiz);
static void mini_xml_parser_parse_pi(MiniXmlParser *thiz);
static void mini_xml_parser_parse_text(MiniXmlParser *thiz);
static void mini_xml_parser_reset_buffer(MiniXmlParser *thiz);

MiniXmlParser *mini_xml_parser_create(void)
{
    return (MiniXmlParser *)ZALLOC(sizeof(MiniXmlParser));
}

void mini_xml_parser_set_builder(MiniXmlParser *thiz, MiniXmlBuilder *builder)
{
    thiz->builder = builder;
    return;
}

void mini_xml_parser_parse(MiniXmlParser *thiz, const char *xml, size_t length)
{
    int i = 0;

    enum _State
    {
        STAT_NONE,
        STAT_AFTER_LT,
        STAT_START_TAG,
        STAT_END_TAG,
        STAT_TEXT,
        STAT_PRE_COMMENT1,
        STAT_PRE_COMMENT2,
        STAT_COMMENT,
        STAT_PROCESS_INSTRUCTION,
    } state = STAT_NONE;

    thiz->read_ptr = xml;

    for (; *thiz->read_ptr != '\0' && i < length; thiz->read_ptr++, i++)
    {
        char c = thiz->read_ptr[0];

        switch (state)
        {
            case STAT_NONE:
            {
                if (c == '<')
                {
                    mini_xml_parser_reset_buffer(thiz);
                    state = STAT_AFTER_LT;
                }
                else if (!isspace(c))
                {
                    state = STAT_TEXT;
                }

                break;
            }

            case STAT_AFTER_LT:
            {
                if (c == '?')
                {
                    state = STAT_PROCESS_INSTRUCTION;
                }
                else if (c == '/')
                {
                    state = STAT_END_TAG;
                }
                else if (c == '!')
                {
                    state = STAT_PRE_COMMENT1;
                }
                else if (isalpha(c) || c == '_')
                {
                    state = STAT_START_TAG;
                }
                else
                {
                    mini_xml_builder_on_error(thiz->builder, 0, 0, "unexpected char");
                }

                break;
            }

            case STAT_START_TAG:
            {
                mini_xml_parser_parse_start_tag(thiz);
                state = STAT_NONE;
                break;
            }

            case STAT_END_TAG:
            {
                mini_xml_parser_parse_end_tag(thiz);
                state = STAT_NONE;
                break;
            }

            case STAT_PROCESS_INSTRUCTION:
            {
                mini_xml_parser_parse_pi(thiz);
                state = STAT_NONE;
                break;
            }

            case STAT_TEXT:
            {
                mini_xml_parser_parse_text(thiz);
                state = STAT_NONE;
                break;
            }

            case STAT_PRE_COMMENT1:
            {
                if (c == '-')
                {
                    state = STAT_PRE_COMMENT2;
                }
                else
                {
                    mini_xml_builder_on_error(thiz->builder, 0, 0, "expected \'-\'");
                }

                break;
            }

            case STAT_PRE_COMMENT2:
            {
                if (c == '-')
                {
                    state = STAT_COMMENT;
                }
                else
                {
                    mini_xml_builder_on_error(thiz->builder, 0, 0, "expected \'-\'");
                }
            }
#if 0/*langaojie temp test*/ //need break?
#endif
            case STAT_COMMENT:
            {
                mini_xml_parser_parse_comment(thiz);
                state = STAT_NONE;
                break;
            }

            default:
                break;
        }

        if (*thiz->read_ptr == '\0')
        {
            break;
        }
    }

    return;
}

static void mini_xml_parser_reset_buffer(MiniXmlParser *thiz)
{
    thiz->buffer_used   = 0;
    thiz->attrs_nr      = 0;
    thiz->attrs[0]      = NULL;
    return;
}

static unsigned long mini_xml_parser_strdup(MiniXmlParser *thiz, const char *start, size_t length)
{
    int offset  = -1;

    if ((thiz->buffer_used + length) >= thiz->buffer_total)
    {
        size_t length = thiz->buffer_total + (thiz->buffer_total >> 1) + 128;
        char *buffer = REALLOC(thiz->buffer, length);

        if (buffer != NULL)
        {
            thiz->buffer = buffer;
            thiz->buffer_total = length;
        }
    }

    if ((thiz->buffer_used + length) >= thiz->buffer_total)
    {
        return offset;
    }

    offset = thiz->buffer_used;
    strncpy(thiz->buffer + offset, start, length);

    thiz->buffer[offset + length] = '\0';
    strtrim(thiz->buffer + offset);

    thiz->buffer_used   += length + 1;
    return offset;
}

static void mini_xml_parser_parse_attrs(MiniXmlParser *thiz, char end_char)
{
    int i = 0;

    enum _State
    {
        STAT_PRE_KEY,
        STAT_KEY,
        STAT_PRE_VALUE,
        STAT_VALUE,
        STAT_END,
    } state = STAT_PRE_KEY;

    char value_end      = '\"';
    const char *start   = thiz->read_ptr;

    thiz->attrs_nr = 0;

    for (; *thiz->read_ptr != '\0' && thiz->attrs_nr < MAX_ATTR_NR; thiz->read_ptr++)
    {
        char c = *thiz->read_ptr;

        switch (state)
        {
            case STAT_PRE_KEY:
            {
                if (c == end_char || c == '>')
                {
                    state = STAT_END;
                }
                else if (!isspace(c))
                {
                    state = STAT_KEY;
                    start = thiz->read_ptr;
                }
            }
#if 0/*langaojie temp test*/ //need break?
#endif
            case STAT_KEY:
            {
                if (c == '=')
                {
                    thiz->attrs[thiz->attrs_nr++] = (char *)mini_xml_parser_strdup(thiz, start, thiz->read_ptr - start);
                    state = STAT_PRE_VALUE;
                }

                break;
            }

            case STAT_PRE_VALUE:
            {
                if (c == '\"' || c == '\'')
                {
                    state = STAT_VALUE;
                    value_end = c;
                    start = thiz->read_ptr + 1;
                }

                break;
            }

            case STAT_VALUE:
            {
                if (c == value_end)
                {
                    thiz->attrs[thiz->attrs_nr++] = (char *)mini_xml_parser_strdup(thiz, start, thiz->read_ptr - start);
                    state = STAT_PRE_KEY;
                }
            }

            default:
                break;
        }

        if (state == STAT_END)
        {
            break;
        }
    }

    for (i = 0; i < thiz->attrs_nr; i++)
    {
        thiz->attrs[i] = thiz->buffer + (size_t)(thiz->attrs[i]);
    }

    thiz->attrs[thiz->attrs_nr] = NULL;
    return;
}

static void mini_xml_parser_parse_start_tag(MiniXmlParser *thiz)
{
    enum _State
    {
        STAT_NAME,
        STAT_ATTR,
        STAT_END,
    } state = STAT_NAME;

    char *tag_name      = NULL;
    const char *start   = thiz->read_ptr - 1;

    for (; *thiz->read_ptr != '\0'; thiz->read_ptr++)
    {
        char c = *thiz->read_ptr;

        switch (state)
        {
            case STAT_NAME:
            {
                if (isspace(c) || c == '>' || c == '/')
                {
                    tag_name = (char *)mini_xml_parser_strdup(thiz, start, thiz->read_ptr - start);
                    state = (c != '>' && c != '/') ? STAT_ATTR : STAT_END;
                }

                break;
            }

            case STAT_ATTR:
            {
                mini_xml_parser_parse_attrs(thiz, '/');
                state = STAT_END;
                break;
            }

            default:
                break;
        }

        if (state == STAT_END)
        {
            break;
        }
    }

    tag_name = thiz->buffer + (size_t)tag_name;
    mini_xml_builder_on_start_element(thiz->builder, tag_name, (const char **)thiz->attrs);

    if (thiz->read_ptr[0] == '/')
    {
        mini_xml_builder_on_end_element(thiz->builder, tag_name);
    }

    for (; *thiz->read_ptr != '>' && *thiz->read_ptr != '\0'; thiz->read_ptr++);

    return;
}

static void mini_xml_parser_parse_end_tag(MiniXmlParser *thiz)
{
    char *tag_name      = NULL;
    const char *start   = thiz->read_ptr;

    for (; *thiz->read_ptr != '\0'; thiz->read_ptr++)
    {
        if (*thiz->read_ptr == '>')
        {
            tag_name = thiz->buffer + mini_xml_parser_strdup(thiz, start, thiz->read_ptr - start);
            mini_xml_builder_on_end_element(thiz->builder, tag_name);
            break;
        }
    }

    return;
}

static void mini_xml_parser_parse_comment(MiniXmlParser *thiz)
{
    enum _State
    {
        STAT_COMMENT,
        STAT_MINUS1,
        STAT_MINUS2,
    } state = STAT_COMMENT;

    const char *start   = ++thiz->read_ptr;

    for (; *thiz->read_ptr != '\0'; thiz->read_ptr++)
    {
        char c = *thiz->read_ptr;

        switch (state)
        {
            case STAT_COMMENT:
            {
                if (c == '-')
                {
                    state = STAT_MINUS1;
                }

                break;
            }

            case STAT_MINUS1:
            {
                if (c == '-')
                {
                    state = STAT_MINUS2;
                }
                else
                {
                    state = STAT_COMMENT;
                }

                break;
            }

            case STAT_MINUS2:
            {
                if (c == '>')
                {
                    mini_xml_builder_on_comment(thiz->builder, start, thiz->read_ptr - start - 2);
                    return;
                }
            }

            default:
                break;
        }
    }

    return;
}

static void mini_xml_parser_parse_pi(MiniXmlParser *thiz)
{
    enum _State
    {
        STAT_NAME,
        STAT_ATTR,
        STAT_END
    } state = STAT_NAME;

    char *tag_name      = NULL;
    const char *start   = thiz->read_ptr;

    for (; *thiz->read_ptr != '\0'; thiz->read_ptr++)
    {
        char c = *thiz->read_ptr;

        switch (state)
        {
            case STAT_NAME:
            {
                if (isspace(c) || c == '>')
                {
                    tag_name = (char *)mini_xml_parser_strdup(thiz, start, thiz->read_ptr - start);
                    state = c != '>' ? STAT_ATTR : STAT_END;
                }

                break;
            }

            case STAT_ATTR:
            {
                mini_xml_parser_parse_attrs(thiz, '?');
                state = STAT_END;
                break;
            }

            default:
                break;
        }

        if (state == STAT_END)
        {
            break;
        }
    }

    tag_name = thiz->buffer + (size_t)tag_name;
    mini_xml_builder_on_pi_element(thiz->builder, tag_name, (const char **)thiz->attrs);

    for (; *thiz->read_ptr != '>' && *thiz->read_ptr != '\0'; thiz->read_ptr++);

    return;
}

static void mini_xml_parser_parse_text(MiniXmlParser *thiz)
{
    const char *start   = thiz->read_ptr - 1;

    for (; *thiz->read_ptr != '\0'; thiz->read_ptr++)
    {
        char c = *thiz->read_ptr;

        if (c == '<')
        {
            if (thiz->read_ptr > start)
            {
                mini_xml_builder_on_text(thiz->builder, start, thiz->read_ptr - start);
            }

            thiz->read_ptr--;
            return;
        }
        else if (c == '&')
        {
            mini_xml_parser_parse_entity(thiz);
        }
    }

    return;
}

static void mini_xml_parser_parse_entity(MiniXmlParser *thiz)
{
    /*TODO*/
    return;
}

void mini_xml_parser_destroy(MiniXmlParser *thiz)
{
    if (thiz != NULL)
    {
        FREE(thiz->buffer);
        FREE(thiz);
    }

    return;
}

static const char *strtrim(char *str)
{
    char *p = NULL;

    p = str + strlen(str) - 1;

    while (p != str && isspace(*p))
    {
        *p = '\0';
        p--;
    }

    p = str;

    while (*p != '\0' && isspace(*p))
    {
        p++;
    }

    if (p != str)
    {
        char *s = p;
        char *d = str;

        while (*s != '\0')
        {
            *d = *s;
            d++;
            s++;
        }

        *d = '\0';
    }

    return str;
}
