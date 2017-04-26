#ifndef __LIBUTILS___
#define __LIBUTILS___

#include "t2fs.h"
#include "types.h"
#include "walker.h"

/* UTILS.C*/
void superblockPrint (SUPERBLOCK_T superblock);
void recordPrint (RECORD_T record);
void inodePrint (INODE_T inode);
void direntryPrint (DIRENT2 direntry);
void recordWalkerPrint(RECORD_WALKER walker);
void handlesPrint();
void bitmapPrint (int handle);
void listDirWithInode(int inode_index);
void tree (int inode_index, int level);
#endif
