#ifndef __LIBTYPES___
#define __LIBTYPES___

#include "t2fs.h"
#include "constants.h"

struct record_map{
    /* Block of memory*/
    int block;
    /* Record index inside block*/
    int index;
};

struct inode_walk{
    /* I-NODE*/
    struct t2fs_inode inode;
    /* Inode index on memory*/
    int inode_index;
    /* Block cursor*/
    int cursor;

    /* Single indirection pointer*/
    int sg_ptr[PTR_PER_BLOCK];
    int sg_tag;
    /* Double indirection pointer*/
    int db_ptr[PTR_PER_BLOCK];
    int db_tag;
};

struct record_walk{
    /* Entry for child*/
    int child_code;
    struct t2fs_record child;
    struct record_map child_pos;

    /* Entry for parent*/
    int parent_code;
    struct t2fs_record parent;
    struct record_map parent_pos;
};


typedef struct t2fs_superbloco SUPERBLOCK_T;
typedef struct t2fs_record RECORD_T;
typedef struct t2fs_inode INODE_T;

typedef struct {unsigned char at[SECTOR_SIZE];} SECTOR_T;
typedef struct {unsigned char at[BLOCK_SIZE];} BLOCK_T;

typedef struct record_map RECORD_MAP;
typedef struct inode_walk INODE_WALKER;
typedef struct record_walk RECORD_WALKER;

/* TYPECONVERT*/
void blockToRecords(BLOCK_T* block_buffer, RECORD_T* vector);
void blockToPointers(BLOCK_T* block_buffer, int *vector);
void pointersToBlock(BLOCK_T* block_buffer, int *vector);

void sectorToSuperblock(SECTOR_T* sector_buffer, SUPERBLOCK_T* superblock);
int sectorToRecord(BLOCK_T block_buffer, int entry_index, RECORD_T* record);
int sectorToInode(SECTOR_T sector_buffer, int inode_index, INODE_T* inode);

void recordToDirEnt(RECORD_T* record, DIRENT2* direntry);

/* INIT DATAS*/
BLOCK_T newBlockWithInvalidPtr();
BLOCK_T newBlockWithInvalidRecord();
INODE_T initInode ();
RECORD_T initRecord();

#endif
