//===-- WDCSEInstrInfo.h - WDC Instruction Information ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the WDC implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_WDC_WDCSEINSTRINFO_H
#define LLVM_LIB_TARGET_WDC_WDCSEINSTRINFO_H

#include "WDCConfig.h"

#include "WDCInstructionInfo.h"
#include "WDCSERegisterInfo.h"
#include "WDCMachineFunction.h"

namespace llvm {

class WDCSEInstrInfo final : public WDCInstrInfo {
  const WDCSERegisterInfo RI;

public:
  explicit WDCSEInstrInfo(const WDCSubtarget &STI);

  const WDCRegisterInfo &getRegisterInfo() const override;
  bool expandPostRAPseudo(MachineInstr &MI) const override;

  /// Adjust SP by Amount bytes.
  void adjustStackPtr(unsigned SP, int64_t Amount, MachineBasicBlock &MBB,
                      MachineBasicBlock::iterator I) const override;

  void storeRegToStack(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator MI, Register SrcReg,
                               bool isKill, int FrameIndex,
                               const TargetRegisterClass *RC,
                               const TargetRegisterInfo *TRI,
                               int64_t Offset) const override;

  void loadRegFromStack(MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator MI,
                                Register DestReg, int FrameIndex,
                                const TargetRegisterClass *RC,
                                const TargetRegisterInfo *TRI,
                                int64_t Offset) const override;

private:
  void expandRTL(MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const;
};
}

#endif

