//===-- WDCInstPrinter.cpp - Convert WDC MCInst to assembly syntax ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an WDC MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "WDCInstPrinter.h"

#include "WDCInstructionInfo.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

#define PRINT_ALIAS_INSTR
#include "WDCGenAsmWriter.inc"

void WDCInstPrinter::printRegName(raw_ostream &OS, MCRegister RegNo) {
//- getRegisterName(RegNo) defined in WDCGenAsmWriter.inc which indicate in 
//   WDC.td.
  OS << '$' << StringRef(getRegisterName(RegNo)).lower();
}

//@1 {
void WDCInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                StringRef Annot, const MCSubtargetInfo &STI,
                                raw_ostream &O) {
  // Try to print any aliases first.
  if (!printAliasInstr(MI, Address, O))
//@1 }
    //- printInstruction(MI, O) defined in WDCGenAsmWriter.inc which came from 
    //   WDC.td indicate.
    printInstruction(MI, Address, O);
  printAnnotation(O, Annot);
}

void WDCInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                   raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    printRegName(O, Op.getReg());
    return;
  }

  if (Op.isImm()) {
    O << '#' << Op.getImm();
    return;
  }

  assert(Op.isExpr() && "unknown operand kind in printOperand");
  Op.getExpr()->print(O, &MAI, true);
}

void WDCInstPrinter::printUnsignedImm(const MCInst *MI, int opNum,
                                       raw_ostream &O) {
  const MCOperand &MO = MI->getOperand(opNum);
  if (MO.isImm())
    O << (unsigned short int)MO.getImm();
  else
    printOperand(MI, opNum, O);
}

void WDCInstPrinter::
printMemOperand(const MCInst *MI, int opNum, raw_ostream &O) {
  // Load/Store memory operands -- imm($reg)
  // If PIC target the target is loaded as the
  // pattern ld $t9,%call16($gp)
  
  if (const auto & regOperand = MI->getOperand(opNum); regOperand.isReg() && regOperand.getReg() == WDC::S)
  {
    const auto &offsetOperand = MI->getOperand(opNum+1);
    O << offsetOperand.getImm() << ",S";
    return;
  }

  // default printing of BASE + offset
  printOperand(MI, opNum+1, O);
  O << "(";
  printOperand(MI, opNum, O);
  O << ")";
}

//#if CH >= CH7_1
// The DAG data node, mem_ea of WDCInstrInfo.td, cannot be disabled by
// ch7_1, only opcode node can be disabled.
void WDCInstPrinter::
printMemOperandEA(const MCInst *MI, int opNum, raw_ostream &O) {
  // when using stack locations for not load/store instructions
  // print the same way as all normal 3 operand instructions.
  printOperand(MI, opNum, O);
  O << ", ";
  printOperand(MI, opNum+1, O);
  return;
}
//#endif