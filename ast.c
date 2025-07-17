// ast.c
#include "ast.h"
#include <stdlib.h>
#include <string.h>

ASTNode *ast_node_new(ASTNodeType type, size_t line, size_t column) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = type;
    node->line = line;
    node->column = column;
    memset(&node->as, 0, sizeof(node->as));
    return node;
}

void ast_node_list_append(ASTNodeList **list, ASTNode *node) {
    ASTNodeList *item = malloc(sizeof(ASTNodeList));
    item->node = node;
    item->next = NULL;
    if (*list == NULL) {
        *list = item;
    } else {
        ASTNodeList *cur = *list;
        while (cur->next) cur = cur->next;
        cur->next = item;
    }
}

static void free_list(ASTNodeList *list) {
    while (list) {
        ASTNodeList *next = list->next;
        free_ast(list->node);
        free(list);
        list = next;
    }
}

void free_ast(ASTNode *node) {
    if (!node) return;
    switch (node->type) {
        case AST_PROGRAM:
            free_list(node->as.program);
            break;
        case AST_EXPR_STMT:
            free_ast(node->as.expr_stmt.expr);
            break;
        case AST_VAR_ASSIGN:
            free(node->as.var_assign.name);
            free_ast(node->as.var_assign.value);
            break;
        case AST_IF:
            free_ast(node->as.if_stmt.cond);
            free_ast(node->as.if_stmt.then_branch);
            free_ast(node->as.if_stmt.else_branch);
            break;
        case AST_WHILE:
            free_ast(node->as.while_stmt.cond);
            free_ast(node->as.while_stmt.body);
            break;
        case AST_RETURN:
            free_ast(node->as.return_stmt.value);
            break;
        case AST_FUNCTION:
            free(node->as.func_def.name);
            for (size_t i = 0; i < node->as.func_def.param_count; i++) {
                free(node->as.func_def.params[i]);
            }
            free(node->as.func_def.params);
            free_ast(node->as.func_def.body);
            break;
        case AST_BLOCK:
            free_list(node->as.block.statements);
            break;
        case AST_BINARY_OP:
            free_ast(node->as.binary.left);
            free_ast(node->as.binary.right);
            break;
        case AST_LITERAL:
            if (node->as.literal.is_string) {
                free(node->as.literal.str);
            }
            break;
        case AST_VAR_REF:
            free(node->as.var_ref.name);
            break;
        case AST_FUNC_CALL:
            free(node->as.func_call.name);
            for (size_t i = 0; i < node->as.func_call.arg_count; i++) {
                free_ast(node->as.func_call.args[i]);
            }
            free(node->as.func_call.args);
            break;
    }
    free(node);
}
