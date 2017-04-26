#
# Makefile ESQUELETO
#
# DEVE ter uma regra "all" para geração da biblioteca
# regra "clean" para remover todos os objetos gerados.
#
# NECESSARIO adaptar este esqueleto de makefile para suas necessidades.
#
#

CC=gcc
CFLAGS=-Wall -g
LIB_DIR=../lib/
INC_DIR=../include/
BIN_DIR=../bin/
SRC_DIR=./
TST_DIR=./

all: $(BIN_DIR)readroot.o $(BIN_DIR)recordwalker.o $(BIN_DIR)openclose.o $(BIN_DIR)createdelete.o

$(BIN_DIR)readroot.o: $(SRC_DIR)readroot.c
	$(CC) $(CFLAGS) -o $(TST_DIR)readroot.out -I$(INC_DIR) -L$(LIB_DIR) $(SRC_DIR)readroot.c -lt2fs

$(BIN_DIR)recordwalker.o: $(SRC_DIR)recordwalker.c
	$(CC) $(CFLAGS) -o $(TST_DIR)recordwalker.out -I$(INC_DIR) -L$(LIB_DIR) $(SRC_DIR)recordwalker.c -lt2fs

$(BIN_DIR)openclose.o: $(SRC_DIR)openclose.c
	$(CC) $(CFLAGS) -o $(TST_DIR)openclose.out -I$(INC_DIR) -L$(LIB_DIR) $(SRC_DIR)openclose.c -lt2fs

$(BIN_DIR)createdelete.o: $(SRC_DIR)createdelete.c
	$(CC) $(CFLAGS) -o $(TST_DIR)createdelete.out -I$(INC_DIR) -L$(LIB_DIR) $(SRC_DIR)createdelete.c -lt2fs

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~
