//===-- WDCMCCodeEmitter.cpp - Convert WDC Code to Machine Code ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the WDCMCCodeEmitter class.
//
//===----------------------------------------------------------------------===//
//

#include "WDCMCCodeEmitter.h"
#include "WDCMCExpr.h"
#include "WDCFixupKinds.h"
#include <llvm/MC/MCCodeEmitter.h>
#include <llvm/MC/MCInstrInfo.h>
#include <llvm/MC/MCInst.h>
#include <llvm/MC/MCContext.h>
#include <llvm/MC/MCRegisterInfo.h>
#include <llvm/MC/MCExpr.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Casting.h>

#define GET_INSTRMAP_INFO
#define GET_INSTRINFO_ENUM
#include <WDCGenInstrInfo.inc>
#undef GET_INSTRMAP_INFO
#undef GET_UNSTRINFO_ENUM

namespace llvm {
namespace {
class WDCMCCodeEmitter : public llvm::MCCodeEmitter {
    WDCMCCodeEmitter(const WDCMCCodeEmitter &) = delete;
    WDCMCCodeEmitter& operator=(const WDCMCCodeEmitter &) = delete;
    const MCInstrInfo &MCII;
    MCContext &Ctx;

  public:
    WDCMCCodeEmitter(const MCInstrInfo &mcii, MCContext &Ctx_)
        : MCII{mcii}, Ctx{Ctx_} {}

    ~WDCMCCodeEmitter() override = default;

  private:
    void EmitInstruction(uint64_t Val, unsigned Size,
                         SmallVectorImpl<char> &CB) const {
      // Output the instruction encoding in little endian byte order.
      for (unsigned i = 0; i < Size; ++i) {
        unsigned Shift = i * 8;
        CB.push_back((Val >> Shift) & 0xff);
      }
    }

    /// Encode the given \p Inst to bytes and append to \p CB.
    void encodeInstruction(const MCInst &MI, SmallVectorImpl<char> &CB,
                           SmallVectorImpl<MCFixup> &Fixups,
                           const MCSubtargetInfo &STI) const override {
      const auto Binary = getBinaryCodeForInstr(MI, Fixups, STI);

      // Check for unimplemented opcodes.
      const auto &Desc = MCII.get(MI.getOpcode());

      // Pseudo instructions don't get encoded and shouldn't be here
      // in the first place!
    //   if ((Desc.TSFlags & WDCII::FormMask) == Cpu0II::Pseudo)
    //     llvm_unreachable("Pseudo opcode found in encodeInstruction()");

      EmitInstruction(Binary, Desc.getSize(), CB);
    }

    // getBinaryCodeForInstr - TableGen'erated function for getting the
    // binary encoding for an instruction.
    uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                   SmallVectorImpl<MCFixup> &Fixups,
                                   const MCSubtargetInfo &STI) const;

    // getMachineOpValue - Return binary encoding of operand. If the machine
    // operand requires relocation, record the relocation and return zero.
    // This function is invoked from the TableGen-generated function getBinaryCodeForInstr()
    unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                               SmallVectorImpl<MCFixup> &Fixups,
                               const MCSubtargetInfo &STI) const {
      if (MO.isReg()) {
        unsigned Reg = MO.getReg();
        unsigned RegNo = Ctx.getRegisterInfo()->getEncodingValue(Reg);
        return RegNo;
      } else if (MO.isImm()) {
        return static_cast<unsigned>(MO.getImm());
      } 

      // MO must be an Expr.
      assert(MO.isExpr() && "Machine Op value must be an expression");
      return getExprOpValue(MO.getExpr(), Fixups, STI);
    }

    unsigned getMemEncoding(const MCInst &MI, unsigned OpNo,
                            SmallVectorImpl<MCFixup> &Fixups,
                            const MCSubtargetInfo &STI) const;

    unsigned getExprOpValue(const MCExpr *Expr,
                            SmallVectorImpl<MCFixup> &Fixups,
                            const MCSubtargetInfo &STI) const {
      const MCExpr::ExprKind Kind = Expr->getKind();
      if (Kind == MCExpr::Constant) {
        return cast<MCConstantExpr>(Expr)->getValue();
      }

      if (Kind == MCExpr::Binary) {
        unsigned Res =
            getExprOpValue(cast<MCBinaryExpr>(Expr)->getLHS(), Fixups, STI);
        Res += getExprOpValue(cast<MCBinaryExpr>(Expr)->getRHS(), Fixups, STI);
        return Res;
      }

      if (Kind == MCExpr::Target) {
        const auto wdcExpr = cast<WDCMCExpr>(Expr);

        auto FixupKind = WDC::Fixups(0);
        switch (wdcExpr->getKind()) {
        default:
          llvm_unreachable("Unsupported fixup kind for target expression!");
        } // switch
        Fixups.push_back(MCFixup::create(0, Expr, MCFixupKind(FixupKind)));
        return 0;
      }

      // All of the information is in the fixup.
      return 0;
    }
}; // class WDCMCCodeEmitter

#include <WDCGenMCCodeEmitter.inc>
}
}

llvm::MCCodeEmitter *llvm::createWDCMCCodeEmitter(const MCInstrInfo &MCII,
                                            MCContext &Ctx) {
  return new llvm::WDCMCCodeEmitter{MCII, Ctx};
}
