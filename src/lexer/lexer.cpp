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
using namespace std;



enum TokenType
{
    Number,
    Identifier,
    Equals,
    OpenParen,
    CloseParen,
    OpenCurlyBrackets,
    CloseCurlyBrackets,
    BinaryOperator,
    Let,
    Constant,
    Function,
    Bool,
    Double,
    String,
    Float,
    Long,
    Class,
    Struct,
    Interface,
    Enum,
    Ui8,
    Ui16,
    Ui32,
    Ui64,
    Ui128,
    i8,
    i16,
    i32,
    i64,
    i128

};

struct Booleans {
    string True, False;
};

struct Token
{
    string value;
    TokenType type;
};

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
}

std::vector<std::string> splitString(const std::string &sourceCode)
{
    std::vector<std::string> words;
    std::string word;
     for (char ch : sourceCode)
    {
        if (!std::isspace(static_cast<unsigned char>(ch)))
        {
            word += ch;
        }
        else if (!word.empty())
        {
            words.push_back(word);
            word.clear();
        }
    }

    if (!word.empty())
    {
        words.push_back(word);
    }

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

bool isAlpha(const string &str) {
    for (char ch: str) {
        if (!isalpha(ch))
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
        else if(src.front() == "=")
        {
            tokens.push_back(token(shift(src), TokenType::Equals));
        }
        else if(src.front() == "function" || src.front() == "function()") {
            tokens.push_back(token(shift(src), TokenType::Function));
        }
        else if(src.front() == "Class") {
            tokens.push_back(token(shift(src), TokenType::Class));
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


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Incorrect arguments" << std::endl;
        std::cerr << "Correct usage: ./dejavu <input file path --> input.vu>" << std::endl;
        return EXIT_FAILURE;
    }

    {
        const char *ext = ".ll";
        size_t xlen = strlen(ext);
        size_t slen = strlen(argv[1]);
        int found = strcmp(argv[1] + slen - xlen, ext) == 0;
        if (found == 0)
        {
            std::cerr << "Invalid code file" << std::endl;
            return EXIT_FAILURE;
        }
    }

    INIT_RESERVED_IDENTIFIER();

    std::string sourceCode;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
        sourceCode = contents_stream.str();
    }

    std::vector<Token> tokens = tokenize(sourceCode);
    for (int i = 0; i < tokens.size(); ++i)
    {
        std::cout << "Value: " << tokens[i].value << "   Type: " << tokens[i].type << std::endl;
    }
}