//===-- WDCSubtarget.cpp - WDC Subtarget Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the WDC specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "WDCSubtarget.h"

#include "WDCMachineFunction.h"
#include "WDC.h"
#include "WDCRegisterInfo.h"
#include "WDCInstructionInfo.h"
#include "WDCTargetMachine.h"
#include "WDCFrameLowering.h"
#include "WDCISelLowering.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "WDC-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "WDCGenSubtargetInfo.inc"

extern bool FixGlobalBaseReg;

void WDCSubtarget::anchor() { }

//@1 {
WDCSubtarget::WDCSubtarget(const Triple &TT, StringRef CPU,
                             StringRef FS, 
                             const WDCTargetMachine &_TM) :
//@1 }
  // WDCGenSubtargetInfo will display features by llc -march=wdc -mcpu=help
  WDCGenSubtargetInfo(TT, CPU, /*TuneCPU*/ CPU, FS),
  TM(_TM), TargetTriple(TT), TSInfo(),
      InstrInfo(
          WDCInstrInfo::create(initializeSubtargetDependencies(CPU, FS, TM))),
      FrameLowering(WDCFrameLowering::create(*this)),
      TLInfo(WDCTargetLowering::create(TM, *this)) {

}

llvm::WDCSubtarget::~WDCSubtarget() = default;

bool WDCSubtarget::isPositionIndependent() const {
  return TM.isPositionIndependent();
}

WDCSubtarget &
WDCSubtarget::initializeSubtargetDependencies(StringRef CPU, StringRef FS,
                                               const TargetMachine &TM) {
  if (TargetTriple.getArch() == Triple::wdc) {
    if (CPU == "help") {
      CPU = "";
      return *this;
    } else {
      CPU = "65c816";
    }
  } else {
    errs() << "!!!Error, TargetTriple.getArch() = " << TargetTriple.getArch()
           << "CPU = " << CPU << "\n";
    exit(0);
  }

  if (CPU == "65c816") {
    WDCArchVersion = wdc65c816;
  }

//   if (isCpu032I()) {
//     HasCmp = true;
//     HasSlt = false;
//   }
//   else if (isCpu032II()) {
//     HasCmp = false;
//     HasSlt = true;
//   }
//   else {
//     errs() << "-mcpu must be empty(default:cpu032II), cpu032I or cpu032II" << "\n";
//   }

  // Parse features string.
  ParseSubtargetFeatures(CPU, /*TuneCPU*/ CPU, FS);
  // Initialize scheduling itinerary for the specified CPU.
  InstrItins = getInstrItineraryForCPU(CPU);

  return *this;
}

bool WDCSubtarget::abiUsesSoftFloat() const {
  return true;
}

const WDCABIInfo &WDCSubtarget::getABI() const { return TM.getABI(); }

const TargetInstrInfo *WDCSubtarget::getInstrInfo() const {
  return InstrInfo.get();
}

const TargetFrameLowering *WDCSubtarget::getFrameLowering() const {
  return FrameLowering.get();
}

const TargetRegisterInfo *WDCSubtarget::getRegisterInfo() const {
  return &InstrInfo->getRegisterInfo();
}

const TargetLowering *WDCSubtarget::getTargetLowering() const {
  return TLInfo.get();
}

const InstrItineraryData *WDCSubtarget::getInstrItineraryData() const {
  return &InstrItins;
}