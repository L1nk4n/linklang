#pragma once
#include <ostream>
#include "ir.h"


void printModule(const Module& module, std::ostream& out);
void printFunction(const Function& fn, std::ostream& out);
void printBlock(const BasicBlock& bb, std::ostream& out);
void printInstruction(const Instruction& inst, std::ostream& out);
void printValue(const Value& v, std::ostream& out);

std::string irTypeToString(IRType t);
std::string opcodeToString(Opcode op);
