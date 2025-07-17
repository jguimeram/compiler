// compiler.c
#define _GNU_SOURCE
#include "compiler.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void compile_program(ASTNode *program, Bytecode *bc);
static void compile_statement(ASTNode *stmt, Bytecode *bc);
static void compile_block(ASTNode *block, Bytecode *bc);
static void compile_expression(ASTNode *expr, Bytecode *bc);

Bytecode *compile(ASTNode *ast) {
    Bytecode *bc = bytecode_new();
    compile_program(ast, bc);
    emit_byte(bc, OP_HALT);
    return bc;
}

static void compile_program(ASTNode *program, Bytecode *bc) {
    ASTNodeList *cur = program->as.program;
    while (cur) {
        compile_statement(cur->node, bc);
        cur = cur->next;
    }
}

static void compile_statement(ASTNode *stmt, Bytecode *bc) {
    switch (stmt->type) {
        case AST_EXPR_STMT:
            compile_expression(stmt->as.expr_stmt.expr, bc);
            break;
        case AST_VAR_ASSIGN: {
            compile_expression(stmt->as.var_assign.value, bc);
            int idx = bytecode_new_constant(bc, (intptr_t)strdup(stmt->as.var_assign.name));
            emit_op_const(bc, OP_STORE, (uint8_t)idx);
            break;
        }
        case AST_IF: {
            compile_expression(stmt->as.if_stmt.cond, bc);
            size_t jmpIf = bc->code_size;
            emit_byte(bc, OP_JMP_IF_FALSE);
            emit_byte(bc, 0);
            compile_block(stmt->as.if_stmt.then_branch, bc);
            size_t jmp = bc->code_size;
            emit_byte(bc, OP_JMP);
            emit_byte(bc, 0);
            bc->code[jmpIf+1] = (uint8_t)(bc->code_size - (jmpIf + 2));
            if (stmt->as.if_stmt.else_branch) {
                compile_block(stmt->as.if_stmt.else_branch, bc);
            }
            bc->code[jmp+1] = (uint8_t)(bc->code_size - (jmp + 2));
            break;
        }
        case AST_WHILE: {
            size_t loopStart = bc->code_size;
            compile_expression(stmt->as.while_stmt.cond, bc);
            size_t exitJump = bc->code_size;
            emit_byte(bc, OP_JMP_IF_FALSE);
            emit_byte(bc, 0);
            compile_block(stmt->as.while_stmt.body, bc);
            emit_byte(bc, OP_JMP);
            emit_byte(bc, (uint8_t)(loopStart - (bc->code_size + 2)));
            bc->code[exitJump+1] = (uint8_t)(bc->code_size - (exitJump + 2));
            break;
        }
        case AST_RETURN:
            compile_expression(stmt->as.return_stmt.value, bc);
            emit_byte(bc, OP_RET);
            break;
        default:
            break;
    }
}

static void compile_block(ASTNode *block, Bytecode *bc) {
    ASTNodeList *cur = block->as.block.statements;
    while (cur) {
        compile_statement(cur->node, bc);
        cur = cur->next;
    }
}

static void compile_expression(ASTNode *expr, Bytecode *bc) {
    switch (expr->type) {
        case AST_LITERAL: {
            int val = expr->as.literal.value;
            int idx = bytecode_new_constant(bc, val);
            emit_op_const(bc, OP_CONSTANT, (uint8_t)idx);
            break;
        }
        case AST_VAR_REF: {
            int idx = bytecode_new_constant(bc, (intptr_t)strdup(expr->as.var_ref.name));
            emit_op_const(bc, OP_LOAD, (uint8_t)idx);
            break;
        }
        case AST_BINARY_OP:
            compile_expression(expr->as.binary.left, bc);
            compile_expression(expr->as.binary.right, bc);
            switch (expr->as.binary.op) {
                case T_PLUS:  emit_byte(bc, OP_ADD); break;
                case T_MINUS: emit_byte(bc, OP_SUB); break;
                case T_STAR:  emit_byte(bc, OP_MUL); break;
                case T_SLASH: emit_byte(bc, OP_DIV); break;
                case T_GT:    emit_byte(bc, OP_GT);  break;
                case T_LT:    emit_byte(bc, OP_LT);  break;
                case T_GTE:   emit_byte(bc, OP_GTE); break;
                case T_LTE:   emit_byte(bc, OP_LTE); break;
                case T_EQ:    emit_byte(bc, OP_EQ);  break;
                case T_NEQ:   emit_byte(bc, OP_NEQ); break;
                default: break;
            }
            break;
        case AST_FUNC_CALL:
            if (strcmp(expr->as.func_call.name, "print")==0 && expr->as.func_call.arg_count==1) {
                compile_expression(expr->as.func_call.args[0], bc);
                emit_byte(bc, OP_PRINT);
            }
            break;
        default:
            break;
    }
}
