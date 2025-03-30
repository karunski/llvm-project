//===-- WDCTargetMachine.cpp - Define TargetMachine for WDC 65816 -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implements the info about WDC target spec.
//
//===----------------------------------------------------------------------===//

#include "WDCTargetMachine.h"
#include "WDC.h"
#include "WDCSubtarget.h"
#include "WDCTargetObjectFile.h"
#include "WDCSEISelDAGToDAG.h"

#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

#define DEBUG_TYPE "wdc"

extern "C" void LLVMInitializeWDCTarget() {
  // Register the target.
  RegisterTargetMachine<WDCTargetMachine> X(TheWDCTarget);
}

static std::string computeDataLayout(const Triple &TT, StringRef CPU,
                                     const TargetOptions &Options) {
  return "e-S16-p:24:16-i8:8-i16:8-i32:8-i64:8-f32:8-f64:8-n16-a:8";
}

static Reloc::Model getEffectiveRelocModel(bool JIT,
                                           std::optional<Reloc::Model> RelocModel) {
  if (!RelocModel.has_value() || JIT)
    return Reloc::Static;
  return *RelocModel;
}

// DataLayout --> Big-endian, 32-bit pointer/ABI/alignment
// The stack is always 8 byte aligned
// On function prologue, the stack is created by decrementing
// its pointer. Once decremented, all references are done with positive
// offset from the stack/frame pointer, using StackGrowsUp enables
// an easier handling.
// Using CodeModel::Large enables different CALL behavior.
WDCTargetMachine::WDCTargetMachine(const Target &T, const Triple &TT,
                                   StringRef CPU, StringRef FS,
                                   const TargetOptions &Options,
                                   std::optional<Reloc::Model> RM,
                                   std::optional<CodeModel::Model> CM,
                                   CodeGenOptLevel OptimLevel, bool JIT)
    //- Default is big endian
    : CodeGenTargetMachineImpl{T,
                    computeDataLayout(TT, CPU, Options),
                    TT,
                    CPU,
                    FS,
                    Options,
                    getEffectiveRelocModel(JIT, RM),
                    getEffectiveCodeModel(CM, CodeModel::Small),
                    OptimLevel},
      /*isLittle(isLittle),*/ TLOF{std::make_unique<WDCTargetObjectFile>()},
      ABI{WDCABIInfo::computeTargetABI()},
      DefaultSubtarget{TT, CPU, FS, *this} {
  // initAsmInfo will display features by llc -march=WDC -mcpu=help on 3.7 but
  // not on 3.6
  initAsmInfo();
}

WDCTargetMachine::~WDCTargetMachine() = default;

// void WDCebTargetMachine::anchor() { }

// WDCebTargetMachine::WDCebTargetMachine(const Target &T, const Triple &TT,
//                                          StringRef CPU, StringRef FS,
//                                          const TargetOptions &Options,
//                                          Optional<Reloc::Model> RM,
//                                          Optional<CodeModel::Model> CM,
//                                          CodeGenOpt::Level OL, bool JIT)
//     : WDCTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, JIT, false) {}

// void WDCelTargetMachine::anchor() { }

// WDCelTargetMachine::WDCelTargetMachine(const Target &T, const Triple &TT,
//                                          StringRef CPU, StringRef FS,
//                                          const TargetOptions &Options,
//                                          Optional<Reloc::Model> RM,
//                                          Optional<CodeModel::Model> CM,
//                                          CodeGenOpt::Level OL, bool JIT)
//     : WDCTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, JIT, true) {}

const WDCSubtarget *
WDCTargetMachine::getSubtargetImpl(const Function &F) const {
  std::string CPU = TargetCPU;
  std::string FS = TargetFS;

  auto &I = SubtargetMap[CPU + FS];
  if (!I) {
    // This needs to be done before we create a new subtarget since any
    // creation will depend on the TM and the code generation flags on the
    // function that reside in TargetOptions.
    resetTargetOptions(F);
    I = std::make_unique<WDCSubtarget>(TargetTriple, CPU, FS, *this);
  }
  return I.get();
}

namespace {
//@WDCPassConfig {
/// WDC Code Generator Pass Configuration Options.
class WDCPassConfig : public TargetPassConfig {
public:
  WDCPassConfig(WDCTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  WDCTargetMachine &getWDCTargetMachine() const {
    return getTM<WDCTargetMachine>();
  }

  bool addInstSelector() override {
    addPass(createWDCSEISelDag(getWDCTargetMachine(), getOptLevel()));
    return false;
  }

  const WDCSubtarget &getWDCSubtarget() const {
    return *getWDCTargetMachine().getSubtargetImpl();
  }
};
} // namespace

TargetPassConfig *WDCTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new WDCPassConfig(*this, PM);
}
