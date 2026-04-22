#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct Instruction;
struct BasicBlock;
struct Function;
struct Module;

enum class IRType {
  I8,
  I16,
  I32,
  I64,
  I128,
  Ui8,
  Ui16,
  Ui32,
  Ui64,
  Ui128,
  Ptr,
  Void,
};

struct Value {
  int id;
  IRType type;
  int64_t constant;
  bool isConstant() const { return id == -1; }
};

enum class Opcode {
  Const,
  Add, Sub, Mul, Div,
  Alloca,
  Load,
  Store,
  Ret,
  PrintStr,
  PrintInt,
  PrintNewLine
};

struct Instruction {
  Opcode op;
  Value result;
  std::vector<Value> operands;
};

struct BasicBlock {
  std::string label;
  std::vector<Instruction> instructions;
};

struct Function {
  std::string name;
  IRType returnType;
  std::vector<IRType> paramTypes;
  std::vector<BasicBlock> blocks;
  int nextValueId = 0;
};

struct Module {
  std::vector<Function> functions;
  std::vector<std::string> stringLiterals;
};
