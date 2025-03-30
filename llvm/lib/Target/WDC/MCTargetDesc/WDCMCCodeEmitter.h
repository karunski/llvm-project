//===-- WDCMCCodeEmitter.h - Convert WDC Code to Machine Code -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the WDCMCCodeEmitter class.
//
//===----------------------------------------------------------------------===//
//
#ifndef LLVM_LIB_TARGET_WDC_MCTARGETDESC_WDCMCCODEEMITTER_H
#define LLVM_LIB_TARGET_WDC_MCTARGETDESC_WDCMCCODEEMITTER_H
namespace llvm {
class MCContext;
class MCCodeEmitter;
class MCInstrInfo;

MCCodeEmitter *createWDCMCCodeEmitter(const MCInstrInfo &MCII, MCContext &Ctx);
} // namespace llvm
#endif