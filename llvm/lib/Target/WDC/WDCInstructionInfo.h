//===-- WDCInstrInfo.h - WDC Instruction Information ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the WDC implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_WDC_WDCINSTRINFO_H
#define LLVM_LIB_TARGET_WDC_WDCINSTRINFO_H

#include "WDCConfig.h"

#include "WDC.h"
#include "WDCRegisterInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "WDCGenInstrInfo.inc"

namespace llvm {

class WDCInstrInfo : public WDCGenInstrInfo {
  const WDCRegisterInfo RI;

  virtual void anchor();
  // const WDCSubtarget &Subtarget;
  explicit WDCInstrInfo(const WDCSubtarget &STI);

public:
  static std::unique_ptr<const WDCInstrInfo> create(WDCSubtarget &STI);

  /// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
  /// such, whenever a client has an instance of instruction info, it should
  /// always be able to get register info as well (through this method).
  ///
  const WDCRegisterInfo &getRegisterInfo() const;

  bool expandPostRAPseudo(MachineInstr &MI) const override;

  /// Adjust SP by Amount bytes.
  void adjustStackPtr(unsigned SP, int64_t Amount,
                              MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator I) const;

  /// Return the number of bytes of code the specified instruction may be.
  unsigned GetInstSizeInBytes(const MachineInstr &MI) const;

  void storeRegToStackSlot(
      MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, Register SrcReg,
      bool isKill, int FrameIndex, const TargetRegisterClass *RC,
      const TargetRegisterInfo *TRI, Register VReg,
      MachineInstr::MIFlag Flags = MachineInstr::NoFlags) const override;

  virtual void loadRegFromStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, Register DestReg,
    int FrameIndex, const TargetRegisterClass *RC,
    const TargetRegisterInfo *TRI, Register VReg,
    MachineInstr::MIFlag Flags = MachineInstr::NoFlags) const override;

private:
  void storeRegToStack(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator MI, Register SrcReg,
                               bool isKill, int FrameIndex,
                               const TargetRegisterClass *RC,
                               const TargetRegisterInfo *TRI,
                               int64_t Offset) const;

  void loadRegFromStack(MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator MI,
                                Register DestReg, int FrameIndex,
                                const TargetRegisterClass *RC,
                                const TargetRegisterInfo *TRI,
                                int64_t Offset) const;

  MachineInstr *foldMemoryOperandImpl(MachineFunction &MF, MachineInstr &MI,
                                      ArrayRef<unsigned> Ops,
                                      MachineBasicBlock::iterator InsertPt,
                                      int FrameIndex,
                                      LiveIntervals *LIS = nullptr,
                                      VirtRegMap *VRM = nullptr) const override;

  MachineMemOperand *GetMemOperand(MachineBasicBlock &MBB, int FI,
                                   MachineMemOperand::Flags Flags) const;

  void expandRTL(MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const;
  void expandADD(MachineBasicBlock &MBB, MachineBasicBlock::iterator I, unsigned realOpcode) const;
  void expandSUB(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                 const unsigned realOpcode) const;
  void expandSRA(MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const;
};
}

#endif