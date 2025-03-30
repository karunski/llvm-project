// Return
def WDCRetNode : SDNode<"WDCISD::Ret", SDTNone,
                     [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def BinaryOpNode : SDTypeProfile<1, 2, [SDTCisInt<0>, SDTCisInt<1>]>;
def ADDiNode  : SDNode<"WDCISD::ADDi",  BinaryOpNode, [SDNPOutGlue]>;
def ADDsrNode : SDNode<"WDCISD::ADDsr", BinaryOpNode, [SDNPHasChain, SDNPOutGlue]>;
def ANDsrNode : SDNode<"WDCISD::ANDsr", BinaryOpNode, [SDNPHasChain]>;
def SUBsrNode : SDNode<"WDCISD::SUBsr", BinaryOpNode, [SDNPHasChain, SDNPOutGlue]>;
def ORAsrNode : SDNode<"WDCISD::ORAsr", BinaryOpNode, [SDNPHasChain]>;
