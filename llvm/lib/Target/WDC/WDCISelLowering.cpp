//===-- WDCISelLowering.cpp - WDC DAG Lowering Implementation -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that WDC uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//
#include "WDCISelLowering.h"

#include "WDCMachineFunction.h"
#include "WDCTargetMachine.h"
#include "WDCTargetObjectFile.h"
#include "WDCSubtarget.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "WDC-lower"

//@3_1 1 {
const char *WDCTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  case WDCISD::JmpLink:           return "WDCISD::JmpLink";
  case WDCISD::TailCall:          return "WDCISD::TailCall";
  case WDCISD::Hi:                return "WDCISD::Hi";
  case WDCISD::Lo:                return "WDCISD::Lo";
  case WDCISD::GPRel:             return "WDCISD::GPRel";
  case WDCISD::Ret:               return "WDCISD::Ret";
  case WDCISD::EH_RETURN:         return "WDCISD::EH_RETURN";
  case WDCISD::DivRem:            return "WDCISD::DivRem";
  case WDCISD::DivRemU:           return "WDCISD::DivRemU";
  case WDCISD::Wrapper:           return "WDCISD::Wrapper";
  default:                         return NULL;
  }
}
//@3_1 1 }

//@WDCTargetLowering {
WDCTargetLowering::WDCTargetLowering(const WDCTargetMachine &TM,
                                     const WDCSubtarget &STI)
    : TargetLowering{TM}, Subtarget{STI}, ABI{TM.getABI()} {}

const WDCTargetLowering *WDCTargetLowering::create(const WDCTargetMachine &TM,
                                                     const WDCSubtarget &STI) {
  return llvm::createWDCSETargetLowering(TM, STI);
}

//===----------------------------------------------------------------------===//
//  Lower helper functions
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Misc Lower Operation implementation
//===----------------------------------------------------------------------===//

#include "WDCGenCallingConv.inc"

//===----------------------------------------------------------------------===//
//@            Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

//@LowerFormalArguments {
/// LowerFormalArguments - transform physical registers into virtual registers
/// and generate load operations for arguments places on the stack.
SDValue
WDCTargetLowering::LowerFormalArguments(SDValue Chain,
                                         CallingConv::ID CallConv,
                                         bool IsVarArg,
                                         const SmallVectorImpl<ISD::InputArg> &Ins,
                                         const SDLoc &DL, SelectionDAG &DAG,
                                         SmallVectorImpl<SDValue> &InVals)
                                          const {

  return Chain;
}
// @LowerFormalArguments }

//===----------------------------------------------------------------------===//
//@              Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

SDValue
WDCTargetLowering::LowerReturn(SDValue Chain,
                                CallingConv::ID CallConv, bool IsVarArg,
                                const SmallVectorImpl<ISD::OutputArg> &Outs,
                                const SmallVectorImpl<SDValue> &OutVals,
                                const SDLoc &DL, SelectionDAG &DAG) const {
  return DAG.getNode(WDCISD::Ret, DL, MVT::Other,
                     Chain, DAG.getRegister(WDC::A, MVT::i16));
}
