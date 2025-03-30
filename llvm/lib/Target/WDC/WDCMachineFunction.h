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
  explicit WDCFunctionInfo(MachineFunction& machineFunc);

  ~WDCFunctionInfo();

  int getVarArgsFrameIndex() const { return VarArgsFrameIndex; }
  void setVarArgsFrameIndex(int Index) { VarArgsFrameIndex = Index; }

  unsigned getSRetReturnReg() const { return SRetReturnReg; }
  void setSRetReturnReg(unsigned Reg) { SRetReturnReg = Reg; }

  void createEhDataRegsFI();
  int getEhDataRegFI(unsigned Reg) const { return EhDataRegFI[Reg]; }

private:
  MachineFunction& MF;

    /// VarArgsFrameIndex - FrameIndex for start of varargs area.
  int VarArgsFrameIndex = 0;

  unsigned MaxCallFrameSize = 0;

  /// SRetReturnReg - Some subtargets require that sret lowering includes
  /// returning the value of the returned struct in a register. This field
  /// holds the virtual register into which the sret argument is passed.
  unsigned SRetReturnReg;

  /// Frame objects for spilling eh data registers.
  int EhDataRegFI[2] = {};
};
} // end of namespace llvm

#endif // WDC_MACHINE_FUNCTION_INFO_H