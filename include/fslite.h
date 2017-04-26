#ifndef __LIBFSLITE___
#define __LIBFSLITE___

/*

*******************FORMATO BÁSICO DO DISCO*********************

* Tamanho de setores: 256 bytes.
* Quantidade de setores: 32768

* Tamanho de blocos de dados: 16 setores, 4096 bytes (16*256)
* Quantidade de blocos de dados: 2040

* Tamanho de um i-node: 16 bytes
* Quantidade de i-nodes: 2000

* Bitmaps podem endereçar: 2048 endereços (8*256)
* Tamanho máximo arquivo : 4,299,169,792 bytes

--------------------------------------------------
* Index |    Descrição     | Setores |   Bytes   *
*     0 |  SUPERBLOCO      |       1 |       256 *
*     1 |  BITMAP BLOCO    |       1 |       256 *
*     2 |  BITMAP INODE    |       1 |       256 *
*     3 |  INODE AREA      |     125 |     32000 *
*   128 |  BLOCO DE DADOS  |  32,640 | 8,355,840 *
--------------------------------------------------

Diretório raiz ("/") corresponde ao inode de índice 0.

***************************************************************
*/
#include "t2fs.h"
#include "walker.h"
#include "types.h"
#include "constants.h"

struct file_st{
    /* Validation field*/
    int valid;
    /* File cursor*/
    int cursor;
    /* Modified Flag*/
    int modified;

    /* Walkers*/
    struct record_walk record_walker;
    struct inode_walk inode_walker;
};

struct dir_st{
    /* Validation field*/
    int valid;
    /* Current entry to readdir*/
    int current_entry;

    /* Walkers*/
    struct record_walk record_walker;
    struct inode_walk inode_walker;
};

struct _handles{
    /* File handles*/
    struct file_st files[ENTRY_MAX];
    /* Directory handles*/
    struct dir_st dirs[ENTRY_MAX];

    int init;
};

typedef struct file_st FILE_T;
typedef struct dir_st DIR_T;
typedef struct _handles HANDLES;

int newInode(INODE_T* inode);
void deleteInode(int inode_index);

int getNextValidRecord(INODE_WALKER* walker, RECORD_T *buffer, int last_index);
int getNextInvalidRecord(INODE_WALKER* walker, RECORD_MAP* rec_pos);
int getLastValidRecord(INODE_WALKER* walker, RECORD_T *buffer);

int newHandle(int option);
int pathValidation(char* pathname);

HANDLES initHandles();
FILE_T initFile(INODE_WALKER inode_w, RECORD_WALKER record_w);
DIR_T initDir(INODE_WALKER inode_w, RECORD_WALKER record_w);

extern HANDLES handles;

#endif
