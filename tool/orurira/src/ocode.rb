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
  In what follows CPU means the real back end CPU on which the OCODE is run 
  after it has been translated to that CPU's machine code. The OCPU means the 
  virtual "CPU" the OCODE is generated for.
  
  The OCPU supports 3 general kinds of data: integers, addresses, and
  floating point. 

  The OCPU has several general purpose registers which have the same size as the 
  of the natural register size of the CPU. In C parlance, this is sizeof(int).
  THis means, the general purpose registers may be of different sizes 
  depending on the CPU the OCODE is translated to. A value that has the same size 
  of the registers of the OCPU is called a machine word or mword. Allowed sizes 
  for the machine word are 1, 2, 4, 8, ... , (2**n) bytes. A byte is normally 8 
  bits long. Most instructions of  the OCPU handle registers of machine word 
  size only. 
  
  The OCPU has at least 2 general purpose registers, again, depending on the 
  CPU. The registers are named :r<n> where n increases from 0 to
  The two guaranteed general purpose registers are named :r0 and :r1
  
  Furthermore there are several special purpose OCPU registers which 
  may or may not be  identical to certain general purpose registers in the CPU:
  
  :high  : HIGH is the high register. 
           It is used for multiplication and division.
  :low   : LOW is the low register. 
           It is used for multiplication and division.
  :sp    : SP is the stack pointer.
  :ip    : IP is the instruction pointer.  
  :creturn : creturn is the C return register. It's the register that 
             C compilers use on the CPU 

  On the OCPU, all addresses, both those of data and those of executable 
  routines are supposed to fit inside a machine word. Memory is byte addressable, 
  however, memory access for machine word based instructions must be aligned on 
  machine word boundaries.

  Floating point values have a size of at least 32 bits, but are generally of 
  the largest precision supported, whith the limitation that the size of the 
  floating point values must be an integer multiple of the size of the machine 
  word. In C parlance, this is sizeof(double).
 
  The OCPU supports or emulates floating point math. The OCPU  has at least 2 
  floating point registers which are named :f<n> <here n starts at 0. 
  The name oft he two guaranteed floating point registers is f0 and f1 
  
ABSF  # Compute the absolute value of the floating point value
ADDF  # Floating Point Addition
EQF   # Compare the floating point values for equality 
LEF   # Compare floating point values for less than or equal 
LTF   # Compare floating point values for less than
CIF   # Convert floating point value to integer
DIVF  # Compute the quotient of the floating point values
LF    # Load floating point value from memory
MOVF  # Move floating point value between floating point registers
MULF  # Multiply floating point values 
NEGF  # Negate the floating point double value
SF    # Store the floating float value to memory
SUBF  # Substract floating point values
FSTR  # Moves the :fflags register to the designated :rfflag register

=end



class Ocode







end

# reference : 
# http://www.mrc.uidaho.edu/mrc/people/jff/digital/MIPSir.html
# http://www.doc.ic.ac.uk/lab/secondyear/spim/node20.html
# Intel: 
# http://www.penguin.cz/~literakl/intel/intel.html



ADDI  # Add immediate (with overflow)
ADDIU # Add immediate unsigned (no overflow)
ADDU  # Add unsigned (no overflow)
AND   # Bitwise and
ANDI  # Bitwise and immediate

BEQ   # Branch on equal
BGEZ  # Branch on greater than or equal to zero
BGEZAL # Branch on greater than or equal to zero and link
BGTZ  # Branch on greater than zero
BLEZ  # Branch on less than or equal to zero

BLTZ  # Branch on less than zero
BLTZAL # Branch on less than zero and link
BNE   # Branch on not equal
DIV   # Divide
DIVU  # Divide unsigned

J     # Jump
JAL   # Jump and link
JR    # Jump register
LB    # Load byte
LUI   # Load upper immediate

LW    # Load word
MFHI  # Move from HI
MFLO  # Move from LO
MULT  # Multiply
MULTU # Multiply unsigned

NOOP  # no operation
OR    # Bitwise or
ORI   # Bitwise or immediate
SB    # Store byte
SLL   # Shift left logical

SLLV  # Shift left logical variable
SLT   # Set on less than (signed)
SLTI  # Set on less than immediate (signed)
SLTIU # Set on less than immediate unsigned
SLTU  # Set on less than unsigned

SRA   # Shift right arithmetic
SRL   # Shift right logical
SRLV  # Shift right logical variable
SUB   # Subtract
SUBU  # Subtract unsigned

SW    # Store word
SYSCALL # System call
XOR   # Bitwise exclusive or
XORI  # Bitwise exclusive or immediate
