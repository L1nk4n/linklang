#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

enum class Type {
    Ui8, Ui16, Ui32, Ui64, Ui128,
    I8, I16, I32, I64, I128,
    Double, Float, Long, String,
    Enum, Array, Interface, Struct, Object, Class,
    Void,
    UNKNOWN
};

std::string typeToString(Type t);

struct Symbol {
    enum class Kind { Variable, Function, Struct };
    Kind kind;
    Type type = Type::UNKNOWN;
    std::vector<Type> paramTypes;
    Type returnType = Type::UNKNOWN;
    std::unordered_map<std::string, Type> fields;
};

struct Scope {
    Scope* parent = nullptr;
    std::unordered_map<std::string, Symbol> symbols;

    Symbol* lookup(const std::string& name);
    bool declare(const std::string& name, Symbol sym);
};

struct SemanticContext {
    Scope* currentScope;
    Type currentFunctionReturnType = Type::UNKNOWN;
    std::vector<std::string> errors;

    void error(const std::string& msg) { errors.push_back(msg); }
};

struct Expr {
    virtual ~Expr() = default;
    virtual Type analyze(SemanticContext& ctx) = 0;
};

struct Stmt {
    virtual ~Stmt() = default;
    virtual void analyze(SemanticContext& ctx) = 0;
};

struct Param {
    std::string name;
    Type type;
};

struct Field {
    std::string name;
    Type type;
};

struct FunctionDecl : Stmt {
    std::string name;
    Type returnType;
    std::vector<Param> params;
    std::vector<std::unique_ptr<Stmt>> body;

    FunctionDecl(std::string n, Type ret, std::vector<Param> p,
                 std::vector<std::unique_ptr<Stmt>> b)
        : name(std::move(n)), returnType(ret),
          params(std::move(p)), body(std::move(b)) {}

    void analyze(SemanticContext& ctx) override;
};

struct StructDecl : Stmt {
    std::string name;
    std::vector<Field> fields;

    StructDecl(std::string n, std::vector<Field> f)
        : name(std::move(n)), fields(std::move(f)) {}

    void analyze(SemanticContext& ctx) override;
};

struct VarDecl : Stmt {
    std::string name;
    Type type;

    VarDecl(std::string n, Type t)
        : name(std::move(n)), type(t) {}

    void analyze(SemanticContext& ctx) override;
};

struct ReturnStmt : Stmt {
    std::unique_ptr<Expr> expr;
    ReturnStmt() = default;
    explicit ReturnStmt(std::unique_ptr<Expr> e) : expr(std::move(e)) {}

    void analyze(SemanticContext& ctx) override;

};

struct ExitStmt : Stmt {
    void analyze(SemanticContext& ctx) override {}
};

struct BuiltinCallStmt : Stmt {
    std::string name;

    BuiltinCallStmt(std::string n) : name(std::move(n)) {}

    void analyze(SemanticContext& ctx) override {}
};

struct ClassDecl : Stmt {
    std::string name;
    std::vector<std::unique_ptr<Stmt>> body;

    ClassDecl(std::string n, std::vector<std::unique_ptr<Stmt>> b)
        : name(std::move(n)), body(std::move(b)) {}

    void analyze(SemanticContext& ctx) override;
};

struct EnumDecl : Stmt {
    std::string name;
    std::vector<std::unique_ptr<Stmt>> body;

    EnumDecl(std::string n, std::vector<std::unique_ptr<Stmt>> b)
        : name(std::move(n)), body(std::move(b)) {}

    void analyze(SemanticContext& ctx) override;
};

struct ForStmt : Stmt {
    std::string varName;
    Type varType;
    std::vector<std::unique_ptr<Stmt>> body;

    ForStmt(std::string vn, Type vt, std::vector<std::unique_ptr<Stmt>> b)
        : varName(std::move(vn)), varType(vt), body(std::move(b)) {}

    void analyze(SemanticContext& ctx) override;
};

struct WhileStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> body;

    WhileStmt(std::vector<std::unique_ptr<Stmt>> b)
        : body(std::move(b)) {}

    void analyze(SemanticContext& ctx) override;
};

class SemanticAnalyzer {
public:
    std::vector<std::string> analyze(std::vector<std::unique_ptr<Stmt>>& program);
};
