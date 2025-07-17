// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "compiler.h"
#include "vm.h"

static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s <source_file>\n", prog);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *source = malloc(fsize + 1);
    if (fread(source, 1, fsize, file) != (size_t)fsize) {
        perror("Error reading file");
        fclose(file);
        free(source);
        return EXIT_FAILURE;
    }
    source[fsize] = '\0';
    fclose(file);

    size_t token_count;
    Token *tokens = lex(source, &token_count);
    ASTNode *ast = parse(tokens, token_count);
    Bytecode *bc = compile(ast);

    int exit_code = run_bytecode(bc);

    bytecode_free(bc);
    free_ast(ast);
    free_tokens(tokens, token_count);
    free(source);

    return exit_code;
}
