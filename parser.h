// parser.h
#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"
#include "ast.h"

ASTNode *parse(Token *tokens, size_t count);

#endif // PARSER_H
