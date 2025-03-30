//===-- WDCCallingConv.td - Calling Conventions for WDC --*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for WDC architecture.
//===----------------------------------------------------------------------===//

/// CCIfSubtarget - Match if the current subtarget has a feature F.
class CCIfSubtarget<string F, CCAction A>:
  CCIf<!strconcat("State.getTarget().getSubtarget<WDCSubtarget>().", F), A>;

def CSR_O32 : CalleeSavedRegs<(add A, X, Y)>;
                                //    (sequence "S%u", 1, 0))>;