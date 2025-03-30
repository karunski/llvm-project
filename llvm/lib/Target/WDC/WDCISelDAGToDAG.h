//===---- WDCISelDAGToDAG.h - A Dag to Dag Inst Selector for WDC --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the WDC target.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_WDC_WDCISELDAGTODAG_H
#define LLVM_LIB_TARGET_WDC_WDCISELDAGTODAG_H

#include "llvm/Support/CodeGen.h"

//===----------------------------------------------------------------------===//
// Instruction Selector Implementation
//===----------------------------------------------------------------------===//


namespace llvm {

class WDCTargetMachine;
class FunctionPass;

FunctionPass *createWDCISelDag(WDCTargetMachine &TM,
  CodeGenOptLevel OptLevel);
}

#endif