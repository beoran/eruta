#
#
# Orurira generates intermediate level code, called ocode 
# for a virtual sort-of-RISC-like cpu, called the OCPU 
# which is then finally translated into real assembly code 
# for the target CPU and OS. The Ocode class generates this ocode 
# at with the given instructions.
# Currently, it only generates x86 assembly from Linux OS.
#
#

=begin  
  In what follows CPU means the real back end CPU, and OCPU means the 
  virtual "CPU" the OCODE is generated for.

  The OCPU has 4 general purpose registers that may be of different sizes depending on the CPU the OCODE is translated to. The size of the registers of the OCPU is called a machine word or mword. Allowed sizes for the machine word are 1, 2, 4, 8, (2**n) bytes. Most instructions of the OCPU handle registers of machine word size only. 
  
  The OCPU has 4 general purpose registers. The four general purpose registers are named :r1, :r2, :r3 and :r4. 
  
  Furthermore there are several special purpose OCPU registers which 
  may or may not be  identical to general purpose registers in the CPU:
  
  :rhigh : rhigh is the high register. 
           It is used for multiplication and division.
  :rlow  : rlow is the low register. 
           It is used for multiplication and division.
  :sp    : SP is the stack pointer.
  :ip    : IP is the instruction pointer.  
  :rcreturn : rcreturn is the C return register. 
              It's the register that C uses one the CPU

  On the OCPU, all addresses are supposed to fit inside a machine word.  

  Furthermore, the OCPU may support floating point math. The 
  OCPU  has 4 floating point registers which have a size that are 
  64 bits wide.
        
        
  
  As an extension, a few instructions that handle bytes
  
  All instructions of the OCPU
  handle  


Floating Point Instructions

The MIPS has a floating point coprocessor (numbered 1) that operates on single precision (32-bit) and double precision (64-bit) floating point numbers. This coprocessor has its own registers, which are numbered $f0-$f31. Because these registers are only 32-bits wide, two of them are required to hold doubles. To simplify matters, floating point operations only use even-numbered registers--including instructions that operate on single floats.

Values are moved in or out of these registers a word (32-bits) at a time by lwc1, swc1, mtc1, and mfc1 instructions described above or by the l.s, l.d, s.s, and s.d pseudoinstructions described below. The flag set by floating point comparison operations is read by the CPU with its bc1t and bc1f instructions.

In all instructions below, FRdest, FRsrc1, FRsrc2, and FRsrc are floating point registers (e.g., $f2).


abs.d FRdest, FRsrcFloating Point Absolute Value Double
abs.s FRdest, FRsrcFloating Point Absolute Value Single
Compute the absolute value of the floating float double (single) in register FRsrc and put it in register FRdest.


add.d FRdest, FRsrc1, FRsrc2Floating Point Addition Double
add.s FRdest, FRsrc1, FRsrc2Floating Point Addition Single
Compute the sum of the floating float doubles (singles) in registers FRsrc1 and FRsrc2 and put it in register FRdest.


c.eq.d FRsrc1, FRsrc2Compare Equal Double
c.eq.s FRsrc1, FRsrc2Compare Equal Single
Compare the floating point double in register FRsrc1 against the one in FRsrc2 and set the floating point condition flag true if they are equal.


c.le.d FRsrc1, FRsrc2Compare Less Than Equal Double
c.le.s FRsrc1, FRsrc2Compare Less Than Equal Single
Compare the floating point double in register FRsrc1 against the one in FRsrc2 and set the floating point condition flag true if the first is less than or equal to the second.


c.lt.d FRsrc1, FRsrc2Compare Less Than Double
c.lt.s FRsrc1, FRsrc2Compare Less Than Single
Compare the floating point double in register FRsrc1 against the one in FRsrc2 and set the condition flag true if the first is less than the second.


cvt.d.s FRdest, FRsrcConvert Single to Double
cvt.d.w FRdest, FRsrcConvert Integer to Double
Convert the single precision floating point number or integer in register FRsrc to a double precision number and put it in register FRdest.


cvt.s.d FRdest, FRsrcConvert Double to Single
cvt.s.w FRdest, FRsrcConvert Integer to Single
Convert the double precision floating point number or integer in register FRsrc to a single precision number and put it in register FRdest.


cvt.w.d FRdest, FRsrcConvert Double to Integer
cvt.w.s FRdest, FRsrcConvert Single to Integer
Convert the double or single precision floating point number in register FRsrc to an integer and put it in register FRdest.


div.d FRdest, FRsrc1, FRsrc2Floating Point Divide Double
div.s FRdest, FRsrc1, FRsrc2Floating Point Divide Single
Compute the quotient of the floating float doubles (singles) in registers FRsrc1 and FRsrc2 and put it in register FRdest.


l.d FRdest, addressLoad Floating Point Double ${}^{\dagger}$
l.s FRdest, addressLoad Floating Point Single ${}^{\dagger}$
Load the floating float double (single) at address into register FRdest.


mov.d FRdest, FRsrcMove Floating Point Double
mov.s FRdest, FRsrcMove Floating Point Single
Move the floating float double (single) from register FRsrc to register FRdest.


mul.d FRdest, FRsrc1, FRsrc2Floating Point Multiply Double
mul.s FRdest, FRsrc1, FRsrc2Floating Point Multiply Single
Compute the product of the floating float doubles (singles) in registers FRsrc1 and FRsrc2 and put it in register FRdest.


neg.d FRdest, FRsrcNegate Double
neg.s FRdest, FRsrcNegate Single
Negate the floating point double (single) in register FRsrc and put it in register FRdest.


s.d FRdest, addressStore Floating Point Double ${}^{\dagger}$
s.s FRdest, addressStore Floating Point Single ${}^{\dagger}$
Store the floating float double (single) in register FRdest at address.


sub.d FRdest, FRsrc1, FRsrc2Floating Point Subtract Double
sub.s FRdest, FRsrc1, FRsrc2Floating Point Subtract Single
Compute the difference of the floating float doubles (singles) in registers FRsrc1 and FRsrc2 and put it in register FRdest. 







=end



class Ocode







end

# reference : 
# http://www.mrc.uidaho.edu/mrc/people/jff/digital/MIPSir.html
# http://www.doc.ic.ac.uk/lab/secondyear/spim/node20.html
# Intel: 
# http://www.penguin.cz/~literakl/intel/intel.html



ADDI # Add immediate (with overflow)
ADDIU # Add immediate unsigned (no overflow)
ADDU # Add unsigned (no overflow)
AND # Bitwise and
ANDI # Bitwise and immediate

BEQ # Branch on equal
BGEZ # Branch on greater than or equal to zero
BGEZAL # Branch on greater than or equal to zero and link
BGTZ # Branch on greater than zero
BLEZ # Branch on less than or equal to zero

BLTZ # Branch on less than zero
BLTZAL # Branch on less than zero and link
BNE # Branch on not equal
DIV # Divide
DIVU # Divide unsigned

J # Jump
JAL # Jump and link
JR # Jump register
LB # Load byte
LUI # Load upper immediate

LW # Load word
MFHI # Move from HI
MFLO # Move from LO
MULT # Multiply
MULTU # Multiply unsigned

NOOP # no operation
OR # Bitwise or
ORI # Bitwise or immediate
SB # Store byte
SLL # Shift left logical

SLLV # Shift left logical variable
SLT # Set on less than (signed)
SLTI # Set on less than immediate (signed)
SLTIU # Set on less than immediate unsigned
SLTU # Set on less than unsigned

SRA # Shift right arithmetic
SRL # Shift right logical
SRLV # Shift right logical variable
SUB # Subtract
SUBU # Subtract unsigned

SW # Store word
SYSCALL # System call
XOR # Bitwise exclusive or
XORI # Bitwise exclusive or immediate
