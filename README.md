# PHPC Compiler

A minimal, educational compiler written in C that accepts a PHP-like syntax and generates bytecode for a simple stack-based virtual machine. This project is designed to illustrate each stage of a compiler—from lexing and parsing, through AST construction and code generation, to execution in a VM.

---

## Project Structure

```
phpc/
├── README.md
├── example.phpc
├── Makefile
├── tokens.h
├── lexer.h
├── lexer.c
├── ast.h
├── parser.h
├── parser.c
├── bytecode.h
├── bytecode.c
├── compiler.h
├── compiler.c
├── vm.h
├── vm.c
└── main.c
```

---

## Getting Started

### Building

```bash
make phpc
```

### Running

```bash
./bin/phpc example.phpc
```

## License

This project is licensed under the [MIT License](LICENSE).
