/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : mini_xml_buider.h
* By        : Andy.zhang
* Func      : a xml buider
* Version   : v1.0
* ============================================================================================================
* 2010-9-3 9:46:25  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
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
