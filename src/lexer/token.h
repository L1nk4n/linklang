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
    Quotes,
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
    WriteFile,
    ReadFile,
    AppendFile,
    Input,
    ReadLine,
    Open,
    Close,
    Error,
    Warn,
    Print,
    PrintLn,
    FetchReturn,
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
    ToFloat,
    Parse,


    SubString,
    Split,
    Replace,
    Contains,
    StartsWith,
    EndsWith,
    ToUpper,
    ToLower,
    Trim,
    IndexOf,
    Format,



    Abs,
    Min,
    Max,
    Pow,

    Concat,

    Bool,
    Double,
    String,
    StringLiteral,
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
