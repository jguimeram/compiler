// ast.h
#ifndef AST_H
#define AST_H

#include <stddef.h>
#include "tokens.h"

typedef enum {
    AST_PROGRAM,
    AST_EXPR_STMT,
    AST_VAR_ASSIGN,
    AST_IF,
    AST_WHILE,
    AST_RETURN,
    AST_FUNCTION,
    AST_BLOCK,
    AST_BINARY_OP,
    AST_LITERAL,
    AST_VAR_REF,
    AST_FUNC_CALL
} ASTNodeType;

typedef struct ASTNodeList {
    struct ASTNode *node;
    struct ASTNodeList *next;
} ASTNodeList;

typedef struct ASTNode {
    ASTNodeType type;
    size_t line, column;
    union {
        ASTNodeList *program;
        struct { struct ASTNode *expr; } expr_stmt;
        struct { char *name; struct ASTNode *value; } var_assign;
        struct { struct ASTNode *cond, *then_branch, *else_branch; } if_stmt;
        struct { struct ASTNode *cond, *body; } while_stmt;
        struct { struct ASTNode *value; } return_stmt;
        struct { char *name; char **params; size_t param_count; struct ASTNode *body; } func_def;
        struct { ASTNodeList *statements; } block;
        struct { TokenType op; struct ASTNode *left, *right; } binary;
        struct { int value; } literal;
        struct { char *name; } var_ref;
        struct { char *name; struct ASTNode **args; size_t arg_count; } func_call;
    } as;
} ASTNode;

ASTNode *ast_node_new(ASTNodeType type, size_t line, size_t column);
void ast_node_list_append(ASTNodeList **list, ASTNode *node);
void free_ast(ASTNode *node);

#endif // AST_H
