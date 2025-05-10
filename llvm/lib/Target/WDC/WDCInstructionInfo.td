//===- WDCInstructionInfo.td - Target Description for 65816 Target -*- tablegen -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the 65816 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//
include "WDCInstructionFormats.td"
include "WDCSDNodes.td"
include "WDCPseudoInstructions.td"


def RTL    : WDCReturnInstruction<0x6b, "RTL">;

let Defs=[Carry], isReMaterializable=1, isAsCheapAsAMove=1 in {
def CLC    : InstImpl<0x18, (outs), (ins), "CLC", [(CLCNode)]>;
def SEC    : InstImpl<0x38, (outs), (ins), "SEC", [(SECNode)]>;
}

let isAdd = 1,Constraints="$dst = $src" in {
  let Uses=[M] in {
    def INC : InstImpl<0x1A, (outs RegsA16:$dst), (ins RegsA16:$src), "INC A", [(set RegsA16:$dst, (ADDNode RegsA16:$src, OnePattern))]>;
  }
  def INCGP : InstPseudo<(outs RegsGP16:$dst), (ins RegsGP16:$src), [(set RegsGP16:$dst, (add RegsGP16:$src, OnePattern))]>;
}

let isAdd = 1,Defs=[Carry],Constraints="$src = $dst" in {
  let Uses=[M] in {
    def ADDdp : InstPseudo<(outs RegsA16:$dst), (ins RegsA16:$src, OprndAddr8:$slot),
                           [(set RegsA16:$dst, (ADDNode RegsA16:$src, PatternDp:$slot))]>;
    def ADDi : InstPseudo<(outs RegsA16:$dst), (ins RegsA16:$src, UnsignedImm16Operand:$const),
                          [(set RegsA16:$dst, (ADDNode RegsA16:$src, Imm16Pattern:$const))]>;
    def ADDal : InstPseudo<(outs RegsA16:$dst), (ins RegsA16:$src, OprndAbsLong:$addr),
                           [(set RegsA16:$dst, (ADDNode RegsA16:$src, (Wrapper (tglobaladdr:$addr))))]>;
  }

  let Uses=[Carry,M] in {
    def ADCal : InstAbsLong<0x6f, (outs RegsA16:$dst), (ins RegsA16:$src, OprndAbsLong:$addr), "ADC\t$addr",
                          [(set RegsA16:$dst, (addc RegsA16:$src, (Wrapper (tglobaladdr:$addr))))]>;
    def ADCdp : InstDrctPg<0x65, (outs RegsA16:$dst), (ins RegsA16:$src, OprndAddr8:$addr), "ADC\t$addr",
                          [(set RegsA16:$dst, (ADCsrNode RegsA16:$src, PatternDp:$addr))]>;
    def ADCi  : InstImm16<0x69, (outs RegsA16:$dst),  (ins RegsA16:$src, UnsignedImm16Operand:$const), "ADC\t$const", 
                          [(set RegsA16:$dst, (ADCiNode RegsA16:$src, UnsignedImm16Pattern:$const))]>;
  }
}

let Constraints="$dst = $src",Uses=[M] in {
  def ANDi : InstImm16<0x29, (outs RegsA16:$dst), (ins RegsA16:$src, UnsignedImm16Operand:$const), 
                             "AND\t$const", [(set RegsA16:$dst, (ANDNode RegsA16:$src, Imm16Pattern:$const))]>;
  def ANDdp : InstDrctPg<0x25, (outs RegsA16:$dst), (ins RegsA16:$src, OprndAddr8:$addr),
                             "AND\t$addr", [(set RegsA16:$dst, (ANDNode RegsA16:$src, PatternDp:$addr))]>;
  def ANDsr : InstStkRel<0x23, (outs RegsA16:$dst), (ins RegsA16:$src, OprndAddr8:$spoffset),
                             "AND\t$spoffset", [(set RegsA16:$dst, (ANDNode RegsA16:$src, PatternDp:$spoffset))]>;
  def ANDal : InstAbsLong<0x2F, (outs RegsA16:$dst), (ins RegsA16:$src, OprndAbsLong:$addr),
                             "AND\t$addr", [(set RegsA16:$dst, (ANDNode RegsA16:$src, (Wrapper (tglobaladdr:$addr))))]>;
}

def ASL : InstImpl<0x0A, (outs RegsA16:$dst), (ins RegsA16:$src), 
                                 "ASL\tA", [(set RegsA16:$dst, (shl RegsA16:$src, OnePattern))]>;

let isBranch=true,isTerminator=true in {
  def BEQ : InstImm8<0xF0, (outs), (ins SignedImm8Operand:$const), "BEQ\t$const", []>;
  def BCS : InstImm8<0xB0, (outs), (ins SignedImm8Operand:$const), "BCS\t$const", []>;
  def BRA : InstImm8<0x80, (outs), (ins SignedImm8Operand:$const), "BRA\t$const", []>;
}

def CMPGPi : InstPseudo<(outs StatusReg:$sr), (ins RegsGP16:$src, UnsignedImm16Operand:$const), []>;
def CMPi : InstImm16<0xc9, (outs StatusReg:$sr), (ins RegsA16:$src, UnsignedImm16Operand:$const),
                                  "CMP\t$const", []>;
def CPXi : InstImm16<0xe0, (outs StatusReg:$sr), (ins RegsX16:$src, UnsignedImm16Operand:$const),
                                  "CPX\t$const", []>;
def CPYi : InstImm16<0xc0, (outs StatusReg:$sr), (ins RegsY16:$src, UnsignedImm16Operand:$const),
                                  "CPY\t$const", []>;
def CMPsr : InstStkRel<0xc3, (outs StatusReg:$sr),
                                        (ins RegsA16:$src, StackOffsetOperand:$spoffset),
                                        "CMP\t$spoffset", []>;
def CMPdp : InstDrctPg<0xc5, (outs StatusReg:$sr),
                              (ins RegsA16:$src, OprndAddr8:$addr),
                              "CMP\t$addr", []>;
def CMPal : InstAbsLong<0xcf, (outs StatusReg:$sr),
                              (ins RegsA16:$src, OprndAbsLong:$addr),
                              "CMP\t$addr", []>;

let Constraints="$dst = $src",Uses=[M] in {
  def EORi : InstImm16<0x49, (outs RegsA16:$dst), (ins RegsA16:$src, UnsignedImm16Operand:$const), "EOR\t$const", 
                         [(set RegsA16:$dst, (EORNode RegsA16:$src, UnsignedImm16Pattern:$const))]>;
  def EORdp : InstDrctPg<0x45, (outs RegsA16:$dst), (ins RegsA16:$src, OprndAddr8:$addr), "EOR\t$addr", 
                         [(set RegsA16:$dst, (EORNode RegsA16:$src, PatternDp:$addr))]>;
  def EORal : InstAbsLong<0x4F, (outs RegsA16:$dst), (ins RegsA16:$src, OprndAbsLong:$addr), "EOR\t$addr", 
                         [(set RegsA16:$dst, (EORNode RegsA16:$src, (Wrapper (tglobaladdr:$addr))))]>;
}

let isAdd = 1,mayLoad=1,mayStore=1,Uses=[M] in {
  def INCdp : InstDrctPg<0xE6, (outs), (ins OprndAddr8:$addr), "INC\t$addr", 
      [(store (add (load PatternDp:$addr), OnePattern), PatternDp:$addr)]>;
}

let mayLoad=1 in {
  def LDAal : InstAbsLong<0xAF, (outs RegsA16:$dst), (ins OprndAbsLong:$src),
                          "LDA\t$src", [(set RegsA16:$dst, (load (Wrapper (tglobaladdr:$src))))]>;
  def LDAal8 : InstPseudo<(outs RegsA16:$dst), (ins OprndAbsLong:$src),
                        [(set RegsA16:$dst, (extload (Wrapper (tglobaladdr:$src))))]>;
  def LDAdpil : InstDrctPg<0xA7, (outs RegsA16:$dst), (ins OprndDrctPgIndrct:$addr),
                          "LDA\t$addr",
                          [(set RegsA16:$dst, (LDAdpilNode frameIndexPattern:$addr))]>;
}
//def LDAi  : WDCLoadInstructionImmediate<0xA9, "LDA", UnsignedImm16Operand>;
let Defs=[A],Uses=[M],hasSideEffects=false in {
  // def LDAi : InstImm16<0xA9, (outs RegsA16:$dst), (ins UnsignedImm16Operand:$constant),
  //                     "LONGA ON\n\tLDA\t$constant", [(set RegsA16:$dst, (load imm:$constant))]>;
  def LDAi8 : InstImm8<0xA9, (outs RegsA8:$dst), (ins UnsignedImm8Operand:$const),
                      "LONGA OFF\n\tLDA\t$const", [(set RegsA8:$dst, (load imm:$const))]>;
}

let mayLoad=1,hasSideEffects=0 in {
  // load a stack slot (direct page addressing into any one of the registers)
  def LDGPdp : InstPseudo<(outs RegsGP16:$dst), (ins OprndAddr8:$addr),
                          [(set RegsGP16:$dst, (load PatternDp:$addr))]>;
  let Uses=[M] in {
    let isReMaterializable=1 in {
      def LDAi : InstImm16<0xA9, (outs RegsA16:$dst), (ins UnsignedImm16Operand:$const),
                        "LDA\t$const", [(set RegsA16:$dst, imm:$const)]>;
    }
    def LDAdp : InstDrctPg<0xA5, (outs RegsA16:$dst), (ins OprndAddr8:$addr), "LDA\t$addr", 
                           [(set RegsA16:$dst, (load PatternDp:$addr))]>;
  }
  def LDXdp : InstDrctPg<0xA6, (outs RegsX16:$dst), (ins OprndAddr8:$addr), "LDX\t$addr", 
                          [(set RegsX16:$dst, (load PatternDp:$addr))]>;
  def LDYdp : InstDrctPg<0xA4, (outs RegsY16:$dst), (ins OprndAddr8:$addr), "LDY\t$addr", 
                           [(set RegsY16:$dst, (load PatternDp:$addr))]>;
  def LDGPi : InstPseudo<(outs RegsGP16:$dst), (ins UnsignedImm16Operand:$const),
                          [(set RegsGP16:$dst, (load imm:$const))]>;
  def LDXi  : InstImm16<0xA2, (outs RegsX16:$dst), (ins UnsignedImm16Operand:$const), "LDX\t$const", 
                          [(set RegsX16:$dst, imm:$const)]>;
  def LDYi  : InstImm16<0xA0, (outs RegsY16:$dst), (ins UnsignedImm16Operand:$const), "LDY\t$const", 
                          [(set RegsY16:$dst, imm:$const)]>;
}

// Pseudo instruction to copy the address of a stack slot into the accumulator
def LEAsr  : InstPseudo<(outs RegsA16:$dst), (ins StackOffsetOperand:$frameIndex),
                        [(set RegsA16:$dst, (LEANode frameIndexPattern:$frameIndex))]>;

let Constraints="$dst = $src", Uses=[M,Carry], Defs=[Carry] in {
  def LSR : InstImpl<0x4A, (outs RegsA16:$dst), (ins RegsA16:$src),
                                  "LSR\tA", 
                                  [(set RegsA16:$dst, (srl RegsA16:$src, OnePattern))]>;
}

let Constraints="$dst = $src",Uses=[M] in {
  def ORAi  : InstImm16<0x09, (outs RegsA16:$dst), (ins RegsA16:$src, UnsignedImm16Operand:$const), "ORA\t$const", 
                          [(set RegsA16:$dst, (ORANode RegsA16:$src, Imm16Pattern:$const))]>;
  def ORAdp : InstDrctPg<0x05, (outs RegsA16:$dst), (ins RegsA16:$src, OprndAddr8:$addr), "ORA\t$addr", 
                          [(set RegsA16:$dst, (ORANode RegsA16:$src, PatternDp:$addr))]>;
  def ORAsr : InstStkRel<0x03, (outs RegsA16:$dst), (ins RegsA16:$src, OprndAddr8:$spoffset), "ORA\t$spoffset", 
                          [(set RegsA16:$dst, (ORANode RegsA16:$src, PatternDp:$spoffset))]>;
  def ORAl  : InstAbsLong<0x0F, (outs RegsA16:$dst), (ins RegsA16:$src, OprndAbsLong:$addr), "ORA\t$addr", 
                          [(set RegsA16:$dst, (ORANode RegsA16:$src, (Wrapper (tglobaladdr:$addr))))]>;
}

let Uses=[M,C] in {
  def PHA : InstImpl<0x48, (outs), (ins), "PHA", []>;
} 
def PHD : WDCStackInstruction<0x0B, "PHD">;
def PHP : WDCStackInstruction<0x08, "PHP">;
let Uses=[M],Defs=[C] in {
  def PLA : InstImpl<0x68, (outs), (ins), "PLA", []>;
}
def PLB : WDCStackInstruction<0xAB, "PLB">;
def PLD : WDCStackInstruction<0x2B, "PLD">;
def PLP : WDCStackInstruction<0x28, "PLP">;

let Constraints="$src = $dst",Uses=[M,C] in {
  def ROL : InstImpl<0x2a, (outs RegsA16:$dst), (ins RegsA16:$src),
                                  "ROL\tA", []>;
  def ROR : InstImpl<0x6A, (outs RegsA16:$dst), (ins RegsA16:$src),
                                  "ROR\tA", []>;
}

let Constraints="$dst = $src",Uses=[M] in {
  def ROTL : InstPseudo<(outs RegsA16:$dst), (ins RegsA16:$src),
                         [(set RegsA16:$dst, (rotl RegsA16:$src, OnePattern))]>;
}

let Defs=[M],hasSideEffects=false in {
  def REP : InstImm8<0xC2, (outs StatusReg:$P), (ins UnsignedImm8Operand:$const), "REP\t$const", []>;
  def SEP : InstImm8<0xE2, (outs StatusReg:$P), (ins UnsignedImm8Operand:$const), "SEP\t$const", []>;
}

let isAsCheapAsAMove=1,Defs=[M],hasSideEffects=false in {
  def SetM : InstPseudo<(outs), (ins Operand<i1>:$val), []>;
}

let isReturn=1, isTerminator=1, isBarrier=1, hasCtrlDep=1 in {
  def RetRTL : InstPseudo<(outs), (ins), [(WDCRetNode)]>;
}

let Defs=[Carry],Constraints="$src = $dst" in {
  let Uses=[M,Carry] in {
    def SBCi  : InstImm16<0xE9,  (outs RegsA16:$dst), (ins RegsA16:$src, UnsignedImm16Operand:$const), "SBC\t$const", 
                          [(set RegsA16:$dst, (sub RegsA16:$src,  UnsignedImm16Pattern:$const))]>;
    def SBCal : InstAbsLong<0xEF, (outs RegsA16:$dst), (ins RegsA16:$src, OprndAbsLong:$addr), "SBC\t$addr", 
                          [(set RegsA16:$dst, (subc RegsA16:$src, (Wrapper (tglobaladdr:$addr))))]>;
    def SBCsr : InstStkRel<0xE3, (outs RegsA16:$dst), (ins RegsA16:$src, OprndAddr8:$spoffset), "SBC\t$spoffset", 
                          [(set RegsA16:$dst, (SBCNode RegsA16:$src, PatternDp:$spoffset))]>;
    def SBCdp : InstDrctPg<0xE5, (outs RegsA16:$dst), (ins RegsA16:$src, OprndAddr8:$addr), "SBC\t$addr", 
                          [(set RegsA16:$dst, (SBCNode RegsA16:$src,  PatternDp:$addr))]>;
  }
  let Uses=[M] in {
    def SUBdp : InstPseudo<(outs RegsA16:$dst), (ins RegsA16:$src, OprndAddr8:$slot),
                           [(set RegsA16:$dst, (SUBNode RegsA16:$src, PatternDp:$slot))]>;
    def SUBal : InstPseudo<(outs RegsA16:$dst), (ins RegsA16:$src, OprndAbsLong:$addr),
                           [(set RegsA16:$dst, (SUBNode RegsA16:$src, (Wrapper (tglobaladdr:$addr))))]>;
  }
}

def SELECT_CC_eq_i : InstPseudo<(outs RegsGP16:$dst), 
                            (ins RegsGP16:$src, UnsignedImm16Operand:$const, UnsignedImm16Operand:$trueval, 
                             UnsignedImm16Operand:$falseval),
                            [(set RegsGP16:$dst,
                                  (selectcc RegsGP16:$src, UnsignedImm16Pattern:$const,
                                            UnsignedImm16Pattern:$trueval, UnsignedImm16Pattern:$falseval, 
                                            SETEQ
                                  )
                             )]>;
let usesCustomInserter=true in {
  def SETEQ_i : InstPseudo<(outs RegsGP16:$dst), (ins RegsGP16:$src, UnsignedImm16Operand:$const),
                         [(set RegsGP16:$dst, (SETCCNode RegsGP16:$src, UnsignedImm16Pattern:$const, SETEQ))]>;
  def SETEQ_al : InstPseudo<(outs RegsGP16:$dst), (ins RegsA16:$src, OprndAbsLong:$addr),
                         [(set RegsGP16:$dst, (SETCCNode RegsA16:$src, (Wrapper (tglobaladdr:$addr)), SETEQ))]>;
}

let Constraints="$src = $dst" in {
  def SEXT : InstPseudo<(outs RegsA16:$dst), (ins RegsA16:$src), 
                      [(set RegsA16:$dst, (sext_inreg RegsA16:$src, i8))]>;
}

let Constraints="$src = $dst" in {
  def SRA : InstPseudo<(outs RegsA16:$dst), (ins RegsA16:$src),
                         [(set RegsA16:$dst, (sra RegsA16:$src, OnePattern))]>;
}

let mayStore=true, hasSideEffects=false in {
  def STGPdp : InstPseudo<(outs), (ins RegsGP16:$src, OprndAddr8:$addr),
                          [(store RegsGP16:$src, PatternDp:$addr)]>;
  let Uses=[M] in {
    def STAa   : InstAbs<0x8D,     (outs), (ins RegsA16:$src, OprndAddr16:$addr),  "STA\t$addr",   
                        [(store RegsA16:$src, NearGlobalPattern:$addr)]>;
    def STAal  : InstAbsLong<0x8F, (outs), (ins RegsA16:$src, OprndAbsLong:$addr), "STA\t$addr",   
                        [(store RegsA16:$src, (Wrapper (tglobaladdr:$addr)))]>;
    def STAaltrunc : InstAbsLong<0x8F, (outs), (ins FakeRegs:$src, OprndAbsLong:$addr), "STA\t$addr",   
                        [(truncstore FakeRegs:$src, (Wrapper (tglobaladdr:$addr)))]>;
    def STAdp  : InstDrctPg<0x85,  (outs), (ins RegsA16:$src, OprndAddr8:$addr),   "STA\t$addr",   
                        [(store RegsA16:$src, PatternDp:$addr)]>;
    def STAdpi : InstDrctPg<0x92,  (outs), (ins RegsA16:$src, OprndAddr8:$addr),   "STA\t($addr)", 
                        [(store RegsA16:$src,  NearIndirPattern:$addr)]>;
    def STAdpil : InstDrctPg<0x87, (outs), (ins RegsA16:$src, OprndAddr8:$addr),   "STA\t[$addr]", 
                        [(store RegsA16:$src, PatternDpil:$addr)]>;
    def STAsr  : InstStkRel<0x83,  (outs), (ins RegsA16:$src, OprndAddr8:$spoffset), "STA\t$spoffset,S", 
                            [(store RegsA16:$src, PatternDp:$spoffset)]>;
  }
}

let mayStore=true,hasSideEffects=false in {
  def STYdp : InstDrctPg<0x8C, (outs), (ins RegsY16:$src, OprndAddr8:$addr), "STY\t$addr",
                         [(store RegsY16:$src, PatternDp:$addr)]>;
  def STXdp : InstDrctPg<0x8E, (outs), (ins RegsX16:$src, OprndAddr8:$addr), "STX\t$addr", 
                         [(store RegsX16:$src, PatternDp:$addr)]>;
  let Uses=[M],hasSideEffects=false in {
    def STZa  : InstAbs<0x9C,    (outs), (ins OprndAddr16:$addr), "STZ\t$addr", [(store PatternZero16, NearGlobalPattern:$addr)]>;
    def STZdp : InstDrctPg<0x64, (outs), (ins OprndAddr8:$addr),  "STZ\t$addr", [(store PatternZero16, PatternDp:$addr)]>;
  }
}

let Defs=[X],Uses=[C] in {
  def TAX : InstImpl<0xAA, (outs), (ins), "TAX", []>;
}
let Defs=[Y],Uses=[C] in {
  def TAY : InstImpl<0xA8, (outs), (ins), "TAY", []>;
}
// "Transfer Carry to least significant bit of A" == ROL A 
def TCA : InstPseudo<(outs RegsA16:$dst), (ins StatusReg:$src), []>;
def TSC : WDCTransferInstruction<0x3B, (outs RegsA16:$dst), (ins), "TSC">;
def TDC : WDCTransferInstruction<0x7D, (outs RegsA16:$dst), (ins), "TDC">;
def TCS : WDCTransferInstruction<0x1B, (outs), (ins RegsA16:$src), "TCS">;
def TCD : WDCTransferInstruction<0x5B, (outs), (ins RegsA16:$src), "TCD">;

let Defs=[C],Uses=[X] in {
  def TXA : InstImpl<0x8A, (outs), (ins), "TXA", []>;
}
let Defs=[C],Uses=[Y] in {
  def TYA : InstImpl<0x98, (outs), (ins), "TYA", []>;
}

let Constraints="$dst = $src" in {
  def XBA : InstImpl<0xEB, (outs RegsA16:$dst), (ins RegsA16:$src),
                     "XBA", [(set RegsA16:$dst, (bswap RegsA16:$src))]>;
}

// Small immediates
def : Pat<(globaladdr:$addr), (imm:$addr)>;
// def : Pat<(i16 SignedImm16Pattern:$in), (LDAi imm:$in)>;
def : Pat<(seteq RegsA16:$src, UnsignedImm16Pattern:$const), (
  ANDi (LSR (COPY_TO_REGCLASS (CMPi RegsA16:$src, imm:$const), RegsA16)), 1)>;

def : Pat<(LoNode tglobaladdr:$ptr), (LDAi $ptr)>;
def : Pat<(HiNode tglobaladdr:$ptr), (LDAi $ptr)>;

// SETCC comparisons between the accumulator and a frame index
// For seteq (setne comes from this as well)
def : Pat<(SETCCNode RegsA16:$src, PatternDp:$addr, SETEQ),
          (ANDi (LSR (COPY_TO_REGCLASS (CMPdp RegsA16:$src, $addr), RegsA16)), 1)>;
// def : Pat<(SETCCNode RegsA16:$src, UnsignedImm16Pattern:$const, SETEQ), 
//           (ANDi (LSR (COPY_TO_REGCLASS (CMPi RegsA16:$src, imm:$const), RegsA16)), 1)>;
def : Pat<(SETCCNode RegsA16:$src, PatternDp:$addr, SETGE),
          (ANDi (TCA (CMPdp RegsA16:$src, $addr)), 1)>;
def : Pat<(SETCCNode RegsA16:$src, (Wrapper (tglobaladdr:$addr)), SETGE),
          (ANDi (TCA (CMPal RegsA16:$src, $addr)), 1)>;

def mem_ea : Operand<iPTR> {
  let PrintMethod = "printMemOperandEA";
  let EncoderMethod = "getMemEncoding";
}
