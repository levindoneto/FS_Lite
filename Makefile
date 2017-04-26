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
LIB_DIR=./lib/
INC_DIR=./include/
BIN_DIR=./bin/
SRC_DIR=./src/
TST_DIR=./testes/

all: $(BIN_DIR)t2fs.o $(BIN_DIR)fslite.o $(BIN_DIR)walker.o $(BIN_DIR)diskio.o \
	$(BIN_DIR)types.o $(BIN_DIR)utils.o
	ar rcs $(LIB_DIR)libt2fs.a \
	$(BIN_DIR)t2fs.o $(BIN_DIR)fslite.o $(BIN_DIR)walker.o $(BIN_DIR)diskio.o \
	$(BIN_DIR)types.o $(BIN_DIR)utils.o \
	$(LIB_DIR)apidisk.o $(LIB_DIR)bitmap2.o

$(BIN_DIR)t2fs.o: $(SRC_DIR)t2fs.c
	$(CC) $(CFLAGS) -c -o $(BIN_DIR)t2fs.o -I$(INC_DIR) $(SRC_DIR)t2fs.c

$(BIN_DIR)fslite.o: $(SRC_DIR)fslite.c
	$(CC) $(CFLAGS) -c -o $(BIN_DIR)fslite.o -I$(INC_DIR) $(SRC_DIR)fslite.c

$(BIN_DIR)walker.o: $(SRC_DIR)walker.c
	$(CC) $(CFLAGS) -c -o $(BIN_DIR)walker.o -I$(INC_DIR) $(SRC_DIR)walker.c

$(BIN_DIR)diskio.o: $(SRC_DIR)diskio.c
	$(CC) $(CFLAGS) -c -o $(BIN_DIR)diskio.o -I$(INC_DIR) $(SRC_DIR)diskio.c

$(BIN_DIR)types.o: $(SRC_DIR)types.c
	$(CC) $(CFLAGS) -c -o $(BIN_DIR)types.o -I$(INC_DIR) $(SRC_DIR)types.c

$(BIN_DIR)utils.o: $(SRC_DIR)utils.c
	$(CC) $(CFLAGS) -c -o $(BIN_DIR)utils.o -I$(INC_DIR) $(SRC_DIR)utils.c


# VM RULES
.PHONY: init pull tar
init:
	@sudo mount -t vboxsf fslite ~/Share/

pull:
	@rm -r ./* && cp -r ~/Share/* ./

tar:
	@cd .. && tar -zcvf t2fs.tar.gz fslite

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~
