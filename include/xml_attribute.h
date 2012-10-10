/*
 * The Open Source Unix Installer
 *
 * The MIT License
 *
 * Copyright 2011-2012 Andrey Pudov.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * XML attribute implementation that can be used in linked list.
 * The name and value are wide-char strings, next is a pointer to the next 
 * XML attribute in the list (NULL by default). 
 * The name and value are can not be NULL;
 *
 * @author  Andrey Pudov    <andrey@andreypudov.com>
 * @version 0.00.00
 * @name    xml_attribute.h
 * @date:   Feb 26, 2012
 */

#ifndef INSTALLER_UTIL_XML_ATTRIBUTE_H_
#define INSTALLER_UTIL_XML_ATTRIBUTE_H_

#include <stdlib.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XMLAttribute XMLAttribute;

struct _XMLAttribute {
    wchar_t *name;
    wchar_t *value;

    /* the linke to the next element in the list */
    XMLAttribute *next;
};

XMLAttribute* installer_xml_attribute_new(const wchar_t *name, const wchar_t *value);
void installer_xml_attribute_delete(XMLAttribute *attribute);

wchar_t* installer_xml_attribute_get_name(XMLAttribute *attribute);
wchar_t* installer_xml_attribute_get_value(XMLAttribute *attribute);

void installer_xml_attribute_set_name(XMLAttribute *attribute, const wchar_t *name);
void installer_xml_attribute_set_value(XMLAttribute *attribute, const wchar_t *value);

#ifdef __cplusplus
}
#endif

#endif /* INSTALLER_UTIL_XML_ATTRIBUTE_H_ */
