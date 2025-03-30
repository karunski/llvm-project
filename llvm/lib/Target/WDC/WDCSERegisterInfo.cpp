//===-- WDCSERegisterInfo.cpp - WDC Register Information ------== -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the WDC implementation of the TargetRegisterInfo
// class.
//
//===----------------------------------------------------------------------===//

#include "WDCSERegisterInfo.h"

using namespace llvm;

#define DEBUG_TYPE "WDC-reg-info"

WDCSERegisterInfo::WDCSERegisterInfo(const WDCSubtarget &ST)
  : WDCRegisterInfo(ST) {}

const TargetRegisterClass *
WDCSERegisterInfo::intRegClass(unsigned Size) const {
  return &WDC::CPURegsRegClass;
}

