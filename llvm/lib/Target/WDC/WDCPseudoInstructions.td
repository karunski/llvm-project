class ADDSUBStackRelativePseudo<SDNode PseudoOpNode>
: InstPseudo<
  (outs AccumulatorRegisterClass:$dst),
  (ins AccumulatorRegisterClass:$src, StackOffsetOperand:$slot),
  [(set AccumulatorRegisterClass:$dst, (PseudoOpNode AccumulatorRegisterClass:$src, frameIndexPattern:$slot))]
>
{
  let hasCtrlDep = 1;
  let isCommutable = 0;
  let isAdd = 1;
  let Constraints = "$dst = $src";
  let mayLoad = 1;
}

class ADDImmediatePseudo
: InstPseudo<
  (outs AccumulatorRegisterClass:$dst),
  (ins AccumulatorRegisterClass:$src, Operand<i16>:$constant),
  [(set AccumulatorRegisterClass:$dst, (ADDiNode AccumulatorRegisterClass:$src, (i16 imm:$constant)))]
>
{
  let isCommutable = 0;
  let isAdd = 1;
  let Constraints = "$dst = $src";
  let mayLoad = 1;
}