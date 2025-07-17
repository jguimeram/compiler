// bytecode.h
#ifndef BYTECODE_H
#define BYTECODE_H

#include <stdint.h>
#include <stddef.h> // size_t

#define MAX_CONSTANTS 256

typedef enum {
    OP_CONSTANT,
    OP_LOAD,
    OP_STORE,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_GT,
    OP_LT,
    OP_GTE,
    OP_LTE,
    OP_EQ,
    OP_NEQ,
    OP_JMP,
    OP_JMP_IF_FALSE,
    OP_CALL,
    OP_RET,
    OP_PRINT,
    OP_HALT
} OpCode;

typedef struct {
    uint8_t *code;
    size_t code_size;
    int constants[MAX_CONSTANTS];
    size_t const_count;
} Bytecode;

Bytecode *bytecode_new(void);
void bytecode_free(Bytecode *bc);
int bytecode_new_constant(Bytecode *bc, int value);
void emit_byte(Bytecode *bc, uint8_t byte);
void emit_op_const(Bytecode *bc, OpCode op, uint8_t const_index);

#endif // BYTECODE_H
