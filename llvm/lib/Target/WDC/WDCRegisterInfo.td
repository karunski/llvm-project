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
 // def ZERO : Cpu0GPRReg<0,  "zero">, DwarfRegNum<[0]>;
 // def AT   : Cpu0GPRReg<1,  "1">,    DwarfRegNum<[1]>;
 // def V0   : Cpu0GPRReg<2,  "2">,    DwarfRegNum<[2]>;
 // def V1   : Cpu0GPRReg<3,  "3">,    DwarfRegNum<[3]>;
 // def A0   : Cpu0GPRReg<4,  "4">,    DwarfRegNum<[4]>;
 // def A1   : Cpu0GPRReg<5,  "5">,    DwarfRegNum<[5]>;
 // def T9   : Cpu0GPRReg<6,  "t9">,   DwarfRegNum<[6]>;
 // def T0   : Cpu0GPRReg<7,  "7">,    DwarfRegNum<[7]>;
 // def T1   : Cpu0GPRReg<8,  "8">,    DwarfRegNum<[8]>;
 // def S0   : Cpu0GPRReg<9,  "9">,    DwarfRegNum<[9]>;
 // def S1   : Cpu0GPRReg<10, "10">,   DwarfRegNum<[10]>;
 // def GP   : Cpu0GPRReg<11, "gp">,   DwarfRegNum<[11]>;
 // def FP   : Cpu0GPRReg<12, "fp">,   DwarfRegNum<[12]>;
 // def SP   : Cpu0GPRReg<13, "sp">,   DwarfRegNum<[13]>;
 // def LR   : Cpu0GPRReg<14, "lr">,   DwarfRegNum<[14]>;
 // def SW   : Cpu0GPRReg<15, "sw">,   DwarfRegNum<[15]>;
//  def MAR  : Register< 16, "mar">,  DwarfRegNum<[16]>;
//  def MDR  : Register< 17, "mdr">,  DwarfRegNum<[17]>;

  def S  : WDCArchReg<"S">,   DwarfRegNum<[3]>;
  def PC : WDCArchReg<"PC">,  DwarfRegNum<[4]>;
  def P :  WDCArchReg<"P">,   DwarfRegNum<[5]>;
}

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
def StatusReg : RegisterClass<"WDC", [i8], 8, (add S)>;

//@Architectural Registers class
def ArchRegs : RegisterClass<"WDC", [i16], 8, (add PC)>;

def OutRegs : RegisterClass<"WDC", [i16], 8, (add A, X, Y)>;