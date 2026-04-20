#include <iostream>
#include <locale>
#include <unordered_map>
#include <string>
#include <memory>

enum class Type {
  Ui8,
  Ui16,
  Ui32,
  Ui64,
  Ui128,
  I8,
  I16,
  I32,
  I64,
  I128,

  Double,
  String,
  Float,
  Long,

  Enum,
  Array,
  Interface,
  Struct,
  Object,
  Class,
  UNKOWN
};

struct Expr {
  virtual ~Expr() = default;
  virtual Type analyze() = 0;
};

struct Number : Expr {
  Type type;
  Number(Type t) : type(t) {}

  Type analyze() override {
    return type;
  }
};

struct Variable : Expr {
  std::string name;
  Variable(const std::string& n) : name(n) {}
  Type analyze() override;
};

struct BinaryOp : Expr {
  std::unique_ptr<Expr> left, right;

  BinaryOp(std::unique_ptr<Expr> l, std::unique_ptr<Expr> r)
    : left(std::move(l)), right(std::move(r)) {}

  Type analyze() override;
};


std::unordered_map<std::string, Type> symbolTable;

Type Variable::analyze()
{
  if(symbolTable.find(name) == symbolTable.end())
  {
    std::cerr << "Semantic error: Undeclared variable" << name << "'\n";
    return Type::UNKOWN;
  }
  return symbolTable.at(name);
}

Type BinaryOp::analyze()
{
  Type leftType = left->analyze();
  Type rightType = right->analyze();

  if(leftType == Type::UNKOWN || rightType == Type::UNKOWN)
    return Type::UNKOWN;

  if(leftType != rightType)
  {
    std::cerr << "Semantic error: Type mismatch in binary operation\n";
    return Type::UNKOWN;
  }
  return leftType;
}


int main() {
  symbolTable["x"] = Type::I8;

  auto expr = std::make_unique<BinaryOp>(
      std::make_unique<Variable>("x"),
      std::make_unique<Number>(Type::I8)
  );

  Type result = expr->analyze();

  if(result != Type::UNKOWN)
  {
    std::cout << "Expression is semantically valid!\n";
  }

  return 0;
}
