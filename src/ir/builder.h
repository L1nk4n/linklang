#pragma once
#include "ir.h"
#include "../semantic/semantic.h"

class IRBuilder {
  public:
    Module build(std::vector<std::unique_ptr<Stmt>>& program);

  private:
    Module module;
    Function* currentFunction = nullptr;
    BasicBlock* currentBlock = nullptr;


    void lowerStmt(Stmt* s);
    void lowerFunctionDecl(FunctionDecl* f);
    void lowerVarDecl(VarDecl* v);
    void lowerReturnStmt(ReturnStmt* r);
    Value lowerExpr(Expr* e);
    void lowerBuiltinCall(BuiltinCallStmt* b);

    Value freshValue(IRType t);
    Value constInt(int64_t v, IRType t);
    Value emit(Opcode op, IRType resultType, std::vector<Value> operands);
};
