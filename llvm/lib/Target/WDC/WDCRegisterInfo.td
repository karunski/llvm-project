//===-- WDCRegisterInfo.td - 65816 Register defs -----------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the  65816 register file
//===----------------------------------------------------------------------===//

// We have banks of 16 registers each.
class WDCReg<string name> : Register<name> {
  // For tablegen(... -gen-emitter)  in CMakeLists.txt  
  let Namespace = "WDC";
}

// WDC65816 CPU Registers
class WDCGeneralPurposeReg<string name> : WDCReg<name>;
//class Cpu0GPRReg<bits<16> Enc, string n> : Cpu0Reg<Enc, n>;

// WDC65816 Architectural Registers
class WDCArchReg<string name> : WDCReg<name>;
//class Cpu0C0Reg<bits<16> Enc, string n> : Cpu0Reg<Enc, n>;

//===----------------------------------------------------------------------===//
//@Registers
//===----------------------------------------------------------------------===//
// The register string, such as "9" or "gp" will show on "llvm-objdump -d"
//@ All registers definition
let Namespace = "WDC" in {
  //@ General Purpose Registers
  def A : WDCGeneralPurposeReg<"A">, DwarfRegNum<[0]>;
  def X : WDCGeneralPurposeReg<"X">, DwarfRegNum<[1]>;
  def Y : WDCGeneralPurposeReg<"Y">, DwarfRegNum<[2]>;
  def S  : WDCArchReg<"S">,   DwarfRegNum<[3]>;
  def PC : WDCArchReg<"PC">,  DwarfRegNum<[4]>;
  def CARRY : WDCReg<"C">;
}

class WDCStatusReg : WDCArchReg<"P">
{
  let SubRegs = [CARRY];
  let SubRegIndices = [SubRegIndex<1, 0>];
}

def P : WDCStatusReg, DwarfRegNum<[6]>;

//===----------------------------------------------------------------------===//
//@Register Classes
//===----------------------------------------------------------------------===//

def CPURegs : RegisterClass<"WDC", [i16], 8, (add
  // Accumulator
  A,
  // Index Registers
  X, Y,
  // Reserved
  S, P
)>;

def AccumulatorRegisterClass : RegisterClass<"WDC", [i16], 8, (add A)>;

def IndexRegs : RegisterClass<"WDC", [i16], 8, (add X, Y)>;

//@Status Registers class
def StatusReg : RegisterClass<"WDC", [i8], 8, (add P)>;

//@Architectural Registers class
def ArchRegs : RegisterClass<"WDC", [i16], 8, (add PC, S)>;

def OutRegs : RegisterClass<"WDC", [i16], 8, (add A, X, Y)>;

let Size=8 in {
  def CarryRegisterClass : RegisterClass<"WDC", [i1], 8, (add CARRY)>;
}