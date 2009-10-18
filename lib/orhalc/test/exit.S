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

.equiv _STDOUT    , 1
.equiv _STDIN     , 2
.equiv _STDERR    , 3
.equiv _sys_exit  , 1
.equiv _sys_fork  , 2
.equiv _sys_read  , 3
.equiv _sys_write , 4
.equiv _sys_open  , 5
.equiv _sys_close , 6

.macro sys_call
  int   $0x80
.endm  

.macro sys_call_0 fun
  movl  \fun, %eax
  sys_call
.endm

.macro sys_call_1 fun, arg1
  movl  \arg1 , %ebx
  sys_call_0 \fun
.endm

.macro sys_call_2 fun, arg1, arg2
  movl  \arg2 , %ecx
  sys_call_1 \fun, \arg1
.endm

.macro sys_call_3 fun, arg1, arg2, arg3
  movl  \arg3 , %edx
  sys_call_2 \fun, \arg1, \arg2
.endm

.macro sys_call_4 fun, arg1, arg2, arg3, arg4
  movl  \arg4 , %edi
  sys_call_3 \fun, \arg1, \arg2, \arg3
.endm



.macro sys_write fileno, str, str_size 
  sys_call_3 $_sys_write, \fileno, \str, \str_size
.endm

.macro sys_exit return 
  sys_call_1 $_sys_exit, \return
.endm

# We use a Forth-like return stack, and a separate parameter stack
# NEXT jumps to (returns to) the next function to call 
.macro NEXT
  lodsl
  jmp *(%eax)
.endm

.macro PUSHRET reg
  lea -4(%ebp),%ebp	// push reg on to return stack
  movl \reg,(%ebp)
.endm

.macro POPRET reg
  mov (%ebp),\reg		// pop top of return stack to reg
  lea 4(%ebp),%ebp
.endm



_multret:
  pop %eax  
  push $2
  push $2
  push %eax
  ret

_start:
 sys_write $_STDOUT, $str, $str_size
 call _multret
 pop %eax
 pop %eax
 sys_exit %eax
 
