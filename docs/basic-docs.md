# linklang docs

This is docs for the people who might want to contribute, there will be information how you will be able to create a pull request, and how you will be able to contribute to this project. I've tried to write this as trivival as possible but please contact me if there is improvments to make or syntax errors.

## Project Structure
  linklang/
  ├── src/
  │   ├── main.cpp              — Entry point. Validates .ll file extension, reads
  source, runs lexer → parser → semantic analyzer
  │   ├── lexer/
  │   │   ├── token.h            — Defines TokenType enum (all token kinds) and Token
  struct {value, type}
  │   │   ├── token.cpp          — (empty)
  │   │   ├── lexer.h            — (empty)
  │   │   └── lexer.cpp          — Lexer implementation. Splits source into tokens
  │   ├── parser/
  │   │   ├── parser.h           — Declares ParseResult struct and parseTokens()
  function
  │   │   └── parser.cpp         — Parser class. Converts token stream into AST
  (statements)
  │   ├── semantic/
  │   │   ├── semantic.h         — Defines AST node types (Stmt/Expr), Type enum, Scope,
   Symbol, SemanticAnalyzer
  │   │   └── semantic.cpp       — Semantic analysis. Scope management, duplicate
  detection, type tracking
  │   ├── common/                — (empty, reserved for shared utilities)
  │   └── runtime/               — (empty, reserved for future runtime/interpreter)
  ├── tests/
  │   ├── syntax_check.ll        — Tests basic syntax: functions, classes, enums,
  structs, for/while, builtins
  │   └── functions_check.ll     — Tests built-in function calls and FetchReturn
  ├── build/                     — (empty, for compiled output)
  ├── docs/                      — (empty, reserved for documentation)
  ├── examples/                  — (empty, reserved for example .ll programs)
  ├── include/                   — (empty, reserved for public headers)
  └── README.md                  — Points to docs website

---
## All functions
src/main.cpp

| Function | What it does
|-------------------|---------------------------------------------------------------------------------------------------------|
|`main(argc, argv)` | Validates CLI args, checks `.ll` extension, reads source file, runs lexer -> parser -> semantic pipeline

src/lexer/lexer.cpp
| Function | What it does
|-------------------|---------------------------------------------------------------------------------------------------------|
| `INIT_RESERVED_IDENTIFIER()` | Populates the reservedIdent map with all keywords and built-in function names, their TokenType
| `splitString(sourceCode)` | Splits raw source code into word tokens, handling string literals, two-char operators (`++, ==, !=, <=, >=, +=, -=, *=, /=, &&, ||`), and single-char punctuation
| `shift(src)` | Pops and returns the first element from a string vector
| `isStringLiteral(str)` | Returns true if string strarts and ends with `"`
| `isLong(str)` | Returns true if string is valid long long integer
| `isUi8(str)` | Returns true if string is a binary value fitting in unsigned 8-bit (0-255)
| `isUi16(str)` | Returns true if string is a binary value fitting in unsigned 16-bit (0-65535)
| `isUi32(str)` | Returns true if string is a binary value fitting in unsigned 32-bit
| `isUi64(str)` | Returns true if string is a binary value fitting in unsigned 64-bit
| `isUi128(str)` | Returns true if string is a binary value fitting in unsigned 128-bit
| `isi8(str)` | Returns true if string is a binary value fitting in signed 8-bit
| `isDouble(str)` | Returns true if string is a decimal number with a dot
| `isBool(str)` | Returns true if string is `"true"` or `"false"`
| `isNumber(str)` | Returns true if string contains only digits
| `isAlpha(str)` | Returns true if string is a valid identifier (starts with letter/underscore, then alphanumeric/uynderscore)
| `isClass(str)` | Returns true if string is `"Class"`
| `isStruct(str)` | Returns true if string is `"Struct"`
| `isArray(str)` | Returns true if string is `"Array"`
| `isSkippable(ch)` | Returns true if char is space, tab, or newline
| `token(value, tokentype)` | Constructs and returns a Token struct
| `tokenize(sourceCode)` | Main lexer function. Converts source code strinbg into a vector<Token>


src/lexer/token.h

Definition: enum class TokenType
What it does: All token kinds: literals (Number, StringLiteral, Bool), types (String, Double, Long, I8-I128, Ui8-Ui128), operators (BinaryOperator, ComparisionOperator, LogicalOperator, AssignmentForm, IncrementOperator), punctuation, (OpenParen, CloseParen, etc.), keywords (Let, Constant, Function, Class, Struct, Enum, For, While, If, Return, Exit), built-in functions (Print, PrintLn, WriteFile, ReadFile, AppendFile, Input, ReadLine, Open, Close, Error, Warn, TypeOf, SizeOf, Length, Push, Pop, Clear, ToString, ToInt, ToBool, FetchReturn, SubString, Split, Replace, Contains, StartsWith, EndsWith, ToUpper, ToLower, Trim, IndexOf, Format, Abs, Min, Max, Pow, Concat)
--------------------
Definition: struct Token
What it does: Holds a value (string) and type (TokenType)

src/parser/parser.cpp - class Parser

|Method | What it does
|-------------------|---------------------------------------------------------------------------------------------------------|
| `Parser(toks)` | Constructor. Takes token vector, sets cursor to 0
| `parse()` | Main loop. Parses all statements until end of tokens. Returns true/false
| `getAST()` | Returns the parsed AST as `vector<unique_ptr<Stmt>>`
| `isAtEnd()` | Returns true if cursor is past the end
| `peek()` | Returns current token without advancing
| `previous()` | Returns the token before the current cursor
| `check(type)` | Returns true if current token matches the given type
| `match(type)` | If current token matches, advances and returns true
| `advance()` | Moves cursor forward by one
| `expect(type, message)` | Like match but prints error on failure
| `isTypeToken(type)` | Returns true if toklen is a type keyword (String, Bool, Double, Float, Long, I8-I128, Ui8-Ui128)
| `tokenTypeToType(tt)` | Converts a TokenType to the semantic Type enum
| `parseStatement` | Dispatches the correct parse function based on current token
| `block(stmts)` | Parses `{ ... }` block, collecting statements
| `type()` | Matches and consumes a type token
| `letDeclaration()` | parses `let <name> = <expr>;`
| `typedDeclaration()` | Parses `<type> <name> = <expr>;`
| `functionDeclaration()` | Parses `Function <name> { ... }`
| `classDeclaration()` | Parses `Class <name> { ... }`
| `enumDeclaration()` | Parses `Enum <name> { ... }`
| `structDeclaration()` | Parses `Struct <name> { ... }`
| `forStatement()` | Parses `For (<type> <var> = <expr>; <cond>; <var>++) { ... }`
| `whileStatement()` | Parses `While (<expr>) { ... }`
| `returnStatement()` | Parses `Return <expr>;`
| `exitStatement()` | Parses `Exit;`
| `fetchReturnExpression()` | Parses `FetchReturn(<functionName>)`
| `printStatement()` | Parses `Print(<args>);`
| `printlnStatement()` | Parses `PrintLn(<args>);`
| `TypeOfStatement()` | Parses `TypeOf(<args>);`
| `SizeOfStatement()` | Parses `SizeOf(<args>):`
| `lengthStatement()` | Parses `Length(<identifier>);`
| `writeFileStatement()` | Parses `WriteFile(<args>);`
| `readFileStatement()` | Parses `ReadFile(<args>);`
| `appendFileStatement()` | Parses `AppendFile(<args>);`
| `openStatement()` | Parses `Open(<args>);`
| `closeStatement()` | Parses `Close(<handle>);` - requires exactly one argument
| `inputStatement()` | Parses `Input();` - no arguments
| `readLineStatement()` | Parses `ReadLine();` - no arguments
| `errorStatement()` | Parses `Error(<msg>)` - requires a message
| `warnStatement()` | Parses `Warn(<msg>)` - requires a message
| `subStringStatement()` | Parses `SubString(<identifier>, ...);`
| `splitStatement()` | Parses `Split(<identifier>, <string>);`
| `replaceStatement()` | Parses `Replace(<target>, <replacement>);`
| `containsStatement()` | Parses `Contains(<haystack>, <needle>);`
| `startsWithStatement()` | Parses `StartsWith(<identifier>, <string>);`
| `endsWithStatement()` | Parses `EndsWith(<identifier>, <string>);`
| `toUpperStatement()` | Parses `ToUpper(<identifier>, <string>);`
| `toLowerStatement()` | Parses `ToLower(<identifier>, <string>);`
| `trimStatement()` | Parses `Trim(<identifier>, <string>);`
| `indexOfStatement()` | Parses `IndexOf(<identifier>, <string>);`
| `formatStatement()` | Parses `Format(<identifier or string>);`
| `expression()` | Parses binary/comparision/logical expressions (left-to-right)
| `term()` | Parses multiplication/division/modulo (higher precedence)
| `factor()` | Parses atoms: identifiers (with dot access), numbers, bools, strings, parenthesized exprs, FetchReturn
| `parseTokens(tokens)` | Free function. Creates Parser, runs it, returns ParseResult

src/semantic/semantic.h  AST Node Types
| Type | What it does
|-------------------|---------------------------------------------------------------------------------------------------------|
| `enum class Type` | All semantic types: Ui8-Ui128, I8-I128, Double, Float, Long, String, Enum, Array, Interface, Struct, Object, Class, Void, UNKNOWN
| `struct Symbol` | Represents a declared symbol. Has kind (Variable/Function/Struct), type, paramTypes, returnType, fields
| `struct Scope` | A scope with a parent pointer and symbol table. Supports lookup(name) and declare(name, symbol)
| `struct SemanticContext` | Holds current scope, current function return type, and error list
| `struct Expr` | Abstract base for expressions (has analyze() returning Type)
| `struct Stmt` | Abstract base for statements (has analyze() returning void)
| `struct Param` | Function parameter: name + type
| `struct Field` | Struct field: name + type
| `struct FunctionDecl` | AST node for function declarations
| `struct StructDecl` | AST node for struct declarations
| `struct VarDecl` | AST node for variable declarations (let or typed)
| `struct ReturnStmt` | AST node for return statements
| `struct ExitStmt` | AST node for exit statements
| `struct BuiltinCallStmt` | AST node for built-in function calls
| `struct ClassDecl` | AST node for class declarations
| `struct EnumDecl` | AST node for enum declarations
| `struct Forstmt` | AST node for for-loops
| `struct WhileStmt` | AST node for while-loops
| `class SemanticAnalyzer` | Entry point for semantic analysis. Runs analyze() on the full program

src/semantic/semantic.cpp
| Function | What it does
|-------------------|---------------------------------------------------------------------------------------------------------|
| `Scope::lookup(name)` | Walks the scope chain upward top find a symbol by name
| `Scope::declare(name, sym)` | Declares a symbol in the current scope. Returns false if already exists
| `FunctionDecl::analyze(ctx)` | Registers function in scope, creates child scope for params+body, anbalyzes body
| `StructDecl::analyze(ctx)` | Registers struct and its fields in scope, checks for duplicate fields
| `VarDecl::analyze(ctx)` | Registers variable in scope, checks for duplicate declaration
| `ReturnStmt::analyze(ctx)` | (currently empty - placeholder)
| `ClassDecl::analyze(ctx)` | Creates child scope, analyzes class body
| `EnumDecl::analyze(ctx)` | Creates child scope, analyzes enum body
| `ForStmt::analyze(ctx)` | Creates child scope with loop variable, analyzes loop body
| `WhileStmt::analyze(ctx)` | Creates child scope, analyzes loop body
| `SemanticAnalyzer::analyze(program)` | Creates global scope, runs analyze on every statement, returns error list
---
## Pipeline Summary

Source (.ll file) -> tokenize() -> parseTokens() -> SemanticAnalyzer::analyze() -> success/errors

--


