//===---- WDCABIInfo.h - Information about WDC ABI's --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_WDC_MCTARGETDESC_WDCABIINFO_H
#define LLVM_LIB_TARGET_WDC_MCTARGETDESC_WDCABIINFO_H

#include "WDCConfig.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/TargetParser/Triple.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/MC/MCRegisterInfo.h"

namespace llvm {

class MCTargetOptions;
class StringRef;
class TargetRegisterClass;

class WDCABIInfo {
public:
  enum class ABI { Unknown, O32, S32 };

protected:
  ABI ThisABI;

public:
  WDCABIInfo(ABI ThisABI) : ThisABI(ThisABI) {}

  static WDCABIInfo Unknown() { return WDCABIInfo(ABI::Unknown); }
  static WDCABIInfo O32() { return WDCABIInfo(ABI::O32); }
  static WDCABIInfo S32() { return WDCABIInfo(ABI::S32); }
  static WDCABIInfo computeTargetABI();

  bool IsKnown() const { return ThisABI != ABI::Unknown; }
  bool IsO32() const { return ThisABI == ABI::O32; }
  bool IsS32() const { return ThisABI == ABI::S32; }
  ABI GetEnumValue() const { return ThisABI; }

  /// The registers to use for byval arguments.
  const ArrayRef<MCPhysReg> GetByValArgRegs() const;

  /// The registers to use for the variable argument list.
  const ArrayRef<MCPhysReg> GetVarArgRegs() const;

  /// Obtain the size of the area allocated by the callee for arguments.
  /// CallingConv::FastCall affects the value for O32.
  unsigned GetCalleeAllocdArgSizeInBytes(CallingConv::ID CC) const;

  /// Ordering of ABI's
  /// WDCGenSubtargetInfo.inc will use this to resolve conflicts when given
  /// multiple ABI options.
  bool operator<(const WDCABIInfo Other) const {
    return ThisABI < Other.GetEnumValue();
  }

  unsigned GetStackPtr() const;
  //unsigned GetFramePtr() const;
  // unsigned GetNullPtr() const;

  unsigned GetEhDataReg(unsigned I) const;
  int EhDataRegSize() const;
};
}

#endif