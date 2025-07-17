// parser.c
#define _GNU_SOURCE
#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    Token *tokens;
    size_t count;
    size_t pos;
} Parser;

static Token *peek(Parser *p) {
    return p->pos < p->count ? &p->tokens[p->pos] : NULL;
}
static Token *advance(Parser *p) {
    return p->pos < p->count ? &p->tokens[p->pos++] : NULL;
}
static int match(Parser *p, TokenType tt) {
    if (peek(p)->type == tt) {
        advance(p);
        return 1;
    }
    return 0;
}
static void expect(Parser *p, TokenType tt, const char *msg) {
    if (!match(p, tt)) {
        Token *t = peek(p);
       fprintf(stderr, "Parse error at %zu:%zu: %s", t->line, t->column, msg);
        exit(EXIT_FAILURE);
    }
}
static int get_prec(TokenType op) {
    switch (op) {
        case T_STAR: case T_SLASH: case T_MOD: return 3;
        case T_PLUS: case T_MINUS: return 2;
        case T_GT: case T_LT: case T_GTE:
        case T_LTE: case T_EQ: case T_NEQ: return 1;
        default: return 0;
    }
}

static ASTNode *parse_program(Parser *p);
static ASTNode *parse_statement(Parser *p);
static ASTNode *parse_block(Parser *p);
static ASTNode *parse_expression(Parser *p, int min_prec);
static ASTNode *parse_primary(Parser *p);

ASTNode *parse(Token *tokens, size_t count) {
    Parser p = { tokens, count, 0 };
    return parse_program(&p);
}

static ASTNode *parse_program(Parser *p) {
    ASTNode *root = ast_node_new(AST_PROGRAM, 0, 0);
    while (peek(p)->type != T_EOF) {
        if (peek(p)->type == T_FUNCTION) {
            advance(p);
            Token *name = advance(p);
            expect(p, T_LPAREN, "Expected '(' after function name");
            char **params = NULL; size_t pc=0;
            if (!match(p, T_RPAREN)) {
                do {
                    Token *t = advance(p);
                    params = realloc(params, sizeof(char*)*(pc+1));
                    params[pc++] = strdup(t->text);
                } while(match(p, T_COMMA));
                expect(p, T_RPAREN, "Expected ')' after params");
            }
            ASTNode *body = parse_block(p);
            ASTNode *fn = ast_node_new(AST_FUNCTION, name->line, name->column);
            fn->as.func_def.name = strdup(name->text);
            fn->as.func_def.params = params;
            fn->as.func_def.param_count = pc;
            fn->as.func_def.body = body;
            ast_node_list_append(&root->as.program, fn);
        } else {
            ASTNode *stmt = parse_statement(p);
            ast_node_list_append(&root->as.program, stmt);
        }
    }
    return root;
}

static ASTNode *parse_statement(Parser *p) {
    Token *t = peek(p);
    if (t->type == T_IF) {
        advance(p);
        expect(p, T_LPAREN, "Expected '(' after if");
        ASTNode *cond = parse_expression(p, 0);
        expect(p, T_RPAREN, "Expected ')' after condition");
        ASTNode *then_br = parse_block(p);
        ASTNode *else_br = NULL;
        if (match(p, T_ELSE)) {
            else_br = parse_block(p);
        }
        ASTNode *n = ast_node_new(AST_IF, t->line, t->column);
        n->as.if_stmt.cond = cond;
        n->as.if_stmt.then_branch = then_br;
        n->as.if_stmt.else_branch = else_br;
        return n;
    }
    if (t->type == T_WHILE) {
        advance(p);
        expect(p, T_LPAREN, "Expected '(' after while");
        ASTNode *cond = parse_expression(p, 0);
        expect(p, T_RPAREN, "Expected ')' after condition");
        ASTNode *body = parse_block(p);
        ASTNode *n = ast_node_new(AST_WHILE, t->line, t->column);
        n->as.while_stmt.cond = cond;
        n->as.while_stmt.body = body;
        return n;
    }
    if (t->type == T_RETURN) {
        advance(p);
        ASTNode *val = parse_expression(p, 0);
        expect(p, T_SEMICOLON, "Expected ';' after return value");
        ASTNode *n = ast_node_new(AST_RETURN, t->line, t->column);
        n->as.return_stmt.value = val;
        return n;
    }
    if (t->type == T_IDENTIFIER && t->text[0]=='$' &&
        p->pos + 1 < p->count && p->tokens[p->pos + 1].type == T_ASSIGN) {
        Token *name = advance(p); // consume identifier
        advance(p); // consume '='
        ASTNode *v = parse_expression(p, 0);
        expect(p, T_SEMICOLON, "Expected ';' after assignment");
        ASTNode *n = ast_node_new(AST_VAR_ASSIGN, name->line, name->column);
        n->as.var_assign.name = strdup(name->text);
        n->as.var_assign.value = v;
        return n;
    }
    ASTNode *expr = parse_expression(p, 0);
    expect(p, T_SEMICOLON, "Expected ';' after expression");
    ASTNode *n = ast_node_new(AST_EXPR_STMT, expr->line, expr->column);
    n->as.expr_stmt.expr = expr;
    return n;
}

static ASTNode *parse_block(Parser *p) {
    expect(p, T_LBRACE, "Expected '{'");
    ASTNode *node = ast_node_new(AST_BLOCK, 0, 0);
    while (!match(p, T_RBRACE)) {
        ASTNode *s = parse_statement(p);
        ast_node_list_append(&node->as.block.statements, s);
    }
    return node;
}

static ASTNode *parse_primary(Parser *p) {
    Token *t = peek(p);
    if (match(p, T_NUMBER)) {
        ASTNode *n = ast_node_new(AST_LITERAL, t->line, t->column);
        n->as.literal.is_string = 0;
        n->as.literal.value = atoi(t->text);
        return n;
    }
    if (match(p, T_STRING)) {
        ASTNode *n = ast_node_new(AST_LITERAL, t->line, t->column);
        n->as.literal.is_string = 1;
        n->as.literal.str = strdup(t->text);
        return n;
    }
    if (t->type == T_IDENTIFIER || t->type == T_PRINT) {
        advance(p);
        if (peek(p)->type==T_LPAREN) {
            char *name=strdup(t->text);
            advance(p);
            ASTNode **args=NULL; size_t ac=0;
            if (!match(p, T_RPAREN)) {
                do {
                    ASTNode *a = parse_expression(p, 0);
                    args = realloc(args, sizeof(ASTNode*)*(ac+1));
                    args[ac++] = a;
                } while(match(p, T_COMMA));
                expect(p, T_RPAREN, "Expected ')' after args");
            }
            ASTNode *n = ast_node_new(AST_FUNC_CALL, t->line, t->column);
            n->as.func_call.name = name;
            n->as.func_call.args = args;
            n->as.func_call.arg_count = ac;
            return n;
        }
        ASTNode *n = ast_node_new(AST_VAR_REF, t->line, t->column);
        n->as.var_ref.name = strdup(t->text);
        return n;
    }
    if (match(p, T_LPAREN)) {
        ASTNode *n = parse_expression(p, 0);
        expect(p, T_RPAREN, "Expected ')'");
        return n;
    }
    fprintf(stderr, "Unexpected token '%s' at %zu:%zu", t->text, t->line, t->column);
    exit(EXIT_FAILURE);
}

static ASTNode *parse_expression(Parser *p, int min_prec) {
    ASTNode *lhs = parse_primary(p);
    while (1) {
        Token *t = peek(p);
        int prec = get_prec(t->type);
        if (prec == 0 || prec < min_prec) break;
        TokenType op = t->type;
        advance(p);
        ASTNode *rhs = parse_expression(p, prec+1);
        ASTNode *n = ast_node_new(AST_BINARY_OP, t->line, t->column);
        n->as.binary.op = op;
        n->as.binary.left = lhs;
        n->as.binary.right = rhs;
        lhs = n;
    }
    return lhs;
}
