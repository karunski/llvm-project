add_llvm_component_library(LLVMWDCDesc
  WDCMCTargetDesc.cpp
  WDCABIInfo.cpp
  WDCMCCodeEmitter.cpp
  WDCMCExpr.cpp
  WDCMCAsmBackend.cpp
  WDCMCAsmInfo.cpp
  WDCELFObjectWriter.cpp

  LINK_COMPONENTS
  MC
  WDCInfo
  Support
  WDCAsmPrinter

  ADD_TO_COMPONENT
  WDC
  )