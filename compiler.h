// compiler.h
#ifndef COMPILER_H
#define COMPILER_H

#include "ast.h"
#include "bytecode.h"

Bytecode *compile(ASTNode *ast);

#endif // COMPILER_H
