//===-- WDCTargetMachine.h - Define TargetMachine for WDC -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the WDC specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_WDC_WDCTARGETMACHINE_H
#define LLVM_LIB_TARGET_WDC_WDCTARGETMACHINE_H

#include "WDCConfig.h"

#include "MCTargetDesc/WDCABIInfo.h"
#include "WDCSubtarget.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"

namespace llvm {
class formatted_raw_ostream;
class WDCRegisterInfo;

class WDCTargetMachine : public CodeGenTargetMachineImpl {
  //bool isLittle;
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  // Selected ABI
  WDCABIInfo ABI;
  WDCSubtarget DefaultSubtarget;

  mutable StringMap<std::unique_ptr<WDCSubtarget>> SubtargetMap;
public:
  WDCTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                    StringRef FS, const TargetOptions &Options,
                    std::optional<Reloc::Model> RM, std::optional<CodeModel::Model> CM,
                    CodeGenOptLevel OL, bool JIT);
  ~WDCTargetMachine() override;

  const WDCSubtarget *getSubtargetImpl() const {
    return &DefaultSubtarget;
  }

  const WDCSubtarget *getSubtargetImpl(const Function &F) const override;

  // Pass Pipeline Configuration
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }
  // bool isLittleEndian() const { return isLittle; }
  const WDCABIInfo &getABI() const { return ABI; }
};

// /// Cpu0ebTargetMachine - Cpu032 big endian target machine.
// ///
// class Cpu0ebTargetMachine : public Cpu0TargetMachine {
//   virtual void anchor();
// public:
//   Cpu0ebTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
//                       StringRef FS, const TargetOptions &Options,
//                       Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
//                       CodeGenOpt::Level OL, bool JIT);
// };

// /// Cpu0elTargetMachine - Cpu032 little endian target machine.
// ///
// class Cpu0elTargetMachine : public Cpu0TargetMachine {
//   virtual void anchor();
// public:
//   Cpu0elTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
//                       StringRef FS, const TargetOptions &Options,
//                       Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
//                       CodeGenOpt::Level OL, bool JIT);
// };
} // End llvm namespace

#endif
