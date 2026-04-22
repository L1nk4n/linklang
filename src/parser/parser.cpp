#include "parser.h"
#include <iostream>

using namespace std;

class Parser {
private:
    const vector<Token>& tokens;
    size_t current;
    vector<unique_ptr<Stmt>> program;

public:
    Parser(const vector<Token>& toks) : tokens(toks), current(0) {}

    bool parse() {
        while (!isAtEnd()) {
            auto stmt = parseStatement();
            if (!stmt) return false;
            program.push_back(std::move(stmt));
        }
        return true;
    }

    vector<unique_ptr<Stmt>> getAST() { return std::move(program); }

private:
    bool isAtEnd() const {
        return current >= tokens.size();
    }

    const Token& peek() const {
        return tokens[current];
    }

    const Token& previous() const {
        return tokens[current - 1];
    }

    bool check(TokenType type) const {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    bool match(TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }

    void advance() {
        if (!isAtEnd()) {
            current++;
        }
    }

    bool expect(TokenType type, const string& message) {
        if (match(type)) {
            return true;
        }

        cerr << "Syntax error: " << message;
        if (!isAtEnd()) {
            cerr << " near '" << peek().value << "'";
        }
        cerr << endl;
        return false;
    }

    bool isTypeToken(TokenType type) const {
        return type == TokenType::String ||
            type == TokenType::Bool ||
            type == TokenType::Double ||
            type == TokenType::Float ||
            type == TokenType::Long ||
            type == TokenType::I8 ||
            type == TokenType::I16 ||
            type == TokenType::I32 ||
            type == TokenType::I64 ||
            type == TokenType::I128 ||
            type == TokenType::Ui8 ||
            type == TokenType::Ui16 ||
            type == TokenType::Ui32 ||
            type == TokenType::Ui64 ||
            type == TokenType::Ui128;
    }

    Type tokenTypeToType(TokenType tt) {
        switch (tt) {
            case TokenType::I8:     return Type::I8;
            case TokenType::I16:    return Type::I16;
            case TokenType::I32:    return Type::I32;
            case TokenType::I64:    return Type::I64;
            case TokenType::I128:   return Type::I128;
            case TokenType::Ui8:    return Type::Ui8;
            case TokenType::Ui16:   return Type::Ui16;
            case TokenType::Ui32:   return Type::Ui32;
            case TokenType::Ui64:   return Type::Ui64;
            case TokenType::Ui128:  return Type::Ui128;
            case TokenType::Double: return Type::Double;
            case TokenType::Float:  return Type::Float;
            case TokenType::Long:   return Type::Long;
            case TokenType::String: return Type::String;
            default:                return Type::UNKNOWN;
        }
    }

    unique_ptr<Stmt> parseStatement() {
        if (check(TokenType::Let)) return letDeclaration();
        if (check(TokenType::Function)) return functionDeclaration();
        if (check(TokenType::Class)) return classDeclaration();
        if (check(TokenType::Enum)) return enumDeclaration();
        if (isTypeToken(peek().type)) return typedDeclaration();
        if (check(TokenType::Struct)) return structDeclaration();
        if (check(TokenType::For)) return forStatement();
        if (check(TokenType::While)) return whileStatement();
        if (check(TokenType::Return)) return returnStatement();
        if (check(TokenType::Exit)) return exitStatement();

        if (check(TokenType::Length)) { if (!lengthStatement()) return nullptr; return make_unique<BuiltinCallStmt>("Length"); }
        if (check(TokenType::Print)) return printStatement();
        if (check(TokenType::PrintLn)) return printlnStatement();
        if (check(TokenType::TypeOf)) { if (!TypeOfStatement()) return nullptr; return make_unique<BuiltinCallStmt>("TypeOf"); }
        if (check(TokenType::SizeOf)) { if (!SizeOfStatement()) return nullptr; return make_unique<BuiltinCallStmt>("SizeOf"); }
        if (check(TokenType::WriteFile)) { if (!writeFileStatement()) return nullptr; return make_unique<BuiltinCallStmt>("WriteFile"); }
        if (check(TokenType::ReadFile)) { if (!readFileStatement()) return nullptr; return make_unique<BuiltinCallStmt>("ReadFile"); }
        if (check(TokenType::AppendFile)) { if (!appendFileStatement()) return nullptr; return make_unique<BuiltinCallStmt>("AppendFile"); }
        if (check(TokenType::Warn)) { if (!warnStatement()) return nullptr; return make_unique<BuiltinCallStmt>("Warn"); }
        if (check(TokenType::Error)) { if (!errorStatement()) return nullptr; return make_unique<BuiltinCallStmt>("Error"); }
        if (check(TokenType::Close)) { if (!closeStatement()) return nullptr; return make_unique<BuiltinCallStmt>("Close"); }
        if (check(TokenType::Open)) { if (!openStatement()) return nullptr; return make_unique<BuiltinCallStmt>("Open"); }
        if (check(TokenType::ReadLine)) { if (!readLineStatement()) return nullptr; return make_unique<BuiltinCallStmt>("ReadLine"); }
        if (check(TokenType::Input)) { if (!inputStatement()) return nullptr; return make_unique<BuiltinCallStmt>("Input"); }
        if (check(TokenType::SubString)) { if (!subStringStatement()) return nullptr; return make_unique<BuiltinCallStmt>("SubString"); }
        if (check(TokenType::Split)) { if (!splitStatement()) return nullptr; return make_unique<BuiltinCallStmt>("Split"); }
        if (check(TokenType::Replace)) { if (!replaceStatement()) return nullptr; return make_unique<BuiltinCallStmt>("Replace"); }
        if (check(TokenType::Contains)) { if (!containsStatement()) return nullptr; return make_unique<BuiltinCallStmt>("Contains"); }
        if (check(TokenType::StartsWith)) { if (!startsWithStatement()) return nullptr; return make_unique<BuiltinCallStmt>("StartsWith"); }
        if (check(TokenType::EndsWith)) { if (!endsWithStatement()) return nullptr; return make_unique<BuiltinCallStmt>("EndsWith"); }
        if (check(TokenType::ToUpper)) { if (!toUpperStatement()) return nullptr; return make_unique<BuiltinCallStmt>("ToUpper"); }
        if (check(TokenType::ToLower)) { if (!toLowerStatement()) return nullptr; return make_unique<BuiltinCallStmt>("ToLower"); }
        if (check(TokenType::Trim)) { if (!trimStatement()) return nullptr; return make_unique<BuiltinCallStmt>("Trim"); }
        if (check(TokenType::IndexOf)) { if (!indexOfStatement()) return nullptr; return make_unique<BuiltinCallStmt>("IndexOf"); }
        if (check(TokenType::Format)) { if (!formatStatement()) return nullptr; return make_unique<BuiltinCallStmt>("Format"); }

        cerr << "Syntax error: expected statement";
        if (!isAtEnd()) {
            cerr << " near '" << peek().value << "'";
        }
        cerr << endl;
        return nullptr;
    }

    bool block(vector<unique_ptr<Stmt>>& stmts) {
        if (!expect(TokenType::OpenCurlyBrackets, "expected '{' to start block"))
            return false;
        while (!isAtEnd() && !check(TokenType::CloseCurlyBrackets)) {
            auto stmt = parseStatement();
            if (!stmt) return false;
            stmts.push_back(std::move(stmt));
        }
        if (!expect(TokenType::CloseCurlyBrackets, "expected '}' to close block"))
            return false;
        return true;
    }

    bool type() {
        if (match(TokenType::String) ||
            match(TokenType::Bool) ||
            match(TokenType::Double) ||
            match(TokenType::Float) ||
            match(TokenType::Long) ||
            match(TokenType::I8)   ||
            match(TokenType::I16)  ||
            match(TokenType::I32)  ||
            match(TokenType::I64)  ||
            match(TokenType::I128) ||
            match(TokenType::Ui8)  ||
            match(TokenType::Ui16) ||
            match(TokenType::Ui32) ||
            match(TokenType::Ui64) ||
            match(TokenType::Ui128)) {
            return true;
        }

        cerr << "Syntax error: expected type";
        if (!isAtEnd()) {
            cerr << " near '" << peek().value << "'";
        }
        cerr << endl;
        return false;
    }

    unique_ptr<Stmt> letDeclaration() {
        if (!expect(TokenType::Let, "expected 'let'")) return nullptr;
        if (!expect(TokenType::Identifier, "expected identifier after 'let'")) return nullptr;
        string name = previous().value;
        if (!expect(TokenType::Equals, "expected '=' after identifier")) return nullptr;
        if (!expression()) {
            cerr << "Syntax error: expected expression after '='";
            if (!isAtEnd()) cerr << " near '" << peek().value << "'";
            cerr << endl;
            return nullptr;
        }
        if (!expect(TokenType::SemiColon, "expected ';' after let declaration")) return nullptr;
        return make_unique<VarDecl>(name, Type::UNKNOWN);
    }

    unique_ptr<Stmt> typedDeclaration() {
        TokenType typeToken = peek().type;
        if (!type()) return nullptr;
        if (!expect(TokenType::Identifier, "expected identifier after type")) return nullptr;
        string name = previous().value;
        if (!expect(TokenType::Equals, "expected '=' after identifier")) return nullptr;
        if (!expression()) {
            cerr << "Syntax error: expected expression after '='";
            if (!isAtEnd()) cerr << " near '" << peek().value << "'";
            cerr << endl;
            return nullptr;
        }
        if (!expect(TokenType::SemiColon, "expected ';' after typed declaration")) return nullptr;
        return make_unique<VarDecl>(name, tokenTypeToType(typeToken));
    }

    unique_ptr<Stmt> functionDeclaration() {
        if (!expect(TokenType::Function, "expected 'Function'")) return nullptr;
        if (!expect(TokenType::Identifier, "expected identifier after 'Function'")) return nullptr;
        string name = previous().value;
        vector<unique_ptr<Stmt>> body;
        if (!block(body)) return nullptr;
        return make_unique<FunctionDecl>(name, Type::I32, vector<Param>{}, std::move(body));
    }

    unique_ptr<Stmt> classDeclaration() {
        if (!expect(TokenType::Class, "expected 'Class'")) return nullptr;
        if (!expect(TokenType::Identifier, "expected identifier after 'Class'")) return nullptr;
        string name = previous().value;
        vector<unique_ptr<Stmt>> body;
        if (!block(body)) return nullptr;
        return make_unique<ClassDecl>(name, std::move(body));
    }

    unique_ptr<Stmt> enumDeclaration() {
        if (!expect(TokenType::Enum, "expected 'Enum'")) return nullptr;
        if (!expect(TokenType::Identifier, "expected identifier after 'Enum'")) return nullptr;
        string name = previous().value;
        vector<unique_ptr<Stmt>> body;
        if (!block(body)) return nullptr;
        return make_unique<EnumDecl>(name, std::move(body));
    }

    unique_ptr<Stmt> structDeclaration() {
        if (!expect(TokenType::Struct, "expected 'Struct'")) return nullptr;
        if (!expect(TokenType::Identifier, "expected identifier after 'Struct'")) return nullptr;
        string name = previous().value;
        vector<unique_ptr<Stmt>> body;
        if (!block(body)) return nullptr;
        return make_unique<StructDecl>(name, vector<Field>{});
    }

    unique_ptr<Stmt> forStatement() {
        if (!expect(TokenType::For, "expected 'for'")) return nullptr;
        if (!expect(TokenType::OpenParen, "expected '(' after 'for'")) return nullptr;
        TokenType typeToken = peek().type;
        if (!type()) return nullptr;
        if (!expect(TokenType::Identifier, "expected variable name")) return nullptr;
        string varName = previous().value;
        if (!expect(TokenType::Equals, "expected '=' in initialization")) return nullptr;
        if (!expression()) return nullptr;
        if (!expect(TokenType::SemiColon, "expected ';' after init")) return nullptr;
        if (!expression()) return nullptr;
        if (!expect(TokenType::SemiColon, "expected ';' after condition")) return nullptr;
        if (!expect(TokenType::Identifier, "expected variable in increment")) return nullptr;
        if (!expect(TokenType::IncrementOperator, "expected '++'")) return nullptr;
        if (!expect(TokenType::CloseParen, "expected ')'")) return nullptr;
        vector<unique_ptr<Stmt>> body;
        if (!block(body)) return nullptr;
        return make_unique<ForStmt>(varName, tokenTypeToType(typeToken), std::move(body));
    }

    unique_ptr<Stmt> whileStatement() {
        if (!expect(TokenType::While, "expected 'while'")) return nullptr;
        if (!expect(TokenType::OpenParen, "expected '(' after 'while'")) return nullptr;
        if (!expression()) {
            cerr << "Syntax error: expected condition in while";
            if (!isAtEnd()) cerr << " near '" << peek().value << "'";
            cerr << endl;
            return nullptr;
        }
        if (!expect(TokenType::CloseParen, "expected ')' after condition")) return nullptr;
        vector<unique_ptr<Stmt>> body;
        if (!block(body)) return nullptr;
        return make_unique<WhileStmt>(std::move(body));
    }

    unique_ptr<Expr> parseExprNode() {
      if(check(TokenType::StringLiteral))
      {
        std::string text = peek().value;
        advance();
        return std::make_unique<StringLiteral>(text);
      }
        if (check(TokenType::Number)) {
            string text = peek().value;
            advance();
            try {
                int64_t v = stoll(text);
                return make_unique<IntLiteral>(v, Type::I32);
            } catch (...) {
                cerr << "Parse error: invalid integer literal '" << text << "'\n";
                return nullptr;
            }
        }

        if (check(TokenType::Identifier)) {
            string name = peek().value;
            advance();
            return make_unique<IdentExpr>(name);
        }

        cerr << "Syntax error: expected expression";
        if (!isAtEnd()) cerr << " near '" << peek().value << "'";
        cerr << "\n";
        return nullptr;
    }

    unique_ptr<Stmt> returnStatement() {
        if (!expect(TokenType::Return, "expected 'Return'")) return nullptr;
        auto e = parseExprNode();
        if (!e) return nullptr;
        if (!expect(TokenType::SemiColon, "expected ';' after 'Return'")) return nullptr;
        return make_unique<ReturnStmt>(std::move(e));
    }

    unique_ptr<Stmt> exitStatement() {
        if (!expect(TokenType::Exit, "expected 'Exit'")) return nullptr;
        if (!expect(TokenType::SemiColon, "expected ';' after 'Exit'")) return nullptr;
        return make_unique<ExitStmt>();
    }

    bool fetchReturnExpression() {
        if (!expect(TokenType::FetchReturn, "expected 'FetchReturn'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'FetchReturn'")) return false;
        if (!expect(TokenType::Identifier, "expected declared function name inside 'FetchReturn'")) return false;
        if (!expect(TokenType::CloseParen, "expected ')' after 'FetchReturn' argument")) return false;
        return true;
    }

    bool SizeOfStatement() {
        if (!expect(TokenType::SizeOf, "expected 'SizeOf")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'SizeOf")) return false;
        if (!check(TokenType::CloseParen)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression inside of 'SizeOf'";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
            while (match(TokenType::Comma)) {
                if (!expression()) {
                    cerr << "Syntax error: expected expression after ','";
                    if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                    cerr << endl;
                    return false;
                }
            }
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'SizeOf' arguments")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'SizeOf'")) return false;
        return true;
    }

    bool TypeOfStatement() {
        if (!expect(TokenType::TypeOf, "expected 'TypeOf'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'TypeOf'")) return false;
        if (!check(TokenType::CloseParen)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression inside 'TypeOf'";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
            while (match(TokenType::Comma)) {
                if (!expression()) {
                    cerr << "Syntax error: expected expression after ','";
                    if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                    cerr << endl;
                    return false;
                }
            }
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'TypeOf' arguments")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'TypeOf'")) return false;
        return true;
    }

    bool splitStatement() {
        if (!expect(TokenType::Split, "expected 'Split'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'Split'")) return false;
        if (!expect(TokenType::Identifier, "expected declared variable name inside of 'Split'")
        || !expect(TokenType::StringLiteral, "expected an string inside of 'Split'")) return false;
        if (!check(TokenType::CloseParen)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression inside of 'Split'";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'Split' argument")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'Split'")) return false;
        return true;
    }

    bool replaceStatement() {
        if (!expect(TokenType::Replace, "expected 'Replace'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'Replace'")) return false;
        if (check(TokenType::Identifier) || check(TokenType::StringLiteral)) {
            advance();
        } else {
            cerr << "Syntax error: expected identifier or string as first argument of 'Replace'";
            if (!isAtEnd()) cerr << " near '" << peek().value << "'";
            cerr << endl;
            return false;
        }
        if (!expect(TokenType::Comma, "expected ',' between 'Replace' arguments")) return false;
        if (check(TokenType::Identifier) || check(TokenType::StringLiteral)) {
            advance();
        } else {
            cerr << "Syntax error: expected identifier or string as second argument of 'Replace'";
            if (!isAtEnd()) cerr << " near '" << peek().value << "'";
            cerr << endl;
            return false;
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'Replace' arguments")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'Replace'")) return false;
        return true;
    }

    bool containsStatement() {
        if (!expect(TokenType::Contains, "expected 'Contains'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'Contains'")) return false;
        if (check(TokenType::Identifier) || check(TokenType::StringLiteral)) {
            advance();
        } else {
            cerr << "Syntax error: expected identifier or string as first argument of 'Contains'";
            if (!isAtEnd()) cerr << " near '" << peek().value << "'";
            cerr << endl;
            return false;
        }
        if (!expect(TokenType::Comma, "expected ',' between 'Contains' arguments")) return false;
        if (check(TokenType::Identifier) || check(TokenType::StringLiteral)) {
            advance();
        } else {
            cerr << "Syntax error: expected identifier or string as second argument of 'Contains'";
            if (!isAtEnd()) cerr << " near '" << peek().value << "'";
            cerr << endl;
            return false;
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'Contains' arguments")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'Contains'")) return false;
        return true;
    }

    bool startsWithStatement() {
        if (!expect(TokenType::StartsWith, "expected 'StartsWith'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'StartsWith'")) return false;
        if (!expect(TokenType::Identifier, "expected declared variable name inside of 'StartsWith'")
        || !expect(TokenType::String, "expected string inside of 'StartsWith'")) return false;
        if (!check(TokenType::CloseParen)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression inside of 'StartsWith'";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'StartsWith' argument")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'StartsWith'")) return false;
        return true;
    }

    bool endsWithStatement() {
        if (!expect(TokenType::EndsWith, "expected 'EndsWith")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'EndsWith'")) return false;
        if (!expect(TokenType::Identifier, "expected declared variable name inside of 'EndsWith'")
        || !expect(TokenType::String, "expected string inside of 'EndsWith'")) return false;
        if (!check(TokenType::CloseParen)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression inside of 'EndsWith'";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'EndsWith' argument")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'EndsWith'")) return false;
        return true;
    }

    bool toUpperStatement() {
        if (!expect(TokenType::ToUpper, "expected 'ToUpper'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'ToUpper'")) return false;
        if (!expect(TokenType::Identifier, "expected declared variable name inside of 'ToUpper'")
        || !expect(TokenType::String, "expected string inside of 'ToUpper'")) return false;
        if (!check(TokenType::CloseParen)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression inside of 'ToUpper'";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'ToUpper' argument")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'ToUpper'")) return false;
        return true;
    }

    bool toLowerStatement() {
        if (!expect(TokenType::ToLower, "expected 'ToLower'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'ToLower'")) return false;
        if (!expect(TokenType::Identifier, "expected declared variable name inside of 'ToLower'")
        || !expect(TokenType::String, "expected string inside of 'ToLower'")) return false;
        if (!check(TokenType::CloseParen)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression inside of 'ToLower'";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'ToLower' argument")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'ToLower'")) return false;
        return true;
    }

    bool trimStatement() {
        if (!expect(TokenType::Trim, "expected 'Trim'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'Trim'")) return false;
        if (!expect(TokenType::Identifier, "expected declared variable name inside of 'Trim'")
        || !expect(TokenType::String, "expected string inside of 'Trim'")) return false;
        if (!check(TokenType::CloseParen)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression inside of 'Trim'";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'Trim' argument")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'Trim'")) return false;
        return true;
    }

    bool indexOfStatement() {
        if (!expect(TokenType::IndexOf, "expected 'IndexOf'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'IndexOf'")) return false;
        if (!expect(TokenType::Identifier, "expected declared variable name inside of 'IndexOf'")
        || !expect(TokenType::String, "expected string inside of 'IndexOf'")) return false;
        if (!check(TokenType::CloseParen)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression inside of 'IndexOf'";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'IndexOf' argument")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'IndexOf'")) return false;
        return true;
    }

    bool formatStatement() {
        if (!expect(TokenType::Format, "expected 'Format'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'Format'")) return false;
        if (check(TokenType::Identifier)) {
            advance();
        } else if (check(TokenType::StringLiteral)) {
            advance();
        } else {
            cerr << "Syntax error: expected identifier or string inside of 'Format'";
            if (!isAtEnd()) cerr << " near '" << peek().value << "'";
            cerr << endl;
            return false;
        }
        if (!check(TokenType::CloseParen)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression inside of 'Format'";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'Format' argument")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'Format'")) return false;
        return true;
    }

    bool subStringStatement() {
        if (!expect(TokenType::SubString, "expected 'SubString'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'SubString")) return false;
        if (!expect(TokenType::Identifier, "expected declared variable name inside of 'SubString'")) return false;
        if (!check(TokenType::CloseParen)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression inside of 'SubString'";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
            while (match(TokenType::Comma)) {
                if (!expression()) {
                    cerr << "Syntax error: expected expression after ','";
                    if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                }
            }
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'SubString' arguments")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'SubString'")) return false;
        return true;
    }

    bool lengthStatement() {
        if (!expect(TokenType::Length, "expected 'Length'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'Length'")) return false;
        if (!expect(TokenType::Identifier, "expected declared variable name inside 'Length'")) return false;
        if (!check(TokenType::CloseParen)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression inside of 'Length'";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'Length' argument")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'Length'")) return false;
        return true;
    }

    bool writeFileStatement() {
        if (!expect(TokenType::WriteFile, "expected 'WriteFile'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'WriteFile'")) return false;
        if (!check(TokenType::CloseParen)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression inside 'WriteFile'";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
        }
        while (match(TokenType::Comma)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression after ','";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'WriteFile' arguments")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'WriteFile'")) return false;
        return true;
    }

    bool openStatement() {
        if (!expect(TokenType::Open, "expected 'Open'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'Open'")) return false;
        if (!check(TokenType::CloseParen)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression inside 'Open'";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
        }
        while (match(TokenType::Comma)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression after ','";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'Open' arguments")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'Open'")) return false;
        return true;
    }

    bool closeStatement() {
        if (!expect(TokenType::Close, "expected 'Close'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'Close'")) return false;
        if (check(TokenType::CloseParen)) {
            cerr << "Syntax error: 'Close' requires a handle argument" << endl;
            return false;
        }
        if (!expression()) {
            cerr << "Syntax error: expected expression inside 'Close'";
            if (!isAtEnd()) cerr << " near '" << peek().value << "'";
            cerr << endl;
            return false;
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'Close' handle")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'Close'")) return false;
        return true;
    }

    bool inputStatement() {
        if (!expect(TokenType::Input, "expected 'Input'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'Input'")) return false;
        if (!expect(TokenType::CloseParen, "expected ')' after 'Input'")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'Input'")) return false;
        return true;
    }

    bool readLineStatement() {
        if (!expect(TokenType::ReadLine, "expected 'ReadLine'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'ReadLine'")) return false;
        if (!expect(TokenType::CloseParen, "expected ')' after 'ReadLine'")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'ReadLine'")) return false;
        return true;
    }

    bool errorStatement() {
        if (!expect(TokenType::Error, "expected 'Error'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'Error'")) return false;
        if (check(TokenType::CloseParen)) {
            cerr << "Syntax error: 'Error' requires a message" << endl;
            return false;
        }
        if (!expression()) {
            cerr << "Syntax error: expected expression inside 'Error'";
            if (!isAtEnd()) cerr << " near '" << peek().value << "'";
            cerr << endl;
            return false;
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'Error' message")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'Error'")) return false;
        return true;
    }

    bool warnStatement() {
        if (!expect(TokenType::Warn, "expected 'Warn'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'Warn'")) return false;
        if (check(TokenType::CloseParen)) {
            cerr << "Syntax error: 'Warn' requires a message" << endl;
            return false;
        }
        if (!expression()) {
            cerr << "Syntax error: expected expression inside 'Warn'";
            if (!isAtEnd()) cerr << " near '" << peek().value << "'";
            cerr << endl;
            return false;
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'Warn' message")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'Warn'")) return false;
        return true;
    }

    bool readFileStatement() {
        if (!expect(TokenType::ReadFile, "expected 'ReadFile")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'ReadFile'")) return false;
        if (!check(TokenType::CloseParen)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression inside 'ReadFile'";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
        }
        while (match(TokenType::Comma)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression after ','";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'ReadFile' arguments")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'ReadFile'")) return false;
        return true;
    }

    bool appendFileStatement() {
        if (!expect(TokenType::AppendFile, "expected 'AppendFile")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'AppendFile'")) return false;
        if (!check(TokenType::CloseParen)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression inside 'AppendFile'";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
        }
        while (match(TokenType::Comma)) {
            if (!expression()) {
                cerr << "Syntax error: expected expression after ','";
                if (!isAtEnd()) cerr << " near '" << peek().value << "'";
                cerr << endl;
                return false;
            }
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'AppendFile' arguments")) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after 'AppendFile'")) return false;
        return true;
    }

    unique_ptr<Stmt> printStatement() {
        if (!expect(TokenType::Print, "expected 'Print'")) return nullptr;
        if (!expect(TokenType::OpenParen, "expected '(' after 'Print'")) return nullptr;
        
        std::vector<std::unique_ptr<Expr>> args;
        if (!check(TokenType::CloseParen)) {
            auto e = parseExprNode();
            if(!e) return nullptr;
            args.push_back(std::move(e));
            while (match(TokenType::Comma)) {
              auto e2 = parseExprNode();
              if(!e2) return nullptr;
              args.push_back(std::move(e2));
            }
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'Print' arguments")) return nullptr;
        if (!expect(TokenType::SemiColon, "expected ';' after 'Print'")) return nullptr;
        return make_unique<BuiltinCallStmt>("Print", std::move(args));
    }

  unique_ptr<Stmt> printlnStatement() {
        if (!expect(TokenType::PrintLn, "expected 'PrintLn'")) return nullptr;
        if (!expect(TokenType::OpenParen, "expected '(' after 'PrintLn'")) return nullptr;

        std::vector<std::unique_ptr<Expr>> args;
        if (!check(TokenType::CloseParen)) {
            auto e = parseExprNode();
            if(!e) return nullptr;
            args.push_back(std::move(e));
            while (match(TokenType::Comma)) {
                auto e2 = parseExprNode();
                if(!e2) return nullptr;
                args.push_back(std::move(e2));
            }
        }
        if (!expect(TokenType::CloseParen, "expected ')' after 'PrintLn' arguments")) return nullptr;
        if (!expect(TokenType::SemiColon, "expected ';' after 'PrintLn'")) return nullptr;
        return make_unique<BuiltinCallStmt>("PrintLn", std::move(args));
    }

    bool expression() {
        if (!term()) {
            return false;
        }

        while (!isAtEnd() &&
               (check(TokenType::BinaryOperator) ||
                check(TokenType::ComparisionOperator) ||
                check(TokenType::LogicalOperator))) {
            advance();

            if (!term()) {
                cerr << "Syntax error: expected expression after operator";
                if (!isAtEnd()) {
                    cerr << " near '" << peek().value << "'";
                }
                cerr << endl;
                return false;
            }
        }

        return true;
    }

    bool term() {
        if (!factor()) {
            return false;
        }

        while (!isAtEnd() && check(TokenType::BinaryOperator)) {
            string op = peek().value;

            if (op == "*" || op == "/" || op == "%") {
                advance();

                if (!factor()) {
                    cerr << "Syntax error: expected factor after '" << op << "'";
                    if (!isAtEnd()) {
                        cerr << " near '" << peek().value << "'";
                    }
                    cerr << endl;
                    return false;
                }
            } else {
                break;
            }
        }

        return true;
    }

    bool factor() {
        if (check(TokenType::FetchReturn)) {
            return fetchReturnExpression();
        }

        if (match(TokenType::StringLiteral)) return true;

        if (match(TokenType::Identifier)) {
            while (match(TokenType::Dot)) {
                if (!expect(TokenType::Identifier, "expected property after '.'")) {
                    return false;
                }
            }
            return true;
        }

        if (match(TokenType::Number)) return true;
        if (match(TokenType::Bool)) return true;

        if (match(TokenType::OpenParen)) {
            if (!expression()) return false;
            return expect(TokenType::CloseParen, "expected ')'");
        }

        return false;
    }
};

ParseResult parseTokens(const vector<Token>& tokens) {
    Parser parser(tokens);
    ParseResult result;
    result.success = parser.parse();
    if (result.success) {
        result.statements = parser.getAST();
    }
    return result;
}
