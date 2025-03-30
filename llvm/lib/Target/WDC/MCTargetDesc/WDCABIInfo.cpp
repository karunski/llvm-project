//===---- WDCABIInfo.cpp - Information about WDC ABI's ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "WDCConfig.h"

#include "WDCABIInfo.h"
#include "WDCRegisterInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

static cl::opt<bool>
EnableWDCS32Calls("wdc-s32-calls", cl::Hidden,
                    cl::desc("WDC S32 call: use stack only to pass arguments."), cl::init(false));

namespace {
// static const MCPhysReg O32IntRegs[4] = {WDC::A0, WDC::A1};
//static const MCPhysReg S32IntRegs[] = {};
}

const ArrayRef<MCPhysReg> WDCABIInfo::GetByValArgRegs() const {
//   if (IsO32())
//     return makeArrayRef(O32IntRegs);
  if (IsS32())
    return {};
  llvm_unreachable("Unhandled ABI");
}

const ArrayRef<MCPhysReg> WDCABIInfo::GetVarArgRegs() const {
//   if (IsO32())
//     return makeArrayRef(O32IntRegs);
  if (IsS32())
    return {};
  llvm_unreachable("Unhandled ABI");
}

unsigned WDCABIInfo::GetCalleeAllocdArgSizeInBytes(CallingConv::ID CC) const {
//   if (IsO32())
//     return CC != 0;
  if (IsS32())
    return 0;
  llvm_unreachable("Unhandled ABI");
}

WDCABIInfo WDCABIInfo::computeTargetABI() {
  WDCABIInfo abi(ABI::Unknown);

  //if (EnableWDCS32Calls)
    abi = ABI::S32;
//   else
//     abi = ABI::O32;
//   // Assert exactly one ABI was chosen.
  assert(abi.ThisABI != ABI::Unknown);

  return abi;
}

unsigned WDCABIInfo::GetStackPtr() const {
  return WDC::S;
}

// unsigned WDCABIInfo::GetFramePtr() const {
//   return WDC::FP;
// }

// unsigned WDCABIInfo::GetNullPtr() const {
//   return WDC::ZERO;
// }

unsigned WDCABIInfo::GetEhDataReg(unsigned I) const {
  static const unsigned EhDataReg[] = {
    WDC::A, WDC::X
  };

  return EhDataReg[I];
}

int WDCABIInfo::EhDataRegSize() const {
  if (ThisABI == ABI::S32)
    return 0;
  else
    return 2;
}