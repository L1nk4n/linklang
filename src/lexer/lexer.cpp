#include <cstdint>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>
#include <map>
#include <cctype>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <math.h>
#include "token.h"
using namespace std;



typedef std::map<std::string, TokenType> ReservedIdentMap;
ReservedIdentMap reservedIdent;

void INIT_RESERVED_IDENTIFIER()
{
    reservedIdent["let"] = TokenType::Let;
    reservedIdent["constant"] = TokenType::Constant;
    reservedIdent["const"] = TokenType::Constant;
    reservedIdent["double"] = TokenType::Double;
    reservedIdent["long"] = TokenType::Long; 
    reservedIdent["bool"] = TokenType::Bool;
    reservedIdent["true"] = TokenType::Bool;
    reservedIdent["false"] = TokenType::Bool;
    reservedIdent["Enum"] = TokenType::Enum;
    reservedIdent["Interface"] = TokenType::Interface;
    reservedIdent["Array"] = TokenType::Array;
    reservedIdent["for"] = TokenType::For;
    reservedIdent["while"] = TokenType::While;
    reservedIdent["i8"] = TokenType::I8;
    reservedIdent["i16"] = TokenType::I16;
    reservedIdent["i32"] = TokenType::I32;
    reservedIdent["i64"] = TokenType::I64;
    reservedIdent["i128"] = TokenType::I128;

    reservedIdent["ui8"] = TokenType::Ui8;
    reservedIdent["ui16"] = TokenType::Ui16;
    reservedIdent["ui32"] = TokenType::Ui32;
    reservedIdent["ui64"] = TokenType::Ui64;
    reservedIdent["ui128"] = TokenType::Ui128;
}

std::vector<std::string> splitString(const std::string &sourceCode)
{
    std::vector<std::string> words;
    std::string word;

    auto pushWord = [&]() {
        if (!word.empty()) {
            words.push_back(word);
            word.clear();
        }
    };

    for (size_t i = 0; i < sourceCode.size(); ++i)
    {
        unsigned char ch = sourceCode[i];

        if (std::isspace(ch))
        {
            pushWord();
            continue;
        }

        if (i + 1 < sourceCode.size())
        {
            std::string two = sourceCode.substr(i, 2);

            if (two == "++" || two == "==" || two == "!=" ||
                two == "<=" || two == ">=" || two == "+=" ||
                two == "-=" || two == "*=" || two == "/=" ||
                two == "&&" || two == "||")
            {
                pushWord();
                words.push_back(two);
                ++i;
                continue;
            }
        }

        if (ch == '(' || ch == ')' ||
            ch == '{' || ch == '}' ||
            ch == '[' || ch == ']' ||
            ch == '+' || ch == '-' ||
            ch == '*' || ch == '/' ||
            ch == '=' || ch == ';' ||
            ch == '<' || ch == '>' ||
            ch == '.' || ch == ',' ||
            ch == ':' || ch == '!')
        {
            pushWord();
            words.push_back(std::string(1, ch));
        }
        else
        {
            word += ch;
        }
    }

    pushWord();
    return words;
}

long long SHIFT_CURR = 0;
string shift(vector<string> &src)
{
    string current = src.front();
    src.erase(src.begin());
    return current;
}

bool isLong(const string &str) {
    if (str.empty()) return false;
    for (unsigned char ch : str) {
        if (!isdigit(ch)) return false;
    }
    try {
        stoll(str);
        return true;
    } catch(...) {
        return false;
    }
}

bool isUi8(const string &str) {
    if (str.empty())
        return false;
    for (unsigned char ch : str) {
        if (ch != '0' && ch != '1')
            return false;
    }
    try {
        size_t idx = 0;
        int value = stoi(str, &idx, 2);

        if (idx != str.size())
            return false;

        return value >= 0 && value <= 255;
    } catch (...) {
        return false;
    }
}

bool isUi16(const string &str) {
    if (str.empty())
        return false;
    for (unsigned char ch : str) {
        if (ch != '0' && ch != '1')
            return false;
    }
    try {
        size_t idx = 0;
        int value = stoi(str, &idx, 2);

        if (idx != str.size())
            return false;
        return value >= 0 && value <= 65535;
    } catch (...) {
        return false;
    }
}

bool isUi32(const string &str) {
    if (str.empty())
        return false;
    for (unsigned char ch : str) {
        if (ch != '0' && ch != '1')
            return false;
    }
    try {
        size_t idx = 0;
        int value = stoi(str, &idx, 2);

        if (idx != str.size())
            return false;
        return value >= 0 && value <= uint32_t(4294967295);
    } catch (...) {
        return false;
    }
}

bool isUi64(const string &str) {
    if (str.empty())
        return false;
    for (unsigned char ch : str) {
        if (ch != '0' && ch != '1')
            return false;
    }
    try {
        size_t idx = 0;
        int value = stoi(str, &idx, 2);

        if (idx != str.size())
            return false;
        return value >= 0 && value <= 18446744073709551615uLL;
    } catch(...) {
        return false;
    }
}

bool isUi128(const string &str) {
    if (str.empty())
        return false;
    for (unsigned char ch : str) {
        if (ch != '0' && ch != '1')
            return false;
    }
    try {
        size_t idx = 0;
        int value = stoi(str, &idx, 2);

        if (idx != str.size())
            return false;
        return value >= 0 && value <= pow(2, 128); 
    } catch (...) {
        return false;
    }
}

bool isi8(const string &str) {
    if (str.empty())
        return false;
    for(signed char ch : str) {
        if (ch != '0' && ch != '1')
            return false;
    }
    try {
        size_t idx = 0;
        int value = stoi(str, &idx, 2);

        if (idx != str.size())
            return false;
        return value <= 0 && value <= 255;
    } catch (...) {
        return false;
    }
}

bool isDouble(const std::string& str) {
    if (str.empty()) return false;

    bool seenDot = false;
    bool seenDigit = false;

    for (unsigned char ch : str) {
        if (std::isdigit(ch)) {
            seenDigit = true;
        } else if (ch == '.' && !seenDot) {
            seenDot = true;
        } else {
            return false;
        }
    }
    return seenDigit && seenDot;
}

bool isBool(const string &str) {
    return str == "true" || str == "false";
}

bool isNumber(const string &str) {
    for (unsigned char ch : str) {
        if (!isdigit(ch))
            return false;
    }
    return true;
}

bool isAlpha(const std::string &str) {
    if (str.empty()) return false;

    if (!std::isalpha((unsigned char)str[0]) && str[0] != '_')
        return false;

    for (size_t i = 1; i < str.size(); ++i) {
        unsigned char ch = (unsigned char)str[i];
        if (!std::isalnum(ch) && ch != '_')
            return false;
    }

    return true;
}

bool isClass(const string &str) {
    return str == "Class";
}

bool isStruct(const string &str) {
    return str == "Struct";
}

bool isArray(const string &str) {
    return str == "Array";
}

bool isSkippable(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\n';
}


void printRandom()
{
    cout << SHIFT_CURR << endl;
}

Token token(string value, TokenType tokentype) {
    return {value, tokentype};
}

vector<Token> tokenize(string &sourceCode)
{

    if(reservedIdent.empty())
    {
        INIT_RESERVED_IDENTIFIER();
    }
    vector<Token> tokens;
    vector<string> src = splitString(sourceCode);
    while (!src.empty()){
        if(src.front() == "(")
        {
            tokens.push_back(token(shift(src), TokenType::OpenParen));
        }
        else if(src.front() == ")")
        {
            tokens.push_back(token(shift(src), TokenType::CloseParen));
        }
        else if(src.front() == ";")
        {
            tokens.push_back(token(shift(src), TokenType::SemiColon));
        }
        else if(src.front() == "[")
        {
            tokens.push_back(token(shift(src), TokenType::OpenBracket));
        }
        else if(src.front() == "]")
        {
            tokens.push_back(token(shift(src), TokenType::CloseBracket));
        }
        else if(src.front() == "{")
        {
            tokens.push_back(token(shift(src), TokenType::OpenCurlyBrackets));
        }
        else if(src.front() == "}")
        {
            tokens.push_back(token(shift(src), TokenType::CloseCurlyBrackets));
        }
        else if(src.front() == "+" || src.front() == "-" || src.front() == "*" || src.front() == "/")
        {
            tokens.push_back(token(shift(src), TokenType::BinaryOperator));
        }
        else if(src.front() == "++")
        {
            tokens.push_back(token(shift(src), TokenType::IncrementOperator));
        }
        else if (src.front() == "+=" || src.front() == "-=" || src.front() == "*=" || src.front() == "/="
        || src.front() == "%=") {
            tokens.push_back(token(shift(src), TokenType::AssignmentForm));
        }
        else if (src.front() == "==" || src.front() == "!=" || src.front() == "<" || src.front() == ">"
        || src.front() == "<=" || src.front() == ">=")
        {
            tokens.push_back(token(shift(src), TokenType::ComparisionOperator));
        }
        else if (src.front() == "&&" || src.front() == "||" || src.front() == "!")
        {
            tokens.push_back(token(shift(src), TokenType::LogicalOperator));
        }
        else if(src.front() == ".")
        {
            tokens.push_back(token(shift(src), TokenType::Dot));
        }
        else if(src.front() == "=")
        {
            tokens.push_back(token(shift(src), TokenType::Equals));
        }
        else if(src.front() == "Function" || src.front() == "function()") {
            tokens.push_back(token(shift(src), TokenType::Function));
        }
        else if(src.front() == "Class") {
            tokens.push_back(token(shift(src), TokenType::Class));
        }
        else if(src.front() == "Enum")
        {
            tokens.push_back(token(shift(src), TokenType::Enum));
        }
        else if(src.front() == "Array")
        {
            tokens.push_back(token(shift(src), TokenType::Array));
        }
        else if(src.front() == "Struct") {
            tokens.push_back(token(shift(src), TokenType::Struct));
        }
        else {
            if (isNumber(src.front()))
            {
                string number;
                while(!src.empty() && isNumber(src.front()))
                {
                    number += shift(src);
                }
                tokens.push_back(token(number, TokenType::Number));
            }
            else if (isAlpha(src.front()))
            {
                std::string ident = shift(src);

                if (isBool(ident)) {
                    tokens.push_back(token(ident, TokenType::Bool));
                } else {
                    auto it = reservedIdent.find(ident);
                    if (it != reservedIdent.end())
                        tokens.push_back(token(ident, it->second));
                    else
                        tokens.push_back(token(ident, TokenType::Identifier));
                }
            } 
            else if (isSkippable(src.front()[0]))
            {
                shift(src);
            }
            else
            {
                std::cout << "Unrecognized character found! " << std::endl;
                exit(1);
            }
        }
    }

    return tokens;
}
