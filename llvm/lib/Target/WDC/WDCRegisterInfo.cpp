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
  auto &machineInstruction = *II;
  auto &machineFunction = *machineInstruction.getParent()->getParent();
  // auto &machineFrameInfo = machineFunction.getFrameInfo();
  // auto *wdcFunctionInfo = machineFunction.getInfo<WDCFunctionInfo>();

  unsigned i = 0;
  while (!machineInstruction.getOperand(i).isFI()) {
    ++i;
    assert(i < machineInstruction.getNumOperands() && "Instr doesn't have FrameIndex operand!");
  }

  LLVM_DEBUG(errs() << "\nFunction : " << machineFunction.getFunction().getName() << "\n";
             errs() << "<--------->\n"
                    << machineInstruction);

  const auto frameIndex = machineInstruction.getOperand(i).getIndex();
  const auto stackSize = machineFunction.getFrameInfo().getStackSize();
  const auto stackPointerOffset = machineFunction.getFrameInfo().getObjectOffset(frameIndex);

  LLVM_DEBUG(errs() << "frameIndex : " << frameIndex << "\n"
                    << "stackPointerOffset   : " << stackPointerOffset << "\n"
                    << "stackSize  : " << stackSize << "\n");

  // const auto &calleeSavedInfo = machineFrameInfo.getCalleeSavedInfo();
  // int MinCalleeSavedFrameIdx = 0;
  // int MaxCalleeSavedFrameIdx = -1;

  // if (!calleeSavedInfo.empty()) {
  //   MinCalleeSavedFrameIdx = calleeSavedInfo.front().getFrameIdx();
  //   MaxCalleeSavedFrameIdx = calleeSavedInfo.back().getFrameIdx();
  // }

  // The following stack frame objects are always referenced relative to $sp:
  //  1. Outgoing arguments.
  //  2. Pointer to dynamically allocated stack space.
  //  3. Locations for callee-saved registers.
  // Everything else is referenced relative to whatever register
  // getFrameRegister() returns.
  const auto frameRegister = WDC::S;

  // Calculate final offset.
  // - There is no need to change the offset if the frame object is one of the
  //   following: an outgoing argument, pointer to a dynamically allocated
  //   stack space or a $gp restore location,
  // - If the frame object is any of the following, its offset must be
  // adjusted
  //   by adding the size of the stack:
  //   incoming argument, callee-saved register location or local variable.
  const auto Offset = stackPointerOffset + stackSize +
                      machineInstruction.getOperand(i + 1).getImm();

  LLVM_DEBUG(errs() << "Offset     : " << Offset << "\n" << "<--------->\n");

  // If MI is not a debug value, make sure Offset fits in the 16-bit immediate
  // field.
  if (!machineInstruction.isDebugValue() && !isInt<8>(Offset)) {
    errs() << "!!!ERROR!!! Not support large frame over 8-bit at this point.\n"
           << "Though CH3_5 support it."
           << "Reference: "
              "http://jonathan2251.github.io/lbd/"
              "backendstructure.html#large-stack\n"
           << "However the CH9_3, dynamic-stack-allocation-support bring "
              "instruction "
              "move $fp, $sp that make it complicated in coding against the "
              "tutoral "
              "purpose of Cpu0.\n"
           << "Reference: "
              "http://jonathan2251.github.io/lbd/"
              "funccall.html#dynamic-stack-allocation-support\n";
    assert(0 && "(!MI.isDebugValue() && !isInt<8>(Offset))");
  }

  machineInstruction.getOperand(i).ChangeToRegister(frameRegister, false);
  machineInstruction.getOperand(i + 1).ChangeToImmediate(Offset);
  return false;
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