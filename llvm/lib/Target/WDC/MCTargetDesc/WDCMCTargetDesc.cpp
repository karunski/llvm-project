//===-- WDCMCTargetDesc.cpp - WDC Target Descriptions -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides WDC specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "WDCMCTargetDesc.h"
#include "WDCMCAsmInfo.h"
#include "InstPrinter/WDCInstPrinter.h"
#include "llvm/MC/MachineLocation.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCInstrAnalysis.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#include "WDCGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "WDCGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "WDCGenRegisterInfo.inc"

namespace
{

MCAsmInfo *createWDCMCAsmInfo(const MCRegisterInfo &MRI,
                                      const Triple &TT,
                                      const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new WDCMCAsmInfo{TT};

  unsigned SP = MRI.getDwarfRegNum(WDC::S, true);
  MCCFIInstruction Inst = MCCFIInstruction::createDefCfaRegister(nullptr, SP);
  MAI->addInitialFrameState(Inst);

  return MAI;
}

MCInstrInfo *createWDCMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo{};
  InitWDCMCInstrInfo(X); // defined in WDCGenInstrInfo.inc
  return X;
}

MCRegisterInfo *createWDCMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitWDCMCRegisterInfo(X, WDC::P); // defined in WDCGenRegisterInfo.inc
  return X;
}

MCSubtargetInfo *createWDCMCSubtargetInfo(const Triple &TT,
                                                  StringRef CPU, StringRef features) {

  /// Select the WDC Architecture Feature for the given triple and cpu name.
  /// The function will be called at command 'llvm-objdump -d' for WDC elf
  /// input.
  static const auto selectWDCArchFeature = [](const Triple &/*TT*/, StringRef /*CPU*/) {
    std::string WDCArchFeature;
    // if (CPU.empty() || CPU == "generic") {
    //   if (TT.getArch() == Triple::wdc) {
    //     if (CPU.empty() || CPU == "WDC32II") {
    //       WDCArchFeature = "+WDC32II";
    //     } else {
    //       if (CPU == "WDC32I") {
    //         WDCArchFeature = "+WDC32I";
    //       }
    //     }
    //   }
    // }
    return WDCArchFeature;
  };

  auto allFeatures = selectWDCArchFeature(TT,CPU);
  if (!features.empty()) {
    if (!allFeatures.empty())
      allFeatures = allFeatures + "," + features.str();
    else
      allFeatures = features.str();
  }
  return createWDCMCSubtargetInfoImpl(TT, CPU, /*TuneCPU*/ CPU, allFeatures);
// createWDCMCSubtargetInfoImpl defined in WDCGenSubtargetInfo.inc
}

MCInstPrinter *createWDCMCInstPrinter(const Triple &T,
                                              unsigned SyntaxVariant,
                                              const MCAsmInfo &MAI,
                                              const MCInstrInfo &MII,
                                              const MCRegisterInfo &MRI) {
  return new WDCInstPrinter{MAI, MII, MRI};
}

class WDCMCInstrAnalysis : public MCInstrAnalysis {
public:
  WDCMCInstrAnalysis(const MCInstrInfo *Info) : MCInstrAnalysis(Info) {}
};
}

static MCInstrAnalysis *createWDCMCInstrAnalysis(const MCInstrInfo *Info) {
  return new WDCMCInstrAnalysis{Info};

}

//@2 {
extern "C" void LLVMInitializeWDCTargetMC() {
  for (auto target : {&TheWDCTarget}) {
    // Register the MC asm info.
    RegisterMCAsmInfoFn X{*target, createWDCMCAsmInfo};

    // Register the MC instruction info.
    TargetRegistry::RegisterMCInstrInfo(*target, createWDCMCInstrInfo);

    // Register the MC register info.
    TargetRegistry::RegisterMCRegInfo(*target, createWDCMCRegisterInfo);

    // Register the MC subtarget info.
    TargetRegistry::RegisterMCSubtargetInfo(*target, createWDCMCSubtargetInfo);
    // Register the MC instruction analyzer.
    TargetRegistry::RegisterMCInstrAnalysis(*target, createWDCMCInstrAnalysis);
    // Register the MCInstPrinter.
    TargetRegistry::RegisterMCInstPrinter(*target, createWDCMCInstPrinter);
  }
}
//@2 }