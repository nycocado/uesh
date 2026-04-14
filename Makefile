# Configurações do Compilador e Flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g -std=c11
TEST_CFLAGS = $(CFLAGS) -Itest/unity

# Estrutura de Diretórios
SRC_DIR = src
LIB_DIR = lib
OBJ_DIR = obj
BIN_DIR = bin
TEST_DIR = test
UNITY_DIR = test/unity

# Utilitários
UTILS = cp grep head kill ls replace sort tail UEsh
BINS = $(addprefix $(BIN_DIR)/, $(UTILS))

# Objetos da Biblioteca Comum
LIB_OBJ = $(OBJ_DIR)/common.o
# Objeto do Framework de Teste
UNITY_OBJ = $(OBJ_DIR)/unity.o

# Testes
TEST_SOURCES = $(wildcard $(TEST_DIR)/test_*.c)
TEST_BINS = $(patsubst $(TEST_DIR)/%.c, $(BIN_DIR)/test/%, $(TEST_SOURCES))

all: directories $(BINS)

# Executar o shell principal
run: all
	./$(BIN_DIR)/UEsh

# Verificar vazamentos de memória
valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all ./$(BIN_DIR)/UEsh

# Formatar o código
format:
	clang-format -i src/*.c include/*.h lib/*.c test/*.c

# Ajuda
help:
	@echo "Comandos disponíveis:"
	@echo "  make          - Compila todos os utilitários"
	@echo "  make run      - Compila e executa o UEsh"
	@echo "  make test     - Executa os testes unitários (Unity)"
	@echo "  make valgrind - Verifica bugs de memória no UEsh"
	@echo "  make format   - Formata o código fonte"
	@echo "  make clean    - Remove arquivos de build"

# Criar diretórios necessários
directories:
	mkdir -p $(BIN_DIR)/test $(OBJ_DIR)/test

# Compilação da biblioteca comum
$(OBJ_DIR)/common.o: $(LIB_DIR)/common.c include/common.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compilação do Unity
$(OBJ_DIR)/unity.o: $(UNITY_DIR)/unity.c $(UNITY_DIR)/unity.h
	$(CC) $(TEST_CFLAGS) -c $< -o $@

# Regra genérica para os utilitários
$(BIN_DIR)/%: $(SRC_DIR)/%.c $(LIB_OBJ) include/common.h
	$(CC) $(CFLAGS) $< $(LIB_OBJ) -o $@

# Regra para compilar testes
$(BIN_DIR)/test/test_%: $(TEST_DIR)/test_%.c $(LIB_OBJ) $(UNITY_OBJ)
	$(CC) $(TEST_CFLAGS) $^ -o $@

# Atalho para rodar todos os testes
test: directories $(TEST_BINS)
	@for test in $(TEST_BINS); do ./$$test; done

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean test directories run valgrind format help
