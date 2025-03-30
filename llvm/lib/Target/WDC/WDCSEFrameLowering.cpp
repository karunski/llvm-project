//===-- WDCSEFrameLowering.cpp - WDC Frame Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the WDC implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "WDCSEFrameLowering.h"

#include "WDCMachineFunction.h"
#include "WDCSEInstructionInfo.h"
#include "WDCSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

WDCSEFrameLowering::WDCSEFrameLowering(const WDCSubtarget &STI)
    : WDCFrameLowering{STI, STI.stackAlignment()} {}

void WDCSEFrameLowering::emitPrologue(MachineFunction &machineFunction,
                                      MachineBasicBlock &basicBlock) const {
  auto & frameInfo    = machineFunction.getFrameInfo();
  // const auto * wdcFunctionInfo = machineFunction.getInfo<WDCFunctionInfo>();

  const auto &instructionInfo =
      *static_cast<const WDCSEInstrInfo *>(Subtarget.getInstrInfo());
  // const auto &RegInfo =
  //     *static_cast<const WDCRegisterInfo *>(Subtarget.getRegisterInfo());

  auto basicBlockIter = basicBlock.begin();
  const auto debugLoc = basicBlockIter != basicBlock.end() ? basicBlockIter->getDebugLoc() : DebugLoc{};
  // const auto ABI = Subtarget.getABI();
  // const TargetRegisterClass *RC = &WDC::GPROutRegClass;

  // First, compute final stack size.
  uint64_t StackSize = frameInfo.getStackSize();

  // No need to allocate space on the stack.
  if (StackSize == 0 && !frameInfo.adjustsStack()) return;

  // MachineModuleInfo &MMI = machineFunction.getMMI();
  // const MCRegisterInfo *MRI = MMI.getContext().getRegisterInfo();

  // Adjust stack.
  instructionInfo.adjustStackPtr(WDC::S, -StackSize, basicBlock, basicBlockIter);

  // emit ".cfi_def_cfa_offset StackSize"
  // unsigned CFIIndex = 
  //     machineFunction.addFrameInst(
  //     MCCFIInstruction::cfiDefCfaOffset(nullptr, StackSize));
  // BuildMI(basicBlock, basicBlockIter, debugLoc, instructionInfo.get(TargetOpcode::CFI_INSTRUCTION))
  //     .addCFIIndex(CFIIndex);

  // const auto &calleeSavedInfo = frameInfo.getCalleeSavedInfo();

  // if (!calleeSavedInfo.empty()) {
  //   // Find the instruction past the last instruction that saves a callee-saved
  //   // register to the stack.
  //   for (unsigned i = 0; i < calleeSavedInfo.size(); ++i) {
  //     ++basicBlockIter;
  //   }

  //   // Iterate over list of callee-saved registers and emit .cfi_offset
  //   // directives.
  //   for (std::vector<CalleeSavedInfo>::const_iterator I = calleeSavedInfo.begin(),
  //          E = calleeSavedInfo.end(); I != E; ++I) {
  //     const auto Offset = frameInfo.getObjectOffset(I->getFrameIdx());
  //     const auto Reg = I->getReg();
  //     {
  //       // Reg is in CPURegs.
  //       unsigned CFIIndex = machineFunction.addFrameInst(MCCFIInstruction::createOffset(
  //           nullptr, RegInfo.getDwarfRegNum(Reg, true), Offset));
  //       BuildMI(basicBlock, basicBlockIter, debugLoc, instructionInfo.get(TargetOpcode::CFI_INSTRUCTION))
  //           .addCFIIndex(CFIIndex);
  //     }
  //   }
  // }
}

//@emitEpilogue {
void WDCSEFrameLowering::emitEpilogue(MachineFunction &MF,
                                 MachineBasicBlock &MBB) const {
}
//}

const WDCFrameLowering *
llvm::createWDCSEFrameLowering(const WDCSubtarget &ST) {
  return new WDCSEFrameLowering(ST);
}