// lexer.h
#ifndef LEXER_H
#define LEXER_H

#include "tokens.h"

Token *lex(const char *source, size_t *out_count);
void free_tokens(Token *tokens, size_t count);

#endif // LEXER_H
