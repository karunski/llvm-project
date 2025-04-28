//===-- WDCMCInstLower.cpp - Convert WDC MachineInstr to MCInst ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains code to lower WDC MachineInstrs to their corresponding
// MCInst records.
//
//===----------------------------------------------------------------------===//

#include "WDCMCInstLower.h"

#include "WDCAsmPrinter.h"
#include "WDCInstructionInfo.h"
#include "MCTargetDesc/WDCBaseInfo.h"
#include "MCTargetDesc/WDCMCExpr.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"

using namespace llvm;

WDCMCInstLower::WDCMCInstLower(WDCAsmPrinter &asmprinter)
    : AsmPrinter{asmprinter} {}

void WDCMCInstLower::Initialize(MCContext* C) {
  Ctx = C;
}

// static void CreateMCInst(MCInst& Inst, unsigned Opc, const MCOperand& Opnd0,
//                          const MCOperand& Opnd1,
//                          const MCOperand& Opnd2 = MCOperand()) {
//   Inst.setOpcode(Opc);
//   Inst.addOperand(Opnd0);
//   Inst.addOperand(Opnd1);
//   if (Opnd2.isValid())
//     Inst.addOperand(Opnd2);
// }

MCOperand WDCMCInstLower::LowerSymbolOperand(const MachineOperand &MO,
                                    MachineOperandType MOTy, unsigned Offset) const {
  MCSymbol * Symbol = nullptr;

  switch (MOTy) {
  case MachineOperand::MO_GlobalAddress:
    Symbol = AsmPrinter.getSymbol(MO.getGlobal());
    Offset += MO.getOffset();
    break;

  default:
    llvm_unreachable("<unknown operand type>");
  }

  const auto targetKind = [targetMoFlags = MO.getTargetFlags()]() {
    switch (targetMoFlags) {
    case WDCII::MO_ABS_HI:
      return WDCMCExpr::WDCExprKind::ImmAbsLongHi;
    case WDCII::MO_ABS_LO:
      return WDCMCExpr::WDCExprKind::ImmAbsLongLo;
    default:
      return WDCMCExpr::WDCExprKind::AbsLong;
    }
  }();

  const auto Kind = MCSymbolRefExpr::VK_None;
  const auto SymbolExpr = MCSymbolRefExpr::create(Symbol, Kind, *Ctx);

  const auto Expr = [this, Offset, targetKind, SymbolExpr]() -> const MCExpr * {
    const auto Expr = WDCMCExpr::create(targetKind, SymbolExpr, *Ctx);
    if (Offset) {
      // Assume offset is never negative.
      assert(Offset > 0);
      return MCBinaryExpr::createAdd(Expr, MCConstantExpr::create(Offset, *Ctx),
                                     *Ctx);
    }
    return Expr;
  }();
  return MCOperand::createExpr(Expr);
}

//@LowerOperand {
MCOperand WDCMCInstLower::LowerOperand(const MachineOperand& MO,
                                        unsigned offset) const {
  MachineOperandType MOTy = MO.getType();

  switch (MOTy) {
  //@2
  default: llvm_unreachable("unknown operand type");
  case MachineOperand::MO_Register:
    // Ignore all implicit register operands.
    if (MO.isImplicit()) break;
    return MCOperand::createReg(MO.getReg());
  case MachineOperand::MO_Immediate:
    return MCOperand::createImm(MO.getImm() + offset);
  case MachineOperand::MO_GlobalAddress:
    return LowerSymbolOperand(MO, MOTy, offset);
  case MachineOperand::MO_RegisterMask:
    break;
 }

  return MCOperand();
}

void WDCMCInstLower::Lower(const MachineInstr *MI, MCInst &OutMI) const {
  OutMI.setOpcode(MI->getOpcode());

  for (unsigned i = 0, e = MI->getNumOperands(); i != e; ++i) {
    const MachineOperand &MO = MI->getOperand(i);
    MCOperand MCOp = LowerOperand(MO);

    if (MCOp.isValid())
      OutMI.addOperand(MCOp);
  }
}