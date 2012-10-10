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
 * @name    xml_reader.c
 * @date:   Mar 15, 2012
 */

#include "xml_reader.h"

/* private code definition */

enum _TokenType {
    OPEN        = L'<',
    CLOSE       = L'>',
    SLASH       = L'/',
    SPACE       = L' ',
    QUOTE       = L'\'',
    DQUOTE      = L'\"',
    EQUAL       = L'=',
    NEW         = L'\n',
    TAB         = L'\t',
    EXCLAMATION = L'!',
    MINUS       = L'-',
    SQRPO       = L'[',
    SQRPC       = L']',
    QM          = L'?',
    RET         = L'\n'
};

/* private code declaration */
static void _installer_xml_reader_append_character(wchar_t *string,
                                                   wchar_t character);

/* public code definition */
XMLReader* installer_xml_reader_new(FILE *pointer) {
    if (pointer == NULL) {
        return NULL;
    }

    XMLReader *reader = (XMLReader*) malloc(sizeof(XMLReader));
    if (reader == NULL) {
        return NULL;
    }

    reader->pointer = pointer;

    return reader;
}

void installer_xml_reader_delete(XMLReader *reader) {
    if (reader == NULL) {
        return;
    }

    free(reader);
}

/* 
 * Parses the XML file in UTF-8 encoding and generates XML AST (abstract syntax
 * tree) by reading the file in loop where each iteration going to process next
 * wide character from buffer.
 *
 * @param reader  the class contains the pointer to the file to parse
 * @retuen        the XML abstract syntax tree
 */
XMLDocument* installer_xml_reader_read(XMLReader *reader) {
    const size_t  STRING_LENGTH = 256;
    const wchar_t *EMPTY_STRING = L"";

    XMLDocument  *document  = installer_xml_document_new();
    XMLAttribute *attribute = NULL;
    XMLElement   *element   = NULL;
	XMLElement   *element_t = NULL;     /* temporaary element */

    /* xml string to parse */
    wchar_t *string         = NULL;

    int     index           = 0;        /* first level cursor */
    wchar_t character       = WEOF;
    wchar_t delim           = WEOF;
    bool    inside_flag     = false;

    wchar_t *element_name   = (wchar_t*) calloc(STRING_LENGTH, sizeof(wchar_t));
    wchar_t *element_value  = (wchar_t*) calloc(STRING_LENGTH, sizeof(wchar_t));
    wchar_t *attribute_name = (wchar_t*) calloc(STRING_LENGTH, sizeof(wchar_t));
    wchar_t *attribute_value= (wchar_t*) calloc(STRING_LENGTH, sizeof(wchar_t));

    long file_length        = 0;        /* file length */
    long character_number   = 0;        /* how much characters in file */
                                        /* character_numner <= file_length */

    /* function argument and variables are can't be NULL */
    if ((reader == NULL) || (document == NULL)
            || (element_name == NULL) || (element_value == NULL)
            || (attribute_name == NULL) || (attribute_value == NULL)) {
        free(document);

        return NULL;
    }

    /* determines length of the xml file and reads this file into buffer */
    fseek(reader->pointer, 0L, SEEK_END);
    file_length = ftell(reader->pointer);
    fseek(reader->pointer, 0L, SEEK_SET);
    string = (wchar_t*) calloc(file_length + 1, sizeof(wchar_t));

    /* file is empty */
    if ((file_length <= 0) || (string == NULL)) {
        return NULL;
    }

	/* add first element to the document */
	element_t = installer_xml_element_new(L"XML DOCUMENT", L"v1.0");
	element   = installer_xml_document_add_element(document, element_t);
	installer_xml_element_delete(element_t);

	element_t = installer_xml_element_new(EMPTY_STRING, EMPTY_STRING);
	element   = installer_xml_element_add_element(element, element_t);
	installer_xml_element_delete(element_t);

    /* fread(string, sizeof(wchar_t), 30, reader->pointer); */
    character   = fgetwc(reader->pointer); /* leave start point */
    while (((character = fgetwc(reader->pointer)) != WEOF)) {
        string[character_number] = character;
        character_number++;
    }

    /* main parser loop */
    while (index < character_number) {
        character = string[index];

        if ((character == NEW) || (character == TAB) || (character == RET)) {
            index++;

            continue;
        }

        if (character == OPEN) {
            /* comment section */
            if ((string[index + 1] == EXCLAMATION)
                    && (string[index + 2] == MINUS)
                    && (string[index + 3] == MINUS)) {
                memset(attribute_value, 0, STRING_LENGTH * sizeof(wchar_t));
                index += 4;
                character = string[index];

                while (!((string[index] == MINUS)
                        && (string[index + 1] == MINUS)
                        && (string[index + 2] == CLOSE))) {
                    _installer_xml_reader_append_character(attribute_name,
                                                           character);
                    character = string[++index];
                }

                index += 3;

                /* 
                 * comments will not be added to xml structure 
                 */
                
                continue;
            }

            /* closing tag for the last opened element */
            memset(element_name, 0, STRING_LENGTH * sizeof(wchar_t));
            if (string[index + 1] == SLASH) {
                /* element = element->parent */
                element = installer_xml_element_get_parent(element);
                index++;

                while (string[index] != CLOSE) {
                    index++;
                }

                index++;

                continue;
            }

            if (string[index + 1] == QM) {
                character = string[++index];
                inside_flag = true;
            }

            /* create a new element */
            character = string[++index];
            while ((character != SLASH)
                    && (character != SPACE)
                    && (character != CLOSE)) {
                /* going until the element name has been entirely read */
                if ((character != NEW)
                        && (character != TAB)
                        && (character != RET)) {
                    _installer_xml_reader_append_character(element_name,
                                                           character);
                }

                character = string[++index];
            }

			/* element has been seted, going to the child element */
            if (wcscmp(installer_xml_element_get_name(element), EMPTY_STRING) != 0) {
				/* element = element->child */
				element_t = installer_xml_element_new(EMPTY_STRING, EMPTY_STRING);
				element   = installer_xml_element_add_element(element, element_t);

				installer_xml_element_delete(element_t);
            }

            /* element->name = element_name */
            installer_xml_element_set_name(element, element_name);
            while (character == SPACE) {
                character = string[++index];

                if (character == SLASH) {
                    break;
                }

                if ((character == QM) && (inside_flag == true)) {
                    break;
                }

                memset(attribute_name,  0, STRING_LENGTH * sizeof(wchar_t));
                memset(attribute_value, 0, STRING_LENGTH * sizeof(wchar_t));
                /* going until the attribute name has been entirely read */
                while (character != EQUAL) {
                    if ((character != NEW)
                            && (character != TAB)
                            && (character != RET)) {
                        _installer_xml_reader_append_character(attribute_name,
                                                               character);
                    }

                    character = string[++index];
                }

                character = string[++index];
                if ((character == QUOTE) || (character == DQUOTE)) {
                    delim     = character;
                    character = string[++index];
                }

                /* going until the attribute value has been entirely read */
                while ((character != delim) && (delim != WEOF)) {
                    if ((character != NEW)
                            && (character != TAB)
                            && (character != RET)) {
                        _installer_xml_reader_append_character(attribute_value,
                                                               character);
                    }

                    character = string[++index];
                }

                delim     = WEOF;
                character = string[++index];
                attribute = installer_xml_attribute_new(attribute_name,
                                                        attribute_value);

                installer_xml_element_add_attribute(element, attribute);
				installer_xml_attribute_delete(attribute);
            }

            if (character == SLASH) {
                /* element = element->parent */
                element = installer_xml_element_get_parent(element);

                character = string[++index];

                while (character != CLOSE) {
                    character = string[++index];
                }
            }

            /* first xml element end <?xml version="1.0" encoding="UTF-8"?> */
            if ((character == QM) && (inside_flag == true)) {
                /* element = element->parent - parent is NULL for the first element */
                element = installer_xml_element_get_parent(element);

                character   = string[++index];
                inside_flag = false;

                while (character != CLOSE) {
                    character = string[++index];
                }
            }
        }

        /* element has a value */
        if ((character != OPEN)
                && (character != CLOSE)
                && (character != SLASH)
                && (character != SPACE)) {
            memset(element_value,  0, STRING_LENGTH * sizeof(wchar_t));

            while (character != OPEN) {
                if ((character != NEW)
                        && (character != TAB)
                        && (character != RET)) {
                    _installer_xml_reader_append_character(element_value,
                                                           character);
                }

                character = string[++index];
            }

            /* element->value = element_value */
            installer_xml_element_set_value(element, element_value);

            continue;
        }

        index++;
    }

    /* free memmory */
    free(attribute_name);
    free(attribute_value);
    free(element_name);
    free(element_value);

    return document;
}

/* private code definition */

/*
 * Appends the character to the end of the string.
 */
void _installer_xml_reader_append_character(wchar_t *string,
                                            wchar_t character) {
    size_t length = wcslen(string);

    string[length]     = character;
    string[length + 1] = L'\0';
}
