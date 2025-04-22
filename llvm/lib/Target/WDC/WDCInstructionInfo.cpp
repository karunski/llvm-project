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
  const auto instr = static_cast<decltype(WDC::RetRTL)>(MI.getDesc().getOpcode());

  switch (instr) {
  default:
    return false;
  case WDC::ADDdp:
    expandADD(MBB, MI, WDC::ADCdp);
    break;
  case WDC::ADDi:
    expandADD(MBB, MI, WDC::ADCi);
    break;
  case WDC::ADDal:
    expandADD(MBB, MI, WDC::ADCal);
    break;
  case WDC::LDGPdp:
    expandLDGPdp(MBB, MI);
    break;
  case WDC::RetRTL:
    expandRTL(MBB, MI);
    break;
  case WDC::ROTL:
    expandROTL(MBB, MI);
    break;
  case WDC::SetM:
    expandSetM(MBB, MI);
    break;
  case WDC::SRA:
    expandSRA(MBB, MI);
    break;
  case WDC::STGPdp:
    expandSTGPdp(MBB, MI);
    break;
  case WDC::SUBdp:
    expandSUB(MBB, MI, WDC::SBCdp);
    break;
  case WDC::SUBal:
    expandSUB(MBB, MI, WDC::SBCal);
    break;
  case WDC::TCA:
    expandTCA(MBB, MI);
    break;
  case WDC::LEAsr:
    expandLEA(MBB, MI);
    break;
  }

  MBB.erase(MI);
  return true;
}

void llvm::WDCInstrInfo::adjustStackPtr(unsigned SP, int64_t amount,
                                          MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator I) const {
  const auto debugLoc = I != MBB.end() ? I->getDebugLoc() : DebugLoc();

  assert(isInt<16>(amount) && "stack adjustment amount was too great");
  if (amount < 0) {
    if (amount == -2) {
        BuildMI(MBB, I, debugLoc, get(WDC::PHA));
        BuildMI(MBB, I, debugLoc, get(WDC::TSC));
    }
    else {
      amount = -amount;
      BuildMI(MBB, I, debugLoc, get(WDC::SEC));
      // only if in 8-bit A mode 
      //BuildMI(MBB, I, debugLoc, get(WDC::REP), WDC::P).addImm(0x20);
      BuildMI(MBB, I, debugLoc, get(WDC::TSC));
      BuildMI(MBB, I, debugLoc, get(WDC::SBCi), WDC::C).addReg(WDC::C).addImm(amount);
      BuildMI(MBB, I, debugLoc, get(WDC::TCS));
    }
  }
  else
  {
    if (amount == 2)
    {
      BuildMI(MBB, I, debugLoc, get(WDC::PLA));
    }
    else {
      BuildMI(MBB, I, debugLoc, get(WDC::CLC));
      // ONly if in 8-bit a mode.
      //BuildMI(MBB, I, debugLoc, get(WDC::REP), WDC::P).addImm(0x20);
      BuildMI(MBB, I, debugLoc, get(WDC::TSC));
      BuildMI(MBB, I, debugLoc, get(WDC::ADCi), WDC::C).addReg(WDC::C).addImm(amount);
      BuildMI(MBB, I, debugLoc, get(WDC::TCS));
    }
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

  if (SrcReg == WDC::X) {
    // Store the X register to the stack.
    BuildMI(MBB, MI, DebugLoc{}, get(WDC::STXdp))
        .addFrameIndex(FrameIndex);
    return;
  }

  if (RC == &WDC::RegsA16RegClass) {
    BuildMI(MBB, MI, DebugLoc{}, get(WDC::STAdp))
        .addReg(SrcReg, getKillRegState(isKill))
        .addFrameIndex(FrameIndex)
        .addImm(Offset)
        .addMemOperand(MMO);
    return;
  }

  const auto Opc = WDC::STGPdp;
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
  MachineMemOperand *MMO = GetMemOperand(basicBlock, FrameIndex, MachineMemOperand::MOLoad);

  if (DestReg == WDC::X) {
    // Store the X register to the stack.
    BuildMI(basicBlock, blockIter, DebugLoc{}, get(WDC::LDXdp))
        .addFrameIndex(FrameIndex).addImm(Offset).addMemOperand(MMO);
    return;
  }
  
  if (RC == &WDC::RegsA16RegClass) {
    BuildMI(basicBlock, blockIter, debugLoc, get(WDC::LDAdp), DestReg)
      .addFrameIndex(FrameIndex).addImm(Offset).addMemOperand(MMO);
    return;
  }

  // const auto MMO = GetMemOperand(basicBlock, FrameIndex, MachineMemOperand::MOLoad);
  // const auto Opc = WDC::LDAdp;
  assert(false && "Register class not handled!");
}

MachineInstr *llvm::WDCInstrInfo::foldMemoryOperandImpl(
    MachineFunction &MF, MachineInstr &MI, ArrayRef<unsigned> Ops,
    MachineBasicBlock::iterator InsertPt, int FrameIndex, LiveIntervals *LIS,
    VirtRegMap *VRM) const {
  return nullptr;
}

Register llvm::WDCInstrInfo::isLoadFromStackSlot(const MachineInstr &MI,
                                                 int &FrameIndex) const {
  const auto opcode = static_cast<TargetOpcodeTy>(MI.getOpcode());
  if (opcode == WDC::LDGPdp || opcode == WDC::LDAdp || opcode == WDC::LDXdp ||
      opcode == WDC::LDYdp) {
    FrameIndex = MI.getOperand(1).getIndex();
    return MI.getOperand(0).getReg();
  }
  return 0;
}

Register llvm::WDCInstrInfo::isStoreToStackSlot(const MachineInstr &MI,
                                                int &FrameIndex) const {
  const auto opcode = static_cast<TargetOpcodeTy>(MI.getOpcode());
  if (opcode == WDC::STGPdp || opcode == WDC::STAdp) {
    FrameIndex = MI.getOperand(1).getIndex();
    return MI.getOperand(0).getReg();
  }
  return 0;
}

Register llvm::WDCInstrInfo::isLoadFromStackSlotPostFE(const MachineInstr &MI,
                                                       int &FrameIndex) const {
  const auto opcode = static_cast<TargetOpcodeTy>(MI.getOpcode());
  if (opcode == WDC::LDGPdp || opcode == WDC::LDAdp || opcode == WDC::LDXdp ||
      opcode == WDC::LDYdp) {
    return 0;
  }
  return 0;
}

Register llvm::WDCInstrInfo::isStoreToStackSlotPostFE(const MachineInstr &MI,
                                                      int &FrameIndex) const {
  const auto opcode = static_cast<TargetOpcodeTy>(MI.getOpcode());
  if (opcode == WDC::STGPdp || opcode == WDC::STAdp) {
    return 0;
  }
  return 0;
}

void llvm::WDCInstrInfo::expandSTGPdp(MachineBasicBlock& MBB, MachineBasicBlock::iterator MI) const {
  const auto debugLoc = MI->getDebugLoc();
  const auto srcOprnd = MI->getOperand(0);
  const auto addrOprnd = MI->getOperand(1);
  assert(srcOprnd.isReg() && "Expected register operand for STGPdp src!");
  assert(addrOprnd.isImm() && "Expected immediate operand for STGPdp addr!");
  const auto srcOprndReg = srcOprnd.getReg();
  const auto instr = [srcOprndReg]() {
    if (srcOprndReg == WDC::C) {
      return WDC::STAdp;
    }
    if (srcOprndReg == WDC::X) {
      return WDC::STXdp;
    }
    if (srcOprndReg == WDC::Y) {
      return WDC::STYdp;
    }
    assert(false && "Unexpected register operand for STGPdp!");
    return WDC::STAdp;
  }();
  BuildMI(MBB, MI, debugLoc, get(instr)).add(srcOprnd).add(addrOprnd);
}

void llvm::WDCInstrInfo::expandLDGPdp(MachineBasicBlock &MBB,
                                      MachineBasicBlock::iterator MI) const {
  const auto debugLoc = MI->getDebugLoc();
  const auto destOprnd = MI->getOperand(0);
  const auto addrOprnd = MI->getOperand(1);
  assert(destOprnd.isReg() && "Expected register operand for LDGPdp src!");
  assert(addrOprnd.isImm() && "Expected immediate operand for LDGPdp addr!");
  const auto destOprndReg = destOprnd.getReg();
  const auto instr = [destOprndReg]() {
    if (destOprndReg == WDC::C) {
      return WDC::LDAdp;
    }
    if (destOprndReg == WDC::X) {
      return WDC::LDXdp;
    }
    if (destOprndReg == WDC::Y) {
      return WDC::LDYdp;
    }
    assert(false && "Unexpected register operand for LDGPdp!");
    return WDC::LDAdp;
  }();
  BuildMI(MBB, MI, debugLoc, get(instr)).add(addrOprnd);
}

void llvm::WDCInstrInfo::expandADD(MachineBasicBlock &MBB,
                                   MachineBasicBlock::iterator I,
                                   TargetOpcodeTy nativeAddOpc) const {
  const auto dbgLoc = I->getDebugLoc();
  const auto destOprnd = I->getOperand(0);
  const auto lhsOprnd = I->getOperand(1);
  const auto rhsOprnd = I->getOperand(2);
  BuildMI(MBB, I, dbgLoc, get(WDC::CLC));
  BuildMI(MBB, I, dbgLoc, get(nativeAddOpc)).add(destOprnd).add(lhsOprnd).add(rhsOprnd);
}

void llvm::WDCInstrInfo::expandSUB(MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator I, TargetOpcodeTy nativeSubOpc) const {
  const auto dbgLoc = I->getDebugLoc();
  const auto destOprnd = I->getOperand(0);
  const auto lhsOprnd = I->getOperand(1);
  const auto rhsOprnd = I->getOperand(2);
  BuildMI(MBB, I, dbgLoc, get(WDC::SEC));
  BuildMI(MBB, I, dbgLoc, get(nativeSubOpc)).add(destOprnd).add(lhsOprnd).add(rhsOprnd);
}

void llvm::WDCInstrInfo::expandRTL(MachineBasicBlock &MBB,
                                   MachineBasicBlock::iterator I) const {
  BuildMI(MBB, I, I->getDebugLoc(), get(WDC::RTL));
}

void llvm::WDCInstrInfo::expandSetM(MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const {
  const auto dbgLoc = I->getDebugLoc();
  const auto flagOprnd = I->getOperand(1);
  const auto flagVal = flagOprnd.getImm();
  if (flagVal) {
    BuildMI(MBB, I, dbgLoc, get(WDC::SEP)).add(I->getOperand(0)).addImm(0b00100000);
  }
  else {
    BuildMI(MBB, I, dbgLoc, get(WDC::REP)).add(I->getOperand(0)).addImm(0b00100000);
  }
}

void llvm::WDCInstrInfo::expandSRA(MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const {
  BuildMI(MBB, I, I->getDebugLoc(), get(WDC::CMPi)).addReg(WDC::P).addReg(WDC::C).addImm(0x8000);
  BuildMI(MBB, I, I->getDebugLoc(), get(WDC::ROR));
}

void llvm::WDCInstrInfo::expandROTL(MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const {
  const auto resOprnd = I->getOperand(0);
  assert(resOprnd.isReg() && "Expected register operand for ROTL result");
  const auto resReg = resOprnd.getReg();
  const auto srcOprnd = I->getOperand(1);
  assert(srcOprnd.isReg() && "Expected register operand for ROTL source");
  const auto srcReg = srcOprnd.getReg();
  BuildMI(MBB, I, I->getDebugLoc(), get(WDC::ASL), resReg).addReg(srcReg);
  BuildMI(MBB, I, I->getDebugLoc(), get(WDC::ADCi), resReg).addReg(srcReg).addImm(0);
}

void llvm::WDCInstrInfo::expandTCA(MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const {
  BuildMI(MBB, I, I->getDebugLoc(), get(WDC::ROL));
}

void llvm::WDCInstrInfo::expandLEA(MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const {
  const auto dbgLoc = I->getDebugLoc();
  BuildMI(MBB, I, dbgLoc, get(WDC::TSC));
  BuildMI(MBB, I, dbgLoc, get(WDC::CLC));
  const auto frmIdxOprnd = I->getOperand(1);
  BuildMI(MBB, I, dbgLoc, get(WDC::ADCi)).addReg(WDC::A).addReg(WDC::A).add(frmIdxOprnd);
}

void llvm::WDCInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator MI,
                                     const DebugLoc &DL, const Register DestReg,
                                     const Register SrcReg, bool KillSrc,
                                     bool RenamableDest,
                                     bool RenamableSrc) const {
  const auto & dbgLoc = MI->getDebugLoc();
  if (SrcReg == WDC::P) {
    // push 8-bit status register on to the stack
    BuildMI(MBB, MI, dbgLoc, get(WDC::PHP));
    if (DestReg == WDC::C) {
      // Set the Accumulator to 8 bits; pull the byte off the stack, and reset the accumulator back to 16 bit.
      BuildMI(MBB, MI, dbgLoc, get(WDC::SEP), WDC::P).addImm(0b00100000);
      BuildMI(MBB, MI, dbgLoc, get(WDC::PLA));
      BuildMI(MBB, MI, dbgLoc, get(WDC::REP), WDC::P).addImm(0b00100000);
      return;
    }
  }
  
  if (SrcReg == WDC::C) {
    if (DestReg == WDC::X) {
      BuildMI(MBB, MI, dbgLoc, get(WDC::TAX));
      return;
    }
    if (DestReg == WDC::Y) {
      BuildMI(MBB, MI, dbgLoc, get(WDC::TAY));
      return;
    }
  }

  if (SrcReg == WDC::X) {
    if (DestReg == WDC::C) {
      BuildMI(MBB, MI, dbgLoc, get(WDC::TXA));
      return;
    }
  }

  if (SrcReg == WDC::Y) {
    if (DestReg == WDC::C) {
      BuildMI(MBB, MI, dbgLoc, get(WDC::TYA));
      return;
    }
  }

  llvm_unreachable("I didn't implement the rightTargetInstrInfo::copyPhysReg!");

}
