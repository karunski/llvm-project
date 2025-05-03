//===-- WDCISelLowering.cpp - WDC DAG Lowering Implementation -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that WDC uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//
#include "WDCISelLowering.h"

#include "WDCMachineFunction.h"
#include "WDCTargetMachine.h"
#include "WDCTargetObjectFile.h"
#include "WDCSubtarget.h"
#include "WDCRegisterInfo.h"
#include "MCTargetDesc/WDCBaseInfo.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "WDC-lower"

static cl::opt<bool>
EnableWDCTailCalls("enable-wdc-tail-calls", cl::Hidden,
                    cl::desc("WDC: Enable tail calls."), cl::init(false));

//@3_1 1 {
const char *WDCTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  case WDCISD::JmpLink:           return "WDCISD::JmpLink";
  case WDCISD::TailCall:          return "WDCISD::TailCall";
  case WDCISD::Hi:                return "WDCISD::Hi";
  case WDCISD::Lo:                return "WDCISD::Lo";
  case WDCISD::GPRel:             return "WDCISD::GPRel";
  case WDCISD::Ret:               return "WDCISD::Ret";
  case WDCISD::EH_RETURN:         return "WDCISD::EH_RETURN";
  case WDCISD::DivRem:            return "WDCISD::DivRem";
  case WDCISD::DivRemU:           return "WDCISD::DivRemU";
  case WDCISD::Wrapper:           return "WDCISD::Wrapper";
  case WDCISD::ADCi:              return "WDCISD::ADCi";
  case WDCISD::ADCsr:             return "WDCISD::ADCsr";
  case WDCISD::ADD:               return "WDCISD::ADD";
  case WDCISD::ADDsr:             return "WDCISD::ADDsr";
  case WDCISD::AND:               return "WDCISD::AND";
  case WDCISD::EOR:               return "WDCISD::EOR";
  case WDCISD::SETCC:             return "WDCISD::SETCC";
  case WDCISD::SBC:               return "WDCISD::SBC";
  case WDCISD::SUB:               return "WDCISD::SUB";
  case WDCISD::ORA:               return "WDCISD::ORA";
  case WDCISD::LDAdpil:           return "WDCISD::LDAdpil";
  case WDCISD::LDAi:              return "WDCISD::LDAi";
  case WDCISD::LEA:               return "WDCISD::LEA";
  case WDCISD::SEC:               return "WDCISD::SEC";
  case WDCISD::STA:               return "WDCISD::STA";
  case WDCISD::SETM:              return "WDCISD::SETM";
  case WDCISD::CLC:               return "WDCISD::CLC";
  case WDCISD::SEXT:              return "WDCISD::SEXT";
  default:                        return NULL;
  }
}
//@3_1 1 }

//@WDCTargetLowering {
WDCTargetLowering::WDCTargetLowering(const WDCTargetMachine &TM,
                                     const WDCSubtarget &STI)
    : TargetLowering{TM}, Subtarget{STI}, ABI{TM.getABI()} {
  //@WDCSETargetLowering body {
  // Set up the register classes
  addRegisterClass(MVT::i16, &WDC::RegsA16RegClass);
  addRegisterClass(MVT::i8, &WDC::RegsA8RegClass);
  addRegisterClass(MVT::i16, &WDC::IndexRegsRegClass);
  addRegisterClass(MVT::i8, &WDC::StatusRegRegClass);
  addRegisterClass(MVT::i32, &WDC::FakeRegsRegClass);

  setBooleanContents(TargetLowering::ZeroOrOneBooleanContent);

  setOperationAction(ISD::ADD,  MVT::i16, LegalizeAction::Custom);
  setOperationAction(ISD::AND,  MVT::i16, LegalizeAction::Custom);
  setOperationAction(ISD::OR,   MVT::i16, LegalizeAction::Custom);
  setOperationAction(ISD::SUB,  MVT::i16, LegalizeAction::Custom);
  setOperationAction(ISD::ROTL, MVT::i16, LegalizeAction::Custom);
  setOperationAction(ISD::SHL,  MVT::i16, LegalizeAction::Custom);
  setOperationAction(ISD::SRA,  MVT::i16, LegalizeAction::Custom);
  setOperationAction(ISD::SRL,  MVT::i16, LegalizeAction::Custom);
  setOperationAction(ISD::XOR,  MVT::i16, LegalizeAction::Custom);
  setOperationAction(ISD::SETCC, MVT::i16, LegalizeAction::Custom);
  setOperationAction(ISD::GlobalAddress, MVT::i32, LegalizeAction::Custom);
  setOperationAction(ISD::STORE, MVT::i32, LegalizeAction::Custom);
  setLoadExtAction(ISD::LoadExtType::SEXTLOAD, MVT::i16, MVT::i8, LegalizeAction::Expand);

  // setOperationAction(ISD::STORE, MVT::i8, LegalizeAction::Custom);
  //setOperationAction({ISD::Constant}, {MVT::i8,MVT::i16}, LegalizeAction::Custom);

  // setTargetDAGCombine({ISD::LOAD});

  // Only SETEQ and SETGE have direct corresponding results after a CMP instruction (Z, and C, respectively)
  // The rest of the operations can be implemented in terms of these two comparisons:
  // Expands ( a != b ) -> !(a == b), 
  //         ( a < b  ) -> !(a >= b),
  //         ( a > b  ) ->  (b <  a)  -> !(b >= a)
  //         ( a <= b ) ->  (b >= a)
  setCondCodeAction({ISD::SETNE, ISD::SETLE, ISD::SETGT, ISD::SETLT}, MVT::i16, LegalizeAction::Expand); 
  

  // must, computeRegisterProperties - Once all of the register classes are
  //  added, this allows us to compute derived properties we expose.
  computeRegisterProperties(Subtarget.getRegisterInfo());
}

std::unique_ptr<const WDCTargetLowering> WDCTargetLowering::create(const WDCTargetMachine &TM,
                                                     const WDCSubtarget &STI) {
  return std::unique_ptr<const WDCTargetLowering>{new WDCTargetLowering{TM, STI}};
}

//===----------------------------------------------------------------------===//
//  Lower helper functions
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Misc Lower Operation implementation
//===----------------------------------------------------------------------===//

#include "WDCGenCallingConv.inc"

//===----------------------------------------------------------------------===//
//@            Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

//@LowerFormalArguments {f
/// LowerFormalArguments - transform physical registers into virtual registers
/// and generate load operations for arguments places on the stack.
SDValue
WDCTargetLowering::LowerFormalArguments(SDValue Chain,
                                         CallingConv::ID CallConv,
                                         bool IsVarArg,
                                         const SmallVectorImpl<ISD::InputArg> &Ins,
                                         const SDLoc &DL, SelectionDAG &DAG,
                                         SmallVectorImpl<SDValue> &InVals)
                                          const {

  return Chain;
}
// @LowerFormalArguments }

//===----------------------------------------------------------------------===//
//@              Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//
template <typename Ty>
void WDCTargetLowering::WDCCallingConvention::analyzeReturn(
    const SmallVectorImpl<Ty> &RetVals, bool IsSoftFloat,
    const SDNode *CallNode, const Type *RetTy) const {
  CCAssignFn *Fn = RetCC_WDC;

  for (unsigned I = 0, E = RetVals.size(); I < E; ++I) {
    MVT VT = RetVals[I].VT;
    ISD::ArgFlagsTy Flags = RetVals[I].Flags;
    MVT RegVT = VT;//this->getRegVT(VT, IsSoftFloat);

    if (Fn(I, VT, RegVT, CCValAssign::Full, Flags, this->CCInfo)) {
#ifndef NDEBUG
      dbgs() << "Call result #" << I << " has unhandled type "
             << EVT(VT).getEVTString() << '\n';
#endif
      llvm_unreachable(nullptr);
    }
  }
}

SDValue
WDCTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                               bool IsVarArg,
                               const SmallVectorImpl<ISD::OutputArg> &Outs,
                               const SmallVectorImpl<SDValue> &OutVals,
                               const SDLoc &DL, SelectionDAG &DAG) const {
  // CCValAssign - represent the assignment of
  // the return value to a location
  SmallVector<CCValAssign, 16> RVLocs;
  MachineFunction &MF = DAG.getMachineFunction();
  auto & frameInfo = MF.getFrameInfo();

  // CCState - Info about the registers and stack slot.
  CCState CCInfo{CallConv, IsVarArg, MF, RVLocs, *DAG.getContext()};
  WDCCallingConvention callingConventionInfo{CallConv, CCInfo};

  // Analyze return values.
  callingConventionInfo.analyzeReturn(Outs, Subtarget.abiUsesSoftFloat(),
                                      MF.getFunction().getReturnType());

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps{1, Chain};

  // Copy the result values into the output stack
  for (size_t i = 0; i != RVLocs.size(); ++i) {
    SDValue Val = OutVals[i];
    CCValAssign &VA = RVLocs[i];
    // if (VA.isRegLoc()) {
    //   if (RVLocs[i].getValVT() != RVLocs[i].getLocVT())
    //     Val = DAG.getNode(ISD::BITCAST, DL, RVLocs[i].getLocVT(), Val);

    //   Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), Val, Flag);

    //   // Guarantee that all emitted copies are stuck together with flags.
    //   Flag = Chain.getValue(1);
    //   RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
    // }
    // else {
      assert(VA.isMemLoc() && "return value should be is memory location");
      if (IsVarArg) {
        report_fatal_error("Can't return value from vararg function in memory");
      }

      const auto offset = VA.getLocMemOffset();
      const auto objSize = VA.getLocVT().getStoreSize();
      // Create the frame index object for the memory location.
      const auto frameIndex = frameInfo.CreateFixedObject(objSize, offset, false);

      // Create a SelectionDAG node corresponding to a store
      // to this memory location.
      SDValue frameIndexNode = DAG.getFrameIndex(frameIndex, MVT::i32);
      // MemOpChains.push_back(DAG.getStore(
      //     Chain, dl, OutVals[i], FIN,
      //     MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), FI)));
      Chain = DAG.getStore(Chain, DL, Val, frameIndexNode, MachinePointerInfo::getStack(MF, 0));
    // }
  }

//@Ordinary struct type: 2 {
  // The cpu0 ABIs for returning structs by value requires that we copy
  // the sret argument into $v0 for the return. We saved the argument into
  // a virtual register in the entry block, so now we copy the value out
  // and into $v0.
  // if (MF.getFunction().hasStructRetAttr()) {
  //   Cpu0FunctionInfo *Cpu0FI = MF.getInfo<Cpu0FunctionInfo>();
  //   unsigned Reg = Cpu0FI->getSRetReturnReg();

  //   if (!Reg)
  //     llvm_unreachable("sret virtual register not created in the entry block");
  //   SDValue Val =
  //       DAG.getCopyFromReg(Chain, DL, Reg, getPointerTy(DAG.getDataLayout()));
  //   unsigned V0 = Cpu0::V0;

  //   Chain = DAG.getCopyToReg(Chain, DL, V0, Val, Flag);
  //   Flag = Chain.getValue(1);
  //   RetOps.push_back(DAG.getRegister(V0, getPointerTy(DAG.getDataLayout())));
  // }
//@Ordinary struct type: 2 }

  RetOps[0] = Chain;  // Update chain.

  // Add the flag if we have it.
  if (Flag.getNode()) {
    RetOps.push_back(Flag);
  }

  // Return on Cpu0 is always a "ret $lr"
  return DAG.getNode(WDCISD::Ret, DL, MVT::Other, RetOps);
}

static SDValue TryFoldGlobalAddressOperand(SDValue node, bool commutative, const SDLoc & debugLoc, SelectionDAG & dag, WDCISD::NodeType wdcNode) {
  auto lhsOprndNo = 0u;
  auto rhsOprndNo = 1u;

  while (true) {
    const auto rhsOperand = node.getOperand(rhsOprndNo);
    if (const auto loadNode = dyn_cast<LoadSDNode>(rhsOperand.getNode()); loadNode) {
      const auto loadBasePtrValue = loadNode->getBasePtr();
      if (const auto globalAddrNode = dyn_cast<GlobalAddressSDNode>(loadBasePtrValue.getNode()); globalAddrNode) {
        return dag.getNode(wdcNode, debugLoc, {node.getValueType(), MVT::Other},
                           {loadNode->getChain(), node.getOperand(lhsOprndNo),
                            loadBasePtrValue});
      }
    }

    if (commutative && lhsOprndNo == 0u) {
      // Swap the operands and try again.
      std::swap(lhsOprndNo, rhsOprndNo);
      continue;
    }

    return {};
  }

  return {};
}

SDValue llvm::WDCTargetLowering::LowerAdd(SDValue node, const SDLoc & debugLoc, SelectionDAG & DAG) const {
  SDValue lhs = node.getOperand(0);
  SDValue rhs = node.getOperand(1);

  const auto rhsNdTy = static_cast<ISD::NodeType>(rhs.getOpcode());

  if (const auto loweredToStackRel = LowerStackRelativeOperand(node, DAG, WDCISD::ADD); loweredToStackRel) {
    return loweredToStackRel;
  }

  if (rhsNdTy == ISD::Constant) {
    return DAG.getNode(WDCISD::ADD, debugLoc, {node.getValueType()}, {DAG.getEntryNode(), lhs, rhs});
  }

  if (const auto loweredToAbsLong = TryFoldGlobalAddressOperand(node, true, debugLoc, DAG, WDCISD::ADD); loweredToAbsLong) {
    return loweredToAbsLong;
  }

  // 65816 doesn't have any instructions that take registers as operands.
  // Any binary op that has two non-memory operands isn't supported
  // Store the rhs operand's value to a stack slot, then insert a 'load' from that slot as the new operand, then try again.
  const auto tempVal = DAG.CreateStackTemporary(node.getValueType());
  const auto frameIndexNode = dyn_cast<FrameIndexSDNode>(tempVal.getNode());
  const auto pointerInfo = MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), frameIndexNode->getIndex());
  const auto tempStore = DAG.getStore(DAG.getEntryNode(), debugLoc, node.getOperand(1), tempVal, pointerInfo);
  const auto tempLoad = DAG.getLoad(node.getValueType(), debugLoc, tempStore, tempVal, pointerInfo);
  return DAG.getNode(ISD::ADD, debugLoc, {node.getValueType()}, {lhs, tempLoad});
}

SDValue
llvm::WDCTargetLowering::LowerLogic(SDValue node, const SDLoc &debugLoc,
                                    SelectionDAG &DAG, ISD::NodeType isdType,
                                    WDCISD::NodeType wdcNodeType) const {
  SDValue lhs = node.getOperand(0);
  SDValue rhs = node.getOperand(1);

  const auto rhsNdTy = static_cast<ISD::NodeType>(rhs.getOpcode());

  if (const auto loweredToStackRel =
          LowerStackRelativeOperand(node, DAG, wdcNodeType);
      loweredToStackRel) {
    return loweredToStackRel;
  }

  if (rhsNdTy == ISD::Constant) {
    return DAG.getNode(wdcNodeType, debugLoc, {node.getValueType()},
                       {DAG.getEntryNode(), lhs, rhs});
  }

  if (const auto loweredToAbsLong =
          TryFoldGlobalAddressOperand(node, true, debugLoc, DAG, wdcNodeType);
      loweredToAbsLong) {
    return loweredToAbsLong;
  }

  // 65816 doesn't have any instructions that take registers as operands.
  // Any binary op that has two non-memory operands isn't supported
  // Store the rhs operand's value to a stack slot, then insert a 'load' from
  // that slot as the new operand, then try again.
  const auto tempVal = DAG.CreateStackTemporary(node.getValueType());
  const auto frameIndexNode = dyn_cast<FrameIndexSDNode>(tempVal.getNode());
  const auto pointerInfo = MachinePointerInfo::getFixedStack(
      DAG.getMachineFunction(), frameIndexNode->getIndex());
  const auto tempStore = DAG.getStore(DAG.getEntryNode(), debugLoc,
                                      node.getOperand(1), tempVal, pointerInfo);
  const auto tempLoad = DAG.getLoad(node.getValueType(), debugLoc, tempStore,
                                    tempVal, pointerInfo);
  return DAG.getNode(isdType, debugLoc, {node.getValueType()}, {lhs, tempLoad});
}

SDValue llvm::WDCTargetLowering::LowerSub(SDValue node, const SDLoc & debugLoc, SelectionDAG & DAG) const {
  if (const auto loweredToStackRel = LowerStackRelativeOperand(node, DAG, WDCISD::SUB); loweredToStackRel) {
    return loweredToStackRel;
  }

  if (const auto loweredToAbsLong = TryFoldGlobalAddressOperand(node, false, debugLoc, DAG, WDCISD::SUB); loweredToAbsLong) {
    return loweredToAbsLong;
  }

  return {};
}

SDValue llvm::WDCTargetLowering::LowerStackRelativeOperand(SDValue node, SelectionDAG & DAG, WDCISD::NodeType wdcNode) const {
  return LowerStackRelativeOperand(node, 1, DAG, wdcNode);
}

SDValue llvm::WDCTargetLowering::LowerStackRelativeOperand(SDValue node, unsigned oprndNo, SelectionDAG & DAG, WDCISD::NodeType wdcNode) const {
  const SDLoc debugLoc{node};

  // All instructions are effectively using Accumulator as the first operand, and Memory as the second.
  // Therefore, we have to try to fold the load for the memory operand into the instruction if possible.
  // The resulting wdcNode must take a chain operand, which will be assigned to whatever the load was chained to.
  const auto rhsOperand = node.getOperand(oprndNo);
  if (const auto loadNode = dyn_cast<LoadSDNode>(rhsOperand.getNode()); loadNode) {
    const auto loadBasePtrValue = loadNode->getBasePtr();
    if (const auto frameIndexNode = dyn_cast<FrameIndexSDNode>(loadBasePtrValue.getNode()); frameIndexNode) {
      SmallVector<SDValue> newOperands;
      newOperands.push_back(loadNode->getChain()); // First operand is the chain from the load that is folding
      newOperands.push_back(node.getOperand(oprndNo == 1 ? 0 : 1)); // Second operand is the accumulator register.
      newOperands.push_back(loadBasePtrValue); // The instruction will directly load from this frame index base value.
                                                  // This effectively replaces operand 1.
      for (unsigned opNum = 2; opNum < node.getNumOperands(); opNum += 1) {// transfer any remaining operands into the new node
        newOperands.push_back(node.getOperand(opNum));
      }
      return DAG.getNode(wdcNode, debugLoc, {node.getValueType(), MVT::Other}, newOperands);
    }
  }
  return {};
}



SDValue llvm::WDCTargetLowering::ExpandShift(SDValue node, SelectionDAG & DAG, unsigned targetOpcode) const {  
  if (const auto shiftAmtNode = dyn_cast<ConstantSDNode>(node.getOperand(1).getNode()); shiftAmtNode) {
    const auto debugLoc = SDLoc{node};
    const auto srcNd = node.getOperand(0);
    const auto shiftAmt = shiftAmtNode->getZExtValue();
    const auto valueTy = node.getValueType();

    if (shiftAmt >= 8) {
      // Use a swap to get an 8-bit shift or rotate.
      const auto swapNd = DAG.getNode(ISD::BSWAP, debugLoc, valueTy, srcNd);
      const auto maskedNd = [swapNd, targetOpcode, &DAG, &debugLoc, valueTy]() {
        unsigned mask = 0xFFFF;

        switch(targetOpcode) {
          default:
            assert(false && "Unhandled opcode for expanding shift operation.");
            [[fallthrough]];
          case WDC::ROTL:
            return swapNd; // don't mask anything for a rotate.
          case WDC::ASL:
            mask = 0xFF00u;
            break;
          case WDC::LSR:
          case WDC::SRA:
            mask = 0x00FFu;
            break;
        }

        const auto maskValNd = DAG.getConstant(mask, debugLoc, valueTy);
        return DAG.getNode(ISD::AND, debugLoc, valueTy, {swapNd, maskValNd});
      }();
      
      if (shiftAmt == 8) { return maskedNd; }

      return DAG.getNode(
          node.getOpcode(), debugLoc, valueTy,
          {maskedNd, DAG.getConstant(shiftAmt - 8, debugLoc, MVT::i16)});
    }

    // Shift by one; which is legal for this architecture
    SDValue machineASLNode{DAG.getMachineNode(targetOpcode, debugLoc, {valueTy}, {srcNd}), 0};

    if (shiftAmt == 1) { return machineASLNode; }

    // attach another node for shiftamount - 1.  It will get lowered on the next iteration (or it will be shift by one)
    return DAG.getNode(node.getOpcode(), debugLoc, {valueTy},
                {machineASLNode, DAG.getConstant(shiftAmt - 1, debugLoc, MVT::i16)});
  }
  return TargetLowering::LowerOperation(node, DAG);
}

SDValue llvm::WDCTargetLowering::LowerOperation(SDValue node,
                                                SelectionDAG &DAG) const {
  const SDLoc dbgLoc{node};

  if (const auto opcode = static_cast<ISD::NodeType>(node.getOpcode()); opcode == ISD::ADD) {
    return LowerAdd(node, dbgLoc, DAG);
  }
  else if (opcode == ISD::SUB) {
    return LowerSub(node, dbgLoc, DAG);
  }
  else if (opcode == ISD::AND) {
    return LowerLogic(node, dbgLoc, DAG, ISD::AND, WDCISD::AND); 
  }
  else if (opcode == ISD::OR) {
    return LowerLogic(node, dbgLoc, DAG, ISD::OR, WDCISD::ORA);
  }
  else if (opcode == ISD::ROTL) {
    return ExpandShift(node, DAG, WDC::ROTL);
  }
  else if (opcode == ISD::SHL) {
    return ExpandShift(node, DAG, WDC::ASL);
  }
  else if (opcode == ISD::SRA) {
    return ExpandShift(node, DAG, WDC::SRA);
  }
  else if (opcode == ISD::SRL) {
    return ExpandShift(node, DAG, WDC::LSR);
  }
  else if (opcode == ISD::XOR) {
    return LowerLogic(node, dbgLoc, DAG, ISD::XOR, WDCISD::EOR);
  }
  else if (opcode == ISD::SETCC) {
    return LowerSetCC(node, dbgLoc, DAG);
  }
  else if (opcode == ISD::GlobalAddress) {
    return LowerGlobalAddress(cast<GlobalAddressSDNode>(node.getNode()), dbgLoc, DAG);
  }
  else if (opcode == ISD::LOAD) {
    return LowerLoad(cast<LoadSDNode>(node.getNode()), dbgLoc, DAG);
  }
  else if (opcode == ISD::STORE) {
    return LowerStore(cast<StoreSDNode>(node.getNode()), dbgLoc, DAG);
  }
  else if (opcode == ISD::Constant) {
    return LowerConstant(node, dbgLoc, DAG);
  }

  return TargetLowering::LowerOperation(node, DAG);
}

SDValue llvm::WDCTargetLowering::LowerSetCC(SDValue setCCNode, const SDLoc & dbgLoc, SelectionDAG & DAG) const {
  const auto lhsVl = setCCNode.getOperand(0);
  const auto rhsVl = setCCNode.getOperand(1);
  const auto condCodeVl = setCCNode.getOperand(2);
  if (const auto loadNode = dyn_cast<LoadSDNode>(rhsVl.getNode()); loadNode) {
    const auto loadBasePtrValue = loadNode->getBasePtr();
    if (const auto frameIndexNode = dyn_cast<FrameIndexSDNode>(loadBasePtrValue.getNode()); frameIndexNode) {
      // First operand is the chain from the load that is folding
      // Second operand is the accumulator register.
      // The instruction will directly load from this frame index base value. This effectively replaces operand 1.
      // transfer condition code into the new node.
      // The value will be implicitly extended to the word size. (not an i1 like setcc assumes)
      return DAG.getNode(
          WDCISD::SETCC, dbgLoc, {lhsVl.getValueType(), MVT::Other},
          {loadNode->getChain(), lhsVl, loadBasePtrValue, condCodeVl});
    }
  }  return SDValue{};
}

SDValue llvm::WDCTargetLowering::LowerGlobalAddress(GlobalAddressSDNode * glblAddrNd, const SDLoc & dbgLoc, SelectionDAG & DAG) const {
  const auto glblAddr = glblAddrNd->getGlobal();
  const auto addrValT = glblAddrNd->getValueType(0);
  return DAG.getNode(WDCISD::Wrapper, dbgLoc, addrValT, DAG.getTargetGlobalAddress(glblAddr, dbgLoc, addrValT));
}

static SDValue getSetMFlag(SelectionDAG & DAG, unsigned val, const SDLoc & dbgLoc) {
  return DAG.getConstant(val, dbgLoc, MVT::i1);
}

SDValue llvm::WDCTargetLowering::LowerConstant(SDValue cnstSDVal, const SDLoc & dbgLoc, SelectionDAG & DAG) const {
  const auto cnstVlTy = cnstSDVal->getValueType(0);
  const auto cnstNd = cast<ConstantSDNode>(cnstSDVal.getNode());
  if (cnstVlTy == MVT::i8) {
    const auto targetConst = DAG.getTargetConstant(cnstNd->getAPIntValue(), dbgLoc, MVT::i8);
    const auto setM = getSetMFlag(DAG, 1, dbgLoc);
    return DAG.getNode(WDCISD::LDAi, dbgLoc, MVT::i8, {targetConst, setM});
  }
  if (cnstVlTy == MVT::i16) {
    const auto targetConst = DAG.getTargetConstant(cnstNd->getAPIntValue(), dbgLoc, MVT::i16);
    const auto setM = getSetMFlag(DAG, 0, dbgLoc);
    return DAG.getNode(WDCISD::LDAi, dbgLoc, MVT::i16, {targetConst, setM});
  }

  return {};
}

SDValue llvm::WDCTargetLowering::LowerLoad(LoadSDNode * ldNd, const SDLoc & dbgLoc, SelectionDAG & DAG) const {
  const auto addr = ldNd->getBasePtr();
  const auto ldNdVlTp = ldNd->getValueType(0);
  const auto chVlTp = ldNd->getValueType(1);
  const auto addr_node_op = static_cast<ISD::NodeType>(addr.getOpcode());
  const auto chIn = ldNd->getChain();

  if (addr_node_op == ISD::LOAD) {
    const auto innerload = cast<LoadSDNode>(addr);
    const auto innerload_addr = innerload->getBasePtr();
    const auto innerload_addr_op = static_cast<ISD::NodeType>(innerload_addr.getOpcode());
    if (innerload_addr_op == ISD::FrameIndex) {
      return DAG.getNode(WDCISD::LDAdpil, dbgLoc, {ldNdVlTp, chVlTp}, {chIn, innerload_addr});
    }
  }
  return {};
}

SDValue llvm::WDCTargetLowering::LowerStore(StoreSDNode * stNd, const SDLoc& dbgLoc, SelectionDAG & DAG) const {
  const auto valNd = stNd->getValue(); // <-- the thing being stored.
  const auto valNdValTy = valNd.getValueType(); // <-- the size of the thing being stored.
  const auto addrNd = stNd->getBasePtr();
  const auto chNd = stNd->getChain();
  const auto valNdTy = static_cast<ISD::NodeType>(valNd->getOpcode());
  const auto addrTp = static_cast<ISD::NodeType>(addrNd->getOpcode());
  if (valNdTy == ISD::FrameIndex) {
    // The value is a FrameIndex, which is an address.  Currently all addresses are 24-bit.  As far as I can
    // tell llvm requires addresses to have a power-of-2 number of bits; so 32 it is.
    if (addrTp == ISD::FrameIndex) {
      // destination ptr is a FrameIndex; so we're copying the address of a stack variable into another slot on
      // the stack; for example to pass to a function that takes a parameter by reference
      // the upper 16 bits are 0 for a frameindex ptr; because the stack is only 16-bit.
      const auto destAddrNd = cast<FrameIndexSDNode>(addrNd.getNode());
      const auto zeroNd = DAG.getConstant(0, dbgLoc, MVT::i16);
      const auto destAddrPtrInfoHigh = MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), destAddrNd->getIndex(), 2);
      const auto storeHigh = DAG.getStore(chNd, dbgLoc, zeroNd, addrNd, destAddrPtrInfoHigh);
      // The lower 16 bits are the address of the stack slot
      const auto getStkReg = DAG.getNode(WDCISD::LEA, dbgLoc, MVT::i16, valNd);
      const auto destAddrPtrInfoLow = MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), destAddrNd->getIndex(), 0);
      const auto storeLow = DAG.getStore(chNd, dbgLoc, getStkReg, addrNd, destAddrPtrInfoLow);
      SmallVector<SDValue> joinedVals{storeHigh, storeLow};
      return DAG.getTokenFactor(dbgLoc, joinedVals);
    }
  }

  if (valNdTy == ISD::GlobalAddress) {
    const auto glblAddr = cast<GlobalAddressSDNode>(valNd.getNode());
    const auto glblAddrVal = glblAddr->getGlobal();
    const auto glblHigh = DAG.getTargetGlobalAddress(glblAddrVal, dbgLoc, MVT::i32, 0, WDCII::MO_ABS_HI);
    const auto constGlblHigh = DAG.getNode(WDCISD::Hi, dbgLoc, MVT::i16, glblHigh);
    const auto glblLow = DAG.getTargetGlobalAddress(glblAddrVal, dbgLoc, MVT::i32, 0, WDCII::MO_ABS_LO);
    const auto constGlblLow = DAG.getNode(WDCISD::Lo, dbgLoc, MVT::i16, glblLow);
    assert(addrTp == ISD::GlobalAddress && "Only handle store i32 to global address");
    const auto destGlblAddr = cast<GlobalAddressSDNode>(addrNd.getNode());
    const auto destGlblAddrVal = destGlblAddr->getGlobal();
    const auto targetAddrHigh = DAG.getTargetGlobalAddress(destGlblAddrVal, dbgLoc, MVT::i16, 2);
    const auto globalAddrHigh = DAG.getNode(WDCISD::Wrapper, dbgLoc, MVT::i16, targetAddrHigh);
    const auto storeHigh =
        DAG.getStore(chNd, dbgLoc, constGlblHigh, globalAddrHigh,
                     MachinePointerInfo{destGlblAddrVal, 2});
    const auto storeLow = DAG.getStore(chNd, dbgLoc, constGlblLow, addrNd,
                                       MachinePointerInfo{destGlblAddrVal});
    return DAG.getNode(ISD::TokenFactor, dbgLoc, MVT::Other, storeHigh, storeLow);
  }
  
  if (valNdValTy == MVT::i8) {
    const auto setM = getSetMFlag(DAG, 1, dbgLoc);
    return DAG.getNode(WDCISD::STA, dbgLoc, MVT::i8, {chNd, valNd, addrNd, setM});
  }

  return SDValue{};
}

// SDValue llvm::WDCTargetLowering::PerformDAGCombine(SDNode *nodeptr,
//                                                    DAGCombinerInfo &DCI) const {
//   const auto nodeop = static_cast<ISD::NodeType>(nodeptr->getOpcode());
//   if (nodeop == ISD::LOAD) {
//     const auto loadnode = cast<LoadSDNode>(nodeptr);
//     const auto addr = loadnode->getBasePtr();
//     const auto addr_node_op = static_cast<ISD::NodeType>(addr.getOpcode());
//     if (addr_node_op == ISD::LOAD) {
//       const auto innerload = cast<LoadSDNode>(addr);
//       const auto innerload_addr = innerload->getBasePtr();
//       const auto innerload_addr_op = static_cast<ISD::NodeType>(innerload_addr.getOpcode());
//       if (innerload_addr_op == ISD::FrameIndex) {
//         return DCI.CombineTo(nodeptr, )
//       }
//     }
//   }
//   return SDValue();
// }

MVT llvm::WDCTargetLowering::getScalarShiftAmountTy(const DataLayout &,
                                                    EVT evt) const {
  return MVT::i16;
}

EVT llvm::WDCTargetLowering::getSetCCResultType(const DataLayout &DL,
                                                LLVMContext &Context,
                                                EVT VT) const {
  return MVT::i16;
}

bool llvm::WDCTargetLowering::convertSetCCLogicToBitwiseLogic(EVT) const {
  return true;
}

bool llvm::WDCTargetLowering::decomposeMulByConstant(LLVMContext &Context,
                                                     EVT VT, SDValue C) const {
  return true;
}

llvm::WDCTargetLowering::WDCCallingConvention::WDCCallingConvention(
    CallingConv::ID CallConv, CCState &Info,
    SpecialCallingConvType /*SpecialCallingConv*/)
    : CCInfo{Info}, CallConv(CallConv) {
  // Pre-allocate reserved argument area.
  //CCInfo.AllocateStack(reservedArgArea(), Align(1));
}

void llvm::WDCTargetLowering::WDCCallingConvention::analyzeReturn(
    const SmallVectorImpl<ISD::OutputArg> &Outs, bool IsSoftFloat,
    const Type *RetTy) const {
  analyzeReturn(Outs, IsSoftFloat, nullptr, RetTy);
}
