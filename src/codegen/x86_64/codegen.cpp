#include "codegen.h"
#include <iostream>
#include <unordered_map>
#include <cstdint>

static void emitRodata(const Module& module, std::ostream& out) {
  out << ".section .rodata\n";
  out << ".newline:\n";
  out << "    .ascii \"\\n\"\n";
  out << ".newline_len = . - .newline\n\n";

  for(size_t i = 0; i < module.stringLiterals.size(); ++i) {
    out << ".str" << i << ":\n";
    out << "    .ascii \"" << module.stringLiterals[i] << "\"\n";
    out << ".str" << i << "_len = . - .str" << i << "\n\n";
  }
}

void generateCode(const Module& module, std::ostream& out) {
    out << ".intel_syntax noprefix\n";
    emitRodata(module, out);

    out << ".section .text\n";
    out << ".global _start\n\n";
    out << "_start:\n";

    const Function* main = nullptr;
    for (const auto& fn : module.functions) {
        if (fn.name == "main") { main = &fn; break; }
    }
    if (!main || main->blocks.empty()) {
        std::cerr << "codegen: no main function or empty body\n";
        out << "    mov rdi, 0\n    mov rax, 60\n    syscall\n";
        return;
    }

    const BasicBlock& entry = main->blocks.front();
    std::unordered_map<int, int> offsetOf;
    std::unordered_map<int, int> allocaSlot;
    int nextOffset = 8;

    for(const auto& inst : entry.instructions) {
      if(inst.op == Opcode::Alloca) {
        allocaSlot[inst.result.id] = nextOffset;
        nextOffset += 8;
      } else {
        bool producesValue = 
          inst.op != Opcode::Store &&
          inst.op != Opcode::Ret &&
          inst.op != Opcode::PrintStr &&
          inst.op != Opcode::PrintInt &&
          inst.op != Opcode::PrintNewLine;
        if(producesValue) {
          offsetOf[inst.result.id] = nextOffset;
          nextOffset += 8;
        }
      }
    }

    
    int stackSize = nextOffset - 8;
    if (stackSize % 16 != 0) stackSize += 8;

    out << "    push rbp\n";
    out << "    mov rbp, rsp\n";
    if (stackSize > 0) out << "    sub rsp, " << stackSize << "\n";

    
    auto loadOperand = [&](const Value& v, const std::string& reg) {
        if (v.isConstant()) {
            out << "    mov " << reg << ", " << v.constant << "\n";
        } else {
            int off = offsetOf[v.id];
            out << "    mov " << reg << ", [rbp - " << off << "]\n";
        }
    };

  
    for (const auto& inst : entry.instructions) {
        switch (inst.op) {
            case Opcode::Const: {
                int off = offsetOf[inst.result.id];
                out << "    mov rax, " << inst.operands.front().constant << "\n";
                out << "    mov [rbp - " << off << "], rax\n";
                break;
            }

            case Opcode::Alloca: {
                break;
            }

            case Opcode::Load: { 
                const Value& ptr = inst.operands.front();
                int dstOff = offsetOf[inst.result.id];
                int slotOff = allocaSlot[ptr.id];
                out << "    mov rax, [rbp - " << slotOff << "]\n";
                out << "    mov [rbp -" << dstOff << "], rax\n";
                break;
            }

            case Opcode::Store: {
                
                const Value& val = inst.operands[0];
                const Value& ptr = inst.operands[1];
                int slotOff = allocaSlot[ptr.id];
                loadOperand(val, "rax");
                out << "    mov [rbp - " << slotOff << "], rax\n";
                break;
            }

            case Opcode::PrintStr: {
                int64_t idx = inst.operands.front().constant;
                if (idx < 0 || (size_t)idx >= module.stringLiterals.size()) {
                    std::cerr << "codegen: bad string idx\n"; break;
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
                if (!inst.operands.empty()) {
                    loadOperand(inst.operands.front(), "rdi");
                } else {
                    out << "    mov rdi, 0\n";
                }
                
                out << "    mov rsp, rbp\n";
                out << "    pop rbp\n";
                out << "    mov rax, 60\n";
                out << "    syscall\n";
                return;
            }

            default:
                std::cerr << "codegen: unsupported opcode\n";
                break;
        }
    }

    out << "    mov rdi, 0\n    mov rax, 60\n    syscall\n";
}
