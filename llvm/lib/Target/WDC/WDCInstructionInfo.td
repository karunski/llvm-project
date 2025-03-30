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

def LDAi   : WDCLoadInstructionImmediate<0xA9, "LDA", UnsignedImm16Operand>;
def LDAsr  : WDCLoadInstructionStackRelative<0xA3, "LDA">;
def STAsr  : WDCStoreInstructionStackRelative<0x83, "STA">;
def RTL    : WDCReturnInstruction<0x6b, "RTL">;
def TSC    : WDCTransferInstruction<0x3B, "TSC">;
def TDC    : WDCTransferInstruction<0x7D, "TDC">;
def TCS    : WDCTransferInstruction<0x1B, "TCS">;
def TCD    : WDCTransferInstruction<0x5D, "TCD">;

def CLC    : WDCStatusControlInstruction<0x18, "CLC", CarryRegisterClass, []>;
//def CMPi   : WDCImmediateInstruction<0xc9, (outs), (ins AccumulatorRegisterClass:$src, UnsignedImm16Operand:$constant), 
//                                    "CMP\t$constant", []>;

def SEC    : WDCStatusControlInstruction<0x38, "SEC", CarryRegisterClass, []>;

let isAdd = 1 in {
  def ADCi  : WDCAddSubCarryInstructionImmediate<0x69, adde, "ADC", UnsignedImm16Operand>;
  def ADCsr : WDCAddSubCarryInstructionStackRelative<0x63, addc, "ADC">;
  def ADDsr : ADDSUBStackRelativePseudo<ADDsrNode>;
  def ADDi : ADDImmediatePseudo;
}

def ANDi : WDCLogicInstructionImmediate<0x29, and, "AND", UnsignedImm16Operand>;
def ANDsr : InstructionLogicStackRelative<0x23, "AND", ANDsrNode>;

def ASL : WDCImpliedInstruction<0x0A, (outs AccumulatorRegisterClass:$dst), (ins AccumulatorRegisterClass:$src), 
                                 "ASL\tA", [(set AccumulatorRegisterClass:$dst, (shl AccumulatorRegisterClass:$src, OnePattern))]>;

def CMPi : WDCImmediateInstruction<0xc9, (outs StatusReg:$sr), (ins AccumulatorRegisterClass:$src, UnsignedImm16Operand:$constant),
                                   "CMP\t$constant", []>;
def CMPsr : WDCStackRelativeInstruction<0xc3, (outs StatusReg:$sr),
                                        (ins AccumulatorRegisterClass:$src, StackOffsetOperand:$spoffset),
                                        "CMP\t$spoffset", []>;

def EORi : WDCLogicInstructionImmediate<0x49, xor, "EOR", UnsignedImm16Operand>;
def EORsr : InstructionLogicStackRelative<0x43, "EOR", EORsrNode>;

def SUBsr : ADDSUBStackRelativePseudo<SUBsrNode>;

def SBCi   : WDCAddSubCarryInstructionImmediate<0xE9, subc, "SBC", UnsignedImm16Operand>;
def SBCsr  : WDCAddSubCarryInstructionStackRelative<0xE3,subc, "SBC">;

def LSR : WDCImpliedInstruction<0x4A, (outs AccumulatorRegisterClass:$dst), (ins AccumulatorRegisterClass:$src),
                                "LSR\tA", 
                                [(set AccumulatorRegisterClass:$dst, (srl AccumulatorRegisterClass:$src, OnePattern))]>;

def ORAi   : WDCLogicInstructionImmediate<0x09, or, "ORA", UnsignedImm16Operand>;
def ORAsr  : InstructionLogicStackRelative<0x03, "ORA", ORAsrNode>;

def PHB : WDCStackInstruction<0x0B, "PHB">;
def PHP : WDCStackInstruction<0x08, "PHP">;
def PLB : WDCStackInstruction<0xAB, "PLB">;
def PLP : WDCStackInstruction<0x28, "PLP">;

def ROR : WDCImpliedInstruction<0x2A, (outs AccumulatorRegisterClass:$dst), (ins AccumulatorRegisterClass:$src),
                                "ROR\tA", []>;

def ROTL : WDCPseudoInst<(outs AccumulatorRegisterClass:$dst), (ins AccumulatorRegisterClass:$src),
                         [(set AccumulatorRegisterClass:$dst, (rotl AccumulatorRegisterClass:$src, OnePattern))]>;

def SEP : InstructionImm8<0xE2, "SEP">;

def SRA : WDCPseudoInst<(outs AccumulatorRegisterClass:$dst), (ins AccumulatorRegisterClass:$src),
                         [(set AccumulatorRegisterClass:$dst, (sra AccumulatorRegisterClass:$src, OnePattern))]>;

def REP : InstructionImm8<0xC2, "REP">;

let isReturn=1, isTerminator=1, isBarrier=1, hasCtrlDep=1 in {
  def RetRTL : WDCPseudoInst<(outs), (ins), [(WDCRetNode)]>;
}

def XBA : WDCImpliedInstruction<0xEB, (outs AccumulatorRegisterClass:$dst), (ins AccumulatorRegisterClass:$src),
                                "XBA", [(set AccumulatorRegisterClass:$dst, (bswap AccumulatorRegisterClass:$src))]>;

// Small immediates
def : Pat<(i16 SignedImm16Pattern:$in), (LDAi imm:$in)>;
def : Pat<(seteq AccumulatorRegisterClass:$src, UnsignedImm16Pattern:$const), (
  ANDi (LSR (COPY_TO_REGCLASS (CMPi AccumulatorRegisterClass:$src, imm:$const), AccumulatorRegisterClass)), 1)>;
def : Pat<(SETCCsrNode AccumulatorRegisterClass:$src, frameIndexPattern:$spoffset, SETEQ),
          (ANDi (LSR (COPY_TO_REGCLASS (CMPsr AccumulatorRegisterClass:$src, $spoffset), AccumulatorRegisterClass)), 1)>;
// def : Pat<(zext StatusReg:$in), (ANDi (COPY_TO_REGCLASS $in, AccumulatorRegisterClass), 0x00FF)>;
