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

class WDCReg<string name> : Register<name> {
  // For tablegen(... -gen-emitter)  in CMakeLists.txt  
  let Namespace = "WDC";
}


class WDCFakeReg<string name> : WDCReg<name> {
  // let isArtificial = true;
}

//===----------------------------------------------------------------------===//
//@Registers
//===----------------------------------------------------------------------===//
// The register string, such as "9" or "gp" will show on "llvm-objdump -d"
//@ All registers definition
//@ General Purpose Registers
def A : WDCReg<"A">, DwarfRegNum<[1]>;
def B : WDCReg<"B">, DwarfRegNum<[2]>;
def X : WDCReg<"X">, DwarfRegNum<[3]>;
def Y : WDCReg<"Y">, DwarfRegNum<[4]>;
def S  : WDCReg<"S">,   DwarfRegNum<[5]>;
def PC : WDCReg<"PC">,  DwarfRegNum<[6]>;
def Carry : WDCReg<"Carry">;
def M : WDCReg<"M">;

def C : WDCReg<"C">, DwarfRegNum<[0]> {
  let SubRegs = [A,B];
  let SubRegIndices = [SubRegIndex<8,0>, SubRegIndex<8,8>];
  let CoveredBySubRegs = 1;
}

def ADDR0 : WDCFakeReg<"ADDR0"> {
    let SubRegs = [C,Y];
    let SubRegIndices = [SubRegIndex<8,0>, SubRegIndex<8,8>];
}

def P : WDCReg<"P">, DwarfRegNum<[7]>;

//===----------------------------------------------------------------------===//
//@Register Classes
//===----------------------------------------------------------------------===//

def CPURegs : RegisterClass<"WDC", [i16], 8, (add
  // Accumulator
  C,
  // Index Registers
  X, Y,
  // Reserved
  S, P
)>;

def RegsA16 : RegisterClass<"WDC", [i16], 8, (add C)>;
def RegsA8 : RegisterClass<"WDC", [i8], 8, (add A,B)>;

def IndexRegs : RegisterClass<"WDC", [i16], 8, (add X, Y)>;
def RegsX16 : RegisterClass<"WDC", [i16], 8, (add X)>;
def RegsY16 : RegisterClass<"WDC", [i16], 8, (add Y)>;
def RegsGP16 : RegisterClass<"WDC", [i16], 8, (add X,Y,C)>;

//@Status Registers class
def StatusReg : RegisterClass<"WDC", [i8], 8, (add P)>;

//@Architectural Registers class
def ArchRegs : RegisterClass<"WDC", [i16], 8, (add PC, S)>;

// def OutRegs : RegisterClass<"WDC", [i16], 8, (add A, X, Y)>;

def FakeRegs : RegisterClass<"WDC", [i32], 8, (add ADDR0)>;

def Flags : RegisterClass<"WDC", [i1], 8, (add M,Carry)>;

