//===-- WDCMCExpr.cpp - WDC specific MC expression classes --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "WDCMCExpr.h"
#include <llvm/MC/MCContext.h>

llvm::WDCMCExpr::WDCMCExpr(WDCExprKind Kind, const MCExpr *Expr)
    : exprKind{Kind}, expr{Expr} {}

void llvm::WDCMCExpr::printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const {

  const auto outputExpr = [this, &OS, MAI]() {
    int64_t AbsVal = 0;
    if (this->expr->evaluateAsAbsolute(AbsVal)) {
      OS << AbsVal;
    } else {
      expr->print(OS, MAI, true);
    }
  };

  if (exprKind == WDCExprKind::AbsLong) {
    OS << ">";
    outputExpr();
  }
  else if (exprKind == WDCExprKind::ImmAbsLongHi) {
    OS << "#(";
    outputExpr();
    OS << ").high";
  }
  else if (exprKind == WDCExprKind::ImmAbsLongLo) {
    OS << "#(";
    outputExpr();
    OS << ").low";
  }
  else
  {
    assert(false && "Unhandled WDCExprKind");
  }
}

void llvm::WDCMCExpr::visitUsedExpr(MCStreamer &Streamer) const {
    llvm_unreachable("llvm::WDCMCExpr::visitUsedExpr not implemented");
}

bool llvm::WDCMCExpr::evaluateAsRelocatableImpl(MCValue &Res,
                                                const MCAssembler *Asm) const {
  llvm_unreachable("llvm::WDCMCExpr::evaluateAsRelocatableImpl not "
                   "implemented");
  return false;
}

auto llvm::WDCMCExpr::create(WDCExprKind Kind, const MCExpr *Expr,
                                MCContext &Ctx) -> MCExpr *{
  return new (Ctx) WDCMCExpr{Kind, Expr};
}

llvm::WDCMCExpr::~WDCMCExpr() = default;

auto llvm::WDCMCExpr::findAssociatedFragment() const -> MCFragment * {
  return nullptr;//getSubExpr()->findAssociatedFragment();
}