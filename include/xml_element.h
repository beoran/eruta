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
 * @author  Andrey Pudov    <andrey@andreypudov.com>
 * @version 0.00.00
 * @name    xml_element.h
 * @date:   Mar 02, 2012
 */
 
/*
 * The diagram of stored structure
 *
 *              +--------+
 *              | Parent |
 *              +--------+
 *          / /     |      \   (elements/parent, parent, parent)   
 * +-------+    +-------+    +-------+ (next)
 * | Child | -  | Child | -  | Child | ------> NULL;
 * +-------+    +-------+    +-------+
 */

#ifndef INSTALLER_UTIL_XML_ELEMENT_H_
#define INSTALLER_UTIL_XML_ELEMENT_H_

#include <stdlib.h>
#include <wchar.h>

#include "xml_attribute.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XMLElement     XMLElement;

struct _XMLElement {
    wchar_t *name;
    wchar_t *value;
    
    XMLAttribute *attributes;
   
    /* the link to the parent element in the list */ 
    XMLElement *parent;
    /* the link to the next child element in the list */
    XMLElement *elements;
   
    /* the link to the next element in the list */ 
    XMLElement *next;
};

XMLElement* installer_xml_element_new(const wchar_t *name, 
                                      const wchar_t *value);
void installer_xml_element_delete(XMLElement *element);

XMLAttribute* installer_xml_element_add_attribute(XMLElement *element,
                                                  XMLAttribute *attribute);
XMLElement*   installer_xml_element_add_element(XMLElement *element,
                                                XMLElement *child);

XMLAttribute* installer_xml_element_get_attribute(XMLElement *element, 
                                                  XMLAttribute *attribute);
XMLElement*   installer_xml_element_get_element(XMLElement *element, 
                                                XMLElement *child);
XMLElement*   installer_xml_element_get_parent(XMLElement *element);                                                
                                                  
wchar_t* installer_xml_element_get_name(XMLElement *element);
wchar_t* installer_xml_element_get_value(XMLElement *element);

void installer_xml_element_set_parent(XMLElement *element,
                                      XMLElement *parent);

void installer_xml_element_set_name(XMLElement *element, 
                                    const wchar_t *name);
void installer_xml_element_set_value(XMLElement *element, 
                                     const wchar_t *value);

#ifdef __cplusplus
}
#endif

#endif /* INSTALLER_UTIL_XML_ELEMENT_H_ */
