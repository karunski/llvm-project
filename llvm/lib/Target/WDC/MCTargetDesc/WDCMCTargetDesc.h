//===-- WDCMCTargetDesc.h - WDC Target Descriptions -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides WDC specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_WDC_MCTARGETDESC_WDCMCTARGETDESC_H
#define LLVM_LIB_TARGET_WDC_MCTARGETDESC_WDCMCTARGETDESC_H

#include "WDCConfig.h"
#include "llvm/Support/DataTypes.h"

#include <memory>

namespace llvm {
class Target;
class Triple;

extern Target TheWDCTarget;

} // End llvm namespace

// Defines symbolic names for WDC registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "WDCGenRegisterInfo.inc"

// Defines symbolic names for the WDC instructions.
#define GET_INSTRINFO_ENUM
#include "WDCGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "WDCGenSubtargetInfo.inc"

#endif