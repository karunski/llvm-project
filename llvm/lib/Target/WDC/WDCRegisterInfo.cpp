//===-- WDCRegisterInfo.cpp - WDC Register Information -== --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the WDC implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "WDC-reg-info"

#include "WDCRegisterInfo.h"

#include "WDC.h"
#include "WDCSubtarget.h"
#include "WDCMachineFunction.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#define GET_REGINFO_TARGET_DESC
#include "WDCGenRegisterInfo.inc"

using namespace llvm;

WDCRegisterInfo::WDCRegisterInfo(const WDCSubtarget &ST)
    : WDCGenRegisterInfo{WDC::A}, Subtarget{ST} {}

//===----------------------------------------------------------------------===//
// Callee Saved Registers methods
//===----------------------------------------------------------------------===//
/// WDC Callee Saved Registers
// In WDCCallConv.td,
// def CSR_O32 : CalleeSavedRegs<(add LR, FP,
//                                   (sequence "S%u", 2, 0))>;
// llc create CSR_O32_SaveList and CSR_O32_RegMask from above defined.
const MCPhysReg *
WDCRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_O32_SaveList;
}

const uint32_t *
WDCRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID) const {
  return CSR_O32_RegMask; 
}

// pure virtual method
//@getReservedRegs {
BitVector WDCRegisterInfo::
getReservedRegs(const MachineFunction &MF) const {
//@getReservedRegs body {
  static const uint16_t ReservedCPURegs[] = {
    //WDC::ZERO, WDC::AT, WDC::SP, WDC::LR, /*WDC::SW, */WDC::PC
    WDC::P, WDC::PC, WDC::S
  };
  BitVector Reserved(getNumRegs());

  for (unsigned I = 0; I < std::size(ReservedCPURegs); ++I)
    Reserved.set(ReservedCPURegs[I]);

  return Reserved;
}

//@eliminateFrameIndex {
//- If no eliminateFrameIndex(), it will hang on run. 
// pure virtual method
// FrameIndex represent objects inside a abstract stack.
// We must replace FrameIndex with an stack/frame pointer
// direct reference.
bool WDCRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                          int SPAdj, unsigned FIOperandNum,
                                          RegScavenger *RS) const {
  return true;
}
//}

bool
WDCRegisterInfo::requiresRegisterScavenging(const MachineFunction &MF) const {
  return true;
}

bool
WDCRegisterInfo::trackLivenessAfterRegAlloc(const MachineFunction &MF) const {
  return true;
}

// pure virtual method
Register WDCRegisterInfo::
getFrameRegister(const MachineFunction &MF) const {
  const TargetFrameLowering *TFI = MF.getSubtarget().getFrameLowering();
  return TFI->hasFP(MF) ? (WDC::S) :
                          (WDC::S);
}