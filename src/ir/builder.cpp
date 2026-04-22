#include "builder.h"
#include <iostream>

Value IRBuilder::freshValue(IRType t) {
  Value v;
  v.id = currentFunction->nextValueId++;
  v.type = t;
  v.constant = 0;
  return v;
};

Value IRBuilder::constInt(int64_t v, IRType t) {
  Value val;
  val.id = -1;
  val.type = t;
  val.constant = v;
  return val;
};

Value IRBuilder::emit(Opcode op, IRType resultType, std::vector<Value> operands) {
  Instruction inst;
  inst.op = op;
  inst.operands = std::move(operands);

  bool hasResult = (op != Opcode::Store && op != Opcode::Ret);
  if(hasResult) {
    inst.result = freshValue(resultType);
  } else {
    inst.result = Value{-1, IRType::Void, 0};
  }

  currentBlock->instructions.push_back(inst);
  return inst.result;
};

static IRType toIRType(Type t) {
  switch(t) {
    case Type::I32: return IRType::I32;
    case Type::I64: return IRType::I64;
    case Type::Void: return IRType::Void;
    case Type::NotInFunction: return IRType::Void;
    default: return IRType::I32;
  }
}

Value IRBuilder::lowerExpr(Expr* e) {
  if(auto* lit = dynamic_cast<IntLiteral*>(e)) {
    IRType t = toIRType(lit->declaredType);
    return emit(Opcode::Const, t, { constInt(lit->value, t)} );
  }
  if(auto* id = dynamic_cast<IdentExpr*>(e)) {
    std::cerr << "IRBuilder: IdentExpr not yet supported (need alloca/load)\n";
    return constInt(0, IRType::I32);
  }
  if(auto* slit = dynamic_cast<StringLiteral*>(e)) {
    int idx = (int)module.stringLiterals.size();
    module.stringLiterals.push_back(slit->value);
    return constInt(idx, IRType::I32);
  }
  std::cerr << "IRBuilder: unsupported expression kind\n";
  return constInt(0, IRType::I32);
}

void IRBuilder::lowerReturnStmt(ReturnStmt* r) {
  if(!r->expr) {
    emit(Opcode::Ret, IRType::Void, {});
    return;
  }
  Value v = lowerExpr(r->expr.get());
  emit(Opcode::Ret, IRType::Void, { v });
}

void IRBuilder::lowerBuiltinCall(BuiltinCallStmt* b){
  if(b->name == "Print") {
    for(auto& arg : b->args) {
      Value v = lowerExpr(arg.get());
      emit(Opcode::PrintStr, IRType::Void, { v });
    }
  }
  else if(b->name == "PrintLn") {
    for(auto& arg : b->args) {
      Value v = lowerExpr(arg.get());
      emit(Opcode::PrintStr, IRType::Void, { v });
    }
    emit(Opcode::PrintNewLine, IRType::Void, {});
  }
  else {
    std::cerr << "IRBuilder: builtin '" << b->name << "' not yet supported\n";
  }
}

void IRBuilder::lowerVarDecl(VarDecl* v) {
  (void)v;
  std::cerr << "IRBuilder: VarDecl lowering not yet implemented\n";
}

void IRBuilder::lowerStmt(Stmt* s) {
  if(auto* r = dynamic_cast<ReturnStmt*>(s)) {
    lowerReturnStmt(r);
  } else if(auto* v = dynamic_cast<VarDecl*>(s)) {
    lowerVarDecl(v);
  } else if(auto* f = dynamic_cast<FunctionDecl*>(s)) {
    lowerFunctionDecl(f);
  } else if(auto* b = dynamic_cast<BuiltinCallStmt*>(s)) {
      lowerBuiltinCall(b);
  } else {
    std::cerr << "IRBuilder: unsupported statement kind\n";
  }
}

void IRBuilder::lowerFunctionDecl(FunctionDecl* f) {
  Function fn;
  fn.name = f->name;
  fn.returnType = toIRType(f->returnType);
  for(const auto& p : f->params) {
    fn.paramTypes.push_back(toIRType(p.type));
  }

  BasicBlock entry;
  entry.label = "entry";
  fn.blocks.push_back(std::move(entry));
  
  module.functions.push_back(std::move(fn));
  currentFunction = &module.functions.back();
  currentBlock = &currentFunction->blocks.back();

  for(auto& stmt : f->body) {
    lowerStmt(stmt.get());
  }

  currentFunction = nullptr;
  currentBlock = nullptr;
}

Module IRBuilder::build(std::vector<std::unique_ptr<Stmt>>& program) {
  module = Module{};
  for(auto& stmt : program) {
    lowerStmt(stmt.get());
  }
  return std::move(module);
}

