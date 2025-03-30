//===-- WDCISelDAGToDAG.cpp - A Dag to Dag Inst Selector for WDC --------===//
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

#include "WDCISelDAGToDAG.h"
#include "WDC.h"

#include "WDCMachineFunction.h"
#include "WDCRegisterInfo.h"
#include "WDCSEISelDAGToDAG.h"
#include "WDCTargetMachine.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
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

//===----------------------------------------------------------------------===//
// Instruction Selector Implementation
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// WDCDAGToDAGISel - WDC specific code to select WDC machine
// instructions for SelectionDAG operations.
//===----------------------------------------------------------------------===//

bool WDCDAGToDAGISel::runOnMachineFunction(MachineFunction &MF) {
  bool Ret = SelectionDAGISel::runOnMachineFunction(MF);

  return Ret;
}

//@SelectAddr {
/// ComplexPattern used on WDCInstrInfo
/// Used on WDC Load/Store instructions
// bool WDCDAGToDAGISel::
// SelectAddr(SDNode *Parent, SDValue Addr, SDValue &Base, SDValue &Offset) {
// //@SelectAddr }
//   EVT ValTy = Addr.getValueType();
//   SDLoc DL(Addr);

//   // If Parent is an unaligned f32 load or store, select a (base + index)
//   // floating point load/store instruction (luxc1 or suxc1).
//   const LSBaseSDNode* LS = 0;

//   if (Parent && (LS = dyn_cast<LSBaseSDNode>(Parent))) {
//     EVT VT = LS->getMemoryVT();

//     if (VT.getSizeInBits() / 8 > LS->getAlignment()) {
//       assert(0 && "Unaligned loads/stores not supported for this type.");
//       if (VT == MVT::f32)
//         return false;
//     }
//   }

//   // if Address is FI, get the TargetFrameIndex.
//   if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Addr)) {
//     Base   = CurDAG->getTargetFrameIndex(FIN->getIndex(), ValTy);
//     Offset = CurDAG->getTargetConstant(0, DL, ValTy);
//     return true;
//   }

//   Base   = Addr;
//   Offset = CurDAG->getTargetConstant(0, DL, ValTy);
//   return true;
// }

//@Select {
/// Select instructions not customized! Used for
/// expanded, promoted and normal instructions
void WDCDAGToDAGISel::Select(SDNode *Node) {
//@Select }
  unsigned Opcode = Node->getOpcode();

  // If we have a custom node, we already have selected!
  if (Node->isMachineOpcode()) {
    LLVM_DEBUG(errs() << "== "; Node->dump(CurDAG); errs() << "\n");
    Node->setNodeId(-1);
    return;
  }

  // See if subclasses can handle this node.
  if (trySelect(Node))
    return;

  switch(Opcode) {
  default: break;

  }

  // Select the default instruction
  SelectCode(Node);
}

llvm::WDCDAGToDAGISelLegacy::WDCDAGToDAGISelLegacy(std::unique_ptr<SelectionDAGISel> S)
: SelectionDAGISelLegacy{ID, std::move(S)}
{
}

char llvm::WDCDAGToDAGISelLegacy::ID = 0;

StringRef llvm::WDCDAGToDAGISelLegacy::getPassName() const {
  return "WDC DAG->DAG Pattern Instruction Selection";
}
