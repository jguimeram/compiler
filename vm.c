// vm.c
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define STACK_MAX 256
#define VAR_TABLE_SIZE 256

typedef struct {
    const Bytecode *bc;
    size_t ip;
    Value stack[STACK_MAX];
    int sp;
    struct { const char *name; Value value; } vars[VAR_TABLE_SIZE];
    size_t var_count;
} VM;

static void push(VM *vm, Value value) {
    vm->stack[vm->sp++] = value;
}
static Value pop_(VM *vm) {
    return vm->stack[--vm->sp];
}

static Value *lookup_var(VM *vm, int name_idx) {
    const char *name = vm->bc->constants[name_idx].str_val;
    for (size_t i=0; i<vm->var_count; i++) {
        if (strcmp(vm->vars[i].name, name) == 0) {
            return &vm->vars[i].value;
        }
    }
    if (vm->var_count >= VAR_TABLE_SIZE) {
        fprintf(stderr, "Too many variables\n");
        exit(EXIT_FAILURE);
    }
    vm->vars[vm->var_count].name = name;
    vm->vars[vm->var_count].value.type = VAL_INT;
    vm->vars[vm->var_count].value.int_val = 0;
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
                Value *slot = lookup_var(&vm, idx);
                push(&vm, *slot);
                break;
            }
            case OP_STORE: {
                uint8_t idx = bc->code[vm.ip++];
                Value val = pop_(&vm);
                Value *slot = lookup_var(&vm, idx);
                *slot = val;
                break;
            }
            case OP_ADD:    { Value b=pop_(&vm), a=pop_(&vm); push(&vm, (Value){VAL_INT, .int_val=a.int_val+b.int_val}); break; }
            case OP_SUB:    { Value b=pop_(&vm), a=pop_(&vm); push(&vm, (Value){VAL_INT, .int_val=a.int_val-b.int_val}); break; }
            case OP_MUL:    { Value b=pop_(&vm), a=pop_(&vm); push(&vm, (Value){VAL_INT, .int_val=a.int_val*b.int_val}); break; }
            case OP_DIV:    { Value b=pop_(&vm), a=pop_(&vm); push(&vm, (Value){VAL_INT, .int_val=a.int_val/b.int_val}); break; }
            case OP_MOD:    { Value b=pop_(&vm), a=pop_(&vm); push(&vm, (Value){VAL_INT, .int_val=a.int_val%b.int_val}); break; }
            case OP_GT:     { Value b=pop_(&vm), a=pop_(&vm); push(&vm, (Value){VAL_INT, .int_val=a.int_val>b.int_val}); break; }
            case OP_LT:     { Value b=pop_(&vm), a=pop_(&vm); push(&vm, (Value){VAL_INT, .int_val=a.int_val<b.int_val}); break; }
            case OP_GTE:    { Value b=pop_(&vm), a=pop_(&vm); push(&vm, (Value){VAL_INT, .int_val=a.int_val>=b.int_val}); break; }
            case OP_LTE:    { Value b=pop_(&vm), a=pop_(&vm); push(&vm, (Value){VAL_INT, .int_val=a.int_val<=b.int_val}); break; }
            case OP_EQ:     { Value b=pop_(&vm), a=pop_(&vm); push(&vm, (Value){VAL_INT, .int_val=a.int_val==b.int_val}); break; }
            case OP_NEQ:    { Value b=pop_(&vm), a=pop_(&vm); push(&vm, (Value){VAL_INT, .int_val=a.int_val!=b.int_val}); break; }
            case OP_JMP: {
                int8_t offset = (int8_t)bc->code[vm.ip++];
                vm.ip += offset;
                break;
            }
            case OP_JMP_IF_FALSE: {
                int8_t offset = (int8_t)bc->code[vm.ip++];
                Value cond = pop_(&vm);
                if (!cond.int_val) vm.ip += offset;
                break;
            }
            case OP_PRINT: {
                Value val = pop_(&vm);
                if (val.type == VAL_INT)
                    printf("%d", val.int_val);
                else
                    printf("%s", val.str_val);
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
