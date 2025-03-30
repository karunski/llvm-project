//===-- WDCELFObjectWriter.cpp - WDC ELF Writer -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "WDCELFObjectWriter.h"

#include "WDCConfig.h"

#include "WDCBaseInfo.h"
#include "MCTargetDesc/WDCFixupKinds.h"
#include "MCTargetDesc/WDCMCTargetDesc.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"
#include <list>

using namespace llvm;

namespace {
  class WDCELFObjectWriter : public MCELFObjectTargetWriter {
  public:
    WDCELFObjectWriter(uint8_t OSABI);

	~WDCELFObjectWriter() = default;

    unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
    bool needsRelocateWithSymbol(const MCValue & Val, const MCSymbol &Sym,
                                 unsigned Type) const override;
  };
}

WDCELFObjectWriter::WDCELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter{/*Is64Bit_=false*/ false, OSABI, ELF::EM_WDC,
                              /*HasRelocationAddend_ = false*/ false} {}

//@GetRelocType {
unsigned WDCELFObjectWriter::getRelocType(MCContext &Ctx,
                                           const MCValue &Target,
                                           const MCFixup &Fixup,
                                           bool IsPCRel) const {
  // determine the type of the relocation
  auto Type = static_cast<unsigned>(ELF::R_WDC_NONE);
  auto Kind = static_cast<unsigned>(Fixup.getKind());

  switch (Kind) {
  default:
    llvm_unreachable("invalid fixup kind!");
  case FK_Data_4:
    Type = ELF::R_WDC_NONE;
    break;
  case WDC::fixup_WDC_long:
    Type = ELF::R_WDC_LONG;
    break;
  }

  return Type;
}
//@GetRelocType }

bool WDCELFObjectWriter::needsRelocateWithSymbol(const MCValue & Val, const MCSymbol &Sym,
                                                 unsigned Type) const {
  // FIXME: This is extremelly conservative. This really needs to use a
  // whitelist with a clear explanation for why each realocation needs to
  // point to the symbol, not to the section.
  switch (Type) {
  default:
    return true;

//   case ELF::R_CPU0_GOT16:
//   // For Cpu0 pic mode, I think it's OK to return true but I didn't confirm.
//   //  llvm_unreachable("Should have been handled already");
//     return true;

  // These relocations might be paired with another relocation. The pairing is
  // done by the static linker by matching the symbol. Since we only see one
  // relocation at a time, we have to force them to relocate with a symbol to
  // avoid ending up with a pair where one points to a section and another
  // points to a symbol.
//   case ELF::R_CPU0_HI16:
//   case ELF::R_CPU0_LO16:
  // R_CPU0_32 should be a relocation record, I don't know why Mips set it to 
  // false.
//   case ELF::R_CPU0_32:
//     return true;

//   case ELF::R_CPU0_GPREL16:
//     return false;
  }
}

std::unique_ptr<MCObjectTargetWriter> 
llvm::createWDCELFObjectWriter(const Triple &TT) {
  const auto OSABI = MCELFObjectTargetWriter::getOSABI(TT.getOS());
  return std::make_unique<WDCELFObjectWriter>(OSABI);
}
