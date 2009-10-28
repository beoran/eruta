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
.section .data
str: .ascii "Hello!\n"
str_end: 
  .equ str_size, str_end - str
.section .text
.globl _start
.include "orhalib.s"


_multret:
  pop %eax  
  push $2
  push $2
  push %eax
  ret

_start:
  movl        $64, %eax  
  const_alloc %eax
  sys_write   $_STDOUT, $str, $str_size
  call        _multret
  pop         %eax
  pop         %eax
  sys_exit    %eax
 
