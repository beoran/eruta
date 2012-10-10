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
 * @name    xml_element.c
 * @date:   Mar 03, 2012
 */

#include "xml_element.h"

/* private code declaration */
static void _installer_xml_element_attributes_clear(XMLAttribute *attribute);

/* public code definition */
XMLElement* installer_xml_element_new(const wchar_t *name, 
                                      const wchar_t *value) {
    /* arguments are can't be NULL */
    if ((name == NULL) || (value == NULL)) {
        return NULL;
    }

    XMLElement *element = (XMLElement*) malloc(sizeof(XMLElement));
    if (element == NULL) {
        return NULL;
    }

    element->name  = wcsdup(name);
    element->value = wcsdup(value);

    /* the list of attributes is empty */
    element->attributes = NULL;

    /* the list of childs is empty */
    element->parent   = NULL;
    element->elements = NULL;

    element->next = NULL;

    return element;
}

void installer_xml_element_delete(XMLElement *element) {
    if (element == NULL) {
        return;
    }

    free(element->name);
    free(element->value);

    _installer_xml_element_attributes_clear(element->attributes);

    free(element);
}

/* 
 * Appends the specified attribute to the end of the attributes list. 
 * The attributes stored in linked-list where last element linked to the NULL.
 * 
 * @param  element the element where attribute should be appended
 * @param  child   the attribute to be appended to the list
 * @return         the attribute appended to the list
 */
XMLAttribute* installer_xml_element_add_attribute(XMLElement *element, 
                                                  XMLAttribute *attribute) {
    XMLAttribute *head = NULL;

    wchar_t *name  = NULL;
    wchar_t *value = NULL;

    if ((element == NULL) || (attribute == NULL)) {
        return NULL;
    }
    
    head = element->attributes;

    name  = installer_xml_attribute_get_name(attribute);
    value = installer_xml_attribute_get_value(attribute);

    /* the list of attributes is empty */
    if (element->attributes == NULL) {
        element->attributes = installer_xml_attribute_new(name, value);

        /* the pointer to the new attribute  */
        head = element->attributes;
    } else {
        /* going through the list until NULL element */
        while (head->next != NULL) {
            head = head->next;
        }

        head->next = installer_xml_attribute_new(name, value);

        /* the pointer to the new attribute */
        head = head->next;
    }

    return head;
}

/* 
 * Appends the specified element to the end of the elements list.
 * The elements stores in linked-list where last element linked to the NULL. 
 * 
 * @param  element the element where child element should be appended
 * @param  child   the element to be appended to the list
 * @return         the element appended to the list
 */
XMLElement* installer_xml_element_add_element(XMLElement *element, 
                                              XMLElement *child) {
    XMLElement *head = NULL;

    wchar_t *name  = NULL;
    wchar_t *value = NULL;

    if ((element == NULL) || (child == NULL)) {
        return NULL;
    }
    
    head = element->elements;

    name  = installer_xml_element_get_name(child);
    value = installer_xml_element_get_value(child);

    /* the list of elements is empty */
    if (element->elements == NULL) {
        element->elements = installer_xml_element_new(name, value);

        /* the pointer to the new element */
      head = element->elements;
    } else {
        /* going through the list until NULL element */
        while (head->next != NULL) {
            head = head->next;
        }

        head->next = installer_xml_element_new(name, value);

        /* the pointer to the new element */
        head = head->next;
    }

    /* set parent element to new created child element */
	installer_xml_element_set_parent(head, element);

    return head;
}

XMLAttribute* installer_xml_element_get_attribute(XMLElement *element, 
                                                  XMLAttribute *attribute) {
    if (element == NULL) {
        return NULL;
    }

    /* return first elemen of the list */
    if (attribute == NULL) {
        return element->attributes;
    }

    /* have no more attributes in the list */
    if (attribute->next == NULL) {
        return NULL;
    }

    return (attribute->next);
}

XMLElement* installer_xml_element_get_element(XMLElement *element, 
                                              XMLElement *child) {
    if (element == NULL) {
        return NULL;
    }

    /* return first elemen of the list */
    if (child == NULL) {
        return element->elements;
    }

    /* have no more attributes in the list */
    if (child->next == NULL) {
        return NULL;
    }

    return (child->next);
}

XMLElement* installer_xml_element_get_parent(XMLElement* element) {
    if (element == NULL) {
	    return NULL;
	}

    return (element->parent);
}

wchar_t* installer_xml_element_get_name(XMLElement *element) {
    if (element == NULL) {
        return NULL;
    }

    return element->name;
}

wchar_t* installer_xml_element_get_value(XMLElement *element) {
    if (element == NULL) {
        return NULL;
    }

    return element->value;
}

void installer_xml_element_set_parent(XMLElement* element,
                                      XMLElement *parent) {
    if (element == NULL) {
        return;
    }

    element->parent = parent;
}

void installer_xml_element_set_name(XMLElement *element, 
                                    const wchar_t *name) {
    if ((element == NULL) || (name == NULL)) {
        return;
    }

    element->name = wcsdup(name);
}

void installer_xml_element_set_value(XMLElement *element, 
                                     const wchar_t *value) {
    if ((element == NULL) || (value == NULL)) {
        return;
    }

    element->value = wcsdup(value);
}

/* private code definition */
void _installer_xml_element_attributes_clear(XMLAttribute *attribute) {
    if (attribute == NULL) {
        return;
    }

    /* the attribute have a child */
    if (attribute->next != NULL) {
        _installer_xml_element_attributes_clear(attribute->next);
    }

    installer_xml_attribute_delete(attribute);
}
