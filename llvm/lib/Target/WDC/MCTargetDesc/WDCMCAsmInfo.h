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

namespace llvm {
  class Triple;
  class MCRegisterInfo;
  class MCAsmInfo;
  class MCTargetOptions;
  
  MCAsmInfo *createWDCMCAsmInfo(const MCRegisterInfo &MRI,
    const Triple &TT,
    const MCTargetOptions &Options);
} // namespace llvm

#endif