//===-- WDCMachineFunctionInfo.h - Private data used for WDC ----*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the WDC specific subclass of MachineFunctionInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_WDC_WDCMACHINEFUNCTION_H
#define LLVM_LIB_TARGET_WDC_WDCMACHINEFUNCTION_H

#include "WDCConfig.h"

#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/PseudoSourceValue.h"
#include "llvm/Target/TargetMachine.h"
#include <map>

namespace llvm {

//@1 {
/// WDCFunctionInfo - This class is derived from MachineFunction private
/// WDC target-specific information for each MachineFunction.
class WDCFunctionInfo : public MachineFunctionInfo {
public:
  WDCFunctionInfo(MachineFunction& MF)
  : MF(MF), 
    VarArgsFrameIndex(0), 
    MaxCallFrameSize(0)
    {}

  ~WDCFunctionInfo();

  int getVarArgsFrameIndex() const { return VarArgsFrameIndex; }
  void setVarArgsFrameIndex(int Index) { VarArgsFrameIndex = Index; }

private:
  virtual void anchor();

  MachineFunction& MF;

    /// VarArgsFrameIndex - FrameIndex for start of varargs area.
  int VarArgsFrameIndex;

  unsigned MaxCallFrameSize;
};
//@1 }

} // end of namespace llvm

#endif // WDC_MACHINE_FUNCTION_INFO_H