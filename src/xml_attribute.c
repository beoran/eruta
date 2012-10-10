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
 * @name    xml_attribute.c
 * @date:   Feb 26, 2012
 */

#include "xml_attribute.h"

/* public code definition */
XMLAttribute* installer_xml_attribute_new(const wchar_t *name, 
                                          const wchar_t *value) {
    /* arguments are NULL */                                      
    if ((name == NULL) || (value == NULL)) {
	return NULL;
    }
                                              
    XMLAttribute *attribute = (XMLAttribute*) malloc(sizeof(XMLAttribute));
    if (attribute == NULL) {
        return NULL;
    }

    attribute->name  = wcsdup(name);
    attribute->value = wcsdup(value);

    attribute->next = NULL;

    return attribute;
}

void installer_xml_attribute_delete(XMLAttribute *attribute) {
    if (attribute == NULL) {
        return;
    }

    free(attribute->name);
    free(attribute->value);

    free(attribute);
}

wchar_t* installer_xml_attribute_get_name(XMLAttribute *attribute) {
    if (attribute == NULL) {
        return NULL;
    }

    return attribute->name;
}
wchar_t* installer_xml_attribute_get_value(XMLAttribute *attribute) {
    if (attribute == NULL) {
        return NULL;
    }

    return attribute->value;
}

void installer_xml_attribute_set_name(XMLAttribute *attribute, 
                                      const wchar_t *name) {
    if ((attribute == NULL) || (name == NULL)) {
        return;
    }

    attribute->name = wcsdup(name);
}

void installer_xml_attribute_set_value(XMLAttribute *attribute, 
                                       const wchar_t *value) {
    if ((attribute == NULL) || (value == NULL)) {
        return;
    }

    attribute->value = wcsdup(value);
}
