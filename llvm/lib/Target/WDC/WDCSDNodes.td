// Return
def WDCRetNode : SDNode<"WDCISD::Ret", SDTNone,
                     [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def BinaryOpNode : SDTypeProfile<1, 2, [SDTCisInt<0>, SDTCisInt<1>]>;
def ADDiNode  : SDNode<"WDCISD::ADDi",  BinaryOpNode, [SDNPOutGlue]>;
def ADDsrNode : SDNode<"WDCISD::ADDsr", BinaryOpNode, [SDNPHasChain, SDNPOutGlue]>;
def ANDsrNode : SDNode<"WDCISD::ANDsr", BinaryOpNode, [SDNPHasChain]>;
def EORsrNode : SDNode<"WDCISD::EORsr", BinaryOpNode, [SDNPHasChain]>;
def SETCCsrNode : SDNode<"WDCISD::SETCCsr", 
                         SDTypeProfile<1, 3, [SDTCisInt<0>, SDTCisInt<1>, /*, SDTCisPtr*/ SDTCisVT<3, OtherVT>]>, 
                         [SDNPHasChain]>;
def SUBsrNode : SDNode<"WDCISD::SUBsr", BinaryOpNode, [SDNPHasChain, SDNPOutGlue]>;
def ORAsrNode : SDNode<"WDCISD::ORAsr", BinaryOpNode, [SDNPHasChain]>;
def Wrapper : SDNode<"WDCISD::Wrapper", SDTIntUnaryOp>;
def LDAdpilNode : SDNode<"WDCISD::LDAdpil", SDTIntUnaryOp, [SDNPHasChain]>;