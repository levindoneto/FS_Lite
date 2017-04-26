#ifndef __LIBDISKIO___
#define __LIBDISKIO___

#include "types.h"

/* DISKIO.C*/
int readSector(int sector, SECTOR_T* buffer);
int readInode(int inode_index, INODE_T* buffer);
int readBlock(int block, BLOCK_T* buffer);
int readSuperbloco(SUPERBLOCK_T* buffer);

int writeSector(int sector_index, SECTOR_T* buffer);
int writeInode(int inode_index, INODE_T* buffer);
int writeBlock(int block_index, BLOCK_T* buffer);
int writeRecord(int block, int index, RECORD_T* buffer);

#endif
