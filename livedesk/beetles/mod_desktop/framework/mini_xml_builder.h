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
#ifndef __MINI_XML_BUILDER_H__
#define __MINI_XML_BUILDER_H__


__BEGIN_DECLS__

struct _MiniXmlBuilder;
typedef struct _MiniXmlBuilder MiniXmlBuilder;

typedef void (*MiniXmlBuilderOnStartElementFunc)(MiniXmlBuilder *thiz, const char *tag, const char **attrs);
typedef void (*MiniXmlBuilderOnEndElementFunc)(MiniXmlBuilder *thiz, const char *tag);
typedef void (*MiniXmlBuilderOnTextFunc)(MiniXmlBuilder *thiz, const char *text, size_t length);
typedef void (*MiniXmlBuilderOnCommentFunc)(MiniXmlBuilder *thiz, const char *text, size_t length);
typedef void (*MiniXmlBuilderOnPiElementFunc)(MiniXmlBuilder *thiz, const char *tag, const char **attrs);
typedef void (*MiniXmlBuilderOnErrorFunc)(MiniXmlBuilder *thiz, int line, int row, const char *message);
typedef void (*MiniXmlBuilderDestroyFunc)(MiniXmlBuilder *thiz);

struct _MiniXmlBuilder
{
    MiniXmlBuilderOnStartElementFunc on_start_element;
    MiniXmlBuilderOnEndElementFunc   on_end_element;
    MiniXmlBuilderOnTextFunc         on_text;
    MiniXmlBuilderOnCommentFunc      on_comment;
    MiniXmlBuilderOnPiElementFunc    on_pi_element;
    MiniXmlBuilderOnErrorFunc        on_error;
    MiniXmlBuilderDestroyFunc        destroy;

    char priv[1];
};

static inline void mini_xml_builder_on_start_element(MiniXmlBuilder *thiz, const char *tag, const char **attrs)
{
    return_if_fail(thiz != NULL && thiz->on_start_element != NULL);
    thiz->on_start_element(thiz, tag, attrs);
    return;
}

static inline void mini_xml_builder_on_end_element(MiniXmlBuilder *thiz, const char *tag)
{
    return_if_fail(thiz != NULL);

    if (thiz->on_end_element != NULL)
    {
        thiz->on_end_element(thiz, tag);
    }

    return;
}

static inline void mini_xml_builder_on_text(MiniXmlBuilder *thiz, const char *text, size_t length)
{
    return_if_fail(thiz != NULL);

    if (thiz->on_text != NULL)
    {
        thiz->on_text(thiz, text, length);
    }

    return;
}

static inline void mini_xml_builder_on_comment(MiniXmlBuilder *thiz, const char *text, size_t length)
{
    return_if_fail(thiz != NULL);

    if (thiz->on_comment != NULL)
    {
        thiz->on_comment(thiz, text, length);
    }

    return;
}

static inline void mini_xml_builder_on_pi_element(MiniXmlBuilder *thiz, const char *tag, const char **attrs)
{
    return_if_fail(thiz != NULL);

    if (thiz->on_pi_element != NULL)
    {
        thiz->on_pi_element(thiz, tag, attrs);
    }

    return;
}

static inline void mini_xml_builder_on_error(MiniXmlBuilder *thiz, int line, int row, const char *message)
{
    return_if_fail(thiz != NULL);

    if (thiz->on_error != NULL)
    {
        thiz->on_error(thiz, line, row, message);
    }

    return;
}

static inline void mini_xml_builder_destroy(MiniXmlBuilder *thiz)
{
    if (thiz != NULL && thiz->destroy != NULL)
    {
        thiz->destroy(thiz);
    }

    return;
}

__END_DECLS__

#endif /* __MINI_XML_BUILDER_H__ */
