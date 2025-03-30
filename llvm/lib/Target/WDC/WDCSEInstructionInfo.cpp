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

const WDCInstrInfo *llvm::createWDCSEInstrInfo(const WDCSubtarget &STI) {
  return new WDCSEInstrInfo(STI);
}

