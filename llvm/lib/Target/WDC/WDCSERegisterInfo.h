//===-- WDCSERegisterInfo.h - WDC32 Register Information ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the WDC32/64 implementation of the TargetRegisterInfo
// class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_WDC_WDCSEREGISTERINFO_H
#define LLVM_LIB_TARGET_WDC_WDCSEREGISTERINFO_H

#include "WDCConfig.h"

#include "WDCRegisterInfo.h"

namespace llvm {
class WDCSEInstrInfo;

class WDCSERegisterInfo : public WDCRegisterInfo {
public:
  WDCSERegisterInfo(const WDCSubtarget &Subtarget);

  const TargetRegisterClass *intRegClass(unsigned Size) const override;
};

} // end namespace llvm

#endif

