#include <stdlib.h>
#include <string.h>

#include "walker.h"

#include "diskio.h"
#include "bitmap2.h"

int getRecordFromPath(char* pathname, RECORD_WALKER* walker){
    char* path_name;
    char* path_member;
    int i, members;

    walker->child = initRecord();
    walker->child_pos = initRecordMap();
    /*Set root as first entry*/
    walker->child_code = ROOT_REC;
    walker->child.TypeVal = DIR_TYPE;
    walker->child.inodeNumber = 0;

    if (strcmp(pathname, "/") == 0){
        walker->child_code = ROOT_REC;
        return CHILD_FOUND;
    }
    path_name = (char*) malloc(strlen(pathname)+1);
    strncpy(path_name, pathname, strlen(pathname)+1);

    /* Counting members*/
    members = 1;
    strtok(path_name, "/");
    while(strtok(NULL, "/") != NULL)
        members++;

    strncpy(path_name, pathname, strlen(pathname)+1);
    path_member = strtok(path_name, "/");

    for(i = 0; i < members; i++){
        walker->parent = walker->child;
        walker->parent_pos = walker->child_pos;
        walker->parent_code = walker->child_code;

        /* LAST RECORD MUST BE DIRECTORY*/
        if (walker->child.TypeVal != DIR_TYPE){
            walker->child = initRecord();
            walker->child_pos = initRecordMap();
            walker->child_code = INVALID_REC;
            break;
        }

        if (getRecordFromInode(&walker->child, &walker->child_pos,
                    walker->child.inodeNumber, path_member) == FALSE){
            walker->child = initRecord();
            strcpy(walker->child.name, path_member);
            walker->child_pos = initRecordMap();
            walker->child_code = INVALID_REC;
            i++;
            break;
        }
        walker->child_code = VALID_REC;

        path_member = strtok(NULL, "/");
    }
    free(path_name);
    /* Didn't travel all members*/
    if(i != members)
        return NOT_FOUND;

    if (walker->child_code == VALID_REC && walker->parent_code != INVALID_REC)
        return CHILD_PARENT_FOUND;

    if (walker->child_code == INVALID_REC)
        return PARENT_FOUND;

    return NOT_FOUND;
}

int getRecordFromInode(RECORD_T* buffer, RECORD_MAP* map, int inode_index,
    char* entry_name){

    INODE_WALKER walker;
    BLOCK_T block;
    RECORD_T records[RECORD_PER_BLOCK];
    int block_index;
    int i;

    walker = initInodeWalker(inode_index);
    block_index = getNextBlockIndex(&walker);
    while(block_index != INVALID_PTR){
        if (readBlock(block_index, &block) == FALSE)
            return FALSE;
        blockToRecords(&block, records);
        for(i = 0; i < RECORD_PER_BLOCK; i++){
            if (strcmp(records[i].name, entry_name) == 0){
                if (records[i].TypeVal != 0x00){
                    *buffer = records[i];
                    map->block = block_index;
                    map->index = i;
                    return TRUE;
                }
                else
                    return FALSE;
            }
        }
        block_index = getNextBlockIndex(&walker);
    }
    return FALSE;
}

int getCurrentBlockIndex(INODE_WALKER* walker){
    if (walker->cursor < 0)
        return INVALID_PTR;

    if (walker->cursor < INODE_DIR_P){
        return walker->inode.dataPtr[walker->cursor];
    }
    else if (walker->cursor < INODE_DIR_P + PTR_PER_BLOCK){
        int index = walker->cursor - INODE_DIR_P;
        if (walker->sg_tag != 0){
            BLOCK_T block;
            if (readBlock(walker->inode.singleIndPtr, &block) == FALSE)
                return INVALID_PTR;
            blockToPointers(&block, walker->sg_ptr);
            walker->sg_tag = 0;
        }
        return walker->sg_ptr[index];
    }
    else if (walker->cursor < INODE_DIR_P + PTR_PER_BLOCK + PTR_PER_BLOCK*PTR_PER_BLOCK){
        int sg_tag = (walker->cursor - INODE_DIR_P)/PTR_PER_BLOCK;
        int index = (walker->cursor - INODE_DIR_P)%PTR_PER_BLOCK;
        if (walker->db_tag != PTR_PER_BLOCK + 1){
            BLOCK_T block;
            if (readBlock(walker->inode.doubleIndPtr, &block) == FALSE){
                return INVALID_PTR;
            }
            blockToPointers(&block, walker->db_ptr);
            walker->db_tag = PTR_PER_BLOCK + 1;
        }
        if (walker->sg_tag != sg_tag){
            BLOCK_T block;
            if (readBlock(walker->db_ptr[sg_tag - 1], &block) == FALSE){
                return INVALID_PTR;
            }
            blockToPointers(&block, walker->db_ptr);
            walker->sg_tag = sg_tag;
        }
        return walker->sg_ptr[index];
    }
    else
        return INVALID_PTR;
}

int getBlockIndexAt(INODE_WALKER* walker, int index){
    walker->cursor = index;
    return getCurrentBlockIndex(walker);
}

int getNextBlockIndex(INODE_WALKER* walker){
    walker->cursor++;
    return getCurrentBlockIndex(walker);
}

int getLastBlockCursor(INODE_WALKER* walker){
    int block_index;
    /* Init in first block*/
    block_index = getBlockIndexAt(walker, 0);
    /* Iterate until found a invalid pointer */
    while(block_index != INVALID_PTR)
        block_index = getNextBlockIndex(walker);
    return walker->cursor;
}
/*
    Cursor NEED TO BE at first invalid block index
    Return new_block_index or INVALID_PTR if it's fail
*/
int getNewBlockIndex(INODE_WALKER* walker){
    int new_block;

    if (walker->cursor < 0)
        return INVALID_PTR;

    new_block = searchBitmap2(BITMAP_DADOS, FREE);
    if (new_block <= 0)
        return INVALID_PTR;
    setBitmap2(BITMAP_DADOS, new_block, USED);

    if (walker->cursor < INODE_DIR_P){
        /* Set new block*/
        walker->inode.dataPtr[walker->cursor] = new_block;
        writeInode(walker->inode_index, &walker->inode);
    }
    else if (walker->cursor < INODE_DIR_P + PTR_PER_BLOCK){
        BLOCK_T block;
        int index = walker->cursor - INODE_DIR_P;

        if (walker->sg_tag != 0){
            block = newBlockWithInvalidPtr();
            /*INVALID_PTR at singleIndPtr*/
            if (readBlock(walker->inode.singleIndPtr, &block) == FALSE){
                int new_sgpt = searchBitmap2(BITMAP_DADOS, FREE);
                if (new_sgpt <= 0){
                    setBitmap2(BITMAP_DADOS, new_block, FREE);
                    return INVALID_PTR;
                }
                setBitmap2(BITMAP_DADOS, new_sgpt, USED);
                walker->inode.singleIndPtr = new_sgpt;
                writeInode(walker->inode_index, &walker->inode);
            }
            blockToPointers(&block, walker->sg_ptr);
            walker->sg_tag = 0;
        }
        walker->sg_ptr[index] = new_block;
        pointersToBlock(&block, walker->sg_ptr);
        writeBlock(walker->inode.singleIndPtr, &block);
    }
    else if (walker->cursor < INODE_DIR_P + PTR_PER_BLOCK + PTR_PER_BLOCK*PTR_PER_BLOCK){
        BLOCK_T block;
        int sg_tag = (walker->cursor - INODE_DIR_P)/PTR_PER_BLOCK;
        int index = (walker->cursor - INODE_DIR_P)%PTR_PER_BLOCK;

        if (walker->db_tag != PTR_PER_BLOCK + 1){
            block = newBlockWithInvalidPtr();
            /*INVALID_PTR at doubleIndPtr*/
            if (readBlock(walker->inode.doubleIndPtr, &block) == FALSE){
                int new_dbpt = searchBitmap2(BITMAP_DADOS, FREE);
                if (new_dbpt <= 0){
                    setBitmap2(BITMAP_DADOS, new_block, FREE);
                    return INVALID_PTR;
                }
                setBitmap2(BITMAP_DADOS, new_dbpt, USED);
                walker->inode.doubleIndPtr = new_dbpt;
                writeInode(walker->inode_index, &walker->inode);
            }
            blockToPointers(&block, walker->db_ptr);
            walker->db_tag = PTR_PER_BLOCK + 1;
        }

        if (walker->sg_tag != sg_tag){
            block = newBlockWithInvalidPtr();
            /*INVALID_PTR at doubleIndPtr cache index*/
            if (readBlock(walker->db_ptr[sg_tag - 1], &block) == FALSE){
                int new_sgpt = searchBitmap2(BITMAP_DADOS, FREE);
                if (new_sgpt <= 0){
                    setBitmap2(BITMAP_DADOS, new_block, FREE);
                    return INVALID_PTR;
                }
                setBitmap2(BITMAP_DADOS, new_sgpt, USED);
                walker->db_ptr[sg_tag - 1] = new_sgpt;
                pointersToBlock(&block, walker->db_ptr);
                writeBlock(walker->inode.doubleIndPtr, &block);
            }
            blockToPointers(&block, walker->sg_ptr);
            walker->sg_tag = sg_tag;
        }
        walker->sg_ptr[index] = new_block;
        pointersToBlock(&block, walker->sg_ptr);
        writeBlock(walker->db_ptr[sg_tag - 1], &block);
    }
    else{
        setBitmap2(BITMAP_DADOS, new_block, FREE);
        return INVALID_PTR;
    }
    return new_block;
}

int deleteCurrentBlockIndex(INODE_WALKER* walker) {
    int index;
    index = getCurrentBlockIndex(walker);
    if (index == INVALID_PTR) {
        return FALSE;
    }
    else {
        setBitmap2 (BITMAP_DADOS, index, FREE); // Set the entire block to invalid
        return TRUE;
    }
}

int deleteNextBlockIndex(INODE_WALKER* walker) {
    walker->cursor++;
    return deleteCurrentBlockIndex(walker);
}

int deleteBlockIndexAt(INODE_WALKER* walker, int index) {
    walker->cursor = index;
    return deleteCurrentBlockIndex(walker);
}

int deleteBlocksStartingAt(INODE_WALKER* walker, int index){
    if (deleteBlockIndexAt(walker, index) == FALSE)
        return FALSE;
    while(deleteNextBlockIndex(walker) == TRUE)
        continue;
    if (index < INODE_DIR_P + PTR_PER_BLOCK){
        if(walker->inode.doubleIndPtr != INVALID_PTR)
            setBitmap2 (BITMAP_DADOS, walker->inode.doubleIndPtr, FREE);
    }
    if (index < INODE_DIR_P){
        if(walker->inode.singleIndPtr != INVALID_PTR)
            setBitmap2 (BITMAP_DADOS, walker->inode.singleIndPtr, FREE);
    }
    return TRUE;
}

RECORD_MAP initRecordMap(){
    RECORD_MAP map;
    map.block = INVALID_PTR;
    map.index = INVALID_PTR;

    return map;
}

INODE_WALKER initInodeWalker(int inode_index){
    INODE_WALKER walker;
    INODE_T inode;

    readInode(inode_index, &inode);
    walker.inode = inode;
    walker.inode_index = inode_index;
    walker.cursor = -1;
    walker.sg_tag = INVALID_PTR;
    walker.db_tag = INVALID_PTR;

    return walker;
}
