# Compiler settings and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g -std=c11

# Directory structure
SRC_DIR = src
LIB_DIR = lib
OBJ_DIR = obj
BIN_DIR = bin

# Utilities
UTILS = cp grep head kill ls replace sort tail UEsh
BINS = $(addprefix $(BIN_DIR)/, $(UTILS))

# Common library objects
LIB_OBJ = $(OBJ_DIR)/common.o

all: directories $(BINS)

# Run the main shell
run: all
	./$(BIN_DIR)/UEsh

# Check for memory leaks
valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all ./$(BIN_DIR)/UEsh

# Format source code
format:
	clang-format -i src/*.c include/*.h lib/*.c

# Help
help:
	@echo "Comandos disponíveis:"
	@echo "  make          - Compila todos os utilitários"
	@echo "  make run      - Compila e executa o UEsh"
	@echo "  make valgrind - Verifica bugs de memória no UEsh"
	@echo "  make format   - Formata o código fonte"
	@echo "  make clean    - Remove arquivos de build"

# Create required directories
directories:
	mkdir -p $(BIN_DIR) $(OBJ_DIR)

# Build the common library
$(OBJ_DIR)/common.o: $(LIB_DIR)/common.c include/common.h
	$(CC) $(CFLAGS) -c $< -o $@

# Generic rule for utilities
$(BIN_DIR)/%: $(SRC_DIR)/%.c $(LIB_OBJ) include/common.h
	$(CC) $(CFLAGS) $< $(LIB_OBJ) -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean directories run valgrind format help
