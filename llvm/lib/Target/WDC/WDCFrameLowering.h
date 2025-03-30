//===-- WDCFrameLowering.h - Define frame lowering for WDC ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_WDC_WDCFRAMELOWERING_H
#define LLVM_LIB_TARGET_WDC_WDCFRAMELOWERING_H

#include "WDCConfig.h"

#include "WDC.h"
#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {
  class WDCSubtarget;

class WDCFrameLowering : public TargetFrameLowering {
protected:
  const WDCSubtarget & Subtarget;

public:
  explicit WDCFrameLowering(const WDCSubtarget &sti, unsigned Alignment)
      : TargetFrameLowering(StackGrowsDown, Align(Alignment), 0,
                            Align(Alignment)),
        Subtarget{sti} {}

  static const WDCFrameLowering *create(const WDCSubtarget &ST);

  bool hasFPImpl(const MachineFunction &MF) const override;

};

/// Create WDCFrameLowering objects.
const WDCFrameLowering *createWDCSEFrameLowering(const WDCSubtarget &ST);

} // End llvm namespace

#endif