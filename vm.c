// vm.c
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

#define STACK_MAX 256
#define VAR_TABLE_SIZE 256

typedef struct {
    const Bytecode *bc;
    size_t ip;
    int stack[STACK_MAX];
    int sp;
    struct { int key_index; int value; } vars[VAR_TABLE_SIZE];
    size_t var_count;
} VM;

static void push(VM *vm, int value) {
    vm->stack[vm->sp++] = value;
}
static int pop_(VM *vm) {
    return vm->stack[--vm->sp];
}

static int *lookup_var(VM *vm, int name_idx) {
    for (size_t i=0; i<vm->var_count; i++) {
        if (vm->vars[i].key_index == name_idx) {
            return &vm->vars[i].value;
        }
    }
    if (vm->var_count >= VAR_TABLE_SIZE) {
        fprintf(stderr, "Too many variables\n");
        exit(EXIT_FAILURE);
    }
    vm->vars[vm->var_count].key_index = name_idx;
    vm->vars[vm->var_count].value = 0;
    return &vm->vars[vm->var_count++].value;
}

int run_bytecode(const Bytecode *bc) {
    VM vm = { .bc=bc, .ip=0, .sp=0, .var_count=0 };
    while (vm.ip < bc->code_size) {
        OpCode op = (OpCode)bc->code[vm.ip++];
        switch (op) {
            case OP_CONSTANT: {
                uint8_t idx = bc->code[vm.ip++];
                push(&vm, bc->constants[idx]);
                break;
            }
            case OP_LOAD: {
                uint8_t idx = bc->code[vm.ip++];
                int *slot = lookup_var(&vm, idx);
                push(&vm, *slot);
                break;
            }
            case OP_STORE: {
                uint8_t idx = bc->code[vm.ip++];
                int val = pop_(&vm);
                int *slot = lookup_var(&vm, idx);
                *slot = val;
                break;
            }
            case OP_ADD:    { int b=pop_(&vm), a=pop_(&vm); push(&vm, a+b); break; }
            case OP_SUB:    { int b=pop_(&vm), a=pop_(&vm); push(&vm, a-b); break; }
            case OP_MUL:    { int b=pop_(&vm), a=pop_(&vm); push(&vm, a*b); break; }
            case OP_DIV:    { int b=pop_(&vm), a=pop_(&vm); push(&vm, a/b); break; }
            case OP_MOD:    { int b=pop_(&vm), a=pop_(&vm); push(&vm, a%b); break; }
            case OP_GT:     { int b=pop_(&vm), a=pop_(&vm); push(&vm, a>b); break; }
            case OP_LT:     { int b=pop_(&vm), a=pop_(&vm); push(&vm, a<b); break; }
            case OP_GTE:    { int b=pop_(&vm), a=pop_(&vm); push(&vm, a>=b); break; }
            case OP_LTE:    { int b=pop_(&vm), a=pop_(&vm); push(&vm, a<=b); break; }
            case OP_EQ:     { int b=pop_(&vm), a=pop_(&vm); push(&vm, a==b); break; }
            case OP_NEQ:    { int b=pop_(&vm), a=pop_(&vm); push(&vm, a!=b); break; }
            case OP_JMP: {
                int8_t offset = (int8_t)bc->code[vm.ip++];
                vm.ip += offset;
                break;
            }
            case OP_JMP_IF_FALSE: {
                int8_t offset = (int8_t)bc->code[vm.ip++];
                int cond = pop_(&vm);
                if (!cond) vm.ip += offset;
                break;
            }
            case OP_PRINT: {
                int val = pop_(&vm);
                printf("%d", val);
                break;
            }
            case OP_RET:
            case OP_HALT:
                return 0;
            default:
                fprintf(stderr, "Unknown opcode %d at %zu", op, vm.ip-1);
                return 1;
        }
    }
    return 0;
}
