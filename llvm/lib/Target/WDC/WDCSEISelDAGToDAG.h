//===-- WDCSEISelDAGToDAG.h - A Dag to Dag Inst Selector for WDCSE -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Subclass of WDCDAGToDAGISel specialized for WDC32.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_WDC_WDCSEISELDAGTODAG_H
#define LLVM_LIB_TARGET_WDC_WDCSEISELDAGTODAG_H

#include "WDCConfig.h"

#include "WDCISelDAGToDAG.h"

namespace llvm {

class WDCSEDAGToDAGISel : public WDCDAGToDAGISel {

public:
  explicit WDCSEDAGToDAGISel(WDCTargetMachine &TM, CodeGenOptLevel OL)
      : WDCDAGToDAGISel(TM, OL) {}

private:

  bool runOnMachineFunction(MachineFunction &MF) override;

  bool trySelect(SDNode *Node) override;

  void processFunctionAfterISel(MachineFunction &MF) override;

  // Insert instructions to initialize the global base register in the
  // first MBB of the function.
//  void initGlobalBaseReg(MachineFunction &MF);

};

class WDCSEDAGToDAGISelLegacy : public WDCDAGToDAGISelLegacy
{
public:
  WDCSEDAGToDAGISelLegacy(WDCTargetMachine & TargetMachine, CodeGenOptLevel OptimizationLevel);
};

FunctionPass *createWDCSEISelDag(WDCTargetMachine &TM,
                                  CodeGenOptLevel OptLevel);

}

#endif