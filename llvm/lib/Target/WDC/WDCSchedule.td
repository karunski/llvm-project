//===-- WDCSchedule.td - WDC Scheduling Definitions ------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Only 1 functional unit described in the 65816
//===----------------------------------------------------------------------===//
def CPUFuncUnit : FuncUnit;

//===----------------------------------------------------------------------===//
// Instruction Itinerary classes used for 65816
//===----------------------------------------------------------------------===//
def InstrItinNone   : InstrItinClass;
def InstrItinPseudo    : InstrItinClass;

//===----------------------------------------------------------------------===//
// 65816 Generic instruction itineraries.
//===----------------------------------------------------------------------===//
//@ http://llvm.org/docs/doxygen/html/structllvm_1_1InstrStage.html
def WDCGenericItineraries : ProcessorItineraries<[CPUFuncUnit], [], [
////@2
    InstrItinData<InstrItinNone, [InstrStage<1, [CPUFuncUnit]>] >
//  InstrItinData<IIAlu              , [InstrStage<1,  [ALU]>]>,
//  InstrItinData<II_CLO             , [InstrStage<1,  [ALU]>]>,
//  InstrItinData<II_CLZ             , [InstrStage<1,  [ALU]>]>,
//  InstrItinData<IILoad             , [InstrStage<3,  [ALU]>]>,
//  InstrItinData<IIStore            , [InstrStage<1,  [ALU]>]>,
//  InstrItinData<IIBranch           , [InstrStage<1,  [ALU]>]>
]>;
