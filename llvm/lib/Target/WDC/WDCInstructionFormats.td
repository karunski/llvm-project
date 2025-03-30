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

def WDCImmediate : AddressMode<0>;

// Generic Cpu0 Format
class WDCInst<dag outs, dag ins, string asmstr, list<dag> pattern, AddressMode addrMode> : Instruction
{
  // Inst and Size: for tablegen(... -gen-emitter) and 
  // tablegen(... -gen-disassembler) in CMakeLists.txt
  AddressMode m_addrMode = addrMode;
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

class WDCLogicInstruction<
    bits<8>       op,
    dag           outs, 
    dag           ins, 
    string        asmstr, 
    list<dag>     pattern, 
    AddressMode   addrMode>
: WDCInst<outs, ins, asmstr, pattern, addrMode>
{
  let Opcode = op;
}

// Signed 16 bit immediate
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
: WDCLogicInstruction<
    op,
    (outs AccumulatorRegisterClass:$dst),
    (ins AccumulatorRegisterClass:$accumulator, oprnd:$constant),
    !strconcat(asmstr, "\t$constant"),
    [(set AccumulatorRegisterClass:$dst, (opNode AccumulatorRegisterClass:$accumulator, SignedImm16Pattern:$constant))],
    WDCImmediate
  >
{
  field bits<24> Inst;
  let Size = 3;
  let Inst{23-16} = op;
  let DisableEncoding = "$dst,$accumulator";
}

