//===-- WDCTargetMachine.cpp - Define TargetMachine for WDC 65816 -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implements the info about WDC target spec.
//
//===----------------------------------------------------------------------===//

#include "WDCTargetMachine.h"
#include "WDC.h"

#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

#define DEBUG_TYPE "wdc"

extern "C" void LLVMInitializeWDCTarget() {
}