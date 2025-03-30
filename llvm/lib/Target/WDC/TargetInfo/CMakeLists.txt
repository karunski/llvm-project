add_llvm_component_library(LLVMWDCInfo
  WDCTargetInfo.cpp

  LINK_COMPONENTS
  Support

  ADD_TO_COMPONENT
  WDC
  )