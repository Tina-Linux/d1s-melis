/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : mini_xml_parser.h
* By        : Andy.zhang
* Func      : a xml parser
* Version   : v1.0
* ============================================================================================================
* 2010-9-3 9:46:25  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __MINI_XML_PARSER__
#define __MINI_XML_PARSER__

#include "mini_xml_builder.h"

#define MAX_ATTR_NR     48

struct  _MiniXmlParser;
typedef struct _MiniXmlParser   MiniXmlParser;

MiniXmlParser *mini_xml_parser_create(void);
void mini_xml_parser_set_builder(MiniXmlParser *thiz, MiniXmlBuilder *builder);
void mini_xml_parser_parse(MiniXmlParser *thiz, const char *xml, size_t length);
void mini_xml_parser_destroy(MiniXmlParser *thiz);

#endif /* __MINI_XML_PARSER__ */
