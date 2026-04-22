#include "../src/ir/ir.h"
#include "../src/ir/printer.h"
#include <iostream>

int main() {
  Module m;
  Function fn;
  fn.name = "main";
  fn.returnType = IRType::I32;
  BasicBlock entry;
  entry.label = "entry";
  Instruction c;
  c.op = Opcode::Const;
  c.result = Value{0, IRType::I32, 0};
  c.operands.push_back(Value{-1, IRType::I32, 7});
  entry.instructions.push_back(c);

  Instruction r;
  r.op = Opcode::Ret;
  r.result = Value{-1, IRType::Void, 0};
  r.operands.push_back(Value{0, IRType::I32, 0});
  entry.instructions.push_back(r);

  fn.blocks.push_back(std::move(entry));
  m.functions.push_back(std::move(fn));


  printModule(m, std::cout);
}
