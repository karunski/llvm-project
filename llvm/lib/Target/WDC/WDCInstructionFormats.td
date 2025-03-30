//===-- WDCInstructionFormats.td - 65816 Instruction Formats -----*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Describe WDC Instructions:
//
// Every instruction consists of an 8-bit opcode followed
// by operands.  The number of operands and their meaning depend on the address mode.
//
//  opcode  - operation code.
//  addr    - absolute address
//===----------------------------------------------------------------------===//

class AddressMode<bits<4> val> {
  bits<4> Value = val;
}

def WDCAddressModePseudo : AddressMode<0>;
def WDCAddressModeImmediate : AddressMode<1>;
def WDCAddressModeImplied : AddressMode<2>;

// Generic Cpu0 Format
class WDCInst<dag outs, dag ins, string asmstr, list<dag> pattern> : Instruction
{
  // Inst and Size: for tablegen(... -gen-emitter) and 
  // tablegen(... -gen-disassembler) in CMakeLists.txt
  AddressMode m_addrMode;
  let Namespace = "WDC";

  let Size = 0;

  bits<8> Opcode = 0;

  let OutOperandList = outs;
  let InOperandList  = ins;

  let AsmString   = asmstr;
  let Pattern     = pattern;
  let Itinerary   = NoItinerary;

  let DecoderNamespace = "WDC";
  let hasNoSchedulingInfo = true;
}

class WDCImmediateInstruction<bits<8> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
: WDCInst<outs, ins, asmstr, pattern>
{
  field bits<24> Inst;
  let Size = 3;
  let Opcode = opcode;
  let Inst{23-16} = opcode;
  let m_addrMode = WDCAddressModeImplied;
}

class WDCImpliedInstruction<bits<8> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
: WDCInst<outs, ins, asmstr, pattern>
{
  field bits<8> Inst;
  let Size = 3;
  let Opcode = opcode;
  let Inst = opcode;
  let m_addrMode = WDCAddressModeImmediate;
}

def SignedImm16Operand      : Operand<i16> {
  let DecoderMethod= "DecodeSimm16";
}

// Pattern 
def SignedImm16Pattern  : PatLeaf<(imm), [{ return isInt<16>(N->getSExtValue()); }]>;

class WDCLogicInstructionImmmediate<
    bits<8>   op,
    SDNode    opNode,
    string    asmstr,
    Operand   oprnd
>
: WDCImmediateInstruction<
    op,
    (outs AccumulatorRegisterClass:$dst),
    (ins AccumulatorRegisterClass:$accumulator, oprnd:$constant),
    !strconcat(asmstr, "\t$constant"),
    [(set AccumulatorRegisterClass:$dst, (opNode AccumulatorRegisterClass:$accumulator, SignedImm16Pattern:$constant))]
  >
{
  let DisableEncoding = "$dst,$accumulator";
}

class WDCLoadInstructionImmediate<
    bits<8> op,
    SDNode  opnode,
    string  asmstr,
    Operand operand>
: WDCImmediateInstruction<
    op,
    (outs AccumulatorRegisterClass:$dst),
    (ins operand:$constant),
    !strconcat(asmstr, "\t$constant"),
    [(set AccumulatorRegisterClass:$dst, (opnode SignedImm16Pattern:$constant))]
  >
{
  let DisableEncoding = "$dst";
}

class WDCPseudoInst<dag outs, dag ins, string asmstr, list<dag> pattern>:
      WDCInst<outs, ins, asmstr, pattern> {
  let isCodeGenOnly = 1;
  let isPseudo = 1;
  let m_addrMode = WDCAddressModePseudo;
}

class WDCReturnInstruction<bits<8> opcode, string asmstr>
: WDCImpliedInstruction<opcode, (outs), (ins), asmstr, []>
{
  let isReturn = 1;
}