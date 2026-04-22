#include "codegen.h"
#include <iostream>
#include <unordered_map>
#include <cstdint>

void generateCode(const Module& module, std::ostream& out) {
  out << ".intel_syntax noprefix\n";
  out << ".newline:\n";
  out << "    .ascii \"\\n\"\n";
  out << ".newline_len = . - .newline\n\n";
  for(size_t i = 0; i < module.stringLiterals.size(); ++i) {
    out << ".str" << i << ":\n";
    out << "    .ascii \"" << module.stringLiterals[i] << "\"\n";
    out << ".str" << i << "_len = . - .str" << i << "\n\n";
  }

  out << ".section .text\n";
  out << ".global _start\n\n";
  out << "_start:\n";

  const Function* main = nullptr;
  for(const auto& fn : module.functions) {
    if(fn.name == "main") { main = &fn; break; }
  }
  if (!main || main->blocks.empty()) {
        std::cerr << "codegen: no main function or empty body\n";
        out << "    mov rdi, 0\n";
        out << "    mov rax, 60\n";
        out << "    syscall\n";
        return;
    }

  std::unordered_map<int, int64_t> constValue;
  const BasicBlock& entry = main->blocks.front();
  for (const auto& inst : entry.instructions) {
    switch(inst.op) {
      case Opcode::Const: {
        int64_t v = inst.operands.front().constant;
        constValue[inst.result.id] = v;
        break;
      }

      case Opcode::PrintStr: {
        const Value& v = inst.operands.front();
        int64_t idx = v.isConstant() ? v.constant : -1;
        if(idx < 0 || (size_t)idx >= module.stringLiterals.size()) {
          std::cerr << "codegen: Invalid string index for printstr\n";
          break;
        }
        out << "    mov rax, 1\n";                            
        out << "    mov rdi, 1\n";                           
        out << "    lea rsi, [rip + .str" << idx << "]\n";    
        out << "    mov rdx, .str" << idx << "_len\n";
        out << "    syscall\n";
        break;
      }
      case Opcode::PrintNewLine: {
        out << "    mov rax, 1\n";
        out << "    mov rdi, 1\n";
        out << "    lea rsi, [rip + .newline]\n";
        out << "    mov rdx, .newline_len\n";
        out << "    syscall\n";
        break;
      }

      case Opcode::Ret: {
        int64_t exitCode = 0;
        if(!inst.operands.empty()) {
          const Value& v = inst.operands.front();
          if(v.isConstant()) {
            exitCode = v.constant;
          } else {
            auto it = constValue.find(v.id);
            if(it != constValue.end()) {
              exitCode = it->second;
            } else {
              std::cerr << "codegen: ret unsupported SSA value %" << v.id << "\n";
            }
          }
        }
        out << "    mov rdi, " << exitCode << "\n";
        out << "    mov rax, 60\n";
        out << "    syscall\n";
        return;
      }

      default:
          std::cerr << "codegen: unsupported opcode (skipped)\n";
          break;
    }
  }
  out << "    mov rdi, 0\n";
  out << "    mov rax, 60\n";
  out << "    syscall\n";
}
