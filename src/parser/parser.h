#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../lexer/token.h"
#include "../semantic/semantic.h"

extern void INIT_RESERVED_IDENTIFIER();
std::vector<Token> tokenize(std::string& sourceCode);

struct ParseResult {
    bool success;
    std::vector<std::unique_ptr<Stmt>> statements;
};

ParseResult parseTokens(const std::vector<Token>& tokens);
