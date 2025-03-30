//===-- WDCISEISelLowering.h - WDCISE DAG Lowering Interface ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Subclass of WDCITargetLowering specialized for WDC32/64.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_WDC_WDCSEISELLOWERING_H
#define LLVM_LIB_TARGET_WDC_WDCSEISELLOWERING_H

#include "WDCConfig.h"

#include "WDCISelLowering.h"
#include "WDCRegisterInfo.h"

namespace llvm {
  class WDCSETargetLowering : public WDCTargetLowering  {
  public:
    explicit WDCSETargetLowering(const WDCTargetMachine &TM,
                                  const WDCSubtarget &STI);

    SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;
  private:
  };
}

#endif // WDCISEISELLOWERING_H