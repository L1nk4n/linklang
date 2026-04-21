#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include "parser/parser.h"
#include "driver/driver.h"

using namespace std;

int main(int argc, char *argv[])
{
    INIT_RESERVED_IDENTIFIER();
    if (argc != 2) {
        cerr << "Incorrect arguments" << endl;
        cerr << "Correct usage: ./linklang <input file path>" << endl;
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

    vector<Token> tokens = tokenize(sourceCode);

    if (tokens.empty()) {
        cerr << "No tokens produced by lexer" << endl;
        return EXIT_FAILURE;
    }

    ParseResult result = parseTokens(tokens);

    if (!result.success) {
        cout << "Parse failed" << endl;
        return EXIT_FAILURE;
    }
    cout << "Parse success" << endl;

    SemanticAnalyzer analyzer;
    vector<string> errors = analyzer.analyze(result.statements);

    if (!errors.empty()) {
        cerr << "Semantic errors:" << endl;
        for (const auto& err : errors) {
            cerr << "  " << err << endl;
        }
        return EXIT_FAILURE;
    }
    cout << "Semantic analysis passed" << endl;

    if(runPipeline(argv[1], "a.out") != 0)
    {
      cerr << "Code generation failed" << endl;
      return EXIT_FAILURE;
    }

    cout << "Build success ./a.out" << endl;

    return EXIT_SUCCESS;
}
