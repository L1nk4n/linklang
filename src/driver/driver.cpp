#include "driver.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include "../parser/parser.h"
#include "../codegen/x86_64/codegen.h"
#include "../semantic/semantic.h"
#include "../ir/builder.h"
#include "../ir/printer.h"
#include "../codegen/x86_64/emitter.h"

int runPipeline(const std::string &inputPath, const std::string &outputPath) {
  (void)outputPath;

  std::ifstream in(inputPath);
  if(!in) {
    std::cerr << "Failed to open: " << inputPath << "\n";
    return 1;
  }
  std::stringstream ss;
  ss << in.rdbuf();
  std::string source = ss.str();

  auto tokens = tokenize(source);
  auto parseResult = parseTokens(tokens);
  if(!parseResult.success) return 1;
  
  SemanticAnalyzer sema;
  auto errors = sema.analyze(parseResult.statements);
  if(!errors.empty())
  {
    for(const auto& e : errors) std::cerr << e << "\n";
    return 1;
  }

  IRBuilder builder;
  Module ir = builder.build(parseResult.statements);

  std::cerr << "-- IR --\n";
  printModule(ir, std::cerr);
  std::cerr << "-----------\n";

  {
    std::ofstream asmFile("/tmp/linklang_out.s");
    if(!asmFile) return 1;
    generateCode(ir, asmFile);
  }

  if (std::system("as -o /tmp/linklang_out.o /tmp/linklang_out.s") != 0) return 1;
  if (std::system(("ld -o " + outputPath + " /tmp/linklang_out.o").c_str()) != 0) return 1;

  return 0;
}
