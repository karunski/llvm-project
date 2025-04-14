// Return
def WDCRetNode : SDNode<"WDCISD::Ret", SDTNone,
                     [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def BinaryOpNode : SDTypeProfile<1, 2, [SDTCisInt<0>, SDTCisInt<1>]>;
def TypeProfRegMem : SDTypeProfile<1, 2, [SDTCisInt<0>, SDTCisSameAs<0,1>, SDTCisPtrTy<2>]>;

def ADDiNode  : SDNode<"WDCISD::ADDi",  BinaryOpNode, [SDNPOutGlue]>;
def ADCiNode  : SDNode<"WDCISD::ADCi",  SDTypeProfile<1, 2, [SDTCisInt<0>, SDTCisSameAs<0, 1>, SDTCisInt<2>]>, [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;
def ADDNode       : SDNode<"WDCISD::ADD",   TypeProfRegMem, [SDNPHasChain, SDNPOutGlue]>;
def ADCsrNode : SDNode<"WDCISD::ADCsr", TypeProfRegMem, [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;
def ANDsrNode : SDNode<"WDCISD::ANDsr", BinaryOpNode, [SDNPHasChain]>;
def CLCNode   : SDNode<"WDCISD::CLC", SDTypeProfile<0, 0, []>, [SDNPHasChain, SDNPOutGlue]>;
def EORsrNode : SDNode<"WDCISD::EORsr", BinaryOpNode, [SDNPHasChain]>;
def SETCCsrNode : SDNode<"WDCISD::SETCCsr", 
                         SDTypeProfile<1, 3, [SDTCisInt<0>, SDTCisInt<1>, /*, SDTCisPtr*/ SDTCisVT<3, OtherVT>]>, 
                         [SDNPHasChain]>;
def SBCNode : SDNode<"WDCISD::SBC", TypeProfRegMem, [SDNPHasChain, SDNPOutGlue, SDNPInGlue]>;
def SUBNode : SDNode<"WDCISD::SUB", TypeProfRegMem, [SDNPHasChain, SDNPOutGlue]>;
def ORAsrNode : SDNode<"WDCISD::ORAsr", BinaryOpNode, [SDNPHasChain]>;
def Wrapper : SDNode<"WDCISD::Wrapper", SDTIntUnaryOp>;
def LDAdpilNode : SDNode<"WDCISD::LDAdpil", SDTLoad, [SDNPHasChain]>;
def LDAiNode : SDNode<"WDCISD::LDAi", SDTypeProfile<1, 2, [SDTCisInt<0>, SDTCisInt<1>, SDTCisInt<2>, SDTCisSameAs<0,1>]>>;
def LEANode : SDNode<"WDCISD::LEA", SDTypeProfile<1, 1, [SDTCisInt<0>, SDTCisPtrTy<1>]>, []>;
def SECNode : SDNode<"WDCISD::SEC", SDTypeProfile<0, 0, []>, [SDNPHasChain, SDNPOutGlue]>;
def STANode : SDNode<"WDCISD::STA", SDTypeProfile<0, 3, [SDTCisPtrTy<1>, SDTCisInt<2>]>, [SDNPHasChain,SDNPMayStore]>;
def SETMNode : SDNode<"WDCISD::SETM", SDTypeProfile<1, 1, [SDTCisInt<0>, SDTCisInt<1>, SDTCisSameAs<0, 1>]>>;
