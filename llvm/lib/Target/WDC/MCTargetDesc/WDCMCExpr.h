//===-- WDCMCExpr.h - WDC specific MC expression classes ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_WDC_MCTARGETDESC_WDCMCEXPR_H
#define LLVM_LIB_TARGET_WDC_MCTARGETDESC_WDCMCEXPR_H

#include <llvm/MC/MCExpr.h>

namespace llvm {
class WDCMCExpr : public MCTargetExpr {

public:
    enum class WDCExprKind {
        None,
        AbsLong,
    };

    static MCExpr * create(WDCExprKind Kind, const MCExpr *Expr, MCContext &Ctx);
    ~WDCMCExpr() override;

private:
    WDCMCExpr() = delete;
    WDCMCExpr(WDCExprKind Kind, const MCExpr *Expr);

    void printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const override;
    void visitUsedExpr(MCStreamer &Streamer) const override;
    bool evaluateAsRelocatableImpl(MCValue &Res, const MCAssembler * Asm) const override;
    MCFragment *findAssociatedFragment() const override;

    const WDCExprKind exprKind;
    const MCExpr * const expr;
};
}
#endif
