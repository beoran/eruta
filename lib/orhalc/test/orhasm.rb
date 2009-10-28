# The compiler backend targets a fake RISC-like architercture, 
# in the spirit of MIPS and Gnu Lightning
# This is translated to real assembler instructions and (later) 
# bytecodes for the target CPU.
# The main advantage is that it allows compilation at runtime
# without losing portability.

# add signed
def add(r1,r2,r3)
end
 
# add unsigned
def addu(r1, r2, r3)
end

# add immediate
def addi(r1, r2, c)
end

# add immediate unsigned
def addiu(r1, r2, c)
end

# multiply, moving the reslts to rlow and rhigh
def mult(r1, r2, rlow, rhigh) 
end

# Divide with modulus, storing the results in rdiv and rmod
def divmod(r1, r2, rdiv, rmod)
end
 
# Unsigned divide with modulus, storing the results in rdiv and rmod
def divmodu(r1, r2, rdiv, rmod)
end
 
# Load double word (8 bytes, 64 bit) from regmem + c into r1 and r2 
def ld(r1, r2, regmem, c)
end

# Load word (4 bytes, 32 bit) from regmem + c into r1 
def lw(r1, regmem, c)
end

# Load half word signed (2 bytes, 16 bit) from regmem + c into r1
# With sign extension 
def lh(r1, regmem, c)
end


# Load half word unsigned (2 bytes, 16 bit) from regmem + c into r1
# Without sign extension 
def lhu(r1, regmem, c)
end

# Load byte signed (1 bytes, 8 bit) from regmem + c into r1
# With sign extension 
def lh(r1, regmem, c)
end

# Load byte unsigned (1 bytes, 8 bit) from regmem + c into r1
# Without sign extension 
def lbu(r1, regmem, c)
end

# Store double word 
def sd(r1, r2, regmem, c)
end 
 
# Store word  
def sw(r1, regmem, c) 
end

# Store half word 
def sh(r1, regmem, c) 
end
 
# Store byte 
def sb(r1, regmem, c) 
end

# Load immediate
# loads a 32-bit immediate operand into the register specified.
def  li(r1, c)
end

# inportb, outportb, control registers???

# Bitwise and (registers)
# 
def andr(reg1, reg2, reg3) 
end

# Bitwise and immediate
def andi(reg1, reg2, c) 
end

# Bitwise or (registers)
# 
def orr(reg1, reg2, reg3) 
end
  
# Bitwise or immediate
def ori(reg1, reg2, c) 
end

# Bitwise xor (registers)
# 
def xorr(reg1, reg2, reg3) 
end
  
# Bitwise xor immediate
def xori(reg1, reg2, c) 
end


# Bitwise nor (registers)
# 
def norr(reg1, reg2, reg3) 
end

# Set on less than  
# Tests if one register is less than another.
def slt (reg1, reg2, reg3) 
end
  
# Set on less than immediate
# Tests if one register is less than a constant.  
def slti(reg1, reg2, c) 

# Shift left logical  
# shifts C number of bits to the left (multiplies by 2CONST)
def sll(reg1, reg2, c) 
end

# Shift right logical 
# Shifts CONST number of bits to the right - zeros are shifted in 
#  (divides by 2C). Note that this instruction only works as division 
#  of a two's complement number if the value is positive.
def srl(reg1, reg2, c)
end

# Shift right arithmetic
# shifts C number of bits - the sign bit is shifted in 
# (divides 2's complement number by 2C) 
def sra (reg1, reg2, c) 
end 

# Branch on equal
# if ($s == $t) go to PC+4+4*C  I   416   -   Goes to the instruction at the specified address if two registers are equal.   
def beq(reg1, reg2, rel_addr)   
end

# Branch on not equal   
def bne(reg1, reg2, rel_addr)
end

# Unconditional jump (immediate address)  
def jmpi(addr) 
end

# Unconditional jump (address in register)
def jmpr(reg1) 
end

# Call a function (imediate address)
# Arguments should have been set up per calling conventions.
# Should store return address as needed (stack or register)
def call(addr)
end

# Return from a function
# Results should have been stored per the calling conventions.
# Return address is fetched per the normal CPU's conventions
# and should be ready for use
def ret()
end

# Floating point calculations:
# Add single precision
def adds(fpr1, fpr2, fpr3)
end 
 
# Add Doublme precision 
def addd(fpr1, fpr2, fpr3)
end
 
 # Substract single precision
def subs(fpr1, fpr2, fpr3)
end 
 
# Substract double precision 
def subd(fpr1, fpr2, fpr3)
end 

# Multiply single precision
def muls(fpr1, fpr2, fpr3)
end 
 
# Multiply double precision 
def muld(fpr1, fpr2, fpr3)
end 

# Divide single precision
def divs(fpr1, fpr2, fpr3)
end 
 
# divide double precision 
def divd(fpr1, fpr2, fpr3)
end 

# Logicallly compare (less than)
def lts(fpr1, fpr2)
end

# Logicallly compare (less than)
def ltd(fpr1, fpr2)
end

# Branch if float compare was true 
def bifct(rel_adddress)
end

# Branch if float compare was false 
def bifcf(rel_adddress)
end

# Pseudo instructions
# Load Address  
def la(reg, addr) 
end

# Branch if greater than  
def bgt(reg1, reg2, rel_addr) 
end

# Branch if less than   
def blt(reg1, reg2, rel_addr) 
end

# Branch if greater than or equal   
def bgte(reg1, reg2, rel_addr) 
end

# Branch if less than or equal   
def blte(reg1, reg2, rel_addr) 
end

# Branch if greater than signed  
def bgts(reg1, reg2, rel_addr) 
end

# Branch if less than than unsigned  
def blts(reg1, reg2, rel_addr) 
end

# Branch if greater than or equal unsigned   
def bgteu(reg1, reg2, rel_addr) 
end

# Branch if less than or equal unsigned  
def blteu(reg1, reg2, rel_addr) 
end


