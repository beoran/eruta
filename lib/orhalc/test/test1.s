#PURPOSE:   Simple program that exits and returns a
#           status code back to the Linux kernel
#
#INPUT:     none
#
#OUTPUT:    returns a status code.  This can be viewed
#           by typing
#
#           echo $?
#
#           after running the program
#
#VARIABLES:
#           %eax holds the system call number
#           %ebx holds the return status
#
.include "orhalib.s"

OR_TAG_STRING=32


# .macro str_def name, value
#   \name : .long OR_TAG_STRING .long 0 .ascii \value
#   LOCAL name_end, name_size 
#   \name_end : 
#   .equ \name_size , \name_end  - \name
# .endm 
# str_def foo, "Hi, Foo"

.macro str_make name name_end name_size value
  \name : .ascii \value
  \name_end: 
  .equ \name_size, \name_end - \name
.endm

str2_val : .ascii "Hi!"
str_make str2 , str2_end, str2_size, str2_val

.altmacro

.macro ustr name value name_size
  .LOCAL name_end
  \name : .ascii \value
  \name_end: 
  .equ \name_size, \name_end - \name
.endm

.noaltmacro

.section .data
str: .ascii "Hi, orhalib!\n"
str_end: 
  .equ str_size, str_end - str
.section .text




_multret:
  pop %eax  
  push $2
  push $2
  push %eax
  ret

.globl _start
_start:
  movl        $64, %eax  
  const_alloc %eax
  sys_write   $_STDOUT, $str2, $str2_size
  call        _multret
  pop         %eax
  pop         %eax
  movl        $0      , %eax
  sys_exit    $0
 
