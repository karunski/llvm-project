//===-- llvm/Target/WDCTargetObjectFile.h - WDC Object Info ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_WDC_WDCTARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_WDC_WDCTARGETOBJECTFILE_H

#include "WDCConfig.h"

#include "WDCTargetMachine.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {
class WDCTargetMachine;
class WDCTargetObjectFile : public TargetLoweringObjectFileELF {
  MCSection *SmallDataSection;
  MCSection *SmallBSSSection;
  const WDCTargetMachine *TM;

public:
  void Initialize(MCContext &Ctx, const TargetMachine &TM) override;
};
} // end namespace llvm

#endif
