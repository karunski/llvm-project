#include "WDC.h"

#include "llvm/TargetParser/Triple.h"
#include "llvm/ADT/ArrayRef.h"
#include "clang/Basic/MacroBuilder.h"

using namespace std::literals::string_view_literals;

namespace
{
class WDCTargetInfo final : public clang::TargetInfo
{
public:
WDCTargetInfo(const llvm::Triple &Triple, const clang::TargetOptions &)
: TargetInfo{Triple} {
    TLSSupported = false;
    PointerWidth = 24;
    PointerAlign = 8;
    ShortWidth = 8;
    ShortAlign = 8;
    IntWidth = 16;
    IntAlign = 8;
    LongWidth = 32;
    LongAlign = 8;
    LongLongWidth = 32;
    LongLongAlign = 8;
    SuitableAlign = 8;
    DefaultAlignForAttributeAligned = 8;
    HalfWidth = 16;
    HalfAlign = 8;
    FloatWidth = 32;
    FloatAlign = 8;
    DoubleWidth = 32;
    DoubleAlign = 8;
    DoubleFormat = &llvm::APFloat::IEEEsingle();
    LongDoubleWidth = 32;
    LongDoubleAlign = 8;
    LongDoubleFormat = &llvm::APFloat::IEEEsingle();
    SizeType = UnsignedInt;
    PtrDiffType = SignedInt;
    IntPtrType = SignedInt;
    Char16Type = UnsignedInt;
    WIntType = SignedInt;
    Int16Type = SignedInt;
    Char32Type = UnsignedLong;
    SigAtomicType = SignedChar;
    resetDataLayout("e-S8-p:32:16-i8:8-i16:8-i32:8-i64:8-f32:8-f64:8-n16-a:8");
  }

  void getTargetDefines(const clang::LangOptions &, clang::MacroBuilder &Builder) const override {
    Builder.defineMacro("WDC");
    Builder.defineMacro("__WDC");
    Builder.defineMacro("__WDC__");
  }

  llvm::SmallVector<clang::Builtin::InfosShard> getTargetBuiltins() const override {
    return {};
  }

  BuiltinVaListKind getBuiltinVaListKind() const override {
    return clang::TargetInfo::VoidPtrBuiltinVaList;
  }

  bool
  validateAsmConstraint(const char *& /*Name*/,
                        TargetInfo::ConstraintInfo & /*info*/) const override {
    return true;
  }

  std::string_view getClobbers() const override {
    return ""sv;
  }

  llvm::ArrayRef<const char *> getGCCRegNames() const override {
    static const char *const GCCRegNames[] = {
        "A",  "X",  "Y",  "S",  "P"};
    return llvm::ArrayRef(GCCRegNames);
  }

  llvm::ArrayRef<GCCRegAlias> getGCCRegAliases() const override {
    return {};
  }
};
}

std::unique_ptr<clang::TargetInfo> clang::targets::createWDCTargetInfo(const llvm::Triple &Triple, const TargetOptions & Options) {
    return std::make_unique<WDCTargetInfo>(Triple, Options);
}