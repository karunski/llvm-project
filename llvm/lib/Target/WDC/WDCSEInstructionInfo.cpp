//===-- WDCSEInstrInfo.cpp - WDC Instruction Information -----------===//
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

#include "WDCSEInstructionInfo.h"

#include "WDCMachineFunction.h"
#include "WDCTargetMachine.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

WDCSEInstrInfo::WDCSEInstrInfo(const WDCSubtarget &STI)
    : WDCInstrInfo(STI), RI(STI) {}

const WDCRegisterInfo &WDCSEInstrInfo::getRegisterInfo() const {
  return RI;
}

bool llvm::WDCSEInstrInfo::expandPostRAPseudo(MachineInstr &MI) const {
MachineBasicBlock &MBB = *MI.getParent();

  switch (MI.getDesc().getOpcode()) {
  default:
    return false;
  case WDC::RetRTL:
    expandRTL(MBB, MI);
    break;
  }

  MBB.erase(MI);
  return true;
}

void llvm::WDCSEInstrInfo::adjustStackPtr(unsigned SP, int64_t amount,
                                          MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator I) const {
  const auto debugLoc = I != MBB.end() ? I->getDebugLoc() : DebugLoc();
  // unsigned ADDu = Cpu0::ADDu;
  // unsigned ADDiu = Cpu0::ADDiu;

  assert(isInt<16>(amount) && "stack adjustment amount was too great");
  if (amount < 0) {
    amount = -amount;
    BuildMI(MBB, I, debugLoc, get(WDC::SEC));
    BuildMI(MBB, I, debugLoc, get(WDC::TSC));
    BuildMI(MBB, I, debugLoc, get(WDC::SBCi), WDC::A).addReg(WDC::A).addImm(amount);
    BuildMI(MBB, I, debugLoc, get(WDC::TCS));
  }
  else
  {
    BuildMI(MBB, I, debugLoc, get(WDC::CLC));
    BuildMI(MBB, I, debugLoc, get(WDC::TSC));
    BuildMI(MBB, I, debugLoc, get(WDC::ADCi), WDC::A).addReg(WDC::A).addImm(amount);
    BuildMI(MBB, I, debugLoc, get(WDC::TCS));
  }
  // else { // Expand immediate that doesn't fit in 16-bit.
  //   unsigned Reg = loadImmediate(Amount, MBB, I, DL, nullptr);
  //   BuildMI(MBB, I, DL, get(ADDu), SP).addReg(SP).addReg(Reg, RegState::Kill);
  // }
}

void llvm::WDCSEInstrInfo::expandRTL(MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator I) const {
  BuildMI(MBB, I, I->getDebugLoc(), get(WDC::RTL));
}

const WDCInstrInfo *llvm::createWDCSEInstrInfo(const WDCSubtarget &STI) {
  return new WDCSEInstrInfo(STI);
}
