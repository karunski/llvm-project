//===-- WDCTargetInfo.cpp - WDC Target Implementation -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "WDC.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

Target llvm::TheWDCTarget;

extern "C" void LLVMInitializeWDCTargetInfo() {
  RegisterTarget<Triple::wdc, /*HasJIT=*/true> X{TheWDCTarget, "wdc", "WDC65816", "WDC"};
}