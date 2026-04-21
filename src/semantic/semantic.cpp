#include "semantic.h"

Symbol* Scope::lookup(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) return &it->second;
    if (parent) return parent->lookup(name);
    return nullptr;
}

bool Scope::declare(const std::string& name, Symbol sym) {
    if (symbols.count(name)) return false;
    symbols[name] = std::move(sym);
    return true;
}

void FunctionDecl::analyze(SemanticContext& ctx) {
    Symbol funcSym;
    funcSym.kind = Symbol::Kind::Function;
    funcSym.returnType = returnType;
    for (const auto& p : params) funcSym.paramTypes.push_back(p.type);

    if (!ctx.currentScope->declare(name, funcSym)) {
        ctx.error("Function '" + name + "' already declared");
        return;
    }

    Scope functionScope;
    functionScope.parent = ctx.currentScope;

    for (const auto& p : params) {
        Symbol paramSym;
        paramSym.kind = Symbol::Kind::Variable;
        paramSym.type = p.type;
        if (!functionScope.declare(p.name, paramSym)) {
            ctx.error("Duplicate parameter '" + p.name + "' in " + name);
        }
    }

    Scope* savedScope = ctx.currentScope;
    Type savedReturnType = ctx.currentFunctionReturnType;
    ctx.currentScope = &functionScope;
    ctx.currentFunctionReturnType = returnType;

    for (auto& stmt : body) stmt->analyze(ctx);

    ctx.currentScope = savedScope;
    ctx.currentFunctionReturnType = savedReturnType;
}

void StructDecl::analyze(SemanticContext& ctx) {
    Symbol structSym;
    structSym.kind = Symbol::Kind::Struct;
    structSym.type = Type::Struct;
    for (const auto& f : fields) {
        if (structSym.fields.count(f.name)) {
            ctx.error("Duplicate field '" + f.name + "' in struct " + name);
            continue;
        }
        structSym.fields[f.name] = f.type;
    }

    if (!ctx.currentScope->declare(name, structSym)) {
        ctx.error("Struct '" + name + "' already declared");
    }
}

void VarDecl::analyze(SemanticContext& ctx) {
    Symbol sym;
    sym.kind = Symbol::Kind::Variable;
    sym.type = type;
    if (!ctx.currentScope->declare(name, sym)) {
        ctx.error("Variable '" + name + "' already declared in this scope");
    }
}

void ReturnStmt::analyze(SemanticContext& ctx) {
}

void ClassDecl::analyze(SemanticContext& ctx) {
    Scope classScope;
    classScope.parent = ctx.currentScope;
    Scope* saved = ctx.currentScope;
    ctx.currentScope = &classScope;
    for (auto& stmt : body) stmt->analyze(ctx);
    ctx.currentScope = saved;
}

void EnumDecl::analyze(SemanticContext& ctx) {
    Scope enumScope;
    enumScope.parent = ctx.currentScope;
    Scope* saved = ctx.currentScope;
    ctx.currentScope = &enumScope;
    for (auto& stmt : body) stmt->analyze(ctx);
    ctx.currentScope = saved;
}

void ForStmt::analyze(SemanticContext& ctx) {
    Scope forScope;
    forScope.parent = ctx.currentScope;
    Symbol loopVar;
    loopVar.kind = Symbol::Kind::Variable;
    loopVar.type = varType;
    forScope.declare(varName, loopVar);

    Scope* saved = ctx.currentScope;
    ctx.currentScope = &forScope;
    for (auto& stmt : body) stmt->analyze(ctx);
    ctx.currentScope = saved;
}

void WhileStmt::analyze(SemanticContext& ctx) {
    Scope whileScope;
    whileScope.parent = ctx.currentScope;
    Scope* saved = ctx.currentScope;
    ctx.currentScope = &whileScope;
    for (auto& stmt : body) stmt->analyze(ctx);
    ctx.currentScope = saved;
}

std::vector<std::string> SemanticAnalyzer::analyze(std::vector<std::unique_ptr<Stmt>>& program) {
    Scope globalScope;
    SemanticContext ctx;
    ctx.currentScope = &globalScope;

    for (auto& stmt : program) {
        stmt->analyze(ctx);
    }

    return ctx.errors;
}
