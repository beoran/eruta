#
# Run time library for binaries compiled with the Orhalcium compiler
# Works for Linux, but should be made portable.
# 

.equiv _STDOUT    , 1
.equiv _STDIN     , 2
.equiv _STDERR    , 3
.equiv _sys_exit  , 1
.equiv _sys_fork  , 2
.equiv _sys_read  , 3
.equiv _sys_write , 4
.equiv _sys_open  , 5
.equiv _sys_close , 6 
.equiv _sys_brk   , 45


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

.macro sys_brk delta
  sys_call_1 $_sys_brk, \delta
.endm

# We use a Forth-like return stack, and a separate parameter stack
# NEXT jumps to (returns to) the next function to call 
.macro NEXT
  lodsl
  jmp *(%eax)
.endm

.macro PUSHRET reg
  lea -4(%ebp),%ebp // push reg on to return stack
  movl \reg,(%ebp)
.endm

.macro POPRET reg
  mov (%ebp),\reg   // pop top of return stack to reg
  lea 4(%ebp),%ebp
.endm

# Usses a system call to allocate a block of memory with the given size. 
# New address is in %eax. Clobbers %ebx, and anything involved in the syscall.
.macro const_alloc size
  sys_brk $0
  addl    \size, %eax
  movl    %eax , %ebx
  sys_brk  %ebx
.endm   






