// lexer.c
#include "lexer.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_CAPACITY 128

static void add_token(Token **tokens, size_t *count, size_t *capacity,
                      TokenType type, const char *text,
                      size_t line, size_t column) {
    if (*count >= *capacity) {
        *capacity *= 2;
        *tokens = realloc(*tokens, (*capacity) * sizeof(Token));
    }
    Token *t = &(*tokens)[(*count)++];
    t->type = type;
    if (text) {
        strncpy(t->text, text, MAX_TOKEN_TEXT-1);
        t->text[MAX_TOKEN_TEXT-1] = '\0';
    } else {
        t->text[0] = '\0';
    }
    t->line = line;
    t->column = column;
}

Token *lex(const char *source, size_t *out_count) {
    size_t capacity = INITIAL_CAPACITY, count = 0;
    Token *tokens = malloc(capacity * sizeof(Token));
    size_t i = 0, line = 1, col = 1;

    while (source[i] != '\0') {
        char c = source[i];
        if (c==' '||c=='\t'||c=='\r') { i++; col++; continue; }
        if (c=='\n') { i++; line++; col=1; continue; }
        if (c=='/'&&source[i+1]=='/') { i+=2; while(source[i]&&source[i]!='\n') i++; continue; }
        size_t tok_col = col;
        if (isalpha(c)||c=='$') {
            char buf[MAX_TOKEN_TEXT]={0}; size_t len=0;
            while((isalnum(source[i])||source[i]=='_'||source[i]=='$')&&len<MAX_TOKEN_TEXT-1) {
                buf[len++]=source[i++]; col++;
            }
            buf[len]='\0';
            TokenType type = T_IDENTIFIER;
            if (!strcmp(buf,"if")) type=T_IF;
            else if (!strcmp(buf,"else")) type=T_ELSE;
            else if (!strcmp(buf,"while")) type=T_WHILE;
            else if (!strcmp(buf,"function")) type=T_FUNCTION;
            else if (!strcmp(buf,"return")) type=T_RETURN;
            else if (!strcmp(buf,"print")) type=T_PRINT;
            add_token(&tokens,&count,&capacity,type,buf,line,tok_col);
            continue;
        }
        if (isdigit(c)) {
            char buf[MAX_TOKEN_TEXT]={0}; size_t len=0;
            while(isdigit(source[i])&&len<MAX_TOKEN_TEXT-1){ buf[len++]=source[i++]; col++; }
            buf[len]='\0';
            add_token(&tokens,&count,&capacity,T_NUMBER,buf,line,tok_col);
            continue;
        }
        switch(c){
            case '(': add_token(&tokens,&count,&capacity,T_LPAREN,NULL,line,tok_col); i++;col++;break;
            case ')': add_token(&tokens,&count,&capacity,T_RPAREN,NULL,line,tok_col); i++;col++;break;
            case '{': add_token(&tokens,&count,&capacity,T_LBRACE,NULL,line,tok_col); i++;col++;break;
            case '}': add_token(&tokens,&count,&capacity,T_RBRACE,NULL,line,tok_col); i++;col++;break;
            case ';': add_token(&tokens,&count,&capacity,T_SEMICOLON,NULL,line,tok_col); i++;col++;break;
            case ',': add_token(&tokens,&count,&capacity,T_COMMA,NULL,line,tok_col); i++;col++;break;
            case '+': add_token(&tokens,&count,&capacity,T_PLUS,NULL,line,tok_col); i++;col++;break;
            case '-': add_token(&tokens,&count,&capacity,T_MINUS,NULL,line,tok_col); i++;col++;break;
            case '*': add_token(&tokens,&count,&capacity,T_STAR,NULL,line,tok_col); i++;col++;break;
            case '/': add_token(&tokens,&count,&capacity,T_SLASH,NULL,line,tok_col); i++;col++;break;
            case '%': add_token(&tokens,&count,&capacity,T_MOD,NULL,line,tok_col); i++;col++;break;
            case '>':
                if (source[i+1]=='=') { add_token(&tokens,&count,&capacity,T_GTE,NULL,line,tok_col); i+=2; col+=2; }
                else { add_token(&tokens,&count,&capacity,T_GT,NULL,line,tok_col); i++;col++; }
                break;
            case '<':
                if (source[i+1]=='=') { add_token(&tokens,&count,&capacity,T_LTE,NULL,line,tok_col); i+=2; col+=2; }
                else { add_token(&tokens,&count,&capacity,T_LT,NULL,line,tok_col); i++;col++; }
                break;
            case '=':
                if (source[i+1]=='=') { add_token(&tokens,&count,&capacity,T_EQ,NULL,line,tok_col); i+=2; col+=2; }
                else { add_token(&tokens,&count,&capacity,T_ASSIGN,NULL,line,tok_col); i++;col++; }
                break;
            case '!':
                if (source[i+1]=='=') { add_token(&tokens,&count,&capacity,T_NEQ,NULL,line,tok_col); i+=2; col+=2; }
                else { add_token(&tokens,&count,&capacity,T_ERROR,NULL,line,tok_col); i++;col++; }
                break;
            default:
                add_token(&tokens,&count,&capacity,T_ERROR,NULL,line,tok_col);
                i++;col++;
        }
    }
    add_token(&tokens,&count,&capacity,T_EOF,NULL,line,col);
    *out_count = count;
    return tokens;
}

void free_tokens(Token *tokens, size_t count) {
    (void)count;
    free(tokens);
}
