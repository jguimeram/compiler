// tokens.h
#ifndef TOKENS_H
#define TOKENS_H

#include <stddef.h>

// Maximum length for identifiers and literals
#define MAX_TOKEN_TEXT 64

typedef enum {
    // Special
    T_EOF,
    T_ERROR,

    // Keywords
    T_IF,
    T_ELSE,
    T_WHILE,
    T_FUNCTION,
    T_RETURN,
    T_PRINT,

    // Symbols
    T_LPAREN,   // (
    T_RPAREN,   // )
    T_LBRACE,   // {
    T_RBRACE,   // }
    T_SEMICOLON,// ;
    T_COMMA,    // ,

    // Operators
    T_PLUS,     // +
    T_MINUS,    // -
    T_STAR,     // *
    T_SLASH,    // /
    T_ASSIGN,   // =
    T_GT,       // >
    T_LT,       // <
    T_GTE,      // >=
    T_LTE,      // <=
    T_EQ,       // ==
    T_NEQ,      // !=

    // Identifiers and literals
    T_IDENTIFIER, // variable or function names (e.g., $x, foo)
    T_NUMBER      // numeric literals (e.g., 123)
} TokenType;

typedef struct {
    TokenType type;
    char text[MAX_TOKEN_TEXT];
    size_t line;
    size_t column;
} Token;

// Utility to map TokenType → string (for debugging)
const char *token_type_to_string(TokenType type);

#endif // TOKENS_H
