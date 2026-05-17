# Configurações do Compilador e Flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g -std=c11

# Estrutura de Diretórios
SRC_DIR = src
LIB_DIR = lib
OBJ_DIR = obj
BIN_DIR = bin

# Utilitários
UTILS = cp grep head kill ls replace sort tail UEsh
BINS = $(addprefix $(BIN_DIR)/, $(UTILS))

# Objetos da Biblioteca Comum
LIB_OBJ = $(OBJ_DIR)/common.o

all: directories $(BINS)

# Executar o shell principal
run: all
	./$(BIN_DIR)/UEsh

# Verificar vazamentos de memória
valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all ./$(BIN_DIR)/UEsh

# Formatar o código
format:
	clang-format -i src/*.c include/*.h lib/*.c

# Ajuda
help:
	@echo "Comandos disponíveis:"
	@echo "  make          - Compila todos os utilitários"
	@echo "  make run      - Compila e executa o UEsh"
	@echo "  make valgrind - Verifica bugs de memória no UEsh"
	@echo "  make format   - Formata o código fonte"
	@echo "  make clean    - Remove arquivos de build"

# Criar diretórios necessários
directories:
	mkdir -p $(BIN_DIR) $(OBJ_DIR)

# Compilação da biblioteca comum
$(OBJ_DIR)/common.o: $(LIB_DIR)/common.c include/common.h
	$(CC) $(CFLAGS) -c $< -o $@

# Regra genérica para os utilitários
$(BIN_DIR)/%: $(SRC_DIR)/%.c $(LIB_OBJ) include/common.h
	$(CC) $(CFLAGS) $< $(LIB_OBJ) -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean directories run valgrind format help
