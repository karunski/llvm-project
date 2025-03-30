// Return
def WDCRetNode : SDNode<"WDCISD::Ret", SDTNone,
                     [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def BinaryOpNode : SDTypeProfile<1, 2, [SDTCisInt<0>, SDTCisInt<1>]>;

def ADDsrNode : SDNode<"WDCISD::ADDsr", BinaryOpNode, [SDNPHasChain, SDNPOutGlue]>;
def SUBsrNode : SDNode<"WDCISD::SUBsr", BinaryOpNode, [SDNPHasChain, SDNPOutGlue]>;
def ADDiNode  : SDNode<"WDCISD::ADDi",  BinaryOpNode, [SDNPOutGlue]>;
def SHLNode   : SDNode<"WDCISD::SHL",   BinaryOpNode, [SDNPOutGlue]>;
