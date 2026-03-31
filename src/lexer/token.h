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
    IncrementOperator,

    Let,
    Constant,
    Function,
    
    // BuiltIn
    Print,
    PrintLn,
    Return,
    Exit,
    TypeOf,
    SizeOf,
    Length,
    Push,
    Pop,
    Clear,
    ToString,
    ToInt,
    ToBool,

    Abs,
    Min,
    Max,
    Pow,

    Concat,
    SubString,

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
    Array,

    For,
    While,
    If,

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