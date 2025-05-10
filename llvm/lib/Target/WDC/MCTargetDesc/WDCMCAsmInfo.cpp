//===-- WDCMCAsmInfo.cpp - WDC Asm Properties ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the WDCMCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "WDCMCAsmInfo.h"
#include "WDCConfig.h"
#include "WDCRegisterInfo.h"
#include <llvm/TargetParser/Triple.h>
#include <llvm/MC/MCAsmInfoELF.h>
#include <llvm/MC/MCRegisterInfo.h>
#include <llvm/MC/MCDwarf.h>

using namespace llvm;

namespace {
  class WDCMCAsmInfo : public MCAsmInfoELF {
    void anchor() override {}
  public:
    explicit WDCMCAsmInfo(const Triple &TheTriple);
  private:
    bool isAcceptableChar(char C) const override {
      return C == '?' || MCAsmInfoELF::isAcceptableChar(C);
    }

  };
}

WDCMCAsmInfo::WDCMCAsmInfo(const Triple &TheTriple) {
  IsLittleEndian = true; // the default of IsLittleEndian is true

  AlignmentIsInBytes = false;
  Data16bitsDirective = "\t.2byte\t";
  Data32bitsDirective = "\t.4byte\t";
  Data64bitsDirective = "\t.8byte\t";
  PrivateGlobalPrefix = "$";
  // PrivateLabelPrefix: display ?BB for the labels of basic block
  PrivateLabelPrefix = "?";
  CommentString = "#";
  ZeroDirective = "\t.space\t";
  // GPRel32Directive = "\t.gpword\t";
  // GPRel64Directive = "\t.gpdword\t";
  WeakRefDirective = "\t.weak\t";
  UseAssignmentForEHBegin = true;

  SupportsDebugInformation = true;
  ExceptionsType = ExceptionHandling::DwarfCFI;
  DwarfRegNumForCFI = true;
}

MCAsmInfo *llvm::createWDCMCAsmInfo(const MCRegisterInfo &MRI, const Triple &TT,
                                    const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new WDCMCAsmInfo{TT};

  unsigned SP = MRI.getDwarfRegNum(WDC::S, true);
  MCCFIInstruction Inst = MCCFIInstruction::createDefCfaRegister(nullptr, SP);
  MAI->addInitialFrameState(Inst);

  return MAI;
}