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

// Return
def WDCRetNode : SDNode<"WDCISD::Ret", SDTNone,
                     [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def mem_stack : Operand<iPTR> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops StackRegisterClass, SignedImm8Operand);
  let EncoderMethod = "getMemEncoding";
}

def ORAi   : WDCLogicInstructionImmediate<0x09, or, "ORA", UnsignedImm16Operand>;
def LDAi   : WDCLoadInstructionImmediate<0xA9, "LDA", SignedImm16Operand>;
def LDAsr  : WDCLoadInstructionStackRelative<0xA3, "LDA", mem_stack>;
def STAsr  : WDCStoreInstructionStackRelative<0x83, "STA", mem_stack>;
def RTL    : WDCReturnInstruction<0x6b, "RTL">;
def TSC    : WDCTransferInstruction<0x3B, "TSC", AccumulatorRegisterClass, StackRegisterClass>;
def TCS    : WDCTransferInstruction<0x1B, "TCS", StackRegisterClass, AccumulatorRegisterClass>;
def CLC    : WDCTransferInstruction<0x18, "CLC", StatusReg, StatusReg>;
def SEC    : WDCTransferInstruction<0x38, "SEC", StatusReg, StatusReg>;
def ADCi   : WDCAddSubCarryInstructionImmediate<0x69, addc, "ADC", UnsignedImm16Operand>;
def SBCi   : WDCAddSubCarryInstructionImmediate<0xE9, subc, "SBC", UnsignedImm16Operand>;

let isReturn=1, isTerminator=1, isBarrier=1, hasCtrlDep=1 in {
  def RetRTL : WDCPseudoInst<(outs), (ins), "", [(WDCRetNode)]>;
}

// Small immediates
def : Pat<(i16 SignedImm16Pattern:$in), (LDAi imm:$in)>;

