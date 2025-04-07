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
def SEC    : InstImpl<0x38, (outs), (ins), "SEC", []>;
}


let isAdd = 1 in {
  let Uses=[C,Carry,M],Defs=[C,Carry] in {
    def ADCdp : InstDrctPg<0x65, (outs), (ins OprndAddr8:$addr), "ADC\t$addr", [(set C, (ADCsrNode C, PatternDp:$addr))]>;
  }
  def ADCi  : WDCAddSubCarryInstructionImmediate<0x69, adde, "ADC", UnsignedImm16Operand>;
  //def ADCsr : WDCAddSubCarryInstructionStackRelative<0x63, addc, "ADC">;
  //def ADDsr : ADDSUBStackRelativePseudo<ADDsrNode>;
  let Uses=[C,M], Defs=[C,Carry], mayLoad=1 in {
    def ADDsr : InstPseudo<(outs), (ins OprndAddr8:$addr), [(set C, (ADDsrNode C, PatternDp:$addr))]>;
  }
  def ADDi : ADDImmediatePseudo;
}


def ANDi : WDCLogicInstructionImmediate<0x29, and, "AND", UnsignedImm16Operand>;
def ANDsr : InstructionLogicStackRelative<0x23, "AND", ANDsrNode>;

def ASL : InstImpl<0x0A, (outs RegsA16:$dst), (ins RegsA16:$src), 
                                 "ASL\tA", [(set RegsA16:$dst, (shl RegsA16:$src, OnePattern))]>;

def CMPi : InstImm16<0xc9, (outs StatusReg:$sr), (ins RegsA16:$src, UnsignedImm16Operand:$constant),
                                   "CMP\t$constant", []>;
def CMPsr : InstStkRel<0xc3, (outs StatusReg:$sr),
                                        (ins RegsA16:$src, StackOffsetOperand:$spoffset),
                                        "CMP\t$spoffset", []>;

def EORi : WDCLogicInstructionImmediate<0x49, xor, "EOR", UnsignedImm16Operand>;
def EORsr : InstructionLogicStackRelative<0x43, "EOR", EORsrNode>;

let mayLoad=1 in {
  def LDAal : InstAbsLong<0xAF, (outs RegsA16:$dst), (ins OprndAbsLong:$src),
                          "LDA\t$src", [(set RegsA16:$dst, (load (Wrapper (tglobaladdr:$src))))]>;
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
let Defs=[C],Uses=[M],mayLoad=1,hasSideEffects=true,isReMaterializable=1 in {
  def LDAi : InstImm16<0xA9, (outs), (ins UnsignedImm16Operand:$constant),
                       "LDA\t$constant", [(set C, imm:$constant)]>;
}

def LDAsr  : WDCLoadInstructionStackRelative<0xA3, "LDA">;

// Pseudo instruction to copy the address of a stack slot into the accumulator
def LEAsr  : InstPseudo<(outs RegsA16:$dst), (ins StackOffsetOperand:$frameIndex),
                        [(set RegsA16:$dst, (LEANode frameIndexPattern:$frameIndex))]>;

def LSR : InstImpl<0x4A, (outs RegsA16:$dst), (ins RegsA16:$src),
                                "LSR\tA", 
                                [(set RegsA16:$dst, (srl RegsA16:$src, OnePattern))]>;

def SUBsr : ADDSUBStackRelativePseudo<SUBsrNode>;

def SBCi   : WDCAddSubCarryInstructionImmediate<0xE9, subc, "SBC", UnsignedImm16Operand>;
def SBCsr  : WDCAddSubCarryInstructionStackRelative<0xE3, subc, "SBC">;

def ORAi   : WDCLogicInstructionImmediate<0x09, or, "ORA", UnsignedImm16Operand>;
def ORAsr  : InstructionLogicStackRelative<0x03, "ORA", ORAsrNode>;

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

def ROL : InstImpl<0x2a, (outs RegsA16:$dst), (ins RegsA16:$src),
                                "ROL\tA", []>;
def ROR : InstImpl<0x6A, (outs RegsA16:$dst), (ins RegsA16:$src),
                                "ROR\tA", []>;

def ROTL : InstPseudo<(outs RegsA16:$dst), (ins RegsA16:$src),
                         [(set RegsA16:$dst, (rotl RegsA16:$src, OnePattern))]>;

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

def SRA : InstPseudo<(outs RegsA16:$dst), (ins RegsA16:$src),
                         [(set RegsA16:$dst, (sra RegsA16:$src, OnePattern))]>;

let mayStore=true, Uses=[C,M],hasSideEffects=false in {
  def STAa   : InstAbs<0x8D,     (outs), (ins OprndAddr16:$addr),  "STA\t$addr", [(store C, NearGlobalPattern:$addr)]>;
  def STAal  : InstAbsLong<0x8F, (outs), (ins OprndAbsLong:$addr), "STA\t$addr", [(store C, globaladdr:$addr)]>;
  def STAdp  : InstDrctPg<0x85,  (outs), (ins OprndAddr8:$addr),   "STA\t$addr", [(store C, PatternDp:$addr)]>;
  def STAdpi : InstDrctPg<0x92,  (outs), (ins OprndAddr8:$addr),   "STA\t($addr)", [(store C, NearIndirPattern:$addr)]>;
  def STAdpil : InstDrctPg<0x87, (outs), (ins OprndAddr8:$addr),   "STA\t[$addr]", [(store C, PatternDpil:$addr)]>;
  def STAsr  : InstStkRel<0x83,  (outs), (ins RegsA16:$accumulator, StackOffsetOperand:$spoffset),
                          "STA\t$spoffset", 
                          [(STANode RegsA16:$accumulator, frameIndexPattern:$spoffset, M)]>;
  // def STAsr8 : InstStkRel<0x83, (outs), (ins RegsA8:$src, StackOffsetOperand:$spoffset),
  //                         "STA\t$spoffset", 
  //                         [(STANode RegsA8:$src, frameIndexPattern:$spoffset, M)]>;
}

let mayStore=true, Uses=[M],hasSideEffects=false in {
  def STZa  : InstAbs<0x9C,    (outs), (ins OprndAddr16:$addr), "STZ\t$addr", [(store PatternZero16, NearGlobalPattern:$addr)]>;
  def STZdp : InstDrctPg<0x64, (outs), (ins OprndAddr8:$addr),  "STZ\t$addr", [(store PatternZero16, PatternDp:$addr)]>;
}

let Defs=[X],Uses=[C] in {
  def TAX : InstImpl<0xAA, (outs), (ins), "TAX", []>;
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

def XBA : InstImpl<0xEB, (outs RegsA16:$dst), (ins RegsA16:$src),
                                "XBA", [(set RegsA16:$dst, (bswap RegsA16:$src))]>;

// Small immediates
def : Pat<(globaladdr:$addr), (imm:$addr)>;
// def : Pat<(i16 SignedImm16Pattern:$in), (LDAi imm:$in)>;
def : Pat<(seteq RegsA16:$src, UnsignedImm16Pattern:$const), (
  ANDi (LSR (COPY_TO_REGCLASS (CMPi RegsA16:$src, imm:$const), RegsA16)), 1)>;

// SETCC comparisons between the accumulator and a frame index
// For seteq (setne comes from this as well)
def : Pat<(SETCCsrNode RegsA16:$src, frameIndexPattern:$spoffset, SETEQ),
          (ANDi (LSR (COPY_TO_REGCLASS (CMPsr RegsA16:$src, $spoffset), RegsA16)), 1)>;
def : Pat<(SETCCsrNode RegsA16:$src, frameIndexPattern:$spoffset, SETGE),
          (ANDi (TCA (CMPsr RegsA16:$src, $spoffset)), 1)>;

def mem_ea : Operand<iPTR> {
  let PrintMethod = "printMemOperandEA";
  let EncoderMethod = "getMemEncoding";
}

