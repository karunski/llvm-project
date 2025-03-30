//===-- WDCInstrInfo.cpp - WDC Instruction Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the WDC implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "WDCInstructionInfo.h"

#include "WDCTargetMachine.h"
#include "WDCMachineFunction.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "WDCGenInstrInfo.inc"

// Pin the vtable to this file.
void WDCInstrInfo::anchor() {}

//@WDCInstrInfo {
WDCInstrInfo::WDCInstrInfo(const WDCSubtarget &STI) : Subtarget{STI} {}

const WDCInstrInfo *WDCInstrInfo::create(WDCSubtarget &STI) {
  return llvm::createWDCSEInstrInfo(STI);
}

//@GetInstSizeInBytes {
/// Return the number of bytes of code the specified instruction may be.
unsigned WDCInstrInfo::GetInstSizeInBytes(const MachineInstr &MI) const {
//@GetInstSizeInBytes - body
  // switch (MI.getOpcode()) {
  // default:
    return MI.getDesc().getSize();
  // }
}

void llvm::WDCInstrInfo::storeRegToStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, Register SrcReg,
    bool isKill, int FrameIndex, const TargetRegisterClass *RC,
    const TargetRegisterInfo *TRI, Register VReg,
    MachineInstr::MIFlag /*Flags*/) const {
  storeRegToStack(MBB, MI, SrcReg, isKill, FrameIndex, RC, TRI, 0);
}

void llvm::WDCInstrInfo::loadRegFromStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, Register DestReg,
    int FrameIndex, const TargetRegisterClass *RC,
    const TargetRegisterInfo *TRI, Register /*VReg*/,
    MachineInstr::MIFlag /*Flags*/) const {
  loadRegFromStack(MBB, MI, DestReg, FrameIndex, RC, TRI, 0);
}

MachineMemOperand *
llvm::WDCInstrInfo::GetMemOperand(MachineBasicBlock &basicBlock,
                                    int frameIndex,
                                    MachineMemOperand::Flags Flags) const {
  auto &machineFunction = *basicBlock.getParent();
  auto &frameInfo = machineFunction.getFrameInfo();

  return machineFunction.getMachineMemOperand(
      MachinePointerInfo::getFixedStack(machineFunction, frameIndex), Flags,
      frameInfo.getObjectSize(frameIndex),
      frameInfo.getObjectAlign(frameIndex));
}
