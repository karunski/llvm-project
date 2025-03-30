//===--- WDC.h - Declare WDC target feature support -------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares WDC TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_BASIC_TARGETS_WDC_H
#define LLVM_CLANG_LIB_BASIC_TARGETS_WDC_H

#include "clang/Basic/TargetInfo.h"

#include <memory>

namespace llvm {
class Triple;
}

namespace clang {

class TargetOptions;

namespace targets {

std::unique_ptr<TargetInfo> createWDCTargetInfo(const llvm::Triple &Triple, const TargetOptions &);

}
}

#endif