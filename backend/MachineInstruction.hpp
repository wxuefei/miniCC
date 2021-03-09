#ifndef MACHINEINSTRUCTION_HPP
#define MACHINEINSTRUCTION_HPP

#include "MachineOperand.hpp"
#include <vector>

class MachineBasicBlock;

class MachineInstruction {
  using OperandList = std::vector<MachineOperand>;

public:
  enum OperationCode : unsigned {
    // Arithmetic and Logical
    AND = 1 << 16,
    OR,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    CMP, // Logical comparison

    // Conversions
    FTOI, // Float TO Integer
    ITOF, // Integer TO Float

    // Control flow operations
    CALL,
    JUMP,
    BRANCH,
    RET,

    // Memory operations
    LOAD,
    STORE,
    STACK_ALLOC,
  };

  enum CMPRelation { INVALID, EQ, NE, LT, GT, LE, GE };

  MachineInstruction() {}
  MachineInstruction(unsigned Opcode, MachineBasicBlock *Parent)
      : Opcode(Opcode), Parent(Parent) {}

  unsigned GetOpcode() const { return Opcode; }
  void SetOpcode(unsigned Opcode) { this->Opcode = Opcode; }

  size_t GetOperandsNumber() const { return Operands.size(); }

  MachineOperand *GetOperand(size_t Index) { return &Operands[Index]; }
  OperandList &GetOperands() { return Operands; }

  void AddOperand(MachineOperand MO) { Operands.push_back(MO); }
  void SetAttributes(unsigned A) { OtherAttributes = A; }

  unsigned GetRelation() const { return OtherAttributes; }

  MachineBasicBlock *GetParent() const { return Parent; }
  void SetParent(MachineBasicBlock *BB) { Parent = BB; }

  void RemoveOperand(unsigned Index) {
    Operands.erase(Operands.begin() + Index);
  }

  void InsertOperand(unsigned Index, MachineOperand Operand) {
    Operands.insert(Operands.begin() + Index, Operand);
  }

  void RemoveMemOperand() {
    for (size_t i = 0; i < Operands.size(); i++)
      if (Operands[i].IsStackAccess())
        Operands.erase(Operands.begin() + i--);
  }

  bool IsFallThroughBranch() const { return Operands.size() == 2; }
  bool IsLoad() const { return Opcode == LOAD; }
  bool IsStore() const { return Opcode == STORE; }
  bool IsLoadOrStore() const { return Opcode == LOAD || Opcode == STORE; }

private:
  unsigned Opcode = 0;

  // Capture things like the relation for compare instructions
  unsigned OtherAttributes = 0;
  OperandList Operands;

  MachineBasicBlock *Parent = nullptr;
};

#endif