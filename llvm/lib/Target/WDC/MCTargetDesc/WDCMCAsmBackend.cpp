//===-- WDCAsmBackend.cpp - WDC Asm Backend  ----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the WDCAsmBackend class.
//
//===----------------------------------------------------------------------===//
//

#include "WDCMCAsmBackend.h"
#include "WDCELFObjectWriter.h"
#include "WDCFixupKinds.h"
#include <llvm/MC/MCSubtargetInfo.h>
#include <llvm/MC/MCAsmBackend.h>
#include <llvm/MC/MCAssembler.h>
#include <llvm/MC/MCFixupKindInfo.h>
#include <llvm/MC/MCObjectWriter.h>

namespace {

unsigned adjustFixupValue(const llvm::MCFixup &Fixup, uint64_t Value,
                          llvm::MCContext &Ctx) {

  unsigned Kind = Fixup.getKind();

  // Add/subtract and shift
  switch (Kind) {
  default:
    return 0;
  // case FK_GPRel_4:
  // case FK_Data_4:
  // case Cpu0::fixup_Cpu0_LO16:
  //   break;
  // case Cpu0::fixup_Cpu0_HI16:
  // case Cpu0::fixup_Cpu0_GOT:
  //   // Get the higher 16-bits. Also add 1 if bit 15 is 1.
  //   Value = (Value >> 16) & 0xffff;
  //   break;
  }

  return Value;
}

class WDCMCAsmBackend final : public llvm::MCAsmBackend {
  llvm::Triple TheTriple;

public:
  WDCMCAsmBackend(const llvm::Target &T, const llvm::Triple &TT)
      : MCAsmBackend{TT.isLittleEndian() ? llvm::endianness::little
                                         : llvm::endianness::big},
        TheTriple{TT} {}

private:

  std::unique_ptr<llvm::MCObjectTargetWriter>
  createObjectTargetWriter() const override {
    return createWDCELFObjectWriter(TheTriple);
  }

  const llvm::MCFixupKindInfo & getFixupKindInfo(llvm::MCFixupKind Kind) const override {
    // unsigned JSUBReloRec = 0;
    // // if (HasLLD) {
    //   JSUBReloRec = llvm::MCFixupKindInfo::FKF_IsPCRel;
    // } else {
    //   JSUBReloRec =
    //       MCFixupKindInfo::FKF_IsPCRel | MCFixupKindInfo::FKF_Constant;
    // }
    constexpr static llvm::MCFixupKindInfo Infos[llvm::WDC::NumTargetFixupKinds] = {
        // This table *must* be in same the order of fixup_* kinds in
        // Cpu0FixupKinds.h.
        //
        // name                        offset  bits  flags
        {"fixup_WDC_long",             0,      24,    0},      
    };

    if (Kind < llvm::FirstTargetFixupKind) {
      return MCAsmBackend::getFixupKindInfo(Kind);
    }

    assert(unsigned(Kind - llvm::FirstTargetFixupKind) < llvm::WDC::NumTargetFixupKinds &&
           "Invalid kind!");
    return Infos[Kind - llvm::FirstTargetFixupKind];
  }

  /// ApplyFixup - Apply the \p Value for given \p Fixup into the provided
  /// data fragment, at the offset specified by the fixup and following the
  /// fixup kind as appropriate.
  void applyFixup(const llvm::MCAssembler &Asm, const llvm::MCFixup &Fixup,
                  const llvm::MCValue &Target, llvm::MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved,
                  const llvm::MCSubtargetInfo *STI) const override {
    const auto Kind = Fixup.getKind();
    auto &Ctx = Asm.getContext();
    Value = adjustFixupValue(Fixup, Value, Ctx);

    if (!Value)
      return; // Doesn't change encoding.

    // Where do we start in the object
    unsigned Offset = Fixup.getOffset();
    // Number of bytes we need to fixup
    unsigned NumBytes = (getFixupKindInfo(Kind).TargetSize + 7) / 8;
    // Used to point to big endian bytes
    unsigned FullSize;

    switch ((unsigned)Kind) {
    default:
      FullSize = 4;
      break;
    }

    // Grab current value, if any, from bits.
    uint64_t CurVal = 0;

    for (unsigned i = 0; i != NumBytes; ++i) {
      unsigned Idx = TheTriple.isLittleEndian() ? i : (FullSize - 1 - i);
      CurVal |= (uint64_t)((uint8_t)Data[Offset + Idx]) << (i * 8);
    }

    uint64_t Mask =
        ((uint64_t)(-1) >> (64 - getFixupKindInfo(Kind).TargetSize));
    CurVal |= Value & Mask;

    // Write out the fixed up bytes back to the code/data bits.
    for (unsigned i = 0; i != NumBytes; ++i) {
      unsigned Idx = TheTriple.isLittleEndian() ? i : (FullSize - 1 - i);
      Data[Offset + Idx] = (uint8_t)((CurVal >> (i * 8)) & 0xff);
    }
  }

  /// WriteNopData - Write an (optimal) nop sequence of Count bytes
  /// to the given output. If the target cannot generate such a sequence,
  /// it should return an error.
  ///
  /// \return - True on success.
  bool writeNopData(llvm::raw_ostream &OS, uint64_t Count,
                    const llvm::MCSubtargetInfo *STI) const override {
    return true;
  }
};
}

llvm::MCAsmBackend *llvm::createWDCMCAsmBackend(const Target &T,
                                          const MCSubtargetInfo &STI,
                                          const MCRegisterInfo &MRI,
                                          const MCTargetOptions &Options) {
  return new WDCMCAsmBackend(T, STI.getTargetTriple());;
}
