//===-- WDCSEFrameLowering.h - WDC32/64 frame lowering --------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_WDC_WDCSEFRAMELOWERING_H
#define LLVM_LIB_TARGET_WDC_WDCSEFRAMELOWERING_H

#include "WDCConfig.h"

#include "WDCFrameLowering.h"

namespace llvm {

class WDCSEFrameLowering : public WDCFrameLowering {
public:
  explicit WDCSEFrameLowering(const WDCSubtarget &STI);

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
                            RegScavenger *RS) const override;
};

} // End llvm namespace

#endif