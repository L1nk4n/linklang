#include <cstdlib>
#include <iostream>
#include <locale>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstring>

#include "../lexer/token.h"

using namespace std;
extern void INIT_RESERVED_IDENTIFIER();

vector<Token> tokenize(string &sourceCode);

class Parser {
private:
    const vector<Token>& tokens;
    size_t current;

public:
    Parser(const vector<Token>& toks) : tokens(toks), current(0) {}

    bool parse() {
        while (!isAtEnd()) {
            if (!statement()) {
                return false;
            }
        }
        return true;
    }

private:
    bool isAtEnd() const {
        return current >= tokens.size();
    }

    const Token& peek() const {
        return tokens[current];
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

    bool statement() {
        if(check(TokenType::Let)) {
            return letDeclaration();
        }

        if(check(TokenType::Function))
        {
            return functionDeclaration();
        }

        if(check(TokenType::Class))
        {
            return classDeclaration();
        }

        if(check(TokenType::Enum))
        {
            return enumDeclaration();
        }

        if (isTypeToken(peek().type)) {
            return typedDeclaration();
        }

        if(check(TokenType::Struct))
        {
            return structDeclaration();
        }

        if(check(TokenType::Length))
        {
            return lengthStatement();
        }

        if(check(TokenType::For))
        {
            return forStatement();
        }

        if(check(TokenType::Print))
        {
            return printStatement();
        }

        if(check(TokenType::Length))
        {
            return lengthStatement();
        }
        
        if(check(TokenType::TypeOf))
        {
            return TypeOfStatement();
        }

        if(check(TokenType::SizeOf))
        {
            return SizeOfStatement();
        }

        if(check(TokenType::Return))
        {
            return returnStatement();
        }

        if(check(TokenType::WriteFile))
        {
          return writeFileStatement();
        }

        if(check(TokenType::Warn))
        {
          return warnStatement();
        }
        
        if(check(TokenType::Error))
        {
          return errorStatement();
        }

        if(check(TokenType::Close))
        {
          return closeStatement();
        }

        if(check(TokenType::Open))
        {
          return openStatement();
        }

        if(check(TokenType::ReadLine))
        {
          return readLineStatement();
        }

        if(check(TokenType::Input))
        {
          return inputStatement();
        }

        if(check(TokenType::ReadFile))
        {
          return readFileStatement();
        }

        if(check(TokenType::AppendFile))
        {
          return appendFileStatement();
        }

        if(check(TokenType::PrintLn))
        {
            return printlnStatement();
        }

        if(check(TokenType::Exit))
        {
            return exitStatement();
        }

        if(check(TokenType::While))
        {
            return whileStatement();
        }

        cerr << "Syntax error: expected statement";
        if (!isAtEnd()) {
            cerr << " near '" << peek().value << "'";
        }
        cerr << endl;
        return false;
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

    bool returnStatement()
    {
        if(!expect(TokenType::Return, "expected 'Return")) return false;
        if(!expression()) {
            cerr << "Syntax error: expected expression after 'Return'";
            if(!isAtEnd())
            {
                cerr << " near '" << peek().value << "'";
            }
            cerr << endl;
            return false;
        }
        if(!expect(TokenType::SemiColon, "expected ';' after 'Return'")) return false;
        return true;
    }

    bool exitStatement()
    {
        if(!expect(TokenType::Exit, "expected 'Exit'")) return false;
        if(!expect(TokenType::SemiColon, "expected ';' after 'Exit'")) return false;
        return true;
    }

    bool fetchReturnExpression()
    {
        if(!expect(TokenType::FetchReturn, "expected 'FetchReturn'")) return false;
        if(!expect(TokenType::OpenParen, "expected '(' after 'FetchReturn'")) return false;
        if(!expect(TokenType::Identifier, "expected declared function name inside 'FetchReturn'")) return false;
        if(!expect(TokenType::CloseParen, "expected ')' after 'FetchReturn' argument")) return false;
        return true; 
    }


    bool SizeOfStatement()
    {
        if(!expect(TokenType::SizeOf, "expected 'SizeOf")) return false;
        if(!expect(TokenType::OpenParen, "expected '(' after 'SizeOf")) return false;
        if(!check(TokenType::CloseParen)) 
        {
            if(!expression())
            {
                cerr << "Syntax error: expected expression inside of 'SizeOf'";
                if(!isAtEnd())
                {
                    cerr << " near '" << peek().value << "'";
                }
                cerr << endl;
                return false;
            }
            while(match(TokenType::Comma))
            {
                if(!expression())
                {
                    cerr << "Syntax error: expected expression after ','";
                    if(!isAtEnd())
                    {
                        cerr << " near '" << peek().value << "'";
                    }
                    cerr << endl;
                    return false;
                }
            }
        }
        if(!expect(TokenType::CloseParen, "expected ')' after 'Print' arguments")) return false;
        if(!expect(TokenType::SemiColon, "expected ';' after 'Print'")) return false;
        return true;
    }

    bool TypeOfStatement()
    {
        if(!expect(TokenType::TypeOf, "expected 'TypeOf'")) return false;
        if(!expect(TokenType::OpenParen, "expected '(' after 'TypeOf'")) return false;
        if(!check(TokenType::CloseParen))
        {
            if(!expression())
            {
                cerr << "Syntax error: expected expression inside 'TypeOf'";
                if(!isAtEnd())
                {
                    cerr << " near '" << peek().value << "'";
                }
                cerr << endl;
                return false;
            }
            while(match(TokenType::Comma))
            {
                if(!expression())
                {
                    cerr << "Syntax error: expected expression after ','";
                    if(!isAtEnd())
                    {
                        cerr << " near '" << peek().value << "'";
                    }
                    cerr << endl;
                    return false;
                }
            }
        }
        if(!expect(TokenType::CloseParen, "expected ')' after 'Print' arguments")) return false;
        if(!expect(TokenType::SemiColon, "expected ';' after 'Print'")) return false;
        return true;
    }

    bool lengthStatement()
    {
        if(!expect(TokenType::Length, "expected 'Length'")) return false;
        if(!expect(TokenType::OpenParen, "expected '(' after 'Length'")) return false;
        if(!expect(TokenType::Identifier, "expected declared variable name inside 'Length'")) return false;
        if(!check(TokenType::CloseParen))
        {
            if(!expression())
            {
                cerr << "Syntax error: expected expression inside of 'Length'";
                if(!isAtEnd())
                {
                    cerr << " near '" << peek().value << "'";
                }
                cerr << endl;
                return false;
            }
            
        }
        if(!expect(TokenType::CloseParen, "expected ')' after 'Length' argument")) return false;
        if(!expect(TokenType::SemiColon, "expected ';' after 'Length'")) return false;
        return true;
    }

    bool writeFileStatement()
    {
      if(!expect(TokenType::WriteFile, "expected 'WriteFile'")) return false;
      if(!expect(TokenType::OpenParen, "expected '(' after 'WriteFile'")) return false;
      if(!check(TokenType::CloseParen))
      {
        if(!expression())
        {
          cerr << "Syntax error: expected expression inside 'WriteFile'";
          if(!isAtEnd())
          {
            cerr << " near '" << peek().value << "'";
          }
          cerr << endl;
          return false;
        }
      }
      while(match(TokenType::Comma))
      {
        if(!expression())
        {
          cerr << "Syntax error: expected expression after ','";
          if(!isAtEnd())
          {
            cerr << " near '" << peek().value << "'";
          }
          cerr << endl;
          return false;
        }
      }
      if(!expect(TokenType::CloseParen, "expected ')' after 'WriteFile' arguments")) return false;
      if(!expect(TokenType::SemiColon, "expected ';' after 'WriteFile'")) return false;
      return true;
    }

    
    bool openStatement()
    {
      if(!expect(TokenType::Open, "expected 'Open'")) return false;
      if(!expect(TokenType::OpenParen, "expected '(' after 'Open'")) return false;
      if(!check(TokenType::CloseParen))
      {
        if(!expression())
        {
          cerr << "Syntax error: expected expression inside 'Open'";
          if(!isAtEnd())
          {
            cerr << " near '" << peek().value << "'";
          }
          cerr << endl;
          return false;
        }
      }
      while(match(TokenType::Comma))
      {
        if(!expression())
        {
          cerr << "Syntax error: expected expression after ','";
          if(!isAtEnd())
          {
            cerr << " near '" << peek().value << "'";
          }
          cerr << endl;
          return false;
        }
      }
      if(!expect(TokenType::CloseParen, "expected ')' after 'Open' arguments")) return false;
      if(!expect(TokenType::SemiColon, "expected ';' after 'Open'")) return false;
      return true;
    }

    
    bool closeStatement()
    {
      if(!expect(TokenType::Close, "expected 'Close'")) return false;
      if(!expect(TokenType::OpenParen, "expected '(' after 'Close'")) return false;
      if(check(TokenType::CloseParen))
      {
        cerr << "Syntax error: 'Close' requires a handle argument" << endl;
        return false;
      }
      if(!expression())
      {
        cerr << "Syntax error: expected expression inside 'Close'";
        if(!isAtEnd()) cerr << " near '" << peek().value << "'";
        cerr << endl;
        return false;
      }
      if(!expect(TokenType::CloseParen, "expected ')' after 'Close' handle")) return false;
      if(!expect(TokenType::SemiColon, "expected ';' after 'Close'")) return false;
      return true;
    }

    bool inputStatement()
    {
      if(!expect(TokenType::Input, "expected 'Input'")) return false;
      if(!expect(TokenType::OpenParen, "expected '(' after 'Input'")) return false;
      if(!expect(TokenType::CloseParen, "expected ')' after 'Input'")) return false;
      if(!expect(TokenType::SemiColon, "expected ';' after 'Input'")) return false;
      return true;
    }

    bool readLineStatement()
    {
      if(!expect(TokenType::ReadLine, "expected 'ReadLine'")) return false;
      if(!expect(TokenType::OpenParen, "expected '(' after 'ReadLine'")) return false;
      if(!expect(TokenType::CloseParen, "expected ')' after 'ReadLine'")) return false;
      if(!expect(TokenType::SemiColon, "expected ';' after 'ReadLine'")) return false;
      return true;
    }

    bool errorStatement()
    {
      if(!expect(TokenType::Error, "expected 'Error'")) return false;
      if(!expect(TokenType::OpenParen, "expected '(' after 'Error'")) return false;
      if(check(TokenType::CloseParen))
      {
        cerr << "Syntax error: 'Error' requires a message" << endl;
        return false;
      }

      if(!expression())
      {
        cerr << "Syntax error: expected expression inside 'Error'";
        if(!isAtEnd()) cerr << " near '" << peek().value << "'";
        cerr << endl;
        return false;
      }

      if(!expect(TokenType::CloseParen, "expected ')' after 'Error' message")) return false;
      if(!expect(TokenType::SemiColon, "expected ';' after 'Error'")) return false;
      return true;
    }

    bool warnStatement()
    {
      if(!expect(TokenType::Warn, "expected 'Warn'")) return false;
      if(!expect(TokenType::OpenParen, "expected '(' after 'Warn'")) return false;
      if(check(TokenType::CloseParen))
      {
        cerr << "Syntax error: 'Warn' requires a message" << endl;
        return false;
      }
      if(!expression())
      {
        cerr << "Syntax error: expected expression inside 'Warn'";
        if(!isAtEnd()) cerr << " near '" << peek().value << "'";
        cerr << endl;
        return false;
      }
      if(!expect(TokenType::CloseParen, "expected ')' after 'Warn' message")) return false;
      if(!expect(TokenType::SemiColon, "expected ';' after 'Warn'")) return false;
      return true;
    }
    
    bool readFileStatement()
    {
      if(!expect(TokenType::ReadFile, "expected 'ReadFile")) return false;
      if(!expect(TokenType::OpenParen, "expected '(' after 'ReadFile'")) return false;
      if(!check(TokenType::CloseParen))
      {
        if(!expression())
        {
          cerr << "Syntax error: expected expression inside 'ReadFile'";
          if(!isAtEnd())
          {
            cerr << " near '" << peek().value << "'";
          }
          cerr << endl;
          return false;
        }
      }
      while(match(TokenType::Comma))
      {
        if(!expression())
        {
          cerr << "Syntax error: expected expression after ','";
          if(!isAtEnd())
          {
            cerr << " near '" << peek().value << "'";
          }
          cerr << endl;
          return false;
        }
      }
      if(!expect(TokenType::CloseParen, "expected ')' after 'ReadFile' arguments")) return false;
      if(!expect(TokenType::SemiColon, "expected ';' after 'ReadFile'")) return false;
      return true;
    }

    
    bool appendFileStatement()
    {
      if(!expect(TokenType::AppendFile, "expected 'AppendFile")) return false;
      if(!expect(TokenType::OpenParen, "expected '(' after 'AppendFile'")) return false;
      if(!check(TokenType::CloseParen))
      {
        if(!expression())
        {
          cerr << "Syntax error: expected expression inside 'AppendFile'";
          if(!isAtEnd())
          {
            cerr << " near '" << peek().value << "'";
          }
          cerr << endl;
          return false;
        }
      }
      while(match(TokenType::Comma))
      {
        if(!expression())
        {
          cerr << "Syntax error: expected expression after ','";
          if(!isAtEnd())
          {
            cerr << " near '" << peek().value << "'";
          }
          cerr << endl;
          return false;
        }
      }
      if(!expect(TokenType::CloseParen, "expected ')' after 'AppendFile' arguments")) return false;
      if(!expect(TokenType::SemiColon, "expected ';' after 'AppendFile'")) return false;
      return true;
    }

    bool printStatement()
    {
        if(!expect(TokenType::Print, "expected 'Print'")) return false;
        if(!expect(TokenType::OpenParen, "expected '(' after 'Print'")) return false;
        if(!check(TokenType::CloseParen))
        {
            if(!expression()){
                cerr << "Syntax error: expected expression inside 'Print'";
                if(!isAtEnd())
                {
                    cerr << " near '" << peek().value << "'";
                }
                cerr << endl;
                return false;
            }
            while(match(TokenType::Comma))
            {
                if(!expression())
                {
                    cerr << "Syntax error: expected expression after ','";
                    if(!isAtEnd())
                    {
                        cerr << " near '" << peek().value << "'";
                    }
                    cerr << endl;
                    return false;
                }
            }
        }
        if(!expect(TokenType::CloseParen, "expected ')' after 'Print' arguments")) return false;
        if(!expect(TokenType::SemiColon, "expected ';' after 'Print'")) return false;
        return true;
    }

    bool printlnStatement()
    {
        if(!expect(TokenType::PrintLn, "expected 'Print'")) return false;
        if(!expect(TokenType::OpenParen, "expected '(' after 'Print'")) return false;
        if(!check(TokenType::CloseParen))
        {
            if(!expression()){
                cerr << "Syntax error: expected expression inside Print";
                if(!isAtEnd())
                {
                    cerr << " near '" << peek().value << "'";
                }
                cerr << endl;
                return false;
            }
            while(match(TokenType::Comma))
            {
                if(!expression())
                {
                    cerr << "Syntax error: expected expression after ','";
                    if(!isAtEnd())
                    {
                        cerr << " near '" << peek().value << "'";
                    }
                    cerr << endl;
                    return false;
                }
            }
        }
        if(!expect(TokenType::CloseParen, "expected ')' after Print arguments")) return false;
        if(!expect(TokenType::SemiColon, "expected ';' after Print")) return false;
        return true;
    }

    bool forStatement()
    {
        if(!expect(TokenType::For, "expected 'for'")) return false;
        if(!expect(TokenType::OpenParen, "expected '(' after 'for'")) return false;
        if(!type()) return false;
        if(!expect(TokenType::Identifier, "expected variable name")) return false;
        if(!expect(TokenType::Equals, "expected '=' in initialization")) return false;
        if(!expression()) return false;
        if(!expect(TokenType::SemiColon, "expected ';' after init")) return false;
        if(!expression()) return false;
        if (!expect(TokenType::SemiColon, "expected ';' after condition")) return false;
        if (!expect(TokenType::Identifier, "expected variable in increment")) return false;
        if (!expect(TokenType::IncrementOperator, "expected '++'")) return false;
        if (!expect(TokenType::CloseParen, "expected ')'")) return false;
        return block();
    }

    bool whileStatement()
    {
        if (!expect(TokenType::While, "expected 'while'")) return false;
        if (!expect(TokenType::OpenParen, "expected '(' after 'while'")) return false;

        if (!expression()) {
            cerr << "Syntax error: expected condition in while";
            if (!isAtEnd()) {
                cerr << " near '" << peek().value << "'";
            }
            cerr << endl;
            return false;
        }

        if (!expect(TokenType::CloseParen, "expected ')' after condition")) return false;

        return block();
    }

    bool structDeclaration() {
        if(!expect(TokenType::Struct, "expected 'Struct'"))
        {
            return false;
        }
        if(!expect(TokenType::Identifier, "expected identifier after 'Struct'"))
        {
            return false;
        }
        return block();
    }

    bool typedDeclaration()
    {
        if (!type()) {
            return false;
        }

        if (!expect(TokenType::Identifier, "expected identifier after type")) {
            return false;
        }

        if (!expect(TokenType::Equals, "expected '=' after identifier")) {
            return false;
        }

        if (!expression()) {
            cerr << "Syntax error: expected expression after '='";
            if (!isAtEnd()) {
                cerr << " near '" << peek().value << "'";
            }
            cerr << endl;
            return false;
        }

        if (!expect(TokenType::SemiColon, "expected ';' after typed declaration")) {
            return false;
        }

        return true;
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


    bool enumDeclaration() {
        if(!expect(TokenType::Enum, "expected 'Enum'"))
        {
            return false;
        }
        if(!expect(TokenType::Identifier, "expected identifier after 'Enum'"))
        {
            return false;
        }
        return block();
    }

    bool classDeclaration() {
        if(!expect(TokenType::Class, "expected 'Class'"))
        {
            return false;
        }
        if(!expect(TokenType::Identifier, "expected identifier after 'Class'"))
        {
            return false;
        }
        return block();
    }

    bool functionDeclaration() {
        if (!expect(TokenType::Function, "expected 'Function'")) {
            return false;
        }

        if (!expect(TokenType::Identifier, "expected identifier after 'Function'")) {
            return false;
        }

        return block();
    }

    bool arrayDeclaration()
    {
        if(!expect(TokenType::Array, "expected 'Array'")) {
            return false;
        }
        if (!expect(TokenType::Identifier, "expected identifier after 'Array'"))
        {
            return false;
        }
        return block();
    }

    bool forLoop() {
        if(!expect(TokenType::OpenCurlyBrackets, "expected '{' to start 'for' loop"))
        {
            return false;
        }
        while(!isAtEnd() && !check(TokenType::CloseCurlyBrackets))
        {
            if(!statement())
            {
                return false;
            }
        }
        if(!expect(TokenType::CloseCurlyBrackets, "expected '}' to close 'for' loop"))
        {
            return false;
        }
        return true;
    }

    bool block() {
        if(!expect(TokenType::OpenCurlyBrackets, "expected '{' to start block"))
        {
            return false;
        }
        while(!isAtEnd() && !check(TokenType::CloseCurlyBrackets))
        {
            if(!statement())
            {
                return false;
            }
        }
        if(!expect(TokenType::CloseCurlyBrackets, "expected '}' to close block"))
        {
            return false;
        }
        return true;
    }

    bool letDeclaration() {
        if (!expect(TokenType::Let, "expected 'let'")) {
            return false;
        }

        if (!expect(TokenType::Identifier, "expected identifier after 'let'")) {
            return false;
        }

        if (!expect(TokenType::Equals, "expected '=' after identifier")) {
            return false;
        }

        if (!expression()) {
            cerr << "Syntax error: expected expression after '='";
            if (!isAtEnd()) {
                cerr << " near '" << peek().value << "'";
            }
            cerr << endl;
            return false;
        }

        if (!expect(TokenType::SemiColon, "expected ';' after let declaration")) {
            return false;
        }

        return true;
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

int main(int argc, char *argv[])
{
    INIT_RESERVED_IDENTIFIER();
    if (argc != 2) {
        cerr << "Incorrect arguments" << endl;
        cerr << "Correct usage: ./parser <input file path>" << endl;
        return EXIT_FAILURE;
    }

    {
        const char *ext = ".ll";
        size_t xlen = strlen(ext);
        size_t slen = strlen(argv[1]);

        if (slen < xlen || strcmp(argv[1] + slen - xlen, ext) != 0) {
            cerr << "Invalid code file" << endl;
            return EXIT_FAILURE;
        }
    }

    string sourceCode;
    {
        stringstream contents_stream;
        fstream input(argv[1], ios::in);

        if (!input.is_open()) {
            cerr << "Failed to open file: " << argv[1] << endl;
            return EXIT_FAILURE;
        }

        contents_stream << input.rdbuf();
        sourceCode = contents_stream.str();
    }

    INIT_RESERVED_IDENTIFIER();

    vector<Token> tokens = tokenize(sourceCode);

    if (tokens.empty()) {
        cerr << "No tokens produced by lexer" << endl;
        return EXIT_FAILURE;
    }

    Parser parser(tokens);

    if (parser.parse()) {
        cout << "Parse success" << endl;
        return EXIT_SUCCESS;
    }

    cout << "Parse failed" << endl;
    return EXIT_FAILURE;
}
