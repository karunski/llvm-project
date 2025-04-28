//===-- WDCFixupKinds.h - WDC Specific Fixup Entries ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_WDC_MCTARGETDESC_WDCFIXUPKINDS_H
#define LLVM_LIB_TARGET_WDC_MCTARGETDESC_WDCFIXUPKINDS_H

#include <WDCConfig.h>

#include <llvm/MC/MCFixup.h>

namespace llvm {
namespace WDC {
  // Although most of the current fixup types reflect a unique relocation
  // one can have multiple fixup types for a given relocation and thus need
  // to be uniquely named.
  //
  // This table *must* be in the save order of
  // MCFixupKindInfo Infos[WDC::NumTargetFixupKinds]
  // in WDCAsmBackend.cpp.
  //@Fixups {
  enum Fixups {
    //@ Pure upper 24 bit fixup resulting in - R_WDC_LONG.
    fixup_WDC_long = FirstTargetFixupKind,

    // // Pure upper 16 bit fixup resulting in - R_WDC_HI16.
    fixup_Cpu0_HI16,

    // // Pure lower 16 bit fixup resulting in - R_CPU0_LO16.
    // fixup_Cpu0_LO16,

    // // 16 bit fixup for GP offest resulting in - R_CPU0_GPREL16.
    // fixup_Cpu0_GPREL16,

    // // GOT (Global Offset Table)
    // // Symbol fixup resulting in - R_CPU0_GOT16.
    // fixup_Cpu0_GOT,

    

    // // resulting in - R_CPU0_GOT_HI16
    // fixup_Cpu0_GOT_HI16,

    // // resulting in - R_CPU0_GOT_LO16
    // fixup_Cpu0_GOT_LO16,

    // Marker
    LastTargetFixupKind,
    NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
  };
  //@Fixups }
} // namespace Cpu0
} // namespace llvm

#endif // LLVM_CPU0_CPU0FIXUPKINDS_H