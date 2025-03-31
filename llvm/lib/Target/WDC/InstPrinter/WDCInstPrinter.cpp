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

WDCInstPrinter::WDCInstPrinter(const MCAsmInfo &MAI, const MCInstrInfo &MII,
  const MCRegisterInfo &MRI)
: MCInstPrinter{MAI, MII, MRI} {}

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

  assert(Op.isExpr() && "unknown operand kind in printOperand");
  Op.getExpr()->print(O, &MAI, true);
}

void WDCInstPrinter::printUnsignedImm(const MCInst *MI, int opNum,
                                       raw_ostream &O, unsigned immBits) {
  const MCOperand &MO = MI->getOperand(opNum);
  if (MO.isImm()) {
    O << "#$" << format_hex_no_prefix(static_cast<uint16_t>(MO.getImm()), immBits / 4 );
  }
  else {
    printOperand(MI, opNum, O);
  }
}

void WDCInstPrinter::printUnsignedImm8(const MCInst * MI, int opNum, raw_ostream &O) {
  printUnsignedImm(MI, opNum, O, 8);
}

void WDCInstPrinter::printUnsignedImm16(const MCInst * MI, int opNum, raw_ostream &O) {
  printUnsignedImm(MI, opNum, O, 16);
}

void WDCInstPrinter::printAbsoluteLong(const MCInst * MI, int opNum, raw_ostream &O) {
  const MCOperand &MO = MI->getOperand(opNum);
  if (MO.isImm()) {
    const auto imm = static_cast<uint32_t>(MO.getImm());
    O << "$" << format_hex_no_prefix((imm >> 16) & 0xff, 2) << ":"
      << format_hex_no_prefix((imm & 0xffff), 4);
  }

  const auto expr = MO.getExpr();
  expr->print(O, &this->MAI, false);
}

void llvm::WDCInstPrinter::printStackOffset(const MCInst *MI, int opNum,
                                            raw_ostream &os) {
  os << MI->getOperand(opNum).getImm() << ",S";
}

void llvm::WDCInstPrinter::printDirectPageIndirect(const MCInst *MI, int opNum, raw_ostream &os) {
  os << "[" << MI->getOperand(opNum).getImm() << "]";
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