# Locate Lua library
# This module defines
#  MRUBY_FOUND, if false, do not try to link to mruby
#  MRUBY_LIBRARIES
#  MRUBY_INCLUDE_DIR, where to find mruby.h 
#
# Copyright (c) beoran@rubyforge.org 2012
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
FIND_PATH(MRUBY_INCLUDE_DIR mruby.h
  HINTS
  $ENV{MRUBY_DIR}
  PATH_SUFFIXES include/mruby include
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
)

FIND_LIBRARY(MRUBY_LIBRARY 
  NAMES libmruby.a mruby.lib
  HINTS
  $ENV{MRUBY_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

IF(MRUBY_LIBRARY)
  # include the math library for Unix
  IF(UNIX AND NOT APPLE)
    FIND_LIBRARY(MRUBY_MATH_LIBRARY m)
    SET(MRUBY_LIBRARIES "${MRUBY_LIBRARY};${MRUBY_MATH_LIBRARY}" CACHE STRING "MRuby Libraries")
  # For Windows and Mac, don't need to explicitly include the math library
  ELSE(UNIX AND NOT APPLE)
    SET(MRUBY_LIBRARIES "${MRUBY_LIBRARY}" CACHE STRING "MRuby Libraries")
  ENDIF(UNIX AND NOT APPLE)
ENDIF(MRUBY_LIBRARY)

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
# handle the QUIETLY and REQUIRED arguments and set MRUBY_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Mruby  DEFAULT_MSG MRUBY_LIBRARIES MRUBY_INCLUDE_DIR)

MARK_AS_ADVANCED(MRUBY_INCLUDE_DIR MRUBY_LIBRARIES MRUBY_LIBRARY MRUBY_MATH_LIBRARY)

