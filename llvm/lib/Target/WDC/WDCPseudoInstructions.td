class ADDSUBStackRelativePseudo<SDNode PseudoOpNode>
: InstPseudo<
  (outs RegsA16:$dst),
  (ins RegsA16:$src, StackOffsetOperand:$slot),
  [(set RegsA16:$dst, (PseudoOpNode RegsA16:$src, frameIndexPattern:$slot))]
>
{
  let hasCtrlDep = 1;
  let isCommutable = 0;
  let isAdd = 1;
  let Constraints = "$dst = $src";
  let mayLoad = 1;
}
