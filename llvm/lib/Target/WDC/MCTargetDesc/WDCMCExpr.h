//===-- WDCMCExpr.h - WDC specific MC expression classes ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_WDC_MCTARGETDESC_WDCMCEXPR_H
#define LLVM_LIB_TARGET_WDC_MCTARGETDESC_WDCMCEXPR_H

#include <llvm/MC/MCExpr.h>

namespace llvm {
class WDCMCExpr : public MCTargetExpr {
};
}
#endif
