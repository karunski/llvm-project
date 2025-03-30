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
// def WDCAddressModeStackRelative : AddressMode<3>;

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
  let m_addrMode = WDCAddressModeImmediate;
}

class WDCImpliedInstruction<bits<8> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
: WDCInst<outs, ins, asmstr, pattern>
{
  field bits<8> Inst;
  let Size = 1;
  let Opcode = opcode;
  let Inst = opcode;
  let m_addrMode = WDCAddressModeImplied;
}

class WDCStackRelativeInstruction<bits<8> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
: WDCInst<outs, ins, asmstr, pattern>
{
  field bits<16> Inst;
  let Size = 2;
  let Opcode = opcode;
  let Inst{15-8} = opcode; 
}

def SignedImm16Operand      : Operand<i16> {
  let DecoderMethod= "DecodeSimm16";
}
def UnsignedImm16Operand : Operand<i16> {
  let DecoderMethod="DecodeUimm16";
}

def SignedImm8Operand : Operand<i8> {
  let DecoderMethod="DecodeSimm8";
}

def stack_offset : Operand<i16> {
  let PrintMethod = "printStackOffset";
}

// Pattern 
def SignedImm16Pattern  : PatLeaf<(imm), [{ return isInt<16>(N->getSExtValue()); }]>;
def UnsignedImm16Pattern : PatLeaf<(imm), [{ return isUInt<16>(N->getZExtValue()); }]>;
def SignedImm8Pattern : PatLeaf<(imm), [{ return isInt<8>(N->getSExtValue()); }]>;

let WantsRoot = true in
def addr : ComplexPattern<iPTR, 2, "SelectAddr", [frameindex]>;

class WDCLogicInstructionImmediate<
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
    [(set AccumulatorRegisterClass:$dst, (opNode AccumulatorRegisterClass:$accumulator, UnsignedImm16Pattern:$constant))]
  >
{
  let DisableEncoding = "$dst,$accumulator";
}

class WDCAddSubCarryInstructionImmediate<
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
    [(set AccumulatorRegisterClass:$dst, (opNode AccumulatorRegisterClass:$accumulator, UnsignedImm16Pattern:$constant))]
  >
{
  let DisableEncoding = "$dst,$accumulator";
}

class WDCAddSubCarryInstructionStackRelative<bits<8> op, SDNode opNode, string asmstr>
: WDCStackRelativeInstruction<
    op,
    (outs AccumulatorRegisterClass:$dst),
    (ins AccumulatorRegisterClass:$accumulator, stack_offset:$spoffset),
    !strconcat(asmstr, "\t$spoffset"),
    [(set AccumulatorRegisterClass:$dst, (opNode AccumulatorRegisterClass:$accumulator, (i16 imm:$spoffset)))]
  >
{
  let DisableEncoding = "$dst,$accumulator";
  let mayLoad = 1;
}

class WDCLoadInstructionImmediate<
    bits<8> op,
    string  asmstr,
    Operand operand>
: WDCImmediateInstruction<
    op,
    (outs AccumulatorRegisterClass:$dst),
    (ins operand:$constant),
    !strconcat(asmstr, "\t$constant"),
    [(set AccumulatorRegisterClass:$dst, (load SignedImm16Pattern:$constant))]
  >
{
  let DisableEncoding = "$dst";
}

class WDCPseudoInst<dag outs, dag ins, list<dag> pattern>:
      WDCInst<outs, ins, "", pattern> {
  let isCodeGenOnly = 1;
  let isPseudo = 1;
  let m_addrMode = WDCAddressModePseudo;
}



class WDCReturnInstruction<bits<8> opcode, string asmstr>
: WDCImpliedInstruction<opcode, (outs), (ins), asmstr, []>
{
  let isReturn = 1;
}

class WDCTransferInstruction<bits<8> opcode, string asmstr>
: WDCImpliedInstruction<opcode, (outs), (ins), asmstr, []>
{
  // let DisableEncoding = "$src,$dst";
}

class WDCStatusControlInstruction<bits<8> opcode, string asmstr, RegisterClass dest_rc, list<dag> pattern>
: WDCImpliedInstruction<opcode, (outs dest_rc:$carryout), (ins), asmstr, pattern>
{
  let DisableEncoding = "$carryout";
}

class WDCStoreInstructionStackRelative<bits<8> opcode, string asmstr, Operand operand>
: WDCStackRelativeInstruction<
  opcode,
  (outs),
  (ins AccumulatorRegisterClass:$accumulator, operand:$addr),
  !strconcat(asmstr, "\t$addr"),
  [(store AccumulatorRegisterClass:$accumulator, addr:$addr)]>
{
  let DisableEncoding = "$accumulator";
  let mayStore = 1;
}

class WDCLoadInstructionStackRelative<
  bits<8> opcode,
  string asmstr,
  Operand operand>
: WDCStackRelativeInstruction<
    opcode,
    (outs AccumulatorRegisterClass:$dst),
    (ins operand:$addr),
    !strconcat(asmstr, "\t$addr"),
    [(set AccumulatorRegisterClass:$dst, (load addr:$addr))]>
{
  let DisableEncoding = "$dst";
  let canFoldAsLoad = 1;
}

