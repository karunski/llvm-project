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

def ORAi   : WDCLogicInstructionImmmediate<0x09, or, "ORA", SignedImm16Operand>;
def LDAi   : WDCLoadInstructionImmediate<0xA9, ld, "LDA", SignedImm16Operand>;
def RTL    : WDCReturnInstruction<0x6b, "RTL">;

let isReturn=1, isTerminator=1, isBarrier=1, hasCtrlDep=1 in {
  def RetRTL : WDCPseudoInst<(outs), (ins), "", [(WDCRetNode)]>;
}

// Small immediates
def : Pat<(i16 SignedImm16Pattern:$in), (LDAi imm:$in)>;