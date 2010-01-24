#
# Orurira. 
#
#

=begin
  Orurira stands for Orurira is a Ruby Rike Ranguage. Sorry for that joke. ^_^
  The goal of this project is to create a specification of, and a compiler for compiler for a lanuage that is as close to Ruby as possible, limited only by 
  the imperative that everything i nthe language can compile down to real 
  machine code, without the use of any VM.  The main of goals of the Orurira 
  compiler will be Flexibility, Simplicity, Speed, Compatibility with Ruby, 
  in that order of importance.
  
  Orurira, as a language, will be 100% compatible with Ruby's syntax, without 
  any extensions or emissions to it. However, differences in semantics as well 
  as in in functionality will be deemed  acceptable if compilation of these 
  features is difficult. For example, "eval"-like constructs will not be 
  supported initially. 
  
  When writing a compiler for a high level language, the problem is that many 
  low level primitives need to be implemented to support the high level 
  constructs. In other Ruby implementations, these primitives are are 
  implemented in C, C++, Java, or  assembly, but that is unsatifactory for many reasons. On the other hand, it's conceptually difficult to let Ruby generate 
  the primitives it needs for itself. 
  
  The Ruby to C project is a very useful idea, but still, it is limited in that 
  it compiles Ruby down to C. This means that you're still bound to the C 
  calling conventions, and limited to C's capabilities. That's why I think 
  trying to write primitives for Ruby in a C environment is much like trying 
  to push a square peg in a round hole. It's never a nice fit.
  
  For all these reasons, Orurira as a language, will suport two different modes,
  or dialects if you like. The first dialect is simply the "Ruby" dialect, 
  where the second one is the low level "Alox" dialect.
  
  In the Ruby dialect the semantics of Ruby will be followed as closely as is technically feasible.  The Ruby dialect is enabled by default. For it's support, it uses primitives  that must have been defined in Alox mode.   
  
  The second dialect in Orurira will be the Alox language mode. (Alox stands for
  ALuminum OXide, of which rubies are made. Sorry for this joke, again. ^_^) 
  In the Alox dialect, the syntax is still that of Ruby, but the semantics are
  signicficantly different. Alox is, in a way, a domain specific language that 
  can be used to generate primitivesf for the Ruby mode. But it would be more 
  fair to say that Alox is intended is a dialect of Ruby which is generally 
  useful for low level programming.
  
  Alox language mode is started by writing "alox true", or just Alox by 
  itself at the beginning of a source file. It's turned off again by saying 
  alox false. You can also do alox do ... end.
  
  Alox has the following differences in semantics with Ruby:
   
  * Alox is a statically typed language with type inference. 
  * Contrary to Ruby, Alox is an imperative language. 
  * Modules are only name spaces, and nothing else. They're not mix-ins. 
  * Constants cannot be looked up or caclulated dynamically. They do support
    integer expressions. Also some simple string or symbol expressions are 
    allowed.
  * A def foo(a, b) ... ; outside a class of module generates a real low level
    primitive function.
  * Outside of defs, if, case, while and other control structs are only 
    supported for macro-like features.
  * Types are inferred from the parameters of the functions if you give them
    default values. Otherwise, it's inferred from assignments. You can also 
    define the type of a variable foo by saying "type :foo, Typename".
  * Several built-in types are supported.
  * Alox::Byte is usually 8 bits wide, however on some unusual platforms, a 
    byte may be defined as 6,7, or even 9 bits wide.
  * Alox::I<n> : where n is 1, 2, 3, 4, 8: an integer that takes up exactly 
    n bytes in the CPU's memory.
  * Alox::Int is an Alox::I of of platform-specific guaranteed 
    to be the fastest integer usable on this platform that could be used to also contain a data address. All literal integers in  Alox mode are of this type 
    by default, unless if they are too big to fit. In the latter case, see Alox::I<n>.
  * Alox::U<n>. Unsigned integer types.
  * Alox::Size. Unsigned integer large enough to contain an address, also large
    enough for a maximal size offset(hence the name). Alox::Uint is a synonym.
  * Alox::Float. A floating point of double precision. Size is platform-specific
    It guarantees at least 64 bits of precision. All literal floats are of that type.
  * Alox::Boolean for true, false. Booleans may be set to nil, which is the same 
    as setting them to false.
  * Strings are supported, but only as constant, literal byte strings. 
    More capable strings must be implemented in Alox itself (like strlib in C).  
  * Alox::Address is an address of a low level functuion, stacla, array or 
    string. The value nil is always an address. It may also be set to nil.
  * Arrays are supported, but only homogenous arrays of Alox::Byte, Alox:I<n>,        Alox::Int, Alox::U<n>, Alox::Size or Alox::UInt, Alox::Float, Alox::Address.
    Arrays can be resized dynamically, which will be compiled to heap memory allocation if needed. An arrays is of the type array of Alox::Int by 
    default. Generic arrays are not directly supported.
  * Nil is special: Setting an (unsigned) integer value to nil sets it to a 
    platform  dependant value (perhaps 0). Floats are set to NAN. Booleans 
    to false. Addresses to the special NOWHERE address.
  * Address and Int can be connverted to each other without loss of data. 
    Arrays of Address and Int are compatible with each other.
  * Symbols are only supported in special constructs. Otherwise, they are 
    compiled as byte strings. This limitation is imposed so symbols can 
    be implemented in Alox itself. 
  * Ranges are compiled to static arrays of Int with 2 members.
  * Regexes are compiled as byte strings.
  * When included inside a module, a def is only prefixed with that module's 
    name space.
  * Integers, floats and booleans are always passed by value. Others are 
    normally passed by address. 
  * Classes are compiled down to "static classes", r Stacla for short. A stacla 
    acts a bit like structs in C, or records in Pascal, but a are bit more 
    powerful. Methods of the class are translated to statically called functions that take a of the class's type as it's first (hidden) argument.        
  * All @instance_variables of the class must be defined before use with 
    field declarations like these: field :foo, <default value> 
    or field :foo, Typename. 
  * Inheritance is possibe, but statical. If you say class Foo < Bar, 
    all fiels of Bar are included in Foo in such a way that calling calling
    any method of Bar on any instances of Foo will work. When using inheritance,
    the class Foo will use all methods of Bar that have not been overridden.
    constructors, like initialize, must be called automatically. Super works, 
    but again, only statically looks up the method to use at compile time.
  * Staclass suports inlining of their members. If you use an address as a 
    member, that adddress is merely a reference. However, arrays or other 
    staclas may be "inlined" into another staclass, so the resulting stacla 
    isa contiguous block of memory. Inline arrays are supported. An inline 
    array may either be statical, or, if it is dynamical, depend on the value 
    of another member of the stacla. (XXX: explain this better after 
    implementing it).  
  * All operators that are also available in C, work like they do in C.   
  * You can include assembly in a special format.
  * There are special constructs for system calls.
  * There are special constructs for interfacing with C, to be defined later 
    (FFI or Ruby/DL like???)
      
  Design of the compiler: 
  
  The compiler will perform several passes. 
  1) The parser and lexer is, for now at least, the exellent ruby_parser gem of 
  Zenspider.  The s-expressions generated are preprocessed, and then sent to 
  the right compiler for each dialect (Ruby or Alox).   
  2) The compiler then processes the syntax tree to a more specific semantic 
  tree for it's own semantics.
  3) The semantic tree from either dialect is compiled to virtual assembly, 
     and to an information file. The information file a bit like Pascal's 
     module file. It registers information about the compiled unit
  4) The virtual assembly is translated to real assembly, and then to object 
     code.
  5) If needed, linking is performed.
        
  Ocode, the Virtual Assembly:      

  Orurira generates intermediate level vitual asssembly code, called OCODE. 
  This ocode targets a virtual sort-of-RISC-like, MIPS and Gnu Lightening 
  inspired machine, the OCPU. OCODE is then translated into real assembly code 
  for the target CPU and OS. The Ocode class generates this ocode with the 
  given instructions. Currently, it only generates x86 assembly from Linux OS.
  
  In what follows CPU means the real back end CPU on which the translated real assembly is to be run after the ocode has been translated to it. The OCPU 
  means the virtual "CPU" the OCODE is generated for.
  
  The OCPU supports 4 general kinds of data: integers, data addresses, 
  function addresses, and floating point numbers.
   
  The OCPU has a main processor that can work with integers and both kinds of addresses, and one or more coprocessors. Every coprocessor has a flag 
  register that cannot be read directly, but which can be branched on by using 
  BCT or BCF instructions.  

  The OCPU main processor has registers that are large enough to contain the 
  largest posssible data address and function address, and the natural size of 
  the CPU's registers, whichever is larger. A register or memory location that 
  is large enough to contain these   
  
  In C parlance, 
   
  typedef void *     or_data_ptr ; 
  typedef or_data_ptr (*or_func_ptr)(or_data_ptr) ;        
  union or_cell_ {    
    or_data_ptr data;
    or_func_ptr func;
    int         num;
  };
  typedef union or_cell_ or_cell;

  
  On many popular contemporary CPU's, the size of these three is the same. 
  However, on some big iron or embedded systems, the size of the two kinds of addresses may differ. Or the size of addresses may be bigger than the natural register size. In the name of simplicity, the OCPU abstracts away these differences, even when it leads to much slower performance. This means, the general purpose registers may be of different sizes depending on the CPU the OCODE is translated to. A value that has the  same sizeof the registers of the OCPU is called a machine word or mword, abbreviated as W. 
  Allowed sizes for the machine word are 1, 2, 4, 8, ... , (2**n) bytes. A byte 
  is normally 8 bits long, a  lthough uit may also be 9 or 7 or 6 on some 
  strange architectures. Most instructions of the OCPU handle registers of 
  machine word size only.  
  
  Integer values that are of different size of the machine word are described 
  as i<amount of bytes> i1, i2, i4, i8, i16, i32, i64, i128. Alternative 
  names for these are respectively byte (B), short (S), medium (M), large (L), 
  huge (H), gigantic (G), enormous (E), and astronomical (A). I hope we'll never need the last two, though. ^_^
  
  The OCPU has at least 4 general purpose registers, again, depending on the
  CPU. Unlike MIPS architecture, there is no zero register. The general
  purpose are named :r<n> where n increases from 1 to <amount_of_registers>
  The 4 guaranteed general purpose registers are named :r1, :r2, :r3, :r4
  
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
  :s0 .. :sn : the registers that syscall needs to be filled in to           

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
 
  The OCPU supports or emulates floating point math. The OCPU  has at least 4 
  floating point registers which are named :f<n> <here n starts at 1. 
  The name of the guaranteed floating point registers is :f1, :f2, :f3, :f4
   
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
  
  
  The layout of low level objects in memory, be it on the stack or on the heap, 
  is inspired by EBML, where the size and ID are however encoded as a single 
  cell, contrary to the variable-length integer strategy of EBML.
  
  CELL    = OCPU_WORD
  OBJECT  = ELEMENT*
  ELEMENT = ID SIZE DATA
  ID      = CELL
  SIZE    = CELL
  DATA    = VALUE / ELEMENT *
  VALUE   = INT / UINT / FLOAT / STRING / BINARY / DATA_ADDRESS / REL_ADDRESS / FUNC_ADDRESS / EXTRA
  INT     = CELL
  UINT    = CELL
  STRING  = CELL{(SIZE/SIZE_OF_CELL)-1} 0_FILLED_CELL
  BINARY  = CELL{(SIZE/SIZE_OF_CELL)}
  EXTRA   = CELL{(SIZE/SIZE_OF_CELL)}

   

  References used for writing this doc: 
  
  MIPS: 
  http://www.mrc.uidaho.edu/mrc/people/jff/digital/MIPSir.html
  http://www.doc.ic.ac.uk/lab/secondyear/spim/node20.html
  http://www.doc.ic.ac.uk/lab/secondyear/spim/node14.html
  
  Intel: 
  http://www.penguin.cz/~literakl/intel/intel.html


=end



class Ocode
  autoload :Linux   , 'ocode/linux'
  autoload :I386    , 'ocode/i386'

  attr_reader :cpu
  attr_reader :os

  def initialize(cpuclass = I386, osclass = Linux)
    @ocode        = []
    @cpu          = cpuclass.new
    @os           = osclass.new
  end
  
  # Adds and instruction to the o-code being generated. Retunrns the 
  # address of the o-code added. 
  def code(*args)
    addr    = @ocode.size
    @ocode << args
    return addr
  end
  
  def to_s
    buf = ''
    for op in @ocode
      o    = op.dup
      buf << o.shift
      if o.size > 0
        buf << ' '
        buf << o.join(', ')
      end
      buf << "\n"
    end
    return buf
  end
  
  def to_cpu
    for o in @ocode
      @cpu.send(*o)
    end    
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
    code(:bgez, src1, label)
  end
  
  # Branch on Greater than or Equal to Zero and Link
  def bgezal(src1, label)
    code(:bgezal, src1, label)
  end
  
  # Branch on Less than or Equal to Zero
  def blez(src1, label)
    code(:blez, src1, label)
  end
  
  # Branch on Less thanZero
  def bltz(src1, label)
    code(:bltz, src1, label)
  end
  
  # Branch on Less than Zero And Link
  def bltzal(src1, label)
    code(:bltzal, src1, label)
  end
  
  # Branch on Not Equal to an Immediate
  def bnei(src1, immediate, label)
    code(:bnei, src1, label)
  end
  
  # Jump Unconditionally
  def jmp(label)
    code(:jmp, label)
  end 
  
  # Jump And Link
  def jal(label)  
    code(:jal, label)
  end
  
  # Jump Register
  def jr(src1) 
    code(:jr, src1)
  end
  
  # Jump Register And Link
  def jral(src1) 
    code(:jral, src1, label)
  end
    
  # Load and Store instructions
  # Addresses are defined by a constant offset in offset, from
  # an addres in register.
  
  # Load Byte
  def lb(dest, offset, address)
    code(:lb , dest  , offset, address)
  end
   
  # Load Byte Unsigned
  def lbu(dest, offset, address)
    code(:lbu , dest  , offset, address)
  end
  
  # Load Immediate (word)
  def li(dest, immediate)
    code(:li , dest  , immediate)
  end
  
  # Load Short
  def ls(dest, offset, address)
    code(:ls , dest  , offset, address)
  end  
  
  # Load Short Unsigned
  def lsu(dest, offset, address)
    code(:lsu , dest  , offset, address)
  end
   
  # Load Medium
  def lm(dest, offset, address)
    code(:lm , dest   , offset, address)
  end  
  
  # Load Medium Unsigned
  def lmu(dest, offset, address)
    code(:lmu , dest  , offset, address)
  end
  
  # Load Word
  def lw(dest, offset, address)
    code(:lw , dest  , offset, address)
  end
  
  # Load Word to named Coprocessor register
  def lwc(coprocessor_dest       , offset, address)
    code(:lwc , coprocessor_dest , offset, address)
  end
  
  # Store Byte
  def sb(src1, offset, address)
    code(:sb , src1  , offset, address)
  end
  
  # Store Short
  def ss(src1, offset, address)
    code(:ss , src1  , offset, address)
  end
      
  # Store Medium (32 bits)
  def sm(src1, offset, address)
    code(:sm , src1  , offset, address)
  end
  
  # Store Word
  def sw(src1, offset, address)
    code(:sw , src1  , offset, address)
  end
  
  # Store Word from Coprocessor
  def swc(coprocessor_src1      , offset, address)
    code(:swc , coprocessor_src1, offset, address)
  end
  
  # Data Movement Instructions
  
  # Move From Coprocessor register
  def mfc(dest, src1, coprocessor = 1) 
    code(:mfc , dest, src1, coprocessor)
  end
  
  # Move From :High
  def mfhi(dest)
    code(:mfhi, dest)
  end
  
  # Move From :Low
  def mflo(dest)
    code(:mflo, dest) 
  end
  
  # Move to Coprocessor register
  def mtc(dest, src1, coprocessor = 1) 
    code(:mtc , dest, src1, coprocessor)
  end
  
  # Move to :High
  def mthi(src1)
    code(:mthi, src1)
  end
  
  # Move to :Low
  def mtlo(src1)
    code(:mtlo, src1)
  end
    
  # Floating point coprocessor 
  # Compute the absolute value of the floating point value
  def absf(dest, src1)
    code(:absf, dest, src1)
  end
  
  # Floating Point Addition
  def addf(dest, src1, src2)
    code(:addf, dest, src1, src2)
  end
  
  # Compare the floating point values for equality. Sets the coprocessor's flag.
  def eqf(src1, src2)
    code(:eqf, src1, src2)
  end
  
  # Compare floating point values for less than or equal
  def lef(src1, src2)  
    code(:lef, src1, src2)
  end
  
  # Compare floating point values for less than
  def ltf(src1, src2)
    code(:ltf, src1, src2)
  end
      
  # Convert floating point value to integer (truncating it)
  def cif(dest, src1)
    code(:cif, dest, src1)
  end 
  
  # Compute the quotient of the floating point values
  def divf(dest, src1, src2) 
    code(:divf, dest, src1, src2)
  end
  
  # Move floating point value between floating point registers
  def movf(dest, src1)
    code(:movf, dest, src1)
  end
  
  # Multiply floating point values
  def mulf(dest, src1, src2)
    code(:mulf, dest, src1, src2)
  end
  
  # Negate the floating point double value
  def negf(dest, src1)
    code(:negf, dest, src1)
  end
  
  # Substract floating point values  
  def subf(dest, src1, src2)
    code(:subf, dest, src1, src2)
  end
  
  # Exception and Trap Instructions
  # Restore From Exception
  def rfe()
    code(:rfe)
  end

  # Generic system call
  def syscall()
    code(:syscall)
  end
  
  # Break. Raises an exception.
  def brk(imediate=1)
    code(:brk, immediate)
  end
  
  # No operation.  
  def nop
    code(:nop)
  end
  
  # End of instructions that CPU must provide 
  
  # C Language Interface XXX: needs to be worked out. 
  def callc() # Call C Function.
    code(:callc)
  end
   
  
  # Pseudo Instructions
  # Defines a label for the B*, J and JAL instructions to jump to
  # The label is global by defult, pass :local as second param 
  # for a local label 
  def lbl(label, kind = :global)
    kind ||= global
    code(:lbl, label, kind)
  end
  
  # Define data. Data is assembled into this address
  # Switch to data mode first with data, then switch back to program text mode 
  # with text
  def dd(data)
    code(:dd, data)
  end
  
  def data
    code(:data)
  end
  
  def text
    code(:text)
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
  
  
  # Specific system calls 
  def sys_exit
  end
  
  def sys_fork
  end
  
  def sys_read
  end
  
  def sys_write
  end
  
  def sys_open
  end
  
  def sys_close
  end
  
  # Allocates memory  
  def sys_alloc
  end
  
 

end

=begin
# how to use mmap: ...

..section .data
..comm mmap_, 24

..section .text
..globl _start
_start:
#mmap_ is a struct which holds parameters for mmap kernel call
movl $0, mmap_ # I don't have preferences, just need memory somewhere
movl $64, mmap_+4 # length of requested memory
movl $3, mmap_+8 # read, write, PROT_WRITE | PROT_READ, 0x02, 0x01

# error left in to demonstrate "error handler" :)
movl $0x20, mmap_+12 # map anonymously

# movl $0x21, mmap_+12 # map anonymously | MAP_SHARED
# thanks, Chuck!

movl $-1, mmap_+16 # fd, -1 for portability
movl $0, mmap_+20 # offset is ignored

movl $90, %eax # mmap number of syscall
lea mmap_, %ebx #loading address of struct to %ebx
int $0x80 # syscall
cmpl $0xFFFFF000, %eax
ja error

movl %eax, %edx # probably result of syscall is in %eax
movl $0, (%edx) #testing memory we got... this falls in segfault

xorl %eax, %eax # claim no error

exit:
movl %eax, %ebx #code
movl $1, %eax #exit
int $0x80

# fancy error handler
error:
negl %eax
jmp exit
=end
if $0 == __FILE__
  o = Ocode.new
  SYS_EXIT  = 1
  SYS_WRITE = 4
  STDOUT    = 1
  o.data
  o.lbl(:_str, :local)
  s = 'Hello Orurira!\n'
  o.dd(s)
  o.text
  o.lbl(:_start)  
  o.li(:s0, SYS_WRITE)
  o.li(:s1, STDOUT)
  o.li(:s2, :$_str)
  o.li(:s3, s.size)
  o.syscall  
  o.li(:s0, SYS_EXIT)
  o.li(:s1, 0)
  o.syscall
  
  
  puts o.to_s
  o.to_cpu
  puts o.cpu.to_s
  OUTNAME = "ocode_test.s"
  OBJNAME = "ocode_test.o"
  EXENAME = "ocode_test"
  fout    = File.open(OUTNAME, "w+")
  fout.write(o.cpu.to_s)
  fout.close
  system("as -g #{OUTNAME} -o #{OBJNAME}")
  system("ld #{OBJNAME} -o #{EXENAME}")
  system("./#{EXENAME}")
  
  

end


