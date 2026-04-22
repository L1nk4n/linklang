#include "printer.h"

std::string irTypeToString(IRType t)
{
  switch(t) {
    case IRType::I32: return "i32";
    case IRType::I64: return "i64";
    case IRType::Ptr: return "ptr";
    case IRType::Void: return "void";
  }
  return "?";
};

std::string opcodeToString(Opcode op)
{
  switch(op) {
    case Opcode::Const: return "const";
    case Opcode::Add: return "add";
    case Opcode::Sub: return "sub";
    case Opcode::Mul: return "mul";
    case Opcode::Div: return "div";
    case Opcode::Alloca: return "alloca";
    case Opcode::Load: return "load";
    case Opcode::Store: return "store";
    case Opcode::Ret: return "ret";
    case Opcode::PrintStr: return "printstr";
    case Opcode::PrintNewLine: return "printnl";
    case Opcode::PrintInt: return "printint";
  }
  return "?";
};

void printValue(const Value& v, std::ostream& out) {
  if(v.isConstant())
  {
    out << v.constant;
  } else {
    out << "%" << v.id;
  }
}

void printInstruction(const Instruction& inst, std::ostream& out)
{
  out << "    ";
  bool hasResult =
    inst.op != Opcode::Store &&
    inst.op != Opcode::Ret &&
    inst.op != Opcode::PrintStr &&
    inst.op != Opcode::PrintInt &&
    inst.op != Opcode::PrintNewLine;

  if(hasResult)
  {
    out << "%" << inst.result.id << " = ";
  }

  out << opcodeToString(inst.op);

  if(hasResult)
  {
    out << " " << irTypeToString(inst.result.type);
  }

  for(size_t i = 0; i < inst.operands.size(); ++i)
  {
    out << (i == 0 ? " " : ", ");
    printValue(inst.operands[i], out);
  }

  out << "\n";
}

void printBlock(const BasicBlock& bb, std::ostream& out)
{
  out << bb.label << ":\n";
  for(const auto& inst : bb.instructions)
  {
    printInstruction(inst, out);
  }
}


void printFunction(const Function& fn, std::ostream& out)
{
  out << "function " << fn.name << " -> " << irTypeToString(fn.returnType) << " {\n";
  for(const auto& bb : fn.blocks)
  {
    printBlock(bb, out);
  }
  out << "}\n";
}

void printModule(const Module& module, std::ostream& out)
{
  if(!module.stringLiterals.empty()) {
    out << "string_table:\n";
    for (size_t i = 0; i < module.stringLiterals.size(); ++i) {
      out << "    @str[" << i << "] = \"" << module.stringLiterals[i] << "\"\n"; 
    }
    out << "\n";
  }
  for(const auto& fn : module.functions)
  {
    printFunction(fn, out);
    out << "\n";
  }
}
