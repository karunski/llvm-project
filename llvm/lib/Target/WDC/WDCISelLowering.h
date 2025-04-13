//===-- WDCISelLowering.h - WDC DAG Lowering Interface --------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_WDC_WDCISELLOWERING_H
#define LLVM_LIB_TARGET_WDC_WDCISELLOWERING_H

#include "WDCConfig.h"

#include "MCTargetDesc/WDCABIInfo.h"
#include "WDC.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/IR/Function.h"
#include "llvm/CodeGen/TargetLowering.h"
#include <deque>

namespace llvm {
  namespace WDCISD {
    enum NodeType {
      // Start the numbering from where ISD NodeType finishes.
      FIRST_NUMBER = ISD::BUILTIN_OP_END,

      // Jump and link (call)
      JmpLink,

      // Tail call
      TailCall,

      // Get the Higher 16 bits from a 32-bit immediate
      // No relation with Cpu0 Hi register
      Hi,
      // Get the Lower 16 bits from a 32-bit immediate
      // No relation with Cpu0 Lo register
      Lo,

      // Handle gp_rel (small data/bss sections) relocation.
      GPRel,

      // Thread Pointer
      ThreadPointer,

      // Return
      Ret,

      EH_RETURN,

      // DivRem(u)
      DivRem,
      DivRemU,

      Wrapper,
      DynAlloc,

      Sync,

      ADCi,
      ADCsr,
      ADDsr,
      ADD,
      ADDi,
      ANDsr,
      EORsr,
      SETCCsr,
      SUB,
      SBC,
      ORAsr,
      LDAdpil,
      LDAi,
      LEA,
      SEC,
      STA,
      SETM,
      CLC,
    };
  }

  //===--------------------------------------------------------------------===//
  // TargetLowering Implementation
  //===--------------------------------------------------------------------===//
  class WDCFunctionInfo;
  class WDCSubtarget;

  //@class WDCTargetLowering
  class WDCTargetLowering : public TargetLowering  {
  private:
    explicit WDCTargetLowering(const WDCTargetMachine &TM,
                                const WDCSubtarget &STI);
  public:
    static std::unique_ptr<const WDCTargetLowering> create(const WDCTargetMachine &TM,
                                            const WDCSubtarget &STI);

    /// getTargetNodeName - This method returns the name of a target specific
    //  DAG node.
    const char *getTargetNodeName(unsigned Opcode) const override;

  protected:

    /// ByValArgInfo - Byval argument information.
    struct ByValArgInfo {
      unsigned FirstIdx; // Index of the first register used.
      unsigned NumRegs;  // Number of registers used for this argument.
      unsigned Address;  // Offset of the stack area used to pass this argument.

      ByValArgInfo() : FirstIdx(0), NumRegs(0), Address(0) {}
    };

    // Subtarget Info
    const WDCSubtarget &Subtarget;
    // Cache the ABI from the TargetMachine, we use it everywhere.
    const WDCABIInfo &ABI;

    class WDCCallingConvention {
    public:
      enum SpecialCallingConvType {
        NoSpecialCallingConv
      };

      WDCCallingConvention(CallingConv::ID CallConv, CCState &Info,
             SpecialCallingConvType SpecialCallingConv = NoSpecialCallingConv);

      void analyzeCallResult(const SmallVectorImpl<ISD::InputArg> &Ins,
                             bool IsSoftFloat, const SDNode *CallNode,
                             const Type *RetTy) const;

      void analyzeReturn(const SmallVectorImpl<ISD::OutputArg> &Outs,
                         bool IsSoftFloat, const Type *RetTy) const;

      const CCState &getCCInfo() const { return CCInfo; }

      /// hasByValArg - Returns true if function has byval arguments.
      bool hasByValArg() const { return !ByValArgs.empty(); }

      /// reservedArgArea - The size of the area the caller reserves for
      /// register arguments. This is 16-byte if ABI is O32.
      //unsigned reservedArgArea() const;

      typedef SmallVectorImpl<ByValArgInfo>::const_iterator byval_iterator;
      byval_iterator byval_begin() const { return ByValArgs.begin(); }
      byval_iterator byval_end() const { return ByValArgs.end(); }

    private:

      /// Return the type of the register which is used to pass an argument or
      /// return a value. This function returns f64 if the argument is an i64
      /// value which has been generated as a result of softening an f128 value.
      /// Otherwise, it just returns VT.
      // MVT getRegVT(MVT VT, bool IsSoftFloat) const;

      template<typename Ty>
      void analyzeReturn(const SmallVectorImpl<Ty> &RetVals, bool IsSoftFloat,
                         const SDNode *CallNode, const Type *RetTy) const;

      CCState &CCInfo;
      CallingConv::ID CallConv;
      SmallVector<ByValArgInfo, 2> ByValArgs;
    };

  private:

    // Lower Operand specifics
    SDValue lowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;

	//- must be exist even without function all
    SDValue
      LowerFormalArguments(SDValue Chain,
                           CallingConv::ID CallConv, bool IsVarArg,
                           const SmallVectorImpl<ISD::InputArg> &Ins,
                           const SDLoc &dl, SelectionDAG &DAG,
                           SmallVectorImpl<SDValue> &InVals) const override;

    SDValue LowerReturn(SDValue Chain,
                        CallingConv::ID CallConv, bool IsVarArg,
                        const SmallVectorImpl<ISD::OutputArg> &Outs,
                        const SmallVectorImpl<SDValue> &OutVals,
                        const SDLoc &dl, SelectionDAG &DAG) const override;

    SDValue LowerOperation(SDValue node, SelectionDAG &DAG) const override;

    SDValue LowerGlobalAddress(GlobalAddressSDNode *glblAddrNd, const SDLoc & dbgLoc,
                               SelectionDAG &DAG) const;

    SDValue LowerConstant(SDValue csntNd, const SDLoc &dbgLoc,
                          SelectionDAG &DAG) const;

    SDValue LowerLoad(LoadSDNode *ldNd, const SDLoc &dbgLoc,
                      SelectionDAG &DAG) const;

    SDValue LowerStore(StoreSDNode *stNd, const SDLoc &dbgLoc,
                       SelectionDAG &DAG) const;

    SDValue LowerAdd(SDValue node, const SDLoc &debugLoc,
                     SelectionDAG &DAG) const;

    SDValue LowerSub(SDValue node, const SDLoc &debugLoc,
                     SelectionDAG &DAG) const;

    SDValue LowerStackRelativeOperand(SDValue node, SelectionDAG &DAG, WDCISD::NodeType wdcNodeType) const;

    SDValue ExpandShift(SDValue node, SelectionDAG &DAG, unsigned targetOpcode) const;

    // SDValue PerformDAGCombine(SDNode *N, DAGCombinerInfo &DCI) const override;

    MVT getScalarShiftAmountTy(const DataLayout &, EVT) const override;

    EVT getSetCCResultType(const DataLayout &DL, LLVMContext &Context,
                           EVT VT) const override;

    bool convertSetCCLogicToBitwiseLogic(EVT vt) const override;
  };
}

#endif // WDCISELLOWERING_H