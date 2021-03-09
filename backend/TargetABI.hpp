#ifndef TARGETABI_HPP
#define TARGETABI_HPP

#include "TargetRegister.hpp"
#include <vector>

class TargetABI {
  using RegList = std::vector<TargetRegister *>;

public:
  TargetABI() {}

  unsigned GetStackAlignment() const { return StackAlignment; }
  void SetStackAlignment(unsigned Alignment) { StackAlignment = Alignment; }

  RegList &GetArgumentRegisters() { return ArgumentRegisters; }
  void SetArgumentRegisters(RegList ArgRegs) { ArgumentRegisters = ArgRegs; }

  RegList &GetCallerSavedRegisters() { return CallerSavedRegisters; }
  void SetCallerSavedRegisters(RegList CallerSaved) {
    CallerSavedRegisters = CallerSaved;
  }

  RegList &GetReturnRegisters() { return ReturnRegisters; }
  void SetReturnRegisters(RegList ReturnRegs) { ReturnRegisters = ReturnRegs; }

protected:
  unsigned StackAlignment;
  RegList ArgumentRegisters;
  RegList CalleSavedRegisters;
  RegList CallerSavedRegisters;
  RegList ReturnRegisters;
};

#endif