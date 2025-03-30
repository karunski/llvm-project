//===-- WDCSEISelLowering.cpp - WDCSE DAG Lowering Interface --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Subclass of WDCTargetLowering specialized for WDC32.
//
//===----------------------------------------------------------------------===//
#include "WDCMachineFunction.h"
#include "WDCSEISelLowering.h"

#include "WDCRegisterInfo.h"
#include "WDCTargetMachine.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "WDC-isel"

static cl::opt<bool>
EnableWDCTailCalls("enable-wdc-tail-calls", cl::Hidden,
                    cl::desc("WDC: Enable tail calls."), cl::init(false));

//@WDCSETargetLowering {
WDCSETargetLowering::WDCSETargetLowering(const WDCTargetMachine &TM,
                                           const WDCSubtarget &STI)
    : WDCTargetLowering(TM, STI) {
//@WDCSETargetLowering body {
  // Set up the register classes
  addRegisterClass(MVT::i16, &WDC::AccumulatorRegisterClassRegClass);
  addRegisterClass(MVT::i16, &WDC::IndexRegsRegClass);
  // addRegisterClass(MVT::i16, &WDC::CPURegsRegClass);

// must, computeRegisterProperties - Once all of the register classes are 
//  added, this allows us to compute derived properties we expose.
  computeRegisterProperties(Subtarget.getRegisterInfo());
}

SDValue WDCSETargetLowering::LowerOperation(SDValue Op,
                                             SelectionDAG &DAG) const {

  return WDCTargetLowering::LowerOperation(Op, DAG);
}

const WDCTargetLowering *
llvm::createWDCSETargetLowering(const WDCTargetMachine &TM,
                                 const WDCSubtarget &STI) {
  return new WDCSETargetLowering(TM, STI);
}
