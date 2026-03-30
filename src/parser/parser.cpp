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

        cerr << "Syntax error: expected statement";
        if (!isAtEnd()) {
            cerr << " near '" << peek().value << "'";
        }
        cerr << endl;
        return false;
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
        if (match(TokenType::Number)) return true;
        if (match(TokenType::Identifier)) return true;
        if (match(TokenType::Bool)) return true;

        if (match(TokenType::OpenParen)) {
            if (!expression()) {
                return false;
            }

            if (!expect(TokenType::CloseParen, "expected ')' after expression")) {
                return false;
            }

            return true;
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