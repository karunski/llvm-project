add_llvm_component_library(LLVMWDCAsmPrinter
  WDCInstPrinter.cpp

  LINK_COMPONENTS
  Support

  ADD_TO_COMPONENT
  WDC
  )
