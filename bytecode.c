// bytecode.c
#include "bytecode.h"
#include <stdlib.h>
#include <stdio.h>

Bytecode *bytecode_new(void) {
    Bytecode *bc = malloc(sizeof(Bytecode));
    bc->code = NULL;
    bc->code_size = 0;
    bc->const_count = 0;
    return bc;
}

void bytecode_free(Bytecode *bc) {
    free(bc->code);
    free(bc);
}

int bytecode_new_constant(Bytecode *bc, int value) {
    if (bc->const_count >= MAX_CONSTANTS) {
       fprintf(stderr, "Too many constants");
        exit(EXIT_FAILURE);
    }
    bc->constants[bc->const_count] = value;
    return bc->const_count++;
}

void emit_byte(Bytecode *bc, uint8_t byte) {
    bc->code = realloc(bc->code, bc->code_size + 1);
    bc->code[bc->code_size++] = byte;
}

void emit_op_const(Bytecode *bc, OpCode op, uint8_t const_index) {
    emit_byte(bc, op);
    emit_byte(bc, const_index);
}
