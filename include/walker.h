#ifndef __LIBWALKER___
#define __LIBWALKER___

#include "t2fs.h"
#include "types.h"

int getRecordFromPath(char* pathname, RECORD_WALKER* walker);
int getRecordFromInode(RECORD_T* buffer, RECORD_MAP* map, int inode_index, char* entry_name);
int getRecordFromRoot (RECORD_T* buffer, char* entry_name);

int getCurrentBlockIndex(INODE_WALKER* walker);
int getBlockIndexAt(INODE_WALKER* walker, int index);
int getNextBlockIndex(INODE_WALKER* walker);
int getNewBlockIndex(INODE_WALKER* walker);
int getLastBlockCursor(INODE_WALKER* walker);

int deleteBlocksStartingAt(INODE_WALKER* walker, int index);
int deleteBlockIndexAt(INODE_WALKER* walker, int index);
int deleteCurrentBlockIndex(INODE_WALKER* walker);
int deleteNextBlockIndex(INODE_WALKER* walker);

RECORD_MAP initRecordMap();
INODE_WALKER initInodeWalker(int inode_index);

#endif
