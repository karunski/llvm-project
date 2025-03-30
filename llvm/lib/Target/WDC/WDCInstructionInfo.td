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

def ORAi   : WDCLogicInstructionImmediate<0x09, or, "ORA", UnsignedImm16Operand>;
def LDAi   : WDCLoadInstructionImmediate<0xA9, "LDA", SignedImm16Operand>;
def LDAsr  : WDCLoadInstructionStackRelative<0xA3, "LDA">;
def STAsr  : WDCStoreInstructionStackRelative<0x83, "STA">;
def RTL    : WDCReturnInstruction<0x6b, "RTL">;
def TSC    : WDCTransferInstruction<0x3B, "TSC">;
def TDC    : WDCTransferInstruction<0x7D, "TDC">;
def TCS    : WDCTransferInstruction<0x1B, "TCS">;
def TCD    : WDCTransferInstruction<0x5D, "TCD">;

def CLC    : WDCStatusControlInstruction<0x18, "CLC", CarryRegisterClass, []>;
def SEC    : WDCStatusControlInstruction<0x38, "SEC", CarryRegisterClass, []>;

let isAdd = 1 in {
  def ADCi  : WDCAddSubCarryInstructionImmediate<0x69, adde, "ADC", UnsignedImm16Operand>;
  def ADCsr : WDCAddSubCarryInstructionStackRelative<0x63, addc, "ADC">;
  def ADDsr : ADDSUBStackRelativePseudo<ADDsrNode>;
}

def SUBsr : ADDSUBStackRelativePseudo<SUBsrNode>;

def ADDi : ADDImmediatePseudo;

def SBCi   : WDCAddSubCarryInstructionImmediate<0xE9, subc, "SBC", UnsignedImm16Operand>;
def SBCsr  : WDCAddSubCarryInstructionStackRelative<0xE3,subc, "SBC">;

def PHB : WDCStackInstruction<0x0B, "PHB">;
def PLB : WDCStackInstruction<0xAB, "PLB">;

let isReturn=1, isTerminator=1, isBarrier=1, hasCtrlDep=1 in {
  def RetRTL : WDCPseudoInst<(outs), (ins), [(WDCRetNode)]>;
}

// Small immediates
def : Pat<(i16 SignedImm16Pattern:$in), (LDAi imm:$in)>;
// def : Pat<(i1  ClearCarryPattern), (CLC)>;
