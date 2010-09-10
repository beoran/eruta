
#define NORI_INTERN_ONLY
#include "nori_intern.h"

typedef uint32_t NoriU32;
typedef int32_t  NoriI32;
typedef double   NoriF64;
typedef uint8_t  NoriByte;
typedef void *   NoriPtr;

union NoriReg_;

typedef union NoriReg_ NoriReg;

/** A register is a single cell in the virtual machine's frames */
union NoriReg_ {
  NoriPtr     ptr;
  NoriI32     i32;
  NoriF64     f64;
};


/** A frame is a runtime environment for a single function/method/closure 
in the virtual machine */
struct NoriFrame_;

typedef struct NoriFrame_ NoriFrame; 
 
#define NORI_FRAME_REGS 256

/** Special Zero register. */
#define NORI_REG_ZERO 0 
/** Normal registers. (Up to 24 allowed) */
#define NORI_REG_BASE 1 
#define NORI_REG_R1   1
#define NORI_REG_R2   2
#define NORI_REG_R3   3
#define NORI_REG_R4   4
#define NORI_REG_R5   5
#define NORI_REG_R6   6
#define NORI_REG_R7   7
#define NORI_REG_R8   8
#define NORI_REG_R9   9
#define NORI_REG_R10  10
#define NORI_REG_R11  11
#define NORI_REG_R12  12
#define NORI_REG_R13  13
#define NORI_REG_R14  14
#define NORI_REG_R15  15
#define NORI_REG_R16  16
#define NORI_REG_R17  17
#define NORI_REG_R18  18
#define NORI_REG_R19  19
#define NORI_REG_R20  20
#define NORI_REG_R21  21
#define NORI_REG_R22  22
#define NORI_REG_R23  23
#define NORI_REG_R24  24
/* Registers 25 though 31 are special */
#define NORI_REG_SPECIAL_BASE 25
#define NORI_REG_SP   31

/** Register base for arguments this function was called with (up to 32 allowed). */
#define NORI_REG_MYARG_BASE 32
#define NORI_REG_MYARG(ARG)   (NORI_REG_MYARG_BASE   + (ARG))
/** Register base for results to be returned from this function (up to 32 allowed). */
#define NORI_REG_MYRES_BASE 64
#define NORI_REG_MYRES(RES)   (NORI_REG_MYRES_BASE   + (RES))

/** Register base for arguments to be passed to called functions (up to 32 allowed). */
#define NORI_REG_ARG_BASE 96
#define NORI_REG_ARG(ARG)     (NORI_REG_ARG_BASE     + (ARG))

/** Register base for results from called functions (up to 32 allowed). */
#define NORI_REG_RES_BASE 128
#define NORI_REG_RES(RES)     (NORI_REG_RES_BASE     + (RES))

/** Register base for local variables of this frame. (up to 96 allowed). */
#define NORI_REG_LOCAL_BASE 156
#define NORI_REG_LOCAL(LOCAL) (NORI_REG_LOCAL_BASE    + (LOCAL))

struct NoriVm_;
typedef struct NoriVm_ NoriVm;

 
struct NoriFrame_ {
  NoriVm * vm;
  // Backpointer to the VM
  /** The registers */
  NoriReg regs[NORI_FRAME_REGS];
  /** Every frame has the bytecode of the function it can execute, 
   *  and it's own IP. This will make fibers easier to implement. 
   */
  NoriByte  * code      ;
  NoriSize    codesize  ;
  NoriSize    ip        ;
};

 
/** Opcode operation functors. */
NORI_FUNCTYPE(void) NoriOpFunc(NoriVm * vm);
 
 
#define NORI_VM_OPS 256
 
/** The virtual machine for executing bytecodes. */ 
struct NoriVm_ {
  NoriFrame * frames    ;
  NoriFrame * active    ;
  NoriSize    framesize ;
  NoriOpFunc *ops[NORI_VM_OPS];
};

/** Helpers. */
/** Read a register from a frame. */
#define NORI_FRAME_REG(FRAME, REGI)     ((FRAME)->regs + (REGI))
#define NORI_FRAME_REG_SET(FRAME, REGI, VAL) \
        ((REGI == NORI_REG_ZERO) ? NORI_REG_ZERO : ((FRAME)->regs[(REGI)] = VAL)
        
NoriReg * nori_frame_reg(NoriFrame * frame, NoriByte regi) {
  return frame->regs + regi;
}

/** Writes to a register. Writes to register 0 are ignored. */ 
void nori_frame_reg_set(NoriFrame * frame, NoriByte regi, NoriReg val) {
  if(regi == NORI_REG_ZERO) return; 
  frame->regs[regi]  = val;
}

#define NORI_REG_I32(REG) ((REG)->i32)
#define NORI_REG_F64(REG) ((REG)->f64)
#define NORI_REG_PTR(REG) ((REG)->ptr)

#define NORI_REG_I32_SET(REG, VAL) (NORI_REG_I32(REG) = VAL)
#define NORI_REG_F64_SET(REG, VAL) (NORI_REG_F64(REG) = VAL)
#define NORI_REG_PTR_SET(REG, VAL) (NORI_REG_PTR(REG) = VAL)
 
/** Reads a signed integer from a register. */
NoriI32 nori_reg_i32(NoriReg * reg) {
  return NORI_REG_I32(reg);
} 

/** Reads a floating point value from a register. */
NoriF64 nori_reg_f64(NoriReg * reg) {
  return NORI_REG_F64(reg);
} 

/** Reads a pointer from a register. */
NoriPtr nori_reg_ptr(NoriReg * reg) {
  return NORI_REG_PTR(reg);
} 

/** Writes a signed integer to a register. */
NoriI32 nori_reg_i32_set(NoriReg * reg, NoriI32 val) {
  return NORI_REG_I32_SET(reg, val);
} 

/** Reads a floating point value from a register. */
NoriF64 nori_reg_f64_set(NoriReg * reg, NoriF64 val) {
  return NORI_REG_F64_SET(reg, val);
} 

/** Reads a pointer from a register. */
NoriPtr nori_reg_ptr_set(NoriReg * reg, NoriPtr val) {
  return NORI_REG_PTR_SET(reg, val);
} 


/** Sets the instruction pointer of the frame . */
void nori_frame_ip_set(NoriFrame * frame, NoriI32 val) {
  frame->ip = val;
}

/** Modifies the instruction pointer of the frame . */
void nori_frame_ip_by(NoriFrame * frame, NoriI32 by) {
  frame->ip += by;
}

/** Advances the instruction pointer in a frame by one. */
NoriPtr nori_frame_ip_up(NoriFrame * frame) {
  frame->ip ++;
}


/** Operations. */
#define NORI_VM_NOP   0

#define NORI_VM_ILOAD 1
#define NORI_VM_IADD  2
#define NORI_VM_ISUB  3
#define NORI_VM_IMUL  4
#define NORI_VM_IDIV  5
#define NORI_VM_IMOD  6

#define NORI_VM_FLOAD 10
#define NORI_VM_FADD  11
#define NORI_VM_FSUB  12
#define NORI_VM_FMUL  13
#define NORI_VM_FDIV  14
#define NORI_VM_FMOD  15

#define NORI_VM_CALL  20
#define NORI_VM_CALLC 21

#define NORI_VM_JMP   22
#define NORI_VM_TO    23
#define NORI_VM_CMP   24
#define NORI_VM_JIT   25

#define NORI_VM_OR    30
#define NORI_VM_AND   31
#define NORI_VM_XOR   32
#define NORI_VM_NOT   33

#define NORI_VM_MOV   40


void nori_vm_nop(NoriVm * vm) {  
}

void nori_vm_iload(NoriVm * vm) {
}


NoriOpFunc nori_vm_opfunc(NoriVm * vm, NoriByte opcode) {
  return vm->ops[opcode]; 
}

NoriOpFunc nori_frame_opfunc(NoriFrame * frame, NoriByte opcode) {
  return nori_vm_opfunc(frame->vm); 
}


int nori_frame_runop(NoriFrame * frame) {
  NoriByte   opcode   ;
  NoriOpFunc op = NULL;
  if (frame->ip >= frame->codesize) return FALSE;  
  if (frame->ip < 0) return FALSE;
  opcode         = frame->code[frame->ip];
  nori_frame_ip_up(frame);
  op             = nori_frame_opfunc(frame, opcode);
  
  
  return TRUE;
} 

int nori_vm_runop(NoriVm * vm) {
  
}









