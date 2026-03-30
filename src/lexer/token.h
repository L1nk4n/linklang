#pragma once

#include <string>

// Token types used by both lexer and parser
enum class TokenType
{
    Number,
    Identifier,

    Equals,

    OpenParen,
    CloseParen,
    OpenBracket,
    CloseBracket,
    OpenCurlyBrackets,
    CloseCurlyBrackets,

    Dot,
    Comma,
    Colon,
    SemiColon,

    LogicalOperator,
    ComparisionOperator,
    AssignmentForm,
    BinaryOperator,

    Let,
    Constant,
    Function,
    Def,

    Bool,
    Double,
    String,
    Float,
    Long,

    Class,
    Object,
    Struct,
    Interface,
    Enum,

    Ui8,
    Ui16,
    Ui32,
    Ui64,
    Ui128,

    I8,
    I16,
    I32,
    I64,
    I128
};

struct Token
{
    std::string value;
    TokenType type;
};