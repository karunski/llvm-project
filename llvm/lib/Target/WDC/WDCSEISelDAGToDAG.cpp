//===-- WDCSEISelDAGToDAG.cpp - A Dag to Dag Inst Selector for WDCSE ----===//
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

#include "WDCSEISelDAGToDAG.h"

#include "MCTargetDesc/WDCBaseInfo.h"
#include "WDC.h"
#include "WDCMachineFunction.h"
#include "WDCRegisterInfo.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
using namespace llvm;

#define DEBUG_TYPE "WDC-isel"

bool WDCSEDAGToDAGISel::runOnMachineFunction(MachineFunction &MF) {
  Subtarget = &static_cast<const WDCSubtarget &>(MF.getSubtarget());
  return WDCDAGToDAGISel::runOnMachineFunction(MF);
}

void WDCSEDAGToDAGISel::processFunctionAfterISel(MachineFunction &MF) {
}

//@selectNode
bool WDCSEDAGToDAGISel::trySelect(SDNode *Node) {
  unsigned Opcode = Node->getOpcode();
  SDLoc DL(Node);

  ///
  // Instruction Selection not handled by the auto-generated
  // tablegen selection should be handled here.
  ///

  ///
  // Instruction Selection not handled by the auto-generated
  // tablegen selection should be handled here.
  ///
//   EVT NodeTy = Node->getValueType(0);
//   unsigned MultOpc;

  switch(Opcode) {
  default: break;

  }

  return false;
}

llvm::WDCSEDAGToDAGISelLegacy::WDCSEDAGToDAGISelLegacy(
    WDCTargetMachine &TargetMachine, CodeGenOptLevel OptimizationLevel)
    : WDCDAGToDAGISelLegacy{std::make_unique<WDCSEDAGToDAGISel>(
          TargetMachine, OptimizationLevel)} {}

FunctionPass *llvm::createWDCSEISelDag(WDCTargetMachine &TM,
                                       CodeGenOptLevel OptLevel) {
  return new WDCSEDAGToDAGISelLegacy{TM, OptLevel};
}
