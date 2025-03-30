//===-- Cpu0AsmBackend.h - Cpu0 Asm Backend  ------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the WDCAsmBackend class.
//
//===----------------------------------------------------------------------===//
//

#ifndef LLVM_LIB_TARGET_WDC_MCTARGETDESC_WDCASMBACKEND_H
#define LLVM_LIB_TARGET_WDC_MCTARGETDESC_WDCASMBACKEND_H

namespace llvm {

class MCAsmBackend;
class Target;
class MCSubtargetInfo;
class MCRegisterInfo;
class MCTargetOptions;

// MCAsmBackend
MCAsmBackend * createWDCMCAsmBackend(const Target &T,
    const MCSubtargetInfo &STI,
    const MCRegisterInfo &MRI,
    const MCTargetOptions &Options);
}

#endif