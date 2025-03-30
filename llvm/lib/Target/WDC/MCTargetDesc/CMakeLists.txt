add_llvm_component_library(LLVMWDCDesc
  WDCMCTargetDesc.cpp
  WDCABIInfo.cpp

  LINK_COMPONENTS
  MC
  WDCInfo
  Support

  ADD_TO_COMPONENT
  WDC
  )