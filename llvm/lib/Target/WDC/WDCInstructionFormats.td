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

def UnsignedImm8Operand : Operand<i8> {
  let PrintMethod = "printUnsignedImm8";
}

// Generic WDC Instruction Format
class Inst<bits<8> opcode, dag outs, dag ins, string asmstr, list<dag> pattern> : Instruction
{
  // Inst and Size: for tablegen(... -gen-emitter) and 
  // tablegen(... -gen-disassembler) in CMakeLists.txt
  AddressMode m_addrMode;
  let Namespace = "WDC";

  let Size = 0;

  // All instructions have an 8-bit opcode.
  bits<8> Opcode = opcode;

  let OutOperandList = outs;
  let InOperandList  = ins;

  let AsmString   = asmstr;
  let Pattern     = pattern;
  let Itinerary   = NoItinerary;

  let DecoderNamespace = "WDC";
  let hasNoSchedulingInfo = true;
}

class InstImm16<bits<8> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
: Inst<opcode, outs, ins, asmstr, pattern>
{
  field bits<24> Inst;
  bits<16> constant;
  let Size = 3;
  let Inst{23-8} = constant;
  let Inst{7-0} = Opcode;
  let m_addrMode = WDCAddressModeImmediate;
}

class InstImm8<bits<8> opcode, string asmstr> 
: Inst<opcode, (outs), (ins UnsignedImm8Operand:$const), !strconcat(asmstr, "\t$const"), []> {
  field bits<16> Inst;
  bits<8> const;
  let Size = 2;
  let Inst{7-0} = Opcode;
  let Inst{15-8} = const;
}

class InstImpl<bits<8> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
: Inst<opcode, outs, ins, asmstr, pattern>
{
  field bits<8> Inst;
  let Size = 1;
  let Inst = Opcode;
  let m_addrMode = WDCAddressModeImplied;
}

class InstStkRel<bits<8> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
: Inst<opcode, outs, ins, asmstr, pattern>
{
  field bits<16> Inst;
  bits<8> spoffset;
  let Size = 2;
  let Inst{15-8} = spoffset; 
  let Inst{7-0} = Opcode;
  let mayLoad = true;
}

class InstAbsLong<bits<8> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
: Inst<opcode, outs, ins, asmstr, pattern> {
  field bits<32> Inst;
  bits<24> LongPtr;
  let Size = 4;
  let Inst{7-0} = Opcode;
  let mayLoad = true;
}

class InstDrctPgIndrctLong<bits<8> opcode, dag outs, dag ins, string asmstr, list<dag> pattern>
: Inst<opcode, outs, ins, asmstr, pattern> {
  field bits<16> Inst;
  bits<8> DrctPgAddr;
  let Size = 2;
  let Inst{7-0} = Opcode;
  let mayLoad = true;
}

def SignedImm16Operand      : Operand<i16> {
  let DecoderMethod= "DecodeSimm16";
}
def UnsignedImm16Operand : Operand<i16> {
  let DecoderMethod="DecodeUimm16";
  let PrintMethod = "printUnsignedImm16";
}

def SignedImm8Operand : Operand<i8> {
  let DecoderMethod="DecodeSimm8";
}

def StackOffsetOperand : Operand<iPTR> {
  let PrintMethod = "printStackOffset";
}

def OprndDrctPgIndrct : Operand<iPTR> {
  let PrintMethod = "printDirectPageIndirect";
}

def OprndAbsLong : Operand<iPTR> {
  let PrintMethod = "printAbsoluteLong";
}

// Pattern 
def SignedImm16Pattern  : PatLeaf<(imm), [{ return isInt<16>(N->getSExtValue()); }]>;
def UnsignedImm16Pattern : PatLeaf<(imm), [{ return isUInt<16>(N->getZExtValue()); }]>;
def SignedImm8Pattern : PatLeaf<(imm), [{ return isInt<8>(N->getSExtValue()); }]>;
def OnePattern : PatLeaf<(i16 imm), [{ return N->isOne(); }]>;

let WantsRoot = true in
def frameIndexPattern : ComplexPattern<i16, 1, "SelectFrameIndex", [frameindex]>;

class WDCLogicInstructionImmediate<
    bits<8>   op,
    SDNode    opNode,
    string    asmstr,
    Operand   oprnd
>
: InstImm16<
    op,
    (outs AccumulatorRegisterClass:$dst),
    (ins AccumulatorRegisterClass:$accumulator, oprnd:$constant),
    !strconcat(asmstr, "\t$constant"),
    [(set AccumulatorRegisterClass:$dst, (opNode AccumulatorRegisterClass:$accumulator, UnsignedImm16Pattern:$constant))]
  >
{
  let DisableEncoding = "$dst,$accumulator";
  let Constraints = "$dst = $accumulator";
  let isCommutable = true;
}

class WDCAddSubCarryInstructionImmediate<
    bits<8>   op,
    SDNode    opNode,
    string    asmstr,
    Operand   oprnd
>
: InstImm16<
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
: InstStkRel<
    op,
    (outs AccumulatorRegisterClass:$dst),
    (ins AccumulatorRegisterClass:$accumulator, StackOffsetOperand:$spoffset),
    !strconcat(asmstr, "\t$spoffset"),
    [(set AccumulatorRegisterClass:$dst, (opNode AccumulatorRegisterClass:$accumulator, (frameIndexPattern:$spoffset)))]
  >
{
  let DisableEncoding = "$dst,$accumulator";
  let mayLoad = 1;
}

class WDCLoadInstructionImmediate<
    bits<8> op,
    string  asmstr,
    Operand operand>
: InstImm16<
    op,
    (outs AccumulatorRegisterClass:$dst),
    (ins operand:$constant),
    !strconcat(asmstr, "\t$constant"),
    [(set AccumulatorRegisterClass:$dst, (load SignedImm16Pattern:$constant))]
  >
{
  let DisableEncoding = "$dst";
}

class InstPseudo<dag outs, dag ins, list<dag> pattern>:
      Inst<0, outs, ins, "", pattern> {
  let isCodeGenOnly = 1;
  let isPseudo = 1;
  let m_addrMode = WDCAddressModePseudo;
}

class WDCReturnInstruction<bits<8> opcode, string asmstr>
: InstImpl<opcode, (outs), (ins), asmstr, []>
{
  let isReturn = 1;
}

class WDCTransferInstruction<bits<8> opcode, dag outs, dag ins, string asmstr>
: InstImpl<opcode, outs, ins, asmstr, []>
{
  // let DisableEncoding = "$src,$dst";
}

class WDCStatusControlInstruction<bits<8> opcode, string asmstr, RegisterClass dest_rc, list<dag> pattern>
: InstImpl<opcode, (outs dest_rc:$carryout), (ins), asmstr, pattern>
{
  let DisableEncoding = "$carryout";
}

class WDCStackInstruction<bits<8> opcode, string asmstr>
: InstImpl<opcode, (outs), (ins), asmstr, []>
{
}

class WDCStoreInstructionStackRelative<bits<8> opcode, string asmstr>
: InstStkRel<
  opcode,
  (outs),
  (ins AccumulatorRegisterClass:$accumulator, StackOffsetOperand:$spoffset),
  !strconcat(asmstr, "\t$spoffset"),
  [(store AccumulatorRegisterClass:$accumulator, frameIndexPattern:$spoffset)]>
{
  let DisableEncoding = "$accumulator";
  let mayStore = 1;
}

class WDCLoadInstructionStackRelative<
  bits<8> opcode,
  string asmstr>
: InstStkRel<
    opcode,
    (outs AccumulatorRegisterClass:$dst),
    (ins StackOffsetOperand:$spoffset),
    !strconcat(asmstr, "\t$spoffset"),
    [(set AccumulatorRegisterClass:$dst, (load frameIndexPattern:$spoffset))]>
{
  let DisableEncoding = "$dst";
  let canFoldAsLoad = 1;
}


class InstructionLogicStackRelative<
  bits<8> opcode,
  string  asmstr,
  SDNode  opnode
> : InstStkRel<opcode, 
                                (outs AccumulatorRegisterClass:$dst),
                                (ins AccumulatorRegisterClass:$src, StackOffsetOperand:$spoffset),
                                !strconcat(asmstr, "\t$spoffset"),
                                [(set AccumulatorRegisterClass:$dst, 
                                      (opnode AccumulatorRegisterClass:$src, 
                                              frameIndexPattern:$spoffset)
                                 )
                                ]
    > {
  let Constraints = "$dst = $src";
}