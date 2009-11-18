# PURPOSE:  Simple test program for orulib.s
#
#

.section .data
str: .ascii "Hello!\n"
str_end: 
  .equ str_size, str_end - str
.section .text
.globl _start
.include "orulib.s"

_multret:
  pop %eax  
  push $2
  push $2
  push %eax
  ret

_start:
  subl        $64 , %esp  # allocate stack space
  movl        $64 , %eax
  const_alloc %eax
  sys_write   $_STDOUT, $str, $str_size
  call        _multret
  pop         %eax
  pop         %eax
  sys_exit    %eax
 
