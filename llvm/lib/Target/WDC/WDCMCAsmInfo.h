//===-- WDCMCAsmInfo.h - WDC Asm Info ------------------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the WDCMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_WDC_MCTARGETDESC_WDCMCASMINFO_H
#define LLVM_LIB_TARGET_WDC_MCTARGETDESC_WDCMCASMINFO_H

#include "WDCConfig.h"

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
  class Triple;

  class WDCMCAsmInfo : public MCAsmInfoELF {
    void anchor() override;
  public:
    explicit WDCMCAsmInfo(const Triple &TheTriple);
  };

} // namespace llvm

#endif