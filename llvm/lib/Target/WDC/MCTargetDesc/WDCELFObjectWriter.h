#ifndef LLVM_LIB_TARGET_WDC_ELF_OBJECT_WRITER_H
#define LLVM_LIB_TARGET_WDC_ELF_OBJECT_WRITER_H

#include <memory>

namespace llvm {
class MCObjectTargetWriter;
class Triple;

std::unique_ptr<MCObjectTargetWriter>
createWDCELFObjectWriter(const Triple &TT);

} // namespace llvm

#endif