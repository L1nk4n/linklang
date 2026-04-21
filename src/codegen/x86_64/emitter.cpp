#include "emitter.h"

void emitExitProgram(std::ostream& out, int exitCode)
{
  out << ".intel_syntax noprefix\n"; 
  out << ".global _start\n\n";
  out << ".text\n";
  out << "_start:\n";
  out << "    mov rax, 60\n";
  out << "    mov rdi, " << exitCode << "\n";
  out << "    syscall\n";
}
