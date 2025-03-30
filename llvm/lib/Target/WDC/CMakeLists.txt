add_llvm_component_group(WDC)

set(LLVM_TARGET_DEFINITIONS WDCOther.td)

# Generate WDCGenRegisterInfo.inc and WDCGenInstrInfo.inc which included by 
#  your hand code C++ files. 
# WDCGenRegisterInfo.inc came from WDCRegisterInfo.td, WDCGenInstrInfo.inc 
#  came from WDCInstrInfo.td.
tablegen(LLVM WDCGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM WDCGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM WDCGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM WDCGenMCPseudoLowering.inc -gen-pseudo-lowering)
tablegen(LLVM WDCGenCallingConv.inc -gen-callingconv)
tablegen(LLVM WDCGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM WDCGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM WDCGenCodeEmitter.inc -gen-emitter)


# WDCCommonTableGen must be defined
add_public_tablegen_target(WDCCommonTableGen)

# WDCCodeGen should match with LLVMBuild.txt WDCCodeGen
add_llvm_target(WDCCodeGen
  WDCTargetMachine.cpp
  WDCSubtarget.cpp
  WDCFrameLowering.cpp
  WDCInstructionInfo.cpp
  WDCISelLowering.cpp
  WDCRegisterInfo.cpp
  WDCTargetObjectFile.cpp
  WDCISelDAGToDAG.cpp
  WDCSEISelDAGToDAG.cpp
  WDCMCInstLower.cpp
  WDCSEInstructionInfo.cpp
  WDCSEISelLowering.cpp
  WDCMCAsmInfo.cpp
  WDCAsmPrinter.cpp
  WDCMachineFunction.cpp

  LINK_COMPONENTS
  Analysis
  AsmPrinter
  CodeGen
  Core
  MC
  WDCDesc
  WDCInfo
  WDCAsmPrinter
  SelectionDAG
  Support
  Target
  GlobalISel

  ADD_TO_COMPONENT
  WDC
  )

# Should match with "subdirectories =  MCTargetDesc TargetInfo" in LLVMBuild.txt
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
add_subdirectory(InstPrinter)