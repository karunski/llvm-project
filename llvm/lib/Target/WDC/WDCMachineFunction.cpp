//===-- WDCMachineFunctionInfo.cpp - Private data used for WDC ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "WDCMachineFunction.h"

#include "MCTargetDesc/WDCBaseInfo.h"
#include "WDCInstructionInfo.h"
#include "WDCSubtarget.h"
#include "llvm/IR/Function.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"

using namespace llvm;

bool FixGlobalBaseReg;

WDCFunctionInfo::~WDCFunctionInfo() = default;

WDCFunctionInfo::WDCFunctionInfo(MachineFunction &machineFunc)
    : MF{machineFunc} {}

void WDCFunctionInfo::createEhDataRegsFI() {
  const TargetRegisterInfo &TRI = *MF.getSubtarget().getRegisterInfo();
  for (int I = 0; I < 2; ++I) {
    const TargetRegisterClass &RC = WDC::CPURegsRegClass;

    EhDataRegFI[I] = MF.getFrameInfo().CreateStackObject(
        TRI.getSpillSize(RC), TRI.getSpillAlign(RC), false);
  }
}


