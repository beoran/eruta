# PURPOSE:  Simple test program for orulib.s
#
#

.section .data
str: .ascii "Hello test1!\n"
  .equ str_size, . - str
flo:
  .double 0e1.234
  
.section .text
.globl _start
.type _start, @function
.globl _multret
.type _multret, @function
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
  jmp _jmp_to
  sys_exit    $69
  nop
  nop
  nop
  nop
_jmp_to:  
  sys_write   $_STDOUT, $str, $str_size
  call        _multret
  pop         %eax
  pop         %eax
  sys_exit    %eax
 
