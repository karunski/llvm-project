//===-- WDC.h - Top-level interface for WDC representation ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM WDC back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_WDC_WDC_H
#define LLVM_LIB_TARGET_WDC_WDC_H

#include "WDCConfig.h"
#include "MCTargetDesc/WDCMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class WDCTargetMachine;
  class FunctionPass;

} // end namespace llvm;

#endif
