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
 * @name    xml_reader.h
 * @date:   Mar 15, 2012
 */

#ifndef INSTALLER_UTIL_XML_READER_H_
#define INSTALLER_UTIL_XML_READER_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "xml_document.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _XMLReader XMLReader;

struct _XMLReader {
    FILE *pointer;
};

XMLReader* installer_xml_reader_new(FILE *pointer);
void installer_xml_reader_delete(XMLReader *reader);

XMLDocument* installer_xml_reader_read(XMLReader *reader);

#ifdef __cplusplus
}
#endif

#endif /* INSTALLER_UTIL_XML_READER_H_ */
