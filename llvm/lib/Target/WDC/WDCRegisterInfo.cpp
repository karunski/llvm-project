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
// returns values generated from CalleeSavedRegsWDC in WDCCallConv.td
const MCPhysReg *
WDCRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CalleeSavedRegsWDC_SaveList;
}

const uint32_t *
WDCRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID) const {
  return CalleeSavedRegsWDC_RegMask; 
}

// pure virtual method
//@getReservedRegs {
BitVector WDCRegisterInfo::
getReservedRegs(const MachineFunction &MF) const {
//@getReservedRegs body {
  static const uint16_t ReservedCPURegs[] = {
    //WDC::ZERO, WDC::AT, WDC::SP, WDC::LR, /*WDC::SW, */WDC::PC
    /*WDC::P,*/ WDC::PC, WDC::S
  };
  BitVector Reserved(getNumRegs());

  for (unsigned I = 0; I < std::size(ReservedCPURegs); ++I)
    Reserved.set(ReservedCPURegs[I]);

  return Reserved;
}

//@eliminateFrameIndex {
//- If no eliminateFrameIndex(), it will hang on run. 
// pure virtual method

//  For 5 local i16 vars + 1 return i16:
//  SP+11+3 FrameIndex -1 ObjectOffset 0  |
//  SP+13    return addr (3 bytes)
//  SP+11    direct page storage
//  SP+9   FrameIndex 0  ObjectOffset  -2 |
//  SP+7   FrameIndex 1  ObjectOffset  -4 |- local vars 2 * 5 = 10 bytes
//  SP+5   FrameIndex 2  ObjectOffset  -6 |
//  SP+3   FrameIndex 3  ObjectOffset  -8 |
//  SP+1   FrameIndex 4  ObjectOffset -10 |
//  SP
// Actual offset = 10 bytes (frame size) + ObjectOffset + 1 + (overhead for return val etc)

// FrameIndex represent objects inside a abstract stack.
// We must replace FrameIndex with an stack/frame pointer
// direct reference.
bool WDCRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                          int SPAdj, unsigned FIOperandNum,
                                          RegScavenger *RS) const {
  auto &machineInstruction = *II;
  auto &machineFunction = *machineInstruction.getParent()->getParent();
  //auto &machineFrameInfo = machineFunction.getFrameInfo();
  // auto *wdcFunctionInfo = machineFunction.getInfo<WDCFunctionInfo>();

  unsigned i = 0;
  while (!machineInstruction.getOperand(i).isFI()) {
    ++i;
    assert(i < machineInstruction.getNumOperands() && "Instr doesn't have FrameIndex operand!");
  }

  LLVM_DEBUG(errs() << "\nFunction : " << machineFunction.getFunction().getName() << "\n";
             errs() << "<--------->\n"
                    << machineInstruction);

  const auto frameIndexOp = machineInstruction.getOperand(i);
  const auto opcode = machineInstruction.getOpcode();
  auto mmo_offset = 0;
  if (opcode == WDC::STAsr && machineInstruction.hasOneMemOperand()) {
    const auto mmo_iter = machineInstruction.memoperands_begin();
    const auto & mmo = *mmo_iter;
    mmo_offset = mmo->getOffset();
  }

  const auto frameIndex = frameIndexOp.getIndex();
  const auto stackSize = machineFunction.getFrameInfo().getStackSize();
  static const auto FrameReservedOverhead = 5; /* return address is 3 bytes, saved DP is 2 bytes */
  const auto stackPointerOffset = machineFunction.getFrameInfo().getObjectOffset(frameIndex)
    +mmo_offset + (frameIndex < 0 ? FrameReservedOverhead : 0)  + 1 /* SP is always one below actual stack top */;

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

  // Calculate final offset.
  // - There is no need to change the offset if the frame object is one of the
  //   following: an outgoing argument, pointer to a dynamically allocated
  //   stack space or a $gp restore location,
  // - If the frame object is any of the following, its offset must be
  // adjusted
  //   by adding the size of the stack:
  //   incoming argument, callee-saved register location or local variable.

  // this is the scheme I cooked up when inventing ADDsr.  Just one operand, the frame index, 
  // is turned into the Offset.  The stack register is implied.
  const auto offset = stackPointerOffset + stackSize;
  LLVM_DEBUG(errs() << "Offset " << offset << " = stackPointerOffset " << stackPointerOffset << " + stackSize " << stackSize << "\n");
  machineInstruction.getOperand(i).ChangeToImmediate(offset);

  if (!machineInstruction.isDebugValue() && !isInt<8>(offset)) {
    errs() << "stack offset dosn't fit in 8 bits";
    assert(0 && "(!MI.isDebugValue() && !isInt<8>(Offset))");
  }
  LLVM_DEBUG(errs() << "<--------->\n");

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

const TargetRegisterClass *
WDCRegisterInfo::intRegClass(unsigned Size) const {
  return &WDC::CPURegsRegClass;
}

const TargetRegisterClass *llvm::WDCRegisterInfo::getLargestLegalSuperClass(
    const TargetRegisterClass *RC, const MachineFunction &) const {
  if (RC == &WDC::RegsA16RegClass) {
    return &WDC::RegsGP16RegClass;
  }
  return RC;
}
