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

def WDCAddStackRelativeNode : SDNode<
  "WDCISD::ADDsr", 
  SDTypeProfile<1, 2, [SDTCisInt<0>, SDTCisInt<1>]>,
  [SDNPHasChain]
>;

def WDCAddImmediateNode : SDNode<
  "WDCISD::ADDi",
  SDTypeProfile<1, 2, [SDTCisInt<0>, SDTCisInt<1>]>,
  []
>;

def mem_stack : Operand<iPTR> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops S, SignedImm8Operand);
  let EncoderMethod = "getMemEncoding";
}

def ORAi   : WDCLogicInstructionImmediate<0x09, or, "ORA", UnsignedImm16Operand>;
def LDAi   : WDCLoadInstructionImmediate<0xA9, "LDA", SignedImm16Operand>;
def LDAsr  : WDCLoadInstructionStackRelative<0xA3, "LDA", mem_stack>;
def STAsr  : WDCStoreInstructionStackRelative<0x83, "STA", mem_stack>;
def RTL    : WDCReturnInstruction<0x6b, "RTL">;
def TSC    : WDCTransferInstruction<0x3B, "TSC">;
def TCS    : WDCTransferInstruction<0x1B, "TCS">;

def CLC    : WDCStatusControlInstruction<0x18, "CLC", CarryRegisterClass, []>;
def SEC    : WDCStatusControlInstruction<0x38, "SEC", CarryRegisterClass, []>;
def ADCi   : WDCAddSubCarryInstructionImmediate<0x69, adde, "ADC", UnsignedImm16Operand>;
def ADCsr  : WDCAddSubCarryInstructionStackRelative<0x63, addc, "ADC">;

class ADDStackRelativePseudo
: WDCPseudoInst<
  (outs AccumulatorRegisterClass:$dst),
  (ins AccumulatorRegisterClass:$src, Operand<i16>:$slot),
  [(set AccumulatorRegisterClass:$dst, (WDCAddStackRelativeNode AccumulatorRegisterClass:$src, frameIndexPattern:$slot))]
>
{
  let hasCtrlDep = 1;
  let isCommutable = 0;
  let isAdd = 1;
  let Constraints = "$dst = $src";
  let mayLoad = 1;
}

def ADDsr : ADDStackRelativePseudo;

class ADDImmediatePseudo
: WDCPseudoInst<
  (outs AccumulatorRegisterClass:$dst),
  (ins AccumulatorRegisterClass:$src, Operand<i16>:$constant),
  [(set AccumulatorRegisterClass:$dst, (WDCAddImmediateNode AccumulatorRegisterClass:$src, (i16 imm:$constant)))]
>
{
  let isCommutable = 0;
  let isAdd = 1;
  let Constraints = "$dst = $src";
  let mayLoad = 1;
}
def ADDi : ADDImmediatePseudo;

def SBCi   : WDCAddSubCarryInstructionImmediate<0xE9, subc, "SBC", UnsignedImm16Operand>;

let isReturn=1, isTerminator=1, isBarrier=1, hasCtrlDep=1 in {
  def RetRTL : WDCPseudoInst<(outs), (ins), [(WDCRetNode)]>;
}

// Small immediates
def : Pat<(i16 SignedImm16Pattern:$in), (LDAi imm:$in)>;
// def : Pat<(i1  ClearCarryPattern), (CLC)>;
