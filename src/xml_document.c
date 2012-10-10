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
 * @name    xml_document.c
 * @date:   Mar 13, 2012
 */

#include "xml_document.h"

/* private code declaration */
static void _installer_xml_document_elements_clear(XMLElement *elements);

/* public code definition */
XMLDocument* installer_xml_document_new() {
    XMLDocument *document = (XMLDocument*) malloc(sizeof(XMLDocument));
    if (document == NULL) {
        return NULL;
    }

    /* the list of elements is empty */
    document->elements = NULL;

    return document;
}

void installer_xml_document_delete(XMLDocument *document) {
    if (document == NULL) {
        return;
    }

    _installer_xml_document_elements_clear(document->elements);

    free(document);
}

XMLElement* installer_xml_document_add_element(XMLDocument *document,
                                        XMLElement *element) {
    XMLElement *head = NULL;
    
    wchar_t *name  = NULL;
    wchar_t *value = NULL;

    if ((document == NULL) || (element == NULL)) {
        return NULL;
    }
    
    head = document->elements;

    name  = installer_xml_element_get_name(element);
    value = installer_xml_element_get_value(element);

    /* the list of elements is empty */
    if (document->elements == NULL) {
        document->elements = installer_xml_element_new(name, value);

        /* the pointer to the new attribute  */
        head = document->elements;
    } else {
        /* going through the list until NULL element */
        while (head->next != NULL) {
            head = head->next;
        }

        head->next = installer_xml_element_new(name, value);
        head->next->parent = head; /* sets parent to the new element */

        /* the pointer to the new element */
        head = head->next;
    }

    return head;
}

XMLElement* installer_xml_document_get_element(XMLDocument *document,
                                                 XMLElement *element) {
    if (document == NULL) {
        return NULL;
    }

    /* return first element of the list */
    if (element == NULL) {
        return document->elements;
    }

    /* have no more elements in the list */
    if (element->next == NULL) {
        return NULL;
    }

    return (element->next);
}

/* private code definition */
void _installer_xml_document_elements_clear(XMLElement *element) {
    if (element == NULL) {
        return;
    }

    /* the element have a child */
    if (element->next != NULL) {
        _installer_xml_document_elements_clear(element->next);
    }

    installer_xml_element_delete(element);
}
