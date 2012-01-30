------------
 About yXML
------------

yoctoXML (yXML) is a very tiny and fast XML reader/writer.
It allows to parse simple XML data from a string and also to write simple XML data to string.
It allocates memory dynamically, so XML data can be any length.
It uses no additional libraries, only pure C and its standard libraries.
The code of yXML is only about 300 lines in C.
It is available under GPL-compatible modified BSD license (see LICENSE.txt).

Copyright (C) 2009 Alexander G. Karpov a.k.a. mrhx.

------------------------------
 Max speed of reading/writing
------------------------------

Parsing string XML data (yxml_read)     :  37-52 MB/sec  (MB = MBytes = 1024 KB)
Writing XML data to string (yxml_write) :  122 MB/sec

Tested on the following CPU:

model name	 :  AMD Turion(tm) 64 Mobile Technology MK-36
cpu MHz		 :  2000
cache size	 :  512 KB
bogomips	 :  3990.15

Built with GCC 4.3.3 (Ubuntu 4.3.3-5ubuntu4) with "-O3" optimization.

-------------------------------------------------------------
 Visit my web-site for updates and more interesting software
-------------------------------------------------------------

http://codege.org/

--------
 E-mail
--------

admin@codege.org


==========================================================================
 Copyright (C) 2009 Alexander G. Karpov a.k.a. mrhx.
 Created in Russia, Saint Petersburg.
 Date: 2009-08-12
 v1.1: 2009-08-24
==========================================================================

