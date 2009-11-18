#
#
#
#

=begin
  Orurira generates intermediate level code, called OCODE for a virtual sort-of-RISC-like, MIPS-inspired machine, the OCPU. OCODE is then translated 
  into real assembly code for the target CPU and OS. The Ocode class generates 
  this ocode  at with the given instructions. Currently, it only generates 
  x86 assembly from Linux OS.
  
  In what follows CPU means the real back end CPU on which the OCODE is run 
  after it has been translated to that CPU's machine code. The OCPU means the 
  virtual "CPU" the OCODE is generated for.
  
  The OCPU supports 3 general kinds of data: integers, addresses, and
  floating point.
   
  The OCPU has a main processor for integer values, and one or more 
  coprocessors. Every coprocessor has a flag register that cannot be read
  firectly, but which can be branched on by using BCT or BCF instructions.  

  The OCPU main processor has several general purpose registers which have the 
  same size as the of the natural register size of the CPU. In C parlance, this 
  is sizeof(int). This means, the general purpose registers may be of different sizes depending on the CPU the OCODE is translated to. A value that has the 
  same sizeof the registers of the OCPU is called a machine word or mword, abbreviated as W. 
  Allowed sizes for the machine word are 1, 2, 4, 8, ... , (2**n) bytes. A byte 
  is normally 8 bits long. Most instructions of the OCPU handle registers of machine word size only.  
  
  Integer values that are of different size of the machine word are described 
  as i<amount of bits> i8, i16, i32, i64, i128, i256, i512, i1024. Alternative 
  names for these are respectively byte (B), short (S), medium (M), large (L), 
  huge (H), gigantic (G), enormous (E), and astronomical (A). I hope we'll never need the last two, though. ^_^    
  
  The OCPU has at least 4 general purpose registers, again, depending on the 
  CPU. Unlike MIPS architecture, there is no zero register. The general 
  purpose are named :r<n> where n increases from 1 to <amount_of_registers>  
  The 4 guaranteed general purpose registers are named :r1, :r2, r3, r4
  
  Furthermore there are several special purpose OCPU registers which 
  may or may not be  identical to certain general purpose registers in the CPU:
  
  :high  : HIGH is the high register. 
           It is used for multiplication and division.
  :low   : LOW is the low register. 
           It is used for multiplication and division.
  :sp    : SP is the stack pointer.
  :ip    : IP is the instruction pointer.
  :link  : Link register, where (return) addresses are stored.  
  :creturn : creturn is the C return register. It's the register that 
             C compilers use on the CPU 

  On the OCPU, all addresses, both those of data and those of executable 
  routines are supposed to fit inside a machine word. Memory is byte 
  addressable, however, memory access for machine word based instructions 
  must be aligned on machine word boundaries. 
  
  Addresses must be defined using the LBL pseudo-instruction, which takes the symbolic name of the address as it's operand.
  
  The OCPU has the same endianness as the CPU.
  
  Floating point values have a size of at least 32 bits, but are generally of 
  the largest precision supported, whith the limitation that the size of the 
  floating point values must be an integer multiple of the size of the machine 
  word. In C parlance, this is sizeof(double).
 
  Conceptually, floating point is performed by coprocessor number 1.
 
  The OCPU supports or emulates floating point math. The OCPU  has at least 2 
  floating point registers which are named :f<n> <here n starts at 1. 
  The name of the two guaranteed floating point registers is :f1 and :f2
   
  The OCPU supports the following instructions. Instructions normally have 3 operands. Unlike real MIPS, OCPU immediate operands can be of the full mword 
  size. Generally operands are in the order dest, src1, src2.
  
  Arithmetic Instructions
  ADD   # Addition with overflow
  ADDI  # Addition Immediate with overflow
  ADDU  # Addition Unsigned without overflow
  ADDIU # Addition Immediate Unsigned without overflow
  DIV   # Divide, quotient in :low, remainder in :hi
  DIVU  # Divide Unsigned, quotient in :low, remainder in :hi
  MULT  # Multiply, low word in :low, high word in :high
  MULTU # Multiply Unsigned, low word in :low, high word in :high
  SUB   # Substract with overflow
  SUBU  # Substract Unsigned without overflow  
  
  Logical Instructions
  AND   # Bitwise And 
  ANDI  # Bitwise And Immediate
  NOR   # Bitwise Nor
  OR    # Bitwise Or
  ORI   # Bitwise Or Immediate
  XOR   # Bitwise Xor
  XORI  # Bitwise Xor Immediate
  
  Bit Shifting Instructions
  SLL   # Shift left logical Register
  SLLI  # Shift left logical Immediate
  SRA   # Shift right arithmetic Register    
  SRAI  # Shift right arithmetic Immediate  
  SRL   # Shift right logical Register
  SRLI  # Shift right logical Immediate
  
  
  Comparison Instructions
  SLT   # Set if Less Than 
  SLTI  # Set if less Than Immediate
  SLTU  # Set if less Than Unsigned 
  SLTIU # Set if less Than Immediate Unsigned
  
  Branch and Jump Instructions
  BCT   # Branch Coprocessor True
  BCF   # Branch Coprocessor False
  BCF   # Branch if Coprocessor flag is False
  BCT   # Branch if Coprocessor flag is True
  BEQ   # Branch on Equal
  BGEZ  # Branch on Greater than or Equal to Zero
  BGEZAL# Branch on Greater than or Equal to Zero and Link
  BGTZ  # Branch on Greater than Zero
  BLEZ  # Branch on Less than or Equal to Zero
  BLTZ  # Branch on Less than Zero
  BLTZAL# Branch on Less than Zero and Link
  BNE   # Branch on Not Equal
  JMP   # Jump Unconditionally
  JAL   # Jump And Link
  JR    # Jump Register
  JRAL  # Jump Register And Link
  
  Load and Store instructions
  Addresses are defined by a constant offset from an addres in a register.  
  
  LB    # Load Byte
  LBU   # Load Byte Unsigned  
  LI    # Load Immediate (word) 
  LS    # Load Short 
  LSU   # Load Short Unsigned
  LM    # Load Medium (only if the mword size > Medium size)
  LMU   # Load Medium Unsigned (only if the mword size > Medium size)
  LW    # Load Word
  LWC   # Load Word to Coprocessor
  SB    # Store Byte
  SS    # Store Short
  SM    # Store Medium Unsigned (only if the mword size > Medium size )
  SW    # Store Word
  SWC   # Store Word from Coprocessor 
  
  Data Movement Instructions
  
  MFC   # Move From Coprocessor register
  MFHI  # Move From :High
  MFLO  # Move From :Low
  MTC   # Move To Coprocessor register
  MTLO  # Move To :High
  MTLO  # Move To :Low
  
  Exception and Trap Instructions
  RFE   # Restore From Exception
  SYSTEM# System call
  BREAK # Break. Raises an exception. 
  NOP   # No operation.
  
  C Language Interface XXX: needs to be worked out. 
  CALLC # Call C Function. 
  
  Pseudo Instructions
  LBL   # Defines a label for the B*, J and JAL to jump to
  
  The OCPU floating point coprocessor supports the following instructions. Instructions normally have 3 operands. Comparison operators set the 
  coprocessor's flag register that cannnot be read, but that can be used with 
  BCT or BCF to branch based on the result. 
    
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

  References used for writing this doc: 
  
  MIPS: 
  http://www.mrc.uidaho.edu/mrc/people/jff/digital/MIPSir.html
  http://www.doc.ic.ac.uk/lab/secondyear/spim/node20.html
  http://www.doc.ic.ac.uk/lab/secondyear/spim/node14.html
  
  Intel: 
  http://www.penguin.cz/~literakl/intel/intel.html


=end



class Ocode

  def initialize
    @ocode        = []
    @asm          = [] 
    # Register translations for x86 
    @regs         = { :r1 => :eax , :r2 => :ebx, :r3 => :ecx , :r3 => :edx  }
    @regs[:high]  = :edx
    @regs[:low]   = :ecx
    @regs[:sp]    = :esp
    @regs[:ip]    = :eip
    @regs[:fp]    = :esi
    # Size of the machine word
    @wordsize     = 4
  end
  
  # Adds and instruction to the o-code being generated. Retunrns the 
  # address of the o-code added. 
  def code(*args)
    addr    = @ocode.size * @wordsize 
    @ocode << args
    return addr
  end
  
  # adds a line of assembly to the output of the ocode
  def asm(*args)
    addr    = @asm.size * @wordsize
    @asm << args
    return addr
  end
  
  def immediate?(src)
    return src.is_a? Fixnum
  end
  
  # Dispatches the method to an immediate or register call depending on 
  # src2
  def ior(rname, iname, dest, src1, src2)
    if immediate(src2)
      return self.send(iname, dest, src1, src1)  
    else
      return self.send(rname, dest, src1, src1)
    end    
  end

  # Arithmetic instructions
  # Addition between registers with overflow
  def addr(dest, src1, src2)
    code(:add  , dest, src1, src2) 
  end   
  
  # Addition immediate with overflow
  def addi(dest, src1, immediate)
    code(:addi  , dest, src1, src2)
  end
   
  # Addition between registers without overflow
  def addru(dest, src1, src2)
    code(:addu  , dest, src1, src2) 
  end   
  
  # Addition immediate with overflow
  def addiu(dest, src1, immediate)
    code(:addiu , dest, src1, src2)
  end 

  # Divide signed, quotient in :low, remainder in :high
  def divhl(src1, src2)
    code(:div, src1, src2)  
  end
  
  # Divide unsigned, quotient in :low, remainder in :high
  def divhlu(src1, src2)
    codeu(:div, src1, src2)
  end
  
  # Multiply signed, result in :low, overflow :high
  def multhl(src1, src2)
    code(:mult, src1, src2)  
  end
  
  # Multiply signed, result in :low, overflow :high
  def multlu(src1, src2)
    code(:multu, src1, src2)
  end
  
  # Substract with overflow
  def subr(dest, src1, src2)
    code(:sub, dest, src1, src2)
  end
  
  # Substract unsigned without overflow
  def subru(dest, src1, src2)
    code(:sub, dest, src1, src2)
  end
  
  
  # Logical Instructions
  # Bitwise And Registers    
  def andr(dest, src1, src2)
  end
  
  # Bitwise And Immediate
  def andi(dest, src1, src2)
  end
  
  # Bitwise Nor Registers 
  def norr(dest, src1, src2)
  end
  
  # Bitwise Or Registers 
  def orr(dest, src1, src2)
  end
  
  # Bitwise Or Immediate 
  def ori(dest, src1, src2)
  end
  
  # Bitwise xor Registers  
  def xorr(dest, src1, src2)
  end
    
  # Bitwise xor Immediate  
  def xori(dest, src1, src2)
  end
  
  # Bit Shifting Instructions
  # Shift left logical registers
  def sllr(dest1, src1, src2)
  end
   
  # Shift left logical immediate
  def slli(dest1, src1, immediate)
  end
  
  # Shift right arithmetic registers
  def srar(dest1, src1, src2)
  end
  
  # Shift right arithmetic immediate
  def srai(dest1, src1, immediate)
  end
  
  # Shift right logical registers
  def srlr(dest1, src1, src2)
  end
  
  # Shift right logical immediate
  def srli(dest1, src1, immediate)
  end
  
  # Comparison Instructions
  
  # Set if Less Than
  def sltr(dest1, src1, src2)
  end
  
  # Set if Less Than immediate
  def slti(dest1, src1, src2)
  end
  
  # Set if Less Than unsigned
  def sltur(dest1, src1, src2)
  end
  
  # Set if Less Than immediate unsigned
  def sltiu(dest1, src1, src2)
  end
  
  
  # Branch and Jump Instructions
  # For these instructions, label is the label to jump to 
  # offset is the offset, 
  # and register is the register that the address is read from
  # In other words, the branch address is src1(src2)

  # Branch on Coprocessor flag False
  def bcf(coprocessor, label)
  end
  
  # Branch on Coprocessor flag true
  def bct(coprocessor, label)
  end
  
  # Branch on Equal to an Immediate
  def beqi(src1, immediate, label)
  end
  
  # Branch on Greater than or Equal to Zero
  def bgez(src1, label)
  end
  
  # Branch on Greater than or Equal to Zero and Link
  def bgezal(src1, label)
  end
  
  # Branch on Less than or Equal to Zero
  def blez(src1, label)
  end
  
  # Branch on Less thanZero
  def bltz(src1, label)
  end
  
  # Branch on Less than Zero And Link
  def bltzal(src1, label)
  end
  
  # Branch on Not Equal to an Immediate
  def bnei(src1, immediate, label)
  end
  
  # Jump Unconditionally
  def jmp(label)
  end 
  
  # Jump And Link
  def jal(label)  
  end
  
  # Jump Register
  def jr(src1) 
  end
  
  # Jump Register And Link
  def jral(src1) 
  end
  
  
  # Load and Store instructions
  # Addresses are defined by a constant offset in offset, from
  # an addres in register.
  
  # Load Byte
  def lb(dest, offset, address)
  end
   
  # Load Byte Unsigned
  def lbu(dest, offset, address)
  end
  
  # Load Immediate (word)
  def li(dest, offset, address)
  end
  
  # Load Short
  def ls(dest, offset, address)
  end  
  
  # Load Short Unsigned
  def lsu(dest, offset, address)
  end
   
  # Load Medium
  def lm(dest, offset, address)
  end  
  
  # Load Medium Unsigned
  def lmu(dest, offset, address)
  end
  
  # Load Word
  def lw(dest, offset, address)
  end
  
  # Load Word to named Coprocessor register
  def lwc(coprocessor_dest, offset, address)
  end
  
  # Store Byte
  def sb(dest, offset, address)
  end
  
  # Store Short
  def ss(dest, offset, address)
  end
      
  # Store Medium
  def sm(dest, offset, address)
  end
  
  # Store Word
  def sw(dest, offset, address)
  end
  
  # Store Word from Coprocessor
  def swc(coprocessor_dest, offset, address)
  end
  
  # Data Movement Instructions
  
  # Move From Coprocessor register
  def mfc(dest, src1, coprocessor = 1) 
  end
  
  # Move From :High
  def mfhi(dest)
  end
  
  # Move From :Low
  def mflo(dest)
  end
  
  # Move to Coprocessor register
  def mtc(dest, src1, coprocessor = 1) 
  end
  
  # Move to :High
  def mthi(src1)
  end
  
  # Move to :Low
  def mtlo(src1)
  end
    
  # Floating point coprocessor 
  # Compute the absolute value of the floating point value
  def absf(dest, src1)
  end
  
  # Floating Point Addition
  def addf(dest, src1, src2)
  end
  
  # Compare the floating point values for equality. Sets the coprocessor's flag.
  def eqf(src1, src2)
  end
  
  # Compare floating point values for less than or equal
  def lef(src1, src2)  
  end
  
  # Compare floating point values for less than
  def ltf(src1, src2)
  end
      
  # Convert floating point value to integer (truncating it)
  def cif(dest, src1)
  end 
  
  # Compute the quotient of the floating point values
  def divf(dest, src1, src2) 
  end
  
  # Move floating point value between floating point registers
  def movf(dest, src1)
  end
  
  # Multiply floating point values
  def mulf(dest, src1, src2)
  end
  
  # Negate the floating point double value
  def negf(dest, src1)
  end

  
  # Substract floating point values  
  def subf(dest, src1, src2)
  end
  
  # Exception and Trap Instructions
  # Restore From Exception
  def rfe()
  end

  # System call
  def system()
  end
  
  # Break. Raises an exception.
  def brk()
  end
  
  # No operation.  
  def nop
  end  
  
  # C Language Interface XXX: needs to be worked out. 
  def callc() # Call C Function.
  end
   
  
  # Pseudo Instructions
  # Defines a label for the B*, J and JAL instructions to jump to
  def lbl(label)
  end
  
  # Short hand functions, for covenience.
  # These synthesise instructions from those OCODE supports 
  # Add immediate or registers to destination with overflow
  def add(dest, src1, src2)
    ior(:addr, :addi, dest, src1, src2)
  end
   
  # Add immediate or registers to destination without overflow
  def addu(dest, src1, src2)
    ior(:addru, :addiu, dest, src1, src2)
  end
  
  
  # Divide, quotient is moved to dest
  def div(dest, src1, src2)
    divhl(src1, src2)
    mvlo(dest)
  end
  
  # Divide unsigned, quotient is moved to dest
  def divu(dest, src1, src2)
    divhlu(src1, src2)
    mvlo(dest)
  end
  
  # Modulus 
  def mod(dest, src1, src2)
    divhl(src1, src2)
    mvhi(dest)
  end
  
  # Modulus unsigned
  def modu(dest, src1, src2)
    divhlu(src1, src2)
    mvhi(dest)
  end

  # Multiply, overflow is ignored
  def mul(dest, src1, src2)
    multhl(src1, src2)
    mvlo(dest)
  end
  
  # Multiply unsigned, overflow is ignored
  def mulu(dest, src1, src2)
    multhlu(src1, src2)
    mvlo(dest)
  end
  
  # Substract immediate with overflow
  def subi(dest, src1, immediate)
    li(dest, immediate)
    subr(dest, src1, immediate)
  end
  
  # Substract immediate without overflow
  def subiu(dest, src1, immediate)
    li(dest, immediate)
    subru(dest, src1, immediate)
  end
  
  # Substract immediate or registers to destination with overflow
  def sub(dest, src1, src2)
    ior(:subr, :subi, dest, src1, src2)
  end
   
  # Substract immediate or registers to destination without overflow
  def subu(dest, src1, src2)
    ior(:subru, :subiu, dest, src1, src2)
  end
  
  # Binary and for immeidates and registers
  def and(dest, src1, src2)
    ior(:andr, :andi, dest, src1, src2)
  end
  
  # Bitwise Nor Immediate 
  def nori(dest, src1, immediate)
    li(dest, immediate)
    norr(dest, src1, dest)    
  end
    
  # Bitwise Not for registers (immediates doesn't make much sense)
  def not(dest, src1)
    nori(dest, src1, 0)
  end
  
  # Binary nor for immediates and registers
  def nor(dest, src1, src2)
    ior(:norr, :nori, dest, src1, src2)
  end  
  
  # Binary or for immediates and registers
  def or(dest, src1, src2)
    ior(:orr, :ori, dest, src1, src2)
  end
  
  # Binary xor for immediates and registers
  def xor(dest, src1, src2)
    ior(:xorr, :xori, dest, src1, src2)
  end
  
  # Shorthand: Load floating point value from memory
  def lf(dest, offset, address)
  end
 
  # Shorthand: Store the floating float value to memory
  def sf(dest, offset, address)
  end
    
  

end



