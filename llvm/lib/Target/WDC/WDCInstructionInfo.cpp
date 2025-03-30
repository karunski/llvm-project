//===-- WDCInstrInfo.cpp - WDC Instruction Information ------------------===//
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

#include "WDCInstructionInfo.h"

#include "WDCTargetMachine.h"
#include "WDCMachineFunction.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "WDCGenInstrInfo.inc"

// Pin the vtable to this file.
void WDCInstrInfo::anchor() {}

//@WDCInstrInfo {
WDCInstrInfo::WDCInstrInfo(const WDCSubtarget &STI) : RI{STI}{}

std::unique_ptr<const WDCInstrInfo> WDCInstrInfo::create(WDCSubtarget &STI) {
  return std::unique_ptr<const WDCInstrInfo>{new WDCInstrInfo{STI}};
}

//@GetInstSizeInBytes {
/// Return the number of bytes of code the specified instruction may be.
unsigned WDCInstrInfo::GetInstSizeInBytes(const MachineInstr &MI) const {
//@GetInstSizeInBytes - body
  // switch (MI.getOpcode()) {
  // default:
    return MI.getDesc().getSize();
  // }
}

void llvm::WDCInstrInfo::storeRegToStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, Register SrcReg,
    bool isKill, int FrameIndex, const TargetRegisterClass *RC,
    const TargetRegisterInfo *TRI, Register VReg,
    MachineInstr::MIFlag /*Flags*/) const {
  storeRegToStack(MBB, MI, SrcReg, isKill, FrameIndex, RC, TRI, 0);
}

void llvm::WDCInstrInfo::loadRegFromStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, Register DestReg,
    int FrameIndex, const TargetRegisterClass *RC,
    const TargetRegisterInfo *TRI, Register /*VReg*/,
    MachineInstr::MIFlag /*Flags*/) const {
  loadRegFromStack(MBB, MI, DestReg, FrameIndex, RC, TRI, 0);
}

MachineMemOperand *
llvm::WDCInstrInfo::GetMemOperand(MachineBasicBlock &basicBlock,
                                    int frameIndex,
                                    MachineMemOperand::Flags Flags) const {
  auto &machineFunction = *basicBlock.getParent();
  auto &frameInfo = machineFunction.getFrameInfo();

  return machineFunction.getMachineMemOperand(
      MachinePointerInfo::getFixedStack(machineFunction, frameIndex), Flags,
      frameInfo.getObjectSize(frameIndex),
      frameInfo.getObjectAlign(frameIndex));
}

const WDCRegisterInfo &WDCInstrInfo::getRegisterInfo() const {
  return RI;
}

bool llvm::WDCInstrInfo::expandPostRAPseudo(MachineInstr &MI) const {
MachineBasicBlock &MBB = *MI.getParent();

  switch (MI.getDesc().getOpcode()) {
  default:
    return false;
  case WDC::RetRTL:
    expandRTL(MBB, MI);
    break;
  case WDC::ADDi:
    expandADD(MBB, MI, WDC::ADCi);
    break;
  case WDC::ADDsr:
    expandADD(MBB, MI, WDC::ADCsr);
    break;
  case WDC::SUBsr:
    expandSUB(MBB, MI, WDC::SBCsr);
    break;
  case WDC::ROTL:
    expandROTL(MBB, MI);
    break;
  case WDC::SRA:
    expandSRA(MBB, MI);
    break;
  case WDC::TCA:
    expandTCA(MBB, MI);
    break;
  }

  MBB.erase(MI);
  return true;
}

void llvm::WDCInstrInfo::adjustStackPtr(unsigned SP, int64_t amount,
                                          MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator I) const {
  const auto debugLoc = I != MBB.end() ? I->getDebugLoc() : DebugLoc();
  // unsigned ADDu = Cpu0::ADDu;
  // unsigned ADDiu = Cpu0::ADDiu;

  assert(isInt<16>(amount) && "stack adjustment amount was too great");
  if (amount < 0) {
    amount = -amount;
    BuildMI(MBB, I, debugLoc, get(WDC::SEC));
    BuildMI(MBB, I, debugLoc, get(WDC::TSC));
    BuildMI(MBB, I, debugLoc, get(WDC::SBCi), WDC::A).addReg(WDC::A).addImm(amount);
    BuildMI(MBB, I, debugLoc, get(WDC::TCS));
  }
  else
  {
    BuildMI(MBB, I, debugLoc, get(WDC::CLC));
    BuildMI(MBB, I, debugLoc, get(WDC::TSC));
    BuildMI(MBB, I, debugLoc, get(WDC::ADCi), WDC::A).addReg(WDC::A).addImm(amount);
    BuildMI(MBB, I, debugLoc, get(WDC::TCS));
  }
  // else { // Expand immediate that doesn't fit in 16-bit.
  //   unsigned Reg = loadImmediate(Amount, MBB, I, DL, nullptr);
  //   BuildMI(MBB, I, DL, get(ADDu), SP).addReg(SP).addReg(Reg, RegState::Kill);
  // }
}

void llvm::WDCInstrInfo::storeRegToStack(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MI, Register SrcReg,
    bool isKill, int FrameIndex, const TargetRegisterClass *RC,
    const TargetRegisterInfo *TRI, int64_t Offset) const {
  MachineMemOperand *MMO = GetMemOperand(MBB, FrameIndex, MachineMemOperand::MOStore);

  const auto Opc = WDC::STAsr;
  assert(Opc && "Register class not handled!");

  BuildMI(MBB, MI, DebugLoc{}, get(Opc))
      .addReg(SrcReg, getKillRegState(isKill))
      .addFrameIndex(FrameIndex)
      .addImm(Offset)
      .addMemOperand(MMO);
}

void llvm::WDCInstrInfo::loadRegFromStack(MachineBasicBlock &basicBlock,
                                            MachineBasicBlock::iterator blockIter,
                                            Register DestReg, int FrameIndex,
                                            const TargetRegisterClass *RC,
                                            const TargetRegisterInfo */*TRI*/,
                                            int64_t Offset) const {
  const auto debugLoc = blockIter != basicBlock.end() ? blockIter->getDebugLoc() : DebugLoc{};

  const auto MMO = GetMemOperand(basicBlock, FrameIndex, MachineMemOperand::MOLoad);
  const auto Opc = WDC::LDAsr;
  assert(Opc && "Register class not handled!");
  BuildMI(basicBlock, blockIter, debugLoc, get(Opc), DestReg)
      .addFrameIndex(FrameIndex)
      .addImm(Offset)
      .addMemOperand(MMO);
}

MachineInstr *llvm::WDCInstrInfo::foldMemoryOperandImpl(
    MachineFunction &MF, MachineInstr &MI, ArrayRef<unsigned> Ops,
    MachineBasicBlock::iterator InsertPt, int FrameIndex, LiveIntervals *LIS,
    VirtRegMap *VRM) const {
  return nullptr;
}

void llvm::WDCInstrInfo::expandRTL(MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator I) const {
  BuildMI(MBB, I, I->getDebugLoc(), get(WDC::RTL));
}

void llvm::WDCInstrInfo::expandADD(MachineBasicBlock &MBB,
                                   MachineBasicBlock::iterator I,
                                   const unsigned realOpcode) const {
  const MachineOperand operands[] = {I->getOperand(0), I->getOperand(1),
                                     I->getOperand(2)};
  BuildMI(MBB, I, I->getDebugLoc(), get(WDC::CLC));
  BuildMI(MBB, I, I->getDebugLoc(), get(realOpcode))
      .add(operands[0])
      .add(operands[1])
      .add(operands[2]);
}

void llvm::WDCInstrInfo::expandSUB(MachineBasicBlock &MBB, MachineBasicBlock::iterator I, const unsigned realOpcode) const {
  BuildMI(MBB, I, I->getDebugLoc(), get(WDC::SEC));
  BuildMI(MBB, I, I->getDebugLoc(), get(realOpcode))
      .add(I->getOperand(0))
      .add(I->getOperand(1))
      .add(I->getOperand(2));
}

void llvm::WDCInstrInfo::expandSRA(MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const {
  BuildMI(MBB, I, I->getDebugLoc(), get(WDC::CMPi)).addReg(WDC::P).addReg(WDC::A).addImm(0x8000);
  BuildMI(MBB, I, I->getDebugLoc(), get(WDC::ROR));
}

void llvm::WDCInstrInfo::expandROTL(MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const {
  BuildMI(MBB, I, I->getDebugLoc(), get(WDC::ASL));
  BuildMI(MBB, I, I->getDebugLoc(), get(WDC::ADCi)).addReg(WDC::A).addReg(WDC::A).addImm(0);
}

void llvm::WDCInstrInfo::expandTCA(MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const {
  BuildMI(MBB, I, I->getDebugLoc(), get(WDC::ROL));
}

void llvm::WDCInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator MI,
                                     const DebugLoc &DL, Register DestReg,
                                     Register SrcReg, bool KillSrc,
                                     bool RenamableDest,
                                     bool RenamableSrc) const {
  if (SrcReg == WDC::P) {
    // push 8-bit status register on to the stack
    BuildMI(MBB, MI, MI->getDebugLoc(), get(WDC::PHP));
    if (DestReg == WDC::A) {
      // Set the Accumulator to 8 bits; pull the byte off the stack, and reset the accumulator back to 16 bit.
      BuildMI(MBB, MI, MI->getDebugLoc(), get(WDC::SEP)).addImm(0b00100000);
      BuildMI(MBB, MI, MI->getDebugLoc(), get(WDC::PLA));
      BuildMI(MBB, MI, MI->getDebugLoc(), get(WDC::REP)).addImm(0b00100000);
      return;
    }
  }

  llvm_unreachable("I didn't implement the rightTargetInstrInfo::copyPhysReg!");

}
