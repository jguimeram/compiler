CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I.
SRC = main.c lexer.c parser.c ast.c compiler.c bytecode.c vm.c
OBJ = $(SRC:.c=.o)

bin/phpc: $(OBJ) | bin
	$(CC) $(CFLAGS) -o bin/phpc $(OBJ)

bin:
	mkdir -p bin

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) bin/phpc
