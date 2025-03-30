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
  case WDCISD::ADDi:              return "WDCISD::ADDi";
  case WDCISD::ADDsr:             return "WDCISD::ADDsr";
  case WDCISD::SUBsr:             return "WDCISD::SUBsr";
  case WDCISD::SHL:              return "WDCISD::SHL";
  default:                         return NULL;
  }
}
//@3_1 1 }

//@WDCTargetLowering {
WDCTargetLowering::WDCTargetLowering(const WDCTargetMachine &TM,
                                     const WDCSubtarget &STI)
    : TargetLowering{TM}, Subtarget{STI}, ABI{TM.getABI()} {
  //@WDCSETargetLowering body {
  // Set up the register classes
  addRegisterClass(MVT::i16, &WDC::AccumulatorRegisterClassRegClass);
  addRegisterClass(MVT::i16, &WDC::IndexRegsRegClass);

  // must, computeRegisterProperties - Once all of the register classes are
  //  added, this allows us to compute derived properties we expose.
  computeRegisterProperties(Subtarget.getRegisterInfo());

  setOperationAction(ISD::ADD, MVT::i16, LegalizeAction::Custom);
  setOperationAction(ISD::SUB, MVT::i16, LegalizeAction::Custom);
  setOperationAction(ISD::SHL, MVT::i16, LegalizeAction::Custom);
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

//@LowerFormalArguments {
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
      SDValue frameIndexNode = DAG.getFrameIndex(frameIndex, MVT::i16);
      // MemOpChains.push_back(DAG.getStore(
      //     Chain, dl, OutVals[i], FIN,
      //     MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), FI)));
      Chain = DAG.getStore(Chain, DL, Val, frameIndexNode, MachinePointerInfo::getStack(MF, frameIndex));
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

SDValue llvm::WDCTargetLowering::LowerAdd(SDValue node, SelectionDAG & DAG) const {
  const SDLoc debugLoc{node};

  const SDValue operands[] = {node.getOperand(0), node.getOperand(1) };
  //const EVT types[] = {operands[0].getValueType(), operands[1].getValueType()};
  const SDNode * nodes[] = {operands[0].getNode(), operands[1].getNode()};
  const unsigned nodesOpcodes[] = {nodes[0]->getOpcode(), nodes[1]->getOpcode()};
  if (nodesOpcodes[1] == ISD::LOAD) {
    const auto loadNode = nodes[1];
    const SDValue loadOperands[] = {loadNode->getOperand(0), loadNode->getOperand(1), loadNode->getOperand(2)};
    const SDNode * loadOperandNodes[] = {loadOperands[0].getNode(), loadOperands[1].getNode(), loadOperands[2].getNode()};
    // const unsigned loadOperandOpcodes[] = {loadOperandNodes[0]->getOpcode(), loadOperandNodes[1]->getOpcode()};
    if (const auto frameIndexNode = dyn_cast<FrameIndexSDNode>(loadOperandNodes[1]); frameIndexNode) {
      return DAG.getNode(
          WDCISD::ADDsr, debugLoc, {node.getValueType()},
          {loadOperands[0], operands[0], loadOperands[1]});
    }
  }
  else if (nodesOpcodes[1] == ISD::Constant) {
    return DAG.getNode(WDCISD::ADDi, debugLoc, {node.getValueType()},
                        {operands[0], operands[1]});
  }
  return TargetLowering::LowerOperation(node, DAG);
}

SDValue llvm::WDCTargetLowering::LowerSub(SDValue node, SelectionDAG & DAG) const {
  const SDLoc debugLoc{node};

  const auto subtractorOperand = node.getOperand(1);
  if (const auto loadNode = dyn_cast<LoadSDNode>(subtractorOperand.getNode()); loadNode) {
    const auto loadBasePtrValue = loadNode->getBasePtr();
    if (const auto frameIndexNode = dyn_cast<FrameIndexSDNode>(loadBasePtrValue.getNode()); frameIndexNode) {
      return DAG.getNode(WDCISD::SUBsr, debugLoc, {node.getValueType()}, {loadNode->getChain(), node.getOperand(0), loadBasePtrValue});
    }
  }
  return TargetLowering::LowerOperation(node, DAG);
}

SDValue llvm::WDCTargetLowering::LowerShl(SDValue node, SelectionDAG & DAG) const {
  const SDLoc debugLoc{node};
  const auto shiftAmount = node.getOperand(1);
  if (const auto shiftAmtNode = dyn_cast<ConstantSDNode>(shiftAmount.getNode()); shiftAmtNode) {
    return DAG.getNode(WDCISD::SHL, debugLoc, {node.getValueType()}, {node.getOperand(0), shiftAmount});
  }
  return TargetLowering::LowerOperation(node, DAG);
}

SDValue llvm::WDCTargetLowering::LowerOperation(SDValue node,
                                                SelectionDAG &DAG) const {
  if (const auto opcode = node.getOpcode(); opcode == ISD::ADD) {
    return LowerAdd(node, DAG);
  }
  else if (opcode == ISD::SUB) {
    return LowerSub(node, DAG);
  }
  else if (opcode == ISD::SHL) {
    return LowerShl(node, DAG);
  }

  return TargetLowering::LowerOperation(node, DAG);
}

SDValue llvm::WDCTargetLowering::PerformDAGCombine(SDNode *nodeptr,
                                                   DAGCombinerInfo &DCI) const {
  return SDValue();
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
