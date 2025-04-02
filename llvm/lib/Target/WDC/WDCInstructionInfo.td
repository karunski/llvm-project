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


def STAsr  : WDCStoreInstructionStackRelative<0x83, "STA">;
def RTL    : WDCReturnInstruction<0x6b, "RTL">;

def CLC    : WDCStatusControlInstruction<0x18, "CLC", CarryRegisterClass, []>;

def SEC    : WDCStatusControlInstruction<0x38, "SEC", CarryRegisterClass, []>;

let isAdd = 1 in {
  def ADCi  : WDCAddSubCarryInstructionImmediate<0x69, adde, "ADC", UnsignedImm16Operand>;
  def ADCsr : WDCAddSubCarryInstructionStackRelative<0x63, addc, "ADC">;
  def ADDsr : ADDSUBStackRelativePseudo<ADDsrNode>;
  def ADDi : ADDImmediatePseudo;
}

def ANDi : WDCLogicInstructionImmediate<0x29, and, "AND", UnsignedImm16Operand>;
def ANDsr : InstructionLogicStackRelative<0x23, "AND", ANDsrNode>;

def ASL : InstImpl<0x0A, (outs AccumulatorRegisterClass:$dst), (ins AccumulatorRegisterClass:$src), 
                                 "ASL\tA", [(set AccumulatorRegisterClass:$dst, (shl AccumulatorRegisterClass:$src, OnePattern))]>;

def CMPi : InstImm16<0xc9, (outs StatusReg:$sr), (ins AccumulatorRegisterClass:$src, UnsignedImm16Operand:$constant),
                                   "CMP\t$constant", []>;
def CMPsr : InstStkRel<0xc3, (outs StatusReg:$sr),
                                        (ins AccumulatorRegisterClass:$src, StackOffsetOperand:$spoffset),
                                        "CMP\t$spoffset", []>;

def EORi : WDCLogicInstructionImmediate<0x49, xor, "EOR", UnsignedImm16Operand>;
def EORsr : InstructionLogicStackRelative<0x43, "EOR", EORsrNode>;

def LDAal : InstAbsLong<0xAF, (outs AccumulatorRegisterClass:$dst), (ins OprndAbsLong:$src),
                        "LDA\t$src", [(set AccumulatorRegisterClass:$dst, (load (Wrapper (tglobaladdr:$src))))]>;
def LDAdpil : InstDrctPgIndrctLong<0xA7, (outs AccumulatorRegisterClass:$dst), (ins OprndDrctPgIndrct:$spoffset),
                        "LDA\t$spoffset",
                        [(set AccumulatorRegisterClass:$dst, (LDAdpilNode frameIndexPattern:$spoffset))]>;
def LDAi   : WDCLoadInstructionImmediate<0xA9, "LDA", UnsignedImm16Operand>;
def LDAsr  : WDCLoadInstructionStackRelative<0xA3, "LDA">;

// Pseudo instruction to copy the address of a stack slot into the accumulator
def LEAsr  : InstPseudo<(outs AccumulatorRegisterClass:$dst), (ins StackOffsetOperand:$frameIndex),
                        [(set AccumulatorRegisterClass:$dst, (LEANode frameIndexPattern:$frameIndex))]>;

def LSR : InstImpl<0x4A, (outs AccumulatorRegisterClass:$dst), (ins AccumulatorRegisterClass:$src),
                                "LSR\tA", 
                                [(set AccumulatorRegisterClass:$dst, (srl AccumulatorRegisterClass:$src, OnePattern))]>;

def SUBsr : ADDSUBStackRelativePseudo<SUBsrNode>;

def SBCi   : WDCAddSubCarryInstructionImmediate<0xE9, subc, "SBC", UnsignedImm16Operand>;
def SBCsr  : WDCAddSubCarryInstructionStackRelative<0xE3, subc, "SBC">;

def ORAi   : WDCLogicInstructionImmediate<0x09, or, "ORA", UnsignedImm16Operand>;
def ORAsr  : InstructionLogicStackRelative<0x03, "ORA", ORAsrNode>;

def PHD : WDCStackInstruction<0x0B, "PHD">;
def PHP : WDCStackInstruction<0x08, "PHP">;
def PLA : WDCStackInstruction<0x68, "PLA">;
def PLB : WDCStackInstruction<0xAB, "PLB">;
def PLD : WDCStackInstruction<0x2B, "PLD">;
def PLP : WDCStackInstruction<0x28, "PLP">;

def ROL : InstImpl<0x2a, (outs AccumulatorRegisterClass:$dst), (ins AccumulatorRegisterClass:$src),
                                "ROL\tA", []>;
def ROR : InstImpl<0x6A, (outs AccumulatorRegisterClass:$dst), (ins AccumulatorRegisterClass:$src),
                                "ROR\tA", []>;

def ROTL : InstPseudo<(outs AccumulatorRegisterClass:$dst), (ins AccumulatorRegisterClass:$src),
                         [(set AccumulatorRegisterClass:$dst, (rotl AccumulatorRegisterClass:$src, OnePattern))]>;

def REP : InstImm8<0xC2, "REP">;
def SEP : InstImm8<0xE2, "SEP">;

let isReturn=1, isTerminator=1, isBarrier=1, hasCtrlDep=1 in {
  def RetRTL : InstPseudo<(outs), (ins), [(WDCRetNode)]>;
}

def SRA : InstPseudo<(outs AccumulatorRegisterClass:$dst), (ins AccumulatorRegisterClass:$src),
                         [(set AccumulatorRegisterClass:$dst, (sra AccumulatorRegisterClass:$src, OnePattern))]>;

// "Transfer Carry to least significant bit of A" == ROL A 
def TCA : InstPseudo<(outs AccumulatorRegisterClass:$dst), (ins StatusReg:$src), []>;
def TSC : WDCTransferInstruction<0x3B, (outs AccumulatorRegisterClass:$dst), (ins), "TSC">;
def TDC : WDCTransferInstruction<0x7D, (outs AccumulatorRegisterClass:$dst), (ins), "TDC">;
def TCS : WDCTransferInstruction<0x1B, (outs), (ins AccumulatorRegisterClass:$src), "TCS">;
def TCD : WDCTransferInstruction<0x5B, (outs), (ins AccumulatorRegisterClass:$src), "TCD">;

def XBA : InstImpl<0xEB, (outs AccumulatorRegisterClass:$dst), (ins AccumulatorRegisterClass:$src),
                                "XBA", [(set AccumulatorRegisterClass:$dst, (bswap AccumulatorRegisterClass:$src))]>;

// Small immediates
def : Pat<(globaladdr:$addr), (imm:$addr)>;
def : Pat<(i16 SignedImm16Pattern:$in), (LDAi imm:$in)>;
def : Pat<(seteq AccumulatorRegisterClass:$src, UnsignedImm16Pattern:$const), (
  ANDi (LSR (COPY_TO_REGCLASS (CMPi AccumulatorRegisterClass:$src, imm:$const), AccumulatorRegisterClass)), 1)>;

// SETCC comparisons between the accumulator and a frame index
// For seteq (setne comes from this as well)
def : Pat<(SETCCsrNode AccumulatorRegisterClass:$src, frameIndexPattern:$spoffset, SETEQ),
          (ANDi (LSR (COPY_TO_REGCLASS (CMPsr AccumulatorRegisterClass:$src, $spoffset), AccumulatorRegisterClass)), 1)>;
def : Pat<(SETCCsrNode AccumulatorRegisterClass:$src, frameIndexPattern:$spoffset, SETGE),
          (ANDi (TCA (CMPsr AccumulatorRegisterClass:$src, $spoffset)), 1)>;

def mem_ea : Operand<iPTR> {
  let PrintMethod = "printMemOperandEA";
  let EncoderMethod = "getMemEncoding";
}

