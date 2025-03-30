//===-- WDCInstrInfo.h - WDC Instruction Information ----------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_WDC_WDCINSTRINFO_H
#define LLVM_LIB_TARGET_WDC_WDCINSTRINFO_H

#include "WDCConfig.h"

#include "WDC.h"
#include "WDCRegisterInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "WDCGenInstrInfo.inc"

namespace llvm {

class WDCInstrInfo : public WDCGenInstrInfo {
  virtual void anchor();
protected:
  const WDCSubtarget &Subtarget;
public:
  explicit WDCInstrInfo(const WDCSubtarget &STI);

  static const WDCInstrInfo *create(WDCSubtarget &STI);

  /// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
  /// such, whenever a client has an instance of instruction info, it should
  /// always be able to get register info as well (through this method).
  ///
  virtual const WDCRegisterInfo &getRegisterInfo() const = 0;

  /// Return the number of bytes of code the specified instruction may be.
  unsigned GetInstSizeInBytes(const MachineInstr &MI) const;

protected:
};
const WDCInstrInfo *createWDCSEInstrInfo(const WDCSubtarget &STI);
}

#endif