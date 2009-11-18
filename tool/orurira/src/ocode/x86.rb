class Ocode
  class X86
  # We use AT&T syntax for the order. However, the % and $, etc prefixes are
  # generated automatically
  
  def initialize
    # Register translations for x86 
    @asm          = [] # program text    
    @data         = [] # data definitions
    @regs         = { :r1 => :eax , :r2 => :ebx, :r3 => :ecx , :r3 => :edx  }
    @regs[:high]  = :edx
    @regs[:low]   = :ecx
    @regs[:sp]    = :esp
    @regs[:ip]    = :eip
    @regs[:fp]    = :esi
    osregs        =  { :s0 => :eax, :s1 => :ebx, :s2 => :ecx, :s3 => :edx }
    @regs.merge!(osregs) 
    # Size of the machine word
    @wordsize     = 4    
  end
  
  
  def translate_register(reg)
    return @regs[reg]
  end
  
  def register?(reg)
    trans = translate_register(reg)
    return trans if reg
    return nil
  end
  
  def immediate?(part)
    return part.to_int if part.respond_to?(:to_int)
  end
  
  def part_to_asm(part)
    reg = register?(part)
    if reg
      return "%#{reg}"
    end
    imm = immediate?(part)
    if imm
      return "$#{imm}"
    end  
    # if we get here, it should be a label or a string, so stringify it as is
    return part.to_s    
  end
  
  
  def to_s()
    buf = ""
    for inst in @asm
      i     = inst.dup
      first = i.shift
      buf << '  ' unless first =~ /\A\./ ||  first =~ /\A\_/
      buf << first
      if i.size > 0
        buf << ' '
        buf << i.join(', ')
      end
      buf << "\n"
    end
    return buf    
  end
  
  def data
    asm(".section .data\n")
  end
  
  def text
    asm(".section .text\n")
  end
  
  # adds a line of assembly to the output of the ocode
  def asm(*args)
    addr    = @asm.size * @wordsize
    @asm << args.map { |part| part_to_asm(part) } 
    return addr
  end
    
  # Arithmetic instructions
  # Addition between registers with overflow
  def addr(dest, src1, src2)
    asm(:movl , src1, dest)     
    asm(:addl , src2, dest) 
  end   
  
  # Addition immediate with overflow
  def addi(dest, src1, immediate)
    asm(:movl , src1, immediate)
    asm(:addl , src2, dest)
  end
   
  # Addition between registers without overflow
  def addru(dest, src1, src2)
    
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
    code(:subu, dest, src1, src2)
  end
  
  
  # Logical Instructions
  # Bitwise And Registers    
  def andr(dest, src1, src2)
    code(:and, dest, src1, src2)
  end
  
  # Bitwise And Immediate
  def andi(dest, src1, src2)
    code(:andi, dest, src1, src2)
  end
  
  # Bitwise Nor Registers 
  def norr(dest, src1, src2)
    code(:nor, dest, src1, src2)
  end
  
  # Bitwise Or Registers 
  def orr(dest, src1, src2)
    code(:or, dest, src1, src2)  
  end
  
  # Bitwise Or Immediate 
  def ori(dest, src1, src2)
    code(:ori, dest, src1, src2)
  end
  
  # Bitwise xor Registers  
  def xorr(dest, src1, src2)
    code(:xor, dest, src1, src2)
  end
    
  # Bitwise xor Immediate  
  def xori(dest, src1, src2)
    code(:xori, dest, src1, src2)
  end
  
  # Bit Shifting Instructions
  # Shift left logical registers
  def sllr(dest1, src1, src2)
    code(:ssl, dest, src1, src2)
  end
   
  # Shift left logical immediate
  def slli(dest1, src1, immediate)
    code(:ssli, dest, src1, immediate)
  end
  
  # Shift right arithmetic registers
  def srar(dest1, src1, src2)
    code(:srar, dest, src1, src2)
  end
  
  # Shift right arithmetic immediate
  def srai(dest1, src1, immediate)
    code(:srai, dest, src1, immediate)
  end
  
  # Shift right logical registers
  def srlr(dest1, src1, src2)
    code(:srlr, dest, src1, src2)
  end
  
  # Shift right logical immediate
  def srli(dest1, src1, immediate)
    code(:srli, dest, src1, immediate)
  end
  
  # Comparison Instructions
  
  # Set if Less Than
  def sltr(dest1, src1, src2)
    code(:slt, dest, src1, src2)
  end
  
  # Set if Less Than immediate
  def slti(dest1, src1, src2)
    code(:slti, dest, src1, src2)
  end
  
  # Set if Less Than unsigned
  def sltur(dest1, src1, src2)
    code(:sltu, dest, src1, src2)
  end
  
  # Set if Less Than immediate unsigned
  def sltiu(dest1, src1, src2)
    code(:sltiu, dest, src1, src2)
  end
  
  
  # Branch and Jump Instructions
  # For these instructions, label is the label to jump to 
  # offset is the offset, 
  # and register is the register that the address is read from
  # In other words, the branch address is src1(src2)

  # Branch on Coprocessor flag False
  def bcf(coprocessor, label)
    code(:bcf, coprocessor, label)
  end
  
  # Branch on Coprocessor flag true
  def bct(coprocessor, label)
    code(:bcf, coprocessor, label)
  end
  
  # Branch on Equal to an Immediate
  def beqi(src1, immediate, label)
    code(:beqi, coprocessor, label)
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
  def li(dest, immediate)
    asm(:movl , immediate, dest)
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

  # Generic system call
  def syscall()
    asm(:int, 0x80)
  end
  
  # Break. Raises an exception.
  def brk()
  end
  
  # No operation.  
  def nop
  end  
  
  # label 
  def lbl(label, kind)
    label = label.to_s
    ldef  = label.to_s + ':'
    if kind == :global
      asm(".globl #{label}")
    end  
    asm(ldef)
  end
  
  # Data definition
  def dd(data)
    asm(".ascii \"#{data}\"")
  end
  
  
  end
end  